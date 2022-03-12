// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"


TEST_CASE("variable names", "[val-names]") {
  vector<string> argv = {
    "./test08",

  };

  CheckArg ca("test08");

  ca.add_autohelp();
  ca.add('z', "ag", "auto-generated name", CA_VT_REQUIRED, "");
  ca.add('y', "ng", "no value name", CA_VT_REQUIRED);
  ca.add('i', "input", "file to read from", CA_VT_REQUIRED, "IPT");
  ca.add('a', "alpha", "alpha option", CA_VT_REQUIRED, "A");
  ca.add("beta", "beta option", CA_VT_REQUIRED, "B");

  ca.add(
    'c', "gamma", "gamma option",
    [](auto, const auto &, const auto &) -> int { return 0; }, CA_VT_REQUIRED, "C");
  ca.add(
    "delta", "delta option", [](auto, const auto &, const auto &) -> int { return 0; },
    CA_VT_REQUIRED, "D");

  int rc    = ca.parse(argv);
  auto help = ca.autohelp();

  CHECK(CA_ALLOK == rc);

  REQUIRE(help ==
    "Usage: test08 [options]\n"
    "\n"
    "Options:\n"
    "   -z, --ag=AG      auto-generated name\n"
    "   -a, --alpha=A    alpha option\n"
    "       --beta=B     beta option\n"
    "       --delta=D    delta option\n"
    "   -c, --gamma=C    gamma option\n"
    "   -h, --help       show this help message and exit\n"
    "   -i, --input=IPT  file to read from\n"
    "   -y, --ng         no value name\n"
  //"   -y, --ng=        no value name\n"
  );
}
