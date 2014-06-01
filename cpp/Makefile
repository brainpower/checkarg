#!/usr/bin/make -f

CC=gcc
CXX=g++
LD=$(CC)
CXXLD=$(CXX)
RMF=rm -f
AR=ar
INSTALL=install
LNS=ln -s

CFLAGS=-std=c11 -Og -g -fPIC
CXXFLAGS=-std=c++11 -Og -g -fPIC
LIBS=
PREFIX=/usr

CAHDR=checkarg.hpp
CAHDRS=src/checkarg.hpp src/checkarg_private.hpp
CAOBJS=src/checkarg.o

LIBNAME=checkarg
SHLIB=libcheckarg.so.0.0.0
SHLIBM=libcheckarg.so.0
SHLIBN=libcheckarg.so
STLIB=libcheckarg.a
PCFILE=checkarg.pc

HDRS=$(CAHDRS)
OBJS=tests/test_args.o
EXEC=tests/test_args

.PHONY: clean check
.SUFFIXES: .cpp .c

.cpp.o:
	@echo -e "  \e[1mCXX     \e[0m$<"
	@$(CXX) $(CXXFLAGS) $< -c -o $@
	
#.cxx.o:
#	@echo "  CXX     $<"
#	@$(CXX) $(CXXFLAGS) $< -c -o $@
	
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
	@echo -e "  \e[1mCXXLD   \e[0m$@"
	@$(CXXLD) -shared -Wl,-soname,$(SHLIBM) -o $@ $(CAOBJS)

$(EXEC): $(EXEC).o $(OBJS) $(STLIB) $(HDRS)
	@echo -e "  \e[1mCXXLD   \e[0m$@"
	@$(CXX) $(LDFLAGS) $(OBJS) $(STLIB) -o $@ $(LIBS)

install: all
	@echo -e "  \e[1mINSTALL \e[0m$(SHLIB)"
	@$(INSTALL) -Dm755  $(SHLIB) $(DESTDIR)$(PREFIX)/lib/$(SHLIB)
	@echo -e "  \e[1mLN      \e[0m$(SHLIBM)"
	@$(LNS) $(SHLIB) $(DESTDIR)$(PREFIX)/lib/$(SHLIBM)
	@echo -e "  \e[1mLN      \e[0m$(SHLIBN)"
	@$(LNS) $(SHLIB) $(DESTDIR)$(PREFIX)/lib/$(SHLIBN)

	@echo -e "  \e[1mINSTALL \e[0m$(CAHDR)"
	@$(INSTALL) -Dm755  src/$(CAHDR) $(DESTDIR)$(PREFIX)/include/checkarg/$(CAHDR)
	
	@echo -e "  \e[1mINSTALL \e[0m$(PCFILE)"
	@$(INSTALL) -Dm755  data/$(PCFILE) $(DESTDIR)$(PREFIX)/lib/pkgconfig/$(PCFILE)

clean:
	@for e in $(OBJS) $(CAOBJS); do \
		echo -e "  \e[1mRM      \e[0m$$e" ;\
		$(RM) $$e ;\
	done;

	@echo -e "  \e[1mRM      \e[0m$(EXEC).o"
	@$(RM) $(EXEC).o
	
	@echo -e "  \e[1mRM      \e[0m$(EXEC)"
	@$(RM) $(EXEC)

	@echo -e "  \e[1mRM      \e[0m$(STLIB)"
	@$(RM) $(STLIB)

	@echo -e "  \e[1mRM      \e[0m$(SHLIB)"
	@$(RM) $(SHLIB)

check: $(EXEC) tests/run_tests.sh
	tests/run_tests.sh
