#!/bin/bash

script_dir="$(dirname "$(readlink -f "${0}")")"
source "${script_dir}/../checkarg.sh"

checkarg_init "test02"

checkarg_add_autohelp

checkarg_add "i" "input" "file to read from" 1
checkarg_add "t" "test" "run pos_arg test" 1

checkarg_set_posarg_help " [files...]" "one or more output files"

checkarg_parse "$@"
ret="$?"

if [[ $ret == 0 ]]; then
	if checkarg_isset "test"; then
		if [[ -v ZSH_VERSION ]]; then
			first_pos_arg="${checkarg_pos_args[1]}"
		else
			first_pos_arg="${checkarg_pos_args[0]}"
		fi

		if [[ "${first_pos_arg}" == "$(checkarg_value "test")" ]]; then
			exit 0
		else
			exit 99
		fi
	fi
fi

exit "$ret"
