# $Id: dhsCommands.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/dhgCommands.tcl
#
# PURPOSE:
# To set-up all of the DHS commands needed for the DHS GUI.
#
# CLASS NAME(S)
# CDhsCommands			: Sets up the DHS service for issuing commands
#			  	  to the DHS Command Server.
#
# METHOD NAME(S)
# PUBLIC
# CDhsCommands::constructor	: Constructor for the CDhsCommands class
# CDhsCommands::destructor	: Destructor for the CDhsCommands class
# CDhsCommands::cDhsCommandInit	: Initizes the command class.
# CDhsCommands::dhsExit		: Handles exit command request
# CDhsCommands::debugLevel	: Handles debugLevel command request
# CDhsCommands::disabled	: True if commands are disabled.
# CDhsCommands::errorMonitor	: Called by OCSWish with the status of
#				  a commands execution.
# CDhsCommands::flush		: Handles flush command request
# CDhsCommands::hisLogWrite	: Handles hisLogWrite command request
# CDhsCommands::init		: Handles init command request
# CDhsCommands::ping		: Handles ping command request
# CDhsCommands::reset		: Handles reset command request
# CDhsCommands::resetHealth	: Handles resetHealth command request
# CDhsCommands::test		: Handles test command request
# CDhsCommands::simulateLevel	: Handles simulateLevel command request
#
# PROTECTED
# CDhsCommands::confirmChoice	: Confirms whether a command is to be
#				  executed.
# CDhsCommands::disableWindow	: Disables a window set all tag bindings to
#				  "" and stores the original value.
# CDhsCommands::enableWindow	: Enables a window, resets tag bindings.
# CDhsCommands::printDebug	: Prints out debbing information.
#
# PRIVATE
# CDhsCommands::cmdConfig	: Determines who and where the command
#				  Server is.			
# CDhsCommands::setBindTags	: Resets the window bindings after commands
#				  have been disabled.
# CDhsCommands::dsCommand	: Gets executed when a simulateLevel or
#				  debugLevel command is issued.
# CDhsCommands::subsystemConfig	: Gets the subsystems from the configuration
#				  file.
# 
#
# GLOBAL PROCEDURE(S)	
# cDhsCommands			: Lowercased access method to the 
#				  CDhsCommands class.
#
# $Log: dhsCommands.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.27  1999/06/16 23:23:15  jaeger
# Removed debugging code that is no longer needed.
#
# Revision 1.26  1999/06/08 05:15:29  jaeger
# Added setCursor method.  Updated documentation.
#
# Revision 1.25  1999/02/25 03:13:41  jaeger
# Added stoRefresh command.
#
# Revision 1.24  1999/01/28 19:08:48  jaeger
# Fixed the "wait for window grab release" in the error monitor.
#
# Revision 1.23  1998/08/14 21:49:21  jaeger
# Added subsystem to flush command.  errorMonitor waits until it can
# "grab all X-events" before processing command information.  Added
# full subsystem name to message dialogs.
#
# Revision 1.22  1998/07/20 19:40:05  jaeger
# moved bitmaps to "images" directory, code changed to reflect this.
#
# Revision 1.21  1998/07/16 19:56:11  jaeger
# Added code to show current simulation level on the dialogue window.
#
# Revision 1.20  1998/06/25 17:54:39  jaeger
# MMade debugging information that is displayed consistent.
#
# Revision 1.19  1998/06/19 22:42:00  jaeger
# Brought code documenation up to Gemini standards.  Also ensured
# that all commands return similar information.
#
# Revision 1.18  1998/06/15 16:25:58  jaeger
# Switched from cmdSystemExit to exit command.  Added a text box witht he
# current debug/simulate level on the pop-ups.  Added a method for confirming
# commands, elimiated lots of duplilcated code.  Send a 0 length string
# instead of all the subsystems known, for full system commands.  Removed
# the command completion pop-ups for storage server commands.
#
# Revision 1.17  1998/05/22 16:51:07  jaeger
# Fixed up ping command so it works like the others.  Will now print out
# command information when debugLevel is set to MIN or FULL, note that
# it still needs to print out what command it is sending when the debug
# level is set to one of these levels.
#
# Revision 1.16  1998/05/20 21:10:54  jaeger
# Error monitor now checks for "init" command instead of "Initialize".
# Disabled commands during a reset ie. "set disabled 1"
#
# Revision 1.15  1998/05/15 17:47:19  jaeger
# Added code for disabling windows during particular commands.  Removed
# data server commands, they are now in a separate class.  Added the
# ping command.
#
# Revision 1.14  1998/05/06 20:48:04  jaeger
# Added enableWindow and disableWindow which handle the disabling of
# window during particular commands.  Also added code to enable warnings
# in a CStgMain window.
#
# Revision 1.13  1998/05/04 20:45:55  jaeger
# Changed the name of the class, better name for Rose in case it is
# ever used.  Changed to read configuration file for the list of
# subsystems.  Monitoring of commands now allows some commands to
# "disable" the console and changes the cursor to a watch cursor, and
# can report when commands have completed. Fixed up code for "shut down"
# command.  Read the location and name of the command server from the
# configuration file.
#
# Revision 1.12  1998/04/15 22:39:58  jaeger
# Changed name to CDhsCommands, added error monitoring facilites.
# Altered code to reflect the new config class.
#
# Revision 1.11  1998/04/14 19:17:23  jaeger
# Made sure string variables were double quoted.
#
# Revision 1.10  1998/04/08 20:48:39  jaeger
# Made sure string values had double quotes around them.
#
# Revision 1.9  1998/03/18 22:39:45  jaeger
# Changed contructor args to take a configObject and a statusObject
# instead of assuming the existance of a particular configObject
# and statusObject.
#
# Revision 1.8  1998/03/06 18:02:28  jaeger
# did a cs var add before it is set so that the values are flushed.
# updated some of the error messages, more informative.
#
# Revision 1.7  1997/12/08 18:35:20  jaeger
# Fixed problem of getting a value from sim/debug window after it has
# been destroyed.
#
# Revision 1.6  1997/12/04 17:33:03  jaeger
# Fixed bugs in flush command.
#
# Revision 1.5  1997/12/04 17:17:28  jaeger
# Changed the dataset delete command so it waits to see if the dataset
# really gets deleted.
#
# Revision 1.4  1997/12/03 20:57:15  jaeger
# final revisiion, checked in for install
#
# Revision 1.3  1997/11/14 00:38:29  jaeger
# Add dialog boxes with commands
#
# Revision 1.2  1997/11/13 23:53:19  jaeger
# Checked in for install
#
# Revision 1.1  1997/10/30 19:24:39  jaeger
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
# CDhsCommands
#
# OPTIONS
# None.
#
# PUBLIC METHODS: 
# cDhsCommandInit	: Initializes the CDhsCommand class.
# cget          	: Standard cget method, returns the value of the 
#			  specified option.
# configure     	: Standard Tk method, sets or returns options.
# dhsExit		: Shut down entire DHS.
# disabled		: Returns true if commands are disabled.
# debugLevel		: Set the debug level of a subsystem or DHS.
# errorMonitor		: Command monitor, determines if a command executed
#			  with or without errors (intended to be called
#			  internally).
# flush			: Flush the status record values.
# hisLogWrite		: Write the DHS log message.
# init	       		: initialize DHS.
# isa           	: Sets the widget's class.
# reset			: reset DHS.
# resetHealth		: Set a subsystems Health back to GOOD.
# simulateLevel		: Set the debug level of a subsystem or DHS.
# test			: test DHS.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To set-up a central place that issues the DHS commands.
#
#-
#***********************************************************************
#

itcl::class CDhsCommands {
    #
    # Public methods and data
    #

    public method 	constructor {}
    public method	destructor {}

    public method 	cDhsCommandInit { statusObj conifgObj}
    public method 	debugLevel { subsystem }
    public method	dhsExit { win }
    public method	disabled {}
    public method	errorMonitor { subsystem name status info }
    public method       flush { {subsystem {}} }
    public method	hisLogWrite {}
    public method	init { window {subsystem {}} }
    public method	ping { {subsystem {}} }
    public method	reset { window {subsystem {}} }
    public method	resetHealth { {subsystem {}} }
    public method	simulateLevel { subsystem }
    public method	test { window {subsystem {}} }


    #
    # Protected methods and data
    #

    protected method	confirmChoice { subsystem msg cmd args }
    protected method	disableWindow { subsystem window }
    protected method	enableWindow { subsystem }
    protected method	printDebug { cmd args } {
	global ::debug
	if { $debug == "MIN" || $debug == "FULL" } {
	    if { $args == "" } {
		puts [ format "EXECUTING: `%s' command" $cmd ]
	    } else {
		puts [ format "EXECUTING: `%s' command with arguments:\n\t%s" \
			   $cmd "$args" ]
	    }
	} 
    }


    protected common disabled 0
    protected common window ""

    protected variable activeSubsystems {}	;#List of active DHS subsystems.
    protected variable commandServer ""
    protected variable cmdHostname ""
    protected variable dataServer ""
    protected variable dtsHostname ""


    #
    # Private methods and data
    #

    private method	cmdConfig { numTokens tokens }
    private method 	dsCommand { command subsystem }
    private method	setBindTags { win mode }
    private method	setCursor { win cursor } 
    private method	subsystemConfig { numTokens tokens }

    private variable bindTags
    private variable isInit 0
    private variable subsystemNames
    private variable statusObject

};



#
#***********************************************************************
#+
# PROCEDURE NAME:
# cDhsCommands
#
# INVOCATION:
# cDhsCommands name statusObj
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name	(string)	Name of the CDhsCommand object.
# (>) statusObj (stgStatus|dhgStatus) Name of either a stgStatus object
#					or a dhgStatus object.
#
# PROCEDURE VALUE:
# name - the name of the CDhsCommands object
#
# PURPOSE:
# Provide a lowercased access method for the CDhsCommands class.
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
# CDhsCommands::constructor
#
# DEFICIENCIES:
# None
#-
#***********************************************************************
#

proc cDhsCommands {
   name		   
} {
    uplevel ::CDhsCommands $name
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystemList (list)     List of the active DHS subsystems.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Constructor for the CDhsCommands class
#
# DESCRIPTION:
# Set-up a number of commands in the DHS service.
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
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

body CDhsCommands::constructor {
 } {
    #
    # Create the DHS service
    # 

    service dhs
    if { ![ winfo exists .f ] } {
	frame .f
    }

    #
    # Sequence Commnds 
    #

    dhs cs init
    cs init add subsystem
    cs init set subsystem mode in
    if { [ cs init verify ] == "failed" } {
	return [ list failed "Initialize command verification failed." ]
    }

    dhs cs ping
    cs ping add subsystem
    cs ping set subsystem mode in
    if { [ cs ping verify ] == "failed" } {
	return [ list failed "Ping command verification failed." ]
    }

    dhs cs reset 
    cs reset add subsystem
    cs reset set subsystem mode in
    if { [ cs reset verify ] == "failed" } {
	return [ list failed "Reset command verification failed." ]
    }

    dhs cs test
    cs test add subsystem
    cs test set subsystem mode in
    if { [ cs test verify ] == "failed" } {
	return [ list failed "Test command verification failed." ]
    }


    #
    # General DHS system control commands
    #
    
    dhs cs debugLevel
    cs debugLevel add level
    cs debugLevel set level mode in
    cs debugLevel add subsystem
    cs debugLevel set subsystem mode in
    if { [ cs debugLevel verify ] == "failed" } {
	return [ list failed "Debug Level command verification failed." ]
    }

    dhs cs exit
    cs exit add subsystem
    cs exit set subsystem mode in
    if { [ cs exit verify ] == "failed" } {
	return [ list failed "Exit command verification failed." ]
    }

    dhs cs flush
    cs flush add subsystem
    cs flush set subsystem mode in
    if { [ cs flush verify ] == "failed" } {
	return [ list failed "Flush command verification failed." ]
    }

    dhs cs resetHealth
    cs resetHealth add subsystem
    cs resetHealth set subsystem mode in
    if { [ cs resetHealth verify ] == "failed" } {
	return [ list failed "Rest Health command verification failed." ]
    }

    dhs cs simulateLevel
    cs simulateLevel add level
    cs simulateLevel set level mode in
    cs simulateLevel add subsystem
    cs simulateLevel set subsystem mode in
    if { [ cs simulateLevel verify ] == "failed" } {
	return [ list failed "Simulation Level command verification failed." ]
    }


    #
    # Public History Server Commands, created only if the
    # history server is active.
    #

     dhs cs hisLogWrite
     if { [ cs hisLogWrite verify ] == "failed" } {
	 return [ list failed " Write History Log command verification failed." ]
     }

    return "$this"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::destructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# Destructor for the "dhgStatus" class.  
#
# PURPOSE:
# Statement of purpose  
# First line should be a summary not terminated with a period.
#
# DESCRIPTION:
# Description of the algorithm.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhsCommands::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCommands::destructor {
} {
    return
}


# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::cDhsCommandInit
#
# INVOCATION:
# object cDhsCommandInit statusObj configObj
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObj	( cDhgStatus )	DHG status object.
# (>) configObj	( cConfigGen )	Gen configuration object.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To read the configuration file information and set the appropriate
# parameters.
#
# DESCRIPTION:
# Get the subsystem and command server information from the configuration
# file.  If no errors occured then set the destination name parameter
# for each command.
#
# EXTERNAL VARIABLES:
# CDhsCommands::hostname
# CDhsCommands::commandServer
# CDhsCommands::isInit
#
# PRIOR REQUIREMENTS:
# configOpen has been called.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCommands::cDhsCommandInit {
    statusObj
    configObject
} {
    set statusObject $statusObj


    #
    # Find the subsystem information.
    #

    if { [ $configObject configGet subsystem [ code $this 		\
	   subsystemConfig ] ] != "SUCCESS" } {
	error "Unable to read subsystem configuration information"
    }


    #
    # Find where the command server is located and set the destination
    # name for all of the commands.
    #

    if { [ $configObject configGet commandServer [ code $this 		\
         cmdConfig ] ] != "SUCCESS" } {
	error "Unable to read commandServer configuration information."
    }


    if { $cmdHostname == "" } {
	error "Command server hostname not known."
    }

    if { $commandServer == "" } {
	error "command Server identity not known."
    }



    if { [ lsearch $activeSubsystems "DTS" ] != -1 } {
	#
	# Find where the data server is located and set the destination
	# name for all of the commands.
	#

	if { [ $configObject configGet dataServer [ code $this 		\
		cmdConfig ] ] != "SUCCESS" } {
	    error "Unable to read Data Server configuration information."
	}


	if { $dtsHostname == "" } {
	    error "Data server hostname not known."
	}
	
	if { $dataServer == "" } {
	    error "Data Server identity not known."
	}
    }


    #
    # Set the destination parameter for all of the commands.
    #

    set destination ${cmdHostname}:$commandServer

    cs exit set destName value $destination
    cs debugLevel set destName value $destination
    cs flush set destName value $destination
    cs hisLogWrite set destName value $destination
    cs init set destName value $destination
    cs ping set destName value $destination
    cs reset set destName value $destination
    cs simulateLevel set destName value $destination
    cs test set destName value $destination
    cs resetHealth set destName value $destination

    set isInit 1

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::debugLevel
#
# INVOCATION:
# object debugLevel subsystem
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem (string) The subsystem that needs the debugLevel changed.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To execute the DHS debugLevel command, via the command server.
#
# DESCRIPTION:
# Get the new debug level, from a dialog box.  If this dialog box
# already exists then place it above the other window.  Execute the
# command if the correct button is selected from the dialouge, with
# the given debug level.
#
# Once the new debug level has been proveided send the DHS debugLevel
# command to the command server. 
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::dsCommands
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::debugLevel {
    subsystem
} {
    global ::dhsStatus ::_dhgDefaults

    if { [ isFalse $isInit ] } {
	errorDialog "CDhsCommand class must be initialized, $this\
 cDhsCommandInit  has NOT been called."
	return [ list "Not Posted" "debugLeveL" "[ code $this errorMonitor ]" ]
    }

    # 
    # Make sure the subsystem is an active subsystyem
    #

    set subsystem [ string toupper $subsystem ]
    foreach item $subsystem {
	if { [ lsearch $activeSubsystems $subsystem ] == -1 } {
	    if { "$subsystem" != "DHS"  } {
		errorDialog "Can not set Debug Level for subsystem the ${subsystem}, it is not active."
		return [ list "Not Posted" "debugLeveL" "[ code $this errorMonitor ]" ]
	    }
	}
    }


    #
    # Make sure commands are not disabled for the subsystem.
    #

    if { [ lsearch $disabled "DHS" ] != -1 ||
	 [ lsearch $disabled $subsystem ] != -1 } {
	errorDialog "All commands are currently disabled."
	return [ list "Not Posted" "debugLeveL" "[ code $this errorMonitor ]" ]
    }

    set debugWin ".debugLevel_[ string tolower $subsystem ]"
    if { [ winfo exists $debugWin  ] } {
	#
	# DebugWin already exists os make it viewable.
	#

	if { ![ winfo ismapped $debugWin  ] } {
	    wm deiconify $debugWin
	}
	raise $debugWin
	set childsite [ $debugWin childsite ]
    } else {
	#
	# DebugWin does not exist so create it.
	#

	set lowerSub [ string tolower $subsystem ]
        global ::${lowerSub}Status

	set name "$_dhgDefaults($lowerSub,name)"
	dialog $debugWin  -title "$name Debug Level"

	set childsite [ $debugWin childsite ]
	_dhgStatus link ${lowerSub}Debug ${lowerSub}Status(debug)

	iwidgets::entryfield $childsite.current					\
	    -labelpos w							\
	    -labeltext "Current Debug Level:"				\
	    -relief ridge						\
	    -textfont $_dhgDefaults(entryFont)				\
	    -textvariable ${lowerSub}Status(debug)
	    
	iwidgets::optionmenu $childsite.opts  				\
	    -labeltext "Selected Debug Level:"
 
        $childsite.opts insert end NONE MIN FULL

	set status "[ string tolower $subsystem ]Debug"
	set debugLevel "[ $statusObject value $status ]"
	if { [ lsearch {NONE MIN FULL} $debugLevel ] != -1 } {
	    $childsite.opts select $debugLevel
	} 

	pack $childsite.current -expand y -fill x -pady 10 -side top
	pack $childsite.opts -expand y -fill x -pady 3 -side top
	eval iwidgets::Labeledwidget::alignlabels [ list $childsite.current	\
	      $childsite.opts ]
    }
    

    #
    # Set up the button commands.
    #

    $debugWin buttonconfigure Apply -command 		\
        "cs debugLevel set level value \"\[ $childsite.opts get \] \"
	 [ code $this dsCommand debug $subsystem ]"
    $debugWin buttonconfigure Cancel -command 		\
	       "destroy $debugWin"
    $debugWin buttonconfigure OK -command			\
        "cs debugLevel set level value \"\[ $childsite.opts get \] \"
         destroy $debugWin
         [ code $this dsCommand debug $subsystem ]"
    $debugWin buttonconfigure Help -command		\
	    "_helpInfo openUrl dhgDebugCmd.html"

    if { ![ winfo ismapped $debugWin  ] } {
	$debugWin center
	$debugWin activate
    } else {
	raise $debugWin
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::dhsExit
#
# INVOCATION:
# object dhsExit
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win	(path name)	Window path name to be disabled.
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "exit" "commandMonitor"
#
# PURPOSE:
# To send the dhsExit command to the command server for 
# execution.  This command shuts-down the entire DHS.
#
# DESCRIPTION:
# Display a dialogue verifying the exuction of the exit command.  If
# verified send the command to the status server.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::dhsExit {
    win
} {
    global ::__errorMonitor

    set msg "Do you really want to SHUT DOWN the entire DHS? \n\nIt could have adverse affects."
    set args "subsystem = `'"

    if { [ confirmChoice "DHS" "$msg" exit "$args" ] } {
	set disabled 1
	disableWindow DHS $win

	cs exit add subsystem
	cs exit set subsystem value \"\"
	set __errorMonitor "[code $this errorMonitor DHS]"
	cs exit postCallback "[code $this errorMonitor DHS]"
	return [ list Posted "exit" "[ code $this errorMonitor ]" ]
    } else {
	return [ list "Not Posted" "exit" "[ code $this errorMonitor ]" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::disable
#
# INVOCATION:
# object disable
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# (boolean) TRUE  : If commands are diabled.
# (boolean) FASLE : If commands are not diabled.
#
# PURPOSE:
# To find out if commands are disabled.
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

body	CDhsCommands::disabled {
} {
    return $disabled
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::errorMonitor
#
# INVOCATION:
# object errorMonitor subsystem name status info
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem (string)	Subsystem that command is associated with.
# (>) name	(string)	Command monitor
# (>) status	(string)	Status of the command
# (>) info	(string)	Informative message
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To catch any errors that occur when executing the ICD 3 commansds, also
# to respond to successful completion of commands.
#
# DESCRIPTION:
# Test if the command succeeded or failed.  If it failed display and
# errorDialog with the error message.  If it passed check to see if
# it is one of the commands that reports success - if it is display
# a dialogue noting the commands success.
#
# Enable any windows that were disabled during the command.
#
# EXTERNAL VARIABLES:
# CDhsCommands::Window
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
#
# DEFICIENCIES:
# Timing problems may occur.  If command responses are sent before
# this method completes they will be lost.  Alternative implementation
# should be considered.  One which keeps a queue of commands received.
# This queue would be augmented each time a response is returned and
# processing is done based on the values in the queue.
#-
#***********************************************************************
#

body	CDhsCommands::errorMonitor {
    subsystem
    name
    status
    info
} {
    global ::debug ::__errorMonitor


    set cmd [ lindex [ split $name "-" ] 0 ]

    #
    # Problems can occur if a modal window is displayed when commands
    # arrive, so we try and avoid this.
    #

    set grabbedWindow [ grab current ] 
    if { $grabbedWindow != "" && $status != "busy" } {
	if { $window == "all" } {
	    set win ".f"
        } else {
	    set win $window
	}

	if { "$win" != "" && 
	     [ lsearch -exact $grabbedWindow $win ] == -1 } {
	    after 500 "[ code $this errorMonitor ] [ list $subsystem ] 	\
               [ list $name ] [ list $status ] [ list $info ]"
	    return
	}
    }


    #
    # Display debugging information.
    #

    if { $debug == "FULL" } {
	puts [ format "COMMAND: %-20s Status: %-10s\n\t %s\n" "$name"	\
		   "$status" "$info" ]
    }

    if { $status == "busy" && $debug == "MIN" } {
	puts [ format "COMMAND: %-20s Status: %-10s\n\t %s\n" "$name"	\
		   "$status" "$info" ]
    }


    #
    # Handle any errors.
    #

    if { $status == "error" } {
	if { $cmd != "" } {
	    cm $name release
	}
	enableWindow $subsystem


	#
	# If the command was "exit" make sure the DHS Console exits too.
	#

	if { "$cmd" == "exit" } {
	    set winPos [ lsearch [ winfo children . ] ".cdhgmain*" ]
	    if { $winPos != -1 } {
		errorDialog \{$status in $cmd command:\n$info\}
		delete object [ lindex [ winfo children . ] $winPos ]
		exit
	    }
	} else {
	    #
	    # Display the error message.
	    #

	    errorDialog "$status in $cmd command:\n$info"
	}
    }


    #
    # Handle successful completion of a command.
    #

    if { $status == "idle" } {
	if { "$cmd" == "flush" ||
	     "$cmd" == "hisLogWrite" ||
	     "$cmd" == "init" ||
	     "$cmd" == "ping" ||
	     "$cmd" == "reset" ||
	     "$cmd" == "resetHealth" ||
	     "$cmd" == "test" } {
	    #
	    # Display the "successful" command message and enable the window.
	    #

	    enableWindow $subsystem
	    cm $name release
	    infoDialog "$cmd command has completed successfully."
	} elseif { "$cmd" == "stoCancelAction" ||
		   "$cmd" == "stoRefresh" ||
		   "$cmd" == "stoRequestAction" ||
		   "$cmd" == "stoRequestRollback" } {
	    #
	    # Enable the storage server window.
	    #

	    enableWindow $subsystem
	    cm $name release
	} elseif { $debug == "MIN" } {
	    puts [ format "COMMAND: %-20s Status: %-10s\n\t %s\n" "$name" \
		   "$status" "$info" ]
	}
    }


    if { "$cmd" == "exit" && "$status" == "idle" } {
	#
	# Exit command so make the DHS Consol shutdown
	#
	
	set winPos [ lsearch [ winfo children . ] ".cdhgmain*" ]
	if { $winPos != -1 } {
	    after 3000 "delete object [ lindex [ winfo children . ] $winPos ]; cmdtrace on; exit"
	    # XXX allan: above exit (_ocsExit) hangs sometimes
	    #after 3000 "kill [pid]"
	}
    } 


    #
    # If the error monitor variable was set then unset it.
    #

    if { [ info exist __errorMonitor ] } {
	if { $__errorMonitor == "errorMonitor" } {
	    unset __errorMonitor
	}
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::flush
#
# INVOCATION:
# object flush subsystem
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem (string) DHS subsystem that needs flushing.
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "flush" "commandMonitor"
#
# PURPOSE:
# To execute the dHS flush command, which flushes the data buffers
# of a list of subsystems.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::flush {
    { subsystem {} }
} {
    global ::_dhgDefaults ::__errorMonitor

    if { $subsystem == "" } {
	set subsystem "DHS"
    }
    set subsystem [ string toupper $subsystem ]

    if { $subsystem == "DHS" } {
	set msg "Do you really want to FLUSH the entire DHS?"
	set args "subsystem = `'"
    } else {
	set name "$_dhgDefaults([ string tolower $subsystem ],name)"
	set msg "Do you really want to FLUSH the $name?"
	set args "subsystem = `$subsystem'"
    }


    if { [ confirmChoice "$subsystem" "$msg" flush "$args" ] } {
	#
	# Execute the flush command.
	#

	cs flush add subsystem
	if { $subsystem == "DHS" || $subsystem == "" } {
	    cs flush set subsystem value \"\"
	    set __errorMonitor "[code $this errorMonitor DHS]"
	    cs flush postCallback "[code $this errorMonitor DHS]"
	    return [ list  Posted "flush `'" "[ code $this errorMonitor ]" ]
	} else {
	    cs flush set subsystem value "$subsystem"
	    set __errorMonitor "[code $this errorMonitor $subsystem]"
	    cs flush postCallback "[code $this errorMonitor $subsystem]"
	    return [ list  Posted "flush `$subsystem'"  		\
		"[ code $this errorMonitor ]" ]
	}
    } else {
	#
	# Don't Execute the flush command.
	#

	if { $subsystem == "DHS" || $subsystem == "" } {
	    return [ list "Not Posted" "flush `'" "[ code $this errorMonitor ]" ]
	} else {
	    return [ list "Not Posted" "flush `$subsystem'" 		\
			"[ code $this errorMonitor ]" ]
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::hisLogWrite
#
# INVOCATION:
# object hisLogWrite
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "hisLogWrite" "commandMonitor"
#
# PURPOSE:
# To execute the DHS hisLogWrite command, which write the history
# log into the archive.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::hisLogWrite {
} {
    global ::__errorMonitor

    set msg "Do you really want to SAVE the History Log?"
	
    if { [ confirmChoice HIS "$msg" hisLogWrite ] } {
	set CDhsCommands::window ""
	set toolbarList {}
	set __errorMonitor "[code $this errorMonitor HIS]"
	cs hisLogWrite postCallback "[code $this errorMonitor HIS]"
	return [ list Posted "hisLogWrite" "[ code $this errorMonitor ]" ]
    } else {
	return [ list "Not Posted" "hisLogWrite" "[ code $this errorMonitor ]" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::init
#
# INVOCATION:
# object init
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUES:
# (list) : "Posted|NotPosted" "init ?args?" "commandMonitor"
#
# PURPOSE:
# Issue an init command to DHS.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::init {
    window
    { subsystem {} }
} {
    global ::_dhgDefaults ::__errorMonitor


    if { $subsystem == "" } {
	set subsystem "DHS"
    }
    set subsystem [ string toupper $subsystem ]

    if { $subsystem == "DHS" } {
	set msg "Do you really want to INITIALIZE the entire DHS?\n\nWARNING:\
 data may be lost."
	set args "subsystem = `'"
    } else {
	set name "$_dhgDefaults([ string tolower $subsystem ],name)"
	set msg "Do you really want to INITIALIZE the $name?\n\nWARNING:\
 data may be lost."
	set args "subsystem = `$subsystem'"
    }


    if { [ confirmChoice "$subsystem" "$msg" init "$args" ] } {
	#
	# Execute the initialize command.
	#

	set disabled 1
	disableWindow $subsystem $window

	cs init add subsystem
	if { $subsystem == "DHS" || $subsystem == "" } {
	    cs init set subsystem value \"\"
	    set __errorMonitor "[code $this errorMonitor DHS]"
	    cs init postCallback "[code $this errorMonitor DHS]"
	    return [ list Posted "init `'" "[ code $this errorMonitor ]" ]
	 } else {
	     cs init set subsystem value "$subsystem"
	     set __errorMonitor "[code $this errorMonitor $subsystem]"
	     cs init postCallback "[code $this errorMonitor $subsystem]"
	     return [ list Posted "init `$subsystem'" 		\
			"[ code $this errorMonitor ]" ] 
	 }
    } else {
	if { $subsystem == "DHS" || $subsystem == "" } {
	    return [ list "Not Posted" "init `'" [ code $this errorMonitor ]" ]
	} else {
	    return [ list "Not Posted" "init `$subsystem'" 	\
		[ code $this errorMonitor ]" ]
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::ping
#
# INVOCATION:
# object ping
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUES:
# (list) : "Posted|NotPosted" "ping ?args?" "commandMonitor"
#
# PURPOSE:
# Issue a ping command to DHS.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::ping {
    { subsystem {} }
} {
    global ::_dhgDefaults ::__errorMonitor

    if { $subsystem == "" } {
	set subsystem "DHS"
    }
    set subsystem [ string toupper $subsystem ]

    if { $subsystem == "DHS" } {
	set msg "Do you really want to PING all of the DHS subsystems"
	set args "subsystem = `'"
    } else {
	set name "$_dhgDefaults([ string tolower $subsystem ],name)"
	set msg "Do you really want to PING the $name"
	set args "subsystem = `$subsystem'"
    }

    if { [ confirmChoice "$subsystem" "$msg" ping "$args" ] } {
	#
	# Execute the ping command.
	#

	cs ping add subsystem
	if { $subsystem == "DHS" || $subsystem == "" } {
	    cs ping set subsystem value \"\"
	    set __errorMonitor "[code $this errorMonitor DHS]"
	    cs ping postCallback "[code $this errorMonitor DHS]"
	    return [ list  Posted "ping `'" "[ code $this errorMonitor ]" ]
	} else {
	    cs ping set subsystem value "$subsystem"
	    set __errorMonitor "[code $this errorMonitor $subsystem]"
	    cs ping postCallback "[code $this errorMonitor $subsystem]"
	    return [ list  Posted "ping `$subsystem'"  		\
		"[ code $this errorMonitor ]" ]
	}
    } else {
	#
	# Don't Execute the ping command.
	#

	if { $subsystem == "DHS" || $subsystem == "" } {
	    return [ list "Not Posted" "ping `'" "[ code $this errorMonitor ]" ]
	} else {
	    return [ list "Not Posted" "ping `$subsystem'" 		\
			"[ code $this errorMonitor ]" ]
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::reset
#
# INVOCATION:
# object reset
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUES:
# (list) : "Posted|NotPosted" "reset ?args?" "commandMonitor"
#
# PURPOSE:
# Issue an init command to DHS.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::reset {
    window
    { subsystem {} }
} {
    global ::_dhgDefaults ::__errorMonitor

    set subsystem [ string toupper $subsystem ]
    if { $subsystem == "" } {
	set subsystem "DHS"
    }

    if { $subsystem == "DHS" } {
	set msg "Do you really want to RESET the entire DHS?\n\nWARNING:\
 data may be lost."
	set args "subsystem = `'"
    } else {
	set name "$_dhgDefaults([ string tolower $subsystem ],name)"
	set msg "Do you really want to RESET the $name?\n\nWARNING:\
 data may be lost."
	set args "subsystem = `$subsystem'"
    }

    if { [ confirmChoice "$subsystem" "$msg" reset "$args" ] } {
	#
	# Execute the reset command.
	#

	set disabled 1
	disableWindow $subsystem $window

	cs reset add subsystem
	if { $subsystem == "DHS" || $subsystem == "" } {
	    cs reset set subsystem value \"\"
	    set __errorMonitor "[code $this errorMonitor DHS]"
	    cs reset postCallback "[code $this errorMonitor DHS]"
	    return [ list  Posted "reset `'" "[ code $this errorMonitor ]" ]
	} else {
	    cs reset set subsystem value "$subsystem"
	    set __errorMonitor "[code $this errorMonitor $subsystem]"
	    cs reset postCallback "[code $this errorMonitor $subsystem]"
	    return [ list Posted "reset `$subsystem'" "[ code $this errorMonitor ]" ]
	}
    } else {
	#
	# Don't Execute the reset command.
	#

	if { $subsystem == "DHS" || $subsystem == "" } {
	    return [ list "Not Posted" "reset `'" "[ code $this errorMonitor ]" ]
	} else {
	    return [ list "Not Posted" "reset `$subsystem'" 		\
			"[ code $this errorMonitor ]" ]
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# dhgCommand::resetHealth
#
# INVOCATION:
# object resetHealth subsystem
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem (string) The subsystem whose health is to be reset.
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "resetHealth ?args?" "commandMonitor"
#
# PURPOSE:
# To execute the DHS resetHealth command, which resets a subsystems Health.  
# Changes the subsystems health to "GOOD"
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::resetHealth { {subsystem {}} } {
    global ::_dhgDefaults ::__errorMonitor


    #
    # Some variable initialization
    # 

    if { $subsystem == "" } {
	set subsystem "DHS"
    }
    set subsystem [ string toupper $subsystem ]

    
    if { $subsystem == "DHS" } {
	set msg "Do you really want to RESET the HEALTH of all the DHS subsystems?"
	set args "subsystem = `'"
    } else {
	set name "$_dhgDefaults([ string tolower $subsystem ],name)"
	set msg "Do you really want to Reset the Health of the $name?"
	set args "subsystem = `$subsystem'"
    }



    if { [ confirmChoice "$subsystem" "$msg" resetHealth "$args" ] } {
	#
	# Execute the resetHealth command.
	#

	set CDhsCommands::window ""
	set toolbarList {}
	cs resetHealth add subsystem
	if { $subsystem == "DHS" || $subsystem == "" } {
	    cs resetHealth set subsystem value \"\"
	    cs resetHealth postCallbak "$this errorMonitor DHS"
	    set __errorMonitor "[code $this errorMonitor DHS reset-]"
	    return [ list  Posted "resetHealth `'" "[ code $this errorMonitor ]" ]
	} else {
	    cs resetHealth set subsystem value $subsystem	
	    cs resetHealth postCallbak "$this errorMonitor $subsystem"
	    set __errorMonitor "[code $this errorMonitor $subsystem reset-]"
	    return [ list  Posted "resetHealth `$subsystem'" 		\
			"[ code $this errorMonitor ]" ]
	}
    } else {
	#
	# Don't Execute the resetHealth command.
	#

	if { $subsystem == "DHS" || $subsystem == "" } {
	    return [ list "Not Posted" "resetHealth `'" 		\
			"[ code $this errorMonitor ]" ]
	} else {
	    return [ list "Not Posted" "resetHealth `$subsystem'" 	\
			"[ code $this errorMonitor ]" ]
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::simulateLevel
#
# INVOCATION:
# object simulateLevel subsystem
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem (string) The subsystem that needs the simulateLevel changed.
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "simulateLevel ?args?" "commandMonitor"
#
# PURPOSE:
# To execute the DHS simulateLevel command, via the command server.
#
# DESCRIPTION:
# Get the new simulation level, from a dialog box.  If this dialog box
# already exists then place it above the other window.
#
# Once the new simulation level has been proveided send the DHS 
# simulateLevel command to the command server. 
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::dsCommand
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::simulateLevel {
    subsystem 
} {
    global ::_dhgDefaults

    set upperSub [ string toupper $subsystem ]
    set lowerSub [ string tolower $subsystem ]

    if { [ isFalse $isInit ] } {
	errorDialog "CDhsCommand class must be initialized, $this\
 cDhsCommandInit has NOT been called."
        return [ list "Not Posted" "simulateLevel" 		\
		"[ code $this errorMonitor ]" ]
    }


    # 
    # Make sure the subsystem is an active subsystyem
    #

    set subsystem [ string toupper $subsystem ]
    foreach item $subsystem {
	if { [ lsearch $activeSubsystems $item ] == -1 } {
	    if { "$subsystem" != "DHS"  } {
		errorDialog "Can not set Debug Level for subsystem\
 ${upperSub}, it is not active."
	        return [ list "Not Posted" "simulateLevel" 		\
			"[ code $this errorMonitor ]" ]
	    }
	}
    }

    if { [ lsearch $disabled "DHS" ] != -1 ||
	 [ lsearch $disabled "$upperSub" ] != -1 } {
	errorDialog "Simulate Level command is currently disabled for subsystem $upperSub."
	return [ list "Not Posted" "simulateLevel" "[ code $this errorMonitor ]" ]
    }


    set simulateWin .simLevel_$lowerSub
    if { [ winfo exists $simulateWin ] } {
	#
	# SimulateWin already exists os make it viewable.
	#

	if { ![ winfo ismapped $simulateWin  ] } {
	    wm deiconify $simulateWin
	}
	raise $simulateWin
	set childsite [ $simulateWin childsite ]
    } else {
	#
	# SimulateWin does not exist so create it.
	#

	set name "$_dhgDefaults($lowerSub,name)"
	dialog $simulateWin -title "$name Simulation Level"

	set childsite [ $simulateWin childsite ]
	_dhgStatus link ${lowerSub}Sim ${lowerSub}Status(sim)
	
	iwidgets::entryfield $childsite.current					\
	    -labelpos w							\
	    -labeltext "Current Simulate Level:"			\
	    -relief ridge						\
	    -textfont $_dhgDefaults(entryFont)				\
	    -textvariable ${lowerSub}Status(sim)

	optionmenu $childsite.opts  					\
	    -labeltext "Selected Simulate Level:"

	$childsite.opts insert end NONE VSM FAST FULL

	pack $childsite.current -expand y -fill x -pady 10 -side top
	pack $childsite.opts -expand y -fill x -pady 3 -side top
	eval iwidgets::Labeledwidget::alignlabels [ list $childsite.current	\
	      $childsite.opts ]
    }

    set simLevel "[ $statusObject value ${lowerSub}Sim ]"
    if { [ lsearch {NONE VSM FAST FULL} $simLevel ] != -1 } {
	$childsite.opts select $simLevel
    }
	

    #
    # Set up the button commands.
    #

    $simulateWin buttonconfigure Apply -command 		\
        "cs simulateLevel set level value \"\[ $childsite.opts get \] \"
	[ code $this dsCommand simulate $subsystem ]"
    $simulateWin buttonconfigure Cancel 			\
	-command "destroy $simulateWin"
    $simulateWin buttonconfigure OK -command			\
        "cs simulateLevel set level value \"\[ $childsite.opts get \] \"
	 destroy $simulateWin
	 [ code $this dsCommand simulate $subsystem ]"
    $simulateWin buttonconfigure Help -command		\
	"_helpInfo openUrl dhgSimCmd.html"

    if { ! [ winfo ismapped $simulateWin  ] } {
	$simulateWin center
	$simulateWin activate
    } else {
	raise $simulateWin
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::test
#
# INVOCATION:
# object test
#
# where object is an instance of the CDhsCommands class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUES:
# (list) : "Posted|NotPosted" "test ?args?" "commandMonitor"
#
# PURPOSE:
# Issue an init command to DHS.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# CDhsCommands::cDhsCommandInit must be called first.
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhsCommands::test {
    window
    { subsystem {} }
} {
    global ::_dhgDefaults ::__errorMonitor

    if { $subsystem == "" } {
	set subsystem "DHS"
    }
    set subsystem [ string toupper $subsystem ]

    if { $subsystem == "DHS" } {
	set msg "Do you really want to TEST the whole DHS?"
	set args "subsystem = `'"
    } else {
	set name "$_dhgDefaults([ string tolower $subsystem ],name)"
	set msg "Do you really want to TEST the $name?"
	set args "subsystem = `$subsystem'"
    }
    
    if { [ confirmChoice "$subsystem" "$msg" test "$args" ] } {
	#
	# Execute the test command.
	#
	
	set disabled 1
	disableWindow $subsystem $window

	cs test add subsystem
	if { $subsystem == "DHS" || $subsystem == "" } {
	    cs test set subsystem value \"\"
	    set __errorMonitor "[code $this errorMonitor DHS]"
	    cs test postCallback "[code $this errorMonitor DHS]"
	    return [ list  Posted "test `'" "[ code $this errorMonitor ]" ]
	 } else {
	     cs test set subsystem value "$subsystem"
	     set __errorMonitor "[code $this errorMonitor $subsystem]"
	     cs test postCallback "[code $this errorMonitor $subsystem]"
	    return [ list  Posted "test `$subsystem'" "[ code $this errorMonitor ]" ]
	 }
    } else {
	if { $subsystem == "DHS" } {
	    return [ list "Not Posted" "test `'" "[ code $this errorMonitor ]" ]
	} else {
	    return [ list "Not Posted" "test `$subsystem'" 		\
		"[ code $this errorMonitor ]" ]
	}
    }
}

# ------------------------------------------------------------------
#                         PROTECTED METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::confirmChoice
#
# INVOCATION:
# confirmChoice subsystem msg cmd args
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) subsystem	(string)	Name of the subsystem the command will
#			        be issued to.
# (>) msg	(string)	The message to be displayed in the pop-up.
# (>) cmd	(string)	The command to be issued if the choice
#				is confirmed.
# (>) ?args?	(string)	The arguments and values that are given
#				to the command.
#
# PROCEDURE VALUE:
# (boolean) 1: If the command is to be issued.
# (boolean) 0: If the command is not to be issued.
#
# PURPOSE:
# To confirm the executions of a particular command.
#
# DESCRIPTION:
# If the subsystem has not been disabled then prompt the user (if msg is
# not "") to see if they wish to have the command executed.  If the command
# it to be executed then pring the debuging messages.
#
# EXTERNAL VARIABLES:
#             ::debug
# CDhsCommands::activeSubsystems
# CDhsCommands::disabled
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# confirmDialog
# errorDialog
# CDhsCommands::printDebug
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCommands::confirmChoice {
    subsystem
    msg
    cmd
    {args ""}
} {
    global ::debug


    # 
    # Make sure the subsystem is an active subsystyem
    #

    set subsystem [ string toupper $subsystem ]
    foreach item $subsystem {
	if { [ lsearch $activeSubsystems $item ] == -1 } {
	    if { $item != "DHS" } {
		errorDialog "Can not $cmd subsystem ${subsystem},\
 it is not active."
		return 0
	    }
	}
    }

    
    #
    # Make sure commands can be sent to the subsystem.
    #

    if { [ lsearch $disabled "DHS" ] != -1 ||
	 [ lsearch $disabled $subsystem ] != -1 } {
	errorDialog "Commands are currently disabled for $subsystem."
	return 0
    }

    
    #
    # Ping the subsystem(s) to make sure they are working.
    #

#OMIT    cs ping add subsystem 
    if { $subsystem == "DHS" } {
#	cs ping set subsystem value "$activeSubsystems"
#	if { [ lindex [ cs ping postWait ] 0 ] != "idle" } {}
#	    errorDialog "At least on of $activeSubsystem subsystem are not responding."
#	    return 0
#{	}
    } else {
#	cs ping set subsystem value "$subsystem"
#	if { [ lindex [ cs ping postWait ] 0 ] != "idle" } {}
#	    errorDialog "$subsystem is not responding."
#	    return 0
#{	}
#END OMIT
    }


    #
    #  Confirm the execution of the command with a pop-up dialouge
    #

    if { "$msg" == "" } { 
	set result 1
    } else {
	set result [ confirmDialog "$msg" ]
    }


    if { [ isTrue "$result" ]  } {
	printDebug "$cmd" "$args"
	return 1
    } else {
	return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::disableWindow
#
# INVOCATION:
# CHdsComands::disableWindow window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name) 	The window that is not to receive any events.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To prevent a window from getting any events.
#
# DESCRIPTION:
# If the subsystem given is DHS then disable all windows, otherwise only
# disable the subsystem window and any of its children.  Windows are disabled
# by changing the cursor to the the hourglass cursor and unsetting the 
# tags each window is bound to.  The tags are remembered in order to enable
# them later.
#
# EXTERNAL VARIABLES:
# CDhsCommands::disabled.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhsCommands::bindTags
# CDhsCommands::setCursor
# CDhsCommands::enableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCommands::disableWindow {
    subsystem
    win
} {
    #
    # Add the subsystem to be disabled to the list of disabled subsystems.
    #
    
    if { [ lsearch $disabled [ string toupper $subsystem ] ] != -1 } {
	lappend disabled $subsystem
    }


    #
    # If it is the Stroage Server disable warnings.
    #

    if { [ winfo class [ winfo toplevel $win ] ] == "CStgMain" } {
	[ winfo toplevel $win ] disableWarnings
    }


    #
    #  Disable the subsystem window (if the subsystem is DHS then all 
    #  windows).
    #
	
    if { [ string toupper "$subsystem" ] == "DHS" } {
	foreach child [ winfo children . ] {
	    setCursor $child hourglass
	}
	set CDhsCommands::window "all"
	if { [ grab status .f ] == "none" } {
	    grab set  .f
	}
    } else {
	set CDhsCommands::window "[ winfo toplevel $win ]"
	setCursor [ winfo toplevel $win ] hourglass
	setBindTags [ winfo toplevel $win ] disable
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::enableWindow
#
# INVOCATION:
# enableWindow
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To enable a window's ability to capture events.
#
# DESCRIPTION:
# Allow a subsystem or DHS (all window's) to receive X window events.  This
# is done by changing the cursor to an arrow and by re-binding the tags
# for the subsystem window(s) and their children.  The subsystem is remove
# from the disabled list once it has been enabled.
#
# EXTERNAL VARIABLES:
# CDhsCommands::disabled.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhsCommands::bindTags
# CDhsCommands::setCursor
# CDhsCommands::disableWindow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCommands::enableWindow {
    subsystem
} {
    #
    # Enable all of the desired windows.
    #

    if { "$window" == "all" } {
	foreach win [ grab current ] {
	    grab release $win
	}
	foreach child [ winfo children . ] {
	    setCursor $child arrow
	}
    } elseif { "$window" != "" }  {
	if { [ winfo class $window ] == "CStgMain" } {
	    $window enableWarnings
	}
	setBindTags $window enable
	setCursor $window arrow
	set window ""
    }


    #
    # Remove the susbystem for the disabled list.
    #

    if { [ set pos [ lsearch $disabled [ string toupper $subsystem ] ] ]
	 != -1 } {
	set disabled [ lreplace $diabled $pos $pos ]
    }
    

    return
}

# ------------------------------------------------------------------
#                         PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::cmdConfig
#
# INVOCATION:
# cmdConfig numTokens tokens
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens	(int)	Number of tokens to parse.
# (>) tokens	(list)	List containing the tokens found on a line in
#				the configuration file.	
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To interpret a line from the configruation file and set class
# variables appropriately.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CStoCommands::commandServer
# CStoCommands::cmdHostname
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

body	CDhsCommands::cmdConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "commandServer" } {
	if { $numTokens != 3 } {
	    error "Configuration file error, expect 3 tokens for commandServer\
 configuration but got $numTokens."
	}

	set cmdHostname [ lindex $tokens 2 ]
	set commandServer [ lindex $tokens 1 ]
    } elseif { [ lindex $tokens 0 ] == "dataServer" } {
	if { $numTokens != 3 } {
	    error "Configuration file error, expect 3 tokens for dataServer\
 configuration but got $numTokens."
	}

	set dtsHostname [ lindex $tokens 2 ]
	set dataServer [ lindex $tokens 1 ]
    } else {
	error "Expected configuration information for commandServer\
or dataServer not [ lindex $tokens 0 ]"
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::dsCommand
#
# INVOCATION:
# dsCommand command subsystem
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) command	(string)    Issue debug or simulate command.
# (>) subsystem	(string)    Which subsystem(s) to issue command on.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To set-up the debugLevel or simulateLevel parameter values   The
# command can then be "posted".   This method is used in order to
# delay the parameter setting to when the "OK" or "Apply" button 
# is pressed.  This is important, and necessary, to do when there
# are more than one debugLevel, and/or simulateLevel windows.
#
# DESCRIPTION:
# 1. If the subsystem is "DHS" the command is to be executed on
#	all active subsystems.
# 2. Determine if the command is "simualte" of "debug" and set
#	the appropriate "subsystem" and "level" parameter values.
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
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

body	CDhsCommands::dsCommand {
    command
    subsystem
} {
    global ::debug ::__errorMonitor

    if { $subsystem  == "DHS" || $subsystem == "" } {
	set value ""
    } else {
	set value "$subsystem"
    }
    if { $command == "debug" } {
	cs debugLevel add subsystem
	if { $value == "" } {
	    cs debugLevel set subsystem value \"\"
	} else {
	    cs debugLevel set subsystem value "$value"
	}
	set __errorMonitor "[code $this errorMonitor $subsystem]"


	#
	# Print out debugging information.
	#

	set level "[ cs debugLevel get level value ]"
	set args "subsystem = `$value' and level = `$level'"
	printDebug debugLevel "$args"

	
	#
	# Post the command
	#

	cs debugLevel postCallback "[code $this errorMonitor $subsystem]"
	if { $value == "" } {
	    return [ list  Posted "debugLevel `' `$level'" 		\
			 "[ code $this errorMonitor ]" ]
	} else {
	    return [ list  Posted "debugLevel `$value' `$level'" 	\
			 "[ code $this errorMonitor ]" ]
	}
    } elseif { $command == "simulate" } {
	cs simulateLevel add subsystem
	if { $value == "" } {
	    cs simulateLevel set subsystem value \"\"
	} else {
	    cs simulateLevel set subsystem value "$value"
	}
	set __errorMonitor "[code $this errorMonitor $subsystem]"


	#
	# Print out debugging information.
	#

	set level "[ cs simulateLevel get level value ]"
	set args "subsystem = `$value' and level = `$level'"
	printDebug simulateLevel "$args"

	
	#
	# Post the command
	#

	cs simulateLevel postCallback "[code $this errorMonitor $subsystem]"
	if { $value == "" } {
	    return [ list  Posted "simulateLevel `' `$level'" 		\
			 "[ code $this errorMonitor ]" ]
	} else {
	    return [ list  Posted "simulateLevel `$value' `$level'" 	\
			 "[ code $this errorMonitor ]" ]
	}
    } else {
	error "Command \"$command\" is not recognized."
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::setBindTags
#
# INVOCATION:
# setBindTags win
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win	(path name)	Path name of window whose FocusIn binding
# 				is to be alted.
# (>) mode	(enable|disable) Indicates whether the window is allowd
#				 to have the focus.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To disallow/allow a particular window to have the focus.
#
# DESCRIPTION:
# Depending on the mode set the windows bindtags to either diable
# the window or enable it.
#
# This method is called recursively to set/unset the tags bound to
# each child of the given window.
#
# EXTERNAL VARIABLES:
# CDhsCommands::bindTags	array listing windows and there tags
#				before disabling.
#
# PRIOR REQUIREMENTS:
# The window must exist.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCommands::setBindTags {
    win
    mode
} {
    set mode [ string toupper $mode ] 
    set class [ string tolower [ winfo class $win ] ]
    set toplevel [ winfo toplevel $win ]

    switch -exact -- $mode {
	DISABLE {
	    set children [ winfo children $win ]
	    foreach child [ winfo children $win ] {
		set bindTags($child) [ bindtags $child ]
		if { [ lsearch $bindTags($child) $toplevel ] != -1 } {
		    bindtags $child [ list $toplevel all ]
		} else {
		    bindtags $child [ list all ]
		}
		setBindTags $child $mode
	    }
	    if { $class == "toplevelwidget" ||
		 $class == "toplevel" } {
		set bindTags($win) [ bindtags $win ]
		bindtags $win [ list all ]
	    }
	}
	ENABLE {
	    foreach child [ winfo children $win ] {
		if { [ info exist bindTags($child) ] } {
		    bindtags $child  $bindTags($child) 
		    setBindTags $child $mode
		}
	    }
	    if { ( $class == "toplevelwidget" ||
		   $class == "toplevel" ) &&
		 [ info exist bindTags($win) ] } {
		bindtags $win $bindTags($win)
	    }
	}
	default {
	    error "Unrecognized window mode: $mode"
	}
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommands::subsystemConfig
#
# INVOCATION:
# subsystemConfig numTokens tokens
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens	(int)	Number of tokens to parse.
# (>) tokens	(list)	List containing the tokens found on a line in
#				the configuration file.	
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To interpret a line from the configruation file and set class
# variable appropriately.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CStoCommands::activeSubsystems
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

body	CDhsCommands::subsystemConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "subsystem" } {
	error "Expected to read subsystem information from config file\
 not [ lindex $tokens 0 ]"
    }
    
    if { $numTokens != 2 } {
	error "Configuration file error, expect 2 tokens for subsystem\
 configuration but got $numTokens."
    }
    lappend activeSubsystems [ string toupper [ lindex $tokens 1 ] ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCommandsConfig::setCursor
#
# INVOCATION:
# setCursor win cursor
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win	(string)	Valid window path name.
# (>) cursor	(string)	Name of the cursor to use.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To set the cursor style which is to be used when the cursor is in the
# given window.
#
# DESCRIPTION:
# Set the given windows cursor then recursively call to set all of
# the window's children.  The hourglass cursor is a special cursor known
# to this application only.
#
# EXTERNAL VARIABLES:
# ::dhgDir	The location of the DHS GUI Tcl source.
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

body	CDhsCommands::setCursor { 
    win  
    cursor
} {
    global ::dhgDir

    if { "[ winfo toplevel $win ]" == "$win" } {
	if { $cursor == "hourglass" } {
	    $win configure -cursor "@$dhgDir/images/hourglass.xbm $dhgDir/images/hourglassmsk.xbm black white"
	} else {
	    $win configure -cursor "$cursor"
	}
    }

    foreach child [ winfo children $win ] {
	setCursor $child $cursor
    }

    return
}
