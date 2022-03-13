#!/bin/bash
script_dir="$(dirname "$(readlink -f "${0}")")"

SELF="$(readlink /proc/$$/exe)" || true
if [[ -z "$SELF" ]]; then
  SELF="$(ps -p $$ --no-headers -o cmd)" || true
fi
SELF="${SELF:-${SHELL:-bash}}"

printf "Running tests using: %s\n\n" "$SELF"


ok(){
  printf "\033[39;1m[ \033[32;1m ok \033[39;1m ]\033[0m\n"
}

fail(){
  printf "\033[39;1m[ \033[31;1mfail\033[39;1m ]\033[0m\n"
}

test_rc(){
  local tname="$1"
  local expect="$2"
  shift 2;

  printf "   - Running test: %s... " "${tname}"
  $SELF "$name" "$@" >/dev/null ; ret=$?

  if [[ $expect == "$ret" ]]; then
    ok
  else
    fail
    printf "Expected: %s - Got: %s\n" "$expect" "$ret"
  fi

}

test_stdout(){
  local tname="$1"
  local expect="$2"
  shift 2
  local tmpout="${name}.${tname// /_}.out"

  printf "   - Running test: %s... " "${tname}"
  $SELF "$name" "$@" > "$tmpout"

  # --strip-trailing-cr fixes test run on windows,
  # while only skipping \r not all whitespace like -w would
  if diff -Nru --strip-trailing-cr "$expect" "$tmpout"; then
    ok
  else
    fail
  fi
  rm -f "$tmpout"
}

test_stdout_str(){
  local tname="$1"
  local expect="$2"
  shift 2
  local tmpout="${name}.${tname// /_}.out"

  printf "   - Running test: %s... " "${tname}"
	tmpout="$($SELF "$name" "$@")"

  if [[ "$expect" == "$tmpout" ]]; then
    ok
  else
    fail
  	echo "Exp: '$expect'"
  	echo "Got: '$tmpout'"
  fi
}

cd "${script_dir}" || exit 1
for test in *.bpt; do
    (
    # shellcheck source=test01.bpt
    . ./"$test"
    printf "\n * Running test set %s...\n" "${name}"
    prepare || exit 1
    run
    clean
    )
done

#printf " * Testing --help ... "
#if tests/test_args --help >/dev/null; then
#	printf "[ok]\n"
#else
#	printf "[fail]\n"
#fi

#printf " * Testing missing value return code ... "
#tests/test_args -i 2>/dev/null
#if [ $? = 4 ]; then
#	printf "[ok]\n"
#else
#	printf "[fail]\n"
#fi
