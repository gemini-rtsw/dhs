# $Id: table.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1997>				(c) 1997
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
# dhsConsole/src/table.itk
#
# PURPOSE:
# To create a widget that looks and acts like a table.
#
# CLASS NAME(S)
# CTable		- Creates a table widget.  Also allows column headings
#		  	  to be specified, and rows can be added and deleted
#		  	  to the table.
#
# METHOD NAME(S)
# None.
#
# $Log: table.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.13  1999/06/18 16:19:52  jaeger
# Added option definitions that should have been there.    Reduced the
# number of "usual" options.
#
# Revision 1.12  1999/06/11 22:18:55  jaeger
# Added -justify option.
#
# Revision 1.11  1998/08/05 23:06:12  jaeger
# Got rid of a miscellaneous "n" in the file - cause real neat things
# to happen.
#
# Revision 1.10  1998/07/22 20:06:53  jaeger
# Added comments to bring it up to Gemini standards.
#
# Revision 1.9  1998/07/16 18:19:03  jaeger
# Changed the name of the class to "CTable" and made "end" a valid
# row specification, it refers to the last (bottom-most) row in the
# table.
#
# Revision 1.8  1998/04/14 19:19:35  jaeger
# Fixed after commands.
#
# Revision 1.7  1998/04/08 20:50:50  jaeger
# Added methods for getting the window path name of rows and cells, adding
# -tablefont option.  Finished writing some of the missing methods.
#
# Revision 1.6  1998/03/18 22:39:04  jaeger
# Made sure the hull window was returned by the constructor.
#
# Revision 1.5  1998/03/06 17:49:44  jaeger
# Added labelmargin configbody and fixed up the packing of the windows.
#
# Revision 1.4  1997/12/04 17:23:36  jaeger
# Added documentation.  Added selectRow method - allows a specified row
# to be selected.
#
# Revision 1.3  1997/12/03 20:57:15  jaeger
# final revisiion, checked in for install
#
# Revision 1.2  1997/11/13 23:53:19  jaeger
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
# CTable
#
# OPTIONS:
# activebackground
#		Specifies background color to used when active.
# activerelief  Specifies the relief to used when active.
# background	Specifies the background color of the shell.
# borderwidth	Width of the 3-D border around the checkBox.
# cellbd	The size of the border around each cell.
# clientdata	A hook that clients can use to store data with each widget.
# columnwidths  Specifies the width of each column
# cursor	Specifies the mouse cursor to be used.
# elementborderwidth 
#		Specifies the width of borders drawn around the  internal  
#               elements  of the scrollbar (the two arrows and the
#               slider).
# foreground	Specifies the  normal  foreground  color  to  use  when
#		displaying the widget.
# headings 	Specifies the headings that appear above the columns.
# height	height
# highlightcolor
#		Specifies the color to use when the widget has the 
#		input focus.
# highlightthickness
#		Specifies the width of the highlight rectable to draw
#		around the widget when it has the input focus.
# hscrollmode	Specifies the scrolling mode for the horizontal scrollbar.
# jump		Specifies whether or not the scrollbars jump.
# justify	Specifies how the labels and columns are to be justified.
# labelbitmap	Specifies the label's bitmap.
# labelfont     Specifies the label's font.
# labelimage    Specifies the label's image.
# labelmargin	Specifies the margin between the label and the  childsite
# labelpos	Specifies the relative postion of the label with respect
#		    the childsite.
# labeltext     Specifies the text that appears in the label.
# labelvariable	Specifies the text variable of the label around the childsite.
# relief	Specifies the 3-D effect desired for the widget. Valid
#		    values are: raised, sunken, flat, ridge and groove.
# repeatdelay	Specifies the number of milliseconds a button or key
#		    must be held down before it gegins to auto-repeat.
# repeatinterval
#		Specifies the number of miliseconds between auto-repeats.
# sbwidth	Specifies the scrollbar width.
# scrollmargin  Specifies the margin between the scrollbar and the window.
# selectbackground
#		Specifies the color to use when displaying a selected item.
# selectborderwidth
#		Specifies the borderwidth to use when displaying a 
#		selected item.
# selectforeground
#		Specifies the color to use when displaying a selected item.
# tablefont	Specifies the font to use in the table portion of the
#		widget.
# troughcolor   Specifiest the color to use for the scrollbars trough.
# vscrollmode   Specifies the vertical scrollbar mode
# width		Specifies the width of the table.
#
# PUBLIC METHODS: 
# addCell	Add a cell at the end of a row.
# addRow	Add a row to the bottom of the table.
# cget		Standard cget method, returns the value of the specified
#			option.
# childsite	Returns the pathname of the childsite widget.
# component	Standard [ incr ] Tk component method.
# config	See configure
# configure	Standard Tk method, sets or returns options.
# deleteCell	Remove a cell from the table.
# deleteRow	Remove a row from the table.
# getCell	Get the contents of a cell.
# getCellPath
# getRow	Get the contents of a row.
# getRowPath	Returns the window path name of the given cell.	
# insertCell	Insert a cell before the given row/col specified.
# insertRow	Insert a row before a specified row.
# isa		Sets the widget's class.
# justify	Justifies the table via the scroll bars in one
#	        of four directions:  left, right, top, or bottom.
# selectRow	Returns the window path name of a given row.	
# selectedRow	Return the row index of the selected row, -1 if no
#		row is selected.
# setCell	Set the contents of a given cell.
# setRow	Set the contents of a given row.
# xview		Alter the horizontal view of the table.
# yview		Alter the vertical view of the table.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Create a widget that displayes a table and allows the table
# to be manipulated.
#
#-
#***********************************************************************
#

#
# Default resources.
#

option add *CTable.hscrollMode static widgetDefault
option add *CTable.headings {} widgetDefault
option add *CTable.justify center widgetDefault
option add *CTable.labelPos n widgetDefault
option add *CTable.relief sunken widgetDefault
option add *CTable.selectBackground #c3c3c3 widgetDefault
option add *CTable.selectBorderWidth 1 widgetDefault
option add *CTable.selectForeground Black
option add *CTable.scrollMargin 3 widgetDefault
option add *CTable.vscrollMode static widgetDefault


#
# Usual options.
#

itk::usual CTable {
    keep -background 							\
	-borderwidth 							\
	-cursor 							\
	-foreground 							\
	-height								\
	-hscrollmode							\
	-justify							\
	-tablefont							\
	-vscrollmode							\
    	-width
}



#
# The class definition.
#

class CTable {
    inherit iwidgets::Scrolledframe

    #
    # Option definitions.
    #

    itk_option define -cellbd cellBorderWidth BorderWidth 0
    itk_option define -columnwidths columnWidths ColumnWidths {}
    itk_option define -headings headings Headings {}
    itk_option define -height height Height {
	$itk_component(hull) configure -height $itk_option(-height)
    }
    itk_option define -hscrollmode hscrollMode HScrollMode {
	$itk_component(hull) configure -hscrollmode $itk_option(-hscrollmode)
    }
    itk_option define -justify justify Justify {}
    itk_option define -labelmargin labelMargin Margin ""
    itk_option define -labeltext labelText Text "" 
    itk_option define -selectbackground selectBackground Background #c3c3c3
    itk_option define -selectborderwidth selectBorderWidth BorderWidth 1
    itk_option define -selectforeground selectForeground Background Black
    itk_option define -tablefont tableFont Font 			\
	"-Adobe-Helvetica-Bold-R-Normal-*-*-120-*-*-*-*-*-*"
    itk_option define -vscrollmode vscrollMode VScrollMode {
	$itk_component(hull) configure -vscrollmode $itk_option(-vscrollmode)
    }
    itk_option define -width width Width {
	$itk_component(hull) configure -width $itk_option(-width)
    }


    #
    # Public methods and data
    #

    public method constructor { args } 
    public method destructor {}

    public method addRow { values }
    public method deleteRow { firstRow { lastRow "" } } 
    public method getRow { row } 
    public method getRowPath { row }
    public method insertRow { row values } 
    public method selectedRow {} 
    public method selectRow { row } 
    public method setRow { row  values } 

    public method addCell { row value }
    public method deleteCell { row col } 
    public method getCell { row col }
    public method getCellPath { row col }
    public method insertCell { row col value } 
    public method setCell { row col value } 

    public method xview { args }
    public method yview { args }


    #
    # Protected methods and data
    #

    # None.


    #
    # Private methods and data
    #

    private method configureFiller {} 
    private method configureFrame {} 
    private method fillerWidth {} 
    private method hScrollCommand { args } 
    private method maxWindowSize {} 
    private method packShellSlaves {} 
    private method selection { rowWindow } 
    private method updateFiller {} 

    private variable currentSelection -1
    private variable headingWidth 0
    private variable numColumns 0
    private variable repackPending ""
    private variable rowList {}
    private variable unique 0

    private method validateRow { row } 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cTable
#
# INVOCATION:
# cTable pahtName ?args?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName 	- The window path name of the new CTable object.
# (>) args 	- Arguments to pass to itk_initialize
#
# PROCEDURE VALUE:
# (string) pathName: name of the CTable object that is created, the
#		     window path name.
#
# PURPOSE:
# Provide a lower case access method to the CTable class.
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
# CTable::contructor
# CTable::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	cTable {
    pathName
    args
} {
    uplevel #0 ::CTable $name $args
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Arguments to pass to itk_initialize
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Construct for the "CTable" class.
#
# DESCRIPTION:
# 1. Create the headings for the table.
# 2. Bind the margins to "Configure" so that they are adjusted properly.
# 3. Evaluate the arguments
# 4. Set up the scrollbars so they have borders
# 5. make sure the label margin is packed in the right place.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CTable::constructor {
    args
} {


    #
    # Create a frame for the headings
    #
    
    itk_component add headFrame {
	frame $itk_component(shell).headFrame
    } {
	keep -background -cursor
    }


    itk_component add headCanvas  {
	canvas $itk_component(headFrame).headCanvas			\
	    -borderwidth 0 						\
	    -height 1.0							\
	    -highlightthickness 0 					\
	    -relief flat 						\
	    -scrollregion "0 0 1 1" 					\
	    -width 1.0 							
    } {
	keep -background -cursor
    }
    pack $itk_component(headCanvas) -side left -fill both -expand y



    #
    # Create a Frame inside canvas to hold widgets to be scrolled 
    # 
    #

    itk_component add headings {
	frame $itk_component(headCanvas).headings
    } {
	keep -background -cursor
    }
    pack $itk_component(headings) -fill both -expand yes -side top

    $itk_component(headCanvas) create window 1 1 -tags frameTag 	\
            -window $itk_component(headings) -anchor nw

    frame $itk_component(headCanvas).separator -background black 	\
	-borderwidth 2 -height 2 -relief flat 


    #
    # Create a frame to contain the horizontal margin
    #
    
    itk_component add headMargin {
	frame $itk_component(headFrame).margin -width [ fillerWidth ]
    } {
	keep -background -cursor
    }
    pack $itk_component(headMargin) -side right


    set itk_component(table) [ $this childsite ] 
    set itk_component(childsite) [ $this childsite ]

    bind $itk_component(headings) <Configure> [ code $this configureFrame ]
    bind $itk_component(headCanvas) <Configure> [ code $this configureFrame ]
    bind $itk_component(headMargin) <Configure> [ code $this configureFiller ]
    bind $itk_component(scrCanvas) <Configure> [ code $this configureFrame ]

    eval itk_initialize $args

    #
    # Set-up the scrollars so they work with the table.
    #

    $itk_component(hSB) config -borderwidth 2 -command 			\
           "[ code $this hScrollCommand ]"
    $itk_component(vSB) config -borderwidth 2

    $itk_component(lwchildsite) configure -relief sunken -bd 2
    $itk_component(label) configure -justify center
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::destructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Destructor for the "CTable" class.
#
# DESCRIPTION:
# If there are any repacks pending cancel them.
#
# EXTERNAL VARIABLES:
# repackPending (private class varible) - value returned from the after
#					  command when a repack was requested.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
body 	CTable::destructor {
} {
    if {$repackPending != ""} {after cancel $repackPending }
}



# ------------------------------------------------------------------
#                             OPTIONS
# ------------------------------------------------------------------


#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::cellbd
#
# INVOCATION:
# pathname configure -cellbd ?value?
#             OR
# CTable pathName -cellbd value
#
# where pathName is a window path of a "CTable" widget and value is a
# any value accepted by Tk_GetPixel.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the size of the border around each cell
#
# DESCRIPTION:
# If no headings are given then remove the headings frame from the display
# Else make sure the headings are visible and add the headings to the
#   display
#     
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

configbody 	CTable::cellbd {
    set i 0
    foreach heading $itk_option(-headings) {
	pack configure $itk_component(headings).heading$i 	\
	    -padx $itk_option(-cellbd)
	incr i
    }

    foreach row $rowList {
	foreach child [ winfo children $row ] {
	    $child configure -borderwidth $itk_option(-cellbd)
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::columnwidths
#
# INVOCATION:
# pathname configure -columnwidths ?value?
#             OR
# CTable pathName -columnwidths value
#
# where pathName is a window path of a "CTable" widget and value is a
# a list of pixel values, one pixel value for the number of cells in
# any given row.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the size of the width of each cell
#
# DESCRIPTION:
# Description of the algorithm.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

configbody 	CTable::columnwidths {
    if { $numColumns < [ llength $itk_option(-columnwidths) ] } {
	set numColumns [ llength $itk_option(-columnwidths) ]	
    }

    for { set idx 0 } { $idx < $numColumns } { incr idx } {
	set headingWidth 0
	if { [ winfo exists $itk_component(headings).heading$idx ] } {
	    $itk_component(headings).heading$idx config -width $width
	    set headingWidth [ expr $headingWidth + $width ]
	}

	foreach row $rowList {
	    if { [ catch [ $row.cell$idx configure -width $width ] ] } {
	    }
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::headings
#
# INVOCATION:
# pathname configure -headings ?value?
#             OR
# CTable pathName -headings value
#
# where pathName is a window path of a "CTable" widget and value is a
# a list of strings, one string for each heading.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the size of the headings that appear above the columns.
# 
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# repackPending  (private class variable)  Specifies if there is a repack
#					   of the shell slaves pending.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::packShellSlaves
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody 	CTable::headings {
    if { $numColumns < [ llength $itk_option(-headings) ] } {
	set numColuns [ llength $itk_option(-headings) ]
    }

    if { [ llength $itk_option(-headings) ] == 0 } {
	#
	# No headings specified so remove the headings from the display.
	#

	foreach heading [ winfo children $itk_component(headings) ] {
	    destroy $heading
	}

	$itk_component(headCanvas) config -xscrollcommand ""
	pack forget $itk_component(headFrame)
	return {}
    }

    if { [ lsearch [ pack slaves $itk_component(shell) ]		\
	       $itk_component(headFrame) ] == -1 } {
	#
	# We need to pack the headFrame
	# 

	pack $itk_component(headFrame) -anchor nw -expand n -side top 	\
	    -fill x -ipadx 0 -ipady 0 -padx 0 -pady 0


	if { $repackPending == "" } {
	    set repackPending [ after idle "[ code $this packShellSlaves ]" ]
	}
    }

    set height 0
    set headingNum 0
    set blankLine \
"                                                                                                                                                                                                                                          " 
    foreach heading $itk_option(-headings) {
	if { [ llength $itk_option(-columnwidths) ] > $headingNum } {
	    set width [ lindex $itk_option(-columnwidths) $headingNum ]
	} else {
	    set width 20
	}

	
	set heading "[string range $blankLine 0 [ expr $width - 1 ] ]\n${heading}"
        if { [ winfo exists $itk_component(headings).heading$headingNum ] } {
	    #
	    #  The heading exists so just change the text. 
	    #

            $itk_component(headings).heading$headingNum configure       \
                -text "$heading" -width $width
        } else {
	    #
	    #  The heading does not exists so create it.
	    #

	    label $itk_component(headings).heading$headingNum           \
                -borderwidth $itk_option(-cellbd)                       \
		-font "$itk_option(-tablefont)"				\
		-justify $itk_option(-justify)                         	\
    		-text "$heading"					\
		-width $width

	    pack $itk_component(headings).heading$headingNum -anchor w  \
                -side left
        }

	if { [ set curHeight [ winfo reqheight 			\
	     $itk_component(headings).heading$headingNum ] ] > $height } {
	    set height $curHeight
	}
	set headingWidth [ expr $headingWidth + $width ]

        incr headingNum
    }   


    #
    # make the height of the headings canvas big enough to display
    # the headings
    #

    $itk_component(headCanvas) configure -height [ expr $height + 6 ]
    $itk_component(headCanvas) coords separator 3 [ expr $height + 2 ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::justify
#
# INVOCATION:
# pathname configure -justify ?value?
#             OR
# CTable pathName -junstify value
#
# where pathName is a window path of a "CTable" widget and value is 
# either left, center, or right.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the cell justification.
#
# DESCRIPTION:
# Justify all of the labels followed by all of the cells.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None. 
#
# DEFICIENCIES:
# In order to force justification of the labels we need to put a blank
# before the text.  This adds extra space which may not be desired.
# The only reason why this is needed is because of a bug in the Tk
# label widget.
#-
#***********************************************************************
#

configbody 	CTable::justify {
    for { set idx 0 } { $idx < $numColumns } { incr idx } {
	#
	# Justify the text in all of the labels.
	#

	if { [ winfo exists $itk_component(headings).heading$idx ] } {
	    $itk_component(headings).heading$idx configure	\
		-justify $itk_option(-justify)
	}


	#
	# Justify all of the celss.
	#

	foreach row $rowList {
	    if { [ catch [ $row.cell$idx configure 		\
		       -justify $itk_option(-justify)  ] ] } {
	    }
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::labelmargin
#
# INVOCATION:
# pathname configure -labelmargin ?value?
#             OR
# CTable pathName -labelmargin value
#
# where pathName is a window path of a "CTable" widget and value is a
# string.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the margin to be displayed in the table's label.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# repackPending  (private class variable)  Specifies if there is a repack
#					   of the shell slaves pending.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::packShellSlaves
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody 	CTable::labelmargin { 
    set margin $itk_component(shell).labmargin
    $margin configure -width 1						\
	-height [ winfo pixels $margin $itk_option(-labelmargin) ]

    set packedItems [ pack slaves $itk_component(shell) ]
    if { [ lsearch $packedItems $margin ] == -1 } {
	#
	# Need to pack the label margin.
	#

	pack $margin -anchor center -expand 0 -fill none -ipadx 0 	\
	    -ipady 0 -padx 0 -pady 0 -side top
    }

    if { $repackPending == "" } {
	set repackPending [ after idle "[ code $this packShellSlaves ]" ]
    }	
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::labeltext
#
# INVOCATION:
# pathname configure -labeltext ?value?
#             OR
# CTable pathName -labeltext value
#
# where pathName is a window path of a "CTable" widget and value is a
# string.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the text to be displayed in the table's label.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# repackPending  (private class variable)  Specifies if there is a repack
#					   of the shell slaves pending.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::packShellSlaves
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody 	CTable::labeltext { 
    if { [ llength $itk_option(-labeltext) ] == 0 } {
	#
	# No label specified so don't display it.
	#

	pack forget $itk_component(label)
	return {}
    }

    if { [ lsearch [ pack slaves $itk_component(shell) ]		\
	       $itk_component(label) ] == -1 } {
	#
	# Need to pack the label component.
	#

	pack $itk_component(label) -anchor n -expand n -side top -fill none \
	    -ipadx 0 -ipady 0 -padx 0 -pady 0

	if { $repackPending == "" } {
	    set repackPending [ after idle "[ code $this packShellSlaves ]" ]
	}
    }


    #
    # Set the label's text.
    #

    $itk_component(label) config -text "$itk_option(-labeltext)"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::tablefont
#
# INVOCATION:
# pathname configure -tablefont ?value?
#             OR
# CTable pathName -tablefont value
#
# where pathName is a window path of a "CTable" widget and value is a
# valid font.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# n.a.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Specifies the font that is used for the text in the table, including
# the headings.
#
# DESCRIPTION:
# 1. Change the font of the headings
# 2. Change the font of each cell in the table.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# NOne.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

configbody	CTable::tablefont {
    set newFont $itk_option(-tablefont)

    set i 0
    foreach heading $itk_option(-headings) {
	$itk_component(headings).heading$i configure -font "$newFont"
	incr i
    }

    foreach row $rowList {
	foreach child [ winfo children $row ] {
	    $child configure -font "$newFont"
	}
    }
}
	
# ------------------------------------------------------------------
#                         PUBLIC  METHODS
# ------------------------------------------------------------------


#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::adRow
#
# INVOCATION:
# pathName addRow values
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) values	(list)  List of values to be added, 1 item per column.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To append a row to the table.
#
# DESCRIPTION:
# Create a new row at the bottom of the table.
# Foreach value given creats a cell (entry) whose contents is the
# value.
# Setup bindings for row selection.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::deleteRow
# CTable::insertRow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CTable::addRow {
    values
} {
    set newRow $itk_component(table).row[ incr unique ]
    frame $newRow
    pack $newRow -anchor w -expand y -side top -fill x

    if { [ llength $values ] > $numColumns } {
	set numColumns [ llength $values ]
    }

    set i 0
    foreach value $values {
	if { [ llength $itk_option(-columnwidths) ] > $i } {
	    set width [ lindex $itk_option(-columnwidths) $i ]
	} else {
	    set width 20
	}

        entry $newRow.cell$i                 				\
            -borderwidth $itk_option(-cellbd)                           \
            -font "$itk_option(-tablefont)"				\
	    -highlightthickness	0					\
	    -insertborderwidth 0					\
            -justify $itk_option(-justify)                             \
            -relief flat                                                \
            -state normal                                               \
            -width $width
 
        $newRow.cell$i insert 0 "$value"
        $newRow.cell$i config  -state disabled 
 
	bind $newRow.cell$i <Button-1> [ code $this selection $newRow ]

	pack $newRow.cell$i -side left
	incr i
    }

    lappend rowList $newRow
    return [ expr [ llength $rowList ] - 1 ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::deleteRow
#
# INVOCATION:
# pathName deleteRow firstRow ?lastRow?
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) firstRow 	(int)   Numerical row value, first row is 0.
# (>) lastRow	(int)   Numerical row value, first row is 0.
#
# PROCEDURE VALUE:
# (list) The contents of the row is returned as a list.
#
# PURPOSE:
# To remove a row from the table.
#
# DESCRIPTION:
# Validate the row values given.
# If the selected row is being deleted then deselect it.
# Foreach row in the range specified destroy it and all of its contents.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::insert
# CTable::add
#
# DEFICIENCIES:
# Should update numColumns if the number of columns has decreased.
#-
#***********************************************************************
#

body 	CTable::deleteRow { 
    firstRow
    { lastRow "" }
} {
    #
    # Verify the validity of rows
    #

    set firstRow [ validateRow $firstRow ]
    if { $lastRow == "" } {
	set lastRow $firstRow
    } elseif { $lastRow == "end"  } {
	set lastRow [ expr [ llength $rowList ] -  1 ]
    } else {
	set lastRow [ validateRow $lastRow ]
    }

    if { $lastRow < $firstRow } {
	return
    }


    set first $firstRow
    while { $firstRow <= $lastRow } {
	set rowWindow [ lindex $rowList $firstRow ]


	#
	# If the row being deleted is selected then adjust currentSelection.
	#

	if { $currentSelection == "$firstRow" } {
	    set currentSelection -1
	}


	#
	# Remove the bindings from the row to be deleted.
	#

	foreach window [ pack slaves $rowWindow ] {
	    if { [ winfo exists $window ] } {
		bind $window <Button-1>	{}
	    }
	}


	#
	# Remove the row.
	#

	destroy $rowWindow
	incr firstRow
    }

    set rowList [ lreplace $rowList $first $lastRow ]
    return 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::getCell
#
# INVOCATION:
# pathName getCell row col
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
# (>) col	(int) 	A numerical column identifier.
#
# PROCEDURE VALUE:
# (string) 	The contents of the specified table cell.
#
# PURPOSE:
# To find the contents of a table cell, given by "row" and "col".
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body 	CTable::getCell {
    row
    col
} {
    #
    # Verify validity of row and column values.
    #
    
    set row [ validateRow $row ]
    if { ! [ winfo exists [ lindex $rowList $row ].cell$col ] } {
	error "Invalid column value \"$col\""
    }


    #
    # Find the value at the given row and col
    #

    return [ [ lindex $rowList $row ].cell$col get ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::getCellPath
#
# INVOCATION:
# pathName getCell row col
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
# (>) col	(int) 	A numerical column identifier.
#
# PROCEDURE VALUE:
# (strinf) pathName : 	The window path name of the specified cell.
#
# PURPOSE:
# To find the path name of a table cell, given by "row" and "col".
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body	CTable::getCellPath {
    row
    col
} {
    set row [ validateRow $row ]
    set rowWindow [ lindex $rowList $row ]
    

    #
    # Validate column
    #

    if { ! [ regexp {^[ 0-9 ]+$} $row ] } {
	error "Invalid column \"$col\": all Table columns must be numerical\
values, left most column is 0"
    }

    if { ! [ winfo exists $rowWindow.cell$col ] } {
	error "Invalid column \"$col\""
    }

    return "$rowWindow.cell$col"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::getRow
#
# INVOCATION:
# pathName getRow row
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
#
# PROCEDURE VALUE:
# (list) values : The contents of the specified table row, each cell
#		  value is a separate item in the list.
#
# PURPOSE:
# To find the contents of a table row, given by "row".
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body	CTable::getRow {
    row
} {
    set row [ validateRow $row ]

    set valueList {}
    set rowWindow [ lindex $rowList $row ]
    
    foreach window [ pack slaves $rowWindow ] {
	if { [ winfo exists $window ] } { 
	    lappend valueList "[ $window get ]"
	}
    }
    return $valueList
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::getRowPath
#
# INVOCATION:
# pathName getRowPath row
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
#
# PROCEDURE VALUE:
# (string) pathname : The window path name of the specified row.
#
# PURPOSE:
# To find the path name of a row in the table, given by "row".
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body	CTable::getRowPath {
    row
} {
    set row [ validateRow $row ]
    return [ lindex $rowList $row ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::insertRow
#
# INVOCATION:
# pathName insertRow row values
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
# (>) values	(list)  List of values to be added, 1 item per column.
#
# PROCEDURE VALUE:
# (int) - the row index of the inserted row.
#
# PURPOSE:
# To insert a row before the given "row".  The first row is 0.
#
# DESCRIPTION:
# Create the new row before the specified row.
# For each value specified create a cell in the row, with the given
# value.
# Create all the necessary bindings.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::addRow
# CTable::deleteRow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CTable::insertRow { 
    row 
    values 
} {
    set newRow $itk_component(table).row[ incr unique ]
    frame $newRow

    if { [ llength $values ] > $numColumns } {
	set numColumns [ llength $values ]
    }

    if { [ llength $rowList ] == 0} { 
	set row 0
	pack $newRow -anchor w -expand y -side top -fill x 
    } else {
	set row [ validateRow $row ]
	set before [ lindex $rowList $row ]
	pack $newRow -anchor w -before $before -expand y -side top -fill x 
    }
    
    set i 0
    foreach value $values {
	if { [ llength $itk_option(-columnwidths) ] > $i } {
	    set width [ lindex $itk_option(-columnwidths) $i ]
	} else {
	    set width 20
	}

        entry $newRow.cell$i                 				\
            -borderwidth $itk_option(-cellbd)                           \
            -font "$itk_option(-tablefont)"				\
	    -highlightthickness	0					\
	    -justify $itk_option(-justify)				\
	    -insertborderwidth 0					\
            -relief flat                                                \
            -state normal                                               \
            -width $width
 
        $newRow.cell$i insert 0 "$value"
        $newRow.cell$i config  -state disabled 
 
	bind $newRow.cell$i <Button-1> [ code $this selection $newRow ]	
	pack $newRow.cell$i -side left
	incr i
    }

    set rowList [ linsert $rowList $row $newRow ]
    return [ expr $row ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::selectRow
#
# INVOCATION:
# pathName selectRow row
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
#
# PROCEDURE VALUE:
# positive int 	: If successful
# -1	       	: If not successful
#
# PURPOSE:
# To detemine which row selected.  The numerical row identifier is
# returned.  The first row is 0.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# currentSelection  (private class variable) Contains the current selection.
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

body	CTable::selectRow {
    row
} {
    set row [ validateRow $row ]
    selection [ lindex $rowList $row ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::selectedRow
#
# INVOCATION:
# pathName selectedRow
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# positive int 	: If a row is selected.
# -1	       	: If no rows are selected.
#
# PURPOSE:
# To detemine which row selected.  The numerical row identifier is
# returned.  The first row is 0.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# currentSelection  (private class variable) Contains the current selection.
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

body 	CTable::selectedRow {
} {
    if { $currentSelection == -1 } {
	return -1 
    } {
	return $currentSelection
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::setCell
#
# INVOCATION:
# pathName setCell row col
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
# (>) col	(int) 	A numerical column identifier.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To set the value of a particular cell.
#
# DESCRIPTION:
# If the cell already exists then change change the contents of the
# cell.  If it doesn't exist then create the cell, with the given
# value.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body	CTable::setCell {
    row
    col
    value
} {
    set row [ validateRow $row ]
    set rowWindow [ lindex $rowList $row ]

    if { ! [ winfo exists $rowWindow.cell${col} ] } {
	#
	# Create the cell
	#
	    
	if { [ llength $itk_option(-columnwidths) ] > $col } {
	    set width [ lindex $itk_option(-columnwidths) $col ]
	} else {
	    set width 20
	}
# May want to create padding cells if $col is greater than the last 
# cell.
	entry $rowWindow.cell$col                 		    	\
	    -borderwidth $itk_option(-cellbd)                          	\
	    -font "-Adobe-Helvetica-Bold-R-Normal--*-120-*-*-*-*-*-*"   \
	    -highlightthickness	0				    	\
	    -insertborderwidth 0				  	\
	    -justify $itk_option(-justify)                              \
	    -relief flat                                                \
	    -state normal                                               \
	    -width $width

 
	$rowWindow.cell$col insert 0 "$value"
	$rowWindow.cell$col config  -state disabled 
	
	bind $rowWindow.cell$col <Button-1> [ code $this 		\
		   selection $rowWindow ]
	pack $rowWindow.cell$col -side left
    } else {
	#
	# Change the contents of the cell
	#

	$rowWindow.cell${col} config -state normal
	$rowWindow.cell${col} delete 0 end 
	$rowWindow.cell${col} insert 0 "$value"
	$rowWindow.cell${col} config -state disabled
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::setRow
#
# INVOCATION:
# pathName setRow row
#
# where pathName is a window path.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int) 	A numerical row identifier.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To set the values shown in a particular row.
#
# DESCRIPTION:
# If the row already exists then change change the contents of the
# row.  If it doesn't exist then create the row, with the given
# value.  The first value in the given value list is placed in the
# leftmost cell.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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
body	CTable::setRow {
    row
    valueList
} {
    set row [ validateRow $row ]
    set rowWindow [ lindex $rowList $row ]

    set index 0
    foreach value $valueList {
	if { ! [ winfo exists $rowWindow.cell${index} ] } {
	    #
	    # Create the cell
	    #
	    
	    if { [ llength $itk_option(-columnwidths) ] > $i } {
		set width [ lindex $itk_option(-columnwidths) $index ]
	    } else {
		set width 20
	    }

	    entry $rowWindow.cell$index                 		\
		-borderwidth $itk_option(-cellbd)                       \
		-font "-Adobe-Helvetica-Bold-R-Normal--*-120-*-*-*-*-*-*"   \
		-highlightthickness	0				\
		-insertborderwidth 0					\  
		-justify $itk_option(-justify)                        	\
		-relief flat                                            \
		-state normal                                           \    
		-width $width

 
	    $rowWindow.cell$index insert 0 "$value"
	    $rowWindow.cell$index config  -state disabled 
 
	    bind $rowWindow.cell$index <Button-1> [ code $this 		\
			selection $rowWindow ]
	    pack $rowWindow.cell$index -side left
	} else {
	    #
	    # Change the contents of the cell
	    #

	    $rowWindow.cell${index} config -state normal
	    $rowWindow.cell${index} delete 0 end 
	    $rowWindow.cell${index} insert 0 "$value"
	    $rowWindow.cell${index} config -state disabled
	}
	
	incr index
    }
}

# ------------------------------------------------------------------
#                         PRIVATE  METHODS
# ------------------------------------------------------------------


#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::configureFiller
#
# INVOCATION:
# configureFiller
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set up to do an update of the filler if one is not all ready 
# planned.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# ScrolledFrame::_reconfigurePlanned	Indicates if there is a reconfigure
#					already planned.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::updateFiller
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CTable::configureFiller {
} {
    if { $_reconfigPlanned == "" } {
	set _reconfigPlanned [ after idle "[ code $this updateFiller ]" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::configureFrame
#
# INVOCATION:
# configureFrame
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Responds to configure events on the frame widget.  When the frame 
# changes size, adjust scrolling region size.
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
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CTable::configureFrame {
} {
    $itk_component(headCanvas) configure				\
	    -scrollregion [ $itk_component(headCanvas) bbox frameTag 	\
	    separator ] 

#    $itk_component(headCanvas).separator configure -width
#	[ winfo reqwidth $itk_component(table) ]
    _configureFrame
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::fillerWidth
#
# INVOCATION:
# fillerWidth
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# int : The width of the vertical scrollbar plus the verticl
#       margin.
#
# PURPOSE:
# Compute the width of the filler frame for the heading frame
# based on the vertical  scrollbar width plus the margin.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# ScrolledFrame::_vmode
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

body 	CTable::fillerWidth {
} {
    if { $_vmode == "on" } {
	return [ expr [ winfo reqwidth $itk_component(vSB) ] + \
		[ winfo reqwidth $itk_component(vMargin) ] ]
    } else {
	return 1
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::hScrollCommand
#
# INVOCATION:
# hScrollCommand
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - arguments that are accepted by the canvas xview command.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To adjust the xview of the table and of the headings do to a
# change in the horizontal scrollbar.
#
# DESCRIPTION:
# Change the xview of both the table and the headings.
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

body 	CTable::hScrollCommand { 
    args 
} {
    eval $itk_component(headCanvas) xview $args
    eval $itk_component(scrCanvas) xview $args
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::selection
#
# INVOCATION:
# selection selectedWindow
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) selectedWindow	(path name) 	The window path name of the
#					selected row.
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Given the selected row make the entire row appear as if it was
# selected.  This method is intended to by bound to a mouse click
# event, in a particular row.
#
# DESCRIPTION:
# 1. Deselect the current row, if there is one selected.
# 2. Select the given row, if it is valid.
#
# EXTERNAL VARIABLES:
# CTable::currentSelection  (int)   The numerical row index of the currently
#				   selected row.
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body 	CTable::selection { 
    rowWindow 
} {
    if { [ set newSelection [ lsearch $rowList $rowWindow ] ] == -1 } {
	error "Invalid Row selected"
    }

    if { $currentSelection != -1 } {
	set window [ lindex $rowList $currentSelection ]
	foreach cell [ winfo children $window ] 	{
	    $cell config -bg $itk_option(-background)
	}
    }

    set currentSelection $newSelection
    foreach cell [ winfo children $rowWindow ] {
	$cell config -bg $itk_option(-selectbackground)
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::packShellSlaves
#
# INVOCATION:
# packShellSlaves.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To make sure the children of the tables shell are packed 
# correctly.
#
# DESCRIPTION:
# If the label, labelmargin and headFrame are all packed make sure they
#   appear with the label on top, labelmargin, headFrame, then childsite.
# If the label and headFrame are packed make sure the appear the the label
#   on top then the headFrame, then the childsite.
# If only the headFrame is packed make sure it appears before the
# chilsite.
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

body 	CTable::packShellSlaves {
} {
    set shell $itk_component(shell)
    set packedItems [ pack slaves $shell ]


    if { [ lsearch $packedItems $itk_component(label) ] != -1 } {
	if { [ lsearch $packedItems $shell.labmargin ] != -1 } {
	    #
	    # Both the label and label margin are packed. So
	    # make sure the label appears at the top and the
	    # label margin below it.
	    #

	    pack configure $itk_component(label) -before	\
		$itk_component(shell).labmargin 

	    if { [ lsearch $packedItems $itk_component(headFrame) ] != -1 } {
		#
		# The heading frame is packed so make sure it appears
		# below the label. margin and before the childsite.
		#

		pack configure $itk_component(shell).labmargin 		\
		    -before $itk_component(headFrame) 			\
		    -after $itk_component(label) 

		pack configure $itk_component(headFrame) 		\
		    -after $itk_component(shell).labmargin		\
		    -before $itk_component(lwchildsite)
	    } else {
		pack configure $itk_component(shell).labmargin 	\
		    -after $itk_component(label) 
	    }
	} else {
	    #
	    # Ths label is packed but not the label margin.
	    #

	    pack configure $itk_component(label) 			\
    		-before $itk_component(lwchildsite)			
	    if { [ lsearch $packedItems $itk_component(headFrame) ] != -1 } {
		#
		# Make sure the headings frame, which is packed, appears
		# after the label and before the childsite.
		#

		pack configure $itk_component(headFrame) 		\
    		    -after $itk_component(label)			\
		    -before $itk_component(lwchildsite)
	    }
	}
    } elseif { [ lsearch $packedItems $itk_component(headFrame) ] != -1 } {
	#
	# Only the headings frame is packed, make sure it is above
	# the childsite.
	#
		pack configure $itk_component(headFrame) 		\
		    -before $itk_component(lwchildsite)
    }
    set repackPending ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::updateFiller
#
# INVOCATION:
# updateFiller
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Update the width of the filler following a configure event on 
# a scrollbar or margin.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# Scrolledframe::_reconfigPlanned	Indicates if a reconfiguration
#					is already planned.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CTable::confgiFiller
# CTable::fillerWidth
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CTable::updateFiller {
} {
    $itk_component(headFrame).margin config -width [ fillerWidth ] -height 1

    set _reconfigPlanned ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CTable::validateRow
#
# INVOCATION:
# validateRow row
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(int)	Numerical row index.
#
# PROCEDURE VALUE:
# row (int) :	The row is returned if it is valid.
# error	    :   If the row is not valid.
#
# PURPOSE:
# To determine if a value is a valid row index. "end" is a valid row,
# it is the last one.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CTable::rowList	(list)   List of the window pathNames for each row.
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

body 	CTable::validateRow {
    row
} { 
    #
    # If there are no rows there are no indexes.
    #

    if { [ set numRows [ llength $rowList ] ] <= 0 } {
        error "Table \"$itk_component(hull)\" has no rows."
    }

    if { [ regexp {^[ 0-9 ]+$} $row ] } {
        #
        # Numerical value.
        #
	
	if { $row < $numRows } {
	    return $row
	} else {
	    error "Invalid row \"$row\": must be a value between 0 and $numRows"
	}
    } elseif { $row == "end"} {
	return [ expr $numRows -  1 ]
    } else {
	error "Invalid row \"$row\": all Table rows must be numerical values, topmost row is 0"
    }
}

