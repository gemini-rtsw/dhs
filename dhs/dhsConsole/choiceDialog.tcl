# $Id: choiceDialog.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/choiceDialog.itk
#
# PURPOSE:
# Displays a number of choices in a dialog window and make the
# user's selection available.
#
# CLASS NAME(S)
# CChoiceDialog	- Creates a dialog with a number of choices and
#		  makes the user's selection available.
#
# METHOD NAME(S)
# PUBLIC
# CChoiceDialog::activate
#		- Displays the window and wait based on the modality.
# CChoiceDialog::clear
#		- Clears the selections.
# CChoiceDialog::constructor
#		- Constructor for the CChoiceDialog class.
# CChoiceDialog::deactivate
#		- Deactivate the display of the shell.
# CChoiceDialog::destructor
#		- Destructor for the CChoiceDialog class.
# CChoiceDialog::getChoice
#		- Get the current choice (selection).
# CChoiceDialog::setChoice
#		- Set the choice (selection).
#
# PROTECTED
# CChoiceDialog::add_buttons
#		- Responsible for adding the buttons that are displayed.
#		  Overrides the inherited method DialogWidget::add_buttons.
# CChoiceDialog::set_result
#		- Responsible for setting the result, or choice.
#		  Overrides the inherited method DialogWidget::set_result.
#
# PRIVATE
# CChoiceDialog::commandProc
#		- Gets executed evertime a radio button is selected.
# CChoiceDialog::doLayout
#		- To arrange the choices.
# CChoiceDialog::invokeButton
#		- Called when the return key is pressed, invokes the button
#		  with the focus.
# CChoiceDialog::placeWidget
# 		- To place a widget (radio button) at a specific position in
# 		  the choice frame.
#
#
# PUBLIC PROCEDURE(S) 
# choiceDialog	- A lower case access to the CChoiceDialog, to
#		  make it complient with Gemini Standards.
#
# $Log: choiceDialog.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:08  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.6  1999/02/09 00:14:23  jaeger
# Added a windo existence check.
#
# Revision 1.5  1999/01/26 06:09:55  jaeger
# Fixed bugs in doLayout.  Didn't draw a new list of buttons properly.
#
# Revision 1.4  1998/08/05 06:21:14  jaeger
# Fixed miscellaneous bugs: -orient now works properly, -variable no
# longer has a problem which choices with a space, -cols now works
# properly, all calls to doLayout now use an after script so it
#
# only gets called once.
#
# Revision 1.3  1998/06/17 19:49:52  jaeger
# Added method information at the top of the file.
#
# Revision 1.2  1998/06/17 19:21:19  jaeger
# Added in the necessary comments in the code.  Also fixed the -buttons
# option.
#
# Revision 1.1  1998/05/22 18:06:30  jaeger
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
# CChoiceDialog
#
# OPTIONS:
# background    - Specifies the background color of the shell.
# bitmap	- Optional bitmap to display to left of message
# buttons	- specifies the buttons that are to appear on the dialog
# center	- If true, center the dialogue window on startup.
# choice	- specifies the choices taht are to appear on the dialog
# clientdata    - A hook that clients can use to store data with each widget.
# cols		- specifies the maximum number of columns the choices are
#		  to be displayed in.  Note only one of -cols and -rows
#		  can be specifed. A value of 0 means infinite.
# command	- command that is executed when a choice is made.
# cursor        - Specifies the mouse cursor to be used.
# default	- Index of button that is to display the default ring 
#		  (-1 means none).
# foreground    - Specifies the foreground color of the shell.
# highlightcolor
#               - Specifies the color to use when the widget has the 
#               input focus.
# highlightthickness
#               - Specifies the width of the highlight rectable to draw
# image		- Optional image to display above the message
# justify	- Indicates how the textual message is to be justified.
# messagefont	- Specifies the text font to use in the message.
# messagewidth	- Speicfies how wide (in pixels) the message is.
# modal		- If true, grab all events.
# number	- Set an optional unique instance or clone number for 
#		  the dialog.
# orient	- specifies the orintation of the label
# rows		- specifies the maximum number o rows the choices are to
#		  be diaplyed in.  Note only of of -cols and -rows can 
#      		  be specified.  A value of 0, means infinite.
# shorthelpwin	- Optionally specify a different TopLevelWidget to 
#		  display short help messages.
# standalone	- Indicates if the window is to be a standalone application.
# text		- Specifies the textual message to be displayed.
# title		- Title to display in dialog's decorative frame.
# transient	- If true and the dialouge is a child of a TopLevel widget.
# value		- Which choice should be initially selected.
# variable	- Specifies a variable who's value is the selection.
# withdraw	- If true, withdraw the dialogue window on startup.
#
# PUBLIC METHODS: 
# activate	- Displays the window and wait based on the modality.
# clear		- Clears the selections.
# add_help_button
#		- If any help has been defined then add a button to show it.
# add_menu_short_help
#		- Set the text of the short help message to be displayed 
#		  whenever the mouse enters the menu item with the given 
#                 label (see short_help).
# add_menubar	- Add an option menubar to the top of the dialogue.
# add_menubutton- Add a menu button to the menubar, or reset it's menu to empty
#		  if the menubutton already exists.
# add_menuitem  - Add a menu item of the given type to the given menu and 
# 		  arrange to have the given short help message displayed when 
#		  the mouse is over the item.
# add_short_help- Set the text of the short help message to be displayed 
#		  whenever the mouse enters the widget w (see short_help).
# busy		- Run the given tcl command in the scope of this class
# 		  while displaying the (blt) busy cursor in the dialogue
# center	- Center this window on the screen. 
# cget          - Standard cget method, returns the value of the specified
#                 option.
# component     - Standard [ incr ] Tk component method.
# config        - See configure
# configure     - Standard Tk method, sets or returns options.
# configure_menubutton
#		- Configure something of the named menubutton.
# deactivate    - Deactivate the display of the shell.
# getChoice	- Get the current choice (selection).
# get_menu	- Return the path name of the menu for the given menubutton 
#		  label
# get_menubutton- Return the path name of the menubutton for the given 
#		  menubutton label
# hide_windows	- Toggle the visibility of all popup windows.
# isa           - Sets the widget's class.
# list_windows	- Return a list of top level windows that are children of the
#		  dialogue.
# make_short_help
#		- Add a subwindow at the bottom of the screen for short 
#		  help messages
# quit		- Use this method to quit the application if you might want to 
# 		  reuse the window later.
# setChoice	- Set the choice to a specific value.
# show_help	- Show the help file associated with this window.
# test		- Run the given tcl command and print out any errors.
#
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Create a class that will display a number of choices in a dialog
# window.
#
#-
#***********************************************************************
#

itcl::class 	CChoiceDialog {
    inherit util::DialogWidget

    #
    # Options
    #

    itk_option define -buttons buttons Buttons {OK} { add_buttons }
    itk_option define -choice choice Choice {} { 
	if { $updatePending == "" } {
	    set updatePending [ after idle "[ code $this doLayout ]" ]
	}
    }
    itk_option define -cols cols Columns {0} { 
	if { $updatePending == "" } {
	    set updatePending [ after idle "[ code $this doLayout ]" ]
	}
    }
    itk_option define -command command Command {}
    itk_option define -orient orient Orient {horizontal} 
    itk_option define -rows row Rows {0} { 
	if { $updatePending == "" } {
	    set updatePending [ after idle "[ code $this doLayout ]" ]
	}
    }
    itk_option define -value value Value {}
    itk_option define -variable variable Variable "" {
	if { $itk_option(-variable) != "" } { 
	    uplevel #0 "set $itk_option(-variable) \"$selection\""
	}
    }


    #
    # Public methods and data
    #

    public method constructor { msg }
    public method destructor {} {}

    public method activate {}
    public method clear {} {
	if { $selection != "" } {
	    set current [ lsearch $itk_option(-choice) $selection ]
	    if { [ info exists itk_component(choice$current) ] } {
		$itk_component(choice$current) deselect
	    }
	    set selection ""
	} 
    }
    public method deactivate {} { 
	if { [ winfo exists $itk_component(hull) ] } {
		wm withdraw $itk_component(hull) 
	}
	return "$selection"
    }
    public method getChoice {}  { return "$selection" }
    public method setChoice { selection } {
	set CChoiceDialog::selection $selection
	if { $itk_option(-variable) != "" } {
	    uplevel #0 "set $itk_option(-variable) \"$selection\""
	}
    }


    #
    # Protected methods and data
    #


    protected method add_buttons {}

    #
    # Used by the Dialog widget/activate method to determine
    # the "result".
    #

    protected method set_result {} {
	global ::choice
	return [ set choice $selection ]
    }

    #
    # Private methods and data
    #

    private variable selection ""	
    private variable choiceHeights {}   
    private variable choiceWidths {}    
    private variable constructed false
    private variable updatePending ""


    #
    # Gets executed evertime a radio button is selected.
    #

    private method commandProc {selection} {
	setChoice $selection
	if { $itk_option(-variable) != "" } {
	    uplevel #0 "set $itk_option(-variable) \"$selection\""
	}
	set cmd $itk_option(-command)
	if { "$cmd" != "" } {
	    lappend cmd $selection
	}
	eval $cmd
    }
    private method doLayout {}
    private method invokeButton {} {
	set focusWin [ focus ]
	if { [ winfo class $focusWin ] == "Button" } {
	    $focusWin flash
	    $focusWin invoke
	}
    }
    private method placeWidget { widget row col args }
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# choiceDialog
#
# INVOCATION:
# choiceDialog pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)    A window path name for a nonexistent window. 
# (>) options   (string)        One or more valid option/value pairs. 
#
# The pathName and options are passed on the CChoiceDialog class.
#
# PROCEDURE VALUE:
# pathName (string) : The window path name provided is returned.
#
# PURPOSE:
# To provide a lower cased access metho to the CChoiceDialog class.
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
# CChoiceDialog
# CChoiceDialog::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	choiceDialog {
    pathName
    args
} {
    uplevel ::CChoiceDialog $pathName $args
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CChoiceDialog::constructor
#
# INVOCATION:
# N.A.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Arguments to itk_initialize 
# 
# PROCEDURE VALUE: 
# None. 
# 
# PURPOSE: 
# Constructor for the CChoiceDialog class. 
#
# DESCRIPTION:
# Create components to hold the buttons and the choices.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CChoiceDialog::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CChoiceDialog::constructor {
    args
} {
    wm withdraw $itk_component(hull)


    #
    # Create the button box
    #

    itk_component add bot {
	frame $w_.bot -relief raised -borderwidth 2
    } 
    pack $itk_component(bot) -side bottom -fill x


    #
    # Create a frame to hold the choices.
    #

    itk_component add choice {
	frame $itk_component(ext).choice 
    } {
	keep -background -cursor
    }
    pack $itk_component(choice) -side left -fill x -expand 1 		\
	-padx 2m -pady 2m -ipady 1m

    bind $w_ <Return> "[ code $this invokeButton ]"
    eval itk_initialize $args

    set constructed true
}


# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CChoiceDialog::orient
#
# INVOCATION:
# pathname configure -orient ?value? 
#             OR 
# CChoiceDialog pathName -orient value 
# 
# where pathName is a window path of a CChoiceDialog widget and value is 
# either "horizontal" or "vertical"
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# N.A.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Specifies the orientations of the text and label.
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

configbody	CChoiceDialog::orient {
    if {"$itk_option(-orient)" == "horizontal"} {
	set side_ left
    } else {
	set side_ top
    }

    if { [ winfo exists .c.top.def ] && [ winfo exists .c.top.ext ] } {
	pack forget .c.top.def .c.top.ext
	pack .c.top.def .c.top.ext -in .c.top -anchor center -expand 1      \
            -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 -side $side_
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CChoiceDialog::value
#
# INVOCATION:
# pathname configure -value ?value? 
#             OR 
# CChoiceDialog pathName -value value 
# 
# where pathName is a window path of a CChoiceDialog widget and value is 
# one of the items specified in the -choice option.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# N.A.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Specifies the intial choice.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CChoiceDialog::selection
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

configbody	CChoiceDialog::value {
    if { "$itk_option(-value)" == "" } {
	clear
    } elseif { [ set pos [ lsearch $itk_option(-choice) 		\
			       $itk_option(-value) ] ] != -1 } {
	set selection [ lindex $itk_option(-choice) $pos ]
	if { [ info exists itk_component(choice$pos) ] } {
	    $itk_component(choice$pos) select
	}
    } else {
	error "Invalid value `$itk_option(-value): Must be one of $itk_option(-choice)"
    } 
}

# ------------------------------------------------------------------
#                          PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CChoiceDialog;:activate
#
# INVOCATION:
# pathName activate
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# Empty String (string) : If a user doesn't select any of the choices.
# choice       (string) : User's selection choice
#
# PURPOSE:
# Display the choiceDialog and determine which selection has been
# chosen.
#
# DESCRIPTION:
# 1. Set the focus to the choice dialog.
# 2. Display the dialog
# 3. Wait until the "OK" button has been selected.
# 4. Determine what was selected.
# 5. Destroy the dialog and return the selection.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CChoiceDialog::deactivate
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CChoiceDialog::activate {
} {
    global ::choice


    #
    # Set a grab and claim the focus.
    #
    
    set oldFocus [focus]
    if {$itk_option(-modal)} {
	catch {grab $w_}
    }

    
    #
    # Display the window
    #

    wm deiconify $w_
    tkwait visibility $w_
    if {$itk_option(-default) >= 0} {
	focus $itk_component(button$itk_option(-default))
    } else {
	focus $w_
    }
 

    #
    # Wait for the user to respond, then restore the focus and
    # return the selection.
    #
    
    tkwait variable choice
    set result [ set_result ]
    catch {unset $variable_}
    catch {destroy $w_}
    catch {focus $oldFocus}
    return $result
}

# ------------------------------------------------------------------
#                          PROTECTED METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# add_buttons
#
# INVOCATION:
# add_buttons
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
# 
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Create the buttons that are displayed on the input dialog
#
# DESCRIPTION:
# 1. Destroy any existing buttons.
# 2. Create a button for each one specified in the -buttons option.
#
# Note that the "OK" button is the only button that will cause the
# window to disappear after a call to "activate".
#
# Overrides the "add_buttons" method in the util::DialogWidget class.
#
# EXTERNAL VARIABLES:
# ???
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# ???
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CChoiceDialog::add_buttons {
} {
    if { [ isFalse $constructed ] } {
	#
	# This is needed since the "Dialog" class calls this routine
	# when activate, which creates undesirable affects.
	#

	return
    }

   
    #
    # Remove current buttons from display.
    #

    if {  [ array names itk_component default ] != "" } {
	pack forget $itk_component(default)
    }

    foreach button [ array names itk_component "button*" ] {
	pack forget $itk_component($button)
    }


    #
    # Create a button for each item specified by the -buttons option
    #

    set i 0
    foreach button $itk_option(-buttons) { 
	if { [ array names itk_component button$i ] == "" } {
	    itk_component add button$i { 
		button $itk_component(bot).button$i -text $button
	    } {
	    }
	} else {
	    if { [ $itk_component(bot).button$i cget -text ] == "OK" } {
		$itk_component(bot).button$i configure -command ""
	    }
	    $itk_component(bot).button$i configure -text $button
	} 


	if { [ string toupper $button ] == "OK" } {
	    #
	    # OK button indicates that the user has made a desicion.
	    #

	    $itk_component(button$i) configure 				\
		-command "set choice \[ [ code $this set_result ] \]"
	}

	
	if { $i == $itk_option(-default) } {
	    #
	    # Create a box around the default button
	    #
	    
	    if { [ array names itk_component default ] == "" } {
		itk_component add default {
		    frame $itk_component(bot).default -borderwidth 1	\
			-relief sunken 
		}
	    }
	
	    raise $itk_component(button$i) $itk_component(default)
	    pack $itk_component(default) -side left -expand 1 	\
		    -padx 3m -pady 2m
	    pack $itk_component(button$i) -in $itk_component(default) 	\
		-padx 2m -pady 2m -ipadx 2m -ipady 1m
	} else {
	    pack $itk_component(button$i) -side left -expand 1 		\
		-padx 3m -pady 3m -ipadx 2m -ipady 1m
	}
	incr i
    }
}

# ------------------------------------------------------------------
#                      PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# doLayout
#
# INVOCATION:
# doLayout
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To arrange the "choices".
#
# DESCRIPTION:
# Foreach choice create a radiou button.
# Determine which row and column the button should appear in and
#    place it there.
#
# EXTERNAL VARIABLES:
# CChoiceDialog;:choiceHeights
# CChoiceDialog;:choiceWidths
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CChoiceDialog::placeWidget
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CChoiceDialog::doLayout {
} {
    #
    # Destroy all of the radio buttons.
    #

    foreach radioBut [ array names itk_component "choice*" ] {
	if { "$radioBut" != "choice" } {
	    destroy $itk_component($radioBut)
	    unset itk_component($radioBut)
	}
    }
    

    set rows $itk_option(-rows) 
    set cols $itk_option(-cols)
    set choices $itk_option(-choice)
    set n [ llength $choices ]
    set row 0
    set col 0
    set choiceHeights {}
    set choiceWidths {}
    if { $n == 0 } { set updatePending ""; return } 


    #
    # determine how many rows and columns
    #

    if {$rows != 0} {
	set cols [ expr $n / $rows + 1 ]
    } elseif {$cols != 0} {
	set rows [ expr $n / $cols + 1 ]
    } else {
	set rows 1
	set cols $n
    }  

    set i 0
    for {set row 0} {$row < $rows} {incr row} {
	for {set col 0} {$col < $cols} {incr col} {
	    # One Tk component is created for each item in the list 
	    # given by the -choice argument. The component names are
	    # choice0, choice1, ...

	    set name [ lindex $choices $i ]
	    itk_component add choice$i {
		set button_($name)					\
		    [ radiobutton $itk_component(choice).choice$i 	\
			  -text $name 					\
			  -value $name 					\
			  -variable $variable_ 				\
			  -command [ code $this commandProc $name ] ]
	    } {
		keep -state -anchor -background -foreground
		rename -font -valuefont valueFont Font
		rename -width -valuewidth valueWidth Width
	    }

	    
	    #
	    #  Display the radio button
	    #

	    placeWidget $itk_component(choice$i) $row $col -expand n	\
		-fill both -side left
	

	    if { [ incr i ] == $n } {
		break
	    }
	}
	if { $i == $n } {
	    break
	}
    }


    #
    # set default value
    #

    if { "$itk_option(-value)" == "" } {
	set selection [ lindex $choices 0 ]
	if { [ info exists itk_component(choice0) ] } {
	    $itk_component(choice0) select
	}
    } elseif { [ set pos [ lsearch $itk_option(-choice) 	\
			       $itk_option(-value) ] ] != - 1 } {
	set selection $itk_option(-value)
	if { [ info exists itk_component(choice$pos) ] } {
	    $itk_component(choice$pos) select
	}
    }

    set updatePending ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CChoiceDialog::placeWidget
#
# INVOCATION:
# placeItem widget row col args
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) widget	(path name)	Path name of a window to place.
# (>) row	(integer)	Row where the widget it to be placed.
# (>) col	(integer)	Column where the widget is to be placed.
# (>) args	(string)	Options for packing.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To place a widget (radio button) at a specific position in
# the choice frame.
#
# DESCRIPTION:
# If the cell exists remove any window that are currently in it and
#   place the new one in it.
# Otherwise create a preceeding rows and columns that do not already
#   exist.  Create the cell if necessary and place the widget inside
#   of it.
#
# Adjust heights and widths if necessary.
#
# EXTERNAL VARIABLES:
# CChoiceDialog;:choiceHeights
# CChoiceDialog;:choiceWidths
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

body 	CChoiceDialog::placeWidget {
    widget
    row
    col
    args
} {
    #
    # Find the height and width of the current widget
    #

    set width [ winfo reqwidth $widget ]
    set height [ winfo reqheight $widget ]

    set cell "cell${row}_$col"
    if { [ info exists itk_component($cell) ] } {
	#
	# The cell exist so remove any object inside of it.
	#

	foreach slave [ pack slaves $itk_component($cell) ] {
	    pack forget $slave
	}
    } else {
	#
	# Make sure all preceeding rows and the desired row exist,
	# if not create them.
	#

	set maxRow [ expr [ llength [ array names itk_component "Row*" ] ] - 1]

	while { $maxRow < $row } {
	    incr maxRow
	    itk_component add Row$maxRow {
		frame $itk_component(choice).row$row
	    } {
		keep -background -cursor
	    }
	    pack $itk_component(Row$maxRow) -fill both -side top -expand y
	}


	#
	# Make sure all preceeding columns and the desired column exist,
	# if not create them.
	#

	set rowPath $itk_component(Row$row) 
	set maxCol [ llength [ winfo children $rowPath ] ]
	while { $maxCol <= $col } {
	    itk_component add cell${row}_$maxCol {
		frame $rowPath.col$maxCol
	    } {
		keep -background -cursor
	    } 
	    pack $itk_component(cell${row}_$maxCol) -expand y -fill both \
		-side left 
	    incr maxCol
	}
    }


    #
    # Make sure all of the columns have the same width.
    #

    foreach rowFound [ array names itk_component "Row*" ] {
	if { [ winfo exists $itk_component($rowFound).col$col ] } {
	    $itk_component($rowFound).col$col configure -width $width
	}
    }
    if { $choiceWidths == "" || [ llength $choiceWidths ] <= $col } {
	set i [ llength $choiceWidths ]
	while { $i < $col } {
	    lappend choiceWidths 0
	    incr i
	} 
	lappend choiceWidths $width
    } else {
	set choiceWidths [ lreplace $choiceWidths $col $col $width ]
    }


    #
    # Make sure all of the rows have the same height.
    #

    if { $choiceHeights == "" || [ llength $choiceHeights ] <= $row	\
	     || $height < [ lindex choiceHeights $col ] } {
	set rowPath $itk_component(Row$row)
	foreach colFound [ winfo children $rowPath ] {
	    $colFound configure -height $height
	}
	if { $choiceHeights == "" || [ llength $choiceHeights ] <= $row  } {
	    set i [ llength $choiceHeights ]
	    while { $i < $row } {

		lappend choiceHeights 0
		incr i
	    }
	    lappend choiceHeights $height
	} else {
	    set choiceHeights [ lreplace $choiceHeights $row $row $height ]
	}
    }


    #
    # Pack the window inside and make sure it is on top.
    #

    eval pack configure $widget -in $itk_component($cell) $args 
    raise $widget
}
