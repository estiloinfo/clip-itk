/* [ ClipBuf */

void
destroy_ClipBuf(ClipBuf * buf)
{
	if (!buf)
		return;
	if (buf->buf)
		free(buf->buf);
}

void
setLen_ClipBuf(ClipBuf * buf, int newlen)
{
	buf->buf = realloc(buf->buf, newlen);
	buf->len = newlen;
}

CLIP_DLLEXPORT int
_clip_type(ClipVar * vp)
{
	int r = 0;

	if (vp)
		r = _clip_vptr(vp)->t.type;
	return r;
}

CLIP_DLLEXPORT char *
_clip_type_name(ClipVar * vp)
{
	char *rc = "U";

	if (vp)
	{
		switch (_clip_type(vp))
		{
		case NUMERIC_t:
			rc = "N";
			break;
		case CHARACTER_t:
			if (vp->t.memo)
				rc = "M";
			else
				rc = "C";
			break;
		case LOGICAL_t:
			rc = "L";
			break;
		case DATE_t:
			rc = "D";
			break;
		case DATETIME_t:
			rc = "T";
			break;
		case OBJECT_t:
			rc = "O";
			break;
		case PCODE_t:
		case CCODE_t:
			rc = "B";
			break;
		case ARRAY_t:
			rc = "A";
			break;
		case MAP_t:
			rc = "O";
			break;
		}
	}
	return rc;
}

CLIP_DLLEXPORT ClipVar *
_clip_vptr(ClipVar * vp)
{
	if (vp && vp->t.flags & F_MPTR)
		vp = _clip_vptr(vp->p.vp);
	return vp;
}

/* ] ClipBuf */
/* [ ClipVect */

void
destroy_ClipVect(ClipVect * vect)
{
	if (!vect)
		return;
	free(vect->items);
}

void
add_ClipVect(ClipVect * vect, void *item)
{
	int count = ++(vect->count);
	vect->items = realloc(vect->items, sizeof(void *) * count);

	vect->items[count - 1] = item;
}

void
clear_ClipVect(ClipVect * vect)
{
	vect->count = 0;
	free(vect->items);
	vect->items = 0;
	/*vect->items = realloc(vect->items, sizeof(void *) * 4);*/
}

void
remove_ClipVect(ClipVect * vect, int no)
{
	int count = vect->count;

	if (no < 0 || no >= count)
		return;
	vect->count = --count;
	memmove(vect->items + no, vect->items + no + 1, (count - no) * sizeof(void *));
}

void
insert_ClipVect(ClipVect * vect, void *item, int pos)
{
	int count = vect->count;

	if (pos < 0 || pos > count)
		return;
	vect->count = ++count;
	vect->items = realloc(vect->items, sizeof(void *) * count);
	memmove(vect->items + pos + 1, vect->items + pos, (count - pos - 1) * sizeof(void *));

	vect->items[pos] = item;
}

/* ] ClipVect */
/* [ ClipMachine */

static int machineCount = 0;

typedef struct
{
	long hash;
	ClipFunction *f;
	ClipBlock block;
}
FuncEntry;

static ClipMachine *cur_mp = 0;
static ClipMachine *first_mp = 0;

int
_clip_init_tty(ClipMachine * ret)
{
	int r;
	char errbuf[128];

	if (ret->screen)
	{
		restart_tty(ret->screen->base);
		return 0;
	}

	r = init_tty(ret->screen_base, 0, _clip_envp, 0, (ScreenPgChars *) _clip_pgtbl, errbuf, sizeof(errbuf));

	if (r >= 0)
	{
		ret->screen = new_Screen(ret->screen_base);
		if (r > 0)
			_clip_logg(0, "init screen: %s", errbuf);
		r = 0;
	}
	else
	{
		free(ret->screen_base);
		snprintf(ret->syserr, sizeof(ret->syserr), "%s", errbuf);
		ret->screen_base = 0;
		ret->screen = 0;
	}

	if (ret->screen)
	{
		ret->wboard.bottom = ret->windows->rect.bottom = ret->screen->base->Lines - 1;
		ret->wboard.right = ret->windows->rect.right = ret->screen->base->Columns - 1;
		ret->windows->format = ret->windows->rect;
	}

	return r;
}

int
_clip_restore_tty(ClipMachine * mp)
{
	return restore_tty(mp->screen->base);
}

static int errorlevel = 0; /* result of errorlevel() */

static HashTable *all_publics = 0;
static HashTable *all_hashnames = 0;
static HashTable *all_functions = 0;

static HashTable *all_aliases = 0;
static HashTable *all_fields = 0;
static HashTable *all_keys = 0;
static HashTable *all_store = 0;

/*
static ClipVect *all_areas = 0;
static ClipVect *all_areaStack = 0;
*/

static Container *all_container = 0;

static DBDriver *all_dbdrivers = 0;
struct _RDD_DATA_VTBL_ **all_data_drivers;
struct _RDD_INDEX_VTBL_ **all_idx_drivers;
struct _RDD_MEMO_VTBL_ **all_memo_drivers;
static int all_ndbdrivers = 0;
static int all_ndata_drivers = 0;
static int all_nidx_drivers = 0;
static int all_nmemo_drivers = 0;
static char all_def_data_driver[6];
static char all_def_idx_driver[6];
static char all_def_memo_driver[6];
static char all_def_db_driver[9];

static SQLDriver *all_sqldrivers = 0;
static int all_nsqldrivers = 0;

static HashTable *all_tasklocks = 0;
static HashTable *all_fileopens = 0;
/*static ClipWindow *all_windows = 0;*/

static int *all_kbdbuf;
static int *all_kbdptr;

int _clip_sig_flag = 0;
void _clip_signal_real(int sig);


CLIP_DLLEXPORT struct ClipMachine *
new_ClipMachine(struct Screen *screen)
{
	ClipMachine *ret;

#ifdef MEMDEBUG
	{
		static int inited = 0;

		if (!inited)
		{
			malloc_logpath = "_memdebug";
			malloc_debug(MEMDEBUG);
			inited = 1;
		}
	}
#endif

	ret = NEW(ClipMachine);
	if (all_functions)
		ret->functions = all_functions;
	else
		ret->functions = all_functions = new_HashTable();
	++machineCount;

	ret->dbdrivers      = &all_dbdrivers;
	ret->ndbdrivers     = &all_ndbdrivers;
	ret->data_drivers   = &all_data_drivers;
	ret->ndata_drivers  = &all_ndata_drivers;
	ret->idx_drivers    = &all_idx_drivers;
	ret->nidx_drivers   = &all_nidx_drivers;
	ret->memo_drivers   = &all_memo_drivers;
	ret->nmemo_drivers  = &all_nmemo_drivers;
	ret->def_data_driver = all_def_data_driver;
	ret->def_idx_driver  = all_def_idx_driver;
	ret->def_memo_driver = all_def_memo_driver;
	ret->def_db_driver   = all_def_db_driver;

	ret->sqldrivers     = &all_sqldrivers;
	ret->nsqldrivers    = &all_nsqldrivers;

	if(all_tasklocks)
		ret->tasklocks = all_tasklocks;
	else
		ret->tasklocks = all_tasklocks = new_HashTable();

	if(all_fileopens)
		ret->fileopens = all_fileopens;
	else
		ret->fileopens = all_fileopens = new_HashTable();

	if (all_publics)
		ret->publics = all_publics;
	else
		ret->publics = all_publics = new_HashTable();
	ret->privates = new_HashTable();
	ret->spaces = new_HashTable();

/*
	if (all_aliases)
		ret->aliases = all_aliases;
	else
		ret->aliases = all_aliases = new_HashTable();
*/
	ret->aliases = new_HashTable();
/*
	if (all_fields)
		ret->fields = all_fields;
	else
		ret->fields = all_fields = new_HashTable();
*/
	ret->fields = new_HashTable();

	if (all_keys)
		ret->keys = all_keys;
	else
		ret->keys = all_keys = new_HashTable();

	if (all_store)
		ret->store = all_store;
	else
		ret->store = all_store = new_HashTable();

	ret->next = first_mp;
	first_mp = ret;
	ret->decimals = 2;
	ret->fileCreateMode = _clip_fileStrModeToNumMode("664");
	ret->dirCreateMode = _clip_fileStrModeToNumMode("753");
	ret->flags = CONSOLE_FLAG+INTENSITY_FLAG;
	ret->date_format = strdup(CLIP_DATEFORMAT_DEFAULT);
	ret->rootpath = NULL;
	if (CLIP_CENTURY_DEFAULT)
		ret->flags += CENTURY_FLAG;
#ifdef _WIN32
	ret->flags1 += DISPBOX_FLAG;
#endif
	ret->epoch = CLIP_EPOCH_DEFAULT;
	ret->path = strdup(".");
	ret->defaul = strdup(".");
	ret->typeahead = 32;

	if (all_kbdbuf)
		ret->kbdbuf = all_kbdbuf;
	else
		ret->kbdbuf = all_kbdbuf = calloc(ret->typeahead,sizeof(int));

	ret->kbdptr = &all_kbdptr;
	*(ret->kbdptr) = ret->kbdbuf;

	ret->cursor = SC_NORMAL;
	ret->altfile = 0;
	ret->out = stdout;
	ret->delimchars = strdup("  ");
	ret->eventmask = _CLIP_INKEY_KEYBOARD;
	if (screen)
		ret->screen = screen;
	else
	{
		ret->screen_base = NEW(ScreenBase);
		/*_clip_init_tty(ret);
		_clip_restore_tty(ret);*/
	}
	ret->mblocksize = 64;
	ret->attr.standard = COLOR_WHITE | COLOR_BACK_BLACK;
	ret->attr.enhanced = COLOR_BLACK | COLOR_BACK_WHITE;
	ret->attr.border = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->attr.background = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->attr.unselected = COLOR_BLACK | COLOR_BACK_WHITE;
	ret->attr.u1 = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->attr.u2 = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->attr.u3 = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->attr.u4 = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->attr.u5 = COLOR_BLACK | COLOR_BACK_BLACK;
	ret->buf = malloc(64);
	ret->buflen = 64;
	ret->driver = init_struct->_dbdrivers[0];
	ret->syserr[0] = 0;

#if 1
	ret->windows = NEW(ClipWindow);
#else
	if (all_windows)
		ret->windows = all_windows;
	else
		ret->windows = all_windows = NEW(ClipWindow);
#endif

	ret->wnum = 0;
	if (ret->screen)
	{
		ret->wboard.bottom = ret->windows->rect.bottom = ret->screen->base->Lines - 1;
		ret->wboard.right = ret->windows->rect.right = ret->screen->base->Columns - 1;
		ret->windows->format = ret->windows->rect;
	}
	ret->wshadow = -1;
#if 1
	if (all_container)
		ret->container = all_container;
	else
		ret->container = all_container = NEW(Container);
#else
	ret->container = NEW(Container);
#endif

	if (all_hashnames)
		ret->hashnames = all_hashnames;
	else
		ret->hashnames = all_hashnames = new_HashTable();
	ret->print_max_deep = CLIP_MAX_PRINT_DEEP;
	ret->print_deep = 0;

	{
		int i;

		for (i = 0; i < 256; i++)
		{
			ret->prntbl[i] = i;
			ret->prn_xlat[i] = i;
			ret->term_xlat[i] = i;
		}
	}

/*
	if (all_areas)
	{
		ret->areas = all_areas;
		ret->areaStack = all_areaStack;
	}
	else
	{
		ret->areas = all_areas = NEW(ClipVect);
		ret->areaStack = all_areaStack = NEW(ClipVect);
	}
*/
	ret->areas = NEW(ClipVect);
	ret->areaStack = NEW(ClipVect);

	ret->index_buffer_limit = 32;

	ret->flags |= ESCAPE_FLAG;
	ret->flags |= EXCLUSIVE_FLAG;
	ret->flags |= TRANSLATE_FLAG;
	ret->flags1 |= OPTIMIZE_FLAG;

	/*
#ifdef OS_LINUX
	if (ret->out && isatty(fileno(ret->out)))
		ret->flags1 |= FLUSHOUT_FLAG;
#endif
	*/
	ret->flags1 |= FLUSHOUT_FLAG;


	return ret;
}

char *_clip_progname = 0;
int _clip_argc = 0;
char **_clip_argv = 0;
char **_clip_envp = 0;
int log_level = 1;
FILE *logg = 0;
static int exited = 0;

int _clip_raw_argc = 0;
char **_clip_raw_argv = 0;

void
_clip_exit(void)
{
	/* close all ClipMachines */

	if (exited)
		return;
	exited = 1;


	while (first_mp)
	{
		ClipMachine *mp = first_mp;

		first_mp = first_mp->next;
		delete_ClipMachine(mp);
	}
	_clip_logg(2, "exit");
	if (logg != stderr)
		fclose(logg);

#ifdef MEMDEBUG
	malloc_shutdown();
#endif

}

#define M_OFFS(base,nl,ns) ((base)+8+nl*sizeof(long)+ns*sizeof(short))

static char *
get_env(char **envp, char *name)
{
	char *s;
	int l;
	int ln = strlen(name);

	for (; envp && (s = *envp); ++envp)
	{
		l = strcspn(s, "=");
		if (ln == l && !memcmp(name, s, l))
		{
			s += l;
			if (*s)
				return s + 1;
			else
				return s;
		}
	}
	return 0;
}

CLIP_DLLEXPORT char *
_clip_getenv(char *name)
{
	char *r;

	r = get_env(_clip_envp, name);
	return r;
}

static int
put_env(char ***envp, char *name)
{
	char *s;
	char **ep;
	int l, len, ln;

	ln = strcspn(name, "=");
	for (ep = *envp, len = 0; ep && (s = *ep); ++ep, ++len)
	{
		l = strcspn(s, "=");
		if (ln == l && !memcmp(name, s, l))
		{
			/* owerwrite */
			free(s);
			*ep = strdup(name);
			return 1;
		}
	}

	*envp = realloc(*envp, (len + 2) * sizeof(char *));

	(*envp)[len] = strdup(name);
	(*envp)[len + 1] = 0;

	return 0;
}

CLIP_DLLEXPORT int
_clip_putenv(char *name)
{
	int r;

	r = put_env(&_clip_envp, name);
	return r;
}

CLIP_DLLEXPORT int
_clip_put_env(char *name, char *val)
{
	int l1 = strlen(name);
	int l2 = strlen(val);
	int r;

#ifdef OS_MINGW
	char *buf = malloc(l1 + l2 + 2);
#else
	char *buf = alloca(l1 + l2 + 2);
#endif

	memcpy(buf, name, l1);
	memcpy(buf + l1 + 1, val, l2);
	buf[l1] = '=';
	buf[l1 + l2 + 1] = 0;
	r = put_env(&_clip_envp, buf);
#ifdef OS_MINGW
	free(buf);
#endif

	return r;
}

static void
set_sysvars(void)
{
	char *e;

	e = get_env(_clip_envp, "CLIP_LOGLEVEL");
	if (e && *e)
		log_level = atoi(e);

}

static void
process_environ(char *root, char *name)
{
	char buf[256];
	FILE *file;

	snprintf(buf, sizeof(buf), "%s/%s", root, name);
	file = fopen(buf, "rt");
	if (!file)
		goto ret;
	while (fgets(buf, sizeof(buf), file))
	{
		char *s, *e, *g;
		int l;
		int overwrite = 0;

		s = buf;

		  again:
		l = strspn(s, " \t");
		s += l;
		l = strlen(s);
		if (l < 2)
			continue;
		s[l - 1] = 0;
		if (s[0] == '#')
			continue;
		if (s[0] == '+')
		{
			s++;
			overwrite = 1;
		}
		else if (s[0] == '?')
		{
			char *g1;

			s++;
			e = strchr(s, '=');
			if (e)
				*e++ = 0;
			else
				continue;
			g = strchr(e + 1, '?');
			if (g)
				*g++ = 0;
			else
				continue;
			g1 = get_env(_clip_envp, s);
			if (!g1)
				continue;
			if (strcmp(e, g1))
				continue;
			/* condition matched */
			s = g;
			goto again;
		}

		if (!overwrite)
		{
			e = strchr(s, '=');
			if (e)
				*e = 0;
			g = get_env(_clip_envp, s);
			/* don't overwrite!! */
			if (g)
				continue;
			if (e)
				*e = '=';
		}
		put_env(&_clip_envp, s);
	}

	fclose(file);
	  ret:
	set_sysvars();
}

int _clip_debuglevel = 0;

static void
debug_enable(char *opt)
{
	char *s;

	s = strchr(opt, '=');
	if (s)
		_clip_debuglevel = atoi(s + 1);
	else
		_clip_debuglevel++;
/*_clip_logg(0, "set debug level to %d", _clip_debuglevel);*/
}

static void
license_on_start(char *opt)
{
	fprintf(stdout, "%s", CLIP_LICENSE);
	exit(0);
}

static void
profile_enable(char *opt)
{
	char *s;

	s = strchr(opt, '=');
	if (s)
		_clip_profiler = atoi(s + 1);
	else
		_clip_profiler++;
/*_clip_logg(0, "set profiledebug level to %d", _clip_debuglevel);*/
}

static int need_stop = 0;

static void
breakpoint_on_start(char *opt)
{
	debug_enable("");
	need_stop = 1;
}

struct OptionAction
{
	const char *name;
	void (*action) (char *opt);
};

static struct OptionAction hidden_options[] = {
	{"--debug", debug_enable},
	{"--profile", profile_enable},
	{"--stop", breakpoint_on_start},
	{"--license", license_on_start},
};

#define HIDDEN_NUM (sizeof(hidden_options)/sizeof(struct OptionAction))

static void
process_args(int argc, char **argv, int *Argcp, char ***Argvp)
{
	int i, j;

	for (i = 0; i < argc; i++)
	{
		int found = 0;

		for (j = 0; j < (int)HIDDEN_NUM; j++)
		{
			struct OptionAction *op;
			int l, l1;
			char *ap;

			op = hidden_options + j;
			ap = argv[i];

			l = strlen(op->name);
			l1 = strlen(ap);
			if ((l <= l1) && (l == l1 || ap[l] == '=') && !memcmp(op->name, ap, l))
			{
				op->action(ap);
				found = 1;
				break;
			}
		}
		if (!found)
		{
			int l;

			l = (*Argcp)++;
			(*Argvp) = realloc(*Argvp, sizeof(char *) * (l + 1));

			(*Argvp)[l] = argv[i];
		}
	}
}

int
_clip_init(ClipMachine * mp, int Argc, char **Argv, char **envp)
{
	char *e;
	ClipFunction **np, ****npp;
	const char **sp;
	ClipFile **fp, ****fpp;
	int ret;

	if (mp->inited)
		return 0;

	if (!init_struct)
		exit(100);

	mp->inited = 1;

	if (!logg)
		logg = stderr;

	if (!_clip_progname)
	{
		int arg_c = 0;
		char **arg_v = 0;
		char *p1, *p2;

		_clip_put_env("CLIP_HOSTCS", init_struct->CLIP_HOSTCS);

		_clip_raw_argc = Argc;
		_clip_raw_argv = Argv;

		process_args(Argc, Argv, &arg_c, &arg_v);

		_clip_argc = arg_c;
		_clip_argv = arg_v;

		e = strrchr(arg_v[0], '/');

		if (e)
			_clip_progname = e + 1;
		else
			_clip_progname = arg_v[0];

		for (; (e = *envp); ++envp)
			put_env(&_clip_envp, e);

		atexit(_clip_exit);

#ifndef OS_MINGW
		signal(SIGHUP, _clip_signal);
		signal(SIGQUIT, _clip_signal);
		signal(SIGKILL, _clip_signal);
		signal(SIGPIPE, _clip_signal);
		signal(SIGALRM, _clip_signal);
		if (_clip_debuglevel)
			signal(SIGUSR1, _clip_sigdebug);
		else
			signal(SIGUSR1, SIG_IGN);
#endif

		signal(SIGINT, _clip_sigint);
		signal(SIGILL, _clip_signal);
		signal(SIGABRT, _clip_signal);
		signal(SIGFPE, _clip_signal);
		signal(SIGSEGV, _clip_signal);
		signal(SIGTERM, _clip_signal);

		set_sysvars();

		_clip_logg(2, "start");

		/* read CLIPROOT/environment */
		e = get_env(_clip_envp, "CLIPROOT");
		if (e && *e)
			CLIPROOT = strdup(e);

		process_environ(CLIPROOT, "etc/environment");
		e = get_env(_clip_envp, "CLIP_LANG");
		if (e == NULL)
			e = get_env(_clip_envp, "LANG");
		if (e && *e)
		{
			char buf[256];

			snprintf(buf, sizeof(buf), "%s/lang", CLIPROOT);
			_clip_logg(3, "process LANG environ '%s/%s'", buf, e);
			process_environ(buf, e);
		}
		e = get_env(_clip_envp, "CHARSET");
		if (e && *e)
		{
			char buf[256];

			snprintf(buf, sizeof(buf), "%s/lang", CLIPROOT);
			_clip_logg(3, "process CHARSET environ '%s/%s'", buf, e);
			process_environ(buf, e);
		}
		e = get_env(_clip_envp, "TERM");
		if (e && *e)
		{
			char buf[256];

			snprintf(buf, sizeof(buf), "%s/term", CLIPROOT);
			_clip_logg(3, "process TERM environ '%s/%s'", buf, e);
			process_environ(buf, e);
		}

		e = _clip_getenv("CLIP_LOCALE");
		if (!e)
		{
			e = _clip_getenv("CLIP_LANG");
			if (e == NULL)
				e = _clip_getenv("LANG");
			if (e && !strcmp(e, "C"))
				e = 0;
			if (!e)
			{
				e = _clip_getenv("LC_ALL");
				if (e && !strcmp(e, "C"))
					e = 0;
			}
			if (!e)
			{
				e = _clip_getenv("LC_MESSAGES");
				if (e && !strcmp(e, "C"))
					e = 0;
			}
			if (e)
				_clip_put_env("CLIP_LOCALE", e);
		}

		e = _clip_getenv("CLIP_CLIENTCS");
		if (!e || !*e)
		{
			e = _clip_getenv("CLIP_LOCALE");
			if (e)
			{
				char *s;

				s = strrchr(e, '.');
				if (s)
					e = s + 1;
				else
					e = 0;
			}
			if (!e)
				e = _clip_getenv("CLIP_CHARSET");
			if (!e)
				e = _clip_getenv("CHARSET");
			if (e)
			{
				char *s;

				s = strdup(e);
				for (e = s; *e; e++)
					*e = tolower(*e);
				_clip_put_env("CLIP_CLIENTCS", s);
				free(s);
			}
		}
		p1 = _clip_getenv("CLIP_HOSTCS");
		p2 = _clip_getenv("CLIP_CLIENTCS");
		if (p1 && !strcasecmp(p1, "c") && p2 && strcasecmp(p2, "c"))
			_clip_put_env("CLIP_HOSTCS", p2);
		e = _clip_getenv("CLIP_HOSTCS");
		if (e && *e)
		{
			_clip_logg(3, "load charset tables '%s'", e);
			load_charset_tables(e, _clip_cmptbl, _clip_uptbl, _clip_lowtbl, _clip_isalpha_tbl, _clip_pgtbl);
			_clip_hostcs = strdup(e);
		}

		p1 = _clip_getenv("CLIP_HOSTCS");
		p2 = _clip_getenv("CLIP_CLIENTCS");
		if (p1 && p2 && strcmp(p1, p2))
		{
			cons_CharsetEntry *cs1 = 0, *cs2 = 0;
			int len1 = 0, len2 = 0;

			_clip_logg(2, "set stream io translation  %s -> %s", p1, p2);

			if (load_charset_name(p1, &cs1, &len1))
			{
				_clip_logg(0, "cannot load charset file '%s': %s", p1, strerror(errno));
			}

			if (load_charset_name(p2, &cs2, &len2))
			{
				_clip_logg(0, "cannot load charset file '%s': %s", p2, strerror(errno));
			}

			if (cs1 && cs2)
			{
				make_translation(cs1, len1, cs2, len2, _clip_outtbl);
				make_translation(cs2, len2, cs1, len1, _clip_intbl);
			}

			free(cs1);
			free(cs2);
		}

		e = get_env(_clip_envp, "CLIP_LOCALE_ROOT");
		if (e && *e)
			CLIP_LOCALE_ROOT = strdup(e);

		e = _clip_getenv("CLIP_LOCALE");
		if (e && *e)
		{
			_clip_add_locale(e);
		}
	}

	for (npp = init_struct->_libinits; *npp; ++npp)
	{
		for (np = **npp; *np; ++np)
			if ((ret = _clip_main_func(mp, *np, _clip_argc, _clip_argv, _clip_envp)))
				return ret;
	}
	for (np = init_struct->_inits; *np; ++np)
		if ((ret = _clip_main_func(mp, *np, _clip_argc, _clip_argv, _clip_envp)))
			return ret;

	for (fpp = init_struct->_libcpfiles; *fpp; ++fpp)
	{
		for (fp = **fpp; *fp; ++fp)
			_clip_register_file(mp, *fp);
	}
	for (fp = init_struct->_cpfiles; *fp; ++fp)
	{
		ClipFile *file = *fp;

		_clip_register_file(mp, file);
	}

	for (fpp = init_struct->_libcpfiles; *fpp; ++fpp)
	{
		for (fp = **fpp; *fp; ++fp)
			_clip_load_inits(mp, *fp);
	}
	for (fp = init_struct->_cpfiles; *fp; ++fp)
	{
		ClipFile *file = *fp;

		_clip_load_inits(mp, file);
	}

	for (sp = init_struct->_pfunctions; *sp; ++sp)
		if ((ret = _clip_load(mp, *sp, 0, 0)))
			return ret;

	if (need_stop)
	{
		need_stop = 0;
		_clip_in_breakpoint();
	}

	return 0;
}

int
_clip_register_file(ClipMachine * mp, ClipFile * file)
{
	char *modp = file->body;
	char *modbeg = M_OFFS(modp, 2, 0);
	long funcOffs = GETLONG(M_OFFS(modp, 7, 4));
	int funcnum = GETLONG(M_OFFS(modp, 4, 2));
	char *bp = (modbeg + funcOffs);
	int i;

	_clip_hash_buckets(mp, file);

	for (i = 0; i < funcnum; ++i, bp += 2 * sizeof(long))
	{
		long hash = GETLONG(bp);
		char *func = modbeg + GETLONG(bp + sizeof(long));
		ClipBlock block;

		block.file = file;
		block.func = func;
		if (_clip_register_block(mp, block, hash))
			++file->refCount;
	}

	return 0;
}

void
_clip_log_init(char *filename)
{
	static int inited = 0;
	char buf[256];

	if (filename == NULL)
	{
		if (inited)
			return;
		inited = 1;
		sprintf(buf, "%s.log", _clip_progname);
		logg = fopen(buf, "at");
		if (!logg)
		{
			fprintf(stderr, "cannot open log file '%s'", buf);
			logg = stderr;
		}
	}
	else
	{
		inited = 1;
		if (logg != stderr)
			fclose(logg);
		logg = fopen(filename, "at");
		if (!logg)
		{
			fprintf(stderr, "cannot open log file '%s'", buf);
			logg = stderr;
		}
	}
}

int
_clip_loglevel(int newlevel)
{
	int ret = log_level;

	if (newlevel >= 0)
		log_level = newlevel;
	return ret;
}

void
_clip_out_log(char *buf, int len)
{
	_clip_log_init(NULL);
	fwrite(buf, len, 1, logg);
}

void
_clip_flush_log()
{
	fflush(logg);
}

CLIP_DLLEXPORT void
_clip_logg(int level, const char *fmt, ...)
{
	va_list ap;
	time_t tt;
	struct tm *tp;

	if (level > log_level)
		return;

	_clip_log_init(NULL);
	if (!logg)
		goto ret;
	tt = time(0);
	tp = localtime(&tt);

	fprintf(logg, "%02d:%02d:%02d %s:%lu:%d ", tp->tm_hour, tp->tm_min, tp->tm_sec, _clip_progname,
		(unsigned long) getpid(), level);
	va_start(ap, fmt);
	vfprintf(logg, _clip_gettext(fmt), ap);
	va_end(ap);
	fputc('\n', logg);
	fflush(logg);

	  ret:
		;
}

void
_clip_signal(int sig)
{
	static int oldsig = 0;
	static int cnt = 0;

	if(cnt >= 10)
		exit(100+sig);
	if(sig == oldsig)
		cnt++;
	else
		cnt = 0;
	oldsig = sig;
	_clip_logg(0, "CLIP VM debug signal %d #%d",sig,cnt);
	_clip_sig_flag = sig;
}

void
_clip_signal_real(int sig)
{
	const char *msg;
	ClipFrame *fp;
	ClipMachine *mp;

	switch (sig)
	{
	case 0:
		return;
#ifndef OS_MINGW
	case SIGHUP:
		msg = "SIGHUP";
		break;
	case SIGQUIT:
		msg = "SIGQUIT";
		break;
	case SIGKILL:
		msg = "SIGKILL";
		break;
	case SIGPIPE:
		msg = "SIGPIPE";
		break;
	case SIGALRM:
		msg = "SIGALRM";
		break;
#endif
	case SIGINT:
		msg = "SIGINT";
		break;
	case SIGILL:
		msg = "SIGILL";
		break;
	case SIGABRT:
		msg = "SIGABRT";
		break;
	case SIGFPE:
		msg = "SIGFPE";
		break;
	case SIGSEGV:
		msg = "SIGSEGV";
		break;
	case SIGTERM:
		msg = "SIGTERM";
		break;
	default:
		msg = "UNKNOWN";
		break;
	}
	mp = first_mp; /*cur_ClipMachine();*/
	_clip_sig_flag = 0;
	_clip_trap_err(mp, EG_SIGNAL, 0, 0, "SIGNAL", sig, msg);
	_clip_call_errblock(mp, EG_SIGNAL);

	_clip_logg(0, "got signal %s, exiting...", msg);
	fprintf(stderr, "\n%s: got signal %s, exiting...\n", _clip_progname, msg);
	fflush(stderr);

	for (fp = cur_mp->fp; fp; fp = fp->up)
	{
		_clip_logg(0, "trace: file '%s' line %d", fp->filename, fp->line);
		fprintf(stderr, "trace: file '%s' line %d\n", fp->filename, fp->line);
		fflush(stderr);
	}

/*_clip_exit();*/
	exit(100 + sig);
}

ClipMachine *
cur_ClipMachine()
{
	/* thread-depend code */
	return cur_mp;
}

static VarEntry *
new_VarEntry(long hash)
{
	NEWVAR(VarEntry, ret);
	ret->hash = hash;
	return ret;
}

static void
delete_VarEntry(ClipMachine * mp, VarEntry * vp)
{
	if (!vp)
		return;
	if (vp->next)
		delete_VarEntry(mp, vp->next);
	_clip_destroy(mp, &vp->var);
	free(vp);
}

static void
delete_VarTable(ClipMachine * mp, HashTable * hp)
{
	int r;

	for (r = HashTable_first(hp); r; r = HashTable_next(hp))
	{
		VAR(VarEntry, vp, HashTable_current(hp));
		delete_VarEntry(mp, vp);
	}
	delete_HashTable(hp);
}

static void
clear_VarTable(ClipMachine * mp, HashTable * hp)
{
	int r;

	for (r = HashTable_first(hp); r; r = HashTable_next(hp))
	{
		VAR(VarEntry, vp, HashTable_current(hp));
		delete_VarEntry(mp, vp);
	}
	HashTable_clear(hp);
}

static void
delete_spaces(ClipMachine * mp, HashTable * hp)
{
	int r;

	for (r = HashTable_first(hp); r; r = HashTable_next(hp))
	{
		VAR(HashTable, vp, HashTable_current(hp));
		delete_VarTable(mp, vp);
	}
	delete_HashTable(hp);
}

static void
delete_Functions(ClipMachine * mp, HashTable * hp)
{
	int r;

	for (r = HashTable_first(hp); r; r = HashTable_next(hp))
	{
		VAR(FuncEntry, fp, HashTable_current(hp));
		/*if (!fp->f)
		   delete_ClipFile(mp, fp->block.file); */
		free(fp);
	}
	delete_HashTable(hp);
}

void
delete_ClipMachine(ClipMachine * mp)
{
	FILE *out;
	int i;
	ClipFunction **np, ****npp;
	int r;

	if (mp->deleted)
		return ;
	mp->deleted = 1;

	if (_clip_profiler)
	{
		fini_profiler(mp);
	}

	if (mp==first_mp)
		first_mp = mp->next;
	else
	{
		ClipMachine *mpp;
		for(mpp = first_mp; mpp && mpp->next; mpp = mpp->next)
		{
			if(mpp->next == mp)
			{
				mpp->next = mp->next;
				break;
			}
		}
	}

	/*if (machineCount == 1)*/
	{
		_clip_close_all(mp);
		clip_RDDCLOSEALL(mp);
	}

	for (i = mp->nexits - 1; i >= 0; --i)
	{
		_clip_main_code(mp, mp->exits + i, _clip_argc, _clip_argv, _clip_envp);
		destroy_Block(mp, mp->exits + i);
	}

	for (i = mp->ncexits - 1; i >= 0; --i)
		_clip_main_func(mp, mp->cexits[i], _clip_argc, _clip_argv, _clip_envp);

	for (npp = init_struct->_libexits; *npp; ++npp)
	{
		for (np = **npp; *np; ++np)
			_clip_main_func(mp, *np, _clip_argc, _clip_argv, _clip_envp);
	}
	for (np = init_struct->_exits; *np; ++np)
		_clip_main_func(mp, *np, _clip_argc, _clip_argv, _clip_envp);

	clear_VarTable(mp, mp->privates);
	/*clear_VarTable(mp, mp->publics);*/

	for (i = 0; i < mp->dlls.count; ++i)
#ifdef OS_MINGW
		lt_dlclose(mp->dlls.items[i]);
#else
		dlclose(mp->dlls.items[i]);
#endif

	out = (FILE *) mp->out;
	if (out != stdout)
		fclose(out);
	out = (FILE *) mp->extra;
	if (out)
		fclose(out);
	out = (FILE *) mp->alt;
	if (out)
		fclose(out);
	_clip_close_printer(mp);

	if(mp->pbuf)
	{
		destroy_Buf(mp->pbuf);
		free(mp->pbuf);
	}

	delete_VarTable(mp, mp->privates);
	delete_spaces(mp, mp->spaces);
	delete_HashTable(mp->aliases);
	delete_HashTable(mp->fields);

	if (machineCount == 1)
	{
		delete_VarTable(mp, mp->publics);
		all_publics = 0;
		all_aliases = 0;
		all_fields = 0;
		for (r = HashTable_first(mp->store); r; r = HashTable_next(mp->store))
			free(HashTable_current(mp->store));
		delete_HashTable(mp->store);
		all_store = 0;
	}

	if (machineCount == 1)
	{
		for (r = HashTable_first(mp->hashnames); r; r = HashTable_next(mp->hashnames))
			free(HashTable_current(mp->hashnames));
		delete_HashTable(mp->hashnames);
		all_hashnames = 0;
	}

	_clip_delete(mp, mp->trapVar);
	_clip_delete(mp, mp->errorblock);
	_clear_trap(mp);

/*	if (machineCount == 1)
	{*/
		destroy_ClipVect(mp->areas);
		destroy_ClipVect(mp->areaStack);
		free(mp->areas);
		free(mp->areaStack);
/*	}*/

	free(mp->date_format);
	free(mp->path);
	free(mp->defaul);
	free(mp->altfile);
	free(mp->extrafile);
	if ( mp->rootpath != NULL )
		free(mp->rootpath);
	free(mp->delimchars);
	if (machineCount == 1)
	{
		delete_Functions(mp, mp->functions);
		all_functions = 0;
	}

	free(mp->buf);

	for (i = 0; i < mp->history.count; ++i)
		free(mp->history.items[i]);

	destroy_ClipVect(&mp->history);

	free(mp->exits);
	free(mp->cexits);
	/*free(*mp->dbdrivers);*/

	if (machineCount == 1)
		free(mp->kbdbuf);

	destroy_ClipVect(&mp->dlls);

	if (mp->screen_base)
	{
		if (mp->screen)
			delete_Screen(mp->screen);
		destroy_tty(mp->screen_base);
		free(mp->screen_base);
	}

	/*if ( machineCount == 1 )*/
	{
		/*all_windows = 0;*/
		for (i = 0; i <= mp->wnum; i++)
		{
			ClipWindow *wp = mp->windows + i;

			if (wp->save)
				free(wp->save);
		}
		free(mp->windows);
	}

	--machineCount;

	free(mp);
}

CLIP_DLLEXPORT void
_clear_trap(ClipMachine * mp)
{
	int i;

	for (i = 0; i < mp->trapTrace.count; ++i)
		free(mp->trapTrace.items[i]);
	clear_ClipVect(&mp->trapTrace);
}

CLIP_DLLEXPORT void
_clip_log_call(ClipMachine * mp)
{
	ClipFrame *fp = mp->fp;
	if (_clip_profiler)
		_clip_start_profiler(mp);

	if (log_level >= 5)
	{
		_clip_logg(5, "C call: proc '%s' file '%s' line %d", fp->procname ? fp->procname : "unknown", fp->filename,
			   fp->line);
	}
}

CLIP_DLLEXPORT int
_clip_errorblock(ClipMachine * mp, ClipVar * vp, int genCode)
{
	int r;
	ClipVar res;
	memset(&res, 0, sizeof(res));

	r = _clip_errorblock_res(mp, vp, genCode, &res);

	_clip_destroy(mp, &res);

	if (!r)
		r = _clip_log(&res);
	else
		r = 0;


	return 0;
}


CLIP_DLLEXPORT int
_clip_errorblock_res(ClipMachine * mp, ClipVar * vp, int genCode, ClipVar *res)
{
	int r;

	if (mp->noerrblock)
		return 0;

	/* default err handler */
	if (!mp->errorblock || (_clip_type(mp->errorblock) != CCODE_t && _clip_type(mp->errorblock) != PCODE_t))
	{
		char *msg = 0;
		int i;

		descr_str(mp, vp, &msg);
		if (!msg)
			msg = "unknown runtime error";
		printf("\n%s: system errorblock:\n", _clip_progname);
		printf("%s: runtime error: %s\n", _clip_progname, msg);
		_clip_logg(0, "%s: systerm errorblock: runtime error: %s", _clip_progname, msg);
		for (i = 0; i < mp->trapTrace.count; ++i)
		{
			printf("trace: %s\n", (char *) mp->trapTrace.items[i]);
			_clip_logg(0, "trace: %s", (char *) mp->trapTrace.items[i]);
		}
		clip___QUIT(mp);
	}

	mp->errorlevel++;
	if (mp->errorlevel > CLIP_MAX_ERRORBLOCK_DEEP)
	{
		int i;

		printf("%s: errorblock nested too deep (%d):\n", _clip_progname, CLIP_MAX_ERRORBLOCK_DEEP);
		_clip_logg(0, "%s: errorblock nested too deep (%d): ", _clip_progname, CLIP_MAX_ERRORBLOCK_DEEP);
		for (i = 0; i < mp->trapTrace.count; ++i)
		{
			printf("trace: %s\n", (char *) mp->trapTrace.items[i]);
			_clip_logg(0, "trace: %s", (char *) mp->trapTrace.items[i]);
		}

		clip___QUIT(mp);
	}


	if (_clip_type(vp) == MAP_t)
	{
		double d = 0;

		_clip_mgetn(mp, vp, HASH_gencode, &d);
		if (d == 0.0)
		{
			_clip_var_num(genCode, res);
			_clip_madd(mp, vp, HASH_gencode, res);
			_clip_destroy(mp, res);
		}
	}

	r = _clip_eval(mp, mp->errorblock, 1, vp, res);
	mp->errorlevel--;

	return r;
}

static int
do_call(ClipMachine * mp, ClipFunction f, int argc, int isProc, int rest, ClipVarFrame * uplocals)
{
	int ret, _argc = mp->argc;
	ClipFrame *fp = mp->fp;
	ClipVar *bp = mp->bp;
	ClipVarFrame *s_up = mp->fp->localvars;

	mp->argc = argc;
	mp->bp = fp->sp;
	if (uplocals)
		mp->fp->localvars = uplocals;

	if (_clip_profiler)
	{
		_clip_stop_profiler(mp);
	}

	if (log_level >= 6)
	{
		_clip_logg(6, "C call from: proc '%s' file '%s' line %d", fp->procname ? fp->procname : "unknown", fp->filename,
			   fp->line);
	}

	for (;;)
	{
		int dnext = 0;

		if (_clip_debuglevel)
		{
			dnext = _clip_debugnext;
			/*printf("-dnext=%d line %d\n", dnext, fp->line); */
			_clip_debug(mp);
		}

#ifdef USE_TASKS
		Task_yield();
#endif

		ret = f(mp);

		if (_clip_sig_flag)
			_clip_signal_real(_clip_sig_flag);

		if (_clip_debuglevel)
		{
			/*printf("+dnext=%d _clip_debugnext=%d line %d\n", dnext, _clip_debugnext, fp->line); */
			if (!dnext && _clip_debugnext)
			{
				_clip_debugnext = 0;
/*_clip_debug(mp);*/
			}
		}

		if (log_level >= 6)
			_clip_logg(6, "C return=%d to: proc '%s' file '%s' line %d", ret,
				   fp->procname ? fp->procname : "unknown", fp->filename, fp->line);

		if (!ret)
			break	;

		if (!mp->trapVar)
			_clip_generr(mp, ret);

		if (ret < 0)
			break;

		if (!_clip_errorblock(mp, mp->trapVar, ret))
		{
			ret = -ret;
			break;
		}
		mp->argc = _argc;
		mp->fp = fp;
		mp->bp = bp;
		mp->fp->localvars = s_up;
	}

	mp->argc = _argc;
	mp->fp = fp;
	mp->bp = bp;
	mp->fp->localvars = s_up;

	if (isProc)
		++argc;
	for (; argc; --argc)
	{
		--(fp->sp);
		_clip_destroy(mp, fp->sp);
	}

	if (_clip_profiler)
	{
		_clip_start_profiler(mp);
		mp->pbucket->callno--;
	}

	return ret;
}

static void
init_profiler(ClipMachine *mp)
{
	if(mp->profiler)
		return;

	mp->profiler = new_HashTable();
}


#if 0
static void
delete_profile_bucket(void *bpp)
{
	ProfileBucket *bp = (ProfileBucket*) bpp;
	if (!bp)
		return;
	free(bp->procname);
	free(bp->filename);
#if 0
	destroy_Coll(&(bp->callby));
#endif
	free(bp);
}

static int
cmp_profile_bucket(void *p1, void *p2)
{
	long h1 = ((ProfileBucket *)p1)->hash;
	long h2 = ((ProfileBucket *)p2)->hash;
	if (h1<h2)
		return -1;
	else if (h1>h2)
		return 1;
	else
		return 0;
}

#endif

static ProfileBucket *
find_profile_bucket_name(ClipMachine *mp, const char *procname, const char *filename)
{
	long hash;
	ProfileBucket *bp;

	hash = _clip_casehashbytes(0, procname, strlen(procname));
	hash = _clip_casehashbytes(hash, filename, strlen(filename));

	init_profiler(mp);
	bp = (ProfileBucket*) HashTable_fetch(mp->profiler, hash);
	if (!bp)
	{
		bp = NEW(ProfileBucket);
		bp->hash = hash;
		bp->procname = strdup(procname);
		bp->filename = strdup(filename);
#if 0
		init_Coll(&bp->callby, delete_profile_bucket, cmp_profile_bucket);
#endif
		HashTable_insert(mp->profiler, bp, hash);
	}

	return bp;
}

static ProfileBucket *
find_profile_bucket(ClipMachine *mp)
{
	const char *fs = "", *ps = "";

	if (mp->fp && mp->fp->procname)
		ps = mp->fp->procname;
	if (mp->fp && mp->fp->filename)
		fs = mp->fp->filename;

	return find_profile_bucket_name(mp, ps, fs);
}

#ifndef timer_add
#define	timer_add(a, b, result)							  \
  do {										  \
	(result)->tv_sec = (a)->tv_sec + (b)->tv_sec;				  \
	(result)->tv_usec = (a)->tv_usec + (b)->tv_usec;				  \
	if ((result)->tv_usec >= 1000000)						  \
	  {										  \
	++(result)->tv_sec;							  \
	(result)->tv_usec -= 1000000;						  \
	  }										  \
  } while (0)
#endif

#ifndef timer_sub
#define timer_sub(a, b, result) \
   do {                         \
	   (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;     \
	   (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;  \
	   if ((result)->tv_usec < 0) {                      \
		  --(result)->tv_sec;                        \
		  (result)->tv_usec += 1000000;              \
	   }                                                 \
   } while (0)
#endif



static int
start_profiler(ClipMachine *mp, ProfileBucket *bp)
{
	ProfileBucket *pbp;
	struct timeval tv;

	pbp = mp->pbucket;

	if(mp->pbucket && mp->pbucket->started)
		_clip_stop_profiler(mp);

	if (bp->started)
	{
		mp->pbucket = bp;
		_clip_stop_profiler(mp);
	}

	gettimeofday(&tv, 0);
	bp->start = tv;
	mp->pbucket = bp;
	bp->started = 1;
	bp->callno ++;

	return 0;
}

int CLIP_DLLEXPORT
_clip_start_profiler(ClipMachine *mp)
{
	ProfileBucket *bp;

	bp = find_profile_bucket(mp);

	return start_profiler(mp, bp);
}

int CLIP_DLLEXPORT
_clip_start_profiler_name(ClipMachine *mp, const char *procname, const char *filename)
{
	ProfileBucket *bp;

	if (!_clip_profiler)
		return 0;

	bp = find_profile_bucket_name(mp, procname, filename);

	return start_profiler(mp, bp);
}

static int
stop_profiler(ClipMachine *mp, ProfileBucket *bp, struct timeval *tv)
{
	struct timeval dt, rt;

	if (!bp->started)
		return 0;

	timer_sub(tv, &bp->start, &dt);

	timer_add(&bp->self, &dt, &rt);
	bp->self = rt;
	bp->started = 0;

	return 0;
}

int CLIP_DLLEXPORT
_clip_stop_profiler(ClipMachine *mp)
{
	struct timeval tv;
	ProfileBucket *bp;

	gettimeofday(&tv, 0);
	bp = mp->pbucket;
	if (!bp)
		bp = find_profile_bucket(mp);

	return stop_profiler(mp, bp, &tv);
}

int CLIP_DLLEXPORT
_clip_stop_profiler_name(ClipMachine *mp, const char *procname, const char *filename)
{
	struct timeval tv;
	ProfileBucket *bp;

	if (!_clip_profiler)
		return 0;

	gettimeofday(&tv, 0);
	bp = find_profile_bucket_name(mp, procname, filename);

	return stop_profiler(mp, bp, &tv);
}

static int
cmp_profile_out(void *p1, void *p2)
{
	ProfileBucket *bp1 = (ProfileBucket*) p1;
	ProfileBucket *bp2 = (ProfileBucket*) p2;

	if (bp1->self.tv_sec > bp2->self.tv_sec)
		return -1;
	else if (bp1->self.tv_sec < bp2->self.tv_sec)
		return 1;
	else if (bp1->self.tv_usec > bp2->self.tv_usec)
		return -1;
	else if (bp1->self.tv_usec < bp2->self.tv_usec)
		return 1;
	else
		return 0;
#if 0
	{
	int r;
	r = strcasecmp(bp1->procname, bp2->procname);
	if (r)
		return r;
	r = strcasecmp(bp1->filename, bp2->filename);
	return r;
	}
#endif
}

static void
fini_profiler(ClipMachine *mp)
{
	Coll coll;
	int r, i;
	FILE *out;
	char path[256];
	struct timeval total;
	unsigned long ms, us;

	init_profiler(mp);
	/* generate report here */

	init_Coll(&coll, 0, cmp_profile_out);

	snprintf(path, sizeof(path), "%s.pro", _clip_progname);
	out = fopen(path, "w");
	if (!out)
		out = stderr;


	for(r = HashTable_first(mp->profiler); r; r = HashTable_next(mp->profiler))
	{
		ProfileBucket *bp = (ProfileBucket *) HashTable_current(mp->profiler);
		insert_Coll(&coll, bp);
	}

	total.tv_sec = 0;
	total.tv_usec = 0;
	for(i = 0; i < coll.count; i++)
	{
		ProfileBucket *bp = (ProfileBucket *) coll.items[i];
		struct timeval tv;

		if (!bp->procname[0] && !bp->filename[0])
			continue;

		ms = bp->self.tv_sec * 1000 + bp->self.tv_usec/1000;
		us = bp->self.tv_usec%1000;

		tv = total;
		timer_add(&tv, &bp->self, &total);

		fprintf(out, "%18s %-13s %8ld calls %8lu.%03lu ms\n", bp->procname, bp->filename, bp->callno, ms, us);
	}

	ms = total.tv_sec * 1000 + total.tv_usec/1000;
	us = total.tv_usec%1000;
	fprintf(out, "---------------------------------\n");
	fprintf(out, "total registered %8lu.%03lu ms\n", ms, us);

	destroy_Coll(&coll);

	if (out != stderr)
		fclose(out);
}


static int
_clip_refclone(ClipMachine * mp, ClipVar * dest, ClipVar * src)
{
	if (src->t.flags == F_MREF)
	{
		_clip_destroy(mp, dest);
		dup_ref(dest, src);
		return 0;
	}
	else
		return _clip_clone(mp, dest, src);
}

CLIP_DLLEXPORT int
_clip_eval(ClipMachine * mp, ClipVar * blockp, int argc, ClipVar * stackp, ClipVar * retp)
{
#ifdef OS_MINGW
	ClipVar *stack = malloc(sizeof(ClipVar) * (argc + 1));
#else
	ClipVar *stack = alloca(sizeof(ClipVar) * (argc + 1));
#endif
	ClipFrame frame = { stack, stack + 1 + argc, __file__, __LINE__, 0, 0, 0, 0, 0, 0, "eval",
		argc + 1, 0
	};
	enum ClipVarType type = _clip_type(blockp);
	int i, j, r;

	if (type != PCODE_t && type != CCODE_t)
	{
		_clip_trap_str(mp, __file__, __LINE__, "clip_eval: non-code argument");
#ifdef OS_MINGW
		free(stack);
#endif
		return _clip_call_errblock(mp, 1);
	}

	if (_clip_profiler)
	{
		_clip_stop_profiler(mp);
	}

	blockp = _clip_vptr(blockp);
	memset(stack, 0, sizeof(ClipVar) * (argc + 1));

	for (i = 1, j = 0; j < argc; ++j, ++i)
	{
		_clip_refclone(mp, stack + i, stackp + j);
	}

	frame.up = mp->fp;
	mp->fp = &frame;

	if (_clip_profiler)
	{
		_clip_stop_profiler(mp);
	}

	if (type == CCODE_t)
		r = _clip_func(mp, blockp->c.u.func, argc, 0, blockp->c.uplocals);
	else
		r = _clip_code_func(mp, blockp->c.u.block, argc, 0, blockp->c.uplocals);

	mp->fp = frame.up;
	if (retp)
		*retp = stack[0];
	else
		_clip_destroy(mp, stack + 0);

	if (_clip_profiler)
	{
		_clip_start_profiler(mp);
		mp->pbucket->callno--;
	}


#ifdef OS_MINGW
	free(stack);
#endif
	return r;
}

int
clip_CLIP(ClipMachine * mp)
{
	char *funcname = _clip_parc(mp, 1);
	int i, parcount = _clip_parinfo(mp, 0);
	ClipVar *params, *retVal = RETPTR(mp);

	if (funcname == NULL)
	{
		int r;

		r = _clip_trap_err(mp, EG_ARG, 0, 0, __FILE__, __LINE__, "CLIP");
		return _clip_call_errblock(mp, r);
	}
	params = malloc(parcount * sizeof(ClipVar));
	for (i = 2; i <= parcount; i++)
		params[i - 2] = *_clip_spar(mp, i);

	_clip_clip(mp, funcname, parcount - 1, params, retVal);

	free(params);
	return 0;
}

int
clip_CLIPA(ClipMachine * mp)
{
	char *funcname = _clip_parc(mp, 1);
	int parcount;
	ClipVar *params, *retVal = RETPTR(mp);
	ClipVar *ap = _clip_par(mp, 2);

	if (!funcname || !ap || ap->t.type != ARRAY_t)
	{
		int r;

		r = _clip_trap_err(mp, EG_ARG, 0, 0, __FILE__, __LINE__, "CLIPA");
		return _clip_call_errblock(mp, r);
	}

	parcount  = ap->a.count;
	params = ap->a.items;

	_clip_clip(mp, funcname, parcount, params, retVal);

	return 0;
}

CLIP_DLLEXPORT int
_clip_clip(ClipMachine * mp, const char *funcname, int argc, ClipVar * stackp, ClipVar * retp)
{
	ClipVar *stack;
	ClipFrame frame = { 0, 0, __file__, __LINE__, 0, 0, 0, 0, 0, 0, "clip", 0, 0};
	int i, j, r;
	ClipFunction *fp;
	ClipBlock *bp;
	long hash;

	hash = _clip_casehashbytes(0, funcname, strlen(funcname));
	if (!_clip_get_function(mp, hash, &fp, &bp))
	{
		return 1;
	}

	stack = malloc(sizeof(ClipVar) * (argc + 1));
	frame.stack = stack;
	frame.sp = stack + 1 + argc;
	memset(stack, 0, sizeof(ClipVar) * (argc + 1));

	for (i = 1, j = 0; j < argc; ++j, ++i)
		_clip_clone(mp, stack + i, stackp + j);

	frame.up = mp->fp;
	mp->fp = &frame;

	if (fp)
		r = _clip_func(mp, fp, argc, 0, 0);
	else
		r = _clip_code_func(mp, bp, argc, 0, 0);

	mp->fp = frame.up;
	if (retp)
		*retp = stack[0];
	else
		_clip_destroy(mp, stack + 0);

	free(stack);
	return r;
}

CLIP_DLLEXPORT void
_clip_push_func(struct ClipMachine *mp, ClipFunction f, int nlocals, int nolocals)
{
	ClipVar *sp;
	ClipVar *vp;
	ClipFrame *fp = mp->fp;

	vp = NEW(ClipVar);

	vp->t.count = 1;
	vp->t.type = CCODE_t;
	vp->t.flags = F_NONE;
	vp->c.u.func = f;

	if (nlocals)
	{
		ClipVarFrame *localvars = calloc(1, sizeof(ClipVarFrame) + nlocals * sizeof(ClipVar));

		localvars->vars = (ClipVar *) (localvars + 1);
		memcpy(localvars->vars, mp->fp->sp - nlocals, nlocals * sizeof(ClipVar));
		localvars->refcount = 1;
		localvars->size = nlocals;
		vp->c.uplocals = localvars;
		mp->fp->sp -= nlocals;
	}
	else if (!nolocals && fp->localvars && fp->localvars->refcount > 0)
	{
		fp->localvars->refcount++;
		vp->c.uplocals = fp->localvars;
	}

	sp = mp->fp->sp++;
	CLIP_CHECK_STACK;

	CLEAR_CLIPVAR(sp);
	sp->t.flags = F_MPTR;
	sp->t.type = CCODE_t;
	sp->p.vp = vp;

}

CLIP_DLLEXPORT int
_clip_func(ClipMachine * mp, ClipFunction f, int argc, int rest, ClipVarFrame * uplocals)
{
	int r;
	ClipFrame *inMacro = mp->inMacro;

	mp->inMacro = 0;
	r = do_call(mp, f, argc, 0, rest, uplocals);
	mp->inMacro = inMacro;
	return r;
}

CLIP_DLLEXPORT int
_clip_func_name(struct ClipMachine *mp, int argc)
{
	ClipFrame *fp = mp->fp;
	ClipVar *vp = _clip_vptr(fp->sp - argc - 2);
	long hash;
	int ret;

	if (vp->t.type == CHARACTER_t)
		hash = _clip_casehashbytes(0, vp->s.str.buf, vp->s.str.len);
	else if (vp->t.type == NUMERIC_t)
	{
		if (vp->t.memo)
			hash = (long) rational_toDouble(vp->r.r);
		else
			hash = (long) vp->n.d;
	}
	else
	{
		_clip_trap_str(mp, __file__, __LINE__, "call by name not from CHARACTER_t or NUMERIC_t value");
		return _clip_call_errblock(mp, 1);
	}
	if ((ret = _clip_func_hash(mp, hash, argc, 0, 0)))
		return ret;
	--fp->sp;
	_clip_destroy(mp, fp->sp - 1);
	fp->sp[-1] = fp->sp[0];

	return 0;
}

static void
init_machine(ClipMachine * mp)
{
	/* thread-depend code */
	cur_mp = mp;
}

CLIP_DLLEXPORT int
_clip_proc(struct ClipMachine *mp, ClipFunction f, int argc, int rest, ClipVarFrame * uplocals)
{
	int r;
	ClipFrame *inMacro = mp->inMacro;

	mp->inMacro = 0;
	r = do_call(mp, f, argc, 1, rest, uplocals);
	mp->inMacro = inMacro;
	return r;
}

CLIP_DLLEXPORT int
_clip_register(ClipMachine * mp, ClipFunction func, const char *name)
{
	return _clip_register_hash(mp, func, _clip_hashstr(name));
}

CLIP_DLLEXPORT int
_clip_register_hash(ClipMachine * mp, ClipFunction func, long hash)
{
	NEWVAR(FuncEntry, fp);
	fp->f = func;
	fp->hash = hash;
#if 0
	if (HashTable_insert(mp->functions, fp, fp->hash))
		return 1;
	else
	{
		free(fp);
		return 0;
	}
#else
	HashTable_store(mp->functions, fp, fp->hash);
	return 1;
#endif
}

int
_clip_register_block(ClipMachine * mp, ClipBlock block, long hash)
{
	NEWVAR(FuncEntry, fp);
	fp->f = 0;
	fp->block = block;
	fp->hash = hash;
	/*HashTable_remove(mp->functions, hash); */
	/*HashTable_insert(mp->functions, fp, fp->hash); */
	HashTable_store(mp->functions, fp, fp->hash);
	/*if (HashTable_insert(mp->functions, fp, fp->hash)) */
	/*if (HashTable_store(mp->functions, fp, fp->hash))
	   return 1;
	   else
	   {
	   free(fp);
	   return 0;
	   }
	 */
	return 1;
}

int
_clip_get_function(ClipMachine * mp, long hash, ClipFunction ** fpp, ClipBlock ** bpp)
{
	ClipFunction *fp;
	FuncEntry *vp;

	vp = (FuncEntry *) HashTable_fetch(mp->functions, hash);
	if (!vp)
	{
		fp = init_struct->_clip_builtin(hash);
		if (fp)
		{
			*fpp = fp;
			*bpp = 0;
			return 1;
		}
		return 0;
	}

	if (vp->f)
	{
		*fpp = vp->f;
		*bpp = 0;
	}
	else
	{
		*fpp = 0;
		*bpp = &vp->block;
	}

	return 1;
}

CLIP_DLLEXPORT int
_clip_call_errblock(ClipMachine * mp, int ret)
{
	if (!ret)
		return 0;

	if (!mp->trapVar)
		_clip_generr(mp, ret);

	if (ret < 0)
		return ret;

	if (!_clip_errorblock(mp, mp->trapVar, ret))
		ret = -ret;
	return ret;
}

CLIP_DLLEXPORT int
_clip_func_hash(struct ClipMachine *mp, long hash, int argc, int rest, ClipVarFrame * uplocals)
{
	ClipFunction *fp;
	ClipBlock *bp;
	int r = 0;
	ClipFrame *inMacro = mp->inMacro;

	mp->inMacro = 0;
	if (!_clip_get_function(mp, hash, &fp, &bp))
	{
		char buf[64];
		VarEntry *vep;

		vep = fetch_var(mp, hash);

		if (vep && (vep->var.t.type == ARRAY_t || vep->var.t.type == MAP_t))
		{
			/* support FoxPro syntax aVar(nElement) */
			ClipVar *vp;
			ClipVar *ap = mp->fp->sp - argc - 1;
			_clip_clone(mp, ap, &vep->var);
			vp = clip_fetch(mp, argc, 1, 0, 0, 0);
			if (vp)
				goto ret;
		}
		if (vep && (vep->var.t.type == PCODE_t || vep->var.t.type == CCODE_t))
		{
			/* if function not found - call codeblock from memvar*/
			int _argc = mp->argc;
			ClipFrame *fp = mp->fp;
			ClipVar *bp = mp->bp;

			ClipVar res;
			memset(&res, 0, sizeof(res));

			mp->argc = argc;
			mp->bp = fp->sp;
			r =_clip_eval(mp, &(vep->var), mp->argc, ARGPTR(mp, 1), &res);
			//r = _clip_eval(mp, &(vep->var), mp->argc,0, &res);
			mp->argc = _argc;
			mp->fp = fp;
			mp->bp = bp;
			argc++;
			for (; argc; --argc)
			{
				--(fp->sp);
				_clip_destroy(mp, fp->sp);
			}
			_clip_push(mp, &res);
			_clip_destroy(mp, &res);
			r = 0;
			goto ret;
		}
#if 1
		{
			/* call error with args */
			int _argc = mp->argc;
			ClipFrame *fp = mp->fp;
			ClipVar *bp = mp->bp;
			ClipVar res;
			memset(&res, 0, sizeof(res));

			mp->argc = argc;
			mp->bp = fp->sp;
			_clip_hash_name(mp, hash, buf, sizeof(buf));
			r = _clip_trap_err(mp, EG_NOFUNC, 0, 0, 0, 0, buf);
			mp->argc = _argc;
			mp->fp = fp;
			mp->bp = bp;
			if (!mp->trapVar)
				_clip_generr(mp, r);
			_clip_errorblock_res(mp, mp->trapVar, r, &res);
			argc++;
			for (; argc; --argc)
			{
				--(fp->sp);
				_clip_destroy(mp, fp->sp);
			}
			_clip_push(mp, &res);
			_clip_destroy(mp, &res);
			r = 0;
			goto ret;
		}
#else
		{
			/* old code for call error without args */
			_clip_hash_name(mp, hash, buf, sizeof(buf));
			r = _clip_trap_err(mp, EG_NOFUNC, 0, 0, 0, 0, buf);
			r = _clip_call_errblock(mp, r);
		}
#endif
	}
	if (fp)
	{
		char *oldbuf = 0, *buf = 0;

		if (log_level >= 4)
		{
			if (mp->fp->procname)
			{
				buf = calloc(1, 64);
				_clip_hash_name(mp, hash, buf, 64);
				oldbuf = (char *) mp->fp->procname;
				mp->fp->procname = buf;
			}
		}

		r = _clip_func(mp, fp, argc, rest, uplocals);

		if (log_level >= 4)
		{
			if (oldbuf)
			{
				mp->fp->procname = oldbuf;
				free(buf);
			}
		}

	}
	else
		r = _clip_code_func(mp, bp, argc, rest, uplocals);

	  ret:
	mp->inMacro = inMacro;
	return r;
}

CLIP_DLLEXPORT int
_clip_proc_hash(struct ClipMachine *mp, long hash, int argc, int rest, ClipVarFrame * uplocals)
{
	ClipFunction *fp;
	ClipBlock *bp;
	int r = 0;
	ClipFrame *inMacro = mp->inMacro;

	mp->inMacro = 0;
	if (!_clip_get_function(mp, hash, &fp, &bp))
	{
		char buf[64];

		_clip_hash_name(mp, hash, buf, sizeof(buf));
		r = _clip_trap_err(mp, EG_NOFUNC, 0, 0, 0, 0, buf);
		return _clip_call_errblock(mp, r);
	}

	if (fp)
	{
		char *oldbuf = 0, *buf = 0;

		if (log_level >= 4)
		{
			if (mp->fp->procname)
			{
				buf = calloc(1, 64);
				_clip_hash_name(mp, hash, buf, 64);
				oldbuf = (char *) mp->fp->procname;
				mp->fp->procname = buf;
			}
		}

		r = _clip_proc(mp, fp, argc, rest, uplocals);

		if (log_level >= 4)
		{
			if (oldbuf)
			{
				mp->fp->procname = oldbuf;
				free(buf);
			}

		}
	}
	else
		r = _clip_code_proc(mp, bp, argc, rest, uplocals);

	mp->inMacro = inMacro;
	return r;
}

CLIP_DLLEXPORT void
_clip_return(ClipMachine * mp)
{
	mp->bp[-mp->argc - 1] = *(--(mp->fp->sp));
}

CLIP_DLLEXPORT void
_clip_pop(ClipMachine * mp)
{
	_clip_destroy(mp, --(mp->fp->sp));
}

static void
delete_ClipVarFrame(ClipMachine * mp, ClipVarFrame * localvars)
{
	if (localvars && localvars->refcount > 0 && ((--(localvars->refcount)) < 1))
	{
		int i;

		for (i = 0; i < localvars->size; i++)
			_clip_destroy(mp, localvars->vars + i);
		free(localvars);
	}
}

CLIP_DLLEXPORT void
_clip_destroy_locals(ClipMachine * mp)
{
	if (mp->fp->localvars)
	{
		if (mp->fp->localvars->refcount == 0)
		{
			int i;

			for (i = 0; i < mp->fp->localvars->size; i++)
				_clip_destroy(mp, mp->fp->localvars->vars + i);
		}
	}
}


#if 0
static void
resume_mp(ClipMachine * mp)
{
	ClipFrame *fp = mp->fp;

	for (; fp->sp > fp->stack;)
	{
		--(fp->sp);
		_clip_destroy(mp, fp->sp);
	}
}
#endif
#define MAX_LOOP_DEEP 1024

static int
calc_loopcount(ClipMachine * mp, ClipVar * vp, ClipVarFrame * vfp, int deep)
{
	int r = 0;
	unsigned i;

	vp = _clip_vptr(vp);

	if (deep > MAX_LOOP_DEEP)
		return 0;
	deep++;

	switch (vp->t.type)
	{
	case CCODE_t:
	case PCODE_t:
		if (vp->c.uplocals == vfp)
			r = 1;
		break;
	case MAP_t:
		for (i = 0; i < vp->m.count; i++)
		{
			ClipVar *vpp = &(vp->m.items[i].v);

			r += calc_loopcount(mp, vpp, vfp, deep);
		}
		break;
	case ARRAY_t:
		for (i = 0; i < vp->a.count; i++)
		{
			ClipVar *vpp = vp->a.items + i;

			r += calc_loopcount(mp, vpp, vfp, deep);
		}
		break;
	default:
		break;
	}
	return r;
}


CLIP_DLLEXPORT ClipVar *
_clip_add_tempvar(ClipMachine *mp, ClipVar *vp)
{
	ClipFrame *fp;
	ClipVarVect *vvp;
	ClipVar *np;

	if (!vp)
		return 0;

	fp = mp->fp;
	vvp = fp->tempvars;
	if (!vvp)
	{
		vvp = NEW(ClipVarVect);
		fp->tempvars = vvp;
	}

	vvp->items = realloc (vvp->items, (vvp->count+1)*sizeof(ClipVar));
	np = vvp->items + vvp->count;
	vvp->count++;

	_clip_clone(mp, np, vp);

	return np;
}

CLIP_DLLEXPORT void
_clip_resume(ClipMachine * mp, int nlocals, int nreflocals)
{
	int i;
	ClipFrame *fp = mp->fp;
	ClipVarFrame *vfp = fp->localvars;

	for (; fp->sp > fp->stack;)
	{
		--(fp->sp);
		_clip_destroy(mp, fp->sp);
	}

/*      resume_mp(mp); */

	if (vfp)
	{
		for (i = 0; i < nlocals && i < vfp->size; ++i)
			_clip_destroy(mp, vfp->vars + i);

		for (; i < nlocals + nreflocals && i < vfp->size; ++i)
		{
			ClipVar *vp = vfp->vars + i;

/*#if 1
			if ((vp->t.flags & F_MPTR) && calc_loopcount(mp, vp, vfp, 0) == vfp->refcount - 1)
			{
					_clip_destroy(mp, vp);
			}
#endif
*/
#if 1
			if ((vp->t.flags & F_MPTR) && vp->p.vp->t.count == 1
				&& (calc_loopcount(mp, vp, vfp, 0) == vfp->refcount - 1))
				_clip_destroy(mp, vp);
#else
			if ((vp->t.flags & F_MPTR) && (vp->t.type == MAP_t || vp->t.type == ARRAY_t))
			{
				if (vp->p.vp->t.count > 1)
					vp->p.vp->t.count--;
				else
					_clip_destroy(mp, vp);
			}
#endif
		}

		delete_ClipVarFrame(mp, vfp);
	}

	if (fp->privates)
	{
		long *p = fp->privates;
		int num = *p;

		++p;
		remove_private_vect(mp, num, p);
		free(fp->privates);
	}

	if (fp->tempvars)
	{
		ClipVarVect *vvp = fp->tempvars;

		for(i = 0; i < vvp->count; i++)
			_clip_destroy(mp, vvp->items+i);

		free(vvp->items);
		free(vvp);
		fp->tempvars = 0;
	}

	mp->fp = mp->fp->up;
}

CLIP_DLLEXPORT void
_clip_vresume(ClipMachine * mp, int num, ClipVar * vect)
{
	/*resume_mp(mp); */
	for (; num; --num, ++vect)
		_clip_destroy(mp, vect);
}

CLIP_DLLEXPORT void
_clip_delete(ClipMachine * mp, ClipVar * vp)
{
	if (!vp)
		return;
	_clip_destroy(mp, vp);
	free(vp);
}

CLIP_DLLEXPORT void
_clip_destroy(ClipMachine * mp, ClipVar * vp)
{
	if (!vp)
		return;

	switch (vp->t.flags)
	{
	case F_MPTR:
	case F_MREF:
		if ((--(vp->p.vp->t.count)) <= 0)
		{
			_clip_destroy(mp, vp->p.vp);
			free(vp->p.vp);
		}
		CLEAR_CLIPVAR(vp);
		return;
	case F_MSTAT:
		return;
	case F_NONE:
		break;
	}

	switch (vp->t.type)
	{
	case CHARACTER_t:
		free(vp->s.str.buf);
		break;
	case NUMERIC_t:
		if (vp->t.memo)
		{
			rational_destroy(vp->r.r);
		}
		break;
	case ARRAY_t:
		{
			int i;

			for (i = vp->a.count - 1; i >= 0; --i)
				_clip_destroy(mp, vp->a.items + i);
			free(vp->a.items);
		}
		break;
	case MAP_t:
		{
			int i;

#if 1
			int ind;

			/* try run `destroy` member */
			if (search_map(vp->m.items, vp->m.count, HASH_destroy, &ind))
			{
				ClipVar *ep;
				int type;

				ep = &(vp->m.items[ind].v);
				type = _clip_type(ep);

				if (type == PCODE_t || type == CCODE_t)
				{
					ClipVar vpp;

					memset(&vpp, 0, sizeof(vpp));
					vpp.t = vp->t;
					vpp.t.flags = F_MPTR;
					vpp.p.vp = vp;
					vp->t.count++;
					_clip_eval(mp, _clip_vptr(ep), 1, &vpp, 0);
				}
			}
#endif

			for (i = vp->m.count - 1; i >= 0; --i)
				_clip_destroy(mp, &(vp->m.items[i].v));
			free(vp->m.items);
		}
		break;
	case CCODE_t:

		delete_ClipVarFrame(mp, vp->c.uplocals);
		break;

	case PCODE_t:

		delete_ClipVarFrame(mp, vp->c.uplocals);
		destroy_Block(mp, vp->c.u.block);
		free(vp->c.u.block);

		break;
	default:
		break;
	}
	CLEAR_CLIPVAR(vp);
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_destroy(ClipMachine * mp, ClipVar * vp)
{
	/*if (vp->t.flags != F_MREF)*/
		_clip_destroy(mp, vp);
	return vp;
}

int
type_weight(int type)
{
	int r = 0;

	switch (type)
	{
	case ARRAY_t:
		r = 0;
		break;
	case CCODE_t:
	case PCODE_t:
		r = 1;
		break;
	case CHARACTER_t:
		r = 2;
		break;
	case LOGICAL_t:
		r = 3;
		break;
	case DATE_t:
		r = 4;
		break;
	case DATETIME_t:
		r = 5;
	case NUMERIC_t:
		r = 6;
		break;
	case UNDEF_t:
	default:
		r = 7;
		break;
	}
	return r;
}

CLIP_DLLEXPORT int
_clip_cmp(ClipMachine * mp, ClipVar * lval, ClipVar * rval, int *res, int bytype)
{
	ClipVar *Rval = rval, *Lval = lval;
	int ltype, rtype;
	int l = 0;

	Rval = _clip_vptr(Rval);
	Lval = _clip_vptr(Lval);

	ltype = _clip_type(Lval), rtype = _clip_type(Rval);

	if (bytype && ltype != rtype)
	{
		int wl, wr;

		wl = type_weight(ltype);
		wr = type_weight(rtype);
		if (wl < wr)
			l = -1;
		else if (wl > wr)
			l = 1;
		else
			l = 0;
	}
	else if (ltype == CHARACTER_t && rtype == CHARACTER_t)
	{
		int rrlen = Rval->s.str.len;
		int rlen = rrlen;
		int lllen = Lval->s.str.len;
		int llen = lllen;
		char *s;

		for (s = Rval->s.str.buf + rlen - 1; rlen; --rlen, --s)
			if (*s != ' ' /* && *s != '\n'*/)
				break;

		if (!(mp->flags & EXACT_FLAG))
		{		/* SET EXACT OFF - empty rstr, len must match for equal */
			if (rrlen == 0)
				l = 0;
#if 1
			else if (lllen == 0 && rrlen > 0)
				l = -1;
			else	/* if ( rrlen > lllen ) */
			{
				if (lllen > rrlen)
					lllen = rrlen;
				l = _clip_strnncmp(Lval->s.str.buf, Rval->s.str.buf, lllen, rrlen);
			}
#else
			else if (llen < rrlen)
			{
				l = _clip_strncmp(Lval->s.str.buf, Rval->s.str.buf, llen);
				if (!l)
					l = -1;
			}
			else
				l = _clip_strncmp(Lval->s.str.buf, Rval->s.str.buf, rrlen);
#endif
		}
		else
			/* SET EXACT ON - all but trailing spaces */
		{
			if (llen == 0 && rlen > 0 /* llen > rlen */ )
				l = -1;
			else
			{
				for (s = Rval->s.str.buf + rlen - 1; rlen && *s == ' '; --rlen, --s)
					;
				for (s = Lval->s.str.buf + llen - 1; llen && *s == ' '; --llen, --s)
					;
				if (rlen == 0 && llen > 0)
					l = 1;
				else if (rlen > 0 && llen == 0)
					l = -1;
				else
					l = _clip_strnncmp(Lval->s.str.buf, Rval->s.str.buf, llen, rlen);
			}
		}
	}
	else if (ltype == DATE_t && rtype == DATE_t)
	{
		l = (Lval->d.julian < Rval->d.julian ? -1 : (Lval->d.julian > Rval->d.julian ? 1 : 0));
	}
	else if (ltype == DATETIME_t && rtype == DATETIME_t)
	{
		l = (Lval->dt.julian < Rval->dt.julian ? -1 : (Lval->dt.julian > Rval->dt.julian ? 1 : 0));
		if (l == 0)
			l = (Lval->dt.time < Rval->dt.time ? -1 : (Lval->dt.time > Rval->dt.time ? 1 : 0));
	}
	else if (ltype == NUMERIC_t && rtype == NUMERIC_t)
	{
		if (!Lval->t.memo && !Rval->t.memo)
		{
			double diff;

			diff = fabs(Lval->n.d - Rval->n.d);
			if (diff < (Lval->n.d * MIN_NUMERIC_DIFF))
				l = 0;
			else if (Lval->n.d < Rval->n.d)
				l = -1;
			else if (Lval->n.d > Rval->n.d)
				l = 1;
			else
				l = 0;
			/*
			   l = (Lval->n.d < Rval->n.d ? -1 : (Lval->n.d > Rval->n.d ? 1 : 0));
			 */
		}
		else
		{
			if (Lval->t.memo && Rval->t.memo)
				l = rational_cmp(Lval->r.r, Rval->r.r);
			else
			{
				rational *tmp;

				if (Lval->t.memo)
				{
					tmp = rational_double_init(Rval->n.d);
					l = rational_cmp(Lval->r.r, tmp);
				}
				else
				{
					tmp = rational_double_init(Lval->n.d);
					l = rational_cmp(tmp, Rval->r.r);
				}
				rational_destroy(tmp);
			}
		}
	}
	else if (ltype == LOGICAL_t && rtype == LOGICAL_t)
	{
		if (Lval->l.val && !Rval->l.val)
			l = 1;
		else if (!Lval->l.val && Rval->l.val)
			l = -1;
		else
			l = 0;
	}
	else if (ltype == ARRAY_t && rtype == ARRAY_t)
	{
		l = (Lval != Rval);
	}
	else if (ltype == MAP_t && rtype == MAP_t)
	{
		l = (Lval != Rval);
	}
	else if (ltype == OBJECT_t && rtype == OBJECT_t)
	{
		l = (Lval->o.obj != Rval->o.obj);
	}
	else if (ltype == CCODE_t && rtype == CCODE_t)
	{
		l = (Lval->c.u.func != Rval->c.u.func);
	}
	else if (ltype == PCODE_t && rtype == PCODE_t)
	{
		l = (Lval->c.u.block != Rval->c.u.block);
	}
	else if ((ltype == PCODE_t && rtype == CCODE_t) || (ltype == CCODE_t && rtype == PCODE_t))
	{
		l = 0;
	}
	else if (ltype == UNDEF_t)
	{
		l = (rtype == UNDEF_t ? 0 : -1);
	}
	else if (rtype == UNDEF_t)
	{
		l = (ltype == UNDEF_t ? 0 : 1);
	}
	else
	{
		return _clip_trap_operation(mp, __file__, __LINE__, "==", Lval);
		/*return _clip_call_errblock(mp, 1);*/
	}

	*res = l;
	return 0;
}

const char *
_clip_strstr(const char *src, int slen, const char *dst, int dlen)
{
	const char *end, *de, *s = src, *d = dst;

	/*
	if(!dlen)
		return src;
	*/
	for (end = src + slen, de = dst + dlen - 1; src < end; src++)
		if (*src == *d)
		{
			s = src;
			for (; src < end && d < de && *(++src) == *(++d);)
				;
			if (d == de && *src == *d)
				return s;
			src = s;
			d = dst;
		}
	return 0;
}

static int
do_assign(ClipMachine * mp, ClipVar * lval, ClipVar * rval, int op)
{
	int ret = 0;
	int l;
	ClipVar *Rval = rval, *Lval = lval;
	int ltype, rtype;

	Rval = _clip_vptr(Rval);
	Lval = _clip_vptr(Lval);

	ltype = _clip_type(Lval);
	rtype = _clip_type(Rval);

	if (ltype == MAP_t)
	{
		int ind;
		long hash = 0;

		switch (op)
		{
		case '+':
			hash = HASH_operator_add;
			break;
		case '-':
			hash = HASH_operator_sub;
			break;
		case '*':
			hash = HASH_operator_mul;
			break;
		case '/':
			hash = HASH_operator_div;
			break;
		case '%':
			hash = HASH_operator_mod;
			break;
		case '^':
			hash = HASH_operator_pow;
			break;
		case '|':
			hash = HASH_operator_or;
			break;
		case '&':
			hash = HASH_operator_and;
			break;
		case '$':
			hash = HASH_operator_in;
			break;
		case 'e':
			hash = HASH_operator_eq;
			break;
		case 'E':
			hash = HASH_operator_eeq;
			break;
		case 'N':
			hash = HASH_operator_neq;
			break;
		case '<':
			hash = HASH_operator_lt;
			break;
		case '>':
			hash = HASH_operator_gt;
			break;
		case 'L':
			hash = HASH_operator_le;
			break;
		case 'G':
			hash = HASH_operator_ge;
			break;
		}

		if (hash)
		{
			ClipVar *vp = Lval;

			/* try run `destroy` member */
			if (search_map(vp->m.items, vp->m.count, hash, &ind))
			{
				ClipVar *ep;
				int type;

				ep = &(vp->m.items[ind].v);
				type = _clip_type(ep);

				if (type == PCODE_t || type == CCODE_t)
				{
					int i;
					ClipVar stack[2], res;

					memset(stack, 0, sizeof(stack));
					memset(&res, 0, sizeof(res));

					stack[0].t = vp->t;
					stack[0].t.flags = F_MPTR;
					stack[0].p.vp = vp;
					vp->t.count++;

					_clip_clone(mp, stack + 1, rval);

					ret = _clip_eval(mp, _clip_vptr(ep), 2, stack, &res);

					_clip_clone(mp, lval, &res);

					for (i = 0; i < 2; i++)
						_clip_destroy(mp, stack + i);

					_clip_destroy(mp, &res);
				}
				return ret;
			}
		}
	}

	switch (op)
	{
	case '+':
		if (ltype == CHARACTER_t && rtype == CHARACTER_t)
		{
			int len = Lval->s.str.len + Rval->s.str.len;

			if (Lval->t.flags == F_MSTAT)
			{
				char *s = malloc(len + 1);

				Lval->t.flags = F_NONE;
				memcpy(s, Lval->s.str.buf, Lval->s.str.len);
				Lval->s.str.buf = s;
			}
			else
				Lval->s.str.buf = realloc(Lval->s.str.buf, len + 1);
			memcpy(Lval->s.str.buf + Lval->s.str.len, Rval->s.str.buf, Rval->s.str.len + 1);
			Lval->s.str.len = len;
			Lval->t.MACRO = Rval->t.MACRO;
		}
		else if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			_clip_math_operation(mp, op, Lval, Rval);
		}
		else if (ltype == DATE_t && rtype == NUMERIC_t)
		{
			if (Rval->t.memo)
				Lval->d.julian += rational_toDouble(Rval->r.r);
			else
				Lval->d.julian += Rval->n.d;
		}
		else if (ltype == NUMERIC_t && rtype == DATE_t)
		{
			double tmp;

			if (Lval->t.memo)
				tmp = rational_toDouble(Lval->r.r);
			else
				tmp = Lval->n.d;
			Lval->t.type = DATE_t;
			Lval->d.julian = Rval->d.julian;
			Lval->d.julian += tmp;
		}
		else if (ltype == DATETIME_t && rtype == NUMERIC_t)
		{
			long tmp;

			if (Rval->t.memo)
				tmp = (long) rational_toDouble(Rval->r.r);
			else
				tmp = (long) Rval->n.d;
			Lval->dt.julian += tmp / (24 * 60 * 60);
			Lval->dt.time += (tmp % (24 * 60 * 60)) * 1000;
		}
		else if (ltype == NUMERIC_t && rtype == DATETIME_t)
		{
			long tmp;

			if (Lval->t.memo)
				tmp = (long) rational_toDouble(Lval->r.r);
			else
				tmp = (long) Lval->n.d;
			Lval->t.type = DATETIME_t;
			Lval->dt.julian = Rval->dt.julian + tmp / (24 * 60 * 60);
			Lval->dt.time = Rval->dt.time + (tmp % (24 * 60 * 60)) * 1000;
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "+", Lval);
			/*return 1;*/
		}
		break;
	case '-':
		if (ltype == CHARACTER_t && rtype == CHARACTER_t)
		{
			int llen = Lval->s.str.len;
			int len,slen = 0;
			char *s;

			for (s = Lval->s.str.buf + llen - 1; llen; --llen, --s, ++slen)
				if (*s != ' ' && *s != '\t')
					break;
			len = llen + Rval->s.str.len;

			if (Lval->t.flags == F_MSTAT)
			{
				char *s = malloc(len + 1);

				Lval->t.flags = F_NONE;
				memcpy(s, Lval->s.str.buf, Lval->s.str.len);
				Lval->s.str.buf = s;
			}
			else
				Lval->s.str.buf = realloc(Lval->s.str.buf, len + slen + 1);
			memcpy(Lval->s.str.buf + llen, Rval->s.str.buf, Rval->s.str.len + 1);
			memset(Lval->s.str.buf + len,' ',slen);
			Lval->s.str.buf[len+slen] = 0;
			Lval->s.str.len = len+slen;
			Lval->t.MACRO = Rval->t.MACRO;
		}
		else if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			_clip_math_operation(mp, op, Lval, Rval);
		}
		else if (ltype == DATE_t && rtype == NUMERIC_t)
		{
			if (Rval->t.memo)
				Lval->d.julian -= rational_toDouble(Rval->r.r);
			else
				Lval->d.julian -= Rval->n.d;
		}
		else if (ltype == DATETIME_t && rtype == DATETIME_t)
		{
			long tmp = (Lval->dt.julian - Rval->dt.julian) * 24 * 60 * 60;

			tmp += (Lval->dt.time - Rval->dt.time) / 1000;

			Lval->t.type = NUMERIC_t;
			Lval->n.d = tmp;
		}
		else if (ltype == DATETIME_t && rtype == NUMERIC_t)
		{
			long tmp;

			if (Rval->t.memo)
				tmp = (long) rational_toDouble(Rval->r.r);
			else
				tmp = (long) Rval->n.d;
			Lval->dt.julian -= tmp / (24 * 60 * 60);
			Lval->dt.time -= (tmp % (24 * 60 * 60)) * 1000;
			if (Lval->dt.time < 0)
			{
				Lval->dt.julian--;
				Lval->dt.time = (24 * 60 * 60 * 1000) + Lval->dt.time;
			}
		}
		else if (ltype == NUMERIC_t && rtype == DATE_t)
		{
			double tmp;

			if (Lval->t.memo)
				tmp = rational_toDouble(Lval->r.r);
			else
				tmp = Lval->n.d;
			Lval->t.type = DATE_t;
			Lval->d.julian = Rval->d.julian;
			Lval->d.julian -= tmp;
		}
		else if (ltype == DATE_t && rtype == DATE_t)
		{
			Lval->n.d = Lval->d.julian - Rval->d.julian;
			Lval->t.type = NUMERIC_t;
			Lval->t.len = 0;
			Lval->t.dec = 0;
			Lval->t.memo = 0;
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "-", Lval);
			/*return 1;*/
		}
		break;
	case '*':
		if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			_clip_math_operation(mp, op, Lval, Rval);
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "*", Lval);
			/*return 1;*/
		}
		break;
	case '/':
		if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			_clip_math_operation(mp, op, Lval, Rval);
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "/", Lval);
			/*return 1;*/
		}
		break;
	case '%':
		if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			if (!Lval->t.memo && !Rval->t.memo)
			{
				Lval->n.d = fmod(Lval->n.d, Rval->n.d);
				Lval->t.len = 0;
				Lval->t.dec = 0;
				Lval->t.memo = 0;
			}
			else
			{
				rational *tmp = 0;

				if (Lval->t.memo && Rval->t.memo)
				{
					tmp = rational_fmod(Lval->r.r, Rval->r.r);
				}
				else
				{
					if (Lval->t.memo)
					{
						rational *tmp1;

						tmp1 = rational_double_init(Rval->n.d);
						tmp = rational_fmod(Lval->r.r, tmp1);
						rational_destroy(tmp1);
					}
					else
					{
						rational *tmp1;

						tmp1 = rational_double_init(Lval->n.d);
						tmp = rational_fmod(tmp1, Lval->r.r);
						rational_destroy(tmp1);
					}
				}
				rational_destroy(Lval->r.r);
				Lval->r.r = tmp;
			}

		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "%", Lval);
			/*return 1;*/
		}
		break;
	case '^':
		if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			if (Lval->t.memo)
			{
				long tmp;

				if (Rval->t.memo)
					tmp = (long) rational_toDouble(Rval->r.r);
				else
					tmp = (long) Rval->n.d;
				rational_powa(Lval->r.r, tmp);
			}
			else
			{
				Lval->n.d = pow(Lval->n.d, Rval->n.d);
				if (!finite(Lval->n.d))
					Lval->n.d = 0;
				Lval->t.len = 0;
				Lval->t.dec = 0;
			}
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "**", Lval);
			/*return 1;*/
		}
		break;
	case '$':
		if (ltype == CHARACTER_t && rtype == CHARACTER_t)
		{
			const char *s = _clip_strstr(Rval->s.str.buf, Rval->s.str.len,
						Lval->s.str.buf, Lval->s.str.len);

			_clip_destroy(mp, lval);
			lval->l.val = (s ? 1 : 0);
			lval->t.type = LOGICAL_t;
		}
		else if (rtype == MAP_t)
		{
			int ind, r;
			long no;

			switch (ltype)
			{
			case NUMERIC_t:
				if (Lval->t.memo)
					no = rational_toDouble(Lval->r.r);
				else
					no = Lval->n.d;
				break;
			case CHARACTER_t:
				/*no = _clip_casehashbytes(0, Lval->s.str.buf, Lval->s.str.len); */
				no = _clip_hashbytes(0, Lval->s.str.buf, Lval->s.str.len);
				break;
			default:
				goto inv_in_arg;
			}

			r = search_map(Rval->m.items, Rval->m.count, no, &ind);
			_clip_destroy(mp, lval);
			lval->l.val = (r ? 1 : 0);
			lval->t.type = LOGICAL_t;
		}
		else if (rtype == ARRAY_t)
		{
			int i, c, r, res = 0;

			c = Rval->a.count;

			for (i = 0; i < c; ++i)
			{
				ClipVar *vp = Rval->a.items + i;

				r = _clip_cmp(mp, Lval, vp, &res, 1);
				if (r)
					continue;
				if (res)
					break;
			}
			_clip_destroy(mp, lval);
			lval->l.val = (!res ? 1 : 0);
			lval->t.type = LOGICAL_t;
		}
		else
		{
			  inv_in_arg:
			return _clip_trap_operation(mp, __file__, __LINE__, "$", Lval);
			/*return 1;*/
		}
		break;
	case 'E':
	case 'e':
	case 'N':
#if 1				/* uri added : oper != work as =, not == */
		if ((op == 'E') && (ltype == CHARACTER_t && rtype == CHARACTER_t))
#else
		if ((op == 'E' || op == 'N') && (ltype == CHARACTER_t && rtype == CHARACTER_t))
#endif
#if 1
			/*l = !strcmp(Lval->s.str.buf, Rval->s.str.buf); */
		{
			int ll, lr;

			ll = Lval->s.str.len;
			lr = Rval->s.str.len;

			if (ll != lr)
				l = 0;
			else
				l = !memcmp(Lval->s.str.buf, Rval->s.str.buf, ll);
		}
#else
/*Rust begin */
		{
			int len = Lval->s.str.len < Rval->s.str.len ? Lval->s.str.len : Rval->s.str.len;

			l = !_clip_strncmp(Lval->s.str.buf, Rval->s.str.buf, len);
		}
/*Rust end */
#endif
		else
		{
			if ((ret = _clip_cmp(mp, lval, rval, &l, 0)))
				return ret;
			l = !l;
		}

		_clip_destroy(mp, lval);
		switch (op)
		{
		case 'E':
		case 'e':
			lval->l.val = l;
			break;
		default:
			lval->l.val = !l;
			break;
		}
		lval->t.type = LOGICAL_t;
		break;
	case '|':
		if (ltype == LOGICAL_t && rtype == LOGICAL_t)
		{
			l = (Lval->l.val || Rval->l.val);
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "OR", lval);
			/*return 1;*/
		}
		lval->l.val = l;
		lval->t.type = LOGICAL_t;
		break;
	case '&':
		if (ltype == LOGICAL_t && rtype == LOGICAL_t)
		{
			l = (Lval->l.val && Rval->l.val);
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "AND", lval);
			/*return 1;*/
		}
		lval->l.val = l;
		lval->t.type = LOGICAL_t;
		break;
	case '<':
	case '>':
	case 'L':
	case 'G':
		if (ltype == CHARACTER_t && rtype == CHARACTER_t)
		{
			l = _clip_strnncmp(Lval->s.str.buf, Rval->s.str.buf, Lval->s.str.len, Rval->s.str.len);
#if 1   /* added 20030118 for test '? "" <="0"' */
			ret = _clip_cmp(mp, lval, rval, &l, 0);
#endif
			//printf("\nl=%d,ret=%d\n",l,ret);
			_clip_destroy(mp, lval);
		}
		else if (ltype == DATE_t && rtype == DATE_t)
		{
			if (Lval->d.julian < Rval->d.julian)
				l = -1;
			else if (Lval->d.julian > Rval->d.julian)
				l = 1;
			else
				l = 0;
		}
		else if (ltype == DATETIME_t && rtype == DATETIME_t)
		{
			if (Lval->dt.julian < Rval->dt.julian)
				l = -1;
			else if (Lval->dt.julian > Rval->dt.julian)
				l = 1;
			else
				l = 0;
			if (l == 0)
			{
				if (Lval->dt.time < Rval->dt.time)
					l = -1;
				else if (Lval->dt.time > Rval->dt.time)
					l = 1;
				else
					l = 0;
			}
		}
		else if (ltype == NUMERIC_t && rtype == NUMERIC_t)
		{
			if (!Lval->t.memo && !Rval->t.memo)
			{
				double diff;

				diff = fabs(Lval->n.d - Rval->n.d);
				if (diff < (Lval->n.d * MIN_NUMERIC_DIFF))
					l = 0;
				else if (Lval->n.d < Rval->n.d)
					l = -1;
				else if (Lval->n.d > Rval->n.d)
					l = 1;
				else
					l = 0;
			}
			else
			{
				if (Lval->t.memo && Rval->t.memo)
					l = rational_cmp(Lval->r.r, Rval->r.r);
				else
				{
					rational *tmp = NULL;

					if (Lval->t.memo)
					{
						tmp = rational_double_init(Rval->n.d);
						l = rational_cmp(Lval->r.r, tmp);
					}
					else
					{
						tmp = rational_double_init(Lval->n.d);
						l = rational_cmp(tmp, Rval->r.r);
					}
					rational_destroy(tmp);
				}
			}
			_clip_destroy(mp, lval);
		}
		else if (ltype == LOGICAL_t && rtype == LOGICAL_t)
		{
			if (Lval->l.val && !Rval->l.val)
				l = -1;
			else if (!Lval->l.val && Rval->l.val)
				l = 1;
			else
				l = 0;
		}
		else if (ltype == OBJECT_t && rtype == OBJECT_t)
		{
			l = Lval->o.rtti->cmp(mp, Lval->o.obj, Lval->o.rtti, Rval);
			_clip_destroy(mp, lval);
		}
		else
		{
			return _clip_trap_operation(mp, __file__, __LINE__, "==", lval);
			/*return 1;*/
		}
		switch (op)
		{
		case 'L':
			lval->l.val = (l <= 0 ? 1 : 0);
			break;
		case 'G':
			lval->l.val = (l >= 0 ? 1 : 0);
			break;
		case '>':
			lval->l.val = (l > 0 ? 1 : 0);
			break;
		case '<':
			lval->l.val = (l < 0 ? 1 : 0);
			break;
		default:
			lval->l.val = l;
		}
		lval->t.type = LOGICAL_t;
		lval->t.flags = F_NONE;
		break;
	}

	return ret;
}

CLIP_DLLEXPORT int
_clip_op(ClipMachine * mp, int op)
{
	int ret;
	ClipVar *rval = mp->fp->sp - 1;
	ClipVar *lval = mp->fp->sp - 2;

	if ((ret = do_assign(mp, lval, rval, op)))
		return ret;
		/*return _clip_call_errblock(mp, ret);*/
	_clip_destroy(mp, rval);
	--mp->fp->sp;
	return 0;
}

CLIP_DLLEXPORT int
_clip_not(ClipMachine * mp)
{
	ClipVar *vp = _clip_vptr(mp->fp->sp - 1);

	if (vp->t.type != LOGICAL_t)
	{
		_clip_trap_str(mp, __file__, __LINE__, "non-logical type in NOT operator");
		return _clip_call_errblock(mp, 1);
	}
	vp->l.val = !vp->l.val;
	return 0;
}

CLIP_DLLEXPORT int
_clip_minus(ClipMachine * mp)
{
	ClipVar *vp = _clip_vptr(mp->fp->sp - 1);

	if (vp->t.type != NUMERIC_t)
	{
		_clip_trap_str(mp, __file__, __LINE__, "non-numeric type in MINUS operator");
		return _clip_call_errblock(mp, 1);
	}
	if (vp->t.memo)
		rational_inverse(vp->r.r);
	else
		vp->n.d = -vp->n.d;
	return 0;
}

CLIP_DLLEXPORT int
_clip_opassign(struct ClipMachine *mp, void *Lval, int op)
{
	int ret;
	ClipVar *rval = mp->fp->sp - 1;

	if ((ret = do_assign(mp, (ClipVar *) Lval, rval, op)))
		return ret;
	_clip_destroy(mp, rval);
	--mp->fp->sp;
	return 0;
}

CLIP_DLLEXPORT int
_clip_opiassign(struct ClipMachine *mp, void *Lval, int op)
{
	int ret;
	ClipVar *rval = mp->fp->sp - 1;
	ClipVar *lval = (ClipVar *) Lval;

	if ((ret = do_assign(mp, lval, mp->fp->sp - 1, op)))
		return ret;
	_clip_destroy(mp, rval);
	if ((ret = _clip_mclone(mp, rval, lval)))
		return ret;
	return 0;
}

CLIP_DLLEXPORT int
_clip_assign(struct ClipMachine *mp, void *Lval)
{
	ClipVar *rval = mp->fp->sp - 1;
	ClipVar *lval = (ClipVar *) Lval;
	int ret;

	if (lval)
	{
		if ((lval->t.flags & F_MPTR) && lval->t.field && lval->p.fp)
		{
			return _clip_assign_field(mp, lval->p.fp->fieldhash, lval->p.fp->areahash);
		}
		else
		{
			if (!(lval->t.flags & F_MPTR) && (rval->t.flags == F_MREF) && lval->t.type == UNDEF_t
				 && !(rval->t.type == ARRAY_t || rval->t.type == MAP_t)
				)
			{
					_clip_dup(mp, lval, _clip_vptr(rval));
			}
#if 0
			else if (lval->t.type == ARRAY_t || lval->t.type == MAP_t)
			{
				_clip_destroy(mp, lval);
				ret = _clip_mclone(mp, lval, rval);
			}
#endif
			else if ((ret = _clip_mclone(mp, lval, rval)))
				return ret;
		}
		lval->t.field = 0;
		if (lval->t.type == CHARACTER_t)
			lval->t.memo = 0;
	}

	_clip_destroy(mp, rval);
	mp->fp->sp--;
	return 0;
}

CLIP_DLLEXPORT int
_clip_refassign(ClipMachine * mp, void *Lval, int isTop)
{
	ClipVar *rval = mp->fp->sp - 1;
	ClipVar *lval = (ClipVar *) Lval;

	_clip_destroy(mp, lval);

	if (!(rval->t.flags & F_MPTR))
		return 1;
	*lval = *rval;
	if (isTop)
		mp->fp->sp--;
	else
	{
		rval->p.vp->t.count++;
	}

	return 0;
}

CLIP_DLLEXPORT int
_clip_iassign(struct ClipMachine *mp, void *Lval)
{
	int ret;
	ClipVar *rval = mp->fp->sp - 1;
	ClipVar *lval = (ClipVar *) Lval;

	if ((lval->t.flags & F_MPTR) && lval->t.field && lval->p.fp)
	{
		ret = _clip_iassign_field(mp, lval->p.fp->fieldhash, lval->p.fp->areahash);
	}
	else
	{
		if (!(lval->t.flags & F_MPTR) && (rval->t.flags == F_MREF) && lval->t.type == UNDEF_t
				 && !(rval->t.type == ARRAY_t || rval->t.type == MAP_t)
		)
		{
				ret = _clip_dup(mp, lval, _clip_vptr(rval));
		}
		else
			ret = _clip_mclone(mp, lval, rval);
		lval->t.field = 0;
		if (lval->t.type == CHARACTER_t)
			lval->t.memo = 0;
	}

	return ret;
}

static ClipVar *
find_lval(ClipMachine * mp, char *name, int namelen)
{
	VarEntry *vep;
	ClipVar *lval = 0;
	long hash;
	char *fname = 0;
	int fl = 0, r, ndim = 0;
	long *dim = 0;

	r = _clip_parse_name(mp,name, namelen, 0, 0, &fname, &fl, &dim, &ndim);
	if (r == 2)		/* field */
		return 0;

	hash = _clip_casehashbytes(0, fname, fl);
	vep = fetch_var(mp, hash);

	if (vep)
		lval = &vep->var;

	if (lval && r == 3)
	{
		lval = _clip_aref(mp, lval, ndim, dim);
		free(dim);
	}

	return lval;
}

CLIP_DLLEXPORT int
_clip_macroassign(ClipMachine * mp, int op, int isTop, int fieldprec)
{
	int r;
	ClipVar *rval = mp->fp->sp - 2;
	ClipVar *vp = _clip_vptr(mp->fp->sp - 1);
	ClipVar *lval = 0;

	if (vp->t.type != CHARACTER_t)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "macro assign with non-character argument (%s)", _clip_typename(vp));
		return _clip_call_errblock(mp, 1);
	}

	if (op == '=')
		r = _clip_nameassign(mp, vp->s.str.buf, vp->s.str.len, rval, fieldprec);
	else
	{
		lval = find_lval(mp, vp->s.str.buf, vp->s.str.len);

		if (!lval)
		{
			_clip_trap_printf(mp, __file__, __LINE__, "no variable name: '%.*s'", vp->s.str.len, vp->s.str.buf);
			return _clip_call_errblock(mp, 1);
		}

		if (op == '=')
			r = _clip_mclone(mp, lval, rval);
		else
			r = do_assign(mp, lval, rval, op);
	}

	if (r)
		return r;

	_clip_destroy(mp, --(mp->fp->sp));

	if (isTop)
		_clip_destroy(mp, --(mp->fp->sp));

	return 0;
}

CLIP_DLLEXPORT void
_clip_pop_fieldhash(ClipMachine * mp, long *area, long *field)
{
	ClipVar *vp, *vpp;

	vpp = --(mp->fp->sp);
	*area = -1;
	*field = -1;

	vp = _clip_vptr(vpp);

	switch (vp->t.type)
	{
	case CHARACTER_t:
		{
			int al = 0, fl = 0;
			char *aname = 0, *fname = 0;

			_clip_parse_name(mp,vp->s.str.buf, vp->s.str.len, &aname, &al, &fname, &fl, 0, 0);
			if (aname)
				*area = _clip_casehashbytes(0, aname, al);
			if (fname)
				*field = _clip_casehashbytes(0, fname, fl);
		}
		break;
	case NUMERIC_t:
		*field = _clip_double(vp);
		break;
	case OBJECT_t:
		*field = vp->o.rtti->hash(mp, vp->o.obj, vp->o.rtti);
		break;
	default:
		break;
	}

	_clip_destroy(mp, vpp);
}

CLIP_DLLEXPORT int
_clip_cond(ClipMachine * mp, int *ifp)
{
	int r = _clip_tcond(mp, ifp);

	if (r)
		return r;

	mp->fp->sp--;
	_clip_destroy(mp, mp->fp->sp);
	return 0;
}

CLIP_DLLEXPORT int
_clip_tcond(ClipMachine * mp, int *ifp)
{
	ClipVar *vp = _clip_vptr(mp->fp->sp - 1);

	if (vp->t.type != LOGICAL_t)
	{
		_clip_trap_str(mp, __file__, __LINE__, "non-logical var in condition");
		return _clip_call_errblock(mp, 1);
	}
	if (vp->t.flags & F_MPTR)
		*ifp = vp->p.vp->l.val;
	else
		*ifp = vp->l.val;

	return 0;
}

CLIP_DLLEXPORT void *
_clip_memdup(const void *src, int len)
{
	void *ret = malloc(len + 1);

	memcpy(ret, src, len);
	((char *) ret)[len] = 0;
	return ret;
}

CLIP_DLLEXPORT int
_clip_mclone(struct ClipMachine *mp, ClipVar * dest, ClipVar * src)
{
	if (dest->t.flags == F_MREF && dest->p.vp->t.count > 1)
	{
		ClipVar *dp, *srcp;
		int count, ret;

		dp = dest->p.vp;
		srcp = src;
		count = dp->t.count;
		ret = 0;

		switch (src->t.flags)
		{
		case F_MREF:
			srcp = src->p.vp;
			if (srcp == dp)
				break;
		case F_MPTR:
			if (src->t.field && src->p.fp)
			{
				dest->p.fp = src->p.fp;
				dest->t.field = 1;
			}
		default:
			_clip_destroy(mp, dp);
			ret = _clip_clone(mp, dp, srcp);
			dp->t.count = count;
			dest->t.type = srcp->t.type;
			dest->t.flags = F_MREF;
			dest->t.field = 0;
			break;
		}
		return ret;
	}
	else
	{
		_clip_destroy(mp, dest);
		return _clip_clone(mp, dest, src);
	}
}

CLIP_DLLEXPORT int
_clip_clone(struct ClipMachine *mp, ClipVar * dest, ClipVar * src)
{
	ClipType t;

	CLEAR_CLIPVAR(dest);
	if (!src)
		return 0;

	t = src->t;
	switch (src->t.flags)
	{
	case F_MPTR:
		dup_ref(dest, src);
		return 0;
	case F_MREF:
		switch (src->t.type)
		{
		case MAP_t:
		case ARRAY_t:
		case CCODE_t:
		case PCODE_t:
			dup_ref(dest, src);
			return 0;
		default:
			return _clip_clone(mp, dest, src->p.vp);
		}
	case F_MSTAT:
		t.flags = F_NONE;
	case F_NONE:
		switch (src->t.type)
		{
		case UNDEF_t:
			break;
		case NUMERIC_t:
			if (src->t.memo)
			{
				dest->t.type = NUMERIC_t;
				dest->t.flags = F_NONE;
				dest->t.len = src->t.len;
				dest->t.dec = src->t.dec;
				dest->t.memo = 1;
				dest->r.r = rational_copy(src->r.r);
			}
			else
				*dest = *src;
			break;
		case LOGICAL_t:
		case DATE_t:
		case DATETIME_t:
			*dest = *src;
			break;
		case CCODE_t:
		case PCODE_t:
			*dest = *src;
			if (src->c.uplocals && src->c.uplocals->refcount)
				src->c.uplocals->refcount++;
			return 0;
		case CHARACTER_t:
			{
				int len = src->s.str.len;

				dest->s.str.buf = (char *) _clip_memdup(src->s.str.buf, len);
				dest->s.str.len = len;

			}
			break;
		case OBJECT_t:
			src->o.rtti->clone(mp, dest, src);
			break;
		default:
			break;
		}
	}
	dest->t = t;
	return 0;
}

CLIP_DLLEXPORT int
_clip_dup(ClipMachine * mp, ClipVar * dest, ClipVar * src)
{
	dest->t = src->t;

	switch (src->t.flags)
	{
	case F_MPTR:
	case F_MREF:
#if 0
		{
			enum ClipVarType type = src->t.type;

			if (type == CCODE_t || type == PCODE_t)
			{
				dest->p.vp = src->p.vp;
				++(src->p.vp->t.count);
				return 0;
			}
		}
#endif
		src = src->p.vp;
		dest->t.flags = F_NONE;
		break;
	case F_MSTAT:
		/*
		   *dest = *src;
		   return 0;
		 */
	case F_NONE:
		break;
	}

	switch (src->t.type)
	{
	case NUMERIC_t:
		if (src->t.memo)
		{
			dest->t.type = NUMERIC_t;
			dest->t.flags = F_NONE;
			dest->t.len = src->t.len;
			dest->t.dec = src->t.dec;
			dest->t.memo = 1;
			dest->r.r = rational_copy(src->r.r);
		}
		else
			*dest = *src;
		break;
	case LOGICAL_t:
	case DATE_t:
	case DATETIME_t:
		*dest = *src;
		break;
	case CCODE_t:
	case PCODE_t:
		{
		/**dest = *src;*/
			CLEAR_CLIPVAR(dest);
			dest->t.flags = F_MPTR;
			dest->t.type = src->t.type;
			dest->p.vp = src;

			src->t.count++;
#if 0
			vp->t.type = src->t.type;
			vp->t.flags = F_NONE;
			vp->c = src->c;
			if (vp->t.type == PCODE_t && vp->c.u.block->file)
				vp->c.u.block->file->refCount++;
#if 0
			if (vp->t.type == PCODE_t)
			{
				ClipBlock *bp;

				bp = NEW(ClipBlock);
				*bp = *(src->c.u.block);
				vp->c.u.block = bp;
				/*bp->file->refCount++; */
			}
			else
				vp->c.u.func = src->c.u.func;
#endif
#endif
		}
#if 0
		if (src->c.uplocals && src->c.uplocals->refcount)
			src->c.uplocals->refcount++;
#endif
		break;
	case CHARACTER_t:
		{
			int len = src->s.str.len;

			dest->s.str.buf = (char *) _clip_memdup(src->s.str.buf, len);
			dest->s.str.len = len;
		}
		break;
	case OBJECT_t:
		src->o.rtti->clone(mp, dest, src);
		break;
	case ARRAY_t:
		{
			ClipVar *ap = NEW(ClipVar);
			int count = src->a.count, i;

			dest->t.type = ARRAY_t;
			dest->t.flags = F_MPTR;
			dest->p.vp = ap;
			dest->t.field = 0;
			dest->p.fp = 0;
			ap->t.type = ARRAY_t;
			ap->t.flags = F_NONE;
			ap->t.count = 1;
			ap->a.items = malloc(sizeof(ClipVar) * count);
			ap->a.count = count;
			for (i = 0; i < count; i++)
			{
				ClipVar *src_p, *tg_p;

				tg_p = ap->a.items + i;
				src_p = src->a.items + i;
				_clip_dup(mp, tg_p, src_p);
			}
		}
		break;
	case MAP_t:
		{
			ClipVar *ap = NEW(ClipVar);
			int count = src->m.count, i;

			dest->t.type = MAP_t;
			dest->t.flags = F_MPTR;
			dest->p.vp = ap;
			dest->t.field = 0;
			dest->p.fp = 0;
			ap->t.type = MAP_t;
			ap->t.flags = F_NONE;
			ap->t.count = 1;
			ap->m.items = malloc(sizeof(ClipVarEl) * count);
			ap->m.count = count;
			for (i = 0; i < count; i++)
			{
				ClipVarEl *src_p, *tg_p;

				tg_p = ap->m.items + i;
				src_p = src->m.items + i;
				_clip_dup(mp, &tg_p->v, &src_p->v);
				tg_p->no = src_p->no;
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_push(ClipMachine * mp, void *vpp)
{
	ClipVar *sp = mp->fp->sp;
	ClipVar *vp = (ClipVar *) vpp;
	int ret;

	if (vp->t.type != UNDEF_t && (vp->t.flags & F_MPTR) && vp->t.field && vp->p.fp)
	{
		ret = _clip_take_field(mp, vp->p.fp->fieldhash, vp->p.fp->areahash, vp);
		if (ret)
			return ret;
	}

	ret = _clip_clone(mp, sp, vp);

	++(mp->fp->sp);

	/*
	if (!(mp->flags1 & NOEXPAND_MACRO_FLAG))
	{
	*/
		if (!ret)
		{
			if (sp->t.type == CHARACTER_t && sp->t.MACRO)
				ret = _clip_expand_var(mp, sp);
		}
	/*}*/

	CLIP_CHECK_STACK;

	return ret;
}

CLIP_DLLEXPORT void
_clip_push_nil(ClipMachine *mp)
{
	ClipFrame *fp = mp->fp;
	fp->sp->t.type = UNDEF_t;
	fp->sp->t.flags = F_NONE;
	fp->sp++;
}

CLIP_DLLEXPORT void
_clip_push_true(ClipMachine * mp)
{
	ClipFrame *fp = mp->fp;
	fp->sp->t.type = LOGICAL_t;
	fp->sp->t.flags = F_NONE;
	fp->sp->l.val = 1;
	fp->sp++;
}

CLIP_DLLEXPORT void
_clip_push_false(ClipMachine * mp)
{
	ClipFrame *fp = mp->fp;
	fp->sp->t.type = LOGICAL_t;
	fp->sp->t.flags = F_NONE;
	fp->sp->l.val = 0;
	fp->sp++;
}

CLIP_DLLEXPORT int
_clip_push_static(ClipMachine * mp, void *vpp)
{
	int r;

	r = _clip_push(mp, vpp);
	if (!r)
	{
		ClipVar *vp;

		vp = _clip_vptr(mp->fp->sp - 1);
		if (vp->t.type == CCODE_t || vp->t.type == PCODE_t)
		{
			if (!vp->c.uplocals && mp->fp->localvars)
			{
				mp->fp->localvars->refcount++;
				vp->c.uplocals = mp->fp->localvars;
			}
		}
	}
	return r;
}

CLIP_DLLEXPORT void
_clip_push_hash(ClipMachine * mp, long hash)
{
	ClipVar *sp = mp->fp->sp;

	CLEAR_CLIPVAR(sp);
	sp->t.memo = 0;
	sp->t.type = NUMERIC_t;
	sp->t.flags = F_NONE;
	sp->t.len = 10;
	sp->t.dec = 0;
	sp->n.d = hash;

	mp->fp->sp++;
	CLIP_CHECK_STACK;
}

CLIP_DLLEXPORT int
_clip_ref(ClipMachine * mp, void *Vp, int noadd)
{
	ClipVar *nv;
	ClipVar *vp = (ClipVar *) Vp;
	ClipVar *sp = mp->fp->sp;
	CLEAR_CLIPVAR(sp);

	if (!vp)
		return EG_NOVAR;

	if (vp->t.flags & F_MPTR)
	{
		if (noadd)
			*sp = *vp;
		else
		{
			dup_ref(sp, vp);
			sp->t.flags = F_MREF;
		}

		++(mp->fp->sp);
		CLIP_CHECK_STACK;
		return 0;
	}

	nv = NEW(ClipVar);

	if (vp->t.flags & F_MSTAT)
	{
		/*vp->t.flags &= ~F_MSTAT; */
		_clip_clone(mp, nv, vp);
	}
	else
		*nv = *vp;

	nv->t.count = noadd ? 1 : 2;
	vp->t.flags = F_MREF /*mptr?F_MPTR:F_MREF */ ;
	vp->p.vp = nv;
	vp->t.field = 0;
	vp->p.fp = 0;

	sp->t.type = nv->t.type;
	sp->t.flags = F_MREF /*mptr?F_MPTR:F_MREF */ ;
	sp->p.vp = nv;
	sp->t.field = 0;
	sp->p.fp = 0;

	++(mp->fp->sp);
	CLIP_CHECK_STACK;
	return 0;
}

CLIP_DLLEXPORT long
_clip_hash(ClipMachine * mp, ClipVar * vp)
{
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case CHARACTER_t:
		return _clip_hashbytes(0, vp->s.str.buf, vp->s.str.len);
	case NUMERIC_t:
		return _clip_double(vp);
	case DATE_t:
		return vp->d.julian;
	case LOGICAL_t:
		return vp->l.val;
	case OBJECT_t:
		return vp->o.rtti->hash(mp, vp->o.obj, vp->o.rtti);
	default:
		break;
	}
	return 0;
}

static long
calc_casehash(ClipMachine * mp, ClipVar * vp, int trim)
{
	if (!vp)
		return 0;
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case CHARACTER_t:
		{
			char *s = vp->s.str.buf;
			int l = vp->s.str.len;

			if (trim)
			{
				char *e;

				for (e = s + l - 1; e >= s; e--)
				{
					switch (*e)
					{
					case ' ':
					case '\t':
					case '\r':
					case '\n':
						l--;
						continue;
					}
					break;
				}
			}
			return _clip_casehashbytes(0, s, l);
		}
	case NUMERIC_t:
		return _clip_double(vp);
	case DATE_t:
		return vp->d.julian;
	case LOGICAL_t:
		return vp->l.val;
	case OBJECT_t:
		return vp->o.rtti->hash(mp, vp->o.obj, vp->o.rtti);
	default:
		break;
	}
	return 0;
}

CLIP_DLLEXPORT long
_clip_casehash(ClipMachine * mp, ClipVar * vp)
{
	return calc_casehash(mp, vp, 0);
}

CLIP_DLLEXPORT long
_clip_pop_hash(ClipMachine * mp)
{
	ClipVar *vp = --(mp->fp->sp);
	long r;

	r = calc_casehash(mp, vp, 1);
	_clip_destroy(mp, vp);

	return r;
}

CLIP_DLLEXPORT long
_clip_pop_shash(ClipMachine * mp)
{
	ClipVar *vp = --(mp->fp->sp);
	long r;

	r = _clip_hash(mp, vp);
	_clip_destroy(mp, vp);

	return r;
}

/* ] ClipMachine */
