// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"

TEST_CASE("setters: positional argument help", "setters") {
  vector<string> argv = {
    "/test02",
  };

  CheckArg ca("test02");

  ca.add_autohelp();
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);
  ca.add('t', "test", "run tests", CA_VT_REQUIRED);

  ca.set_posarg_help("[files...]", "one or more output files");

  int ret = ca.parse(argv);

  CHECK(ret == CA_ALLOK);
  REQUIRE(ca.usage() == "Usage: test02 [options] [files...]");
  REQUIRE(ca.autohelp() ==
    "Usage: test02 [options] [files...]\n"
    "\n"
    "Options:\n"
    "   -h, --help   show this help message and exit\n"
    "   -i, --input  file to read from\n"
    "   -t, --test   run tests\n"
    "\n"
    "Positional Arguments:\n"
    "one or more output files\n"
  );
}

TEST_CASE("setters: override usage line", "setters") {
  vector<string> argv = {
    "/test02",
  };

  CheckArg ca("test02");

  ca.add_autohelp();
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED);
  ca.add('t', "test", "run tests", CA_VT_REQUIRED);

  ca.set_usage_line("this is a really stupid usage line");

  int ret = ca.parse(argv);

  CHECK(ret == CA_ALLOK);
  REQUIRE(ca.usage() == "Usage: this is a really stupid usage line");
  REQUIRE(ca.autohelp() ==
    "Usage: this is a really stupid usage line\n"
    "\n"
    "Options:\n"
    "   -h, --help   show this help message and exit\n"
    "   -i, --input  file to read from\n"
    "   -t, --test   run tests\n"
  );
}
