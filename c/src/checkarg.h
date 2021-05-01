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

#ifndef CHECKARG_H
#define CHECKARG_H

#include <stdint.h>
#include <stddef.h> /* size_t */

typedef struct _CheckArg CheckArg;
typedef struct _CheckArgPrivate CheckArgPrivate;
typedef int (*CheckArgFP)(CheckArg*, const char *,const char *);
typedef CheckArg* CheckArgPtr;

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

char*  checkarg_usage(CheckArg*);
char*  checkarg_autohelp(CheckArg*);

void checkarg_show_help(CheckArg*);
void checkarg_show_usage(CheckArg*);

int checkarg_show_autohelp(CheckArg*, const char* larg, const char* val);

enum CAError{
  CA_ALLOK=0,
  CA_ERROR,
  CA_INVOPT,
  CA_INVVAL,
  CA_MISSVAL,
  CA_CALLBACK,
  CA_ALLOC_ERR,
};

#endif /* CHECKARG_H */

/* vim: set ft=c : */
