AC_INIT([macchanger], [1.7.0])
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AM_CONFIG_HEADER(config.h)
AC_SUBST(VERSION)

AC_PROG_INSTALL
AC_PROG_CC
AC_CANONICAL_HOST

case "$host" in
*-*-linux*)
	AC_DEFINE([LLADDR(s)], [s->sll_addr], [the link level address])
;;
*-*-freebsd*)
	AC_DEFINE([sockaddr_ll], [sockaddr_dl], [the socket link structures])
	AC_DEFINE([sll_family], [sdl_family], [the socket family member])
	AC_DEFINE([AF_PACKET], [AF_LINK], [the domain name])
	AC_DEFINE([ifr_hwaddr], [ifr_addr], [the address member])
	AC_DEFINE([SIOCSIFHWADDR], [SIOCSIFLLADDR], [the ioctl call])
;;
esac

AC_MSG_CHECKING([if sockaddr has the sa_len member])
AC_LINK_IFELSE(
	[AC_LANG_PROGRAM(
		[[ #include <sys/socket.h> ]],
		[[ struct sockaddr sock; return (sock.sa_len); ]])],
	[AC_DEFINE([HAVE_SOCKADDR_SA_LEN], [1],
		   [socket address structures have length fields])
])

AC_CHECK_HEADER([ifaddrs.h], ,[AC_MSG_ERROR(
	[*** ifaddrs.h missing - please install first or check config.log ***])])

AC_CHECK_HEADERS([linux/if_packet.h net/if_dl.h])

if test "x$ac_cv_header_linux_if_packet_h" = "xyes" ; then
	AC_DEFINE([HAVE_IF_PACKET], [1],
		    [The Linux if_packet.h header found])
elif test "x$ac_cv_header_net_if_dl_h" = "xyes" ; then
	AC_DEFINE([HAVE_IF_DL], [1],
		    [The BSD if_dl.h header found])
else
	AC_MSG_ERROR([*** sockaddr_* missing - please install first or check config.log ***])
fi

AC_CHECK_HEADER(linux/ethtool.h, AC_DEFINE([HAVE_ETHTOOL], [1], [Linux ethernet headers found]), )

AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
doc/Makefile])
