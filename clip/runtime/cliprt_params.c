/* [ param */

CLIP_DLLEXPORT void
_clip_param(ClipMachine * mp, int no, int lno)
{
	ClipVar *param = _clip_ref_local(mp, lno);

	_clip_destroy(mp, param);

	if (!param)
		return;
	if (no < mp->argc)
	{
		ClipVar *vp = mp->bp - (mp->argc - no);

		if (vp->t.flags == F_MREF)
		{
			_clip_destroy(mp, param);
			*param = *vp;
			vp->p.vp->t.count++;
		}
		else
			_clip_mclone(mp, param, vp);
		if (param->t.flags & F_MPTR && param->t.field)
		{
			param->t.field = 0;
			param->p.fp = 0;
		}
	}
	else
		_clip_destroy(mp, param);
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_param(ClipMachine * mp, int no, ClipVar * params)
{
	if (no >= mp->argc)
		return params + no;
	else
		return mp->bp - (mp->argc - no);
}

/* ] param */
/* [ memvars */

CLIP_DLLEXPORT void
_clip_memvar_public(ClipMachine * mp, long hash)
{
	VAR(VarEntry, vp, HashTable_fetch(mp->publics, hash));
	if (!vp)
	{
		VarEntry *ep = new_VarEntry(hash);

		ep->var.t.type = LOGICAL_t;
		HashTable_insert(mp->publics, ep, hash);
	}
}

CLIP_DLLEXPORT void
_clip_add_private(ClipMachine * mp, long hash)
{
	add_private(mp, hash);
}

CLIP_DLLEXPORT void
_clip_memvar_private(ClipMachine * mp, long hash)
{
	add_private(mp, hash);
}

CLIP_DLLEXPORT int
_clip_is_public(ClipMachine * mp, long hash)
{
	VAR(VarEntry, vp, HashTable_fetch(mp->publics, hash));

	return vp ? 0 : -1;
}

CLIP_DLLEXPORT int
_clip_is_private(ClipMachine * mp, long hash)
{
	VAR(VarEntry, vp, HashTable_fetch(mp->privates, hash));

	return vp ? 0 : -1;
}

CLIP_DLLEXPORT int
_clip_is_memvar(ClipMachine * mp, long hash)
{
	return (!_clip_is_private(mp, hash) || !_clip_is_public(mp, hash)) ? 0 : 1;
}

CLIP_DLLEXPORT struct _HashTable *
_clip_space(ClipMachine * mp, long space)
{
	VAR(HashTable, hp, HashTable_fetch(mp->spaces, space));
	if (!hp)
	{
		hp = new_HashTable();
		HashTable_insert(mp->spaces, hp, space);
	}
	return hp;
}

CLIP_DLLEXPORT ClipVar *
_clip_memvar_space(ClipMachine * mp, struct _HashTable * hp, long hash, int newvar)
{
	VarEntry *vp = (VarEntry *) HashTable_fetch(hp, hash);

	if (!vp)
	{
		if (!newvar)
			return 0;
		vp = new_VarEntry(hash);
		HashTable_insert(hp, vp, hash);
	}

	return &(vp->var);
}

CLIP_DLLEXPORT void
_clip_memvar_param(ClipMachine * mp, long hash, int no)
{
	ClipVar *cp = 0;
	VarEntry *vp;

	if (no >= 0 && no < mp->argc)
		cp = mp->bp - (mp->argc - no);

	vp = add_private(mp, hash);
	if (cp)
	{
		_clip_destroy(mp, &vp->var);
		vp->var = *cp;
		if ((vp->var.t.flags & F_MPTR) && vp->var.t.field)
		{
			vp->var.t.field = 0;
			vp->var.p.fp = 0;
		}
		CLEAR_CLIPVAR(cp);
	}
}

static void
remove_private_vect(ClipMachine * mp, int num, void *vect)
{
	long *lp = (long *) vect;

	for (; num > 0; --num, ++lp)
	{
		long hash = GETLONG(lp);
		VarEntry *vp = (VarEntry *) HashTable_fetch(mp->privates, hash);

		if (!vp)
			continue;
		if (vp->next)
		{
			HashTable_remove(mp->privates, hash);
			HashTable_store(mp->privates, vp->next, hash);
			vp->next = 0;
		}
		else
		{
			HashTable_remove(mp->privates, hash);
		}
		delete_VarEntry(mp, vp);
	}
}

CLIP_DLLEXPORT void
_clip_remove_privates(ClipMachine * mp, int num, ... /*long hash */ )
{
	va_list ap;

	va_start(ap, num);
	remove_private_vect(mp, num, ap);
	va_end(ap);
}

CLIP_DLLEXPORT void
_clip_vremove_privates(ClipMachine * mp, int num, long *vect)
{
	remove_private_vect(mp, num, vect);
}

static VarEntry *
add_private(ClipMachine * mp, long hash)
{
	long *p;
	VarEntry *vp, *np;
	ClipFrame *fp;

	if (mp->inMacro)
		fp = mp->inMacro;
	else
		fp = mp->fp;

	if (fp)
	{
		p = fp->privates;
		if (p)
		{
			int n;
			long c;

			for (n = *p, p++; n >= 0; n--, p++)
				if (hash == GETLONG(p))
				{
					vp = (VarEntry *) HashTable_fetch(mp->privates, hash);
					if (vp)
						return vp;
				}
			p = fp->privates;
			c = GETLONG(p);
			p = realloc(p, (c + 2) * sizeof(long));

			SETLONG(p, c + 1);
			SETLONG(p + c + 1, hash);
			fp->privates = p;
		}
		else
		{
			p = fp->privates = malloc(sizeof(long) * 2);

			SETLONG(p, 1);
			SETLONG(p + 1, hash);
		}
	}

	vp = (VarEntry *) HashTable_fetch(mp->privates, hash);
	np = new_VarEntry(hash);
	if (!vp)
		HashTable_insert(mp->privates, np, hash);
	else
	{
		np->next = vp;
		HashTable_remove(mp->privates, hash);
		HashTable_store(mp->privates, np, hash);
	}
	return np;
}

static VarEntry *
fetch_var(ClipMachine * mp, long hash)
{
	VarEntry *vp = (VarEntry *) HashTable_fetch(mp->privates, hash);

	if (!vp)
		vp = (VarEntry *) HashTable_fetch(mp->publics, hash);
	return vp;
}

static ClipVar *
fetch_obj(ClipMachine * mp, ClipVar * obj, long hash)
{
	if (obj->t.type != MAP_t)
		return 0;
	return _clip_mget(mp, obj, hash);
}

CLIP_DLLEXPORT int
_clip_memvar(ClipMachine * mp, long hash)
{
	VarEntry *vp;

	vp = fetch_var(mp, hash);

	if (!vp)
	{
#if 0
		vp = add_private(mp, hash);
#else
		char buf[64];
		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "no memvar variable '%s'", buf);
		return _clip_call_errblock(mp, EG_NOVAR);
#endif
	}
	_clip_push(mp, &vp->var);
	return 0;
}

CLIP_DLLEXPORT int
_clip_fmemvar(ClipMachine * mp, long hash)
{
	VarEntry *vp;

	if (mp->obj)
	{
		ClipVar *vpp;

		vpp = fetch_obj(mp, _clip_vptr(mp->obj), hash);
		if (vpp)
		{
			_clip_push(mp, vpp);
			return 0;
		}
	}

	if (!_clip_try_field(mp, hash))
		return 0;

	vp = fetch_var(mp, hash);

	if (!vp)
	{
		char buf[64];

		/*int r; */

		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "no variable '%s'", buf);
		/*r = _clip_trap_err(mp, EG_NOVAR, 0, 0, 0, 0, buf); */
		return _clip_call_errblock(mp, EG_NOVAR);
	}
	/*vp = add_private(mp, hash); */
	_clip_push(mp, &vp->var);
	return 0;
}

CLIP_DLLEXPORT int
_clip_memvarf(ClipMachine * mp, long hash)
{
	VarEntry *vp;

	vp = fetch_var(mp, hash);

	if (!vp)
	{
		char buf[64];

		/*int r; */

		if (!_clip_try_field(mp, hash))
			return 0;

		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "no variable '%s'", buf);
		/*r = _clip_trap_err(mp, EG_NOVAR, 0, 0, 0, 0, buf); */
		return _clip_call_errblock(mp, EG_NOVAR);
	}
	_clip_push(mp, &vp->var);
	return 0;
}

CLIP_DLLEXPORT int
_clip_ref_fmemvar(ClipMachine * mp, long hash)
{
	VarEntry *vp;

	if (!_clip_try_field(mp, hash))
		return 0;

	vp = fetch_var(mp, hash);

	if (!vp)
	{
#if 1
		vp = add_private(mp, hash);
#else
		char buf[64];

		/*int r; */

		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "no variable '%s'", buf);
		/*r = _clip_trap_err(mp, EG_NOVAR, 0, 0, 0, 0, buf); */
		return _clip_call_errblock(mp, EG_NOVAR);
#endif
	}
	_clip_ref(mp, &vp->var, 0);
	return 0;
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_memvar(ClipMachine * mp, long hash)
{
	VarEntry *vp = fetch_var(mp, hash);

	if (!vp)
	{
#if 1
		vp = add_private(mp, hash);
#else
		char buf[64];
		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "no variable '%s'", buf);
		_clip_call_errblock(mp, EG_NOVAR);
		return 0;
#endif
	}
	return &vp->var;
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_memvar_noadd(ClipMachine * mp, long hash)
{
	VarEntry *vp = fetch_var(mp, hash);

	if (!vp)
	{
#if 0
		return 0;
#else
		char buf[64];
		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "no memvar variable '%s'", buf);
		_clip_call_errblock(mp, EG_NOVAR);
		return 0;
#endif
	}
	return &vp->var;
}

CLIP_DLLEXPORT int
_clip_public(ClipMachine * mp, long hash)
{
	VarEntry *vp = (VarEntry *) HashTable_fetch(mp->publics, hash);

	if (!vp)
	{
		char buf[64];

		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "try access to unknown public var '%s'", buf);
		return _clip_call_errblock(mp, 1);
	}
	_clip_push(mp, &vp->var);
	return 0;
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_public(ClipMachine * mp, long hash)
{
	VarEntry *vp = (VarEntry *) HashTable_fetch(mp->publics, hash);

	if (!vp)
	{
		char buf[64];

		_clip_hash_name(mp, hash, buf, sizeof(buf));
		_clip_trap_printf(mp, __file__, __LINE__, "try access to unknown public var '%s'", buf);
		return 0;
	}
	return &vp->var;
}

CLIP_DLLEXPORT ClipVar *
_clip_ref_public_noadd(ClipMachine * mp, long hash)
{
	VarEntry *vp = (VarEntry *) HashTable_fetch(mp->publics, hash);

	if (!vp)
		return 0;
	else
		return &vp->var;
}

/* ] memvars */
/* [ traps */

/* retrapper - add filename&line info into trapStack */
CLIP_DLLEXPORT void
_clip_trap(ClipMachine * mp, const char *filename, int line)
{
	char *s;
	char buf[256];

	snprintf(buf, 256, "file '%s' line %d", filename, line);
#if 0
	if (!mp->noerrblock)
		_clip_logg(4, "trace: %s", buf);
#endif
	s = strdup(buf);
	add_ClipVect(&mp->trapTrace, s);
}

/* trap head of stack */
CLIP_DLLEXPORT void
_clip_trap_pop(ClipMachine * mp)
{
	ClipVar *vp = --(mp->fp->sp), *Vp = NEW(ClipVar);

	*Vp = *vp;
	mp->trapVar = Vp;
}

CLIP_DLLEXPORT void
_clip_trap_str(ClipMachine * mp, const char *filename, int line, const char *str)
{
	ClipVar *vp;

	vp = _clip_new_error(mp, _clip_gettext(str), 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	_clip_trap_var(mp, filename, line, vp);
}

CLIP_DLLEXPORT int
_clip_trap_operation(ClipMachine * mp, const char *filename, int line, const char *oper,
	ClipVar *lval)
{
	ClipVar *vp;
	int r;
	char buf[256];
	snprintf(buf,256,_clip_gettext("invalid arguments for  '%s' operation"),oper);

	vp = _clip_new_error(mp, buf, 2, 0, 0, 0, 0, 0, 0, 0, 0, oper);
	_clip_trap_var(mp, filename, line, vp);

	if (!mp->trapVar)
		_clip_generr(mp, 1);

	_clip_destroy(mp, lval);
	r = _clip_errorblock_res(mp, mp->trapVar, 1, lval);

	if (!r && _clip_type(lval) == UNDEF_t)
		r = 1;

	return r;
}

CLIP_DLLEXPORT void
_clip_trap_printf(ClipMachine * mp, const char *filename, int line, const char *fmt, ...)
{
	va_list ap;
	char buf[256];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), _clip_gettext(fmt), ap);
	va_end(ap);
	_clip_trap_str(mp, filename, line, buf);
}

CLIP_DLLEXPORT void
_clip_trap_printv(ClipMachine * mp, const char *filename, int line, const char *fmt, void *ap)
{
	char buf[256];

	vsnprintf(buf, sizeof(buf), _clip_gettext(fmt), ap);
	_clip_trap_str(mp, filename, line, buf);
}

CLIP_DLLEXPORT void
_clip_trap_var(ClipMachine * mp, const char *filename, int line, ClipVar * vp)
{
	_clip_delete(mp, mp->trapVar);
	_clear_trap(mp);
	if (vp)
	{
		/*vp = _clip_vptr(vp); */
		mp->trapVar = vp;
	}
	else
	{
		mp->trapVar = NEW(ClipVar);
	}
	_clear_trap(mp);
	_clip_trap(mp, filename, line);
}

CLIP_DLLEXPORT void
_clip_trap_invargv(ClipMachine * mp, const char *filename, int line)
{
	_clip_trap_str(mp, filename, line, "invalid arguments");
}

/* ] traps */
