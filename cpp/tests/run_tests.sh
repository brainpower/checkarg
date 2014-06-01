#!/bin/sh
script_dir="$(dirname $(readlink -f "${0}"))"

compile_sources(){
  compile_$type
}

compile_cpp(){
  g++ -std=c++11 "${sources[@]}" -o $name
}

clean(){
  rm -f *.o
  rm -f "$name"
}

test_rc(){
  local tname="$1"
  local expect="$2"
  shift 2;

  printf "   - Running test: ${tname}... "
  ./$name "$@" >/dev/null
  
  if [[ $expect == $? ]]; then
    printf "[ ok ]\n"
  else
    printf "[ failed ]\n"
  fi

}

test_stdout(){
  local tname="$1"
  local expect="$2"
  shift 2
  local tmpout="${name}.${tname// /_}.out"

  printf "   - Running test: ${tname}... "
  ./$name "$@" > "$tmpout" 

  if diff -Nru $expect "$tmpout"; then
    printf "[ ok ]\n"
  else
    printf "[ failed ]\n"
  fi
  rm -f "$tmpout"
}

cd "${script_dir}"
for test in *.bpt; do
  if [ -x "$test" ]; then
    (
    . ./"$test"
    printf "\n * Running test set ${name}...\n"
    prepare
    run
    clean
    )
  fi
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
