
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

checkarg_init "test06"

checkarg_add_autohelp

checkarg_add      "a" "alpha"                "opt a"
checkarg_add_cb   "b" "beta"    "callback_b" "opt b"
checkarg_add      "c" "gamma"                "opt c"
checkarg_add_cb   "d" "delta"   "callback_d" "opt d"

checkarg_parse "$@"
caret="$?"

allset=1

if ! checkarg_isset "alpha"; then
	allset=0
fi
if ! checkarg_isset "beta"; then
	allset=0
fi
if ! checkarg_isset "gamma"; then
	allset=0
fi
if ! checkarg_isset "delta"; then
	allset=0
fi

if [[ $caret -gt 0 ]]; then
	exit $caret
elif [[ $allset == 0 ]]; then
	exit 99
else
	exit 0
fi

