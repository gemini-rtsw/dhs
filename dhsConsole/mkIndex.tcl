#!../bin/tclutil_wish
#
# mkIndex.tcl - generate a tclIndex file in the current directory
# "@(#) $Id: mkIndex.tcl,v 1.1 2004-08-30 09:15:32 brighton Exp $"

package require Itcl
auto_mkindex . *.tcl
exit 0
