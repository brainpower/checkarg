// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"
#include <iostream>

string cb_opt;

int
callback_b(CheckArg *const, const string &o, const string &) {
  cb_opt = o;
  return 0;
}
int
callback_d(CheckArg *const, const string &o, const string &) {
  cb_opt = o;
  return 0;
}

TEST_CASE("options: long non-value options", "[non-val-opt]") {
  cb_opt.clear();

  string opt                = GENERATE("alpha", "beta", "gamma", "delta");
  const vector<string> argv = {
    "/test05",
    "--" + opt,
  };

  CheckArg ca(argv, "test05");
  ca.add('a', "alpha", "non-value opt a");
  ca.add('b', "beta", callback_b, "non-value opt b");
  ca.add("gamma", "non-value long opt gamma");
  ca.add("delta", callback_d, "non-value long opt delta");
  ca.add('e', "epsilon", "non-value long opt epsilon");

  int ret = ca.parse();

  CHECK(ret == CA_ALLOK);

  // the option was seen and parsed
  REQUIRE(ca.isset(opt));

  // this option never gets passed, so it should not be set
  CHECK(!ca.isset("epsilon"));


  if (opt == "beta" || opt == "delta") {
    REQUIRE(cb_opt == opt);  // the callback ran!
  }
  else {  //
    REQUIRE(cb_opt == "");
  }
};

TEST_CASE("options: long non-value options (lambda callbacks)", "[non-val-opt]") {
  string opt = GENERATE("alpha", "beta", "gamma", "delta");
  int cb_rc  = GENERATE(0, 1, 2);
  string cb_opt2;

  const vector<string> argv = {
    "/test05",
    "--" + opt,
  };

  CheckArg ca(argv, "test05");
  ca.add('a', "alpha", "non-value opt a");
  ca.add(
    'b', "beta",
    [&](auto, const auto &opt, auto &) -> int {
      cb_opt2 = opt;
      return cb_rc;
    },
    "non-value opt b");
  ca.add("gamma", "non-value long opt gamma");
  ca.add(
    "delta",
    [&](auto, const auto &opt, auto &) -> int {
      cb_opt2 = opt;
      return cb_rc;
    },
    "non-value long opt delta");
  ca.add("epsilon", "non-value long opt");

  int ret = ca.parse();

  if (opt == "beta" || opt == "delta") {
    // callback ran and if cb_rc == 0
    // it shall report success, a callback failure otherwise
    CHECK(ret == (cb_rc == 0 ? CA_ALLOK : CA_CALLBACK));
  }
  else {
    CHECK(ret == CA_ALLOK);
  }

  REQUIRE(ca.isset(opt));

  // this option never gets passed, so it should not be set
  CHECK(!ca.isset("epsilon"));

  if (opt == "beta" || opt == "delta") {
    REQUIRE(cb_opt2 == opt);  // callback ran
  }
  else {
    REQUIRE(cb_opt2 == "");  // callback did not run
  }
};

TEST_CASE("options: short non-value options", "[non-val-opt]") {
  cb_opt.clear();

  string opt                = GENERATE("alpha", "beta");
  const vector<string> argv = {
    "/test05",
    "-" + string(1, opt[0]),
  };

  CheckArg ca(argv, "test05");
  ca.add('a', "alpha", "non-value opt a");
  ca.add('b', "beta", callback_b, "non-value opt b");
  ca.add("gamma", "non-value long opt gamma");
  ca.add("delta", callback_d, "non-value long opt delta");

  int ret = ca.parse();

  INFO("arg: -" << string(1, opt[0]));
  CHECK(ret == CA_ALLOK);

  REQUIRE(ca.isset(opt));  // the option was seen and parsed
  CHECK(!ca.isset("gamma"));
  CHECK(!ca.isset("delta"));

  if (opt == "beta") {
    REQUIRE(cb_opt == opt);  // the callback ran!
  }
  else {  //
    REQUIRE(cb_opt == "");
  }
};

TEST_CASE("options: value given to long non-value options", "[non-val-opt]") {
  // this only works with long options of the form: --option=value
  // otherwise values would be interpreted as positional arguments
  // or invalid short options
  string option = GENERATE("alpha", "beta", "gamma", "delta");
  string cb_option;

  const vector<string> argv = {
    "/test05",
    "--" + option + "=value",
  };

  CheckArg ca(argv, "test05");
  ca.add('a', "alpha", "non-value opt a");
  ca.add(
    'b', "beta", [&](auto, const auto &opt, auto &) -> int { return 0; },
    "non-value opt b");
  ca.add("gamma", "non-value long opt gamma");
  ca.add(
    "delta", [&](auto, const auto &opt, auto &) -> int { return 0; },
    "non-value long opt delta");
  ca.add("epsilon", "non-value long opt");

  int ret = ca.parse();

  REQUIRE(CA_INVVAL == ret);
};
