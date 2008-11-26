#!../bin/tclutil_wish
#
# mkIndex.tcl - generate a tclIndex file in the current directory
# "@(#) $Id: mkIndex.tcl,v 1.1 2004-08-13 20:18:54 brighton Exp $"

package require Itcl
auto_mkindex . *.tcl
exit 0
