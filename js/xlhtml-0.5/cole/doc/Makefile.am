## Process this file with automake to produce Makefile.in


EXTRA_DIST =	cole-manual.sgml \
		introduction.sgml \
		compile.sgml \
		using.sgml \
		bugs.sgml \
		gpl.sgml \
		legal.sgml \
		HACKING \
		cole-manual.sgml-old

dist-hook: all
	cp $(srcdir)/cole-sections.txt $(distdir)
	cp -r $(srcdir)/tmpl $(distdir)
	cp -r $(srcdir)/cole-manual $(distdir)



# Headers to scan for declarations
scanned_headers = ../internal.h ../support.h ./cole.h


# The name of the module.
DOC_MODULE = cole

# The top-level SGML file.
DOC_MAIN_SGML_FILE = cole-manual.sgml

# The directory containing the source code (if it contains documentation).
DOC_SOURCE_DIR = $(top_srcdir)

# The directory where the documentation will be installed
TARGET_DIR = $(HTML_DIR)/$(DOC_MODULE)



scan:
	cp $(top_builddir)/cole.h $(srcdir); \
	(cd $(srcdir); \
	gtkdoc-scan --module=$(DOC_MODULE) $(scanned_headers); \
	rm cole.h)
templates: scan
	(cd $(srcdir); \
	gtkdoc-mktmpl --module=$(DOC_MODULE))
sgml:
	(cd $(srcdir); \
	gtkdoc-mkdb --module=$(DOC_MODULE) --source-dir=$(DOC_SOURCE_DIR))
html:
	(cd $(srcdir); \
	cat ../BUGS | sed "s/>/&gt;/g" | sed "s/</&lt;/g" > BUGS; \
	db2html $(DOC_MAIN_SGML_FILE); \
	rm BUGS)
rtf:
	(cd $(srcdir); \
	cat ../BUGS | sed "s/>/&gt;/g" | sed "s/</&lt;/g" > BUGS; \
	db2rtf $(DOC_MAIN_SGML_FILE); \
	-rm BUGS)
ps:
	(cd $(srcdir); \
	cat ../BUGS | sed "s/>/&gt;/g" | sed "s/</&lt;/g" | sed "s/&/&amp;/g" > BUGS; \
	db2ps $(DOC_MAIN_SGML_FILE); \
	-rm BUGS)

regenerate-all: templates sgml html



clean-local:
	(cd $(srcdir); \
	rm -f *~ *.bak *.hierarchy *.signals *.args; \
	rm -fr DBTOHTML_OUTPUT_DIR* *.junk $(DOC_MODULE)-decl-list.txt)

maintainer-clean-local: clean
	(cd $(srcdir); \
	rm -rf sgml html cole-manual tmpl/*.bak; \
	rm -rf $(DOC_MODULE)-decl.txt)



# if db2html generate subdirectories, it's broken the next rule
# install-data-local: all
# 	$(INSTALL) -d $(TARGET_DIR)
# 	$(INSTALL) ./cole-manual/* $(TARGET_DIR)



## Solaris make doesn't like directory as a target...

.PHONY : ps rtf html sgml templates scan

