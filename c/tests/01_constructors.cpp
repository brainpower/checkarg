// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>

#include "test.hpp"


TEST_CASE("constructor: simple", "[constructor]") {
  int argc = 1;

  const char *argv[] = {
    "/usr/bin/name",
    NULL,
  };


  CheckArgUPtr ca(checkarg_new("name", NULL, NULL), &checkarg_free);

  auto ret = checkarg_parse(ca.get(), argc, (char **)argv);
  REQUIRE(ret == CA_ALLOK);


  SECTION("call name") {
    auto ret2 = checkarg_argv0(ca.get());

    REQUIRE(ret2 == string("/usr/bin/name"));
  }

  SECTION("appname") {
    auto usage = unique_ptr<char, decltype(&free)>(checkarg_usage(ca.get()), &free);

    REQUIRE(usage.get() == string("Usage: name [options]"));
  }
}

TEST_CASE("constructor: simple (vector)", "[constructor]") {
  const vector<const char *> argv = {
    "/usr/bin/name",
  };

  CheckArgUPtr ca(checkarg_new("name", NULL, NULL), &checkarg_free);

  auto ret = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());
  REQUIRE(ret == CA_ALLOK);

  SECTION("call name") {
    auto ret2 = checkarg_argv0(ca.get());
    REQUIRE(ret2 == string("/usr/bin/name"));
  }

  SECTION("appname") {
    auto usage = unique_ptr<char, decltype(&free)>(checkarg_usage(ca.get()), &free);

    REQUIRE(usage.get() == string("Usage: name [options]"));
  }
}


#if 0  // HAS_BASENAME
TEST_CASE("constructor: app name autodetect", "[constructor]"){
  const vector<string> argv = {
    "/usr/bin/name",
  };

  CheckArg ca(argv);

  auto ret = checkarg_parse();
  REQUIRE(ret == CA_ALLOK);

  SECTION("call name") {
    auto ret2 = checkarg_argv0();
    REQUIRE(ret2 == "/usr/bin/name");
  }

  SECTION("appname") {
    auto usage = checkarg_usage();

    REQUIRE(usage == "Usage: name [options]");
  }
}
#endif
