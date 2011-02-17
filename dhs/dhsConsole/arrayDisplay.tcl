# $Id: arrayDisplay.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsStoConsole/src/arrayDisplay.itk
#
# PURPOSE:
# Display the values contained in a status array record such as
# "DHSB:STO:stage%d" 
#
# CLASS NAME(S)
# CArrayDisplay		Monitors an array of status records and displays
#			the list in a window.
#
# METHOD NAME(S)
# CArrayDisplay::constructor
#			- Constructor for the CArrayDisplay class
# CArrayDisplay::destructor
#			- Destructor for the CArrayDisplay class
# CArrayDisplay::activate
#			- Displays the shell and wait based on the modality.
# CArrayDisplay::getPosition
#			- Determines what row a record should be at in
#			  table
# CArrayDisplay::getValueList
#			- Gets the values for a set of records; the values
#			  that are shown in one row of the table.
# CArrayDisplay::initTable
#			- Initializes the table.
# CArrayDisplay::maxRecord
#			- Determines the number of records there are.
# CArrayDisplay::resortTable
#			- Resorts the records displayed table.
# CArrayDisplay::setupStatusMon
#			- Setups the monitoring of the records.
# CArrayDisplay::showArrayRecs
#			- Gets called whenever the "inUse" value of a
#			  record gets changed.
# CArrayDisplay::updateArrayRecs
#			- Gets called whenever any of the records given
#			  by the -recornames options is changed.
# CArrayDisplay::updateTable
#			- Updates the table based on new values.
#
# GLOBAL PROCEDURES
# cArrayDisplay		Lower case method to the CArrayClass
#
# $Log: arrayDisplay.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.15  2000/01/05 20:35:45  jaeger
# Fixed a typo (fomatString replaced with formatString)
#
# Revision 1.14  1999/06/15 16:35:45  jaeger
# Changed some while loops to more appropriate for loops.
#
# Revision 1.13  1999/06/11 18:36:54  jaeger
# Added -justify option (from table widget).
#
# Revision 1.12  1999/06/09 20:34:31  jaeger
# fixed indexing bug in getPostion.  The effect of this bug was to place
# the first item at the bottom of the list.
#
# Revision 1.11  1999/06/08 22:59:26  jaeger
# Fixed "getPostion" method.  It didn't always find the correct insertion
# point for a record.
#
# Revision 1.10  1999/06/08 21:37:40  jaeger
# Changed bounds on array size.  It now expects to go from 0 to the
# maximum value given (inclusive) instead of up to the maximum value.
#
# Revision 1.9  1999/06/08 06:24:43  jaeger
# Made sure all methods have a return statement.  Fixed typos in
# -state configbody.  Added additional error reporting.  Made sure
# all calls to updateTable specified a time.
#
# Revision 1.8  1999/06/07 18:48:30  jaeger
# Added -state option
#
# Revision 1.7  1998/07/16 18:16:05  jaeger
# Updated to reflect changes in the CTable class.
#
# Revision 1.6  1998/06/25 20:12:00  jaeger
# Getting row value in updateArray Recs was incorrect - now fixed.
#
# Revision 1.5  1998/06/17 19:41:10  jaeger
# Added method information at the top of the file.
#
# Revision 1.4  1998/06/17 19:21:03  jaeger
# Added in the necessary comments in the code.
#
# Revision 1.3  1998/05/05 04:47:45  jaeger
# Removed -helpwin option.  Made sure all strings were double quoted.
#
# Revision 1.2  1998/04/14 19:46:53  jaeger
# Made sure [ code ] that gives a string result are double quoted.
#
# Revision 1.1  1998/04/08 20:27:21  jaeger
# Initial revision
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

itk::usual CArrayDisplay {
    keep -background -cursor -foreground -master -modality -title
}

option add *CArrayDisplay.arraySize 0 widgetDefault 
option add *CArrayDisplay.inuse "" widgetDefault
option add *CArrayDisplay.recordnames {} widgetDefault
option add *CArrayDisplay.rowcmd "" widgetDefault
option add *CArrayDisplay.state normal widgetDefault
option add *CArrayDisplay.sortcmd "" widgetDefault


#
#***********************************************************************
#+
# CLASS NAME:
# CArrayDisplay
#
# OPTIONS:
# arraysize	Specifies the number of elements that are in the status
#		record or the status record that contains the number of
#		elements.
# background	Specifies the background color of the shell.
# buttonboxpadx Specifies a non-negative padding distance to leave between 
#		the button group and the outer edge of the button box in 
#		the x direction. 
# buttonboxpady Specifies a non-negative padding distance to leave between 
#		the button group and the outer edge of the button box in 
#		the y direction. 
# cellbd	The size of the border around each cell in the table.
# clientdata	A hook that clients can use to store data with each widget.
# columnwidths  Specifies the width of each column in the table.
# cursor	Specifies the mouse cursor to be used.
# elementborderwidth 
#		Specifies the border width of each cell in the table.
# foreground	Specifies the foreground color of the shell.
# headings 	Specifies the headings that appear above the columns.
# hscrollmode	Specifies the scrolling mode for the horizontal scrollbar.
# inuse		Specifies the "inUse" record to check, if this record has
#		a positive value then the record is displayed.
# jump		Specifies whether or not the scrollbars jump.
# labelbitmap	Specifies the label's bitmap.
# labelfont     Specifies the label's font.
# labelimage.   Specifies the label's image.
# labelmargin	Specifies the margin between the label and the  childsite
# labelpos	Specifies the relative postion of the label with respect
#		    the childsite.
# labeltext     Specifies the text that appears in the label.
# labelvariable	Specifies the text variable of the label around the childsite.
# master	Defines the shell as being a transient window with the
#		master window given by the master option
# modality	Allows the shell to grab control of the screen in one of
#		three different ways:  application, system, or none.
# padx		Specifies a padding distance for the childsite in the 
#		x direction
# pady		Specifies a padding distance for the childsite in the 
#		y direction
# recordNames	Specifies the list of status records that are to be 
#		displayed/monitored.
# rowcmd	Specifies the command that is called when a row is added
#		or changes position in the table.
# state		Specifies whether the buttons are in a "normal", "active",
#		or "disbaled" state.
# sbwidth	Specifies the scrollbar width.
# scrollmargin  Specifies the margin between the scrollbar and the window.
# selectbackground
#		Specifies the color to use when displaying a selected item.
# separator	Specifies whether a line is drawn to separate the buttons
#		from the dialog box
# sortcmd	Specifies the command to execute that provides the sorting
#		order of the records being monitored.
# tablefont	Specifies the font used for the text displayed in the table.
# tableheight	Specifies the height of the table
# tablerelief	Specifies the relief (sunken, raised, ...) for the table.
# tablewidth	Specifies the width of the table.
# thickness	Specifies the thickness of the separator.
# title		Specifies the title that appears above the window.
# troughcolor   Specifiest the color to use for the scrollbars trough.
# vscrollmode   Specifies the vertical scrollbar mode
#
#
# PUBLIC METHODS: 
#
# activate	Display the shell and wait based on the modality.
# add		Add a button distinguished by tag to the end of the button box.
# buttonconfigure
#		Allows the buttons to be configured.
# center	Centers the shell with respect to another widget.
# cget		Standard cget method, returns the value of the specified
#			option.
# childsite	Returns the pathname of the childsite widget.
# component	Standard [ incr ] Tk component method.
# config	See configure
# configure	Standard Tk method, sets or returns options.
# deactivate	Deactivate the display of the shell.
# default	Sets the default button to the button given.
# delete	Deletes the button given from the button box. 
# hide		Hides the button given.
# index		Returns the numerical index corresponding to a button.
# insert	Same as the add command except that it inserts the new button 
#		just before the one given
# invoke	Invoke the command associated with a button.	
# isa		Sets the widget's class.
# show		Display the button give, it was previously hidden.
# showArrayRecs	Call this routine when a record is to be added or
#		removed from the list.  This should be 
# updateArrayRecs
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None
#
# PURPOSE:
# Display the contents of a statue array record. 
#
#-
#***********************************************************************
#

itcl::class CArrayDisplay {
    inherit iwidgets::Dialogshell

    #
    # Option definitions.
    #

    itk_option define -arraysize arraySize ArraySize 0 { 
	setupStatusMon
	if { "$updatePending" != "" } {
	    updateTable later
	}
    }

    itk_option define -inuse inUse InUse "" { 
	setupStatusMon
	if { "$updatePending" != "" } {
	    updateTable later
	}
    }


    itk_option define -recordnames recordNames RecordNames {}
    itk_option define -rowcmd rowCommand RowCommand ""
    itk_option define -state state State ""
    itk_option define -sortcmd sortCommand Command ""


    #
    # Public methods and data
    #
    
    public method	constructor { statusObj args }
    public method	destructor { }

    public method	activate { }


    #
    # Protected methods and data  -- NONE
    #


    #
    # Private methods and data
    #

    private method	getPosition { recNum } 
    private method      getValueList { recNum }
    private method	initTable {} 
    private method	maxRecord {}
    private method	resortTable { {when later} }
    private method	setupStatusMon {}
    private method 	showArrayRecs { name alarm time valueList }
    private method	updateArrayRecs { name alarm time valueList }
    private method	updateTable { {when later} }

    private variable	oldInUse 	{}  ;# List of old inuse values
    private variable	oldRecs 	{}  ;# List of old records
    private variable	resortPending   ""  ;# TRUE if resorting records
    private variable	statusObject 	""  ;# Instance of CDhgStatus class.
    private variable   	updatePending 	""  ;# TRUE if an update is pending
    private variable	visibleRecs 	{}  ;# List of displayed records.
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cArrayDisplay
#
# INVOCATION:
# cArrayDisplay pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)    A window path name for a nonexistent window.
# (>) options   (string)        One or more valid option/value pairs.
#
# The pathName and options are passed on to the CArrayDisplay class.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lowercased access method for the CArrayDisplay class.
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
# CArrayDisplay
# CArrayDisplay::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	::cArrayDisplay {
    pathName 
    statusObj
    args 
} {
    uplevel ::CArrayDisplay "$pathName" "$statusObj" "$args"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Arguments to itk_initialize
#
# PROCEDURE VALUE:
# pathname - window pathname of the class.
#
# PURPOSE:
# Constuctor for the CArrayDisplay class.
# 
# DESCRIPTION:
# Create a table in the dialog and initialize.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::statusObject
# CArrayDisplay::visibleRecs
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CArrayDisplay::destructor
#
# DEFICIENCIES:
# Doesn't verify that the status object is indeed what is expected.
#-
#***********************************************************************
#

body	CArrayDisplay::constructor {
    statusObj
    args
} {
    set statusObject "$statusObj"
    set visibleRecs {}

    itk_component add table {
	CTable $itk_component(dschildsite).table -labelmargin 20	\
	    -height 200							\
	    -width 600
    } {    
	keep -background -cellbd -columnwidths -cursor -elementborderwidth \
	    -foreground -headings -hscrollmode -jump -justify -labelbitmap \
	    -labelfont	-labelimage -labelmargin -labelpos -labeltext 	\
	    -labelvariable -sbwidth -scrollmargin -selectbackground 	\
	    -tablefont -troughcolor -vscrollmode
	rename -height tableheight tableHeight Height
	rename -relief tablerelief tableRelief Relief
	rename -width tablewidth tableWidth Width
    }
    pack $itk_component(table) -fill both -expand y

    eval itk_initialize $args

    initTable
    setupStatusMon

    return "$itk_component(hull)"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay
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
# Destructor for the "CArrayDisplay" class.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CArrauDisplay::resortPending
# CArrauDisplay::updatePending
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CArrayDisplay::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CArrayDisplay::destructor {
} {
    if { "$updatePending" != "" } {
	after cancel $updatePending
    }

    if { "$resortPending" != "" } {
	after cancel "$resortPending"
    }

    return
}

# ------------------------------------------------------------------
#                            OPTIONS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::recordnames
#
# INVOCATION:
# pathName configure -recordnames ?value?
#             OR
# arrayDisplay pathName -recordnames value
#
# where pathName is a window path of a "CArraySize" widget and value 
# is a list of each status record that is to be monitored for the
# array record.  For example for the Stage dir record the list 
# might be something like:
#	{ DHSB:STO:stage%d:reqId DHSB:STO:stage%d:unitName	\
#  		DHSB:STO:stage%d:dirName }
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the status records that are to be displayed in the table for
# the status record  array. The order the are given in the list is the 
# order they are displayed on the table.
#
# DESCRIPTION:
# Remove all of the records from the table and redisplay.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::updatePending
# CArrayDisplay::visibleRecs
#
# PRIOR REQUIREMENTS:
# None
#
# SEE ALSO:
# CArrayDisplay::updateTable
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody	CArrayDisplay::recordnames {
    #
    # If initialized then change the contents of the table.
    #

    if { [ llength "$visibleRecs" ] > 0 } {
	$itk_component(table) deleteRow 0 end
	set visibleRecs {}
    }

    if { "$updatePending" != "" } {
	updateTable later
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::rowcmd
#
# INVOCATION:
# pathName configure -rowcmd ?value?
#             OR
# arrayDisplay pathName -rowcmd value
# 
# where pathName is a window path of a "CArraySize" widget and value 
# is a command that is to be executed on each row of the table.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# N.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies a command that is to be executed for each for in the table.
# Intended to provide a way of formatting the look of the row.  It is
# It is that the command takes the window path of the table and a row
# number.
#
# DESCRIPTION:
# Execute the new row command on all of the visible rows.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::visibleRecs
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CArrayDisplay::updateTable
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody	CArrayDisplay::rowcmd {
    if { [ string trim "$itk_option(-rowcmd)" ] == "" } {
	return
    }


    #
    # Execute the cmd on each row in the table.
    #

    set row 0
    foreach recNum "$visibleRecs" {
	eval $itk_option(-rowcmd) $itk_component(table) $row $recNum
	incr row
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::state
#
# INVOCATION:
# pathName configure -state ?value?
#             OR
# arrayDisplay pathName -state value
# 
# where pathName is a window path of a "CArraySize" widget and value 
# is a command that can be exectued to determine the order of the
# values being displayed.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# N.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies whether the buttons on the array display are in a
# normal, active, or disabled. 
#
# DESCRIPTION:
# Either enable the buttons, or they are "active", or 
# disable them.
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
# None
#-
#***********************************************************************
#

configbody	CArrayDisplay::state {
    if { "$itk_option(-state)" != "active" &&
	"$itk_option(-state)" != "normal" &&
	"$itk_option(-state)" != "disabled" } {
	error "bad state value \"$itk_option(-state)\": must be normal, active, or disabled."
    }
	
    foreach button [ $itk_component(bbox) component ] {
	if { "$button" != "hull" } {
	    $itk_component(bbox) component $button configure 		\
		-state $itk_option(-state)
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::sortcmd
#
# INVOCATION:
# pathName configure -sortcmd ?value?
#             OR
# arrayDisplay pathName -sortcmd value
# 
# where pathName is a window path of a "CArraySize" widget and value 
# is a command that can be exectued to determine the order of the
# values being displayed.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# N.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies a command that will sort the records that are displayed.
# It is expected that the command returns the order that the records
# are sorted in. The list of records being monitored is passed into
# the command.
#
# DESCRIPTION:
# Remove all of the records that are displayed and redisplay them.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::updatePending
# CArrayDisplay::visibleRecs
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CArrayDisplay::updateTable
#
# DEFICIENCIES:
# None
#-
#***********************************************************************
#

configbody	CArrayDisplay::sortcmd {
    #
    # set order eval $itk_option(-sortcmd)
    # place items in the order given.
    #

    if { [ llength "$visibleRecs" ] > 0 } {
	$itk_component(table) deleteRow 0 end
	set visibleRecs {}
    }

    if { "$updatePending" != "" } {
	updateTable later
    }

#    return
}

# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::activate
#
# INVOCATION:
# object activate
#
# where object is an instance of a CArrayDisplay class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display the window.
#
# DESCRIPTION:
# If there are no visible records then display a pop-up window, indicating
# that there is nothing to display, otherwise display the window.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::visibleRecs
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# infoDialog
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CArrayDisplay::activate {
} {
    if { [ llength "$visibleRecs" ] < 1 && "$itk_option(-recordnames)" != "" } {
	infoDialog "No records are currently in use, nothing to display"
    } else {
        Shell::activate
    }

    return
}

# ------------------------------------------------------------------
#                        PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDisplayArray::getPosition
#
# INVOCATION:
# getPosition recNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) recNum 	(int) 	Record number whose position in the table is desired.
#
# PROCEDURE VALUE:
# pos 	(int)	Insertion point for the record
# -1 	(int)	If record is already displayed or an error occurs.
#
# PURPOSE:
# To find where the given record should be placed in the table.
#
# DESCRIPTION:
# Determine the maximum number of records in the array and the sorted 
# order.  Based on the sorted list find where the given record goes
# in the list of visible records.
#
# EXTERNAL VARIABLES:
# CArrayDisplay:visibleRecs
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

body	CArrayDisplay::getPosition {
    recNum
} {
    if { ! [ isNumber $recNum ] } {
	return -1
    }

    if { [ lsearch "$visibleRecs" $recNum ] != -1 } {
	return -1
    }


    #
    # Find the order the records are to be sorted in.
    #

    set max "[ maxRecord ]"
    set sortedList {}
    if { [ string trim "$itk_option(-sortcmd)" ] != "" } {
	set sortedList "[ eval $itk_option(-sortcmd) ]"
    } 
    
    if { $sortedList == {} } {
	set sortedList { 0 1 2 3 4 5 6 7 8 9 }
	set formatString "%d0 %d1 %d2 %d3 %d4 %d5 %d6 %d7 %d8 %d9"
	set index 1
	while { [ llength $sortedList ] < $max } {
	    eval lappend sortedList [ format $formatString $index $index \
		  $index $index $index $index $index $index $index $index ]
	    incr index
	}
    }


    #
    # Find the insertion point.  Go through the sorted list one by one
    # until either the end of the list of visible records is reached,
    # the end of the sorted records is reached, or the given record
    # can not be found in the "tail" of the sorted list.
    #

    if { [ set sortedPos [ lsearch "$sortedList" $recNum ] ] == -1 } {
	return -1
    }

    set pos -1
    for { set curPos [ expr $sortedPos - 1 ] } { $curPos >= 0  } 	\
	    { incr curPos -1 } {
	set current [ lindex "$sortedList" $curPos ]
	if { [ set pos [ lsearch "$visibleRecs" $current ] ] != -1 } {
	    break
	}
    }
    incr pos 
    

    return $pos
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::getValueList
#
# INVOCATION:
# getValueList recNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) recNum (int) 
#
# PROCEDURE VALUE:
# valueList (list) 
#
# PURPOSE:
# To find the status record values for a particular record in the array.
#
# DESCRIPTION:
# Go through the recordNames and find its value.  The valueList returned
# is in the same order as the record names.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::statusObject
#
# PRIOR REQUIREMENTS:
# CArrayDisplay::statusObject exists
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CArrayDisplay::getValueList {
    recNum
} {
    if { ! [ isNumber $recNum ] } {
	error "getValueList expected a numeriacl number not: $recNum"
    }

    set valueList {}
    foreach recordDesc "$itk_option(-recordnames)" {
	set record "[ format $recordDesc $recNum ]"
	if { [ simMode ] } {
	    lappend valueList "Simulation Mode"
	} else {
	    lappend valueList "[ $statusObject value $record ]"
	}
    }
    return "$valueList"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::initTable
#
# INVOCATION:
# initTable
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To intialize the contents of the table, after the option values have
# been evaluated.
#
# DESCRIPTION:
# If the table isn't already being updated then update it.
#
# EXTERNAL VARIABLES:
# CArrayDisplaye::updatePending
#
# PRIOR REQUIREMENTS:
# itk_initialize has been called.
# table component has been created.
#
# SEE ALSO:
# CArrayDisplay:updateTable
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CArrayDisplay::initTable {
} {
    if { $updatePending != "" } {
	updateTable now
    } else {
	after cancel $updatePending
	updateTable now
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::maxRecord
#
# INVOCATION:
# maxRecord
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# max	(int)		Maximum number of records in the array.
#
# PURPOSE:
# Find the maximum number of records in the statue record array.
# First line should be a summary not terminated with a period.
#
# DESCRIPTION:
# If no value was given for -arraysize then set max 0, if the 
# value of -arraysize is a number then use it, if -arraysize is
# a status record get its value and use it.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::statusObject
#
# PRIOR REQUIREMENTS:
# Simulate level is set.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CArrayDisplay::maxRecord {
} {
    set maxString "[ string trim $itk_option(-arraysize) ]"
    if {  "$maxString" == "" } {
	#
	# No value so use 0
	#

        set max 0
    } elseif { [ isNumber $maxString ] } {
	#
	# A numerival value so use it.
	#

	set max $maxString
    } else {
	#
	# Use the value stored in the status record given
	#

	if { [ simMode ] } {
	    set max 1
	} else {
	    set max "[ $statusObject value $maxString ]"
	    if { ! [ isNumber $maxString ] } {
		set max 0
	    }
	}
    }
    return $max
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::resortTable
#
# INVOCATION:
# resortTable ?when?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) when 	(string) Either "now" or "later"
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To sort the contents of the table according to the (-sortcmd) or
# according to the record number (in increasing order).
#
# DESCRIPTION:
# If the table is to be resorted later then call "after idle".
# Find the sorting order, the number of records in the array, and the
# list of records that are visible. Sort the visible records based on
# sorted list given.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::resortPending
# CArrayDisplay::visibleRecs
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# Could be more effiecient.
#-
#***********************************************************************
#

body	CArrayDisplay::resortTable {
    { when "later" }
} {
    #
    # Find out when the table is to be resorted.
    #

    if { $when == "later" } {
	if { $resortPending == "" } {
	    set resortPending [ after idle "[ code $this resortTable now ]" ]
	} 
	return
    } elseif { $when != "now" } {
	error "bad resortTable option \"$when\":should be now or later."
    }


    #
    # Some variable initialization.
    #

    set max "[ maxRecord ]"
    if { "[ string trim $visibleRecs ]" == "" } {
	set visibleRecs { }
    }


    #
    # Find the order the table is to be sorted in.
    #

    if { [ string trim $itk_option(-sortcmd) ] != "" } {
	set sortedList "[ eval $itk_option(-sortcmd) ]"
    } else { 
	set sortedList {}
    }
    
    if { $sortedList == {} } {
	#
	# Sort by increasing value, if no list was given.
	#

	set sortedList { 0 1 2 3 4 5 6 7 8 9 }
	set formatString "%d0 %d1 %d2 %d3 %d4 %d5 %d6 %d7 %d8 %d9"
	set index 1
	while { [ llength "$sortedList" ]  < $max } {
	    eval lappend sortedList [ format "$formatString" $index $index \
		  $index $index $index $index $index $index $index $index ]
	    incr index
	}
    }

    
    #
    # Now do the sorting.
    # 

    set index 1
    set visible "$visibleRecs"
    foreach current [ lrange "$visible" 1 end ] {
	set prev [ lindex "$visibleRecs" [ expr $index - 1 ] ]

	if { [ set prevPos [ lsearch "$sortedList" $prev ] ] == -1 } {
	    #
	    # Ignore this one, it isn't in the sorted list.
	    #

	    continue
	}
	

	#
	# Find all the records that occur before the given one.
	#
	
	set pos $index
	while { $pos > -1 && $prevPos > 0 } {
	    set beforeList [ lrange "$sortedList" 0 [ expr $prevPos - 1 ] ]

	    if { [ lsearch "$beforeList" $current ] == -1 } {
		break
	    }
		
	    if { [ set pos [ lsearch "$visibleRecs" $prev ] ] == -1 } {
		#
		# If the previous one isn't in the list we have a problem.
		#
		
		return
	    }
		
	    if { $pos > -1 } {
		set pos [ expr $pos - 1 ]
		set prev [ lindex "$visibleRecs" $pos ]
		set prevPos [ lsearch "$sortedList" $prev ]
	    }
	}


	#
	# Now that we know what is before the record we know
	# where to insert it.
	#

	if { $pos != $index } {
	    #
	    # The record needs to move!
	    #

	    if { $pos < [ expr [ llength "$visibleRecs" ] - 1 ] } {
		set pos [ expr $pos + 1 ]
	    }

	    set valueList "[ $itk_component(table) getRow $index ]"
	    $itk_component(table) deleteRow $index
	    set visibleRecs [ lreplace "$visibleRecs" $index $index ]

	    if { $pos == [ llength "$visibleRecs" ] } {
		#
		# Add it to the end of the visible records.
		#

		$itk_component(table) addRow "$valueList"
		if { "$visibleRecs" == "" } {
		    set visibleRecs [ list $current ]
		} else {
		    lappend visibleRecs $current
		}
	    } elseif { $pos > -1 && $pos < [ llength "$visibleRecs" ] } {
		#
		# Add it anywhere but the end of the visible records
		#

		$itk_component(table) insertRow $pos "$valueList"
		set visibleRecs [ linsert $visibleRecs $pos $current ]
	    } else {
		#
		# It doesn't get added, must have a positive position.
		#

		incr index
		continue
	    }


	    #
	    # Do any special row formating.
	    #

	    if { [ string trim $itk_option(-rowcmd) ] != "" } {
		eval $itk_option(-rowcmd) $itk_component(table) 	\
		    [ lsearch "$visibleRecs" $current ] $current
	    }
	}
	incr index
    }
    
    set resortPending ""
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::setupStatusMon
#
# INVOCATION:
# setupStatusMon 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To setup the monitoring of the status records.
#
# DESCRIPTION:
# Make sure "showArrayRecs" is notified of all changes in "inUse" records
# and "updateArrayRecs" is notified of any changes in the records given
# in -recordnames.  Also make sure we aren't notified for all the old
# values that were being traced.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::oldInUse
# CArrayDisplay::oldRecs
# CArrayDisplay::statusObject
#
# PRIOR REQUIREMENTS:
# itk_initialize has been called, the object has been intialized.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CArrayDisplay::setupStatusMon {
} {
    set max "[ maxRecord ]"

    
    #
    # Make sure showArrayRecs doesn't get called for any old value.
    #

    foreach statusItem $oldInUse {
	$statusObject unproc $statusItem "[ code $this showArrayRecs ]"
    }
    set oldInUse {}
    

    #
    # showArrayRecs gets called when "inUse" values change.
    #

    if { [ string trim $itk_option(-inuse) ] != "" } {
	for { set index 0 } { $index < "$max" } { incr index } {
	    set statusItem [ format "$itk_option(-inuse)" $index ]
	    $statusObject proc $statusItem "[ code $this showArrayRecs ]"
	    lappend oldInUse "$statusItem"
	}
    }


    #
    # Make sure updateArrayRecs doesn't get called for any old value.
    #

    foreach statusItem $oldRecs {
	$statusObject unproc $statusItem "[ code $this updateArrayRecs ]"
    }
    set oldRecs {}


    #
    # updateArrayRecs gets called when "inUse" values change.
    #

    for { set index 0 } { $index < "$max" } { incr index } {
	foreach record "$itk_option(-recordnames)" {
	    set statusItem [ format "$record" $index ]
	    $statusObject proc $statusItem "[ code $this updateArrayRecs ]"
	    lappend oldRecs "$statusItem"
	}
    }
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::showArrayRecs
#
# INVOCATION:
# pathname showArrayRecs recordName alarm time valueList
#
# where pathname is a CArrayDisplay window.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name	( string )	Name of record that has been modified.
# (>) alarm	( string )	Alarm value.
# (>) time	( date )	Time of modification.
# (>) valueList ( list )	Records new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To either add or remove from the table based on the "inUse" value.
#
# DESCRIPTION:
# If the value is positive and the record is not already visible then
# display it.  Conversly if the value is negative and the record is
# visible then remove it.  Otherwise leave it as it is.
#
# This procedure is expected to be used with the "inUse" record of
# the status record array.  If the inuse option has not been specified 
# then this method will not be called.  If the record is added and
# -rowcmd has been specified the command is called.
#
# EXTERNAL VARIABLES:
# CArrayDisplay::visibleRecs
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

body	CArrayDisplay::showArrayRecs {
    name
    alarm
    time
    valueList
} {
    #
    # Whole wack of initialization.
    #

    set name [ lindex [ split "$name" . ] 1 ]
    set recNum [ extractNums "$name" ]
    if { [ llength "$recNum" ] > 1 } {
	error "Unable to determine which record is to be shown/not shown."
    }

    if { [ string trim "$itk_option(-inuse)" ] == "" } {
	return
    }

    
    if { [ string trim "$visibleRecs" ] == "" } {
	set visibleRecs {}
    }


    if { [ isFalse "$valueList" ] && 					\
	     [ set pos [ lsearch "$visibleRecs" $recNum ] ] != -1 } {
	#
	# Remove the recod from the display.
	#

	$itk_component(table) deleteRow $pos
	set visibleRecs [ lreplace "$visibleRecs" $pos $pos ]
	if { "$visibleRecs" == "" } {
	    set visibleRecs {}
	}
    } elseif { [ isTrue "$valueList" ] && 				\
		   [ lsearch "$visibleRecs" $recNum ] == -1 } {
	#
	# Add the record to the table.
	# 

	set valueList "[ getValueList $recNum ]"
	set pos [ getPosition $recNum ]
	if { $pos == [ llength "$visibleRecs" ] } {
	    #
	    # Add to the end
	    #

	    $itk_component(table) addRow "$valueList"
	    if { $visibleRecs == "" } {
		set visibleRecs [ list $recNum ]
	    } else {
		lappend visibleRecs $recNum
	    }
	} elseif { $pos > -1 && $pos < [ llength "$visibleRecs" ] } {
	    #
	    # Add somewhere besides the end.
	    #

	    $itk_component(table) insertRow $pos "$valueList"
	    set visibleRecs [ linsert "$visibleRecs" $pos $recNum ]
	} else { 
	    #
	    # Error
	    #
	    
	    error "Unable to place record $recNum in array display at position $pos."
	}


	#
	# Do any special row formating code.
	#

	if { [ string trim "$itk_option(-rowcmd)" ] != "" } {
	    eval $itk_option(-rowcmd) $itk_component(table) [ lsearch 	\
		     "$visibleRecs" $recNum ] $recNum
	}
    }
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::updateArrayRecs
#
# INVOCATION:
# pathname updateArrayRecs recordName alarm time valueList
#
# where pathname is a CArrayDisplay window.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name	( string )	Name of record that has been modified.
# (>) alarm	( string )	Alarm value.
# (>) time	( date )	Time of modification.
# (>) valueList ( list )	Records new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the contents of the table with the new value in the
# status record.
#
# DESCRIPTION:
# Find the row and column the status record is at in the table.
# Use the setRow or setCol table command to update the value.
# Make sure -rowcmd is called if it has been set.
#
# EXTERNAL VARIABLES:
# CArrauDisplay::resortPending
# CArrayDisplay::visibleRecs
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

body	CArrayDisplay::updateArrayRecs {
    name
    alarm
    time
    valueList
} {
    #
    # Bunch of initialization.
    #

    set name [ lindex [ split "$name" . ] 1 ]
    set recNum [ extractNums "$name" ]
    if { [ llength $recNum ] > 1 } {
	error "Unable to determine which record is to be updated."
    }


    #
    # Find the row the record is at.
    #

    if { [ set row [ lsearch "$visibleRecs" $recNum ] ] == -1 } {
	return
    }


    #
    # Find the col the record is at.
    #
		 
    set front [ lindex [ split "$name" $recNum ] 0 ]
    set back  [ lindex [ split "$name" $recNum ] end ]
    set formatStr "${front}%d$back"
    set col [ lsearch "$itk_option(-recordnames)" $formatStr ]


    #
    # Change the contents of the row.
    # 

    if { $col == -1 } {
	set valueList "[ getValueList $recNum ]"
	$itk_component(table) setRow $row "$valueList"
    } else {
	$itk_component(table) setCell $row $col "$valueList"
    }


    #
    # Do any special row formating code.
    #    

    if { [ string trim "$itk_option(-rowcmd)" ] != "" } {
	eval $itk_option(-rowcmd) $itk_component(table) $row $recNum
    }


    #
    # Make sure the row is sorted properly. 
    #

    if { $resortPending == "" } {
	resortTable
    }
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CArrayDisplay::updateTable
#
# INVOCATION:
# updateTable ?time?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) time	(string)   When update should be done "now" or "later".
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the contents of the table.  Most likely due to a change in
# the status record  values, or a change in on of the configuration 
# parameters.
#
# DESCRIPTION:
# Determine when the update is to occur.  Make sure the visible records
# are resorted and add any rows that are not being displayed.
#
# It is assumed the unwanted ones are removed prior to calling this
# method.
#
# EXTERNAL VARIABLES:
# CArrayDisplay:updatePending
# CArrayDisplay:visibleRecs
# CArrauDisplay::resortPending
# CArrayDisplay::statusObject
#
# PRIOR REQUIREMENTS:
# itk_initialize has been called.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# Could be more efficient.
#-
#***********************************************************************
#

body	CArrayDisplay::updateTable {
    { when "later" }
} {
    if { $when == "later" } {
	if { $updatePending == "" } {
	    set updatePending [ after idle "[ code $this updateTable now ]" ]
	} 
	return
    } elseif { $when != "now" } {
	error "bad option \"$when\": should be now or later."
    }

    set max "[ maxRecord ]"
    if { [ string trim "$visibleRecs" ] == "" } {
	set visibleRecs { }
    }


    #
    # Make sure visibleRecs and sortedList are in the same order.
    # Rearrange the rows if necessary.
    #

    if { $resortPending != "" } {
	after cancel $resortPending
    }
    resortTable now


    #
    # Add any rows that aren't being displayed and should be.
    #

    set inUseStr "[ string trim $itk_option(-inuse) ]"
    for { set index 0 } { $index < "$max" } { incr index } {
	set isDisplayed 1
	if { $inUseStr == "" } {
	    if { [ lsearch "$visibleRecs" $index ] == -1 } {
		set isDisplayed 0
	    }
	} else {
	    if { [ simMode ] } {
		set inUse 1
	    } else {
		if { [ simMode ] } {
		    set inUse 1
		} else { 
		    set inUse "[ $statusObject value [ format $inUseStr $index ] ]"
		}
	    }
	    if { [ isTrue "$inUse" ] && 				\
		     [ lsearch "$visibleRecs" $index ] == -1 } {
		set isDisplayed 0
	    }
	}

	if { [ isFalse $isDisplayed ] } {
	    #
	    # Add the record to the table.
	    #
	    
	    set pos [ getPosition $index ]
	    set valueList "[ getValueList $index ]"
	    if { $pos == [ llength "$visibleRecs" ] } {
		#
		# Add to end of list.
		# 

		$itk_component(table) addRow "$valueList"
		if { "$visibleRecs" == "" } {
		    set visibleRecs [ list $index ]
		} else {
		    lappend visibleRecs $index
		}
	    } elseif { $pos > -1 && $pos < [ llength "$visibleRecs" ] } {
		#
		# Insert record anywhere but at the end
		#

		$itk_component(table) insertRow $pos "$valueList"
		set visibleRecs [ linsert "$visibleRecs" $pos $index ]
	    } else {
		#
		# Error
		#

		continue
	    }


	    #
	    # Do any special row formatting.
	    #

	    if { [ string trim "$itk_option(-rowcmd)" ] != "" } {
		eval $itk_option(-rowcmd) $itk_component(table) 	\
		    [ lsearch "$visibleRecs" $index ] $index
	    }
	}
    }

    set updatePending ""
    return
}
