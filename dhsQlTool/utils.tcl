# $Id: utils.tcl,v 1.1.1.1 2002-11-24 20:29:54 brighton Exp $
#
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
# dhsConsole/src/utils.tcl
#
# PURPOSE:
# To provide a number of helper routines for the DHS console.
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# None.
#
# GLOBAL PROCEDURE(S)
# isBoolean		Determines if some value is a boolean value.
# isFalse		Determines if some value is a "negative" value.
# isNumber		Determines if some value is a numerical value.
# isTrue		Determines if some value is a "true" value.
# isZero		Determines if some value is zero.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.1  1998/10/08 19:51:24  nhill
# Initial revision
#
# Revision 1.18  1998/08/14 21:54:19  jaeger
# Brought code documentation up to Gemini standards.
#
# Revision 1.17  1998/07/20 19:39:19  jaeger
# moved bitmaps to "images" directory, code changed to reflect this.
#
# Revision 1.16  1998/07/16 20:05:11  jaeger
# Removed help button on choiceDialog.  Added short help information.
#
# Revision 1.15  1998/06/25 20:12:45  jaeger
# Added a couple update idletasks commands so things return when they should.
#
# Revision 1.14  1998/06/19 22:35:04  jaeger
# Changed instances of exit to return a value.
#
# Revision 1.13  1998/06/15 04:15:31  jaeger
# Added isBolean function and fixed errors when reading command line args.
#
# Revision 1.12  1998/05/25 20:10:05  jaeger
# Removed the Help button from the inputDialog, to make it consistent
# with the other dialogs.
#
# Revision 1.11  1998/05/22 17:56:10  jaeger
# Fixed the usage message.
#
# Revision 1.10  1998/05/22 16:47:04  jaeger
# Fixed bugs in "bgerror".  Problems occured when error messages had
# double quotes.  Switch to my choiceDialog instead of the hacked up
# ESO one.  Printed error messages on various debug levels only now.
#
# Revision 1.9  1998/05/20 21:08:12  jaeger
# Fixed bgerror, wasn't calling the "error monitor" properly.
#
# Revision 1.8  1998/05/15 16:49:16  jaeger
# Fixed some of the spacing.  Changed the logic of the bgerror procedure.
#
# Revision 1.7  1998/05/06 21:40:07  jaeger
# Fixed bug with choice dialog, extra characters after "
#
# Revision 1.6  1998/05/06 20:43:42  jaeger
# Added bgerror routine to catch background error.  Change infoDialog,
# errorDialog, warningDialog to use the incr widget message dialog
# widget instead of the eso Dialog widget.  This was done to avoid
# since the Dialog widget seemed to freeze when command responses
# were reported.
#
# Revision 1.5  1998/05/04 20:37:22  jaeger
# Merged utilities from Storage Server and DHS consoles.
#
# Revision 1.4  1998/04/15 22:40:53  jaeger
# Added simMode, extractNums, extractListItem procedures.  Altered
# resource display to use a different instance of the status class, this
# was needed due to the changes in the status class.
#
# Revision 1.3  1998/03/06 17:50:34  jaeger
# Added resWindow and createAbout procedures
#
# Revision 1.2  1997/11/13 23:53:19  jaeger
# Checked in for install
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isBoolean
#
# INVOCATION:
# isBoolean value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a boolean value.
# (boolean) 1: If the value is a boolean value.
#
# PURPOSE:
# To determine is some string represents a boolean value.
# A boolean value is one of:
#   f, t, false, true, 0, 1, n, y, no, yes, off, or on
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	isBoolean {
    value
} {
    set value [ string trim $value ]
    if { "$value" == "" } {
	return 0
    } elseif { [ isFalse "$value" ] } {
	return 1
    } elseif { [ isTrue "$value" ] } {
	return 1
    } else {
	return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isFalse
#
# INVOCATION:
# isFalse value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is "false".
# (boolean) 1: If the value is "false".
#
# PURPOSE:
# To determine is some string is false.
# A false value is one of:
#   f, false, 0, n, no, or off
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    isFalse { 
    bool
} {
    if { "$bool" == "" } {
	return 0
    } 
    set bool [ string trim "$bool" ]
    if { [ regexp -nocase -- (^f$)|(^false$)|(^0$)|(^n$)|(^no$)|(^off$) "$bool" ] 
} {
        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isNumber
#
# INVOCATION:
# isNumber value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a number.
# (boolean) 1: If the value is a number.
#
# PURPOSE:
# To determine is some string represents a numerical value.
# A numberical value is either an integer or a real number.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    isNumber {
    value
} {
    if { "$value" == "" } {
	return 0
    } 

    set value [ string trim "$value" ]
    if { "$value" == "" } {
	return 0
    } elseif { [ regexp -- {^[ -+ ]?[ 0-9 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0-9 ]?.[ 0-9 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0-9 ]+.[ 0-9 ]+$} "$value" ] } {
        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isTrue
#
# INVOCATION:
# isTrue value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a "true" value.
# (boolean) 1: If the value is a "true" value.
#
# PURPOSE:
# To determine is some string represents a boolean value.
# A "true" value is one of:
#   t, true, 1, y, yes, or on
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    isTrue { 
    bool
} {
    if { "$bool" == "" } {
	return 0
    } 
    set bool [ string trim "$bool" ]
    if { [ regexp -nocase -- (^t$)|(^true$)|(^1$)|(^y$)|(^yes$)|(^on$) "$bool" ] } {
        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isZero
#
# INVOCATION:
# isZero value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a zero value.
# (boolean) 1: If the value is a zero value.
#
# PURPOSE:
# To determine is some string represents a zero value.
# A zero value is one of:
#   0. 0.0000 or 0E??
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    isZero {
    value
} {
    if { "$value" == "" } {
	return 0
    } 
    set value [ string trim "$value" ]
    if { [ regexp -- {^[ -+ ]?[ 0 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0 ]?.[ 0 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0 ]+.[ 0 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0 ]+E[ 0-9 ]+$} "$value" ] } {
        return 1
    } else {
        return 0
    }
}               
