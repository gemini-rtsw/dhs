# $Id: config.tcl,v 1.1 2004-08-30 09:15:32 brighton Exp $
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
# dhsQlTool/src/config.tcl
#
# PURPOSE:
# Contains the CDhsConfigure class
#
# CLASS NAME(S)
# CDhsConfigure - Contains the configuration for the DHS Quick Look Tool.
#
# METHOD NAME(S)
# CDhsConfigure::constructor - Constructor for the class.
# CDhsConfigure::change - Activate the configuration widget to allow the
#		configuration to be changed.
# CDhsConfigure::configSet - Called when the ok button on the configuration
#		widget is pressed.
# CDhsConfigure::get - Return the current value of a configuration item.
# CDhsConfigure::load - Prompt for a file name and load configuration 
#		information from the file.
# CDhsConfigure::loadFile - Load configuration information from a specified
#		file.
# CDhsConfigure::save - Prompt for a file name and save the current
#		configuration to the file.
#
# $Log: not supported by cvs2svn $
# Revision 1.2  2004/08/13 20:18:54  brighton
# Linux test/port
#
# Revision 1.1.1.1  2002/11/24 20:29:36  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.5  1999/02/05 18:42:59  nhill
# Changed the configGen configOpen method name to configOpenFile.
#
# Revision 1.4  1999/01/29 17:51:33  nhill
# Changed the order of the Data Server id and address to match the more
# common convention.
#
# Revision 1.3  1999/01/20 23:26:33  nhill
# Added image arithmetic configuration.
#
# Revision 1.2  1998/10/08 19:38:24  nhill
# Updated to save more configuration information, and to use the configGen
# routines.
#
# Revision 1.1  1997/10/24 22:20:05  nhill
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
# CDhsConfigure
#
# PUBLIC METHODS: 
# constructor	- Constructor for the class.
# change 	- Display a form to allow the configuration to be changed.
# get 		- Get the value of a configuration item.
# load 		- Prompt for a file name and load configuration information 
#		  from the file.
# loadFile	- Load configuration information from a specified file.
# save		- Prompt for a file name and save the current configuration 
#		  to the file.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# Reads, writes and stores the configuration for the dhs QL tool
#
#-
#***********************************************************************
#

itcl::class CDhsConfigure {
    inherit util::TopLevelWidget


    #
    # Public methods and data
    #

			constructor { args } {}
    method		change {}
    proc		get { name }
    method		load {}
    method		loadFile { fileName }
    method		save {}


    #
    # Private methods and data
    #

    private method	configSet {}
    private method	setAutoCut { numTokens tokens }
    private method	setAutoSkip { numTokens tokens }
    private method	setCustomStream { numTokens tokens }
    private method	setDataServer { numTokens tokens }
    private method	setDisplayNewest { numTokens tokens }
    private method	setQlServer { numTokens tokens }
    private method	setImageArith { numTokens tokens }

    private common	dcConfigData
}
usual	CDhsConfigure {
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::constructor
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
# Constructor for the CDhsConfigure class
#
# DESCRIPTION:
# - Open and read the default configuration file.
# - Create the widget to allow the configuration to be changed.
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

body		CDhsConfigure::constructor {
    args
} {
    global	appName
    global	env


    util::TopLevelWidget::constructor -center 0 -standalone 0

    eval	itk_initialize $args


    #
    #  Initialize the configuration array.
    #

    set dcConfigData(dataServerName) {}
    set dcConfigData(dataServerAddr) {}


    #
    #  Try to find a configuration file.
    #

    set appVarName [string toupper $appName]_CONFIG_DIR
    set configFileName {}

    if { [ info exists env($appVarName) ] } {
	if { [ file exists "$env($appVarName)/$appName.config"] } {
	    set configFileName $env($appVarName)/$appName.config
	}
    }
    
    if { $configFileName == {} && [ info exists env(DEFAULT_CONFIG_DIR) ] } {
	if { [ file exists "$env(DEFAULT_CONFIG_DIR)/$appName.config" ] } {
	    set configFileName $env(DEFAULT_CONFIG_DIR)/$appName.config
	}
    }
    
    if { $configFileName == {} && [ file exists $appName.config ] } {
	set configFileName ./$appName.config
    }


    #
    #  If a configuration file was found, read it.
    #

    if { $configFileName != {} } {
	puts stdout "Reading configuration file: $configFileName"
	if { [ catch {loadFile $configFileName} msg ] != 0 } {
	    global	errorInfo
	    set savedInfo $errorInfo
	    puts "$msg $savedInfo"
	    error
	}
    } else {
	puts "No Configuration file found."
	error
    }


    #
    #  Set up the configuration editing frame.
    #

    itk_component add dataServerAddr {
	    Entryfield $itk_interior.dsAddr -labelmargin 5		\
		    -labeltext "Data Server Address"
    }

    itk_component add dataServerName {
	    Entryfield $itk_interior.dsName -labelmargin 5		\
		    -labeltext "Data Server Name"
    }

    pack $itk_component(dataServerAddr) -side top -expand no -fill y	\
	    -padx 10 -pady 10 -anchor e
    pack $itk_component(dataServerName) -side top -expand no -fill y	\
	    -padx 10 -pady 10 -anchor e


    #
    # Set up the buttons to control the frame.
    #

    buttonbox $itk_interior.bbox
    $itk_interior.bbox add ok -text "Ok" -command [ code $this configSet ] 
    $itk_interior.bbox add cancel -text "Cancel" 			\
	    -command "[ code grab release $itk_interior] ; [code wm withdraw $itk_interior ]"
    pack $itk_interior.bbox -side bottom -expand yes -fill both

    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::change
#
# INVOCATION:
# object change
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Active the configuration screen to allow the configuration to be changed
#
# DESCRIPTION:
# - Copy the configuration data to the edit frame.
# - Activate the edit frame.
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

body		CDhsConfigure::change {
} {
    #
    # Copy the configuration data to the edit frame.
    #

    foreach item [ array names dcConfigData ] {
	if { [ info exists itk_component($item) ] } {
	    $itk_component($item) clear
	    $itk_component($item) insert 0 $dcConfigData($item)
	}
    }


    #
    # Activate the edit frame.
    #

    wm deiconify $itk_interior
    grab set $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::configSet
#
# INVOCATION:
# object configSet
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Called when the ok button on the configuration pannel is pressed
#
# DESCRIPTION:
# Copy the data from the pannel to the configuration variables, and
# then withdraw the pannel.
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

body		CDhsConfigure::configSet {
} {
    #
    # Copy the configuration data from the edit form into the configuration
    # data array.
    #

    foreach item [ array names dcConfigData ] {
	if { [ info exists itk_component($item) ] } {
	    set dcConfigData($item) [ $itk_component($item) get ]
	}
    }


    #
    # Withdraw the edit window.
    #

    grab release $itk_interior
    wm withdraw $itk_interior
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::get
#
# INVOCATION:
# object get name
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) name - Name of the configuration item to return
#
# PROCEDURE VALUE:
# (string) - The value of the configuration item.
#
# PURPOSE:
# Return the current value of a configuration item
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

body		CDhsConfigure::get {
    name
} {
    return $dcConfigData($name)
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::load
#
# INVOCATION:
# object load
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Load the configuration from a file.
#
# DESCRIPTION:
# - If necessary, create the file selection widget.
# - Prompt for a configuration file name.
# - Execute the loadFile method to read the file.
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

body		CDhsConfigure::load {
} {
    #
    # If necessary, create the file selection widget.
    #

    if { ! [ info exists itk_component(fileSelect) ] ||			\
	    ! [ winfo exists $itk_component(fileSelect) ] } {
	#
	#  Create a file selection widget.
	#

	itk_component add fileSelect {
	    FileSelect $itk_interior.fs -full 0
	}
    }


    #
    # Prompt for the file name, and load the file if one was selected.
    #

    if { [ $itk_component(fileSelect) activate ] } {
	loadFile [ $itk_component(fileSelect) get ]

	[ winfo parent $itk_interior ] streamSet
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# loadFile
#
# INVOCATION:
# Load the configuration parameters from a file
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) fileName - The name of the file to read.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Load configuration parameters from a file.
#
# DESCRIPTION:
# - Open the file.
# - Read configuration items until the end of file is seen. Blank lines,
#   and lines starting with "#" are ignored.
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

body		CDhsConfigure::loadFile {
    fileName
} {
    set config [CConfigGen $w_.config]


    foreach item [ array names dcConfigData ] {
	unset dcConfigData($item)
    }
    set dcConfigData(customStreams) {}


    #
    # Open the file.
    #

    [ winfo parent $itk_interior ].customStreamWidget clear
    [ winfo parent $itk_interior ].streamWidget unSubscribeAll

    $config configOpenFile $fileName

    if { [ $config configGet qlServer 					\
	    [ code $this setQlServer ] ] != "SUCCESS" } {
	error "Unable to read Quick Look Server configuration information."
    }

    if { [ $config configGet dataServer 				\
	    [ code $this setDataServer ] ] != "SUCCESS" } {
	error "Unable to read dataserver configuration information."
    }

    $config configGet customStream [ code $this setCustomStream ]
    $config configGet autoCut [ code $this setAutoCut ]
    $config configGet displayNewest [ code $this setDisplayNewest ]
    $config configGet autoSkip [ code $this setAutoSkip ]
    $config configGet imageArithmetic [ code $this setImageArith ]

    $config configClose

    delete object $config
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::save
#
# INVOCATION:
# object save
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Save the current configuration to a file
#
# DESCRIPTION:
# - If necessary, create a file selection widget.
# - Prompt for a file name and save the configuration to the file.
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

body		CDhsConfigure::save {
} {
    #
    # If necessary create the file selection widget.
    #

    if { ! [ info exists itk_component(fileSelect) ] ||			\
	    ! [ winfo exists $itk_component(fileSelect) ] } {
	#
	#  Create a file selection widget.
	#

	itk_component add fileSelect {
	    FileSelect $itk_interior.fs -full 0
	}
    }


    #
    # Prompt for a file name.
    #

    if { [ $itk_component(fileSelect) activate ] } {
	set fileId [ open [ $itk_component(fileSelect) get ] w ]


	#
	# Write the configuration data to the file.
	#

	puts $fileId "dataServer $dcConfigData(dataServerName) $dcConfigData(dataServerAddr)"
	puts $fileId ""
	puts $fileId "qlServer $dcConfigData(qlServerName)"
	puts $fileId ""

	set streams [ concat "[ [ winfo parent $itk_interior ].streamWidget streamList ]" "[ [ winfo parent $itk_interior ].customStreamWidget streamList ]" ]
	set last  {}
	foreach stream [ lsort $streams ] {
	    if { "$last" != "$stream" } {
		puts $fileId "customStream	$stream"
	    }
	    set last $stream
	}

	puts $fileId ""
	if { [ cQlServer::getAutoCut ] } {
	    puts $fileId "autoCut on"
	} else {
	    puts $fileId "autoCut off"
	}

	if { [ cQlServer::getAutoSkip ] } {
	    puts $fileId "autoSkip on"
	} else {
	    puts $fileId "autoSkip off"
	}

	if { [ cQlServer::getDisplayLast ] } {
	    puts $fileId "displayLast on"
	} else {
	    puts $fileId "displayLast off"
	}

	if { [ [ cQlServer::imageArithName ] getOperator ] != {} } {
	    puts $fileId "imageArithmetic [ [ cQlServer::imageArithName ] getOperatorName ] [ [ cQlServer::imageArithName ] getDatasetName ] [ [ cQlServer::imageArithName ] getFrameId ]"
	}

	close $fileId
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setAutoCut
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) tokens (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Parse an autoCut line from the configuration file
#
# DESCRIPTION:
# Check that the keyword and number of arguments is correct, and that the
# argument is a boolean value. If the configuration parses ok, set the
# initial autocut value.
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

body		CDhsConfigure::setAutoCut	{
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "autoCut" } {
	if { $numTokens != 2 } {
	    error "Configuration file error, expected 2 tokens for autoCut configuration but got $numTokens"
	}

	if { [ isTrue [ lindex $tokens 1 ] ] } {
	    cQlServer::autoCut 1
	} elseif { [ isFalse [ lindex $tokens 1 ] ] } {
	    cQlServer::autoCut 0
	} else {
	    error "Expected boolean value for autoCut, not [ lindex $tokens 1 ]"
	}
    } else {
	error "Expected configuration information for autoCut, not [ lindex $tokens 0 ]"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setAutoSkip
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) tokens (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Parse an autoSkip line from the configuration file
#
# DESCRIPTION:
# Check that the keyword and number of arguments is correct, and that the
# argument is a boolean value. If the configuration parses ok, set the
# initial autoSkip value.
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

body		CDhsConfigure::setAutoSkip {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "autoSkip" } {
	if { $numTokens != 2 } {
	    error "Configuration file error, expected 2 tokens for autoSkip configuration but got $numTokens"
	}

	if { [ isTrue [ lindex $tokens 1 ] ] } {
	    cQlServer::autoSkip 1
	} elseif { [ isFalse [ lindex $tokens 1 ] ] } {
	    cQlServer::autoSkip 0
	} else {
	    error "Expected boolean value for autoSkip, not [ lindex $tokens 1 ]"
	}
    } else {
	error "Expected configuration information for autoSkip, not [ lindex $tokens 0 ]"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setCustomStream
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) paramName (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Called to add a new custom stream to the list of custom streams
#
# DESCRIPTION:
# Check that the keyword and the number of parameters is correct, and if the
# data parses ok, add the new custom stream to the list of custom streams.
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

body		CDhsConfigure::setCustomStream {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "customStream" } {
	if { $numTokens != 2 } {
	    error "Configuration file error, expected 2 tokens for customStream configuration but got $numTokens"
	}

	[ winfo parent $itk_interior ].customStreamWidget add [ lindex $tokens 1 ]
	lappend dcConfigData(customStreams) [ lindex $tokens 1 ]
    } else {
	error "Expected configuration information for customStreamm, not [ lindex $tokens 0 ]"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setDataServer
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) paramName (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the data server information from the configuration file.
#
# DESCRIPTION:
# Check that the keyword and the number of arguments is correct. If the 
# data parses ok, set the data server identification information.
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

body	CDhsConfigure::setDataServer {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "dataServer" } {
	if { $numTokens != 3 } {
	    error "Configuration file error, expected 3 tokens for dataServer configuration but got $numTokens"
	}

	set dcConfigData(dataServerName) [ lindex $tokens 1 ]
	set dcConfigData(dataServerAddr) [ lindex $tokens 2 ]
    } else {
	error "Expected configuration information for dataServer, not [ lindex $tokens 0 ]"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setDisplayNewest
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) tokens (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the display newest flag from the configuration file
#
# DESCRIPTION:
# Check that the keyword and number of arguments is correct, and that the
# argument is a boolean value. If the configuration parses ok, set the
# initial displayNewest value.
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

body		CDhsConfigure::setDisplayNewest {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "displayNewest" } {
	if { $numTokens != 2 } {
	    error "Configuration file error, expected 2 tokens for displayNewest configuration but got $numTokens"
	}

	if { [ isTrue [ lindex $tokens 1 ] ] } {
	    cQlServer::displayLast 1
	} elseif { [ isFalse [ lindex $tokens 1 ] ] } {
	    cQlServer::displayLast 0
	} else {
	    error "Expected boolean value for displayNewest, not [ lindex $tokens 1 ]"
	}
    } else {
	error "Expected configuration information for displayNewest, not [ lindex $tokens 0 ]"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setImageArith
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) tokens (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Set the image arithmetic information from the configuration file.
#
# DESCRIPTION:
# Check that the keyword and number of tokens is correct, and if they
# are, set the image arithmetic.
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

body	CDhsConfigure::setImageArith {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "imageArithmetic" } {
	if { $numTokens != 4 } {
	    error "Configuration file error, expected 4 tokens for image arithmetic configuration but got $numTokens"
	}
	[ cQlServer::imageArithName ] setOperator [ lindex $tokens 1 ]
	[ cQlServer::imageArithName ] setDatasetName [ lindex $tokens 2 ]
	[ cQlServer::imageArithName ] setFrameId [ lindex $tokens 3 ]
    } else {
	error "Expected configuration information for image arithmetic, not [ lindex $tokens 0 ]"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CDhsConfigure::setQlServer
#
# INVOCATION:
# N.A. Called by the configGet method of the CConfigGen class.
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numTokens (int) The number of tokens on the line.
# (>) tokens (string list) The list of tokens.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# Set the Quick Look Server information from the configuration file.
#
# DESCRIPTION:
# check that the keyword and number of tokens is correct, and if they are,
# set the Quick Look Server identity.
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

body	CDhsConfigure::setQlServer {
    numTokens
    tokens
} {
    if { [ lindex $tokens 0 ] == "qlServer" } {
	if { $numTokens != 2 } {
	    error "Configuration file error, expected 2 tokens for qlServer configuration but got $numTokens"
	}

	set dcConfigData(qlServerName) [ lindex $tokens 1 ]
	cQlServer::serverName $dcConfigData(qlServerName)
    } else {
	error "Expected configuration information for qlServer, not [ lindex $tokens 0 ]"
    }
}
