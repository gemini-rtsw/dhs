# $Id: stgMain.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsStoConsole/tcl/stgMain.itk
#
# PURPOSE:
# Create the main window of the STO console.
#
# CLASS NAME(S)
# CStgMain			: Class that creates the main window of the 
#			 	  Storage Server subsystem window.
#
# METHOD NAME(S)
# PUBLIC
# CStgMain::constructor		: Constructor for the CStgMain class.
# CStgMain::destructor		: Destructor for the CStgMain class.
# CStgMain::disableWarnings	: Disables warning messages.
# CStgMain::enableWarnings	: Enables warning messages.
#
# PRIVATEN
# CStgMain::appNameConfig	: Determine the application name, if
#				  running as a standalone application.
# CStgMain::createMenus		: Create the menus and menubar
# CStgMain::createStoStatus	: Create the status overview section
# CStgMain::createReqInfo	: Create the request overview section
# CStgMain::getConfigInfo	: Gets the configuration information,
#				  if running stand alone.
# CStgMain::initActionButtons	: Initialize the request buttons.
# CStgMain::repackLabels	: Make sure request labels are aligned.
# CStgMain::setupStatusMon		: Set up the monitoring of status records.
# CStgMain::showRequest		: Called when a request has either just
#				  come into use or is no longer in use.
# CStgMain::updateActionButtons	: Called when either a request's action 
#				  state or action value changes and update
#				  display accordingly.
# CStgMain::updateActionUnit	: Called when a request's action value
#				  changes, and update the button display.
# CStgMain::updateActionState	: Called when a request's action state 
#				  changes, and update the button display.
# CStgMain::versionConfig	: Get the version information from the 
#    				  configuration file - only if standalone.
#
# GLOBAL PROCEDURE(S)
# cStgMain			: Lowercased access to CStgMain class
#
# $Log: stgMain.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.19  1999/06/16 22:22:10  jaeger
# Updated code documentation.  Removed many of the references to EPICS and
# changed it to "status".  Made sure all methods have a "return" statement.
# Added device support.  Changed the "Quit" menu to "Close".  "Geminized" the
# status record naming.  If warning are disabled then the messages are
# written to standard out.  Now does not warn when request data are
# empty strings.  Added state "ROLLBACK REQUESTED".
#
# Revision 1.18  1999/02/27 00:26:09  jaeger
# Fixed refresh command, "f" is underlined instead of "r" on the menu.
#
# Revision 1.17  1999/02/01 19:55:28  jaeger
# Delay displaying of Netscape until help is selected.
#
# Revision 1.16  1999/01/28 19:10:13  jaeger
# When adding a new request warnings are only enabled if they were
# enabled originally.
#
# Revision 1.15  1998/08/14 21:52:02  jaeger
# Change "displayReqInfo" to "displayRequestInfo"
#
# Revision 1.14  1998/07/20 19:39:51  jaeger
# moved bitmaps to "images" directory, code changed to reflect this.
#
# Revision 1.13  1998/07/16 20:04:07  jaeger
# Added context sensitive help and short help information.
#
# Revision 1.12  1998/06/25 20:14:57  jaeger
# Changed to reflect changes in the led, progressbar, and statusBox classes.
# Brought code documentation up to Gemini standards.
#
# Revision 1.11  1998/06/19 22:52:11  jaeger
# Removed dependency on configuration file, but can still run as a
# stand alone application.  Altered to use changes in CLogMonitor class.
# Changed the temporarily displayed request to request 8, less likely
# that it will actually be displayed.
#
# Revision 1.10  1998/06/15 16:31:15  jaeger
# Added subsystem health LED.  Altered start-up logic slightly, temporarily
# show a request (if none are shown) so the labels get aligned properly.
#
# Revision 1.9  1998/05/25 20:09:15  jaeger
# Changed the look of the entry boxes (relief is now ridge) so user's
# won't be tempted to edit them.
#
# Revision 1.8  1998/05/22 16:48:30  jaeger
# Print out which config file is being read when debug level is set to
# MIN or FULL only.  Used to be always.
#
# Revision 1.7  1998/05/20 21:08:52  jaeger
# Switched to a single status acceptor (single instance of the CDhgStatus class.)
#
# Revision 1.6  1998/05/20 19:08:44  jaeger
# Added proper deletion of objects in the destructor.
#
# Revision 1.5  1998/05/15 16:57:26  jaeger
# Made changes to monitoring of status items, due to changes in the
# CDhgStatus class for using a status acceptor in the dhs service.
# Added a drop down menu for keeping a "log" for the health description.
#
# Revision 1.4  1998/05/06 20:46:41  jaeger
# Added a display warnings flag to temporarily turn them off during a reset
# command and during start-up.  Also added a couple of methods so
# outsiders (command class) can change these values.
#
# Revision 1.3  1998/05/04 20:54:11  jaeger
# Made modifications to allow the console to be executed from the DHS
# Console.  Changed how EPICS was handled due to difference in the
# dhgStatus class.
#
# Fixed destructor so it deletes all objects that are created by the
# class.  Changed from subsystem global variable to _stgDefaults.
# Changes made to reflect changes in both the CDhsCommands and CStoCommands
# classes.  Use create toolbar utility instead of the createToolbar method.
#
# Revision 1.2  1998/04/14 19:47:30  jaeger
# Fixed after commands.
#
# Revision 1.1  1998/04/08 21:07:12  jaeger
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
# CStgMain
#
# PUBLIC METHODS: 
# constructor		: Constructor for the CStgMain class.
# destructor		: Destructor for the CStgMain class.
# disableWarnings	: Disables warning messages.
# enableWarnings	: Enables warning messages.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Creates the main window of the DHS Storage Server subsystem window.
#
#-
#***********************************************************************
#

itk::usual CStgMain {}
class CStgMain {
    inherit util::TopLevelWidget

    #
    # Public methods and data
    #

    public method	constructor { {statusObj {}} {configObj {}} }
    public method	destructor {}

    public method	disableWarnings {} { set displayWarnings 0 }
    public method	enableWarnings {} { set displayWarnings 1 }


    #
    # Protected methods and data -- NONE
    #

	
    #
    # Private methods and data
    #

    private variable appName
    private variable actionList {Retrieve Queue Prep Write Cleanup Notify}
    private variable actionUnitList {Size Size Num Num Num Value}
    private variable configObject ""
    private variable displayWarnings 0
    private variable statusObject
    private variable shortVersion ""
    private variable standAlone	0
    private variable repackPending ""
    private variable requestButtons
    private variable visibleRequests {}


    private method 	appNameConfig { numTokens tokens }
    private method 	createMenus { window }
    private method 	createStoStatus { window }
    private method 	createReqInfo { window }
    private method	getConfigInfo { configObj }
    private method 	initActionButtons {}
    private method	repackLabels { { when later } }
    private method 	setupStatusMon { {statusObj {}} }
    private method	showRequest { name alarm time valueList }
    private method	showWarning { name alarm time valueList } {
	warningDialog "[ $statusObject value stoAlert ]"
    }


    private method	updateActionButtons { prevState nextState \
						  nextUnit name valueList }
    private method 	updateActionUnit { name alarm time valueList }
    private method 	updateActionState { name alarm time valueList }
    private method 	versionConfig { numTokens tokens }
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cStgMain
#
# INVOCATION:
# cStgMain pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)    A window path name for a nonexistent window.
# (>) options   (string)        One or more valid option/value pairs.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lowercased acces method to the CStgMain class.
#
# DESCRIPTION:
# If the object alread exists then display the window, otherwise
# create it.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStgMain
# CStgMain::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc cStgMain { 
    pathName 
    { statusObj {} }
    { configObj {} }
} {
    if { [ winfo exists $pathName ] } {
	if { ![ winfo ismapped $pathName ] } {
	    wm deiconify $pathName 
	}
	raise $pathName
    } else {
	uplevel CStgMain $pathName $statusObj $configObj
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObj 	(CDhgStatus)	Instance of CDhgStatus class.
# (>) configObj (CConfigGen)	Instance of CConfigGen class.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# constructor for the CStgMain class.
#
# DESCRIPTION:
# Setup status record monitoring and get configuration file information,
# if standalone application.  Create the short help, menubars, status
# overview, request overview and toolbars.
# Initialize the buttons and align the status labels.
#
# EXTERNAL VARIABLES:
# ::debug	(boolean)   Current debug level.
# ::stgDir 	(string)    DHS GUI directory
# ::stoStatus	(array)     Array containing STO's status information.
# ::_helpDir	(string)    Directory that contains all of the HTML files.
# ::_stgDefaults	
#		(array)	    defines all of the commands, help messages,
#		            and display status for each DHS (sub)system.
# ::_stgHelp	(CHelp)	    Instance of the CHelp class.
# CStgMain::configObject
#		(CConfigGen) Instance of the CConfigGen class.
# CStgMain::displayWarnings (boolean)   TRUE indicates warnings are displayed.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStgMain::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CStgMain::constructor {
    {statusObj {}}
    {configObj {}}
} {
    global ::dhgDir 
    global ::_stgDefaults 
    global ::_helpDir 
    global ::debug
    global ::stoStatus


    if { $statusObj == "" && $configObj == "" } {
	set standAlone 1
    }

    wm withdraw $itk_component(hull)


    #
    # Get configuration information
    #

    getConfigInfo $configObj


    #
    # Set up the status monitoring and DHS Commands.
    #
    # NOTE: _cStoCommands and _cStoCommands are added to the global 
    # scope.  These should probably be passed as arguments, where
    # needed.
    #

    setupStatusMon $statusObj

    uplevel #0 cStoCommands _cStoCommands
    _cStoCommands cStoCommandInit "$statusObject" $configObject


    #
    # Add short help to the toplevel widget.
    #

    $this make_short_help
    

    #
    # Create the menubar and menus.
    #

    $this add_menubar
    createMenus $this

    
    #
    # Make the main diplay area scrollable.
    #
	    
    itk_component add scrolledFrame {
	iwidgets::scrolledframe $itk_interior.sfFrame -height 580 		\
	    -hscrollmode dynamic					\
	    -vscrollmode dynamic 					\
	    -width 540
    } {
	keep -background -cursor -foreground 
    }
    pack $itk_component(scrolledFrame) -expand y -fill both
    set sfChildsite [ $itk_component(scrolledFrame) childsite ]


    #
    # Create the title
    #

    itk_component add title { 
	cLed $sfChildsite.led						\
	    -colorassoc {{green good} {yellow warning} {red bad} {grey70 *}} \
	    -ledvariable "stoStatus(health)"			\
	    -ledwidth 10						
    } {
	keep -background -cursor
    }
    set stoStatus(health) [ $statusObject value stoHealth ]
    $statusObject link stoHealth stoStatus(health)

    pack $sfChildsite.led -side top -anchor nw -padx 15 -fill x -expand y
	

    set ledChild [ $sfChildsite.led childsite ]
    label $ledChild.title 						\
	-justify center							\
        -text "Storage Server's Status Information"
    pack $ledChild.title -padx 15 -fill x -expand y


    #
    # Create the DHS status display area.
    #

    createStoStatus $sfChildsite.status
    createToolbar stoCmd $sfChildsite $_stgDefaults(sto,commandName) 	\
	$_stgDefaults(sto,commands) $_stgDefaults(sto,commandDesc)	\
	$_stgDefaults(sto,commandHelp) $this _cStoCommands


    #
    # Now that the health log window exists we need to configure it.
    #

    set optMenu $itk_component(menubar).options.m
    set logWin [ $sfChildsite.status component Desc ]
    $optMenu entryconfigure 1 						\
	-command "getLogLength $logWin {Storage Server Health Description}"


    #
    # Put separator 
    #
 
    itk_component add separator1 {
	frame $sfChildsite.separator1 					\
	    -height 2 -relief sunken -bd 1
    } {
	keep -cursor -background
    }
    pack $itk_component(separator1) -side top -fill x -expand n -pady 10 -padx 5


    #
    # Create the Request Information.
    #

    itk_component add requestFrame {
	iwidgets::labeledwidget $sfChildsite.requestFrame				\
	-labelpos n 							\
	-labeltext "Request Overview" 
    } {
	keep -background 						\
	    -cursor							\
	    -foreground							
    }

    set reqChildsite [ $itk_component(requestFrame) childsite ]
    createReqInfo $reqChildsite

    $itk_component(requestFrame) hull configure -width 250
#    bind $itk_component(requestFrame) <Configure> [ code $this repackLabels ]
    pack $itk_component(requestFrame) -anchor n -fill both -expand yes \
	-side top 

    set cmds [ list 							\
	"displayActionInfo $statusObject $itk_component(hull)" 		\
        "displayUnitInfo   $statusObject $itk_component(hull)" 		\
	"displayStageInfo  $statusObject $itk_component(hull)" 		\
	"displayDeviceInfo $statusObject $itk_component(hull)" ]

    createToolbar stoViews $sfChildsite {Actions Units Stages Devices}	\
	"$cmds"								\
	{ "display the action array." 					\
	  "display the content of the staging areas."			\
	  "display the unit array" 					\
	  "display the list of devices." }				\
	{ "_stgHelpInfo openUrl dhgSto.html#viewsToolbar"		\
	  "_stgHelpInfo openUrl dhgSto.html#viewsToolbar"		\
          "_stgHelpInfo openUrl dhgSto.html#viewsToolbar" 		\
	  "_stgHelpInfo openUrl dhgSto.html#viewsToolbar" }		\
	$this


    #
    # Set the size, title,  and icon details for the Console
    #

    wm iconbitmap $itk_component(hull) @$dhgDir/images/smallGemini.xbm
    wm iconname $itk_component(hull) STO
    wm geometry $itk_component(hull) 1030x600
    wm title $itk_component(hull) "Gemini Storage Server"


    #
    # Create help information.  
    #
    # NOTE:  _stgHelpInfo is created in the global scope this should
    #	     be passed as a parameter, where needed.
    #


    uplevel #0 cHelp _stgHelpInfo $_helpDir/dhgSto.html
    if { "[ uplevel #0 _stgHelpInfo getHtmlDir ]" == "" } {
	uplevel #0 _stgHelpInfo setHtmlDir $_helpDir
    }


    #
    # Make sure all of the DHS status are aligned and display the
    # main window.
    #

    initActionButtons
    update idletasks

	     
    $sfChildsite.status align
    $itk_component(hull) configure -cursor {arrow black}
    wm deiconify $itk_component(hull)

    $statusObject enable
    set displayWarnings 1
    after idle "[ code $this repackLabels now ]"

    return "$itk_component(hull)"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::destructor
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
# Destructor for the "CStgMain" class.
#
# DESCRIPTION:
# Make sure the help information (firefox) is destroyed and make sure 
# the fillers are not being reconfigured.  Also remove the associations
# between particular status records and methods of this class.
#
# EXTERNAL VARIABLES:
# CStgMain::statusObject
#		(CDhgStatus) 	Instance of the CDhgStatus class.
# ::_stgHelp	(CHelp)	    	Instance of the CHelp class.
# ::_stoConfig	(CDhgStatus) 	Instance of the CDhgStatus class.
# ::_cStoCommands
#		(CStoCommands) 	Instance of the CStoCommands class.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStgMain::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStgMain::destructor {} {
    if { $repackPending != "" } {
	after cancel $repackPending
    }

    set maxRequests "[ $statusObject value stoReqArraySize ]"
    for { set index 0 } { $index < $maxRequests } { incr index } {
	$statusObject unproc stoReq${index}InUse 		\
	    [ code $this showRequest ]
	$statusObject unproc stoReq${index}RetrieveState		\
	    [ code $this updateActionState ]
	$statusObject unproc stoReq${index}RetrieveSize  	\
	    [ code $this updateActionUnit ]

	$statusObject unproc stoReq${index}QueueState  		\
	    [ code $this updateActionState ]
	$statusObject unproc stoReq${index}QueueSize      	\
	    [ code $this updateActionUnit ]

	$statusObject unproc stoReq${index}PrepState 		\
	    [ code $this updateActionState ]
	$statusObject unproc stoReq${index}PrepNum    		\
	    [ code $this updateActionUnit ]

	$statusObject unproc stoReq${index}WriteState     	\
	    [ code $this updateActionState ]
	$statusObject unproc stoReq${index}WriteNum      	\
	    [ code $this updateActionUnit ]

	$statusObject unproc stoReq${index}CleanupState    	\
	    [ code $this updateActionState ]
	$statusObject unproc stoReq${index}CleanupNum     	\
	    [ code $this updateActionUnit ]

	$statusObject unproc stoReq${index}NotifyState     	\
	    [ code $this updateActionState ]
	$statusObject unproc stoReq${index}NotifyValue    	\
	    [ code $this updateActionUnit ]
    }

    if { [ find objects ::_stoConfig ] != "" } {
	delete object _stoConfig
    }

    if { [ find objects ::_cStoCommands ] != "" } {
	delete object _cStoCommands
    }

    if { [ find objects ::_stgHelpInfo ] != "" } {
	delete object _stgHelpInfo
    }

    return
}

# ------------------------------------------------------------------
#                       PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::appNameConfig
#
# INVOCATION:
# Object appNameConfig numTokens tokens
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
# To interpret a line from the configruation file and set this applications
# name.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CCStgMain::appName	(string)	: Name given to this application.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStgMain::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStgMain::appNameConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "identity" } {
	error "Expected configuration information for identity not [ lindex $tokens 0 ]"
    }
    
    if { $numTokens != 2 } {
	error "Configuration file error, expect 2 tokens but got $numTokens for identity."
    }

    set appName [ lindex $tokens 1 ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::createMenus
#
# INVOCATION:
# createCommand window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name)	Window path name of the toplevel widget.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To create the menus for the Storage Server subsystem window.
#
# DESCRIPTION:
# Create the Commands, Views, Options, Help, and Quit menus with all
# of the selections, short help and context-sensitive help.
#
# EXTERNAL VARIABLES:
# ::_stgDefaults	
#		(array)	    defines all of the commands, help messages,
#		            and display status for each DHS (sub)system.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
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

body 	CStgMain::createMenus {
    window
} {
    global ::_stgDefaults


    #
    # Set-up the Command menu
    #

    set commandMenu [ $window add_menubutton "Commands" left ]
    $window component commands configure -underline 0
    $window add_short_help [ $window component commands ]		\
	{ {bitmap b1} Command menu: Reset STO, Set Debug level, Set 	\
 Simulation Level, and Test STO }

    set i 0 
    foreach command $_stgDefaults(sto,commands) {
	if { $command == "separator" } {
	    $commandMenu add separator
	} elseif { [ string first "resetHealth" $command ] != -1 } {
	    $window add_menuitem $commandMenu command 			\
		[ lindex $_stgDefaults(sto,commandName) $i ]		\
		"{bitmap b1} [ lindex $_stgDefaults(sto,commandDesc) $i ]" \
		-command "_cStoCommands $command"			\
		-underline 6
	} elseif { [ string first stoRefresh $command ] != -1 } {
	    $window add_menuitem $commandMenu command 			\
		[ lindex $_stgDefaults(sto,commandName) $i ]		\
		"{bitmap b1} [ lindex $_stgDefaults(sto,commandDesc) $i ]" \
		-command "_cStoCommands $command"			\
		-underline 2
	} else {
	    $window add_menuitem $commandMenu command 			\
		[ lindex $_stgDefaults(sto,commandName) $i ]		\
		"{bitmap b1} [ lindex $_stgDefaults(sto,commandDesc) $i ]" \
		-command "_cStoCommands $command"			\
		-underline 0
	}
	incr i
    }

    
    #
    # Set-up the Views menu
    #

    set viewsMenu [ $window add_menubutton "Views" left ]
    $window component views configure -underline 0 
    $window add_short_help [ $window component views ]			\
	{ {bitmap b1} Views Menu: Actions, Units, and Stages }


    $window add_menuitem $viewsMenu command Actions			\
       { {bitmap b1} Display current STO action queues status.}		\
	-command "displayActionInfo $statusObject $itk_component(hull)"	\
	-underline 0
    $window add_menuitem $viewsMenu command Units			\
	{ {bitmap b1} Display STO Unit information..}			\
	-command "displayUnitInfo $statusObject $itk_component(hull)"	\
	-underline 0
    $window add_menuitem $viewsMenu command Stages			\
       { {bitmap b1} Display contents of the Storage Server's staging areas.}\
	-command "displayStageInfo $statusObject $itk_component(hull)"	\
	-underline 0
    $window add_menuitem $viewsMenu command Devices			\
       { {bitmap b1} Display the list of devices.}			\
	-command "displayDeviceInfo $statusObject $itk_component(hull)"	\
	-underline 0


    #
    # Set-up the Options menu
    #

    set optMenu [ $window add_menubutton "Options" left ]
    $window component options configure -underline 0
    $window add_short_help [ $window component options ] 		\
	{ {bitmap b1} Options Menu: }

    $this add_menuitem $optMenu command {Health Description Length}	\
       { {bitmap b1} Set the number of log messages kept.}		\
	-underline 0


    #
    # Set-up the Help menu
    #

    set helpMenu [ $window add_menubutton "Help" left ]
    $window component help configure -underline 0
    $window add_short_help [ $window component help ] { {bitmap b1} Help\
 Menu: About DHS and extended help information.}

    $window add_menuitem $helpMenu command About			\
       { {bitmap b1} Brief description on DHS.}				\
	-command "createAbout \"$shortVersion\""			\
	-underline 0
    $window add_menuitem $helpMenu command Extended...			\
       { {bitmap b1} Detailed help information.}			\
	-command "_stgHelpInfo openUrl dhgSto.html"			\
	-underline 0


    #
    # Set-up the Quit menu
    #

    set quitMenu [ $window add_menubutton "Quit" right ]
    $window component quit configure -underline 0 
    if { [ isTrue $standAlone ] } {
	$window add_short_help [ $window component quit ]		\
	    { {bitmap b1} Quit Menu: Exit the Storage Server Console.}
	$window add_menuitem $quitMenu command "Exit"			\
	    { {bitmap b1} Exit the Storage Server user interface.}	\
	    -command "delete object $this; after idle exit"		\
	    -underline 1
    } else {
	$window add_short_help [ $window component quit ]		\
	    { {bitmap b1} Quit Menu: Remove the Storage Server Console from the display.}
	$window add_menuitem $quitMenu command "Close Window"		\
	    { {bitmap b1} Remove the Storage Server window from the display.}\
	    -command "wm withdraw $itk_component(hull)"			\
	    -underline 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::createReqInfo
#
# INVOCATION:
# createReqInfo	window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(pathName)	Parent of the request info window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display the status record request array information.
#
# DESCRIPTION:
# Create all window for all possible requests, there is a fixed 
# maximum.  However, do not display them all only the ones in use.
# 
# Create Action labels at the top.
# Create a row of information for each request, A row contains a button
# for each action separated by buttons which initiate various actions.
# The left most part of the row contains the name of the request and the
# buttons contain the amount of data that is available. Below the
# request information display the unit labels.
#
# EXTERNAL VARIABLES:
# CStgMain::requestButtons
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
# ::_stgDefaults	
#		(array)	    defines all of the commands, help messages,
#		            and display status for each DHS (sub)system.
# ::reqStatus 	(array)	    A status array containing all of the status
#			    all of the status information for requests.
#
# PRIOR REQUIREMENTS:
# Input parameter "window" exists
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CStgMain::createReqInfo {
    window
} {
    global ::_stgDefaults
    global ::reqStatus

    set maxRequests "[ $statusObject value stoReqArraySize ]"


    #
    # Create Action labels.
    #

    frame $window.actionFrame 
    pack $window.actionFrame -pady 2 -fill x
    button $window.actionFrame.name -disabledforeground black 		\
	-text "Name" -relief flat -state disabled -width 10
    pack  $window.actionFrame.name -side left 
    label $window.actionFrame.ret -bd 0 -justify center -text "Retrieve"
    pack  $window.actionFrame.ret -side left
    label $window.actionFrame.queue -bd 0 -justify center -text "Queue"
    pack  $window.actionFrame.queue -side left
    label $window.actionFrame.prep -bd 0 -justify center -text "Prepare"
    pack  $window.actionFrame.prep -side left
    label $window.actionFrame.write -bd 0 -justify center -text "Write"
    pack  $window.actionFrame.write -side left
    label $window.actionFrame.clean -bd 0 -justify center -text "Cleanup" 
    pack  $window.actionFrame.clean -side left
    label $window.actionFrame.notify -bd 0 -justify center -text "Notify" 
    pack  $window.actionFrame.notify -side left

    
    #
    # Crete the request info.
    #
    
    for { set reqNum 0 } { $reqNum < $maxRequests } { incr reqNum } {
	#
	# Create a frame to hold the request details.
	#
	
	itk_component add req$reqNum {
	    frame $window.req$reqNum
	} {
	    keep -background -cursor
	}

	set inUse "[ $statusObject value stoReq${reqNum}InUse ]"
	if { [ isTrue $inUse ] || [ simMode ] } {
	    lappend visibleRequests "$window.req$reqNum"
	    pack $window.req$reqNum -fill x
	}



	#
	# Create an identifing button.
	#
	
	set cmd "displayRequestInfo $reqNum $statusObject $itk_component(hull)"

	$statusObject link stoReq${reqNum}ReqName		\
	    reqStatus(name$reqNum)
	button $window.req$reqNum.button 				\
	    -command "$cmd"						\
	    -textvariable reqStatus(name$reqNum)			\
	    -width 10
	pack $window.req$reqNum.button -side left
	$this add_short_help $window.req$reqNum.button 			\
	    {Name associated with the request. {bitmap b1} to view more information }


	#
	# Create the retrieve to notify details.
	#
	
	set i 0
	set epics {RetrieveSize QueueSize PrepNum WriteNum CleanupNum 
	    NotifyValue}
	set infoType {RETRIEVABLE QUEUEABLE PREPARABLE WRITEABLE CLEANUPABLE 
	    NOTIFYABLE}
	set widths {9 5 4 4 4 6}
	set helpIndex 5

	foreach status {Retrieve Queue Prepare Write Cleanup Notify} {
	    set epicsVar reqStatus([ lindex $epics $i ]$reqNum)

	    $statusObject link "stoReq${reqNum}[ lindex $epics $i ]" "$epicsVar"
	    
	    if { $status == "Prepare" } {
		set epicsSIR stoReq${reqNum}PrepState
	    } else {
		set epicsSIR stoReq${reqNum}${status}State
	    }
	    set width [ lindex $widths $i ]
	    set frame [ frame $window.req$reqNum.[ string tolower 	\
		       [ lindex $epics $i ] ] ]


	    set cmd "_cStoCommands stoInfo $frame.units [ lindex 	\
   		    $infoType $i ] $reqNum"
	    set msg "Information describing [ lindex 			\
		    $_stgDefaults(sto,requestHelp) $helpIndex ]"
	    button $frame.units						\
		-command $cmd						\
		-font $_stgDefaults(entryFont)				\
		-justify right						\
		-relief raised						\
		-textvariable $epicsVar					\
		-width $width
	    $this add_short_help $frame.units 				\
		"{bitmap b1} $msg"
	    incr helpIndex

	    
	    if { $status == "Retrieve" || $status == "Cleanup" ||
		 $status == "Notify" } {
		set hull $itk_component(hull)
		set commandList [ list 				       	\
		      "_cStoCommands stoCancelAction $hull $status $reqNum" \
		      "_cStoCommands stoRequestAction $hull $status  $reqNum" ]
		set requestButtons($epicsSIR) [ stopGoButtons $frame	\
		      _stgDefaults { stop forward } $commandList ]
	    } else {
		set commandList [ list 				    \
		      "_cStoCommands stoCancelAction $hull $status $reqNum"  \
		      "_cStoCommands stoRequestAction $hull $status $reqNum" \
		      "_cStoCommands stoRequestRollback $hull $status $reqNum" ]
		set requestButtons($epicsSIR) [ stopGoButtons $frame\
		      _stgDefaults { stop forward reverse } $commandList ]
	    }
	    set stopGo [ lindex $requestButtons($epicsSIR) 0 ]
	    pack $frame -fill y -side left
	    pack $frame.units -fill y -side left -padx 12
	    pack $stopGo -side left -fill y
	    


	    #
	    # Create context sensitive help and short help
	    #

	    $this add_short_help $stopGo 		       		\
	       "{bitmap b1} [ lindex $_stgDefaults(sto,requestHelp) $helpIndex ]"
	    incr helpIndex
	    if { [ llength $requestButtons($epicsSIR) ] == 3 } {
		set sgIndex 1
	    } elseif { [ llength $requestButtons($epicsSIR) ] == 4 } {
		$this add_short_help [ lindex $requestButtons($epicsSIR) 1 ] \
		    "{bitmap b1} Undo the last $status action that was 	\
processed."
		set sgIndex 2
	    }
	    $this add_short_help [ lindex $requestButtons($epicsSIR) $sgIndex ]\
		"{bitmap b1} Cancel a pending $status action or Stop a \
$status action that is in progress."
	    incr sgIndex
	    $this add_short_help [ lindex $requestButtons($epicsSIR) $sgIndex ]\
		"{bitmap b1} Initiate the processing of a  $status action."
	    

	    bind $window.req$reqNum.button <Button-3> {_stgHelpInfo openUrl "dhgSto.html#request"}
	    bind $frame <Button-3> {_stgHelpInfo openUrl "dhgSto.html#request"}
	    bind $frame.units <Button-3> {_stgHelpInfo openUrl "dhgSto.html#request"}
	    foreach btn $requestButtons($epicsSIR) {
		bind $btn <Button-3> {_stgHelpInfo openUrl "dhgSto.html#request"}
	    }

	    incr i
	}
    }


    #
    # Create unit labels
    #

    frame $window.unitFrame 
    pack $window.unitFrame -pady 2 -fill x 
    button $window.unitFrame.blank -disabledforeground black 		\
	-relief flat -state disabled -width 10
    pack  $window.unitFrame.blank -side left
    label $window.unitFrame.ret -justify center -text "Megabytes"	\
	-font "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*"
    pack $window.unitFrame.ret -side left
    label $window.unitFrame.queue -justify center -text "Units"		\
	-font "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*"
    pack $window.unitFrame.queue -side left
    label $window.unitFrame.prep -justify center -text "Units"		\
	-font "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*"
    pack $window.unitFrame.prep -side left
    label $window.unitFrame.write -justify center -text "Units"		\
	-font "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*"
    pack $window.unitFrame.write -side left
    label $window.unitFrame.clean -justify center -text "Units"		\
	-font "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*"
    pack $window.unitFrame.clean -side left
    label $window.unitFrame.notify -justify center -text "Ready"	\
	-font "-*-Courier-Medium-R-Normal--*-120-*-*-*-*-*-*"
    pack $window.unitFrame.notify -side left


    #
    # Set-up the monitoring of status value changes.
    #

    for { set index 0 } { $index < $maxRequests } { incr index } {
	$statusObject proc stoReq${index}InUse 		\
	    [ code $this showRequest ]
	$statusObject proc stoReq${index}RetrieveState	\
	    [ code $this updateActionState ]
	$statusObject proc stoReq${index}RetrieveSize  	\
	    [ code $this updateActionUnit ]

	$statusObject proc stoReq${index}QueueState  	\
	    [ code $this updateActionState ]
	$statusObject proc stoReq${index}QueueSize      	\
	    [ code $this updateActionUnit ]

	$statusObject proc stoReq${index}PrepState 	\
	    [ code $this updateActionState ]
	$statusObject proc stoReq${index}PrepNum    	\
	    [ code $this updateActionUnit ]

	$statusObject proc stoReq${index}WriteState      	\
	    [ code $this updateActionState ]
	$statusObject proc stoReq${index}WriteNum      	\
	    [ code $this updateActionUnit ]

	$statusObject proc stoReq${index}CleanupState     	\
	    [ code $this updateActionState ]
	$statusObject proc stoReq${index}CleanupNum      	\
	    [ code $this updateActionUnit ]

	$statusObject proc stoReq${index}NotifyState     	\
	    [ code $this updateActionState ]
	$statusObject proc stoReq${index}NotifyValue     	\
	    [ code $this updateActionUnit ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::createStoStatus
#
# INVOCATION:
# createStoStatus window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window  	(path name)  Pathname of the DHS status window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To create the section of the STO subsystem window that displays the 
# overall running conditions of the STO.
#
# DESCRIPTION:
# Create a status box for all of the status information.  Foreatch status
# record place it's value in an entryfield and label it appropriately.
# The health description is displayed in a comboebox, entryfield with
# a drop down list.  Create the short help and context sensitive help. 
#
# EXTERNAL VARIABLES:
# CStgMain::configObject
#		(CConfigGen) Instance of the CConfigGen class.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
# ::_stgDefaults	
#		(array)	    defines all of the commands, help messages,
#		            and display status for each DHS (sub)system.
# ::_stgHelp	(CHelp)	    	Instance of the CHelp class.
# ::stoStatus	(array)     Array containing STO's status information.
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

body CStgMain::createStoStatus {
    window
} {
    global ::_stgDefaults
    global ::stoStatus
    
    set statusHelp [ list "dhgState.html" "dhgHealth.html" "dhgDebug.html"\
			 "dhgSim.html" "dhgHealthDesc.html" ]
    

    #
    # Create the box that will contains the status information.
    #

    itk_component add stoStatus {
	cStatusBox $window 						\
	    -statusmargin 30
    } {
	keep -background -cursor -foreground
    }
    pack $window -fill both -expand y -pady 3 -padx 10


    #
    # Add short help information
    #
    
    $this add_short_help $window "Status information for DHS"

    
    #
    # Add status information for each status item found.
    #

    set i 0
    foreach status $_stgDefaults(sto,status) {
	if { "$status" == "Desc" } {
	    $window add cLogMonitor $status $statusObject sto$status 	\
		-editable false						\
		-labeltext [ lindex $_stgDefaults(sto,statusDesc) $i ]	\
		-relief ridge						\
		-textfont $_stgDefaults(entryFont)			\
		-unique false
	} else {
	    $statusObject link sto$status stoStatus($status)
	    $window add entryfield $status				\
		-textfont $_stgDefaults(entryFont)			\
		-labeltext [ lindex $_stgDefaults(sto,statusDesc) $i ]	\
		-relief ridge						\
		-state disabled				  		\
	    	-textvariable stoStatus($status)
	}

	$this add_short_help [ $window component $status ] 		\
	    [ lindex $_stgDefaults(sto,help) $i ]

	
	#
	# Set-up context sensitive help.
	#
	
	switch -- "$status" {
	    Debug { set htmlPage "dhgDebug.html" }
	    Desc { set htmlPage "dhgHealthDesc.html" }
	    Health { set htmlPage "dhgHealth.html" }
	    Sim { set htmlPage "dhgSim.html" }
	    State { set htmlPage "dhgState.html" }
	    default {
		set htmlPage "dhg${status}.html#status"
	    }
	}
	bind  [ $window component $status ] <Button-3> 			\
	    "_stgHelpInfo openUrl \"${htmlPage}\""
	incr i
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::getConfigInfo
#
# INVOCATION:
# getConfigInfo ?configObj?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) configObj	(CConfigGen)	Instance of CConfigGen class.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To find the information the storage server window needs from the
# configuration file.
#
# DESCRIPTION:
# This method provides backwards compatibility for when the Storage
# Server could be run as a separate entity from the DHS Console.  It
# also can act like it is part of the DHS Console.
#
# If the given config object is a zero length string then we need to
# create an instance of the CConfigGen class - this is intended to be
# used when the Storage Server window is run by itself.
#
# Find the verions information for the "About" window.
#
# EXTERNAL VARIABLES:
# CStgMain::configObject
#		(CConfigGen) Instance of the CConfigGen class.

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

body	CStgMain::getConfigInfo {
    configObj
} {
    global ::debug 

    if { $configObj != "" } {
	set configObject $configObj
    }  else {
	#
	# create the configObject and open the file.
	# Also read the version information.
	#

	uplevel #0 cConfigGen _stoConfig
	set result [ _stoConfig configOpen dhsConsole ] 
	if { [ lindex $result 0 ] == "ERROR" } {
	    error "[ lindex $result 1 ]"
	} elseif { $debug == "MIN" || $debug == "FULL" } {
	    puts "INFO: Reading configuration file:\n\t[ lindex $result 1 ]"
	}

	set configObject _stoConfig 


	#
	# Set-up the applications name and kind.
	# 

	if { [ $configObject configGet identity [ code $this appNameConfig ] ] \
		 != "SUCCESS" } {
	    error "Unable to read identity information from configuration file."
	}

	appData add appKind Console
	appData add appName $appName
	set configPath [ file dirname [ lindex $result 1 ] ]
	cd $configPath
	#OMIT    appData add configFile "$configPath/CA_CONFIG"
	if { $debug == "MIN" || $debug == "FULL" } {
	    #OMIT	puts "Reading Channel Access configuration file:\n\t[ appData lookup configFile ]"
	}
   }


    #
    # Find Version information.
    #

    if { [ $configObject configGet version [ code $this versionConfig ] ] \
	     != "SUCCESS" } {
	error "Unable to read version information from configuration file."
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::initActionButtons
#
# INVOCATION:
# CStgMain::initActionButtons
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To set the intial value of the request buttons.
#
# DESCRIPTION:
# Call CStgMain::updateActionUnit for each unit value for all of the
# requests that are in use
#
# EXTERNAL VARIABLES:
# CStgMain::actionList	    (list)	List of storage server actions.
# CStgMain::actionUnitList  (list)	List of unit values names.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
#
# PRIOR REQUIREMENTS:
# CStgMain::createReqInfo has been exectued
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
body	CStgMain::initActionButtons {
} {
    set maxRequests "[ $statusObject value stoReqArraySize ]"
    for { set index 0 } { $index < $maxRequests } { incr index } {
	set inUse "[ $statusObject value stoReq${index}InUse ]"

	if { [ isFalse $inUse ] || [ simMode ] } {
	    continue
	} 

	set actIndex 0
	foreach action $actionList {
	    eval updateActionUnit dhsSAD.stoReq${index}${action}[ lindex \
      		$actionUnitList $actIndex ] \"\" \"\" \"[ $statusObject \
	        value stoReq${index}${action}[ lindex $actionUnitList 	\
	        $actIndex ] ]\"		
	    incr actIndex
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::repackLabels
#
# INVOCATION:
# repackLabels ?when?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) when      (string) Either "now" or "later"
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To make sure the correct widths have been given to the various labels
#
# DESCRIPTION:
# Based on the widths of the various bits of a request information that
# is displayed align the labels so they appear over the appropriate
# items.  They are aligned by adjusting their widths.  If there is no
# requests displayed one is temporarily displayed.
#
# Both the action and unit labels are adjusted.
#
# EXTERNAL VARIABLES:
# CStgMain::visibleRequests (list)	List of request currently displayed.
#
# PRIOR REQUIREMENTS:
# CStgMain::CreateReqInfo has been executed.
#
# SEE ALSO:
# CStgMain::createReqInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStgMain::repackLabels {
    when
} {
    #
    # Find out when the table is to be resorted.
    #

    
    if { $when == "later" } {
        if { $repackPending == "" } {
            set repackPending [ after idle "[ code $this repackLabels now ]" ]
        } 
        return
    } elseif { $when != "now" } {
        error "bad option \"$when\":whould be now or later."
    }

    set repackPending ""
    set window [ $itk_component(requestFrame) childsite ]


    # 
    # Find the widths in one row of request information.
    #

    if { [ llength $visibleRequests ] < 1 } {
	if { [ winfo viewable $itk_component(hull) ] } {
	    pack $itk_component(req8) -fill x 				\
		-before [ winfo parent $itk_component(req8) ].unitFrame
	    set reqWindow $itk_component(req8)
	    tkwait visibility $itk_component(req8)
	} else {
	    set repackPending ""
	    return
	}
    } else {
	set reqWindow [ lindex $visibleRequests 0 ]
    }

    set widths {}
    foreach item [ pack slaves $reqWindow ] {
	set children [ pack slaves $item ]
	if { $children == {} } {
	    lappend widths [ winfo width $item ]
	} else {
	    foreach child $children {
		lappend widths [ winfo width $child ]
	    }
	}
    }


    #
    # Adjust action labels
    #

    set padding 0
    set index 0
    foreach action [ pack slaves $window.actionFrame ] {
	set labelWidth [ winfo width $action ]
	set buttonWidth [ lindex $widths [ expr $index +  1 ] ]

	if { $index == 0 } {
	    set unitWidth [ lindex $widths $index ]
	    set padding [ expr $labelWidth - $unitWidth + 8 ]
	    incr index
	} else {
	    if { $index == 1 } { 
		set unitWidth [ expr [ lindex $widths $index ] + 24 ]
	    } else { 
		set unitWidth [ expr [ lindex $widths $index ] + 12 ]
	    }

	    set center [ expr $unitWidth + ( $buttonWidth / 2 ) ]
	    set padx [ expr $center - ( $labelWidth / 2 ) - $padding ]
	    if { $padx < 0 } { 
		set padx 0
	    }
	    pack configure $action -padx $padx
	    set padding [ expr $padx - ( ( $buttonWidth / 2 ) + 12 -	\
					     ( $labelWidth / 2 ) ) ]
	    incr index 2
	}
    }


    #
    # Adjust unit labels
    #

    set padding 0
    set index 0
    foreach unit [ pack slaves $window.unitFrame ] {
	if { $index == 0 } {
	    set padding 0
	    incr index
	} else {
	    set labelWidth [ winfo width $unit ]
	    if { $index == 1 } {
		set unitWidth [ lindex $widths $index ]
		set buttonWidth 12
		incr index 1
	    } else {
		set unitWidth [ lindex $widths [ expr $index + 1 ]]
		set buttonWidth [ expr 12 + [ lindex $widths $index ] ]
		incr index 2
	    }
	    

	    set center [ expr $buttonWidth + ( $unitWidth / 2 ) ]
	    set padx [ expr $center - ( $labelWidth / 2 ) - $padding ]
	    if { $padx < 0 } {
		set padx 0
	    }
	    pack configure $unit -padx $padx
	    set padding [ expr $padx - ( ( $unitWidth / 2 ) + 12 -	\
					     ( $labelWidth / 2 ) ) ]

	}
    }

    if { [ llength $visibleRequests ] < 1 } {
	pack forget $itk_component(req8)
    }


    #
    # Need to adjust the hight of the request childsite to make
    # sure everything is visible.
    #

    [ $itk_component(requestFrame) childsite ] configure -height 100


    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::setupStatusMon
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
# To setup the monitoring of EPICS values for the DHS console.
#
# DESCRIPTION:
# Create an instance of the stgStatus monitoring class
# and link the led's to the "updateled" procedure
#
# EXTERNAL VARIABLES:
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
# ::__stgStatus (CDhgStatus) Instance of the CDhgStatus class.
#
# PRIOR REQUIREMENTS:
# An instance of the CConfigGen exists and its configOpen method has
# been called.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStgMain::setupStatusMon {
    { statusObj {} }
} {
    #
    # Set up the epics or dhs  service and variables/procedures for monitoring
    # status information.
    # 

    if { $statusObj == "" } {
	service epics
	uplevel #0 cDhgStatus __stgObject "sta sto"
	set statusObject "__stgObject"
    } else {
	set statusObject "$statusObj"
    }

    $statusObject disable
    $statusObject proc stoAlert "[ code $this showWarning ]"

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::showRequest
#
# INVOCATION:
# showRequest name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the status item being updated.
# (>) alarm     (string)        Alarm status, OK, Warning or Bad
# (>) time      (time)          Time of the update.
# (>) valueList (list)          The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display or remove a request from the main window depending on if
# its "inUse" value is true or false.
#
# DESCRIPTION:
# Try and find the window that is associated witht he request whose
# "inuse" value has changed.  If it is true then we make sure the
# request is displayed on the main window, if it is false we remove
# it from the main window.
#
# EXTERNAL VARIABLES:
# CStgMain::actionList	    (list)	List of storage server actions.
# CStgMain::displayWarnings (boolean)   TRUE indicates warnings are displayed.
# CStgMain::visibleRequests (list)	List of request currently displayed.
#
# PRIOR REQUIREMENTS:
# CStgMain::createReqInfo has been executed.
#
# SEE ALSO:
# extractNums
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStgMain::showRequest {
    name
    alarm
    time
    valueList
} {
    set disabled 0
    if { [ isTrue $displayWarnings ] } { 
	set disabled 1
	disableWarnings
    }


    #
    # Make sure the main window has been created.
    #

    if { ! [ winfo exists $itk_component(hull) ] } {
	after 100 "[ code $this showRequest ] $name $alarm \"$time\" \"$valueList\""
    }


    #
    # Find the window corresponding to the request being updated.
    #

    set name [ lindex [ split $name . ]  1 ]
    set reqNum [ extractNums $name ]
    set windowName "req$reqNum"
    set window $itk_component(req$reqNum)


    #
    # Display or remove the window depending on the value given in the
    # valueList
    #

    if { [ winfo exists $window ] } {
	set parent [ winfo parent $window ] 
	if { [ isTrue "$valueList" ] } {
	    if { ! [ winfo ismapped $window ] } { 
		pack $window -fill x -before $parent.unitFrame
	    }

	    if { [ lsearch $visibleRequests $window ] == -1 } {
		lappend visibleRequests $window
	    }
	} elseif { [ isFalse "$valueList" ] } {
	    if {  [ winfo ismapped $window ] } {
		pack forget $window
	    }

	    if { [ set pos [ lsearch $visibleRequests $window ] ] != -1 } {
		set visibleRequests [ lreplace $visibleRequests $pos $pos ]
	    }
	} elseif { [ isTrue $displayWarnings ] } {
	    warningDialog "Invalid in use value `$valueList', expected a boolean."
	} 
    } elseif  { [ isTrue $displayWarnings ] } {
	warningDialog"Invalid value request number `$reqNum', expected a\
 value beteween 0  and 9."
    }

    if { [ isTrue $disabled ] } {
	enableWarnings
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::updateActionButtons.
#
# INVOCATION:
# updateActionButtons prevState nextState nextUnit name alarm time 
#				valueList ?warnings?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) prevState	(string)	State SIR record for the previous action
# (>) nextState (string)	State SIR record for the next action
# (>) nextUnit	(string)	Unit SIR record for the next action
# (>) name      (string)        Name of the status item being updated.
# (>) valueList (list)          The new value.
# (>) warnings	(boolean)	True if warnings are to be displayed.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the request information being displayed for the SIR record
# specified by the name parameter.
#
# DESCRIPTION:
# Detemine the values of the EPICS records "name", "prevState", "nextState",
# and "nextUnit".  Also find the unit value for "name".
# Use the STO greyout algorithm to determine the colour and state of the
# buttons.
# 
#		Grey out Algorithm
#		-------------------
# If state is "IDLE"
#     Cancel/Stop button is grey
#     Forward button is grey if unit value is false, zero or "Not Known"
#        OR if the prev Action state value is "ROLLBACK IN-PROGRESS"
#     Rollback/Reverse button is grey if the unit value is zero or "NOT  KNOW"
#	 OR the next Action state value is either "REQUESTED" or "IN-PROGRESS"
# 
# If the state is "REQUESTED"
#     Cancel/Stop button is not grey
#     Forward button is yellow and disabled
#     Reverse/Rollback button is grey
#
# If the state is "IN-PROGRESS"
#     Cancel/Stop button is not grey
#     Forward button is green and disabled
#     Reverse/Rollback button is grey
#
# If the state is "ROLLBACK IN-PROGRESS" or "ROLLBACK REQUESTED"
#     Cancel/Stop button is grey
#     Forward button is grey
#     Reverse/Rollback button is green and disabled
#
# EXTERNAL VARIABLES:
# CStgMain::actionList	    (list)	List of storage server actions.
# CStgMain::actionUnitList  (list)	List of unit values names.
# CStgMain::displayWarnings (boolean)   TRUE indicates warnings are displayed.
# CStgMain::reqButtonArray  (array)	An array of all the button paths.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
#
# PRIOR REQUIREMENTS:
# CStgMain::createReqInfo has been executed.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStgMain::updateActionButtons {
    prevState
    nextState
    nextUnit
    name
    valueList
} {
    global ::debug

    #
    # Find the prevState, nextState, nextUnit values.  Also find the
    # the state value, state name, and unit value  for the EPICS record 
    # specified by "name".
    #

    if { [ string trim $prevState ] != "" } {
	set prevStateValue "[ $statusObject value $prevState ]"
    }

    if { [ string trim $nextState ] != "" } {
	set nextStateValue "[ $statusObject value $nextState ]"
    }

    if { [ string trim $nextUnit ] != "" } {
	set nextUnitValue "[ $statusObject value $nextUnit ]"
    }

    set action [ extractListItem $actionList $name ]
    set reqNum [ extractNums $name ]


    #
    # Find the name and values of the state and unit EPICS records
    #
	
    set stateName $name
    set stateValue "$valueList"
    set pos [ lsearch $actionList $action ]
    if { $pos < 0 } {
	#
	# Error 
	#

	if { [ isTrue $warnings ] } {
	    after idle "warningDialog \"$thisAction was not found in the STG's action list as a result the request information may not reflect the values in EPICS\""
	} elseif { $debug == "FULL" } {
	    puts "ERROR: $thisAction was not found in the STG's action list as a result the request information may not reflect the values in EPICS"
	}
	return
    }
    
    set unitName "stoReq${reqNum}${action}[ lindex $actionUnitList $pos ]"
    set unitValue "[ $statusObject value $unitName ]"


    #
    # Find the list of windows that make up the set of buttons
    #

    if { [ lsearch [ array names requestButtons ] $stateName  ] == -1 } {
	#
	# Error
	#

	if { [ isTrue $displayWarnings ] } {
	    after idle "warningDialog \"Unable to locate request buttons for $stateName\""
        } elseif { $debug == "FULL" } {
            puts "ERROR: Unable to locate request buttons for $stateName"
	}
	return
    }    
    eval set windowList \$requestButtons($stateName)


    #
    # Determine the forward, cancel, and reverse window path names.
    #

    set isError 0
    set action [ extractListItem $actionList $stateName ]
    switch -exact -- "$action" {
	Retrieve -
	Notify -
	Cleanup {
	    set stopWindow [ lindex $windowList 1 ]
	    set forwardWindow [ lindex $windowList 2 ] 
	    set reverseWindow ""

	    if { ! [ winfo exists [ lindex $windowList 1 ] ] ||
		 ! [ winfo exists [ lindex $windowList 2 ] ] } {
		set isError 1
	    } 
	}
	Queue -
	Prep -
	Write  {
	    set reverseWindow [ lindex $windowList 1 ]
	    set stopWindow [ lindex $windowList 2 ]
	    set forwardWindow [ lindex $windowList 3 ] 

	    if { ! [ winfo exists [ lindex $windowList 1 ] ] ||
		 ! [ winfo exists [ lindex $windowList 2 ] ] ||
		 ! [ winfo exists [ lindex $windowList 3 ] ] } {
		set isError 1
	    }
	}

	default {
	    set isError 2
	}
    }

    
    #
    # Check if there were any errors.
    #

    if { $isError == 1 } {
	#
	# Error 
	#

	if { [ isTrue $displayWarnings ] } {
	    after idle "warningDialog \"Not all of the expected buttons were found for $stateName EPICS record.\""
	} elseif { $debug == "FULL" } {
	    puts "ERROR: Not all of the expected buttons were found for $stateName EPICS record."
	}
	return
    }  elseif { $isError == 2 } {
	error "Invalid action `$action', should be one of $actionList"
    }


    #
    # Determine the state and colour of the various buttons.
    #

    switch -exact -- "$stateValue" {
	"-" -
	"" {
	    # Ignore, STO server intially sets all records to "-" or ""
	}
	"IDLE" {
	    $stopWindow configure -state disabled
	    if { ( $action != "Notify" && ! [ isNumber "$unitValue" ] ) || \
		     ( $action == "Notify" && ! [ isBoolean "$unitValue" ] ) } {
		#
		# Do Nothing, value is not valid.
		#
	    } elseif { ( $action == "Notify" && [ isFalse "$unitValue" ] )
		       || ( $action != "Notify" && [ isZero "$unitValue" ] ) } {
		$forwardWindow configure -background "#d9d9d9"		\
		    -foreground Black					\
		    -state disabled
	    } elseif { [ string trim $prevState ] != "" && (
			 "$prevStateValue" == "ROLLBACK IN-PROGRESS" ||
			 "$prevStateValue" == "ROLLBACK REQUESTED" ) } {
		$forwardWindow configure -background "#d9d9d9"		\
		    -foreground Black					\
		    -state disabled
	    } else {
		$forwardWindow configure -background "#d9d9d9"		\
		    -foreground Black					\
		    -state normal
	    }
       
    
	    if { [ string trim $reverseWindow ] != ""  } {
		if { ! [ isNumber "$nextUnitValue" ] } {
		    #
		    # Do Nothing, value is not valid.
		    #
		} elseif { [ isZero "$nextUnitValue" ] } {
		    $reverseWindow configure -background "#d9d9d9"	\
			-foreground Black				\
			-state disabled 
		} elseif { [ string trim $nextState ] != "" && 
			   ( "$nextStateValue" == "REQUESTED" ||
			     "$nextStateValue" == "IN-PROGRESS" ) } {
		    $reverseWindow configure -background "#d9d9d9"	\
			-foreground Black				\
			-state disabled 
		} else {
		    $reverseWindow configure -background "#d9d9d9"	\
			-foreground Black	   			\
			-state normal
		}
	    }
	}

	"REQUESTED" {	
	    $stopWindow configure -state normal -foreground Black

	    $forwardWindow configure -background yellow2		\
		-foreground Black					\
		-state disabled

	    if { $reverseWindow != "" } {
	 	$reverseWindow configure -background "#d9d9d9"		\
		    -foreground Black					\
		    -state disabled
	    }
	}

	"IN-PROGRESS" {
	    $stopWindow configure -state normal -foreground Black

	    $forwardWindow configure -background green2			\
    		-foreground Black					\
		-state disabled

	    if { $reverseWindow != "" } {
		$reverseWindow configure  -background "#d9d9d9" 	\
		    -foreground Black					\
		    -state disabled
	    }
	}

	"ROLLBACK REQUESTED"  {
	    if { $reverseWindow == "" } {
		set isError 1
	    } else { 
		$reverseWindow configure -background yellow2		\
		    -foreground Black					\
		    -state disabled

		$stopWindow configure -foreground Black -state disabled 

		$forwardWindow configure -background "#d9d9d9"		\
		    -foreground Black					\
		    -state disabled
	    }
	}

	"ROLLBACK IN-PROGRESS"  {
	    if { $reverseWindow == "" } {
		set isError 1
	    } else { 
		$reverseWindow configure -background green2		\
		    -foreground Black					\
		    -state disabled

		$stopWindow configure -foreground Black -state disabled 

		$forwardWindow configure -background "#d9d9d9"		\
		    -foreground Black					\
		    -state disabled
	    }
	}

	default {
	    set isError 2
	}
    }


    #
    # Check if there were any errors.
    #

    if { $isError == 1 } {
	#
	# Error 
	#

	if { [ isTrue $displayWarnings ] } {
	    after idle "warningDialog  \"Reverse/Rollback button was NOT found for $stateName EPICS record.\""
	} elseif { $debug == "FULL" } {
	    puts "ERROR: Reverse/Rollback button was NOT found for $stateName EPICS record."
	}
	return
    }  elseif { $isError == 2 &&
		[ isTrue [ $statusObject value stoReq${reqNum}InUse ] ] } {
	if { [ isTrue $displayWarnings ] } { 
	    if { $action == "Queue" || $action == "Prep" || 		\
		     $action == "Write" } {
		if { [ isTrue $displayWarnings ] } {
		    after idle "warningDialog \"Invalid state `$stateValue', should be one of IDLE,  REQUESTED, IN-PROGRESS, or 'ROLLBACK IN-PROGRESS'.\""
		}
	    } else {
		if { [ isTrue $displayWarnings ] } {
		    after idle "warningDialog \"Invalid state `$stateValue', should be one of IDLE,  REQUESTED, or IN-PROGRESS.\""
		}
	    }
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::updateActionState
#
# INVOCATION:
# updateActionState name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the status item being updated.
# (>) alarm     (string)        Alarm status, OK, Warning or Bad
# (>) time      (time)          Time of the update.
# (>) valueList (list)          The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# An action's state value has changed in EPICS so we need to update
# the request display
#
# DESCRIPTION:
# Update the request information for the action whose state value has
# changed.  Update the previous actions information since it's 
# available requests (action/rollback/stop)  is affected by the 
# current action. Also update the next actions information for the
# same reasons.
#
# EXTERNAL VARIABLES:
# CStgMain::actionList	    (list)	List of storage server actions.
# CStgMain::actionUnitList  (list)	List of unit values names.
# CStgMain::displayWarnings (boolean)   TRUE indicates warnings are displayed.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
#
# PRIOR REQUIREMENTS:
# CStgMain::createReqInfo has been executed.
#
# SEE ALSO:
# CStgMain::updateActionButtons
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStgMain::updateActionState {
    name
    alarm
    time
    valueList
} {
    set thisState [ lindex [ split $name . ] 1 ]
    set thisStateValue $valueList
    set thisAction [ extractListItem $actionList $thisState ]
    set reqNum [ extractNums $thisState ]

    set pos [ lsearch $actionList $thisAction ]
    set prev [ expr $pos - 1 ]
    set next [ expr $pos + 1 ]
    if { $pos < 0 || $pos > 5 } {
	#
	# Error 
	#

	if { [ isTrue $displayWarnings ] } {
	    after idle "warningDialog \"$thisAction was not found in the STG's action list as a result the request information may not reflect the values in EPICS\""
	}
	return
    }


    #
    # update this action
    #
    
    set thisUnit stoReq${reqNum}${thisAction}[ lindex $actionUnitList $pos ]
    if { $pos ==  0 } {
	set nextState "stoReq${reqNum}[ lindex $actionList $next ]State"
	set nextUnit "stoReq${reqNum}[ lindex $actionList $next ][ lindex $actionUnitList $next ]"
	set args "\"\" $nextState $nextUnit"
    } elseif { $pos == 5 } {
	set prevState "stoReq${reqNum}[ lindex $actionList $prev ]State"
	set args "$prevState \"\" \"\""
    } else {
	set prevState "stoReq${reqNum}[ lindex $actionList $prev ]State"
	set nextState "stoReq${reqNum}[ lindex $actionList $next ]State"
	set nextUnit "stoReq${reqNum}[ lindex $actionList $next ][ lindex $actionUnitList $next ]"
	set args "$prevState $nextState $nextUnit" 
    }
    eval updateActionButtons [ subst $args ] $thisState \"$thisStateValue\"


    #
    # update previous action information
    #

    set reset false
    if { [ isTrue $displayWarnings ] } {
	set displayWarnings false
	set reset true
    }

    if { $pos == 1 } {
	set value "[ $statusObject value $prevState ]"
	updateActionButtons "" $thisState $thisUnit $prevState "$value" 
    } elseif { $pos != 0 } {
	set value "[ $statusObject value $prevState ]"
	set prePos [ expr $pos - 2 ]
	set prePrevState "stoReq${reqNum}[ lindex $actionList $prePos ]State"
	updateActionButtons $prePrevState $thisState $thisUnit		\
	    $prevState "$value"
    }


    #
    # update next action information
    #

    if { $pos == 4 } {
	set value "[ $statusObject value $nextState ]"
	updateActionButtons $thisState "" "" $nextState "$value"
    } elseif { $pos != 5 } {
	set value "[ $statusObject value $nextState ]"
	set nextPos [ expr $pos + 2 ]
	set nextNextState "stoReq${reqNum}[ lindex $actionList $nextPos]State"
	set nextNextUnit "stoReq${reqNum}[ lindex $actionList $nextPos][ lindex $actionUnitList $nextPos ]"
	updateActionButtons $thisState $nextNextState $nextNextUnit	\
		$nextState "$value"
    }

    if { [ isTrue $reset ] } {
	set displayWarnings true
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStgMain::updateActionUnit
#
# INVOCATION:
# updateActionUnit name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the status item being updated.
# (>) alarm     (string)        Alarm status, OK, Warning or Bad
# (>) time      (time)          Time of the update.
# (>) valueList (list)          The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# An action's unit value has changed in EPICS so we need to update
# the request display
#
# DESCRIPTION:
# Update the request information for the action whose value has
# changed.  Update the previous actions information since it's 
# available requests (action/rollback/stop)  is affected by the 
# current action.
#
# EXTERNAL VARIABLES:
# CStgMain::actionList	    (list)	List of storage server actions.
# CStgMain::actionUnitList  (list)	List of unit values names.
# CStgMain::displayWarnings (boolean)   TRUE indicates warnings are displayed.
# CStgMain::statusObject
#		(CDhgStatus) Instance of the CDhgStatus class.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStgMain::updateActionButtons
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStgMain::updateActionUnit {
    name
    alarm
    time
    valueList
} {
    #
    # Initialize some variables.
    #
    
    set thisUnit [ lindex [ split $name . ] 1 ]
    set reqNum [ string trim [ extractNums $thisUnit ] ]

    set thisAction [ extractListItem $actionList $thisUnit ]
    set thisState "stoReq${reqNum}${thisAction}State"
    set thisStateValue "[ $statusObject value $thisState ]"

    set pos [ lsearch $actionList $thisAction ]    
    set prev [ expr $pos - 1 ]
    set next [ expr $pos +  1 ]


    if { $pos < 0  } {
	#
	# Error 
	#

	if { [ isTrue $displayWarnings ] } {
	    after idle "warningDialog \"$thisAction was not found in the STG's action list as a result the request information may not reflect the values in EPICS\""
	}
    } 

    

    #
    # Make sure the new value is a positive number, If not 
    # don't update the buttons.
    #

    set valueList [ string trim "$valueList" ]
    if { [ string toupper $thisAction ] != "NOTIFY" } {
	if { ! [ isNumber "$valueList" ] } {
	    if { "$valueList" != "-" && "$valueList" != "" } {
		if { [ isTrue $displayWarnings ] } {
		    after idle "warningDialog \"Invalid $thisAction [ lindex $actionUnitList $pos ] `$valueList': should be a positive number.\""
		}
	    }
	    return
	} elseif { "$valueList" < 0 } {
	    if { [ isTrue $displayWarnings ] } {
		after idle "warningDialog \"Invalid $thisAction [ lindex $actionUnitList $pos ] `$valueList': should be a positive number.\""
	    }
	    return
	}
    } else { 
	if { ! [ isFalse "$valueList" ] && ! [ isTrue "$valueList" ] } {
	    if { "$valueList" != "-" && "$valueList" != "" } {
		if { [ isTrue $displayWarnings ] } {
		    after idle "warningDialog \"Invalid $thisAction [ lindex $actionUnitList $pos ] `$valueList': should be a boolean value (yes/no).\""
		}
	    }
	    return
	}
    }


    #
    # Update action buttons for this action ( the one that has changed ).
    #

    if { $pos ==  0 } {
	set nextState "stoReq${reqNum}[ lindex $actionList $next ]State"
	set nextUnit "stoReq${reqNum}[ lindex $actionList $next ][ lindex $actionUnitList $next ]"
	set args "\"\" $nextState $nextUnit"
    } elseif { $pos == 5 } {
	set prevState "stoReq${reqNum}[ lindex $actionList $prev ]State"
	set args "$prevState \"\" \"\""
    } else {
	set prevState "stoReq${reqNum}[ lindex $actionList $prev ]State"
	set nextState "stoReq${reqNum}[ lindex $actionList $next ]State"
	set nextUnit "stoReq${reqNum}[ lindex $actionList $next ][ lindex $actionUnitList $next ]"
	set args "$prevState $nextState $nextUnit" 
    }

    eval updateActionButtons [ subst $args ] $thisState \"$thisStateValue\"


    #
    # Update action buttons for the previous action.
    #

    if { $pos == 0 } {
	return
    } elseif { $pos == 1 } {
	set args "\"\" $thisState $thisUnit"
    } else {
	set prePrevState "stoReq${reqNum}[ lindex $actionList [ expr $pos - 2 ] ]State"
	set args "$prePrevState $thisState $thisUnit"	
    }

   if { $prevState != "" } {
       set prevValue "[ $statusObject value $prevState ]"
       eval updateActionButtons [ subst $args ] $prevState \"$prevValue\"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# versionConfig
#
# INVOCATION:
# config numTokens tokens
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
# Trivial
#
# EXTERNAL VARIABLES:
# CStgMain::version	(string)	: Version of DHS.
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

body	CStgMain::versionConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "version" } {
	error "Expected configuration information for version not [ lindex $tokens 0 ]"
    }
    
    if { $numTokens != 2 } {
	error "Configuration file error, expect 2 tokens for version info but got $numTokens."
    }

    set shortVersion [ lindex $tokens 1 ]
}


# $Id: stgDefaults.tcl,v 1.1.1.1 2002-11-24 20:25:42 brighton Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1998>				(c) 1998
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
# dhsConsole/src/stgDefaults.tcl
#
# PURPOSE:
# Define a number of default values to be used with the CStgMain class.
# All default values are stored in the *global* associative array
# _stgDefaults.
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# None.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.7  1999/06/08 05:17:46  jaeger
# Update short help information.
#
# Revision 1.6  1999/02/25 03:14:34  jaeger
# Added stoRefresh command.
#
# Revision 1.5  1998/07/16 20:00:06  jaeger
# Fixed up the help information.
#
# Revision 1.4  1998/06/22 21:28:02  jaeger
# Added code documentation to bring it up to Gemini standards.
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#


#
# Global font value.
#

set _stgDefaults(entryFont) "-Adobe-Helvetica-Bold-R-Normal-*-12-120-*-*-*-*-*-*"
set _stgDefaults(labelFont) "-Adobe-Helvetica-Bold-R-Normal-*-12-120-*-*-*-*-*-*"



#
# Define the Storage Server subsystem
# This probably should be in the file dhsConsole/src/stgMain.tcl
# but resides here for now.
#
# name          - Long subsystem name 
# status        - Status identifers being monitored
# statusDesc    - Lond description of each status being monitored.
# help          - Short help information for each status being monitored.
# commandName   - List of the subsystems Long command names.
# commands      - The actuall subsystem commands that are executed.
# commandDesc   - Short help information for each command
# commandHelp   - HTML help page associated with the command.
#

set _stgDefaults(sto,name)	"Storage Server"
set _stgDefaults(sto,status) { State Health Debug Sim Desc }
set _stgDefaults(sto,statusDesc) {State: Health: {Debug Level:}		\
	{Simulation Level:} {Health Description:}}
set _stgDefaults(sto,help) {                                      	\
    {Current State of the Storage Server.}				\
    {Health of the Storage Server.}					\
    {Current level of Debugging for the Storage Server.}		\
    {Current Simulation Level for the Storage Server.}			\
    {{bitmap b1} Long health description for the Storage Server.}}
set _stgDefaults(sto,commandName) {Debug Simulation {Reset Health} 	\
     blank Refresh blank Reset Initialize blank Ping Test}
set _stgDefaults(sto,commands) {{debugLevel sto} {simulateLevel sto} 	\
     {resetHealth sto} separator {stoRefresh .cstgmain} separator    	\
     {reset sto} {init sto} separator {ping sto} {test sto}}
set _stgDefaults(sto,commandDesc) {					\
    {Set Debug Level for the Command Server.} 				\
    {Set Simulation Level for Command Server.} 				\
    {Set the Health of the Command Server to 'GOOD'.} blank		\
    {Reread the database values and display them.} blank		\
    {Reset the Command Server.}						\
    {Initailze the Commande Server.} blank				\
    {Ping Storage Server to see if it is responding to commands.}	\
    {Perform tests on the Command Server.}}
set _stgDefaults(sto,commandHelp) {					\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} }



#
# Short help information for the request buttons.
#

set _stgDefaults(sto,requestHelp) {
    { Unique, numerical request identifier. }				\
    { Indicates if the request is a USER or ARCHIVE request. }	\
    { Type of media that is to be made (eg. CD, DVD, Tape).}		\
    { Number of copies that are created for each unit of media. }   \
    { If TRUE then retrieval of files is automatic.}			\
    { The amount of data that can be retrieved ( in Megabytes ).}	\
    { The current state of retrival (IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The amount of data available for queueing (has been retrieved). }	\
    { Current state or queueing ( ROLLBACK IN-PROGRESS, IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The number of units that can be prepared (have been queued). }	\
    { Current state or preparing ( ROLLBACK IN-PROGRESS, IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The number of units that can be written (have been prepared). }	\
    { Current state or writing( ROLLBACK IN-PROGRESS, IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The number of units that can be cleaned up (have been written). }	\
    { The current state of cleaning up (IDLE, REQUESTED, or IN-PROGRESS).} \
    { True if notification can be performed, False otherwise. }		\
    { The current state of notification (IDLE, REQUESTED, or IN-PROGRESS).}}

