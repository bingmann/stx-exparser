# $Id$

if HAVE_WXWIDGETS

bin_PROGRAMS = wxParserDemo

wxParserDemo_SOURCES = WMain.cpp WMain.h WMain_wxg.cpp

AM_CFLAGS = -I/usr/include
AM_CXXFLAGS = -I/usr/include @WX_CXXFLAGS@ -I$(top_srcdir)/libstx-eparser
LDADD = @WX_LIBS@ $(top_srcdir)/libstx-eparser/libstx-eparser.a

endif

#if GOT_RESCOMP
#resources.o: resources.rc
#	$(RESCOMP) -I$(srcdir) $(srcdir)/resources.rc resources.o

#LDADD += resources.o
#endif
