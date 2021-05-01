
project('checkarg++', 'cpp',
	default_options: ['cpp_std=c++20', 'b_ndebug=if-release'],
	version: '1.2.0'
)

# {{{ files
sources = [
	'./src/checkarg++.cpp'
]
#public headers
headers = [
	'./src/checkarg++.hpp'
]
# }}}

# {{{ config
compiler = meson.get_compiler('cpp')
conf_inc = include_directories('.')
cpp_args = []

cdata = configuration_data()
cdata.set_quoted('CA_VER_STRING', meson.project_version())
cdata.set10('CA_PRINTERR', get_option('printerr'))
cdata.set10(
	'HAS_VASPRINTF',
	compiler.has_function('vasprintf', prefix : '#include <cstdio>')
)

# FIXME: maybe check for other possibilities, like POSIX basename(3)
# (urg, a very weird one this one is :( )
if compiler.check_header('filesystem')
	cpp_args += '-DHAS_STD_FILESYSTEM'
endif
if compiler.has_function('basename', prefix: '#include <libgen.h>')
	cpp_args += '-DHAS_POSIX_BASENAME'
endif
# }}}

# {{{ binaries
configure_file(
	output: 'config.h',
	configuration: cdata
)

checkargpp_lib = library(
	'checkarg++', sources,
	version: meson.project_version(),
	pic: true,
	install: true,
	cpp_args: cpp_args,
)

checkargpp_dep = declare_dependency(
	include_directories: include_directories('src'),
	link_with: checkargpp_lib,
	version: meson.project_version(),
	compile_args: cpp_args,
)
# }}}

# {{{ install data and headers

install_headers(
	headers,
	subdir: 'checkarg++'
)

# }}}

# {{{ pkg-config
pkgcfg = import('pkgconfig')
pkgcfg.generate(
	libraries: checkargpp_dep,
	subdirs: ['checkarg++', ],
	version: meson.project_version(),
	name: 'libcheckarg++',
	filebase: 'checkarg++',
	description: 'A command line argument parser for C++.',
)


if get_option('tests')
  subdir('tests')
endif
