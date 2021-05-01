
catch2_dep = dependency('catch2', version: '>2')
#if not catch2_dep.found()
#  check2 = subproject('catch2')




tests = {
}

foreach filename, name : tests
  test_driver = executable(
    'test_' + filename,
    files([filename + '.c', 'test_driver.c']),
    install: false,
    dependencies : [checkarg_dep, catch2_dep],
    c_args: ['-Wall', '-Werror']
  )
  test(name, test_driver, args: ['-r', 'tap', '-i'], protocol: 'tap')
endforeach
