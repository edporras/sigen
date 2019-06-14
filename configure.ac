# Copyright 1999-2019 Ed Porras
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
# BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_define([sigen_version],[2.7.0])

AC_PREREQ([2.69])
AC_INIT([sigen], [sigen_version], [ep@digressed.net])
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_SRCDIR([src/bat.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE_UNQUOTED([SIGEN_VERSION], ["sigen_version"], [Explicitly named version])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AX_CXX_COMPILE_STDCXX_14

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# ENABLE_DUMP on / off. On by default but can be disabled with --disable-text-dump option
AC_ARG_ENABLE([text-dump], AS_HELP_STRING([--disable-text-dump], [Disable text dump output]))
AS_IF([test "x$enable_text_dump" != "xno"], [
   AC_DEFINE([ENABLE_DUMP], [1], [enable text dump output])
])

# for the makefiles
AM_CONDITIONAL([ENABLE_DUMP_SRC], [test "x$enable_text_dump" != "xno"])

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_OUTPUT
