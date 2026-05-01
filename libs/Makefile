ifndef CLIPROOT
	CLIPROOT:=$(shell cd ../../cliproot 2>/dev/null && pwd || echo /usr/local/clip)
endif

-include $(CLIPROOT)/include/Makefile.inc

dirs = clip-gzip clip-bzip2 clip-crypto clip-fcgi \
       clip-mysql clip-postgres clip-odbc \
       clip-com clip-oasis clip-rtf clip-r2d2 \
       clip-xml clip-postscript
#      clip-ui         (requires clip-xml installed as a lib)
#      clip-gd         (no libgd-dev available)
#      clip-oracle     (proprietary, no dev headers available)
#      clip-interbase  (no dev headers available)
#      clip-gtk2       (GTK2 removed from Debian 13+)
#      clip-glade2     (GTK2 removed from Debian 13+)

#clip-cti  clip-fw clip-gtk clip-gtkextra

all local user system install lib:
	for dir in $(dirs); \
	do \
		(echo --- $$dir >> ${LOG};\
		cd $$dir;\
		if [ -f configure ]; then\
			./configure $@ && [ $$? = 0 ] && $(MAKE) $@; \
		else \
			$(MAKE) $@; \
		fi); \
	done

clean:
	for dir in $(dirs); \
		do (cd $$dir && $(MAKE) clean || true; rm -rf debian build-stamp $dir.spec);\
	done

distclean: clean
	for dir in $(dirs); \
		do (cd $$dir && $(MAKE) distclean || true);\
	done

deb: clean
	for dir in $(dirs); \
		do ./mkdeb $$dir; \
	done

rpm:
	for dir in $(dirs); \
		do ./mkrpm $$dir; \
	done

rpm-opt:
	for dir in $(dirs); \
		do ./mkrpm -opt $$dir; \
	done

tgz tbz:
	for dir in $(dirs); \
		do ./mktgz $$dir; \
	done


