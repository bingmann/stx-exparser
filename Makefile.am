# $Id$

SUBDIRS = libstx-exparser testsuite . wxparserdemo perl-binding

noinst_PROGRAMS = test

test_SOURCES = test.cc

test_LDADD = $(srcdir)/libstx-exparser/libstx-exparser.a

AM_CFLAGS = -W -Wall -I$(srcdir)/libstx-exparser
AM_CXXFLAGS = -W -Wall -Wold-style-cast -I$(srcdir)/libstx-exparser

if GCOV

clean-local:
	find -name "*.da" -o -name "*.gcov" -o -name "*.gcda" -o -name "*.gcno" | xargs rm || true

run-gcov: clean-gcov
	mkdir -p coverage/
	lcov --directory . --zerocounters --path `pwd`
	-$(MAKE) check
	lcov --directory . --capture --output-file coverage/all.info
	lcov -e coverage/all.info "/tdata/*" -o coverage/testsuite.info
	genhtml -o coverage --num-spaces 8 coverage/testsuite.info --title "STX ExpressionParser Testsuite" --prefix `pwd`

clean-gcov:
	find -name "*.gcda" | xargs rm || true

endif
