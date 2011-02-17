# $Id: form.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/form.tcl
#
# PURPOSE:
# Creates a form megawidget
#
# CLASS NAME(S)
# CForm				: Create a from that may contain multiple
#				  input widgets.  The form may be a top-level
#			   	  widget.  Each entry wiget has a label and
#				  the labels may be aligned.
#
# METHOD NAME(S)
# CForm::add			: Adds a new entry item to the form.
# CForm::align			: Aligns the labels of the entry items.
# CForm::constructor		: Constructor for the CForm class.
# CForm::delete			: Removes an entry item from the form.
#				  NOT IMPLEMENTED.
# CForm::destructor		: Destructor for the CForm class.
# CForm::index			: Returns the numerical position of a
#				  particular entry item.
# CForm::insert			: Inserts a new entry item onto the form.
#				  NOT IMPLEMENTED.
# CForm::itemConfigure		: Allows an entry item to be configured,
#				  Executes the configure method on it.
#
# GLOBAL PROCEDURE(S)
# cForm				: lower case access method to the 
#				   CForm class.
#
# $Log: form.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.1  1999/06/16 05:42:16  jaeger
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
# CForm
#
# OPTIONS:
# background		: Specifies the background.
# borderwidth		: Specifies width of the 3-D border that appears 
#		  	  around the form.
# clientdata		: A hook that clients can use to store data with 
#			  each widget.
# cursor		: Specifies the mouse cursor to be used.
# foreground    	: Specifies the normal foreground color.
# height		: Specifies the height of the widget.
# itembackground	: Background colour on the entry widgets.
# itemforeground	: Foreground colour on the entry widgets.
# itemmargin		: Margin between the label and its childsite for
#			  all of the entry widgets.
# itempos		: The relative position of the label with its
#			  respective childsite for each of the entry widets.
# relief		: Specifies the 3-D effect desired for the widget.
# width			: Specifies the width of the status box.
#
# PUBLIC METHODS: 
# add			: Add a labelled entry widget to the form.
# align			: Align the labels of the entry widgets.
# cget			: Standard cget method, returns the value of the 
#			  specified option.
# component		: Standard [incr] Tk component method.
# config		: See configure
# configure		: Standard Tk method, sets or returns options.
# delete		: Removes an entry widget from the form.
# index			: Find the numerical index of an entry widget, topmost
#		   	  one is 0.
# insert		: Insert an entry widget just before the one given.
# isa			: Sets the widget's class.
# itemConfigure		: Configure a partiuclar entry item(widget).
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None
#
# PURPOSE:
# To display a number of labelled entry widgets on a form.
#
#-
#***********************************************************************
#


#
# Default resources
#

option add *CForm.align 	yes 	widgetDefault
option add *CForm.borderWidth 	2 	widgetDefault
option add *CForm.height 	300 	widgetDefault
option add *CForm.itembackground #d9d9d9 widgetDefault
option add *CForm.labelPos 	w 	widgetDefault
option add *CForm.relief 	groove 	widgetDefault
option add *CForm.width 	500 	widgetDefault


#
# Usual options.
#

itk::usual CForm {
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

class CForm {
    inherit iwidgets::Scrolledframe
	
    #
    # Option definitions
    #

#    itk_option add hull.borderWidth
#    itk_option add hull.height 
#    itk_option add hull.relief
#    itk_option add hull.width

    itk_option define -itembackground itemBackground Background #d9d9d9
    itk_option define -itemforeground itemForeground Foreground Black
    itk_option define -itemmargin itemMargin Margin 1
    itk_option define -itempos itemPos Position w
	
    
    #
    # Public methods and data
    #
   
    public method constructor { args } {
	eval itk_initialize $args
	return "$itk_component(hull)"
    }

    public method destructor {} {}

    public method align {} {}
    public method add { id label widget args } 
    public method delete { index } 
    public method get { {index {}}  }
    public method index { index }
    public method insert { index id label widget item args } 
    public method itemConfigure { id component args }  


    #
    # Protected methods and data - NONE
    #


    #
    # Private methods and data
    #

    private method makeNewWidget { id label widget args }

    private variable unique 0		;# Unique entry identifier.
    private variable widgets {} 	;# List of labeled widgets.
    private common widgetValues		;# Array of values
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cForm
#
# INVOCATION:
# cForm pathName ?options?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)	A window path name for a nonexistent window.
# (>) options   (string)	One or more valid option/value pairs.
#
# The pathName and options are passed on to the CForm class.
#
# PROCEDURE VALUE:
# pathName - the window path name provided is returned.
#
# PURPOSE:
# To provide a lowercased access method for the CStatusBox class.
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
# CStatusBox::constructor
# CStatusBox::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc cForm { pathName args } {
    uplevel CForm $pathName $args
}

# ------------------------------------------------------------------
#                       PUBLIC  METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CForm::add
#
# INVOCATION:
# pathName add id label widget ?option value option value ...?
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) id      	(string) 	Widget identifier.
# (>) label	(string)	Text that will appear on the label.
# (>) widget	(string)	A valid tcl/tk widget (command).
# (>) option    (string) 	Any valid option for widgetType
# (>) value     (string) 	Value for the widgetType option.
#
# PROCEDURE VALUE:
# id (string): The given id is returned.
#
# PURPOSE:
# To add a new widget and an associated label with the identifier "id".
# Any option/value pairs are applied to the entry widget. The standard
# options which apply to a label are applied to the label portion.
#
# DESCRIPTION:
# Create the new widget and add it to the bottom of the form.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CForm::delete
# CForm::insert
# CForm::makeNewWidget
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CForm::add {
    id
    label
    widget
    args
} {
    #
    # Make the new widget
    #

    eval makeNewWidget \"$id\" \"$label\" \"$widget\" $args


    #
    # Display the widget at the bottom of the form.
    #

    pack $itk_component($id) -anchor w -expand yes -padx 4 -pady 4 -fill x


    #
    # Add the item and its path name to the internal lists.
    #

    lappend widgets $id

    return $id
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CForm::align
#
# INVOCATION:
# pathName align
#
# where pathName is a window path which is a CFrom widget.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To align the labels of all the widgets displayed on the form.
#
# DESCRIPTION:
# Use the CLabeledWidget align method to align the labels
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CLabeledWidget::align
#
# DEFICIENCIES:
# Only work with widgets whose label is to the west.
#-
#***********************************************************************
#

body	CForm::align { }  {
    CLabeledWidget::align $widgets

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CForm::delete
#
# INVOCATION:
# pathName delete index
#
# where pathName is a window path which is a CFrom widget.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) index	(integer|end|pattern)	A valid from index.
#
# integer	- Specifes a labeled widget numericall, where 0 it
#		  the topmost.
# end		- The last labeled widget.
# pattern	- The "id" given to the add or insert methods.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To remove a labeled widget from the form
#
# DESCRIPTION:
# Find the item to be removed and destroy it.  Also remove it from
# the internal list of widgets.
#
# EXTERNAL VARIABLES:
# CForm::widgets (list) :  List of all the widgets on the form.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CForm::add
# CForm::insert
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CForm::delete { 
    index
}  {
    #
    # Determine which widget is to be deleted.
    #

    set idx [ index $index ]
    set widget [ lindex $widgets $idx ]
    set variable [ $itk_component($widget) cget -widgetvariable ]

    #
    # Destroy the widget.
    #

    destroy $itk_component($widget)

    
    #
    # update variables.
    #

    unset $variable
    set widgets [ lreplace $widgets $idx $idx ]


    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CFrom::get
#
# INVOCATION:
# pathName get ?index?
#
# where pathName is a window path which is a CFrom widget.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) index	(integer|end|pattern)	A valid from index.
#
# integer	- Specifes a labeled widget numerically, where 0 it
#		  the topmost.
# end		- The last labeled widget.
# pattern	- The "id" given to the add or insert methods.
#
# PROCEDURE VALUE:
# value	(string)	The contents or value of the given labeled 
#			widget or all of the labeled widgets.
#
# PURPOSE:
# To retrieve and return the contents or value of the labeled widget.
#
# DESCRIPTION:
# If a widget was specified then return the value of that widget.  If no
# widget was given then find the value of all of them.
#
# EXTERNAL VARIABLES:
# CForm::widgets (list):	List of all the widgets on the form.
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

body	CForm::get { 
    {index {}}
}  {
    if { "$index" == "" } {
	#
	# Find the value of all items on the form.
	#

	set widgetList $widgets
    } else {
	#
	# Find the specified widget's value.
	#

	set idx [ index $index ]
	set widgetList [ lindex $widgets $idx ]
    }

    set retValue {}
    foreach widget $widgetList {
	#
	# Find the variable the value is stored in.
	#
    
	set var [ $itk_component($widget) cget -widgetvariable ]
	set arrayItem [ lindex [ split "$var" "(" ] 1 ]
	set arrayItem [ lindex [ split "$arrayItem" ")" ] 0 ]


	#
	# get the value
	#

	lappend retValue "$widgetValues($arrayItem)"
    }

    return "$retValue"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CForm::index
#
# INVOCATION:
# pathName index id
#
# where pathName is a window path which is a CFrom widget.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) id	(integer|end|pattern)	Unique wdiget identifier
#
# number	- Specifies wthe widget numerically, where 0 is the
#		  topmost one.
# "end"		- The last widget.
# pattern	- The "id" given matches an "id" previously given to
#		  an "add" or "insert" method.
#
# PROCEDURE VALUE:
# retVAlue (integer) : 	Numerical index of the labeled widget.
#
# PURPOSE:
# Find the index of the labeled widget whose identifier matches the
# widget identifier given. The widget identifiers are the values given
# to either the add or insert methods.
#
# DESCRIPTION:
# If the index given was an "id" then find the id in the widget list.
# if the index was an integer verify it's correctness and return the
# integer value (if valid).  If the "id" given was "end" given the index
# of the last widget.
#
# EXTERNAL VARIABLES:
# CForm::widgets (list):	List of all the widgets on the form.
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

body	CForm::index { 
    index 
}  {
    #
    # If there are no widgets there are no indexes.
    #

    if { [ set numWidgets [ llength $widgets ] ] <= 0 } {
        error "CForm \"$itk_component(hull)\" has no widget components."
    }


    if { [ regexp {^[0-9]+$} $index ] } {
        #
        # Numerical value.
        #

        if { $index < $numWidgets && $index >=  0 } {
            set retValue $index
        }  else {
            error "CFrom index \"$index\" is out of range; 0 to\
                   $numWidgets"
        }
    } elseif { $index == "end" } {
        #
        # end - last widget
        #
 
        set retValue [ expr $numWidgets - 1 ]
    } else {
        if { [ set idx [ lsearch $widgets $index ] ] != -1 } {
            #
            # The index was found in the list of widget identifiers.
            # 
 
            set retValue $idx
        } else {
            error "bad CForm index \"$index\": must be number between\
                0 and $numWidgets, \"end\", or a widget identifier."
        }
    }

    return "$retValue"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CForm::insert
#
# INVOCATION:
# pathName insert index id label widget args
#
# where pathName is a window path which is a CFrom widget.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) index	(integer)	Specifies the labeled widget which 
# (>) id      	(string) 	Widget identifier.
# (>) label	(string)	Text that will appear on the label.
# (>) widget	(string)	A valid tcl/tk widget (command).
# (>) option    (string) 	Any valid option for "widget"
# (>) value     (string) 	Value for the "widget" option.
#
# PROCEDURE VALUE:
# The widget identifier (id) given to the method.
#
# PURPOSE:
# To insert a labelled widget before the widget found at "index."
#
# DESCRIPTION:
# Find where the item is to be inserted.  Create the new widget and
# insert it onto the form.
#
# EXTERNAL VARIABLES:
# CForm::widgets (list):	List of all the widgets on the form.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CForm::add
# CFrom::delete
# CForm::makeNewWidget
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CForm::insert { 
    index 
    id 
    label 
    widget 
    args
}  {
    #
    # Find what goes before the new labeled widget
    #

    set idx [ index $index ]
    set prevWidget [ lindex $widgets $idx ]


    #
    # Create the new widget
    #

    eval makeNewWidget \"$id\" \"$label\" \"$widget\" $args


    #
    # Display the new widget in the appropriate position.
    #

    pack $itk_component($id) -before $itk_component($prevWidget)	\
	-anchor w -expand yes -padx 4 -pady 4 


    #
    # Add the item and its path name to the internal lists.
    #

    set widgets [ linsert $widgets $idx $id ]


    return $id
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CForm::itemConfigure
#
# INVOCATION:
# pathName itemConfigure index ?args?
#
# where pathName is a window path which is a CFrom widget.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) index	(integer|end|pattern)	A valid from index.
# (>) args	(options)	        Valid option/value pair for the 
#					given widget.
#
# integer	- Specifes a labeled widget numericall, where 0 it
#		  the topmost.
# end		- The last labeled widget.
# pattern	- The "id" given to the add or insert methods.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To configure a given labeled widget.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CForm::widgets (list):	List of all the widgets on the form.
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

body	CForm::itemConfigure { 
    id 
    component 
    args 
}  {
    set idx [ index $id ]
    set widget [ lindex $widgets $idx ]

    if { $component == "" } {
	eval $itk_component($widget) configure $args 
    } else {
	eval $itk_component($widget) component $component configure $args 
    }

    return
}

# ------------------------------------------------------------------
#                         PRIVATE  METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CFrom::makeNewWidget
#
# INVOCATION:
# makeNewWidget is label widget args
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) id      	(string) 	Widget identifier.
# (>) label	(string)	Text that will appear on the label.
# (>) widget	(string)	A valid tcl/tk widget (command).
# (>) option    (string) 	Any valid option for widgetType
# (>) value     (string) 	Value for the widgetType option.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To add a new labeled widget to the form.
#
# DESCRIPTION:
# Create an instance of the CLabeledWidget class.  Set-up the appropriate
# bindings for
#
# EXTERNAL VARIABLES:
# CForm::unique (integer): Unique numerical, widget identifier 
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CForm::add
# CFrom::insert
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CForm::makeNewWidget {
    id
    label
    widget
    args
} {
    #
    # Define the window pathname.
    # 
    
    set windowName [ $this childsite].item${unique}


    #
    # Create a frame which will contain label and widget.
    #

    itk_component add $id { 
	CLabeledWidget $windowName $widget
    } {
	keep   -cursor
	rename -background itembackground itemBackground Background
	rename -foreground itemforeground itemForeground Foreground
	rename -labelpos itemPos itemPos Pos
    }

    $windowName configure -labeltext "$label"
    $windowName configure -widgetvariable 			\
	"[ scope widgetValues(${this},${unique}) ]" 
    eval $windowName configure $args

    
    #
    #  Make sure all bindings for the widget apply to its components
    #

    foreach i [ $itk_component($id) component ] { 
	set pathName [ $itk_component($id) component $i ]
	bindtags $pathName [ linsert [ bindtags $pathName ] end \
		$itk_component($id) ]
    }
    
    incr unique

    return
}
