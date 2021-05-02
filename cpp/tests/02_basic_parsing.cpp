#include "test.hpp"


TEST_CASE("parsing: help using --help", "[parsing]") {
  vector<string> argv = {
    "/test01",
  };
  string option = GENERATE("--help", "-h");
  argv.push_back(option);

  bool cb_ran = false;
  string out;

  CheckArg ca(argv, "test01");
  // the default callback calls exit(0), which is bad here
  // ca.add_autohelp();
  ca.add(
    'h', "help",
    [&out, &cb_ran](CheckArg *const ca, const string &, const string &) -> int {
      cb_ran = true;
      out    = ca->autohelp();
      return CA_ALLOK;
    },
    "show this help message and exit", CA_VT_NONE);
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);
  auto ret = ca.parse();

  CHECK(ret == CA_ALLOK);  // -h ran fine
  CHECK(cb_ran);           // callback actually ran
  REQUIRE(out ==
    "Usage: test01 [options]\n"
    "\n"
    "Options:\n"
    "   -h, --help   show this help message and exit\n"
    "   -i, --input  file to read from\n"
  ); // callback had access to CA object
}

// FIXME: check CA_ERROR return code

TEST_CASE("parsing: correct option and value", "[parsing]") {
  const vector<string> argv = {
    "/usr/bin/name",
    "-i",
    "input.in",
  };

  CheckArg ca(argv, "name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse();

  REQUIRE(ret == CA_ALLOK);                  // value ok
  REQUIRE(ca.isset("input"));                // option detected
  REQUIRE(ca.value("input") == "input.in");  // value stored correctly
}


TEST_CASE("parsing: invalid option", "[parsing]") {
  const vector<string> argv = {
    "/usr/bin/name",
    "-x",
  };

  CheckArg ca(argv, "name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse();

  REQUIRE(ret == CA_INVOPT);  // invalid option
}

TEST_CASE("parsing: missing value", "[parsing]") {
  const vector<string> argv = {
    "/usr/bin/name",
    "-i",
  };

  CheckArg ca(argv, "name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse();

  REQUIRE(ret == CA_MISSVAL);  // missing value
}
