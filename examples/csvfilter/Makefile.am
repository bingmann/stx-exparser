# $Id$

noinst_PROGRAMS = csvfilter

csvfilter_SOURCES = csvfilter.cc

csvfilter_LDADD = $(top_srcdir)/libstx-exparser/libstx-exparser.a

AM_CFLAGS = -W -Wall -I$(top_srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(top_srcdir)/libstx-exparser

EXTRA_DIST = City.csv Country.csv
