# $Id: dhsQltCtl.tcl,v 1.4 2004-10-12 08:55:12 brighton Exp $
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
# dhsQlTool/src/DhsQltCtl.tcl
#
# PURPOSE:
# Contains a class to define and control the DHS QLT control window.
#
# CLASS NAME(S)
# CDhsCustomStreams	_ The itk widget class for the custom stream window.
# CDhsImageArithmetic	- The itk widget class for the image arithmetic setup
#			  window.
# CDhsQltCtl		- The itk widget class for the DHS control window.
# CDhsStreamSelect	- The itk widget class for the server defined stream
#			  selection window.
#
# METHOD NAME(S)
# CDhsCustomStreams::constructor
#			- Constructor for the CDhsCustomStreams class.
# CDhsCustomStreams::add
#			- Add a stream to the custom streams.
# CDhsCustomStreams::addStream
#			- Copy a stream name from the entry box to the stream
#			  listbox in the CDhsCustomStreams widget.
# CDhsCustomStreams::clear
#			- clear the list of streams.
# CDhsCustomStreams::display
#			- Activate the CDhsCustomStreams widget.
# CDhsCustomStreams::loadStreamFile
#			- Add a list of streams from a file to the stream 
#			  listbox in the CDhsCustomStreams widget.
# CDhsCustomStreams::okButton
#			- Executed when the CDhsCustomStreams widget ok button
#			  is pressed.
# CDhsCustomStreams::removeSelected
#			- Executed when the CDhsCustomStreams widget delete
#			  button is pressed.
# CDhsCustomStreams::saveStreamFile
#			- Save the current contents of the CDhsCustomStreams
#			  widget stream listbox to a file.
# CDhsCustomStreams::streamList
#			- Return the current list of custom streams.
# CDhsImageArithmetic::constructor
#			- Constructor for the CDhsImageArithmetic class.
# CDhsImageArithmetic::display
#			- Display the image arithmetic setup widget.
# CDhsImageArithmetic::dsGetCallback
#			- Callback used by the qlsDsGet command when 
#			  retrieving an operand dataset.
# CDhsImageArithmetic::getDatasetName
#			- Get the currently set operand dataset.
# CDhsImageArithmetic::getFrameId
#			- Get the currently set operand frame id.
# CDhsImageArithmetic::getOperand
#			- Get the currently set operand.
# CDhsImageArithmetic::getOperator
#			- Get the currently set operator.
# CDhsImageArithmetic::setDataset
#			- Set the currently set operand dataset.
# CDhsImageArithmetic::setOperator
#			- Set the currently selected operator.
# CDhsImageArithmetic::setOperand
#			- Set the operand files for the arithmatic.
# CDhsQltCtl::constructor	
#			- Constructor for the CDhsQltCtl class.
# CDhsQltCtl::destructor	
#			- Destructor for the CDhsQltCtl class.
# CDhsQltCtl::ImageArithmetic
#			- Method to set up the image arithmetic.
# CDhsQltCtl::datasetDisplay
#			- Method to prompt for the name of a dataset and
#			  arrange for the dataset to be displayed.
# CDhsQltCtl::displaySave
#			- Save the current displayed image to permanent store.
# CDhsQltCtl::selectFrame
#			- Executed when a frame is selected in the frame
#			  listbox.
# CDhsQltCtl::setFrameList
#			- Get the list of frames in the cQltFrame class and
#			  put it into the frame listbox.
# CDhsQltCtl::streamSet	- Put the list of subscribed streams into the stream
#			  listbox.
# CDhsStreamSelect::constructor
#			- Constructor for the CDhsStreamSelect class.
# CDhsStreamSelect::display
#			- Display the stream select widget window.
# CDhsStreamSelect::populate
#			- This method is executed when a response to a
#			  qlsQueryStreams command is received.
# CDhsStreamSelect::selectStreams
#			- Set the list of quick look streams and notify
#			  the parent CDhsQltCtl object that the list should
#			  be updated.
# CDhsStreamSelect::streamList
#			- Return the current list of selected streams.
#
# $Log: not supported by cvs2svn $
# Revision 1.3  2004/08/24 13:57:13  brighton
# Linux port (see PORTING_NOTES, especially the part at the end labeled "Other Changes")
#
# Revision 1.2  2004/08/13 20:18:54  brighton
# Linux test/port
#
# Revision 1.1.1.1  2002/11/24 20:29:48  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.13  1999/02/05 18:44:29  nhill
# Removed unused method "ImageArithmetic. Documentation typo fixes.
#
# Revision 1.12  1999/01/29 17:55:14  nhill
# Improved the error checking in CDhsQltCtl::displaySave.
#
# Revision 1.11  1999/01/25 20:26:31  nhill
# Bug fixes.
#
# Revision 1.10  1999/01/21 00:49:47  nhill
# Added image arithmetic to the interface.
#
# Revision 1.9  1998/10/08 19:39:48  nhill
# Changed to allow more configuration information to be saved.
#
# Revision 1.8  1998/07/22 18:56:25  jaeger
# Added on-line help to the Quick look tool and an "About" window.
# Also put "underlines" on the menu selections so the keyboard can
# be used to access them.
#
# Revision 1.7  1998/05/05 17:09:41  nhill
# Changed the image name to a variable.
#
# Revision 1.6  1997/12/10 21:56:01  nhill
# Added automatic skipping of frames.
#
# Revision 1.5  1997/11/30 21:36:47  nhill
# Fixed stream names to allow spaces in the names.
#
# Revision 1.4  1997/11/12 18:28:20  nhill
# Removed unused variable dssFrameList from the cQltFrame class.
#
# Revision 1.3  1997/10/24 22:21:08  nhill
# Beta check in.
#
# Revision 1.2  1997/10/01 17:18:37  nhill
# Checkpoint for install.
#
# Revision 1.1  1997/09/19 17:24:35  nhill
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
# CDhsCustomStreams
#
# PUBLIC METHODS: 
# constructor	- Constructor for the CDhsCustomStreams class.
# addStream	- Copy a stream name from the entry box to the stream listbox 
#		  in the CDhsCustomStreams widget.
# display	- Activate the CDhsCustomStreams widget.
# loadStreamFile- Add a list of streams from a file to the stream listbox in 
#		  the CDhsCustomStreams widget.
# okButton	- Executed when the CDhsCustomStreams widget ok button is 
#		  pressed.
# removeSelected- Executed when the CDhsCustomStreams widget delete button 
#		  is pressed.
# saveStreamFile- Save the current contents of the CDhsCustomStreams widget 
#		  stream listbox to a file.
# streamList	- Return the current list of custom streams.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# This class is an itk widget that displays a form for changing the list
# of custom quick look streams currently subscribed
#
#-
#***********************************************************************
#

itk::usual	CDhsCustomStreams

itcl::class CDhsCustomStreams {
    inherit	util::TopLevelWidget


    #
    # Public methods and data
    #

			constructor { parent args } {}
    method		add { streamName }
    method		addStream {}
    method		clear {}
    method		display {}
    method		loadStreamFile {}
    method		okButton {}
    method		removeSelected {}
    method		saveStreamFile {}
    method		streamList {}


    #
    # Private methods and data
    #

    private variable	dcsCustomStreams {}
    private variable	dcsParent
};
usual CDhsCustomStreams {
}

#
#***********************************************************************
#+
# CLASS NAME:
# CDhsImageArithmetic
#
# PUBLIC METHODS: 
# constructor	- Constructor for the class.
# display	- Display the image arithmetic setup widget.
# dsGetCallback	- Callback used by the qlsDsGet command when retrieving 
#		  an operand dataset.
# getDatasetName- Get the operand dataset name.
# getFrameId	- Get the operand frame id.
# getOperand	- Get the currently selected operand.
# getOperator	- Get the currently selected operator.
# getOperatorName- Get the string corresponding to the currently selected 
#		  operator.
# ready		- Determine whether the data processing has received the
#		  operand frame, and is therefore ready to go.
# setArith	- Get the arithmetic parameters from the user interface
#		  and and set up the arithmetic.
# setDataset	- Set the operand dataset name.
# setFrameId	- Set the operand frame id.
# setOperator	- Set the currently selected operator.
# setOperand	- Set the operand files.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# This class is used to modify the currently selected image arithmetic.
#
#-
#***********************************************************************
#

itcl::class CDhsImageArithmetic {
    inherit	util::TopLevelWidget


    #
    # Public methods and data
    #

		constructor { parent args } {}
    method	display {}
    method	dsGetCallback { monitorName cmdStatus info }
    method	getDatasetName {} { return $iaDatasetName }
    method	getFrameId {} { return $iaFrameId }
    method	getOperand {} 
    method	getOperator {}
    method	getOperatorName {} { return $iaOperator }
    method	ready {} { return [ expr { $iaHeaderFName != {} && 	\
			$iaDataFName != {} } ] }
    method	setArith {}
    method	setDataset {} { 
		    if { $iaDatasetName != {} } { 
			cQlServer::datasetGet $iaDatasetName "$this dsGetCallback"
		    }
		}
    method	setDatasetName { datasetName }  
    method	setFrameId { frameId } { set iaFrameId $frameId }
    method	setOperator { operator }
    method	setOperand { headerFName dataFName }


    #
    # Private methods and data
    #

    private variable	iaOperator
    private variable	iaDatasetName
    private variable	iaFrameId
    private variable	iaHeaderFName
    private variable	iaDataFName
    private common	iaOperatorList {none add subtract multiply divide mask}
};
usual CDhsImageArithmetic {
}

#
#***********************************************************************
#+
# CLASS NAME:
# CDhsQltCtl
#
# PUBLIC METHODS: 
# constructor	- Constructor for the CDhsQltCtl class.
# destructor	- Destructor for the CDhsQltCtl class.
# ImageArithmetic- Method to set up the image arithmetic.
# datasetDisplay- Method to prompt for the name of a dataset and arrange for 
#		  the dataset to be displayed.
# displaySave	- Save the current displayed image to permanent store.
# selectFrame	- Executed when a frame is selected in the frame listbox.
# setFrameList	- Get the list of frames in the cQltFrame class and put it 
#		  into the frame listbox.
# streamSet	- Put the list of subscribed streams into the stream listbox.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# The class containing the QLT control window
#
#-
#***********************************************************************
#

itk::usual	CDhsQltCtl
itcl::class CDhsQltCtl {
    inherit	util::TopLevelWidget


    #
    # Public methods and data
    #

    			constructor { args } {}
    			destructor { 
			   if { [ find objects ::_qltHelp ] != "" } {
			       delete object _qltHelp
			   }
			    exit 
			}
    method		datasetDisplay {}
    method		displaySave {}
    method		initialize {}
    method		selectFrame {}
    method		streamSet {}
    method		setFrameList {}
}
usual CDhsQltCtl {
}

#
#***********************************************************************
#+
# CLASS NAME:
# CDhsStreamSelect
#
# PUBLIC METHODS: 
# constructor	- Constructor for the CDhsStreamSelect class.
# display	- Display the stream select widget window.
# populate	- This method is executed when a response to a qlsQueryStreams 
#		  command is received.
# selectStreams	- Set the list of quick look streams and notify the parent 
#		  CDhsQltCtl object that the list should be updated.
# streamList	- Return the current list of selected streams.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# This class is an itk widget that displays a form for changing the list
# of Quick Look Server defined streams
#
#-
#***********************************************************************
#

itk::usual	CDhsStreamSelect
itcl::class CDhsStreamSelect {
    inherit	util::TopLevelWidget


    #
    # Public methods and data
    #

			constructor { parent args } {}
    method		display {}
    method		populate { a b c }
    method		selectStreams { }
    method		streamList { }
    method		unSubscribeAll { }


    #
    # Private methods and data
    #

    private variable	dssParent {}
    private variable	dssSelectedStreams {}
};
usual CDhsStreamSelect {
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args - Arguments to pass to itk_initialize
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Constructor for the CDhsCustomStreams class
#
# DESCRIPTION:
# Create each of the components of the custom stream select widget.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCustomStreams::constructor { 
    parent
    args 
} {
    util::TopLevelWidget::constructor -center 0 -standalone 0 		\
	    -title "Define Custom Streams"

    eval itk_initialize $args 

    set dcsParent $parent
    bind $itk_component(hull) <Button-3> "_qltHelp openUrl qltCustomStream.html"


    #
    # Set up the menu.
    #

    $itk_interior make_short_help
    $itk_interior add_menubar


    #
    #  Add a file menu.
    #

    set fileMenu [ $itk_interior add_menubutton "File" ]
    $this component file configure -underline 0
    add_short_help $itk_interior.menubar.file	 			\
	    {File menu: load or save custom streams}

    add_menuitem $fileMenu command "Load custom streams..." 		\
	    {Read stream names from a file and add to the list}		\
	    -command [ code $itk_interior busy "$this loadStreamFile" ]	\
	    -underline 0
    
    add_menuitem $fileMenu command "Save custom streams..." 		\
	    {Save stream names to a file}				\
	    -command [ code $itk_interior busy "$this saveStreamFile" ]	\
	    -underline 0

    
    #
    #  Add a help menu.
    #

    set helpMenu [ $itk_interior add_menubutton "Help" right ]
    $this component help configure -underline 0
    add_short_help $itk_interior.menubar.help 				\
	    {Help menu: Show information about this pannel}
    	
    add_menuitem $helpMenu command "About QLT..."		        \
	{Display a window with the Quick Look Tool version information.} \
	-command "displayAbout"						\
	-underline 0

    add_menuitem $helpMenu command "Help..."			        \
	{Display help information about the control panel in Netscape.}	\
	-command "_qltHelp openUrl qltCustomStream.html"		\
	-underline 0


    #
    #  Create an entrybox for adding new streams
    #

    itk_component add streamEntry {
	Entryfield $itk_interior.entry -width 40 			\
		-labeltext "Enter new custom stream"			\
		-labelpos n -command [ code $this addStream ]
    }
    add_short_help $itk_component(streamEntry) 				\
	    {Type the name of the new stream followed by enter.}
    pack $itk_component(streamEntry) -side top -expand yes -fill both 	\
	    -padx 10 -pady 10


    #
    # Set up a list of currently selected custom streams.
    #

    itk_component add streamList {
	scrolledlistbox $itk_interior.streamList 			\
	    -labeltext "Current Custom Streams" -hscrollmode dynamic	\
	    -vscrollmode dynamic -selectmode extended
    }
    add_short_help $itk_component(streamList) 				\
	    {Custom streams. {bitmap b1} = Select stream. Extended list select bindings.}
    pack $itk_component(streamList) -side top -expand yes -fill both	\
	    -padx 10 -pady 10


    #
    # Create the button box at the bottom of the window.
    #

    itk_component add bbox { buttonbox $itk_interior.bbox }


    $itk_interior.bbox add ok -text "Ok" -command [ code $this okButton ]
    $itk_interior.bbox add rm -text "Delete" 				\
	    -command [ code $this removeSelected ]
    $itk_interior.bbox add reset -text "Reset" 				\
	    -command [ code $this display ]
    $itk_interior.bbox add cancel -text "Cancel" 			\
	    -command [ code wm withdraw $itk_interior ]
    pack $itk_interior.bbox -side bottom -expand yes -fill both

    add_short_help [ $itk_component(bbox) component ok ]		\
	    {Set the custom streams and close window.}
    add_short_help [ $itk_component(bbox) component rm ]		\
	    {Remove selected streams from the list.}
    add_short_help [ $itk_component(bbox) component reset ]		\
	    {Revert to previous streams.}
    add_short_help [ $itk_component(bbox) component cancel ]		\
	    {Revert to previous streams and close window.}
    

    #
    # Hide the window.
    #

    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::add
#
# INVOCATION:
# object add streamName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) streamName (string) The name of the new stream.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Add a new stream to the custom stream list
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::add {
    streamName
} {
    if { $streamName != {} } {
	lappend dcsCustomStreams $streamName
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::addStream
#
# INVOCATION:
# window addStream 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Copy the name from the entry box to the list of streams
#
# DESCRIPTION:
# Get the value from the entry box, and if the name isn't already in the
# list of streams, insert it.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::addStream {
} {
    #
    # Get the stream name.
    #

    set streamName [ $itk_component(streamEntry) get ]



    if { $streamName != {} } {
	#
	# Get the current list of streams from the list box.
	#

	set streamList [ $itk_component(streamList) get 0 end ]

	
	#
	# If the new stream isn't already in the list of streams, insert
	# the new stream into the list.
	#

	if { [ lsearch -exact $streamList $streamName ] == -1 } {
	    $itk_component(streamList) insert end $streamName
	}


	#
	# Clear the entry box.
	#

	$itk_component(streamEntry) delete 0 end
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::clear
#
# INVOCATION:
# object.clear
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Clear the list of custom streams
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::clear {
} {
    $itk_component(streamList) delete 0 end 
    set dcsCustomStreams {}
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::display
#
# INVOCATION:
# object.display
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display the CDhsCustomStreams widget and intialize the stream list
# to the current set of custom streams.
#
# DESCRIPTION:
# - Map the widget.
# - Copy the list of streams into the stream listbox.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsCustomStreams::display {
} {
    #
    # Display the widget.
    #

    wm deiconify $itk_interior
    raise $itk_interior


    #
    # Clear the stream listbox.
    #

    $itk_component(streamList) clear 


    #
    # Put the current set of custom streams into the listbox.
    #

    if { $dcsCustomStreams != {} } {
	eval $itk_component(streamList) insert 0 $dcsCustomStreams
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::loadStreamFile
#
# INVOCATION:
# object loadStreamFile
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Add a list of streams in a file to the stream list.
#
# This method should probably be changed to ignore comments in the input
# file.
#
# DESCRIPTION:
# - If necessary, create the file selection widget.
# - Display the file selection widget and wait for a file to be selected.
# - Open the file and insert the streams read from the file into the stream
#   listbox.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::loadStreamFile {
} {
    #
    # If the file selection widget doesn't already exist, create it.
    #

    if { ! [ info exists itk_component(fileSelect) ] ||			\
	    ! [ winfo exists $itk_component(fileSelect) ] } {
	itk_component add fileSelect {
	    FileSelect $itk_interior.fs -full 0
	}
    }


    #
    # Display the file selection widget and wait for the user to select
    # the file.
    #

    if { [ $itk_component(fileSelect) activate ] } {
	#
	# Open the selected file.
	#

	set fileId [ open [ $itk_component(fileSelect) get ] r ]


	#
	# Read the streams from the file.
	#

	set newStreams [ split [ read $fileId nonewline ] "\n" ]
	close $fileId


	#
	# Get the existing list of streams.
	#

	set streamList [ $itk_component(streamList) get 0 end ]


	#
	# Add each stream to the list of streams.
	#

	foreach stream $newStreams {
	    if { $stream != {} } {
		#
		# If the stream isn't already in the list of streams, add it.
		#

		if { [ lsearch -exact $streamList $stream ] == -1 } {
		    $itk_component(streamList) insert end $stream
		    lappend streamList $stream
		}
	    }
	}
    } 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::okButton
#
# INVOCATION:
# object selectStreams
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Copy the streams from the stream list box to the streams data member
#
# DESCRIPTION:
# - Copy the steams from the listbox to the list of custom streams.
# - Notify the parent that the list has changed.
# - Hide the custom stream widget.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::okButton {
} {
    #
    # Read the streams from the listbox.
    #

    set dcsCustomStreams [ $itk_component(streamList) get 0 end ]


    #
    # Notify the parent CDhsQltCtl object that the list of streams has changed.
    #

    $dcsParent streamSet


    #
    # Hide the custom stream widget.
    #

    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::removeSelected
#
# INVOCATION:
# object removeSelected
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Remove the selected streams from the list of custom streams
#
# DESCRIPTION:
# - Get the indexes of the streams selected in the listbox
# - Delete the streams from the listbox. 
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# The streamList object must exist.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::removeSelected {
} {
    #
    # Get the indexes of the selected streams.
    #

    set selected [ $itk_component(streamList) curselection ]


    #
    # Delete each of the streams, in order of decreasing index number so that
    # the deletions don't change the index numbers of the rest of the streams
    # to be deleted.
    #

    foreach i [ lsort -integer -decreasing $selected ] {
	$itk_component(streamList) delete $i
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::saveStreamFile
#
# INVOCATION:
# object loadStreamFile
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Write the current set of custom streams to a file.
#
# DESCRIPTION:
# - If necessary, create the file selection widget.
# - Display the file selection widget and wait for a file to be selected.
# - Open the file and write the contents of the listbox.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::saveStreamFile {
} {
    #
    # If necessary, create a file selection widget.
    #

    if { ! [ info exists itk_component(fileSelect) ] ||			\
	    ! [ winfo exists $itk_component(fileSelect) ] } {
	itk_component add fileSelect {
	    FileSelect $itk_interior.fs -full 0
	}
    }


    #
    # Wait for a file to be selected.
    #

    if { [ $itk_component(fileSelect) activate ] } {
	#
	# Get the current set of custom streams.
	#

	set streams [ $itk_component(streamList) get 0 end ]


	#
	# Open the file.
	#

	set fileId [ open [ $itk_component(fileSelect) get ] w ]


	#
	# Write the stream name to the file.
	#

	foreach stream $streams {
	    puts $fileId $stream
	}

	close $fileId
    } 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsCustomStreams::streamList
#
# INVOCATION:
# set list [ object streamList ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Return the current list of streams from the custom streams list
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsCustomStreams::streamList {
} {
    return $dcsCustomStreams
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) parent The parent object
# (>) args Arguments to pass to itk_initialize
#
# PROCEDURE VALUE:
# ([C type]) description of the return value.
#
# PURPOSE:
# Initialize a CDhsImageArithmetic object
#
# DESCRIPTION:
# - Set the data member values.
# - Add the interface elements to the image arithmatic popup.
# - Hide the window.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::constructor {
    parent
    args 
} {
    set iaDatasetName {}
    set iaFrameId {Main HDU}
    set iaOperator {none}
    set iaHeaderFName {}
    set iaDataFName {}


    util::TopLevelWidget::constructor -center 0 -standalone 0 		\
	    -title "Image Arithmetic Setup"

    eval itk_initialize $args

    bind $itk_component(hull) <Button-3> "_qltHelp openUrl qltImageArith.html"

    #
    # Add the dataset name entry box.
    #

    frame $itk_interior.fullFrameId
    pack $itk_interior.fullFrameId -side top -expand yes -fill x
    itk_component add datasetName {
    	Entryfield $itk_interior.datasetName -width 30 			\
	-labeltext "Operand dataset" -labelpos n			\
    }
    $parent add_short_help $itk_component(datasetName)			\
	    {The operand dataset name.}
    pack $itk_component(datasetName) -side left -expand yes -fill x	\
    		-padx 10 -pady 10 -in $itk_interior.fullFrameId


    #
    # Add the frame id entry box.
    #

    itk_component add frameId {
    	Entryfield $itk_interior.frameId -width 10 			\
	-labeltext "Operand frame" -labelpos n				\
    }
    $parent add_short_help $itk_component(frameId)			\
	    {Frame id of the operand frame}
    pack $itk_component(frameId) -side left -expand yes -fill x	\
    		-padx 10 -pady 10 -in $itk_interior.fullFrameId


    #
    # Add a button box to control the widget.
    #

    buttonbox $itk_interior.bbox
    $itk_interior.bbox add ok -text "Ok" -command [ code $this setArith ]
    $itk_interior.bbox add cancel -text "Cancel" 			\
	    -command [ code wm withdraw $itk_interior ]
    $itk_interior.bbox add help -text "Help" 				\
	    -command "_qltHelp openUrl qltImageArith.html"
    pack $itk_interior.bbox -side bottom -expand yes -fill both

    $parent add_short_help [ $itk_interior.bbox component ok ]	\
	    {Set the selected image arithmetic and close window.}
    $parent add_short_help [ $itk_interior.bbox component cancel ]	\
	    {Revert to previous image arithmetic setup and close window.}
    $parent add_short_help [ $itk_interior.bbox component help ]	\
	{Display on-line help information for this window.}


    #
    # Add the operator selection button.
    #

    itk_component add operatorButton {
	optionmenu $itk_interior.operand -labeltext "Operator" 		\
		-labelpos n
    }
    eval $itk_component(operatorButton) insert end $iaOperatorList
    $parent add_short_help $itk_component(operatorButton)		\
	    {Select the operator for the image arithmetic.}
    pack $itk_component(operatorButton) -side bottom -expand yes 	\
		-fill x -padx 10 -pady 10

    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::display
#
# INVOCATION:
# object display
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This method is called to display the image arithmetic setup widget
#
# DESCRIPTION:
# - Copy the data from the data members to the interface elements.
# - display the image arithmetic window.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body			CDhsImageArithmetic::display {
} {
    #
    # Initialize the interface fields.
    #

    $itk_component(datasetName) clear
    $itk_component(datasetName) insert 0 $iaDatasetName
    $itk_component(frameId) clear
    $itk_component(frameId) insert 0 $iaFrameId
    $itk_component(operatorButton) select $iaOperator


    #
    # Activate the image arithmetic widget
    #

    wm deiconify $itk_interior
    raise $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::dsGetCallback
#
# INVOCATION:
# N.a. called by the OCSWish command monitor.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) monitor		- The name of the command monitor.
# (>) cmdStatus		- The command status.
# (>) info		- The command message string.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This set up to be executed for each response received when getting
# the operand dataset
#
# The method looks for errors, and de-activates data processing if the
# operand dataset couldn't be retrieved.
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::dsGetCallback {
    monitor
    cmdStatus
    info
} {
    if { $cmdStatus == "error" } {
	set iaOperator "none"
	bgerror "$info"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::getOperand
#
# INVOCATION:
# set files [ object getOperand ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# A list of two paths, the header and data file.
#
# PURPOSE:
# Return the files containing the operand for the image arithmetic.
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::getOperand {
} {
    if { $iaHeaderFName == {} } {
	setDataset
    }
    return "$iaHeaderFName $iaDataFName" 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::getOperator
#
# INVOCATION:
# set operator [ object getOperator ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# The operator to selected
#
# PURPOSE:
# Return the curretly selected operator
#
# The operator will be one of "" (no operation), "+=" (addition), "-=", 
# "/=" (division), "*=" (multiplication), or "^=" bad pixel mask.
#
# DESCRIPTION:
# Convert the operation into a sybolic string.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsImageArithmetic::getOperator {
} {
    case $iaOperator in							\
	none {
	    return ""
	}								\
	add {
	    return "+="
	}								\
	subtract {
	    return "-="
	}								\
	multiply {
	    return "*="
	}								\
	divide {
	    return "/="
	}								\
	mask {
	    return "^="
	}								\
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::setArith
#
# INVOCATION:
# object setArith
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Copies the currently displayed information from the interface widget
# and sets up data processing.
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::setArith {
} {
    set iaOperator [ $itk_component(operatorButton) get ]

    if { $iaDatasetName != [ $itk_component(datasetName) get ] ||	\
	    $iaFrameId != [ $itk_component(frameId) get ] } {
	setDatasetName [ $itk_component(datasetName) get ]
	setFrameId [ $itk_component(frameId) get ]

	setDataset
    }


    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::setDatasetName
#
# INVOCATION:
# object setDatasetName $datasetName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) datasetName	The name of the operand dataset.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the dataset name and frame id of the calibration dataset, and
# arange for it to be retrieved from the data server.
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::setDatasetName {
    datasetName
} {
    catch { file delete $iaHeaderFName }
    catch { file delete $iaDataFName }
    set iaHeaderFName {}
    set iaDataFName {}

    set iaDatasetName $datasetName
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::setOperand
#
# INVOCATION:
# object setOperand
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) headerFName	File containing the FITS header.
# (>) dataFName		File containing the data.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the operand data files
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::setOperand {
    headerFName
    dataFName
} {
    if { $iaHeaderFName != {} } {
	catch { file delete $iaHeaderFName }
    }
    if { $iaDataFName != {} } {
	catch { file delete $iaDataFName }
    }

    set iaHeaderFName $headerFName
    set iaDataFName $dataFName
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsImageArithmetic::setOperator
#
# INVOCATION:
# object setOperator $operator
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) operator 		The new operator.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the operator
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsImageArithmetic::setOperator {
    operator
} {
    if { [ lsearch $iaOperatorList $operator ] != -1 } {
	set iaOperator $operator
    } else {
    	error "Operator must be one of {$iaOperatorList}, not $operator."
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args Arguments to pass to itk_initialize
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Constructor for the CDhsQltCtl class
#
# DESCRIPTION:
# Create each of the components of the control widget.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsQltCtl::constructor { 
    args 
} {
    global _qltHelpDir

    util::TopLevelWidget::constructor -center 1 -standalone 0			\
	    -title "Quick Look Tool Control Panel"


    $itk_interior make_short_help
    $itk_interior add_menubar


    #
    # Set up the on-line help information
    #

    uplevel #0 "cHelp _qltHelp $_qltHelpDir/dhsQltCtl.html"
    _qltHelp setHtmlDir $_qltHelpDir
    bind $itk_component(hull) <Button-3> "_qltHelp openUrl dhsQltCtl.html"


    #
    # Add the popup get the image arithmetic setup
    #

    itk_component add imageArithmetic {
	CDhsImageArithmetic $itk_interior.imageArithmetic $this
    }


    #
    # Add a dialogMessage popup, for general use.
    #

    itk_component add dialogMessage {
	messagedialog $itk_interior.d -modality application
    }
    $itk_component(dialogMessage) hide Cancel
    $itk_component(dialogMessage) buttonconfigure OK -text "Continue"


    #
    # Create the streamSelect widget.
    #

    itk_component add streamWidget {
	CDhsStreamSelect $itk_interior.streamWidget $this
    }


    #
    # Create the customStream widget.
    #

    itk_component add customStreamWidget {
	CDhsCustomStreams $itk_interior.customStreamWidget $this
    }


    #
    # Create the configuration widget
    #

    itk_component add configureWidget {
	CDhsConfigure $itk_interior.configureWidget 
    }


    #
    # Set up the menu bar.
    #

    #
    #  Add a dataset menu.
    #

    set datasetMenu [ $itk_interior add_menubutton "Dataset" ]
    $this component dataset configure -underline 0

    add_short_help $itk_interior.menubar.dataset 			\
	    {Dataset menu: AutoCut, Display Newest, AutoSkip, Save Display, Display Dataset}

    add_menuitem $datasetMenu checkbutton "AutoCut" 			\
	    {Automatically execute autocut on display update}		\
	    -command { [ [ cQlServer::getImageName ] 	\
			 get_image ] autocut }				\
	    -underline 4						\
	    -variable ::cQlServer::autoCut 

    add_menuitem $datasetMenu checkbutton "Display Newest"		\
	    {Automatically replace displayed frame with newest frame}	\
	    -underline 8						\
	    -variable ::cQlServer::displayLast

    add_menuitem $datasetMenu checkbutton "AutoSkip" 			\
	    {Automatically skip updates when falling behind}		\
	    -underline 0						\
	    -variable ::cQlServer::autoSkip 

    add_menuitem $datasetMenu command "Save Display" 			\
	    {Save the Gemini permanent store} 				\
	    -command [code $itk_interior busy "$this displaySave" ]	\
	    -underline 0
	    
    add_menuitem $datasetMenu command "Display Dataset..."		\
	    {Display a specific dataset} 				\
	    -command [code $itk_interior busy "$this datasetDisplay" ]	\
	    -underline 8

    add_menuitem $datasetMenu command "Image Arithmetic..."		\
	    {Set up image arithmetic} 					\
	    -command [code $itk_interior busy 				\
	    "$itk_component(imageArithmetic) display" ] -underline 0


    #
    #  Add a stream menu
    #

    set streamMenu [ $itk_interior add_menubutton "Stream" ]
    $this component stream configure -underline 0
    add_short_help $itk_interior.menubar.stream 			\
	    {Stream menu: Subscribe Streams, Custom Streams}

    global errorInfo errorCode
    add_menuitem $streamMenu command "Subscribe Streams..."		\
 	    {Subscribe to streams known to the DHS} -command		\
 	    [code $itk_interior busy "$itk_component(streamWidget) display" ] \
	    -underline 0

    add_menuitem $streamMenu command "Custom Streams..."		\
 	    {Subscribe to a customized list of streams} -command	\
 	    [code $itk_interior busy "$itk_component(customStreamWidget) \
 	    display" ]							\
	    -underline 0


    #
    #  Add a configuration menu
    #

    set configMenu [ $itk_interior add_menubutton "Configure" ]
    $this component configure configure -underline 0
    add_short_help $itk_interior.menubar.configure 			\
	    {Configure menu: Load Configuration, Save Configuration, Change Configuration}

    add_menuitem $configMenu command "Load Configuration..."		\
	    {Load configuration data from a file} -command		\
	    [code $itk_interior busy "$itk_component(configureWidget) load" ]\
	    -underline 0

    add_menuitem $configMenu command "Save Configuration..."		\
	    {Save configuration data to a file} -command		\
	    [code $itk_interior busy "$itk_component(configureWidget) save" ]\
	    -underline 0

    add_menuitem $configMenu command "Change Configuration..."		\
	    {Change configuration parameters} -command			\
	    [code $itk_interior busy "$itk_component(configureWidget) change" ]\
	    -underline 0

    #
    #  Add a help menu
    #

    set helpMenu [ $itk_interior add_menubutton "Help" right ]
    $this component help configure -underline 0
    add_short_help $itk_interior.menubar.help 				\
	    {Help menu: Show information about this pannel.}

    add_menuitem $helpMenu command "About QLT..."		        \
	{Display a window with the Quick Look Tool version information.} \
	-command "displayAbout"						\
	-underline 0

    add_menuitem $helpMenu command "Help..."			        \
	{Display help information about the control panel in Netscape.}	\
	-command "_qltHelp openUrl dhsQltCtl.html"			\
	-underline 0


    #
    # Create the two vertical stacks for the lists of streams and the list
    # of frames.
    #

    panedwindow $itk_interior.pw -orient vertical -width 600 -height 300
    $itk_interior.pw add streams
    $itk_interior.pw add frames
    pack $itk_interior.pw -expand yes -fill both


    #
    # Create the selected streams list box.
    #

    itk_component add streamList { 
	scrolledlistbox [ $itk_interior.pw childsite 0 ].sslb 		\
	    -labeltext "Streams Selected" -hscrollmode dynamic 		\
	    -vscrollmode dynamic
    } 
    add_short_help $itk_component(streamList) 				\
	    "List of selected streams."
    pack $itk_component(streamList) -expand yes -fill both


    #
    # Create the right hand colunmn containing the list of frames.
    #

    set frameList [ $itk_interior.pw childsite 1 ]


    #
    # Add the frame listbox.
    #

    itk_component add frameList { 
	scrolledlistbox $frameList.frameList -labeltext "Available Frames" \
		-hscrollmode dynamic -vscrollmode dynamic 		\
		-selectioncommand [ code $this selectFrame ]
    }
    add_short_help $itk_component(frameList) 				\
	    "List of available frames. {bitmap b1} = display frame."
    pack $itk_component(frameList) -expand yes -fill both
    


    #
    # Set up the callback function to be executed whenever the list of
    # frames changes.
    #

    cQltFrame::traceFrame [ code $this setFrameList ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::datasetDisplay
#
# INVOCATION:
# object datasetDisplay
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Prompt for the name of a dataset, and arrage for the dataset to be displayed
#
# DESCRIPTION:
# - Prompt for the name of a dataset.
# - Send a request to the quick look server to send the dataset.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsQltCtl::datasetDisplay {
} {
    #
    #  Prompt for the dataset name.
    #
    set prompt $itk_interior.promptdialog
    if {! [winfo exists $prompt]} {
	::iwidgets::Promptdialog $prompt \
	    -modality application \
	    -labeltext "Enter the dataset to display"
    }
    $prompt hide Help
    $prompt hide Apply
    focus [$prompt component prompt component entry]
    $prompt center
    $prompt delete 0 end

    if { [ $prompt activate ] != "" } {
	#
	# Get the dataset name.
	#

	set dsName [ $prompt get ]
	if { $dsName != "" } {
	    #
	    #  Send a command to the quick look server to display the dataset.
	    #

	    cQlServer::datasetGet $dsName
	} else {
	    $itk_component(dialogMessage) configure -text "No dataset entered"
	    $itk_component(dialogMessage) center
	    $itk_component(dialogMessage) activate
	}
    } 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::displaySave
#
# INVOCATION:
# object displaySave
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Save the currently displayed image
#
# DESCRIPTION:
# - Get the displayed dataset name.
# - Use the rtdimage dump method to save the displayed image to a FITS file.
# - Send the FITS file to the Gemini permanent store with the dhsPut program.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# An image must be displayed.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsQltCtl::displaySave {
} {
    global		tmpDir
    global		uniqueName


    #
    # Figure out the dataset name.
    #

    set dsName [ cQlServer::displayDs ]

    if { $dsName == {} } {
	$itk_component(dialogMessage) configure -text "No dataset displayed"
	$itk_component(dialogMessage) center
	$itk_component(dialogMessage) activate
    } else {
	#
	# Find the rtdimage object and execute the dump method.
	#

	[ [ cQlServer::getImageName ] get_image ]	\
		dump $tmpDir/$dsName-QL.fits
	

	#
	# Send the file to the data server.
	#

	if { [ catch { puts [ exec dhsPut				\
		-d [CDhsConfigure::get dataServerName ]			\
		-ip [CDhsConfigure::get dataServerAddr ]		\
		-t FITS_UK -l PERM -n $dsName-QL -id dhsPut-$uniqueName \
		-f $tmpDir/$dsName-QL.fits ] } msg ] != 0 } { 
	    exec rm $tmpDir/$dsName-QL.fits
	    bgerror $msg
	} else {
	    #
	    # Delete the temporary file.
	    #

	    exec rm $tmpDir/$dsName-QL.fits
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::initialize
#
# INVOCATION:
# object initialize
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Initialize the items which can't be done in the constructor.
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsQltCtl::initialize {
} {
    #
    # Initialize the contents of the selected streams listbox to the currently
    # selected streams.
    #

    streamSet 
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::selectFrame
#
# INVOCATION:
# n.a. called by the scrolledlistbox class when an item is selected
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This method is called whenever a frame is selected
#
# DESCRIPTION:
# - Get the index of the selected frame.
# - Get the object id of the seleted frame
# - Display the selected frame.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsQltCtl::selectFrame {
} {
    #
    # Get the index of the selected frame in the listbox.
    #

    set selection [ $itk_component(frameList) curselection ]

    if { $selection != {} } {
	#
	# Get the object names of the current list of frames.
	#

	set frameList [ cQltFrame::frameList ]


	#
	# Get the object name of the selected frame. This assumes that the
	# frames in the listbox is the same as the frames returned by the
	# cQltFrame::frameList procedure, and are in the same order.
	#

	set frame [ lindex $frameList $selection ]


	#
	# Display the selected frame.
	#

	::cQltFrame::$frame updateRegion
	cQlServer::frameDisplay ::cQltFrame::$frame
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::setFrameList
#
# INVOCATION:
# ctl setFrameList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Read the list of frames from the cQltFrame class and put them into
# the list of frames
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsQltCtl::setFrameList {
} {
    $itk_component(frameList) clear
    #$itk_component(frameList) configure -items [ cQltFrame::allFrames ]
    eval $itk_component(frameList) insert end  [cQltFrame::allFrames]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsQltCtl::streamSet
#
# INVOCATION:
# object streamSet
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Put the specified list of streams into the stream list
#
# DESCRIPTION:
# - Concatenate the list of selected Quick Look Server streams, with the
#   list of custom streams.
# - Send a command to the Quick Look Server to subscribe to the combined
#   list of streams.
# - Set the list of subscribed streams in the subscribed streams listbox.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsQltCtl::streamSet {
} {
    #
    # Create a combined list of subscribed Quick Look Server streams, and
    # custom streams.
    #

    set streamList [ concat [ $itk_component(streamWidget) streamList ] \
		[ $itk_component(customStreamWidget) streamList ] ]

    #
    # Send a command to the QLS subscribing to the list of streams.
    #

    cQlServer::streamSet $streamList


    #
    # Set the contents of the subscribed streams listbox.
    #
    $itk_component(streamList) clear
    eval $itk_component(streamList) insert end $streamList
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStreamSelect::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) args Arguments to pass to itk_initialize
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Constructor for the CDhsStreamSelect class
#
# DESCRIPTION:
# Create each of the components of the stream select widget.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsStreamSelect::constructor { 
    parent
    args 
} {
    util::TopLevelWidget::constructor -center 0 -standalone 0			\
	    -title "Stream Select"

    eval itk_initialize $args 

    set dssParent $parent
    bind $itk_component(hull) <Button-3> "_qltHelp openUrl qltStreamSelect.html"

    #
    # Create the double list for the list of selected and not selected streams.
    #

    DoubleList $itk_interior.dlist -lefttitle "Streams not selected"	\
	    -righttitle "Streams selected" -selectmode extended
    pack $itk_interior.dlist -expand yes -fill both -pady 10 -padx 10


    #
    # Create a button box to control the widget.
    #

    buttonbox $itk_interior.bbox
    $itk_interior.bbox add ok -text "Ok" -command [ code $this selectStreams ]
    $itk_interior.bbox add cancel -text "Cancel" 			\
	    -command [ code wm withdraw $itk_interior ]
    $itk_interior.bbox add help -text "Help" 				\
	    -command "_qltHelp openUrl qltStreamSelect.html"
    pack $itk_interior.bbox -side bottom -expand yes -fill both

    $dssParent add_short_help [ $itk_interior.bbox component ok ]	\
	    {Set the selected streams and close window.}
    $dssParent add_short_help [ $itk_interior.bbox component cancel ]	\
	    {Revert to previous slected streams and close window.}
    $dssParent add_short_help [ $itk_interior.bbox component help ]	\
	{Display on-line help information for this window.}


    #
    # Hide the widget.
    #

    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStreamSelect::display
#
# INVOCATION:
# object.display
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Activate the select stream widget.
#
# DESCRIPTION:
# - Display the select stream widget.
# - Get a list of streams from the Quick Look Server.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDhsStreamSelect::populate
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsStreamSelect::display {
} {
    #
    # Activate the select stream widget.
    #

    wm deiconify $itk_interior
    raise $itk_interior


    #
    # Clear the listboxes in the double list.
    #

    $itk_interior.dlist.right clear
    $itk_interior.dlist.left clear


    #
    # Send a command to the Quick Look Server to get a new list of streams.
    # The listboxs will be populated when the response to the command is
    # received.
    #

    cQlServer::queryStreams $this
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStreamSelect::populate
#
# INVOCATION:
# n.a. Called by the OCSWish command receiver monitor
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) monitorName - Name of the command monitor
# (>) cmdStatus - Current command status
# (>) info - String returned with the command status.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# The method is executed when a command to retrieve a list of streams 
# completes
#
# DESCRIPTION:
# - Extract the list of streams from the command response attribute.
# - ForEach stream:
#   - If the stream is already selected, insert it into the right column 
#     of the stream select double list.
#   - if the stream is not selected, insert it into the left column of
#     the stream select double list.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsStreamSelect::populate {
    monitorName
    cmdStatus
    info
} {
    #
    # Ignore any responses except the final one.
    #

    if { $cmdStatus == "idle" } {
	#
	# Get the list of streams from the command.
	#

	set streamList [ cm $monitorName get streams value ]


	#
	# Sort the list of streams. The sortkey procedure is used to
	# produce a key which will ensure a correct alpha numeric sort 
	# of the stream names.
	#

	foreach stream $streamList {
	    lappend x [ sortKey $stream ] $stream
	    lappend newList $x
	    unset x
	}
	set newList [ lsort $newList ]
	unset streamList


	#
	# Copy the stream names back into streamList, in the order produced
	# by the sortKey value.
	#

	foreach stream $newList {
	    lappend streamList [ lindex $stream 1 ]
	}

	foreach stream $streamList {
	    if { [ lsearch -exact $dssSelectedStreams $stream ] == -1 } {
		#
		# The stream isn't in the current selection list, so insert
		# it into the left listbox.
		#

		$itk_interior.dlist.left append $stream
	    } else {
		#
		# The stream is in the current selection list, so insert
		# it into the right listbox.
		#

		$itk_interior.dlist.right append $stream
	    }
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStreamSelect::selectStreams
#
# INVOCATION:
# object selectStreams
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This method is executed when the ok button on the stream select widget
# is pressed
#
# DESCRIPTION:
# Set the 
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDhsStreamSelect::selectStreams {
} {
    #
    # update the list of selected streams.
    #

    set dssSelectedStreams [ $itk_interior.dlist.right get_contents ]


    #
    # Notify the parent CDhsQltCtl object that the list of streams has
    # changed.
    #

    $dssParent streamSet


    #
    # Hide the stream select widget.
    #

    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStreamSelect::streamList
#
# INVOCATION:
# set list [ object streamList ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Return the current list of streams from the predifined stream list
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsStreamSelect::streamList {
} {
    return $dssSelectedStreams
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsStreamSelect::unSubscribeAll
#
# INVOCATION:
# object unSubscribeAll
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Unsubscribe from all Quick Look Streams
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
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		CDhsStreamSelect::unSubscribeAll {
} {
    set dssSelectedStreams {}
}
