# $Id: datasetDisplay.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/dataset.itk
#
# PURPOSE:
# A class for displaying datasets.
#
# CLASS NAME(S)
# CDatasetDisplay	: Displays the set of datasets the data server is
#			  aware of.
# 
# METHOD NAME(S)
# PUBLIC
# CDatasetDisplay::addRow	  : Adds a row to the dataset table.
# CDatasetDisplay::constructor	  : Constructor for the CDatasetDisplay class.
# CDatasetDisplay::clear	  : Clears the dataset table.
# CDatasetDisplay::deleteRow	  : Removes a row from the dataset table.
# CDatasetDisplay::destructor	  : Destructor for the CDatasetDisplay class.
# CDatasetDisplay::displayHeader  : Displays the FITS header of a dataset.
#
# PRIVATE
# CDatasetDisplay::constructButtons
#				  : Constructs the buttons on the window.
# CDatasetDisplay::constructTable : Constructs the dataset table.
# CDatasetDisplay::delete	  : Deletes the selected dataset
# CDatasetDisplay::deleteMonitor  : Method that is called by the dataset
#				    delete command.
# CDatasetDisplay::findDataset	  : Finds where a dataset is locaed in the
#				    table.
# CDatasetDisplay::selectedRow	  : Returns which row is selected in the table.
#
# GLOBAL PROCEDURE(S)
# cDatasetDisplay	: lowercased access method to the CDatasetDisplay
#			  class.
#
# $Log: datasetDisplay.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.12  1999/06/11 22:19:13  jaeger
# Added -justify option.
#
# Revision 1.11  1999/06/09 20:44:01  jaeger
# Re-order the buttons that are displayed.  Made sure all methods had
# a return statement.
#
# Revision 1.10  1998/07/20 19:40:39  jaeger
# moved bitmaps to "images" directory, code changed to reflect this.
#
# Revision 1.9  1998/07/16 19:54:53  jaeger
# Added short help and context-sensitive help.  Made changes to
# reflect changes in Table class.
#
# Revision 1.8  1998/06/18 20:37:40  jaeger
# Brought code documentation upt to Gemini standards.   Changed addRow to
# take each item that appears on the row.  Changed deleteRow to deleteMonit
# and wrote a proper public deleteRow method.
#
# Revision 1.7  1998/05/15 17:42:06  jaeger
# Added code so it works with the data server, instead of using
# test data.
#
# Revision 1.6  1998/05/04 20:38:56  jaeger
# Fixed the deletion of datasets.  Put double quotes around strings,
# including commands that return strings and variables that are
# strings.  Changed the heading, placed newlines in some of them.
#
# Revision 1.5  1998/04/14 19:05:00  jaeger
# Corrected after commands
#
# Revision 1.4  1998/03/06 17:48:17  jaeger
# Removed disk space usage from dataset display window.
#
# Revision 1.3  1997/12/03 20:57:15  jaeger
# final revisiion, checked in for install
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
# CDatasetDisplay
#
# OPTIONS:
# background    : Specifies the background color of the shell.
# buttonboxpadx : Specifies a non-negative padding distance to leave between 
#                 the button group and the outer edge of the button box in 
#                 the x direction. 
# buttonboxpady : Specifies a non-negative padding distance to leave between 
#                 the button group and the outer edge of the button box in 
#                 the y direction. 
# clientdata    : A hook that clients can use to store data with each widget.
# cursor        : Specifies the mouse cursor to be used.
# foreground    : Specifies the foreground color of the shell.
# master        : Defines the shell as being a transient window with the
#                 master window given by the master option
# modality      : Allows the shell to grab control of the screen in one of
#                 three different ways:  application, system, or none.
# padx          : Specifies a padding distance for the childsite in the 
#                 x direction
# pady          : Specifies a padding distance for the childsite in the 
#                 y direction
# separator     : Specifies whether a line is drawn to separate the buttons
#                 from the dialog box
# tablefont	: Specifies the font to be used in the table.
# thickness     : Specifies the thickness of the separator.
# title         : Specifies the title that appears above the window.
#
# PUBLIC METHODS: 
# activate      : Display the shell and wait based on the modality.
# add           : Add a button distinguished by tag to the end of the 
#		  button box.
# addRow	: Adds a row of information to the dataset table.
# buttonconfigure
#               : Allows the buttons to be configured.
# center        : Centers the shell with respect to another widget.
# cget          : Standard cget method, returns the value of the specified
#                       option.
# childsite     : Returns the pathname of the childsite widget.
# component     : Standard [ incr ] Tk component method.
# config        : See configure
# configure     : Standard Tk method, sets or returns options.
# deactivate    : Deactivate the display of the shell.
# default       : Sets the default button to the button given.
# deleteRow	: Removes a row from the dataset table.
# displayHeader	: Displays the header of a given dataset.
# hide          : Hides the button given.
# index         : Returns the numerical index corresponding to a button.
# insert        : Same as the add command except that it inserts the new button 
#                 just before the one given
# invoke        : Invoke the command associated with a button.    
# isa           : Sets the widget's class.
# show          : Display the button give, it was previously hidden.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To Display the datasets the data server is aware of.
#
#-
#***********************************************************************
#

itk::usual CDatasetDisplay {
    keep -background -cursor -foreground -modality -separator -thickness
}

itcl::class CDatasetDisplay {
    inherit iwidgets::Dialogshell


    #
    # Public methods and data
    #

    public method constructor { cmdObj masterWindow args } 
    public method destructor {} {}

    public method addRow { fileId fileName format volume size }
    public method clear {}
    public method deleteRow { fileId }
    public method displayHeader {} 


    #
    # Protected methods and data -- NONE
    #

    #
    # Private methods and data
    #

    private method constructButtons {}
    private method constructTable {}
    private method delete {} 
    private method deleteMonitor { row name status info }
    private method findDataset {}
    private method selectedRow {}

    private variable commandObject
    private variable fileIdList {}
    private variable headerList {}
    private variable shortHelpParent ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cDatasetDisplay
#
# INVOCATION:
# cDatasetDisplay name cmdObject arge
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Name of the CDhsCommand object.
# (>) cmdObject (CDtsCommands)  CDtsCommands object.
# (>) args	(string)	Valid attribute/value pairs.
#
# PROCEDURE VALUE:
# (strint) name : the name of the CDhsCommands object
#
# PURPOSE:
# Provide a lowercased access method for the CDatasetDisplay class.
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
 
proc 	cDatasetDisplay {
    name            
    cmdObject
    masterWindow
    args
} {
    return [ uplevel ::CDatasetDisplay $name $cmdObject $masterWindow $args ]
}

#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) cmdObject (CDhsCommands)		   Instance of CDhsCommands class.
# (>) args	(option/optionValue pairs) Arguments
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Construct a toplevel window for displaying datset.
#
# DESCRIPTION:
# Create a table and set up buttons to be displayed.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDatasteDisplay::constructButtons
# CDatasteDisplay::constructTable
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CDatasetDisplay::constructor {
    cmdObj
    masterWindow
    args
} {
    global ::dhgDir

    set commandObject "$cmdObj"
    set shortHelpParent [ winfo toplevel $masterWindow ]

    #
    # Set the size, title and icon information for the window.
    # 

    wm withdraw $itk_component(hull)
    wm iconname $itk_component(hull) "Datasets"
    wm iconbitmap $itk_component(hull) @$dhgDir/images/smallGemini.xbm
    wm minsize $itk_component(hull) 330 290
    wm geometry $itk_component(hull) 935x390

    $itk_component(hull) configure -title "Dataset List"
    $itk_component(hull) center


    #
    # Set-up the buttons and their commands.
    #

    constructButtons


    #
    # Make a frame to put everything in.  This is needed to make
    # things to appear correctly within the shell, ie. everything
    # isn't centered in relation to everything else.
    # 

    itk_component add theFrame {
	frame [ $itk_component(hull) childsite ].theFrame
    } {
	keep -background -cursor 
    }
    pack $itk_component(theFrame) -side top -fill both -anchor n -expand y

    
    #
    # Add the table with for the dataset information.
    #

    constructTable
    
    
    eval itk_initialize $args
	
    foreach comp [ $this component ] {
	bind [ $this component $comp ] <Button-3> 			\
	    "_helpInfo openUrl dhgDtsDataset.html#datasetWindow"
    }

    return "$itk_component(hull)"
}

# ------------------------------------------------------------------
#                       PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::addRow
#
# INVOCATION:
# Object addRow fileId data
#
# Where Object is and instance of the CDatasetDisplay class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) fileId	(string)	Unique file identifier.
# (>) data	(string)	The data to be displayed in the row.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To add a row of information to the display list.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CDatasetDisplay::fileIdList	- List all fileId's currently being viewed.
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

body	CDatasetDisplay::addRow {
    fileId
    fileName
    format
    volume
    size
} {
    set table $itk_component(table)
    if { $fileIdList == "" } {
	set fileIdList [ list $fileId ]
	$table addRow [ list "$fileName" "$format" "$volume" "$size" ]
    } elseif { [ lsearch $fileIdList $fileId ] == -1 } {
	lappend fileIdList $fileId
	$table addRow [ list "$fileName" "$format" "$volume" "$size" ]
    } else {
	errorDialog "fileId as already been added to the dataset List."
	return
    }
    

    #
    # Add short help messages
    #

    $shortHelpParent add_short_help [ $table getCellPath end 0 ]	\
	"Name of the file as it resides on disk (not fully specified)."
    $shortHelpParent add_short_help [ $table getCellPath end 1 ]	\
	"File format of the file, such as (F)ITS or (R)AW."
    $shortHelpParent add_short_help [ $table getCellPath end 2 ]	\
	"The volume where the file resides, such as the Data Server's Temporary Store (TEMP)"
    $shortHelpParent add_short_help [ $table getCellPath end 3 ]	\
	"How big the file is, in Kilobytes."

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::clear
#
# INVOCATION:
# Object clear
#
# Where Object is and instance of the CDatasetDisplay class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To clear the contents of the dataset display.
#
# DESCRIPTION:
# Trivial
#
# EXTERNAL VARIABLES:
# CDatasetDisplay::fileIdList	- List of file Id's being displayed
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDatasetDisplay::addRow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDatasetDisplay::clear {
} {
    $itk_component(table) deleteRow 0 end
    set fileIdList {}
    
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::deleteRow
#
# INVOCATION:
# Object delete fileId
#
# Where Object is and instance of the CDatasetDisplay class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) fileId	(string)	Unique file/dataset identifier.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Remove a parictular dataset from the table and from the temporary
# storage (uses bdDelete). 
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
# CDatasetDisplay::findDataset
# CDtsCommands::dtsDatasetDelete
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDatasetDisplay::deleteRow {
    fileId
} {
    if { [ set pos [ lsearch $fileIdList $fileId ] ] == -1 } {
	return
    }

    #
    # Attempt to delete the dataset, deleteRow is called
    #

    $commandObject dtsDatasetDelete $dataset "[ code $this deleteMonitor $pos ]"

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::displayHeader
#
# INVOCATION:
# Object displayHeader dataset
#
# Where Object is and instance of the CDatasetDisplay class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) dataset	(string)    Unique dataset identifier.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display a particular dataset's header in a window.
#
# DESCRIPTION:
# Find the dataset and call dtsDatasetHeader method with the dataset
# that was found.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDtsCommands::dtsDatasetHeader
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDatasetDisplay::displayHeader {
} {
    #
    # Find the desired dataset.
    #

    set dataset [ findDataset ]
    if { $dataset == "" } {
	errorDialog "Please select a dataset from the table."
    } else {
	if { $headerList == "" } {
	    set headerList [ list $dataset ]
	} elseif { [ lsearch $headerList $dataset ] == -1 } {
	    lappend headerList $dataset
	}
	$commandObject dtsDatasetHeader $dataset $shortHelpParent
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
# CDatasetDisplay::constructButtons
#
# INVOCATION:
# constructButtons
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# A helper function for the constructor, makes code more readable.
# Constructs the buttons to be displayed on the datasetDisplay window.
#
# DESCRIPTION:
# Create OK, Refresh, Header, Delete and Help buttons.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDatasetDisplay::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDatasetDisplay::constructButtons {
} {
    #
    # OK button
    #

    $this add OK -command "destroy $itk_component(hull)"		\
	-text "OK"
    $shortHelpParent add_short_help [ $itk_component(bbox) component OK ]\
	{{bitmap b1} Remove this window from the X display.}


    #
    # View header button
    #

    $this add Header -command [ code $this displayHeader ] 		\
	-text "Header"
    $shortHelpParent add_short_help [ $itk_component(bbox) component Header ]\
	{{bitmap b1} Display the FITS header for the selected dataset.}


    #
    # Delete dataset button
    #

    $this add Delete -command "[ code $this delete ]"			\
	-text "Delete"
    $shortHelpParent add_short_help [ $itk_component(bbox) component Delete ]\
	{{bitmap b1} Delete the selected dataset from DHS (only datastes in the "PERM" volume may be deleted).}


    #
    # Refresh button
    #

    $this add Refresh 							\
        -command "$commandObject dtsDatasetInfo $shortHelpParent"	\
	-text "Refresh"
    $shortHelpParent add_short_help [ $itk_component(bbox) component Refresh ]\
	{{bitmap b1} Query Sybase for an up-to-date list of datasets.}


    #
    # Help button.
    #

    $this add Help 							\
	-command "_helpInfo openUrl dhgDtsDataset.html#datasetWindow"	\
	-text "Help"
    $shortHelpParent add_short_help [ $itk_component(bbox) component Help ]\
       {{bitmap b1} Display on-line help information describing this window.}

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::constructTable
#
# INVOCATION:
# constructTable
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# A helper function for the constructor, makes code more readable.
# Constructs the table containing the list of datasets.
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
# CDatasetDisplay::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDatasetDisplay::constructTable {
} {
    itk_component add table {
	CTable $itk_component(theFrame).table				\
	    -columnwidths { 45 10 20 30 } 				\
	    -headings {"File Name" Format "Volume\n Name" "   Size\n(Kilobytes)" }\
	    -height 300							\
	    -hscrollmode static						\
	    -justify left						\
	    -labelmargin 15						\
	    -labeltext "Dataset List"					\
	    -labelpos n							\
	    -vscrollmode static						\
	    -width 500
    } {
	keep -cursor

	rename -background tablebackground tableBackground Background
	rename -foreground tableforeground tableForeground Foreground
    }
    pack $itk_component(table) -expand y -fill both -padx 2 -pady 2 	\
	-side top -anchor nw

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::delete
#
# INVOCATION:
# delete
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Remove a parictular dataset from the table and from the temporary
# storage (uses bdDelete). 
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
# CDatasetDisplay::deleteMonitor
# CDatasetDisplay::findDataset
# CDtsCommands::dtsDatasetDelete
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDatasetDisplay::delete {
} {
    set dataset [ findDataset ]
    if { $dataset == "" } {
	return
    }


    #
    # Attempt to delete the dataset, deleteRow is called
    #

    $commandObject dtsDatasetDelete $dataset "[ code $this deleteMonitor\
 [ selectedRow ] ]"

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::deleteMonitor
#
# INVOCATION:
# deleteMonitor row name status info
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) row	(integer)	Which row is associated with dataset.
# (>) name	(string)	Command monitor
# (>) status	(string)	Status of the command
# (>) info	(string)	Informative message
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Remove a particular row from the table if the bdDelete command
# succeeded.
#
# DESCRIPTION:
# If an error occured display the error in a dialog window, if the
# deletion succeeded then remove the row from the table, otherwise
# ignore it.  If the deletion succeeded make sure the header window,
# if there is one, for this dataset is also destoryed.
#
# This method is intended to be called by CDtsCommands::dtsDatasetDelete
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

body	CDatasetDisplay::deleteMonitor {
    row
    name
    status
    info
} {
    if { $status == "error" } {
	errorDialog "$info"
    } elseif { $status == "idle" } {
	set dataset [ lindex $fileIdList $row ]
	if { [ set pos [ lsearch $headerList $dataset ] ] != -1 } {
	    set window .__$dataset
	    if { [ winfo exists	$window ] } {
		catch { destroy $window } {}
	    }
	    set headerList [ lreplace $headerList $pos $pos ]
	}
	set fileIdList [ lreplace $fileIdList $row $row ]
	$itk_component(table) deleteRow $row
    } else {
	# DO NOTHING 
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDatasetDisplay::findDataset
#
# INVOCATION:
# findDataset
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None
#
# PROCEDURE VALUE:
# (string) : Selected, unique dataset identifier, if there is one selected.
# ""       : If no datsets are selected in the table.
#
# PURPOSE:
# To determine which dataset is currenetly selected in the dataset list.
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
# CDatasetDisplay::selectedRow
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDatasetDisplay::findDataset {} {
    set row [ selectedRow ]

    if { $row != -1 } {
	return [ lindex $fileIdList $row ]
    } else {
	return ""
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# DatatsetDisplay::selectedRow
#
# INVOCATION:
# selectedRow
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# (int) : Row currently selected in the table.
# -1    : If no row is selected.
#
# PURPOSE:
# To determine which row (dataest) is currenetly in the table.
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
# errorDialog
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDatasetDisplay::selectedRow {
} {
    set row [ $itk_component(table) selectedRow ]
    if { $row == -1 } {
	errorDialog "Please select one of the datasets"
	return -1
    } else {
	return $row
    }
}
