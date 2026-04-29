/* [ utils */
/*
 *    �����-> ������ *    ww - �������� (0 - �� 1 - �����)
 */
CLIP_DLLEXPORT void
_clip_cdate(long ju, int *dd, int *mm, int *yy, int *ww)
{
	long alf, a, b, c, d, e;

	alf = (long) ((ju - 1867216.25) / 36524.25);
	a = ju < 2299161L ? ju : ju + 1L + alf - alf / 4;
	b = a + 1524;
	c = (long) ((b - 122.1) / 365.25);
	d = (long) (365.25 * c);
	e = (long) ((b - d) / 30.6001);
	*dd = b - d - (long) (30.6001 * e);
	if (e < 14)
		*mm = e - 1;
	else
		*mm = e - 13;
	if (*mm > 2)
		*yy = c - 4716;
	else
		*yy = c - 4715;
	*ww = (ju + 1) % 7L;
	if (*yy <= 0 )
		*yy = 1;
}

/*
 *    ������-> ����� */
CLIP_DLLEXPORT long
_clip_jdate(int dd, int mm, int yy)
{
	int a, b;

	b = 0;
	if (mm <= 2)
	{
		yy--;
		mm += 12;
	}
	if ((yy * 10000.0 + mm * 100.0 + dd) >= 15821015.0)
	{
		a = yy / 100;
		b = 2 - a + a / 4;
	}
	return ((long) (365.25 * yy - (yy ? 0.0 : 0.75)) + (long) (30.6001 * (mm + 1) + dd + 1720995 + b));
}

CLIP_DLLEXPORT int
_clip_parinfo(ClipMachine * mp, int num)
{
	if (num == 0)
		return mp->argc;
	if (num < 0 || num > mp->argc)
		return 0;

	return _clip_type(mp->bp - (mp->argc - (num - 1)));
}

CLIP_DLLEXPORT int
_clip_parni(ClipMachine * mp, int num)
{
	return (int) _clip_parnd(mp, num);
}

CLIP_DLLEXPORT long
_clip_parnl(ClipMachine * mp, int num)
{
	return (long) _clip_parnd(mp, num);
}

CLIP_DLLEXPORT ClipVar *
_clip_par(ClipMachine * mp, int num)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));

		return _clip_vptr(vp);
	}
	return 0;
}

CLIP_DLLEXPORT ClipVar *
_clip_spar(ClipMachine * mp, int num)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));

		return vp;
	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_par_isref(ClipMachine * mp, int num)
{
	ClipVar *vp = _clip_spar(mp, num);

	if (!vp)
		return 0;
	if (vp->t.flags & F_MPTR)
		return 1;
	return 0;
}

CLIP_DLLEXPORT int
_clip_par_assign(ClipMachine * mp, int num, ClipVar * vp)
{
	ClipVar *lp = _clip_spar(mp, num);

	if (!lp || !vp)
		return -1;
	if (!(lp->t.flags & F_MPTR))
		return 1;
	return _clip_mclone(mp, lp, vp);
}

CLIP_DLLEXPORT int
_clip_par_assign_str(ClipMachine * mp, int num, char *str, int len)
{
	return _clip_par_assign_str_m(mp, num, _clip_memdup(str, len), len);
}

CLIP_DLLEXPORT int
_clip_par_assign_str_m(ClipMachine * mp, int num, char *str, int len)
{
	ClipVar v;
	int r;

	if (!_clip_par_isref(mp, num))
		return -1;
	CLEAR_CLIPVAR(&v);
	v.s.str.buf = str;
	v.s.str.len = len;
	v.t.type = CHARACTER_t;
	v.t.flags = F_NONE;
	v.t.memo = 0;
	r = _clip_par_assign(mp, num, &v);
	_clip_destroy(mp, &v);
	return r;
}

CLIP_DLLEXPORT double
_clip_parnd(ClipMachine * mp, int num)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = _clip_vptr((mp->bp - (mp->argc - (num - 1))));

		if (vp->t.type == NUMERIC_t && !vp->t.memo)
		{
			return vp->n.d;
		}
		if (vp->t.type == NUMERIC_t && vp->t.memo)
			return rational_toDouble(vp->r.r);
	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_parp(ClipMachine * mp, int num, int *len, int *dec)
{
	*len = *dec = 0;

	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));

		if (vp->t.flags & F_MPTR)
			vp = vp->p.vp;
		*len = vp->t.len;
		*dec = vp->t.dec;
		return 0;
	}

	return -1;
}

CLIP_DLLEXPORT int
_clip_parl(ClipMachine * mp, int num)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));
		enum ClipVarType type = _clip_type(vp);

		if (type == LOGICAL_t)
		{
			if (vp->t.flags & F_MPTR)
				vp = vp->p.vp;
			return vp->l.val;
		}
	}
	return 0;
}

CLIP_DLLEXPORT char *
_clip_parcl(ClipMachine * mp, int num, int *len)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));
		enum ClipVarType type = _clip_type(vp);

		if (type == CHARACTER_t)
		{
			if (vp->t.flags & F_MPTR)
				vp = vp->p.vp;
			if (len)
				*len = vp->s.str.len;
			return vp->s.str.buf;
		}
	}
	return 0;
}

CLIP_DLLEXPORT char *
_clip_parc(ClipMachine * mp, int num)
{
	return _clip_parcl(mp, num, 0);
}

CLIP_DLLEXPORT void
_clip_retndp(ClipMachine * mp, double n, int len, int dec)
{
	ClipVar *vp = mp->bp - mp->argc - 1;

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);

	vp->t.type = NUMERIC_t;
	vp->t.flags = F_NONE;
	vp->t.memo = 0;
	vp->t.len = len;
	vp->t.dec = dec;
	if (!finite(n))
		n = 0;
	vp->n.d = n;
}

CLIP_DLLEXPORT void
_clip_retni(ClipMachine * mp, int n)
{
	_clip_retndp(mp, n, 0, 0);
}

CLIP_DLLEXPORT void
_clip_retnl(ClipMachine * mp, long n)
{
	_clip_retndp(mp, n, 0, 0);
}

CLIP_DLLEXPORT void
_clip_retnd(ClipMachine * mp, double n)
{
	_clip_retndp(mp, n, 0, 0);
}

CLIP_DLLEXPORT void
_clip_retnr(ClipMachine * mp, rational * r, int len, int dec)
{
	ClipVar *vp = mp->bp - mp->argc - 1;

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);

	vp->t.type = NUMERIC_t;
	vp->t.flags = F_NONE;
	vp->t.len = len;
	vp->t.dec = dec;
	vp->t.memo = 1;
	vp->r.r = r;
}

CLIP_DLLEXPORT void
_clip_retl(ClipMachine * mp, int n)
{
	ClipVar *vp = mp->bp - mp->argc - 1;

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);

	vp->t.type = LOGICAL_t;
	vp->t.flags = F_NONE;
	vp->l.val = n;
}

CLIP_DLLEXPORT void
_clip_retcn(ClipMachine * mp, char *s, int l)
{
	ClipVar *vp = mp->bp - mp->argc - 1;

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);

	vp->t.type = CHARACTER_t;
	vp->t.flags = F_NONE;

	if (!s)
	{
		l = 0;
		s = "";
	}

	vp->s.str.buf = _clip_memdup(s, l);
	vp->s.str.len = l;
}

CLIP_DLLEXPORT void
_clip_retcn_m(ClipMachine * mp, char *s, int l)
{
	ClipVar *vp = RETPTR(mp);

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);
	memset(vp, 0, sizeof(ClipVar));
	vp->t.type = CHARACTER_t;
	vp->t.flags = F_NONE;

	vp->s.str.buf = s;
	vp->s.str.len = l;
}

CLIP_DLLEXPORT void _clip_retc(ClipMachine * mp, char *s)
{
	int l = 0;

	if (s)
		l = strlen(s);
	_clip_retcn(mp, s, l);
}

CLIP_DLLEXPORT int
_clip_storclen(ClipMachine * mp, const char *str, int len, int num, int ind)
{
#if 1
	ClipVar *vp = _clip_spar(mp, num);
	ClipVar lp ;

	if (!vp)
		return 0;

	if (vp->t.type == ARRAY_t)
	{
		if (ind < 0 || ind >= (int)vp->a.count)
			return 0;
		vp = _clip_vptr(vp->a.items + ind);
	}

	if (vp->t.flags == F_MSTAT)
		return 0;

	_clip_var_str(str, len, &lp);
	_clip_mclone(mp, vp, &lp);
	_clip_destroy(mp,&lp);
#else
	ClipVar *vp = _clip_par(mp, num);

	if (!vp)
		return 0;

	if (vp->t.type == ARRAY_t)
	{
		if (ind < 0 || ind >= vp->a.count)
			return 0;
		vp = _clip_vptr(vp->a.items + ind);
	}

	if (vp->t.flags == F_MSTAT)
		return 0;

	_clip_destroy(mp, vp);
	_clip_var_str(str, len, vp);
#endif
	return 1;
}

CLIP_DLLEXPORT int
_clip_storc(ClipMachine * mp, const char *str, int num, int ind)
{
	return _clip_storclen(mp, str, strlen(str), num, ind);
}

CLIP_DLLEXPORT int
_clip_stornd(ClipMachine * mp, double d, int num, int ind)
{
#if 1
	ClipVar *vp = _clip_spar(mp, num);
	ClipVar lp ;

	if (!vp)
		return 0;

	if (vp->t.type == ARRAY_t)
	{
		if (ind < 0 || ind >= (int)vp->a.count)
			return 0;
		vp = _clip_vptr(vp->a.items + ind);
	}

	if (vp->t.flags == F_MSTAT)
		return 0;

	_clip_var_num(d,&lp);
	_clip_mclone(mp, vp, &lp);
	_clip_destroy(mp,&lp);
#else
	ClipVar *vp = _clip_par(mp, num);

	if (!vp)
		return 0;

	if (vp->t.type == ARRAY_t)
	{
		if (ind < 0 || ind >= vp->a.count)
			return 0;
		vp = _clip_vptr(vp->a.items + ind);
	}

	if (vp->t.flags == F_MSTAT)
		return 0;

	_clip_destroy(mp, vp);
	_clip_var_num(d, vp);
#endif
	return 1;
}

CLIP_DLLEXPORT int
_clip_storni(ClipMachine * mp, int d, int num, int ind)
{
	return _clip_stornd(mp, d, num, ind);
}

CLIP_DLLEXPORT int
_clip_stornl(ClipMachine * mp, long d, int num, int ind)
{
	return _clip_stornd(mp, d, num, ind);
}

CLIP_DLLEXPORT int
_clip_storl(ClipMachine * mp, int val, int num, int ind)
{
	ClipVar *vp = _clip_par(mp, num);

	if (!vp)
		return 0;

	if (vp->t.type == ARRAY_t)
	{
		if (ind < 0 || ind >= (int)vp->a.count)
			return 0;
		vp = _clip_vptr(vp->a.items + ind);
	}

	if (vp->t.flags == F_MSTAT)
		return 0;

	_clip_destroy(mp, vp);
	_clip_var_log(val, vp);

	return 1;
}

static int
incr_var(ClipMachine * mp, ClipVar * vp, int isIncr)
{
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case NUMERIC_t:
		if (vp->t.memo)
		{
			rational *tmp = rational_long_init((long) 1);

			rational_adda(vp->r.r, tmp);
			rational_destroy(tmp);
		}
		else
			vp->n.d += (isIncr ? 1 : -1);
		return 0;
	case DATE_t:
		vp->d.julian += (isIncr ? 1 : -1);
		return 0;
	default:
		_clip_trap_str(mp, __file__, __LINE__, "invalid type in INCR/DECR operation");
		return _clip_call_errblock(mp, 1);
	}
}

CLIP_DLLEXPORT int
_clip_incr(struct ClipMachine *mp, void *lval, int isIncr, int isPost)
{
	int ret;
	ClipVar *vp = (ClipVar *) lval;

	if (isPost)
	{
		if ((ret = _clip_push(mp, vp)))
			return ret;
		if ((ret = incr_var(mp, vp, isIncr)))
			return ret;
	}
	else
	{
		if ((ret = incr_var(mp, vp, isIncr)))
			return ret;
		if ((ret = _clip_push(mp, vp)))
			return ret;
	}
	return 0;
}

CLIP_DLLEXPORT ClipVar *
_clip_unptr(ClipVar * vp)
{
	return _clip_vptr(vp);
}

CLIP_DLLEXPORT int
_clip_find_obj(ClipMachine * mp, long otype, ClipObjRtti ** rtti)
{
	ClipObjRtti **rp;

	for (rp = init_struct->_objrtti; *rp; ++rp)
		if ((*rp)->hashId == otype)
		{
			*rtti = *rp;
			return 0;
		}

	return 1;
}

static int
do_main(ClipMachine * mp, long hash, ClipFunction * func, ClipBlock * block, int argc, char **argv, char **envp)
{
	int i, ret;
#ifdef OS_MINGW
	ClipVar *_stack = malloc(sizeof(ClipVar) * (argc + 1));
#else
	ClipVar *_stack = alloca(sizeof(ClipVar) * (argc + 1));
#endif
	ClipFrame _frame =
		{ _stack, _stack, "" /*__FILE__*//* ": do_main()" */ , /*__LINE__*/ 0, 0, 0, 0, 0, 0, 0, 0, argc + 1, 0 };

	_frame.up = mp->fp;
	mp->fp = &_frame;
	if (_clip_init(mp, argc, argv, envp))
		goto _trap;
	argc = _clip_argc;
	argv = _clip_argv;

	if (!argc)
		argc = 1;
	mp->argc = argc - 1;
	_frame.sp->t.type = UNDEF_t;
	_frame.sp->t.flags = F_NONE;
	_frame.sp++;
	CLIP_CHECK_STACK;
	for (i = 1; i < argc; ++i, ++_frame.sp)
		_clip_var_str(argv[i], strlen(argv[i]), _frame.sp);

	init_machine(mp);
	if (func)
		ret = do_call(mp, func, argc - 1, 0, 0, 0);
	else if (block)
		ret = _clip_code_func(mp, block, argc - 1, 0, 0);
	else
		ret = _clip_func_hash(mp, hash, argc - 1, 0, 0);

	if (ret)
	{
		_clip_trap(mp, __file__, __LINE__);
		goto _trap;
	}

	ret = _clip_long(_stack);
	goto _return;
	  _trap:
	{
		char *msg = 0;

		descr_str(mp, mp->trapVar, &msg);
		if (!msg)
			msg = "unknown runtime error";
		printf("\n%s: runtime error: %s\n", _clip_progname, msg);
		_clip_logg(1, "%s: runtime error: %s", _clip_progname, msg);
		for (i = 0; i < mp->trapTrace.count; ++i)
		{
			printf("trace: %s\n", (char *) mp->trapTrace.items[i]);
			_clip_logg(1, "trace: %s", (char *) mp->trapTrace.items[i]);
		}
		ret = 1;
	}
	  _return:
	_clip_resume(mp, 0, 0);
	if (ret == 0)
		ret = errorlevel;
#ifdef OS_MINGW
	free(_stack);
#endif
	return ret;
}

CLIP_DLLEXPORT int
_clip_main(ClipMachine * mp, long hash, int argc, char **argv, char **envp)
{
	int ret;
	
#ifdef USE_TASK
	if (mp->main_func_level == 0)
		Task_INIT();
#endif
	mp->main_func_level++;
	ret = do_main(mp, hash, NULL, NULL, argc, argv, envp);
	mp->main_func_level--;
	if (mp->main_func_level == 0)
		delete_ClipMachine(mp);
	return ret;
}

CLIP_DLLEXPORT int
_clip_main_func(ClipMachine * mp, ClipFunction func, int argc, char **argv, char **envp)
{
	int ret;
	mp->main_func_level++;
	ret = do_main(mp, 0, func, NULL, argc, argv, envp);
	mp->main_func_level--;
	if (mp->main_func_level == 0)
		delete_ClipMachine(mp);
	return  ret;
}

CLIP_DLLEXPORT int
_clip_main_code(ClipMachine * mp, ClipBlock * block, int argc, char **argv, char **envp)
{
	int ret;
	mp->main_func_level++;
	ret = do_main(mp, 0, NULL, block, argc, argv, envp);
	mp->main_func_level--;
	if (mp->main_func_level == 0)
		delete_ClipMachine(mp);
	return ret;
}

CLIP_DLLEXPORT const char *
_clip_typename(ClipVar * vp)
{
	enum ClipVarType type = _clip_type(vp);

	if (!vp)
		return "NIL";
	switch (type)
	{
	case UNDEF_t:
		return "UNDEF";
	case NUMERIC_t:
		return "NUMERIC";
	case CHARACTER_t:
		return "CHARACTER";
	case LOGICAL_t:
		return "LOGICAL";
	case DATE_t:
		return "DATE";
	case OBJECT_t:
		return "OBJECT";
	case PCODE_t:
		return "PCODE";
	case CCODE_t:
		return "CCODE";
	case ARRAY_t:
		return "ARRAY";
	case MAP_t:
		return "MAP";
	case MEMO_t:
		return "MEMO";
	case DATETIME_t:
		return "DATETIME";
	}
	return "UNKNOWN";
}

/* ] utils */

static int
cmp_hash_bucket(const void *p1, const void *p2)
{
	unsigned long h1 = (unsigned long) GETLONG(p1);
	unsigned long h2 = ((ClipHashBucket *) p2)->hash;

	if (h1 < h2)
		return -1;
	else if (h1 > h2)
		return 1;
	else
		return 0;
}

CLIP_DLLEXPORT void
_clip_hash_name(ClipMachine * mp, long hash, char *buf, int buflen)
{
	ClipFrame *fp;
	char *s = 0;

	if (!buflen)
		return;
	else
		buf[0] = 0;

	switch (hash)
	{
	case HASH_main:
		s = "MAIN";
		break;
	case HASH_name:
		s = "NAME";
		break;
	case HASH_filename:
		s = "filename";
		break;
	case HASH_operation:
		s = "operation";
		break;
	case HASH_tries:
		s = "tries";
		break;
	case HASH_canSubstitute:
		s = "canSubstitute";
		break;
	case HASH_canRetry:
		s = "canRetry";
		break;
	case HASH_canDefault:
		s = "canDefault";
		break;
	case HASH_osCode:
		s = "osCode";
		break;
	case HASH_genCode:
		s = "genCode";
		break;
	case HASH_severity:
		s = "severity";
		break;
	case HASH_description:
		s = "description";
		break;
	case HASH_args:
		s = "args";
		break;
	case HASH_cargo:
		s = "cargo";
		break;
	case HASH_subCode:
		s = "subCode";
		break;
	case HASH_subSystem:
		s = "subSystem";
		break;
	case HASH_syserr:
		s = "syserr";
		break;
	}

	if (s)
	{
		strncpy(buf, s, buflen);
		return;
	}

	s = (char *) HashTable_fetch(mp->hashnames, hash);
	if (s)
	{
		strncpy(buf, s, buflen);
		return;
	}

	for (fp = mp->fp; fp; fp = fp->up)
	{
		if (fp->names)
		{
			ClipHashBucket *bp;

			bp = (ClipHashBucket *) bsearch(&hash, fp->names->buckets,
							fp->names->num, sizeof(ClipHashBucket), cmp_hash_bucket);
			if (bp)
			{
				strncpy(buf, fp->names->mem + bp->offs, buflen);
				return;
			}
		}
	}

	snprintf(buf, buflen, "0x%08lx", hash);
}

CLIP_DLLEXPORT long
_clip_pardj(ClipMachine * mp, int num)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));
		enum ClipVarType type = _clip_type(vp);

		if (type == DATE_t)
		{
			if (vp->t.flags & F_MPTR)
				vp = vp->p.vp;
			return vp->d.julian;
		}
	}
	return 0;
}

CLIP_DLLEXPORT long
_clip_pardtj(ClipMachine * mp, int num, long *time)
{
	if (num > 0 && num <= mp->argc)
	{
		ClipVar *vp = (mp->bp - (mp->argc - (num - 1)));
		enum ClipVarType type = _clip_type(vp);

		if (type == DATETIME_t)
		{
			if (vp->t.flags & F_MPTR)
				vp = vp->p.vp;
			*time = vp->dt.time;
			return vp->dt.julian;
		}
	}
	return 0;
}

CLIP_DLLEXPORT long
_clip_pardc(ClipMachine * mp, int num, int *yy, int *mm, int *dd, int *ww)
{
	long julian = _clip_pardj(mp, num);

	_clip_cdate(julian, dd, mm, yy, ww);
	return julian;
}

CLIP_DLLEXPORT void
_clip_retdj(ClipMachine * mp, long julian)
{
	ClipVar *vp = RETPTR(mp);

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);

	vp->t.type = DATE_t;
	vp->t.flags = F_NONE;
	vp->t.len = 8;
	vp->t.dec = 0;
	vp->d.julian = julian;
}

CLIP_DLLEXPORT void
_clip_retdtj(ClipMachine * mp, long julian, long time)
{
	ClipVar *vp = RETPTR(mp);

	if (vp->t.type==CHARACTER_t && vp->s.str.buf != NULL )
		free(vp->s.str.buf);

	vp->t.type = DATETIME_t;
	vp->t.flags = F_NONE;
	vp->t.len = 8;
	vp->t.dec = 0;
	vp->dt.julian = julian;
	vp->dt.time = time;
}

CLIP_DLLEXPORT void
_clip_retdc(ClipMachine * mp, int yy, int mm, int dd)
{
	_clip_retdj(mp, _clip_jdate(dd, mm, yy));
}

CLIP_DLLEXPORT int
_clip_stordj(ClipMachine * mp, long julian, int num, int ind)
{
	ClipVar *vp = _clip_par(mp, num);

	if (!vp)
		return 0;

	if (vp->t.type == ARRAY_t)
	{
		if (ind < 0 || ind >= (int)vp->a.count)
			return 0;
		vp = _clip_vptr(vp->a.items + ind);
		goto assign;
	}
	else if (vp->t.type == DATE_t)
	{
		  assign:
		if (vp->t.flags == F_MSTAT)
			return 0;
		vp->d.julian = julian;
	}
	else
	{
		return 0;
	}

	return 1;
}

CLIP_DLLEXPORT int
_clip_stordc(ClipMachine * mp, int yy, int mm, int dd, int num, int ind)
{
	return _clip_stordj(mp, _clip_jdate(dd, mm, yy), num, ind);
}

#define DD_BUF_LEN 	7

long
_clip_str_to_date(char *str, char *format, int epoch)
{
	int i, j, len;
	int d = 0, m = 0, y = 0;
	int b[3] = { 0, 0, 0 };
	int mms[12] = {31,29,31,30,31,30,31,31,30,31,30,31};
	char ch = 0;

	if (epoch == 0)
		epoch = CLIP_EPOCH_DEFAULT;
	if (format == NULL)
		format = CLIP_DATEFORMAT_DEFAULT;

	len = strlen(format);
	for (i = 0, j = 0; i < len; i++)
	{
		if (ch == format[i])
			continue;
		switch (format[i])
		{
		case 'y':
		case 'Y':
			y = j;
			break;
		case 'm':
		case 'M':
			m = j;
			break;
		case 'd':
		case 'D':
			d = j;
			break;
		case ' ':
			break;
		default:
			j++;
			break;
		}
		if (j > 2)
			break;
		ch = format[i];
	}
	if (j < 2)
		return 0;

	len = strlen(str);

	for (i = 0; i < len && !isdigit(str[i]); i++);

	for (j = 0, b[0] = i; i < len && j < 2; i++)
	{
		ch = str[i];
		if ((ch < '0' || ch > '9') && ch != ' ')
		{
			for (; i < len && !isdigit(str[i]); i++);
			j++;
			b[j] = i;
		}
	}
	if (j < 2)
		return 0;
	m = atoi(str + b[m]);
	d = atoi(str + b[d]);
	y = atoi(str + b[y]);
	if (m == 0 && d == 0 && y == 0)
		return 0;
	if (m > 12 || d > 31)
		return 0;
	/*
	printf("\nctod m=%d,d=%d,mms[m]=%d\n",m,d,mms[m]);
	*/
	if (m > 0 && (d > mms[m-1]))
		return 0;

	i = epoch % 100;
	j = epoch / 100;
	if (y < 100)
	{
		if (y < i)
			y += (j + 1) * 100;
		else
			y += j * 100;
	}
	return _clip_jdate(d, m, y);
}

CLIP_DLLEXPORT void
_clip_datetostr(long date, char *buf)
{
	int dd, mm, yy, ww;
	char *sss = "0123456789";

	buf[8] = 0;
	if (date == 0)
	{
		memset(buf, ' ', 8);
		return;
	}
	_clip_cdate(date, &dd, &mm, &yy, &ww);
	buf[7] = sss[dd % 10];
	buf[6] = sss[dd / 10];
	buf[5] = sss[mm % 10];
	buf[4] = sss[mm / 10];

	buf[3] = sss[yy % 10];
	yy /= 10;
	buf[2] = sss[yy % 10];
	yy /= 10;
	buf[1] = sss[yy % 10];
	yy /= 10;
	buf[0] = sss[yy];
}

CLIP_DLLEXPORT char *
_clip_date_to_str(long date, char *format)
{
	int i, fl;
	int dl = 1, ml = 1, yl = 1;
	int dd, mm, yy, ww;
	char ch, ch2;
	char ds[DD_BUF_LEN + 1], ms[DD_BUF_LEN + 1], ys[DD_BUF_LEN + 1];
	char *ret;

	if (format == NULL)
		format = CLIP_DATEFORMAT_DEFAULT;
	fl = strlen(format);
	ret = malloc(fl + 1);
	memset(ret, 32, fl - 1);
	ret[fl] = 0;
	memset(ds, 32, DD_BUF_LEN);
	ds[DD_BUF_LEN] = 0;
	memset(ms, 32, DD_BUF_LEN);
	ms[DD_BUF_LEN] = 0;
	memset(ys, 32, DD_BUF_LEN);
	ys[DD_BUF_LEN] = 0;

	_clip_cdate(date, &dd, &mm, &yy, &ww);
	if (date != 0)
	{
		snprintf(ds, DD_BUF_LEN + 1, "%07d", dd);
		snprintf(ms, DD_BUF_LEN + 1, "%07d", mm);
		snprintf(ys, DD_BUF_LEN + 1, "%07d", yy);
	}
	for (i = fl; i >= 0; i--)
	{
		ch = format[i];
		switch (ch)
		{
		case 'd':
		case 'D':
			ch2 = ds[DD_BUF_LEN - dl];
			dl++;
			break;
		case 'm':
		case 'M':
			ch2 = ms[DD_BUF_LEN - ml];
			ml++;
			break;
		case 'y':
		case 'Y':
			ch2 = ys[DD_BUF_LEN - yl];
			yl++;
			break;
		default:
			ch2 = ch;
			break;
		}
		ret[i] = ch2;
		/*printf("%c%c%d%d%d.",ch,ch2,dl,ml,yl); */
	}
	return ret;
}

/*
   1) returns number of characters matched, -1 if failed.
   (Can return 0 on patterns like "" or "$")
   2) does not require pattern to match to end of string
   3) case-independent
 */

CLIP_DLLEXPORT int
_clip_glob_match(const char *string, const char *pattern, int caseflag)
{
	char c2;
	int match = 0;		/* # of chars matched */

	while (1)
	{
		/* If at end of pattern, success! */
		if (*pattern == 0)
		{
#if 0
			return match;
#else
			/* uri added: bug with pattern length < string length */
			if (*string == 0)
				return match;
			else
				return (-1);
#endif
		}

		/* If last pattern character is '$', verify that entire
		 * string has been matched.
		 */
		if ((*pattern == '$') && (pattern[1] == 0))
		{
			if (*string == 0)
				return (match);
			else
				return (-1);
		}

		/* Check for a "*" as the next pattern character.  It matches
		 * any substring.  We handle this by calling ourselves
		 * recursively for each postfix of string, until either we
		 * match or we reach the end of the string.
		 */

		if (*pattern == '*')
		{

			int head_len;
			const char *tail;

			pattern += 1;
			if (*pattern == 0)
			{
				return (strlen(string) + match);	/* DEL */
			}

			/* find longest match - switched to this on 12/31/93 */
			head_len = strlen(string);	/* length before tail */
			tail = string + head_len;
			/*printf("pattern='%s' tail='%s'\n", pattern, tail); */
			while (head_len >= 0)
			{
				int rc;

				if (-1 != (rc = _clip_glob_match(tail, pattern, caseflag)))
				{
					/*printf("rc=%d match=%d head_len=%d tail='%s' str='%s' patt='%s'\n", rc, match, head_len, tail,
					   sstr, patt); */
					return rc + match + head_len;	/* DEL */
				}
				tail--;
				head_len--;
			}

			return -1;	/* DEL */
		}

#if 1
		if (*pattern == '?')
		{
			if (!*string)
			{
				for (; *pattern; pattern++)
					if (*pattern != '?' && *pattern != '*')
						return -1;
				return match;
			}
		}
#endif
		/*
		 * after this point, all patterns must match at least one
		 * character, so check this
		 */

		if (*string == 0)
			return -1;

#if 1
		/* Check for a "?" as the next pattern character.  It matches
		 * any single character.
		 */

		if (*pattern == '?')
		{
			goto thisCharOK;
		}
#endif
		/* Check for a "[" as the next pattern character.  It is followed
		 * by a list of characters that are acceptable, or by a range
		 * (two characters separated by "-").
		 */

		if (*pattern == '[')
		{
			pattern += 1;
			while (1)
			{
				if ((*pattern == ']') || (*pattern == 0))
				{
					return -1;	/* was 0; DEL */
				}
				if (*pattern == *string)
				{
					break;
				}
				if (pattern[1] == '-')
				{
					c2 = pattern[2];
					if (c2 == 0)
					{
						return -1;	/* DEL */
					}
					if ((*pattern <= *string) && (c2 >= *string))
					{
						break;
					}
					if ((*pattern >= *string) && (c2 <= *string))
					{
						break;
					}
					pattern += 2;
				}
				pattern += 1;
			}

/* OOPS! Found a bug in vanilla Tcl - have sent back to Ousterhout */
/* but he hasn't integrated it yet. - DEL */

#if 0
			while ((*pattern != ']') && (*pattern != 0))
#else
			while (*pattern != ']')
			{
				if (*pattern == 0)
				{
					pattern--;
					break;
				}
#endif
				pattern += 1;
			}
			goto thisCharOK;
		}

		/* If the next pattern character is backslash, strip it off
		 * so we do exact matching on the character that follows.
		 */

		if (*pattern == '%')
		{
			pattern += 1;
			if (*pattern == 0)
			{
				return -1;
			}
			switch (*pattern)
			{
			case 'r':
				if (*string != '\r')
					return -1;
				break;
			case 'n':
				if (*string != '\n')
					return -1;
				break;
			case 's':
				if (*string != ' ')
					return -1;
				break;
			case 'e':
				if (*string != '\033')
					return -1;
				break;
			default:
				if (*string != *pattern)
					return -1;
				break;
			}
		}
		else if ((caseflag && _clip_toupper(*pattern) != _clip_toupper(*string)) || (!caseflag && *pattern != *string))
		{
			return -1;
		}

		  thisCharOK:
		pattern += 1;
		string += 1;
		match++;
	}
}

typedef struct
{
	void *item;
	void (*destroy) (void *);
}
item_bucket;

CLIP_DLLEXPORT void *
_clip_fetch_item(ClipMachine * mp, long hash)
{
	item_bucket *ip;

	ip = (item_bucket *) HashTable_fetch(mp->store, hash);
	if (ip)
		return ip->item;
	else
		return 0;
}

CLIP_DLLEXPORT void
_clip_store_item(ClipMachine * mp, long hash, void *item)
{
	item_bucket *ip;

	_clip_free_item(mp, hash);
	ip = NEW(item_bucket);
	ip->item = item;
	HashTable_store(mp->store, ip, hash);
}

CLIP_DLLEXPORT void
_clip_store_item_destroy(ClipMachine * mp, long hash, void *item, void (*destroy) (void *))
{
	item_bucket *ip;

	_clip_free_item(mp, hash);
	ip = NEW(item_bucket);
	ip->item = item;
	ip->destroy = destroy;
	HashTable_store(mp->store, ip, hash);
}

CLIP_DLLEXPORT void
_clip_free_item(ClipMachine * mp, long hash)
{
	item_bucket *ip;

	ip = (item_bucket *) HashTable_remove(mp->store, hash);
	if (ip)
	{
		if (ip->destroy)
			ip->destroy(ip->item);
		else
			free(ip->item);
		free(ip);
	}
}

CLIP_DLLEXPORT void
_clip_remove_item(ClipMachine * mp, long hash)
{
	void *rp;

	rp = HashTable_remove(mp->store, hash);
	free(rp);
}

/*
 * CONTAINER
 */

CLIP_DLLEXPORT int
_clip_compare_c_item(const void *p1, const void *p2)
{
	return ((ContainerItem *) p1)->key - ((ContainerItem *) p2)->key;
}

CLIP_DLLEXPORT int
_clip_compare_c_item_type(const void *p1, const void *p2)
{
	return ((ContainerItem *) p1)->type - ((ContainerItem *) p2)->type;
}

CLIP_DLLEXPORT void *
_clip_fetch_c_item(ClipMachine * cm, int key, int type)
{
	ContainerItem citem, *ci = NULL;

	if (cm->container == NULL)
		return NULL;

	citem.key = key;
	ci = bsearch(&citem, cm->container->items, cm->container->len, sizeof(ContainerItem), _clip_compare_c_item);
	return (ci && ci->type == type ? ci->item : NULL);
}

CLIP_DLLEXPORT void *
_clip_fetch_c_item_type(ClipMachine * cm, int type, int *key)
{
	ContainerItem citem, *ci = NULL;

	if (cm->container == NULL)
		return NULL;

	if (key)
		*key = -1;
	citem.type = type;
	ci = bsearch(&citem, cm->container->items, cm->container->len, sizeof(ContainerItem), _clip_compare_c_item_type);
	if (ci && key)
		*key = ci->key;
	return (ci ? ci->item : NULL);
}

CLIP_DLLEXPORT int
_clip_destroy_c_item(ClipMachine * cm, int key, int type)
{
	int len;
	Container *cn = cm->container;
	ContainerItem citem, *ci = NULL;

	if (cn == NULL)
		return 0;
	citem.key = key;
	ci = bsearch(&citem, cn->items, cn->len, sizeof(ContainerItem), _clip_compare_c_item);
	if (ci == NULL || ci->type != type)
		return 0;

	if (ci->destroy)
		ci->destroy(ci->item);

	len = cn->len * sizeof(ContainerItem) - (ci - cn->items + 1) * sizeof(ContainerItem);

	/*printf( "_destroy_: items=%lx, item=%lx, len=%d\n", cn->items, ci, len ); */

	if (len > 0)
		memmove(ci, ci + 1, len);

	cn->len--;

	return 1;
}

CLIP_DLLEXPORT int
_clip_store_c_item(ClipMachine * cm, void *item, int type, void (*destroy) (void *))
{
	Container *cn = NULL;
	ContainerItem citem;
	int i = 0;

	citem.item = item;
	citem.type = type;
	citem.destroy = destroy;

	cn = cm->container;

	/* search for first unused key */
	for (i = 0; i < cn->len; i++)
	{
		if (i != cn->items[i].key)
			break;
	}
	citem.key = i;

	cn->items = realloc(cn->items, (cn->len + 1) * sizeof(ContainerItem));

	cn->items[cn->len++] = citem;

	qsort(cn->items, cn->len, sizeof(ContainerItem), _clip_compare_c_item);

	/*printf( "arr:" );
	   for( i=0; i < cn->len; i++ ) {
	   printf( "<it=%lx, f=%lx>", &(cn->items)[i], (cn->items)[i].item );
	   }
	   printf( "\n" ); */

	return citem.key;
}

/*
 * CONTAINER end
 */

static char *
_clip_normalize_path(char * _path)
{
	/* translate "path1/../path2" -> "path2" */
	int pos[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int pos2[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int flag[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int i,beg,end,len,cur = 1, cur2 = 0;
	char *tmp;
	char *path;

	path = strdup(_path);

	i = 0;
	if ( path[i]=='/' || path[i]=='\\')
		 { i++; pos[0]=1;}
	for (len=strlen(path); i<len; i++)
	{
		if ( path[i] == '.' )
			flag[cur]++;
		else if ( path[i]=='/' || path[i]=='\\')
		{
			pos[cur]=i+1;
			cur++;
		}
		else
			flag[cur]--;
		if (cur>15)
			break;
	}
	for (cur=0,cur2=-1; cur<16; cur++)
	{
		if ( flag[cur] == 2) /* subpath ==".." */
			cur2--;
		else
			cur2++;
		if ( cur2 < 0)
			cur2 = 0;
		//printf("\ncur=%d,flag=%d,cur2=%d",cur,flag[cur],cur2);
		pos2[cur2] = cur;
	}

	tmp = strdup(path);
	//printf("\npos[0]=%d,pos[1]=%d,pos[2]=%d",pos[0],pos[1],pos[2]);
	//printf("\npo2[0]=%d,po2[1]=%d,po2[2]=%d",pos2[0],pos2[1],pos2[2]);
	memset(path,0,len);
	for (i=0,len=0; i<16; i++)
	{
		beg = pos[ pos2[i] ];
		end = pos[ pos2[i]+1 ];
		if (end == 0)
		{
			end = strlen(tmp);
			i = 200;
		}
		strncpy(path+len,tmp+beg,end-beg);
		len+=(end-beg);
		//printf("\ni=%d,pos2=%d,beg=%d,end=%d,tmp=%s,str=%s\n",i,pos2[i],beg,end,tmp,path);
	}
	return path;
}

CLIP_DLLEXPORT int
_clip_translate_path(ClipMachine * mp, const char *str, char *buf, int buflen)
{
#ifdef _WIN32
	if ((memcmp(str, "\\\\", 2) == 0) || (memcmp(str, "//", 2) == 0))
	{
		/* network resource */
		strncpy(buf, str, buflen);
		return 0;
	}

	if (str != NULL && (*str == '/' || *str == '\\') && memcmp(str, "/cygdrive/", 10))
	{
		/* add default drive */
		char *s = _clip_fetch_item(mp, CLIP_CUR_DRIVE);
		if (mp->rootpath==NULL)
			snprintf(buf, buflen - 1, "%s%s", s, str);
		else
		{
			char * _str = NULL;
			_str = _clip_normalize_path( ( char *) str);
			snprintf(buf, buflen - 1, "%s%s%s",s,mp->rootpath, _str);
			free(_str);
		}
		return 0;
	}
	if (str != NULL && strlen(str) >= 2 && str[1] == ':' && str[2] != '/' && str[2] != '\\')
	{
		/* drive with default path */
		char *def_path;

		def_path = _clip_fetch_item(mp, _hash_cur_dir[toupper(*str) - 65]);
		snprintf(buf, buflen - 1, "%c:%s%s\\%s", *str, mp->rootpath, def_path, str + 2);
		return 0;
	}
	if (mp->rootpath == NULL )
		snprintf(buf,buflen-1,"%s",str);
	else
	{
		char * _str = NULL;
		char * tmp = strdup(str);
		_str = _clip_normalize_path(tmp);
		snprintf(buf,buflen-1,"%s%s",mp->rootpath,_str);
		free(_str);
		free(tmp);
	}
	return 0;
#else
	char *s, *e;
	char *root = 0;
	int bl, ch;

	buf[0] = 0;
	if (strchr(str, '|'))
	{
		strncpy(buf, str, buflen);
		buf[buflen - 1] = 0;
		return 0;
	}

	if ((s = strchr(str, ':')) && s == str + 1)
	{
		char dn[3];
		long dh;

		dn[0] = toupper(*str);
		dn[1] = ':';
		dn[2] = 0;
		dh = _clip_hashstr(dn);
		if ((root = _clip_fetch_item(mp, dh)) == NULL)
		{
			return -1;
		}

		str = s + 1;
		while ((ch = *str) && (ch == '\\' || ch == '/'))
			++str;
	}

	bl = 0;
	if (root)
	{
		snprintf(buf, buflen - 1, "%s", root);
		bl = strlen(buf);
		if (bl > 0 && (buf[bl - 1] == '/' || buf[bl - 1] == '\\'))
			--bl;
		buf[bl++] = '/';
	}
	/*
	else if (str[0] == '.' && str[1] == '.')
	{
		strncpy(buf, str, buflen);
		return 1;
	}
	*/
	else if ( *str != '/' /*&& *str != '\\'*/)
	{
		if ( mp->rootpath == NULL )
		{
			getcwd(buf, buflen);
			bl = strlen(buf);
			buf[bl++] = '/';
		}
	}
	if ( mp->rootpath != NULL )
	{
		char * _str = NULL;
		char * tmp = strdup(buf);
		snprintf(buf,buflen, "%s%s",mp->rootpath,tmp);
		free(tmp);
		bl = strlen(buf);
		
		tmp = strdup(str);
		_str = (char *)_clip_normalize_path(tmp);
		free(tmp);
		snprintf(buf + bl, buflen - bl, "%s", _str);
		free(_str);
	}
	else
	{
		snprintf(buf + bl, buflen - bl, "%s", str);
	}
	bl = strlen(buf);
	for (e = buf + bl, s = buf; s < e; ++s)
	{
		if (*s == '\\')
			*s = '/';
	}
	s = strrchr(buf,'/');
	if (s && s != buf )
	{
		char ocwd[PATH_MAX];
		char fname[PATH_MAX];
		char ch;

		getcwd(ocwd,sizeof(ocwd));

		strncpy(fname,s+1,sizeof(fname));

		ch  = *s;
		* s = 0;
		if (chdir(buf) == 0)
		{
			getcwd(buf,buflen);
			strcat(buf,"/");
			strcat(buf,fname);
			chdir(ocwd);
		}
		else
			*s = ch;
	}
	bl = strlen(buf);
	if (mp->flags & TRANSLATE_FLAG)
	{
#if 1
		int i;
		for (i=0; i<bl; i++)
		{
			buf[i] = tolower((unsigned char) (buf[i]));
		}
#else
		char *tmp;
		for (e = buf + bl, s = buf; s < e; ++s)
			*s = tolower(*(unsigned char *) s);
		for (tmp=buf+strlen(buf)-1; *tmp == ' ' && tmp>=buf; tmp--);
		tmp++; *tmp=0;
#endif
	}
	return 0;
#endif
}

CLIP_DLLEXPORT void
_clip_mptr(ClipMachine * mp, ClipVar * refs, int num)
{
	while (num--)
	{
		ClipVar *vp = NEW(ClipVar);

		refs->t.type = UNDEF_t;
		refs->t.flags = F_MREF;
		refs->p.vp = vp;
		refs->t.field = 0;
		refs->p.fp = 0;
		vp->t.count = 1;

		refs++;
	}
}

CLIP_DLLEXPORT ClipVar *
_clip_new_error(ClipMachine * mp,
		const char *description, int severity,
		int genCode, int osCode,
		int canDefault, int canRetry, int canSubstitute,
		int tries, const char *subSystem, int subCode, const char *operation)
{
	ClipVar *rp = NEW(ClipVar);
	ClipVar var;
	char *s;
	long dim;
	int i;

	_clip_map(mp, rp);

	memset(&var, 0, sizeof(var));

	/* 'cargo' */

	dim = mp->argc;
	_clip_array(mp, &var, 1, &dim);
	for (i = 0; i < mp->argc; i++)
	{
		dim = i;
		_clip_aset(mp, &var, ARGPTR(mp, i + 1), 1, &dim);
	}
	_clip_madd(mp, rp, HASH_args, &var);
	_clip_destroy(mp, &var);

	_clip_var_str(description, strlen(description), &var);
	_clip_madd(mp, rp, HASH_description, &var);
	_clip_destroy(mp, &var);

	_clip_var_num(severity, &var);
	_clip_madd(mp, rp, HASH_severity, &var);
	_clip_destroy(mp, &var);

	_clip_var_num(genCode, &var);
	_clip_madd(mp, rp, HASH_genCode, &var);
	_clip_destroy(mp, &var);

	/* 'osCode' */
	_clip_var_num(osCode, &var);
	_clip_madd(mp, rp, HASH_osCode, &var);
	_clip_destroy(mp, &var);

	/* 'canDefault' */
	_clip_var_log(canDefault, &var);
	_clip_madd(mp, rp, HASH_canDefault, &var);
	_clip_destroy(mp, &var);

	/* 'canRetry' */
	_clip_var_log(canRetry, &var);
	_clip_madd(mp, rp, HASH_canRetry, &var);
	_clip_destroy(mp, &var);

	/* 'canSubstitute' */
	_clip_var_log(canSubstitute, &var);
	_clip_madd(mp, rp, HASH_canSubstitute, &var);
	_clip_destroy(mp, &var);

	/* 'tries' */
	_clip_var_num(tries, &var);
	_clip_madd(mp, rp, HASH_tries, &var);
	_clip_destroy(mp, &var);

	/* 'filename' */
	if (mp->fp)
		s = (char *) mp->fp->filename;
	else
		s = "";
	_clip_var_str(s, strlen(s), &var);
	_clip_madd(mp, rp, HASH_filename, &var);
	_clip_destroy(mp, &var);

	/* 'operation' */
	s = (char *) operation;
	if (!s)
		s = (char *) mp->fp->procname;
	if (!s)
		s = "";
	_clip_var_str(s, strlen(s), &var);
	_clip_madd(mp, rp, HASH_operation, &var);
	_clip_destroy(mp, &var);

	/* 'subSystem' */
	s = (char *) subSystem;
	if (!s)
		s = "";
	_clip_var_str(s, strlen(s), &var);
	_clip_madd(mp, rp, HASH_subSystem, &var);
	_clip_destroy(mp, &var);

	/* 'subCode' */
	_clip_var_num(subCode, &var);
	_clip_madd(mp, rp, HASH_subCode, &var);
	_clip_destroy(mp, &var);

	/* 'syserr' */
	_clip_var_str(mp->syserr, strlen(mp->syserr), &var);
	mp->syserr[0] = 0;
	_clip_madd(mp, rp, HASH_syserr, &var);
	_clip_destroy(mp, &var);

	return rp;
}

CLIP_DLLEXPORT const char *
_clip_errname(int code)
{
	char *msg;

	switch (code)
	{
	default:
		msg = _clip_gettext("unknown error");
		break;
	case EG_ARG:
		msg = _clip_gettext("invalid argument");
		break;
	case EG_BOUND:
		msg = _clip_gettext("bound error");
		break;
	case EG_STROVERFLOW:
		msg = _clip_gettext("string too long");
		break;
	case EG_NUMOVERFLOW:
		msg = _clip_gettext("number too big");
		break;
	case EG_ZERODIV:
		msg = _clip_gettext("divide by zero");
		break;
	case EG_NUMERR:
		msg = _clip_gettext("number error");
		break;
	case EG_SYNTAX:
		msg = _clip_gettext("syntax error");
		break;
	case EG_COMPLEXITY:
		msg = _clip_gettext("syntax too complex");
		break;

	case EG_MEM:
		msg = _clip_gettext("memory error");
		break;
	case EG_NOFUNC:
		msg = _clip_gettext("no function");
		break;
	case EG_NOMETHOD:
		msg = _clip_gettext("no method");
		break;
	case EG_NOVAR:
		msg = _clip_gettext("no variable");
		break;
	case EG_NOALIAS:
		msg = _clip_gettext("no alias");
		break;
	case EG_NOVARMETHOD:
		msg = _clip_gettext("no varmethod");
		break;

	case EG_CREATE:
		msg = _clip_gettext("create error");
		break;
	case EG_OPEN:
		msg = _clip_gettext("open error");
		break;
	case EG_CLOSE:
		msg = _clip_gettext("close error");
		break;
	case EG_READ:
		msg = _clip_gettext("read error");
		break;
	case EG_WRITE:
		msg = _clip_gettext("write error");
		break;
	case EG_PRINT:
		msg = _clip_gettext("print error");
		break;

	case EG_UNSUPPORTED:
		msg = _clip_gettext("unsupported");
		break;
	case EG_LIMIT:
		msg = _clip_gettext("limit exhaust");
		break;
	case EG_CORRUPTION:
		msg = _clip_gettext("corruption detected");
		break;
	case EG_DATATYPE:
		msg = _clip_gettext("wrong datatype");
		break;
	case EG_DATAWIDTH:
		msg = _clip_gettext("wrong datawidth");
		break;
	case EG_NOTABLE:
		msg = _clip_gettext("no table");
		break;
	case EG_NOORDER:
		msg = _clip_gettext("no order");
		break;
	case EG_SHARED:
		msg = _clip_gettext("share violation");
		break;
	case EG_UNLOCKED:
		msg = _clip_gettext("unlocked");
		break;
	case EG_READONLY:
		msg = _clip_gettext("readonly");
		break;
	case EG_APPENDLOCK:
		msg = _clip_gettext("appendlock");
		break;
	case EG_BADALIAS:
		msg = _clip_gettext("bad alias");
		break;
	case EG_LOCK:
		msg = _clip_gettext("locking error");
		break;
	case EG_SIGNAL:
		msg = _clip_gettext("system signal");
		break;
	}

	return msg;
}

CLIP_DLLEXPORT void
_clip_generr(ClipMachine * mp, int code)
{
	if (mp->trapVar)
		return;

	if (code < 0)
		code = -code;

	mp->trapVar = _clip_new_error(mp, _clip_errname(code), 2, code, 0, 0, 0, 0, 0, 0, 0, 0);
}

CLIP_DLLEXPORT int
_clip_trap_err(ClipMachine * mp, int genCode, int canDefault, int canRetry,
		   const char *subSystem, int subCode, const char *operation)
{
	ClipVar *vp;

	vp = _clip_new_error(mp, _clip_errname(genCode), 2, genCode, 0, canDefault,
				 canRetry, 0, 1, subSystem, subCode, operation);

	_clip_trap_var(mp, mp->fp ? mp->fp->filename : "", mp->fp ? mp->fp->line : 0, vp);

	return genCode;
}

CLIP_DLLEXPORT ClipVar *
_clip_mget(ClipMachine * mp, ClipVar * vp, long no)
{
	int ind;

	if (!_clip_mind(mp, vp, no, &ind))
	{
		ClipVar *ep, *ap;

		ap = _clip_vptr(vp);
		ep = &(ap->m.items[ind].v);
		return ep;
	}

	return 0;
}

CLIP_DLLEXPORT int
_clip_mtype(ClipMachine * mp, ClipVar * vp, long no)
{
	int ind;

	if (!_clip_mind(mp, vp, no, &ind))
	{
		ClipVar *ap;

		ap = _clip_vptr(vp);
		return ap->m.items[ind].v.t.type;
	}

	return UNDEF_t;
}

CLIP_DLLEXPORT int
_clip_mgetn(ClipMachine * mp, ClipVar * vp, long no, double *n)
{
	int ind;

	if (!_clip_mind(mp, vp, no, &ind))
	{
		ClipVar *ep, *ap;

		ap = _clip_vptr(vp);

		ep = &(ap->m.items[ind].v);
		*n = _clip_double(ep);
		return 0;
	}
	return -1;
}

CLIP_DLLEXPORT int
_clip_mgetl(ClipMachine * mp, ClipVar * vp, long no, int *n)
{
	int ind;

	if (!_clip_mind(mp, vp, no, &ind))
	{
		ClipVar *ep, *ap;

		ap = _clip_vptr(vp);

		ep = &(ap->m.items[ind].v);
		*n = _clip_log(ep);
		return 0;
	}
	return -1;
}

CLIP_DLLEXPORT int
_clip_mgetc(ClipMachine * mp, ClipVar * vp, long no, char **strp, int *lenp)
{
	int ind;

	if (!_clip_mind(mp, vp, no, &ind))
	{
		ClipVar *ep, *ap;
		int r;

		ap = _clip_vptr(vp);

		ep = &(ap->m.items[ind].v);
		r = _clip_strFromVar(mp, ep, strp, lenp);
		return r;
	}
	return -1;
}

CLIP_DLLEXPORT void
_clip_upstr(char *s, int len)
{
	unsigned char *us = (unsigned char *) s;

	for (; len; us++, len--)
	{
		int c = *us;

		*us = _clip_uptbl[c];
	}
}

CLIP_DLLEXPORT int
_clip_toupper(int c)
{
	return _clip_uptbl[c];
}

CLIP_DLLEXPORT int
_clip_tolower(int c)
{
	return _clip_lowtbl[c];
}

CLIP_DLLEXPORT int
_clip_calc_hash(ClipMachine * mp)
{
	ClipVar *vp = mp->fp->sp - 1, *vvp;
	long hash = -1;

	vvp = _clip_vptr(vp);

	if (vvp->t.type == NUMERIC_t)
	{
		if (vvp->t.memo)
			hash = (long) rational_toDouble(vvp->r.r);
		else
			hash = (long) vvp->n.d;
	}
	else if (vvp->t.type == CHARACTER_t)
		hash = _clip_casehashbytes(0, vvp->s.str.buf, vvp->s.str.len);

	_clip_destroy(mp, vp);
	vp->t.flags = F_NONE;
	vp->t.type = NUMERIC_t;
	vp->t.len = 10;
	vp->t.dec = 0;
	vp->t.memo = 0;
	vp->n.d = hash;

	return 0;
}

CLIP_DLLEXPORT int
_clip_calc_hash2(ClipMachine * mp, int rev)
{
	ClipVar *vp = mp->fp->sp - 1, *vvp;
	long hash = -1, hash2 = -1;

	vvp = _clip_vptr(vp);

	if (vvp->t.type == NUMERIC_t)
	{
		if (vvp->t.memo)
			hash = (long) rational_toDouble(vvp->r.r);
		else
			hash = (long) vvp->n.d;
	}
	else if (vvp->t.type == CHARACTER_t)
	{
		char *s = vvp->s.str.buf;
		int l = vvp->s.str.len;
		char *e;

		e = strstr(s, "->");
		if (e)
		{
			hash = _clip_casehashword(e + 2, strlen(e + 2));
			hash2 = _clip_casehashword(s, e - s);
		}
		else
			hash = _clip_casehashword(s, l);
	}

	_clip_destroy(mp, vp);
	vp->t.flags = F_NONE;
	vp->t.type = NUMERIC_t;
	vp->t.len = 10;
	vp->t.dec = 0;
	vp->t.memo = 0;
	vp->n.d = (rev ? hash2 : hash);

	vp = mp->fp->sp;
	++(mp->fp->sp);
	vp->t.flags = F_NONE;
	vp->t.type = NUMERIC_t;
	vp->t.len = 10;
	vp->t.dec = 0;
	vp->t.memo = 0;
	vp->n.d = (rev ? hash : hash2);

	CLIP_CHECK_STACK;
	return 0;
}

CLIP_DLLEXPORT void
_clip_math_operation(ClipMachine * mp, int op, ClipVar * Lval, ClipVar * Rval)
{
	rational *tmp1 = NULL, *tmp2 = NULL;
	double d1, d2;
	int ldec = Lval->t.dec;
	int rdec = Rval->t.dec;
	int llen = Lval->t.len;
	int rlen = Rval->t.len;

	if (mp->flags & RATIONAL_FLAG)
	{
		if (!Lval->t.memo)
			tmp1 = rational_double_init(Lval->n.d);
		else
			tmp1 = Lval->r.r;
		if (!Rval->t.memo)
			tmp2 = rational_double_init(Rval->n.d);
		else
			tmp2 = Rval->r.r;
		switch (op)
		{
		case '-':
			rational_suba(tmp1, tmp2);
			break;
		case '+':
			rational_adda(tmp1, tmp2);
			break;
		case '/':
			rational_diva(tmp1, tmp2);
			break;
		case '*':
			rational_mula(tmp1, tmp2);
			break;

		}
		if (!Lval->t.memo)
		{
			Lval->t.memo = 1;
			Lval->r.r = tmp1;
		}
		if (!Rval->t.memo)
			rational_destroy(tmp2);
	}
	else
	{
		if (Lval->t.memo)
			d1 = rational_toDouble(Lval->r.r);
		else
			d1 = Lval->n.d;
		if (Rval->t.memo)
			d2 = rational_toDouble(Rval->r.r);
		else
			d2 = Rval->n.d;
		switch (op)
		{
		case '-':
			d1 -= d2;
			break;
		case '+':
			d1 += d2;
			break;
		case '/':
			if (d2 == 0.0)
				d1 = 0.0;
			else
				d1 /= d2;
			break;
		case '*':
			d1 *= d2;
			break;
		}
		if (Lval->t.memo)
			rational_destroy(Lval->r.r);
		Lval->t.memo = 0;
		Lval->n.d = d1;
	}
	switch (op)
	{
	case '-':
		llen = (llen < rlen ? rlen : llen);
		ldec = (ldec < rdec ? rdec : ldec);
		break;
	case '+':
		llen = (llen < rlen ? rlen : llen);
		ldec = (ldec < rdec ? rdec : ldec);
		break;
	case '/':
		if ( ldec == 0 )
		{
			ldec += mp->decimals;
			llen += mp->decimals+1;
		}
		break;
	case '*':
		llen += Rval->t.len;
		ldec += Rval->t.dec;
		break;
	}
	if (ldec > 5)
		ldec = 5;
	if (mp->flags & FIXED_FLAG)
		ldec = mp->decimals;
	if (ldec > 0 && llen <= 10+ldec )
		llen = 11+ldec;
	if (llen > 20)
		llen = 20;
	Lval->t.len = llen;
	Lval->t.dec = ldec;
	return;
}

CLIP_DLLEXPORT int
_clip_forstep(ClipMachine * mp, int *_if)
{
	double d_step, d_to, d_val;
	ClipVar *step = _clip_vptr(mp->fp->sp - 1);
	ClipVar *to = _clip_vptr(mp->fp->sp - 2);
	ClipVar *val = _clip_vptr(mp->fp->sp - 3);

	if (step->t.type != NUMERIC_t
		|| (to->t.type != NUMERIC_t && to->t.type != DATE_t) || (val->t.type != NUMERIC_t && val->t.type != DATE_t))
		return EG_ARG;

	d_step = _clip_double(step);
	d_to = _clip_double(to);
	d_val = _clip_double(val);

	*_if = 1;

	if (d_step > 0)
	{
		if (d_val > d_to)
			*_if = 0;
	}
	else if (d_step < 0)
	{
		if (d_val < d_to)
			*_if = 0;
	}

	_clip_pop(mp);
	_clip_pop(mp);
	_clip_pop(mp);

	return 0;
}

CLIP_DLLEXPORT int
_clip_map_first(ClipMachine * mp, int keys, int *_if)
{
	ClipVar *rp = mp->fp->sp - 1;
	ClipVar *arr = _clip_vptr(rp);
	ClipVar *vp = 0;
	long no = 0;

	*_if = 0;

	if (arr->t.type == MAP_t)
	{
		arr->m.current = 0;
		if (arr->m.count > 0)
		{
			no = arr->m.items[0].no;
			vp = &(arr->m.items[0].v);
			*_if = 1;
		}
	}
	else if (arr->t.type == ARRAY_t)
	{
		arr->a.current = 0;
		if (arr->a.count > 0)
		{
			no = 1;
			vp = arr->a.items;
			*_if = 1;
		}
	}
	else
		return EG_ARG;

	if (*_if)
	{
		_clip_destroy(mp, rp);
		if (keys)
			_clip_var_num(no, rp);
		else
			_clip_clone(mp, rp, vp);
	}
	else
		_clip_pop(mp);

	return 0;
}

CLIP_DLLEXPORT int
_clip_map_next(ClipMachine * mp, int keys, int *_if)
{
	ClipVar *rp = mp->fp->sp - 1;
	ClipVar *arr = _clip_vptr(rp);
	ClipVar *vp = 0;
	long no = 0;

	*_if = 0;

	if (arr->t.type == MAP_t)
	{
		arr->m.current += 1;
		if (arr->m.current < arr->m.count)
		{
			no = arr->m.items[arr->m.current].no;
			vp = &(arr->m.items[arr->m.current].v);
			*_if = 1;
		}
	}
	else if (arr->t.type == ARRAY_t)
	{
		arr->a.current += 1;
		if (arr->a.current < arr->a.count)
		{
			no = arr->a.current + 1;
			vp = arr->a.items + arr->a.current;
			*_if = 1;
		}
	}
	else
		return EG_ARG;

	if (*_if)
	{
		_clip_destroy(mp, rp);
		if (keys)
			_clip_var_num(no, rp);
		else
			_clip_clone(mp, rp, vp);
	}
	else
		_clip_pop(mp);

	return 0;
}

CLIP_DLLEXPORT void
_clip_swap(ClipMachine * mp, int offset)
{
	ClipVar v;

	v = mp->fp->sp[-1];
	memmove(mp->fp->sp - offset, mp->fp->sp - offset - 1, offset * sizeof(ClipVar));
	mp->fp->sp[-offset - 1] = v;
}

CLIP_DLLEXPORT void
_clip_push_locale(ClipMachine * mp)
{
	ClipVar *vp = _clip_vptr(mp->fp->sp - 1);
	char *s = 0;

	if (vp->t.type == CHARACTER_t)
	{
		char *e, *p, *m;
		int i;

		p = vp->s.str.buf;
		for (i = 0, e = p; *e && *e != 1; e++, i++)
			;
		e++;
		m = malloc(i + 1);
		memcpy(m, p, i);
		m[i] = 0;

		_clip_locale_msg(m, e, &s);
#if 0
		for (e = s; *e; e++)
			*e = _clip_intbl[*(unsigned char *) e];
#endif
		free(m);
	}
	else
		s = strdup("");

	vp = mp->fp->sp - 1;
	_clip_destroy(mp, vp);
	_clip_var_str(s, strlen(s), vp);
	free(s);
}

static ClipVar *
local_ref(ClipMachine * mp, int no)
{
	ClipVar *vp;
	ClipVarFrame *lp = mp->fp->localvars;

	if (lp && no < lp->size)
		vp = lp->vars + no;
	else
	{
		int nlocals = no + 1;
		ClipVarFrame *localvars = realloc(lp, sizeof(ClipVarFrame) + nlocals * sizeof(ClipVar));

		localvars->vars = (ClipVar *) (localvars + 1);
		if (lp)
		{
			memset(localvars->vars + no, 0, (nlocals - localvars->size) * sizeof(ClipVar));
		}
		else
		{
			memset(localvars->vars, 0, nlocals * sizeof(ClipVar));
			localvars->refcount = 1;
		}
		localvars->size = nlocals;
		mp->fp->localvars = localvars;

		vp = localvars->vars + no;
	}

	if (vp->t.flags & F_MPTR && vp->p.vp->t.flags & F_MPTR)
	{
#if 1
		if (vp->p.vp->t.flags == F_MPTR &&
			(vp->p.vp->t.type == ARRAY_t || vp->p.vp->t.type == MAP_t) )
		{
		}
		else
#endif
			vp = vp->p.vp;
	}

	return vp;
}

CLIP_DLLEXPORT int
_clip_push_local(ClipMachine * mp, int no)
{
	return _clip_push(mp, local_ref(mp, no));
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_local(ClipMachine * mp, int no)
{
	return local_ref(mp, no);
}

CLIP_DLLEXPORT ClipVar *
_clip_pop_ref(ClipMachine * mp)
{
	return mp->fp->sp->p.vp;
}

CLIP_DLLEXPORT int
_clip_unref(ClipMachine * mp, ClipVar * lval)
{
	_clip_destroy(mp, lval);
	return 0;
}

CLIP_DLLEXPORT int
_clip_refmacro(ClipMachine * mp)
{
	VarEntry *vep;
	long fhash = 0, ahash;
	char *fname = 0, *aname = 0;
	int fl = 0, al = 0, r;
	ClipVar *lval = 0;
	ClipVar *sp = mp->fp->sp - 1;
	ClipVar *vp = _clip_vptr(sp);
	long *dim = 0;
	int ndim = 0;

	if (vp->t.type != CHARACTER_t)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "macro reference with non-character argument (%s)",
				  _clip_typename(vp));
		return 0;
	}

	r = _clip_parse_name(mp,vp->s.str.buf, vp->s.str.len, &aname, &al, &fname, &fl, &dim, &ndim);

	if (aname)
		ahash = _clip_casehashbytes(0, aname, al);
	else
		ahash = -1;

	if (fname)
		fhash = _clip_casehashbytes(0, fname, fl);

	_clip_destroy(mp, sp);
	mp->fp->sp--;

	if (r == 2)		/* field */
	{
		return _clip_field(mp, fhash, ahash);
	}
	else if (r == 1)	/* memvar */
	{
	}
	else
		/* field/memvar */
	{
		if (!_clip_try_field(mp, fhash))
			return 0;
	}

	vep = fetch_var(mp, fhash);

	if (vep)
		lval = &vep->var;

	if (!lval)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "no variable name: '%.*s'", vp->s.str.len, vp->s.str.buf);
		return _clip_call_errblock(mp, 1);
	}
	else if (r == 3)
	{
		lval = _clip_aref(mp, lval, ndim, dim);
		free(dim);
	}

	_clip_ref(mp, lval, 0);

	return 0;
}

static void
dup_ref(ClipVar * dest, ClipVar * src)
{
	if (dest == src || src->p.vp == dest)
		return;
	CLEAR_CLIPVAR(dest);
	dest->t.type = src->t.type;
	dest->t.flags = src->t.flags;
	dest->p.vp = src->p.vp;
	dest->t.field = src->t.field;
	dest->p.fp = src->p.fp;
	++(src->p.vp->t.count);
}

CLIP_DLLEXPORT ClipFieldDef *
_clip_get_fielddef(ClipMachine * mp, long areahash, long namehash)
{
	long hash;
	ClipFieldDef *fp;

	hash = _clip_hashbytes(0, (char *) &areahash, sizeof(areahash));
	hash = _clip_hashbytes(hash, (char *) &namehash, sizeof(namehash));

	fp = (ClipFieldDef *) HashTable_fetch(mp->fields, hash);
	if (!fp)
	{
		fp = calloc(1, sizeof(ClipFieldDef));
		fp->areahash = areahash;
		fp->fieldhash = namehash;
		HashTable_insert(mp->fields, fp, hash);
	}

	return fp;
}

static void
choose_quot(char *str, int l, char *lquot, char *rquot)
{
	char *s;
	int ch, n = 0;

	do
	{
		int lq = *lquot;
		int rq = *rquot;

		for (s = str; (ch = *s); ++s)
			if (ch == lq || ch == rq)
				break;
		if (!ch)
			return;
		switch (*lquot)
		{
		case '"':
			*lquot = '\'';
			*rquot = '\'';
			break;
		case '\'':
		case '[':
			*lquot = '"';
			*rquot = '"';
			break;
		}
	}
	while (n++ < 3);
}

CLIP_DLLEXPORT int
_clip_catstr(ClipMachine * mp, int num)
{
	int i;
	char *s = 0, buf[128], *s1;
	int l = 0, l1, len, dec;
	ClipVar *rp;

	for (i = 0; i < num; i++)
	{
		ClipVar *vp = _clip_vptr(mp->fp->sp - num + i);

		switch (vp->t.type)
		{
		default:
			snprintf(buf, sizeof(buf), "NIL");
			s1 = buf;
			l1 = 3;
			break;
		case CHARACTER_t:
			l1 = vp->s.str.len;
			s1 = vp->s.str.buf;
			break;
		case NUMERIC_t:
			len = vp->t.len;
			dec = vp->t.dec;
			if (!len)
			{
				len = 10;
				dec = 2;
			}
			if (!vp->t.memo)
			{
				int c;

				snprintf(buf, sizeof(buf), "%*.*f", len, dec, vp->n.d);
				for (s1 = buf + strlen(buf); s1 > buf; --s1)
					if ((*s1) == ',')
						(*s1) = '.';
				for (s1 = buf; (c = *s1) && c == ' '; s1++)
					;
				l1 = strlen(s1);
			}
			else
			{
				if (len)
					s1 = rational_toString(vp->r.r, 10, dec, 0);
				else
				{
					s1 = rational_toString(vp->r.r, len, dec, 0);
					snprintf(buf, sizeof(buf), "%s", s1);
					free(s1);
				}
				l1 = strlen(buf);
			}
			break;
		}

		{
			s = realloc(s, l + l1 + 1);
			memcpy(s + l, s1, l1);
			s[l + l1] = 0;
			l += l1;
		}

		_clip_destroy(mp, mp->fp->sp - num + i);
	}

	mp->fp->sp -= num;
	rp = mp->fp->sp;

	rp->s.str.buf = s;
	rp->s.str.len = l;
	rp->t.type = CHARACTER_t;
	rp->t.flags = F_NONE;
	rp->t.memo = 0;

	mp->fp->sp++;
	CLIP_CHECK_STACK;

	return 0;
}

CLIP_DLLEXPORT int
_clip_quot(ClipMachine * mp)
{
	ClipVar *vp = mp->fp->sp - 1;
	ClipVar *rp = _clip_vptr(vp);
	char *s;
	int l;

	if (rp->t.type == CHARACTER_t)
	{
		l = rp->s.str.len;
		s = calloc(1, l + 3);
		memcpy(s + 1, rp->s.str.buf, l);
		s[0] = '"';
		s[l + 1] = '"';
		choose_quot(s, l + 2, s, s + l + 1);
		s[l + 2] = 0;
		l += 2;
	}
	else if (rp->t.type == DATE_t)
	{
		int dd, mm, yy, ww;

		_clip_cdate(rp->d.julian, &dd, &mm, &yy, &ww);
		s = malloc(17);
		snprintf(s, 17, "STOD(\"%04d%02d%02d\")", yy, mm, dd);
		l = 16;
	}
	else if (rp->t.type == DATETIME_t)
	{
		s = _clip_ttoc(mp, rp->dt.julian, rp->dt.time, &l, mp->date_format, mp->hours, mp->seconds);
	}
	else if (rp->t.type == LOGICAL_t)
	{
		s = malloc(4);
		snprintf(s, 4, "%s", rp->l.val ? ".T." : ".F.");
		l = 3;
	}
	else
		return 0;

	_clip_destroy(mp, vp);

	vp->s.str.buf = s;
	vp->s.str.len = l;
	vp->t.type = CHARACTER_t;
	vp->t.flags = F_NONE;
	vp->t.memo = 0;

	return 0;
}

CLIP_DLLEXPORT int
_clip_strcmp(const char *str1, const char *str2)
{
	int l1 = strlen(str1);
	int l2 = strlen(str2);

	return _clip_strnncmp(str1, str2, l1, l2);
}

CLIP_DLLEXPORT int
_clip_strncmp(const char *str1, const char *str2, int len)
{
	int l1 = strlen(str1);
	int l2 = strlen(str2);

	if (l1 > len)
		l1 = len;
	if (l2 > len)
		l2 = len;
	return _clip_strnncmp(str1, str2, l1, l2);
}

CLIP_DLLEXPORT int
_clip_strnncmp(const char *str1, const char *str2, int len1, int len2)
{
	unsigned char ch1 = 0, ch2 = 0;
	const unsigned char *us1, *us2;

	for (us1 = (const unsigned char *)str1, us2 = (const unsigned char *)str2; len1 && len2; us1++, us2++, len1--, len2--)
	{
		ch1 = *(unsigned char *)us1;
		ch2 = *(unsigned char *)us2;
		if (ch1 < 32 && ch2 > 31)
			ch1 = 0;

		if (ch1 > 31 && ch2 < 32)
			ch2 = 0;

		ch1 = _clip_cmptbl[ch1];
		ch2 = _clip_cmptbl[ch2];

		if (ch1 < ch2)
			return -(ch2 - ch1);
		else if (ch1 > ch2)
			return ch1 - ch2;
	}

	if (len1)
		return ch1;
	else if (len2)
		return -ch2;
	return 0;
}

CLIP_DLLEXPORT int
_clip_strcasecmp(const char *str1, const char *str2)
{
	int l1 = strlen(str1);
	int l2 = strlen(str2);

	return _clip_strnncasecmp(str1, str2, l1, l2);
}

CLIP_DLLEXPORT int
_clip_strncasecmp(const char *str1, const char *str2, int len)
{
	int l1 = strlen(str1);
	int l2 = strlen(str2);

	if (l1 > len)
		l1 = len;
	if (l2 > len)
		l2 = len;
	return _clip_strnncasecmp(str1, str2, l1, l2);
}

CLIP_DLLEXPORT int
_clip_strnncasecmp(const char *str1, const char *str2, int len1, int len2)
{
	unsigned char ch1 = 0, ch2 = 0;
	const unsigned char *us1, *us2;

	for (us1 = (const unsigned char *)str1, us2 = (const unsigned char *)str2; len1 && len2; us1++, us2++, len1--, len2--)
	{
		ch1 = _clip_uptbl[*us1];
		ch2 = _clip_uptbl[*us2];
		if (ch1 < 32 && ch2 > 31)
			ch1 = 0;

		if (ch1 > 31 && ch2 < 32)
			ch2 = 0;

		ch1 = _clip_cmptbl[ch1];
		ch2 = _clip_cmptbl[ch2];

		if (ch1 < ch2)
			return -(ch2 - ch1);
		else if (ch1 > ch2)
			return ch1 - ch2;
	}

	if (len1)
		return ch1;
	else if (len2)
		return -ch2;
	return 0;
}

void
_clip_unix_path(char *p, int doslike)
{
#if 1
	int i,j,len=strlen(p);
	char * buf = p;
	for (i=0,j=0; i<len; i++)
	{
		if (doslike && buf[i] == ' ')
			continue;
		if (buf[i] == '\\')
			buf[j] = '/';
		if (doslike)
			buf[j] = tolower((unsigned char) (buf[i]));
		j ++;
	}
	buf[j] = 0;
#else
	while (*p)
	{
		if (*p == '\\')
			*p = '/';
		else if (tolow)
			*p = tolower(*p);
		p++;
	}
#endif
}

int
_clip_absolute_path(ClipMachine * cm, const char *path, char *p, int len)
{
	char op[PATH_MAX];
	char *e = p;

#ifndef _WIN32
	const char *s = path;

	p[0] = 0;
	if (path[1] == ':')
	{
		char dn[3];
		char *root;

		dn[0] = toupper(*s);
		dn[1] = ':';
		dn[2] = 0;
		root = _clip_fetch_item(cm, _clip_hashstr(dn));
		if (!root)
			return 0;
		strncpy(p, root, len - 1);
		e = p + strlen(p);
		if (*(e - 1) != '/')
		{
			*e = '/';
			e++;
			*e = 0;
		}
		s += 2;
		if (path[2] == '\\' || path[2] == '/')
			s++;
	}
	strncpy(e, s, p + len - e - 2);
#else
	if ((memcmp(path, "\\\\", 2) == 0) || (memcmp(path, "//", 2) == 0))
	{
		/* network resource */
		strncpy(p, path, len);
	}
	else
	{
		if (path != NULL && (*path == '/' || *path == '\\') && memcmp(path, "/cygdrive/", 10))
		{

			/* add default drive */
			char *s = _clip_fetch_item(cm, CLIP_CUR_DRIVE);

			snprintf(p, len - 1, "%s%s", s, path);
		}
		else
			strncpy(p, path, len);
	}
#endif
	_clip_unix_path(p, cm->flags & TRANSLATE_FLAG);

	errno = 0;
	getcwd(op, sizeof(op));
	if (errno)
		return 1;
	chdir(p);
/*	if (errno)
		return 1;*/
	errno = 0;
	getcwd(p, len);
	if (errno)
		return 1;
	chdir(op);
	if (errno)
		return 1;

	e = p + strlen(p) - 1;
	if (*e != '/' && e - p < len - 2)
	{
		*(e + 1) = '/';
		*(e + 2) = 0;
	}

	return 0;
}

CLIP_DLLEXPORT int
_clip_path(ClipMachine * cm, const char *fn, char *p, int len, int create)
{
	char path[PATH_MAX];
	const char *s, *s1 = NULL, *s2 = NULL;

	if (strchr(fn, '|'))
	{
		strncpy(p, fn, len);
		p[len - 1] = 0;
		return 0;
	}

	if ((memcmp(fn, "\\\\", 2) == 0) || (memcmp(fn, "//", 2) == 0))
	{
		strncpy(p, fn, len);
	}
	else if ((s = strrchr(fn, '/')) || (s1 = strrchr(fn, '\\')) || (s2 = strrchr(fn, ':')))
	{
		char path[PATH_MAX];
		int f = 0;

		if (!s1)
			s1 = strrchr(fn, '\\');
		if (!s2)
			s2 = strrchr(fn, ':');
		s = (s > s1) ? s : s1;
		if (s2 > s)
			f = 1;
		s = (s > s2) ? s : s2;
		memcpy(path, fn, s - fn + f);
		path[s - fn + f] = 0;
		if (_clip_absolute_path(cm, path, p, len))
			return 1;
		strncat(p, s + 1, len - strlen(p) - 1);
		_clip_unix_path(p, cm->flags & TRANSLATE_FLAG);
	}
	else
	{
		if (_clip_absolute_path(cm, cm->defaul, p, len))
			return 1;
		strncat(p, fn, len - strlen(p) - 1);
		_clip_unix_path(p, cm->flags & TRANSLATE_FLAG);
		if (!create && access(p, F_OK))
		{
			char *c, *c1 = path;

			strncpy(path, cm->path, sizeof(path) - 1);
			while (c1 != (char *) 1 && access(p, F_OK))
			{
				c = strchr(c1, ';');
				if (c)
				{
					*c = 0;
				}
				else
				{
					c = strchr(c1, ',');
					if (c)
						*c = 0;
				}
				if (_clip_absolute_path(cm, c1, p, len))
					return 1;
				strncat(p, fn, len - strlen(p) - 1);
				_clip_unix_path(p, cm->flags & TRANSLATE_FLAG);
				c1 = c + 1;
			}
		}
	}
	return 0;
}

typedef struct
{
	char *src;
	char *dst;
	unsigned char tbl[256];
	unsigned char *pg;	/*[128] */
}
TranslateTable;

static int
cmp_TranslateTable(void *p1, void *p2)
{
	TranslateTable *tp1 = (TranslateTable *) p1;
	TranslateTable *tp2 = (TranslateTable *) p2;
	int r;

	r = strcasecmp(tp1->src, tp2->src);
	if (r)
		return r;

	return strcasecmp(tp1->dst, tp2->dst);
}

static void
delete_TranslateTable(void *p)
{
	TranslateTable *tp = (TranslateTable *) p;

	if (!tp)
		return;

	free(tp->src);
	free(tp->dst);
	if (tp->pg)
		free(tp->pg);

	free(tp);
}

static Coll *tr_charsets = 0;

static unsigned char
pg2pgch(unsigned char ch)
{
	switch (ch)
	{
	case PG_HLINE:
		return PGCH_HLINE;
	case PG_LLCORNER:
		return PGCH_LLCORNER;
	case PG_LRCORNER:
		return PGCH_LRCORNER;
	case PG_TTEE:
		return PGCH_TTEE;
	case PG_RTEE:
		return PGCH_RTEE;
	case PG_LTEE:
		return PGCH_LTEE;
	case PG_BTEE:
		return PGCH_BTEE;
	case PG_ULCORNER:
		return PGCH_ULCORNER;
	case PG_URCORNER:
		return PGCH_URCORNER;
	case PG_VLINE:
		return PGCH_VLINE;

	case PG_HLINE2:
		return PGCH_HLINE2;
	case PG_LLCORNER2:
		return PGCH_LLCORNER2;
	case PG_LRCORNER2:
		return PGCH_LRCORNER2;
	case PG_TTEE2:
		return PGCH_TTEE2;
	case PG_RTEE2:
		return PGCH_RTEE2;
	case PG_LTEE2:
		return PGCH_LTEE2;
	case PG_BTEE2:
		return PGCH_BTEE2;
	case PG_ULCORNER2:
		return PGCH_ULCORNER2;
	case PG_URCORNER2:
		return PGCH_URCORNER2;
	case PG_VLINE2:
		return PGCH_VLINE2;

	case PG_PLUS:
		return PGCH_PLUS;
	case PG_PLUS2:
		return PGCH_PLUS2;
	case PG_BLOCK:
		return PGCH_BLOCK;

	case PG_LARROW:
		return PGCH_LARROW;
	case PG_RARROW:
		return PGCH_RARROW;

	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_translate_charset(char *p1, char *p2, unsigned char *str, unsigned char *ostr, int len)
{
	cons_CharsetEntry *cs1 = 0, *cs2 = 0;
	int len1 = 0, len2 = 0;
	int ind;
	TranslateTable stbl, *tp = 0;

	if (!p1 || !p2 || !str || !ostr)
		return EG_ARG;

	if (!strcasecmp(p1, p2))
	{
		memcpy(ostr, str, len);
		return 0;
	}

	stbl.src = p1;
	stbl.dst = p2;

	if (tr_charsets && search_Coll(tr_charsets, &stbl, &ind))
	{
		tp = (TranslateTable *) tr_charsets->items[ind];
		goto translate;
	}

	if (load_charset_name(p1, &cs1, &len1))
	{
		_clip_logg(2, "translate_charset: cannot load charset file '%s': %s", p1, strerror(errno));
	}

	if (load_charset_name(p2, &cs2, &len2))
	{
		_clip_logg(2, "translate_charset: cannot load charset file '%s': %s", p2, strerror(errno));
	}

	if (cs1 && cs2)
	{
		int src_pg, dst_pg;

		tp = calloc(1, sizeof(TranslateTable));
		tp->src = strdup(p1);
		tp->dst = strdup(p2);
		make_translation(cs1, len1, cs2, len2, tp->tbl);

		src_pg = !charset_have_pg(cs1, len1);
		dst_pg = !charset_have_pg(cs2, len2);
		if (src_pg && !dst_pg)
		{
			unsigned char *pg;

			pg = malloc(256);
			load_charset_tables(p1, 0, 0, 0, 0, pg);

			memmove(pg, pg + 128, 128);
			tp->pg = realloc(pg, 128);
		}

		if (!tr_charsets)
			tr_charsets = new_Coll(delete_TranslateTable, cmp_TranslateTable);
		insert_Coll(tr_charsets, tp);
	}

	free(cs1);
	free(cs2);

	  translate:
	if (tp)
	{
		int i;
		int s, s1;

		for (i = 0; i < len; i++)
		{
			s = str[i];
			if ((s & 0x80) && tp->pg && (s1 = tp->pg[s & 0x7f]))
				ostr[i] = pg2pgch(s1);
			else
				ostr[i] = tp->tbl[s];
		}
	}
	else
	{
		memcpy(ostr, str, len);
	}

	return 0;
}

/*
*	ALTD(0) - disable debugging
*	ALTD(1) - enable debugging
*	ALTD(2) - enable debugging and wait connection
*/

int
clip_ALTD(ClipMachine * mp)
{
	if (!_clip_debuglevel)
	{
		_clip_debuglevel = 1;
#ifndef OS_MINGW
		signal(SIGUSR1, _clip_sigdebug);
#endif
	}

	_clip_in_breakpoint();
	return 0;
}

void
_clip_check_stack(ClipMachine * mp)
{
	ClipFrame *fp = mp->fp;

	if (fp && fp->sp > (fp->stack + fp->stklen))
	{
		_clip_call_errblock(mp, EG_MEM);
		_clip_logg(0, "stack check failed at line %d file %d", fp->line, fp->filename ? fp->filename : "unknown");
		exit(1);
	}
}

CLIP_DLLEXPORT char *
_clip_host_charset()
{
	return _clip_hostcs;
}

int
clip_ERRORLEVEL(ClipMachine * mp)
{
	int level = errorlevel;

	if (mp->argc > 0)
		errorlevel = _clip_parni(mp, 1);

	_clip_retni(mp, level);
	return 0;
}


int
clip___QUIT(ClipMachine * mp)
{
#ifdef USE_TASKS
/*	Task_killAll();*/
#endif
	exit(errorlevel);
}

