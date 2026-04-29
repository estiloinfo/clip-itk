/* [ arrays */

/* create from stack */
CLIP_DLLEXPORT void
_clip_sarray(ClipMachine * mp, int n)
{
	ClipVar arr, *ap, *sp = mp->fp->sp;
	int i;

	ap = NEW(ClipVar);
	CLEAR_CLIPVAR(&arr);
	arr.t.type = ARRAY_t;
	arr.t.flags = F_MPTR/*F_MREF*/;
	arr.p.vp = ap;
	ap->t.type = ARRAY_t;
	ap->t.flags = F_NONE;
	ap->t.count = 1;
	ap->a.items = malloc(sizeof(ClipVar) * n);
	ap->a.count = n;
	for (i = 0; i < n; i++)
	{
		ClipVar *vpp = ap->a.items + i;

		*vpp = sp[-n + i];
		vpp->t.field = 0;
	}
	sp[-n] = arr;
	mp->fp->sp -= n - 1;
}

static void
new_array(ClipVar * vp, int n, long *dims)
{
	ClipVar *ap;
	int i, size;

	CLEAR_CLIPVAR(vp);
	if (!n)
	{
		vp->t.type = 0;
		vp->t.flags = F_NONE;
		return;
	}

	--n;
	size = GETLONG(dims);
	++dims;

	ap = NEW(ClipVar);
	vp->t.type = ARRAY_t;
	vp->t.flags = F_MPTR;
	vp->p.vp = ap;
	ap->t.type = ARRAY_t;
	ap->t.flags = F_NONE;
	ap->t.count = 1;
	ap->a.items = malloc(sizeof(ClipVar) * size);
	ap->a.count = size;
	for (i = 0; i < size; i++)
		new_array(&ap->a.items[i], n, dims);
}

CLIP_DLLEXPORT void
_clip_dimarray(ClipMachine * mp, int n)
{
	ClipVar *sp = mp->fp->sp;
	int i;
	long *dims;

#ifdef OS_MINGW
	dims = malloc(sizeof(long) * n);
#else
	dims = alloca(sizeof(long) * n);
#endif

	for (i = 0; i < n; i++)
		dims[i] = _clip_long(sp - n + i);

	mp->fp->sp -= n - 1;
	new_array(mp->fp->sp - 1, n, dims);
#ifdef OS_MINGW
	free(dims);
#endif
}

/* create with n dimentions */
CLIP_DLLEXPORT void
_clip_newarray(ClipMachine * mp, int n, ...)
{
	ClipVar *sp = mp->fp->sp;
	va_list ap;

	va_start(ap, n);
	new_array(sp, n, (long *) ap);
	va_end(ap);

	++(mp->fp->sp);
	CLIP_CHECK_STACK;
}

CLIP_DLLEXPORT void
_clip_vnewarray(ClipMachine * mp, int n, long *vect)
{
	ClipVar *sp = mp->fp->sp;

	new_array(sp, n, vect);
	++(mp->fp->sp);
	CLIP_CHECK_STACK;
}

CLIP_DLLEXPORT int
_clip_array(ClipMachine * mp, ClipVar * dest, int ndim, long *dims)
{
	new_array(dest, ndim, dims);
	return 0;
}

CLIP_DLLEXPORT int
_clip_adel(ClipMachine * mp, ClipVar * ap, int ndim, long *dims)
{
	int c;
	long ind;

	ap = _clip_vptr(ap);
	if (ap->t.type != ARRAY_t || !ndim)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "adel for non-array object");
		return _clip_call_errblock(mp, 1);
	}

	c = ap->a.count;
	ind = GETLONG(dims);

	if (ind < 0 || ind >= c)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "index for adel is out-of range: ind=%d, count=%d", ind + 1, c);
		return _clip_call_errblock(mp, 1);
	}

	if (ndim > 1)
	{
		ClipVar *app;

		app = ap->a.items + ind;
		return _clip_adel(mp, app, ndim - 1, dims + 1);
	}

	_clip_destroy(mp, ap->a.items + ind);
	memmove(ap->a.items + ind, ap->a.items + ind + 1, (c - ind - 1) * sizeof(ClipVar));
	memset(ap->a.items + c - 1, 0, sizeof(ClipVar));

	return 0;
}

CLIP_DLLEXPORT int
_clip_asize(ClipMachine * mp, ClipVar * ap, int ndim, long *dims)
{
	int c;
	long i, ind;

	ap = _clip_vptr(ap);

	if (ap->t.type != ARRAY_t || !ndim)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "asize for non-array object");
		return _clip_call_errblock(mp, 1);
	}

	c = ap->a.count;
	ind = GETLONG(dims);

	if (ndim > 1)
	{
		ClipVar *app;

		ind = dims[0];
		if (ind < 0 || ind >= c)
		{
			_clip_trap_printf(mp, __file__, __LINE__, "index for asize is out-of range: ind=%d, count=%d", ind + 1,
					  c);
			return _clip_call_errblock(mp, 1);
		}
		app = ap->a.items + ind;
		return _clip_asize(mp, app, ndim - 1, dims + 1);
	}

	for (i = ind; i < c; ++i)
		_clip_destroy(mp, ap->a.items + i);

	ap->a.items = realloc(ap->a.items, sizeof(ClipVar) * ind);
	ap->a.count = ind;

	for (i = c; i < ind; ++i)
		memset(ap->a.items + i, 0, sizeof(ClipVar));

	return 0;
}

CLIP_DLLEXPORT int
_clip_ains(ClipMachine * mp, ClipVar * ap, int ndim, long *dims)
{
	int c;
	long ind;

	ap = _clip_vptr(ap);

	if (ap->t.type != ARRAY_t || !ndim)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "ains for non-array object");
		return _clip_call_errblock(mp, 1);
	}

	c = ap->a.count;
	ind = GETLONG(dims);
	if (ind < 0 || ind >= c)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "index for ains is out-of range: ind=%d, count=%d", ind + 1, c);
		return _clip_call_errblock(mp, 1);
	}
	if (ndim > 1)
	{
		ClipVar *app;

		app = ap->a.items + ind;
		return _clip_ains(mp, app, ndim - 1, dims + 1);
	}

	if (c > 0)
	{
		_clip_destroy(mp, &ap->a.items[c - 1]);
		memmove(ap->a.items + ind + 1, ap->a.items + ind, (c - ind - 1) * sizeof(ClipVar));
	}
	memset(ap->a.items + ind, 0, sizeof(ClipVar));

	return 0;
}

CLIP_DLLEXPORT int
_clip_aadd(ClipMachine * mp, ClipVar * ap, ClipVar * vp)
{
	int c;

	ap = _clip_vptr(ap);

	if (ap->t.type != ARRAY_t)
		return 1;

	c = ++ap->a.count;
	ap->a.items = realloc(ap->a.items, sizeof(ClipVar) * c);
	memset(ap->a.items + c - 1, 0, sizeof(ClipVar));
	_clip_clone(mp, ap->a.items + c - 1, vp);

	return 0;
}

CLIP_DLLEXPORT ClipVar *
_clip_aref(ClipMachine * mp, ClipVar * ap, int ndim, long *dims)
{
	int c;
	long ind;
	ClipVar *app = 0;

	ap = _clip_vptr(ap);

	if ((ap->t.type != ARRAY_t && ap->t.type != MAP_t) || !ndim)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "aget for non-array object");
		return 0;
	}

	if (ap->t.type == MAP_t)
	{
		app = _clip_mget(mp, ap, dims[0]);
	}
	else
	{
		c = ap->a.count;
		ind = GETLONG(dims);

		if (ind < 0 || ind >= c)
		{
			_clip_trap_printf(mp, __file__, __LINE__, "index for aget is out-of range: ind=%d, count=%d", ind + 1,
					  c);
			return 0;
		}

		app = ap->a.items + ind;
	}

	if (ndim > 1)
	{
		return _clip_aref(mp, app, ndim - 1, dims + 1);
	}

	return app;
}

CLIP_DLLEXPORT int
_clip_aget(ClipMachine * mp, ClipVar * ap, ClipVar * resp, int ndim, long *dims)
{
	ClipVar *app;

	app = _clip_aref(mp, ap, ndim, dims);
	if (app)
		return _clip_clone(mp, resp, app);
	else
		return EG_NOVAR;
}

CLIP_DLLEXPORT int
_clip_aset(ClipMachine * mp, ClipVar * ap, ClipVar * vp, int ndim, long *dims)
{
	int c;
	long ind;
	ClipVar *app;

	ap = _clip_vptr(ap);

	if ((ap->t.type != ARRAY_t && ap->t.type != MAP_t) || !ndim)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "aset for non-array object");
		return _clip_call_errblock(mp, 1);
	}

	if (ap->t.type == MAP_t)
	{
		app = _clip_mget(mp, ap, dims[0]);
	}
	else
	{
		c = ap->a.count;
		ind = GETLONG(dims);

		if (ind < 0 || ind >= c)
		{
			_clip_trap_printf(mp, __file__, __LINE__, "index for aset is out-of range: ind=%d, count=%d", ind + 1,
					  c);
			return _clip_call_errblock(mp, 1);
		}

		app = ap->a.items + ind;
	}

	if (ndim > 1)
	{
		return _clip_aset(mp, app, vp, ndim - 1, dims + 1);
	}

/*_clip_destroy(mp, app);*/
	return _clip_mclone(mp, app, vp);
}

CLIP_DLLEXPORT void
_clip_unref_arr(ClipMachine * mp)
{
	ClipVar *sp = mp->fp->sp - 1, *ap;
	int type = _clip_type(sp);

	if ((type != ARRAY_t && type != MAP_t) || (sp->t.flags != F_MPTR && sp->t.flags != F_MREF))
		return;
	ap = _clip_vptr(sp->p.vp);

	_clip_dup(mp, sp, ap);
}

static void
print_dim(ClipMachine * mp, char *buf, int size, int dim, int Dim, long *Vect)
{
	char *s = buf, *e = buf + size - 20;
	int i;

	for (i = 0, *s = 0; i < Dim - dim && s < e; ++i)
	{
		char bb[64];

		s = s + strlen(s);
		_clip_hash_name(mp, GETLONG(Vect + i), bb, sizeof(bb));
		sprintf(s, "[%s]", bb);
	}
}

int
search_map(ClipVarEl * items, int count, long key, int *index)
{
	int l, h, res, i, c;
	long ln;

	l = 0;
	h = count - 1;
	res = 0;
	while (l <= h)
	{
		i = (l + h) >> 1;
		ln = items[i].no;
		if (ln < key)
			c = -1;
		else if (ln > key)
			c = 1;
		else
			c = 0;

		if (c < 0)
			l = i + 1;
		else
		{
			h = i - 1;
			if (c == 0)
			{
				res = 1;
				l = i;
			}
		}
	}
	if (index)
		*index = l;
	return res;
}

static ClipVarEl *
get_map_el(ClipMapVar * a, long no)
{
	int ind;

	if (search_map(a->items, a->count, no, &ind))
		return a->items + ind;
	else
		return 0;
}

CLIP_DLLEXPORT int
_clip_map(ClipMachine * mp, ClipVar * vp)
{
	ClipVar *ap;

	_clip_destroy(mp, vp);
	ap = NEW(ClipVar);
	vp->t.type = MAP_t;
	vp->t.flags = F_MPTR;
	vp->p.vp = ap;
	ap->t.type = MAP_t;
	ap->t.flags = F_NONE;
	ap->t.count = 1;
	ap->m.items = 0;
	ap->m.count = 0;

	return 0;
}

CLIP_DLLEXPORT int
_clip_mputn(ClipMachine * mp, ClipVar * ap, long no, double n)
{
	ClipVar v;
	int r;

	memset(&v, 0, sizeof(v));
	_clip_var_num(n, &v);
	r = _clip_madd(mp, ap, no, &v);
	_clip_destroy(mp, &v);

	return r;
}

CLIP_DLLEXPORT int
_clip_mputl(ClipMachine * mp, ClipVar * ap, long no, int l)
{
	ClipVar v;
	int r;

	memset(&v, 0, sizeof(v));
	_clip_var_log(l, &v);
	r = _clip_madd(mp, ap, no, &v);
	_clip_destroy(mp, &v);

	return r;
}

CLIP_DLLEXPORT int
_clip_mputc(ClipMachine * mp, ClipVar * ap, long no, char *str, int len)
{
	ClipVar v;
	int r;

	memset(&v, 0, sizeof(v));
	_clip_var_str(str, len, &v);
	r = _clip_madd(mp, ap, no, &v);
	_clip_destroy(mp, &v);

	return r;
}

CLIP_DLLEXPORT int
_clip_madd(ClipMachine * mp, ClipVar * ap, long no, ClipVar * vp)
{
	int ind, c;
	ClipVarEl *vlp;

	ap = _clip_vptr(ap);

	if (ap->t.type != MAP_t)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "madd for non-map object");
		return _clip_call_errblock(mp, 1);
	}

	c = ap->m.count;

	if (search_map(ap->m.items, c, no, &ind))
	{
		vlp = ap->m.items + ind;
		_clip_destroy(mp, &vlp->v);
	}
	else
	{
		ap->m.items = realloc(ap->m.items, (c + 1) * sizeof(ClipVarEl));
		if (ind < c)
			memmove(ap->m.items + ind + 1, ap->m.items + ind, (c - ind /* -1 */ ) * sizeof(ClipVarEl));
		ap->m.count++;
		vlp = ap->m.items + ind;
		memset(vlp, 0, sizeof(ClipVarEl));
		vlp->no = no;
	}
	_clip_mclone(mp, &vlp->v, vp);

	return 0;
}

CLIP_DLLEXPORT int
_clip_mdel(ClipMachine * mp, ClipVar * ap, long no)
{
	int ind;

	ap = _clip_vptr(ap);

	if (ap->t.type != MAP_t)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "mdel for non-map object");
		return _clip_call_errblock(mp, 1);
	}
	if (search_map(ap->m.items, ap->m.count, no, &ind))
	{
		_clip_destroy(mp, &((ap->m.items + ind)->v));

		ap->m.count--;
		if (ind < (int)ap->m.count)
			memmove(ap->m.items + ind, ap->m.items + ind + 1, (ap->m.count - ind) * sizeof(ClipVarEl));
	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_mind(ClipMachine * mp, ClipVar * ap, long no, int *ind)
{
	ap = _clip_vptr(ap);
	if (ap->t.type != MAP_t)
	{
		_clip_trap_printf(mp, __file__, __LINE__, "mind for non-map object");
		return _clip_call_errblock(mp, 1);
	}

	return !search_map(ap->m.items, ap->m.count, no, ind);
}

static int
test_maponerror(ClipMachine * mp, ClipVar * map, char * member)
{
	int ind, ret = 1;

	/* try run `onerror` member */
	if (search_map(map->m.items, map->m.count, HASH_onerror, &ind))
	{
		ClipVar *ep;

		map->t.memo = 0;
		ep = _clip_vptr(&(map->m.items[ind].v));

		if (ep->t.type == PCODE_t || ep->t.type == CCODE_t)
		{
			int i;
			ClipVar stack[3];
			ClipVar *rval = mp->fp->sp - 1;

			memset(stack, 0, sizeof(stack));

			stack[0].t = map->t;
			stack[0].t.flags = F_MPTR;
			stack[0].p.vp = map;
			map->t.count++;

			_clip_var_str(member, strlen(member), stack + 1);
			_clip_clone(mp, stack + 2, rval);

			_clip_eval(mp, _clip_vptr(ep), 3, stack, rval);

			for (i = 0; i < 3; i++)
				_clip_destroy(mp, stack + i);
			ret = 0;
		}
		map->t.memo = 1;
	}
	return ret;
}

static ClipVar *
fetch_arr(ClipMachine * mp, ClipVar * ap, int dim, long *vect, int Dim, long *Vect, int store, ClipVar ** mapp, long *hashp)
{
	long no;
	ClipVar *vp = NULL;
	ClipVarEl *vlp = 0;
	int isMap, type;
	int pdim;
	long *pvect;
	ClipVar *pap;

	if (!dim)
		return ap;

	type = _clip_type(ap);

	isMap = (type == MAP_t ? 1 : 0);

	if ( type == CHARACTER_t)
	{
		long pos, len=1, slen;
		char *buf;
		ClipVar *sp = mp->fp->sp, var;

		pos = _clip_hash(mp, sp - dim);
		pos --;
		if ( dim > 1 )
			len = _clip_hash(mp, sp - dim + 1);
		if ( len < 1 )
			len = 1;

		vp = _clip_vptr(ap);
		slen = vp->s.str.len;
		buf = vp->s.str.buf;

		CLEAR_CLIPVAR(&var);
		if ( pos<0 || pos >= slen )
		{
			pos = 0;
			len = 0;
		}
		if ( pos + len > slen)
			len = slen - pos;
		_clip_var_str(buf+pos, len, &var);
		vp = _clip_add_tempvar(mp, &var);
		_clip_destroy(mp, &var);
		return vp;
	}

	if (type != ARRAY_t && !isMap)
	{
		char buf[64];

		print_dim(mp, buf, sizeof(buf), dim - 1, Dim, Vect);
		_clip_trap_printf(mp, mp->fp->filename, mp->fp->line, "try fetch element from non-array object%s%s",
				  buf[0] ? " " : "", buf);
		_clip_call_errblock(mp, EG_BOUND);
		return NULL;
	}

	pap = ap;
	pvect = vect;
	pdim = dim;

	ap = _clip_vptr(ap);
	no = GETLONG(vect);

	++vect;
	--dim;

	if (isMap)
	{
		int ind;
		int c = ap->m.count;

		if (search_map(ap->m.items, c, no, &ind))
			vlp = ap->m.items + ind;
		else if (store)
		{
			/*addvar: */
			ap->m.items = realloc(ap->m.items, (c + 1) * sizeof(ClipVarEl));
			if (ind < c)
				memmove(ap->m.items + ind + 1, ap->m.items + ind, (c - ind /*-1*/ ) * sizeof(ClipVarEl));
			ap->m.count++;
			vlp = ap->m.items + ind;
			memset(vlp, 0, sizeof(ClipVarEl));
			vlp->no = no;
		}
		else
		{
			if ((mp->flags1 & MAPERR_FLAG))
			{
				/*goto addvar; */
				vp = mp->fp->sp;
				memset(vp, 0, sizeof(ClipVar));
				vp->p.vp = vp;
				return vp;
			}
			else
			{
				char buf[64];

				_clip_hash_name(mp, no, buf, sizeof(buf));
				if ( test_maponerror(mp,ap,buf) )
				{
					_clip_trap_printf(mp, mp->fp->filename, mp->fp->line, "map fetch error: index %s", buf);
					_clip_call_errblock(mp, EG_BOUND);
					return 0;
				}
				else
				{
					vp = fetch_arr(mp, pap, pdim, pvect, Dim, Vect, store, mapp, hashp);
					return vp;
				}
			}
		}

		if (store && !dim && ap->t.memo && mapp && hashp)
		{
			*mapp = ap;
			*hashp = no;
		}
	}
	else if (no < 1 || no > (int)ap->a.count)
	{
		char buf[64];

		print_dim(mp, buf, sizeof(buf), dim, Dim, Vect);
		_clip_trap_printf(mp, mp->fp->filename, mp->fp->line, "out-of-bounds array %s, index=%d, count=%d", buf, no,
				  ap->a.count);
		_clip_call_errblock(mp, EG_BOUND);
		return NULL;
	}
	if (isMap)
	{
		vp = fetch_arr(mp, &vlp->v, dim, vect, Dim, Vect, store, mapp, hashp);
	}
	else
		vp = fetch_arr(mp, ap->a.items + no - 1, dim, vect, Dim, Vect, store, mapp, hashp);

	return vp;
}

static int
clip_count(ClipVar *vp)
{
	int count;

	if (vp && vp->t.flags & F_MPTR)
	{
		count = vp->p.vp->t.count + clip_count(vp->p.vp);
	}
	else
		count = 0;
	return count;
}

static ClipVar *
clip_fetch(ClipMachine * mp, int dim, int push, int store, ClipVar ** mapp, long *hashp)
{
	ClipFrame *fp = mp->fp;
	ClipVar *sp = fp->sp;
	ClipVar *ap = sp - dim - 1;
	ClipVar arr;
	ClipVar *vp = NULL;
	/*ClipVar *app;*/
	int i;
#ifdef OS_MINGW
	long *vect = malloc(sizeof(long) * dim);
#else
	long *vect = alloca(sizeof(long) * dim);
#endif

	memset(&arr, 0, sizeof(ClipVar));
	_clip_clone(mp, &arr, ap);

	for (i = 0; i < dim; ++i)
	{
		long h;
		void *p;

		h = _clip_hash(mp, sp - dim + i);
		p = vect + i;

		*(long *) (p) = (h);
		/*SETLONG(vect + i, _clip_hash(mp, sp - dim + i)); */
	}


	vp = fetch_arr(mp, &arr, dim, vect, dim, vect, store, mapp, hashp);

	if (!vp)
		return 0;

	for (i = dim + 1; i; --i)
	{
		sp = --(fp->sp);
		_clip_destroy(mp, sp);
	}

	if (push)
	{
		if (!vp)
		{
			vp = mp->fp->sp++;
			CLIP_CHECK_STACK;
			memset(vp, 0, sizeof(ClipVar));
		}
		else
			_clip_push(mp, vp);
	}

	if (!vp)
	{
		_clip_trap_str(mp, mp->fp->filename, mp->fp->line, "map/array fetch error");
		_clip_call_errblock(mp, EG_BOUND);
	}

#if 0
	app = _clip_vptr(&arr);
	if (app->t.count == 1 && !push)
#else
	if (clip_count(&arr) == 1 && !push)
#endif
	{
		vp = _clip_add_tempvar(mp, vp);
	}

	_clip_destroy(mp, &arr);
#ifdef OS_MINGW
	free(vect);
#endif
	return vp;
}

CLIP_DLLEXPORT int
_clip_ifetch(ClipMachine * mp, int dim)
{
	return clip_fetch(mp, dim, 1, 0, 0, 0) ? 0 : 1;
}

CLIP_DLLEXPORT int
_clip_fetch(ClipMachine * mp, int dim)
{
	return clip_fetch(mp, dim, 0, 0, 0, 0) ? 0 : 1;
}

CLIP_DLLEXPORT ClipVar *
_clip_fetchref(ClipMachine * mp, int dim)
{
	ClipVar *vp;

	vp = clip_fetch(mp, dim, 0, 0, 0, 0);
	/*printf("%p\n",vp); */
	return vp;
}

static void
test_modify(ClipMachine * mp, ClipVar * map, long hash)
{
	int ind;

	/* try run `modify` member */
	if (search_map(map->m.items, map->m.count, HASH_modify, &ind))
	{
		ClipVar *ep;

		map->t.memo = 0;
		ep = _clip_vptr(&(map->m.items[ind].v));

		if (ep->t.type == PCODE_t || ep->t.type == CCODE_t)
		{
			int i;
			ClipVar stack[3];
			ClipVar *rval = mp->fp->sp - 1;

			memset(stack, 0, sizeof(stack));

			stack[0].t = map->t;
			stack[0].t.flags = F_MPTR;
			stack[0].p.vp = map;
			map->t.count++;

			_clip_var_num(hash, stack + 1);
			_clip_clone(mp, stack + 2, rval);

			_clip_eval(mp, _clip_vptr(ep), 3, stack, rval);

			for (i = 0; i < 3; i++)
				_clip_destroy(mp, stack + i);
		}
		map->t.memo = 1;
	}
}

CLIP_DLLEXPORT int
_clip_istore(ClipMachine * mp, int dim, int byref)
{
	ClipVar *map = 0;
	long hash = 0;
	ClipVar *vp = clip_fetch(mp, dim, 0, 1, &map, &hash);

	if (!vp)
		return 1;

	if (map)
		test_modify(mp, map, hash);

	if (byref == 2)
		_clip_destroy(mp, vp);
	else if (byref == 1)
		_clip_refassign(mp, vp, 0);
	else
		_clip_iassign(mp, vp);
	return 0;
}

CLIP_DLLEXPORT int
_clip_store(ClipMachine * mp, int dim, int byref)
{
	ClipVar *map = 0;
	long hash = 0;
	ClipVar *vp = clip_fetch(mp, dim, 0, 1, &map, &hash);

	if (!vp)
		return 1;

	if (map)
		test_modify(mp, map, hash);

	if (byref == 2)
	{
		_clip_destroy(mp, vp);
		CLEAR_CLIPVAR(mp->fp->sp);
		mp->fp->sp++;
		CLIP_CHECK_STACK;
	}
	else if (byref == 1)
		_clip_refassign(mp, vp, 1);
	else
		_clip_assign(mp, vp);
	return 0;
}

CLIP_DLLEXPORT int
_clip_opistore(ClipMachine * mp, int dim, int op)
{
	ClipVar *vp = clip_fetch(mp, dim, 0, 1, 0, 0);

	if (!vp)
		return 1;
	_clip_opiassign(mp, vp, op);
	return 0;
}

CLIP_DLLEXPORT int
_clip_opstore(ClipMachine * mp, int dim, int op)
{
	ClipVar *vp = clip_fetch(mp, dim, 0, 1, 0, 0);

	if (!vp)
		return 1;
	_clip_opassign(mp, vp, op);
	return 0;
}

/* ] arrays */
