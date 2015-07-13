#!/bin/bash
script_dir="$(dirname $(readlink -f "${0}"))"
source "${script_dir}/../checkarg.sh"

checkarg_init "test03"

checkarg_add_autohelp

checkarg_add "i" "input" "filr to read from" 1
checkarg_add "t" "test"  "run pos_arg test"  1

checkarg_set_posarg_help " [files...]" "one or more output files"

checkarg_parse "$@"
ret="$?"

for arg in "${checkarg_pos_args[@]}"; do
	printf "%s\n" "$arg"
done

exit "$ret"

