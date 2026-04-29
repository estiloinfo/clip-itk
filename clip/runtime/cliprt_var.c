/* [ var */

CLIP_DLLEXPORT int
_clip_log(ClipVar * vp)
{
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case NUMERIC_t:
		if (vp->t.memo)
			return rational_empty(vp->r.r);
		else
			return vp->n.d == 0 ? 0 : 1;
	case LOGICAL_t:
		return vp->l.val;
	case CHARACTER_t:
		return vp->s.str.len == 0 ? 0 : 1;
	case ARRAY_t:
		return vp->a.count == 0 ? 0 : 1;
	case MAP_t:
		return vp->m.count == 0 ? 0 : 1;
	default:
		break;
	}
	return 0;
}

CLIP_DLLEXPORT long
_clip_long(ClipVar * vp)
{
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case NUMERIC_t:
		if (vp->t.memo)
			return (long) rational_toDouble(vp->r.r);
		else
			return (long) vp->n.d;
	case DATE_t:
		return vp->d.julian;
	case LOGICAL_t:
		return vp->l.val;
	case CHARACTER_t:
		return vp->s.str.len;
	case ARRAY_t:
		return vp->a.count;
	case MAP_t:
		return vp->m.count;
	default:
		break;
	}
	return 0;
}

CLIP_DLLEXPORT double
_clip_double(ClipVar * vp)
{
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case NUMERIC_t:
		if (vp->t.memo)
			return rational_toDouble(vp->r.r);
		else
			return vp->n.d;
	case DATE_t:
		return vp->d.julian;
	case LOGICAL_t:
		return vp->l.val;
	case CHARACTER_t:
		return vp->s.str.len;
	case ARRAY_t:
		return vp->a.count;
	case MAP_t:
		return vp->m.count;
	default:
		break;
	}
	return 0;
}

static int
descr_str(ClipMachine * mp, ClipVar * vp, char **str)
{
	int l;

	if (!vp)
		return 0;
	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case CHARACTER_t:
		return _clip_strFromVar(mp, vp, str, &l);
		break;
	case MAP_t:
		{
			int ind, l;

			if (!_clip_mind(mp, vp, HASH_description, &ind))
			{
				ClipVar *ep, *ap;

				ap = _clip_vptr(vp);

				ep = &(ap->m.items[ind].v);
				return _clip_strFromVar(mp, ep, str, &l);
			}
		}
		break;
	default:
		break;
	}
	return 0;

}

CLIP_DLLEXPORT int
_clip_strFromVar(ClipMachine * mp, ClipVar * vp, char **str, int *lenp)
{
	if (!vp)
		return 0;

	vp = _clip_vptr(vp);

	switch (vp->t.type)
	{
	case CHARACTER_t:
		{
			ClipBuf buf = { 0, 0 };

			int ret = _clip_expand(mp, &buf, &vp->s.str);

			if (!ret)
			{
				*str = buf.buf;
				*lenp = buf.len;
			}
			else
			{
				free(buf.buf);
				*str = 0;
				*lenp = 0;
			}
			return ret;
		}
	default:
		*lenp = 0;
	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_eval_macro(ClipMachine * mp, char *str, int len, ClipVar * dest)
{
	char *s = str, *e;
	int simple = 1;

	if (len == 0)
	{
		dest->t.type = UNDEF_t;
		dest->t.flags = F_NONE;
		return 0;
	}

	if (!(isalpha(*s) || *s == '_'))
		simple = 0;
	else
		for (s = str + 1, e = str + len; s < e; ++s)
			if (!(isalnum(*s) || *s == '_'))
			{
				simple = 0;
				break;
			}

	if (simple)
	{
		VarEntry *vp;
		long hash = _clip_casehashbytes(0, str, len);

		if (!_clip_take_field(mp, hash, -1, dest))
			return 0;

		vp = fetch_var(mp, hash);

		if (!vp)
		{
			if (mp->obj)
			{
				ClipVar *vpp;

				vpp = fetch_obj(mp, _clip_vptr(mp->obj), hash);
				if (vpp)
					return _clip_clone(mp, dest, vpp);
			}
		}

		if (!vp)
		{
			if (mp->noerrblock)
			{
				ClipVar var;

				memset(&var, 0, sizeof(var));
				return _clip_clone(mp, dest, &var);
			}
			else
			{
				_clip_trap_printf(mp, __file__, __LINE__, "no variable name: '%.*s'", len, str);
				return _clip_call_errblock(mp, 1);
				/*vp = add_private(mp, hash); */
			}
		}
		return _clip_clone(mp, dest, &vp->var);
	}
	else
	{
		ClipBlock block;
		int ret = _clip_compile_Block(mp, str, len, &block);
		ClipVar stack[1];
		ClipFrame frame = { stack, stack + 1, __file__, __LINE__, 0, 0, 0, 0, 0, 0, "eval_macro", 1, 0 };
		ClipFrame *inMacro = mp->inMacro;

		if (ret)
			return -1;
		memset(stack, 0, sizeof(stack));

		mp->inMacro = mp->fp;
		frame.up = mp->fp;
		mp->fp = &frame;

		ret = _clip_code_func(mp, &block, 0, 0, 0);

		mp->inMacro = inMacro;
		mp->fp = frame.up;
		*dest = stack[0];
		destroy_Block(mp, &block);
		if (ret)
			return 1;
		else
			return 0;
	}
}

CLIP_DLLEXPORT int
_clip_expand_var(ClipMachine * mp, ClipVar * vp)
{
	ClipBuf dest;
	int ret;
	char *s, *e;

	vp = _clip_vptr(vp);

	if (vp->t.type == CHARACTER_t)
	{
		ret = _clip_expand(mp, &dest, &(vp->s.str));
		if (ret)
			return ret;
	}
	else
	{
		_clip_destroy(mp, vp);
		vp->t.type = CHARACTER_t;
		vp->t.flags = F_MSTAT;
		vp->t.MACRO = 0;
		vp->s.str.buf = "";
		vp->s.str.len = 0;
		return 0;
	}
	_clip_destroy(mp, vp);
	vp->t.type = CHARACTER_t;
	vp->t.flags = 0;
	vp->t.MACRO = 1;
	vp->s.str = dest;
	for (s = dest.buf, e = dest.buf + dest.len; s < e; ++s)
		if (*s == '&')
		{
			ret = 1;
			break;
		}
	if (!ret)
		vp->t.MACRO = 0;
	return 0;
}

static int
find_macro(char *s, char *e, char **beg, char **end, char **next, int *dot)
{
	*dot = 0;
	if (s >= e)
		return 0;
	for (; s < e; ++s)
		if (*s == '&')
		{
			++s;
			if (s >= e)
				return 0;
			*beg = s;
			if (*s == '.')
				*dot = 1;
			if (*s == '\\')
			{
				++s;
				if (s >= e)
					return 0;
				switch (*s)
				{
				case 'x':
				case 'X':
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
					goto cont;
				}
				++s;
				*end = s;
				*next = s;
				return 1;
			}
			else
			{
				if (*s == '.')
					*dot = 1;

				  cont:

				for (; s <= e; ++s)
					if (s == e || !(isalnum(*s) || *s == '_'))
					{
						*end = s;
						if (*s == '.')
						{
							++s;
							*dot = 1;
						}
						*next = s;
						return 1;
					}
			}
			*end = s;
			++s;
			*next = s;
		}
	return 0;
}

static int
octdig(int ch)
{
	return ch - '0';
}

static int
hexdig(int ch)
{
	switch (ch)
	{
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
	case 'a':
		return 10;
	case 'B':
	case 'b':
		return 11;
	case 'C':
	case 'c':
		return 12;
	case 'D':
	case 'd':
		return 13;
	case 'E':
	case 'e':
		return 14;
	case 'F':
	case 'f':
		return 15;
	}
	return 0;
}

CLIP_DLLEXPORT int
_clip_expand(ClipMachine * mp, ClipBuf * dest, ClipBuf * src)
{
	char *s, *e, *beg, *end, *next;
	int dot;
	OutBuf buf;

	init_Buf(&buf);

	s = src->buf;
	e = s + src->len;
	for (; find_macro(s, e, &beg, &end, &next, &dot); s = next)
	{
		long hash;
		VarEntry *vp;
		ClipVar *vpp;
		int ch;

		putBuf_Buf(&buf, s, (beg - 1) - s);
		if (end == beg)
		{
			putByte_Buf(&buf, '&');
			if (dot)
				putByte_Buf(&buf, '.');
			continue;
		}

		if (*beg == '\\')
		{
			++beg;
			if (end == beg)
			{
				putByte_Buf(&buf, '&');
				putByte_Buf(&buf, '\\');
				continue;
			}

			switch ((ch = *beg))
			{
			case 'x':
			case 'X':
				{
					++beg;
					if ((end - beg) != 2)
					{
						  undo:
						putByte_Buf(&buf, '&');
						putByte_Buf(&buf, '\\');
						while (beg < end)
							putByte_Buf(&buf, *beg++);
						continue;
					}
					else
					{
						unsigned char b1, b2;

						b1 = hexdig(beg[0]);
						b2 = hexdig(beg[1]);
						putByte_Buf(&buf, b1 << 4 | b2);
					}
				}
				continue;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				{
					unsigned char b1, b2, b3;

					if ((end - beg) != 3)
						goto undo;
					b1 = octdig(beg[0]);
					b2 = octdig(beg[1]);
					b3 = octdig(beg[2]);
					putByte_Buf(&buf, b1 << 6 | b2 << 3 | b3);
				}
				continue;
			default:
				if ((end - beg) != 1)
					goto undo;
				switch (ch)
				{
				case 'n':
					putByte_Buf(&buf, '\n');
					break;
				case 'r':
					putByte_Buf(&buf, '\r');
					break;
				case 't':
					putByte_Buf(&buf, '\t');
					break;
				case 'v':
					putByte_Buf(&buf, '\v');
					break;
				case 'a':
					putByte_Buf(&buf, 7);
					break;
				case 'e':
					putByte_Buf(&buf, 27);
					break;
				default:
					putByte_Buf(&buf, ch);
					break;
				}
				continue;
			}
		}


		hash = _clip_casehashbytes(0, beg, end - beg);
		vp = fetch_var(mp, hash);
#if 0
		if (!vp)
		{
			_clip_trap_str(mp, __file__, __LINE__, "undefined macro-variable in string");
			goto _trap;
		}
#endif
#if 0
		if (!(vp->var.t.type & CHARACTER_t))
		{
			_clip_trap_str(mp, __file__, __LINE__, "non-character result of macro in string");
			goto _trap;
		}
#endif
		if (vp && !(mp->flags1 & NOEXPAND_MACRO_FLAG))
		{
			vpp = _clip_vptr(&vp->var);
			switch (vpp->t.type)
			{
			case CHARACTER_t:
				putBuf_Buf(&buf, vpp->s.str.buf, vpp->s.str.len);
				break;
			case NUMERIC_t:
				{
					if (vpp->t.memo)
					{
						char *bf;

						if (vpp->t.len)
							bf = rational_toString(vpp->r.r, 10, vpp->t.dec, 0);
						else
							bf = rational_toString(vpp->r.r, 10, mp->decimals, 0);
						putBuf_Buf(&buf, bf, strlen(bf));
						free(bf);
					}
					else
					{
						char bf[32];

						if (vpp->t.len)
							snprintf(bf, sizeof(bf), "%*.*f", vpp->t.len, vpp->t.dec, vpp->n.d);
						else
							snprintf(bf, sizeof(bf), "%g", vpp->n.d);
						putBuf_Buf(&buf, bf, strlen(bf));
					}
				}
				break;
			case LOGICAL_t:
				if (vpp->l.val)
					putBuf_Buf(&buf, ".T.", 3);
				else
					putBuf_Buf(&buf, ".F.", 3);
				break;
			case DATE_t:
				{
					int dd, mm, yy, ww;
					char bf[32];

					_clip_cdate(vpp->d.julian, &dd, &mm, &yy, &ww);
					sprintf(bf, "%02d/%02d/%d", dd, mm, yy);
					putBuf_Buf(&buf, bf, strlen(bf));
				}
				break;
			case DATETIME_t:
				{
					int len;
					char *r =
						_clip_ttoc(mp, vpp->dt.julian, vpp->dt.time, &len, mp->date_format, mp->hours,
							   mp->seconds);

					putBuf_Buf(&buf, r, len);
					free(r);
				}
				break;
			default:
				goto badtype;
				break;
			}
		}
		else
		{
			  badtype:
			putByte_Buf(&buf, '&');
			while (beg < end)
				putByte_Buf(&buf, *beg++);
			if (dot)
				putByte_Buf(&buf, '.');
		}
	}
	/* rest of string */
	if (s < e)
		putBuf_Buf(&buf, s, e - s);

	dest->len = buf.ptr - buf.buf;
	putByte_Buf(&buf, 0);
	dest->buf = buf.buf;
	return 0;
}

CLIP_DLLEXPORT int
_clip_macro(ClipMachine * mp)
{
	ClipVar *vp = mp->fp->sp - 1;

	vp = _clip_vptr(vp);

	if (vp->t.type == CHARACTER_t)
	{
		int l = vp->s.str.len;
		char *s = _clip_memdup(vp->s.str.buf, l);
		ClipVar *vpp = mp->fp->sp - 1;
		int r;

		_clip_destroy(mp, vpp);

		r = _clip_eval_macro(mp, s, l, vpp);
		free(s);

		return r;
	}
	else
	{
		_clip_trap_printf(mp, __file__, __LINE__, "macro operation with non-character argument (%s)",
				  _clip_typename(vp));
		return _clip_call_errblock(mp, 1);
	}
}

CLIP_DLLEXPORT void
_clip_var_str(const char *str, int len, ClipVar * dest)
{
	dest->s.str.buf = (char *) _clip_memdup(str, len);
	dest->s.str.len = len;
	dest->t.type = CHARACTER_t;
	dest->t.flags = F_NONE;
	dest->t.memo = 0;
}

CLIP_DLLEXPORT void
_clip_var_num(double num, ClipVar * vp)
{
	vp->t.type = NUMERIC_t;
	vp->t.flags = F_NONE;
	vp->t.len = 0;
	vp->t.dec = 0;
	vp->t.memo = 0;
	vp->n.d = num;
}

CLIP_DLLEXPORT void
_clip_var_log(int val, ClipVar * vp)
{
	vp->t.type = LOGICAL_t;
	vp->t.flags = F_NONE;
	vp->l.val = val;
}

/* ] var */
/* [ pcount */

CLIP_DLLEXPORT void
_clip_pcount(ClipMachine * mp)
{
	ClipVar *vp = mp->fp->sp;

	vp->t.type = NUMERIC_t;
	vp->t.flags = F_NONE;
	vp->t.len = 0;
	vp->t.dec = 0;
	vp->t.memo = 0;
	vp->n.d = mp->argc;
	mp->fp->sp++;
	CLIP_CHECK_STACK;
}

CLIP_DLLEXPORT void
_clip_pshift(ClipMachine * mp)
{
	--mp->argc;
	_clip_pcount(mp);
}

CLIP_DLLEXPORT int
_clip_parn(ClipMachine * mp)
{
	int no;
	ClipVar *sp = mp->fp->sp - 1;

	no = _clip_long(sp);

	_clip_destroy(mp, sp);

	if (no < 1 || no > mp->argc)
		return 0;

	mp->fp->sp--;
	_clip_push(mp, ARGPTR(mp, no));
	return 0;
}

/* ] pcount */
/* [ Object calls */

static int obj_call(struct ClipMachine *mp, int argc, long hash, int call);

#define CALL_CALL 0
#define CALL_GET 1
#define CALL_SET 2

CLIP_DLLEXPORT int
_clip_call(struct ClipMachine *mp, int argc, long hash)
{
	return obj_call(mp, argc, hash, CALL_CALL);
}

static int
obj_call(struct ClipMachine *mp, int argc, long hash, int call)
{
	ClipVar *op = _clip_vptr(mp->fp->sp - argc);
	int ret = 0;
	int _argc = mp->argc;
	ClipFrame *fp = mp->fp;
	ClipVar *bp = mp->bp;
	ClipFrame *inMacro = mp->inMacro;

	mp->inMacro = 0;
	switch (op->t.type)
	{
	case OBJECT_t:
		mp->argc = argc;
		mp->bp = fp->sp;

		switch (call)
		{
		case CALL_CALL:
			ret = op->o.rtti->call(mp, op->o.obj, op->o.rtti, hash);
			break;
		case CALL_GET:
			ret = op->o.rtti->get(mp, op->o.obj, op->o.rtti, hash);
			break;
		case CALL_SET:
			ret = op->o.rtti->set(mp, op->o.obj, op->o.rtti, hash);
			break;
		}

		mp->argc = _argc;
		mp->fp = fp;
		mp->bp = bp;
		for (; argc; --argc)
		{
			--(fp->sp);
			_clip_destroy(mp, fp->sp);
		}

		break;
	case MAP_t:
		{
			ClipVarEl *vlp;
			ClipVar *vpp;
			enum ClipVarType type;

			vlp = get_map_el(&op->m, hash);
			if (!vlp)
			{
				char buf[32];

				_clip_hash_name(mp, hash, buf, sizeof(buf));
				_clip_trap_printf(mp, __file__, __LINE__, "no method %s", buf);
				ret = EG_NOMETHOD;
				break;
			}
			vpp = &vlp->v;
			type = _clip_type(vpp);

			if (type != CCODE_t && type != PCODE_t)
			{
				if (argc == 1)
				{
					ClipVar *rp = mp->fp->sp - argc - 1;

					_clip_clone(mp, rp, vpp);
					--(fp->sp);
					_clip_destroy(mp, fp->sp);
					ret = 0;
				}
				else
				{
					char buf[32];

					_clip_hash_name(mp, hash, buf, sizeof(buf));
					_clip_trap_printf(mp, __file__, __LINE__, "method %s is not code", buf);
					ret = EG_NOMETHOD;
				}
				break;
			}

			vpp = _clip_vptr(vpp);

			if (type == CCODE_t)
				ret = _clip_func(mp, vpp->c.u.func, argc, 0, vpp->c.uplocals);
			else
				ret = _clip_code_func(mp, vpp->c.u.block, argc, 0, vpp->c.uplocals);

		}
		break;
	default:
		ret = _clip_func_hash(mp, hash, argc, 0, 0);
		break;
	}

	mp->inMacro = inMacro;
	if (ret)
		return _clip_call_errblock(mp, ret);

	return ret;
}

CLIP_DLLEXPORT int
_clip_get(struct ClipMachine *mp, long hash)
{
	return obj_call(mp, 0, hash, CALL_GET);
}

CLIP_DLLEXPORT int
_clip_set(struct ClipMachine *mp, long hash)
{
	return obj_call(mp, 1, hash, CALL_SET);
}

/* ] Object calls */
/* [ OutBuf */

void
init_Buf(OutBuf * bp)
{
	bp->buf = bp->ptr = malloc(64);
	bp->end = bp->buf + 64;
}

void
destroy_Buf(OutBuf * bp)
{
	if (!bp)
		return;
	if (bp->buf)
		free(bp->buf);
}

static void
grow_buf(OutBuf * bp, int min)
{
	int size = bp->end - bp->buf;
	int dsize = size / 2;
	int pos = bp->ptr - bp->buf;

	if (min < 16)
		min = 16;
	if (dsize < min)
		dsize = min;
	size += dsize;
	bp->buf = realloc(bp->buf, size);
	bp->end = bp->buf + size;
	bp->ptr = bp->buf + pos;
}

void
putByte_Buf(OutBuf * bp, unsigned char b)
{
	*bp->ptr = b;
	++(bp->ptr);
	if (bp->ptr >= bp->end)
		grow_buf(bp, 0);
}

void
putBuf_Buf(OutBuf * bp, char *p, int len)
{
	if ((bp->end - bp->ptr) <= len)
		grow_buf(bp, len + 1);
	memcpy(bp->ptr, p, len);
	bp->ptr += len;
}

void
putShort_Buf(OutBuf * bp, short s)
{
	putBuf_Buf(bp, (char *) &s, sizeof(s));
}

void
putLong_Buf(OutBuf * bp, long l)
{
	putBuf_Buf(bp, (char *) &l, sizeof(l));
}

void
putInt32_Buf(OutBuf * bp, long l)
{
	int r = (int) l;
	putBuf_Buf(bp, (char *) &r, sizeof(r));
}

void
putDouble_Buf(OutBuf * bp, double d)
{
	putBuf_Buf(bp, (char *) &d, sizeof(d));
}

void
putStr_Buf(OutBuf * bp, char *str)
{
	putBuf_Buf(bp, str, strlen(str) + 1);
}

/* ] OutBuf */
