ifdef CLIPROOT
include $(CLIPROOT)/include/Makefile.inc
CLIP	= $(CLIPROOT)/bin/clip
CLIPFLAGS = -l -b -a -O -n -I .. -I $(CLIPROOT)/include -I $(CLIPROOT)/include/netto
else
CLIP = clip
CLIPFLAGS = -l -b -a -O -n -I ..
endif
CLIPLIBS  =

LIBS  = -lclip-netto -lclip-nanfor
OBJS  =	getsys.o common.o \
	help.o print.o \
	mdbedit.o mainmenu.o aktions.o obsolete.o \
	cliplib.o filer.o viewer.o editor.o foxpro.o \
	tb2html.o menu.o \
	_clip.o modistru.o vkrz.o \
	nanfor.o nettos.o \
	errorsys.o \
	config.o examplep.o

.SUFFIXES: .prg .o

#LANG=ru_RU.KOI8-R
#export LANG

all: bdbf$(EXESUFF) bdbf_d$(EXESUFF)

bdbf_d$(EXESUFF):	bdbf.prg bcommon.ch bdbfmsg.ch $(OBJS)
	$(CLIP) $(CLIPFLAGS) -s -e -obdbf_d$(EXESUFF) -l bdbf.prg $(OBJS) $(LIBS)

bdbf$(EXESUFF): bdbf.prg bcommon.ch bdbfmsg.ch $(OBJS)
	$(CLIP) $(CLIPFLAGS) -s -e --full-static -obdbf$(EXESUFF) -l bdbf.prg $(OBJS) $(LIBS)

clean:
	rm -rf *.o *.ppo *.c *.s *.a *.so *.b *.BAK *.bak *.log
	rm -rf bdbf *.exe *~ core* *core *.nm *.ex test

install:
	cp bdbf_d$(EXESUFF) $(DESTDIR)$(CLIPROOT)/bin/bdbf_d$(EXESUFF)
	cp bdbf$(EXESUFF) $(DESTDIR)$(CLIPROOT)/bin/bdbf$(EXESUFF)
	cp *.hlp $(DESTDIR)$(CLIPROOT)/bin/
	tar cf - locale.po | (cd $(DESTDIR)$(CLIPROOT) && tar xf -)
	rm -rf `find $(DESTDIR)$(CLIPROOT)/locale.po -type d -name CVS -print`

.prg.o:
	$(CLIP) $(CLIPFLAGS) $<

.prg.po:
	$(CLIP) $(CLIPFLAGS) -p $<

