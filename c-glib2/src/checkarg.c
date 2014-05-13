
#include "checkarg.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

typedef struct _MapIntStr  ErrEntry;

struct _CheckArgPrivate{
	

	GHashTable *valid_args_cb,
	           *valid_args,
	           *valid_args_vals,
	           *autohelp,
						 *short2long;

	char** pos_args;

	int argc;
	char** argv;

	const char *appname,
	           *descr,
	           *appendix,
	           *next_is_val_of;
	char       *usage_line,
	           *posarg_usage,
	           *posarg_descr;

	int autohelp_on;
	
};

struct _MapIntStr {
	int key;
	const char *val;
};

ErrEntry _errors[] = {
	{ 1, "Test" },
};


int check_arg_arg(const char*);
int check_arg_arg_long(const char*);
int check_arg_arg_short(const char*);
const char check_arg_long_to_short(const char*);


CheckArg* check_arg_new(const int argc, char **argv, const char *appname, const char *desc, const char *appendix){
	CheckArg* ca = (CheckArg*)malloc(sizeof(CheckArg));
	ca->p = (CheckArgPrivate*)malloc(sizeof(CheckArgPrivate));

	ca->p->argc     = argc;
	ca->p->argv     = argv;
	ca->p->appname  = appname;
	ca->p->descr    = desc;
	ca->p->appendix = appendix;

	ca->p->usage_line = (char*)malloc(strlen(appname)+11);
	strcpy(ca->p->usage_line, appname);
	strcat(ca->p->usage_line, " [options]");

	ca->p->valid_args = g_hash_table_new(g_str_hash, g_str_equal);

	return ca;
}

void check_arg_free(CheckArg *ca){
	g_free(ca->p->valid_args);

	free(ca->p->usage_line);
	free(ca->p);
	free(ca);
}

int check_arg_add (CheckArg *ca, const char sopt, const char *lopt, const char *help, int8_t has_val){
	g_hash_table_insert(ca->p->valid_args, lopt, has_val);

}
int check_arg_add_cb(CheckArg *ca, const char sopt, const char *lopt, int(*cb)(CheckArg*, const char*, const char*), const char* help, int8_t has_val){
}

int check_arg_add_long(CheckArg *ca, const char *lopt, const char *help, int8_t has_val){
}
int check_arg_add_long_cb(CheckArg *ca, const char *lopt, int(*cb)(CheckArg*, const char*, const char*), const char *help, int8_t has_val){
}

int check_arg_add_autohelp(CheckArg *ca){
}

int check_arg_parse(CheckArg *ca){}


int check_arg_set_posarg_help(CheckArg *ca, const char *usage, const char *desc){}
int check_arg_set_usage_line(CheckArg *ca, const char *line){}

char** check_arg_pos_args(CheckArg *ca){}

const char* check_arg_value(CheckArg *ca, const char *lopt){}

int8_t check_arg_isset(CheckArg *ca, const char *lopt){}


const char* ca_str_error(const int errno){}
void        ca_error(const int errno){}
