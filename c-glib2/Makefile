#!/usr/bin/make -f

CC=gcc
CXX=g++
CLD=$(CC)
CXXLD=$(CXX)
RMF=rm -f
AR=ar
PKG_CONFIG=pkg-config

CFLAGS=-std=c11 -Og -g -fPIC -Wall
CFLAGS+=`$(PKG_CONFIG) --cflags --libs glib-2.0`
CXXFLAGS=-std=c++11 -Og -g -fPIC
LIBS=

CAHDRS=src/checkarg.h
CAOBJS=src/checkarg.o

LIBNAME=checkarg
SHLIB=libcheckarg.so.0.0.0
SHLIBM=libcheckarg.so.0
STLIB=libcheckarg.a

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
	@$(CLD) -shared -Wl,-soname,$(SHLIBM) -o $@ $(CAOBJS)

$(EXEC): $(EXEC).o $(OBJS) $(STLIB) $(HDRS)
	@echo -e "  \e[1mCXXLD   \e[0m$@"
	@$(CC) $(LDFLAGS) $(OBJS) $(STLIB) -o $@ $(LIBS)


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
