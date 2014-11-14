#!/usr/bin/make -f

MVER=1
VER=$(MVER).2.0

CC=gcc
LD=$(CC)
RMF=rm -f
AR=ar
INSTALL=install
LNS=ln -s

CFLAGS=-std=gnu11 -Og -g -fPIC -Wall
LIBS=
PREFIX=/usr

LIBNAME=checkarg
CAHDR=$(LIBNAME).h
CAHDRS=src/$(LIBNAME).h src/checkarg_private.h
CAOBJS=src/$(LIBNAME).o

SHLIB=lib$(LIBNAME).so.$(VER)
SHLIBM=lib$(LIBNAME).so.$(MVER)
SHLIBN=lib$(LIBNAME).so
STLIB=lib$(LIBNAME).a
PCFILE=$(LIBNAME).pc

.PHONY: clean check doc
.SUFFIXES: .c

.c.o:
	@echo -e "  \e[1mCC      \e[0m$<"
	@$(CC) $(CFLAGS) $< -c -o $@


all: shared

shared: $(SHLIB)

static: $(STLIB)

$(STLIB): $(CAOBJS)
	@echo -e "  \e[1mAR      \e[0m$@"
	@$(AR) rcs $@ $(CAOBJS)

$(SHLIB): $(CAOBJS)
	@echo -e "  \e[1mLD      \e[0m$@"
	@$(LD) -shared -Wl,-soname,$(SHLIBM) -o $@ $(CAOBJS)

doc:
	@echo -e "  \e[1mDOXYGEN \e[0m"
	@cd doc && doxygen

install: all
	@echo -e "  \e[1mINSTALL \e[0m$(SHLIB)"
	@$(INSTALL) -Dm755  $(SHLIB) $(DESTDIR)$(PREFIX)/lib/$(SHLIB)
	@echo -e "  \e[1mLN      \e[0m$(SHLIBM)"
	@$(LNS) $(SHLIB) $(DESTDIR)$(PREFIX)/lib/$(SHLIBM)
	@echo -e "  \e[1mLN      \e[0m$(SHLIBN)"
	@$(LNS) $(SHLIB) $(DESTDIR)$(PREFIX)/lib/$(SHLIBN)

	@echo -e "  \e[1mINSTALL \e[0m$(CAHDR)"
	@$(INSTALL) -Dm755  src/$(CAHDR) $(DESTDIR)$(PREFIX)/include/$(LIBNAME)/$(CAHDR)

	@echo -e "  \e[1mINSTALL \e[0m$(PCFILE)"
	@$(INSTALL) -Dm755  data/$(PCFILE) $(DESTDIR)$(PREFIX)/lib/pkgconfig/$(PCFILE)

clean:
	@for e in $(OBJS) $(CAOBJS); do \
		echo -e "  \e[1mRM      \e[0m$$e" ;\
		$(RM) $$e ;\
	done;

	@echo -e "  \e[1mRM      \e[0m$(STLIB)"
	@$(RM) $(STLIB)

	@echo -e "  \e[1mRM      \e[0m$(SHLIB)"
	@$(RM) $(SHLIB)

check: static tests/run_tests.sh
	tests/run_tests.sh
