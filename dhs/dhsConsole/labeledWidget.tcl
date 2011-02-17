# $Id: labeledWidget.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1999				(c) 1999
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
# dhsConsole/src/labeledWidget.itk
#
# This is an altered version of the class iwidgets::Labeledwidget
# class, which is part of the incr Widgets Tcl/Tk extension.
#
# PURPOSE:
# Defines a widget that has a label associated with it.
#
# CLASS NAME(S)
# CLabeledWidget
#
# METHOD NAME(S)
# CLabeledWidget::alignLabels		: Takes a list of widgets derived
#					  from the CLabeledWidget class and
#					  adjusts the label margin to align
#					  all of their labels.
# 
# GLOBAL PROCEDURE(S)
# cLabeledWidget			: lower case access method to the 
#					  CLabeledWidget class.
#
# $Log: labeledWidget.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.1  1999/06/18 05:27:53  jaeger
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
# CLabeledWidget
#
# OPTIONS
# Actual options vary depending on the which widget is given to the
# constructor.  The "usual" options for the widget will be added to
# the widgets list of options.
#
# The minimum list of options are:
# background    Specifies the background color.
# clientdata    A hook that clients can use to store data with each widget.
# cursor        Specifies the mouse cursor to be used.
# font		Specifies the font to be used.
# foreground    Specifies the normal foreground color to use when
#               displaying the widget.
# highlightcolor
#               Specifies the color to use when the widget has the 
#               input focus.
# highlightthickness
#               Specifies the width of the highlight rectable to draw
#               around the widget when it has the input focus.
# labelbitmap   Specifies the label's bitmap.
# labelimage    Specifies the label's image.
# labelmargin   Specifies the margin between the label and the  childsite
# labelpos      Specifies the relative postion of the label with respect
#                   the childsite.
# labeltext     Specifies the text that appears in the label.
# labelvariable Specifies the variable associated with the label.
# widgetvariable Specifies the variable associated with the widget.
#
# PUBLIC METHODS: 
# alignLabels
# cget          Standard cget method, returns the value of the specified
#                       option.
# component     Standard [ incr ] Tk component method.
# config        See configure
# configure     Standard Tk method, sets or returns options.
# isa           Sets the widget's class.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To create a mega-widget which contains some given widget and a label.
# 
#
#-
#***********************************************************************
#


#
# Default resources
# 

option add *CLabeledWidget.labelMargin 1 widgetDefault
option add *CLabeledWidget.widgetvariable "" widgetDefault


#
# Usual options.
#

itk::usual CLabeledWidget { 
    keep -background -cursor -foreground -labelfont
 }

# ------------------------------------------------------------------
#                            LABELEDWIDGET
# ------------------------------------------------------------------

class CLabeledWidget { 
    inherit itk::Widget


    #
    # Option definitions
    #

    itk_option define -labelpos labelPos Position w {
	positionLabel
    }

    itk_option define -labelmargin labelMargin Margin 1 {
	positionLabel
    }

    itk_option define -labeltext labelText Text {} { 
	$itk_component(label) configure -text $itk_option(-labeltext)
	positionLabel
    }

    itk_option define -labelvariable labelVariable Variable {} { 
	$itk_component(label) configure 			\
	    -textvariable $itk_option(-labelvariable)
	positionLabel
    }

    itk_option define -labelbitmap labelBitmap Bitmap {} { 
	$itk_component(label) configure -bitmap $itk_option(-labelbitmap)
	positionLabel
    }

    itk_option define -labelimage labelImage Image {} { 
	$itk_component(label) configure -image $itk_option(-labelimage)
	positionLabel
    }

    itk_option define -widgetvariable widgetVariable Variable {}

    #
    # Public methods and data
    #

    constructor { args } {}
    destructor {}
    proc alignlabels { args } {}


    #
    # Protected methods and data - NONE
    #

    protected method positionLabel { {when later} }
    protected method cmd { name1 name2 opt }

    protected variable reposition ""  ;# non-null => positionLabel pending


    #
    # Private methods and data
    #

    private method updateWidget { name1 name2 op } {}

	
    private variable widgetType ""	;# Type of widget assoc. with the
					 # label.
    private variable widgetVariable ""	;# Variable that is being traced.
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cLabeledWidget
#
# INVOCATION:
# cLabeledWidget pathName widget ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)	A window path name for a nonexistent window.
# (>) widget	(string)	The name of a valid Tcl/Tk widget.
# (>) options   (string)	One or more valid option/value pairs.
#
# The pathName, widget, and options are passed on to the CForm class.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# Provide a lowercased access method for the CLabeledWidget class.
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
# CLabeledWidget::constructor
# CLabeledWidget::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc cLabeledWidget { pathName widget args } { 
    uplevel CLabeledWidget $pathName $widget $args
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLabeledWidget::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) widget - The name of a valid tcl/tk widget.
# (>) args - Arguments to itk_initialize
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Constructor for the "CLabeledWidget" class.	
#
# DESCRIPTION:
# Create the label and widget and position them.  Also process any
# arguments (options) that were specified.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLabeledWidget::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CLabeledWidget::constructor { widget args } { 
    # 
    # Create the outermost frame to maintain geometry.
    #

    itk_component add shell { 
	frame $itk_interior.shell
    } { 
	keep -background -cursor
    }
    pack $itk_component(shell) -fill both -expand yes
    

    #
    # Determine the options the given widget has
    #

    if { $widget == "" } { 
	error "wrong # args: should be \"CLabeledwidget pathname widget ?options?\""
    }


    #
    # Create the widget.
    #

    itk_component add widget { 
	$widget $itk_component(shell).widget
    } { 
	usual
    }
    pack $itk_component(widget) -expand yes
    set widgetType $widget
    

    #
    # Create label.
    #

    itk_component add label { 
	label $itk_component(shell).label
    } { 
	keep -background -foreground -cursor -font
    }

    
    #
    # Create margin between label and the child site.
    #

    itk_component add labmargin { 
	frame $itk_component(shell).labmargin
    } { 
	keep -background -cursor
    }
    

    #
    # Explicitly handle configs that may have been ignored earlier.
    #

    eval itk_initialize $args


    # 
    # When idle, position the label.
    #
    
    positionLabel

    return "$itk_component(hull)"
}

# ------------------------------------------------------------------
#                        DESTRUCTOR
# ------------------------------------------------------------------


 #
#***********************************************************************
#+
# PROCEDURE NAME:
# CLed::destructor
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
# Destructor for the "CLed" class.
#
# DESCRIPTION:
# Cancel the reposition of the LED if the command is pending.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLed::constructor
#
# DEFICIENCIES:
# None.
#-
#************************************************

body 	CLabeledWidget::destructor {} { 
    #
    # Make sure we don't reposition the label after the object
    # has been destroyed.
    #
    
    if { $reposition != "" } { after cancel $reposition }	

    
    #
    # Remove all tracing of variables that was set-up.
    #

    if { "$widgetVariable" != "" } {
	set traceInfo [ namespace eval :: "trace vinfo \"$widgetVariable\"" ]

	foreach traceItem $traceInfo {
	    set cmd [ lindex $traceItem 1 ]
	    if { "[ code $this updateWidget ]" == "$cmd" } {
		set op [ lindex $traceItem 0 ]
		namespace eval :: "trace vdelete \"$widgetVariable\" $op \"$cmd\""
	    }
	}
    }
}

# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLabeledWidget::widgetvariable
#
# INVOCATION:
# pathName configure -widgetvariable ?variable?
#	      OR
# cLabeledWidget pathName -widgetvariable ?variable?
#
# where pathName is a window path of a "CLabeledWidget" and variable is
# the name of a Tcl/Tk variable.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the variable whose value is some value associated with the
# widget portion of the labelled widget.
#
# DESCRIPTION:
# Depending on the type of widget that is paired with the label either
# set a widget option or use a traced variable to keep track of the
# changes.
#
# EXTERNAL VARIABLES:
# CLabeledWidget::widgetType
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None. 
#
# DEFICIENCIES:
# Currently CTable, listbox, scrolledlistbox, scrolledtest, and text
# widgets are minimally supported.  The contents of the widgets will
# change when the value of the variable changes but the variables value
# doesn't change when the widgets content changes.
#-
#***********************************************************************
#

configbody CLabeledWidget::widgetvariable {
    switch -exact -- "$widgetType" {
	button -
	cLogMonitor -
	CLogMonitor -
	combobox -
	entry -
	entryfield -
	label -
	spininit -
	spinner	{
		$itk_component(widget) configure			\
		    -textvariable $itk_option(-widgetvariable)
	}

	cTable -
	CTable -
	listbox -
	scrolledlistbox -
	scrolledtext -
	text
	{
	    #
	    # Need to add some method of rebinding and bindings on
	    # the widget so the variable's value get updated when
	    # the widget's content changes.
	    #

	    set newVariable $itk_option(-widgetvariable)
	    if { ( "$newVariable" == "" ) || 				\
		     ( "$widgetVariable" != "$newVariable" ) } {
		set traceInfo [ namespace eval :: "trace vinfo \"$widgetVariable\"" ]
		#
		# Remove the trace on the old widget variable.
		#

		foreach traceItem $traceInfo {
		    set cmd [ lindex $traceItem 1 ]
		    if { "[ code $this updateWidget ]" == "$cmd" } {
			set op [ lindex $traceItem 0 ]
			namespace eval :: "trace vdelete \"$widgetVariable\" $op \"$cmd\""
		    }
		}


		#
		# set-up the trace on the new widget variable.
		#

		set widgetVariable $newVariable
		if { "$widgetVariable" != "" } {
		    set traceInfo [ namespace eval :: "trace vinfo \"$widgetVariable\"" ]
		    set found 0
		    foreach traceItem $traceInfo {
			set cmd [ lindex $traceItem 1 ]
			if { "[ code $this updateWidget ]" == "$cmd" } {
			    set found 1
			    break
			}
		    }

		    if { [ isFalse $found ] } {
			#
			# Update the widget.
			#
			
			namespace eval :: "trace variable \"$widgetVariable\" wu \"[ code $this updateWidget ]\""
		    }
		}
	    }
	}

	cProgressBar -
	CProgressBar -
	scale {
	    $itk_component(widget) configure 				\
		-variable $itk_option(-widgetvariable)
	}
	    
	CLed -
	cLed {
	    $itk_component(widget) configure 				\
		-ledvariable $itk_option(-widgetvariable)
	}   
	
	checkbox -
	radio -
	radiobox {
	    set "$itk_option(-widgetvariable)" ""
	    $itk_component(widget) configure -command 			\
		"set \"$itk_option(-widgetvariable)\" \[ $itk_component(widget) get \]"
	}		       
	

	optionmenu {
	    $itk_component(widget) component menuLabel configure 	\
		-textvariable $itk_option(-widgetvariable)
	}

 	default	{
	    warningDialog "Unable to use widgetVariable option for\
tracing  ${widgetType}'s value" $itk_component(hull)
	}
    }
}


# ------------------------------------------------------------------
#                         PUBLIC   METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLabeledWidget::alignLabels
#
# INVOCATION:
# CLabeledWidget::alignLabels args
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args	(list)  A list of mega-widgets whose class is CLabeledWidget 
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To adjust the label margins for the list of mega-widgets given so that
# their labels are alligned.
#
# DESCRIPTION:
# Find the width the labels need to be.  Adjust the width of the labels
# if necessary.
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
# Work only when the -labelpos of all widget is "w' and if all widgets
# have a label with and image, bitmap, or text on it.
#-
#***********************************************************************
#

proc 	CLabeledWidget::alignlabels { args } {
    update
    set maxLabelWidth 0

    
    #
    # Verify that all the widgets are of type CLabeledWidget and 
    # determine the size of the maximum length label string.
    #

    foreach iwid $args { 
	set objcmd [ find objects -isa CLabeledWidget *::$iwid ]
	
	if { $objcmd == "" } { 
	    error "$iwid is not a \"CLabeledWidget\""
	}
	
	set widgetWidth [ winfo reqwidth $iwid.shell.widget ]
	set shellWidth [ winfo reqwidth $iwid.shell ]
	
	if { [ expr $shellWidth - $widgetWidth ] > $maxLabelWidth } { 
	    set maxLabelWidth [ expr $shellWidth - $widgetWidth ]
	}
    }

    
    #
    # Adjust the margins for the labels such that the widgets and
    # labels line up.
    #

   
    foreach iwdid $args {
	if { $maxLabelWidth > $labelSize } { 
	    set dist [ expr $maxLabelWidth - \
		   ( $labelSize - [ winfo reqwidth $iwid.shell.labmargin ] ) ]
	
	    set objcmd [ find objects -isa CLabeledWidget *::$iwid ]
	    $objcmd configure -labelmargin $dist 
	}
    }
}


# ------------------------------------------------------------------
#                      PROTECTED   METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CLabeledWidget::positionLabel
#
# INVOCATION:
# CLabeledWidget::positionLabel ?when?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) when	(later|now)	States when the label is to be positioned.
#
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Packs the label and label margin.  If when is now, the
# change is applied immediately.  If it is later or it is not
# specified, then the change is applied later, when the application
# is idle.
#
# DESCRIPTION:
# Repack the label and widget positioning them base on the value
# of the -labelpos option.
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

body 	CLabeledWidget::positionLabel { {when later} } { 
    #
    # Determine whether positioning shouldd be done now or later.
    #

    if { $when == "later" } { 
	if { $reposition == "" } { 
	    set reposition [ after idle [ code $this positionLabel now ] ]
	 }
	return
    } elseif { $when != "now" } { 
	error "bad option \"$when\": should be now or later"
    }
    set reposition ""


    #
    # Reposition the label only if it has something on it.
    #

    if { ( $itk_option(-labeltext) != {} ) ||
	    ( $itk_option(-labelbitmap) != {} ) ||
	    ( $itk_option(-labelimage) != {} ) } { 
	
	switch -- $itk_option(-labelpos) { 
	    nw { 
		pack configure $itk_component(widget) -side top
		
		$itk_component(labmargin) configure -width 1 -height \
		    [ winfo pixels $itk_component(labmargin) \
			  $itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) -side top \
			-before $itk_component(widget)
		pack configure $itk_component(label) -anchor w \
			-side top -before $itk_component(labmargin)
	    }
	    
	    n { 
		pack configure $itk_component(widget) -side top
		
		$itk_component(labmargin) configure -width 1 -height \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) -side top \
			-before $itk_component(widget)
		pack configure $itk_component(label) -anchor center \
			-before $itk_component(labmargin) -side top
	    }
	    
	    ne { 
		pack configure $itk_component(widget) -side top
		
		$itk_component(labmargin) configure -width 1 -height \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) -side top \
			-before $itk_component(widget)
		pack configure $itk_component(label) -anchor e \
			-side top -before $itk_component(labmargin)
	    }
	    
	    e { 
		pack configure $itk_component(widget) -side right
		
		$itk_component(labmargin) configure -height 1 -width \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) \
			-side right -before $itk_component(widget)
		pack configure $itk_component(label) -anchor center \
			-side right -before $itk_component(labmargin)
	    }
	    
	    se { 
		pack configure $itk_component(widget) -side top
		
		$itk_component(labmargin) configure -width 1 -height \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) \
			-side top -after $itk_component(widget)
		pack configure $itk_component(label) -anchor e \
			-side bottom -after $itk_component(labmargin)
	    }
	    
	    s { 
		pack configure $itk_component(widget) -side top
		
		$itk_component(labmargin) configure -width 1 -height \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) \
			-side top -after $itk_component(widget)
		pack configure $itk_component(label) -anchor center \
			-side bottom -after $itk_component(labmargin)
	    }
	    
	    sw { 
		pack configure $itk_component(widget) -side top
		
		$itk_component(labmargin) configure -width 1 -height \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) \
			-side top -after $itk_component(widget)
		pack configure $itk_component(label) -anchor w \
			-side bottom -after $itk_component(labmargin)
	    }
	    
	    w { 
		pack configure $itk_component(widget) -side right
		
		$itk_component(labmargin) configure -height 1 -width \
			[ winfo pixels $itk_component(labmargin) \
			$itk_option(-labelmargin) ]
		pack configure $itk_component(labmargin) \
			-side left -before $itk_component(widget)
		pack configure $itk_component(label) -anchor center \
			-side left -before $itk_component(labmargin)
	    }
	}

    #
    # Else, neither the  label text, bitmap, or image have a value, so
    # unpack them the label and margin.
    #
    } else { 
	pack forget $itk_component(label)
	pack forget $itk_component(labmargin)
    }

    return
 }

#
#***********************************************************************
#+
# PROCEDURE NAME:
# widgetVarTrace
#
# INVOCATION:
# widgetVarTrace name1 name2 op
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name1     (string)        Name of the variable or associtive array 
# (>) name2     (string)        Element of an associative array 
# (>) op        (string)        Operation that was performed on the variable 
# 
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To determine the new value of the variable being traced.
#
# DESCRIPTION:
# Make sure the variable passed in is the one being traced.
# If it is then find its value an update the widget appropriately.
#
# EXTERNAL VARIABLES:
# CLabeledWidet::
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# -widgetvariable
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CLabeledWidget::updateWidget { 
    name1 
    name2 
    op 
} { 
    #
    # Extract the variable name, taking care to handle 
    # "scoped" incr tcl/tk variables.
    #

    if { [ string match "*@scope*::*" $name1 ] } {
        set varScope [ lindex $name1 1 ] 
        set varName [ lindex $name1 2 ] 
    } else { 
        set varScope "::" 
        set varName $name1 
    } 
 
    if { $name2 == "" } { 
        set var "$varName" 
    } else { 
        set var "${varName}(${name2})" 
    } 

    set fullVarName "${varScope}${var}" 
    if { ( "$fullVarName" != "$widgetVariable" &&
	   "$widgetVariable" != "$varName" ) ||
         [ namespace eval $varScope "info exists $varName" ] == 0 } {
        # 
        # We aren't suppose to be monitoring this variable or
	# it does not exist. 
        #  
 
        return 
    } 


    if { $op == "u" } {
	#
	# The variable has been unset so we should no longer trace it
	#

        $itk_component(hull) configure -widgetvariable ""
    } elseif { $op == "w" } {
	#
	# Find the new value and call a command that processes this
	# new value.
	#

        set value [ uplevel \#0 subst \$$var ]

	
	#
	#  Update the contents of the widget
	#

	switch -exact -- "$widgetType" {
	    cTable -
	    CTable {
		catch "$itk_component(widget) deleteRow 0 end"
		foreach line [ split "$value" "\n" ] {
		    $itk_component(widget) addRow "$line"
		}
	    }

	    listbox -
	    scrolledlistbox  {
		$itk_component(widget) delete 0 end
		foreach line [ split "$value" "\n" ] {
		    $itk_component(widget) insert end "$line"
		}
	    }

	    scrolledtext -
	    text {
		$itk_component(widget) delete 1.0 end
		$itk_component(widget) insert 1.0 "$value"
	    }

	    default {
		eval puts \"WARNING: Unable to change contents of widget to: \$$widgetVariable\"
	    }
	}
	
    }

    return
}
