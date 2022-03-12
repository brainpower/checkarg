// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"

TEST_CASE("constructor: simple", "[constructor]") {
  int argc           = 1;
  const char *argv[] = {
    "/usr/bin/name",
    NULL,
  };

  CheckArg ca("name");

  auto ret = ca.parse(argc, (char **)argv);
  REQUIRE(ret == CA_ALLOK);


  SECTION("call name") {
    auto ret2 = ca.argv0();

    REQUIRE(ret2 == "/usr/bin/name");
  }

  SECTION("appname") {
    auto usage = ca.usage();

    REQUIRE(usage == "Usage: name [options]");
  }
}

TEST_CASE("constructor: simple (vector)", "[constructor]") {
  const vector<string> argv = {
    "/usr/bin/name",
  };

  CheckArg ca("name");

  auto ret = ca.parse(argv);
  REQUIRE(ret == CA_ALLOK);

  SECTION("call name") {
    auto ret2 = ca.argv0();
    REQUIRE(ret2 == "/usr/bin/name");
  }

  SECTION("appname") {
    auto usage = ca.usage();

    REQUIRE(usage == "Usage: name [options]");
  }
}


#ifdef HAS_STD_FILESYSTEM
TEST_CASE("constructor: app name autodetect", "[constructor]") {
  const vector<string> argv = {
    "/usr/bin/name",
  };

  CheckArg ca;

  auto ret = ca.parse(argv);
  REQUIRE(ret == CA_ALLOK);

  SECTION("call name") {
    auto ret2 = ca.argv0();
    REQUIRE(ret2 == "/usr/bin/name");
  }

  SECTION("appname") {
    auto usage = ca.usage();

    REQUIRE(usage == "Usage: name [options]");
  }
}
#endif
