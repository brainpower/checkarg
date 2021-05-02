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
	checkarg_add_value(ca.get(), 'i', "input", "file to read from", "IPT", CA_VT_REQUIRED);
  checkarg_add_value(ca.get(), 'a', "alpha", "alpha option",      "A",   CA_VT_REQUIRED);
  checkarg_add_long_value(ca.get(), "beta",  "beta option",       "B",   CA_VT_REQUIRED);
  // clang-format on

  checkarg_add_cb_value(
    ca.get(), 'c', "gamma", callback, "gamma option", "C", CA_VT_REQUIRED);
  checkarg_add_long_cb_value(
    ca.get(), "delta", callback, "delta option", "D", CA_VT_REQUIRED);

  int rc    = checkarg_parse(ca.get());
  auto help = unique_ptr<char>(checkarg_autohelp(ca.get()));

  CHECK(CA_ALLOK == rc);

  REQUIRE(string(help.get()) ==
	  "Usage: test08 [options]\n"
	  "\n"
	  "Options:\n"
    "   -a, --alpha=A    alpha option\n"
    "       --beta=B     beta option\n"
    "       --delta=D    delta option\n"
    "   -c, --gamma=C    gamma option\n"
    "   -h, --help       show this help message and exit\n"
    "   -i, --input=IPT  file to read from\n"
	);
}
