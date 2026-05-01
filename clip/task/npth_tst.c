#include <stdio.h>
#include <npth.h>

static void *thread_func(void *arg)
{
    return arg;
}

int main(void)
{
    npth_t tid;
    void *retval;

    if (npth_init() != 0)
        return 1;

    if (npth_create(&tid, NULL, thread_func, NULL) != 0)
        return 1;

    if (npth_join(tid, &retval) != 0)
        return 1;

    return 0;
}
