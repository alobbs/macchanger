AC_INIT([macchanger], [1.7.0])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_CONFIG_HEADER(config.h)
AC_SUBST(VERSION)

AC_PROG_INSTALL
AC_PROG_CC

AC_CHECK_LIB(chula, chula_buffer_new)
AC_CHECK_HEADER(libchula/libchula.h, ,
			 [AC_MSG_ERROR([libchula is required: https://github.com/alobbs/libchula])])

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
doc/Makefile])
