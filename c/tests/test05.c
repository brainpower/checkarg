
#define CA_PRINTERR
#include <stdio.h>
#include "../src/checkarg.h"

int callback_b(CheckArgPtr c, const char* a, const char* v ) { return 0; }
int callback_d(CheckArgPtr c, const char* a, const char* v ) { return 0; }
int callback_e(CheckArgPtr c, const char* a, const char* v ) { printf("%s %s", a, v); return 0;}
int callback_z(CheckArgPtr c, const char* a, const char* v ) { printf("%s %s", a, v); return 0;}

int main(int argc, char **argv){

	CheckArgPtr ca = checkarg_new(argc, argv, "test05", NULL, NULL);

	checkarg_add_value(        ca, 'a', "alpha",                "non-value opt a",         1);
	checkarg_add_cb_value(     ca, 'b', "beta",    callback_b, "non-value opt b",          1);
	checkarg_add_long_value(   ca,      "gamma",               "non-value long opt gamma", 1);
	checkarg_add_long_cb_value(ca,      "delta",   callback_d, "non-value long opt delta", 1);
	checkarg_add_cb_value(     ca, 'e', "epsilon", callback_e, "non-value opt e",          1);
	checkarg_add_long_cb_value(ca,      "zeta",    callback_z, "non-value long opt zeta",  1);

	checkarg_add_autohelp(ca);

	int caret = checkarg_parse(ca);

	if( checkarg_isset(ca, "alpha") )
		printf("alpha %s", checkarg_value(ca, "alpha"));

	else if( checkarg_isset(ca, "beta"))
		printf("beta %s", checkarg_value(ca, "beta"));

	else if( checkarg_isset(ca, "gamma") )
		printf("gamma %s", checkarg_value(ca, "gamma"));

	else if( checkarg_isset(ca, "delta") )
		printf("delta %s", checkarg_value(ca, "delta"));

	return caret;
}

