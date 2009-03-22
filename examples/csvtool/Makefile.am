# $Id$

noinst_PROGRAMS = csvtool

csvtool_SOURCES = csvtool.cc strnatcmp.h strnatcmp.c

csvtool_LDADD = $(top_srcdir)/libstx-exparser/libstx-exparser.la

AM_CFLAGS = -W -Wall -I$(top_srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(top_srcdir)/libstx-exparser
