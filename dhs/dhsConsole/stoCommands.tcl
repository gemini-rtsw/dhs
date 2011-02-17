# $Id: stoCommands.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsStoConsole/src/stgCommands.itk
#
# PURPOSE:
# To set-up all of the DHS commands needed for the STO console.
#
# CLASS NAME(S)
# CStoCommands		- Sets up the DHS service for issuing commands
#			  to the DHS Storage Server.
#
# METHOD NAME(S)
# PUBLIC
# CStoCommands::constructor		: CStoCommands class constructor.
# CStoCommands::cStoCommandInit		: Initilizes the CStoCommand object.
# CStoCommands::destructor		: CStoCommands class destructor.
# CStoCommands::stoCancelAction		: Issues a cancel action command.
# CStoCommands::stoInfo			: Displays action information.
# CStoCommands::stoInfoMonitor		: Command monitor for stoInfo cmd.
# CStoCommands::stoRefresh		: Executes the storage server
#					  stoRefresh command.
# CStoCommands::stoRequestAction	: Initiates an action.
# CStoCommands::stoRequestRollback	: Undoes an action.
#
# PRIVATE
# CStoCommands::getDevices		: Determines which devices are
#					  applicable for a requst.
# CStoCommands::infoHelp		: Displays the help for the info window.
# CStoCommands::refreshInfo		: Handles the refresh button on
#					  the info window.
#
#
# GLOBAL PROCEDURE(S)
# cStoCommands		Lower case access metho to the CStoCommands class.
#
# $Log: stoCommands.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.17  2000/01/05 20:35:04  jaeger
# Fixed a typo (withdrasy replaced with withdraw)
#
# Revision 1.16  1999/06/24 21:05:22  jaeger
# Fixed but in stoInfo command.  Didn't empty window on a Refresh.
#
# Revision 1.15  1999/06/18 16:19:05  jaeger
# Made the Default font a fixed width font.
#
# Revision 1.14  1999/06/16 17:13:19  jaeger
# Added "infoHelp" method which is responsible for dislplaying the short
# help messages for the stoInfo window.  Changed stoInfoMonitor to return
# quickly.  Command responses are lost if it doesn't.  Added getDevices
# method for retrieving the device selection list for writing.  Added
# "logicalDev" attribute to the stoRequestAction command.  Added "number"
# attribute to the stoRequestRollback command.  Made sure all methods have
# a "return" in them.  Made sure correct debuggin information is being
# printed.  Documented the code. Updated the pop-up window for the
# "stoRequestAction" and "stoRequestRollback" commands, they now ask the
# operator for the new attribute information.
#
# Revision 1.13  1999/02/25 03:14:34  jaeger
# Added stoRefresh command.
#
# Revision 1.12  1999/01/28 19:09:24  jaeger
# Fixed the "wait for window grab release" in the error monitor.
#
# Revision 1.11  1999/01/26 06:24:58  jaeger
# Added support for inserting default values in the pop-up windows.  Added
# some documentation.
#
# Revision 1.10  1998/08/14 21:53:29  jaeger
# Brought code documentation up to Gemini standards.  Delay processing
# of commands while a window has "grabbed" all X-events.
#
# Revision 1.9  1998/07/17 17:54:21  jaeger
# Removed stoErrorMonitor method since it wasn't used!  Fixed short help,
# contetext sensitive help, and all help button and menus to point to
# the correct pages.
#
# Revision 1.8  1998/06/19 22:44:31  jaeger
# Removed dependency on configuration file, but can still runa as a
# stand alone application.  Altered to use changes in CLogMonitor class.
# Changed the temporarily displayed request to request 8, less likely
# that it will actually be displayed.
#
# Revision 1.7  1998/06/15 16:34:30  jaeger
# Use CDhsCommands::confirmChoice method to confirm selection - eliminates
# code.  Added debugging information.
#
# Revision 1.6  1998/05/22 17:05:16  jaeger
# Allowed floating point values to be entered for RETRIEVAL Action Request.
# Shortened some of the error messages.
#
# Revision 1.5  1998/05/20 21:08:29  jaeger
# Reverted back to a single status acceptor.
#
# Revision 1.4  1998/05/15 16:55:45  jaeger
# Removed the reading of the configuration file for the command server
# information, this is inherited from the dhsCommand class instead.
#
# Revision 1.3  1998/05/06 20:45:49  jaeger
# Removed the grabs since they disabled all windows not just the
# Storage Server window - no fix for this yet.
#
# Revision 1.2  1998/05/04 20:58:34  jaeger
# Now inherits from the CDhsCommands class.  This allow the CStoCommands
# class to easily implement "busy" states during the stoRequestAction,
# stoCancelAction, and stoRequestRollbacks.  Now "disables" the info
# button when a "stoInfo" command is issued.  Changed initialization to
# include intialization of CDhsCommands class.  stoErrorMonitor is no
# longer used - instead the inherited errorMonitor method is used.
#
# Revision 1.1  1998/04/08 20:28:28  jaeger
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
# CStoCommands
#
# OPTIONS
# None.
#
# PUBLIC METHODS: 
# constructor		: CStoCommands class constructor.
# cStoCommandInit	: Initializes the CStoCommand object.
# destructor		: CStoCommands class destructor.
# stoCancelAction	: Issues a cancel action command.
# stoInfo		: Displays action information.
# stoInfoMonitor	: Command monitor for stoInfo cmd.
# stoRefresh		: Sends a "stoRefresh" command to the Storage Server.
# stoRequestAction	: Initiates an action.
# stoRequestRollback	: Undoes an action.
#
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To set-up a central place that issues the storage server commands.
#
#-
#***********************************************************************
#

class CStoCommands {
    inherit CDhsCommands

    #
    # Public methods and data
    #

    public method 	constructor {}
    public method	destructor {} {}

    public method	cStoCommandInit { statusObj configObj }
    public method   	stoCancelAction { win action requestNum }
    public method   	stoInfo { win type requestNum }
    public method	stoInfoMonitor { type mediaId mediaName name status 
				info }
    public method	stoRefresh { win }
    public method	stoRequestAction { win action requestNum }
    public method   	stoRequestRollback { win action requestNum }


    #
    # Protected methods and data  -- NONE
    #


    #
    # Private methods and data
    #

    private  method	getDevices { requestNum }
    private  method	infoHelp { toplevelWindow type table row recNum }
    private  method   	refreshInfo { type mediaId mediaName }

    private variable 	destination
    private variable	isInit 0
    private variable 	statusObject {}
    private common	stoInfoButton
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cStoCommands
#
# INVOCATION:
# cStoCommands objectName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) objectName	(string)	Name for the new CStoCommands object.
#
# PROCEDURE VALUE:
# objectName	(string) : The name of the new object that is created.
#
# PURPOSE:
# Lowercased access method to the CStoCommands class.
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
# CStoCommands::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc ::cStoCommands {
    objectName
} {
	uplevel ::CStoCommands $objectName
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# objectName (string) :  The name of the object that is created is returned.
#
# PURPOSE:
# Constructor for the CStoCommands class
#
# DESCRIPTION:
# Set-up a number of commands in the DHS service for the storage server.
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

body CStoCommands::constructor {
} {
    #
    # Create the DHS service and define the various private variables.
    # 

    service dhs


    #
    # Define the various storage server commands.
    #


    dhs cs stoCancelAction
    cs stoCancelAction add action 
    cs stoCancelAction set action mode in
    cs stoCancelAction add mediaId
    cs stoCancelAction set mediaId mode in
    if  { [ cs stoCancelAction verify ] == "failed" } {
	return [ list failed "stoCancelAction command verification failed." ]
    }

    dhs cs stoInfo
    cs stoInfo add type
    cs stoInfo set type mode in
    cs stoInfo add mediaId
    cs stoInfo set mediaId mode in
    cs stoInfo add stoDataHeader 
    cs stoInfo set stoDataHeader mode out
    cs stoInfo add stoData 
    cs stoInfo set stoData mode out
    if  { [ cs stoInfo verify ] == "failed" } {
	return [ list failed "stoInfo command verification failed." ]
    }

    dhs cs stoRefresh
    if  { [ cs stoRefresh verify ] == "failed" } {
	return [ list failed "stoRefresh command verification failed." ]
    }

    dhs cs stoRequestAction
    cs stoRequestAction add action
    cs stoRequestAction set action mode in
    cs stoRequestAction add mediaId 
    cs stoRequestAction set mediaId mode in
    cs stoRequestAction add size 
    cs stoRequestAction set size mode in
    cs stoRequestAction add force 
    cs stoRequestAction set force mode in
    cs stoRequestAction add number 
    cs stoRequestAction set number mode in
    cs stoRequestAction add logicalDev
    cs stoRequestAction set logicalDev mode in
    if  { [ cs stoRequestAction verify ] == "failed" } {
	return [ list failed "stoRequestAction command verification failed." ]
    }

    dhs cs stoRequestRollback
    cs stoRequestRollback add action
    cs stoRequestRollback set action mode in
    cs stoRequestRollback add mediaId 
    cs stoRequestRollback set mediaId mode in
    cs stoRequestRollback add number 
    cs stoRequestRollback set number mode in
    if  { [ cs stoRequestRollback verify ] == "failed" } {
	return [ list failed "stoRequestRollback command verification failed." ]
    }

    return "$this"
}

# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::cStoCommandInit
#
# INVOCATION:
# ObjectName cStoCommandinit statusObject configObj
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObj 	( CDhgStatus )	Stutus Object (acceptor).
# (>) CDhgConfigObj	( cConfigGen )	Gen configuration object.
#
# PROCEDURE VALUE:
# (boolean) 1: If initialiaztion is successful.
# (boolean) 0: If initialiaztion is unsuccessful.
#
# PURPOSE:
# To read the configuration file information and set the appropriate
# parameters.   Also to call the inherited init method 
# (CDhsCommands::cDhsCommandInit)
#
# DESCRIPTION:
# Initialize the inherited class CDhsCommands.  Make sure we have all
# the information from the configuration file.  Set the destination
# name for the commands.
#
# EXTERNAL VARIABLES:
# CStoCommands::hostname	(string)	Host where the command
#						server is running.
# CStoCommands::commandServer	(string)	Name assigned to the command
#						server.
# CStoCommands:statusObject	(CDhgStatus)    Instace of the status class.
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

body	CStoCommands::cStoCommandInit {
    statusObj
    configObject
} {
    #
    # Make sure the base class is initialzed.
    #

    set statusObject $statusObj

    cDhsCommandInit $statusObj $configObject

    if { $cmdHostname == "" } {
	error "Command Server cmdHostname not known."
    }

    if { $commandServer == "" } {
	error "Command Server identity not known."
    }


    #
    # Set the destination parameter for all of the commands.
    #

    set destination ${cmdHostname}:$commandServer

    cs stoCancelAction set destName value $destination
    cs stoInfo set destName value $destination
    cs stoRefresh set destName value $destination
    cs stoRequestAction set destName value $destination
    cs stoRequestRollback set destName value $destination

    return "[ set isInit 1 ]"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::stoCancelAction
#
# INVOCATION:
# objectName stoCancelAction win action requestNum		
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win	(pathname)	Storage Server window, which will be disabled.
# (>) action	(string)	Action that requires more info
# (>) requestNum(int)		Index in the request array.
# 
# PROCEDURE VALUE:
# (list) : {{Posted|Cancelled} message commandMonitor}
#
# PURPOSE:
# To cancel an action being processed or wating to be processed, based on
# a response from a user prompt.
#
# DESCRIPTION:
# Find out some of the information describing the request.  Then prompt
# the user, verifying the cancelling of the command.  If the command is
# to be cancelled then send the stoCancelRequest command to the Storage
# Server.
#
# EXTERNAL VARIABLES:
# CStoCommands::statusObject   (CDhgStatus) Instance of the status class.
#	      ::__errorMonitor (string)     error monitor being used.
#
# PRIOR REQUIREMENTS:
# CStocommands::statusObject exists
#
# SEE ALSO:
# CDhsCommands::confirmChoice.
# CDhsCommands::disableWindow
# CDhsCommands::errorMonitor
# CDhsCommands::printDebug
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStoCommands::stoCancelAction {
    win
    action		
    requestNum		
} {
    global ::__errorMonitor

    if { [ isFalse $isInit ] } {
	errorDialog "CStoCommand class must be initialized, $this\
 cStoCommandInit  has NOT been called."
	return [ list Error "$this has not been initialized." ]
    }

    set mediaId "[ $statusObject value stoReq${requestNum}ReqId ]"
    set name "[ $statusObject value stoReq${requestNum}ReqName ]"
    set action [ string toupper $action ]
    set msg "Do you really want to cancel the $action action for media\
 request ${name}, $mediaId?" 
    set args "action = $action mediaId = $mediaId"
   
    if { [  confirmChoice "STO" "$msg" stoCancelAction "$args" ] } {
	disableWindow STO $win
	cs stoCancelAction add action
	cs stoCancelAction set action value "$action"
	cs stoCancelAction add mediaId
	cs stoCancelAction set mediaId value "$mediaId"
	printDebug stoCancelAction "$args"
	set __errorMonitor "$this errorMonitor STO"
	cs stoCancelAction postCallback "$this errorMonitor STO"
	return { Posted "stoCancelAction posted." "[ code $this errorMonitor ]"}
    } else {
	return { Canceled "stoCancelAction not posted" "" }
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::stoInfo
#
# INVOCATION:
# objectName stoInfo win action requestNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win	(pathname)	Storage Server window, which will be disabled.
# (>) action	(string)	Action that requires more info
# (>) requestNum(int)		Index in the request array.
# 
# PROCEDURE VALUE:
# (list) : {{Posted|Cancelled} message commandMonitor}
#
# PURPOSE:
# To get information on a particular action and display it.
#
# DESCRIPTION:
# Get some information about the request.  Prompt the user.  If the
# user wants to view the information then send the stoInfo command
# to the Storage Server.  CStoCommands::stoInfoMonitor handles displaying
# the information.
#
# EXTERNAL VARIABLES:
# CStoCommands::statusObject   (CDhgStatus) Instace of the DHS Status class.
#	      ::__errorMonitor (string)     error monitor being used.
# CStoCommands::stoInfoButton  (array)	    window path to the info button.
#
# PRIOR REQUIREMENTS:
# CStocommands::statusObject exists
#
# SEE ALSO:
# CDhsCommands::confirmChoice.
# CDhsCommands::printDebug
# CStoCommands::stoInfoMonitor.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStoCommands::stoInfo {
    win
    type
    requestNum
} { 
    global ::__errorMonitor

    if { [ isFalse $isInit ] } {
	errorDialog "CStoCommand class must be initialized, $this\
 cStoCommandInit  has NOT been called."
	return [ list Error "$this has not been initialized." ]
    }

    set mediaName "[ $statusObject value stoReq${requestNum}ReqName ]"
    set type [ string toupper $type ]
    set mediaId "[ $statusObject value stoReq${requestNum}ReqId ]"

    set args "type = $type and mediaId = $mediaId"
    if { [ confirmChoice "STO" "" stoInfo "$args" ] } {
	set stoInfoButton(${mediaId},${type}) $win
	$win config -state disabled
	update idletasks

	cs stoInfo add type
	cs stoInfo set type [ string toupper $type ]

	cs stoInfo add mediaId 
	cs stoInfo set mediaId "$mediaId"

	cs stoInfo stoData init
	cs stoInfo stoDataHeader init

	set cmd "[ code $this stoInfoMonitor ] [ list $type ] 		\
		[ list $mediaId ] [ list  $mediaName ]"
	printDebug stoInfo "$args"
	set __errorMonitor "$cmd"
	cs stoInfo postCallback "$cmd"
	return [ list "Posted" "stoInfo $type $mediaId" "[ code $this stoInfoMonitor]" ]
    } else {
	return [ list "Not Posted" "stoInfo $type $mediaId" "" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::stoInfoMonitor
#
# INVOCATION:
# stoInfoMonitor type mediaId mediaName name status info
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) type	(string)	Action that requires more info
# (>) mediaId	(int)		Which request the action aplies to.
# (>) name	(string)	Command monitor
# (>) status	(string)	Status of the command
# (>) info	(string)	Informative message
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To catch any errors that occur in the stoInfo command and
# display the error message.  Also display the info (data) that
# is returned by the command upon successful completion.
#
# DESCRIPTION:
# If an error occurs display the error message.  If it is a "busy" message
# then ignore the message.  If it is an "idle" or "done" message then 
# display the results returned.
#
# EXTERNAL VARIABLES:
#	      ::__errorMonitor (string)     Error monitor being used.
# CStoCommands::stoInfoButton  (array)	    Window path to the info button.
# CDhsCommands::window	       (string)	    Name of the window that is disabled.
#					    In this case the storage server
#					    window should be disabled.
#
# PRIOR REQUIREMENTS:
# CStocommands::statusObject exists
#
# SEE ALSO:
# CDhsCommands::confirmChoice.
# CStoCommands::infoHelp
# CStoCommands::stoInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStoCommands::stoInfoMonitor {
    type
    mediaId
    mediaName
    name
    status
    info
} {
    global ::debug ::__errorMonitor


    #
    # Problems can occur if a modal window is displayed when commands
    # arrive, so we try and avoid this.
    #

    set grabbedWindow [ grab current ] 
    if { [ grab current ] != "" } {
	if { $window == "all" } {
	    set win ".f"
        } else {
	    set win $window
	}

	if { "$win" != "" && 
	     [ lsearch -exact $grabbedWindow $win ] == -1 } {
	    set cmd "[ code $this stoInfoMonitor ] [ list $type ]	\
		[ list $mediaId ] [ list $mediaName ] [ list $name ] 	\
		[ list $status ] [ list $info ]"
	    after 500 "$cmd"
	    return
	}
    }
 

    #
    # Display debugging details.
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
    # Return immediately if the status is busy.  There is nothing to
    # for a busy status.
    #

    if { $status == "busy" } { return }


    #
    # Handle any errors
    #

    if { $status == "error" } {
	errorDialog "$info"
	$stoInfoButton(${mediaId},${type}) configure -state normal
	return
    }

    
    #
    # Display the information returned.
    #

    set headings "[ cm $name get stoDataHeader ]"
    set data ""
    set data "[ cm $name get stoData ]"


    #
    # If the window already exists then empty it's contents.
    #

    set toplevelWindow 						\
	    "[ winfo toplevel $stoInfoButton(${mediaId},${type}) ]"
    set window "${toplevelWindow}.[ string tolower "$type" ]$mediaId"
    if { [ winfo exists $window ] } {
	$window component table deleteRow 0 end 
	set window $window
	$window config -headings [ list "$headings" ] 
    }


    #
    # If there is nothing to display then indicate so.
    #

    if { "$data" == "" } {
	if { [ winfo exists $window ] } {	
	    wm withdraw $window
	}
	infoDialog "No information to be shown, nothing to display."
	$stoInfoButton(${mediaId},${type}) configure -state normal
	return
    } 


    #
    # Create the window if it doesn't alear
    #

    if { ! [ winfo exists $window ] } {	
	set label "Media Request ${mediaName} ($mediaId)\n\
         $type Action Information"
	set font "-*-Courier-Medium-R-Normal-*-*-160-*-*-*-*-*-*" 
	set heading [ string trim $headings ]
	set window [ cArrayDisplay $window -cellbd4 		\
	         -columnwidths { 140 }					\
		 -headings [ list "$headings" ]				\
		 -hscrollmode dynamic					\
	  	 -justify left						\
		 -labelmargin 40					\
          	 -labeltext "$label"					\
		 -tablefont "$font"					\
		 -tableheight 400					\
		 -tablewidth 500 					\
		 -vscrollmode dynamic ]

	#
	# Add the buttons to the window.
	#

	set cmd "[ code $this refreshInfo ] [ list $type ]		\
		    [ list  $mediaId ] [ list $mediaName ]"
	$window add Ok -command "wm withdraw $window" -text "Ok"
	$window add Refresh 					\
	    -command "$cmd"						\
	    -text "Refresh"
	$window add Help 						\
	    -command "_stgHelpInfo openUrl dhgStoInfo.html#infoWindow" \
	    -text "Help"

	#
	# Add short help
	#

	$toplevelWindow add_short_help 				\
	    [ $window component bbox component Ok ]			\
	    { {bitmap b1} OK button: Remove the window for the display.}
	$toplevelWindow add_short_help 				\
	    [ $window component bbox component Refresh ]		\
	    { {bitmap b1} Refresh button: Refresh the display or redisplay.}
	$toplevelWindow add_short_help 				\
	    [ $window component bbox component Help ]		\
	    { {bitmap b1} Help button: Display detailed help information.}

	#
	# Add context-sensitive help
	#
	    
	bind $window  <Button-3>			\
	    "_stgHelpInfo openUrl dhgStoInfo.html#infoWindow"

    }


    #
    # Insert the text
    # 

    set tableWindow [ $window component table ]
    set idx 0
    foreach rowData [ split "$data" "\n" ] {
	$window component table addRow [ list $rowData ] 
	infoHelp $toplevelWindow $type $tableWindow $idx ""
	incr idx
    }


    #
    # Display the window
    #

    if { ! [ winfo ismapped $window ] } {
	$window activate
    }


    $stoInfoButton(${mediaId},${type}) configure -state normal
    $window configure -state normal

    cm $name release


    #
    # If the error monitor variable was set then unset it.
    #

    if { [ info exist __errorMonitor ] } {
	if { $__errorMonitor == "stoInfoMonitor" } {
	    unset __errorMonitor
	}
    }

    return
}

#
#***********************************************************************
# CStoCommands::stoRefresh
#
# INVOCATION:
# stoRefresh { win }
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win       (path name)     Window path name to be disabled.
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "stoRefresh" "commandMonitor"
#
# PURPOSE:
# To send the stoRefresh command to the Storage server, via the Command
# Server.
#
# DESCRIPTION:
# Display a dialogue verifying the exuction of the stoRefresh command.  If
# verified send the command to the Storage Server, via the Command Server.
#
# EXTERNAL VARIABLES:
#	      ::__errorMonitor (string)     error monitor being used.
#
# PRIOR REQUIREMENTS:
# CStocommands::statusObject exists
#
# SEE ALSO:
# CDhsCommands::confirmChoice
# CDhsCommands::disableWindow
# CDhsCommands::errorMonitor
# CDhsCommands::printDebug
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body  CStoCommands::stoRefresh {
    win
} {
    global ::__errorMonitor

    set msg "Do you wish to Refresh the Storage Server's display"
    
    if { [ confirmChoice "STO" "$msg" "stoRefresh" ] } {
	disableWindow STO $win
	printDebug stoRefresh ""
	set __errorMonitor "$this errorMonitor STO"
	cs stoRefresh postCallback "$this errorMonitor STO"
	return [ list Posted "stoRefresh" "[ code $this errorMonitor ]" ]
    } else {
	return [ list "Not Posted" "stoRefresh" "" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# stoRequestAction
#
# INVOCATION:
# objectName stoCancelAction win action requestNum		
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) win	(pathname)	Storage Server window, which will be disabled.
# (>) action	(string)	Action that requires more info
# (>) requestNum(int)		Index in the request array.
# 
# PROCEDURE VALUE:
# (list) : {{Posted|Cancelled} message commandMonitor}
#
# PURPOSE:
# To send a stoRequestAction command to the storage server.
#
# DESCRIPTION:
# Find out some of the information describing the request.  Then prompt
# the user, verifying the action and getting input (if necessary).
# If the action is to be performed then
# send the stoRequestAction command to the Storage Server.
#
# EXTERNAL VARIABLES:
# CStoCommands::statusObject   (CDhgStatus) Instance of the status class.
#	      ::__errorMonitor (string)     error monitor being used.
#
# PRIOR REQUIREMENTS:
# CStocommands::statusObject exists
#
# SEE ALSO:
# CDhsCommands::disableWindow
# CDhsCommands::errorMonitor
# CDhsCommands::enableWindow
# CDhsCommands::printDebug
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStoCommands::stoRequestAction {
    win
    action
    requestNum
} {

    global ::__errorMonitor

    #
    # Make sure class is initialized.
    #

    if { [ isFalse $isInit ] } {
	errorDialog "CStoCommand class must be initialized, $this\
 cStoCommandInit  has NOT been called."
	return [ list Error "$this has not been initialized." ]
    }


    #
    # Make sure commands are enabled.
    #

    if { [ lsearch $disabled "DHS" ] != -1 ||
	 [ lsearch $disabled "STO" ]  != -1 } {
	errorDialog "All commands are currently disabled."
	return [ list "Error" "stoRequesAction command is disabled." ]
    }


    set mediaId "[ $statusObject value stoReq${requestNum}ReqId ]"
    set mediaType "[ $statusObject value stoReq${requestNum}MediaType ]"
    set name "[ $statusObject value stoReq${requestNum}ReqName ]"

    cs stoRequestAction add size
    cs stoRequestAction add force
    cs stoRequestAction add number
    cs stoRequestAction add logicalDev

    cs stoRequestAction set size value \"\"
    cs stoRequestAction set force value \"\"
    cs stoRequestAction set number value \"\"
    cs stoRequestAction set logicalDev value \"\"
    
    set cancelled 0
    set action [ string toupper $action ]
    switch -exact -- "$action" {
	RETRIEVE {
	    set max "[ $statusObject value stoReq${requestNum}RetrieveSize ]"

	    set value(0) [ inputDialog "Please enter the megabytes of data \
that is to be Retrieved for media request ${name}." "$max" ]
	    set param(0) size

	    if { "$value(0)" == "" } {
		set cancelled 1
	    }
	}
	    
	QUEUE {
	    set max "[ $statusObject value stoReq${requestNum}QueueSize ]"
	    if { [ isNumber $max ] } {
		set max [ expr round( "$max" + 0.5 ) ]
	    }
	    set result [ queueDialog "$name" "$mediaId" "$max" "$win" ]

	    if { "$result" == "" } {
		set cancelled 1
	    }

	    set value(0) [ lindex $result 0 ]
	    set param(0) number

	    if { "[ lindex $result 1 ]" == "Not" } {
		set value(1) 0
	    } elseif { "[ lindex $result 1 ]" == "Forced" } {
		set value(1) 1
	    } else {
		set value(1) "[ lindex $result 1]"
	    }

	    set param(1) force
	}
	 

	PREPARE {
	    set max "[ $statusObject value stoReq${requestNum}PrepNum ]"

	    set value(0) [ inputDialog "Please enter the number of units\
 to be Prepared for media request ${name}" "$max"]
	    set param(0) number

	    if { "$value(0)" == "" } {
		set cancelled 1
	    }
	}


	WRITE {
	    set max "[ $statusObject value stoReq${requestNum}WriteNum ]"
	    set deviceList [ getDevices "$requestNum" ]
	    set result [ writeDialog "$name" "$mediaId" "$max" 		\
			     "$deviceList" "$win" ]

	    if { "$result" == "" } {
		set cancelled 1
	    }

	    set value(0) [ lindex $result 0 ]
	    set param(0) number

	    set value(1) [ lindex $result 1 ]
	    set param(1) logicalDev

	    if { "[ lindex $result 2 ]" == "Not" } {
		set value(2) 0
	    } elseif { "[ lindex $result 2 ]" == "Forced" } {
		set value(2) 1
	    } else {
		set value(2) "[ lindex $result 2]"
	    }
	    set param(2) force
	}


	CLEANUP {
	    set max "[ $statusObject value stoReq${requestNum}CleanupNum ]"
	    
	    set value(0) [ inputDialog "Please enter the number of\
 units to be Cleaned Up for media request ${name}.\n\nWarning:Data will be\
 permanently removed from retrieval and media staging areas." "$max"] 
	    set param(0) number

	    if { "$value(0)" == "" } {
		set cancelled 1
	    }
	}

	NOTIFY {
	    set answer [ confirmDialog "Are you sure you want to Notify \
the data server that media request ${name} has been completed?\n\n\
Warning: After notification data can NOT be recovered if data on media is \
corrupt." ]

	    if { [ isFalse "$answer" ] } {
		set error 1
	    } elseif { ! [ isBoolean $answer ] } {
		set error 1
		infoDialog "$action NOT performed: Unable to interpret\
 answer: $answer"
	    }
	}
	default {
	    #
	    # Error
	    # 

	    errorDialog "Unrecognized stoRequestAction actionr: $action"
	    set cancelled 1
	    break;		    
	}
    }

    if { [ isTrue $cancelled ] } {
	return [ list  Canceled "stoRequestAction not posted for media\
 ${name}, ${mediaId}, and action $action." "" ]
    }
    disableWindow STO $win

    
    #
    # Construct the command
    #

    cs stoRequestAction add action
    cs stoRequestAction set action value "$action"
    cs stoRequestAction add mediaId
    cs stoRequestAction set mediaId value "$mediaId"

    set args "action = $action, mediaId = $mediaId"
    set idx 0
    set error 0
    foreach idx [ array names param ] {
	set value($idx) "[ string trim $value($idx) ]"
	cs stoRequestAction add $param($idx)
	cs stoRequestAction set $param($idx) "$value($idx)"

	set args "${args}, $param($idx) = $value($idx)"

	switch -- "$param($idx)" {
	    logicalDev {
		#
		# Do Nothing  -- could check to see if it is a device name.
		#
	    }

	    force {
		if { ! [ isBoolean $value($idx) ] } {
		    infoDialog "$action NOT performed: $value($idx) is not a \
boolean value." "$win"
		    set error 1
		    break;
		}
	    }

	    size {
		if { ! [ isNumber $value($idx) ]  } {
		    infoDialog "$action NOT performed: $value($idx) is not a \
numerical value."
		    set error 1
		    break;
		} elseif { $value($idx) < -1 ||
		        $value($idx) > $max } {
		    infoDialog "$action NOT performed: $value($idx) is not a \
a positive numerical value less than $max."
		    set error 1
		    break;		    
		}
	    }
		
	    number {
		if { ! [ isInteger $value($idx) ] } {
		    infoDialog "$action NOT performed: $value($idx) is not an \
integer value."
		    set error 1
		    break;
		} elseif { $value($idx) <= 0 ||
			   $value($idx) > $max } {
		    infoDialog "$action NOT performed: $value($idx) is not a \
positive integer less than or equal to $max."
		    set error 1
		    break;		    
		}
	    }

	    default {
		#
		# ERROR
		#

		errorDialog "Unrecognized stoRequestAction parameter: $param"
		set error 1
		break;		    
	    }
	}
    }


    if { [ isTrue $error ] } {
	#
	# Bad input entered so do not process the command.
	#

	enableWindow STO
	return [ list  Canceled "stoRequestAction not posted for media\
 ${name}, ${mediaId}, and action $action." "" ]
    }

    printDebug stoRequestAction "$args"
    set __errorMonitor "$this errorMonitor STO"
    cs stoRequestAction postCallback "$this errorMonitor STO" "$this errorMonitor STO"
    return [ list  Posted "stoRequestAction $action $mediaId posted." "[ code $this errorMonitor]" ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::stoRequestRollback
#
# INVOCATION:
# Object stoRequestRollback action requestNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) action	   (string)	Action to rollback.
# (<) paramName (long) Descripton of the parameter
#
# PROCEDURE VALUE:
# (list) : "Posted|NotPosted" "stoRequestRollback" "commandMonitor"
#
# PURPOSE:
# To sned a stoRequestRollback command to the storage server.
#
# DESCRIPTION:
# Extract the required information to execute the command from the
# statusObject
#
# EXTERNAL VARIABLES:
# CStoCommands::statusObject   (CDhgStatus) Instance of the status class.
#	      ::__errorMonitor (string)     error monitor being used.
#
# PRIOR REQUIREMENTS:
# CStoCommands::statusObject exists
#
# SEE ALSO:
# CDhscommands::confirmChoice
# CDhsCommands::errorMonitor
# CDhsCommands::disableWindow
# CDhsCommands::printDebug
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
body	CStoCommands::stoRequestRollback {
    win
    action
    requestNum
} { 
    global ::__errorMonitor

    if { [ isFalse $isInit ] } {
	errorDialog "CStoCommand class must be initialized, $this\
 cStoCommandInit  has NOT been called."
	return [ list Error "$this has not been initialized." ]
    }

    set mediaId "[ $statusObject value stoReq${requestNum}ReqId ]"
    set name "[ $statusObject value stoReq${requestNum}ReqName ]"
    set action "[ string toupper $action ]"
    switch -exact -- $action {
	"QUEUE"   { 
	    set revAction "DEQUEUE" 
	    set max "[ $statusObject value stoReq${requestNum}PrepNum ]"
	}
	"PREPARE" { 
	    set revAction "UNPREPARE" 
	    set max "[ $statusObject value stoReq${requestNum}WriteNum ]"
	}
	"WRITE"   { set revAction "UNWRITE" 
	    set max "[ $statusObject value stoReq${requestNum}CleanupNum ]"
	}
	default   { 
	    set revAction "" 
	    set max 0
	}
    }
    
    set numUnits [ inputDialog "Please enter the number of media units \
to $revAction for media request ${name}." "$max" ]

    set doRollback 1
    if { "$numUnits" == "" } {
	set doRollback 0
    } elseif { ! [ isInteger $numUnits ] } {
	infoDialog "$revAction NOT performed: $numUnits is not an integer value."
	set doRollback 0
    } elseif { $numUnits <= 0 ||  $numUnits > $max } {
	infoDialog "$action NOT performed: $numUnits is not a positive \
integer less than or equal to $max."
	set doRollback 0
    }


    if { [ isFalse $doRollback ] } {
	return [ list  Canceled "stoRequestRollback not posted for media\
 ${name}, ${mediaId}, and rollback action $action." "" ]
    }


    disableWindow STO $win
    set args "action = $action, mediaId = $mediaId, number = $numUnits"
    cs stoRequestRollback add action
    cs stoRequestRollback set action value "$action"
    cs stoRequestRollback add mediaId
    cs stoRequestRollback set mediaId value "$mediaId"
    cs stoRequestRollback add number
    cs stoRequestRollback set number value "$numUnits"

    printDebug stoRequestAction "$args"
    set __errorMonitor "$this errorMonitor STO"
    cs stoRequestRollback postCallback "$this errorMonitor STO"

    return [ list Posted "stoRequestRollback $action $mediaId posted." "[code $this errorMonitor ]" ]
}


# ------------------------------------------------------------------
#                         PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::getDevices
#
# INVOCATION:
# getDevices requestNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) requestNum    (int) 
#
# PROCEDURE VALUE:
# deviceList (list) :
#
# PURPOSE:
# Determine which devices may be used to write a media units to a medium.
# The devices which are inuse or free for use are listed.
#
# DESCRIPTION:
# Loop through the list of devices extract those that are appropriate. 
#
# EXTERNAL VARIABLES:
# CStoCommands::statusObject   (CDhgStatus) Instance of the status class.
#
# PRIOR REQUIREMENTS:
# CStoCommands::statusObject exists
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStoCommands::getDevices {
    requestNum
} {
    set mediaType "[ $statusObject value stoReq${requestNum}MediaType ]"
    set mediaType "[ string trim $mediaType ]"
    set maxDevices "[ $statusObject value stoDeviceArraySize ]"
    set deviceList {}

    for { set idx 0 } { $idx < $maxDevices } { incr idx } {
	set devType "[ $statusObject value stoDevice${idx}Type ]"
	set devType "[ string trim "$devType" ]"
	set available "[ string trim [ $statusObject value 	\
		stoDevice${idx}Available ] ]"

	if { "$mediaType"  != 	"$devType" || [ isFalse $available ] } {
	    #
	    # Media type doesn't match or the device is not avaible so
	    # skip this one.
	    #

	    continue
	}
		
	set name "[ string trim [ $statusObject value 	\
		stoDevice${idx}LogicalName ] ]"
	set numDevices "[ string trim [ $statusObject value 	\
		stoDevice${idx}NumDevices ] ]"
	set devInUse "[ string trim [ $statusObject value 		\
		stoDevice${idx}DeviceInUse ] ]"
	lappend deviceList [ list "$name" "$numDevices" "$devInUse" ]
    }

    return "$deviceList"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::infoHelp
#
# INVOCATION:
# infoHelp toplevelWindow type table row recNum
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) toplevelWindow (string)	window pathname with short help display
# (>) type	     (string)	type of information being displayed.
# (>) table	     (string)	window pathname to the table displaying the
#				information.
# (>) row	     (integer) 	Which row in the table.
# (>) recNum	     (integer)	Which record is in the row.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To add short help descriptions to the stoInfo pop-up windows.  The
# short help descriptions should appear on the storage server's window.
#
# DESCRIPTION:
# Determine the type of data being displayed.  Based on the type set-up
# the appropriate short help messages.
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
# It would be nice to set-up the help on a column basis, however, for the
# time being the stoInfo returns rows of information, instead of columns.
#-
#***********************************************************************
#

body 	CStoCommands::infoHelp {
    toplevelWindow
    type
    table 
    row 
    recNum
} {
    set toplevelWindow [ winfo toplevel $toplevelWindow ]
    set rowPath [ $table getRowPath $row ]

    switch -- "$type" {
	RETRIEVABLE -
	QUEUEABLE {
	    $toplevelWindow add_short_help $rowPath			\
		{Unique file identifier.  File size in bytes. Current State of the file.}
#OMIT	    $toplevelWindow add_short_help [ $table getCellPath $row 0 ] \
		{Unique, file identifier.}
#OMIT	    $toplevelWindow add_short_help [ $table getCellPath $row 1 ] \
		{The size of the file in bytes.}
#OMIT	    $toplevelWindow add_short_help [ $table getCellPath $row 1 ] \
		{Current state/status of the file.}
	}

	PREPARABLE -
	WRITEABLE -
	CLEANUPABLE -
	NOTIFYABLE {
	    $toplevelWindow add_short_help $rowPath			\
		{Unique request identifier.  Name assigned to the unit. Size of the unit in Megabytes. State of the unit. Number of copies written.}
	    
#OMIT  	    $toplevelWindow add_short_help [ $table getCellPath $row 0 ] \
		{Unique, numerical request identifier.}
#OMIT	        $toplevelWindow add_short_help [ $table getCellPath $row 1 ] \
		{Name assigned to a media unit by the queuing actions.}
#OMIT	        $toplevelWindow add_short_help [ $table getCellPath $row 2 ] \
		{Number of Megabytes of data the media unit contains.}
#OMIT	        $toplevelWindow add_short_help [ $table getCellPath $row 3 ] \
	    	{Current state/status of the unit.}
#OMIT	        $toplevelWindow add_short_help [ $table getCellPath $row 4 ] \
	    	{Number of copies that have been written to a medium (CD-ROM).}
	}

	default {
	    error "Invalid stoInfo type: $type"
	}
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStoCommands::refreshInfo
#
# INVOCATION:
# refreshInfo type mediaId mediaName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) type	(string)	Action that requires more info
# (>) mediaId	(int)		Which request the action aplies to.
# (>) mediaName (string)	Name of the request the action aplies to.
#
# PROCEDURE VALUE:
# (list) : {{Posted|Cancelled} message commandMonitor}
#
# PURPOSE:
# To get information on a particular action and display it in the window
# where the refresh button was selected.
#
# DESCRIPTION:
# Prompt the user. Set-up the stoInfo command and issue it if so desired.
#
# EXTERNAL VARIABLES:
#	      ::__errorMonitor (string)     error monitor being used.
# CStoCommands::stoInfoButton  (array)	    window path to the info button.
#
# PRIOR REQUIREMENTS:
# CStoCommands::stoInfo has been called.
#
# SEE ALSO:
# CDhsCommands::confirmChoice.
# CDhsCommands::printDebug
# CStoCommands::stoInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStoCommands::refreshInfo {
    type
    mediaId
    mediaName
} {
    global ::__errorMonitor

    set toplevelWindow 						\
	[ winfo toplevel $stoInfoButton(${mediaId},${type}) ]
    set windowName "${toplevelWindow}.[ string tolower "$type" ]$mediaId"

    if { [ confirmChoice "STO" "" stoInfo "type = $type and mediaId = $mediaId" ] } {
	set win $stoInfoButton(${mediaId},${type})
	$win configure -state disabled
	$windowName configure -state disabled

	set args "$type = type mediaId=$mediaId"
	cs stoInfo add type
	cs stoInfo set type "$type"
	cs stoInfo add mediaId
	cs stoInfo set mediaId "$mediaId"
	cs stoInfo stoData init
	cs stoInfo stoDataHeader init
	
	set cmd "[ code $this stoInfoMonitor ] [ list $type ] 		\
		[ list $mediaId ] [ list $mediaName ]"
	printDebug stoInfo "$args"
	set __errorMontior "$cmd"
	cs stoInfo postCallback "$cmd"
	return { Posted "stoInfo posted." "[ code $this stoInfoMonitor ]" }
    } else {
	return { Canceled "stoInfo not posted" ""}
    }
}

