## Process this file with automake to produce Makefile.in

EXTRA_DIST = README ChangeLog TODO THANKS test.ppt ppthtml.1

INCLUDES = -I../cole

noinst_PROGRAMS = pptdump 
man_MANS = ppthtml.1
bin_SCRIPTS = nspptview
bin_PROGRAMS = ppthtml
LDADD = ../cole/libcole.a
ppthtml_SOURCES = ppthtml.c 
AM_CFLAGS = -Wall -Wshadow -Wcast-align -Wpointer-arith 
