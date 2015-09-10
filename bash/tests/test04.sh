
#!/bin/bash
script_dir="$(dirname $(readlink -f "${0}"))"
source "${script_dir}/../checkarg.sh"

ret_test04=0

callback_b(){
	true
}
callback_d(){
	true
}
callback_e(){
	ret_test04=104
}
callback_z(){
	ret_test04=105
}

checkarg_init "test04"

checkarg_add_autohelp

checkarg_add      "a" "alpha"                "non-value opt a"
checkarg_add_cb   "b" "beta"    "callback_b" "non-value opt b"
checkarg_add_long     "gamma"                "non-value long opt gamma"
checkarg_add_long_cb  "delta"   "callback_d" "non-value long opt delta"
checkarg_add_cb   "e" "epsilon" "callback_e" "non-value opt e"
checkarg_add_long_cb  "zeta"    "callback_z" "non-value long opt zeta"

checkarg_parse "$@"
caret="$?"




if checkarg_isset "alpha"; then
	ret_test04=100
elif checkarg_isset "beta"; then
	ret_test04=101
elif checkarg_isset "gamma"; then
	ret_test04=102
elif checkarg_isset "delta"; then
	ret_test04=103
fi

if [[ "$caret" -gt 0 ]]; then
	exit $caret
fi
exit "$ret_test04"

