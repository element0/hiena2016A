#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hiena_fudge], [0.01], [errno@dreamboysoft.com])
AM_PROG_AR
LT_INIT([dlopen])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([fudge_bison.y])
AC_CONFIG_HEADERS([config.h])

AC_SUBST([project_name], [hiena])

# Checks for programs.
AC_PROG_YACC
AC_PROG_CC
AM_PROG_LEX     # replaces AC_PROG_LEX
AC_PROG_INSTALL
AM_PROG_LIBTOOL

# Checks for libraries.
#AC_CHECK_LIB([dl], [dlopen])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([inttypes.h libintl.h malloc.h stddef.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
