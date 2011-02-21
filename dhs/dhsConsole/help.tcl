# $Id: help.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/help.itk
#
# PURPOSE:
# Create on-line help for an application.
#
# CLASS NAME(S)
# CHelp class		: A class that displays help information, in HTML.
#	        	  Uses firefox to display the help information.
#
# METHOD NAME(S)
# CHelp::constructor 	: Constructor for the CHelp class.
# CHelp::destructor 	: Destructor for the CHelp class.
# CHelp::close 		: Closes the Firefox application.
# CHelp::getHtmlDir	: Returns the directory that is searched for HTML
#			  files.
# CHelp::openUrl 	: Opens a particular HTML file.
# CHelp::setHtmlDir 	: Sets the directory that is searched for HTML
#			  files.
#
# PRIVATE
# CHelp::checkPid	: Checks output from "ps" command for the
#			  Firefox application started by this class
#
# GLOBAL PROCEDURE(S)
# cHelp		: Lowercased access method to the CHelp class.
#
# $Log: help.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.8  1999/06/08 19:12:17  jaeger
# Use "kill -0" instead of output from "ps" to determine the existence
# of a given process id.  Made sure all firefox calls are run in the
# background.
#
# Revision 1.7  1999/02/01 19:55:05  jaeger
# Changed constructor so by default firefox is not displayed, it may
# be displayed iconically or in a window on start-up depending on
# the arguments given to the constructor.
#
# Revision 1.6  1998/06/22 17:00:41  jaeger
# Fixe up some of the documentation.  Made the constructor "iconic"
# parameter optional.
#
# Revision 1.5  1998/06/19 05:25:33  jaeger
# Brought code documentation up to Gemini Standards.  Moved small method
# definitions into the class definition.  Changed the name to CHelp from
# help class.
#
# Revision 1.4  1998/06/15 05:05:27  jaeger
# Got rid of the "-install" flag used with firefox, fixed up the
# ps call.
#
# Revision 1.3  1998/05/06 20:42:45  jaeger
# added "catch" to catch any errors when killing the firefox process.
#
# Revision 1.2  1998/05/04 18:40:42  jaeger
# Fixed code so one firefox is shared between all instances.
#
# Revision 1.1  1997/11/13 23:53:19  jaeger
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
# CHelp
#
# OPTIONS:
# None.
#
# PUBLIC METHODS: 
# cget          : Standard cget method, returns the value of the specified
#                 option.
# close 	: Closes the Firefox application.
# configure     : Standard Tk method, sets or returns options.
# constructor 	: Constructor for the CHelp class.
# destructor 	: Destructor for the CHelp class.
# getHtmlDir	: Returns the directory that is searched for HTML
#		  files.
# isa           : Sets the widget's class.
# openUrl 	: Opens a particular HTML file.
# setHtmlDir 	: Sets the directory that is searched for HTML
#		  files.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To display on-line help information in HTML files.
#
#-
#***********************************************************************
#

class CHelp {
    #
    # Public methods and data
    #

    public method constructor { url {start "none"} }
    public method destructor {}

    public method close {}
    public method getHtmlDir {} { 
	return $htmlDir
    }
    public method openUrl { url }
    public method setHtmlDir { pathName } {
	set htmlDir $pathName
    }
    

    #
    # Protected methods and data -- NONE
    #


    #
    # Private methods and data
    #

    private method checkPid
    
    private common psId ""
    private common htmlDir ""
    private common objectList {}
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cHelp
#
# INVOCATION:
# cHelp objectName url ?start?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>)objectName	(string) 	    Name for the new CHelp object.
# (>) url	(string)	    A URL of an HTML file.
# (>) ?start?   (iconic|viewable)   Indicate how firefox is to be started,
#				    iconic, viewable, or not at all.
#
# PROCEDURE VALUE:
# objectName (CHelp) : Name of the object that is created.
#
# PURPOSE:
# Lowercase access method to the CHelp class.
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
# CHelp::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	cHelp {
    objectName
    url
    {start "none"}
} {
    uplevel CHelp "$objectName" "$url" "$start"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CHelp::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) url	(string)	A URL of an HTML file.
# (>) ?start?   (iconic|viewable)   Indicate how firefox is to be started,
#				    iconic, viewable, or not at all.
#
# PROCEDURE VALUE:
# (string) objectName : Name of the object that is created.
#
# PURPOSE:
# Constructor for the "CHelp" class.
#
# DESCRIPTION:
# Add "this" object to the Object list.  If there is no Firefox
# browser running then start one.
#
# EXTERNAL VARIABLES:
# CHelp::objectList
# CHelp::psId
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CHelp::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CHelp::constructor {
    url
    {start "none"}
} {
    lappend objectList "$this"

    if { $psId != "" } {
	return
    }

    switch -exact -- "$start" {
	case "iconic" {
	    set psId [ exec firefox $url & ]
	}
	case "viewable" {
	    set psId [ exec firefox $url & ]
	}
	default {
	    #
	    # Don't start firefox.
	    #
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CHelp::destructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Try and find "this" object on the objectList.  If it is there
# remove it.  If there are no longer any object on the objectList
# then destroy the Firefox application, if there is one running.
#
# DESCRIPTION:
# Remove the firefox process.
#
# EXTERNAL VARIABLES:
# CHelp::objectList
# CHelp::psId
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CHelp::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CHelp::destructor {
} {
    set pos [ lsearch $objectList "$this" ]
    if { $pos >= 0 } {
	set objectList [ lreplace $objectList $pos $pos ]
    }

    if { $psId != "" && [ llength $objectList ] == 0 } {
	catch "exec kill -15 $psId"
    }
}

# ------------------------------------------------------------------
#                        PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CHelp::close
#
# INVOCATION:
# objectName close
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To close/exit the firefox process initiated by this class.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CHelp::psId
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None 
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CHelp::close {
} {
    if { $psId != "" } {
	if { [ catch "exec kill -15 $psId" msg ] } {
	}
    } else {
	error "Firefox is not running. $msg"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CHelp::openUrl
#
# INVOCATION:
# objectName openUrl url
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) url 	(string)    The URL of an HTML page.
#
# PROCEDURE VALUE:
# (string) url : The url provided.
#
# PURPOSE:
# To display a particular URL in a Firefox application.
#
# DESCRIPTION:
# Prepend the HTML directory to the given URL.  Display the page in
# a Firefox browser, if one isn't running start one.
#
# EXTERNAL VARIABLES:
# CHelp::psId
# CHelp::htmlDir
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

body	CHelp::openUrl {
    url
} {
    if { $htmlDir == "" } {
	set page "$url"
    } else {
	set page "$htmlDir/$url"
    }

    if { [ catch checkPid msg ] } {
	set psId [ exec firefox $page & ]
    } else {
	after idle "exec firefox $page &"
    }
}

# ------------------------------------------------------------------
#                      PRIVATE METHODS
# ------------------------------------------------------------------


#
#***********************************************************************
#+
# PROCEDURE NAME:
# CHelp::checkPid
#
# INVOCATION:
# checkPid
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# If successful the process Id of the firefox application associated
# with the help information.  Otherwise the return value of the TCL/TK
# "error" command.
#
# PURPOSE:
# To see if the firefox application is still running.
#
# DESCRIPTION:
# Check the value of psId, make sure we have one.
# If we have a psId then use the "kill -0" system call to find out 
# if the process is still active.
#
# EXTERNAL VARIABLES:
# CHelp::psId
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

body 	CHelp::checkPid {
} {
    if { "$psId"  == "" } {
	error "The help window is not available."
    } else { 
	if { [ catch "exec kill -0 $psId" msg ] } {
	    error "$msg"
	}
    }
    return $psId
}

