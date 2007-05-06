# $Id$

SUBDIRS = libstx-exparser testsuite . wxparserdemo perl-binding

noinst_PROGRAMS = test

test_SOURCES = test.cc

test_LDADD = $(srcdir)/libstx-exparser/libstx-exparser.a

AM_CFLAGS = -W -Wall -I$(srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(srcdir)/libstx-exparser
