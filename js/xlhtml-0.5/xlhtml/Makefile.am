## Process this file with automake to produce Makefile.in

EXTRA_DIST = README ChangeLog TODO THANKS Test.xls Test.html xlhtml.1 

INCLUDES = -I../cole
noinst_PROGRAMS = xldump xlcdump
noinst_SCRIPTS = xls-handler gpdemo
man_MANS = xlhtml.1
bin_SCRIPTS = nsopen nsxlview
bin_PROGRAMS = xlhtml
LDADD = ../cole/libcole.a
xlhtml_SOURCES = support.c xlhtml.c html.c ascii.c xml.c 
xldump_SOURCES = xldump.c 
xlcdump_SOURCES = xlcdump.c 
AM_CFLAGS =   -Wall  -Wshadow -Wcast-align -Wpointer-arith 
support.o: version.h

doc:
	@echo Generating documentation...
	doxygen doxygen.conf
	@echo done.

distclean:
	rm -rf doc
