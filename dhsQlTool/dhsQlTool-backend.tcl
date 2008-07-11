#!/bin/sh
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1997				(c) 1997
# National Research Council		Conseil national de recherches
# Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
# All rights reserved			Tous droits reserves
# 					
# NRC disclaims any warranties,		Le CNRC denie toute garantie
# expressed, implied, or statu-		enoncee, implicite ou legale,
# tory, of any kind with respect	de quelque nature que se soit,
# to the software, including		concernant le logiciel, y com-
# without limitation any war-		pris sans restriction toute
# ranty of merchantability or		garantie de valeur marchande
# fitness for a particular pur-		ou de pertinence pour un usage
# pose.  NRC shall not be liable	particulier.  Le CNRC ne
# in any event for any damages,		pourra en aucun cas etre tenu
# whether direct or indirect,		responsable de tout dommage,
# special or general, consequen-	direct ou indirect, particul-
# tial or incidental, arising		ier ou general, accessoire ou
# from the use of the software.		fortuit, resultant de l'utili-
# 					sation du logiciel.
#
#***********************************************************************
#
# FILENAME
# dhsQlTool/scripts/dhsQlTool
#
# PURPOSE:
# This is the script to start the DHS Quick Look Tool.
#
#INDENT-OFF*
# $Log: not supported by cvs2svn $
# Revision 1.2  2004/08/30 09:15:32  brighton
# Started testing DS9 support
#
# Revision 1.1  2004/08/24 13:57:13  brighton
# Linux port (see PORTING_NOTES, especially the part at the end labeled "Other Changes")
#
# Revision 1.2  2004/08/13 20:18:54  brighton
# Linux test/port
#
# Revision 1.1.1.1  2002/11/24 20:29:56  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
#INDENT-ON*
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

set dhs_base $::env(DHS_BASE)
set dhs_library $dhs_base/lib/dhsQlTool
set _qltHelpDir $dhs_library/html
lappend auto_path $dhs_base/lib $dhs_library
set debug 0

# XXX allan: was in TCLLIBPATH, for gdb debugging XXX
#lappend auto_path $env(HLPG_LIB_DIR) $env(SKYCAT_INSTALL)/lib/skycat

#
# Set the application name and a name unique to this application. Note
# that this name must be unique, even between hosts, since it is used
# as the IMP application name.
#

set appName	dhsQlt
set uniqueName	[join "dhsQlc [ exec hostname ] [ pid ]" "" ]

package require Tclx
package require Itcl
package require Itk
package require Iwidgets
## fkraemer: comment the following line out because of error message when 
## importing Skycat (further down) that namespace blt already exists
# package require BLT
##
package require tclxpa

namespace import -force itcl::*
namespace import -force itk::*
namespace import -force iwidgets::*

#
# the environment variable $SKYCAT_LIBRARY should point to the directory
# containing the Tcl library files
#

if {[info exists env(SKYCAT_LIBRARY)]} {
    set skycat_library $env(SKYCAT_LIBRARY)
} else {
    puts "Can't locate Skycat package. Please define the SKYCAT_LIBRARY "
    puts "environment variable to point to the directory containing SkyCat.tcl."
    dhsExit 1
}

if {[catch {package require Skycat} msg]} {
	puts "error loading Skycat package $msg"
	dhsExit 1
}


#
# Make sure the OCS package is loaded.  Note, that the Ocspkg renames,
# creates its own exit command, which doesn't behave like the Tcl/Tk
# exit command.  So there are some fixed here for that.
#

if { [ info commands exit ] != "" } {
    rename exit dhsExit
}

if {[catch {package require Ocspkg} msg]} {
	puts "error loading Ocspkg package $msg"
	dhsExit 1
}

if { [ info commands dhsExit ] != "" } {
    rename exit ocsExit
    proc exit {{value {}} } {
	ocsExit
	dhsExit "$value"
    }
}

#
# print errors also on stderr
#

utilPrintErrors


#
#  Source the skycat code.
#

source $env(SKYCAT_LIBRARY)/SkyCat.tcl

#
# Get the data processing package.
#

if {[catch {package require qldp} msg]} {
	puts "error loading qldp package $msg"
	exit 1
}


#
# Source the DHS Quick Look Tool code.
#

source $dhs_library/dhsQlTool.tcl


#
#  Set up the temporary directory.
#

if {[info exists env(QLT_TMP_DIR)]} {
    set tmpDir $env(QLT_TMP_DIR)
} else {
    set tmpDir /tmp
}


#
# Start the dhsQlTool widget.
#

util::TopLevelWidget::start CDhsQlTool 
