#include "test.hpp"

TEST_CASE("grouped short options", "[grouped]") {
  // long options cannot be grouped, so those dont apply
  vector<string> argv = {
    "/test07",
  };
  {
    vector<string> options = GENERATE(
      (vector<string>{"-abcd"}),
      (vector<string>{"-ab", "-cd"}),
      (vector<string>{"-abc", "-d"}),
      (vector<string>{"-abc", "--delta"}),
      (vector<string>{"-ab", "--gamma", "-d"}),
      (vector<string>{"-ac", "--beta", "--delta"}),
      (vector<string>{"--alpha", "-bcd"})
    );
    argv.insert(argv.end(), std::move_iterator(options.begin()), std::move_iterator(options.end()));
  } // destroy options here, it's elements are "moved from"

  CheckArg ca(argv, "test07");
	ca.add('a', "alpha",               "opt a");
	ca.add('b', "beta",    [](auto, const auto&, const auto&) -> int {return 0;}, "opt b");
	ca.add('c', "gamma",               "opt gamma");
	ca.add('d', "delta",   [](auto, const auto&, const auto&) -> int {return 0;}, "opt delta");
	ca.add('e', "epsy",                "opt e");

  int rc = ca.parse();

  INFO("argv:" << argv);
  CHECK(rc == CA_ALLOK);

  REQUIRE(ca.isset("alpha"));
  REQUIRE(ca.isset("beta"));
  REQUIRE(ca.isset("gamma"));
  REQUIRE(ca.isset("delta"));
  REQUIRE(!ca.isset("epsy"));
}
