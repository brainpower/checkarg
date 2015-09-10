
#define CA_PRINTERR
#include <cstdio>
#include "../src/checkarg++.hpp"

using namespace std;

int callback_b(CheckArgPtr, const string &, const string & ) { return 0; }
int callback_d(CheckArgPtr, const string &, const string & ) { return 0; }
int callback_e(CheckArgPtr, const string &a, const string &v ) { printf("%s %s", a.c_str(), v.c_str()); return 0;}
int callback_z(CheckArgPtr, const string &a, const string &v ) { printf("%s %s", a.c_str(), v.c_str()); return 0;}

int main(int argc, char **argv){

	CheckArg ca(argc, argv, "test05");

	ca.add('a', "alpha",                "non-value opt a",         true);
	ca.add('b', "beta",    callback_b, "non-value opt b",          true);
	ca.add(     "gamma",               "non-value long opt gamma", true);
	ca.add(     "delta",   callback_d, "non-value long opt delta", true);
	ca.add('e', "epsilon", callback_e, "non-value opt e",          true);
	ca.add(     "zeta",    callback_z, "non-value long opt zeta",  true);

	ca.add_autohelp();

	auto caret = ca.parse();

	if( ca.isset("alpha") )
		printf("alpha %s", ca.value("alpha").c_str());

	else if( ca.isset("beta"))
		printf("beta %s", ca.value("beta").c_str());

	else if( ca.isset("gamma") )
		printf("gamma %s", ca.value("gamma").c_str() );

	else if( ca.isset("delta") )
		printf("delta %s", ca.value("delta").c_str() );

	return caret;
}

