
#define CA_PRINTERR
#include <cstdio>
#include "../src/checkarg++.hpp"

using namespace std;

int ret_test04 = 0;

int callback_b(CheckArgPtr, const string &, const string & ) { return 0; }
int callback_d(CheckArgPtr, const string &, const string & ) { return 0; }
int callback_e(CheckArgPtr, const string &, const string & ) { ret_test04 = 104; return 0;}
int callback_z(CheckArgPtr, const string &, const string & ) { ret_test04 = 105; return 0;}


int main(int argc, char **argv){

	CheckArg ca(argc, argv, "test04");

	ca.add('a', "alpha",               "non-value opt a");
	ca.add('b', "beta",    callback_b, "non-value opt b");
	ca.add(     "gamma",               "non-value long opt gamma");
	ca.add(     "delta",   callback_d, "non-value long opt delta");
	ca.add('e', "epsilon", callback_e, "non-value opt e");
	ca.add(     "zeta",    callback_z, "non-value long opt zeta");

	ca.add_autohelp();

	auto caret = ca.parse();

	if( ca.isset("alpha") )

		ret_test04 = 100;

	else if( ca.isset("beta") )

		ret_test04 = 101;

	else if( ca.isset("gamma") )

		ret_test04 = 102;

	else if( ca.isset("delta") )

		ret_test04 = 103;


	if( caret > 0 )
		return caret;

	return ret_test04;
}
