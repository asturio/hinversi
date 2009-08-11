#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([hinversi],[0.8.2],[asturio@gmx.net])
AC_CONFIG_SRCDIR([lib/othello.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([-Wall -Werror])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
# AC_PROG_MSGFMT
# AC_PROG_RANLIB
LT_INIT
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4], [HAVE_CHECK=yes], 
                    [
                        AC_MSG_WARN(check not found. Checks disabled.)
                    ])
AM_CONDITIONAL([HAVE_CHECK], [test "x$HAVE_CHECK" = "xyes"])
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h libintl.h limits.h locale.h stdlib.h string.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.15])
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gettimeofday memset setlocale])

AC_CONFIG_FILES([Makefile
                 check/Makefile
                 human-cli/Makefile
                 cgi/Makefile
                 lib/Makefile
                 po/Makefile.in
                 lib/libhinversi.pc
                 doc/Doxyfile
                 doc/Makefile])
AC_OUTPUT
