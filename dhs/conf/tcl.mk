# Include makefile for Tcl modules
# 
# Before including, define
#
# NAME = name of application
# SOURCES = list of Tcl sources

include ../conf/include.mk

all:

install: index $(INSTALL_TARGET)
	d=${RELEASE_DIR}/lib/${NAME}; \
	test -d $$d || mkdir $$d ;\
	test -d $$d/html || mkdir $$d/html ;\
	test -d $$d/images || mkdir $$d/images ;\
	rm -f $$d/*.tcl $$d/html/*.html $$d/images/* ;\
	cp *.tcl tclIndex $$d ;\
	test -d html && cp html/*.html $$d/html ;\
	test -d images && cp images/* $$d/images ;\
	echo done

index tclIndex: FORCE
	tclsh$(TCL_VERSION) ./mkIndex.tcl

clean: FORCE
	rm -f tclIndex

FORCE:
