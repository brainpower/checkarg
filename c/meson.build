
project('checkarg', 'c',
	default_options: ['c_std=c11'],
	version: '1.2.0'
)
add_global_arguments('-D_GNU_SOURCE', language : 'c')

# {{{ files
sources = [
	'src/checkarg.c'
]
#public headers
headers = [
	'./src/checkarg.h'
]
# }}}

# {{{ config
compiler = meson.get_compiler('c')
conf_inc = include_directories('.')
c_args = []

cdata = configuration_data()
cdata.set_quoted('CA_VER_STRING', meson.project_version())
cdata.set10('CA_PRINTERR', get_option('printerr'))


if false and compiler.has_function('basename', prefix: '#include <libgen.h>')
	c_args += '-DHAS_POSIX_BASENAME'
endif
# }}}

# {{{ binaries
configure_file(
	output: 'config.h',
	configuration: cdata
)

checkarg_lib = library(
	'checkarg', sources,
	version: meson.project_version(),
	pic: true,
	install: true,
	c_args: c_args,
)

checkarg_dep = declare_dependency(
	include_directories: include_directories('src'),
	link_with: checkarg_lib,
	version: meson.project_version(),
	compile_args: c_args,
)
# }}}

# {{{ install data and headers

install_headers(
	headers,
	subdir: 'checkarg'
)

# }}}

# {{{ pkg-config
pkgcfg = import('pkgconfig')
pkgcfg.generate(
	libraries: checkarg_dep,
	subdirs: ['checkarg', ],
	version: meson.project_version(),
	name: 'libcheckarg',
	filebase: 'checkarg',
	description: 'A command line argument parser for C.'
)


if get_option('tests')
  subdir('tests')
endif
