// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>

#include "test.hpp"

int
callback(CheckArg *, const char *, const char *) {
  return 0;
}

TEST_CASE("grouped short options", "[grouped]") {
  // long options cannot be grouped, so those dont apply
  vector<const char *> argv = {
    "/test07",
  };
  {
    vector<const char *> options = GENERATE(
      (vector<const char *>{"-abcd"}), (vector<const char *>{"-ab", "-cd"}),
      (vector<const char *>{"-abc", "-d"}), (vector<const char *>{"-abc", "--delta"}),
      (vector<const char *>{"-ab", "--gamma", "-d"}),
      (vector<const char *>{"-ac", "--beta", "--delta"}),
      (vector<const char *>{"--alpha", "-bcd"}));
    argv.insert(argv.end(), options.begin(), options.end());
  }  // destroy options here, it's elements are literals that live in static memory, so
     // it should be fine

  CheckArgUPtr ca(checkarg_new("test05", NULL, NULL), &checkarg_free);

  // clang-format off
	checkarg_add   (ca.get(), 'a', "alpha",           "opt a", 0, NULL);
	checkarg_add_cb(ca.get(), 'b', "beta",  callback, "opt b", 0, NULL);
	checkarg_add   (ca.get(), 'c', "gamma",           "opt gamma", 0, NULL);
	checkarg_add_cb(ca.get(), 'd', "delta", callback, "opt delta", 0, NULL);
	checkarg_add   (ca.get(), 'e', "epsy",            "opt epsy", 0, NULL);
  // clang-format on

  int rc = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());

  INFO("argv:" << argv);
  CHECK(rc == CA_ALLOK);

  REQUIRE(checkarg_isset(ca.get(), "alpha"));
  REQUIRE(checkarg_isset(ca.get(), "beta"));
  REQUIRE(checkarg_isset(ca.get(), "gamma"));
  REQUIRE(checkarg_isset(ca.get(), "delta"));
  REQUIRE(!checkarg_isset(ca.get(), "epsy"));
}
