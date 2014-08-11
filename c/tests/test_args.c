
#define CA_PRINTERR

#include "../src/checkarg.h"


int main(int argc, char **argv){
  int ret;
	CheckArg *ca = checkarg_new(argc, argv, "test_args", NULL, NULL);

	checkarg_add_autohelp(ca);
	checkarg_add_value(ca, 'i', "input", "file to read from", 1);

	ret = checkarg_parse(ca);

	checkarg_free(ca);
	return ret;


}
