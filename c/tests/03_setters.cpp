#include "test.hpp"

TEST_CASE("setters: positional argument help", "setters") {
  vector<const char *> argv = {
    "/test02",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test02", NULL, NULL),
    &checkarg_free);

  checkarg_add_autohelp(ca.get());
  checkarg_add_value(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED);
  checkarg_add_value(ca.get(), 't', "test", "run tests", CA_VT_REQUIRED);

  checkarg_set_posarg_help(ca.get(), "[files...]", "one or more output files");

  int ret = checkarg_parse(ca.get());

  CHECK(ret == CA_ALLOK);
  string usage, help;
  {
    unique_ptr<char> xusage(checkarg_usage(ca.get()));
    unique_ptr<char> xhelp(checkarg_autohelp(ca.get()));
    usage = xusage.get();
    help  = xhelp.get();
  }
  REQUIRE(usage == "Usage: test02 [options] [files...]");
  REQUIRE(help ==
    "Usage: test02 [options] [files...]\n"
    "\n"
    "Options:\n"
    /* "   -h, --help         show this help message and exit\n" */
    /* "   -i, --input=INPUT  file to read from\n" */
    /* "   -t, --test=TEST    run tests\n" */
    "   -h, --help   show this help message and exit\n"
    "   -i, --input  file to read from\n"
    "   -t, --test   run tests\n"
    "\n"
    "Positional Arguments:\n"
    "one or more output files\n"
  );
}

TEST_CASE("setters: override usage line", "setters") {
  vector<const char *> argv = {
    "/test02",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test02", NULL, NULL),
    &checkarg_free);

  checkarg_add_autohelp(ca.get());
  checkarg_add_value(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED);
  checkarg_add_value(ca.get(), 't', "test", "run tests", CA_VT_REQUIRED);

  checkarg_set_usage_line(ca.get(), "this is a really stupid usage line");

  int ret = checkarg_parse(ca.get());

  CHECK(ret == CA_ALLOK);

  string usage, help;
  {
    unique_ptr<char> xusage(checkarg_usage(ca.get()));
    unique_ptr<char> xhelp(checkarg_autohelp(ca.get()));
    usage = xusage.get();
    help  = xhelp.get();
  }
  REQUIRE(usage == "Usage: this is a really stupid usage line");
  REQUIRE(help ==
    "Usage: this is a really stupid usage line\n"
    "\n"
    "Options:\n"
    /* "   -h, --help         show this help message and exit\n" */
    /* "   -i, --input=INPUT  file to read from\n" */
    /* "   -t, --test=TEST    run tests\n" */
    "   -h, --help   show this help message and exit\n"
    "   -i, --input  file to read from\n"
    "   -t, --test   run tests\n"
  );
}
