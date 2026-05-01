/*
 * pth_compat.h - GNU Pth API compatibility layer built on nPth/pthreads.
 *
 * Implements the subset of GNU Pth used by pth_task.c:
 *   - pth_t, pth_attr_t
 *   - pth_init/pth_kill/pth_version/pth_self
 *   - pth_spawn/pth_join/pth_yield/pth_usleep/pth_select
 *   - pth_suspend/pth_resume  (via per-thread condvar)
 *   - pth_msgport_*           (via mutex + condvar queue)
 *   - pth_event / pth_wait    (MSG events only)
 */

#ifndef PTH_COMPAT_H
#define PTH_COMPAT_H

#include <npth.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

/* Helpers for storing pth_t (pthread_t = unsigned long) in a void* field.
   GNU Pth's pth_t is a pointer so void-to-pth_t casts work natively.
   With nPth we need explicit integer <-> pointer casts.               */
#define PTH_T_STORE(t)   ((void *)(uintptr_t)(t))
#define PTH_T_LOAD(p)    ((pth_t)(uintptr_t)(p))
#define PTH_T_IS_NULL(t) ((t) == (pth_t)0)
#define PTH_T_EQ(a,b)    pthread_equal((a),(b))

/* ------------------------------------------------------------------ */
/* Basic types                                                          */
/* ------------------------------------------------------------------ */

#define pth_t npth_t   /* pthread_t */

/* Return-value conventions differ: GNU Pth uses TRUE/FALSE (1/0),
   nPth/pthreads return 0 on success.                                  */

static inline int pth_init(void)    { return npth_init() == 0 ? 1 : 0; }
#define pth_kill()                  /* not needed with nPth */
#define pth_version()               0x020000
#define pth_self()                  npth_self()

/* ------------------------------------------------------------------ */
/* Scheduling helpers                                                   */
/* ------------------------------------------------------------------ */

/* pth_usleep / pth_select */
#define pth_usleep(us)              npth_usleep(us)
#define pth_select(n,r,w,e,tv)     npth_select(n,r,w,e,tv)

/* ------------------------------------------------------------------ */
/* Thread attributes                                                    */
/* ------------------------------------------------------------------ */

typedef struct {
    npth_attr_t attr;
    char        name[64];
} _pth_attr_struct;
typedef _pth_attr_struct *pth_attr_t;

#define PTH_ATTR_DEFAULT    NULL
#define PTH_ATTR_NAME       1
#define PTH_ATTR_STACK_SIZE 2
#define PTH_ATTR_PRIO       3
#define PTH_PRIO_MAX        0

static inline pth_attr_t pth_attr_new(void)
{
    _pth_attr_struct *a = calloc(1, sizeof(*a));
    if (a) npth_attr_init(&a->attr);
    return a;
}

static inline int pth_attr_set(pth_attr_t attr, int field, ...)
{
    va_list ap;
    if (!attr) return 0;
    va_start(ap, field);
    switch (field) {
    case PTH_ATTR_NAME:
        strncpy(attr->name, va_arg(ap, char *), 63);
        attr->name[63] = '\0';
        break;
    case PTH_ATTR_STACK_SIZE:
        pthread_attr_setstacksize(&attr->attr,
                                  (size_t)va_arg(ap, unsigned int));
        break;
    case PTH_ATTR_PRIO:
        va_arg(ap, int); /* ignored */
        break;
    }
    va_end(ap);
    return 1;
}

static inline void pth_attr_destroy(pth_attr_t attr)
{
    if (!attr) return;
    npth_attr_destroy(&attr->attr);
    free(attr);
}

/* ------------------------------------------------------------------ */
/* Thread creation / join                                               */
/* ------------------------------------------------------------------ */

static inline pth_t pth_spawn(pth_attr_t attr,
                               void *(*func)(void *), void *arg)
{
    npth_t tid = (npth_t)0;
    int r = npth_create(&tid, attr ? &attr->attr : NULL, func, arg);
    if (r != 0) return (npth_t)0;
    if (attr && attr->name[0])
        npth_setname_np(tid, attr->name);
    return tid;
}

static inline int pth_join(pth_t tid, void **retval)
{
    return npth_join(tid, retval) == 0 ? 1 : 0;
}

/* ------------------------------------------------------------------ */
/* Per-thread suspend / resume                                          */
/*                                                                      */
/* GNU Pth suspends cooperatively (removes from scheduler queue).      */
/* Here we use a per-thread condvar: the thread pauses the next time   */
/* it calls pth_yield().                                               */
/* ------------------------------------------------------------------ */

typedef struct _pth_susp_node {
    pth_t                   tid;
    int                     suspended;
    pthread_mutex_t         mutex;
    pthread_cond_t          cond;
    struct _pth_susp_node  *next;
} _pth_susp_node;

static _pth_susp_node   *_pth_susp_list  = NULL;
static pthread_mutex_t   _pth_susp_lock  = PTHREAD_MUTEX_INITIALIZER;

static inline _pth_susp_node *_pth_susp_get(pth_t tid)
{
    _pth_susp_node *s;
    pthread_mutex_lock(&_pth_susp_lock);
    for (s = _pth_susp_list; s; s = s->next)
        if (pthread_equal(s->tid, tid)) goto out;
    s = calloc(1, sizeof(*s));
    if (s) {
        s->tid = tid;
        pthread_mutex_init(&s->mutex, NULL);
        pthread_cond_init(&s->cond, NULL);
        s->next = _pth_susp_list;
        _pth_susp_list = s;
    }
out:
    pthread_mutex_unlock(&_pth_susp_lock);
    return s;
}

static inline int pth_suspend(pth_t tid)
{
    _pth_susp_node *s = _pth_susp_get(tid);
    if (!s) return 0;
    pthread_mutex_lock(&s->mutex);
    s->suspended = 1;
    pthread_mutex_unlock(&s->mutex);
    return 1;
}

static inline int pth_resume(pth_t tid)
{
    _pth_susp_node *s = _pth_susp_get(tid);
    if (!s) return 0;
    pthread_mutex_lock(&s->mutex);
    s->suspended = 0;
    pthread_cond_broadcast(&s->cond);
    pthread_mutex_unlock(&s->mutex);
    return 1;
}

/* pth_yield: check if this thread is suspended, then yield the CPU. */
static inline int pth_yield(void *t)
{
    _pth_susp_node *s = _pth_susp_get(npth_self());
    (void)t;
    if (s) {
        pthread_mutex_lock(&s->mutex);
        while (s->suspended)
            pthread_cond_wait(&s->cond, &s->mutex);
        pthread_mutex_unlock(&s->mutex);
    }
    sched_yield();
    return 1;
}

/* ------------------------------------------------------------------ */
/* Message ports                                                        */
/* ------------------------------------------------------------------ */

typedef struct _pth_msg_node {
    struct _pth_msg_node *next;
} pth_message_t;

typedef struct _pth_port_node {
    char                    name[32];
    pthread_mutex_t         mutex;
    pthread_cond_t          cond;
    pth_message_t          *head;
    pth_message_t          *tail;
    int                     count;
    struct _pth_port_node  *next_port;
} *pth_msgport_t;

static pth_msgport_t   _pth_ports      = NULL;
static pthread_mutex_t _pth_ports_lock = PTHREAD_MUTEX_INITIALIZER;

static inline pth_msgport_t pth_msgport_create(const char *name)
{
    pth_msgport_t mp = calloc(1, sizeof(*mp));
    if (!mp) return NULL;
    strncpy(mp->name, name, 31);
    mp->name[31] = '\0';
    pthread_mutex_init(&mp->mutex, NULL);
    pthread_cond_init(&mp->cond, NULL);
    pthread_mutex_lock(&_pth_ports_lock);
    mp->next_port = _pth_ports;
    _pth_ports = mp;
    pthread_mutex_unlock(&_pth_ports_lock);
    return mp;
}

static inline void pth_msgport_destroy(pth_msgport_t mp)
{
    pth_msgport_t *pp;
    if (!mp) return;
    pthread_mutex_lock(&_pth_ports_lock);
    for (pp = &_pth_ports; *pp; pp = &(*pp)->next_port)
        if (*pp == mp) { *pp = mp->next_port; break; }
    pthread_mutex_unlock(&_pth_ports_lock);
    pthread_mutex_destroy(&mp->mutex);
    pthread_cond_destroy(&mp->cond);
    free(mp);
}

static inline pth_msgport_t pth_msgport_find(const char *name)
{
    pth_msgport_t mp;
    pthread_mutex_lock(&_pth_ports_lock);
    for (mp = _pth_ports; mp; mp = mp->next_port)
        if (strcmp(mp->name, name) == 0) break;
    pthread_mutex_unlock(&_pth_ports_lock);
    return mp;
}

static inline int pth_msgport_pending(pth_msgport_t mp)
{
    int n;
    if (!mp) return 0;
    pthread_mutex_lock(&mp->mutex);
    n = mp->count;
    pthread_mutex_unlock(&mp->mutex);
    return n;
}

static inline int pth_msgport_put(pth_msgport_t mp, pth_message_t *msg)
{
    if (!mp || !msg) return 0;
    pthread_mutex_lock(&mp->mutex);
    msg->next = NULL;
    if (mp->tail) mp->tail->next = msg;
    else          mp->head = msg;
    mp->tail = msg;
    mp->count++;
    pthread_cond_signal(&mp->cond);
    pthread_mutex_unlock(&mp->mutex);
    return 1;
}

static inline pth_message_t *pth_msgport_get(pth_msgport_t mp)
{
    pth_message_t *msg;
    if (!mp) return NULL;
    pthread_mutex_lock(&mp->mutex);
    msg = mp->head;
    if (msg) {
        mp->head = msg->next;
        if (!mp->head) mp->tail = NULL;
        mp->count--;
        msg->next = NULL;
    }
    pthread_mutex_unlock(&mp->mutex);
    return msg;
}

/* ------------------------------------------------------------------ */
/* Events (MSG type only)                                               */
/* ------------------------------------------------------------------ */

#define PTH_EVENT_MSG   1
#define PTH_FREE_THIS   1

typedef struct {
    int            type;
    pth_msgport_t  mp;
} *pth_event_t;

static inline pth_event_t pth_event(int type, pth_msgport_t mp)
{
    pth_event_t ev = calloc(1, sizeof(*ev));
    if (ev) { ev->type = type; ev->mp = mp; }
    return ev;
}

/* Block until at least one message is available on the port. */
static inline int pth_wait(pth_event_t ev)
{
    if (!ev || ev->type != PTH_EVENT_MSG || !ev->mp) return 0;
    pthread_mutex_lock(&ev->mp->mutex);
    while (ev->mp->count == 0)
        pthread_cond_wait(&ev->mp->cond, &ev->mp->mutex);
    pthread_mutex_unlock(&ev->mp->mutex);
    return 1;
}

static inline void pth_event_free(pth_event_t ev, int mode)
{
    (void)mode;
    free(ev);
}

#endif /* PTH_COMPAT_H */
