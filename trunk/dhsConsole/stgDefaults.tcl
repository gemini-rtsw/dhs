# $Id: stgDefaults.tcl,v 1.1.1.1 2002-11-24 20:25:42 brighton Exp $
#
#***********************************************************************
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#
# (c) 1998>				(c) 1998
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
# dhsConsole/src/stgDefaults.tcl
#
# PURPOSE:
# Define a number of default values to be used with the CStgMain class.
# All default values are stored in the *global* associative array
# _stgDefaults.
#
# CLASS NAME(S)
# None.
#
# METHOD NAME(S)
# None.
#
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
#
#
# Revision 1.1.1.1  1999/08/23 19:58:10  cvs-tuc
# Initial install into CVS of dhs-0.16
#
# Revision 1.7  1999/06/08 05:17:46  jaeger
# Update short help information.
#
# Revision 1.6  1999/02/25 03:14:34  jaeger
# Added stoRefresh command.
#
# Revision 1.5  1998/07/16 20:00:06  jaeger
# Fixed up the help information.
#
# Revision 1.4  1998/06/22 21:28:02  jaeger
# Added code documentation to bring it up to Gemini standards.
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#


#
# Global font value.
#

set _stgDefaults(entryFont) "-Adobe-Helvetica-Bold-R-Normal-*-12-120-*-*-*-*-*-*"
set _stgDefaults(labelFont) "-Adobe-Helvetica-Bold-R-Normal-*-12-120-*-*-*-*-*-*"



#
# Define the Storage Server subsystem
# This probably should be in the file dhsConsole/src/stgMain.tcl
# but resides here for now.
#
# name          - Long subsystem name 
# status        - Status identifers being monitored
# statusDesc    - Lond description of each status being monitored.
# help          - Short help information for each status being monitored.
# commandName   - List of the subsystems Long command names.
# commands      - The actuall subsystem commands that are executed.
# commandDesc   - Short help information for each command
# commandHelp   - HTML help page associated with the command.
#

set _stgDefaults(sto,name)	"Storage Server"
set _stgDefaults(sto,status) { State Health Debug Sim Desc }
set _stgDefaults(sto,statusDesc) {State: Health: {Debug Level:}		\
	{Simulation Level:} {Health Description:}}
set _stgDefaults(sto,help) {                                      	\
    {Current State of the Storage Server.}				\
    {Health of the Storage Server.}					\
    {Current level of Debugging for the Storage Server.}		\
    {Current Simulation Level for the Storage Server.}			\
    {{bitmap b1} Long health description for the Storage Server.}}
set _stgDefaults(sto,commandName) {Debug Simulation {Reset Health} 	\
     blank Refresh blank Reset Initialize blank Ping Test}
set _stgDefaults(sto,commands) {{debugLevel sto} {simulateLevel sto} 	\
     {resetHealth sto} separator {stoRefresh .cstgmain} separator    	\
     {reset sto} {init sto} separator {ping sto} {test sto}}
set _stgDefaults(sto,commandDesc) {					\
    {Set Debug Level for the Command Server.} 				\
    {Set Simulation Level for Command Server.} 				\
    {Set the Health of the Command Server to 'GOOD'.} blank		\
    {Reread the database values and display them.} blank		\
    {Reset the Command Server.}						\
    {Initailze the Commande Server.} blank				\
    {Ping Storage Server to see if it is responding to commands.}	\
    {Perform tests on the Command Server.}}
set _stgDefaults(sto,commandHelp) {					\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"}			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} }



#
# Short help information for the request buttons.
#

set _stgDefaults(sto,requestHelp) {
    { Unique, numerical request identifier. }				\
    { Indicates if the request is a USER or ARCHIVE request. }	\
    { Type of media that is to be made (eg. CD, DVD, Tape).}		\
    { Number of copies that are created for each unit of media. }   \
    { If TRUE then retrieval of files is automatic.}			\
    { The amount of data that can be retrieved ( in Megabytes ).}	\
    { The current state of retrival (IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The amount of data available for queueing (has been retrieved). }	\
    { Current state or queueing ( ROLLBACK IN-PROGRESS, IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The number of units that can be prepared (have been queued). }	\
    { Current state or preparing ( ROLLBACK IN-PROGRESS, IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The number of units that can be written (have been prepared). }	\
    { Current state or writing( ROLLBACK IN-PROGRESS, IDLE, REQUESTED, or IN-PROGRESS).}	\
    { The number of units that can be cleaned up (have been written). }	\
    { The current state of cleaning up (IDLE, REQUESTED, or IN-PROGRESS).} \
    { True if notification can be performed, False otherwise. }		\
    { The current state of notification (IDLE, REQUESTED, or IN-PROGRESS).}}

