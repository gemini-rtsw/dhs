# $Id: statusBox.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/dhsConsolestatusBox.itk
#
# PURPOSE:
# To create a box for displaying status information.
#
# Note: Some of the functionality in this class has been borrowed from
#	the [ incr widgets ] radiobox class.
#
# CLASS NAME(S)
# CStatusBox			:  Creates a box that can hold multiple
#				   labelled widgets which contain status
#				   information.
#
# METHOD NAME(S)
# PUBLIC
# CStatusBox::add		: Adds a new status item to the box.
# CStatusBox::align		: Aligns the labels of the status items.
# CStatusBox::constructor	: Constructor for the CStatusBox class.
# CStatusBox::delete		: Removes a status item from the box.
# CStatusBox::destructor	: Destructor for the CStatusBox class.
# CStatusBox::index		: Returns the numerical position of a
#				  particular status item.
# CStatusBox::insert		: Inserts a new status item into the box.
# CStatusBox::itemConfigure	: Allows a status item to be configured,
#				  Executes the configure method on it.
#
# GLOBAL PROCEDURE(S)
# cStatusBox			: lower case access method to the 
#				   CStatus class.
#
# $Log: statusBox.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.9  1999/06/07 22:14:32  jaeger
# Finished implementing the delete and insert methods.  Ensured the
# added status item widgets have the same background colour as the
# hull.
#
# Revision 1.8  1999/01/26 06:12:19  jaeger
# Hopefully fixed probably with different coloured status box then rest of
# display.
#
# Revision 1.7  1998/07/16 18:15:35  jaeger
# Made inheritance fully specified, i.e. added namespace information.
#
# Revision 1.6  1998/06/22 21:07:27  jaeger
# Added documentation to code to bring it up to Gemini standards.
# Moved some of the option and method definitions into the class
# since they were trivial.
#
# Revision 1.5  1998/06/19 22:36:39  jaeger
# Added cLogMonitor as an accepted widgetType
#
# Revision 1.4  1998/05/04 18:43:06  jaeger
# Fixed switch statement.  Added "--" flag.
#
# Revision 1.3  1997/11/13 23:53:19  jaeger
# Checked in for install
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
#***********************************************************************
#+
# CLASS NAME:
# CStatusBox
#
#
# OPTIONS:
# background		: Specifies the background color of the shell.
# borderwidth		: Specifies width of the 3-D border that appears 
#		  	  around the widget.
# clientdata		: A hook that clients can use to store data with 
#			  each widget.
# cursor		: Specifies the mouse cursor to be used.
# foreground    	: Specifies the normal foreground color.
# height		: Specifies the height of the widget.
# labelbitmap		: Specifies the label's bitmap.
# labelfont     	: Specifies the label's font.
# labelimage.   	: Specifies the label's image.
# labelmargin		: Specifies the margin between the label and the 
#			  childsite
# labelpos		: Specifies the relative postion of the label with 
#			  respect the childsite.
# labeltext     	: Specifies the text that appears in the label.
# labelvariable		: Specifies the text variable of the label around 
#			  the childsite.
# relief		: Specifies the 3-D effect desired for the widget.
# statusbackground	: Background colour on the status items.
# statusforeground	: Foreground colour on the status items.
# statusmargin		: Margin between the label and its childsite for
#			  all of the status items.
# statuspos		: The relative position of the label with its
#			  respective childsite for each of the status items
# width			: Specifies the width of the status box.
#
# PUBLIC METHODS: 
# add			: Add a status a labeled widget to the status box.
# align			: Align the labels of the widgets.
# cget			: Standard cget method, returns the value of the 
#			  specified option.
# childsite		: Returns the pathname of the childsite widget.
# component		: Standard [incr] Tk component method.
# config		: See configure
# configure		: Standard Tk method, sets or returns options.
# delete		: Remove a labeled widget from the status box.
# index			: Find the numerical index of a status item, topmost
#		   	  one is 0.
# insert		: Insert a label widget just before the one given.
# isa			: Sets the widget's class.
# itemConfigure		: Configure a partiuclar status item.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None
#
# PURPOSE:
# To display a number of labeled widgets in a box.  
#
#-
#***********************************************************************
#


#
# Default resources
#

option add *CStatusBox.align yes widgetDefault
option add *CStatusBox.borderWidth 2 widgetDefault
option add *CStatusBox.height 0 widgetDefault
option add *CStatusBox.labelPos n widgetDefault
option add *CStatusBox.relief groove widgetDefault
option add *CStatusBox.width 0 widgetDefault


#
# Usual options.
#

itk::usual CStatusBox {
    keep -borderwidth							\
	-cursor								\
	-highlightbackground						\
	-highlightcolor							\
	-highlightthickness						\
	-labels								\
	-relief
}
	

#
# Class definition
#

class CStatusBox {
    inherit iwidgets::Labeledwidget


    #
    # Option definitions
    #

    itk_option define -borderwidth borderWidth BorderWidth 0 {
	[ $this childsite ] configure -borderwidth $itk_option(-borderwidth)
    }

    itk_option define -height height Height 0 {
	[ $this childsite ] configure -height $itk_option(-height)
	#$itk_component(shell) configure -height $itk_option(-height)
    }

    itk_option define -relief relief Relief groove {
	[ $this childsite ] configure -relief $itk_option(-relief)
    }

#    itk_option define -statusbackground statusBackground Background #d9d9d9
    itk_option define -statusforeground statusForeground Foreground Black
    itk_option define -statusmargin statusMargin Margin 1
    itk_option define -statuspos statusPos Position w

    itk_option define -width width Width 0 {
	[ $this childsite ] configure -width $itk_option(-width)
	#$itk_component(shell) configure -width $itk_option(-width)
    }

	
    #
    # Public methods and data
    #

    public method constructor { args } {
	component hull configure -borderwidth 0
	eval itk_initialize $args
    }

    public method destructor {} {}

    public method align {} {
	set childsite [ $this childsite ]
	eval iwidgets::Labeledwidget::alignlabels [ winfo children $childsite ]
    }
    public method add { widgetType item args } 
    public method delete { item }
    public method index { item }
    public method insert { index widgetType item args } 
    public method itemConfigure { item args }  {
	set index [ index $item ]
	eval [ lindex $itemPathNames $index ] configure $args 
    }


    #
    # Protected methods and data - NONE
    #


    #
    # Private methods and data
    #

    private method   makeNewItem { widgetType item args } {}

    private variable items {}		;# List of labeled widgets.
    private variable itemPathNames {}	;# list of labeled widgets pathNames.
    private variable unique 0		;# Unique item identifier.    
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cStatusBox
#
# INVOCATION:
# cStatusBox pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)	A window path name for a nonexistent window.
# (>) options   (string)	One or more valid option/value pairs.
#
# The pathName and options are passed on to the CStatusBox class.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lowercased access method for the CStatusBox class.
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
# CStatusBox::constructor
# CStatusBox::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc cStatusBox { pathName args } {
    uplevel CStatusBox $pathName $args
}

# ------------------------------------------------------------------
#                       PUBLIC  METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStatusBox::add
#
# INVOCATION:
# pathName add widgetType item  ?option value option value ...?
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) widgetType(command) 	Any valid widget derived from the 
#				labeled widget class.
# (>) item      (string) 	Widget identifier.
# (>) option    (string) 	Any valid option for widgetType
# (>) value     (string) 	Value for the widgetType option.
#
# PROCEDURE VALUE:
# item - The given item is returned.
#
# PURPOSE:
# To add a new widget, derived from the LabeledWidget class, with
# the identifier "item".  Any option/value paris are applied to the
# widget.  The widget is added below the others and is a "widgetType"
# widget.
#
# DESCRIPTION:
# 1. Construct the widgetType, makin it a "component" of this
#	widget.
# 2. Align it, if necessary
# 3. Add the widgets path name to all its components "bindtag" list.
# 4. Add the item and items path name to private class lists.
#
# EXTERNAL VARIABLES:
# items	    	(private class variable)    List of the labeled widgets.
# itemPathNames	(private class variable)    List of the labeled widgets
#					    window path names..
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStatusBox::insert
# CStatusBox::delete
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CStatusBox::add {
    widgetType 
    item 
    args
} {
    #
    # Create the new status item
    #

    set itemWindow [ eval makeNewItem \"$widgetType\" \"$item\" $args ]


    #
    # Add the item to the bottom of the CStatusBox.
    #

    pack $itk_component($item) -fill x -expand yes


    #
    # Add the item and its path name to ours protected lists.
    #

    lappend items $item
    lappend itemPathNames $itemWindow

    return $item
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStatusBox::delete
#
# INVOCATION:
# pathName delete item
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) item (number|"end"|pattern) A valid CStatusBox item identifier.
# 
# number    - Specifies which item numerically, where 0 is the topmost.
# "end"     - The last item.
# pattern   - The "item" given with the "add" or "insert" methods.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To remove the widget specified by "item" from the status box.
#
# DESCRIPTION:
# Determine which item needs to be removed and remove it from the display.
#
# EXTERNAL VARIABLES:
# items	    	(private class variable)    List of the labeled widgets.
# itemPathNames	(private class variable)    List of the labeled widgets
#					    window path names..
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStatusBox::add
# CStatusBox::insert
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStatusBox::delete {
    item
} {
    #
    # Determine which status item is to be deleted.
    #

    set idx [ index $item ]
    set itemWindow [ lindex $itemPathNames $idx ]
    set item [ lindex $items $idx ]

    #
    # Destroy the status item
    #

    destroy $itk_component($item)

    
    #
    # Update the lists
    #

    set items [ lreplace $items $idx $idx ]
    set itemPathNames [ lreplace $itemPathNames $idx $idx ]

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# StatusBox::index
#
# INVOCATION:
# pathName index
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) item (number|"end"|pattern) A valid CStatusBox item identifier.
#
# number    - Specifies which widget numerically, where 0 is the topmost.
# "end"     - The last widget.
# pattern   - The "item" given with the "add" or "insert" method.
#
# PROCEDURE VALUE:
# Number:  Numerical index of the widget, if found
# Error:   if not found
#
# PURPOSE:
# Searched the widget item list for the one requested by "index".
#
# DESCRIPTION:
# If the given index is numerical make sure it is between 0 and the
# number of status items; if it is return the value, otherwise error.
# If the index is "end" return the numerical index of the last item.
# If the index is a string try and find it in the list of items, if
# found then return it's position, otherwise error.
#
# EXTERNAL VARIABLES:
# CStatusBox::items
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

body	CStatusBox::index {
    item
} {
    #
    # If there are no widgets there are no indexes.
    #
    if { [ set numWidgets [ llength $items ] ] <= 0 } {
        error "statusBox \"$itk_component(hull)\" has no widgets components."
    }

    if { [ regexp {^[0-9]+$} $item ] } {
        #
        # Numerical value.
        #
 
        if { $item < $numWidgets && $item >=  0 } {
	    return $item
	}  else {
            error "statusBox index \"$item\" is out of range, 0 to\
                   $numWidgets"
	}
    } elseif { $item == "end" } {
        #
        # end - last widget
        #
 
        return [ expr $numWidgets - 1 ]
    } else {
	if { [ set idx [ lsearch $items $item ] ] != -1 } {
	    
            #
            # The index was found in the list of check box tags.
            # 
 
            return $idx
        } else {
            error "bad statusBox index \"$item\": must be number between\
                0 and $numWidgets, \"end\", or a item identifier."
        }
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStatusBox::insert
#
# INVOCATION:
# pathName insert index tag ?option value option value ...?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) index     (string)  	A valid CStatusBox index.
# (>) widgetType(command) 	Any valid widget derived from the 
#				labeled widget class.
# (>) item      (string)        Identifier for the widget.
# (>) option    (string) 	Any valid option for widgetType
# (>) value     (string) 	Value for the widgetType option.
#
# where the index is a number, "end" or pattern.
# number    - Specifies which button numerically, where 0 is the topmost.
# "end"     - The last check button.
# pattern   - The "item" given with the "add" method
#
# PROCEDURE VALUE:
# item - The given item is returned.
#
# PURPOSE:
# To insert a widget, derived from the labeled widget, just before
# the one given by index.  The "item" is used to identify the 
# widget.  The option/value paris are applied to the widget
# when it is constructed.  The widget is a widgeType widget.
#
# DESCRIPTION:
# Find where the new status item is to be inserted.  After creating it
# insert it into the list and on the window
#
# EXTERNAL VARIABLES:
# items	    	(private class variable)    List of the labeled widgets.
# itemPathNames	(private class variable)    List of the labeled widgets
#					    window path names..
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CStatusBox::add
# CStatusBox::delete
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CStatusBox::insert {
    index
    widgetType
    item
    args
} {
    #
    # Find what will be just before the new inserted item.
    #

    set idx [ index $index ]
    set prevItem [ lindex $items $idx ]


    #
    # Create the new item
    #

    eval makeNewItem \"$widgetType\" \"$item\"  $args


    #
    # Display the new item in the appropriate postion
    #

    pack $itk_component($item) -before $itk_component($prevItem)	\
	-fill x -expand y


    #
    # Add the new item to our lists.
    #

    set items [ linsert $items $idx $item ]
    set itemPathNames [ linsert $itemPathNames $idx $item ]


    return "$item"
}

# ------------------------------------------------------------------
#                      PRIVATE  METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CStatusBox::makeNewItem
#
# INVOCATION:
# makeNewItem widgetType item  ?option value option value ...?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) widgetType(command) 	Any valid widget derived from the 
#				labeled widget class.
# (>) item      (string) 	Widget identifier.
# (>) option    (string) 	Any valid option for widgetType
# (>) value     (string) 	Value for the widgetType option.
#
# PROCEDURE VALUE:
# itemWindoe - The name of the new window created.
#
# PURPOSE:
# To create a  new widget, derived from the LabeledWidget class, with
# the identifier "item".  Any option/value pairs given are applied to the
# newly created widget.  
#
# DESCRIPTION:
# Create the new window.  Make sure its color and tags of the new window
# match the class colors and tags. 
#
# EXTERNAL VARIABLES:
# items	    	(private class variable)    List of the labeled widgets.
# itemPathNames	(private class variable)    List of the labeled widgets
#					    window path names..
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

body 	CStatusBox::makeNewItem {
    widgetType 
    item 
    args
} {    
    #
    # Create the new labeled widget (item).
    # 

    set bgColour [ $itk_component(hull) cget -background ]
    set itemWindow [ $this childsite ].item[ incr unique ]

    switch -- "$widgetType" {
	entryfield -
	optionmenu {
	    itk_component add $item {
		eval [ string tolower iwidgets::$widgetType ] $itemWindow 	\
		    -labelpos w $args 
	    } {
		keep -cursor 
		rename -background -statusbackground statusBackground 	\
		    Background
		rename -foreground -statusforeground statusForeground 	\
		    Foreground
		rename -labelpos -statuspos statusPos Position
		rename -labelmargin -statusmargin statusMargin Margin
	    }
	}
	labeledWidget -
	labeledwidget {
	    itk_component add $item {
		eval iwidgets::labeledwidget $itemWindow  -labelpos w $args 
	    } {
		keep -cursor 
		rename -background -statusbackground statusBackground 	\
		    Background
		rename -foreground -statusforeground statusForeground 	\
		    Foreground
		rename -labelpos -statuspos statusPos Position
		rename -labelmargin -statusmargin statusMargin Margin
	    }
	}
	CLogMonitor -
	cLogMonitor {
	    set argStart [ string first "-" "$args" ]
	    set input [ string range $args 0 [ expr $argStart -1 ] ]
	    set args [ string range $args $argStart end ]
	    itk_component add $item {
		eval cLogMonitor $itemWindow $input -labelpos w $args 
	    } {
		keep -cursor 
		rename -background -statusbackground statusBackground 	\
		    Background
		rename -foreground -statusforeground statusForeground 	\
		    Foreground
		rename -labelpos -statuspos statusPos Position
		rename -labelmargin -statusmargin statusMargin Margin
	    }
	}
	default {
	    itk_component add $item {
		eval $widgetType $itemWindow -labelpos w $args 
	    } {
		keep -cursor 
		rename -background -statusbackground statusBackground 	\
		    Background
		rename -foreground -statusforeground statusForeground 	\
		    Foreground
		rename -labelpos -statuspos statusPos Position
		rename -labelmargin -statusmargin statusMargin Margin
	    }
	}
    }


    #
    #  Make sure all bindings for the widget apply to its components
    #  and that the bg colour is correct.
    #

    foreach i [ $itk_component($item) component ] {
	set pathName [ $itk_component($item) component $i ]
	catch "$pathName configure -background $bgColour"
	bindtags $pathName [ linsert [ bindtags $pathName ] end		\
		 $itk_component($item) ]
    }

    return "$itemWindow"
}


