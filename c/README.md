# Building

You've got two options: Use the very basic Makefile or build using the sophisticated meson build system.

## Meson

Use the usual three commands to build:

    mkdir builddir && cd builddir
    meson ..
    ninja

Meson usually builds the shared library by default.
To build the static library or both, use `-Ddefault_library=static` or `-Ddefault_library=both` options of meson respectively.

See [Running Meson](https://mesonbuild.com/Running-Meson.html) for more information on how to build using meson.

"Including" using mesons `subproject()` or `dependency()`'s fallback option is supported, making it easier to integrate into projects.
See [Subprojects](https://mesonbuild.com/Subprojects.html) for more information on this.

## Makefile

The Makefile builds and links the shared library by default.
The `install` target installs the shared library only.
To build the static library you can use the target `static`.
