## Process this file with automake to produce Makefile.in

EXTRA_DIST = present.sdd sprsheet.sdc sprsheet.xls text.doc text.sdw \
	     README-examples

INCLUDES = -I$(top_srcdir)
LDADD = $(top_builddir)/libcole.la
TESTS = filesystem directory file version

noinst_PROGRAMS = filesystem directory file demo version

