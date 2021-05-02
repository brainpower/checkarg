add_languages('cpp')

catch2_dep = dependency('catch2', version: '>2', fallback: 'catch2')
#if not catch2_dep.found()
#  check2 = subproject('catch2')




tests = {
  '01_constructors':    'constructors',
  '02_basic_parsing':   'basic parsing',
  '03_setters':         'setters',
  '04_positional_args': 'positional arg separator',
  '05_non_value':       'options with no values',
  '06_value':           'options with values',
  '07_grouped_shorts':  'options grouped short',
  '08_value_names':     'value names',
}

foreach filename, name : tests
  test_driver = executable(
    'test_' + filename,
    files([filename + '.cpp', 'test_driver.cpp']),
    install: false,
    dependencies : [checkarg_dep, catch2_dep],
    cpp_args: ['-Wall', '-Werror'],
    link_language: 'cpp'
  )
  test(name, test_driver, args: ['-r', 'tap', '-i'], protocol: 'tap')
endforeach
