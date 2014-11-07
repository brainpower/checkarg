
#define CA_PRINTERR

#include "../src/checkarg++.hpp"

using namespace std;

int main(int argc, char **argv){

	CheckArg ca(argc, argv, "test_args");

	ca.add_autohelp();
	ca.add('i', "input", "file to read from", true);
	ca.add('t', "test", "run pos_arg test", true);

	ca.set_posarg_help(" [files...]", "one or more output files");

	int ret = ca.parse();

	if( ca.isset("test") ){
  	if( ca.pos_args()[0] == ca.value("test") )
  	  return CA_ALLOK;
  }

  return ret;
}
