#!/usr/bin/make -f

RMF=rm -f
INSTALL=install
LNS=ln -s

PREFIX?=/usr

LIB=checkarg.sh

.PHONY: clean check doc

all: $(LIB)

install: all
	@echo -e "  \e[1mINSTALL \e[0m$(LIB)"
	@$(INSTALL) -Dm755  $(LIB) $(DESTDIR)$(PREFIX)/lib/checkarg/$(LIB)

clean:
	true

check: tests/run_tests.sh
	tests/run_tests.sh
