# $Id: dhsQlTool.tcl,v 1.1 2004-08-30 09:15:32 brighton Exp $
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
# dhsQlTool/src/dhsQlTool.tcl
#
# PURPOSE:
# Contains the CDhsQlTool class
#
# CLASS NAME(S)
# CDhsQlTool - Main class for the DHS Quick Look Tool.
#
# METHOD NAME(S)
# CDhsQlTool::constructor	
#		- Constructor for the CDhsQlTool class.
#
#***********************************************************************
#+
# CLASS NAME:
# CDhsQlTool
#
# PUBLIC METHODS: 
# Constructor	- The constructor for the class.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# This is the modified version of the SkyCat class used to implement the
# Gemini DHS Quick Look Tool
#
# The only change made here is to add a CDhsQltCtl object to the class.
#
#-
#***********************************************************************
#

itk::usual CDhsQlTool {}
itcl::class CDhsQlTool {
   inherit CDhsQltCtl


    #
    # Public methods and data
    #

    constructor	{ args } { this configure }
    destructor {
	puts "CDhsQlTool destructor"
	if {"$ds9_pid" != ""} {
	    exec kill $ds9_pid
	}
    }

    protected variable ds9_pid {}

};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQlTool::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Initialization arguments.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Initialize an object in the CDhsQltCtl class
#
# DESCRIPTION:
# - Initialize itk with the given arguments.
# - Add a CDhsQltCtl widget to the object.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsQlTool::constructor {
    args
} {
    global	errorInfo errorCode

    eval itk_initialize $args

    #
    # Initialize the OCSWish command acceptors.
    #

    if { [ catch { cQlServer::initialize } msg ] != 0  } {
	#
	# An error occured, display an error message.
	#

	puts $errorInfo
	puts $msg
	set savedInfo $errorInfo
	error $msg $savedInfo
    }

    if {[catch {
	set ds9_pid [ exec ds9 & ]
    } msg]} {
	puts $errorInfo
	puts $msg
	set savedInfo $errorInfo
	error $msg $savedInfo
    }
}
