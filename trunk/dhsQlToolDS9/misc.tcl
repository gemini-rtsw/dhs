# $Id: misc.tcl,v 1.2 2004-10-12 08:55:12 brighton Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1997				(c) 1997
# National Research Council		Conseil national de recherches
# Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
# All rights reserved			Tous droits reserves
# 					
# NRC disclaims any warranties,	Le CNRC denie toute garantie
# expressed, implied, or statu-	enoncee, implicite ou legale,
# tory, of any kind with respect	de quelque nature que se soit,
# to the software, including		concernant le logiciel, y com-
# without limitation any war-		pris sans restriction toute
# ranty of merchantability or		garantie de valeur marchande
# fitness for a particular pur-	ou de pertinence pour un usage
# pose.  NRC shall not be liable	particulier.  Le CNRC ne
# in any event for any damages,	pourra en aucun cas etre tenu
# whether direct or indirect,		responsable de tout dommage,
# special or general, consequen-	direct ou indirect, particul-
# tial or incidental, arising		ier ou general, accessoire ou
# from the use of the software.	fortuit, resultant de l'utili-
# 					sation du logiciel.
#
#***********************************************************************
#
# FILENAME
# dhsQlTool/src/misc.tcl
#
# PURPOSE:
# Whatever
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# displayAbout - Displays the "about" window which gives the version
# 		 information for the Quick Look Tool.
# sortKey - Generates a string that can be used to sort elements alpha
#	  numerically.
#
# $Log: not supported by cvs2svn $
# Revision 1.1  2004/08/30 09:15:32  brighton
# Started testing DS9 support
#
# Revision 1.1.1.1  2002/11/24 20:29:51  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.3  1998/07/22 18:54:43  jaeger
# Added code which displays the "About" window for the Quick Look Tool.
#
# Revision 1.2  1997/10/24 22:21:16  nhill
# Beta check in.
#
# Revision 1.1  1997/09/19 17:24:35  nhill
# Initial revision
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
#***********************************************************************
#+
# PROCEDURE NAME:
# displayAbout
#
# INVOCATION:
# displayAbout ?window?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window    (path name)     Window path name for the about window.
#				Default value is ".about"
#
# PROCEDURE VALUE:
# (string) : Window path name of the about window.
#
# PURPOSE:
# Display the "About" information for the Quick Look Tool
#
# DESCRIPTION:
# ???
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

proc	displayAbout {
    {window {.about}}
} {
    global env


    #
    # The window already exists so make sure it is displayed
    # and return.
    
    if { [ winfo exists $window ] } {
        if { ![ winfo ismapped $window ] } {
            .about activate
        }
        raise $window
        return
    }

   
    #
    # create the About window.
    #

    set version "dhsQlTool 0.07 (c) 1997 NRC/CNRC"
    set text "Gemini Telescopes Quick Look Tool\n(Part of the Gemini Telescopes
\ Data Handing System)\n\n\n$version"
    image create bitmap dhsGemini -file "$env(DHS_BASE)/images/dhsGemini.xbm"
    messagedialog $window                                               \
        -image dhsGemini                                                \
        -text "$text"						       	\
        -imagepos n                                                     \
        -title "About DHS"                                              


    #
    # Hide the Cancel button..
    #

    $window hide Cancel


    #
    # Display the window.
    #

    $window activate
}                                               

#
#***********************************************************************
#+
# PROCEDURE NAME:
# sortKey
#
# INVOCATION:
# set newString [ sortKey $oldString ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) oldString - The original string.
#
# PROCEDURE VALUE:
# newString - A new version of oldString, which when sorted aphabetically,
#	will be in correct alpha numeric order.
#
# PURPOSE:
# Change the input string into a string that will sort correctly alpha
# numerically when sorted alphabetically
#
# e.g. x2 will come before x12.
#
# It is the responsibility of the calling routine to map the modified string
# back into the original string if necessary.
#
# DESCRIPTION:
# The proceedure replaces every sequence of numbers in the string with a
# fixed length prefix containing the length of the sequence of numbers.
#
# For example x2 will become x012, where the new "01" is the length of
# the sequence of numbers, and x12 becomes x0212. when sorted aphabetically,
# x012 comes before x0212, and so the correct ordering is achieved.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# - The prefix is limited to 2 digits, so the longest string of digits 
#   which will work correctly is 99.
# - The string cannot contain ^A.
#-
#***********************************************************************
#

proc	sortKey {
    str
} {
    #
    # Put a ^A around every sequence of numbers and place in variable x.
    #

    regsub -all {([0-9]+)} $str {\1} x


    #
    # Break string x up at the ^A's. This results in items which consist
    # of all numbers or no numbers at all.
    #

    set y [ split $x {} ]
    set out {}


    #
    # Process each of the segments produced by the split.
    #

    foreach segment $y {
	if { [ regexp {[0-9]} $segment ]  } {
	    #
	    # Add the length of the segment to the output list.
	    #

	    lappend out [ format "%02d" [ string length $segment ] ]
	} 


	#
	# Add the segment to the output list.
	#

	lappend out $segment
    }


    #
    # Join the output list into a single string.
    #

    return [ join $out "" ]
}
