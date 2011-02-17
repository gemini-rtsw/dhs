# $Id: logMonitor.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1998				(c) 1998
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
# dhsConsole/src/logMonitor.itk
#
# PURPOSE:
# Defines the class responsible of creating a log for a particular 
# EPICS record
#
# CLASS NAME(S)
# CLogMonitor			: Creates a log for a particular log message.
#
# METHOD NAME(S)
# PUBLIC
# CLogMonitor::constructor	: Constructor for the CLogMonitor class.
# CLogMonitor::destructor	: Destructor for the CLogMonitor class.
# CLogMonitor::getLog		: Get the contents of the log.
#
# PROTECTED
# CLogMonitor::_dropdownBtnRelease : Gets called when the button is released.
# CLogMonitor::_postList	: Gets called when the list is made visible.
#
# PRIVATE
# CLogMonitor::appendLog	: Appends new log info. to a queue.
# CLogMonitor::updateLog	: Actually adds the new log info. to the log.
#
#
# GLOBAL PROCEDURE(S)
# cLogMonitor 	: Lower case method to the CLogMonitor (to make it
#		  (Gemini complient).
#
# $Log: logMonitor.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.11  1999/06/18 22:44:53  jaeger
# Fixed bug in updateLog method.
#
# Revision 1.10  1999/06/15 16:38:02  jaeger
# Changed the methodology of the log queue update to be similar to the
# dhgStatus queue.  As a result it should consume less resources but
# display of log messages may be delayed by as much as 1 second.
#
# Revision 1.9  1999/06/07 21:12:16  jaeger
# Changed hull relief from ridge to groove.
#
# Revision 1.8  1999/01/26 06:11:11  jaeger
# Added support for handling "\n" properly.  So log messages may appear on
# several lines rather than one long line.
#
# Revision 1.7  1998/06/25 17:55:32  jaeger
# Made sure the log contains the first status item.
#
# Revision 1.6  1998/06/19 22:43:00  jaeger
# Brought code documentation up to Gemini standards.  This class now
# inherits from the iTk::combobox class which means that it can be
# used to display the log, not just monitor.
#
# Revision 1.5  1998/05/22 16:49:26  jaeger
# Removed some unneeded debugging stuff.
#
# Revision 1.4  1998/05/20 21:06:48  jaeger
# Added some debugging output.
#
# Revision 1.3  1998/05/20 05:22:42  jaeger
# Removed extra braces from intial log value.
#
# Revision 1.2  1998/05/15 17:45:07  jaeger
# added a "getLog" method.
#
# Revision 1.1  1998/05/04 21:02:48  jaeger
# Initial revision
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

option add *CLogMonitor.logcommand {} widgetDefault
option add *CLogMonitor.length 50 widgetDefault

itk::usual CLogMonitor {
	keep -command -length 
}

#
#***********************************************************************
#+
# CLASS NAME:
# CLogMonitor
#
# OPTIONS
# arrowrelief		: Specifies the relief style to use on the arrow button.
# autoclear		: Specifies whether or not to clear the entry field
#			  as items are added to the list.
# background    	: Specifies the background color of the shell.
# borderwidth		: Specifies a non-negative value indicating the width 
#			  of the 3-D border to draw around the outside.
# childsitedpos		: Specifies the position of the childsite in the 
#			  entry field.
# clientdata    	: A hook that clients can use to store data with 
#			  each widget.
# command		: Specifies a Tcl command to be executed upon 
#			  detection of a Return key press event in the
#			  entry box.
# cursor        	: Specifies the mouse cursor to be used.
# dropdown		: If true, the Combobox will be a dropdown style widget.
# editable		: If true the user can add items to the combobox.
# exportselection	: Specifies whether or not a selection in the widget 
#			  should also be the X selection.
# fixed			: Restrict entry to the specified number of chars. 
# fliparrow		: If true arrow on the button flips up and down
#			  based on whether or not the list is displayed.
# focuscommand		: Specifies a Tcl command to be executed upon 
#			  reception of focus, in the entry box
# foreground    	: Specifies the normal foreground color.
# highlightcolor	: Specifies the color to use when the widget has the 
#			  input focus.
# highlightthickness	: Specifies the width of the highlight rectable to draw
# hscrollmode   	: Specifies the scrolling mode for the horizontal 
#			  scrollbar.
# insertbackground	: Specifies the color to use as background in the 
#			  area covered by the insertion cursor.
# insertborderwidth	: Specifies the  non-negative width of the 3-D border 
#			  to draw around the insertion cursor.
# insertofftime		: Specifies the non-negative number of milliseconds 
#			  the insertion cursor should remain "off".
# insertontime		: Specifies the non-negative number of milliseconds 
#			  the insertion cursor should remain "on".
# insertwidth		: Specifies a value indicating the total width of 
#			  the insertion cursor.
# invalid		: Specifies a Tcl command to be executed upon 
#			  determination of invalid input. 
# items			: Specifies the contents of the listbox as a proper 
#			  list of elements. 
# justify		: Deterines how multiple lines, line up with each other.
# labelbitmap   	: Specifies the label's bitmap.
# labelfont     	: Specifies the label's font.
# labelimage.   	: Specifies the label's image.
# labelmargin   	: Specifies the margin between the label and the 
#			  childsite
# labelpos      	: Specifies the relative postion of the label with 
#			  respect the childsite.
# labeltext     	: Specifies the text that appears in the label.
# labelvariable 	: Specifies the text variable of the label around 
#			  the childsite.
# length		: Specifies the number of items to keep in the log.
# listheight		: Height of the listbox.
# logcommand		: Specifies the command that is executed when the log
#	  	  	  changes.
# margin		: Specifies the width in pixels between the entry 
#			  component and the arrow button
# popupcursor		: Specifies the cursor to be used for dropdown style 
#			  listboxes.
# relief		: Specifies the 3-D effect desired for the widget.
# selectbackground	: Specifies the background color to use when 
#			  displaying a  selected item.
# selectborderwidth	: Specifies the borderwidth to use when displaying a 
#			  selected item.
# selectforeground	: Specifies the foreegroudn color to use when 
#			  displaying a selected item.
# selectioncommand	: Defines the proc to be called when an item is 
#			  selected in the list.
# show			: If true the contents of the entry are not shown.
# state			: Specifies one of two states for the entry: normal 
#			  or disabled.
# textbackground	: Specifies the background color for the listbox.
# textfont		: Specifies the font to be used for text in the listbox.
# textvariable		: Specifies the name of the variable whose value
#			  is displayed in the entry.
# unique		: If true, then duplicates are not allowed to be 
#			  inserted into the list.
# validate		: Allows specification of a validation mechanism,
#			  such as numeric, alphabetic, or real.
# vscrollmode		: Specifies the vertical scrollbar mode
# width			: Specifies the desired width of the entry window.
#
# PUBLIC METHODS: 
# cget          	: Standard cget method, returns the value of the 
#                 	  specified option.
# childsite     	: Returns the pathname of the childsite widget.
# clear			: Clears the entry widget.
# component     	: Standard [ incr ] Tk component method.
# config        	: See configure
# configure     	: Standard Tk method, sets or returns options.
# constructor		: Constructor for the CLogMonitor
# curselection		: Returns the indicies of the currently selected item(s)
#		  	  in the list.
# delete		: Delete text from the entry widget.
# destructor		: Destructor for the CLogMonitor
# get			: Get the contents from the entry widget.
# getLog		: Returns the current log value.
# getcurselection 	: Returns the currently selected item(s) in the list.
# icursor		: Arrange for the insertion cursor to be displayed 
#			  just before the character given or index.
# index			: Returns the numerical index corresponding to entry
#			  index. 
# insert		: Insert the characters of string in the entry
# isa           	: Sets the widget's class.
# justify		: Justifies the contents of the list box.
# scan			: Scans the entry.
# see 			: Adjust the listbox so that the given elment is
#			  visible
# select		: Adjusts the selection within an entry
# size			: Returns the number of elements in the listbox.
# sort			: Sort the current list.
# xview			: Change or query the vertical position of the list box.
# yview			: Change or query the horizoncal position of the 
#			  list box.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Creates a combobox that monitors a particular status record, it keeps
# a "log" of the record values.
#
#-
#***********************************************************************
#

class CLogMonitor {
    inherit iwidgets::Combobox
    #
    # Options
    #

    itk_option define -logcommand logommand Comamnd {} {
	if { $itk_option(-logcommand) != "" } {
	    eval $itk_option(-logcommand) "$log"
	}
    }

    itk_option define -length length Length 50


    #
    # Public methods and data
    #

    public method constructor { statusObj SIR args }
    public method destructor {} {}
    
    public method getLog {} {
	return "$log"
    }


    #
    # Protected methods and data  
    #

    #
    # Is called when the button is released.  Override the method
    # in the combobox class.
    #

    protected method _dropdownBtnRelease {window x y} {
    	# if it's a scrollbar then ignore the release
	if { ($window == [$itk_component(list) component vertsb]) ||
	     ($window == [$itk_component(list) component horizsb]) } {
	    return
	}
	$this see 0
	_unpostList 
    }
    protected method _postList {}

    #
    # Private methods and data
    #

    private method appendLog {}
    private method updateLog { name alarm time valueList }

    private variable statusObject ""
    private variable log {}
    private variable logLock {}
    private variable tmpQueue {}
    private common unique 0
    private variable updating ""
    private variable queue {}

    private variable _ignoreRelease
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cLogMonitor
#
# INVOCATION:
# cLogMonitor name epicsSAD epicsSIR args
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name	(string)	Name of the object to be created.
# (>) statusObj	(CDhgStatus)	CDhgStatus object.
# (>) SIR	(string)	Which status record to log.
# (>) args	(list)		One or more valid option/value pairs.
#
# PROCEDURE VALUE:
# (string) name - The name of the object created.
#
# PURPOSE:
# To provide a lowercase access method to the CLogMonitor class
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLogMonitor
# CLogMonitor::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	cLogMonitor {
    objectName		     
    statusObj
    SIR
    args
} {
    uplevel ::CLogMonitor "$objectName" "$statusObj" "$SIR" $args
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLogMonitor::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObj	(CDhgStatus)	Object of class CDhgStatus
# (>) SIR	(string)	Name of the SIR record to be monitored.
# (>) args 	(string)	Valid option/value pairs.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Constructor for the "CLogMonitor" class.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CLogMonitor::statusObject
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLogMonitor::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CLogMonitor::constructor {
    statusObj
    SIR
    args
} {
    set statusObject $statusObj
    $itk_component(list) component hull configure -relief groove -borderwidth 4
    
    $statusObject link $SIR "logMsg$unique"

    $this configure -textvariable logMsg$unique -editable false
    if {[llength $log] != 0} {
	eval $this insert list end $log
    }
    
    incr unique

    set log "[ list [ $statusObject value $SIR ] ]"
    $statusObject proc $SIR "[ code $this updateLog ]"

    eval itk_initialize $args
}

# ------------------------------------------------------------------
#                       OPTIONS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLogMonitor::length
#
# INVOCATION:
# objectNfame configure -length ?value?
# 		OR
# CLogMonitor objectName -length value
#
# where objectName is the name of a particular instance of the CLogMonitor
# class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) length	(integer)	Length of the log to keep.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the length of the log to keep.
#
# DESCRIPTION:
# If the new length is smaller the the current log length then trim
# the log.  Call the command specified by the -logcommand option with 
# the new log.
#
# EXTERNAL VARIABLES:
# CLogMonitor::log
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

configbody	CLogMonitor::length {
    if { [ llength $log ] > $itk_option(-length) } {
	set end [ expr $itk_option(-length) - 1 ]
	set log [ lrange $log 0 $end ]
    }
    #$this configure -items "$log"
    if {[llength $log] != 0} {
	eval $this insert list end $log
    }


    if { $itk_option(-logcommand) != "" } {
	eval $itk_option(-logcommand) [ list $log ]
    }
}

# ------------------------------------------------------------------
#                      PROTECTED METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLogMonitor::_postList
#
# INVOCATION:
# _postList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display the list box with the log information in it.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# itk::Combobox::_drawArrow
# itk::Combobox::_positionList
# itk::Combobox::_listShowing
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CLogMonitor::_postList {
} {
    if { [ $itk_component(list) size ] == "" } {
        return
    }

    set _isPosted true
    _drawArrow depressed      ;# sad button
    _positionList


    #
    # map window
    #

    wm deiconify $itk_component(popup)
    _listShowing -wait

    raise $itk_component(popup)
    focus $itk_component(popup)

    _drawArrow normal
}

# ------------------------------------------------------------------
#                       PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLogMonitor::appendLog
#
# INVOCATION:
# appenLog
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the log that is kept for the status record.
#
# DESCRIPTION:
# Update the log by qppending the new values that have
# been queued onto the log.
#
# EXTERNAL VARIABLES:
# CLogMonitor::updating
# CLogMonitor::tmpQueue
# CLogMonitor::queue
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLogMonitor::updateLog
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CLogMonitor::appendLog {
} {
    set logLock 1
    
    set tmpList "$log"
    set log "$queue"

    set end [ expr $itk_option(-length) - [ llength $log ]  - 1 ]
    eval lappend log [ lrange $tmpList 0 $end ]
    #$this configure -items "$log"
    if {[llength $log] != 0} {
	eval $this insert list end $log
    }

    if { $itk_option(-logcommand) != "" } {
	after idle "$itk_option(-logcommand) [ list $log ]"
    }

    
    set queue "$tmpQueue"
    set logLock 0
    set updating ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLogMonitor::updateLog
#
# INVOCATION:
# updateLog name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the status record being updated.
# (>) alarm     (string)        Alarm status, OK, Warning or Bad
# (>) time      (time)          Time of the update.
# (>) valueList (list)          The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To add the new status record value to the queue, so that it can
# be logged.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CLogMonitor::updating
# CLogMonitor::tmpQueue
# CLogMonitor::queue
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLogMonitor::appendLog
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CLogMonitor::updateLog { 
    name 
    alarm
    time
    valueList
} {
    set newItem {}
    foreach line [ split "$valueList" "\n" ] {
	lappend newItem $line
    }

    if { [ isTrue $logLock ] } {
	set tmpQueue "[ list $newItem ] $tmpQueue"
    } else {
	set queue "[ list $newItem ] $queue"
    }
	
    if { $updating == "" } {
	set updating "[ after 1000 [ code $this appendLog ] ]"
    }
}

