# $Id: configGen.tcl,v 1.2 2004-08-13 20:18:54 brighton Exp $
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
# dhsStoConsole/src/genConfig.itk
#
# PURPOSE:
# CConfigGen
#
# CLASS NAME(S)
# CConfigGen	: Responsible for reading configuration file information.
#
# METHOD NAME(S)
# PUBLIC
# CConfigGen::configClose	: Closes a configuration file.
# CConfigGen::configGet		: Gets information matching for a gievn keyword.
# CConfigGen::configOpen	: Open a configuration file.
# CConfigGen::configOpenFile	: Open a specific configuration file.
# CConfigGen::constructor	: Constructor for the CConfigGen class.
# CConfigGen::destructor	: Destructor for the CConfigGen class.
#
# PRIVATE
# CConfigGen::parseFile		: Parses the configuration file.
#
# GLOBAL PROCEDURE(S)
# cConfigGen	: lower cased method to CConfigGen class.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/11/24 20:29:33  brighton
# Imported sources
#
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:26  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.2  1999/02/05 18:43:35  nhill
# Added a the configOpenFile method to open a specific config file, and
# reverted the configOpen method back to searching for a file to open.
#
# Revision 1.1  1998/10/08 19:38:55  nhill
# Initial revision
#
# Revision 1.3  1998/06/17 20:07:54  jaeger
# Documented code to bring it up to Gemini standards
#
# Revision 1.2  1998/05/04 18:45:51  jaeger
# Corrected some of the spacing to be Gemini complient.
#
# Revision 1.1  1998/04/15 22:42:50  jaeger
# Initial revision
#
# Revision 1.1  1998/04/08 20:27:40  jaeger
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
# cConfigGen
#
# OPTIONS
# None.
#
# PUBLIC METHODS: 
# configClose	: Closes a configuration file.
# configGet	: Gets information matching for a gievn keyword.
# configOpen	: Open a configuration file.
#
# PUBLIC DATA: (">" input, "!" modified, "<" output)
# None.
#
# PURPOSE:
# To provide facilities for reading configuration files.
#
#-
#***********************************************************************
#

itcl::class	CConfigGen {
    #
    # Public methods and data
    #

    public method 	constructor {} {}
    public method	destructor {} { configClose }

    public method	configClose { }
    public method	configGet { key fn }
    public method	configOpen { appName }
    public method	configOpenFile { fileName }


    #
    # Protected methods and data -- NONE
    #


    #
    # Private methods and data
    #

    private method	parseFile

    private variable	fileId
    private variable	fileArray
};

#
#***********************************************************************
#+
# PROCEDURE NAME:
# cConfigGen
#
# INVOCATION:
# cConfigGen pathName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) pathName  (windowPath)    A window path name for a nonexistent window.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Provide a lowercase 
# First line should be a summary not terminated with a period.
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
# CConfigGen::constructor
# CConfigGen::destructor
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc	cConfigGen {
    pathName    
} {
    uplevel CConfigGen $pathName
}

#
# ------------------------------------------------------------------
#                         PUBLIC METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CConfigGen::configClose
#
# INVOCATION:
# Object configClose 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# Closes an open configuration file.
#
# DESCRIPTION:
# Trivial.
#
# EXTERNAL VARIABLES:
# CConfigGen::fileId
#
# PRIOR REQUIREMENTS:
# The configuration file is still open.
#
# SEE ALSO:
# CConfigGen::configOpen
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CConfigGen::configClose {
} {
    #
    # Close the file.
    # 

    if { $fileId != "" } {
	close $fileId
	set fileId ""
    }


    #
    # Reset the fileArray information.
    #

    foreach key [ array names fileArray ] {
	unset fileArray($key)
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CConfigGen::configGet
#
# INVOCATION:
# Object configGet key fn
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) key	(string)	Key to look for in the config. file.
# (>) fn	(command)	Tcl command to execute when the key is found.
#
# PROCEDURE VALUE:
# status	(string)	Status of config get, ( SUCCESS, 
#				KEY_NOT_FOUND, or NO_CONFIG_FILE ).
#
# PURPOSE:
# To find all of the lines in the configuration file the match the key.
#
# DESCRIPTION:
# Search the file array (file) for the given keyword.  If the keyword
# is found then call the given function with the number of tokesn and
# the tokens.
#
# EXTERNAL VARIABLES:
# CConfigGen::fileId
# CConfigGen::fileArray
#
# PRIOR REQUIREMENTS:
# CConfigGen::configOpen has been called.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CConfigGen::configGet {
    key
    fn
} {
    if { [ array names fileArray $key ] == "" } {
	return "KEY_NOT_FOUND"
    }

    foreach line $fileArray($key) {
	set numTokens [ llength $line ]
	eval [ subst $fn ] \$numTokens \"\$line\"
    }
    return "SUCCESS"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CConfigGen::configOpen
#
# INVOCATION:
# Object configOpen appName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) appName	(string)	Application name.
#
# PROCEDURE VALUE:
# results	(list) : { status string }
#
# where status is either SUCCESS or ERROR, and
# 	string is either the filename or a description of the error.
#
# PURPOSE:
# To open the configuation file for reading.
#
# DESCRIPTION:
# Try and find the configuration file in the directory specified by
# 	*appname*_CONFIG_DIR (where *appname* is the name of the application)
# If it wasn't found then check for the configuration file in the directory
# specified by 
#	DEFAULT_CONFIG_DIR
# If is still isn't found then check the current working directory.
#
# EXTERNAL VARIABLES:
# CConfigGen::fileId
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# cConfigGen::configClose
# cConfigGen::configOpenFile
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CConfigGen::configOpen {
    appName
} {
    global env

    #
    #  Try to find a configuration file.
    #

    set appVarName [ string toupper $appName ]_CONFIG_DIR
    set fileName "$appName.config"
    set configFileName ""

    if { [ eval info exists env($appVarName) ] } {
	if { [ file exists "$env($appVarName)/$fileName" ] } {
 	    set configFileName $env($appVarName)/$fileName
	}
    }
 
    if { $configFileName == "" && [ info exists env(DEFAULT_CONFIG_DIR) ] } {
	if { [ file exists "$env(DEFAULT_CONFIG_DIR)/$fileName" ] } {
	    set configFileName $env(DEFAULT_CONFIG_DIR)/$fileName
	}
    }
     
    if { $configFileName == "" && [ file exists $fileName ] } {
	set configFileName ./$fileName
    }
    if { $configFileName != {} } {
	set fileId [ open $configFileName ]
	set results [ parseFile ] 
	if { [ lindex $results 0 ] == "ERROR" }  {
	    return [ list $results ]
	} else {
	    return [ list  "SUCCESS" "$configFileName" ]
	}
    } else {
	return { "ERROR" "No configuration file found." }
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CConfigGen::configOpenFile
#
# INVOCATION:
# Object configOpenFile fileName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) fileName	(string)	File to open
#
# PROCEDURE VALUE:
# results	(list) : { status string }
#
# where status is either SUCCESS or ERROR, and
# 	string is either the filename or a description of the error.
#
# PURPOSE:
# To open the configuation file for reading.
#
# DESCRIPTION:
# Open the specified confgiuration file, and use method parseFile to parse it.
#
# EXTERNAL VARIABLES:
# CConfigGen::fileId
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# cConfigGen::configClose
# cConfigGen::configOpen
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CConfigGen::configOpenFile {
    fileName
} {
    set configFileName $fileName

    if { [ catch { set fileId [ open $configFileName ] } ] == 0 } {
	set results [ parseFile ] 
	if { [ lindex $results 0 ] == "ERROR" }  {
	    return [ list $results ]
	} else {
	    return [ list  "SUCCESS" "$configFileName" ]
	}
    } else {
	return [ list  "ERROR" "Failed to open configuration file." ]
    }
}


# ------------------------------------------------------------------
#                         PRIVATE METHODS
# ------------------------------------------------------------------

#
#***********************************************************************
#+
# PROCEDURE NAME:
# CConfigGen::parsefile
#
# INVOCATION:
# parefile 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To read the contents of the configuration file and store it in memory.
#
# DESCRIPTION:
# Read the contents of the file.  Any line that starts with "#" is 
# interpretted as a comment. The first string on the line is the
# "keyword" for the line - the rest of the line are the tokens.  
# All keywords found are strored in an associated array, indexed by
# the keyword.  A line found is turned into a list, so multiple lines
# are stored as a list of lists.
#
# EXTERNAL VARIABLES:
# CConfigGen::fileArray
# CConfigGen::fileId
#
# PRIOR REQUIREMENTS:
# CConfigGen::configOpen has been called.
#
# SEE ALSO:
# None.
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

body	CConfigGen::parseFile {
} {
    while { ! [ eof $fileId ] } {
	#
	# Get a line from the file.
	#

	gets $fileId tmpLine

	
	#
	# interpret anything starting with "#" as a comment.
	# Also skip those blank lines.
	#

	if { [ string index [ string trim $tmpLine ] 0 ] == "#" ||
	     [ string length [ string trim $tmpLine ] ] == 0 } {
	    continue
	}


	#
	# Replace any double quotes in the string with curly braces.
	#

	set index 0
	set line ""

	while { [ set front [ string first \" $tmpLine ] ] != - 1 } {
	    if { [ set back [ string first \" [ string range $tmpLine	\
		    [ expr $front + 1 ] end ] ] ] == -1 } {
		return { "ERROR" "Mismatchin double quotes" }
	    }
		
	    set back [ expr $front + $back + 1 ]
	    set frontStr "[ string range $tmpLine $index [ expr $front - 1 ] ]"
	    set midStr "{[ string range $tmpLine [ expr $front + 1 ] 	\
		    [ expr $back - 1 ] ]} "
	    set line "${line}${frontStr}$midStr"
	    set index [ expr $back + 1 ]
	    set tmpLine [ string range $tmpLine $index end ]
	}
	set line "$line $tmpLine"


	#
	# Remove extra tabs and spaces.
	#

	set newLine ""
	foreach element [ split $line " \t" ] {
	    if { $element != {} } {
		if { $newLine == "" } {
		    set newLine $element
		} else {
		    set newLine "$newLine $element"
		}
	    }
	}


	#
	# Store the value
	#

	set key [ lindex $newLine 0 ]
	if { [ array names fileArray $key ] == "" } {
	    set fileArray($key) [ list $newLine ]
	} else {
	    lappend fileArray($key) $newLine
	}
    }
    return { "SUCCESS" "" }
}


