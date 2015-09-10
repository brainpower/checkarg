
#!/bin/bash
script_dir="$(dirname $(readlink -f "${0}"))"
source "${script_dir}/../checkarg.sh"

ret=0

callback_b(){
	true
}
callback_d(){
	true
}
callback_e(){
	echo "$1 $2"
}
callback_z(){
	echo "$1 $2"
}

checkarg_init "test04"

checkarg_add_autohelp

checkarg_add      "a" "alpha"                "value opt a" 1
checkarg_add_cb   "b" "beta"    "callback_b" "value opt b" 1
checkarg_add_long     "gamma"                "value long opt gamma" 1
checkarg_add_long_cb  "delta"   "callback_d" "value long opt delta" 1
checkarg_add_cb   "e" "epsilon" "callback_e" "value opt e" 1
checkarg_add_long_cb  "zeta"    "callback_z" "value long opt zeta" 1

checkarg_parse "$@"
caret="$?"


if checkarg_isset "alpha"; then
	echo "alpha" "$(checkarg_value "alpha")"
elif checkarg_isset "beta"; then
	echo "beta" "$(checkarg_value "beta")"
elif checkarg_isset "gamma"; then
	echo "gamma" "$(checkarg_value "gamma")"
elif checkarg_isset "delta"; then
	echo "delta" "$(checkarg_value "delta")"
fi


exit $caret

