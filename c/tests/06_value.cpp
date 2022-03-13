// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>

#include "test.hpp"


string cb_option, cb_value;

int
callback(CheckArg *, const char *o, const char *v) {
  cb_option = o;
  cb_value  = v;
  return 0;
}


TEST_CASE("options: short value", "[opt-val]") {
  vector<const char *> argv;

  string option = GENERATE("alpha", "beta");
  string optstr;

  const string valstr = "val-" + option;

  cb_option.clear();
  cb_value.clear();


  SECTION("separated value") {
    optstr = string("-") + option[0];
    argv   = {
      "/test06_value",
      optstr.c_str(),
      valstr.c_str(),
    };
  }
  SECTION("combined value") {
    optstr = string("-") + option[0] + valstr;
    argv   = {
      "/test06_value",
      optstr.c_str(),
    };
  }

  CheckArgUPtr ca(checkarg_new("test06", NULL, NULL), &checkarg_free);
  checkarg_add(ca.get(), 'a', "alpha", "value opt a", CA_VT_REQUIRED, NULL);
  checkarg_add_cb(ca.get(), 'b', "beta", callback, "value opt b", CA_VT_REQUIRED, NULL);
  checkarg_add_long(ca.get(), "gamma", "value long opt gamma", CA_VT_REQUIRED, NULL);
  checkarg_add_long_cb(
    ca.get(), "delta", callback, "value long opt delta", CA_VT_REQUIRED, NULL);
  checkarg_add(ca.get(), 'e', "epsilon", "value opt e", CA_VT_REQUIRED, NULL);

  int rc = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());

  CHECK(CA_ALLOK == rc);

  REQUIRE(checkarg_isset(ca.get(), option.c_str()));
  CHECK(!checkarg_isset(ca.get(), "gamma"));
  CHECK(!checkarg_isset(ca.get(), "delta"));
  CHECK(!checkarg_isset(ca.get(), "epsilon"));

  REQUIRE(valstr == checkarg_value(ca.get(), option.c_str()));

  if (option == "beta") {
    CHECK("beta" == cb_option);
    REQUIRE(valstr == cb_value);
  }
}

TEST_CASE("options: long value", "[opt-val]") {
  vector<const char *> argv;

  string option = GENERATE("alpha", "beta", "gamma", "delta");
  string optstr;

  const string valstr = "val-" + option;

  cb_option.clear();
  cb_value.clear();

  SECTION("separated value") {
    optstr = "--" + option;
    argv   = {
      "/test06_value",
      optstr.c_str(),
      valstr.c_str(),
    };
  }
  SECTION("combined value") {
    optstr = "--" + option + "=" + valstr;
    argv   = {
      "/test06_value",
      optstr.c_str(),
    };
  }

  CheckArgUPtr ca(checkarg_new("test06", NULL, NULL), &checkarg_free);

  checkarg_add(ca.get(), 'a', "alpha", "value opt a", CA_VT_REQUIRED, NULL);
  checkarg_add_cb(
    ca.get(), 'b', "beta", callback, "non-value opt b", CA_VT_REQUIRED, NULL);
  checkarg_add_long(
    ca.get(), "gamma", "non-value long opt gamma", CA_VT_REQUIRED, NULL);
  checkarg_add_long_cb(
    ca.get(), "delta", callback, "non-value long opt delta", CA_VT_REQUIRED, NULL);
  checkarg_add(ca.get(), 'e', "epsilon", "non-value opt e", CA_VT_REQUIRED, NULL);

  int rc = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());

  CHECK(CA_ALLOK == rc);

  REQUIRE(checkarg_isset(ca.get(), option.c_str()));
  CHECK(!checkarg_isset(ca.get(), "epsilon"));

  REQUIRE(valstr == checkarg_value(ca.get(), option.c_str()));

  if (option == "beta" || option == "delta") {
    CHECK(option == cb_option);
    REQUIRE(valstr == cb_value);
  }
}

TEST_CASE("options: missing value", "[opt-val]") {
  const char *option = GENERATE("-a", "--alpha", "-b", "--beta", "--gamma", "--delta");

  vector<const char *> argv = {
    "/test06",
    option,
  };

  CheckArgUPtr ca(checkarg_new("test06", NULL, NULL), &checkarg_free);
  checkarg_add(ca.get(), 'a', "alpha", "non-value opt a", CA_VT_REQUIRED, NULL);
  checkarg_add_cb(
    ca.get(), 'b', "beta", callback, "non-value opt b", CA_VT_REQUIRED, NULL);
  checkarg_add_long(
    ca.get(), "gamma", "non-value long opt gamma", CA_VT_REQUIRED, NULL);
  checkarg_add_long_cb(
    ca.get(), "delta", callback, "non-value long opt delta", CA_VT_REQUIRED, NULL);
  checkarg_add(ca.get(), 'e', "epsilon", "non-value opt e", CA_VT_REQUIRED, NULL);

  int rc = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());

  REQUIRE(CA_MISSVAL == rc);
}

TEST_CASE("options: empty value", "[opt-val]") {
  vector<const char *> argv;
  SECTION("empty string separated from option") {
    const char *option =
      GENERATE("-a", "--alpha", "-b", "--beta", "--gamma", "--delta");
    argv = {
      "/test06",
      option,
      "",
    };
  }
  SECTION("empty string combined with option") {
    // this does not work with short options, there is no '=' for those
    const char *option = GENERATE("--alpha=", "--beta=", "--gamma=", "--delta=");
    argv               = {
      "/test06",
      option,
    };
  }

  CheckArgUPtr ca(checkarg_new("test06", NULL, NULL), &checkarg_free);
  checkarg_add(ca.get(), 'a', "alpha", "non-value opt a", CA_VT_REQUIRED, NULL);
  checkarg_add_cb(
    ca.get(), 'b', "beta", callback, "non-value opt b", CA_VT_REQUIRED, NULL);
  checkarg_add_long(
    ca.get(), "gamma", "non-value long opt gamma", CA_VT_REQUIRED, NULL);
  checkarg_add_long_cb(
    ca.get(), "delta", callback, "non-value long opt delta", CA_VT_REQUIRED, NULL);
  checkarg_add(ca.get(), 'e', "epsilon", "non-value opt e", CA_VT_REQUIRED, NULL);

  int rc = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());

  INFO("option was:" << argv[1]);
  REQUIRE(CA_ALLOK == rc);  // an empty string as value should not trigger a CA_MISSVAL
}

TEST_CASE("options: special values", "[opt-val]") {
  string value = GENERATE("=test", "-test", "--test", "-", "--");

  vector<const char *> argv;

  string optstr;

  SECTION("long and separated") {
    argv = {
      "/test06",
      "--input",
      value.c_str(),
    };
  }
  SECTION("short and separated") {
    argv = {
      "/test06",
      "-i",
      value.c_str(),
    };
  }
  SECTION("long and combined") {
    optstr = "--input=" + value;
    argv   = {
      "/test06",
      optstr.c_str(),
    };
  }
  SECTION("short and combined") {
    optstr = "-i" + value;
    argv   = {
      "/test06",
      optstr.c_str(),
    };
  }

  CheckArgUPtr ca(checkarg_new("test06", NULL, NULL), &checkarg_free);
  checkarg_add(ca.get(), 'i', "input", "input file", CA_VT_REQUIRED, NULL);

  int rc = checkarg_parse(ca.get(), argv.size(), (char **)argv.data());

  CHECK(CA_ALLOK == rc);

  REQUIRE(value == checkarg_value(ca.get(), "input"));
}
