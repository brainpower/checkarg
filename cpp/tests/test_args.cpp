
#define CA_PRINTERR

#include "../src/checkarg++.hpp"

using namespace std;

int main(int argc, char **argv){

	CheckArg ca(argc, argv, "test_args");

	ca.add_autohelp();
	ca.add('i', "input", "file to read from", true);

	return ca.parse();


}
