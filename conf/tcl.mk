# Include makefile for Tcl modules
# 
# Before including, define
#
# NAME = name of application
# SOURCES = list of Tcl sources

include ../conf/include.mk

all clean:

install: index $(INSTALL_TARGET)
	d=${RELEASE_DIR}/lib/${NAME}; \
	test -d $$d || mkdir $$d ;\
	test -d $$d/html || mkdir $$d/html ;\
	rm -f $$d/*.tcl $$d/html/*.html;\
	cp *.tcl tclIndex $$d ;\
	cp html/*.html $$d/html

index tclIndex: FORCE
	tclsh$(TCL_VERSION) ./mkIndex.tcl


FORCE:
