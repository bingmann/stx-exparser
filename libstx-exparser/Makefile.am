# $Id$

lib_LTLIBRARIES = libstx-exparser.la

pkginclude_HEADERS = AnyScalar.h ExpressionParser.h

libstx_exparser_la_SOURCES = $(pkginclude_HEADERS) \
	AnyScalar.cc ExpressionParser.cc 

libstx_exparser_la_LDFLAGS= -version-info 0:7:0

AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall

EXTRA_DIST = ExpressionParser.dox
