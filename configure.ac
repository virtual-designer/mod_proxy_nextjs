AC_INIT([mod_proxy_nextjs], [1.0.0], [rakinar2@onesoftnet.eu.org])
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC
AM_PROG_AR

AC_ARG_WITH([apache],
  [AS_HELP_STRING([--with-apache=DIR], [Apache install path])],
  [APACHE_DIR="$withval"],
  [APACHE_DIR="/usr"])

APXS="$APACHE_DIR/bin/apxs"
AC_SUBST([APXS])

AC_MSG_CHECKING([for apxs])

if test -x "$APXS"; then
  AC_MSG_RESULT([found])
else
  AC_MSG_ERROR([apxs not found in $APACHE_DIR/bin])
fi

APACHE_CFLAGS=`$APXS -q CFLAGS`
APACHE_INCLUDES=`$APXS -q INCLUDEDIR`
APACHE_LDFLAGS=`$APXS -q LDFLAGS`

AC_SUBST([APACHE_CFLAGS])
AC_SUBST([APACHE_INCLUDES])
AC_SUBST([APACHE_LDFLAGS])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
