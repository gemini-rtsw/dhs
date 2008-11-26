#!/usr/local/bin/wish4.2 -f "
# $Id: testCmdr.tcl,v 1.1.1.1 2002-11-24 20:36:01 brighton Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) <1998>				(c) <1998>
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
# testCmdr.tcl
#
# PURPOSE:
# To run input scripts that will execute command and confirm tests
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# argInit		Read in command line arguments
# commandExecution	Execute the command
# commandExecutionOk	Process an ok execution of the command
# configOpen		Open a configuration file
# debugString		Print if in debug mode
# end			exit routine
# isFalse		checks input to see if false was returned
# isTrue		checks input to see if true was returned
# handleOutput		Handle the OUTPUT line 
# main			Main routine
# printHeader		Print header information
# printStartup		Print startup information
# printSummary		Print final summary information
# processLine		Process a line from the input file
# queryYesNo		Ask a yes/no question
# readLineIn		Read in a line from the input file
# readLn		Read in a line, different
# resetVariables	Reset variables at the end/beginning of a test
# start			Do initial startup stuff
# testResult		Test the result and save the pass or fail status
# usage			Print usage message
# variableSubstitution	Substitue variables in the command to be executed
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:45  cvs-tuc
# Initial install into CVS of dhs-0.16
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#
# ------------------------------------------------------------------
#                        Functions
# ------------------------------------------------------------------


#
#***********************************************************************
#+
# PROCEDURE NAME:
# argInit
#
# INVOCATION:
# argInit 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) numArgs	: argc
# (>) inputArgs : argv
# None.
#
# PROCEDURE VALUE:
# "Failed"	: If a problem occured.
# "Succeeded"	: If no problems occured.
#
# PURPOSE:
# To read the command line and set the appropriate variables
#
# DESCRIPTION:
# To read the command line and set the appropriate variables.
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
proc argInit {
    numArgs
    inputArgs
} {
    global tcmModes
    global inputFileName assumpFileName
    global tcmVars

    #
    #
    #  Check command line arguments and set
    #  assumptions file name
    #
    for { set i 0 } { $i < $numArgs } { incr i +1 } \
    {
	set argumentList [ lindex $inputArgs $i ]
	case $argumentList in \
	  -debug {
	    #
	    #  Debug mode has been set
	    #
	    set tcmModes(debugMode) 1

	} -simulate {
	    #
	    #  Simulate mode has been set
	    #

	    set tcmModes(simulateMode) 1
	} -a {
	    #
	    #  Assumption file to print out
	    #

	    if { [ expr {$i + 1} ] >= $numArgs } \
	    {
		usage
		end
	    }
	    incr i +1
	    set assumpFileName [ lindex $inputArgs $i ] 
	} -gui {
	    #
	    #  Put output to a gui window.
	    #

	    set tcmVars(gui) 1
	} -skip {
	    #
	    #  Skip all tests except the line listed.
	    #

	    if { [ expr {$i + 1} ] >= $numArgs } \
	    {
		usage
		end
	    }
	    incr i +1
	    set tcmVars(TestName) [ lindex $inputArgs $i ] 
	} -i {
	    #
	    #  Input file name
	    #

	    if { [expr {$i + 1}] >= $numArgs } \
	    {
		usage
		end
	    }
	    incr i +1
	    set inputFileName [ lindex $inputArgs $i ] 
	} -auto {
	    #
	    #  Non-interactive, answer all prompts with yes.
	    #

	    set tcmModes(auto) 1
	} default {
	    usage
	    end
	}
    }

    #
    #  Check that at least the input file name was given.
    #

    if { ![ info exists inputFileName ] } \
    {
	usage
	end
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# commandExecution
#
# INVOCATION:
# commandExecution commandString
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) logFd (File pointer) Pointer to log file.
# (>) outString (string) Expected output of command to execute.
#
# PROCEDURE VALUE:
# -1 : If the command could not be executed, or output did not match.
# 1  : Command completed, with results checked.
#
# PURPOSE:
# To execute the command
#
# DESCRIPTION:
# To execute the command and compare the results.
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

proc    commandExecution { 
    logFd
    outString 
} {

    #
    #  Declare usage of global variables
    #
    global commandString
    global tcmVars

    set match 0
    set error 0

    if { $tcmVars(cmdType) == "CMD" } \
    { 
	regsub -all {\"} $commandString "\\\"" tmpCommand
	regsub -all {\[} $tmpCommand "\\\[" tmpCommand
	regsub -all {\]} $tmpCommand "\\\]" tmpCommand
	regsub -all {\}} $tmpCommand "\\\}" tmpCommand
	regsub -all {\{} $tmpCommand "\\\{" tmpCommand
	if [ catch {open "|csh -fc \"$tmpCommand\" |& cat" r} inputx ] \
	{ 
	    puts $logFd "ERROR, Cannot run cmd: $commandString"
	    testResult 0
	    return -1
	} \
	else \
	{
	    return [ commandExecutionOk $logFd $outString 1 $inputx ]
	}
    } \
    elseif { $tcmVars(cmdType) == "TCLCMD" } \
    {
	if [ catch {eval $commandString} msg ] \
	{
	    regsub -all {\"} $commandString "\\\"" tmpCommand
	    regsub -all {\[} $tmpCommand "\\\[" tmpCommand
	    regsub -all {\]} $tmpCommand "\\\]" tmpCommand
	    regsub -all {\}} $tmpCommand "\\\}" tmpCommand
	    regsub -all {\{} $tmpCommand "\\\{" tmpCommand
	    if [ catch {open "|csh -fc \"$tmpCommand\" |& cat" r} inputx ] \
	    {
		puts $logFd "ERROR, Cannot run cmd: $commandString"
		puts $logFd "\t$msg"
		testResult 0
		return -1
	    } \
	    else \
	    {
		return [ commandExecutionOk $logFd $outString 1 $inputx ]
	    }
	} else {

	    return [ commandExecutionOk $logFd $outString 2 $msg ]
	}
    } \
    elseif { $tcmVars(cmdType) == "GETVAL" } \
    {
	regsub -all {\"} $commandString "\\\"" tmpCommand
	regsub -all {\[} $tmpCommand "\\\[" tmpCommand
	regsub -all {\]} $tmpCommand "\\\]" tmpCommand
	regsub -all {\}} $tmpCommand "\\\}" tmpCommand
	regsub -all {\{} $tmpCommand "\\\{" tmpCommand
	if [ catch {open "|csh -fc \"caget $tmpCommand\" |& cat" r} inputx ] \
	{ 
	    puts $logFd "ERROR, Cannot caget : $commandString"
	    testResult 0
	    return -1
	} \
	else \
	{
	    return [ commandExecutionOk $logFd $outString 1 $inputx ]
	}
    } \
    elseif { $tcmVars(cmdType) == "PUTVAL" } \
    {
	regsub -all {\"} $commandString "\\\"" tmpCommand
	regsub -all {\[} $tmpCommand "\\\[" tmpCommand
	regsub -all {\]} $tmpCommand "\\\]" tmpCommand
	regsub -all {\}} $tmpCommand "\\\}" tmpCommand
	regsub -all {\{} $tmpCommand "\\\{" tmpCommand
	if [ catch {open "|csh -fc \"caput $tmpCommand\" |& cat" r} inputx ] \
	{ 
	    puts $logFd "ERROR, Cannot caput : $commandString"
	    testResult 0
	    return -1
	} \
	else \
	{
	    return [ commandExecutionOk $logFd $outString 1 $inputx ]
	}
    }
    return 1
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# commandExecutionOk
#
# INVOCATION:
# commandExecutionOk commandString
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) logFd (File pointer) Pointer to log file.
# (>) outString (string) Expected output of command to execute.
#
# PROCEDURE VALUE:
# -1 : If the command could not be executed, or output did not match.
# 1  : Command completed, with results checked.
#
# PURPOSE:
# To execute the command
#
# DESCRIPTION:
# To execute the command and compare the results.
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

proc    commandExecutionOk { 
    logFd
    outString
    execType
    resultStream
} {

    #
    #  Declare usage of global variables
    #

    global commandString
    global tcmVars

    if { $execType == 1 } \
    {
	 set contents [split [read $resultStream] \n]
    } \
    else \
    {
	    set contents [split $resultStream \n]
    }
    debugString "OUTPUT to Command = $contents"
    puts $logFd "OUTPUT :"
    foreach line $contents \
    {
	puts $logFd "$line"
    }
    catch {close $resultStream}


    if { [string match $outString $contents ] } \
    {
	puts $logFd "SUCCESS, found output String: $outString"
	debugString "Success, found outputstring = $outString"
	testResult 1
    } \
    else \
    {
	puts $logFd "FAILED to find output String: $outString"
	debugString "Failed, did not find outputstring = $outString"
	testResult 0
	return -1
    }

    return 1
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# configOpen
#
# INVOCATION:
# configOpen appName fileName
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) appName	(string)	Application name.
# (>) fileName	(fileName)	Name of the configuration file.
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
# To open the configuation file for reading.
#
# EXTERNAL VARIABLES:
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#
proc configOpen {
    appName
} {
    global env
    global configFileName

    #
    #  Try to find a configuration file.
    #

    set appVarName [ string toupper $appName ]_CONFIG_DIR
    set fileName "$appName.config"
    set configFileName ""

    if { [ eval info exists env($appVarName) ] } \
    {
	if { [ file exists "$env($appVarName)/$fileName" ] } \
	{
	    set configFileName $env($appVarName)/$fileName
	}
    }

    if { $configFileName == "" && [ info exists env(DEFAULT_CONFIG_DIR) ] } \
    {
	if { [ file exists "$env(DEFAULT_CONFIG_DIR)/$fileName" ] } \
	{
	    set configFileName $env(DEFAULT_CONFIG_DIR)/$fileName
	}
    }
    
    if { $configFileName == "" && [ file exists $fileName ] } \
    {
	set configFileName ./$fileName
    }
    if { $configFileName != {} } \
    {
	return 1
    } \
    else \
    {
	puts "ERROR, No configuration file found."
	return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# debugString
#
# INVOCATION:
# debugString
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) str (string)	String to print
#
# PROCEDURE VALUE:
#
# PURPOSE:
# To print the debug string if running in debug mode
#
# DESCRIPTION:
# To print the debug string if running in debug mode.
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

proc    debugString { 
    str
} {
    global tcmModes

    if { [ info exists tcmModes(debugMode) ] } \
    {
	writeScreen $str
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# end
#
# INVOCATION:
# end 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To print exit and exit
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

proc end {} {
    puts "exiting"
    exit
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# handleOutput
#
# INVOCATION:
# handleOutput keyword restLine
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) resetLine (string) Command to act on.
# (>) logFd (File pointer) Pointer to log file.
#
# PROCEDURE VALUE:
# 0  : Processing of the line did not complete correctly.
# 1  : Processing of the line completed correctly.
#
# PURPOSE:
# To execute the function of the command
#
# DESCRIPTION:
# To execute the function of the command.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    handleOutput { 
    logFd
    restLine 
} {

#
#  Declare usage of global variables
#

    global tcmVars
    global tcmModes
    global commandString


    # Execute the last command received.
    # Determine if the test completed successfully.
    #
    if { ! [ info exists commandString ] } \
    {
	#
	#  Command doesn't exist to execute it, print an error
	#  and go till you find the word TEST
	#

	writeScreen "***Error in input file, got OUTPUT before CMD***"
	writeScreen "***skipping to next test***"
	puts $logFd "***Error in input file, got OUTPUT ***"
	puts  $logFd "***before CMD, skipping to next test ***** "
	set tcmVars(skipTest) 1
    } \
    else \
    {
	#
	#  Execute the command, if simulateMode is not set
	#

	set commandString [variableSubstitution $commandString]
	puts $logFd "--------------------------"
	puts $logFd "COMMAND:	$commandString"
	debugString "Command to Execute = $commandString"
	set tcmVars(numCmds) 0
	if { ! [ info exists tcmModes(simulateMode) ] } \
	{
	    if { [ commandExecution $logFd $restLine] == -1 } \
	    {
		if { $tcmVars(fatalError) } \
		{
		    incr tcmVars(totalTestsRun) 1
		    debugString "Total tests run = $tcmVars(totalTestsRun)"
		    puts  $logFd "*** Fatal error, skipping remainder of tests ***** "
		    writeScreen "*** Fatal error executing:          *****"
		    writeScreen "***   $commandString ****"
		    writeScreen  "*** Skipping remainder of tests ***** "
		    return 0	
		} \
		elseif { $tcmVars(stopOnError) } \
		{
		    puts  $logFd "*** Skipping to next test ***** "
		    writeScreen  "*** Skipping to next test ***** "
		    set tcmVars(skipTest) 1
		}
	    }
	} \
	else \
	{
	    debugString "Not executing, in simulate mode"
	}
    }
    return 1
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# isFalse
#
# INVOCATION:
# isFalse boolValue
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) boolValue (boolean)	Value that is to be checked for falseness.
#
# PROCEDURE VALUE:
# 1 : If the input is one of "n", "no", "f", "false", "off", or "0" 
#     (case not checked)
# 0 : Otherwise
#
# PURPOSE:
# To detemine if a value is "false".
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
# isTrue
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    isFalse { 
    bool

} {
    set bool [ string trim $bool ]
    if { [ regexp -nocase -- (^f$)|(^false$)|(^0$)|(^n$)|(^no$)|(^off$)	\
	       $bool ] } \
    {
        return 1
    } \
    else \
    {
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
# isTrue boolValue
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) boolValue (boolean)	Value that is to be checked for falseness.
#
# PROCEDURE VALUE:
# 1 : If the input is one of "y", "yes", "t", "true", "on" or "1"
#	 (case not checked)
# 0 : Otherwise
#
# PURPOSE:
# To detemine if a value is "true".
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
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    isTrue { 
    bool
} {
    set bool [ string trim $bool ]
    if { [ regexp -nocase -- (^t$)|(^true$)|(^1$)|(^y$)|(^yes$)|(^on$)	\
	       $bool ] } \
    {
        return 1
    } \
    else \
    {
        return 0
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# printHeader
#
# INVOCATION:
# printHeader
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) logFd (File pointer) Pointer to log file.
#
# PROCEDURE VALUE:
#
# PURPOSE:
# To print out final results
#
# DESCRIPTION:
# To print out final results.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    printHeader { 
    logFd
} {

    set datestr [ exec date ]
    writeScreen "**************************************************************"
    writeScreen "**  Starting Tests: $datestr                  **"
    writeScreen "**************************************************************"


    puts $logFd "**************************************************************"
    puts $logFd "**  Starting Tests: $datestr                  **"
    puts $logFd "**************************************************************"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# printStartup
#
# INVOCATION:
# printStartup 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) logFd (File pointer) Pointer to log file.
#
# PROCEDURE VALUE:
# None
#
# PURPOSE:
# To print startup stuff
#
# DESCRIPTION:
# Print startup stuff, like variable substitution, assumption file,
# simulation mode warning, disclaimer.
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

proc printStartup { \
    logFd
} {

    #
    #  Declare globals
    #

    global tcmModes
    global varArray
    global assumpFileName


    #
    #  In debug mode, print off the variables for substitution.
    #

    if { [ info exists tcmModes(debugMode) ] } \
    {
	foreach key [ array names varArray ] \
	{
	    debugString "VariableName is : $key, Value is : $varArray($key) "
	}
    }


    #
    # Print assumptions file, if it exists.
    #

    if { [ info exists assumpFileName ] } \
    {
	if { [ file isfile $assumpFileName ] } \
	{
	    set assumpFd [ open $assumpFileName r ]
	    writeScreen " "
	    debugString "Printing assumptions file::::"
	    while { ! [ eof $assumpFd ] } \
	    {
		if { [ gets $assumpFd ln ] == -1 } \
		{
		    break;
		}
		writeScreen $ln
		puts $logFd $ln
	    }
	    writeScreen " "
	    puts $logFd " "
	}
    }


    #
    #  Print warning if in simulate mode.
    #

    if { [ info exists tcmModes(simulateMode) ] } \
    {
	writeScreen "\nWARNING, running in simulateMode, no commands executed!!\n"
	puts $logFd "\nWARNING, running in simulateMode, no commands executed!!\n"
    }


    #
    # Print Disclaimer
    #

    writeScreen " "
    writeScreen "****************************************************************"
    writeScreen "**  WARNING, this is a destructive script.  Only continue if  **"
    writeScreen "**  you don't mind losing everything.  This includes over-    **"
    writeScreen "**  writing your log file.                                    **"
    writeScreen "****************************************************************"

}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# printSummary
#
# INVOCATION:
# printSummary
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) logFd (File pointer) Pointer to log file.
#
# PROCEDURE VALUE:
#
# PURPOSE:
# To print out final results
#
# DESCRIPTION:
# To print out final results.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    printSummary { 
    logFd
} {
    global tcmVars

    #
    #  Now print to gui, and always to stdout, and to the log file.
    #


    set datestr [ exec date ]
    writeScreen "**************************************************************"
    writeScreen "**  Finished Tests: $datestr            **"
    writeScreen "**                                                          **"
    writeScreen "**  Test Results:                                           **"
    writeScreen "**                                                          **"
    writeScreen [ format "**  Total Tests Run    = %-3s                                **" $tcmVars(totalTestsRun) ]
    writeScreen [ format "**  Total Tests Passed = %-3s                                **" $tcmVars(numPassTests) ]
    writeScreen [ format "**  Total Tests Failed = %-3s                                **" [expr $tcmVars(totalTestsRun)-$tcmVars(numPassTests)] ]
    writeScreen "**                                                          **"
    writeScreen "**  Command Execution Results:                              **"
    writeScreen "**                                                          **"
    writeScreen [ format "**  Total Number of Commands Executed      = %-3s            **" $tcmVars(totalCmdsRun) ]
    writeScreen [ format "**  Total Number of Commands that Passed   = %-3s            **" $tcmVars(numPassCmds) ]
    writeScreen [ format "**  Total Number of Commands that Failed   = %-3s            **" [expr $tcmVars(totalCmdsRun)-$tcmVars(numPassCmds)] ]
    writeScreen "**************************************************************"

    if {  $tcmVars(gui) == 1 } \
    {
	puts "**************************************************************"
	puts "**  Finished Tests: $datestr            **"
	puts "**                                                          **"
	puts "**  Test Results:                                           **"
	puts "**                                                          **"
	puts [ format "**  Total Tests Run    = %-3s                                **" $tcmVars(totalTestsRun) ]
	puts [ format "**  Total Tests Passed = %-3s                                **" $tcmVars(numPassTests) ]
	puts [ format "**  Total Tests Failed = %-3s                                **" [expr $tcmVars(totalTestsRun)-$tcmVars(numPassTests)] ]
	puts "**                                                          **"
	puts "**  Command Execution Results:                              **"
	puts "**                                                          **"
	puts [ format "**  Total Number of Commands Executed      = %-3s            **" $tcmVars(totalCmdsRun) ]
	puts [ format "**  Total Number of Commands that Passed   = %-3s            **" $tcmVars(numPassCmds) ]
	puts [ format "**  Total Number of Commands that Failed   = %-3s            **" [expr $tcmVars(totalCmdsRun)-$tcmVars(numPassCmds)] ]
	puts "**************************************************************"
    }

    puts $logFd "**************************************************************"
    puts $logFd "**  Finished Tests: $datestr            **"
    puts $logFd "**                                                          **"
    puts $logFd "**  Test Results:                                           **"
    puts $logFd "**                                                          **"
    puts $logFd [ format "**  Total Tests Run    = %-3s                                **" $tcmVars(totalTestsRun) ]
    puts $logFd [ format "**  Total Tests Passed = %-3s                                **" $tcmVars(numPassTests) ]
    puts $logFd [ format "**  Total Tests Failed = %-3s                                **" [expr $tcmVars(totalTestsRun)-$tcmVars(numPassTests)] ]
    puts $logFd "**                                                          **"
    puts $logFd "**  Command Execution Results:                              **"
    puts $logFd "**                                                          **"
    puts $logFd [ format "**  Total Number of Commands Executed      = %-3s            **" $tcmVars(totalCmdsRun) ]
    puts $logFd [ format "**  Total Number of Commands that Passed   = %-3s            **" $tcmVars(numPassCmds) ]
    puts $logFd [ format "**  Total Number of Commands that Failed   = %-3s            **" [expr $tcmVars(totalCmdsRun)-$tcmVars(numPassCmds)] ]
    puts $logFd "**************************************************************"
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# processLine
#
# INVOCATION:
# processLine keyword restLine
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) keyword (string) Keyword from the input file.
# (>) resetLine (string) Command to act on.
# (>) logFd (File pointer) Pointer to log file.
#
# PROCEDURE VALUE:
# 0  : Processing of the line did not complete correctly.
# 1  : Processing of the line completed correctly.
#
# PURPOSE:
# To execute the function of the command
#
# DESCRIPTION:
# To execute the function of the command.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    processLine { 
    keyword 
    restLine 
    logFd
} {

#
#  Declare usage of global variables
#

    global tcmVars
    global tcmModes
    global commandString


    debugString "Command = $keyword"

    #
    #  Check to see if we are skipping this test.
    #

    if { $tcmVars(skipTest) == 1 && $keyword != "TEST" } \
    {
	return 1
    }


    case $keyword in \
    CMD {
	#
	#  Got the command, save it.
	#
	
	if { $tcmVars(numCmds) != 0 } \
	{
	    #
	    #  The last command line did not have an output line and
	    #  will not get executed, print warning.
	    #

	    writeScreen "WARNING, last command did not have an OUTPUT line,"
	    writeScreen "  NOT executing: $commandString "
	    puts $logFd "WARNING, last command did not have an OUTPUT line,"
	    puts $logFd "  NOT executing: $commandString "
	}
	set tcmVars(numCmds) 1
	set commandString $restLine
	set tcmVars(cmdType) CMD	

    } TCLCMD {
	#
	#  Got a dhs command, save it.
	#
	set tcmVars(cmdType) $keyword	
	set commandString $restLine

    } STOP {
	#
	# Can NOT continue with rest of tests if there is an error.
	#

	set tcmVars(stopOnError) 1
    } ENDSTOP {
	#
	# Unset the stop on error.
	#

	set tcmVars(stopOnError) 0
    } FATAL {
	#
	# Can NOT continue with any more tests if there if an error in these
	# tests.
	#

	set tcmVars(fatalError) 1
    } ENDFATAL {
	#
	# Can continue with more tests if there is an error in these tests.
	#

	set tcmVars(fatalError) 0
    } CONT {
	#
	# Can continue with next test if there is an error.
	#

    } SKIP {
	#
	# Skip to the next test.
	#

	set tcmVars(skipTest) 1
    } PROMPT {
	#
	# Ask the question given and get the response.
	#

	set promptString [variableSubstitution $restLine]
	puts $logFd "PROMPT:	[ subst $promptString ]"
	if { [ queryYesNo "[ subst $promptString ]" ] } \
	{
	    testResult 1
	} \
	else \
	{
	    testResult 0
	    if { $tcmVars(fatalError) } \
	    {
		incr tcmVars(totalTestsRun) 1
		debugString "Total tests run = $tcmVars(totalTestsRun)"
		puts  $logFd "*** Fatal error, skipping remainder of tests ***** "
		writeScreen  "*** Skipping remainder of tests ***** "
		return 0	
	    } \
	    elseif { $tcmVars(stopOnError) } \
	    {
		resetVariables
		puts  $logFd "*** Skipping to next test ***** "
		writeScreen  "*** Skipping to next test ***** "
		set tcmVars(skipTest) 1
	    }
	}
	flush stdout

    } COMMENT {
	#
	# Print out the command, and just get a return.
	#

	if { ![ info exists tcmModes(auto) ] } \
	{
	    set commentString [variableSubstitution $restLine]
	    if {  $tcmVars(gui) == 1 } \
	    {
		set tmpResult [tk_messageBox -type ok -default ok\
		    -message "[ subst $commentString ]" \
		    -title "Please Note" \
		    -icon info ]
		
	    } \
	    else \
	    {
		puts -nonewline "[ subst $commentString ] (press enter)"
		flush stdout
		gets stdin tmpResult
	    }
	}
	puts $logFd "COMMENT:	[ subst $restLine ]"
	flush stdout

    } DESC {
	#
	# Got a test description, put to log file. 
	#
	
	puts $logFd "DESCRIPTION:	[ subst $restLine ]"

    } GETVAL {
	#
	#  Get a value, save the name to get.
	#
	
	if { $tcmVars(numCmds) != 0 } \
	{
	    #
	    #  The last command line did not have an output line and
	    #  will not get executed, print warning.
	    #

	    writeScreen "WARNING, last command did not have an OUTPUT line,"
	    writeScreen "  NOT executing: $commandString "
	    puts $logFd "WARNING, last command did not have an OUTPUT line,"
	    puts $logFd "  NOT executing: $commandString "
	}
	set tcmVars(numCmds) 1
	set commandString $restLine
	set tcmVars(cmdType) GETVAL

    } PUTVAL {
	#
	#  Put a value, save the name to put.
	#
	
	if { $tcmVars(numCmds) != 0 } \
	{
	    #
	    #  The last command line did not have an output line and
	    #  will not get executed, print warning.
	    #

	    writeScreen "WARNING, last command did not have an OUTPUT line,"
	    writeScreen "  NOT executing: $commandString "
	    puts $logFd "WARNING, last command did not have an OUTPUT line,"
	    puts $logFd "  NOT executing: $commandString "
	}
	set tcmVars(numCmds) 1
	set commandString $restLine
	set tcmVars(cmdType) PUTVAL

    } OUTVAL {
	#
	#  Compare a value
	#


    } ENDTEST {
	#
	#  Got an end test, clear all variables associated with this test.
	#

	if { [ info exists commandString ] } \
	{
	    unset commandString
	}

	resetVariables
	puts $logFd "*******************END TEST*******************************"

    } OUTPUT {
	# Execute the last command received.
	# Determine if the test completed successfully.
	#
	set tmp [ handleOutput $logFd $restLine ]
	if { $tmp == 1 && $tcmVars(skipTest) == 1 } \
	{
	    resetVariables
	    set tcmVars(skipTest) 1
	}
	return $tmp

    } TEST {
	#
	#  Got a new test, print it to the log file, set variables
	#
	if { [ info exists commandString ] } \
	{
	    unset commandString
	}
	resetVariables

	puts $logFd "**************************************************"
	puts $logFd "TEST:		$restLine"
	writeScreen "**************************************************"
	writeScreen "TEST:		$restLine"


	#
    } default {

	#
	#  Ignore line
	#
	puts $logFd "Input file error, got unrecognized cmd: $keyword $restLine"
	puts $logFd "Exitting ... "
	writeScreen "Input file error, got unrecognized cmd: $keyword $restLine"
	return 0
    }
    return 1
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# queryYesNo
#
# INVOCATION:
# query $msg
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) msg (string) Message to display
#
# PROCEDURE VALUE:
# (boolean) 0: If a false value is given to the query.
# (boolean) 1: if a true value is given to the query.
#
# PURPOSE:
# To query the user for a yes/no answer
#
# DESCRIPTION:
# To query the user for a yes/no answer.  If in automatic mode, do not
# get input, assume answer is yes.
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

proc	queryYesNo {
    msg		    
} {

    #
    #  Declare globals
    #

    global tcmModes
    global tcmVars


    #
    #  If running in auto mode, just assume answer is yes.
    #

    if { [ info exists tcmModes(auto) ] }\
    {
	set result t
    } \
    else \
    {
	if {  $tcmVars(gui) == 1 } \
	{
	    set result [tk_messageBox -type yesno -default yes\
		-message $msg \
		-title "Question" \
		-icon question ]
	    
	} \
	else \
	{

	    puts -nonewline "$msg (yes/no)? "
	    flush stdout
	    gets stdin result
	}
    }

    while { ![ isFalse $result ] && ! [ isTrue $result ] } \
    {
	if  {  $tcmVars(gui) == 1 } \
	{
	    set result [tk_messageBox -type yesno -default yes\
		-message "Didn't work? $msg"\
		-icon question ]
	} \
	else \
	{
	    puts "Invalid value `$result': must be a boolean value."
	    puts -nonewline "$msg (yes/no)? "
	    flush stdout
	    gets stdin result
	}
    }

    if { [ isTrue $result ] } {
	return 1
    }

    if { [ isFalse $result ] } {
	return 0
    }

    return 0
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# readLineIn
#
# INVOCATION:
# readLineIn 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
#				changing the value back.
# None.
#
# PROCEDURE VALUE:
#
# PURPOSE:
# To read a line from the input file
#
# DESCRIPTION:
# To read a line from the input file, ignore comment lines.
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
proc readLineIn {
} {
#
#  Declare globals
#

    global inFd
    global returnedLine


    #
    # Get a line from the file.
    #

    if { [ gets $inFd ln ] == -1 } \
    {
	return -1
    }


    #
    # interpret anything starting with "#" as a comment.
    # Also skip those blank lines.
    #

    if { [ string index [ string trim $ln ] 0 ] == "#" ||
	 [ string length [ string trim $ln ] ] == 0 } \
    {
	return 2
    }


    #
    # Remove extra tabs and spaces.
    #

    set newLine ""
    set index 0

    foreach element $ln \
    {
	lappend newLine $element
    }
    set returnedLine $newLine
    return 1
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# readLn
#
# INVOCATION:
# readLn
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) boolValue (boolean)	Value that is to be checked for falseness.
#
# PROCEDURE VALUE:
# 1 : If the input is one of "y", "yes", "t", "true", "on" or "1"
#	 (case not checked)
# 0 : Otherwise
#
# PURPOSE:
# To detemine if a value is "true".
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
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc readln {} \
{
    if { [ gets $inFd ln ] == -1 } \
    {
	end
    }
    return $ln
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# resetVariables
#
# INVOCATION:
# resetVariables
#
# PARAMETERS: (">" input, "!" modified, "<" output)
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To reset variables for a new test
#
# DESCRIPTION:
# To reset variables for a new test.
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

proc	resetVariables {
} {

    #
    #  Declare globals
    #

    global tcmVars


    set tcmVars(stopOnError) 0
    set tcmVars(fatalError) 0
    set tcmVars(skipTest) 0
    set tcmVars(cmdType) CMD	
    set tcmVars(numCmds) 0

    #
    #  Only increment total tests run if something was run for this group.
    #

    if { $tcmVars(groupTestStatus) == "P" } \
    {
	#
	#  Only increment group pass if status has been set to P
	#

	incr tcmVars(totalTestsRun) 1
	incr tcmVars(numPassTests) 1
	set tcmVars(groupTestStatus) "X"
    } elseif { $tcmVars(groupTestStatus) == "F" } \
    {
	incr tcmVars(totalTestsRun) 1
	set tcmVars(groupTestStatus) "X"
    }
    debugString "Total tests run = $tcmVars(totalTestsRun)"

}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# setWindow
#
# INVOCATION:
# setWindow 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None.
#
# PROCEDURE VALUE:
# "Failed"	: If a problem occured.
# "Succeeded"	: If no problems occured.
#
# PURPOSE:
# To setup the test window
#
# DESCRIPTION:
# To setup the test window.
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
proc setWindow {
} {


    global	tcmVars
    global 	inputFileName

    if { $tcmVars(gui) == 0 } \
    {
	return
    }

    #
    # Inform window manager
    # 

    wm title . "OCS Simulator"
    wm minsize . 50 50
    wm iconname . "Simulator"
    debugString "Creating window"


    #
    # Set the background colour
    #

    . configure -bg PeachPuff


    #
    # Create a menubar with Only file on it.
    # -relief (means 3-d), -bd (borderwidth on 3-d), -bg (background)

    frame .mbar -relief raised -bd 2 -bg PeachPuff 
    frame .outer -bg Purple 
    pack .mbar .outer -side top -fill x


    #
    # Add the menu buttons
    #

    menubutton .mbar.file -text File -underline 0 -menu .mbar.file.menu \
	 -bg PeachPuff
    pack .mbar.file -side left


    #
    # EXIT
    #
    menu .mbar.file.menu -bg PeachPuff
    .mbar.file.menu add command -label "Exit" -command exit

    menubutton .mbar.help -text Help -underline 0 -menu .mbar.help.menu \
         -bg PeachPuff
    pack .mbar.help -side right

    
    #
    # START - NO, DON't NEED IT
    # menu .mbar.help.menu -bg peachPuff
    # .mbar.help.menu add command -label "Start" \
    # -command {tt_dialog .d tcsTests PeachPuff \
    # {Hi there. \
    #  Lets enter an input file name?. } \
    #  {} -1 OK}
    # pack .mbar.help -side left


    #
    # Create a text widget listing output from the commands runs.
    #

    frame .scrollFrame -bg PeachPuff
    text .testText -height 20 -width 80 -relief sunken -bd 2 \
		-bg PaleVioletRed -fg Black -yscrollcommand ".scroll set"
    scrollbar .scroll -command ".testText yview"

    pack .scroll -side right -fill y -in .scrollFrame
    pack .testText -side left -in .scrollFrame
    pack .scrollFrame

    .testText configure -state disabled


    #
    # Create the widgets for the input and output filenames
    #

    pack [frame .inFile -bg PeachPuff] -fill x -expand y
    pack [label .inFile.label -text "Input file:    " -bg PeachPuff \
	-fg NavyBlue] -side left
    pack [label .inFile.name -text $inputFileName -relief sunken \
	-bg PeachPuff] \
       -fill x -expand y
    # bind .inFile.entry <Return> { }

    pack [frame .outFile -bg PeachPuff] -fill x -expand y
    pack [label .outFile.label -text "Log file:      " -bg PeachPuff \
	-fg NavyBlue] \
       -side left
    pack [label .outFile.name -text "$inputFileName.log" \
	-relief sunken -bg PeachPuff] \
       -fill x -expand y


    #
    # Make the window show up.
    #

    wm deiconify .
    tkwait visibility .
#    tkwait visilibity .testText
    tkwait visibility .outFile.name
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# start
#
# INVOCATION:
# start 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
#				changing the value back.
#  None.
#
# PROCEDURE VALUE:
# 0		: Error opening and intializing.
# 1		: Everything started ok.
#
# PURPOSE:
# To open and initialize all files and start the services. 
#
# DESCRIPTION:
# To open and initialize all files and start the services.
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
proc start {
} {
#
# Declare globals and initialize them.
#
    global inFd
    global tcmVars
    global lastLine
    global configFileName
    global inputFileName
    global returnedLine
    global varArray


    #
    #  Get the machine name
    #

    if [ catch {exec uname -n} tcmVars(machineName) ] \
    {
	puts "Error getting machineName, exitting"
	return 0
    }


    #
    #  Open the input file for reading and read
    #  in the variable substitution list and get 
    #  the values from the config file listed.
    #

    if [ catch {open $inputFileName r} inFd ] \
    {
	puts "Error opening input file - $inputFileName"
	return 0
    } \
    else \
    {
	#
	#  Read in all VAR's from top of file.
	#

	puts "\nConfiguration variables read are: "
	while { ! [ eof $inFd ] } \
	{
	    #
	    # Get a line from the file.
	    #

	    if { [ set x [ readLineIn ] ] == -1 } \
	    {
		break
	    } \
	    elseif { $x == 2 } \
	    {
		continue
	    }

	    #
	    #  Check that first word is VAR, and there are 5 variables
	    #  on that line: VAR  configName  variableName  variablePosition
	    #

	    if { [ lindex $returnedLine 0 ] != "VAR" } \
	    {
		debugString "No more variables to read, returning..."
		set lastLine $returnedLine
		return 1
	    } \
	    elseif { [ llength $returnedLine ] != 5  } \
	    {
		puts "ERROR, Input file's VAR line must have 5 variables in it"
		puts "   VAR ConfigFile SearchItem VariableName Position "
		return 0
	    }
		
	    set configName [ lindex $returnedLine 1 ]
	    set searchItem [ lindex $returnedLine 2 ]
	    set varName [ lindex $returnedLine 3 ]
	    set varPos [ lindex $returnedLine 4 ]
	    debugString "\tConfigName = $configName, Varible Name=$varName, Position=$varPos"


	    #
	    #  Get the variable values from the config file
	    #

	    if { [ configOpen $configName ] != 1 } \
	    {
		puts "Could not get value $varName from $configName's config file, exiting"
		return 0
	    } \
	    else \
	    {
		#
		#  Get the variable required
		#

		if { [ catch {exec cat $configFileName |  \
			grep "^$searchItem\[ \t\$\]"} varValue ] || $varValue == "" } \
		{
		    puts "Could not get variable $searchItem from position $varPos out of $configFileName, exiting"
		    return 0
		} \
		else \
		{
		    set line [ lindex [ split $varValue "\n" ] 0 ]
		}

		if { [set varValue [ lindex $line $varPos ] ] == " " } \
		{
		    puts "Could not get variable $varName from position $varPos out of $configFileName, exiting"
		    return 0
		}

		#
		# Set up a list with the variable name and value in it.
		#
		puts "\tVariable name = $varName, Value = $varValue "
		if { [ array names varArray $varName ] == "" } \
		{
		    #
		    #  This is the first element in the array
		    set varArray($varName) $varValue
		} \
		else \
		{
		    #
		    #  Append array.
		    lappend varArray($varName) $varValue
		}
	    } 
	}
    }

}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# testResult
#
# INVOCATION:
# testResult $result
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) result (bool) Result to test.  True means it passed.
#
# PROCEDURE VALUE:
#
# PURPOSE:
# To print results and increment counters
#
# DESCRIPTION:
# To print results and increment counters.
#
# EXTERNAL VARIABLES:
# None.
#
# PRIOR REQUIREMENTS:
# None.
#
# SEE ALSO:
# isFalse
#
# DEFICIENCIES:
# None.
#-
#***********************************************************************
#

proc    testResult { 
    result 
} {
    global tcmVars
    global logFd
    global tcmModes


    if { ! [ info exists tcmModes(simulateMode) ] }\
    {
	if { $result } \
	{
	    incr tcmVars(numPassCmds) 1
	    if { $tcmVars(groupTestStatus) == "X" } \
	    {
		set tcmVars(groupTestStatus) "P"
	    }	    
	    puts $logFd "RESULT :	PASSED"
	    writeScreen "RESULT:	PASSED"
	} \
	else \
	{
	    set tcmVars(groupTestStatus) "F"
	    puts $logFd "RESULT :	FAILED"
	    writeScreen "RESULT:	FAILED"
	}
	incr tcmVars(totalCmdsRun) 1
	debugString "Total commands run = $tcmVars(totalCmdsRun)"
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# usage
#
# INVOCATION:
# usage 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# None
#
# PROCEDURE VALUE:
#
# PURPOSE:
# To check that the usage is correct
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

proc	usage {
} {
    puts "ERROR, Usage:"
    puts "              testCmdr -i inputFileName"
    puts "                       \[-a assumptionFileName\]"
    puts "                       \[-auto\] Auto. answers all prompts Yes"
    puts "                       \[-debug\] Gives extra print statements"
    puts "                       \[-gui\] Implement in a gui interface"
    puts "                       \[-simulate\] Does not execute commands"
#    puts "                       \[-skip TestName\]"
    puts "                                    "
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# variableSubstitution
#
# INVOCATION:
# variableSubstitution
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) fixString (string)  String to do variable substitution on.
#
# PROCEDURE VALUE:
# Returns the substituted string.
#
# PURPOSE:
# To substitue variables on the passed in fixString.
#
# DESCRIPTION:
# To substitue variables on the passed in fixString.
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

proc    variableSubstitution { 
    fixString 
} {

#
#  Declare usage of global variables
#

    global tcmVars
    global varArray


    #
    #  Subsitute machine name if it exists in the command string.
    #

    regsub -all "machineName" $fixString $tcmVars(machineName) \
	    newCommandString

    set fixString $newCommandString


    #
    #  Cycle thru all variables, attempting to do substitutions
    #

    foreach key [ array names varArray ] \
    {
	regsub -all $key $fixString $varArray($key) newCommandString
	if { $newCommandString != $fixString } \
	{
	    debugString "           Substituting $key = $varArray($key)"
	}
	set fixString $newCommandString
    }
    return $fixString
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# writeScreen
#
# INVOCATION:
# writeScreen 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
#				changing the value back.
#  (>) mess (string).  Message to print.
#
# PROCEDURE VALUE:
# None.
#
# PURPOSE:
# To print what is passed in
#
# DESCRIPTION:
# To print what is passed in.
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
proc writeScreen {
    mess
} {
#
# Declare globals and initialize them.
#

    global tcmVars


    #
    #  If we are in gui mode, print to the window, else puts.
    #

    if { $tcmVars(gui) == 1 } \
    {
	.testText configure -state normal
	.testText insert end $mess
	.testText insert insert "\n"
	.testText yview -pickplace end
	.testText configure -state disabled
#	tkwait visibility .testText

    } \
    else \
    { 
	puts $mess
    }
}

#
#***********************************************************************
#+
# PROCEDURE NAME:
# main
#
# INVOCATION:
# main 
#
# PARAMETERS: (">" input, "!" modified, "<" output)
# (>) argc (int) Number of arguments in argv.
# (>) argv (char*) Input arguments.
#
# PROCEDURE VALUE:
# "Failed"	: If a problem occured.
# "Succeeded"	: If no problems occured.
#
# PURPOSE:
# Main program.
#
# DESCRIPTION:
# Main program.
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
proc main {
    argc 
    argv
} {

#
# Declare globals and initialize them.
#
    global tcmVars
    global commandString
    global inFd
    global logFd
    global returnedLine
    global lastLine
    global inputFileName


#
#
# Initialize variables.
#
    set tcmVars(totalCmdsRun) 0
    set tcmVars(numPassCmds) 0
    set tcmVars(totalTestsRun) 0
    set tcmVars(numPassTests) 0
    set tcmVars(groupTestStatus) "X"
    set tcmVars(skipTest) 0
    set tcmVars(fatalError) 0
    set tcmVars(gui) 0
    set firstTime 1
    set commandString ""


    
    #
    #  Check command line arguments and set assumptions file name
    #

    argInit $argc $argv


    #
    # This calls the procedure start which opens inputFile for reading 
    # and writing and reads in all VAR's at the top of the file.
    #

    if { [ start ] == 0 } {
	end 
    }


    #
    # This creates a gui window, if specified in the cmd line.
    #

    setWindow


    #
    #  Print startup stuff, like variable substitution, assumption file,
    #  simulation mode warning, disclaimer.  Set the log filename to
    #  be the input file name + .log.  Write assump. file to log file.
    #

    set logFileName $inputFileName.log
    set logFd [ open $logFileName w ]
    printStartup $logFd



    if { [ queryYesNo "Do you want to continue? " ] } \
    {
	#
	#  Set the log file name and
	#  Open log file and print header
	#

	debugString "Log file: $logFileName, has been opened"
	printHeader $logFd

	#
	# Start the reading of the input file
	#

	while { ! [ eof $inFd ] } \
	{
	    #
	    # Get a line from the file.
	    #

	    if { [ info exists firstTime ] } \
	    {
		#
		#  Already got a line. Use it.
		#

		unset firstTime
		set returnedLine $lastLine
	    } \
	    elseif { [ set x [ readLineIn ] ] == -1 } \
	    {
		# End of file

		break
	    } \
	    elseif { $x == 2 } \
	    {
		# Comment

		continue
	    }


	    #
	    #  Process the line by reading the first word and 
	    #  it decides what to do with the rest of the line.
	    #

	    set cmd [ lindex $returnedLine 0 ]
	    if { [ llength $returnedLine ] == 2 } \
	    {
		set restLine [ lindex $returnedLine 1 ]
	    } \
	    else \
	    {
		set restLine [ lrange $returnedLine 1 \
			[ llength $returnedLine ] ]
	    }
	    if { $cmd != "TEST" && $tcmVars(skipTest) } \
	    {
		#
		# Skipping to next test.
		#

		continue
	    }
	    if { [ processLine $cmd $restLine $logFd ] == 0 } \
	    {
		   break;
	    }
	    
	}


	#
	#  Print the summary results
	#

	printSummary $logFd
    }


    #
    # Finished
    #

    writeScreen Exit

    #
    #  Need to pause the screen to view results for gui
    #

    if {  $tcmVars(gui) == 1 } \
    {
	set tmpResult [tk_messageBox -type ok -default ok\
	    -message "Tests complete\nSee screen for results and \nfile: $inputFileName.log for log of results" \
	    -title "Complete" \
	    -icon info ]
	unset tmpResult
    }

    exit

}

####################################
####################################
####### Begin MAIN
####################################
####################################
main $argc $argv

