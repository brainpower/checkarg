/*
 * Copyright (c) 2013-2018 brainpower <brainpower at mailbox dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CHECKARG_PRIVATE_H
#  define CHECKARG_PRIVATE_H

#  include "checkarg.h"
#  include "config.h"

typedef struct _Opt Opt;
struct _Opt {
  uint8_t value_type;
  char sopt;
  char *lopt;
  char *help;
  CheckArgFP cb;
  char *value;
  char *value_name;
  Opt *next;
};

struct _CheckArgPrivate {

  Opt *valid_args;
  Opt *valid_args_last;
  const char **pos_args;
  size_t pos_args_count;

  int argc;
  char **argv;

  uint8_t pos_arg_sep;

  char *appname, *descr, *appendix, *usage_line, *posarg_help_usage, *posarg_help_descr,
    *next_is_val_of;
};


int checkarg_arg(CheckArg *, const char *arg);
int checkarg_arg_short(CheckArg *, const char *arg);
int checkarg_arg_long(CheckArg *, const char *arg);
int checkarg_call_cb(CheckArg *, const char *arg);

Opt *opt_new(
  const char sopt, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name);
void opt_free(Opt *o);

int ca_error(int eno, const char *fmt, ...);

int valid_args_insert(CheckArg *, Opt *opt);
Opt *valid_args_find(CheckArg *, const char *lopt);
Opt *valid_args_find_sopt(CheckArg *, char sopt);
void valid_args_free(Opt *vaptr);

void pos_args_append(CheckArg *, const char *);
void pos_args_free(const char **);

int call_cb(CheckArg *, Opt *);

// fixme should return the number of converted chars,
// and or error code
void string_toupper(char *);

#endif /* CHECKARG_PRIVATE_H */

/* vim: set ft=c : */
