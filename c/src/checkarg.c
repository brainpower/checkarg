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

#include "checkarg_private.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>


const char* errors[] = {
  /*CA_ALLOK*/    "Everything is fine",
  /*CA_ERROR*/    "An Error occurred",
  /*CA_INVARG*/   "Unknown command line argument",
  /*CA_INVVAL*/   "Value given to non-value argument",
  /*CA_MISSVAL*/  "Missing value of argument",
  /*CA_CALLBACK*/ "Callback returned with error code",
  /*CA_ALLOC_ERR*/"Allocation of memory failed",
};


// c'tors

CheckArg*
checkarg_new(const int argc, char **argv, const char *appname, const char *desc, const char *appendix){
  CheckArg *ret = NULL;
  CheckArgPrivate *priv = NULL;

  ret = (CheckArg*) malloc(sizeof(CheckArg));
  if(!ret) return NULL; // malloc failed

  priv = (CheckArgPrivate*)malloc(sizeof(CheckArgPrivate));
  if(!priv) // malloc failed
    goto clean;

  ret->p = priv;

  priv->appname = strdup(appname);
  if(!priv->appname)// malloc failed
    goto clean;

  priv->usage_line = (char*)malloc(strlen(appname)+11);
  if(!priv->usage_line){ // malloc failed
    goto clean_app;
  }

  *priv->usage_line = 0; /* set first char to \0, so strcat will start at the beginning */
  strcat(priv->usage_line, priv->appname);
  strcat(priv->usage_line, " [options]");

  if(desc){
    priv->descr = strdup(desc);
    if(!priv->descr){ // malloc failed
      goto clean_usage;
    }
  } else
    priv->descr = NULL;

  if(appendix){
    priv->appendix = strdup(appendix);
    if(!priv->appendix){ // malloc failed
      goto clean_descr;
    }
  } else
    priv->appendix = NULL;


  priv->next_is_val_of    = NULL;
  priv->posarg_help_descr = NULL;
  priv->posarg_help_usage = NULL;

  /* alloc a bit too much and shrink it to real size in one go later */
  priv->pos_args          = (const char**)malloc(argc*sizeof(char*));
  priv->pos_args_count    = 0;
  priv->valid_args        = NULL;
  priv->valid_args_last   = NULL;

  priv->argv = argv;
  priv->argc = argc;
  priv->autohelp_on = 0;

  return ret;

// clean up and return in case of error
clean_descr:
  free(priv->descr);
clean_usage:
  free(priv->usage_line);
clean_app:
  free(priv->appname);
clean:
  free(priv);
  free(ret);
  return NULL;
}

void
checkarg_free(CheckArg *ca){
  free(ca->p->appname);
  free(ca->p->descr);
  free(ca->p->appendix);
  free(ca->p->usage_line);
  free(ca->p->next_is_val_of);
  free(ca->p->posarg_help_descr);
  free(ca->p->posarg_help_usage);

  // arrays
  pos_args_free(ca->p->pos_args );
  valid_args_free(ca->p->valid_args);

  free(ca->p);
  free(ca);
}

inline int
checkarg_add(CheckArg *ca, const char sopt, const char *lopt, const char *help){
  return checkarg_add_value(ca, sopt, lopt, help, 0);
}

int
checkarg_add_value(CheckArg *ca, const char sopt, const char *lopt, const char *help, const uint8_t has_val){
  Opt *opt = opt_new(sopt, lopt, NULL, help, has_val);
  if(!opt)
    return CA_ALLOC_ERR; // malloc failed

  return valid_args_insert(ca, opt);
}

int
checkarg_add_cb(CheckArg *ca, const char sopt, const char *lopt, CheckArgFP cb, const char *help){
  return checkarg_add_cb_value(ca, sopt, lopt, cb, help, 0);
}

int
checkarg_add_cb_value(CheckArg *ca, const char sopt, const char *lopt, CheckArgFP cb, const char *help, const uint8_t has_val){
  Opt *opt = opt_new(sopt, lopt, cb, help, has_val);
  if(!opt) return CA_ALLOC_ERR;

  return valid_args_insert(ca, opt);
}

int
checkarg_add_long(CheckArg *ca, const char *lopt, const char *help){
  return checkarg_add_long_value(ca, lopt, help, 0);
}

int
checkarg_add_long_value(CheckArg *ca, const char *lopt, const char *help, const int8_t has_val){
  Opt *opt = opt_new(0, lopt, NULL, help, has_val);
  if(!opt) return CA_ALLOC_ERR;
  return valid_args_insert(ca, opt);
}

int
checkarg_add_long_cb(CheckArg *ca, const char *lopt, CheckArgFP cb, const char *help){
  return checkarg_add_long_cb_value(ca, lopt, cb, help, 0);
}

int
checkarg_add_long_cb_value(CheckArg *ca, const char *lopt, CheckArgFP cb, const char *help, const uint8_t has_val) {
  Opt *opt = opt_new(0, lopt, cb, help, has_val);
  if(!opt) return CA_ALLOC_ERR;
  return valid_args_insert(ca, opt);
}

int
checkarg_add_autohelp(CheckArg *ca) {
  Opt *opt = opt_new('h', "help", checkarg_show_autohelp, "show this help message and exit", 0);
  if(!opt) return CA_ALLOC_ERR;
  ca->p->autohelp_on = 1;
  return valid_args_insert(ca, opt);
}

int
checkarg_parse(CheckArg *ca) {
  int ret = CA_ALLOK;
  int i;

  for(i=1; i<ca->p->argc; ++i){
    ret = checkarg_arg(ca, ca->p->argv[i]);
    if( ret != CA_ALLOK ) break;
  }

  if( ca->p->next_is_val_of ) {
    return ca_error(CA_MISSVAL, ": %s!", ca->p->argv[ca->p->argc-1] );
  }

  /* free strings not neccessary anymore */
  free(ca->p->appname);    ca->p->appname    = NULL;
  free(ca->p->appendix);   ca->p->appendix   = NULL;
  free(ca->p->usage_line); ca->p->usage_line = NULL;
  free(ca->p->descr);      ca->p->descr      = NULL;
  free(ca->p->posarg_help_descr); ca->p->posarg_help_descr = NULL;
  free(ca->p->posarg_help_usage); ca->p->posarg_help_usage = NULL;
  free(ca->p->next_is_val_of);    ca->p->next_is_val_of    = NULL;

  /* TODO: free help messages */
  /* TODO: resize pos_args */

  return ret;
}

int
checkarg_set_posarg_help(CheckArg *ca, const char *usage, const char *descr){
  free(ca->p->posarg_help_descr);
  free(ca->p->posarg_help_usage);

  ca->p->posarg_help_usage = strdup(usage);
  if(!ca->p->posarg_help_usage) return CA_ALLOC_ERR; // malloc failed
  ca->p->posarg_help_descr = strdup(descr);
  if(!ca->p->posarg_help_descr){
    free(ca->p->posarg_help_usage);
    ca->p->posarg_help_usage = NULL;
    return CA_ALLOC_ERR; // malloc failed
  }
  return CA_ALLOK;
}

int
checkarg_set_usage_line(CheckArg *ca, const char *arg){
  free(ca->p->usage_line);

  ca->p->usage_line = strdup(arg);
  if(!ca->p->usage_line)
    return CA_ALLOC_ERR;
  return CA_ALLOK;
}

const char*
checkarg_argv0(CheckArg *ca) {
  return ca->p->argv[0];
}

const char**
checkarg_pos_args(CheckArg *ca){
  return (const char**)ca->p->pos_args;
}

size_t
checkarg_pos_args_count(CheckArg *ca){
  return ca->p->pos_args_count;
}

const char*
checkarg_value(CheckArg* ca, const char *key){
  Opt *opt = valid_args_find(ca, key);
  if(opt) return opt->value;
  return NULL;
}

uint8_t
checkarg_isset(CheckArg *ca, const char *key){
  Opt *opt = valid_args_find(ca, key);
  /* find returns NULL if not found */
  if(opt)
    return opt->value == NULL ? 0 : 1;
  return 0;
}

const char*
checkarg_str_err(const int errno) {
  return errors[errno];
}


int
checkarg_show_autohelp(CheckArg *ca, const char* larg, const char* val){
  size_t space = 0;
  size_t tmp = 0;
  Opt *it = ca->p->valid_args;

  do {
    tmp = strlen(it->lopt);
    space = space > tmp ? space : tmp;
  } while( (it=it->next) );

  space += 2; // two more than opt length

  if(ca->p->posarg_help_usage)
    printf("Usage: %s %s\n", ca->p->usage_line, ca->p->posarg_help_usage);
  else
    printf("Usage: %s \n", ca->p->usage_line);

  if(ca->p->descr)
    printf("\n%s\n", ca->p->descr);

  printf("\nOptions:\n");

  it = ca->p->valid_args;
  do {
    if(it->sopt)
      printf("   -%c,", it->sopt);
    else
      printf("      ");

    printf(" --%s%*s%s\n", it->lopt, (int)(space-strlen(it->lopt)), "", it->help);
  } while( (it = it->next ) );

  if(ca->p->posarg_help_descr){
    printf("\nPositional Arguments:\n%s\n", ca->p->posarg_help_descr);
  }

  if(ca->p->appendix){
    printf("\n%s\n", ca->p->appendix);
  }

  exit(0); /* always exit after showing help */
}

Opt*
opt_new(const char sopt, const char *lopt, CheckArgFP cb, const char *help, const uint8_t has_val){
  Opt *opt = (Opt*)malloc(sizeof(Opt));
  if(!opt) return NULL;

  opt->lopt = strdup(lopt);
  if(!opt->lopt){ // malloc failed
    free(opt);
    return NULL;
  }

  opt->help = strdup(help);
  if(!opt->help){ // malloc failed
    free(opt->lopt);
    free(opt);
    return NULL;
  }

  opt->sopt    = sopt;
  opt->has_val = has_val;
  opt->cb      = cb;
  opt->value   = NULL;
  opt->next    = NULL;
  return opt;
}

void opt_free(Opt *o){
  free(o->value);
  free(o->help);
  free(o->lopt);
  free(o);
}

int ca_error(int eno, const char *fmt, ...) {
  va_list al;
  va_start(al, fmt);

  printf("Error: %s", errors[eno]);
  vprintf(fmt, al);
  printf("\n");

  return eno;
}

void pos_args_free(const char **posargs){
  free(posargs);
}

void valid_args_free(Opt* vaptr){
  if(vaptr){
    valid_args_free(vaptr->next);
    opt_free(vaptr);
  }
}

int  valid_args_insert(CheckArg *ca, Opt *opt){
  if(ca->p->valid_args_last) {
    ca->p->valid_args_last->next = opt;
    ca->p->valid_args_last = opt;
    return CA_ALLOK;
  }

  // the first insert
  ca->p->valid_args      = opt;
  ca->p->valid_args_last = opt;
  return CA_ALLOK;
}

/* sadly O(n), returns NULL if not found  */
Opt* valid_args_find(CheckArg *ca, const char *lopt){
  Opt *it = ca->p->valid_args;
  do {
    if( strcmp(it->lopt, lopt) == 0 )
      return it;
  } while( it && (it = it->next) );
  return NULL;
}

Opt* valid_args_find_sopt(CheckArg *ca, char sopt){
  Opt *it = ca->p->valid_args;
  do
    if( it->sopt == sopt )
      return it;
  while( it && (it = it->next) );
  return NULL;
}

int checkarg_arg(CheckArg *ca, const char *arg) {
  if( ca->p->next_is_val_of ){
    //_next_val_of should be an lopt width value
    int ret = 0;

    Opt *opt = valid_args_find(ca, ca->p->next_is_val_of);
    if(opt->value) free(opt->value); /* in case someone specifies an option with value twice */
    opt->value = strdup(arg);
    if( ! opt->value ) // malloc failed
      return ca_error(CA_ALLOC_ERR, "!");

    ret = call_cb(ca, opt);
    free(ca->p->next_is_val_of);
    ca->p->next_is_val_of = NULL;
    return ret;
  }

  if( arg[0] == '-' ) { /* it's an option */
    if( arg[1] == '-') { /* it's a long option */
      return checkarg_arg_long(ca, (arg + 2) ); /* omit first two chars */
    }

    /* it's a short or a group of short options */
    return checkarg_arg_short(ca, (arg + 1) ); /* omit first char */
  }

  /* it's a positional arg */
  pos_args_append(ca, arg);
  return CA_ALLOK;
}

int checkarg_arg_long(CheckArg *ca, const char *lopt){
  char *real_opt, *value;
  char *it;
  Opt *opt;
  int ret = 0;

  value = NULL;
  real_opt = strdup(lopt);
  if( ! real_opt )
    return ca_error(CA_ALLOC_ERR, "!");

  it = real_opt;
  while( *it && *it != '=' ) ++it;

  if( *it == '=' ) {
    value = it +1; /* points to the part after the equal */
    *it = 0;
    /* mark the end of the option with '\0' instead of '=' so it'll become a separate c-str,
     * real_opt should now contain the option only */
  }

  opt = valid_args_find(ca, real_opt);
  if(opt) {
    if( opt->has_val && value ){
      if(opt->value) free(opt->value); /* in case someone specifies an option with value twice */
      opt->value = strdup(value);
      if( ! opt->value )
        goto alloc_error;
    }
    else if( opt->has_val ){
      ca->p->next_is_val_of = strdup(real_opt);
      if( ! ca->p->next_is_val_of )
        goto alloc_error;
    }
    else if( value ) { /* unexpected value given */
      goto invval_error;
    }
    else {
      opt->value = "x"; /* mark option as seen */
    }

    if( !opt->has_val || value ){
      ret = call_cb(ca, opt);
      free(real_opt);
      return ret;
    }
    free(real_opt);
    return CA_ALLOK;
  }

  /* else: no valid arg found -> invarg */
  ret = ca_error(CA_INVARG, ": --%s!", real_opt);
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


int checkarg_arg_short(CheckArg *ca, const char *args){
  const char *it;
  Opt *opt;
  int ret = CA_ALLOK;

  for(it=args; *it; ++it){
    opt = valid_args_find_sopt(ca, *it);
    if(opt) { /* short option found */
      if( opt->has_val ){
        if( *(++it) ) { /* there's a remainder, assign it as value */
          if(opt->value) free(opt->value); /* in case someone specifies an option with value twice */
          opt->value = strdup( it );
          if( !opt->value )
            goto alloc_error;
        } else {
          ca->p->next_is_val_of = strdup( opt->lopt );
          if( ! ca->p->next_is_val_of )
            goto alloc_error;
        }
        return CA_ALLOK; /* we're done here */
      }
      else {
        opt->value = "x";
        ret = call_cb(ca, opt);
        if( ret != CA_ALLOK )
          return ret;
      }
    } else {
      return ca_error(CA_INVARG, ": -%c!", *it);
    }
  }
  return CA_ALLOK;

alloc_error:
  return ca_error(CA_ALLOC_ERR,"!");

}

int call_cb(CheckArg *ca, Opt *opt){
  int ret = CA_ALLOK;

  if( opt->cb ) {
    ret = opt->cb(ca, opt->lopt, opt->value);
    if( ret != CA_ALLOK ){
      return ca_error(CA_CALLBACK, ": %d!", ret);
    }
  }
  return CA_ALLOK;
}

void pos_args_append(CheckArg *ca, const char *arg){
  const char** tmp = ca->p->pos_args + ca->p->pos_args_count;
  *tmp = arg;
  ++(ca->p->pos_args_count);
}
