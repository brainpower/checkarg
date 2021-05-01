
#include "catch2/catch.hpp"

#include "checkarg++.hpp"

#include <cstring>
#include <string>
#include <vector>

using std::vector;
using std::string;

TEST_CASE("constructor: simple", "[constructor]"){
  int argc = 1;
  const char* argv[] = {
    "/usr/bin/name",
    NULL,
  };

  CheckArg ca(argc, (char**)argv, "name");

  auto ret = ca.parse();
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

TEST_CASE("constructor: simple (vector)", "[constructor]"){
  const vector<string> argv = {
    "/usr/bin/name",
  };

  CheckArg ca(argv, "name");

  auto ret = ca.parse();
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
TEST_CASE("constructor: app name autodetect", "[constructor]"){
  const vector<string> argv = {
    "/usr/bin/name",
  };

  CheckArg ca(argv);

  auto ret = ca.parse();
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

