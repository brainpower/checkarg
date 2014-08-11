
#define CA_PRINTERR

#include "../src/checkarg.h"


int main(int argc, char **argv){

	CheckArg *ca = checkarg_new(argc, argv, "test_args", NULL, NULL);

	checkarg_add_autohelp(ca);
	checkarg_add_value(ca, 'i', "input", "file to read from", 1);
	checkarg_add_value(ca, 't', "test", "run pos_arg test", 1);

	checkarg_set_posarg_help(ca, " [files...]", "one or more output files");

	int ret = checkarg_parse(ca);

	if( checkarg_isset(ca, "test") ){
  	if( checkarg_pos_args(ca)[0] == checkarg_value(ca, "test") )
  	  return CA_ALLOK;
  }

  return ret;
}
