# $Id$

SUBDIRS = libstx-eparser testsuite . wxparserdemo

noinst_PROGRAMS = test

test_SOURCES = test.cc

test_LDADD = $(srcdir)/libstx-eparser/libstx-eparser.a

AM_CFLAGS = -W -Wall -I$(srcdir)/libstx-eparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(srcdir)/libstx-eparser
