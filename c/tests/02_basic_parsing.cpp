#include "test.hpp"

bool cb_ran = false;
string out;

int
callback_help(CheckArg *ca, const char *, const char *) {
  cb_ran = true;
  unique_ptr<char> xout(checkarg_autohelp(ca));
  out = xout.get();
  return CA_ALLOK;
}


TEST_CASE("parsing: help using --help", "[parsing]") {
  vector<const char *> argv = {
    "/test01",
  };

  argv.push_back(GENERATE("--help", "-h"));

  cb_ran = false;
  out.clear();

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test01", NULL, NULL),
    &checkarg_free);

  // the default callback calls exit(0), which is bad here
  // checkarg_add_autohelp();

  // it seems one can also not just pass a lambda to a C function
  /* checkarg_add_cb(ca.get(), */
  /*   'h', "help", */
  /*   [&out,&cb_ran](CheckArg *ca, const char*, const char*) -> int { */
  /*     cb_ran = true; */
  /*     unique_ptr<char> xout(checkarg_autohelp(ca)); */
  /*     out = xout.get(); */
  /*     return CA_ALLOK; */
  /*   }, */
  /*   "show this help message and exit" */
  /* ); */
  checkarg_add_cb(
    ca.get(), 'h', "help", &callback_help, "show this help message and exit", 0, NULL);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);
  auto ret = checkarg_parse(ca.get());

  CHECK(ret == CA_ALLOK);  // -h ran fine
  CHECK(cb_ran);           // callback actually ran
  REQUIRE(out ==
    "Usage: test01 [options]\n"
    "\n"
    "Options:\n"
//    "   -h, --help         show this help message and exit\n"
//    "   -i, --input=INPUT  file to read from\n"
    "   -h, --help   show this help message and exit\n"
    "   -i, --input  file to read from\n"
  ); // callback had access to CA object
}

TEST_CASE("parsing: correct option and value", "[parsing]") {
  const vector<const char *> argv = {
    "/usr/bin/name",
    "-i",
    "input.in",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test01", NULL, NULL),
    &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);

  auto ret = checkarg_parse(ca.get());

  // value ok
  REQUIRE(ret == CA_ALLOK);
  // option detected
  REQUIRE(checkarg_isset(ca.get(), "input"));
  // value stored correctly
  REQUIRE(checkarg_value(ca.get(), "input") == string("input.in"));
}


TEST_CASE("parsing: invalid option", "[parsing]") {
  const vector<const char *> argv = {
    "/usr/bin/name",
    "-x",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test01", NULL, NULL),
    &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);

  auto ret = checkarg_parse(ca.get());

  REQUIRE(ret == CA_INVOPT);  // invalid option
}

TEST_CASE("parsing: missing value", "[parsing]") {
  const vector<const char *> argv = {
    "/usr/bin/name",
    "-i",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test01", NULL, NULL),
    &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);

  auto ret = checkarg_parse(ca.get());

  REQUIRE(ret == CA_MISSVAL);  // missing value
}
