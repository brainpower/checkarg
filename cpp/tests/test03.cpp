
#define CA_PRINTERR
#include <cstdio>
#include "../src/checkarg++.hpp"


int main(int argc, char **argv){

	CheckArg ca(argc, argv, "test_args");

	ca.add_autohelp();
	ca.add('i', "input", "file to read from", true);
	ca.add('t', "test",  "run pos_arg test", true);

	ca.set_posarg_help(" [files...]", "one or more output files");

	int ret = ca.parse();

  auto posargs = ca.pos_args();
  for (auto arg : posargs) {
    printf("%s\n", arg.c_str());
  }
  return ret;
}
