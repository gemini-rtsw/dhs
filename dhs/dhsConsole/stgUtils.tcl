# $Id: stgUtils.tcl,v 1.2 2004-08-13 20:18:54 brighton Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1998 				(c) 1998 
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
# dhsStoConsole/src/stgUtils.tcl
#
# PURPOSE:
# Provide a number of utility functions that are needed by the storage
# server console, and probably nothing else.  They are very specialized
# routines.
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# None.
#
# GLOBAL PROCEDURE(S)
# displayActionInfo	: Display Storage Server's action queue.
# displayDeviceInfo	: Display the list of devices available.
# displayRequestInfo	: Display details descring a media request in a window.
# displayStageInfo	: Display Storage Server staging directories.
# displayUnitInfo	: Display Stroage Server unit list.
# formatActionRow	: Applies special formatting to each action displayed.
# formatDeviceRow	: Applies special formatting to each device displayed.
# formatStageRow	: Applies special formatting to each staging area
#			  displayed. 
# formatUnitRow		: Applies special formatting to each unit displayed.
# queueDialog		: Dialogue window displayed when queuing.
# sortActions		: Determines the sorted order for the action queue.
# sortDevices		: Determines the sorted order for the device list.
# sortUnits		: Determines the sorted order for the unit list.
# writeDialog		: Pop-up dialogue window displayed when writing.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/11/24 20:25:49  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.12  1999/06/24 17:47:47  jaeger
# Removed "req%d:AutoRetrieve" record.
#
# Revision 1.11  1999/06/21 20:57:20  jaeger
# Fixed bug in device sorting.   It wasn't doing the bubble sort properly.
#
# Revision 1.10  1999/06/16 19:53:15  jaeger
# Added functions for supporting the displaying of a "device list".
# Added procedures which display a dialogue for queueing and one for
# writting.  Used the "ArraySize" values from the status server/epics
# to determine the maximum number of elements in a list.  Added short
# help descriptions for the action, device, stage, and unit windows; the
# short help is displayed on the Storage Server's window.  Added some
# new items to the request window.  Made sure all functions had a
# "return" command and added code documentation.
#
# Revision 1.9  1999/01/26 06:29:43  jaeger
# Fixed short help bug.  It is only displayed if the parent window is a
# "ToplevelWidget".
#
# Revision 1.8  1998/08/14 21:53:52  jaeger
# Brought code documentation up to Gemini standards.
#
# Revision 1.7  1998/07/16 20:04:36  jaeger
# Added context sensitive help and short help information.
#
# Revision 1.6  1998/06/25 20:13:14  jaeger
# Changed to reflect changed in statusBox class.
#
# Revision 1.5  1998/06/15 16:33:08  jaeger
# Switched max units back to 10.  Added a routine for sorting the units.
# Fixed both action and unit sorting, so where numerical values are
# expected if a value is not numerical it appears after the numerical
# ones.
#
# Revision 1.4  1998/05/25 20:09:35  jaeger
# Changed the look of the entry boxes (relief is now ridge) so user's
# won't be tempted to edit them.  Added sorting of the "UNITS" window.
#
# Revision 1.3  1998/05/15 16:48:32  jaeger
# Added some code for debugging.
#
# Revision 1.2  1998/05/05 04:49:59  jaeger
# move createAbout procedure since it was defined elsewhere. Changed the
# routinges for displaying "Units", "Stages" and "Actions" to use and
# the instance of the CDhgStatus class given in the paramter list.
#
# Revision 1.1  1998/04/08 21:06:28  jaeger
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
# displayActionInfo
#
# INVOCATION:
# displayActionInfo statusObject ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with action information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display the Storage Server action queue in a window.
# 
# DESCRIPTION:
# If the window alread exists then display it above all other windows,
# otherwise create it and display it.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# formatActionRow
# sortActions
# cArrayDisplay
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	displayActionInfo {
   statusObject			
   { parent "" }
} {
    #
    # Find the parent window and action window.
    #
    
    if {"$parent" != ""} {
        if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }
    set window $parent.actionInfo
    

    #
    # Initialization.
    #

    set numActions "[ $statusObject value stoActionArraySize ]"
    set recordList [ list stoAction%dAction  stoAction%dReqId 		\
	 stoAction%dOption stoAction%dActive  stoAction%dPosition ]
    set headings { Action "Request Id" Options } 

    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    $window activate 
	}
	raise $window
    } else {
	#
	# Create the window.
	#

	cArrayDisplay $window $statusObject -arraysize $numActions	\
	    -cellbd 4 							\
	    -columnwidths { 10 10 30 } 					\
	    -headings $headings						\
	    -hscrollmode dynamic					\
	    -inuse "stoAction%dInUse"					\
	    -justify left						\
	    -labelmargin 30						\
	    -labeltext "Contents of the Action Queue" 			\
	    -recordnames $recordList					\
	    -rowcmd "formatActionRow $statusObject $parent"		\
	    -sortcmd "sortActions $statusObject"			\
	    -tableheight 300 						\
	    -tablewidth 485						\
	    -title "Actions"						\
	    -vscrollmode dynamic


	#
	# Set-up the buttons at the bottom
	#

	$window add Ok -command "$window deactivate" -text "Ok"
	$window add Help	 					\
	    -command "_helpInfo openUrl dhgStoQueue.html" \
	    -text Help

	if { "$parent" != "" } {
	    $parent add_short_help [ $window component bbox component Ok ] \
		{ {bitmap b1} OK button: Remove the window for the display.}
	    $parent add_short_help [ $window component bbox component Help ] \
		{ {bitmap b1} Help button: Display detailed help information.}
	}	    
	
	bind $window <Button-3> 					\
	    "_stgHelpInfo openUrl dhgStoAction.html#actionWindow"


	#
	# Display the window
	#

	$window activate
    }

    return
}
#
#***********************************************************************
#+
# PROCEDURE NAME:
# displayDeviceInfo
#
# INVOCATION:
# displayDeviceInfo statusObject ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with action information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display the list of devices.
# 
# DESCRIPTION:
# If the window alread exists then display it above all other windows,
# otherwise create it and display it.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# formatDeviceRow
# sortDevices
# cArrayDisplay
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	displayDeviceInfo {
   statusObject			
   { parent "" }
} {
    #
    # Find the parent window and action window.
    #
    
    if {"$parent" != ""} {
        if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }
    set window $parent.deviceInfo
    

    #
    # Initialization.
    #

    set numDevices "[ $statusObject value stoDeviceArraySize ]"
    set recordList [ list stoDevice%dLogicalName stoDevice%dType 	\
	stoDevice%dCapacity stoDevice%dNumDevices stoDevice%dReqId	\
	stoDevice%dUnitName stoDevice%dAvailable stoDevice%dDeviceInUse \
  	stoDevice%dTypeDesc ]

    set headings { "Logical\nName" "Media\nType" Capacity 		\
		       "Number\nof Devices"  "Request Id" "Unit Name" }

    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    $window activate 
	}
	raise $window
    } else {
	#
	# Create the window.
	#

	cArrayDisplay $window $statusObject -arraysize $numDevices	\
	    -cellbd 4 							\
	    -columnwidths { 15 10 10 12 10 20 }				\
	    -headings $headings						\
	    -hscrollmode dynamic					\
	    -inuse "stoDevice%dInUse"					\
	    -labelmargin 30						\
	    -labeltext "List of logical devices" 			\
	    -recordnames $recordList					\
	    -rowcmd "formatDeviceRow $statusObject $parent"		\
	    -sortcmd "sortDevices $statusObject"			\
	    -tableheight 300 						\
    	    -tablewidth 790						\
	    -title "Devices"						\
	    -vscrollmode dynamic


	#
	# Set-up the buttons at the bottom
	#

	$window add Ok -command "$window deactivate" -text "Ok"
	$window add Help	 					\
	    -command "_helpInfo openUrl dhgStoDevices.html" 		\
	    -text Help
	
	if { "$parent" != "" } {
	    $parent add_short_help [ $window component bbox component Ok ] \
		{ {bitmap b1} OK button: Remove the window for the display.}
	    $parent add_short_help [ $window component bbox component Help ] \
		{ {bitmap b1} Help button: Display detailed help information.}
	}	    

	bind $window <Button-3> 					\
	    "_stgHelpInfo openUrl dhgStoDevices.html"


	#
	# Display the window
	#

	$window activate
    }
    
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# displayRequestInfo
#
# INVOCATION:
# displayRequestInfo recNum statusObject ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) recNum		(integer)	Index number of the action in the
# (>) statusObject	(CDhgStatus)	Status object with req information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display a Storage Server media request in a window.
# 
# DESCRIPTION:
# If the window alread exists then display it above all other windows,
# otherwise create it and display it.
#
# EXTERNAL VARIABLES:
# ::_stgDefaults
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


proc	displayRequestInfo {
   reqNum
   statusObject
   { parent "" }
} {
    global ::_stgDefaults


    #
    # Find the parent window and request window.
    #
    
    if {"$parent" != ""} {
        if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }
    set window $parent.requestInfo$reqNum


    #
    # If window already exists just display it and return.
    #

    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    $window activate
	}
	raise $window
	return
    }


    #
    # Create the status box that contains all of the request info.
    #
    
    set name "[ $statusObject value stoReq${reqNum}ReqName ]"


    dialog $window -title "$name"
    $window hide Cancel
    $window hide Apply
    $window buttonconfigure Help  					\
	-command "_helpInfo openUrl dhgStoRequest.html#requestWindow"

    if { "$parent" != "" } {
	$parent add_short_help [ $window component bbox component OK ]	\
	    { {bitmap b1} OK button: Remove the window for the display.}
	$parent add_short_help [ $window component bbox component Help ] \
		{ {bitmap b1} Help button: Display detailed help information.}
    }


    set sBox [ cStatusBox [ $window childsite ].sBox			\
	-borderwidth 2							\
	-height 500							\
	-labeltext "Request $name Information"				\
	-relief groove							\
        -width 200 ]


    #
    # Create an Entryfield for each of the media request records.
    #

    set labels { "Request Id" "Request Type" "Media Type" 		\
		     "Number of Copies"				\
		     "Retrieval Data" "Retrieve State" 			\
		     "Queue Data" "Queue State" 			\
		     "Prepare Media Units" "Prepare State"		\
		     "Write Media Units" "Write State"			\
		     "Media Units for Cleaned Up" "Cleanup State" \
		     "Notify Value" "Notify State" }
    set sIndex 0
    foreach status { ReqId ReqType MediaType NumCopies  	\
	RetrieveSize RetrieveState QueueSize QueueState PrepNum 	\
	PrepState WriteNum WriteState CleanupNum CleanupState 		\
	NotifyValue NotifyState } {

	global ::reqStatus
	$statusObject link stoReq${reqNum}$status reqStatus($status${reqNum})
	$sBox add entryfield $status 					\
	    -labeltext "[ lindex $labels $sIndex ]:"			\
	    -justify right				       		\
	    -labelmargin 30						\
	    -relief ridge						\
	    -state disabled						\
	    -textfont $_stgDefaults(entryFont)				\
	    -textvariable reqStatus($status${reqNum})


	#
	# Add short help only if the parent is a TopLevelWidget
	#

	if { "$parent" != "" && 
	     ( [ winfo class $parent ] != "TopLevelWidget" ||
	       [ winfo class $parent ] != "CStgMain" ) } {
	    $parent add_short_help [ $sBox component $status ] 		\
		"[ lindex $_stgDefaults(sto,requestHelp) $sIndex ]"
	}


	incr sIndex
    }

    bind $window <Button-3> "_helpInfo openUrl dhgStoRequest.html#requestWindow"
    pack $sBox -expand y -fill both
    $sBox align

    $window activate

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# displayStageInfo
#
# INVOCATION:
# displayStageInfo statusObject ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with staging information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display the Storage Server staging direction information in a window.
# 
# DESCRIPTION:
# If the window alread exists then display it above all other windows,
# otherwise create it and display it.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# formatStageRow
# cArrayDisplay
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc displayStageInfo {
   statusObject		       
   { parent "" }
} {
    #
    # Find the parent window and Stage window.
    #

    if {"$parent" != ""} {
        if { "[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }
    set window ${parent}.stageInfo


    #
    # Initialization.
    #

    set numStages "[ $statusObject value stoStageArraySize ]"
    set recordList { stoStage%dDirName stoStage%dUnitName stoStage%dReqId }
    set headings { Directory "Unit Name" "Request ID" }
    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    $window activate 
	}
	raise $window
    } else {
	cArrayDisplay $window $statusObject -arraysize $numStages	\
	    -cellbd 4 							\
	    -columnwidths { 20 15 10 } 					\
	    -headings $headings						\
	    -hscrollmode dynamic					\
	    -inuse "stoStage%dInUse"					\
	    -labelmargin 30						\
	    -labeltext "Contents of Storage Server Staging Areas" 	\
	    -recordnames $recordList					\
	    -rowcmd "formatStageRow $statusObject $parent"		\
	    -tableheight 270 						\
	    -tablewidth 462						\
	    -title "Stages"						\
	    -vscrollmode dynamic


	#
	# Set-up the buttons at the bottom
	#

	$window add Ok -command "$window deactivate" -text "Ok"
	$window add Help 						\
	    -command "_helpInfo openUrl dhgStoStages.html#stageWindow" 	\
	    -text Help

	if { "$parent" != "" } {
	    $parent add_short_help [ $window component bbox component Ok ] \
		{ {bitmap b1} OK button: Remove the window for the display.}
	    $parent add_short_help [ $window component bbox component Help ] \
		{ {bitmap b1} Help button: Display detailed help information.}
	}	    

	bind $window <Button-3> 					\
	    "_stgHelpInfo openUrl dhgStoStages.html#stageWindow"


	#
	# Display the window
	#

	$window activate
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# displayUnitInfo
#
# INVOCATION:
# displayUnitInfo statusObject ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with unit information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display the Storage Server unit list in a window.
# 
# DESCRIPTION:
# If the window alread exists then display it above all other windows,
# otherwise create it and display it.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# formatUnitRow
# sortUnits
# cArrayDisplay
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	displayUnitInfo {
   statusObject		      
   { parent "" }
} {
    #
    # Find the parent window and unit window.
    #
    
    if {"$parent" != ""} {
        if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }
    set window $parent.unitInfo


    #
    # Initialization.
    #

    set numUnits "[ $statusObject value stoUnitArraySize ]"
    set recordList [ list stoUnit%dUnitName stoUnit%dReqId 		\
	 stoUnit%dUnitSize stoUnit%dPrepValue stoUnit%dWriteNum		\
	 stoUnit%dCleanupValue ]
    set headings { "Unit Name" "Request\nID" "Size" "Prepare\nValue" 	\
		       "Number\nWritten" "Cleanup\nValue" }

    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    $window activate 
	}
	raise $window
    } else {
	cArrayDisplay $window $statusObject -arraysize $numUnits 	\
	    -cellbd 4 							\
	    -columnwidths { 15 10 10 9 12 10 } 				\
	    -headings $headings						\
	    -hscrollmode dynamic					\
	    -inuse "stoUnit%dInUse"					\
	    -labelmargin 30						\
	    -labeltext "Unit Detailed Information"			\
	    -recordnames $recordList					\
	    -rowcmd "formatUnitRow $statusObject $parent"		\
	    -sortcmd "sortUnits $statusObject"				\
	    -tableheight 300 						\
	    -tablewidth 700						\
	    -title "Units"						\
	    -vscrollmode dynamic


	#
	# Set-up the buttons at the bottom
	#

	$window add Ok -command "$window deactivate" -text "Ok"
	$window add Help 						\
	    -command "_helpInfo openUrl dhgStoUnit.html#unitWindow" 	\
	    -text Help

	if { "$parent" != "" } {
	    $parent add_short_help [ $window component bbox component Ok ] \
		{ {bitmap b1} OK button: Remove the window for the display.}
	    $parent add_short_help [ $window component bbox component Help ] \
		{ {bitmap b1} Help button: Display detailed help information.}
	}	    

	bind $window <Button-3> 					\
	    "_stgHelpInfo openUrl dhgStoUnit.html#unitWindow"


	#
	# Display the window
	#

	$window activate
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# formatActionRow
#
# INVOCATION:
# formatActionRow statusObject parent table row recNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with action information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
# (>) table		(CTable)	Window pathname of the table
#					containing the action queue.
# (>) row		(integer)	Number of row in the table to format.
# (>) recNum		(integer)	Index number of the action in the
#					action status record array.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To apply special fomatting to a row of action items in a table.
#
# DESCRIPTION:
# Make the action either green (active) or yellow (inactive).  Remove
# the items in the rows which are not need, only want to shoe the action
# request identifier and option values.  Make short help information 
# available.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayActionInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	formatActionRow { 
    statusObject
    parent
    table 
    row 
    recNum
} {
    #
    # Change the colour of the first cell to green or yellow indicating
    # if it is active or inactive, respectively.
    #

    set active "[ $statusObject value stoAction${recNum}Active ]"
    set window [ $table getCellPath $row 0 ]
    if { [ isTrue $active ] } {
	$window config -foreground green4 -justify left
    } else {
	$window config -foreground yellow2 -justify left
    }

    
    #
    # Last column needs to be left justified.
    #

    set window [ $table getCellPath $row 2 ]
    $window config -justify left

    
    #
    # We only want to display the action type, request id, and option
    # values.
    #

    set length [ llength [ $table getRow $row ] ]
    set last 3

    set slaves [ pack slaves [ $table getRowPath $row ] ]
    set length [ llength $slaves ]
    while { $last < $length } {
	destroy [ lindex $slaves $last ]
	incr last
    }



    if { "$parent" == "" || 
	 ( [ winfo class $parent ] != "TopLevelWidget" &&
	   [ winfo class $parent ] != "CStgMain" ) } {
	return
    }


    #
    # Add the short help information.
    #

    set action [ $table getCell $row 0 ]

    if { [ isTrue $active ] } {
	$parent add_short_help [ $table getCellPath $row 0 ]		\
	    {Type of action that is being processed: Retrieve, Queue, Prepare, Write, Cleanup or Notify.}
    } else {
	$parent add_short_help [ $table getCellPath $row 0 ]		\
	    {Type of action waiting to be processed: Retrieve, Queue, Prepare, Write, Cleanup or Notify.}
    }


    $parent add_short_help [ $table getCellPath $row 1 ]		\
	"Unique request identifier, indicating which request the $action action will be performed on."


    if { [ regexp -nocase -- (retrieve) $action ] } {
	$parent add_short_help [ $table getCellPath $row 2 ]		\
	    {The amount of data (in Mega bytes) that is being retrieved}
    } elseif { [ regexp -nocase -- (queue) $action ] } {
	$parent add_short_help [ $table getCellPath $row 2 ]		\
	    {The number of units that are being queued and whether queueing is forced.}
    } elseif { [ regexp -nocase -- (prepare) $action ] } {
	$parent add_short_help [ $table getCellPath $row 2 ]		\
	    {The number of media units being prepared.}
    } elseif { [ regexp -nocase -- (write) $action ] } {
	$parent add_short_help [ $table getCellPath $row 2 ]		\
	    {The number of media units being written, which device will be used, and whether writing is forced.}
    } elseif { [ regexp -nocase -- (cleanup) $action ] } {
	$parent add_short_help [ $table getCellPath $row 2 ]		\
	    {The number of media units that will be cleaned up.}
    } else {
	$parent add_short_help [ $table getCellPath $row 2 ]		\
	    {No optional values associated with this actions.}
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# formatDeviceRow
#
# INVOCATION:
# formatDeviceRow statusObject parent table row recNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with action information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
# (>) table		(CTable)	Window pathname of the table
#					containing the action queue.
# (>) row		(integer)	Number of row in the table to format.
# (>) devNum		(integer)	Index number of the action in the
#					device status record array.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To apply special fomatting to a row which contains device information.
#
# DESCRIPTION:
# Make the device green (in use), red (not available), or black 
# (not in use and available).  Remove the items in the rows which are
# not displayed but their values are tracked. Finally, make short
# help information available.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayDeviceInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	formatDeviceRow { 
    statusObject
    parent
    table 
    row 
    devNum
} {
    #
    # Change the colour of the first cell to green (in use),
    # red (not available), black available and not in use.
    #

    set available "[ $statusObject value stoDevice${devNum}Available ]"
    set deviceInUse "[ $statusObject value stoDevice${devNum}DeviceInUse ]"
    set window [ $table getCellPath $row 0 ]
    if { [ isFalse $available ] } {
	$window config -foreground red -justify left	
    } elseif { [ isTrue $deviceInUse ] } {
	$window config -foreground yellow2 -justify left	
    } else {
	$window config -foreground green4 -justify left	
    }

    
    #
    # Left justified columns: 1
    # Right justify columns: 2
    #

    [ $table getCellPath $row 1 ] config -justify left
    [ $table getCellPath $row 2 ] config -justify right

    
    #
    # We don't want to display the "available" and "deviceInUse" fields.
    #

    set length [ llength [ $table getRow $row ] ]
    set last 6

    set slaves [ pack slaves [ $table getRowPath $row ] ]
    set length [ llength $slaves ]
    while { $last < $length } {
	destroy [ lindex $slaves $last ]
	incr last
    }



    if { "$parent" == "" || 
	 ( [ winfo class $parent ] != "TopLevelWidget" &&
	   [ winfo class $parent ] != "CStgMain" ) } {
	return
    }


    #
    # Add the short help information.
    #
    
    set deviceDesc "[ $statusObject value stoDevice${devNum}TypeDesc ]"
    
    $parent add_short_help [ $table getCellPath $row 0 ]		\
	{ Logical name assiciated with one or more physical devices. The display colours are as follows: green (can be used), yellow (in use) and red (not available). }
    
    $parent add_short_help [ $table getCellPath $row 1 ]		\
	"The type of device(s) it is: $deviceDesc"

    $parent add_short_help [ $table getCellPath $row 2 ]		\
	{ Amount of data, in Megabytes that is stored on the media that are written by the device(s). }

    $parent add_short_help [ $table getCellPath $row 3 ]		\
	{ Number of devices associated with the logical device. }

    $parent add_short_help [ $table getCellPath $row 4 ]		\
	{ The unique request identifier that is currently using the logical device. }

    $parent add_short_help [ $table getCellPath $row 5 ]		\
	{ The unit name of the unit which is currently using the logical device. }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# formatStageRow
#
# INVOCATION:
# formatStageRow statusObject parent table row stageNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with stage information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
# (>) table		(CTable)	Window pathname of the table
#					containing the staging area information.
# (>) row		(integer)	Number of row in the table to format.
# (>) stageNum		(integer)	Index number of the staging area in the
#					stage status record array.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To apply special fomatting to a row containing information describing a
# single staging area in a table.
#
# DESCRIPTION:
# Make short help information available from the parent window.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayStageInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	formatStageRow { 
    statusObject
    parent
    table 
    row 
    stageNum
} {
    if { "$parent" == "" || 
	 ( [ winfo class $parent ] != "TopLevelWidget" &&
	   [ winfo class $parent ] != "CStgMain" ) } {
	return
    }

    #
    # Add the short help information.
    #

    $parent add_short_help [ $table getCellPath $row 0 ]       		\
	{The name of the staging directory (this is NOT specified as a full path).}

    $parent add_short_help [ $table getCellPath $row 1 ]       		\
	{Name that has been assigned to the unit during the queueing process.}

    $parent add_short_help [ $table getCellPath $row 2 ]		\
	{Unique request identifier, indicating which request the directory and unit are assoicated with.}

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# formatUnitRow
#
# INVOCATION:
# formatUnitRow statusObject parent table row unitNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with unit information.
# (>) parent		(pathname)	This windows parent or master window.
#					Short help will be displayed on this
#					window.
# (>) table		(CTable)	Window pathname of the table
#					containing the unit queue.
# (>) row		(integer)	Number of row in the table to format.
# (>) unitNum		(integer)	Index number of the unit in the
#					unit status record array.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To apply special fomatting to a row of unit items in a table.
#
# DESCRIPTION:
# Make short help information available in the parent window..
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayUnitInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	formatUnitRow { 
    statusObject
    parent
    table 
    row 
    unitNum
} {
    if { "$parent" == "" || 
	 ( [ winfo class $parent ] != "TopLevelWidget" &&
	   [ winfo class $parent ] != "CStgMain" ) } {
	return
    }


    #
    # Add the short help information.
    #

    $parent add_short_help [ $table getCellPath $row 0 ]		\
	{Name that has been assigned to the unit during the queueing process.}
    
    $parent add_short_help [ $table getCellPath $row 1 ]		\
	{Unique request identifier, indicating which request the unit is assoicated with.}

    $parent add_short_help [ $table getCellPath $row 2 ]		\
	{The number of Megabytes of data that belong to the unit.}

    $parent add_short_help [ $table getCellPath $row 3 ]		\
	{\"1\" if the unit has been prepared in a staging directory, \"0\" otherwise.}

    $parent add_short_help [ $table getCellPath $row 4 ]		\
	{The number of media units that have been written to a medium such as CD-ROM.}

    $parent add_short_help [ $table getCellPath $row 5 ]		\
	{\"1\" if the media unit has been removed from the staging directory, \"0\" otherwise..}

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# queueDialog
#
# INVOCATION:
# queueDialog reqName reqId maxUnits "parent"
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) reqName	(string)	Name of the request to be queued.
# (>) reqId	(integer)	Request identifier of the request to be queued.
# (>) maxUnits	(integer)	max. num. of units that can be queued.
# (>) parent	(window path)	Name of the dialog's parent window.
#
# PROCEDURE VALUE:
# retValue	(list)		The values entered on the dialog window.
#				First item in the list is the number of
#				units to queues, the second one indicates
#				whether or not queuing should be forced.
#
# PURPOSE:
# To display a dialogue window to the user where optional queue parameters
# can be specified.
#
# DESCRIPTION:
# Create a dialog with a form to fill in.  On the form there is a place
# to enter the number of units to be queued and two radio buttons used to 
# determine if the queuing is to be forced.
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

proc queueDialog { 
    reqName 
    reqId 
    maxUnits 
    { parent "" }
} {
    #
    # Find the parent window.
    #

    if { "$parent" != "" } {
	if { "[ set parent [ winfo toplevel $parent ] ]" == "." } {
	    set parent ""
	}
    }

    
    #
    # Determine The new window name and if it already exists make
    # sure it is displayed.
    #

    set window "$parent.queue$reqId"
    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    wm deiconify $window 
	}
	raise $window
	return
    }


    #
    # Create the dialogue since it does not exist.
    #

    set msg "Please enter the number of units you wish to queue and choose\
 whether queuing is forced or not forced for media request $reqName."
    set d [ util::DialogWidget $window -bitmap questhead 		\
	-buttons {OK Cancel Help}					\
	-default 0							\
	-messagewidth 5i						\
	-title "Queue request $reqName"					\
	-text "$msg" 							\
        -withdraw 1 ]
    wm iconname $d "Queue $reqName"
    set bgColour [ $d cget -background ]


    #
    # Create the input portion of the dialogue.
    #

    set form [ CForm [ $d component ext ].form 				\
	   -height 60 							\
	   -hscrollmode none						\
	   -relief flat 						\
	   -vscrollmode none 						\
	   -width 150 ]


    #
    # Create the entry box for gathering the number of units.
    #

    $form add numUnits "Number of Units:" entry
    $form itemConfigure numUnits widget -width 12
    [ $form component numUnits component widget ] delete 0 end
    [ $form component numUnits component widget ] insert 0 "$maxUnits"


    #
    # Create the radio buttons for selecting forced or unforced
    # queueing.
    #

    $form add force "" radiobox
    set radiobox [ $form component force component widget ]
    $radiobox configure -relief flat
    $radiobox add Not -text "Not Forced" -width 14
    $radiobox add Forced -text "Forced" -width 9
    $radiobox select 0


    #
    # Display the input area
    #

    pack forget [ $radiobox component Not ] [ $radiobox component Forced ] 
    pack [ $radiobox component Not ] [ $radiobox component Forced ] 	\
	-anchor w 							\
	-expand y 							\
	-fill none 							\
	-ipadx 5 							\
	-ipady 0 							\
	-padx 0 							\
	-pady 0 							\
	-side left
    pack $form -expand y -fill both



    #
    # Configure the button commands to get the data input 
    #
    
    update idletasks
    set variable "$d.choice" 
    $d component button0 configure -command 				\
	"set ::$variable \[ $form get \]"
    $d component button1 configure -command 				\
	"set ::$variable {}"
    $d component button2 configure -command 				\
	"_helpInfo openUrl dhgStoAction.html#actionWindow"


    #
    # Make wure the size of the window and color are correct.
    #

    $form configure -itembackground $bgColour -background $bgColour
    set wrapLength [ winfo pixels [ $d component text ] 5i ]
    $d component text configure -wraplength $wrapLength
    $d configure -modal yes -transient 1
    set height [ expr [ winfo reqheight $d ] + 50 ]
    set width [ winfo reqwidth $d ]


    #
    # Make the dialogue modal and center it over the parent.
    #

    if { "$parent" == "" } {
	$d center_window
    } else {
	set pGeometry [ wm geometry $parent ]
	set pWidth [ lindex [ split $pGeometry "x" ] 0 ]
	set pHeight [ lindex [ split $pGeometry "x" ] 1 ]
	set pXPos [ lindex [ split $pHeight "+" ] 1  ]
	set pYPos [ lindex [ split $pHeight "+" ] 2 ]
	set pHeight [ lindex [ split $pHeight "+" ] 0  ]

	set dWidth [ winfo reqwidth $d ]
	set dHeight [ winfo reqheight $d ]

	set dXPos [ expr ( $pXPos + ( $pWidth /  2 ) ) - ( $dWidth / 2 ) ]
	set dYPos [ expr ( $pYPos + ( $pHeight /  2 ) ) - ( $dHeight / 2 ) ]
	wm geometry $d "${dWidth}x${dHeight}+${dXPos}+${dYPos}"
    }

    return "[ wm deiconify $d; wm geometry $d ${width}x${height}; $d activate ]"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# sortActions
#
# INVOCATION:
# sortActions statusObject
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with action information.
#
# PROCEDURE VALUE:
# (list) : Sorted list of action indicies for the action status array record.
#
# PURPOSE:
# To determine which order the actions should be displayed in.
# Those actions which are active are first, followed by inactive ones.
# Inactive actions are sorted on position, those with a lower "position"
# are first.  The sorted list represents the processing order. 
#
# DESCRIPTION:
# Separate all of the actions into two lists: active and inactive.
# Sort all inactive actions based on position, the action with the
# lowest position is first.  assemble the sorted list by joining the
# active and inactive lists; the active list goes in front.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayActionInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	sortActions {
    statusObject		   
} {
    #
    # Initialization.
    #

    set activeList {}
    set inactiveList {}
    set priorities {}
    set maxActions "[ $statusObject value stoActionArraySize ]"
    

    for { set index 0 } { $index < $maxActions } { incr index } {
	set inUse "[ $statusObject value stoAction${index}InUse ]"
	if { [ isFalse $inUse ] } {
	    #
	    # If this array record is not being used then skip it!
	    #

	    continue
	}

	set active "[ $statusObject value stoAction${index}Active ]"
	set priority "[ $statusObject value stoAction${index}Position ]"
	if { [ isTrue $active ] } {
	    #
	    # Active action, so add to active list.
	    #

	    lappend activeList $index
	} else {
	    #
	    # An inactive action, need to find its insertion point into
	    # the inactive list.
	    #

	    set priority "[ $statusObject value stoAction${index}Position ]"
	    if { [ llength $inactiveList ] < 1 } {
		lappend inactiveList $index
		set priorities [ list $priority ]
	    } elseif { ! [ isNumber $priority ] || ! [ isBoolean $active ] } {
		lappend inactiveList $index
		lappend priorities $priority
	    } else {
		#
		# Bad action, either priority is not a number or active
		# value is not a boolean so add it to the end.
		#

		set pIndex 0
		while { $pIndex < [ llength $inactiveList ] &&		\
		    [ isNumber [ lindex $priorities $pIndex ] ] &&	\
		    $priority >= [ lindex $priorities $pIndex ] } {
		    incr pIndex
		}
		set inactiveList [ linsert $inactiveList $pIndex $index ]
		set priorities [ linsert $priorities $pIndex $priority ]
	    }
	}
    }


    #
    # Construct the sorted list from the active and inactive lists.
    #

    set actionList {}
    if { $activeList != {} } {
	set actionList "$activeList" 
    }

    if { $inactiveList != "" } {
	set actionList "${actionList} $inactiveList"
    }


    return "$actionList"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# sortDevices
#
# INVOCATION:
# sortDevicess statusObject
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with action information.
#
# PROCEDURE VALUE:
# (list) : Sorted list of action indicies for the device status array record.
#
# PURPOSE:
# To determine the display order for the list of devices.  The list if
# sorted first by availability, then by type, then by number of devices,
# followed by the logical device name.
#
# DESCRIPTION:
# Separate all of the actions into three lists: free, inuse, not available.
# Sort each of these list by type, then by number of devices followed by
# the logical name.
#
# Join the three sorted lists with the free ones first, followed by those
# in use, which are followed by those which are not available.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayDeviceInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	sortDevices {
    statusObject		   
} {
    #
    # Initialization.
    #
    
    set freeList {}
    set inUseList {}
    set notAvailList {}
    set maxDevices "[ $statusObject value stoDeviceArraySize ]"

    #
    # Divide the devices into three lists: not available, in use and free.
    #

    for { set index 0 } { $index < $maxDevices } { incr index } {
	set inUse "[ $statusObject value stoDevice${index}InUse ]"
	if { [ isFalse $inUse ] || ! [ isBoolean $inUse ] } {
	    #
	    # This array record is not being used so skip it!
	    #

	    continue
	}

	set available "[ $statusObject value stoDevice${index}Available ]"
	set deviceInUse "[ $statusObject value stoDevice${index}DeviceInUse ]"
	if { [ isFalse $available ] || ! [ isBoolean $inUse ] } {
	    lappend notAvailList $index
	} elseif { [ isTrue $deviceInUse ] } {
	    lappend inUseList $index 
	} else {
	    lappend freeList $index
	}
    }

    #
    # Sort the three lists.  
    #   Order by type, then number of devices, then by name.
    #
    # The insertion sort algorithm is used to do the sort.
    # A binary sort algorithm would be much faster.
    #

    foreach list { notAvailList inUseList freeList } {
	eval set curList \$$list
	eval set length [ llength $curList ]
	set tmpList  "[ lindex $curList 0 ]"

	for { set cur 1 } { $cur < $length } { incr cur } {
	    set curDev [ lindex $curList $cur ]
	    set type "[ $statusObject value stoDevice${curDev}Type ]"
	    set sorted 0
	    foreach sortedDev $tmpList {
		set sortedType "[ $statusObject value stoDevice${sortedDev}Type ]"
		if { [ set cmp [ string compare $type $sortedType ] ] < 0 } {
		    #
		    # We found where this one goes
		    #
		    
		    set tmpList [ linsert $tmpList $sorted $curDev ]
		    break
		} elseif { $cmp > 0 } {
		    #
		    # Check sorted value in the list
		    #

		    incr sorted
		    continue
		}


		#
		# Same type so we need to compare the number of devices
		#
		
		set numDev "[ $statusObject value stoDevice${curDev}NumDevices ]"
		set sortedNumDev "[ $statusObject value stoDevice${sortedDev}NumDevices ]"
		if { $numDev < $sortedNumDev } {
		    #
		    # Found where this one goes.
		    #

		    set tmpList [ linsert $tmpList $sorted $curDev ]
		    break

		} elseif { $numDev > $sortedNumDev } {
		    #
		    # Need to check with next sorted item in the list.
		    #
		    
		    incr sorted
		    continue
		}


		#
		# Need to check the logicalName
		#

		set name "[ $statusObject value stoDevice${curDev}LogicalName ]"
		set sortedName "[ $statusObject value stoDevice${sortedDev}LogicalName ]"
		if { [ string compare $name $sortedName ] < 1 } {
		    #
		    # Found where this one goes
		    #

		    set tmpList [ linsert $tmpList $sorted $curDev ]
		    break
		}

		incr sorted
	    }
	    if { $sorted >= [ llength $tmpList ] } {
		lappend tmpList $curDev
	    }
	}
	set $list $tmpList

    }

    
    #
    # Join the three sorted list into a single sorted device list
    # and return the newly sorted device list.
    #

    set deviceList {}
    if { $freeList != {} } {
	set deviceList "$freeList"
    }
    
    if { $inUseList != {} } {
	if { $deviceList == {} } {
	    set deviceList $inUseList
	} else {
	    set deviceList "$deviceList $inUseList"
	} 
    }


    if { $notAvailList != {} } {
	if { $deviceList == {} } {
	    set deviceList $notAvailList
	} else {
	    set deviceList "$deviceList $notAvailList"
	} 
    }

    return "$deviceList"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# sortUnits
#
# INVOCATION:
# sortUnits statusObject
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObject	(CDhgStatus)	Status object with unit information.
#
# PROCEDURE VALUE:
# (list) : Sorted list of unit indicies for the unit status array record.
#
# PURPOSE:
# To determine which order the units should be displayed in.
# The are first sorted by request id, then by request name.
#
# DESCRIPTION:
# Go through each status record, starting with the 0th one, and find its
# position in the list.  First check its position by request identifier,
# if there is one or more units with the same request identifier then 
# sort by request name lexicographically.  Note those with "BAD" 
# (non-numeric) request identifiers appear last.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# displayUnitInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#


proc	sortUnits {
    statusObject		   
} {
    set unitList {}
    set reqIdList {}
    set nameList {}
    set index 0
    set maxUnits "[ $statusObject value stoUnitArraySize ]"

    while { $index < $maxUnits } {
	set inUse "[ $statusObject value stoUnit${index}InUse ]"
	if { [ isFalse $inUse ] } {
	    incr index
	    continue
	}


	#
	# Find the location in the list based on request ID.
	#

	set name "[ $statusObject value stoUnit${index}UnitName ]"
	set reqId "[ $statusObject value stoUnit${index}ReqId ]"
	set pos 0
	while { $pos < [ llength $reqIdList ] && 			\
	        [ isNumber [ lindex $reqIdList $pos ] ] &&		\
		( [ isNumber $reqId ] &&  				\
		  $reqId > [ lindex $reqIdList $pos ] ) } {
	    incr pos
	}
	if { ! [ isNumber $reqId ] } {
	    while { $pos < [ llength $reqIdList ] && 			\
		  ( [ isNumber [ lindex $reqIdList $pos ] ] ||		\
		  $reqId > [ lindex $reqIdList $pos ] ) } {
		incr pos
	    }
	}

	

	#
	# If the request ID matches another request ID, then
	# sort those with the same request ID by their
	# request names (lexicographically).
	#
	
	if { $pos >= [ llength $reqIdList ] } {
	    lappend unitList $index
	    lappend nameList "$name"
	    lappend reqIdList $reqId
	} elseif { $reqId != [ lindex $reqIdList $pos ]  ||		\
	       ( [ isNumber $reqId ] && 				\
	       ! [ isNumber [ lindex $reqIdList $pos ]  ] ) } {
	    set unitList [ linsert $unitList $pos $index ]
	    set nameList [ linsert $nameList $pos "$name" ]
	    set reqIdList [ linsert $reqIdList $pos $reqId ]
	} else {
	    while { $pos < [ llength $nameList ]  &&			\
		    $reqId == [ lindex $reqIdList $pos ] &&		\
		    "$name" > "[ lindex $nameList $pos ]" } {
		incr pos
	    }
	    set unitList [ linsert $unitList $pos $index ]
	    set nameList [ linsert $nameList $pos "$name" ]
	    set reqIdList [ linsert $reqIdList $pos $reqId ]
	}
	incr index
    }

    return "$unitList"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# writeDialog
#
# INVOCATION:
# writeialog reqName reqId maxUnits "parent"
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) reqName	(string)	Name of the request to be queued.
# (>) reqId	(integer)	Request identifier of the request to be queued.
# (>) maxUnits	(integer)	max. num. of units that can be queued.
# (>) deviceList (list)		Devices that can be selected.
# (>) parent	(window path)	Name of the dialog's parent window.
#
# PROCEDURE VALUE:
# retValue	(list)		The values entered on the dialog window.
#				First item in the list is the number of
#				units to be written, the logical Device
#				that will be written, and the last item
#				is whether or not to force writing.
#
# PURPOSE:
# To display a dialogue window to the user where optional queue parameters
# can be specified.
#
# DESCRIPTION:
# Create a dialogue window with a fill-in form.  The fill in form contains
# an entry box for entry the number of units to write.  A drop-down selection
# menu (combobox) for selecting a device, and a pair of radio buttons for 
# determining whether writing is to be forced.  Display this dialogue and
# return the values that were entered/selected on the form.
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

proc writeDialog { 
    reqName 
    reqId 
    maxUnits 
    deviceList
    { parent "" }
} {
    #
    # Find the parent window.
    #

    if { "$parent" != "" } {
	if { "[ set parent [ winfo toplevel $parent ] ]" == "." } {
	    set parent ""
	}
    }

    
    #
    # Determine the new window name and if it already exists make
    # sure it is displayed.
    #

    set window "$parent.write$reqId"
    if { [ winfo exists $window ] } {
	if { ! [ winfo ismapped $window ] } {
	    wm deiconify $window 
	}
	raise $window
	return
    }


    #
    # Create the dialogue since it does not exist.
    #

    set msg "Please enter the number of units you wish to write, which\
 device will do the writing, and choose whether or not writing is forced\
 for media request ${reqName}."
    set d [ util::DialogWidget $window 					\
	-bitmap questhead 						\
	-buttons {OK Cancel Help}					\
	-default 0							\
	-messagewidth 5i						\
	-title "Write request $reqName"					\
	-text "$msg" 							\
        -withdraw 1 ]
    wm iconname $d "Queue $reqName"
    set bgColour [ $d cget -background ]


    #
    # Create the input portion of the dialogue.
    #

    set form [ CForm [ $d component ext ].form 				\
	   -height 125 							\
	   -hscrollmode none						\
	   -relief flat 						\
	   -vscrollmode none						\
	   -width 150 ]


    #
    # Create the entry box for gathering the number of units.
    #

    $form add numUnits "Number of Units" entry
    set numUnits [  $form component numUnits ]
    $numUnits configure -labelpos n
    $form itemConfigure numUnits widget -width 28
    [ $form component numUnits component widget ] delete 0 end
    [ $form component numUnits component widget ] insert 0 "$maxUnits"


    #
    # Create the drop-down list with the logical devices selection.
    #
    
    set listFormat "%10.10s %6.6s %4.4s"
    set newDevList {}
    foreach deviceItem "$deviceList" {
	lappend newDeviceList [ format "$listFormat" "[ lindex $deviceItem 0 ]"\
		 "[ lindex $deviceItem 1 ]" "[ lindex $deviceItem 2 ]" ]
    }

    $form add logicalDev "Logical Device" combobox
    $form component logicalDev configure -labelpos n
    set logicalDev [ $form component logicalDev component widget ]
    $logicalDev component entry delete 0 end
    $logicalDev component entry insert 0 [ lindex [ lindex $deviceList 0 ] 0 ]
    $form itemConfigure logicalDev widget -relief ridge -state disabled \
	-width 26
    $logicalDev configure -items "$newDeviceList"
    $logicalDev component list component hull configure -relief groove 	\
	-borderwidth 4
    $logicalDev component list configure -labeltext 			\
	"           Number  In\n   Name    of Dev. Use"
    $logicalDev selection set 0

    set comboEntry [ $logicalDev component entry ]
    $logicalDev configure -selectioncommand "$comboEntry delete 11 end"


    #
    # Create the radio buttons for selecting forced or unforced
    # queueing.
    #

    $form add force "" radiobox
    set radiobox [ $form component force component widget ]
    $radiobox configure -relief flat
    $radiobox add Not -text "Not Forced" -width 12
    $radiobox add Forced -text "Forced" -width 8
    $radiobox select 0


    #
    # Display the input area
    #

    pack forget [ $radiobox component Not ] [ $radiobox component Forced ] 
    pack [ $radiobox component Not ] [ $radiobox component Forced ] 	\
	-anchor w 							\
	-expand y 							\
	-fill none 							\
	-ipadx 5 							\
	-ipady 0 							\
	-padx 5 							\
	-pady 0 							\
	-side left
    pack $form -expand y -fill both


    #
    # Configure the button commands to get the data input 
    #
    
    update idletasks
    set variable "$d.choice" 
    $d component button0 configure -command 				\
	"set ::$variable \[ $form get \]"
    $d component button1 configure -command 				\
	"set ::$variable {}"
    $d component button2 configure -command 				\
	"_helpInfo openUrl dhgStoAction.html#actionWindow"


    #
    # Make wure the size of the window and color are correct.
    #

    $form configure -itembackground $bgColour -background $bgColour
    set wrapLength [ winfo pixels [ $d component text ] 5i ]
    $d component text configure -wraplength $wrapLength
    $d configure -modal yes -transient 1
    set height [ expr [ winfo reqheight $d ] + 50 ]
    set width [ winfo reqwidth $d ]


    #
    # Make the dialogue modal and center it over the parent.
    #

    if { "$parent" == "" } {
	$d center_window
    } else {
	set pGeometry [ wm geometry $parent ]
	set pWidth [ lindex [ split $pGeometry "x" ] 0 ]
	set pHeight [ lindex [ split $pGeometry "x" ] 1 ]
	set pXPos [ lindex [ split $pHeight "+" ] 1  ]
	set pYPos [ lindex [ split $pHeight "+" ] 2 ]
	set pHeight [ lindex [ split $pHeight "+" ] 0  ]

	set dWidth [ winfo reqwidth $d ]
	set dHeight [ winfo reqheight $d ]

	set dXPos [ expr ( $pXPos + ( $pWidth /  2 ) ) - ( $dWidth / 2 ) ]
	set dYPos [ expr ( $pYPos + ( $pHeight /  2 ) ) - ( $dHeight / 2 ) ]
	wm geometry $d "${dWidth}x${dHeight}+${dXPos}+${dYPos}"
    }

    return "[ wm deiconify $d; wm geometry $d ${width}x${height}; $d activate ]"
}
