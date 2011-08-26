# $Id: dhgDefaults.tcl,v 1.1.1.1 2002-11-24 20:25:38 brighton Exp $
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
# dhsConsole/src/dhgDefaults.tcl
#
# PURPOSE:
# Define a number of default values to be used with the CDhgMain class.
# All default values are stored in the *global* associative array
# _dhgDefaults.
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
# Revision 1.12  1999/02/25 03:14:34  jaeger
# Added stoRefresh command.
#
# Revision 1.11  1999/01/26 06:15:27  jaeger
# Corrected grammar of "flush" cmd short-help message.
#
# Revision 1.10  1998/08/14 21:47:54  jaeger
# Chages reflect flush command now having a subsystem list.
#
# Revision 1.9  1998/07/16 19:59:30  jaeger
# Fixed up the help information.
#
# Revision 1.8  1998/06/17 21:49:18  jaeger
# Documented code to bring it up to Gemini standards.
#
#
#***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#***********************************************************************
#

#
# Global font value.
#

set _dhgDefaults(entryFont) "-Adobe-Helvetica-Bold-R-Normal-*-12-120-*-*-*-*-*-*"

#
# Define the subsytems of dhs
# This probably should be in the file dhsConsole/src/dhgMain.tcl
# but resides here for now.
#
# name 		- Long subsystem name 
# status	- Status identifers being monitored
# statusDesc	- Lond description of each status being monitored.
# help		- Short help information for each status being monitored.
# commandName	- List of the subsystems Long command names.
# commands	- The actuall subsystem commands that are executed.
# commandDesc	- Short help information for each command
# commandHelp	- HTML help page associated with the command.
#


#
# DHS Command Server
#

set _dhgDefaults(cmd,name) 		{Command Server}
set _dhgDefaults(cmd,status) {State Health Debug Sim Desc}
set _dhgDefaults(cmd,statusDesc) {State: Health: {Debug Level:} 	\
    {Simulation Level:} {Health Description:} }
set _dhgDefaults(cmd,help) {						\
    {Current State of the Command Server.}				\
    {Health of the Command Server.}					\
    {Current level of Debugging for the Command Server.}		\
    {Current Simulation Level for the Command Server.}			\
    {{bitmap b1} Long health description for the Command Server.}}
set _dhgDefaults(cmd,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(cmd,commands) {{debugLevel cmd} {simulateLevel cmd} 	\
    {resetHealth cmd} separator {reset cmd} {init cmd} separator 	\
    {ping cmd} {test cmd}}
set _dhgDefaults(cmd,commandDesc) {					\
    {Set Debug Level for the Command Server.} 				\
    {Set Simulation Level for Command Server.} 				\
    {Set the Health of the Command Server to 'GOOD'.} blank		\
    {Reset the Command Server.}						\
    {Initailze the Command Server.} blank				\
    {Ping the Command Server, check to see if it is running.}		\
    {Perform tests on the Command Server.}}
set _dhgDefaults(cmd,commandHelp) {					\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"}				\
    {_helpInfo openUrl "dhgCmd.html#toolbar"} }


#
# DHS, overall inforamation
#

set _dhgDefaults(dhs,name) {Data Handling System}
set _dhgDefaults(dhs,status) {State Health Debug Sim Log}
set _dhgDefaults(dhs,statusDesc) {State: Health: 			\
     {Debug Level:} {Simulation Level:} {Log Message:}}
set _dhgDefaults(dhs,help) {						\
    {Current State of the Data Handling System.}			\
    {Health of the Data Handling System.}				\
    {Current level of Debugging for the Data Handling System.}		\
    {Current Simulation Level for the Data Handling System.}		\
    {{bitmap b1} DHS history log message.}}
set _dhgDefaults(dhs,commandName) {{Debug} {Simulation} {Reset Health}	\
     blank {Reset} {Initialize}  blank Ping Test blank Shutdown}
set _dhgDefaults(dhs,commands) {{debugLevel dhs} {simulateLevel dhs} 	\
    resetHealth separator {reset} {init}  separator {ping} {test} 	\
    separator dhsExit}
set _dhgDefaults(dhs,commandDesc) {					\
    {Set the Debug Level for DHS.}					\
    {Set the Simulation Level for DHS.} 				\
    {Set the Health of DHS to 'GOOD'.} blank				\
    {Reset the ENTIRE Data Handling System.}				\
    {Re-Initailize the ENTIRE Data Handling System.} blank		\
    {Ping all DHS subsystems to see if they are responding.}		\
    {Perform tests on the DHS.} blank					\
    {Shutdown the entire Data Handling System.}}
set _dhgDefaults(dhs,commandHelp) {					\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} blank			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} blank			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"}			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} blank			\
    {_helpInfo openUrl "dhsConsole.html#toolbar"} }

#
# DHS Data Server
#

set _dhgDefaults(dts,name) {Data Server}
set _dhgDefaults(dts,status) {State Health Debug Sim Desc Inc}
set _dhgDefaults(dts,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} {Number of Incomplete:} }
set _dhgDefaults(dts,help) {						\
    {Current State of the Data Server.}					\
    {Health of the Data Server.}					\
    {Current level of Debugging for the Data Server.}			\
    {Current Simulation Level for the Data Server.}			\
    {{bitmap b1} Long health description for the Data Server.} 		\
    {Number of datasets that have not been fully processedd.} }
set _dhgDefaults(dts,commandName) {{Debug} {Simulation} {Reset Health} 
    blank {Datasets} blank {Reset} {Initialize} blank Ping Test}
set _dhgDefaults(dts,commands) {{debugLevel dts} {simulateLevel dts} 	\
    {resetHealth dts} separator {dtsDatasetInfo} separator {reset dts} \
    {init dts} separator {ping dts} {test dts}}
set _dhgDefaults(dts,commandDesc) {					\
    {Set Debug Level for the Data Server.} 				\
    {Set Simulation Level for Data Server.} 				\
    {Set the Health of the Data Server to 'GOOD'.} blank		\
    {View the list of datasets the Data Server knows of.} blank		\
    {Reset the Data Server.}						\
    {Initailze the Data Server.} blank				\
    {Ping the Data Server to see if it is responding to commands.}	\
    {Perform tests on the Data Server.}}
set _dhgDefaults(dts,commandHelp) {					\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgDts.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"}				\
    {_helpInfo openUrl "dhgDts.html#toolbar"} }


#
# DHS History Server
#

set _dhgDefaults(his,name) {History Server}
set _dhgDefaults(his,status) {State Health Debug Sim Desc}
set _dhgDefaults(his,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} }
set _dhgDefaults(his,help) {						\
    {Current State of the History Server.}				\
    {Health of the History Server.}					\
    {Current level of Debugging for the History Server.}		\
    {Current Simulation Level for the History Server.}			\
    {{bitmap b1} Long health description for the History Server.}}
set _dhgDefaults(his,commandName) {Debug Simulation {Reset Health}      \
    blank {Save History Log} blank Reset Initialize blank Ping Test}
set _dhgDefaults(his,commands) {{debugLevel his} {simulateLevel his} 	\
    {resetHealth his} separator {hisLogWrite} separator {reset his} 	\
    {init his} separator {ping his} {test his}}
set _dhgDefaults(his,commandDesc) {					\
    {Set Debug Level for the History Server.} 				\
    {Set Simulation Level for History Server.} 				\
    {Set the History Server's Health to 'GOOD'} blank			\
    {Save history Log to Archive.} blank				\
    {Reset the History Server.}						\
    {Initailze the Storage Server.} blank				\
    {Ping the History Server to see if it is responding to commands.}	\
    {Perform self tests on the History Server.}}
set _dhgDefaults(his,commandHelp) {					\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgHis.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"}				\
    {_helpInfo openUrl "dhgHis.html#toolbar"} }



#
# DHS On-line Data Processing  Server
#

set _dhgDefaults(olp,name) "On-line Data Processing Server"
set _dhgDefaults(olp,status) {State Health Debug Sim Desc Over \
     Size Num Active}
set _dhgDefaults(olp,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:}  {Health Description:} Overflow 		\
	{Array Size:} {Active Recipe Configurations:} 			\
         {Executing Recipe Configurations:}}
set _dhgDefaults(olp,help)						\
    {{Current State of the On-line Data Processing Server.}		\
    {Health of the On-line Data Processing Server.}			\
    {Current level of Debugging for the On-line Data Processing Server.} \
    {Current Simulation Level for the On-line Data Processing Server.}	\
    {{bitmap b1} Long health description for the On-line Data Processing Server.}	\
    {True if the number of current OLDP jobs exceeds the number allowed.} \
    {Size of teh OLDP Recipe configuration status record arrays.}	\
    {Number of Recipe Configurations currently active.}			\
    {Number of Recipe Configurations currently being processed.}}
set _dhgDefaults(olp,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(olp,commands) {{debugLevel olp} {simulateLevel olp} 	\
    {resetHealth olp} separator {reset olp} {init olp} separator 	\
    {ping olp} {test olp}}
set _dhgDefaults(olp,commandDesc) {					\
    {Set Debug Level for the On-line Data Processing Server.} 		\
    {Set Simulation Level for On-line Data Processing Server.} 		\
    {Set the Health of the On-line Data Processing Server to 'GOOD'.} 	\
    blank {Reset the On-line Data Processing Server.}			\
    {Initailze the On-line Data Processinge Server.} blank		\
    {Ping the On-line Data Processing Server.}				\
    {Perform tests on the On-line Data Processing Server.}}
set _dhgDefaults(olp,commandHelp) {					\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"} blank			\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"} blank			\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"}				\
    {_helpInfo openUrl "dhgOlp.html"} }


#
# DHS Quick Look Server
#

set _dhgDefaults(qls,name) "Quick Look Server"
set _dhgDefaults(qls,status) {State Health Debug Sim Desc}
set _dhgDefaults(qls,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} }
set _dhgDefaults(qls,help) {						\
    {Current State of the Quick Look Server.}				\
    {Health of the Quick Look Server.}					\
    {Current level of Debugging for the Quick Look Server.}		\
    {Current Simulation Level for the Quick Look Server.}		\
    {{bitmap b1} Long health description for the Quick Look Server.} }
set _dhgDefaults(qls,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(qls,commands) {{debugLevel qls} {simulateLevel qls} 	\
    {resetHealth qls} separator {reset qls} {init qls} separator 	\
    {test qls}}
set _dhgDefaults(qls,commands) {{debugLevel qls} {simulateLevel qls} 	\
    {resetHealth qls} separator {reset qls} {init qls} separator 	\
    {ping qls} {test qls}}
set _dhgDefaults(qls,commandDesc) {					\
    {Set Debug Level for the Quick Look Server.} 			\
    {Set Simulation Level for Quick Look Server.} 			\
    {Set the Health of the Quick Look Server to 'GOOD'.} blank		\
    {Reset the Quick Look Server.}					\
    {Initailze the Quick Looke Server.} blank				\
    {Ping the Quick Look Server to see if it is responding to commands.} \
    {Perform tests on the Quick Look Server.}}
set _dhgDefaults(qls,commandHelp) {					\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"}				\
    {_helpInfo openUrl "dhgQls.html#toolbar"} }


#
# DHS Synchronized Data Server
#

set _dhgDefaults(sdp,name) "Synchronized Data Processing Server"
set _dhgDefaults(sdp,status) {State Health Debug Sim Desc Over Size Num}
set _dhgDefaults(sdp,statusDesc) {State: Health: {Debug Level:} 	\
	{Simulation Level:} {Health Description:} Overflow: 		\
	{Array Size:} {Number of Jobs:}}
set _dhgDefaults(sdp,help)  {						\
    {Current State of the Synchronized Data Processing Server.}		\
    {Health of the Synchronized Data Processing Server.}		\
    {Current level of Debugging for the Synchronized Data Processing Server.} \
    {Current Simulation Level for the Synchronized Data Processing Server.} \
    {{bitmap b1} Long health description for the Synchronized Data Processing Server.} \
    {True if the numbmer of current SDP jobs exceeds the number allows.} \
    {Number of jobs monitored by EPICS.}				\
    {Number of jobs being processed by SDP.}}
set _dhgDefaults(sdp,commandName) {Debug Simulation {Reset Health} 	\
        blank Reset Initialize blank Ping Test}
set _dhgDefaults(sdp,commands) {{debugLevel sdp} {simulateLevel sdp} 	\
    {resetHealth sdp} separator {reset sdp} {init sdp} separator 	\
    {ping sdp} {test sdp}}
set _dhgDefaults(sdp,commandDesc) {					\
    {Set Debug Level for the Synchronized Data Processing Server.} 	\
    {Set Simulation Level for Synchronized Data Processing Server.}	\
    {Set the Health of the Synchronized Data Processing Server to 'GOOD'.} \
    blank {Reset the Synchronized Data Processing Server.}		\
    {Initailze the Synchronized Data Processinge Server.} blank		\
    {Ping the Synchronized Data Processing Server.}			\
    {Perform tests on the Synchronized Data Processing Server.}}
set _dhgDefaults(sdp,commandHelp) {					\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"} blank			\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"} blank			\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"}				\
    {_helpInfo openUrl "dhgSdp.html"} }


#
# DHS Status Server
#

set _dhgDefaults(sta,name) "Status Server"
set _dhgDefaults(sta,status) {State Health Debug Sim Desc}
set _dhgDefaults(sta,statusDesc) {State: Health: {Debug Level:} \
	{Simulation Level:} {Health Description:}}
set _dhgDefaults(sta,help) {						\
    {Current State of the Status Server.}				\
    {Health of the Status Server.}					\
    {Current level of Debugging for the Status Server.}			\
    {Current Simulation Level for the Status Server.}			\
    {{bitmap b1} Long health description for the Status Server.}}
set _dhgDefaults(sta,commandName) {Debug Simulation {Reset Health}	\
    blank Flush blank Reset Initialize  blank Ping Test}
set _dhgDefaults(sta,commands) {{debugLevel sta} {simulateLevel sta} 	\
    {resetHealth sta} separator {flush sta} separator {reset sta} {init sta}  \
    separator {ping sta} {test sta}}
set _dhgDefaults(sta,commandDesc) {					\
    {Set Debug Level for the Status Server.}				\
    {Set Simulation Level for the Status Server.} 			\
    {Set the Health of the Status Server to 'GOOD'.} blank		\
    {Flush the values of all status records.} blank			\
    {Reset the Status Server.}						\
    {Initailze the Status Server.} blank				\
    {Ping the Status Server to see if it is responding to commands.}	\
    {Perform tests on the Status Server.}}
set _dhgDefaults(sta,commandHelp) {					\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSta.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"}				\
    {_helpInfo openUrl "dhgSta.html#toolbar"} }




#
# DHS Storage Server
#

set _dhgDefaults(sto,name) "Storage Server"
set _dhgDefaults(sto,status) {State Health Debug Sim Desc}
set _dhgDefaults(sto,statusDesc) {State: Health: {Debug Level:}		\
	{Simulation Level:} {Health Description:}}
set _dhgDefaults(sto,help) {						\
    {Current State of the Storage Server.}				\
    {Health of the Storage Server.}					\
    {Current level of Debugging for the Storage Server.}		\
    {Current Simulation Level for the Storage Server.}			\
    {{bitmap b1} Long health description for the Storage Server.}}
set _dhgDefaults(sto,commandName) {Debug Simulation {Reset Health} 	\
     blank Refresh blank Reset Initialize blank Ping Test}
set _dhgDefaults(sto,commands) {{debugLevel sto} {simulateLevel sto} 	\
     {resetHealth sto} separator {stoRefresh .cstgmain} separator    	\
     {reset sto} {init sto} separator {ping sto} {test sto}}
set _dhgDefaults(sto,commandDesc) {					\
    {Set Debug Level for the Storage Server.} 				\
    {Set Simulation Level for Storage Server.} 				\
    {Set the Health of the Storage Server to 'GOOD'.} blank		\
    {Reread the database values and display them.} blank		\
    {Reset the Storage Server.}						\
    {Initailze the Storage Server.} blank				\
    {Ping the Storage Server to see if it is responding to commands.}	\
    {Perform tests on the Storage Server.}}
set _dhgDefaults(sto,commandHelp) {					\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_stgHelpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"} blank			\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"}				\
    {_helpInfo openUrl "dhgSto.html#toolbar"} }

