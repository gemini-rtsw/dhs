# Makefile

subdirs = dhs

all install clean distclean: FORCE
	@set -x; for i in $(subdirs); do (cd $$i && $(MAKE) $@ || exit 1) done

FORCE:
