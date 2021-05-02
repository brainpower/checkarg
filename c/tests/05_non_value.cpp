#include "test.hpp"
#include <iostream>

int cb_rc = 0;
string cb_opt;


int
callback(CheckArg *const, const char *o, const char *) {
  cb_opt = o;
  return cb_rc;
}


TEST_CASE("options: long non-value options", "[non-val-opt]") {
  cb_rc = 0;
  cb_opt.clear();

  const string opt                = GENERATE("alpha", "beta", "gamma", "delta");
  const string optstr             = "--" + opt;
  const vector<const char *> argv = {
    "/test05",
    optstr.c_str(),
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test05", NULL, NULL),
    &checkarg_free);
  // clang-format off
  checkarg_add        (ca.get(), 'a', "alpha",           "non-value opt a", 0, NULL);
  checkarg_add_cb     (ca.get(), 'b', "beta",  callback, "non-value opt b", 0, NULL);
  checkarg_add_long   (ca.get(),      "gamma",           "non-value long opt gamma", 0, NULL);
  checkarg_add_long_cb(ca.get(),      "delta", callback, "non-value long opt delta", 0, NULL);
  checkarg_add        (ca.get(), 'e', "epsilon",         "non-value long opt epsilon", 0, NULL);
  // clang-format on

  int ret = checkarg_parse(ca.get());

  CHECK(ret == CA_ALLOK);

  // the option was seen and parsed
  REQUIRE(checkarg_isset(ca.get(), opt.c_str()));

  // this option never gets passed, so it should not be set
  CHECK(!checkarg_isset(ca.get(), "epsilon"));


  if (opt == "beta" || opt == "delta") {
    REQUIRE(cb_opt == opt);  // the callback ran!
  }
  else {  //
    REQUIRE(cb_opt == "");
  }
};

TEST_CASE("options: long non-value options (lambda callbacks)", "[non-val-opt]") {
  cb_opt.clear();
  cb_rc = GENERATE(0, 1, 2);

  const string opt    = GENERATE("alpha", "beta", "gamma", "delta");
  const string optstr = "--" + opt;

  const vector<const char *> argv = {
    "/test05",
    optstr.c_str(),
  };


  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test05", NULL, NULL),
    &checkarg_free);
  // clang-format off
  checkarg_add        (ca.get(), 'a', "alpha",           "non-value opt a", 0, NULL);
  checkarg_add_cb     (ca.get(), 'b',  "beta", callback, "non-value opt b", 0, NULL);
  checkarg_add_long   (ca.get(),      "gamma",           "non-value long opt gamma", 0, NULL);
  checkarg_add_long_cb(ca.get(),      "delta", callback, "non-value long opt delta", 0, NULL);
  checkarg_add_long   (ca.get(),    "epsilon",           "non-value long opt", 0, NULL);
  // clang-format on

  int ret = checkarg_parse(ca.get());

  if (opt == "beta" || opt == "delta") {
    // callback ran and if cb_rc == 0
    // it shall report success, a callback failure otherwise
    CHECK(ret == (cb_rc == 0 ? CA_ALLOK : CA_CALLBACK));
  }
  else {
    CHECK(ret == CA_ALLOK);
  }

  REQUIRE(checkarg_isset(ca.get(), opt.c_str()));

  // this option never gets passed, so it should not be set
  CHECK(!checkarg_isset(ca.get(), "epsilon"));

  if (opt == "beta" || opt == "delta") {
    REQUIRE(cb_opt == opt);  // callback ran
  }
  else {
    REQUIRE(cb_opt == "");  // callback did not run
  }
};

TEST_CASE("options: short non-value options", "[non-val-opt]") {
  cb_opt.clear();
  cb_rc = 0;

  const string opt    = GENERATE("alpha", "beta");
  const string optstr = string("-") + opt[0];

  const vector<const char *> argv = {
    "/test05",
    optstr.c_str(),
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test05", NULL, NULL),
    &checkarg_free);
  // clang-format off
  checkarg_add        (ca.get(), 'a', "alpha",           "non-value opt a", 0, NULL);
  checkarg_add_cb     (ca.get(), 'b', "beta",  callback, "non-value opt b", 0, NULL);
  checkarg_add_long   (ca.get(),      "gamma",           "non-value long opt gamma", 0, NULL);
  checkarg_add_long_cb(ca.get(),      "delta", callback, "non-value long opt delta", 0, NULL);
  // clang-format on

  int ret = checkarg_parse(ca.get());

  INFO("arg: " << optstr);
  CHECK(ret == CA_ALLOK);

  REQUIRE(checkarg_isset(ca.get(), opt.c_str()));  // the option was seen and parsed
  CHECK(!checkarg_isset(ca.get(), "gamma"));
  CHECK(!checkarg_isset(ca.get(), "delta"));

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
  cb_opt.clear();
  cb_rc = 0;

  const string option = GENERATE("alpha", "beta", "gamma", "delta");
  const string optstr = "--" + option + "=value";

  const vector<const char *> argv = {
    "/test05",
    optstr.c_str(),
  };

  CheckArgUPtr ca(
    checkarg_new(argv.size(), (char **)argv.data(), "test05", NULL, NULL),
    &checkarg_free);
  // clang-format off
  checkarg_add        (ca.get(), 'a', "alpha",           "non-value opt a", 0, NULL  );
  checkarg_add_cb     (ca.get(), 'b',  "beta", callback, "non-value opt b", 0, NULL);
  checkarg_add_long   (ca.get(),      "gamma",           "non-value long opt gamma", 0, NULL);
  checkarg_add_long_cb(ca.get(),      "delta", callback, "non-value long opt delta", 0, NULL);
  checkarg_add_long   (ca.get(),    "epsilon",           "non-value long opt", 0, NULL);
  // clang-format on

  int ret = checkarg_parse(ca.get());

  REQUIRE(CA_INVVAL == ret);
};
