# $Id$

noinst_PROGRAMS = csvfilter

csvfilter_SOURCES = csvfilter.cc

csvfilter_LDADD = $(top_srcdir)/libstx-exparser/libstx-exparser.la

AM_CFLAGS = -W -Wall -I$(top_srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(top_srcdir)/libstx-exparser

EXTRA_DIST = mysql-world-city.csv mysql-world-country.csv cia-world-factbook.csv
