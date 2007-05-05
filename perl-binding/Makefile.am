# $Id$

if HAVE_SWIG

EXTRA_DIST = stx-eparser.i
BUILT_SOURCES = stx-eparser_wrap.cpp

stx-eparser_wrap.cpp: stx-eparser.i
	$(SWIG) -Wall -Wallkw -perl5 -I$(top_srcdir)/libstx-eparser -o $@ $<

CLEANFILES = stx-eparser_wrap.cpp ExpressionParser.pm STX/ExpressionParser.pm ExpressionParser.so

lib_LTLIBRARIES = ExpressionParser.la

nodist_ExpressionParser_la_SOURCES = stx-eparser_wrap.cpp

ExpressionParser_la_LIBADD = ../libstx-eparser/libstx-eparser.a

ExpressionParser_la_CXXFLAGS = -I$(top_srcdir)/libstx-eparser `perl -MExtUtils::Embed -e ccopts`

all-local:
	rm -rvf STX/
	mkdir STX/
	cp -L ExpressionParser.pm STX/
	ln -sf .libs/ExpressionParser.so

clean-local:
	rmdir STX || true

endif