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

#ifndef CHECKARG_PRIVATE_H
#define CHECKARG_PRIVATE_H

#include "checkarg.h"

typedef uint8_t bool;

typedef struct _Opt Opt;
struct _Opt {
  uint8_t     has_val;
  char        sopt;
  char       *lopt;
  char       *help;
  CheckArgFP  cb;
  char       *value;
  Opt        *next;
};

struct _CheckArgPrivate {

  Opt   *valid_args;
  Opt   *valid_args_last;
  const char **pos_args;
  size_t pos_args_count;

  int    argc;
  char **argv;

  bool autohelp_on;
  bool pos_arg_sep;

  char *appname,
       *descr,
       *appendix,
       *usage_line,
       *posarg_help_usage,
       *posarg_help_descr,
       *next_is_val_of;

};


int checkarg_arg(CheckArg*, const char *arg);
int checkarg_arg_short(CheckArg*, const char *arg);
int checkarg_arg_long(CheckArg*, const char *arg);
int checkarg_call_cb(CheckArg*, const char *arg);

Opt* opt_new(const char sopt, const char *lopt, CheckArgFP cb, const char *help, const uint8_t has_val);
void opt_free(Opt *o);

int ca_error(int eno, const char *fmt, ...);

int  valid_args_insert(CheckArg*, Opt *opt);
Opt* valid_args_find(CheckArg*, const char *lopt);
Opt* valid_args_find_sopt(CheckArg*, char sopt);
void valid_args_free(Opt* vaptr);

void pos_args_append(CheckArg*, const char *);
void pos_args_free(const char **);

int call_cb(CheckArg*, Opt*);

#endif /* CHECKARG_PRIVATE_H */
