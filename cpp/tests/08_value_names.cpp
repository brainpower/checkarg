#include "test.hpp"


TEST_CASE("variable names", "[val-names]") {
  vector<string> argv = {
    "./test08",

  };

	CheckArg ca(argv, "test08");

	ca.add_autohelp();
	ca.add('i', "input", "file to read from", "IPT", CA_VT_REQUIRED);
  ca.add('a', "alpha", "alpha option",      "A",   CA_VT_REQUIRED);
  ca.add(     "beta",  "beta option",       "B",   CA_VT_REQUIRED);

  ca.add(
    'c',
    "gamma",
    [](auto, const auto &, const auto &) -> int {return 0;},
    "gamma option",
    "C",
    CA_VT_REQUIRED
  );
  ca.add(
    "delta",
    [](auto, const auto &, const auto &) -> int {return 0;},
    "delta option",
    "D",
    CA_VT_REQUIRED
  );

	int rc = ca.parse();
	auto help = ca.autohelp();

	CHECK(CA_ALLOK == rc);

	REQUIRE(help ==
	  "Usage: test08 [options]\n"
	  "\n"
	  "Options:\n"
    "   -a, --alpha=A    alpha option\n"
    "       --beta=B     beta option\n"
    "       --delta=D    delta option\n"
    "   -c, --gamma=C    gamma option\n"
    "   -h, --help       show this help message and exit\n"
    "   -i, --input=IPT  file to read from\n"
	);

}
