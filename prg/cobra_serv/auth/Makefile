ifdef CLIPROOT
include $(CLIPROOT)/include/Makefile.inc
endif
CLIP	= $(CLIPROOT)/bin/clip

# need package libpam0g-dev

LIBS = -lpam
CC = gcc
CFLAGS = -O

all:
	@if find /usr/include -iname pam_appl.h 2>/dev/null | grep -q pam_appl.h; then \
		$(MAKE) pam-auth; \
	else \
		echo '*******************************************'; \
		echo 'Warning: libpam0g-dev package not installed'; \
		echo 'Warning: pam-auth compilation skipped'; \
		echo '*******************************************'; \
	fi

pam-auth: pam-auth.c
	$(CC) $(CFLAGS) -o pam-auth pam-auth.c $(LIBS)

clean:
	rm -f pam-auth *.bak *.o

install:
	mkdir -p $(DESTDIR)$(CLIPROOT)/cobra/auth
	cd $(DESTDIR)$(CLIPROOT)/cobra/auth; rm -f pam-auth
	cp Makefile make_mod.sh other pam* $(DESTDIR)$(CLIPROOT)/cobra/auth
	cd $(DESTDIR)$(CLIPROOT)/cobra/auth; test -z $(DESTDIR) && ./make_mod.sh || exit 0

