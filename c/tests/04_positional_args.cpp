// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>

#include "test.hpp"


TEST_CASE("positional args: general usage", "[positional]") {
  const vector<const char *> argv = {
    "/test04", "-i", "input.bin", "--", "file1", "file2",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test04", NULL, NULL),
    &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);

  int ret = checkarg_parse(ca.get());

  CHECK(ret == CA_ALLOK);

  auto args = checkarg_pos_args(ca.get());
  REQUIRE(args[0] == string("file1"));
  REQUIRE(args[1] == string("file2"));
}

TEST_CASE("positional args: args with dashes", "[positional]") {
  // these -- and - args must not be interpreted as options
  // if they appear after a --
  // when parsed as options these would trigger CA_INVOPT
  // hence failing the CHECK() below
  const char *arg1 = GENERATE("--file1", "file1", "-f", "-fg");
  const char *arg2 = GENERATE("--file2", "file2", "-g", "-hi");
  const char *arg3 = GENERATE("--file3", "file3", "-i", "-jk");

  const vector<const char *> argv = {"/test04", "-i", "input.bin", "--",
                                     arg1,      arg2, arg3};

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test04", NULL, NULL),
    &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);

  int ret = checkarg_parse(ca.get());

  CHECK(ret == CA_ALLOK);

  auto args = checkarg_pos_args(ca.get());
  REQUIRE(args[0] == string(arg1));
  REQUIRE(args[1] == string(arg2));
  REQUIRE(args[2] == string(arg3));
}

TEST_CASE("positional args: missing value before '--'", "[positional]") {
  // if the last option before a '--' has a value which was not given
  // the "--" shall be the value instead of the separator
  // the '-j' must be interpreted as an option
  //
  // Maybe consider an alternative here, though:
  // always have '--' be the separator
  // which would trigger a CA_MISSVAL and not interpret the '-j'
  // This is probably closer to the users intention
  const vector<const char *> argv = {
    "/test04", "-i", "--", "file1", "-j", "file2",
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test04", NULL, NULL),
    &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "file to read from", CA_VT_REQUIRED, NULL);
  checkarg_add(ca.get(), 'j', "jay", "just a jay", 0, NULL);

  int ret = checkarg_parse(ca.get());

  // CHECK(ret == CA_MISSVAL); // should it be this?
  // REQUIRE(!checkarg_isset("jay"));

  CHECK(ret == CA_ALLOK);  // or this?
  REQUIRE(checkarg_value(ca.get(), "input") == string("--"));
  REQUIRE(checkarg_isset(ca.get(), "jay"));

  auto args = checkarg_pos_args(ca.get());
  REQUIRE(args[0] == string("file1"));
  REQUIRE(args[1] == string("file2"));
}
