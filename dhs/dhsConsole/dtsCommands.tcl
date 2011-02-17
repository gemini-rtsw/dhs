# $Id: dtsCommands.itk,v 1.1.1.1 2002/02/21 20:23:34 tpaz Exp $
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
# dhsConsole/src/dtsCommands.itk
#
# PURPOSE:
# To set-up all of the DHS commands needed for the DHS Data Server.
#
# CLASS NAME(S)
# CDtsCommands			: Sets up the DHS service for issuing commands
#			 	   to the DHS Storage Server.
#
# METHOD NAME(S)
# PUBLIC
# CDtsCommands::cDtsCommandInit	: Initializes the CDtsCommand object.
# CDtsCommands::dtsDatasetDelete: Deletes a dataset from the Temporary Store.
# CDtsCommands::dtsDatasetHeader: Displays the FITS header of a dataset.
# CDtsCommands::dtsDatasetInfo	: Gets the lists of datasets known to the
#			  	  DHS Data Server. 
# CDtsCommands::dtsInfoMonitor	: Command Monitor used by the Data Server 
#			  	  commands.
#
# PRIVATE
# CDtsCommands::refreshInfo 	: Handles the "Refresh" button.  Essentially
#				  the same as dtsDatsetInfo except some
#				  information is already known.
#
# GLOBAL PROCEDURE(s)
# cDtsCommands			: Lowercased access method to CDtsCommands
#
# $Log: dtsCommands.itk,v $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:09  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.7  1999/06/11 22:19:31  jaeger
# Added data for simulation/testing so the display window is not empty.
#
# Revision 1.6  1998/08/14 21:52:29  jaeger
# Brought code documentation up to Gemini standards.  Delay processing
# of commands while a window has "grabbed" all X-events.
#
# Revision 1.5  1998/07/16 19:58:14  jaeger
# Added short help information.
#
# Revision 1.4  1998/06/25 17:55:13  jaeger
# Fixed some code documentation.
#
# Revision 1.3  1998/06/19 22:38:42  jaeger
# Brought code documentation up to Gemini standards.  Removed "-D" flag in sql
# statement.  Also altered so each row item is extracted instead of
# treatig the row as one data item.
#
# Revision 1.2  1998/06/15 16:29:06  jaeger
# Added error checking, ensure dts doesn't have its commands disabled.
# Also added debugging information.  Also used the CDhsCommands::confirmChoice
# method for confirming commands - eliminated some code.
#
# Revision 1.1  1998/05/15 17:42:46  jaeger
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
# CDtsCommands
#
# OPTIONS
# None.
#
# PUBLIC METHODS: 
# cDtsCommandInit	: Initializes thd CDtsCommand object.
# dtsDatasetDelete	: Deletes a dataset from the Temporary Store.
# dtsDatasetHeader	: Displays the FITS header of a dataset.
# dtsDatasetInfo	: Gets the lists of datasets known to the
#			  DHS Data Server. 
# dtsInfoMonitor	: Command Monitor used by the Data Server 
#			  commands.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To set-up a central place that issues the storage server commands.
#
#-
#***********************************************************************
#

class CDtsCommands {
    inherit CDhsCommands

    #
    # Public methods and data
    #

    public method 	constructor {}
    public method	destructor {} {}

    public method	cDtsCommandInit { statusObj configObj }
    public method	dtsDatasetDelete { dataLabel callbackFn }
    public method	dtsDatasetHeader { fileId masterWin }
    public method	dtsDatasetInfo { masterWin }
    public method	dtsInfoMonitor { name status info }


    #
    # Protected methods and data --- None.
    #

    #
    # Private methods and data
    #
    
    private  method   	refreshInfo { win type mediaId mediaName }

    private variable	isInit 0
    private variable 	masterWindow ""
    private variable 	statusObject ""
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cDtsCommands
#
# INVOCATION:
# cDhsCommands objectName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) objectName	(string)	Name for the new CDtsCommands object.
#
# PROCEDURE VALUE:
# objectName	(string) : The name of the new object that is created.
#
# PURPOSE:
# Lowercased access method to the CDtsCommands class.
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
# CDtsCommands::constructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc ::cDtsCommands {
    name
} {
	uplevel ::CDtsCommands $name
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDtsCommands::constructor
#
# INVOCATION:
# n.a.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# objectName (string) :  The name of the object that is created is returned.
#
# PURPOSE:
# Constructor for the CDtsCommands class
#
# DESCRIPTION:
# Set-up a number of commands in the DHS service for the storage server.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# CDtsCommands::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body CDtsCommands::constructor {
} {
	 
    #
    # Create the DHS service and define the various private variables.
    # 

    service dhs


    #
    # Define the various data server commands (bdDelete and datasetInfo.
    #

    dhs cs bdDelete
    cs bdDelete add datasetName
    cs bdDelete set datasetName mode in
    if { [ cs bdDelete verify ] == "failed" } {
	return [ list failed "Dataset Delete command verification failed." ]
    }

    dhs cs datasetInfo
    cs datasetInfo add database
    cs datasetInfo set database mode out
    cs datasetInfo add password
    cs datasetInfo set password mode out
    cs datasetInfo add server
    cs datasetInfo set server mode out
    cs datasetInfo add userName
    cs datasetInfo set userName mode out
    cs datasetInfo add isqlString
    cs datasetInfo set isqlString mode out
    if { [ cs datasetInfo verify ] == "failed" } {
	return [ list failed "Dataset Info command verification failed." ]
    }
}

# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDtsCommands::cDtsCommandInit
#
# INVOCATION:
# Object cDtsCommandInit statusObj configObj
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) statusObj	( cDhgStatus )	DHG status object.
# (>) configObj	( cConfigGen )	Gen configuration object.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To read the configuration file information and set the appropriate
# parameters.  Also to call the inherited init method 
# (CDhsCommands::cDhsCommandInit)
#
# DESCRIPTION:
# Initialize the inherited class CDhsCommands.  Make sure we have all
# the information from the configuration file.  Set the destination
# name for the commands.
#
# EXTERNAL VARIABLES:
# CDtsCommands::cmdHostname
# CDtsCommands::commandServer
#
# PRIOR REQUIREMENTS:
# configOpen has been called.
#
# SEE ALSO:
# CHdsCommands::cDhsCommandInit
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDtsCommands::cDtsCommandInit {
    statusObj
    configObject
} {
    #
    # Make sure the base class is initialzed.
    #

    set statusObject $statusObj
    cDhsCommandInit $statusObject $configObject


    if { $cmdHostname == "" } {
	error "Data Server hostname not known."
    }

    if { $commandServer == "" } {
	error "Command Server identity not known."
    }

    if { $dtsHostname == "" } { 
	error "Data Server hostname not known."
   }

    if { $dataServer == "" } { 
	error "Data Server identity not known."
    }


    #
    # Set the destination parameter for all of the commands.
    #

    set destination ${cmdHostname}:$commandServer

    cs bdDelete set destName value $destination
    cs datasetInfo set destName value $destination

    set isInit 1
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CthsCommands::dtsDatasetDelete
#
# INVOCATION:
# object dtsDatasetDelete dataLabel
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) dataLabel	    (string) 	The label of a dataset.
# (>) callbackFn    (function)	Cmd monitoring function
#
# PROCEDURE VALUE:
# (list) : "Posted|Not Posted" "bdDelete dataLabel" "commandMonitor"
#
# PURPOSE:
# To execute the DHS dtsDatasetDelete command, which deletes a
# dataset.
#
# DESCRIPTION:
# Make sure this is the command to be executed, with a dialogue
# message and if it is send the command to the data server.
#
# EXTERNAL VARIABLES:
# dhgCommand::activeSubsystems	- List of all the active subsystems.
#
# PRIOR REQUIREMENTS:
# CDtsCommands::cDtsCommandsInit has been called.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body 	CDtsCommands::dtsDatasetDelete {
    dataLabel
    callbackFn
} {
    global ::__errorMonitor

    set msg "Do you really want to delete dataset ${dataLabel}?"
    set args "datasteName = $dataLabel"

    
    if { [ confirmChoice DTS "$msg" bdDelete "$args" ] } {
	cs bdDelete add datasetName 
	cs bdDelete set datasetName value $dataLabel
	set __errorMonitor "$callbackFn"
	cs bdDelete postCallback "$callbackFn"
	return [ list Posted "bdDelete $dataLabel" "$callbackFn" ]
    } else {
	return [ list "Not Posted" "bdDelete $dataLabel" "$callbackFn" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDtsCommands::dtsDatasetHeader
#
# INVOCATION:
# Object dtsDatasetHeader fileId
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) fileId	(String)	Unique file identifier.
#
# PROCEDURE VALUE:
# (list) : "Posted|Not Posted" "datasetHeader" ""
#
# PURPOSE:
# To display the header of a particular dataset.
# First line should be a summary not terminated with a period.
#
# DESCRIPTION:
# Maker sure Data Server Commands are enabled.   If ther are enabled
# use dhsGet to get the FITS header and display the results in
# a pop-up window.
#
# EXTERNAL VARIABLES:
# CDhsCommands::activeSubsystems
# CDtsCommands;:dtsHostName
# CDtsCommands::dataServer
#
# PRIOR REQUIREMENTS:
# CDtsCommands::cDtsCommandsInit has been called.
#
# SEE ALSO:
# None
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDtsCommands::dtsDatasetHeader {
    fileId
    masterWin
} {
    global ::debug


    if { [ lsearch $activeSubsystems "DTS" ] == -1 } {
	errorDialog "Can not get FITS Header for $fileId, the Data Server is\
 NOT active."
	return [ list Error "dtsDatasetInfo, DTS not active" "" ]
    } elseif { [ lsearch $disabled "DHS" ] != -1 ||
	       [ lsearch $disabled "DTS" ] != -1 } {
	errorDialog "dtsDatasetInfo command is currently disabled."
	return [ list Error "dtsDatasetInfo command is disabled." "" ]
    }


    #
    # Display debugging information.
    #

    printDebug dhsGet "-d $dataServer -ip $dtsHostname -t HEADER \
 -id dhsGet -n $fileId -dest /tmp"


    #
    # Get the FITS header
    #

    set result [ exec dhsGet -d $dataServer -ip $dtsHostname -t HEADER 	\
		     -id dhsGet -n $fileId -dest /tmp ]


    #
    # Check for errors
    #

    set found false
    set errorMsg ""
    foreach line [ split $result "\n" ] {
	if { [ string match "Get Request*" "$line" ] } {	
	    set found true
	    if { [ string match "*FAILED*" "$line" ] } {	
		set errorMsg "$line"
	    } 
	} elseif { [ string match "message*" "$line" ] && $errorMsg != "" } { 
	    set errorMsg "$errorMsg\n\n$line"
	} 
    } 

    if { [ isFalse $found ] } {
	errorDialog "$result"
	return
    } elseif { $errorMsg != "" } {
	errorDialog "$errorMsg"
	return
    }


    #
    # Create a window for the dataset, if there is not one, otherwise
    # display it.
    #
    
    set fileId [ string trim $fileId ]
    set window "$fileId"
    while { [ set pos [ string first "." $window ] ] != -1 } {
	set window "[ string range $window 0 [ expr $pos - 1 ] ]_[ string range $window [ expr $pos + 1 ] end ]"
    }
    set window ".__$window"
	
    if { [ winfo exists $window ] } {
	#
	# Window exists so delete its contents and display it.
	#

	set childsite [ eval $window childsite ]
	if { ![ winfo ismapped $window ] } {
	    wm deiconify $window
	}
	$childsite.header delete 1.0 end
	raise $window
    } else {
	#
	# Need to create the window
	#
	
	set shortHelpParent [ winfo toplevel $masterWin ]
	dialog $window


	#
	# Fix up the buttons.
	#

	$window hide Apply
	$window hide Cancel

	$window buttonconfigure Help  				\
	    -command "_helpInfo openUrl dhgDtsFits.html#fitsHdrWindow"

	$shortHelpParent add_short_help					\
	    [ $window component bbox component Help ]		\
	    {{bitmap b1} Display on-line help information describing this window.}
	$shortHelpParent add_short_help					\
	    [ $window component bbox component OK ]			\
               {{bitmap b1} Remove this window from the X display.}

	
	
	set childsite [ eval $window childsite ]
	scrolledtext $childsite.header					\
	    -height 150							\
	    -hscrollmode dynamic					\
	    -labelpos n 						\
	    -labeltext "Header of Dataset: $fileId"			\
	    -textfont "-*-Courier-Bold-R-Normal--*-120-*-*-*-*-*-*"	\
	    -vscrollmode dynamic					\
	    -wrap none
	$shortHelpParent add_short_help	$childsite.header		\
    		"The Primary FITS Header Unit for dataset $fileId."


	pack $childsite.header -expand y -fill both -padx 3 -pady 3 -side top

	eval $window activate
    }


    #
    # Set-up context sensitive help.
    #

    foreach comp [ $window component ] {
	bind [ $window component $comp ]	\
	    <Button-3> "_helpInfo openUrl dhgHeader.html"
    }


    #
    # Since FITS files have 80 character cards we only want to
    # read 80 characters at a time and put them on separate lines.
    #

    set file [ open /tmp/$fileId r ] 
    set i 1
    while { ! [ eof $file ] } {
	set line "[ read $file 80 ]\n"
	$childsite.header insert end "$line"
	incr i
    }
    close $file
    exec rm /tmp/$fileId

    $childsite.header config -height 450 -state disabled -width 590 

}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDtsCommands::dtsDatasetInfo
#
# INVOCATION:
# Object dtsDatasetInfo
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None
#
# PROCEDURE VALUE:
# (list) : "Posted|Not Poseted" "datasetInfo" "commandMonitor"
#
# PURPOSE:
# To show the list of datasets known by the data server.
#
# DESCRIPTION:
# Confirm the command selection, if this is the desired command
# then send it off to the Data Server.
#
# EXTERNAL VARIABLES:
# None
#
# PRIOR REQUIREMENTS:
# CDtsCommands::cDtsCommandsInit has been called.
#
# SEE ALSO:
# CDtsCommands::dtsInfoMonitor
# CDhsCommands::confirmChoice
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDtsCommands::dtsDatasetInfo {
    masterWin
} {
    global ::__errorMonitor

    if { [ confirmChoice DTS "" datasetInfo ] } {
	set __errorMonitor "[ code $this dtsInfoMonitor ]"
	set masterWindow $masterWin
	cs datasetInfo postCallback "[ code $this dtsInfoMonitor ]"
	return [ list "Posted" "datastInfo" "[ code $this dtsInfoMonitor ]" ]
    } else {
	return [ list "Not Posted" "datastInfo" "[ code $this dtsInfoMonitor ]" ]
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDtsCommands::dtsInfoMonitor
#
# INVOCATION:
# Ojbect dtsInfoMonitor name status info
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name      (string)        Command being monitor
# (>) status    (string)        Status of the command
# (>) info      (string)        Informative message
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To monitor the results from the dtsDatasetInfo command.
#
# DESCRIPTION:
# If an error occurs then display the error.
# If the command has completed (idle) then dislplay the list of
# datasets in the datasetDisplay window.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# CDtsCommands::dtsDatasetInfo has been called.
#
# SEE ALSO:
# CDtsCommands::dtsDatasetInfo
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CDtsCommands::dtsInfoMonitor {
    name
    status
    info
} {
    global ::debug ::__errorMonitor


    #
    # Problems can occur if a modal window is displayed when commands
    # arrive, so we try and avoid this.
    #

    if { [ grab current ] != "" && $status != "busy" } {
	after 500 "[ code $this dtsInfoMonitor ] [ list $name ] 	\
		[ list $status ] [ list $info ]"
	return
    }



    set cmd [ lindex [ split $name "-" ] 0 ]

    if { $status == "busy" } {
	if { $debug == "MIN" } {
	    puts [ format "COMMAND: %-20s Status: %-10s\n\t %s\n" "$name" \
		       "$status" "$info" ]
	}
    } elseif { $status == "error" } {
	#
	# Display an error dialogue
	# 

	errorMonitor "DTS" "$name" "$status" "$info"
    } elseif { $status == "idle" } {
	if { $debug == "FULL" } {
	    puts [ format "COMMAND: %-20s Status: %-10s\n\t %s\n" "$name"\
		       "$status" "$info" ]
	}


	#
	# Get the command results.
	#

	set database [ cm $name get database ]
	set password [ cm $name get password ]
	set server [ cm $name get server ]
	set userName [ cm $name get userName ]
	set isql [ cm $name get isqlString ]


	#
	# Execute the isql command
	#

	if { [ catch {set results [ exec -keepnewline isql -P $password -S $server -U $userName -w 200 << "$isql" ]} errMsg ] } {
	    if { "$database" == "" } {

		set results "\n\n\
file1                                          file1.fits                                   FPERM                 3412
testfile2                                      testfile2.tar.gz                              RTEMP                 23409
file6                                          file6.fits                                    FPERM                 8724
file78                                         file78.fits                                   FTEMP                 76134
wv232                                          wv232                                         RTEMP                 2342
wv233                                          wv233                                         RTEMP                 2987
image782.3                                     image782.3.fits.gz                            FPERM                 98343
image9232.2.3:0                                image9232.2.2:0.fits                          FPERM                 234
testing                                        testing.fits                                  FTEMP                 29324
focus32.33                                     focus32.33.fits                               FPERM                 3423
image231.3.3:9                                 image231.3.3:9.fits.gz                        FPERM                 324
wvs3452                                        wvs3452                                       RPERM                 98743
file8321                                       file8321.fits                                 FPERM                 2342
wvs324234                                      wvs23234                                      RPERM                 98762\n\n\n"
	    } else {
		after idle "errorDialog \"$errMsg\""
		return
	    }
	}


	#
	# Display the results.
	#

	if { [ winfo exists .datasetDisplay ] } {
	    .datasetDisplay clear
	} else {
	    cDatasetDisplay .datasetDisplay $this $masterWindow
	}
	

	#
	# Each line is expected to be as follows:
	# file ID:	characters 0-46
	# file Name: 	characters 47-92
	# format: 	characters 93-93
	# volume: 	characters 94-114
	# size: 	characters 115-
	#

	set i 0
	set last [ expr [ llength [ split $results "\n" ] ] - 3 ]
	if { $last <= 1 } {
	    infoDialog "There are no datasets to be displayed."
	    return
	} else {
	    foreach line [ split $results "\n" ] {
		if { "[string trim $line ]" == "" } {
		    incr i
		    continue
		}

		if { $i > 1 &&  $i < $last } {
		    set fileId [ string trim [ string range $line 0 46 ] ]
		    set fileName [ string trim [ string range $line 47 92 ] ]
		    set format [ string trim [ string range $line 93 93 ] ]
		    set volume [ string trim [ string range $line 94 114 ] ]
		    set size [ string trim [ string range $line 115 end ] ]
		    .datasetDisplay addRow "$fileId" "$fileName" 	\
			"$format" "$volume" "$size"
			
		}
		incr i
	    }
	}

	if { ! [ winfo ismapped .datasetDisplay ] } {
	    .datasetDisplay activate
	}
	raise .datasetDisplay
    }


    #
    # If the error monitor variable was set then unset it.
    #

    if { [ info exist __errorMonitor ] } {
	if { $__errorMonitor == "dtsInfoMonitor" } {
	    unset __errorMonitor
	}
    }
}
