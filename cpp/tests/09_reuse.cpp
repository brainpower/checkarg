// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2022 brainpower <brainpower at mailbox dot org>
#include "test.hpp"


TEST_CASE("reuse", "[reuse]") {
  vector<string> argv1 = {"/test08", "-a", "a-val"};
  vector<string> argv2 = {"/test08", "--beta", "b-val"};
  vector<string> argv3 = {"/test08", "-c", "c-val"};
  vector<string> argv4 = {"/test08", "--delta", "d-val"};
  vector<string> argv5 = {"/test08", "-z", "z-val"};
  vector<string> argv6 = {"/test08", "--ag", "ag-val"};
  vector<string> argv7 = {"/test08", "--ng", "ng-val"};

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

  int rc    = ca.parse(argv1);
  CHECK(CA_ALLOK == rc);
  REQUIRE(ca.isset("alpha"));
  REQUIRE(ca.value("alpha") == "a-val");

  rc = ca.parse(argv2);
  CHECK(CA_ALLOK == rc);
  REQUIRE(!ca.isset("alpha"));
  REQUIRE(ca.value("alpha").empty());
  REQUIRE(ca.isset("beta"));
  REQUIRE(ca.value("beta") == "b-val");

  rc = ca.parse(argv3);
  CHECK(CA_ALLOK == rc);
  REQUIRE(!ca.isset("alpha"));
  REQUIRE(ca.value("alpha").empty());
  REQUIRE(!ca.isset("beta"));
  REQUIRE(ca.value("beta").empty());
  REQUIRE(ca.isset("gamma"));
  REQUIRE(ca.value("gamma") == "c-val");

  rc = ca.parse(argv4);
  CHECK(CA_ALLOK == rc);
  REQUIRE(!ca.isset("alpha"));
  REQUIRE(ca.value("alpha").empty());
  REQUIRE(!ca.isset("beta"));
  REQUIRE(ca.value("beta").empty());
  REQUIRE(!ca.isset("gamma"));
  REQUIRE(ca.value("gamma").empty());
  REQUIRE(ca.isset("delta"));
  REQUIRE(ca.value("delta") == "d-val");

  rc = ca.parse(argv5);
  CHECK(CA_ALLOK == rc);
  REQUIRE(!ca.isset("alpha"));
  REQUIRE(ca.value("alpha").empty());
  REQUIRE(!ca.isset("beta"));
  REQUIRE(ca.value("beta").empty());
  REQUIRE(!ca.isset("gamma"));
  REQUIRE(ca.value("gamma").empty());
  REQUIRE(!ca.isset("delta"));
  REQUIRE(ca.value("delta").empty());
  REQUIRE(ca.isset("ag"));
  REQUIRE(ca.value("ag") == "z-val");

  rc = ca.parse(argv6);
  CHECK(CA_ALLOK == rc);
  REQUIRE(!ca.isset("alpha"));
  REQUIRE(ca.value("alpha").empty());
  REQUIRE(!ca.isset("beta"));
  REQUIRE(ca.value("beta").empty());
  REQUIRE(!ca.isset("gamma"));
  REQUIRE(ca.value("gamma").empty());
  REQUIRE(!ca.isset("delta"));
  REQUIRE(ca.value("delta").empty());
  REQUIRE(ca.isset("ag"));
  REQUIRE(ca.value("ag") == "ag-val");

  rc = ca.parse(argv7);
  CHECK(CA_ALLOK == rc);
  REQUIRE(!ca.isset("alpha"));
  REQUIRE(ca.value("alpha").empty());
  REQUIRE(!ca.isset("beta"));
  REQUIRE(ca.value("beta").empty());
  REQUIRE(!ca.isset("gamma"));
  REQUIRE(ca.value("gamma").empty());
  REQUIRE(!ca.isset("delta"));
  REQUIRE(ca.value("delta").empty());
  REQUIRE(!ca.isset("ag"));
  REQUIRE(ca.value("ag").empty());
  REQUIRE(ca.isset("ng"));
  REQUIRE(ca.value("ng") == "ng-val");
}
