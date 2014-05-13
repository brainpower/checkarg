#ifndef CHECKARG_H
#define CHECKARG_H

#include <stdint.h>

typedef struct _CheckArg CheckArg;
typedef struct _CheckArgPrivate CheckArgPrivate;

struct _CheckArg {
	
	CheckArgPrivate *p;
};

CheckArg* check_arg_new(const int argc, char **argv, const char *appname, const char *desc, const char *appendix);
void      check_arg_free(CheckArg *);

int check_arg_add   (CheckArg*, const char sopt, const char *lopt, const char *help, int8_t has_val);
int check_arg_add_cb(CheckArg*, const char sopt, const char *lopt, int(*cb)(CheckArg*, const char*, const char*), const char* help, int8_t has_val);

int check_arg_add_long(CheckArg*, const char *lopt, const char *help, int8_t has_val);
int check_arg_add_long_cb(CheckArg*, const char *lopt, int(*cb)(CheckArg*, const char*, const char*), const char *help, int8_t has_val);

int check_arg_add_autohelp(CheckArg*);

int check_arg_parse(CheckArg*);


int check_arg_set_posarg_help(CheckArg*, const char*, const char*);
int check_arg_set_usage_line(CheckArg*, const char*);

char** check_arg_pos_args(CheckArg*);

const char* check_arg_value(CheckArg*, const char*);

int8_t check_arg_isset(CheckArg*, const char*);


const char* ca_str_error(const int errno);
void        ca_error(const int errno);

enum {
	CA_ALLOK=0,
	CA_ERROR,
	CA_INVARG,
	CA_INVVAL,
	CA_MISSVAL,
	CA_CALLBACK,
};



#endif /* CHECKARG_H */

