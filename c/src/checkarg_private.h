// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2022 brainpower <brainpower at mailbox dot org>

#ifndef CHECKARG_PRIVATE_H
#define CHECKARG_PRIVATE_H

#include "checkarg.h"
#include "config.h"

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

  uint8_t pos_arg_sep;
  uint8_t cleared;

  char *appname, *descr, *appendix, *usage_line, *posarg_help_usage, *posarg_help_descr,
    *next_is_val_of;
  char *callname;
};


static int checkarg_arg(CheckArg *, const char *arg);
static int checkarg_arg_short(CheckArg *, const char *arg);
static int checkarg_arg_long(CheckArg *, const char *arg);

static Opt *opt_new(
  const char sopt, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name);
static void opt_free(Opt *o);

static int ca_error(int eno, const char *fmt, ...);

static int valid_args_insert(CheckArg *, Opt *opt);
#if 0
static int valid_args_append(CheckArg *, Opt *opt);
#endif
static Opt *valid_args_find(CheckArg *, const char *lopt);
static Opt *valid_args_find_sopt(CheckArg *, char sopt);
static void valid_args_free(Opt *vaptr);

static void pos_args_append(CheckArg *, const char *);
static void pos_args_free(const char **);

static int call_cb(CheckArg *, Opt *);

// fixme should return the number of converted chars,
// and or error code
static void string_toupper(char *);

#endif /* CHECKARG_PRIVATE_H */

/* vim: set ft=c : */
