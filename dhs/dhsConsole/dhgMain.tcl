# $Id: dhgMain.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# from the use of the software.	fortuit, resultant de l'utili-
# 					sation du logiciel.
#
#***********************************************************************
#
# FILENAME
# dhsConsole/src/dhgMain.itk
#
# PURPOSE:
# Creates the main window of the DHS console, creates the DHS Console.
#
# CLASS NAME(S)
# CDhgMain	: Class for the main window of the DHS console.
#
# METHOD NAME(S)
# PUBLIC
# CDhgMain::constructor		: Constructor for the CDhgMain class
# CDhgMain::destructor		: Destructor for the CDhgMain class
# CDhgMain::resources		: Returns the resources being monitored.
# CDhgMain::subsystems		: Returns the subsystems that are displayed.
#
# PRIVATE
# CDhgMain::appNameConfig	: Gets the application name from the 
#				  configuration file.
# CDhgMain::configResFiller	: Gets called when a configure event occurs,
#				  alters the size of the resource filler.
# CDhgMain::configSubsystemFiller
#				: Gets called when a configure event occurs,
#				  alters the size of the subsystem filler.
# CDhgMain::createDhsStatus	: Creates the section that displays the
#				  overall DHS status information.
# CDhgMain::createResInfo	: Creates the section that displays the
#				  resource overview.
# CDhgMain::createSubsystemInfo	: Creates a subsystem section in the
#				  subsystem overview.
# CDhgMain::repositionRes	: Repositions the resources that are displayed.
# CDhgMain::resourceConfig	: Determines which resources are to be 
#				  monitored.
# CDhgMain::showRes		: Determines whether or not a resource
#				  should be displayed, when its name or
#				  inUse status records change.
# CDhgMain::setupStatusMon		: Setsup the status monitoring details.
# CDhgMain::subsystemConfig	: Determines which subsytems are to be 
#				  monitored.
# CDhgMain::updateResFiller	: Changes the size of the resource filler.
# CDhgMain::updateRes		: Gets called when resource status 
#				  information changes.
# CDhgMain::updateSubsystemFiller
#				: Changes the size of the subsystem filler.
# CDhgMain::versonConfig	: Determines the DHS Console version from
#				  from the configuration file.
#
# GLOBAL PROCEDURE(S)
# cDhgMain	: lower case access method to the CDhgMain class.
#
# $Log: dhgMain.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.33  1999/06/15 16:36:12  jaeger
# Changed number of resources to the number of resources instead of the
# number of the last resource.
#
# Revision 1.32  1999/06/10 21:00:12  jaeger
# Changes made due to changes in status record names (aliases) in the
# dhsConsole.config file.
#
# Revision 1.31  1999/06/08 23:00:08  jaeger
# Use "array size" status records for determining the size of resources.
# Context-sensitive help for the status box now points to specific web
# pages rather then the general dhsConsole one.
#
# Revision 1.30  1999/06/07 18:58:50  jaeger
# Changed all references to EPICS to "status monitor".  Changed the
# Quit menu.  Now contains one item; "exit".
#
# Revision 1.29  1999/05/12 19:18:29  jaeger
# Fixed config file handling so it looks for SUCCESS instead of ERROR.
#
# Revision 1.28  1999/02/01 19:55:28  jaeger
# Delay displaying of Firefox until help is selected.
#
# Revision 1.27  1998/08/05 23:05:20  jaeger
# Fixed bug where resource 10 was not being dynamically displayed.
#
# Revision 1.26  1998/08/05 06:19:42  jaeger
# Fixed bug where resource info was not being displayed properly
# after the status server executed a test command.  Also cleaned
# up the update Resource code.
#
# Revision 1.25  1998/07/20 19:42:33  jaeger
# Fixed typo "dhsConsoe.html".  Changed code to reflect moving bitmaps
# to "images" directory.  Altered logic of "updateRes".  Instead of
# checking if the status record is a "max" or "used" record first, the
# value is checked first (make sure its a number).
#
# Revision 1.24  1998/07/17 17:59:49  jaeger
# Added dynamic monitoring of resources - yahoo!!!
# Added an led at the top so main matches subsystem windows.
# Fixed up short help messages.
#
# Revision 1.23  1998/06/25 20:13:58  jaeger
# Changed to reflect changes in the led, progressbar, and statusBox classes.
# Resource monitoring add 0.49999 on to capacity value instead of 0.5.
#
# Revision 1.22  1998/06/19 22:47:07  jaeger
# Altered to use changed in CLogMonitor class and CDhgStatus class.
#
# Revision 1.21  1998/06/17 21:43:48  jaeger
#  Documented code to bring it up to Gemini standards and altered slightly to
#  use new CStgMain class.
#
# Revision 1.20  1998/06/15 16:20:01  jaeger
# Fixed up help (Point to correct HTML help pages).  Changed to use
# new features in the LED widget, no longer need updateLed method.
#
# Revision 1.19  1998/05/25 20:08:13  jaeger
# Changed the look of the entry boxes (relief is now ridge) so user's
# won't be tempted to edit them.
#
# Revision 1.18  1998/05/22 16:54:49  jaeger
# Fixed up a couple of bugs.  Reading config file message appears only
# when debug level is set to MIN or FULL.  Removed some "puts" commands
# that weren't needed.
#
# Revision 1.17  1998/05/20 21:13:06  jaeger
# Reverted back to a single status acceptor, ie. only one instance of
# the CDhgStatus class.
#
# Revision 1.16  1998/05/15 18:13:55  jaeger
# Removed the "fixed" specification of the statusRouter.
#
# Revision 1.15  1998/05/15 17:42:59  jaeger
# Changes reflect the changes in the dhgStatus class (now can use a
# dhs service status acceptor).
#
# Revision 1.14  1998/05/06 20:50:14  jaeger
# Changed the size of the main window.
#
# Revision 1.13  1998/05/04 20:41:11  jaeger
# Move createToolbar and commandMenu to one of the utility files.
# Changed to a configGen like approach to configuration files.
# Changed command and status objects to reflect the changes made in
# these.  Made updateRes a class method instead of a global procedure.
# Changed from subsystem global variable to _dhgDefaults.
#
# Revision 1.12  1998/04/15 22:37:39  jaeger
# Converted to new dhsCommand handler and new configuration file.
#
# Revision 1.11  1998/04/14 19:18:00  jaeger
# Made sure string variables and [ code ] were double quoated.  Fixed
# after commands.
#
# Revision 1.10  1998/04/08 20:57:13  jaeger
# Changed back to old way of handling EPICS.
#
# Revision 1.9  1998/04/08 20:49:12  jaeger
# Moved updateLed from a global procedure to a class method.
#
# Revision 1.8  1998/03/18 22:37:45  jaeger
# Added command server specification to config file.  Pass in statusObject and
# configObject to dhgCommands instead of subsystemlist.
#
# Revision 1.7  1998/03/06 17:59:36  jaeger
# Changed storage monitoring to resource monitoring.  Moved createAbout
# from dhgMain to utils.
#
# Revision 1.6  1997/12/04 17:21:58  jaeger
# Added documentation.  Added a routine for displaying dataset store info.
#
# Revision 1.5  1997/12/03 20:57:15  jaeger
# final revisiion, checked in for install
#
# Revision 1.4  1997/11/13 23:53:19  jaeger
# Checked in for install
#
# Revision 1.3  1997/10/30 22:18:32  jaeger
# remove "\" from an end of line which was not continued.
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
#***********************************************************************
#+
# CLASS NAME:
# CDhgmain
#
# OPTION(S)
# background    : Specifies the background color of the shell.
# center        : If true, center the dialogue window on startup.
# clientdata    : A hook that clients can use to store data with each widget.
# cursor        : Specifies the mouse cursor to be used.
# helpfont	: Font used for the short help message.
# number        : Set an optional unique instance or clone number for 
#                 the dialog.
# shorthelpwin  : Optionally specify a different TopLevelWidget to 
#                 display short help messages.
# standalone    : Indicates if the window is to be a standalone application.
# transient     : If true and the dialouge is a child of a TopLevel widget.
# withdraw      : If true, withdraw the dialogue window on startup.
#
# PUBLIC METHODS: 
# add_help_button
#               : If any help has been defined then add a button to show it.
# add_menu_short_help
#               : Set the text of the short help message to be displayed 
#                 whenever the mouse enters the menu item with the given 
#                 label (see short_help).
# add_menubar   : Add an option menubar to the top of the dialogue.
# add_menubutton: Add a menu button to the menubar, or reset it's menu to empty
#                 if the menubutton already exists.
# add_menuitem  : Add a menu item of the given type to the given menu and 
#                 arrange to have the given short help message displayed when 
#                 the mouse is over the item.
# add_short_help: Set the text of the short help message to be displayed 
#                 whenever the mouse enters the widget w (see short_help).
# busy          : Run the given tcl command in the scope of this class
#                 while displaying the (blt) busy cursor in the dialogue
# center        : Center this window on the screen. 
# cget          : Standard cget method, returns the value of the specified
#                 option.
# component     : Standard [ incr ] Tk component method.
# config        : See configure
# configure     : Standard Tk method, sets or returns options.
# configure_menubutton
#               : Configure something of the named menubutton.
# get_menu      : Return the path name of the menu for the given menubutton 
#                 label
# get_menubutton: Return the path name of the menubutton for the given 
#                 menubutton label
# hide_windows  : Toggle the visibility of all popup windows.
# isa           : Sets the widget's class.
# list_windows  : Return a list of top level windows that are children of the
#                 dialogue.
# make_short_help
#               : Add a subwindow at the bottom of the screen for short 
#                 help messages
# quit          : Use this method to quit the application if you might want to 
#                 reuse the window later.
# resources	: returns the list of resources being monitored.
# subsystems	: returns the list of subystems being monitored.
# show_help     : Show the help file associated with this window.
# test          : Run the given tcl command and print out any errors.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Creates the main window of the DHS Console
#
#-
#***********************************************************************
#

itk::usual CDhgMain {}
itcl::class CDhgMain {
    inherit util::TopLevelWidget

    #
    # Public methods and data
    #

    public method constructor {args} {}
    public method destructor {}


    public method resources {} { return "$resourceList" }
    public method subsystems {} { return "$activeSubsystems" }

    #
    # Protected methods and data
    #

    # None.

    #
    # Private methods and data
    #

    private method      appNameConfig { numTokens tokens }
    private method 	configResFiller { window }
    private method 	configSubsystemFiller { window }
    private method 	createDhsStatus { window }
    private method 	createResInfo { window }
    private method 	createSubsystemInfo { window subsystem htmlPage side }
    private method    	repositionRes { {when {later}} }
    private method      resourceConfig { numTokens tokens }      
    private method    	showRes { name alarm time valueList }
    private method 	setupStatusMon {}
    private method	subsystemConfig { numTokens tokens }
    private method 	updateResFiller { window }
    private method	updateRes { name alarm time valueList }
    private method 	updateSubsystemFiller { window }
    private method      versionConfig { numTokens tokens }

    private variable activeSubsystems {}	    ;# displayed subsystems.
    private variable appName			    ;# name for IMP
    private variable helpDir ""			    ;# location of help files.
    private variable reconfigSubsystemPlanned ""    ;# after return value.
    private variable reconfigResPlanned ""	    ;# after return value.
    private variable repositionResPlanned ""        ;# after return value.
    private variable resourceList		    ;# resources to monitor.
    private variable shortVersion ""		    ;# short version info.
    private variable visibleRes {} 		    ;# displaye resources.
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Arguments to itk_initialize
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# constructor for the CDhgMain class.
#
# DESCRIPTION:
#
# EXTERNAL VARIABLES:
# (string) dhgDir 	- DHS GUI directory
# (array) _dhgDefaults  - global array that defines all of the commands, 
#			  help messages, and display status for each DHS 
#			  subsystem.
# (string) _helpDir	- Directory where the help files are found.
# (strign) debug	- debug value (NONE, MIN, FULL)
# (array)  dhsStatus	- Contains all of the overall DHS status information.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgMain::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CDhgMain::constructor {
    args
} {
    global ::dhgDir 
    global ::_dhgDefaults 
    global ::_helpDir 
    global ::debug
    global ::dhsStatus
    
    wm withdraw $itk_component(hull)
    
    #
    # Create the configuration Object and open the configuration file.
    #

    uplevel #0 cConfigGen _dhsConfig
    set result [ _dhsConfig configOpen dhsConsole ]

    if { [ lindex $result 0 ] != "SUCCESS" } {
	error "Error: config file error, $result"
    } elseif { $debug == "MIN" || $debug == "FULL" } {
	puts "INFO: Reading DHS Console configuration file:\n\t[ lindex $result 1 ]"
    }


    #
    # Read the version, subsystem, and resource information from
    # the configuration file.
    #

    if { [ _dhsConfig configGet version [ code $this versionConfig ] ] \
             != "SUCCESS" } {
        error "Unable to read version information from configuration file."
    }


    if { [ _dhsConfig configGet resource [ code $this resourceConfig ] ] \
	 != "SUCCESS" } {
	error "Unable to read resource configuration information"
    }

    if { [ _dhsConfig configGet subsystem [ code $this subsystemConfig ] ] \
	 != "SUCCESS" } {
	error "Unable to read subsystem configuration information"
    }


    #
    # Set-up the applications name and kind.
    # 
 
    if { [ _dhsConfig configGet identity [ code $this appNameConfig ] ] \
             != "SUCCESS" } {
        error "Unable to read identity information from configuration file."
    }
    appData add appKind Console
    appData add appName $appName

    set configPath [ file dirname [ lindex $result 1 ] ]
    cd $configPath

#OMIT   appData add configFile "$configPath/CA_CONFIG"
    #
    # Initialize the monitoring of EPICS.
    #

    if { $debug == "MIN" || $debug == "FULL" } {
#OMIT    puts "Reading Channel Access configuration file:\n\t[ appData lookup configFile ]"
    }
    setupStatusMon


    #
    # setup the sequence commands, and the general dhs controle
    # commands
    #

    uplevel #0 cDhsCommands _cDhsCommands 
    uplevel #0 _cDhsCommands cDhsCommandInit _dhgStatus _dhsConfig


    #
    # Add short help to the toplevel widget.
    #

    $this make_short_help
    

    #
    # Create the menubar
    #

    $this add_menubar


    #
    # Set-up the DHS menu
    #

    set commandMenu [ $this add_menubutton "Commands" left ]
    $this component commands configure -underline 0
    $this add_short_help [ $this component commands ]			\
	{ {bitmap b1} DHS menu: Reset DHS, Set Debug level, Set 	\
	     Simulation Level, and Test DHS }

    commandMenu $this $commandMenu dhs _cDhsCommands
    

    #
    # Set-up the Options menu
    #

    set optMenu [ $this add_menubutton "Options" left ]
    $this component options configure -underline 0 
    $this add_short_help [ $this component options ] 			\
	{ {bitmap b1} Options Menu: }


    $this add_menuitem $optMenu command {Log Length}			\
       { {bitmap b1} Set the number of log messages kept.}		\
	-underline 0


    #
    # Set-up the Help menu
    #

    set helpMenu [ $this add_menubutton "Help" left ]
    $this component help configure -underline 0
    $this add_short_help [ $this component help ] { {bitmap b1} Help\
 Menu: About DHS and extended help information.}
    $this add_menuitem $helpMenu command About				\
       { {bitmap b1} Brief description on DHS.}				\
	-command "createAbout \"$shortVersion\""			\
	-underline 0
    $this add_menuitem $helpMenu command Extended...			\
       { {bitmap b1} Detailed help information.}			\
	-command "_helpInfo openUrl dhsConsole.html"			\
	-underline 0


    #
    # Set-up the Quit menu
    #

    set quitMenu [ $this add_menubutton "Quit" right ]
    $this component quit configure -underline 0 
    $this add_short_help [ $this component quit ] 			\
	{ {bitmap b1} Quit Menu: Quit/exit the DHS Console. }
    $this add_menuitem $quitMenu command Exit				\
	{ {bitmap b1} Exit the DHS GUI.}				\
	-command "delete object $this; after idle exit"			\
	-underline 1


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
	    -ledvariable dhsStatus(health)			\
	    -ledwidth 10						
    } {
	keep -background -cursor
    }
    set dhsStatus(health) [ _dhgStatus value dhsHealth ]
    _dhgStatus link dhsHealth dhsStatus(health)

    pack $sfChildsite.led -side top -anchor nw -padx 15 -fill x -expand y
	

    set ledChild [ $sfChildsite.led childsite ]
    label $ledChild.title 						\
	-justify center							\
        -text "Overall Status of DHS"
    pack $ledChild.title -padx 15 -fill x -expand y


    #
    # Create the DHS status display area.
    #

    createDhsStatus $sfChildsite.status
    createToolbar dhsCmd $sfChildsite "$_dhgDefaults(dhs,commandName)"	\
	"$_dhgDefaults(dhs,commands)" "$_dhgDefaults(dhs,commandDesc)"	\
	"$_dhgDefaults(dhs,commandHelp)" $this _cDhsCommands


    #
    # Now that the log window exists we need to configure it.
    #

    set logWin [ $sfChildsite.status component Log ]
    $optMenu entryconfigure 1 						\
	-command "getLogLength $logWin {Dhs Log Message}"


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
    # Create the subsystem status area.  With the subsystem divided in
    # two, half of them appearing on the left side, the other half
    # on the right side..
    #

    itk_component add subsystemFrame {
	iwidgets::labeledwidget $sfChildsite.subsystemFrame			\
	-labelpos n 							\
	-labeltext "Subsystem Overview" 
    } {
	keep -background 						\
	    -cursor							\
	    -foreground							
    }

    $itk_component(subsystemFrame) component hull configure -width 250
    pack $itk_component(subsystemFrame) -anchor n -fill both -expand yes \
	-side top 

    set subChildsite [ $itk_component(subsystemFrame) childsite ]
    bind $subChildsite <Button-3> "_helpInfo openUrl dhsConsole.html#subsystem"
    set i 0
    set frame 0

    foreach subsystem $activeSubsystems {
	#
	# determine the HTML help page for this status item.
	#

	if { $subsystem == "dhs" } {
	    continue
	} else {
	    set htmlPage "dhsConsole.html#subsystem"
	}

	if { [ expr $i % 2 ] != 1 } {
	    #
	    # Create a new frame and add it to the bottom.
	    # Add the subsystem status indicator on the left.
	    #

	    set frame $i
	    frame $subChildsite.$frame
	    pack $subChildsite.$frame -fill x -expand y -pady 2 -padx 5 \
		-side top
	    createSubsystemInfo "$subChildsite.$frame.${subsystem}Led" \
		$subsystem $htmlPage left
	} else {
	    #
	    # Crate a new subsystem indicator on the right.
	    #

	    createSubsystemInfo "$subChildsite.$frame.${subsystem}Led" \
		$subsystem $htmlPage right
	}
	incr i
    }

    if { [ expr $i % 2 ] == 1 } {
	#
	# Odd number of res items so create a filler to make it
	# an even number - ensures that the last one is lined up
	# up properly
	#
	frame $subChildsite.$frame.filler -width 1 -height 1
	pack $subChildsite.$frame.filler -expand y -fill x -padx 4 	\
	    -pady 4 -side right
	bind $subChildsite.$frame.filler <Configure> [ code $this 	\
	  configSubsystemFiller $subChildsite.$frame.filler ]
    }


    #
    # Put separator 
    #

    itk_component add separator2 {
	frame $sfChildsite.separator2 					\
	    -height 2 -relief sunken -bd 1
    } {
	keep -cursor -background
    }
    pack $itk_component(separator2) -expand n -fill x -padx 5 -pady 10	\
	-side top


    #
    # Resource Information
    #

    itk_component add resFrame {
	iwidgets::labeledwidget $sfChildsite.resFrame 				\
	    -labeltext "Resource Overview"				\
	    -labelpos n 						\
    } {
	keep -background 						\
	    -cursor							\
	    -foreground							
    }

    $itk_component(resFrame) component hull configure -width 250
    pack $itk_component(resFrame) -anchor n -expand yes -fill both 	\
	-side top 

    bind $itk_component(resFrame) <Button-3> 				\
	"_helpInfo openUrl dhsConsole.html#subsystem"
    $this add_short_help $itk_component(resFrame)			\
	"Overview of the disk space used by DHS."
    set resChildsite [ $itk_component(resFrame) childsite ]
    createResInfo $resChildsite

    #
    # Make sure all of the DHS status are aligned and display the
    # main window.
    #

    $sfChildsite.status align


    #
    # Create help information.  
    #
    # NOTE:  _helpInfo is created in the global scope this should
    #	     be passed as a parameter, where needed.
    #

    uplevel #0 cHelp _helpInfo $_helpDir/dhsConsole.html
    uplevel #0 _helpInfo setHtmlDir $_helpDir


    #
    # Set the size, title,  and icon details for the Console
    #

    wm iconbitmap $itk_component(hull) @$dhgDir/images/smallGemini.xbm
    wm iconname $itk_component(hull) DHS
    wm geometry $itk_component(hull) 750x590
    wm title $itk_component(hull) "Gemini Data Handling System"
    wm deiconify $itk_component(hull)

    $itk_component(hull) configure -cursor {arrow}
    after idle "_dhgStatus enable"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cDhgMain
#
# INVOCATION:
# cDhgMain pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)    A window path name for a nonexistent window.
# (>) options   (string)        One or more valid option/value pairs.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lowercased acces method to the CDhgMain class.
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
# CDhgMain
# CDhgMain::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc cDhgMain { 
    pathName 
    args
} {
    uplevel CDhgMain $pathName $args
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::destructor
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
# Destructor for the "CDhgMain" class.
#
# DESCRIPTION:
# Make sure the help information (firefox) is destroyed
# and make sure the fillers are not being reconfigured.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgMain::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgMain::destructor {} {
    _dhsConfig configClose

    if { $reconfigSubsystemPlanned != "" } { 
	after cancel $reconfigSubsystemPlanned 
    }

    if { $reconfigResPlanned != "" } { 
	after cancel $reconfigResPlanned 
    }

    if { $repositionResPlanned != "" } {
	after cancel $repositionResPlanned
    }

    if { [ find objects ::.cstgmain ] != "" } {
	delete object .cstgmain
    }

    if { [ find objects ::_helpInfo ] != "" } {
	delete object _helpInfo
    }
}

# ------------------------------------------------------------------
#                       PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::appNameConfig
#
# INVOCATION:
# Object appNameConfig numTokens tokens
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int)   Number of tokens to parse.
# (>) tokens    (list)  List containing the tokens found on a line in
#                               the configuration file. 
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
# CDhgMain::appName
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CdhgMain::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
 
body    CDhgMain::appNameConfig {
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
# CDhgMain::configResFiller
#
# INVOCATION:
# configResFiller
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name) 	The filler's window path name.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To reconfigure the res filler.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# (integer) CDhgMain::reconfigSubsystemPlanned
#
# SEE ALSO:
# CDhgMain::updateResFiller
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgMain::configResFiller {
    window
} {
    if { $reconfigSubsystemPlanned == "" } {
	set reconfigSubsystemPlanned [ after idle 			\
	   "[ code $this updateResFiller $window ]" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::configSubsystemFiller
#
# INVOCATION:
# configSubsystemFiller
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name) 	The filler's window path name.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To reconfigure the subsystem filler.
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
# CDhgMain::updateSubsystemFiller
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgMain::configSubsystemFiller {
    window
} {
    if { $reconfigSubsystemPlanned == "" } {
	set reconfigSubsystemPlanned [ after idle 			\
	     "[ code $this updateSubsystemFiller $window ]" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# main::createDhsStatus
#
# INVOCATION:
# createDhsStatus window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window  	(path name)  Pathname of the DHS status window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To create the section of the DHG that displays the DHS status 
# information.
#
# DESCRIPTION:
# Create a statubox to hold all of the oveall DHS status records.  The
# log message is shown in a combobox while the others are entryfields.
# Short help messages are specified as well as context sensitive help
# is set-up.
#
# EXTERNAL VARIABLES:
# (array) _dhgDefaults	Defines default colours, fonts, command names,
#		        help pages, etc. 
# (array) dhsStatus	Contains the overall status record values.
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

body CDhgMain::createDhsStatus {
    window
} {
    global ::_dhgDefaults ::dhsStatus

    #
    # Create the box that will contains the status information.
    #

    itk_component add dhsStatus {
	cStatusBox $window -statusmargin 30
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
    foreach status $_dhgDefaults(dhs,status) {
	_dhgStatus link dhs$status dhsStatus($status)
	if { $status == "Log" } {
	    $window add cLogMonitor $status _dhgStatus dhsLog		\
		-editable false						\
		-labeltext [ lindex $_dhgDefaults(dhs,statusDesc) $i ]	\
		-relief ridge						\
		-textfont $_dhgDefaults(entryFont)			\
		-unique false
	} else {
	    $window add entryfield $status				\
		-labeltext [ lindex $_dhgDefaults(dhs,statusDesc) $i ]	\
		-relief ridge						\
		-textfont $_dhgDefaults(entryFont)	    
	    #
	    # Make sure it gets an initial value.
	    #
	    [ $window component $status ] insert 0 			\
		"[ _dhgStatus value dhs$status ]"
            [ $window component $status ] configure 			\
		-state disabled				  		\
		-textvariable dhsStatus($status)

	}


	#
	# Short Help information.
	#

	$this add_short_help [ $window component $status ] 		\
		    [ lindex $_dhgDefaults(dhs,help) $i ]


	#
	# Set-up context sensitive help.
	#

	switch -- "$status" {
	    Debug { set htmlPage "dhgDebug.html" }
	    Health { set htmlPage "dhgHealth.html" }
	    Log { set htmlPage "dhgLog.html" }
	    Sim { set htmlPage "dhgSim.html" }
	    State { set htmlPage "dhgState.html" }
	    default {
		set htmlPage "dhg${status}.html#status"
	    }
	}
	bind  [ $window component $status ] <Button-3> 			\
	    "_helpInfo openUrl \"${htmlPage}\""
	incr i
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::createResInfo
#
# INVOCATION:
# createResInfo window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path)		Window path name where the resource
# 				progress bars will be displayed.
#
# PROCEDURE VALUE:
# None. 
#
# PURPOSE:
# To create progress bars for each resource being monitored by by
# DHS.
#
# DESCRIPTION:
# Foreach resource ..
#   determine the HTML page for the res area.
#   create a progress bar (thermometer)
#   If we are display the dataserver's temporary or permanent res
#	area then make the title a button which will display the
#	contents of these areas.
#   Else the title is a label.
#   Find the amount of space available and the amount used and adjust
#      the progress bar appropriately
#   Create short help information.
#   Pack it on the right or left (depending if it is odd or even)
#      If it is on the left a new frame for the two progress bar's
#      is needed.
#
# EXTERNAL VARIABLES:
# (string) dhgDir 	- DHS GUI directory
# (array) _dhgDefaults  - Defines all of the commands, help messages, font, and 
#			   display status for each DHS subsystem.
# (array) resStatus	- Contains the value of all the resource status records.
#
# CDhgMain::resourceList- List of rexources being monitored.
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
body CDhgMain::createResInfo {
    window
} {
    global ::dhgDir 
    global ::resStatus

    set maxRes "[ _dhgStatus value resArraySize ]"


    #
    # Create frames for each row of information
    #
    
    for {set frame 0} {$frame < $maxRes} {incr frame 2} {
        frame $window.f$frame
    }
    
    set i 0

    set resNum 0
    while { $resNum < $maxRes } {
        #
        # Create the resource Information (LED, button, and thermometer).
        #

        itk_component add resLed$resNum {
            cLed $window.led$resNum                             \
                -ledcolor green                                 \
                -ledwidth 10
        } {
            keep -background -cursor
        }
        set childsite [ $window.led$resNum childsite ]


        #
        # Create a button which, when activated displays the
        # detailed resource information.
        #

        _dhgStatus link res${resNum}Name resStatus(name$resNum)
        set cmd "displayResource $window $resNum _dhgStatus $itk_component(hull)
"
        button $childsite.button                                        \
            -command "$cmd"                                             \
            -pady 0                                                     \
            -textvariable resStatus(name$resNum)                        \
            -width 10
        pack $childsite.button -anchor nw -expand y -fill x -padx 5 -side left


        #
        # Create the slider.
        #

        itk_component add res${resNum} {
            cProgressBar $childsite.slider                              \
                -from 0                                                 \
                -idlecolor blue                                         \
                -labelpos w                                             \
                -length 200                                             \
                -orient horizontal                                      \
                -showvalue false                                        \
                -state idle                                             \
                -valueforeground blue
        } {
            keep -background -cursor -foreground
        }
        pack $childsite.slider -side left -fill both -expand y -padx 5
        $childsite.slider config -width [ expr [ winfo reqheight        \
             $childsite.button  ] - 8 ]



        #
        # Create the context sensitive help and short help
        #
                
        set type [ _dhgStatus value res${resNum}Type ]

        $this add_short_help $window.led$resNum                         \
            "Resource status: green indicated GOOD, yellow indicates WARNING, re
d indicates BAD, and dark gray indicates NOT KNOWN"
        $this add_short_help $childsite.button                          \
            "{bitmap b1} Display detailed resource information."
        $this add_short_help $childsite.slider                          \
            "Slider position indicates the resource usage, as a percentage."

        bind $window.led$resNum <Button-3>                              \
            "_helpInfo openUrl dhsConsole.html#resource"
        bind $childsite.button <Button-3>                               \
            "_helpInfo openUrl dhsConsole.html#resource"
        bind $childsite.slider <Button-3>                               \
            "_helpInfo openUrl dhsConsole.html#resource"

        
        #
        # Call updateRes to initialize the value on the bar
        #
        
        updateRes resourceSAD.res${resNum}Max "" "" 			\
            "[ _dhgStatus value res${resNum}Max ]"
        updateRes resourceSAD.res${resNum}Used "" "" 			\
            "[ _dhgStatus value res${resNum}Used ]"
        if { [ simMode ] } {
                $childsite.slider configure -idlecolor gray70
                $window.led$resNum configure -ledcolor gray70
        }


        #
        # Check to see if this one gets displayed.  It gets displayed if
        # it isn't already displayed and either 
        #       - we are in simulation mode
        #       - or the resource is in use and it is one of the
        #         resources specified in the configuration file.
        #

        set inUse "[ _dhgStatus value res${resNum}InUse ]"
	set name "[ _dhgStatus value res${resNum}Name ]"
        set lowerName [ string tolower $name ]  

        if { [ simMode ]  ||  
             ( [ isTrue $inUse ] && [ lsearch $resourceList $lowerName ] != -1 
             && [ lsearch $visibleRes $lowerName ] == -1 ) } {
            lappend visibleRes $resNum
        }

        incr resNum
    }
    repositionRes now
        
    for {set resNum 0} {$resNum < $maxRes} {incr resNum} {
        _dhgStatus proc res${resNum}Max "[ code $this updateRes ]"
        _dhgStatus proc res${resNum}Used "[ code $this updateRes ]"
        _dhgStatus proc res${resNum}InUse "[ code $this showRes ]"
        _dhgStatus proc res${resNum}Name "[ code $this showRes ]"
    }    
}


#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::createSubsytemInfo
#
# INVOCATION:
# createLed window subsystem htmlPage side
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window        (path name)	    Window pathname for the new LED.
# (>) subsystem     (string)	    Subsystem identifier.
# (>) htmlPage	    (URL)	    HTML help page for the subsystem.
# (>) side	    (right|left)    Which side it gets packed on.
#
# PROCEDURE VALUE:
# window pathname.
#
# PURPOSE:
# To create an LED, subsystem button, and state description for a give
# subsystem
#
# DESCRIPTION:
# 1. create the LED for the subystem.
# 2. set-up the constext sensitive help.
# 3. add short help information
# 4. Add the subsystem button and the state display
#
# EXTERNAL VARIABLES:
# (array) _dhgDefaults	Defines default color, fonts, commands, help messages,
#			etc.
# (array) cmdStatus	Contains the values of all the Command Server status
#			records
# (array) dtsStatus	Contains the values of all the Data Server status
#			records
# (array) hisStatus	Contains the values of all the History Server status
#			records
# (array) qlsStatus	Contains the values of all the Quick Look Server status
#			records
# (array) staStatus	Contains the values of all the Status Server status
#			records
# (array) stoStatus	Contains the values of all the Storage Server status
#			records
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

body CDhgMain::createSubsystemInfo {
    window
    subsystem
    htmlPage
    side
} {
    global ::_dhgDefaults
    global ::${subsystem}Status

    #
    # Create the subsystem LED
    #


    itk_component add $subsystem {
	cLed $window							\
	    -colorassoc {{green good} {yellow warning} {red bad} {grey70 *}} \
	    -ledvariable "${subsystem}Status(health)"	\
	    -ledwidth 10					
    } {
	keep -background -cursor -ledcolor
    }
    _dhgStatus link ${subsystem}Health ${subsystem}Status(health) 


    #
    # Set-up the context sensitive help
    #

    bind $window <Button-3> "_helpInfo openUrl $htmlPage"


    #
    # Add short help information.
    #

    $this add_short_help $window					\
	"Status of the $_dhgDefaults($subsystem,name): green is GOOD, yellow is WARNING, red is BAD, and dark gray means NOT KNOWN"
    pack $window -fill both -expand y -side $side


    #
    # Add the subsystem button.
    #

    set childsite [ $window childsite ]
    button $childsite.button 						\
	-text [ string toupper $subsystem ]				\
	-width 4
    if { $subsystem == "sto" } {
	set cmd "cStgMain .cstgmain {_dhgStatus} {_dhsConfig}"
    } elseif { $subsystem == "dts" } {
	set cmd "displaySubsystem .$subsystem $subsystem \"$shortVersion\" _cDtsCommands"
    } else {
	set cmd "displaySubsystem .$subsystem $subsystem \"$shortVersion\" _cDhsCommands"
    }
    $childsite.button config -command "$cmd"

    $this add_short_help $childsite.button 				\
	"{bitmap b1} Activate to see detailed information on the $_dhgDefaults(${subsystem},name)"


    #
    # Add and entry field for displaying the subsystems state.
    #

    entry $childsite.entry						\
	-cursor {}							\
	-font $_dhgDefaults(entryFont)					\
	-relief ridge						\
	-width 30
    $this add_short_help $childsite.entry 				\
	"Current state of the $_dhgDefaults($subsystem,name)"

    _dhgStatus link ${subsystem}State ${subsystem}Status(state)
    $childsite.entry insert 0 "[ _dhgStatus value ${subsystem}State ]"
    $childsite.entry configure -state disabled			\
	-textvariable ${subsystem}Status(state)


    #
    # set-up context sensitive help for the button and entry.
    #

    bind $childsite.button <Button-3> "_helpInfo openUrl $htmlPage"
    bind $childsite.entry <Button-3> "_helpInfo openUrl $htmlPage"

    pack $childsite.button -side left -fill x -expand y -padx 5
    pack $childsite.entry -side left -fill both -expand y -padx 2
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::repositionRes
#
# INVOCATION:
# repositionRes ?when?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) when      (now|later)     Defines when the repostion is to occur.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To redisplay the the resources.
#
#
# DESCRIPTION:
# Remove all resources from the display.  Then display all resources
# listed in the variable "visibleRes".  If there are an odd number of
# resources then display the filler.
#
# EXTERNAL VARIABLES:
# CDhgMain::repositionResPlanned
# CDhgMain::visibleRes
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

body    CDhgMain::repositionRes {
    {when {later}}
} {
    #
    # Find out when the table is to be resorted.
    #

    if { $when == "later" } {
        if { $repositionResPlanned == "" } {
            set repositionResPlanned [ after idle               \
                    "[ code $this repositionRes now ]" ]
        } 
        return
    } elseif { $when != "now" } {
        error "bad option \"$when\":whould be now or later."
    }
   

    #
    # Remove all of the resources currently being displayed.
    #

    set maxRes "[ _dhgStatus value resArraySize ]"
    if { ! [ isNumber $maxRes ] } {
        set maxRes 11
    }
    set resChildsite [ $itk_component(resFrame) childsite ]

    for {set resNum 0} {$resNum < $maxRes} {incr resNum} {
        pack forget $resChildsite.led$resNum
        if { [ expr $resNum % 2 ] == 0 } {
            pack forget $resChildsite.f$resNum
        }
    }
    pack forget $resChildsite.filler


    #
    # Now display all those that are to be displayed.
    #

    set pos 0
    foreach resNum $visibleRes {
        #
        # Add a new row (frame) if necessary.
        #

        set frameNum [ expr ( $pos / 2 ) * 2 ]
        if { [ expr $pos % 2 ] == 0 } {
            pack $resChildsite.f$frameNum -fill x -expand y -pady 2     \
                -padx 5 -side top
        }


        #
        # Pack the led
        #

        if { [ expr $pos % 2 ] == 0 } {
            pack $resChildsite.led$resNum -expand y -fill x     \
                -in $resChildsite.f$frameNum -padx 4 -pady 4 -side left 
        } else {
            pack $resChildsite.led$resNum -expand y -fill x     \
                -in $resChildsite.f$frameNum -padx 4 -pady 4 -side right 
        }

        incr pos
    }


    #
    # If there are an odd number of resources add a "filler" so
    # they all appear to be the same size.
    #

    if { [ expr [ set pos [ llength $visibleRes ] ] % 2 ] == 1 } {
        if { ! [ winfo exists $resChildsite.filler ] } {
            frame $resChildsite.filler -width 1 -height 1
            
            bind $resChildsite.filler <Configure>                       \
            [ code $this configResFiller $resChildsite.filler ]
        }

        set frameNum [ expr ( $pos / 2 ) * 2 ]
        pack $resChildsite.filler -expand y -fill x                     \
            -in $resChildsite.f$frameNum                                \
            -padx 4 -pady 4 -side right

    }

    set repositionResPlanned ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::resourceConfig
#
# INVOCATION:
# resourceConfig numTokens tokens
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
# CDhgMain::resourceList
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

body	CDhgMain::resourceConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "resource" } {
	error "Expected to read resource information from config file not [ lindex $tokens 0 ]"
    }
    
    if { $numTokens != 2 } {
	error "Configuration file error, expect 2 tokens for resource configuration but got $numTokens."
    }
    lappend resourceList "[ string tolower [ lindex $tokens 1 ] ]"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::showRes
#
# INVOCATION:
# showRes name alarm time valueList
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
# To remove or add a resource, dynamically, to the resource overiew
# on the main window.
#
# This method is called whenever a resource name or inUse status 
# record changes value.
#
# DESCRIPTION:
# Find the name and inUse value for the resource that has changed.
#
# If the name corresponds to one that is to be monitored, it is
# in use, and it is not visible then display it.
#
# If the name does match any of the resources to be monitored or
# is not in use, but is currently visible then remove it.
#
# EXTERNAL VARIABLES:
# CDhgMain::visibleRes
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhgMain::repositionRes
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body    CDhgMain::showRes {
    name
    alarm
    time
    valueList
} {
    if { [ regexp -nocase -- {(res)([0-9]|10|11|12)(InUse)$} $name ] } {
        #
        # Status Record is an inUse record
        #

        set inUse $valueList
        set resNum [ extractNums [ lindex [ split $name . ] 1 ] ]
        if { [ llength $resNum ] != 1 } {
            #
            # Should only be one number in the name of the status record.
            #


            return
        }

        set name "[ _dhgStatus value res${resNum}Name ]"
        set lowerName [ string tolower $name ]  
    } elseif { [ regexp -nocase -- {(res)([0-9]|10|11|12)(name)$} $name ] } {
        set resNum [ extractNums  [ lindex [ split $name . ] 1 ] ]
        if { [ llength $resNum ] != 1 } {
            #
            # Should only be one number in the name of the status record.
            #

            return
        }

        set name $valueList
        set lowerName [ string tolower $name ]  
        set inUse "[ _dhgStatus value res${resNum}InUse ]"
    } else {
	# 
	# Nothing needs changing so return
	#

	return
    }


    if { [ simMode ]  ||  ( [ isTrue $inUse ] && 
            [ lsearch $resourceList $lowerName ] != -1 &&
            [ lsearch $visibleRes $resNum ] == -1 ) } {
        #
        # Add the resource to the display.
        #

        lappend visibleRes $resNum
        repositionRes 
    }   elseif { ( [ isFalse $inUse ] ||                                \
           [ lsearch $resourceList $lowerName ] == -1 ) &&              \
           [ set pos [ lsearch $visibleRes $resNum ] ] != -1 }  {
        #
        # Remove the resource to the display.
        #

        if { [ llength $visibleRes ] == 1 } {
            set visibleRes {} 
        } else {
            set visibleRes [ lreplace $visibleRes $pos $pos ]
        }
        repositionRes 
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::setupStatusMon
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
# To setup the monitoring of status record values for the DHS console.
#
# DESCRIPTION:
# Create an instance of the dhgStatus monitoring class.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgMain::setupStatusMon {
} {
    #
    # Set up the DHS status record monitoring.
    # 

    uplevel #0 cDhgStatus _dhgStatus "$activeSubsystems"
    _dhgStatus disable

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::subsystemConfig
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
# Trivial
#
# EXTERNAL VARIABLES:
# CDhgMain::activeSubsystems
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

body	CDhgMain::subsystemConfig {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] != "subsystem" } {
	error "Expected to read subsystem information from config file not [ lindex $tokens 0 ]"
    }
    
    if { $numTokens != 2 } {
	error "Configuration file error, expect 2 tokens for subsystem configuration but got $numTokens."
    }
    lappend activeSubsystems [ string tolower [ lindex $tokens 1 ] ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# updateRes
#
# INVOCATION:
# updateRes name alarm time valueList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name	(string)	Name of the status item being updated.
# (>) alarm	(string)	Alarm status, OK, Warning or Bad
# (>) time	(time)		Time of the update.
# (>) valueList	(list)		The new value.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To set the space, usage, capacity and avalability values for the
# data server disk space areas ( permanent and temporary ).
#
# This should be a method in the dhgStatus class but since ocswish
# does not seem to allow status accpetors to "proc" with a 
# class method a global procedure has been created.  This will
# adventually become part of the dhgStatus class.
#
# All of the values, except capacity, are 8 character strings, with
# the values right aligned.
#
# DESCRIPTION:
# 1. Determine which resource bar needs updating.
# 2. Find the max and used value for the resource, if both values
#    are numerical then update the progress bar - otherwise change
#    the health indicator to "bad"
# 3. Update the capacity and available values.
#
# EXTERNAL VARIABLES:
# (array) resStatus	Array for monitoring resource status info.
#
# PRIOR REQUIREMENTS:
# An instance of CDhStatus exists..
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhgMain::updateRes {
    name
    alarm
    time
    valueList
} {
    global ::_dhgDefaults ::resStatus

    #
    # Find the subsystem, resource area and which status value has
    # been altered.
    #

    set valueList [ string trim $valueList ]
    set name [ lindex [ split $name . ] 1 ]
    set subsystem [ string range $name 0 2 ]
    if { $subsystem != "res" } {
	#
	# Not a resource value so return.
	#

	return
    }

    set resNum [ extractNums $name ]
    if { [ llength $resNum ] > 1 } {
	error "Unable to determine the number of resources."
    }

    set max maxLed${resNum}
    set used usedLed${resNum}
    set avail avail$resNum
    set cap cap$resNum

    set current [ extractListItem { Max Used Avail Cap } $name ]
    if { "$current" != "Max" && "$current" != "Used" } {
	return
    }
    set current [ string tolower "$current" ]


    #
    # Make sure the new value is an integers.
    #

    if { ![ isNumber "$valueList" ] } {
	#
	# Change slider colour to gray and led to red.
	#

	if { [ array names itk_component resLed$resNum ] != "" } {
	    set ledWindow $itk_component(resLed$resNum)
	    [ $ledWindow childsite].slider config -idlecolor gray70 	\
		-variable ""
	    $ledWindow config -ledcolor red
	}

	    
	#
	# Change all numerical values to Unknown
	#

	if { $current == "max" } {
	    set resStatus($max) Unknown
	} elseif { $current == "used" } {
	    set resStatus($used) Unknown
	}
	set resStatus($cap) Unknown
	set resStatus($avail) Unknown
	return
    } 


    #
    # update the global resStatus values 
    #

    if { "$current" == "max" } {
	set resStatus($max) "$valueList"
    } elseif { "$current" == "used" } {
	set resStatus($used) "$valueList"
    }


    #
    # If one of the "used" or "max" array items don't exist 
    # don't update the capacity nor the availability values.
    #

    if { ! [ info exists resStatus($max) ] } {
	return    
    } elseif { ! [ info exists resStatus($used) ] } {
	return
    }


    #
    # Update the capacity and availability values.
    #
	
    set maxVal [string trim $resStatus($max) ]
    set usedVal [ string trim $resStatus($used) ]

    if { [ isNumber "$usedVal" ] && [ isNumber "$maxVal" ] } {
	#
	# Make sure slider is blue and led green.
	#
    	
	if { [ array names itk_component resLed$resNum ] != "" } {
	    set ledWindow $itk_component(resLed$resNum) 
	    $ledWindow config -ledcolor green 
	    [ $ledWindow childsite].slider config -idlecolor blue 	\
		-to $maxVal -value $usedVal -variable resStatus($used)
	}

	#
	# Make sure used value is within range, between 0 and max.
	#

	if { $usedVal > $maxVal } {
	    set usedVal $maxVal
	    $ledWindow config -ledcolor yellow
	} elseif { $usedVal < 0 } {
	    set usedVal 0
	    $ledWindow config -ledcolor yellow
	}


	#
	# Calculate the available and capacity values.
	#

	set resStatus($avail) [ expr $maxVal - $usedVal ]	
	if { $maxVal == 0 } {

	    set resStatus($cap) "0%"
	} else {
	    set val [ expr round( ( ( "${usedVal}.0" / "$maxVal" ) * 100 ) \
				      + 0.49999 ) ]
	    set resStatus($cap) "${val}%"
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::updateResFiller
#
# INVOCATION:
# updateResFiller window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name) 	The filler's window path name.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the size of the subystem filler.
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
# CDhgMain::configResFiller
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDhgMain::updateResFiller {
    window
} {
    #
    # Find the frame the filler is in.
    # 

    set parent [ winfo parent [ winfo parent $window ] ]


    #
    # Set the width to be half the size of the parent minus the padding.
    # 

    set fillerWidth [ expr ( [ winfo reqwidth $parent ] / 2 ) - 10 ]

    $window configure -width $fillerWidth -height 1
    set reconfigResPlanned ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::updateSubsystemFiller
#
# INVOCATION:
# updateSubsystemFiller window
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	(path name) 	The filler's window path name.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To update the size of the subystem filler.
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
# CDhgMain::configSubsystemFiller
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#


body 	CDhgMain::updateSubsystemFiller {
    window
} {
    #
    # Find the frame the filler is in.
    # 

    set parent [ winfo parent $window ]


    #
    # Set the width to be half the size of the parent minus the padding.
    # 

    set fillerWidth [ expr ( [ winfo reqwidth $parent ] / 2 ) - 6 ]


    $window configure -width $fillerWidth -height 1
    set reconfigSubsystemPlanned ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhgMain::versionConfig
#
# INVOCATION:
# CDhgMain:versionConfig numTokens tokens
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int)   Number of tokens to parse.
# (>) tokens    (list)  List containing the tokens found on a line in
#                               the configuration file. 
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
# CStoCommands::hostname
# CStoCommands::storagServer
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
 
body    CDhgMain::versionConfig {
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

# $Id: dhgDefaults.tcl,v 1.1.1.1 2002-11-24 20:25:38 brighton Exp $
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
# dhsConsole/src/dhgDefaults.tcl
#
# PURPOSE:
# Define a number of default values to be used with the CDhgMain class.
# All default values are stored in the *global* associative array
# _dhgDefaults.
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
# Revision 1.12  1999/02/25 03:14:34  jaeger
# Added stoRefresh command.
#
# Revision 1.11  1999/01/26 06:15:27  jaeger
# Corrected grammar of "flush" cmd short-help message.
#
# Revision 1.10  1998/08/14 21:47:54  jaeger
# Chages reflect flush command now having a subsystem list.
#
# Revision 1.9  1998/07/16 19:59:30  jaeger
# Fixed up the help information.
#
# Revision 1.8  1998/06/17 21:49:18  jaeger
# Documented code to bring it up to Gemini standards.
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
# Global font value.
#

set _dhgDefaults(entryFont) "-Adobe-Helvetica-Bold-R-Normal-*-12-120-*-*-*-*-*-*"

#
# Define the subsytems of dhs
# This probably should be in the file dhsConsole/src/dhgMain.tcl
# but resides here for now.
#
# name 		- Long subsystem name 
# status	- Status identifers being monitored
# statusDesc	- Lond description of each status being monitored.
# help		- Short help information for each status being monitored.
# commandName	- List of the subsystems Long command names.
# commands	- The actuall subsystem commands that are executed.
# commandDesc	- Short help information for each command
# commandHelp	- HTML help page associated with the command.
#


#
# DHS Command Server
#

set _dhgDefaults(cmd,name) 		{Command Server}
set _dhgDefaults(cmd,status) {State Health Debug Sim Desc}
set _dhgDefaults(cmd,statusDesc) {State: Health: {Debug Level:} 	\
    {Simulation Level:} {Health Description:} }
set _dhgDefaults(cmd,help) {						\
    {Current State of the Command Server.}				\
    {Health of the Command Server.}					\
    {Current level of Debugging for the Command Server.}		\
    {Current Simulation Level for the Command Server.}			\
    {{bitmap b1} Long health description for the Command Server.}}
set _dhgDefaults(cmd,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(cmd,commands) {{debugLevel cmd} {simulateLevel cmd} 	\
    {resetHealth cmd} separator {reset cmd} {init cmd} separator 	\
    {ping cmd} {test cmd}}
set _dhgDefaults(cmd,commandDesc) {					\
    {Set Debug Level for the Command Server.} 				\
    {Set Simulation Level for Command Server.} 				\
    {Set the Health of the Command Server to 'GOOD'.} blank		\
    {Reset the Command Server.}						\
    {Initailze the Command Server.} blank				\
    {Ping the Command Server, check to see if it is running.}		\
    {Perform tests on the Command Server.}}
set _dhgDefaults(cmd,commandHelp) {					\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"} }


#
# DHS, overall inforamation
#

set _dhgDefaults(dhs,name) {Data Handling System}
set _dhgDefaults(dhs,status) {State Health Debug Sim Log}
set _dhgDefaults(dhs,statusDesc) {State: Health: 			\
     {Debug Level:} {Simulation Level:} {Log Message:}}
set _dhgDefaults(dhs,help) {						\
    {Current State of the Data Handling System.}			\
    {Health of the Data Handling System.}				\
    {Current level of Debugging for the Data Handling System.}		\
    {Current Simulation Level for the Data Handling System.}		\
    {{bitmap b1} DHS history log message.}}
set _dhgDefaults(dhs,commandName) {{Debug} {Simulation} {Reset Health}	\
     blank {Reset} {Initialize}  blank Ping Test blank Shutdown}
set _dhgDefaults(dhs,commands) {{debugLevel dhs} {simulateLevel dhs} 	\
    resetHealth separator {reset} {init}  separator {ping} {test} 	\
    separator dhsExit}
set _dhgDefaults(dhs,commandDesc) {					\
    {Set the Debug Level for DHS.}					\
    {Set the Simulation Level for DHS.} 				\
    {Set the Health of DHS to 'GOOD'.} blank				\
    {Reset the ENTIRE Data Handling System.}				\
    {Re-Initailize the ENTIRE Data Handling System.} blank		\
    {Ping all DHS subsystems to see if they are responding.}		\
    {Perform tests on the DHS.} blank					\
    {Shutdown the entire Data Handling System.}}
set _dhgDefaults(dhs,commandHelp) {					\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} blank			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} blank			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} blank			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} }

#
# DHS Data Server
#

set _dhgDefaults(dts,name) {Data Server}
set _dhgDefaults(dts,status) {State Health Debug Sim Desc Inc}
set _dhgDefaults(dts,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} {Number of Incomplete:} }
set _dhgDefaults(dts,help) {						\
    {Current State of the Data Server.}					\
    {Health of the Data Server.}					\
    {Current level of Debugging for the Data Server.}			\
    {Current Simulation Level for the Data Server.}			\
    {{bitmap b1} Long health description for the Data Server.} 		\
    {Number of datasets that have not been fully processedd.} }
set _dhgDefaults(dts,commandName) {{Debug} {Simulation} {Reset Health} 
    blank {Datasets} blank {Reset} {Initialize} blank Ping Test}
set _dhgDefaults(dts,commands) {{debugLevel dts} {simulateLevel dts} 	\
    {resetHealth dts} separator {dtsDatasetInfo} separator {reset dts} \
    {init dts} separator {ping dts} {test dts}}
set _dhgDefaults(dts,commandDesc) {					\
    {Set Debug Level for the Data Server.} 				\
    {Set Simulation Level for Data Server.} 				\
    {Set the Health of the Data Server to 'GOOD'.} blank		\
    {View the list of datasets the Data Server knows of.} blank		\
    {Reset the Data Server.}						\
    {Initailze the Data Server.} blank				\
    {Ping the Data Server to see if it is responding to commands.}	\
    {Perform tests on the Data Server.}}
set _dhgDefaults(dts,commandHelp) {					\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgDts.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"} }


#
# DHS History Server
#

set _dhgDefaults(his,name) {History Server}
set _dhgDefaults(his,status) {State Health Debug Sim Desc}
set _dhgDefaults(his,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} }
set _dhgDefaults(his,help) {						\
    {Current State of the History Server.}				\
    {Health of the History Server.}					\
    {Current level of Debugging for the History Server.}		\
    {Current Simulation Level for the History Server.}			\
    {{bitmap b1} Long health description for the History Server.}}
set _dhgDefaults(his,commandName) {Debug Simulation {Reset Health}      \
    blank {Save History Log} blank Reset Initialize blank Ping Test}
set _dhgDefaults(his,commands) {{debugLevel his} {simulateLevel his} 	\
    {resetHealth his} separator {hisLogWrite} separator {reset his} 	\
    {init his} separator {ping his} {test his}}
set _dhgDefaults(his,commandDesc) {					\
    {Set Debug Level for the History Server.} 				\
    {Set Simulation Level for History Server.} 				\
    {Set the History Server's Health to 'GOOD'} blank			\
    {Save history Log to Archive.} blank				\
    {Reset the History Server.}						\
    {Initailze the Storage Server.} blank				\
    {Ping the History Server to see if it is responding to commands.}	\
    {Perform self tests on the History Server.}}
set _dhgDefaults(his,commandHelp) {					\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgHis.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"} }



#
# DHS On-line Data Processing  Server
#

set _dhgDefaults(olp,name) "On-line Data Processing Server"
set _dhgDefaults(olp,status) {State Health Debug Sim Desc Over \
     Size Num Active}
set _dhgDefaults(olp,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:}  {Health Description:} Overflow 		\
	{Array Size:} {Active Recipe Configurations:} 			\
         {Executing Recipe Configurations:}}
set _dhgDefaults(olp,help)						\
    {{Current State of the On-line Data Processing Server.}		\
    {Health of the On-line Data Processing Server.}			\
    {Current level of Debugging for the On-line Data Processing Server.} \
    {Current Simulation Level for the On-line Data Processing Server.}	\
    {{bitmap b1} Long health description for the On-line Data Processing Server.}	\
    {True if the number of current OLDP jobs exceeds the number allowed.} \
    {Size of teh OLDP Recipe configuration status record arrays.}	\
    {Number of Recipe Configurations currently active.}			\
    {Number of Recipe Configurations currently being processed.}}
set _dhgDefaults(olp,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(olp,commands) {{debugLevel olp} {simulateLevel olp} 	\
    {resetHealth olp} separator {reset olp} {init olp} separator 	\
    {ping olp} {test olp}}
set _dhgDefaults(olp,commandDesc) {					\
    {Set Debug Level for the On-line Data Processing Server.} 		\
    {Set Simulation Level for On-line Data Processing Server.} 		\
    {Set the Health of the On-line Data Processing Server to 'GOOD'.} 	\
    blank {Reset the On-line Data Processing Server.}			\
    {Initailze the On-line Data Processinge Server.} blank		\
    {Ping the On-line Data Processing Server.}				\
    {Perform tests on the On-line Data Processing Server.}}
set _dhgDefaults(olp,commandHelp) {					\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"} blank			\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"} blank			\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"} }


#
# DHS Quick Look Server
#

set _dhgDefaults(qls,name) "Quick Look Server"
set _dhgDefaults(qls,status) {State Health Debug Sim Desc}
set _dhgDefaults(qls,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} }
set _dhgDefaults(qls,help) {						\
    {Current State of the Quick Look Server.}				\
    {Health of the Quick Look Server.}					\
    {Current level of Debugging for the Quick Look Server.}		\
    {Current Simulation Level for the Quick Look Server.}		\
    {{bitmap b1} Long health description for the Quick Look Server.} }
set _dhgDefaults(qls,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(qls,commands) {{debugLevel qls} {simulateLevel qls} 	\
    {resetHealth qls} separator {reset qls} {init qls} separator 	\
    {test qls}}
set _dhgDefaults(qls,commands) {{debugLevel qls} {simulateLevel qls} 	\
    {resetHealth qls} separator {reset qls} {init qls} separator 	\
    {ping qls} {test qls}}
set _dhgDefaults(qls,commandDesc) {					\
    {Set Debug Level for the Quick Look Server.} 			\
    {Set Simulation Level for Quick Look Server.} 			\
    {Set the Health of the Quick Look Server to 'GOOD'.} blank		\
    {Reset the Quick Look Server.}					\
    {Initailze the Quick Looke Server.} blank				\
    {Ping the Quick Look Server to see if it is responding to commands.} \
    {Perform tests on the Quick Look Server.}}
set _dhgDefaults(qls,commandHelp) {					\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"} }


#
# DHS Synchronized Data Server
#

set _dhgDefaults(sdp,name) "Synchronized Data Processing Server"
set _dhgDefaults(sdp,status) {State Health Debug Sim Desc Over Size Num}
set _dhgDefaults(sdp,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} Overflow: 		\
	{Array Size:} {Number of Jobs:}}
set _dhgDefaults(sdp,help)  {						\
    {Current State of the Synchronized Data Processing Server.}		\
    {Health of the Synchronized Data Processing Server.}		\
    {Current level of Debugging for the Synchronized Data Processing Server.} \
    {Current Simulation Level for the Synchronized Data Processing Server.} \
    {{bitmap b1} Long health description for the Synchronized Data Processing Server.} \
    {True if the numbmer of current SDP jobs exceeds the number allows.} \
    {Number of jobs monitored by EPICS.}				\
    {Number of jobs being processed by SDP.}}
set _dhgDefaults(sdp,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(sdp,commands) {{debugLevel sdp} {simulateLevel sdp} 	\
    {resetHealth sdp} separator {reset sdp} {init sdp} separator 	\
    {ping sdp} {test sdp}}
set _dhgDefaults(sdp,commandDesc) {					\
    {Set Debug Level for the Synchronized Data Processing Server.} 	\
    {Set Simulation Level for Synchronized Data Processing Server.}	\
    {Set the Health of the Synchronized Data Processing Server to 'GOOD'.} \
    blank {Reset the Synchronized Data Processing Server.}		\
    {Initailze the Synchronized Data Processinge Server.} blank		\
    {Ping the Synchronized Data Processing Server.}			\
    {Perform tests on the Synchronized Data Processing Server.}}
set _dhgDefaults(sdp,commandHelp) {					\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"} blank			\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"} blank			\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"} }


#
# DHS Status Server
#

set _dhgDefaults(sta,name) "Status Server"
set _dhgDefaults(sta,status) {State Health Debug Sim Desc}
set _dhgDefaults(sta,statusDesc) {State: Health: {Debug Level:} \
	{Simulation Level:} {Health Description:}}
set _dhgDefaults(sta,help) {						\
    {Current State of the Status Server.}				\
    {Health of the Status Server.}					\
    {Current level of Debugging for the Status Server.}			\
    {Current Simulation Level for the Status Server.}			\
    {{bitmap b1} Long health description for the Status Server.}}
set _dhgDefaults(sta,commandName) {Debug Simulation {Reset Health}	\
    blank Flush blank Reset Initialize  blank Ping Test}
set _dhgDefaults(sta,commands) {{debugLevel sta} {simulateLevel sta} 	\
    {resetHealth sta} separator {flush sta} separator {reset sta} {init sta}  \
    separator {ping sta} {test sta}}
set _dhgDefaults(sta,commandDesc) {					\
    {Set Debug Level for the Status Server.}				\
    {Set Simulation Level for the Status Server.} 			\
    {Set the Health of the Status Server to 'GOOD'.} blank		\
    {Flush the values of all status records.} blank			\
    {Reset the Status Server.}						\
    {Initailze the Status Server.} blank				\
    {Ping the Status Server to see if it is responding to commands.}	\
    {Perform tests on the Status Server.}}
set _dhgDefaults(sta,commandHelp) {					\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSta.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"} }




#
# DHS Storage Server
#

set _dhgDefaults(sto,name) "Storage Server"
set _dhgDefaults(sto,status) {State Health Debug Sim Desc}
set _dhgDefaults(sto,statusDesc) {State: Health: {Debug Level:}		\
	{Simulation Level:} {Health Description:}}
set _dhgDefaults(sto,help) {						\
    {Current State of the Storage Server.}				\
    {Health of the Storage Server.}					\
    {Current level of Debugging for the Storage Server.}		\
    {Current Simulation Level for the Storage Server.}			\
    {{bitmap b1} Long health description for the Storage Server.}}
set _dhgDefaults(sto,commandName) {Debug Simulation {Reset Health} 	\
     blank Refresh blank Reset Initialize blank Ping Test}
set _dhgDefaults(sto,commands) {{debugLevel sto} {simulateLevel sto} 	\
     {resetHealth sto} separator {stoRefresh .cstgmain} separator    	\
     {reset sto} {init sto} separator {ping sto} {test sto}}
set _dhgDefaults(sto,commandDesc) {					\
    {Set Debug Level for the Storage Server.} 				\
    {Set Simulation Level for Storage Server.} 				\
    {Set the Health of the Storage Server to 'GOOD'.} blank		\
    {Reread the database values and display them.} blank		\
    {Reset the Storage Server.}						\
    {Initailze the Storage Server.} blank				\
    {Ping the Storage Server to see if it is responding to commands.}	\
    {Perform tests on the Storage Server.}}
set _dhgDefaults(sto,commandHelp) {					\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"} }


