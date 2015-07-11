#!/bin/bash

script_dir="$(dirname $(readlink -f "${0}"))"

source "${script_dir}/../checkarg.sh"

checkarg_init "test01" "checkarg test #1"
checkarg_add "d" "debug" "enable debug messages"
checkarg_add "o" "output" "output file" 1
checkarg_add_long "long-only" "option only long"
checkarg_add_autohelp

checkarg_parse "$@"
