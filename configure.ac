AC_INIT([macchanger], [1.7.0])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_CONFIG_HEADER(config.h)
AC_SUBST(VERSION)

AC_PROG_INSTALL
AC_PROG_CC
AC_CANONICAL_HOST

AS_CASE([$host],
	[*-*-linux*],
		[AC_CHECK_HEADER([linux/ethtool.h], ,
			AC_DEFINE([HAVE_LINUX_ETHTOOL], [1],
				  [Linux ethernet headers found]))],
	[*-*-freebsd*],
		[AC_CHECK_HEADERS([ifaddrs.h net/if.h net/if_dl.h net/if_types.h], ,
			AC_DEFINE([HAVE_BSD_ETHTOOL], [1],
				  [BSD ethernet headers found]))],
	[AC_MSG_ERROR([architecture not supported])]
)

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
doc/Makefile])
