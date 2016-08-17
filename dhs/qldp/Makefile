# Makefile

NAME = qldp
SOURCES = image.C qldp.C
#SHARED_LIBS = -ldhsGen  $(SKYCAT_LDLIBS)
SHARED_LIBS = $(LOCAL_LIBS) $(SKYCAT_LDLIBS)
INSTALL_TARGET = local_install

include ../conf/cxx_lib.mk

local_install:
	cp pkgIndex.tcl ../release/lib
