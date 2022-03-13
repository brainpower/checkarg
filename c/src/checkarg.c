// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2022 brainpower <brainpower at mailbox dot org>

#include "checkarg_private.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *errors[] = {
  /*CA_ALLOK    */ "Everything is fine",
  /*CA_ERROR    */ "An Error occurred",
  /*CA_INVOPT   */ "Unknown command line option",
  /*CA_INVVAL   */ "Value given to non-value option",
  /*CA_MISSVAL  */ "Missing value of option",
  /*CA_CALLBACK */ "Callback returned with error code",
  /*CA_ALLOC_ERR*/ "Allocation of memory failed",
  /*CA_BUG      */
  "A wild Bug appeared. This means code has been executed, that should never been"};


/* c'tors */

CheckArg *
checkarg_new(const char *appname, const char *desc, const char *appendix) {

  CheckArg *ret         = NULL;
  CheckArgPrivate *priv = NULL;

  ret = (CheckArg *)malloc(sizeof(CheckArg));
  if (!ret) return NULL; /* malloc failed */

  priv = (CheckArgPrivate *)malloc(sizeof(CheckArgPrivate));
  if (!priv) /* malloc failed */
    goto clean;

  ret->p = priv;

  /* initialize struct with zeros #C99# */
  *priv = (CheckArgPrivate){0};

  priv->appname = strdup(appname);
  if (!priv->appname) goto clean;

  priv->usage_line = (char *)malloc(strlen(appname) + 11);
  if (!priv->usage_line) goto clean;

  *priv->usage_line =
    '\0'; /* set first char to \0, so strcat will start at the beginning */
  strcat(priv->usage_line, priv->appname);
  strcat(priv->usage_line, " [options]");

  if (desc) {
    priv->descr = strdup(desc);
    if (!priv->descr) goto clean;
  }

  if (appendix) {
    priv->appendix = strdup(appendix);
    if (!priv->appendix) goto clean;
  }

  /* just in case, zeroing the struct should have already done this */
  priv->pos_args = NULL;
  priv->callname = NULL;

  return ret;

/* clean up and return in case of error */
clean:
  checkarg_free(ret);
  return NULL;
}

void
checkarg_free(CheckArg *ca) {
  if (ca && ca->p) { /* makes free of partially initialized ca possible */
    free(ca->p->appname);
    free(ca->p->descr);
    free(ca->p->appendix);
    free(ca->p->usage_line);
    free(ca->p->next_is_val_of);
    free(ca->p->posarg_help_descr);
    free(ca->p->posarg_help_usage);
    free(ca->p->callname);

    /* "arrays" and lists */
    pos_args_free(ca->p->pos_args);
    valid_args_free(ca->p->valid_args);

    free(ca->p);
  }
  free(ca);
}

int
checkarg_add(
  CheckArg *ca, const char sopt, const char *lopt, const char *help,
  const uint8_t value_type, const char *value_name) {

  Opt *opt = opt_new(sopt, lopt, NULL, help, value_type, value_name);
  if (!opt) return CA_ALLOC_ERR; /* malloc failed */

  return valid_args_insert(ca, opt);
}

int
checkarg_add_cb(
  CheckArg *ca, const char sopt, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name) {

  Opt *opt = opt_new(sopt, lopt, cb, help, value_type, value_name);
  if (!opt) return CA_ALLOC_ERR;

  return valid_args_insert(ca, opt);
}

int
checkarg_add_long(
  CheckArg *ca, const char *lopt, const char *help, const uint8_t value_type,
  const char *value_name) {

  Opt *opt = opt_new(0, lopt, NULL, help, value_type, value_name);
  if (!opt) return CA_ALLOC_ERR;
  return valid_args_insert(ca, opt);
}

int
checkarg_add_long_cb(
  CheckArg *ca, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name) {

  Opt *opt = opt_new(0, lopt, cb, help, value_type, value_name);
  if (!opt) return CA_ALLOC_ERR;
  return valid_args_insert(ca, opt);
}

int
checkarg_add_autohelp(CheckArg *ca) {
  Opt *opt = opt_new(
    'h', "help", checkarg_show_autohelp, "show this help message and exit", 0, NULL);
  if (!opt) return CA_ALLOC_ERR;
  return valid_args_insert(ca, opt);
}

void
checkarg_reset(CheckArg *ca) {
  free(ca->p->callname);
  ca->p->callname = NULL;

  ca->p->pos_arg_sep = 0;
  free(ca->p->pos_args);
  ca->p->pos_args = NULL;

  free(ca->p->next_is_val_of);
  ca->p->next_is_val_of = NULL;

  Opt *it = ca->p->valid_args;
  if (it) {
    do {
      free(it->value);
      it->value = NULL;
    }
    while ((it = it->next));
  }

  ca->p->cleared = 1;
}

int
checkarg_parse(CheckArg *ca, const int argc, char **argv) {
  int ret = CA_ALLOK;
  int i;

  if (argc < 1) {
    fprintf(stderr, "argc must be at least 1");
    ret = CA_ERROR;
    goto error;
  }
  if (!argv) {
    fprintf(stderr, "argv must not be NULL");
    ret = CA_ERROR;
    goto error;
  }
  if (!argv[0]) {
    fprintf(stderr, "argv must have at least one element");
    ret = CA_ERROR;
    goto error;
  }

  if (!ca->p->cleared) { checkarg_reset(ca); }
  ca->p->cleared = 0;

  ca->p->callname = strdup(argv[0]);
  if (!ca->p->callname) {
    ret = CA_ALLOC_ERR;
    goto error;
  }

  /* alloc a bit too much and shrink it to real size in one go later */
  ca->p->pos_args = (const char **)malloc(argc * sizeof(char *));
  if (!ca->p->pos_args) {
    ret = CA_ALLOC_ERR;
    goto error;
  }

  for (i = 1; i < argc; ++i) {
    ret = checkarg_arg(ca, argv[i]);
    if (ret != CA_ALLOK) goto error;
  }

  if (ca->p->next_is_val_of) { return ca_error(CA_MISSVAL, ": %s!", argv[argc - 1]); }

  /* TODO: resize pos_args */

error:
  return ret;
}

int
checkarg_set_posarg_help(CheckArg *ca, const char *usage, const char *descr) {
  /* free possible previous values */
  free(ca->p->posarg_help_descr);
  free(ca->p->posarg_help_usage);

  ca->p->posarg_help_usage = strdup(usage);
  if (!ca->p->posarg_help_usage) return CA_ALLOC_ERR; /* malloc failed */
  ca->p->posarg_help_descr = strdup(descr);
  if (!ca->p->posarg_help_descr) {
    free(ca->p->posarg_help_usage);
    ca->p->posarg_help_usage = NULL;
    return CA_ALLOC_ERR; /* malloc failed */
  }
  return CA_ALLOK;
}

int
checkarg_set_usage_line(CheckArg *ca, const char *arg) {
  free(ca->p->usage_line);

  ca->p->usage_line = strdup(arg);
  if (!ca->p->usage_line) return CA_ALLOC_ERR;
  return CA_ALLOK;
}

__attribute__((deprecated)) const char *
checkarg_argv0(CheckArg *ca) {
  return ca->p->callname;
}

const char *
checkarg_callname(CheckArg *ca) {
  return ca->p->callname;
}

const char **
checkarg_pos_args(CheckArg *ca) {
  /* turn the cahr** into a const char**
   * FIXME: maybe even a (const char *const *),
   * making the pointers inside the "array" const ?
   * users should have any reason to modify them */
  return (const char **)ca->p->pos_args;
}

size_t
checkarg_pos_args_count(CheckArg *ca) {
  return ca->p->pos_args_count;
}

const char *
checkarg_value(CheckArg *ca, const char *key) {
  Opt *opt = valid_args_find(ca, key);
  if (opt) return opt->value;
  return NULL;
}

uint8_t
checkarg_isset(CheckArg *ca, const char *key) {
  Opt *opt = valid_args_find(ca, key);
  /* find returns NULL if not found */
  if (opt) return opt->value == NULL ? 0 : 1;
  return 0;
}

const char *
checkarg_str_err(const int errno) {
  return errors[errno];
}

/**
 * returns a dynamically allocated pointer which must be freed by the user
 */
char *
checkarg_usage(CheckArg *ca) {
  size_t len = 8;  // "Usage: " + "\0"
  len += strlen(ca->p->usage_line);
  if (ca->p->posarg_help_usage) {
    len += strlen(ca->p->posarg_help_usage) + 1;  // +1 for the space
  }

  char *line = malloc(len);
  if (!line) return NULL;

  if (ca->p->posarg_help_usage) {
    snprintf(line, len, "Usage: %s %s", ca->p->usage_line, ca->p->posarg_help_usage);
  }
  else {
    snprintf(line, len, "Usage: %s", ca->p->usage_line);
  }

  return line;
}

void
checkarg_show_usage(CheckArg *ca) {
  char *line = checkarg_usage(ca);
  printf("%s\n", line);
  free(line);
}

/**
 * returns a dynamically allocated pointer which must be freed by the user
 */
char *
checkarg_autohelp(CheckArg *ca) {
  size_t space = 0;
  Opt *it      = ca->p->valid_args;

  do {
    size_t tmp = strlen(it->lopt);
    /* 1 for the '=' between the option and the value name */
    if (it->value_name && *(it->value_name)) tmp += (1 + strlen(it->value_name));
    if (space < tmp) space = tmp;
  }
  while ((it = it->next));

  /* two more than opt length, so theres some space between the columns */
  space += 2;

  char *usage = checkarg_usage(ca);
  /*
   * 12 == newline after usage + strlen("\nOptions\n") + final \0
   * +2 == 2 * '\n' padding around description and appendix
   * 24 == "Positional Arguments:" + newlines
   */
  size_t msglen = strlen(usage) + 12 + (ca->p->descr ? 2 + strlen(ca->p->descr) : 0)
                +  // +2 -> 2*'\n'
                  (ca->p->posarg_help_descr ? 24 + strlen(ca->p->posarg_help_descr) : 0)
                + (ca->p->appendix ? 2 + strlen(ca->p->appendix) : 0);

  it = ca->p->valid_args;
  do {
    /* 10 == 6 char for short options + 3 chars for ' --' + a newline
     * space is the width of the long options, incl. value_name */
    msglen += 10 + space + strlen(it->help);
  }
  while ((it = it->next));

  size_t malloced_len = msglen;
  char *msg           = malloc(msglen);
  if (!msg) {
    free(usage);
    return NULL;
  }
  int pnum = snprintf(msg, msglen, "%s\n", usage);
  /* next snprintf can write pnum less chars */
  msglen -= pnum;
  /* next snprintf shall start at the end of the already written stuff */
  char *cur = msg + pnum;

  if (ca->p->descr) {
    pnum = snprintf(cur, msglen, "\n%s\n", ca->p->descr);
    msglen -= pnum;
    cur += pnum;
  }

  pnum = snprintf(cur, msglen, "\nOptions:\n");
  msglen -= pnum;
  cur += pnum;

  it = ca->p->valid_args;
  do {
    if (it->sopt)
      pnum = snprintf(cur, msglen, "   -%c,", it->sopt);
    else
      pnum = snprintf(cur, msglen, "      ");
    msglen -= pnum;
    cur += pnum;

    if (it->value_name && *(it->value_name)) {
      /* -1 for the '=' */
      int tmp = space - strlen(it->lopt) - strlen(it->value_name) - 1;
      pnum    = snprintf(
           cur, msglen, " --%s=%s%*s%s\n", it->lopt, it->value_name, tmp, "", it->help);
    }
    else {
      pnum = snprintf(
        cur, msglen, " --%s%*s%s\n", it->lopt, (int)(space - strlen(it->lopt)), "",
        it->help);
    }
    msglen -= pnum;
    cur += pnum;

    /* msglen must stay above zero,
     * if it does not, malloced_len calculation must be buggy
     * because it was smaller than the actual length of the help message
     *
     * same with the distance between cur and msg, (the current length of the message)
     * it shall not exceed malloced_len */
    assert(msglen > 0 && (cur - msg < malloced_len));
  }
  while ((it = it->next));

  if (ca->p->posarg_help_descr) {
    pnum =
      snprintf(cur, msglen, "\nPositional Arguments:\n%s\n", ca->p->posarg_help_descr);
    msglen -= pnum;
    cur += pnum;
  }

  if (ca->p->appendix) {
    pnum = snprintf(cur, msglen, "\n%s\n", ca->p->appendix);
    msglen -= pnum;
    cur += pnum;
  }

  /* msglen should have 1 remaining char
   * because snprintf does not count the trailing \0 it writes into the buffer */
  assert(msglen == 1);

  free(usage);
  return msg;
}

void
checkarg_show_help(CheckArg *ca) {
  char *msg = checkarg_autohelp(ca);
  printf("%s", msg);
  free(msg);
}

int
checkarg_show_autohelp(CheckArg *ca, const char *larg, const char *val) {
  checkarg_show_help(ca);
  exit(0); /* always exit after showing help */
}

static Opt *
opt_new(
  const char sopt, const char *lopt, CheckArgFP cb, const char *help,
  const uint8_t value_type, const char *value_name) {

  Opt *opt = (Opt *)malloc(sizeof(Opt));
  if (!opt) return NULL;

  *opt = (Opt){
    .sopt       = sopt,
    .value_type = value_type,
    .cb         = cb,
    .value      = NULL,
    .value_name = NULL,
    .next       = NULL,
  };

  opt->lopt = strdup(lopt);
  if (!opt->lopt) goto clean;

  opt->help = strdup(help);
  if (!opt->help) goto clean;

  if (value_type > 0 && value_name) {
    if (*value_name) {
      opt->value_name = strdup(value_name);
      if (!opt->value_name) goto clean;
    }
    else {
      opt->value_name = strdup(lopt);
      string_toupper(opt->value_name);
    }
  }

  return opt;

clean:
  opt_free(opt);
  return NULL;
}

/* WARNING: you have to free 'next' yourself if used, like in valid_args_free */
static void
opt_free(Opt *o) {
  if (o) {
    if (o->value_type != CA_VT_NONE) {
      // value is either NULL if opt was not parsed or:
      // if value_type > 0, value may have been strdup'ed and must be free'd
      // otherwise points to the literal "x", which is static
      free(o->value);
    }
    free(o->value_name);
    free(o->help);
    free(o->lopt);
    free(o);
  }
}

static int
ca_error(int eno, const char *fmt, ...) {
  va_list al;
  va_start(al, fmt);

  fprintf(stderr, "Error: %s", errors[eno]);
  vfprintf(stderr, fmt, al);
  fprintf(stderr, "\n");

  va_end(al);
  return eno;
}

static void
pos_args_free(const char **posargs) {
  free(posargs);
}

static void
valid_args_free(Opt *vaptr) {
  if (vaptr) {
    valid_args_free(vaptr->next);
    opt_free(vaptr);
  }
}

static int
valid_args_insert(CheckArg *ca, Opt *opt) {
  /* this inserts a new opt before the one that has a higher sort order
   * that way the list stays sorted, insertion is now O(n), though
   * except when inserting in reverse sort order, then its O(1) */
  if (ca->p->valid_args) {
    Opt *it = ca->p->valid_args;

    // fprintf(stderr, "comparing: %s with %s\n", opt->lopt, it->lopt);
    if (strcmp(opt->lopt, it->lopt) < 0) {
      // fprintf(stderr, "strcmp was < 0, %s is new head\n", opt->lopt);
      /* opt must be before it */
      opt->next              = it;
      ca->p->valid_args      = opt;
      ca->p->valid_args_last = opt->next;
      return CA_ALLOK;
    }
    /* the if after the for loop appends the opt if strcmp was >= 0 */

    for (; it->next; it = it->next) {
      // fprintf(stderr, "comparing: %s with %s\n", opt->lopt, it->next->lopt);
      if (strcmp(opt->lopt, it->next->lopt) < 0) {
        // fprintf(stderr, "strcmp was < 0, %s inserted before %s\n", opt->lopt,
        // it->next->lopt);
        /* opt appears before it->next in lexicographical order,
         * so insert here, so next will be the first after opt */
        Opt *tmp  = it->next;
        it->next  = opt;
        opt->next = tmp;
        return CA_ALLOK;
      }
    }

    if (!it->next) {
      // fprintf(stderr, "strcmp was always > 0, %s is new tail\n", opt->lopt);
      /* we had reached the end without inserting, so append */
      it->next               = opt;
      ca->p->valid_args_last = opt;
      return CA_ALLOK;
    }

    assert(1); /* this should never happen */
    return CA_BUG;
  }

  /* the first insert */
  ca->p->valid_args      = opt;
  ca->p->valid_args_last = opt;
  return CA_ALLOK;
}

#if 0
static int
valid_args_append(CheckArg *ca, Opt *opt) {
  if (ca->p->valid_args_last) {
    ca->p->valid_args_last->next = opt;
    ca->p->valid_args_last       = opt;
    return CA_ALLOK;
  }

  /* the first insert */
  ca->p->valid_args      = opt;
  ca->p->valid_args_last = opt;
  return CA_ALLOK;
}
#endif

/* sadly O(n), returns NULL if not found  */
static Opt *
valid_args_find(CheckArg *ca, const char *lopt) {
  Opt *it = ca->p->valid_args;
  do {
    if (strcmp(it->lopt, lopt) == 0) return it;
  }
  while (it && (it = it->next));
  return NULL;
}

static Opt *
valid_args_find_sopt(CheckArg *ca, char sopt) {
  Opt *it = ca->p->valid_args;
  do
    if (it->sopt == sopt) return it;
  while (it && (it = it->next));
  return NULL;
}

static int
checkarg_arg(CheckArg *ca, const char *arg) {
  if (!ca->p->pos_arg_sep) {
    /* if the separator '--' was given, all following args are positional */

    if (ca->p->next_is_val_of) {
      /* _next_val_of should be an lopt with value */
      int ret = 0;

      Opt *opt = valid_args_find(ca, ca->p->next_is_val_of);
      if (opt->value)
        free(opt->value); /* in case someone specifies an option with value twice */
      opt->value = strdup(arg);
      if (!opt->value) /* malloc failed */
        return ca_error(CA_ALLOC_ERR, "!");

      ret = call_cb(ca, opt);
      free(ca->p->next_is_val_of);
      ca->p->next_is_val_of = NULL;
      return ret;
    }

    if (arg[0] == '-') {                         /* it's an option */
      if (arg[1] == '-') {                       /* it's a long option */
        return checkarg_arg_long(ca, (arg + 2)); /* omit first two chars */
      }

      /* it's a short or a group of short options */
      return checkarg_arg_short(ca, (arg + 1)); /* omit first char */
    }
  }

  /* it's a positional arg */
  pos_args_append(ca, arg);
  return CA_ALLOK;
}

static int
checkarg_arg_long(CheckArg *ca, const char *lopt) {
  char *real_opt, *value;
  char *it;
  Opt *opt;
  int ret = 0;

  if (!*lopt) {
    /* if '--' was given, lopt is an empty string */
    ca->p->pos_arg_sep = 1;
    return CA_ALLOK;
  }

  value    = NULL;
  real_opt = strdup(lopt);
  if (!real_opt) return ca_error(CA_ALLOC_ERR, "!");

  it = real_opt;
  while (*it && *it != '=') ++it;
  /* iterated to either the position of the first '=' or the end of the string */

  if (*it == '=') { /* if we're at an '=', split the value off */
    value = it + 1; /* points to the part after the equal */
    *it   = 0;
    /* mark the end of the option with '\0' instead of '=' so it'll become a separate
     * c-str, real_opt should now contain the option only */
  }

  opt = valid_args_find(ca, real_opt);
  if (opt) {
    if (opt->value_type != CA_VT_NONE && value) {
      if (opt->value)
        free(opt->value); /* in case someone specifies an option with value twice */
      opt->value = strdup(value);
      if (!opt->value) goto alloc_error;
    }
    else if (opt->value_type != CA_VT_NONE) {
      ca->p->next_is_val_of = strdup(real_opt);
      if (!ca->p->next_is_val_of) goto alloc_error;
    }
    else if (value) { /* unexpected value given */
      goto invval_error;
    }
    else {
      opt->value = "x"; /* mark option as seen, any value except NULL is ok here */
    }

    if (opt->value_type == CA_VT_NONE || value) {
      ret = call_cb(ca, opt);
      free(real_opt);
      return ret;
    }
    free(real_opt);
    return CA_ALLOK;
  }

  /* else: no valid arg found -> invarg */
  ret = ca_error(CA_INVOPT, ": --%s!", real_opt);
  free(real_opt);
  return ret;

invval_error:
  ret = ca_error(CA_INVVAL, ": --%s!", real_opt);
  free(real_opt);
  return ret;

alloc_error:
  free(real_opt);
  return ca_error(CA_ALLOC_ERR, "!");
}


static int
checkarg_arg_short(CheckArg *ca, const char *args) {
  const char *it;
  Opt *opt;
  int ret = CA_ALLOK;

  for (it = args; *it; ++it) {
    opt = valid_args_find_sopt(ca, *it);
    if (opt) { /* short option found */
      if (opt->value_type != CA_VT_NONE) {
        if (*(++it)) { /* there's a remainder, assign it as value */
          if (opt->value)
            free(opt->value); /* in case someone specifies an option with value twice */
          opt->value = strdup(it);
          if (!opt->value) goto alloc_error;

          ret = call_cb(ca, opt);
          if (ret != CA_ALLOK) return ret;
        }
        else {
          ca->p->next_is_val_of = strdup(opt->lopt);
          if (!ca->p->next_is_val_of) goto alloc_error;
        }
        return CA_ALLOK; /* we're done here */
      }
      else {
        opt->value = "x"; /* any value except NULL is ok here */
        ret        = call_cb(ca, opt);
        if (ret != CA_ALLOK) return ret;
      }
    }
    else {
      return ca_error(CA_INVOPT, ": -%c!", *it);
    }
  }
  return CA_ALLOK;

alloc_error:
  return ca_error(CA_ALLOC_ERR, "!");
}

static int
call_cb(CheckArg *ca, Opt *opt) {
  if (opt->cb) {
    int ret = opt->cb(ca, opt->lopt, opt->value);
    if (ret != CA_ALLOK) { return ca_error(CA_CALLBACK, ": %d!", ret); }
  }
  return CA_ALLOK;
}

static void
pos_args_append(CheckArg *ca, const char *arg) {
  const char **tmp = ca->p->pos_args + ca->p->pos_args_count;
  *tmp             = arg;
  ++(ca->p->pos_args_count);
}

static void
string_toupper(char *s) {
  while (*s) {
    *s = toupper(*s);
    ++s;
  }
}

/* vim: set ft=c : */
