#                                               -*- Autoconf -*-
# $Id$
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(stx-eparser, 0.7)
AC_CONFIG_SRCDIR([test.cc])
AC_CONFIG_AUX_DIR(scripts)

AM_INIT_AUTOMAKE(foreign)

if test "$CFLAGS" == ""; then
    CFLAGS="-g"
fi
if test "$CXXFLAGS" == ""; then
    CXXFLAGS="-g"
fi

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

AC_LANG([C++])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_FUNC_STRTOD
AC_CHECK_FUNCS([pow sqrt strcasecmp strtol strtoul strtoull])

# Boost Base autoconf check
AX_BOOST_BASE([1.33.0])

# check for Boost.Spirit
AC_CHECK_HEADER([boost/spirit/core.hpp], [],
    AC_MSG_ERROR([
*** could not find the boost headers ***
*** install the libboost-dev package (http://www.boost.org) ***]))

AC_CHECK_HEADER([boost/spirit/phoenix.hpp], [],
    AC_MSG_ERROR([
*** could not find the boost headers ***
*** install the libboost-dev package (http://www.boost.org) ***]))

# check whether cppunit is available
AM_PATH_CPPUNIT(1.10.0)

AM_CONDITIONAL(HAVE_CPPUNIT, test x$CPPUNIT_CONFIG != xno)

if test x$CPPUNIT_CONFIG == xno; then
    AC_MSG_WARN([
	cppunit does not seem to be installed on your system.
	The testsuite will not be built!
    ])
fi

dnl Check for wxWidgets 2.6.0 or later

AM_OPTIONS_WXCONFIG

AM_PATH_WXCONFIG(2.6.0, wxWin=1)

AM_CONDITIONAL(HAVE_WXWIDGETS, test "$wxWin" != 0)

if test "$wxWin" != 1; then
    AC_MSG_WARN([
	wxWidgets does not seem to be installed on your system.
	The wxParserDemo will not be built!

        If you think wxWidgets >= 2.6.0 is installed,
	please check that wx-config is in path, the directory
	where wxWidgets libraries are installed (returned by
	'wx-config --libs' command) is in LD_LIBRARY_PATH or
	equivalent variable.
    ])
fi

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 libstx-eparser/Makefile
	         testsuite/Makefile
		 wxparserdemo/Makefile])
AC_OUTPUT
