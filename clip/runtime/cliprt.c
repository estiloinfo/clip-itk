/*
   Copyright (C) 2001  ITK
   Authors  : Uri Hnykin <uri@itk.ru>, Paul Lasarev <paul@itk.ru>, and other
   License : (GPL) http://www.itk.ru/clipper/license.html
 */
/*
   $Log$
   Revision 1.7  2007/10/17 13:30:03  itk
   uri: fix warnings for gcc 4.2

   Revision 1.6  2007/03/09 14:42:12  itk
   uri: many fix for AMD64

   Revision 1.5  2007/02/21 12:56:20  itk
   uri: small fix, HZ.

   Revision 1.4  2007/02/12 16:36:24  itk
   uri: some fix tasks

   Revision 1.3  2007/02/12 16:13:08  itk
   uri: some fix about task_select and some fix for amd64

   Revision 1.2  2007/01/09 12:11:14  itk
   uri: small fix for compare rational numbers

   Revision 1.1  2006/06/22 19:01:33  itk
   uri: initial

   Revision 1.447  2006/05/12 06:49:34  clip
   uri: fix bug in macroassing and _clip_parse_name.
	&("m[varname]") := &("m[varname]")

   Revision 1.446  2006/04/18 16:39:32  clip
   uri: small fix

   Revision 1.445  2006/04/18 16:11:49  clip
   uri: new release

   Revision 1.444  2005/12/02 08:33:33  clip
   uri: small fix

   Revision 1.443  2005/08/08 09:11:39  clip
   alena: restore

   Revision 1.442  2005/08/08 09:00:30  clip
   alena: fix for gcc 4

   Revision 1.441  2005/07/28 05:57:07  clip
   alena: fix clip_ASCAN memory floating

   Revision 1.440  2005/07/25 10:23:21  clip
   alena: fix bug with "floating" memory  in functions AEVAL() and
   _clip_resume()

   Revision 1.439  2005/06/30 11:32:39  clip
   uri: small fix

   Revision 1.438  2004/12/10 10:43:28  clip
   uri: fixed memory leak in _clip_ret*() with twice call.

   Revision 1.437  2004/11/25 08:28:28  clip
   uri: fix for deleting spaces from filenames for DOS-mode files.

   Revision 1.436  2004/11/04 07:37:27  clip
   uri: some fix in numeric format with "+-" operation

   Revision 1.435  2004/11/03 10:12:35  clip
   uri: small fix about "" $ "1234"

   Revision 1.434  2004/11/01 06:39:45  clip
   uri: delete logg.

   Revision 1.433  2004/10/29 09:15:42  clip
   uri: small fix

   Revision 1.432  2004/10/28 11:47:33  clip
   uri: fix formatiing in STR(), pad*() for numeric data and constants.

   Revision 1.431  2004/10/27 14:43:56  clip
   rust: debugging SIG handler

   Revision 1.430  2004/10/25 09:24:12  clip
   uri: small fix for oget:colorSpec

   Revision 1.429  2004/10/13 12:47:37  clip
   uri: small fix for cygwin

   Revision 1.428  2004/09/30 14:37:17  clip
   rust: path parse fix

   Revision 1.426  2004/08/12 12:32:14  clip
   uri: small fix

   Revision 1.425  2004/08/12 10:21:48  clip
   uri: disable stdout buffering by default

   Revision 1.424  2004/07/05 10:12:20  clip
   alena: small bug in _storc()

   Revision 1.423  2004/07/05 09:31:20  clip
   uri: small fix in _storni()

   Revision 1.422  2004/07/05 08:31:14  clip
   uri: small fix in _storni()

   Revision 1.421  2004/06/11 10:57:32  clip
   rust: freeing MEMBUF on delete_ClipMachine()

   Revision 1.420  2004/05/20 14:41:10  clip
   rust: memory leak fixed

   Revision 1.419  2004/04/30 11:50:02  clip
   uri: add mimeTypesLoad(), mimeTypeGet(cType), mimeTypeSet(cType,cData)

   Revision 1.418  2004/04/08 15:01:46  clip
   rust: suppress call to Task_killAll() in clip___QUIT()

   Revision 1.417  2004/02/17 08:39:56  clip
   rust: missing trailing spaces in string substraction ('-' op)

   Revision 1.416  2004/02/16 13:19:53  clip
   rust: sigsegv with RDD drivers fixed

   Revision 1.415  2004/02/10 11:14:03  clip
   rust: str_str() -> _clip_strstr() with '"" $ "string"' bugfix

   Revision 1.414  2004/01/29 13:47:53  clip
   uri: small fix

   Revision 1.413  2004/01/28 13:49:00  clip
   rust: common ClipMachine->kbdbuf

   Revision 1.412  2004/01/25 10:23:40  clip
   uri: fix bug in exit procedures and inkey()

   Revision 1.411  2004/01/23 14:40:19  clip
   uri: small fix for cygwin

   Revision 1.410  2004/01/23 10:24:49  clip
   uri: small fix in find_macro

   Revision 1.409  2004/01/23 09:42:28  clip
   uri: small fix in macro_in_string

   Revision 1.408  2004/01/23 08:29:23  clip
   uri: small fix ROOTPATH for cygwin

   Revision 1.407  2004/01/16 10:04:33  clip
   uri: small fix for ROOTPATH

   Revision 1.406  2004/01/04 10:24:49  clip
   uri: add set(_SET_ROOTPATH,"rootpath") for add "rootpath" to all filenames,
	via "/var/www/htdocs" in apache.

   Revision 1.405  2003/12/20 09:02:18  clip
   uri: small fix for freeBsd

   Revision 1.404  2003/12/16 11:24:05  clip
   uri: support FoxPro syntax aVar[nElement];
	support call codeblock via function cb:={||}; cb()
	redisign error NOFUNC.

   Revision 1.403  2003/11/27 17:11:59  clip
   uri:small fix int ctod()

   Revision 1.402  2003/11/27 12:55:56  clip
   add CLIPA function
   paul

   Revision 1.401  2003/11/21 11:12:21  clip
   set operation result if errorblock return new value
   paul

   Revision 1.400  2003/11/21 06:21:16  clip
   uri: rewriting generator error for "invalid arguments for %s operation",
	for operation +-*$=%**....

   Revision 1.399  2003/11/19 11:48:25  clip
   add FLUSHOUT_FLAG, _SET_FLUSHOUT to set
   paul

   Revision 1.398  2003/11/11 11:58:16  clip
   uri: small fix: called EG_SIGNAL with first_mp

   Revision 1.397  2003/11/11 11:36:58  clip
   uri: call errorblock with EG_SIGNAL if signal from system sended

   Revision 1.396  2003/11/04 11:18:34  clip
   post signal handler
   paul

   Revision 1.395  2003/10/30 10:29:06  clip
   hash conflict in area/field pairs
   closes #159
   paul

   Revision 1.394  2003/10/13 06:08:48  clip
   uri: fix for "errorlevel(3); quit" code

   Revision 1.393  2003/10/03 09:30:22  clip
   uri: obj:onError(self,attr_name) trigger added

   Revision 1.392  2003/09/11 11:18:06  clip
   uri: small fix

   Revision 1.391  2003/09/09 14:36:15  clip
   uri: fixes for mingw from Mauricio and Uri

   Revision 1.390  2003/09/09 09:41:00  clip
   uri: sorry, removed debug info from prev fix

   Revision 1.389  2003/09/09 09:39:05  clip
   uri: small fix in dirChange()

   Revision 1.388  2003/09/08 15:06:02  clip
   uri: next step fixes for mingw from uri

   Revision 1.387  2003/09/05 12:11:52  clip
   uri: initial fixes for mingw+win32 from uri

   Revision 1.386  2003/09/02 14:27:42  clip
   changes for MINGW from
   Mauricio Abre <maurifull@datafull.com>
   paul

   Revision 1.385  2003/09/01 13:34:12  clip
   uri: fix in translate_path() for non-existing directories.

   Revision 1.384  2003/08/04 09:49:23  clip
   generate 'no variable' when call by reference
   paul

   Revision 1.383  2003/07/30 11:06:06  clip

   paul

   Revision 1.382  2003/07/03 06:10:04  clip
   fix for --static, --fullstatic and libs
   paul

   Revision 1.381  2003/07/02 12:13:27  clip
   possible fix sigsegv #141
   paul

   Revision 1.380  2003/07/01 11:32:13  clip
   fix some memleaks
   paul

   Revision 1.379  2003/06/20 08:21:25  clip
   possible fixes #144
   paul

   Revision 1.378  2003/06/14 09:56:24  clip
   uri: small fix for directory("/")

   Revision 1.377  2003/05/16 12:55:43  clip
   uri: small fix in operator "="

   Revision 1.376  2003/05/16 11:08:02  clip
   initial support for using assembler instead C
   now activated if environment variable CLIP_ASM is defined to any value
   paul

   Revision 1.375  2003/04/29 11:09:39  clip
   memleak on start()
   possibly closes #140
   paul

   Revision 1.374  2003/04/14 14:01:24  clip
   rust: bug in fclose(), reported by Istv� F�di <foldii@terrasoft.hu>

   Revision 1.373  2003/04/02 10:53:19  clip
   rust: _clip_close() added

   Revision 1.372  2003/04/02 07:18:44  clip
   uri: containers for all clipMachines

   Revision 1.371  2003/04/01 07:28:12  clip
   str[x], str[x,y] syntax support
   possible closes #130
   paul

   Revision 1.370  2003/04/01 06:08:24  clip
   uri: initial support syntax 'string[pos,len]', but not work really.

   Revision 1.369  2003/03/28 11:24:52  clip
   possible fix for sigsegv in menu and mouse
   paul

   Revision 1.368  2003/03/25 10:31:13  clip
   possible fixes #133
   paul

   Revision 1.367  2003/03/21 11:49:40  clip
   rust: RDD locks with tasks (DOS compatibility)

   Revision 1.366  2003/03/20 12:22:36  clip
   uri: errorLevel() small fix.

   Revision 1.365  2003/03/12 12:50:42  clip
   rust: tasks share RDDs and subdrivers

   Revision 1.364  2003/03/06 07:16:24  clip
   uri: delete right ' ' from filenames for "set translate path on"

   Revision 1.363  2003/01/22 10:33:38  clip
   dosparam() function
   closes #111
   paul

   Revision 1.362  2003/01/21 14:05:17  clip
   license info:
   hidden flag --license print license
   file clip/registration can contain signed license info
   and must be generated by rsa-sign utilite
   paul

   Revision 1.361  2003/01/18 12:04:30  clip
   uri: small fix in oget:varget()

   Revision 1.360  2003/01/05 12:32:25  clip
   possible fixes #95,#98
   paul

   Revision 1.359  2003/01/05 10:34:22  clip
   possible fixes #98
   paul

   Revision 1.358  2002/12/31 08:03:36  clip
   assign to locals
   closes #95
   paul

   Revision 1.357  2002/12/27 12:11:37  clip
   references to arrays
   again closes #85
   paul

   Revision 1.356  2002/12/26 13:10:40  clip
   reference to local param
   possible fixes #13
   paul

   Revision 1.355  2002/12/23 13:57:47  clip
   reference to temporary object
   frame structure extended!
   closes #90
   paul

   Revision 1.354  2002/12/18 11:28:42  clip
   fix reference to array in params
   closes #85
   paul

   Revision 1.353  2002/12/11 13:09:08  clip
   clear t.memo flag on assign
   possibly closes #80
   paul

   Revision 1.352  2002/12/04 09:30:25  clip
   profile sigsegv fix
   paul

   Revision 1.351  2002/12/04 09:05:52  clip
   possible profile cleanup in _clip_eval
   paul

   Revision 1.350  2002/12/04 07:09:59  clip
   simple profiler realised
   start program with --profile will generate <progname>.pro profile
   limitations: manual written C functions are not profiled, bad accuracy
   paul

   Revision 1.349  2002/12/02 09:24:08  clip
   map obj fetch for simple name
   closes #65
   paul

   Revision 1.348  2002/11/27 14:09:14  clip
   rust: _clip_vptr(rval) to _clip_dup in _clip_[i]assign()

   Revision 1.347  2002/11/27 14:06:07  clip
   clear field flag on assign
   possible closes #63
   paul

   Revision 1.346  2002/11/19 13:36:28  clip
   sigsegv in clip_CLIP func
   closes #42
   paul

   Revision 1.345  2002/11/12 09:47:11  clip
   *** empty log message ***

   Revision 1.344  2002/11/12 09:12:17  clip
   uri: small fix for cygwin

   Revision 1.343  2002/11/10 13:20:17  clip
   fix for gcc-3.2 optimisation
   paul

   Revision 1.342  2002/10/31 16:23:24  clip
   *** empty log message ***

   Revision 1.341  2002/10/30 14:58:44  clip
   uri: small tab formatting.

   Revision 1.340  2002/10/30 14:39:32  clip
   rust: some default SETs

   Revision 1.339  2002/10/26 11:10:02  clip
   initial support for localized runtime messages
   messages are in module 'cliprt'
   paul

   Revision 1.338  2002/10/19 14:23:34  clip
   uri:small fix

   Revision 1.337  2002/10/19 14:10:52  clip
   uri: small fix

   Revision 1.336  2002/10/19 14:08:38  clip
   uri: small fix for cygwin

   Revision 1.335  2002/10/19 14:05:37  clip
   uri: hz

   Revision 1.334  2002/10/19 14:03:52  clip
   uri: small fix in _clip_absolute_path for cygwin

   Revision 1.333  2002/10/19 13:36:42  clip
   uri: small fix for cygwin in _clip_translate_path()

   Revision 1.332  2002/09/25 11:47:25  clip
   add function: loadModuleMsg(cModule, cFilename_mo) -> bResult
   predefined macro: __CLIP_MODULE__  expands to current module name as "modname"
   new accepted environment var: CLIP_LOCALE_ROOT
	used by clip, clip_msgmerge, clip_msgfmt, and at runtime
   paul

   Revision 1.331  2002/09/19 08:36:29  clip
   uri : fix bug in operator "   " != "123"

   Revision 1.330  2002/09/16 09:35:10  clip
   alexey: function _clip_host_charset()

   Revision 1.329  2002/09/11 10:05:54  clip
   '$' is now case-sensitive
   paul

   Revision 1.328  2002/08/27 13:31:57  clip
   add _clip_ref_public_noadd func
   fix for mem restore
   paul

   Revision 1.327  2002/08/07 15:07:00  clip
   uri: small fix in _clip_strnncmp

   Revision 1.326  2002/08/05 09:28:07  clip
   MAPEVAL()
   paul

   Revision 1.325  2002/08/05 08:14:59  clip
   uri: small fixes

   Revision 1.324  2002/08/05 07:25:03  clip
   Operator overloading for objects:
	'+' operator_add
	'-' operator_sub
	'*' operator_mul
	'/' operator_div
	'%' operator_mod
	'^' operator_pow
	'|' operator_or
	'&' operator_and
	'$' operator_in
	'=' operator_eq
	'==' operator_eeq
	'!=' operator_neq
	'<'  operator_lt
	'>'  operator_gt
	'<=' operator_le
	'>=' operator_ge
   paul

   Revision 1.323  2002/07/02 10:32:33  clip
   novar err in macro
   paul

   Revision 1.322  2002/05/23 13:16:10  clip
   uri: add environment CLIP_LANG as LANG processing

   Revision 1.321  2002/05/16 09:07:43  clip
   possible to disable expansion in strings:
   SET MACRO_IN_STRING OFF
   paul

   Revision 1.320  2002/05/14 08:12:25  clip
   rust: few new parameters to _clip_ttoc are added

   Revision 1.319  2002/05/12 11:19:23  clip
   rust: _clip_fetch_c_item_type() - fetches item of type

   Revision 1.318  2002/04/30 13:55:50  clip
   rust: respecting 'type weight' for 'X' fields

   Revision 1.317  2002/04/30 12:52:01  clip
   add -stack flag to configure to enable runtime stack check
   paul

   Revision 1.316  2002/04/30 09:07:18  clip
   fetch error verbose
   paul

   Revision 1.315  2002/04/30 08:59:34  clip
   try fetch from non-array verbose
   paul

   Revision 1.314  2002/04/22 15:29:55  clip
   rust: initial support of DATETIME type

   Revision 1.313  2002/04/19 13:01:08  clip
   rust: bug in _clip_path()

   Revision 1.312  2002/04/18 13:11:56  clip
   initial support for ncpfs
   paul

   Revision 1.311  2002/04/18 11:33:26  clip
   uri: num1==num2 small fixes

   Revision 1.310  2002/04/17 11:58:36  clip
   rust: _clip_path() fixed for paths like \\hostname\dir\file.ext

   Revision 1.309  2002/04/17 11:02:04  clip
   rust: _clip_path() fixed for paths like d:test.dbf

   Revision 1.308  2002/04/17 10:30:28  clip
   uri: _clip_translate_path changed for win32 paths "\\host\resource"

   Revision 1.307  2002/04/15 13:04:21  clip
   trim spaces in alias expression
   paul

   Revision 1.306  2002/04/09 10:44:47  clip
   undef ref fix
   paul

   Revision 1.305  2002/04/05 12:34:14  clip
   "&()" preproc fix
   "&a[1]" runtime fix
   vardef place
   paul

   Revision 1.304  2002/03/28 12:26:45  clip
   assign and reference change
   may be followed by lot of problems...
   paul

   Revision 1.303  2002/03/26 09:32:43  clip
   exit procedure sigsegv
   paul

   Revision 1.302  2002/03/25 10:53:46  clip
   wrong i18n messages recoding
   paul

   Revision 1.301  2002/03/22 11:31:41  clip
   dirty fix for static codeblocks
   paul

   Revision 1.300  2002/03/12 14:42:43  clip
   uri: small fixes

   Revision 1.299  2002/03/11 13:50:05  clip
   bad return on namecall fixed
   paul

   Revision 1.298  2002/03/11 09:50:21  clip
   uri: small fix in _clip_cmp for string compare.

   Revision 1.297  2002/03/10 09:21:20  clip
   uri: fixes in islower,isupper, _clip_cmp

   Revision 1.296  2002/03/06 11:27:52  clip
   string compare
   paul

   Revision 1.295  2002/02/28 08:50:10  clip
   add _SET_MAPERR
   with .t. map fetch will return NIL if no index instead of error generation
   paul

   Revision 1.294  2002/02/18 13:19:25  clip
   uri: str1-str2 bug fixed

   Revision 1.293  2002/02/09 11:58:02  clip
   codeblocks params
   paul

   Revision 1.292  2002/02/06 12:37:33  clip
   sigsegv
   paul

   Revision 1.291  2002/02/06 08:30:13  clip
   static codeblocks
   paul

   Revision 1.290  2002/02/03 10:05:37  clip
   uri: num+date; num-date; fixed

   Revision 1.289  2002/02/01 10:00:54  clip
   charset improvments
   paul

   Revision 1.288  2002/02/01 09:02:15  clip
   pgchars
   paul

   Revision 1.287  2002/01/30 13:02:06  clip
   use DLLREALSUFF instead DLLSUFF in clip_makeslib
   eval(b, @l)
   full filename in load error
   paul

   Revision 1.286  2002/01/28 13:52:35  clip
   ALTD() ignore arguments
   paul

   Revision 1.285  2002/01/28 13:43:23  clip
   add ALTD func

   ALTD(0) - disable debugging
   ALTD(1) - enable debugging
   ALTD(2) - enable debugging and wait connection

   paul

   Revision 1.284  2002/01/28 12:55:40  clip
   array by ref
   paul

   Revision 1.283  2002/01/28 09:09:24  clip
   pass array by reference
   paul

   Revision 1.282  2002/01/28 07:31:49  clip
   C/unknown charset cleanup
   paul

   Revision 1.281  2002/01/22 09:28:00  clip
   _clip_stor* funcs force change type of assigned vars
   paul

   Revision 1.280  2002/01/16 15:29:08  clip
   chinese patch
   preproc bugs
   some pseudographic chars < 32
   paul

   Revision 1.279  2002/01/08 12:24:56  clip
   error in error handler
   paul

   Revision 1.278  2002/01/08 10:01:58  clip
   break
   without parameter trap NIL
   paul

   Revision 1.277  2002/01/04 10:29:48  clip
   big module
   paul

   Revision 1.276  2001/12/26 14:35:06  clip
   container sharing
   paul

   Revision 1.275  2001/12/26 12:12:17  clip
   share container for ClipMachine's
   paul

   Revision 1.274  2001/12/22 10:31:56  clip
   uri: small fix in compare doubles

   Revision 1.273  2001/12/22 10:17:21  clip
   uri: small fix in compare double values (very small values ignored)

   Revision 1.272  2001/12/13 10:13:07  clip
   on call a(1) if no func A, try take 1st member of memvar array A
   paul

   Revision 1.271  2001/12/06 13:53:16  clip
   alexey: added CLIP_DLLEXPORT prefix to _clip_path()

   Revision 1.270  2001/11/27 11:44:46  clip
   add mouse control functions:

   INKEY(timeout, [nEventMask])
   NEXTKEY - use system eventMask
   SET(_SET_EVENTMASK)
   MPRESENT() --> lIsPresent
   MHIDE()
	  note: not fully compatible (parameters are ignored)
   MSHOW()
	  note: not fully compatible (parameters are ignored)
   MSETCURSOR( [<lVisible>] ) --> lIsVisible
   MROW() --> nMouseRow
   MCOL() --> nMouseCol
   MLEFTDOWN() --> lIsPressed
   MRIGHTDOWN() --> lIsPressed
   MSETBOUNDS( [<nTop>], [<nLeft>], [<nBottom>], [<nRight>] ) --> NIL
   MSAVESTATE() --> cSaveState
   MRESTSTATE( <cSaveState> ) --> NIL
   MDBLCLK([<nNewSpeed>]) --> nSpeed
	  note: really not work
   MSETPOS( <nRow>, <nCol> ) --> NIL
	  note: really not work
   MSTATE() --> aState | 0

   paul

   Revision 1.269  2001/11/23 13:36:45  clip
   assign by ref to map/array members:
   o:a := @var
   to break reference, use:
   o:a := @nil
   paul

   Revision 1.268  2001/11/23 12:16:42  clip
   _clip_calc_hash2 defaults to -1
   paul

   Revision 1.267  2001/11/23 11:08:17  clip
   add _clip_hashword(str,len), _clip_casehashword(str,len), which trim
   str before calc hash
   paul

   Revision 1.266  2001/11/22 11:33:32  clip
   a:='test->first'
   ? field->&a
   paul

   Revision 1.265  2001/11/22 08:06:16  clip
   bug in _clip_push_shash for '0' retvalue; ==> switch .. case 0 was'nt work
   paul

   Revision 1.264  2001/11/21 09:14:32  clip
   uri: remake __setCenture & set(_SET_DATEFORMAT)

   Revision 1.263  2001/11/20 13:19:53  clip
   uri: setTxlat(), setPxlat() added

   Revision 1.262  2001/11/16 12:21:29  clip
   process env files now work with
   ?VAR=val? ....
   construction
   paul

   Revision 1.261  2001/11/14 06:38:02  clip
   fix crash on exit after START
   paul

   Revision 1.260  2001/11/13 12:31:01  clip
   share functions between tasks
   TASKID() function
   paul

   Revision 1.259  2001/11/13 10:23:19  clip
   START now share publics and hashnames
   paul

   Revision 1.258  2001/11/12 14:13:46  clip
   START(cFuncname) now work :)
   paul

   Revision 1.257  2001/11/08 14:20:55  clip
   move _clip_translate_charset into cliprt.c
   paul

   Revision 1.256  2001/11/08 14:00:07  clip
   auto recoding of i18n messages
   paul

   Revision 1.255  2001/11/08 09:49:59  clip
   alexey: add function _clip_mtype, what returns a type of map field

   Revision 1.254  2001/11/06 10:20:40  clip
   _clip_call_errblock in various points
   paul

   Revision 1.253  2001/11/02 10:51:39  clip
   type casting in _clip_path()
   rust

   Revision 1.252  2001/11/01 14:54:40  clip
   _clip_path() - makes absolute path respecting SET DEFAULT, SET PATH etc
   rust

   Revision 1.251  2001/10/30 11:56:44  clip
   initial value of public vars to .f.

   Revision 1.250  2001/10/29 11:56:34  clip
   fix for BLINK(intensitive background)
   paul

   Revision 1.249  2001/10/29 06:51:21  clip
   fix text fopen modes
   paul

   Revision 1.248  2001/10/22 07:27:51  clip
   SPARC fixes
   paul

   Revision 1.247  2001/10/17 05:42:21  clip
   uri: bug in cllip_datetostr with empty date

   Revision 1.246  2001/10/16 13:04:34  clip
   uri: add _clip_datetostr

   Revision 1.245  2001/10/11 09:56:52  clip
   uri; :)

   Revision 1.244  2001/10/11 09:12:43  clip
   uri: bug in _glob_match with pattern length < string length

   Revision 1.243  2001/10/10 10:26:17  clip
   add cmp functions with using _clip_cmptbl and stadard
   semantic, fix _clip_cmp for using one.
   _clip_strcmp
   _clip_strncmp
   _clip_strcasecmp
   _clip_strncasecmp

   Revision 1.242  2001/09/28 10:39:26  clip
   reffuncs not applied to FMemvar
   paul

   Revision 1.241  2001/09/21 10:29:15  clip
   memvar priority in macro
   paul

   Revision 1.240  2001/09/21 09:53:25  clip
   charsets handling
   paul

   Revision 1.239  2001/09/20 08:37:38  clip
   add _clip_storclen function
   From: Anton Ustyancev <anton@cinvest.chel.su>
   paul

   Revision 1.238  2001/09/12 10:50:42  clip
   stream output recoding
   paul

   Revision 1.237  2001/09/12 10:13:12  clip
   export some funcs
   paul

   Revision 1.236  2001/09/11 12:28:10  clip
   changes
   paul

   Revision 1.235  2001/09/10 05:32:05  clip
   _clip_par_*   functions
   paul

   Revision 1.234  2001/09/07 09:26:39  clip
   use cygpath
   fix fcntl
   paul

   Revision 1.233  2001/09/07 08:03:00  clip
   binding
   paul

   Revision 1.232  2001/09/07 06:31:48  clip
   dll bindings
   paul

   Revision 1.231  2001/09/06 14:01:06  clip
   -L flag
   paul

   Revision 1.230  2001/09/04 04:03:18  clip
   skip_norm preprocessor match bug with logical constants
   paul

   Revision 1.229  2001/09/03 14:48:50  clip
   absent parameters value
   paul

   Revision 1.228  2001/09/03 13:16:14  clip
   new array & field
   paul

   Revision 1.227  2001/09/03 09:56:56  clip
   refs
   paul

   Revision 1.226  2001/09/01 06:36:46  clip
   simple reference loop detection
   paul

   Revision 1.225  2001/09/01 03:50:00  clip
   possible hanged refs
   paul

   Revision 1.224  2001/08/31 08:56:14  clip
   references

   Revision 1.223  2001/08/30 15:58:39  clip
   library bindings
   paul

   Revision 1.222  2001/08/30 14:37:53  clip
   new library binding
   paul

   Revision 1.221  2001/08/28 07:14:44  clip
   LOGICAL in CODESTR
   paul

   Revision 1.220  2001/08/27 10:18:06  clip
   field flag & parameters
   paul

   Revision 1.219  2001/08/27 09:45:30  clip
   switch operator
   paul

   Revision 1.218  2001/08/26 12:42:48  clip
   reflocals
   paul

   Revision 1.217  2001/08/26 12:11:59  clip
   reflocals
   paul

   Revision 1.216  2001/08/26 11:37:00  clip
   selfref detection in local(b:=@localvar)
   paul

   Revision 1.215  2001/08/26 09:10:06  clip
   references
   paul

   Revision 1.214  2001/08/25 12:30:33  clip
   memleak in @func()
   paul

   Revision 1.213  2001/08/25 10:10:30  clip
   fix
   paul

   Revision 1.212  2001/08/25 09:07:09  clip
   privates creation/removing in macro
   paul

   Revision 1.211  2001/08/25 08:27:28  clip
   private creation in macro
   paul

   Revision 1.210  2001/08/25 08:16:55  clip
   CODESTR fixup
   paul

   Revision 1.209  2001/08/24 16:48:51  clip
   privates/paramters
   paul

   Revision 1.208  2001/08/24 15:59:00  clip
   compare fix
   paul

   Revision 1.207  2001/08/24 14:38:46  clip
   CODESTR pseudofunction
   paul

   Revision 1.206  2001/08/24 12:41:23  clip
   uri: fixed: oper != work as =, not as ==

   Revision 1.205  2001/08/24 08:42:09  clip
   multiple private/parameters operators
   paul

   Revision 1.204  2001/08/24 08:04:38  clip
   macro assign to array/map members
   paul

   Revision 1.203  2001/08/23 13:52:06  clip
   merge with NEWLOCALS
   paul

   Revision 1.199.2.16  2001/08/23 12:20:21  clip
   ISPRIVATE(cName) ->lVal
   ISPUBLIC(cName) ->lVal
   paul

   Revision 1.199.2.15  2001/08/23 10:13:42  clip
   fix field/memvar order in references
   paul

   Revision 1.199.2.14  2001/08/23 08:57:54  clip
   clear field flag in param push
   paul

   Revision 1.199.2.13  2001/08/23 07:47:38  clip
   memfix
   paul

   Revision 1.199.2.12  2001/08/22 14:31:06  clip
   fix
   paul

   Revision 1.199.2.11  2001/08/22 13:52:39  clip
   assign to field
   paul

   Revision 1.199.2.10  2001/08/22 08:45:20  clip
   fix
   paul

   Revision 1.199.2.9  2001/08/22 07:52:06  clip
   ref to macro and parameters
   paul

   Revision 1.199.2.8  2001/08/21 14:29:38  clip
   refernce to macro: b:=@&s
   paul

   Revision 1.199.2.7  2001/08/21 12:10:35  clip
   local init in codeblock, f.e.:

   local a:={1,2,3}
   cb:={|x|local(b:=a[2]),iif(x==NIL,b,b:=x)}
   ? eval(cb)
   ? a
   ? eval(cb,'dddd')
   ? a
   ?

   paul

   Revision 1.199.2.6  2001/08/21 09:33:09  clip
   runtime macro blocks {||&s}
   paul

   Revision 1.199.2.5  2001/08/21 08:29:54  clip
   reference assign (like b:=@a[1])
   paul

   Revision 1.199.2.4  2001/08/20 08:16:59  clip
   fetch from temporary array
   paul

   Revision 1.199.2.3  2001/08/16 13:41:54  clip
   fix
   paul

   Revision 1.199.2.2  2001/08/16 09:46:39  clip
   params
   paul

   Revision 1.199.2.1  2001/08/03 12:07:59  clip
   NEWLOCALS initial
   paul

   Revision 1.202  2001/08/23 04:57:35  clip
   alexey: changes in _clip_str - added *lenp=0 for non-character types

   Revision 1.201  2001/07/31 09:34:01  clip
   uri: added default value for FILE&DIR CREATE MODE

   Revision 1.200  2001/07/31 08:21:50  clip
   uri: add SET_FILECREATEMODE & SET_DIRCREATEMODE and fixed in functions

   Revision 1.199  2001/07/26 11:48:37  clip
   after
   set ('PRINTER_CHARSET', 'cp866')
   and
   set printer to asdf
   all output to printer will be recoded from CLIP_HOSTCS to PRINTER_CHARSET
   if PRINTER_CHARSET is not setted, recode from CLIP_HOSTCS to CLIP_CLIENTCS
   paul

   Revision 1.198  2001/07/24 08:20:28  clip
   alexey: function _clip_mputl added

   Revision 1.197  2001/07/05 08:51:49  clip
   uri: set exact on & right spaces

   Revision 1.196  2001/06/15 12:02:36  clip
   implement
   int _clip_storni(ClipMachine * mp, int n, int num, int ind);
   int _clip_stornl(ClipMachine * mp, long n, int num, int ind);
   int _clip_stornd(ClipMachine * mp, double n, int num, int ind);
   int _clip_storc(ClipMachine * mp, const char *str, int num, int ind);
   int _clip_storl(ClipMachine * mp, int l, int num, int ind);
   int _clip_stordj(ClipMachine * mp, long julian, int num, int ind);
   int _clip_stordc(ClipMachine * mp, int yy, int mm, int dd, int num, int ind);

   paul

   Revision 1.195  2001/06/14 10:20:13  clip
   LOADBLOCK(<cName>) -> code
   paul

   Revision 1.194  2001/06/13 13:05:47  clip
   uri: add case flag in _clip_glob_match(), fixed all calls  this functions

   Revision 1.193  2001/06/13 11:27:38  clip
   BEEP()
   stack deep bug in array init {,,,}
   paul

   Revision 1.192  2001/06/07 09:52:26  clip
   uri:bugs

   Revision 1.191  2001/06/03 13:15:15  clip
   _SET_MBLOCKSIZE added. Default is 64.
   rust

   Revision 1.190  2001/05/31 12:27:20  clip
   _clip_clip
   paul

   Revision 1.189  2001/05/30 09:45:43  clip
   for .. in .. [keys]
   paul

   Revision 1.188  2001/05/25 10:16:22  clip
   small bug in _clip_str_to_date()
   rust

   Revision 1.187  2001/05/14 13:25:42  clip
   memdebug
   paul

   Revision 1.186  2001/05/12 10:23:51  clip
   uri: ctod(" nondigitSymbols dd/mm/gg") bug translate

   Revision 1.185  2001/05/11 09:11:57  clip
   errorblock & pcode
   paul

   Revision 1.184  2001/05/11 08:36:10  clip
   replace & pcode
   paul

   Revision 1.183  2001/05/11 08:16:39  clip
   memory leak, unary minus, type with field in rt compiler
   paul

   Revision 1.182  2001/05/04 10:58:45  clip
   uri: bug in str_to_date with format as " 1. 4.  01"

   Revision 1.181  2001/05/04 08:38:26  clip
   assign_field with macro
   paul

   Revision 1.180  2001/05/04 07:05:15  clip
   procline filename
   paul

   Revision 1.179  2001/04/27 08:07:22  clip
   error handler
   paul

   Revision 1.178  2001/04/24 06:32:30  clip
   change logging format: add PID
   paul

   Revision 1.177  2001/04/23 11:55:29  clip
   break trap
   paul

   Revision 1.176  2001/04/23 10:58:54  clip
   asdf
   paul

   Revision 1.175  2001/04/23 10:27:21  clip
   runtime error
   paul

   Revision 1.174  2001/04/23 10:01:10  clip
   drop functions call in type()
   paul

   Revision 1.173  2001/04/20 09:57:32  clip
   uplocals in _clip_dup
   paul

   Revision 1.172  2001/04/20 09:48:57  clip
   uplocals in _clip_clone
   paul

   Revision 1.171  2001/04/16 07:29:39  clip
   -> ( expr_list )
   paul

   Revision 1.170  2001/04/11 07:42:57  clip
   uri: fix bug "double / 0 == nan"

   Revision 1.169  2001/04/10 12:45:43  clip
   print deep limit
   paul

   Revision 1.168  2001/04/10 12:11:19  clip
   uri: add command "set translate path"

   Revision 1.167  2001/04/09 12:11:40  clip
   _clip_cmp
   paul

   Revision 1.166  2001/04/09 11:21:07  clip
   _clip_cmp bugfix
   paul

   Revision 1.165  2001/04/09 11:12:27  clip
   ASORT,ASCAN work with mishalleous types in array
   add additional parameter to _clip_cmp
   paul

   Revision 1.164  2001/04/05 10:28:48  clip
   for date/iif
   paul

   Revision 1.163  2001/04/03 09:17:00  clip
   license errors
   paul

   Revision 1.162  2001/03/30 11:51:03  clip
   add copyright

   Revision 1.161  2001/03/27 11:10:58  clip
   err if map wrond map index
   paul

   Revision 1.160  2001/03/26 09:47:47  clip
   continue
   paul

   Revision 1.159  2001/03/26 05:48:16  clip
   privates
   paul

   Revision 1.158  2001/03/23 12:09:36  clip
   ASORT
   paul

   Revision 1.157  2001/03/19 10:16:21  clip
   hashnames
   paul

   Revision 1.156  2001/03/16 09:42:16  clip
   next/step cleanups
   paul

   Revision 1.155  2001/03/16 08:56:10  clip
   next command clenup
   paul

   Revision 1.154  2001/03/12 13:47:18  clip
   VAR2STR, STR2VAR funcs
   paul

   Revision 1.153  2001/03/04 12:00:57  clip
   .. in translate_path()

   Revision 1.152  2001/02/24 12:47:52  clip
   wrong dimention order in new_array
   paul

   Revision 1.151  2001/02/23 12:02:37  clip
   TYPE and no-variable
   paul

   Revision 1.150  2001/02/23 11:12:15  clip
   args processing
   paul

   Revision 1.149  2001/02/23 10:57:40  clip
   (null).log bug if --debug option
   paul

   Revision 1.148  2001/02/23 09:20:11  clip
   GLOB function
   trailing ?? are valid
   paul

   Revision 1.147  2001/02/23 07:18:59  clip
   string == bug
   paul

   Revision 1.146  2001/02/22 11:37:38  clip
   start data_command
   paul

   Revision 1.145  2001/02/22 09:53:36  clip
   dbg: expr command
   paul

   Revision 1.144  2001/02/21 12:52:59  clip
   append HASHNAME(hash) - inverce to HASHSTR(str)

   append obj:modify() trigger:
   modify(obj,hash,newval) -> real new val
   MAPMODIFY(obj, enable) switch modify processing

   paul

   Revision 1.143  2001/02/20 11:01:03  clip
   append --stop hidden option - to stop/wait debugger connection before MAIN execution
   paul

   Revision 1.142  2001/02/20 07:38:23  clip
   calls like
   s:substr(2,3)
   are now equivalent to
   substr(s,2,3)
   paul

   Revision 1.141  2001/02/19 13:38:41  clip
   argv[0] bug
   paul

   Revision 1.140  2001/02/19 13:18:37  clip
   hidden option runtime recognition, currently
   one option: --debug[=debuglevel] - to enable runtime debug connection
   paul

   Revision 1.139  2001/02/07 11:47:15  clip
   localdefs
   paul

   Revision 1.138  2001/02/06 13:07:25  clip
   hash in .po, dbg
   paul

   Revision 1.137  2001/02/01 13:05:06  clip
   debug links
   paul

   Revision 1.136  2001/01/28 13:15:26  clip
   _clip_mgetl() - get a logical value from object element

   Revision 1.135  2001/01/26 09:42:20  clip
   small bug in _clip_op(...,'E'): strcmp -> strncmp
   rust

   Revision 1.134  2000/12/26 10:03:42  clip
   load now can load .so
   -s generate .so
   paul

   Revision 1.133  2000/12/25 11:35:18  clip
   call errorblock
   paul

   Revision 1.132  2000/12/20 13:00:37  clip
   privates in macro
   paul

   Revision 1.131  2000/12/20 12:35:13  clip
   privates in macro
   empty arrays in macro
   paul

   Revision 1.130  2000/12/11 10:13:57  clip
   locale setting
   paul

   Revision 1.129  2000/12/11 09:47:15  clip
   append setlocale() call
   all libC string functions now must work correct with correct charsets
   paul

   Revision 1.128  2000/12/10 07:12:01  clip
   locale bug
   paul

   Revision 1.127  2000/12/09 12:13:28  clip
   locale
   paul

   Revision 1.126  2000/12/08 12:59:50  clip
   rus kbd and caps_lock flags

   Revision 1.125  2000/12/08 08:29:22  clip
   privates
   paul

   Revision 1.124  2000/12/07 13:01:27  clip
   privates bug
   paul

   Revision 1.123  2000/12/07 07:36:48  clip
   default BINDIR now $HOME/bin
   add set_sysvars
   paul

   Revision 1.122  2000/12/06 13:13:01  clip
   loadable charset tables. size 256
   paul

   Revision 1.121  2000/12/06 07:34:52  clip
   unicode
   paul

   Revision 1.120  2000/12/04 10:09:49  clip
   keymaps
   paul

   Revision 1.119  2000/12/02 13:47:40  clip
   CLIPROOT now may be envar
   paul

   Revision 1.118  2000/12/02 12:46:57  clip
   scancodes
   paul

   Revision 1.117  2000/11/29 08:20:39  clip
   x call
   Paul Lasarev <paul@itk.ru>

   Revision 1.116  2000/11/28 14:01:10  clip
   serg: ��������� X-window � GTK,
   ����xcreatewindow, xcreatebutton

   Revision 1.115  2000/11/24 12:03:09  clip
   append _clip_mget()
   paul

   Revision 1.114  2000/11/23 12:56:18  clip
   keys, ADEL
   paul

   Revision 1.113  2000/11/23 06:50:23  clip
   load(name [,array])
   paul

   Revision 1.112  2000/11/22 14:52:19  clip
   .po now can be start with any (for example #!/usr/bin/cliprun) string
   paul

   Revision 1.111  2000/11/22 13:52:14  clip
   cpcode init sequence
   paul

   Revision 1.110  2000/11/22 13:45:14  clip

   paul

   Revision 1.109  2000/11/22 11:50:44  clip
   pcode link sequence
   paul

   Revision 1.108  2000/11/22 09:37:42  clip
   init procedure in pcode
   paul

   Revision 1.107  2000/11/21 11:31:12  clip
   io
   paul

   Revision 1.106  2000/11/20 12:59:01  clip
   10.2 default decimals
   destroy method in objs
   paul

   Revision 1.105  2000/11/20 10:36:38  clip
   ISFUNCTION(name)
   paul

   Revision 1.104  2000/11/20 09:59:04  clip
   preprocessor
   #command CMND [<a>] [SIZE <as>] OF <asd> => ....
   paul

   Revision 1.103  2000/11/08 13:49:42  clip
   serg: �������������� ����� ������fopen:
   fopen( "-|" ) - ��������
   fopen( "|-" ) - ���������   fopen( "ls -l |" ) - � ����������������   fopen( "| gzip > a.gz" ) - stdout ���gzip ������ ���.

   Revision 1.102  2000/11/04 11:00:11  clip
   sync_mp
   paul

   Revision 1.101  2000/10/31 09:55:02  clip
   obj['ASDF'] now equivalent to obj[`ASDF`]
   paul

   Revision 1.100  2000/10/27 10:40:56  clip
   bug in rational_adda
   from uri:

   Revision 1.99  2000/10/26 08:11:57  clip
   rationals bugs
   paul

   Revision 1.98  2000/10/25 13:33:17  clip
   memleak in < >
   Paul Lasarev <paul@itk.ru>

   Revision 1.97  2000/10/25 12:52:46  clip
   for..next
   Paul Lasarev <paul@itk.ru>

   Revision 1.96  2000/10/20 08:03:23  clip
   Added rational wherever mentioned NUMERIC_t
   maybe in some places this omitted
   (in places where don't use NUMERIC_t)!!!
   from uri:

   Revision 1.95  2000/10/18 10:56:17  clip
   append rationals
   Paul Lasarev <paul@itk.ru>

   Revision 1.94  2000/09/27 11:30:55  clip
   small fix decimal ����
   from uri:

   Revision 1.93  2000/09/27 09:33:55  clip
   ��������� ��������������������   from uri:

   Revision 1.92  2000/09/06 12:16:41  clip
   rust: bug in _clip_destroy_c_item
   rustCVS: ----------------------------------------------------------------------

   Revision 1.91  2000/08/24 13:54:25  clip
   serg: container, fopen, fread, fwrite, ...

   Revision 1.90  2000/05/31 19:19:09  clip
   uri: fix NaN and Inf  for divide by 0

   Revision 1.89  2000/05/26 23:02:28  clip
   0xXXXXXXXX
   Paul Lasarev <paul@itk.ru>

   Revision 1.88  2000/05/25 19:29:38  clip
   restscreen in window
   Paul Lasarev <paul@itk.ru>

   Revision 1.87  2000/05/25 18:30:11  clip
   tmp fix sigsegv on quit
   Paul Lasarev <paul@itk.ru>

   Revision 1.86  2000/05/24 18:34:15  clip
   _clip_push_area
   indents all
   Paul Lasarev <paul@itk.ru>

   Revision 1.85  2000/05/24 16:56:32  clip
   bugs
   Paul Lasarev <paul@itk.ru>

   Revision 1.84  2000/05/23 23:05:38  clip
   uri: � ����� ���
   Revision 1.83  2000/05/23 18:59:54  clip
   append window functions
   Paul Lasarev <paul@itk.ru>

   Revision 1.82  2000/05/20 20:18:09  clip
   negative aliases
   Paul Lasarev <paul@itk.ru>

   Revision 1.81  2000/05/20 19:18:21  clip
   bug with new hashstr
   Paul Lasarev <paul@itk.ru>

   Revision 1.80  2000/05/20 18:37:30  clip
   change hash function
   append HASH_xxxx extraction into hashcode.h
   Paul Lasarev <paul@itk.ru>

   Revision 1.79  2000/05/19 21:32:08  clip
   memleak in macro
   Paul Lasarev <paul@itk.ru>

   Revision 1.78  2000/05/19 20:19:20  clip
   attr_uN init
   Paul Lasarev <paul@itk.ru>

   Revision 1.77  2000/05/19 19:37:31  clip
   &macro->field
   case-independence
   Paul Lasarev <paul@itk.ru>

   Revision 1.76  2000/05/19 18:53:11  clip
   macro in alias expression
   Paul Lasarev <paul@itk.ru>

   Revision 1.75  2000/05/16 16:22:18  clip
   memory leak
   Paul Lasarev <paul@itk.ru>

   Revision 1.74  2000/05/16 15:11:21  clip
   bad realloc in alltrim
   Paul Lasarev <paul@itk.ru>

   Revision 1.73  2000/05/12 21:39:01  clip
   fix error messages return & idx continue (axl)

   Revision 1.72  2000/05/12 19:44:35  clip
   memory leak
   Paul Lasarev <paul@itk.ru>

   Revision 1.71  2000/05/12 17:08:19  clip
   add error:syserr member
   Paul Lasarev <paul@itk.ru>

   Revision 1.70  2000/05/11 22:33:28  clip
   &xxx. expand
   Paul Lasarev <paul@itk.ru>

   Revision 1.69  2000/05/10 18:33:48  clip
   AADD now clone arrays
   terminal initialization fail only in fullscreen mode
   Paul Lasarev <paul@itk.ru>

   Revision 1.68  2000/05/06 17:23:38  clip
   paul: _clip_cmp now return in *ret <0 0 >0

   Revision 1.67  2000/05/03 19:32:36  clip
   add prefix 'clip_' to all clip functions

   Revision 1.66  2000/04/30 19:35:36  clip
   #translate; obj attributes

   Revision 1.65  2000/04/28 15:25:35  clip
   memo

   Revision 1.64  2000/04/26 22:35:45  clip
   t.memo = 0 in _clip_destroy

   Revision 1.63  2000/04/15 20:57:33  paul
   mandrake fixes

   Revision 1.62  2000/04/11 15:33:00  paul
   memo

   Revision 1.61  2000/04/10 21:42:15  paul
   field->&asdf := expr

   Revision 1.60  2000/04/07 19:59:11  paul
   clip_store_item_destroy

   Revision 1.59  2000/04/01 20:16:35  paul
   case-insensitive _clip_glob_match

   Revision 1.58  2000/03/31 17:27:51  paul
   memfile

   Revision 1.57  2000/03/31 16:53:23  paul
   memfile rw

   Revision 1.56  2000/03/30 20:57:00  axl
   _clip_translate_path -> full unix file name
   remove dbrename & dbremove from _dbf.c

   Revision 1.55  2000/03/30 18:48:23  paul
   pcount

   Revision 1.54  2000/03/30 18:13:20  paul
   fields in macro

   Revision 1.53  2000/03/30 17:04:25  paul
   param

   Revision 1.52  2000/03/30 16:50:28  paul
   param

   Revision 1.51  2000/03/30 16:25:12  paul
   PARAM

   Revision 1.50  2000/03/29 22:36:23  paul
   clip_trap_err

   Revision 1.49  2000/03/29 21:20:48  paul
   only first errblock

   Revision 1.48  2000/03/29 21:15:54  paul
   errorblock nest limited

   Revision 1.47  2000/03/29 20:43:42  paul
   break loop

   Revision 1.46  2000/03/29 19:35:10  paul
   retcode

   Revision 1.45  2000/03/29 18:26:14  paul
   errorblock

   Revision 1.44  2000/03/25 18:02:23  uri
   outlog - first parameter - as level

   Revision 1.43  2000/03/24 19:49:58  uri
   set(_SET_LOGFILE)

   Revision 1.42  2000/03/24 19:02:54  paul
   logg

   Revision 1.41  2000/03/24 18:47:55  uri
   OUTLOG

   Revision 1.40  2000/03/24 18:10:10  paul
   trace

   Revision 1.39  2000/03/24 17:28:44  paul
   call tracing

   Revision 1.38  2000/03/24 17:27:58  uri
   bug in set(_SET_LOGLEVEL) and _clip_loglevel

   Revision 1.37  2000/03/22 19:39:45  paul
   *** empty log message ***

   Revision 1.36  2000/03/22 19:22:21  paul
   *** empty log message ***

   Revision 1.35  2000/03/22 19:04:16  paul
   *** empty log message ***

   Revision 1.34  2000/03/22 18:58:14  paul
   *** empty log message ***

   Revision 1.33  2000/03/22 18:44:31  paul
   *** empty log message ***

   Revision 1.32  2000/03/22 18:38:36  paul
   set

   Revision 1.31  2000/03/22 17:11:51  paul
   parameters by reference

   Revision 1.30  2000/03/21 16:04:19  paul
   .po call before .c

   Revision 1.29  2000/03/21 14:53:27  paul
   macroassign; __FIELD__ with macroassign

   Revision 1.28  2000/03/17 16:27:24  paul
   add ISFIELD ISMEMVAR

   Revision 1.27  2000/03/14 18:19:44  paul
   add _clip_take_field, fix bug in _clip_eval_macro with simple fields

   Revision 1.26  2000/03/10 15:55:53  paul
   memory leak, xon/xoff gluk

   Revision 1.25  2000/03/09 22:24:23  paul
   mem leak

   Revision 1.24  2000/03/09 21:40:31  paul
   memdebug removed

   Revision 1.23  2000/03/09 21:02:52  paul
   memory leak with objs, clip_cmp bug

   Revision 1.22  2000/03/07 20:57:17  paul
   *** empty log message ***

   Revision 1.21  2000/03/07 20:54:59  paul
   _clip_cmp with zero string

   Revision 1.20  2000/03/06 16:54:30  paul
   gluk with absent map fields fetch

   Revision 1.19  2000/02/21 23:01:28  serg
   *** empty log message ***

   Revision 1.18  1999/12/10 21:16:38  uri
   �typename ����MEMO_t

   Revision 1.17  1999/12/10 21:14:02  paul
   gluk in _clip_date_to_str()

   Revision 1.16  1999/12/10 17:28:32  uri
   ���������� �cliprt, ����MEMO_t, ��� �����misc
   ����� ���

   Revision 1.15  1999/12/04 21:25:36  uri
   ����*DATE* ����� ���

   Revision 1.14  1999/11/30 18:39:09  paul
   ref bug

   Revision 1.13  1999/11/30 18:29:39  paul
   ref bug fix

   Revision 1.12  1999/11/09 22:49:14  axl
   Created DBFMEM - dbf in memory.

   Revision 1.11  1999/10/29 19:28:31  paul
   bug in macroassign (stack was corrupted)

   Revision 1.10  1999/10/29 14:02:13  paul
   macroassign now worked

   Revision 1.9  1999/10/27 19:48:14  uri
   type & mptr

   Revision 1.8  1999/10/27 18:49:35  paul
   rptr

   Revision 1.7  1999/10/27 18:36:48  paul
   type of refvars

   Revision 1.6  1999/10/27 17:29:33  paul
   change param handling

   Revision 1.5  1999/10/26 18:38:36  paul
   fff

 */

#include "clipcfg.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#ifdef OS_MINGW
	#include <ltdl.h>
#else
	#include <dlfcn.h>
#endif
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#include "clip.h"
#include "clipvm.h"
#include "hash.h"
#include "screen/screen.h"
#include "error.ch"
#include "hashcode.h"
#include "rational.h"
#include "screen/charset.h"
#include "coll.h"
#include "license.h"

#ifdef USE_TASKS
#include "task/task.h"
#endif

#define NEW(type) calloc(sizeof(type),1)
#define NEWVECT(type,len) calloc(sizeof(type),(len))
#define VAR(type,var,ini) type *var=(type*)(ini)
#define NEWVAR(type,var) type *var=calloc(sizeof(type),1)
#define c_DELETE(type,var)	{destroy_##type(var);free(var);}

#define EXPAND_MACRO

#define MIN_NUMERIC_DIFF 0.00000000000001

extern CLIP_DLLEXPORT void _clip_close_all(ClipMachine * mp);
extern int clip_RDDCLOSEALL(ClipMachine * mp);

#ifdef FORCEALIGN

static long
GETLONG(void *p)
{
	long r;

	memcpy(&r, p, sizeof(r));
	return r;
}

static void
SETLONG(void *ptr, long l)
{
	memcpy(ptr, &l, sizeof(l));
}

#else

#define GETLONG(p) (*(long*)(p))
#define SETLONG(ptr,l) (*(long*)(ptr)=(l))

#endif

static ClipInitStruct *init_struct = 0;
char *_clip_hostcs = "CP437";

CLIP_DLLEXPORT void
_clip_init_struct(ClipInitStruct * sp)
{
	if (!init_struct)
		init_struct = sp;
}

static const char __file__[] = __FILE__;

int search_map(ClipVarEl * items, int count, long key, int *index);
static ClipVarEl *get_map_el(ClipMapVar * a, long no);
static void remove_private_vect(ClipMachine * mp, int num, void *vect);
static int descr_str(ClipMachine * mp, ClipVar * vp, char **str);
static void delete_ClipVarFrame(ClipMachine * mp, ClipVarFrame * localvars);

char *CLIPROOT = CLIP_ROOT;
char *CLIP_LOCALE_ROOT = CLIP_ROOT;

static VarEntry *add_private(ClipMachine * mp, long hash);
static VarEntry *fetch_var(ClipMachine * mp, long hash);

static void dup_ref(ClipVar * dest, ClipVar * src);
static ClipVar *clip_fetch(ClipMachine * mp, int dim, int push, int store, ClipVar ** mapp, long *hashp);

int _clip_profiler = 0;
static void fini_profiler(ClipMachine *mp);


#include "cliprt_machine.c"
#include "cliprt_params.c"
#include "cliprt_arrays.c"
#include "cliprt_var.c"
#include "cliprt_utils.c"
