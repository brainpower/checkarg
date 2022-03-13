// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2022 brainpower <brainpower at mailbox dot org>

#ifndef CHECKARG_H
#define CHECKARG_H

#include <stddef.h> /* size_t */
#include <stdint.h>

typedef struct _CheckArg CheckArg;
typedef struct _CheckArgPrivate CheckArgPrivate;
typedef int (*CheckArgFP)(CheckArg *, const char *, const char *);
typedef CheckArg *CheckArgPtr;

struct _CheckArg {
  CheckArgPrivate *p;
};

CheckArg *checkarg_new(const char *appname, const char *desc, const char *appendix);

void checkarg_free(CheckArg *);

int checkarg_add(
  CheckArg *, const char sopt, const char *lopt, const char *help,
  const uint8_t value_type, const char *value_name);

int checkarg_add_cb(
  CheckArg *, const char sopt, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name);

int checkarg_add_long(
  CheckArg *, const char *lopt, const char *help, const uint8_t value_type,
  const char *value_name);

int checkarg_add_long_cb(
  CheckArg *, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name);

int checkarg_add_autohelp(CheckArg *);

void checkarg_reset(CheckArg *);
int checkarg_parse(CheckArg *, const int argc, char **argv);

int checkarg_set_posarg_help(CheckArg *, const char *usage, const char *descr);
int checkarg_set_usage_line(CheckArg *, const char *arg);

const char *checkarg_argv0(CheckArg *);
const char *checkarg_callname(CheckArg *);
const char **checkarg_pos_args(CheckArg *);
size_t checkarg_pos_args_count(CheckArg *);

const char *checkarg_value(CheckArg *, const char *);
uint8_t checkarg_isset(CheckArg *, const char *);

const char *checkarg_str_err(const int errno);

char *checkarg_usage(CheckArg *);
char *checkarg_autohelp(CheckArg *);

void checkarg_show_help(CheckArg *);
void checkarg_show_usage(CheckArg *);

// FIXME: move this to private?
int checkarg_show_autohelp(CheckArg *, const char *larg, const char *val);

enum CAError {
  CA_ALLOK = 0,
  CA_ERROR,
  CA_INVOPT,
  CA_INVVAL,
  CA_MISSVAL,
  CA_CALLBACK,
  CA_ALLOC_ERR,
  CA_BUG,
};

enum CAValueType {
  CA_VT_NONE = 0,
  CA_VT_REQUIRED,
  // CA_VT_OPTIONAL,
};

#endif /* CHECKARG_H */

/* vim: set ft=c : */
