# $Id: dhgUtils.tcl,v 1.2 2004-08-13 20:18:54 brighton Exp $
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
# dhsConsole/src/dhgUtils.tcl
#
# PURPOSE:
# Contains a number of helper procedures.  These helper procedures are
# specific to the DHS Console.
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# None.
#
# GLOBAL PROCEDURE(S)
# commandMenu		: Creates the command menu
# createAbout		: Creates/displayes the about box.
# displayResource	: Displays a resource window
# displaySubsystem	: Displays a subsystem window.
# getLogLength		: Get a new number of messages to be logged/kept.
# simMode		: Determine if the DHS Console is running
#			  in simulate mode.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/11/24 20:25:41  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.16  1999/06/16 23:23:38  jaeger
# Removed debugging code that is no longer needed.
#
# Revision 1.15  1999/06/10 21:00:47  jaeger
# Changes made due to changes in status record names (aliases) in the
# dhsConsole.config file.
#
# Revision 1.14  1999/06/09 23:18:28  jaeger
# Added global definitions for dhsMsgQueue, dhsTmpMsgQueue, and dhsMsgLock.
#
# Revision 1.13  1999/06/09 20:35:24  jaeger
# Added short help to buttons on the resource pop-up window.
#
# Revision 1.12  1999/06/08 23:10:06  jaeger
# Changed Quit help info to state that the window is removed.  Change
# the subsystem status context sensitive help to point to the html page
# describing the status information.
#
# Revision 1.11  1999/01/26 06:26:23  jaeger
# Added a "default" value to be used when changing the log length.
#
# Revision 1.10  1998/07/20 19:40:20  jaeger
# moved bitmaps to "images" directory, code changed to reflect this.
#
# Revision 1.9  1998/07/17 17:58:58  jaeger
# Added necessary short help information, contetext sensitive help, and
# made all help buttons point to the correct help pages.
#
# Revision 1.8  1998/06/25 20:18:11  jaeger
# Changes reflect changes in the CStatusBox class.  Also made sure
# resource windows were centered.
#
# Revision 1.7  1998/06/19 22:40:54  jaeger
# Altered displaySubsystem to reflect changes in the log monitoring
# class.
#
# Revision 1.6  1998/06/18 21:59:43  jaeger
# Brought code documentation up to Gemini standards.  Removed
# displayDtsStaging (no longer needed).  Got rid of the initialization
# of values in displaySubsystem (no needed).  Renamed resWindow to
# displayResource.  Got rid of special cases for the storage server
# in displaySubsystem (no longer needed).
#
# Revision 1.5  1998/06/15 19:23:14  jaeger
# Changed fullName to fullname
#
# Revision 1.4  1998/06/15 05:14:52  jaeger
# Added Led on subsystem window.  Added "fullName" on resource window.
# Fixed some typos.
#
# Revision 1.3  1998/05/25 20:08:57  jaeger
# Changed the look of the entry boxes (relief is now ridge) so user's
# won't be tempted to edit them.
#
# Revision 1.2  1998/05/15 17:43:37  jaeger
# Changed to reflect latest changes in CDhgStatus ( dhs server status
# acceptor).  Also added code for handling "logs" for health descriptions
# and the dhs log message.
#
# Revision 1.1  1998/05/04 21:01:58  jaeger
# Initial revision
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
#***********************************************************************
#
#	Global variable initialization
#
#***********************************************************************
#

set dhsMsgQueue     {}	;# message queue used by dialogues.
set dhsTmpMsgQueue  {}	;# temporary message queue used by dialogues.
set dhsMsgLock      0	;# lock for dhsMsgQueue



#
#***********************************************************************
#+
# PROCEDURE NAME:
# commandMenu
#
# INVOCATION:
# commandMenu parent menu subsystem dhsCommandObj
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) parent		(path name)	Window path name of the menu's parenet.
# (>) menu		(path name)	The menu's Window path name
# (>) subsystem 	(string)	Unique subsystem identifier.
# (>) dhsCommandObj	(CDhsCommand)	CDhsCommand object.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To create the "Commands" menu for a particular subsystem window,
# or the DHS Console window.
#
# DESCRIPTION:
# Use the values stored in the _dhgDefaults array to determine which
# commands are to be added onto the menu.  If the command is "separator"
# a separator line is placed on the menu.  The commands "init", "dhsExit"<
# "reset", and "test" all need a window parameter, which is added to the
# command.  The dhsCommandObject is prepended to the command.
#
# Expected values in the _dhgDefaults aray are:
#   _dhgDefaults(subsystem,commands)  : A method of the CDhsCommand class,
#					or "separator".
#   _dhgDefaults(subsystem,commandName): Descriptive command name.
#   _dhgDefaults(subsystem,commandDesc): Descriptive sentence describing the
#					 command.
#   
#
# EXTERNAL VARIABLES:
# _dhgDefaults - global array that defines all of the commands, help messages,
#		and display status for each DHS subsystem.
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

proc 	commandMenu {
    parent 
    menu 
    subsystem 
    dhsCommandObj
} {
    global ::_dhgDefaults

    set i 0 
    foreach command $_dhgDefaults($subsystem,commands) {
	if { $command == "separator" } {
	    $menu add separator
	} elseif { [ string first "resetHealth" $command ] != -1 } {
	    #
	    # Need to underline the "H" instead of the first character.
	    #

	    $parent add_menuitem $menu command 				\
		[ lindex $_dhgDefaults($subsystem,commandName) $i ]	\
		"{bitmap b1} [ lindex $_dhgDefaults($subsystem,commandDesc) $i ]" \
		-command "$dhsCommandObj $command"			\
		-underline 6
	} elseif { [ string first "hisLogWrite" $command ] != -1 } {
	    #
	    # Need to underline the "" instead of the first character.
	    #

	    $parent add_menuitem $menu command 				\
		[ lindex $_dhgDefaults($subsystem,commandName) $i ]	\
		"{bitmap b1} [ lindex $_dhgDefaults($subsystem,commandDesc) $i ]" \
		-command "$dhsCommandObj $command"			\
		-underline 13
	} elseif { [ string first "init" $command ] >= 0 ||
		   [ string first "reset" $command ] >= 0 ||
		   [ string first "dhsExit" $command ] >= 0 ||
		   [ string first "test" $command ] >= 0 } {
	    #
	    # Need to add the "parent" window between the command name and
	    # the subsystem information at the end.
	    #

	    set cmd "$command"
	    set insertPt [ string wordend $cmd 0 ]
	    if { [ string range $parent 0 1 ] == "::" } {
		set parent [ string range $parent 2 end ]
	    }
	    set cmd "[ string range $cmd 0 $insertPt ] $parent [ string range $cmd [ expr $insertPt + 1 ] end ]"

	    $parent add_menuitem $menu command 				\
		[ lindex $_dhgDefaults($subsystem,commandName) $i ]	\
		"{bitmap b1} [ lindex $_dhgDefaults($subsystem,commandDesc) $i ]" \
		-command "$dhsCommandObj $cmd"				\
		-underline 0
	} else {
	    $parent add_menuitem $menu command 				\
		[ lindex $_dhgDefaults($subsystem,commandName) $i ]	\
		"{bitmap b1} [ lindex $_dhgDefaults($subsystem,commandDesc) $i ]" \
		-command "$dhsCommandObj $command"			\
		-underline 0
	}
	incr i
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# createAbout
#
# INVOCATION:
# createAbout ?version? ?window?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name) 	Window path name for the about window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display the "About" information for the DHS console in a window.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# dhgDir - DHS GUI directory
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

proc createAbout {
    { version {} }
    { window {.about} }
} {
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

    iwidgets::messagedialog $window 						\
	-image dhsGemini						\
	-text "Gemini Telescopes Data Handling System\n\n\n$version" 	\
	-imagepos n							\
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
# displayResource
#
# INVOCATION:
# displayResource resNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) resNum	(int)	Resource number
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display the values stored in EPICS for a particular resource.
#
# DESCRIPTION:
# If window exists then display it
# Else create it
#	Create dialog box with Help and Ok buttons
# 	Add a status Box that contains the max, used, available, and
#	   capacity values.
#	Display the window.
#
# EXTERNAL VARIABLES:
# (array)  resStatus	- Contains all the information on resources. 
# (string) _dhgDefaults(entryFont)	
#			- The default font value for entry widgets.
#
# PRIOR REQUIREMENTS:
# _dhgStatus exists, instance of dhgStatus.
# _dhgHelpInfo exists, instance of help
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	displayResource {
   window
   resNum
   statusObj
   { parent "" } 
} {
    global ::resStatus
    global ::_dhgDefaults

    #
    # Determine the dialog path name
    #

    if { "$parent" != "" } {
        if { "[ set parent [ winfo toplevel $parent ] ]" == "." } {
            set parent ""
        }
    }

    set name "[ $statusObj value res${resNum}Name ]"
    set window ${parent}.res$resNum

    if { [ winfo exists ${parent}.res$resNum ] } {
	#
	# Window alread exists, so if it isn't displayed activate it
	# and raise it so it is above all other windows.
	#
	
	if { ![ winfo ismapped $window ] } {
	    $window activate
	}
	raise $window
	return $window
    } else {
	#
	# Since the window doesn't exist create it.
	# First create a dialog box with a Help and OK button. 
	#
	
	dialog $window -master $parent -title "$name"
	
	$window hide Cancel
	$window hide Apply
	$window buttonconfigure Help 					\
	    -command "_helpInfo openUrl dhgResource.html#resourceWindow"
	if { "$parent" != "" } {
	    $parent add_short_help 					\
		[ $window component bbox component OK ]			\
	    { {bitmap b1} OK button: Remove the window for the display.}
	    $parent add_short_help 					\
		[ $window component bbox component Help ] 		\
		{ {bitmap b1} Help button: Display detailed help information.}
	}
    }


    #
    # Create a status box, place the name, type and units on one line.
    # Place Max, Used, Available, and Capacity on separate lines.
    #

    $statusObj link res${resNum}Name resStatus(name$resNum)
    set sBox [ cStatusBox [ $window childsite ].sBox$resNum 	\
	   -borderwidth 2 -relief groove -height 250 -width 100	\
	   -labeltext "$name"					\
	   -labelvariable resStatus(name$resNum) ]
    $statusObj link res${resNum}Fullname resStatus(fullname$resNum)

    $sBox add entryfield fullname -textfont $_dhgDefaults(entryFont)	\
    	-labeltext "Full Name:" 					\
    	-justify right							\
	-relief ridge							\
	-state disabled							\
	-textvariable resStatus(fullname$resNum)
    set type [ $statusObj value res${resNum}Type ]
    if { [ regexp -nocase -- (Database) $type ] } {
	[ winfo toplevel $parent ] add_short_help [ $sBox component 	\
   	    fullname ] "The Sybase server and database names (server:databse)"
    } elseif { [ regexp -nocase -- "(Magnetic Disk)" $type ] } {
	[ winfo toplevel $parent ] add_short_help [ $sBox component 	\
   	    fullname ] "The full path to the magnetic disk."
    } else {
	[ winfo toplevel $parent ] add_short_help [ $sBox component 	\
   	    fullname ] "The fully specified name of the resource."
    } 

    $statusObj link res${resNum}Type resStatus(type$resNum)
    $sBox add entryfield type -textfont $_dhgDefaults(entryFont)	\
    	-labeltext "Type:" 						\
    	-justify right							\
	-relief ridge						\
	-state disabled							\
	-textvariable resStatus(type$resNum)
    [ winfo toplevel $parent ] add_short_help [ $sBox component type ]	\
	"The type of resource being monitored."

    $statusObj link res${resNum}Units resStatus(units$resNum)
    $sBox add entryfield units 	-labeltext "Units:"			\
    	-justify right							\
	-relief ridge						\
	-state disabled							\
	-textfont $_dhgDefaults(entryFont)				\
	-textvariable resStatus(units$resNum)

    [ winfo toplevel $parent ] add_short_help [ $sBox component units ]	\
	"The units the max, used, and available values are presented in."

    $statusObj link res${resNum}Max resStatus(max$resNum)
    $sBox add entryfield max -labeltext "Maximum:" 			\
	-justify right							\
	-relief ridge						\
	-state disabled							\
	-textfont $_dhgDefaults(entryFont)				\
	-textvariable resStatus(max$resNum)
    [ winfo toplevel $parent ] add_short_help [ $sBox component max ]	\
	"The maximum amount available of a resource."

    $statusObj link res${resNum}Used resStatus(used$resNum)
    $sBox add entryfield used -labeltext "Used:" 			\
	-justify right							\
	-relief ridge						\
	-state disabled							\
	-textfont $_dhgDefaults(entryFont)				\
	-textvariable resStatus(used$resNum)
    [ winfo toplevel $parent ] add_short_help [ $sBox component used ]	\
	"The current amount of a resource that is used."

    $sBox add entryfield avail -labeltext "Available:" 			\
	-justify right 							\
	-relief ridge							\
	-state disabled							\
	-textfont $_dhgDefaults(entryFont)				\
	-textvariable resStatus(avail$resNum)
    [ winfo toplevel $parent ] add_short_help [ $sBox component avail ]	\
	"The current amount of a resource that is available."

    $sBox add entryfield cap -labeltext "Capacity:" 			\
	-justify right 							\
	-relief ridge							\
	-state disabled							\
	-textfont $_dhgDefaults(entryFont)				\
	-textvariable resStatus(cap$resNum)
    [ winfo toplevel $parent ] add_short_help [ $sBox component cap ]	\
	"The current percentage used of a resource."

    bind $window <Button-3> "_helpInfo openUrl dhgResource.html#resourceWindow"



    #
    # Display the window.
    #

    pack $sBox -expand y -fill both
    $sBox align


    $window center $parent
    $window activate
    update idletasks

    return $window
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# displaySubsystem
#
# INVOCATION:
# displaySubsystem window subsystem version commandObject ?statusList? 
#       ?descriptionList? ?helpList?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	    (window path)   Window pathname for the subsystem window.
# (>) subsystem     (string)	    Subsystem identifier.
# (>) version	    (string)	    DHS Console version information.
# (>) commandObject (CDhsCommand)   CDhsCommand object.
# (>) ?statusList?  (list)	    List of status to display.
# (>) ?descList?    (list)	    Description of each status displayed.
# (>) ?helpList?    (list)	    Help information for each status.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display status information for 
# First line should be a summary not terminated with a period.
#
# DESCRIPTION:
# If the window already exists display it, otherwise create it.
# The following are created:
# 	1. ToplevelWidget window.
# 	2. Add Short help and a menubar to the toplevel window
# 	3. Add the menus to the menubar.
#	4. Add a "health" LED.
# 	5. Foreach status item monitored for the subsystem create an
#	   Entryfield with the value being monitored, except health
#	   descriptions which are comboboxes.
# 	6. Make sure the status items are aligned.
# 	7. Set-up context sensitive help.
# 	8. A toolbar.
#
# EXTERNAL VARIABLES:
# (string) dhgDir 	DHS GUI directory
# (array)  _dhgDefaults	Defines default colours, fonts, subsystem commands, 
#		        help messages, etc.
# (array) cmdStatus     Contains the values of all the Command Server status
#                       records
# (array) dtsStatus     Contains the values of all the Data Server status
#                       records
# (array) hisStatus     Contains the values of all the History Server status
#                       records
# (array) qlsStatus     Contains the values of all the Quick Look Server status
#                       records
# (array) staStatus     Contains the values of all the Status Server status
#                       records
# (array) stoStatus     Contains the values of all the Storage Server status
#                       records
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# createToolbar
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	displaySubsystem  {
    window 
    subsystem 
    version
    commandObject
    {statusList ""} 		
    {descList ""} 
    {helpMsgs ""} 
} {
    global ::dhgDir
    global ::_dhgDefaults
    global ::${subsystem}Status


    # 
    # The window already exists so do not construct it
    # just display it and return.
    #

    set upperSub [ string toupper $subsystem ]
    set lowerSub [ string tolower $subsystem ]
    if { [ winfo exists $window ] } {
	wm deiconify $window 
	raise $window
	return
    }

    
    #
    # If we are creating the DHS Data Server window, we need to
    # create an instanceo of the CDtsCommands class.
    #

    if { $upperSub == "DTS" } {
	uplevel #0 cDtsCommands _cDtsCommands 
	uplevel #0 _cDtsCommands cDtsCommandInit _dhgStatus _dhsConfig
    }


    #
    # If the a list was given use the one defined in the subsystem
    # array.
    #

    if { $statusList == "" } {
	set statusList "$_dhgDefaults($lowerSub,status)"
    }

    if { $descList == "" } {
	set descList "$_dhgDefaults($lowerSub,statusDesc)"
    }
				      
    if { $helpMsgs == "" } {
	set helpMsgs "$_dhgDefaults($lowerSub,help)"
    }
		

    #
    # Construct the toplevel widget.
    #

    util::TopLevelWidget $window					\
	-title "$_dhgDefaults($lowerSub,name)"  			\
	-withdraw 1
    
    
    #
    # Add short help and a menubar to the window.
    #

    $window make_short_help
    $window add_menubar


    #
    # Defined the window icon bitmap and icon name.
    #

    wm iconbitmap $window @$dhgDir/images/smallGemini.xbm 
    wm iconname $window $upperSub


    #
    # Construct the "Commands" menu.
    #

    set cmdMenu [ $window add_menubutton "Commands" left ]
    $window component commands configure -underline 0
    $window add_short_help [ $window component commands ] 		\
	{{bitmap b1} Commands menu: }
    commandMenu $window $cmdMenu $lowerSub $commandObject


    #
    # Construct the "Options" menu.
    #

    set optMenu [ $window add_menubutton "Options" left ]
    $window component options configure -underline 0
    $window add_short_help [ $window component options ] 		\
	{ { bitmap b1 } Options menu: }
    
    $window add_menuitem $optMenu command {Health Description Length}	\
       { {bitmap b1} Set the number of log messages kept.}		\
	-underline 0


    #
    # Construct the "Help" menu.
    #

    set helpMenu [ $window add_menubutton "Help" left ]
    $window component help configure -underline 0
    $window add_short_help [ $window component help ]	 		\
	{ {bitmap b1} Help menu: About DHS and extended help information.}
    $window add_menuitem $helpMenu command About			\
	{ {bitmap b1} Brief description about DHS.}			\
	-command "createAbout \"$version\""				\
	-underline 0
    set htmlPage "dhg[ string range $upperSub 0 0 ][ string range 	\
	$lowerSub 1 end ].html"	
    $window add_menuitem $helpMenu command Extended...			\
       { {bitmap b1} Detailed subsystem help information.}		\
	-command "_helpInfo openUrl $htmlPage"				\
	-underline 0


    #
    # Construct the "Close" menu.
    #

    set quitMenu [ $window add_menubutton Quit right ]
    $window component quit configure -underline 0
    $window add_short_help [ $window component quit ]	 		\
	"{bitmap b1} Quit menu: Remove the $_dhgDefaults($lowerSub,name)\
  window from the display."
    $window add_menuitem $quitMenu command "Close Window"		\
	"{bitmap b1} Remove the $_dhgDefaults($lowerSub,name) window from the display."	\
	-command "wm withdraw $window"					\
	-underline 0


    #
    # Add an led and title at the top of the window.
    #

    set top [ frame $window.top ]
    pack $top -side top -expand y -fill both

    
    cLed $top.led							\
	-colorassoc {{green good} {yellow warning} {red bad} {grey70 *}} \
	-ledvariable "${subsystem}Status(health)"		\
	-ledwidth 10						
    set ${lowerSub}Status(health) [ _dhgStatus value ${lowerSub}Health ]
    _dhgStatus link ${lowerSub}Health ${lowerSub}Status(health) 

    pack $top.led -side top -anchor nw -padx 15 -fill x -expand y

    set ledChild [ $top.led childsite ]
    label $ledChild.title 					\
        -justify center						\
        -text "$_dhgDefaults($lowerSub,name)'s Status Information"
    pack $ledChild.title -padx 15 -fill x -expand y


    #
    # Create a box to hold the status information.
    #

    cStatusBox $top.$lowerSub 						\
	-statusmargin 30
    $window add_short_help $top.$lowerSub 				\
	"Status information for $upperSub"


    #
    # Make sure there are descriptions and help information for
    # every status item.
    #

    if { [ llength $statusList ] != [ llength $descList ] } {
	destroy $window
	error "Error creating $upperSub window."
    }

    set i 0
    foreach status "$statusList" {
	if { "$status" == "Desc" } {
	    $top.$lowerSub add cLogMonitor $status _dhgStatus 		\
		${lowerSub}$status 					\
		-editable false						\
		-labeltext [ lindex $descList $i ]             		\
		-relief ridge						\
		-unique false
#		-textfont $_dhgDefaults(EntryFont)			

	    #
	    # Need to adjust OPtions menu
	    #

	    set logWin [ $top.$lowerSub component $status ]
	    $optMenu entryconfigure 1 					\
		-command "getLogLength $logWin \"$upperSub Health Description\""

	} else {
	    $top.$lowerSub add entryfield $status			\
		-labeltext [ lindex $descList $i ]             		\
		-relief ridge						\
		-state disabled						\
		-textfont $_dhgDefaults(entryFont)
	

	    #
	    # Associciate a variable with the Entryfield
	    #

	    _dhgStatus link ${lowerSub}$status ${lowerSub}Status($status)
	    $top.$lowerSub component $status configure 		\
		-textvariable ${lowerSub}Status($status)
	}


	#
	# Add short help message.
	#

	$window add_short_help [ $top.$lowerSub component $status ] 	\
		[ lindex $helpMsgs $i ]


	#
	# Add context-sensitvie help.
	#

	switch -- "$status" {
	    #	    Active { set htmlPage "dhgNumActive.html" } 
	    Debug { set htmlPage "dhgDebug.html" }
	    Desc { set htmlPage "dhgHealthDesc.html" }
	    Health { set htmlPage "dhgHealth.html" }
	    Inc { set htmlPage "dtsNumInc.html" }
#	    Num { set htmlPage "dhgNumJobs.html" }
#	    Over { set htmlPage "dhgOverflow.html" }
#	    Size { set htmlPage "dhgArraySize.html" }
	    Sim { set htmlPage "dhgSim.html" }
	    State { set htmlPage "dhgState.html" }
	    default {
		set htmlPage "dhg${status}.html#status"
	    }
	}

	bind  [ $top.$lowerSub component $status ] <Button-3> "_helpInfo\
 		openUrl \"${htmlPage}\""

	pack $top.$lowerSub -expand yes -fill both -side left
	incr i
    }


    #
    # Create a toolbar with all of the commands given in the commands menu
    #

    createToolbar ${lowerSub}Cmd $window 				\
	"$_dhgDefaults(${lowerSub},commandName)" 			\
	"$_dhgDefaults(${lowerSub},commands)"				\
	"$_dhgDefaults(${lowerSub},commandDesc)" 			\
	"$_dhgDefaults(${lowerSub},commandHelp)"  			\
	[ winfo  toplevel $window ] $commandObject
 

    #
    # Align the status boxes and disable the commands temporarily
    # for this subsystem.
    #

    $top.$lowerSub align
    if { [ lsearch [ $commandObject disabled ] $upperSub ] != -1 } {
	$commandObject disableWindow $upperSub $window
    } elseif { [ lsearch [ $commandObject disabled ] "DHS" ] != -1  } {
	$commandObject disableWindow DHS $window
    } else {
	$window configure -cursor {arrow black}
    }
    $window config -withdraw 0
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# getLogLength
#
# INVOCATION:
# getLogLength logMonitorObject name
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) logMonitorObject  (CLogMonitor)	Instance of log monitor class.
# (>) name		(string)	Name of what is being logged.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To change the number of messages retained, kept, by a paritcular
# logMonitorObject.
#
# DESCRIPTION:
# Prompt the user for the number of messages to be logged, get this
# value and change the number of messages being logged to this value.
#
# EXTERNAL VARIABLES:
# CLogMonitor
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

proc getLogLength {
    logMonitorObject
    name
} {
    #
    # Get the current number of messages and prompt the user for the
    # new value.
    #

    set currentLength [ $logMonitorObject cget -length ]
    set result [ inputDialog "Currently $currentLength messages are being kept.\n\n Please enter the number of messages to keep for $name?" "$currentLength"]

    
    #
    # Error check the input given and if it is valid set the number of
    # messages to be logged to the new value.
    #

    if { ![ isNumber "$result" ] && "$result" != ""} {
	errorDialog "Bad log length value `$result': should be a numerical value."
    } elseif { "$result" != "" } {
	$logMonitorObject configure -length "$result"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# simMode
#
# INVOCATION:
# simMode
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Determine if the DHS Console is running in simulate mode.
# 
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# ::simulate
#
# PRIOR REQUIREMENTS:
# ::simulate exits.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
proc simMode {
} {           
    global ::simulate
 
    if { "$simulate" == "FULL" || "$simulate" == "FAST" } {
        return 1
    } elseif { "$simulate" == "NONE" || "$simulate" == "VSM" } {
        return 0
    } else {
        return -1
    }
}

