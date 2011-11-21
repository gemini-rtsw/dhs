# $Id: dhsOcsWish.tcl,v 1.4 2004-10-12 08:55:12 brighton Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1997				(c) 1997
# National Research Council		Conseil national de recherches
# Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
# All rights reserved			Tous droits reserves
# 					
# NRC disclaims any warranties,	Le CNRC denie toute garantie
# expressed, implied, or statu-	enoncee, implicite ou legale,
# tory, of any kind with respect	de quelque nature que se soit,
# to the software, including		concernant le logiciel, y com-
# without limitation any war-		pris sans restriction toute
# ranty of merchantability or		garantie de valeur marchande
# fitness for a particular pur-	ou de pertinence pour un usage
# pose.  NRC shall not be liable	particulier.  Le CNRC ne
# in any event for any damages,	pourra en aucun cas etre tenu
# whether direct or indirect,		responsable de tout dommage,
# special or general, consequen-	direct ou indirect, particul-
# tial or incidental, arising		ier ou general, accessoire ou
# from the use of the software.	fortuit, resultant de l'utili-
# 					sation du logiciel.
#
#***********************************************************************
#
# FILENAME
# dhsQlTool/src/dhsOcsWish.tcl
#
# PURPOSE:
# Contains the interface to the OCSWish command acceptors for the DHS
# quick look tool.
#
# CLASS NAME(S)
# cQlServer - The interface to the DHS QL server.
#
# METHOD NAME(S)
# cQlServer::checkDisplay - Attempt to display a new frame.
# cQlServer::datasetGet - Set a request to the Quick Look Server
#		for a specific dataset to be retrieved.
# cQlServer::displayNotify - Cause a frame to be displayed.
# cQlServer::dsGetCallback - Called when a response to the qlsDsGet 
#		command is received.
# cQlServer::frameDisplay - Display the specified frame.
# cQlServer::initialize - Initialize the static data in the cQlServer class.
# cQlServer::pingCmd - Callback function executed when a ping command is 
#		received.
# cQlServer::qltNotifyCmd - Callback function executed when a qltNotify 
#		command is received.
# cQlServer::queryStreams - Sends a qlsQueryStreams command to the QLS.
# cQlServer::streamSet - Set the list of streams this tool is subscribed to.
# cQlServer::testCmd - Callback function for when a ping command is received.
#
# $Log: not supported by cvs2svn $
# Revision 1.3  2004/08/24 13:57:13  brighton
# Linux port (see PORTING_NOTES, especially the part at the end labeled "Other Changes")
#
# Revision 1.2  2004/08/13 20:18:54  brighton
# Linux test/port
#
# Revision 1.1.1.1  2002/11/24 20:29:40  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.15  2001/04/04 17:55:18  nhill
# Changed to automatically update the cut and pixel table windows.
#
# Revision 1.14  2001/03/27 21:16:08  nhill
# Changed to only set the command to idle after the image is displayed. This
# prevents the QlServer from deleting the file before the QLT is done with it.
# Changed to set the displayed from to blank if the update command fails.
#
# Revision 1.13  1999/02/05 18:44:14  nhill
# Documentation typo fixes.
#
# Revision 1.12  1999/01/29 17:53:08  nhill
# Fixed a bug where it was trying to retrieve a image arithmetic operand
# dataset even when image arithmetic was disabled.
#
# Revision 1.11  1999/01/25 20:25:25  nhill
# Added a workaround to the OCSWish habit of remembering old values.
#
# Revision 1.10  1999/01/21 00:28:40  nhill
# Many changes to allow arithmetic on images.
#
# Revision 1.9  1998/10/08 19:39:17  nhill
# Changed to allow more configuration information to be saved.
#
# Revision 1.8  1998/06/16 22:17:00  nhill
# Changed to display the correct frame when frames are skipped.
#
# Revision 1.7  1998/05/05 17:08:58  nhill
# Added separate image a data files.
# Added ping and test commands.
#
# Revision 1.6  1997/12/10 21:55:36  nhill
# Added automatic skipping of frames.
#
# Revision 1.5  1997/11/30 21:36:18  nhill
# Added update idletasks to the processing of notify commands.
#
# Revision 1.4  1997/11/13 00:19:33  nhill
# Bug fix.
#
# Revision 1.3  1997/10/24 22:20:36  nhill
# Beta Check in.
#
# Revision 1.2  1997/10/01 17:18:08  nhill
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
# cQlServer
#
# PUBLIC METHODS: 
# autoCut	- Set the autoCut value.
# autoSkip	- Set the autoSkip value.
# checkDisplay	- Attempt to display a new frame.
# datasetGet	- Send a request to the Quick Look Server to display a 
#		  specific dataset.
# displayDs	- Return the name of the displayed dataset.
# displayLast	- Set the display last variable.
# dsGetCallback	- Called when a response to the qlsDsGet command is
#		  received.
# frameDisplay	- Display the specified frame object.
# getAutoCut	- Get the value of the autoCut variable.
# getAutoSkip	- Get the value of the autoSkip variable.
# getDisplayLast- Get the value of the displayLast variable.
# getImageName	- Get the value of the imageName variable.
# getServername	- Get the value of the cQlServerName variable.
# imageArithName- Get the value of the qlsImageArithName variable.
# initialize	- Intialize the connection to the dhs QL server.
# pingCmd	- Execute a ping command.
# qltNotifyCmd	- Callback function executed when a qltNotify command
#		  is received.
# queryStreams	- Execute a qlsQueryStreams command.
# serverName	- Set the server name variable.
# streamSet	- Execute a qlsStreamSet command.
# testCmd	- Execute a test command.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# displayLast	- Set to true (1) if the most recent dataset should be
#		  displayed as they arrive.
# autoCut	- Set to true (1) if autocut should be run every time
#		  the display is updated.
# autoSkip	- Set to true (1) if images should be automatically skipped
#		  when the display falls behind.
#
# PURPOSE:
# The interface to the DHS QL server.
#
# Note that all methods and data in this class are static.
#
#-
#***********************************************************************
#

itcl::class cQlServer {
    #
    # Public methods and data
    #

    proc		autoCut { value } { set autoCut $value }
    proc		autoSkip { value } { set autoSkip $value }
    proc		checkDisplay {}
    proc		datasetGet { datasetName 		\
			    { callback cQlServer::dsGetCallback } }
    proc		displayDs {} { return $qlsDisplayDs }
    proc		displayLast { value } { set displayLast $value }
    proc		dsGetCallback { monitorName cmdStatus info }
    proc		frameDisplay { frame }
    proc		getAutoCut {} { return $autoCut }
    proc		getAutoSkip {} { return $autoSkip }
    proc		getDisplayLast {} { return $displayLast }
    proc		getImageName {} { return $imageName }
    proc		getServerName {} { return $cQlServerName }
    proc		imageArithName {} { return $qlsImageArithName }
    proc		initialize {}
    proc		pingCmd { cmdRunner directive }
    proc		qltNotifyCmd { cmdRunner directive }
    proc		queryStreams { streamListBox }
    proc		serverName { value } { set qlsServerName $value }
    proc		streamSet { streams }
    proc		testCmd { cmdRunner directive }
    
    proc        setFrameUpdateCallback {callback} { set qlsFrameUpdateCallback $callback }
    proc        getFrameUpdateCallback {} { return $qlsFrameUpdateCallback }

    public common	displayLast 1
    public common	autoCut 1
    public common	autoSkip 1
    public common	imageName ".cdhsqltool1.image"
    public common	cQlServerName ".cdhsqltool1"
    public common	qlsImageArithName ".dqtCtl.imageArithmetic"


    #
    # Private methods and data
    #

    private proc	displayNotify { datastName frameId headerFName 
				dataFName lastFrame queueEmpty regions }

    private common	qlsDestName ""	
    private common	qlsDisplayDs ""
    private common	qlsLastDs ""
    private common	qlsDsGet
    private common	qlsQueryStreams
    private common	qlsNeedsDisplay "false"
    private common	qlsDisplayFrame ""
    private common	qlsServerName ""
    private common	qlsNextFrame ""
    private common	qlsStartCount 0
    private common  qlsFrameUpdateCallback ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::checkDisplay
#
# INVOCATION:
# checkDisplay
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Called to display a frame.
#
# DESCRIPTION:
# - If the calibration frame is required but not available, use after to
#   re-execute this command after a delay, and then return.
# - If the frame is already displayed:
#     - update the data processing on the image (if necessary).
#     - Update the displayed image.
#   autocut.
# - If the frame is not displayed:
#     - Set up the data processing for the frame.
#     - Perform the initial processing for the frame.
#     - display the frame file 
# - if necessary, perform an autocut.
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

body	cQlServer::checkDisplay {
} {
    global tmpDir errorInfo


    if { [ $qlsImageArithName getOperator ] != {} && 
	    ! [ $qlsImageArithName ready ] } {
	after 500 cQlServer::checkDisplay
	return
    }


    set frame $qlsNextFrame
    set newHeaderFile [ $frame cget -qfHeaderFName ]
    set newDataFile [ $frame cget -qfDataFName ]
    set frameId [ $frame cget -qfFrameId ]
    set datasetName [ $frame cget -qfDatasetName ]
    set image [ $imageName get_image ]

    if { $frame == $qlsDisplayFrame } {
	#
	# Update the arithmetic.
	#

	if { [ $qlsImageArithName getOperator ] != {} } {
	    $frame arithApply
	}


	#
	# Update the image.
	#

	$image mmap update


	#
	#  If the cut or pixel table windows are visible, update them.
	#

	if { [ winfo exists $imageName.spectrum ] } {
		$imageName.spectrum notify_cmd
	}
	if { [ winfo exists $imageName.pixtable ] } {
	    set name $imageName.imagef.canvas
	    set x  [ expr { [ winfo pointerx $name ] - [ winfo rootx $name ]}]
	    set y  [ expr { [ winfo pointery $name ] - [ winfo rooty $name ]}]
	    event generate $name <Motion> -when tail -x $x -y $y
	}
    } else {
	#
	# Set up the arithmetic
	#

	dpClear
	if { [ $qlsImageArithName getOperator ] != {} } {
	    set l {}
	    lappend l $tmpDir [ $qlsImageArithName getOperator ]	\
		"$newHeaderFile $newDataFile" 				\
		[ $qlsImageArithName getOperand ]
	    if { [ catch { set files [ dpSet $tmpDir 			\
		    [ $qlsImageArithName getOperator ] 			\
		    "$newHeaderFile $newDataFile" 			\
		    [ $qlsImageArithName getOperand ] ] } msg ] != 0 } {
		$frame arithError true
		bgerror $msg
	    } else {
		#
		# Update the arithmetic.
		#

		$frame arithError false
		$frame arithApply

		set newHeaderFile [ lindex $files 0 ]
		set newDataFile [ lindex $files 1 ]
	    }
	} 


	#
	#  Display the file containing the frame data.
	#

	$image mmap set $newDataFile 0 0 $newHeaderFile 0 0


	#
	#  If the cut or pixel table windows are visible, update them.
	#

	if { [ winfo exists $imageName.spectrum ] } {
		$imageName.spectrum notify_cmd
	} 
	if { [ winfo exists $imageName.pixtable ] } {
	    set name $imageName.imagef.canvas
	    set x  [ expr { [ winfo pointerx $name ] - [ winfo rootx $name ]}]
	    set y  [ expr { [ winfo pointery $name ] - [ winfo rooty $name ]}]
	    event generate $name <Motion> -when tail -x $x -y $y
	}


	#
	# Only keep the last dataset and the displayed datset around.
	# if the previously displayed dataset isn't no longer either
	# of these, delete it.
	#

	if { [ string compare $qlsDisplayDs $datasetName ] != 0 && 	\
		[ string compare $qlsDisplayDs $qlsLastDs ] != 0 } {
	    cQltFrame::deleteDs $qlsDisplayDs
	}
	


	#
	#  Set the new displayed dataset and frame. This isn't done 
	#  immediately at startup, since sometimes the first mmaps
	#  fail, but without actually complaining. 
	#

	set qlsDisplayDs $datasetName
	if { [ expr { $qlsStartCount < 10 } ] } {
	    set qlsStartCount [ expr {$qlsStartCount + 1} ]
	} else {
	    set qlsDisplayFrame $frame
	}
    }


    #
    # If required, execute the autocut function.
    #

    if { $autoCut } {
	$image autocut
    }

    if { $qlsFrameUpdateCallback ne "" } {
        eval $qlsFrameUpdateCallback
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::datasetGet
#
# INVOCATION:
# cQlServer::datasetGet datasetName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) datasetName - The name of the dataset to display.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Send a request to the quick look server that a specific dataset be 
# displayed
#
# DESCRIPTION:
# Send a qlsDsGet command to the server.
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

body		cQlServer::datasetGet {
    datasetName
    { callback cQlServer::dsGetCallback }
} {
    cs $qlsDsGet set dataset value $datasetName
    cs $qlsDsGet postCallback $callback
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::displayNotify
#
# INVOCATION:
# cQlServer::displayNotify $datasetName $frameId $headerFName 		\
#		$dataFName $lastFrame $queueEmpty $regions
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) datasetName 	The name of the dataset to be displayed.
# (>) frameId		The id of the frame to be displayed.
# (>) headerFName	The name of the file containing the frame header.
# (>) dataFName		The name of the file containing the frame data.
# (>) lastFrame		Set to "true" if this is the the best dataset to
#			display for this frame.
# (>) queueEmpty	Set to "true" if there are no more notify commands
#			queued for this QLT.
# (>) regions		The regions that were updated. This should be a list
#			of lists, each sub-list consisting of four values: 
#			x start, x size, y start, and y size of the region.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This function causes a frame to be displayed
#
# DESCRIPTION:
# - Find a cQltFrame object for the frame.
# - Indicate that a region of the frame has been modified.
# - Figure out if the frame needs to be displayed, and if so display it
#   by calling cQlServer::frameDisplay.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# - cQlServer::qltNotifyCmd
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	cQlServer::displayNotify {
    datasetName
    frameId
    headerFName
    dataFName
    lastFrame
    queueEmpty
    regions
} {
    #
    # Find an old, or create a new frame object to represent the frame.
    #

    set frame [ cQltFrame::checkFrame $datasetName $frameId 		\
	    $headerFName $dataFName ]

    foreach region $regions {
	$frame updateRegion $region
    }


    #
    # If this is the last frame, and the user wants us to display the 
    # last frame, or if this is the displayed frame, then update the
    # display.
    #

    if { ( $lastFrame == "true" && $displayLast ) || 			\
	    $frame == $qlsDisplayFrame || $qlsDisplayFrame == {} } {
	set qlsNeedsDisplay "true"
    }

    if { ( $queueEmpty == "true" || ! $autoSkip ) && 
	    $qlsNeedsDisplay == "true" } {
	set qlsNeedsDisplay "false"

	cQlServer::frameDisplay $frame 
	update idletasks
    } 


    #
    # If this is the last frame, then update the last dataset name
    # associated with the last frame.
    #

    if { $lastFrame == "true" } {
	#
	# Only keep the last dataset and the displayed datset around.
	# If the previouse last dataset isn't no longer either of these, 
	# delete it.
	#

	if { [ string compare $qlsLastDs $qlsDisplayDs ] != 0 && 	\
		[ string compare $qlsLastDs $datasetName ] != 0 &&	\
		[ string compare $qlsLastDs "" ] != 0 } {
	    cQltFrame::deleteDs $qlsLastDs
	}
	set qlsLastDs $datasetName
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::dsGetCallback
#
# INVOCATION:
# n.a. called automatically by the command sender.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) monitorName - The command monitor.
# (>) cmdStatus - Current command status.
# (>) info - String returned with the command status.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Called when a response to the qlsDsGet command is received.
#
# DESCRIPTION:
# Just check for errors.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# - cQlServer::datasetGet
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body		cQlServer::dsGetCallback {
    monitorName
    cmdStatus
    info
} {
    #
    # If an error occurs, display the error information. 
    #

    if { $cmdStatus == "error" } {
	error "$info"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::frameDisplay
#
# INVOCATION:
# cQlServer::frameDisplay frame
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) frame - The cQltFrame object to display
# (>) updateAll - If true, image arithmatic will be done on the entire
#		  frame before re-display.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Use the checkDisplay method to display the frame.
#
# DESCRIPTION:
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

body		cQlServer::frameDisplay {
    frame
} {
    set qlsNextFrame $frame
    if { [ $qlsImageArithName getOperator ] != {} } {
	$qlsImageArithName getOperand
    }
    checkDisplay
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::initialize
#
# INVOCATION:
# cQlsServer::initialize
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Initialize the static data in the cQlServer class
#
# DESCRIPTION:
# - Set up the OCSWish DHS service.
# - Create a command acceptor for qltNotify commands.
# - Create a qlsQueryStreams command.
# - Create a qlsDsGet command.
# - Connect to the QLS and send a qlsClientStream command with the
#   initial subscription list.
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

body	cQlServer::initialize {
} {
    global	errorInfo errorCode
    global	uniqueName


    #
    # Set up to use the dhsService.
    #

    appData add appKind dhsQlc
    appData add appName $uniqueName
    service dhs


    #
    # Create a command acceptor for the test command. The command
    # callback proceedure is testCmd.
    #

    dhs ca "test"
    ca "test" registerCb cQlServer::testCmd


    #
    # Create a command acceptor for the ping command. The command
    # callback proceedure is pingCmd.
    #

    dhs ca "ping"
    ca "ping" registerCb cQlServer::pingCmd


    #
    # Create a command acceptor for the qltNotify command. The command
    # callback proceedure is qltNotifyCmd.
    #

    dhs ca "qltNotify"
    ca "qltNotify" registerCb cQlServer::qltNotifyCmd


    #
    # Send a qlsClientStream command to the Quick Look Server. This tests
    # the connection to the server and sets up the default quick look streams.
    #

    dhs cs "qlsClientStream" -command qlsClientStream
    cs qlsClientStream add streams
    cs qlsClientStream set streams mode in
    if { [ CDhsConfigure::get customStreams ] == {} } {
	cs qlsClientStream set streams value { "*" }
    } else {
	cs qlsClientStream set streams value 				\
		[ CDhsConfigure::get customStreams ]
    }

    
    #
    # Send a qlsClientStream command to the server. This also involves figuring
    # out what the name of the server is. The QLS should be called 
    # <hostname>:$qlsServerName.<hostid> if it is a slave server, or 
    # <hostname>:$qlsServerName it is a master server. Try to send a 
    # command to a slave QLS first, and if that fails, try sending to a 
    # master QLS.
    #

    # XXX allan: On linux, hostid has a "0x" prefix, which needs to be removed!
    regsub 0x [ exec hostid ] {} hostid

    set qlsDestName [exec hostname -s]:$qlsServerName.$hostid
    cs qlsClientStream set destName value $qlsDestName

    if { [ catch { cs qlsClientStream post} ] != 0 } {
	set qlsDestName [exec hostname -s]:$qlsServerName
	cs qlsClientStream set destName value $qlsDestName
	cs qlsClientStream post
    }


    #
    # Create a qlsQueryStreams command sender.
    #

    set qlsQueryStreams [ dhs cs qlsQueryStreams -command qlsQueryStreams ]
    cs $qlsQueryStreams add streams
    cs $qlsQueryStreams set streams mode out
    cs $qlsQueryStreams set destName value $qlsDestName


    #
    # Create a qlsDsGet command sender.
    #

    set qlsDsGet [ dhs cs qlsDsGet -command qlsDsGet ]
    cs $qlsDsGet add dataset
    cs $qlsDsGet set dataset mode in
    cs $qlsDsGet set destName value $qlsDestName
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::pingCmd
#
# INVOCATION:
# n.a. Called by the OCSWish command acceptor.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) cmdRunner The OCSWish command runner that accepted the command.
# (>) directive Descripton of the parameter
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Callback function for when a ping command is received.
#
# DESCRIPTION:
# return success.
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

body	cQlServer::pingCmd {
    cmdRunner
    directive
} {
    cr $cmdRunner update IDLE "finished"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::qltNotifyCmd
#
# INVOCATION:
# n.a. Called by the OCSWish command acceptor.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) cmdRunner The OCSWish command runner that accepted the command.
# (>) directive Descripton of the parameter
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Callback function for when a qltNotify command is received.
#
# DESCRIPTION:
# - Get the filename, frame id, dataset name, last frame flag, 
#   queueEmpty flag, and regions from the command parameters.
# - If the dataset is the one to be used for image arithmetic, check
#   to see if this is the required frame, and if it is, notify the
#   CDhsImageArithmetic class that the data is available.
# - If necessary update the displayed image.
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

body	cQlServer::qltNotifyCmd {
    cmdRunner
    directive
} {
    global errorInfo errorCode


    set image [ $imageName get_image ]

    #
    # This is done in a catch because OCS wish doesn't display errors
    # when the occur in a callback. The catch allows the errors to be 
    # displayed.
    #

    if { [ catch {
	#
	# Set the command status to busy.
	#

	cr $cmdRunner update BUSY "started"


	#
	# Extract the parameters from the command.
	#

	set headerFName [ cr $cmdRunner get headerFileName ]
	set dataFName [ cr $cmdRunner get dataFileName ]
	set frameId [ cr $cmdRunner get frameId ]
	set datasetName [ cr $cmdRunner get datasetName ]
	set queueEmpty [ cr $cmdRunner get queueEmpty ]
	set regions [ cr $cmdRunner get regions ]
	set value {}
	foreach element $regions {
	    lappend value {}
	}
	cr $cmdRunner set regions $value


	#
	# Extract the lastframe parameter, setting it to false by
	# default.
	#

	if { [ catch { set lastFrame  					\
		[ cr $cmdRunner get lastFrame ] } ] != 0 } {
	    cr $cmdRunner add lastFrame
	    set lastFrame "false"
	}


	#
	# Check to see if this dataset is the one to be used as the image
	# arithmetic operand. 
	#

	if { $datasetName == [ $qlsImageArithName getDatasetName ] } {
	    #
	    # This is the operand datset, see if is the correct frame.
	    #

	    if { $frameId == [ $qlsImageArithName getFrameId ] ||
		    [ string match "[$qlsImageArithName getFrameId]:*" $frameId ] } {
		#
		# This is the operand frame, tell the CDhsImageArithmetic
		# object about it.
		#

		$qlsImageArithName setOperand $headerFName $dataFName
	    } else {
		#
		# Right dataset, wrong frame, so delete the files.
		#

		catch { file delete $headerFName }
		catch { file delete $dataFName }
	    }
	} else {
	    #
	    # This wasn't the operand dataset, to attempt to display it.
	    #

	    displayNotify $datasetName $frameId $headerFName $dataFName \
		    $lastFrame $queueEmpty $regions
	}


	#
	# Reset the lastframe value to false so if it isn't set
	# when the next command is called, it will default to false.
	#

	cr $cmdRunner set lastFrame value "false"

	cr $cmdRunner update IDLE "finished"
    } msg ] != 0 } {
	#
	# An error occured, display an error message.
	#

	set qlsDisplayFrame ""
	set savedInfo $errorInfo
	bgerror "$msg $savedInfo"
	cr $cmdRunner update ERROR "Finished with error"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::queryStreams
#
# INVOCATION:
# cQlServer::queryStreams
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# streamListBox - The streamListBox object to be populated.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Send a qlsQueryStreams command to the QLS.
#
# DESCRIPTION:
# Post the command, executing the streamListBox populate method
# when it is complete.
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

body	cQlServer::queryStreams { 
    streamListBox
} {
    cs $qlsQueryStreams postCallback "$streamListBox populate"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::streamSet
#
# INVOCATION:
# cQlServer::streamSet { stream1 stream2 stream3 }
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) streams (string list) List of streams to subscribe to.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Set the list of streams a tool has subscribed to
#
# DESCRIPTION:
# - Add the streams to the qlsClientStream command.
# - Post the command
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

body		cQlServer::streamSet {
    streamList
} {
    #
    # Wait for any existing commands to complete.
    #

    catch { cm clientStreamsCmd wait done }

    
    #
    # Set the stream list parameter to the new list of streams.
    #

    cs qlsClientStream add streams
    cs qlsClientStream set streams mode in
    if { $streamList == "" } {
	cs qlsClientStream set streams value "*"
    } else {
	cs qlsClientStream set streams value $streamList
    }


    #
    # Set the destination name and post the command.
    #

    cs qlsClientStream set destName value $qlsDestName
    cs qlsClientStream post
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQlServer::testCmd
#
# INVOCATION:
# n.a. Called by the OCSWish command acceptor.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) cmdRunner The OCSWish command runner that accepted the command.
# (>) directive Descripton of the parameter
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Callback function for when a test command is received.
#
# DESCRIPTION:
# return success.
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

body	cQlServer::testCmd {
    cmdRunner
    directive
} {
    cr $cmdRunner update IDLE "finished"
}
