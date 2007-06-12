# $Id$

if HAVE_WXWIDGETS

bin_PROGRAMS = wxParserDemo

wxParserDemo_SOURCES = WMain.cpp WMain.h WMain_wxg.cpp WMain_wxg.h

AM_CFLAGS = -I/usr/include
AM_CXXFLAGS = -I/usr/include @WX_CXXFLAGS@ -I$(top_srcdir)/libstx-exparser
LDADD = @WX_LIBS@ $(top_srcdir)/libstx-exparser/libstx-exparser.a

EXTRA_DIST = wxParserDemo.wxg resources.rc \
	img/addrow.png img/addrow.xpm \
	img/delrow.png img/delrow.xpm \
	img/progicon.ico \
	img/progicon16.png img/progicon16.xpm img/progicon16.ico \
	img/progicon32.svg img/progicon32.png img/progicon32.xpm img/progicon32.ico \
	img/progicon48.svg img/progicon48.png img/progicon48.xpm img/progicon48.ico

if GOT_RESCOMP
resources.o: resources.rc
	$(RESCOMP) -I$(srcdir) $(srcdir)/resources.rc resources.o

LDADD += resources.o
endif

endif
