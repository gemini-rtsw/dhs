# Include makefile for Tcl modules
# 
# Before including, define
#
# NAME = name of application
# SOURCES = list of Tcl sources


# to be done...
all install clean:


index tclIndex: FORCE
	tclsh$(TCL_VERSION) ./mkIndex.tcl

