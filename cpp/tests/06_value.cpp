// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"


TEST_CASE("options: short value", "[opt-val]") {
  string option = GENERATE("alpha", "beta");
  vector<string> argv;

  SECTION("separated value") {
    argv = {
      "/test06_value",
      "-" + string(1, option[0]),
      "val-" + option,
    };
  }
  SECTION("combined value") {
    argv = {
      "/test06_value",
      "-" + string(1, option[0]) + "val-" + option,
    };
  }

  string cb_option;
  string cb_value;

  CheckArg ca(argv, "test06");
  ca.add('a', "alpha", "value opt a", CA_VT_REQUIRED);
  ca.add(
    'b', "beta",
    [&](auto, auto &o, auto &v) -> int {
      cb_option = o;
      cb_value  = v;
      return 0;
    },
    "value opt b", CA_VT_REQUIRED);
  ca.add("gamma", "value long opt gamma", CA_VT_REQUIRED);
  ca.add(
    "delta",
    [&](auto, auto &o, auto &v) -> int {
      cb_option = o;
      cb_value  = v;
      return 0;
    },
    "value long opt delta", CA_VT_REQUIRED);
  ca.add('e', "epsilon", "value opt e", CA_VT_REQUIRED);

  int rc = ca.parse();

  CHECK(CA_ALLOK == rc);

  REQUIRE(ca.isset(option));
  CHECK(!ca.isset("gamma"));
  CHECK(!ca.isset("delta"));
  CHECK(!ca.isset("epsilon"));

  REQUIRE(("val-" + option) == ca.value(option));

  if (option == "beta") {
    CHECK("beta" == cb_option);
    REQUIRE(("val-" + option) == cb_value);
  }
}

TEST_CASE("options: long value", "[opt-val]") {
  string option = GENERATE("alpha", "beta", "gamma", "delta");
  vector<string> argv;

  SECTION("separated value") {
    argv = {
      "/test06_value",
      "--" + option,
      "val-" + option,
    };
  }
  SECTION("combined value") {
    argv = {
      "/test06_value",
      "--" + option + "=val-" + option,
    };
  }

  string cb_option;
  string cb_value;

  CheckArg ca(argv, "test06");
  ca.add('a', "alpha", "non-value opt a", CA_VT_REQUIRED);
  ca.add(
    'b', "beta",
    [&](auto, auto &o, auto &v) -> int {
      cb_option = o;
      cb_value  = v;
      return 0;
    },
    "non-value opt b", CA_VT_REQUIRED);
  ca.add("gamma", "non-value long opt gamma", CA_VT_REQUIRED);
  ca.add(
    "delta",
    [&](auto, auto &o, auto &v) -> int {
      cb_option = o;
      cb_value  = v;
      return 0;
    },
    "non-value long opt delta", CA_VT_REQUIRED);
  ca.add('e', "epsilon", "non-value opt e", CA_VT_REQUIRED);

  int rc = ca.parse();

  CHECK(CA_ALLOK == rc);

  REQUIRE(ca.isset(option));
  CHECK(!ca.isset("epsilon"));

  REQUIRE(("val-" + option) == ca.value(option));

  if (option == "beta" || option == "delta") {
    CHECK(option == cb_option);
    REQUIRE(("val-" + option) == cb_value);
  }
}

TEST_CASE("options: missing value", "[opt-val]") {
  string option       = GENERATE("-a", "--alpha", "-b", "--beta", "--gamma", "--delta");
  vector<string> argv = {
    "/test06",
    option,
  };

  CheckArg ca(argv, "test06");
  ca.add('a', "alpha", "non-value opt a", CA_VT_REQUIRED);
  ca.add(
    'b', "beta", [&](auto, auto &o, auto &v) -> int { return 0; }, "non-value opt b",
    CA_VT_REQUIRED);
  ca.add("gamma", "non-value long opt gamma", CA_VT_REQUIRED);
  ca.add(
    "delta", [&](auto, auto &o, auto &v) -> int { return 0; },
    "non-value long opt delta", CA_VT_REQUIRED);
  ca.add('e', "epsilon", "non-value opt e", CA_VT_REQUIRED);

  int rc = ca.parse();

  REQUIRE(CA_MISSVAL == rc);
}

TEST_CASE("options: empty value", "[opt-val]") {
  vector<string> argv;
  SECTION("empty string separated from option") {
    string option = GENERATE("-a", "--alpha", "-b", "--beta", "--gamma", "--delta");
    argv          = {
      "/test06",
      option,
      "",
    };
  }
  SECTION("empty string combined with option") {
    // this does not work with short options, there is no '=' for those
    string option = GENERATE("--alpha=", "--beta=", "--gamma=", "--delta=");
    argv          = {
      "/test06",
      option,
    };
  }

  CheckArg ca(argv, "test06");
  ca.add('a', "alpha", "non-value opt a", CA_VT_REQUIRED);
  ca.add(
    'b', "beta", [&](auto, auto &o, auto &v) -> int { return 0; }, "non-value opt b",
    CA_VT_REQUIRED);
  ca.add("gamma", "non-value long opt gamma", CA_VT_REQUIRED);
  ca.add(
    "delta", [&](auto, auto &o, auto &v) -> int { return 0; },
    "non-value long opt delta", CA_VT_REQUIRED);
  ca.add('e', "epsilon", "non-value opt e", CA_VT_REQUIRED);

  int rc = ca.parse();

  INFO("option was:" << argv[1]);
  REQUIRE(CA_ALLOK == rc);  // an empty string as value should not trigger a CA_MISSVAL
}

TEST_CASE("options: special values", "[opt-val]") {
  string value = GENERATE("=test", "-test", "--test", "-", "--");
  vector<string> argv;

  SECTION("long and separated") {
    argv = {
      "/test06",
      "--input",
      value,
    };
  }
  SECTION("short and separated") {
    argv = {
      "/test06",
      "-i",
      value,
    };
  }
  SECTION("long and combined") {
    argv = {
      "/test06",
      "--input=" + value,
    };
  }
  SECTION("short and combined") {
    argv = {
      "/test06",
      "-i" + value,
    };
  }

  CheckArg ca(argv, "test06");
  ca.add('i', "input", "input file", CA_VT_REQUIRED);

  int rc = ca.parse();

  CHECK(CA_ALLOK == rc);

  REQUIRE(value == ca.value("input"));
}
