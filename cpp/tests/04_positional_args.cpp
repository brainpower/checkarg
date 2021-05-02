#include "test.hpp"

TEST_CASE("positional args: general usage", "[positional]") {
  const vector<string> argv = {
    "/test04", "-i", "input.bin", "--", "file1", "file2",
  };

  CheckArg ca(argv, "test04");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  int ret = ca.parse();

  CHECK(ret == CA_ALLOK);

  REQUIRE_THAT(
    ca.pos_args(), Catch::Matchers::Equals(vector<string>{"file1", "file2"}));
}

TEST_CASE("positional args: args with dashes", "[positional]") {
  // these -- and - args must not be interpreted as options
  // if they appear after a --
  // when parsed as options these would trigger CA_INVOPT
  // hence failing the CHECK() below
  string arg1 = GENERATE("--file1", "file1", "-f", "-fg");
  string arg2 = GENERATE("--file2", "file2", "-g", "-hi");
  string arg3 = GENERATE("--file3", "file3", "-i", "-jk");

  const vector<string> argv = {"/test04", "-i", "input.bin", "--", arg1, arg2, arg3};

  CheckArg ca(argv, "test04");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);

  int ret = ca.parse();

  CHECK(ret == CA_ALLOK);

  REQUIRE_THAT(
    ca.pos_args(), Catch::Matchers::Equals(vector<string>{arg1, arg2, arg3}));
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
  const vector<string> argv = {
    "/test04", "-i", "--", "file1", "-j", "file2",
  };

  CheckArg ca(argv, "test04");
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);
  ca.add('j', "jay", "just a jay", CA_VT_NONE);

  int ret = ca.parse();

  // CHECK(ret == CA_MISSVAL); // should it be this?
  // REQUIRE(!ca.isset("jay"));

  CHECK(ret == CA_ALLOK);  // or this?
  REQUIRE(ca.value("input") == "--");
  REQUIRE(ca.isset("jay"));

  REQUIRE_THAT(
    ca.pos_args(), Catch::Matchers::Equals(vector<string>{"file1", "file2"}));
}
