#!/bin/sh

printf " * Testing --help ... "
if ./test_args --help >/dev/null; then
	printf "[ok]\n"
else
	printf "[fail]\n"
fi

printf " * Testing missing value return code ... "
./test_args -i 2>/dev/null
if [ $? = 4 ]; then
	printf "[ok]\n"
else
	printf "[fail]\n"
fi
