// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"


TEST_CASE("parsing: help using --help", "[parsing]") {
  vector<string> argv = {
    "/test01",
  };
  string option = GENERATE("--help", "-h");
  argv.push_back(option);

  bool cb_ran = false;
  string out;

  CheckArg ca("test01");
  // the default callback calls exit(0), which is bad here
  // ca.add_autohelp();
  ca.add(
    'h', "help", "show this help message and exit",
    [&out, &cb_ran](CheckArg *const ca, const string &, const string &) -> int {
      cb_ran = true;
      out    = ca->autohelp();
      return CA_ALLOK;
    },
    CA_VT_NONE);
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);
  auto ret = ca.parse(argv);

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

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(argv);

  REQUIRE(ret == CA_ALLOK);                  // value ok
  REQUIRE(ca.isset("input"));                // option detected
  REQUIRE(ca.value("input") == "input.in");  // value stored correctly
}


TEST_CASE("parsing: invalid option", "[parsing]") {
  const vector<string> argv = {
    "/usr/bin/name",
    "-x",
  };

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(argv);

  REQUIRE(ret == CA_INVOPT);  // invalid option
}

TEST_CASE("parsing: missing value", "[parsing]") {
  const vector<string> argv = {
    "/usr/bin/name",
    "-i",
  };

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(argv);

  REQUIRE(ret == CA_MISSVAL);  // missing value
}

TEST_CASE("parsing: empty argv (std::vector)", "[parsing]") {
  const vector<string> argv{};

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(argv);

  REQUIRE(ret == CA_ERROR);  // empty argv
}

TEST_CASE("parsing: argv == NULL (argc=0, argv=nullptr)", "[parsing]") {
  const char **argv = nullptr;

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(0, (char **)argv);

  REQUIRE(ret == CA_ERROR);  // null argv
}

TEST_CASE("parsing: empty argv (argc=0, argv={nullptr})", "[parsing]") {
  const char **argv = {nullptr};

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(0, (char **)argv);

  REQUIRE(ret == CA_ERROR);  // empty argv
}

TEST_CASE("parsing: empty argv (argc=1, argv={nullptr})", "[parsing]") {
  const char **argv = {nullptr};

  CheckArg ca("name");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  auto ret = ca.parse(1, (char **)argv);

  REQUIRE(ret == CA_ERROR);  // empty argv
}
