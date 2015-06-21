
#define CA_PRINTERR
#include <stdio.h>
#include "../src/checkarg.h"


int main(int argc, char **argv){

	CheckArg *ca = checkarg_new(argc, argv, "test_args", NULL, NULL);

	checkarg_add_autohelp(ca);
	checkarg_add_value(ca, 'i', "input", "file to read from", 1);
	checkarg_add_value(ca, 't', "test", "run pos_arg test", 1);

	checkarg_set_posarg_help(ca, " [files...]", "one or more output files");

	int ret = checkarg_parse(ca);

  const char **posargs = checkarg_pos_args(ca);
  for (int i = 0; i < checkarg_pos_args_count(ca); ++i) {
    printf("%s\n", posargs[i]);
  }
  return ret;
}
