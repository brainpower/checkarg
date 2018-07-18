
project('checkarg++', 'cpp',
	default_options: ['cpp_std=c++14', 'b_ndebug=if-release'],
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
cdata = configuration_data()
cdata.set_quoted('CA_VER_STRING', meson.project_version())

cdata.set10('CA_PRINTERR', get_option('printerr'))

cdata.set10(
	'HAS_VASPRINTF',
	compiler.has_function('vasprintf', prefix : '#include <cstdio>')
)
# }}}

# {{{ binaries
configure_file(
	output: 'config.h',
	configuration: cdata
)

lib = library(
	'checkarg++', sources,
	version: meson.project_version(),
	pic: true,
	install: true
)

checkargpp_dep = declare_dependency(
	include_directories: include_directories('src'),
	link_with: lib,
	version: meson.project_version()
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
	libraries: lib,
	subdirs: ['checkarg++', ],
	version: meson.project_version(),
	name: 'libcheckarg++',
	filebase: 'checkarg++',
	description: 'A command line argument parser for C++.'
)