#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hienafs], [0.01], [errno@raygan.com])
#AM_PROG_AR
LT_INIT([dlopen])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/hienafs.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES([FUSE], [fuse >= 2.3])
LIBS="$LIBS $FUSE_LIBS"

PKG_CHECK_MODULES([KYOTOCABINET], [kyotocabinet])
LIBS="$LIBS $KYOTOCABINET_LIBS"

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h unistd.h])
AC_CHECK_HEADERS([ltdl.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gethostname memset strdup strncasecmp strndup])
AC_SEARCH_LIBS([lt_dlopen], [ltdl])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
