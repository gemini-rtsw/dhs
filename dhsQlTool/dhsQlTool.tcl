# $Id: dhsQlTool.tcl,v 1.3 2004-08-13 20:18:54 brighton Exp $
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
# $Log: not supported by cvs2svn $
# Revision 1.2  2003/01/15 18:23:35  brighton
# fixed pthread_create call, Makefile changes
#
# Revision 1.1.1.1  2002/11/24 20:29:48  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.5  1998/10/08 19:39:33  nhill
# Changed to allow more configuration information to be saved.
#
# Revision 1.4  1998/07/22 05:28:55  jaeger
# Added a destructor so that the Control Panel object gets deleted.
#
# Revision 1.3  1997/11/12 18:28:03  nhill
# Fixed a typo.
#
# Revision 1.2  1997/10/24 22:20:52  nhill
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
   inherit skycat::SkyCat


    #
    # Public methods and data
    #

    constructor	{ args } { this configure }
    destructor {
	delete object .dqtCtl
    }

    # set default X resources for colors and fonts, and set some default key
    # bindings. This method is called from the parent class and overridden 
    # here. These are built-in defaults that the user can also override in 
    # the ~/.Xdefaults file.

    protected method setXdefaults {} {
	# read rtd defaults
	Rtd::setXdefaults

	# read cat lib defaults
	cat::setXdefaults
	
	# read skycat defaults
	skycat::setXdefaults
	
	# since we know this method gets called early, this is a good place to 
	# create window to display while starting up
	make_init_window
    }
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
    itk_component add dqtCtl { CDhsQltCtl .dqtCtl }


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
    
    $itk_component(dqtCtl) initialize
    if { [ catch { $itk_component(dqtCtl) initialize } msg ] != 0  } {
	#
	# An error occured, display an error message.
	#

	puts $errorInfo
	puts $msg
	set savedInfo $errorInfo
	error $msg $savedInfo
    }
}
