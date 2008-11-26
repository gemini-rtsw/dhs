# $Id: dhsConsoleUtils.tcl,v 1.1 2004-08-30 09:15:32 brighton Exp $
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
# dhsConsole/src/utils.tcl
#
# PURPOSE:
# To provide a number of helper routines for the DHS console.
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# None.
#
# GLOBAL PROCEDURE(S)
# addMsg		Add a message to the message queue.
# bgerror		Backgroud error handler.
# createToolbar		Creates a toolbar.
# errorDialog		Displays an error message in a dialog box.
# extractListItem	Extracts a particular list item from a string.
# extractNums		Extracts all occurences of numbers from a string.
# getImages		Finds the desired images and creates an image
#			for each one that is found.
# getMsg		Gets and removes a message from the message queue.
# infoDialog		Displays an informative message in a dialog box.
# inputDialog		Display a dialogue that gathers input.
# isBoolean		Determines if some value is a boolean value.
# isFalse		Determines if some value is a "negative" value.
# isInteger		Determine if some value is an integer value.
# isNumber		Determines if some value is a numerical value.
# isTrue		Determines if some value is a "true" value.
# isZero		Determines if some value is zero.
# parseCommandLine	Parses command line arguments given to ocswish.
# processMsgs		Processes messgages in the msg queue.
# stopGoButtons		Displays a set of stop and go buttons
# warningDialog		Display a warning message in a dialog box.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/11/24 20:25:55  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.26  1999/06/17 21:03:34  jaeger
# Fixed the command-line parsing so it forces only one instance of a
# command-line argument.
#
# Revision 1.25  1999/06/08 21:36:42  jaeger
# Fixed regular expresion in isZero and isNumber.  The "." was being
# interpretted as any single character instead of ".", we needed it
# to be ".".
#
# Revision 1.24  1999/06/08 18:08:24  jaeger
# Removed selectionDialog procedure.  Added isInteger procedure.
# Cleaned up the code which supports the dhsMsgQueue.  Filled in comments
# where they were missing.  Made sure all procedures had a return command.
# Set the message width for all dialogues. isFalse and isTrue altered to
# allow numerical values padded with zeros.
#
# Revision 1.23  1999/05/12 19:19:12  jaeger
# Changed isFalse so numerical values that are not 0 are considered to
# be false values.
#
# Revision 1.22  1999/02/09 00:14:48  jaeger
# choice dialog gets deactivated rather than destroyed.
#
# Revision 1.21  1999/01/28 19:10:26  jaeger
# Added a message queue for error, warning and information messages so
# that no messages are lost and they are displayed in order.  Made all
# dialogues consistenly position and sized.
#
# Revision 1.20  1999/01/26 06:28:22  jaeger
# Fixed usage message.
#
# Revision 1.19  1999/01/25 21:12:24  jaeger
# Added default value to inputDialog
#
# Revision 1.18  1998/08/14 21:54:19  jaeger
# Brought code documentation up to Gemini standards.
#
# Revision 1.17  1998/07/20 19:39:19  jaeger
# moved bitmaps to "images" directory, code changed to reflect this.
#
# Revision 1.16  1998/07/16 20:05:11  jaeger
# Removed help button on choiceDialog.  Added short help information.
#
# Revision 1.15  1998/06/25 20:12:45  jaeger
# Added a couple update idletasks commands so things return when they should.
#
# Revision 1.14  1998/06/19 22:35:04  jaeger
# Changed instances of exit to return a value.
#
# Revision 1.13  1998/06/15 04:15:31  jaeger
# Added isBolean function and fixed errors when reading command line args.
#
# Revision 1.12  1998/05/25 20:10:05  jaeger
# Removed the Help button from the inputDialog, to make it consistent
# with the other dialogs.
#
# Revision 1.11  1998/05/22 17:56:10  jaeger
# Fixed the usage message.
#
# Revision 1.10  1998/05/22 16:47:04  jaeger
# Fixed bugs in "bgerror".  Problems occured when error messages had
# double quotes.  Switch to my choiceDialog instead of the hacked up
# ESO one.  Printed error messages on various debug levels only now.
#
# Revision 1.9  1998/05/20 21:08:12  jaeger
# Fixed bgerror, wasn't calling the "error monitor" properly.
#
# Revision 1.8  1998/05/15 16:49:16  jaeger
# Fixed some of the spacing.  Changed the logic of the bgerror procedure.
#
# Revision 1.7  1998/05/06 21:40:07  jaeger
# Fixed bug with choice dialog, extra characters after "
#
# Revision 1.6  1998/05/06 20:43:42  jaeger
# Added bgerror routine to catch background error.  Change infoDialog,
# errorDialog, warningDialog to use the incr widget message dialog
# widget instead of the eso Dialog widget.  This was done to avoid
# since the Dialog widget seemed to freeze when command responses
# were reported.
#
# Revision 1.5  1998/05/04 20:37:22  jaeger
# Merged utilities from Storage Server and DHS consoles.
#
# Revision 1.4  1998/04/15 22:40:53  jaeger
# Added simMode, extractNums, extractListItem procedures.  Altered
# resource display to use a different instance of the status class, this
# was needed due to the changes in the status class.
#
# Revision 1.3  1998/03/06 17:50:34  jaeger
# Added resWindow and createAbout procedures
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
# PROCEDURE NAME:
# addMsg
#
# INVOCATION:
# addMsg msgType msg
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) msgType	(i|w|e)	    Indicates is a message is informative, warning
#			    or an error.
# (>) msg	(string)    The message.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To add a message to the message queue.
#
# DESCRIPTION:
# Trivial 
#
# EXTERNAL VARIABLES:
# ::dhsMsgLock		;# Lock for the dhsMsgQueue
# ::dhsMsgQueue		;# The queue of messages.
# ::dhsTmpMsgQueue	;# Tmp message queue which is filled when 
#			   the dhsMsgQueue is locked.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# getMsg
# processMsgs
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	addMsg {
    msgType
    msg
} {
    global dhsMsgLock
    global dhsMsgQueue
    global dhsTmpMsgQueue

    #
    # If the dhs message queue is locked then add the new message to
    # the temp queue.
    #

    if { [ isFalse $dhsMsgLock ] } {
	set dhsMsgLock 1
	if { $dhsMsgQueue == {} } {
	    set dhsMsgQueue "$dhsTmpMsgQueue"
	    set dhsTempMsgQueue {}
	} else {
	    set dhsMsgQueue "$dhsMsgQueue $dhsTmpMsgQueue"
	}
	lappend dhsMsgQueue [ list "$msgType" "$msg" ]
	set dhsMsgLock 0
    } else {
	lappend dhsTmpMsgQueue [ list "$msgType" "$msg" ]
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# bgerror
#
# INVOCATION:
# bgerror errorMsg		 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) bgerror	(string) 	The error message.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Handles any background errors that may occur. Redefines the
# Tcl/Tk procecure bgerror.
#
# DESCRIPTION:
# If there is an error monitor defined then send the error to it.
# Otherwise If "bgerror" was redefined then send the error to it.
# Otherwise display the error message.
#
# EXTERNAL VARIABLES:
# ::__errorMonitor
# ::debug
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# processMsg
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

if { [ info commands bgerror ] != "" } {
    rename bgerror __bgerror
}
    
proc	bgerror {
    errorMsg		 
} {
    global __errorMonitor debug


    if { $debug == "FULL" } {
	puts "BACKGROUND ERROR: $errorMsg"
    }

    if { [ info exists __errorMonitor ] && $__errorMonitor != "" } {
	eval $__errorMonitor [list -] [list error] [list $errorMsg]
    } elseif { [ info commands __bgerror ] != "" } {
	if { $debug == "MIN" } {
	    puts "BACKGROUND ERROR: $errorMsg"
	}
	__bgerror [ list $errorMsg ]
    } else {
	after idle "errorDialog [ list $errorMsg ]"
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# confirmDialog
#
# INVOCATION:
# confirmDialog msg ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) msg 	(string) 	The messge to be displayed.
# (>) parent	(pathNmae)	The dialog's parent window, defulat is "."
#
# PROCEDURE VALUE:
# 1 (boolean) : If the answer is yes.
# 0 (boolean) : If the answer is no..
#
# PURPOSE:
# To confirm an action with a dialog box.
#
# DESCRIPTION:
# 1. Find the dialog's parent window.
# 2. Create the Dialog box.
# 3. Return the selected value.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# addMsg
# processMsg
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	confirmDialog {
     msg 
     { parent "" }
} {
    #
    # Find the parent.
    #

    if {"$parent" != ""} {
        if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }


    #
    # Create the dialog.
    #

    set w $parent.confirmDialog
    catch { destroy $w }
    set d [ util::DialogWidget $w 					\
		-bitmap questhead 					\
		-buttons { Yes Cancel }  				\
		-default 1 						\
		-modal yes						\
		-text $msg 						\
		-title Confirm 						\
		-transient 1 ]

    #
    # Fix the length of the text displayed and center the window.
    #

    set wrapLength [ winfo pixels [ $d component text ] 5i ]
    $d component text configure -wraplength $wrapLength
    if { "$parent" == "" } {
	$d center_window
    } else {
	$d center_window $parent
    }

    return [ expr { [ $d activate ] == 0 } ]
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# createToolbar
#
# INVOCATION:
# createToolbar name window ?nameList? ?commandList? ?descList? ?helpList?
# 	 ?topLevel?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name	    (string)	Unique name used to identify the toolbar
# (>) parent	    (path name)	The window the toolbar is to appear in.
# (>) nameList	    (list)	Names that will appear on the buttons.
# (>) commandList   (list)	Commands to be executed for each item.
# (>) descList	    (list)	Short help message for each item.
# (>) helpList	    (list)	HTML help page list.
# (>) toplevel      (path name) Toolbar's toplevel window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To create a toolbar for the DHS console or one of its subsystems.
#
# DESCRIPTION:
# 1. Create the toolbar
# 2. Add a button for each command
# 3. Add context sensitive help.
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
 
proc	createToolbar {
    name
    parent
    { nameList {} }
    { commandList {} }
    { descList {} }
    { helpList {} }
    { toplevel {} }
    { commandObject {} }
} {
    #
    # Make sure all the lists are the same length
    #

    set expectedLen [ llength $nameList ]

    if { [ llength $commandList ] != $expectedLen } {
        #
        # Error
        #
 
        error "ERROR: createToolbar: nameList, commandList are different lengths"
    }
    
 
    #
    # find toplevel of "parent"
    #
 
    if { "$toplevel" != "" } {
        set toplevel [ winfo toplevel $parent ]
    }
 
 
    #
    # Create the toolbar
    #
 
    set bar [ toolbar $parent.[ string tolower $name ] ]
    pack $bar -anchor center -expand y -padx 10 -pady 10
 
 
    #
    # Create a button for each command.
    #
 
    set descLength [ llength "$descList" ]
    set helpLength [ llength "$helpList" ]
    set i 0

    foreach commandName $nameList {
        if {  "$commandName" == "blank"  } {
	    incr i
	    continue
	}

	if { $commandObject != {} } {
	    set cmd "$commandObject [ lindex $commandList $i ]"
	    set insertPt [ string wordend $cmd 0 ]
	    set insertPt [ string wordend $cmd [ expr $insertPt + 1 ] ]
	} else {
	    set cmd "[ lindex $commandList $i ]"
	    set insertPt [ string wordend $cmd 0 ]
	}

	if { "$commandName" == "Initialize" ||
	     "$commandName" == "Reset" || 
	     "$commandName" == "Shutdown" || 
	     "$commandName" == "Test" } {
	    set cmd "[ string range $cmd 0 $insertPt ] $bar [ string range $cmd [ expr $insertPt + 1 ] end ]"
	} elseif { "$commandName" == "Datasets" } {
	    set cmd "$cmd $parent"
	}


	set button [ $bar add button command$i				\
	         -command "$cmd"					\
	         -text "$commandName" ]

	if { $i < $descLength } {
	    $toplevel add_short_help $button			\
		"{bitmap b1} Select button to [ lindex $descList $i ]"
	}

	if { $i < $helpLength } {
	    bind $button <Button-3> "[ lindex $helpList $i ]"
	} 

	pack configure $button -padx 2

        incr i
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# errorDialog
#
# INVOCATION:
# errorDialog message ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) message 	(string) 	The information messge to be displayed.
# (>) parent	(pathNmae)	The dialog's parent window, defulat is "."
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display an error messgae in a dialog
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
# addMsg
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc 	errorDialog {
    msg 
    { parent "" }
} {
    #
    # Determine the dialog's parent
    #

    if {"$parent" != ""} {
	if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
	    set parent ""
	}
    }
    set w $parent.errorDialog

    
    #
    # If error dialogue is being displayed then add the messsage to the
    # message queue.
    #    

    if { [ winfo exists $w ] } {
	addMsg "e" "$msg"
	return
    }

    
    #
    # Create the error dialogue.
    #

    messagedialog $w							\
	-bitmap error  							\
	-justify left							\
	-modality application						\
	-padx 20							\
	-pady 5								\
	-title Error  							\
	-text "Error: $msg"						

    set wrapLength [ winfo pixels [ $w component msg ] 5i ]
    $w configure -wraplength $wrapLength

    $w component shellchildsite configure 				\
	-borderwidth 2 							\
	-relief raised
    $w hide Cancel

    
    #
    # Display the error dialog
    #
    
    if { "$parent" == "" } {
	$w center
    } else {
	$w center $parent
    }


    #
    # Display any error messages that were requested while the
    # dialogue was being displayed.
    #

    after idle "$w activate;processMsgs [ list $w ] \"e\" \"Error: \""
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# extractListItems
#
# INVOCATION:
# extractListItems aList aString
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) aList	(list)		List of items to search for.
# (>) aString	(string)	String that is to be searched.
#
# PROCEDURE VALUE:
# (string) listItem: The first list item that is found in the string
# (string) ""      : If none of the list items were found in the string. 
#
# PURPOSE:
# To extract from the string one of the items given in the list.
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

proc    extractListItem { 
    aList
    aString
} {
    foreach item "$aList"  {
        if { [ string first "$item" "$aString" ] != -1 } {
            return "$item"
        } 
    }
    
    return ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# extractNums
#
# INVOCATION:
# extractNums sourceStr
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) sourceStr	(string)	String where numbers are extracted from
#
# PROCEDURE VALUE:
# (list) numList: A list of the numbers found in the string.
#
# PURPOSE:
# To find all occurences of numbers in a given string.
#
# DESCRIPTION:
# Search the string, recording each number found.
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

proc    extractNums {
     sourceStr
} {
    set numList {}
    set tmpName "$sourceStr"
 
    while { "$sourceStr" != "" && 					\
		[ regexp -- 0|1|2|3|4|5|6|7|8|9 "$tmpName" ] } {

	
	#
	# Find the starting index of the number
	#

        set tmpList "[ split $tmpName {0123456789} ]"
        if { [ lindex "$tmpList" 0 ] == {} } {
            set front 0
        } else {
            set front "[ string length [ lindex $tmpList 0 ] ]"
        }
        

	#
	# Find the index of the last digiet in the number
	#

        set index 1
        set last [ llength $tmpList ]
        set back "$front"
        while { $index < $last && [ lindex "$tmpList" $index ] == {} } {
            incr back
            incr index
        }
 
	
	#
	# Record the number find, reset tmpName to look for the next one.
	#

        lappend numList "[ string range $tmpName $front $back ]"
        set tmpName "[ string range $tmpName [ expr $back + 1 ] end ]"
    }
    return $numList
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# getImages
#
# INVOCATION:
# getImages imageList directoryList
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) imageList    (list)       List of the images to find.
# (>) dirList       (list)      List of the directories to search in for
#                               the images.
# PROCEDURE VALUE:
# 0  : if All images were found
# 1  : If at least one image was not found.
#
# PURPOSE:
# To create images for the each of the images given in the image
# list.  These images can then be used with the "-image" option.
#
# DESCRIPTION:
# Foreach image in the list search the directories for the image.
# Once the image is found no further directories are searched an
# an image is created with the name in the image list.
#
# A warning is issued if the image is not found in any of the
# directories.
#
# EXTERNAL VARIABLES:
# debug         Global value specifying the DHS Consoles debug level.
#
# PRIOR REQUIREMENTS:
# The command-line arguments have been parsed and, "debug" has a
# value.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
 
proc    getImages { 
    images 
    imageDirs 
} {
    global debug 
 
    set allFound 0
 
    foreach image $images {
 
        #
        # Find the file extension.
        #
 
        set ext [ file extension $image ]
        if { $ext == "" } {
            set ext .xbm
        } else {
            set image [ file rootname $image ]
        }
 
        set notFound true
        foreach dir $imageDirs {
            case $ext {
                .xbm {
                    #
                    # Check for the bitmap with the .xbm extsion
                    #
 
                    if [ file exists [ file join $dir $image.xbm ] ] {
                        image create bitmap $image                      \
                            -file [ file join $dir $image.xbm ]
                        set notFound false
                        break
                    } elseif [ file exists [ file join $dir $image ] ] {
                    #
                    # Check for the bitmap without the .xbm extsion
                    #
 
                        image create bitmap $image              \
                            -file [ file join $dir $image ]
                        set notFound false
                        break
                    }
                }
                
                .gif {
                    #
                    # Check for the gif image with .gif extension.
                    #
                    
                    if [ file exists [ file join $dir $image.gif ] ] {
                        image create photo $image               \
                            -file [ file join $dir $image.gif ]
                        set notFound false
                        break
                    } elseif [ file exists [ file join $dir $image ] ] {
                    #
                    # Check for the image without the .xbm extsion
                    #
 
                        image create photo $image               \
                            -file [ file join $dir $image ]
                        set notFound false
                        break
                    }
                }
 
                default {
                    if [ file exists [ file join $dir $image ] ] {

                        image create photo $image               \
                            -file [ file join $dir $image ]
                        break
                    }
                }
            }
        }
 
        if { $notFound } {
            #
            # The image was not found so all were not found and
            # if debug mode is anything but NONE, print a warning.
            #
            
            set allFound 1
            if { $debug != "NONE" } {
                puts "WARNING: image '$image' was not found."
            }
 
        }
    }

    return $allFound
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# getMsg
#
# INVOCATION:
# getMsg ?msgType?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) msgType	(i|w|e)	    Indicates is a message is informative, warning
#			    or an error.
#
# PROCEDURE VALUE:
# msg	(string)    The message.
#
# PURPOSE:
# To get and remove the latest message from the message queue.
#
# DESCRIPTION:
# Search the message queue for a message of the given type.
# If no type is given then take the first message.  If a message is
# found then remove it from the dhsMsgQueue.
#
# EXTERNAL VARIABLES:
# ::dhsMsgLock
# ::dhsMsgQueue
#
# PRIOR REQUIREMENTS:
# ::dhsMsgQueue and ::dhsMsgLock must exist.
#
# SEE ALSO:
# addMsgs
# processMsgs 
#
# DEFICIENCIES:
# None.
#
#-
#***********************************************************************
#

proc	getMsg {
   { msgType "" }
} {
    global dhsMsgLock
    global dhsMsgQueue

    set dhsMsgLock 1
    set found 0
    set msg ""
    for { set index 0 } { $index < [ llength $dhsMsgQueue ] &&\
	 [ isFalse $found ] } { incr index } {
	if { $msgType == "" || \
	     $msgType == [ lindex [ lindex $dhsMsgQueue $index ] 0 ] } {
	    set msg "[ lindex [ lindex $dhsMsgQueue $index ] 1 ]"
	    set dhsMsgQueue [ lreplace $dhsMsgQueue $index $index ]
	    set found 1
	}
    }
    set dhsMsgLock 0


    return "$msg"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# infoDialog
#
# INVOCATION:
# infoDialog message ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) message 	(string) 	The information messge to be displayed.
# (>) parent	(pathNmae)	The dialog's parent window, defulat is "."
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display an informative messgae in a dialog
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

proc 	infoDialog {
    msg 
    { parent "" }
} {
    #
    # Determine the dialog's parent window.
    #

    if {"$parent" != ""} {
	if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
	    set parent ""
	}
    }
    set w $parent.infoDialog


    #
    # If info dialogue is being displayed then add the messsage to the
    # message queue.
    #    

    if { [ winfo exists $w ] } {
	addMsg "i" "$msg"
	return
    }

    #
    # Create the informational Dialogue.
    #
	
    messagedialog $w							\
	-bitmap info 							\
	-justify left							\
	-modality application						\
	-padx 20							\
	-pady 5								\
	-title Information 						\
	-text "$msg"

    set wrapLength [ winfo pixels [ $w component msg ] 5i ]
    $w configure -wraplength $wrapLength
    $w component shellchildsite configure 				\
	-borderwidth 2 							\
	-relief raised
    $w hide Cancel



    #
    # Display the informational dialogue
    #
    
    if { "$parent" == "" } {
	$w center
    } else {
	$w center $parent
    }


    #
    # Display any messages that arrived while the information dialog
    # was being displayed.
    #

    after idle "$w activate;processMsgs [ list $w ] \"i\""

    
    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# inputDialog
#
# INVOCATION:
# inputDialog message ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) message 	(string) 	The information messge to be displayed.
# (>) parent	(pathNmae)	The dialog's parent window, default is "."
#
# PROCEDURE VALUE:
# (string) : The input entered in the dialogue window.
#
# PURPOSE:
# To display a dialogue that prompts for input and returns the input
# that was entered.  If a default value is given it is displayed
# initially.
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

proc 	inputDialog { 
    msg  
    {default {}}
    {parent "" } 
} {
    #
    # Find the parent window.
    #
    if { "$parent" != "" } {
        if {"[ set parent [ winfo toplevel $parent ] ]" == "."} {
            set parent ""
        }
    }


    #
    # Create the dialogue.
    #

    set w $parent.input_dialog
    catch { destroy $w }
    set d [ InputDialog $w 						\
		-bitmap questhead 					\
		-buttons {OK Cancel} 					\
		-default 0 						\
		-messagewidth 4i 					\
		-modal yes 						\
		-title Input 						\
		-text $msg 						\
		-transient 1 ]

    #
    # Insert the default value into the entry box and fix the width
    # of the text displayed.
    #

    if { $default != "" } {
	$d component entry delete 0 end
	$d component entry insert 0 "$default"
    }
    set wrapLength [ winfo pixels [ $d component text ] 5i ]
    $d component text configure -wraplength $wrapLength

    
    #
    # Center the window and get the operator's input.
    #

    if { "$parent" == "" } {
	$d center_window
    } else {
	$d center_window $parent
    }

    return "[ $d activate ]"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isBoolean
#
# INVOCATION:
# isBoolean value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a boolean value.
# (boolean) 1: If the value is a boolean value.
#
# PURPOSE:
# To determine is some string represents a boolean value.
# A boolean value is one of:
#   f, t, false, true, 0, 1, n, y, no, yes, off, or on
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

proc	isBoolean {
    value
} {
    set value [ string trim $value ]
    if { "$value" == "" } {
	return 0
    } elseif { [ isFalse "$value" ] } {
	return 1
    } elseif { [ isTrue "$value" ] } {
	return 1
    } else {
	return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isFalse
#
# INVOCATION:
# isFalse value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is "false".
# (boolean) 1: If the value is "false".
#
# PURPOSE:
# To determine is some string is false.
# A false value is one of:
#   f, false, 0, n, no, or off
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

proc    isFalse { 
    bool
} {
    if { "$bool" == "" } {
	return 0
    } 
    set bool [ string trim "$bool" ]
    if { [ regexp -nocase -- {(^f$)|(^false$)|(^n$)|(^no$)|(^off$)|(^[ -+ ]?[ 0 ]+$)} "$bool" ] 
} {
        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isInteger
#
# INVOCATION:
# isInteger value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not an integer number.
# (boolean) 1: If the value is an integer number.
#
# PURPOSE:
# To determine is some string represents a numerical value.
# A numberical value is either an integer or a real number.
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

proc    isInteger {
    value
} {
    if { "$value" == "" } {
	return 0
    } 

    set value [ string trim "$value" ]
    if { "$value" == "" } {
	return 0
    } elseif { [ regexp -- {^[ -+ ]?[ 0-9 ]+$} "$value" ] } {
        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isNumber
#
# INVOCATION:
# isNumber value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a number.
# (boolean) 1: If the value is a number.
#
# PURPOSE:
# To determine is some string represents a numerical value.
# A numberical value is either an integer or a real number.
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

proc    isNumber {
    value
} {
    if { "$value" == "" } {
	return 0
    } 

    set value [ string trim "$value" ]
    if { "$value" == "" } {
	return 0
    } elseif { [ regexp -- {^[ -+ ]?[ 0-9 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0-9 ]?\.[ 0-9 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0-9 ]+\.[ 0-9 ]+$} "$value" ] } {
        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isTrue
#
# INVOCATION:
# isTrue value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a "true" value.
# (boolean) 1: If the value is a "true" value.
#
# PURPOSE:
# To determine is some string represents a boolean value.
# A "true" value is one of:
#   t, true, 1, y, yes, or on
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

proc    isTrue { 
    bool
} {
    if { "$bool" == "" } {
	return 0
    } 
    set bool [ string trim "$bool" ]
    if { [ regexp -nocase -- {(^t$)|(^true$)|(^y$)|(^yes$)|(^on$)|(^[ + ]?1$)|(^[ + ]?[ 0 ]+1$)} "$bool" ] } {

        return 1
    } else {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isZero
#
# INVOCATION:
# isZero value
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) value	(string)	The string that is to be checked.
#
# PROCEDURE VALUE:
# (boolean) 0: If the value is not a zero value.
# (boolean) 1: If the value is a zero value.
#
# PURPOSE:
# To determine is some string represents a zero value.
# A zero value is one of:
#   0. 0.0000 or 0E??
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

proc    isZero {
    value
} {
    if { "$value" == "" || ! [ isNumber $value ] } {
	return 0
    } 

    set value [ string trim "$value" ]
    if { [ regexp -- {^[ -+ ]?[ 0 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0 ]?\.[ 0 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0 ]+\.[ 0 ]+$} "$value" ] } {
        return 1
    } elseif {  [ regexp -- {^[ -+ ]?[ 0 ]+E[ 0-9 ]+$} "$value" ] } {
        return 1
    } else {
        return 0
    }
}               

#
#***********************************************************************
#+
# PROCEDURE NAME:
# parseCommandLine
#
# INVOCATION:
# parseCommandLine argv argc
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) argv (list) 	List of command-line arguments given to ocswish.
# (!) argc (int) 	The number of command-line arguments.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To parse the command line arguments.
#
# DESCRIPTION:
# 1. Foreach item in argv determine the flag and it's value, if there
#    are any errors display the usage string.
# 2. If "debug" is not defined set it to the default value "NONE".
# 3. If "simulate" is not defined set it to the default value "NONE".
#
# EXTERNAL VARIABLES:
# debug		dhsConsoel debug level (NONE, MIN, or FULL)
# epics		TRUE indicates dhsConsole uses EPICS to read status RECORDS
# simulate	dhsConsole simulation level (NONE, FAST, VSM, or FULL )
# _dhgLongVersion
#		Long version information for the console.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# Dose not accpet any flag values that start with "-".
#-
#***********************************************************************
#

proc 	parseCommandLine {
    argv 
    argc
} {
    global debug epics simulate _dhgLongVersion

    set usageString {USAGE: dhsConsole [ -debug NONE|MIN|FULL ] [ -Help ]\
	 [ -simulate NONE|VSM|FAST|FULL ] [ -V ] [ -epics ]}

    set minusDebug	0
    set minusEpics	0
    set minusHelp	0
    set minusSimulate	0
    set minusV		0


    for { set i 0 } { $i < $argc } {} {
	#
	# Get the flag
	#

	set flag [ lindex $argv $i ]
	incr i


	#
	# Get the value if there is one.
	#

	if { $i < $argc && ! [ string match "-*" [ lindex $argv $i ] ]   } {
	    set value [ lindex $argv $i ]
	    incr i
	} elseif { [ info exists value ] } {
	    unset value
	}


	#
	# Do the appropriate action based on the flag.
	#

	switch -exact -- "$flag" {
	    "-debug" {
		if { [ isTrue $minusDebug ] || ! [ info exists value ] || 
		     ( $value!="NONE" && $value!="MIN" && $value!="FULL" ) } {
		    puts $usageString
		    exit 2 
		}
		
		set debug $value
		set minusDebug 1
	    }

	    "-epics" {
		if { [ info exists value ] || [ isTrue $minusEpics ] } {
		    puts $usageString
		    exit 2
		}
		set epics true
		set minusEpics 1
	    }

	    "-help" -
	    "?" {
		puts "$usageString"
		exit 0
	    }

	    "-simulate" { 
		if { [ isTrue $minusSimulate ] || ! [ info exists value ] } {
		    puts "$usageString"
		    exit 2
		} elseif  {  $value=="NONE" || $value=="VSM" } {
		    set simulate "NONE"
		    set minusSimulate 1
		} elseif { $value=="FAST" || $value=="FULL" } {
		    set simulate "FULL"
		    set minusSimulate 1
		} else {
		    puts $usageString
		    exit 2
		}
	    }

	    "-V" {
		if { [ info exists $value ] } {
		    puts $usageString
		    exit 2
		}

		puts "$_dhgLongVersion"
		exit 0
	    }

	    default {
		puts $usageString
		exit 2
	    }
	}
    }


    #
    # If the debug level is not set than set it to the default
    # value, "NONE".
    #

    if { ! [ info exists debug ] || "$debug" == "" } {
	set debug "NONE"
    }


    #
    # If the epics flag was not specifed then status comes from
    # the DHS status server, so set epics false
    #

    if { ! [ info exists epics ] || "$epics" == "" } {
	set epics false
    }


    #
    # If the simulation level is not set than set it to the default
    # value, "NONE".
    #

    if { ! [ info exists simulate ] || "$simulate" == "" } {
	set simulate "NONE"
    }

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# processMsgs
#
# INVOCATION:
# processMsgs ?w? ?msgType? ?prefix? 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) w		(window pathname) Window that displays the messages.
# (>) msgType	(char)		  Unique msg type identifier as follows:
#				    e - error.
#				    i - information.
#				    w - warning.
# (>) prefix	(string)	  Message prefix.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To display messages that are in the message Queue.
#
# DESCRIPTION:
# Reads and Displays messages of a given type from the message queue.
# If no window is provided then the messages are disaplayed to stdout.
# Otherwise the messages are displayed in the pop-up window given.
# The prefix given is prepended to the message that matches the given
# type.  If not message type is given all messages are displayed.
#
# EXTERNAL VARIABLES:
# ::dhsMsgLock
# ::dhsMsgQueue
#
# PRIOR REQUIREMENTS:
# dhsMsgLock and dhsMsgQueue exist.
#
# SEE ALSO:
# addMsg
# getMsg
#
# DEFICIENCIES:
# It is possible that processMsgs and one of the infoDialog, errorDialog
# and warningDialogs call processMsgs at the same time. 
#-
#***********************************************************************
#

proc processMsgs {
    { w "" }
    { msgType "" }
    { prefix "" }
} {
    global dhsMsgLock
    global dhsMsgQueue 


    #
    # Process later if the queue is being accessed.  If there is nothing
    # to be displayed then destroy the window.
    #

    if { [ isTrue $dhsMsgLock ] && $dhsMsgQueue != "" } {
	after 250 "processMsgs [ list $w ] [ list $msgType ] [ list $prefix ]"
    } elseif { $dhsMsgQueue == "" } {
	catch { destroy $w }
	return
    }


    #
    # Display the message.
    #
    
    set msg [ getMsg "$msgType" ]
    if { $msg != "" } {
	if { $w != "" } {
	    $w configure -text "${prefix}$msg"
	} else {
	    puts "${prefix}$msg"
	}
	after idle "$w activate;processMsgs [ list $w ] [ list $msgType ]\
		[ list $prefix ]"
    } 

    return
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# stopGoButtons
#
# INVOCATION:
# stopGoButtons window defaultArray ?displayList? ?commandList?
#		?label? ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) window	    (pathname)  Window path name for the buttons.
# (>) defaultArray  (array)	Associative array which defines default values.
# (>) displayList   (list)	The list of buttons that are to be displayed.
#				Allowed values are reverse, forward, and stop.
# (>) commandList   (list)	Commands that are to be associated with each
#				button.
# (>) label	    (string)	Label that appears by the buttons.
# (>) parent	    (pathname)	Window pathname of the parent window.
#
# PROCEDURE VALUE:
# (list) : The list of path names of the windows that are created,  in
# 	   the following order: labelled widget, reverse, stop, and forward.
#
# PURPOSE:
# To create a set of buttons for stopping and going.
#
# DESCRIPTION:
# Create a labelled widget to hold the buttons then create the
# buttons (reserve, stop, and forward).
#
# EXTERNAL VARIABLES:
# ::dhgDir	   Directory where the bitmaps for the buttons are.
# ::$defaultArray  Array containg default values.
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

proc 	stopGoButtons { 
    window 
    defaultArray 
    { displayList { reverse stop forward } } 
    { commandList { "" "" "" } } 
    { label "" } 
    { parent "" } 
} { 
    global $defaultArray 
    global dhgDir 
 
    # 
    # Determine the parent 
    # 
 
    if { "$parent" != "" } { 
        if { "[ set parent [ winfo toplevel $parent ] ]" == "." } { 
            set parent "" 
        } 
    } 
 

    # 
    # Create frame for the buttons. 
    # 
 
    eval set font \$${defaultArray}(labelFont)                           
    labeledwidget $window.stopGo                                        \
	-labelfont $font                                                \
	-labelmargin 2                                                  \
	-labeltext $label                                               \
	-labelpos n                                                      
    set childsite [ $window.stopGo childsite ] 
    lappend windowList $window.stopGo 
 

    # 
    # Create Reverse button. 
    # 
 
    if { [ set pos [ lsearch  $displayList reverse ] ] != -1 } { 
        button $childsite.reverse                                       \
	    -bitmap @${dhgDir}/images/reverse.xbm                     	\
	    -command [ lindex $commandList $pos ]                      	\
	    -disabledforeground gray70                                  \
	    -padx 0                                                     \
	    -state disabled 
        lappend windowList $childsite.reverse 
        pack $childsite.reverse -padx 0 -side left  
    } 
 
 
    # 
    # Create Stop button. 
    # 
 
    if { [ set pos [ lsearch  $displayList stop ] ] != -1 } { 
        button $childsite.stop                                          \
	    -bitmap @${dhgDir}/images/stop.xbm                         \
	    -command [ lindex $commandList $pos ]                       \
	    -disabledforeground gray70                                  \
	    -padx 0                                                     \
	    -state disabled 
        pack $childsite.stop -padx 0 -side left 
        lappend windowList $childsite.stop 
    } 
 

    # 
    # Create Forward button 
    # 
 
    if { [ set pos [ lsearch  $displayList forward ] ] != -1 } { 
        button $childsite.forward                                       \
	    -bitmap @${dhgDir}/images/forward.xbm                      \
	    -command [ lindex $commandList $pos ]                       \
	    -disabledforeground gray70                                  \
	    -padx 0                                                     \
	    -state disabled 
        pack $childsite.forward -padx 0 -side left  
        lappend windowList $childsite.forward 
    } 
 
    return $windowList 
} 

#
#***********************************************************************
#+
# PROCEDURE NAME:
# warningDialog
#
# INVOCATION:
# warningDialog message ?parent?
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) message 	(string) 	The warning messge to be displayed.
# (>) parent	(pathNmae)	The dialog's parent window, defulat is "."
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Display a warning messgae in a dialogue.
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

proc	warningDialog {
    msg
    { parent "" } 
} {
    #
    # Determine the dialogue's parent
    #

    if { "$parent" != "" } {
        if { "[ set parent [ winfo toplevel $parent ] ]" == "." } {
            set parent ""
        }
    }
    set w $parent.warningDialog


    #
    # If warning dialogue is being displayed then add the messsage to the
    # message queue.
    #    

    if { [ winfo exists $w ] } {
	addMsg "w" "$msg"
	return
    }


    #
    # Create the warning dialogue.
    #

    messagedialog $w							\
	-bitmap warning							\
	-justify left							\
	-modality application						\
	-padx 20							\
	-pady 5								\
	-title Warning 							\
	-text "Warning: $msg"
    set wrapLength [ winfo pixels [ $w component msg ] 5i ]
    $w configure -wraplength $wrapLength
    $w component shellchildsite configure 				\
	-borderwidth 2							\
	-relief raised
    $w hide Cancel
    

    #
    # Display the warning dialog
    #
    
    if { "$parent" == "" } {
	$w center
    } else {
	$w center $parent
    }
    after idle "$w activate;processMsgs [ list $w ] \"w\" \"Warning: \""

    return
}
