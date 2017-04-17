dnl configure.ac for tenmado
dnl Process this file with autoconf to produce a configure script.
dnl autoconf 2.68 or later is required
# $Id: configure.ac,v 1.54 2012/01/31 04:18:35 oohara Exp $
AC_INIT([tenmado], [0.10], [oohara@libra.interq.or.jp])
AC_PREREQ([2.68])
AC_REVISION([$Revision: 1.54 $])
AM_INIT_AUTOMAKE([foreign tar-ustar])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
# Makefile.in will use awk
AC_PROG_AWK
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AM_PROG_LEX
AC_PROG_YACC

# Checks for libraries.
AC_CHECK_LIB([m], [sin])

# the libdb3 support is obsolete
# don't define HAVE_LIBDB3 or HAVE_LIBDB_3

AC_ARG_WITH([posix],
  AC_HELP_STRING([--without-posix],
                 [don't use (most of) POSIX features (DANGEROUS)]),
  ,
  [with_posix=yes])

if test "x$with_posix" = "xno"
then
  AC_DEFINE([NOT_HAVE_POSIX], [1],
            [Define to 1 if your system does not have POSIX features.])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([stddef.h])
AC_CHECK_HEADERS([limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_STRUCT_TM
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit])
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([strerror])

AC_SUBST([LIBS], ["-lSDL_image `sdl-config --libs` $LIBS"])

AC_CONFIG_FILES([Makefile
happy-L/Makefile
happy-L/default/Makefile
happy-L/image/Makefile
libbac/Makefile
libtenm/Makefile
save/Makefile])
AC_OUTPUT
