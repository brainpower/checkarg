
#define CA_PRINTERR
#include <stdio.h>
#include "../src/checkarg.h"


int ret_test04 = 0;

int callback_b(CheckArgPtr ca, const char *a, const char *v ) { return 0; }
int callback_d(CheckArgPtr ca, const char *a, const char *v ) { return 0; }
int callback_e(CheckArgPtr ca, const char *a, const char *v ) { ret_test04 = 104; return 0;}
int callback_z(CheckArgPtr ca, const char *a, const char *v ) { ret_test04 = 105; return 0;}


int main(int argc, char **argv){

	CheckArg *ca = checkarg_new(argc, argv, "test04", NULL, NULL);

	checkarg_add(        ca, 'a', "alpha",               "non-value opt a");
	checkarg_add_cb(     ca, 'b', "beta",    callback_b, "non-value opt b");
	checkarg_add_long(   ca,      "gamma",               "non-value long opt gamma");
	checkarg_add_long_cb(ca,      "delta",   callback_d, "non-value long opt delta");
	checkarg_add_cb(     ca, 'e', "epsilon", callback_e, "non-value opt e");
	checkarg_add_long_cb(ca,      "zeta",    callback_z, "non-value long opt zeta");

	checkarg_add_autohelp(ca);

	int caret = checkarg_parse(ca);

	if( checkarg_isset(ca, "alpha") )

		ret_test04 = 100;

	else if( checkarg_isset(ca, "beta") )

		ret_test04 = 101;

	else if( checkarg_isset(ca, "gamma") )

		ret_test04 = 102;

	else if( checkarg_isset(ca, "delta") )

		ret_test04 = 103;


	if( caret > 0 )
		return caret;

	return ret_test04;
}
