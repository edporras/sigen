#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
m4_define([sigen_version],[2.6.4])

AC_PREREQ([2.69])
AC_INIT([sigen], [sigen_version], [ep@digressed.net])
AM_INIT_AUTOMAKE
LT_INIT

AC_CONFIG_SRCDIR([src/bat.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_CONFIG_MACRO_DIR([m4])

AC_DEFINE_UNQUOTED([SIGEN_VERSION], ["sigen_version"], [Explicitly named version])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

AX_CXX_COMPILE_STDCXX_11


# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([tests/Makefile])
AC_OUTPUT
