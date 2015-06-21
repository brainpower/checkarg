#.rst:
# FindCheckArg++
# -------
#
# Find the native CheckArg includes and library
#
#
#
# This module searches libcheckarg++, a command line parsing library.
#
# It defines the following variables
#
# ::
#
#   CHECKARG_INCLUDE_DIRS, where to find checkarg++.hpp, etc.
#   CHECKARG_LIBRARIES, the libraries to link against to use checkarg. (this should be empty)
#   CHECKARG_FOUND, If false, do not try to use checkarg.
#   CHECKARG_VERSION_STRING - the version of the PNG library found (since CMake 2.8.8)
#
# Also defined, but not for general use are
#
# ::
#
#   CHECKARG_LIBRARY, where to find the PNG library.
#

#=============================================================================
# Copyright (c) 2013-2015 brainpower <brainpower at gulli dot com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#=============================================================================


find_path(CHECKARG_CA_INCLUDE_DIR checkarg++.hpp
  HINTS
    /usr/local/include
    /usr/include
)

list(APPEND CHECKARG_NAMES libcheckarg++ checkarg++)
list(APPEND CHECKARG_NAMES_DEBUG libcheckarg++d checkarg++d)



find_library(CHECKARG_LIBRARY_RELEASE NAMES ${CHECKARG_NAMES})
find_library(CHECKARG_LIBRARY_DEBUG NAMES ${CHECKARG_NAMES_DEBUG})

include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
select_library_configurations(CHECKARG)
mark_as_advanced(CHECKARG_LIBRARY_RELEASE CHECKARG_LIBRARY_DEBUG)

unset(CHECKARG_NAMES)
unset(CHECKARG_NAMES_DEBUG)

# Set by select_library_configurations(), but we want the one from
# find_package_handle_standard_args() below.
unset(CHECKARG_FOUND)

if (CHECKAREG_LIBRARY AND CHECKARG_CA_INCLUDE_DIR)
  set(CHECKARG_INCLUDE_DIRS ${CHECKARG_CA_INCLUDE_DIR} )
  set(CHECKARG_LIBRARIES ${CHECKARG_LIBRARY})
endif()


if(CHECKARG_CA_INCLUDE_DIR AND EXISTS "${CHECKARG_CA_INCLUDE_DIR}/checkarg++.hpp")

  file(STRINGS "${CHECKARG_CA_INCLUDE_DIR}/checkarg++.hpp"
    checkarg_version_str REGEX "^#define[ \t]+CA_VER_STRING[ \t]+\".+\"")

  string(REGEX REPLACE "^#define[ \t]+CA_VER_STRING[ \t]+\"([^\"]+)\".*" "\\1"
    CHECKARG_VERSION_STRING "${checkarg_version_str}")

  unset(checkarg_version_str)

endif()

# handle the QUIETLY and REQUIRED arguments and set CHECKARG_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(CHECKARG
    REQUIRED_VARS
      CHECKARG_LIBRARY
      CHECKARG_CA_INCLUDE_DIR
    VERSION_VAR
      CHECKARG_VERSION_STRING
)

mark_as_advanced(CHECKARG_CA_INCLUDE_DIR CHECKARG_LIBRARY )
