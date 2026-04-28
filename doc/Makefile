ifdef CLIPROOT
	BIN=$(CLIPROOT)/bin
else
	CLIPROOT:=$(shell cd ../../cliproot; pwd)
	BIN:=$(shell cd ../clip; pwd)
endif

#LANGS = en ru hu es pt ua
LANGS = en ru
PACKAGE = fulldoc
MAIN = index

ROOT := $(shell cd ..; pwd)
PWD := $(shell pwd)

DIRS =  clip/doc libs/clip-gzip libs/clip-bzip2 \
	libs/clip-crypto libs/clip-fcgi \
	libs/clip-com

all: html

install:
	-for L in $(LANGS); \
	do \
		mkdir -p $(DESTDIR)$(CLIPROOT)/doc/html-full/$$L ;\
		cp -R html/$$L/* $(DESTDIR)$(CLIPROOT)/doc/html-full/$$L/ 2>/dev/null || true;\
	done


#include $(CLIPROOT)/include/Makefile.tdoc
include ../clip/Makefile.tdoc
