/*
 * Copyright (C) 2013-2014 brainpower <brainpower at gulli dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CHECKARG_H
#define CHECKARG_H

#ifndef NDEBUG
#define CA_PRINTERR
#endif

#include <stdint.h>
#include <stddef.h> /* size_t */

typedef struct _CheckArg CheckArg;
typedef struct _CheckArgPrivate CheckArgPrivate;
typedef int (*CheckArgFP)(CheckArg*, const char *,const char *);

struct _CheckArg {
  CheckArgPrivate *p;
};

CheckArg* checkarg_new(const int argc, char **argv, const char *appname, const char *desc, const char *appendix);

void checkarg_free(CheckArg*);

int checkarg_add(CheckArg*, const char sopt, const char *lopt, const char *help);
int checkarg_add_value(CheckArg*, const char sopt, const char *lopt, const char *help, const uint8_t has_val);

int checkarg_add_cb(CheckArg*, const char sopt, const char *lopt, CheckArgFP cb, const char *help);
int checkarg_add_cb_value(CheckArg*, const char sopt, const char *lopt, CheckArgFP cb, const char *help, const uint8_t has_val);

int checkarg_add_long(CheckArg*, const char *lopt, const char *help);
int checkarg_add_long_value(CheckArg*, const char *lopt, const char *help, const int8_t has_val);

int checkarg_add_long_cb(CheckArg*, const char *lopt, CheckArgFP cb, const char *help);
int checkarg_add_long_cb_value(CheckArg*, const char *lopt, CheckArgFP cb, const char *help, const uint8_t has_val);

int checkarg_add_autohelp(CheckArg*);

int checkarg_parse(CheckArg*);

int checkarg_set_posarg_help(CheckArg*, const char *usage, const char *descr);
int checkarg_set_usage_line(CheckArg*, const char *arg);

const char*  checkarg_argv0(CheckArg*);
const char** checkarg_pos_args(CheckArg*);
size_t       checkarg_pos_args_count(CheckArg*);

const char*  checkarg_value(CheckArg*, const char*);
uint8_t      checkarg_isset(CheckArg*, const char*);

const char*  checkarg_str_err(const int errno);


int checkarg_show_autohelp(CheckArg*, const char* larg, const char* val);

enum CAError{
  CA_ALLOK=0,
  CA_ERROR,
  CA_INVARG,
  CA_INVVAL,
  CA_MISSVAL,
  CA_CALLBACK,
  CA_ALLOC_ERR,
};

#endif /* CHECKARG_H */
