#include "test.hpp"


int
callback(CheckArg *, const char *, const char *) {
  return 0;
}


TEST_CASE("variable names", "[val-names]") {
  vector<string> argv = {
    "./test08",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test08", NULL, NULL),
    &checkarg_free);


  checkarg_add_autohelp(ca.get());
  // clang-format off
  checkarg_add(ca.get(), 'z', "ag",    "autogenerated value name", CA_VT_REQUIRED, "");
  checkarg_add(ca.get(), 'y', "ng",    "no value name",     CA_VT_REQUIRED, NULL);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, "IPT");
  checkarg_add(ca.get(), 'a', "alpha", "alpha option",      CA_VT_REQUIRED, "A");
  checkarg_add_long(ca.get(), "beta",  "beta option",       CA_VT_REQUIRED, "B");
  // clang-format on

  checkarg_add_cb(
    ca.get(), 'c', "gamma", callback, "gamma option", CA_VT_REQUIRED, "C");
  checkarg_add_long_cb(
    ca.get(), "delta", callback, "delta option", CA_VT_REQUIRED, "D");

  int rc    = checkarg_parse(ca.get());
  auto help = unique_ptr<char>(checkarg_autohelp(ca.get()));

  CHECK(CA_ALLOK == rc);

  REQUIRE(string(help.get()) ==
    "Usage: test08 [options]\n"
    "\n"
    "Options:\n"
    "   -z, --ag=AG      autogenerated value name\n"
    "   -a, --alpha=A    alpha option\n"
    "       --beta=B     beta option\n"
    "       --delta=D    delta option\n"
    "   -c, --gamma=C    gamma option\n"
    "   -h, --help       show this help message and exit\n"
    "   -i, --input=IPT  file to read from\n"
	  "   -y, --ng         no value name\n"
  );
}
