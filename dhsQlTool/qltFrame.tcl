# $Id: qltFrame.tcl,v 1.3 2004-08-24 13:57:13 brighton Exp $
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
# dhsQlTool/src/qltFrame.tcl
#
# PURPOSE:
# Contains the cQltFrame class
#
# CLASS NAME(S)
# cQltFrame	- An object of this type is created for each frame
#
# METHOD NAME(S)
# cQltFrame::constructor- Constructor for the cQltFrame class.
# cQltFrame::destructor	- Destructor for the cQltFrame class.
# cQltFrame::allFrames	- Return a list of the names of all known frames.
# cQltFrame::arithApply	- Apply any required arithmatic to the frame.
# cQltFrame::arithError	- Set the qfArithError data member to indicate that 
#			  the currently selected arithmatic is invalid.
# cQltFrame::checkFrame	- Check to see if a frame object already exists
#			  for the specified name. If it does not exist,
#			  a cQltFrame object is created.
# cQltFrame::deleteDs	- Delete all frames associated with a dataset.
# cQltFrame::frameInfo	- Return a string describing the frame.
# cQltFrame::framelist	- Return a sorted list of all frame objects.
# cQltFrame::traceFrame	- Set the qFrameListTrace data member to a command
#			  which should be executed whenever the list of 
#			  frames changes.
# cQltFrame::updateRegion
#			- Update the list of regions that have been updated 
#			  since the last time the data processing was done.
#
# $Log: not supported by cvs2svn $
# Revision 1.2  2004/08/13 20:18:54  brighton
# Linux test/port
#
# Revision 1.1.1.1  2002/11/24 20:29:53  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.11  1999/02/05 18:45:42  nhill
# Documentation typo fixes.
#
# Revision 1.10  1999/01/25 20:29:21  nhill
# Added a work around to determine the form of region read from the
# OCSWish command acceptor.
#
# Revision 1.9  1999/01/20 23:50:10  nhill
# Removed some debugging statements.
#
# Revision 1.8  1999/01/20 23:46:06  nhill
# Added methods arithApply, arithError, and updateRegion to cQltFrame.
#
# Revision 1.7  1998/05/05 17:10:02  nhill
# Changed to use separate header and data files.
#
# Revision 1.6  1997/12/10 21:56:19  nhill
# Changed the way files are deleted.
#
# Revision 1.5  1997/12/04 23:51:03  nhill
# Added error checking when the file is removed.
#
# Revision 1.4  1997/11/30 21:37:17  nhill
# Changed the way frame names are calculated.
#
# Revision 1.3  1997/11/13 00:19:33  nhill
# Bug fix.
#
# Revision 1.2  1997/10/24 22:21:37  nhill
# Beta check in.
#
# Revision 1.1  1997/10/01 17:19:11  nhill
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
# cQltFrame
#
# PUBLIC METHODS: 
# constructor	- Constructor for the cQltFrame class.
# destructor	- Destructor for the cQltFrame class.
# allFrames	- Return a list of the names of all known frames.
# arithApply	- Apply any required arithmatic to the frame.
# arithError	- Set the qfArithError data member to indicate that the 
#		  currently selected arithmatic is invalid.
# checkFrame	- Check to see if a frame object already exists for the 
#		  specified name. If it does not exist, a cQltFrame object 
#		  is created.
# deleteDs	- Delete all frames associated with a dataset.
# frameInfo	- Return a string describing the frame.
# frameList	- Return a sorted list of all frame objects.
# traceFrame	- Set the qFrameListTrace data member to a command which 
#		  should be executed whenever the list of frames changes.
# updateRegion	- Update the list of regions that have been updated since 
#		  the last time the data processing was done.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# An object of this class is created for each frame known to the Quick
# Look Tool
#
#-
#***********************************************************************
#

itcl::class cQltFrame {
    #
    # Public methods and data
    #

		constructor { datasetName frameId headerFName dataFName } {}
		destructor {}
    proc 	allFrames {}
    method	arithApply {}
    method	arithError { value } { set qfArithError $value }
    proc 	checkFrame { datasetName frameId headerFName dataFName }
    proc	deleteDs { dataset }
    method	frameInfo {}
    proc	frameList {}
    proc 	traceFrame { command }
    method	updateRegion { { region {} } }



    #
    # Private methods and data
    #

    private common	qfFrameListTrace ""
    public variable	qfFrameId ""
    public variable	qfDatasetName ""
    public variable	qfHeaderFName ""
    public variable	qfDataFName ""
    private variable	qfUpdateList {}
    private variable	qfArithError false
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) datasetName	- The dataset name.
# (>) frameId		- The frame id.
# (>) fileName		- The name of the file containing the data.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Initialize a new frame object
#
# DESCRIPTION:
# Save the input parameters in the object.
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

body		cQltFrame::constructor {
    datasetName
    frameId
    headerFName
    dataFName
} {
    set qfFrameId $frameId
    set qfDatasetName $datasetName
    set qfHeaderFName $headerFName
    set qfDataFName $dataFName
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::destructor
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
# Destructor for the cQltFrame class
#
# DESCRIPTION:
# Remove the file from the temporary directory.
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

body		cQltFrame::destructor {
} {
    catch { file delete $qfHeaderFName }
    catch { file delete $qfDataFName }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::allFrames
#
# INVOCATION:
# set frames [ cQltFrame::allFrames ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# A list of strings describing all known frames..
#
# PURPOSE:
# Return a list of strings describing all known frames
#
# The frames are in the order returned by the cQltFrame::frameList proceedure.
#
# DESCRIPTION:
# - Use the cQltFrame::frameList proceedure to get a list of frame objects.
# - For each from object, create a string describing the frame.
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

body		cQltFrame::allFrames {
} {
    set returnList {}

    #
    # Use the frameList proceedure to get a list of frame objects.
    #

    foreach frame [ frameList ] {
	#
	# Create a string describing the frame and append it to the return
	# list.
	#

	lappend returnList [ $frame frameInfo ]
    }


    return $returnList
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::arithApply
#
# INVOCATION:
# $frame arithApply
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This method does the image arithmetic on any regions of the frame which
# have been updated since the last time the image arithmetic was done
#
# The updateRegion method must be called as each region is updated to 
# allow this object to keep track of which regions have been modified.
#
# DESCRIPTION:
# Update the arithmetic for each region that was modified since the last
# update.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# - cQltFrame::updateRegion
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	cQltFrame::arithApply {
} {
    if { [ isFalse $qfArithError ] } {
	if { [ llength $qfUpdateList ] == 1 && 				\
		[ llength [ lindex $qfUpdateList 0 ] ] == 0 } {
	    dpApply
	} else {
	    foreach region $qfUpdateList {
		dpApply [lindex $region 0] [lindex $region 1]		\
			[lindex $region 2] [lindex $region 3]
	    }
	}
	set qfUpdateList {}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::checkFrame
#
# INVOCATION:
# cQltFrame::checkFrame datasetName frameId
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) datasetName - The name of the dataset.
# (>) frameId - The frame id.
# (>) filename - The name of the file containing the data.
#
# PROCEDURE VALUE:
# The frame object name
#
# PURPOSE:
# Check to see if a frame is already in the list of frames, and if not
# add the new frame.
#
# DESCRIPTION:
# - Get a name for the frame object based on the dataset name and frame id.
# - Search for an existing object for the frame.
# - If no existing object is found, create a new one.
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

body	cQltFrame::checkFrame {
    datasetName
    frameId
    headerFName
    dataFName
} {
    #
    # Figure out what the name of the object is or should be.
    #

    regsub -all " " ${datasetName}_${frameId} "-" tmp
    set objectName [ sortKey $tmp ]


    #
    # Look for an existing object for this frame.
    #

    set frame [ itcl_info objects $objectName -class cQltFrame ] 


    #
    # If no existing object was found, create a new one.
    #

    if { $frame == {} } {
	#
	# Create a new frame object.
	#

	set frame [ cQltFrame $objectName $datasetName $frameId 	\
		$headerFName $dataFName ]


	#
	# Update anything that depends on the list of frames.
	#

	if { $qfFrameListTrace != "" } {
	    $qfFrameListTrace
	}
    } 


    #
    # Return the object id of the frame object.
    #

    return ::cQltFrame::$frame
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::deleteDs
#
# INVOCATION:
# deleteDs datasetName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) datasetName - Name of the dataset being deleted.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Deletes all frames in dataset from the list of frames
#
# DESCRIPTION:
# Loop through every known frame, and if the dataset name of the frame
# matches the specified datset, remove it.
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

body		cQltFrame::deleteDs {
    datasetName
} {
    #
    # Loop through all known frames.
    #

    foreach frame [ itcl_info objects -class cQltFrame ] {
	#
	# If the dataset name of the frame matches the dataset being
	# deleted, delete the frame.
	#

	if { [ string compare [ $frame cget -qfDatasetName ] $datasetName ] == 0 } {
	    delete object $frame
	}
    }


    #
    # Execute the list trace function to update the list of frames.
    #

    if { $qfFrameListTrace != "" } {
	$qfFrameListTrace
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::frameInfo
#
# INVOCATION:
# frame frameInfo
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# Dataset name and frame id.
#
# PURPOSE:
# Return a string suitable for including in a display list of frames
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

body		cQltFrame::frameInfo {
} {
    return "$qfDatasetName:$qfFrameId"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::frameList
#
# INVOCATION:
# set frames [ cQltFrame::frameList ]
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# (list) A list of frame names.
#
# PURPOSE:
# Return a sorted list of frame names
#
# DESCRIPTION:
# Get a list of frame objects and sort it.
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

body		cQltFrame::frameList {
} {
    return [ lsort [ itcl_info objects -class cQltFrame ] ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::updateRegion
#
# INVOCATION:
# frame updateRegion 0 100 50 200
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) region	The region that was updated. This consists of four values,
#		the start of the region in X, the length of the region in X.
# 		the start of the region in Y and the length of the region in Y.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# This method updates the list of regions that have been updated since the
# last time the data processing was done
#
# DESCRIPTION:
# The region passed in is appended to qfUpdateList.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# cQltFrame::arithApply
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	cQltFrame::updateRegion {
    { region {} }
} {
    if { $region == {} } {
	set qfUpdateList { {} }
    } else {
	#
	#  All of this is to figure out what form the region takes as
	#  read from the OCS Wish command acceptor.
	#

	if { [ llength $qfUpdateList ] != 1 || 				\
		[ llength [ lindex $qfUpdateList 0 ] ] != 0 } {
	    if { [ llength $region ] == 1 } {
		if { [ llength [ lindex $region 0 ] ] == 4 } {
		    lappend qfUpdateList [ lindex $region 0 ]
		}
	    } else {
		lappend qfUpdateList $region
	    }
	}
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cQltFrame::traceFrame
#
# INVOCATION:
# cQltFrame::traceFrame procedure
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) command - Command to execute when the list of frames changes.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the qFrameListTrace data member to a command which should be executed
# whenever the list of frames changes
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

body		cQltFrame::traceFrame {
    command
} {
    set qfFrameListTrace $command
}
