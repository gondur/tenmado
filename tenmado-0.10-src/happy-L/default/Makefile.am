# Makefile.am for tenmado/happy-L/default
# $Id: Makefile.am,v 1.3 2002/12/18 16:55:02 oohara Exp $
# process this file with automake --foreign to generate Makefile.in

EXTRA_DIST = score-stage-1.txt \
  score-stage-2.txt \
  score-stage-3.txt \
  score-stage-4.txt \
  score-stage-5.txt \
  score-total.txt

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(datadir)/games/tenmado/default/
	$(INSTALL_DATA) *.txt \
  $(DESTDIR)$(datadir)/games/tenmado/default/

uninstall-local:
	-rm -f $(DESTDIR)$(datadir)/games/tenmado/default/*.txt
	-rmdir $(DESTDIR)$(datadir)/games/tenmado/default/
	-rmdir $(DESTDIR)$(datadir)/games/tenmado/
