static char rcsid[] = "$Id: msg.C,v 1.1.1.1 2002-11-24 20:28:27 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsHistory/src/msg.C
//
// PURPOSE:
// Contains code for the HS messages.
//
// METHOD NAME(S)
// cHisStatus::info - Send a history message to the DHS status server.
// cHisStatus::msgDisplay - Display a message.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:21  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.6  1998/07/24 17:09:45  cockayne
// Modified TEST_FAILURE status so it sets health to warning.
//
// Revision 1.5  1998/06/23 19:22:52  cockayne
// Added S_HIS_LOG_WRITE_BEGIN S_HIS_LOG_WRITE_COMPLETE S_LOST_CONN S_NOT_READY,
// and S_SHUTDOWN messages.
//
// Revision 1.4  1998/04/17 22:48:29  nhill
// Updated to use the latest version of the cDhsStatus class.
// Removed the cHisStatus::msgDisplay and cHisStatus.info methods.
//
// Revision 1.3  1997/10/20 21:47:40  cockayne
// Check-in for install.
//
// Revision 1.2  1997/10/06 20:15:57  cockayne
// Check-in for code review
//
// Revision 1.1  1997/09/02 21:13:35  cockayne
// Initial revision
//
// Revision 1.2  1997/08/20 21:22:41  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#include "globals.H"


//
// Define the dhsHistory message strings.
//

msgText( cHisStatus, S_COMMAND_RECEIVED, P_LOG | P_DEBUG_MIN,
	    "Received `%s' command from `%s' on `%s'." );
msgText( cHisStatus, S_CONNECT_REQ, P_LOG | P_DEBUG_MIN,
	    "Connect request from `%s' on `%s'." );
msgText( cHisStatus, S_CONFIG_READ, P_DEBUG_MIN,
	    "Reading configuration file `%s'." );
msgText( cHisStatus, S_DEBUG_LEVEL, P_LOG | P_DEBUG_MIN,
	    "Debug level set to `%s'." );
msgText( cHisStatus, S_DEBUG_MSG, P_NULL,
	    "Debug message from DHS : `%s'." );
msgText( cHisStatus, S_DISCONNECT_REQ, P_LOG | P_DEBUG_MIN,
	    "Disconnect request from `%s' on `%s'." );
msgText( cHisStatus, S_EXITING, P_LOG | P_DEBUG_MIN,
	    "History server exiting." );
msgText( cHisStatus, S_FREMOVE, P_LOG,
	    "Please check why file `%s' is in configured working dir `%s'." );
msgText( cHisStatus, S_HIS_LOG_WRITE_BEGIN, P_LOG | P_HEALTH_WARN,
	    "History Server has begun writing the History Log." );
msgText( cHisStatus, S_HIS_LOG_WRITE_COMPLETE, P_LOG,
	    "History Server has completed writing the History Log." );
msgText( cHisStatus, S_INITIALIZING, P_LOG | P_DEBUG_MIN,
	    "History server intializing." );
msgText( cHisStatus, S_LOST_CONN, P_LOG | P_DEBUG_MIN,
	    "History server lost connection to `%s' at `%s'." );
msgText( cHisStatus, S_NOT_READY, P_LOG | P_DEBUG_MIN,
	    "History server not ready to process connection from "
	    "`%s' at `%s'. Disconnecting." );
msgText( cHisStatus, S_RECOVERING, P_LOG,
	    "History server recovering from previously "
	    "un-finished hisLogWrite command." );
msgText( cHisStatus, S_RUNNING, P_LOG | P_DEBUG_MIN,
	    "History server running." );
msgText( cHisStatus, S_SHUTDOWN, P_LOG,
	    "History Server shutting down." );
msgText( cHisStatus, S_SIMULATE_LEVEL, P_LOG,
	    "Simulation level set to `%s'." );
msgText( cHisStatus, S_TEST_FAILURE, P_LOG | P_HEALTH_WARN,
	    "Test command failed." );
msgText( cHisStatus, S_TEST_SUCCESS, P_LOG | P_DEBUG_MIN,
	    "Test command succeeded." );
msgText( cHisStatus, S_VERSION, P_NULL,
	    "%s\n%s" );

msgText( cHisStatus, E_ABORT, P_LOG,
	    "Abort command from `%s' is refused." );
msgText( cHisStatus, E_CA, P_LOG,
	    "Error %d in channel access function:\n%s\n%s." );
msgText( cHisStatus, E_CALLBACK, P_LOG,
	    "Error %d in channel access callback function." );
msgText( cHisStatus, E_CFITSIO, P_LOG,
	    "Error %d in fitsio function:\n%s\n%s." );
msgText( cHisStatus, E_CHANNEL, P_LOG,
	    "Could not connect EPICS channel : `%s'." );
msgText( cHisStatus, E_CMD_FORMAT, P_LOG,
	    "Invalid command format for command `%s'." );
msgText( cHisStatus, E_DATASERVER_MSG, P_LOG,
	    "Message from dataServer : `%s'." );
msgText( cHisStatus, E_DB, P_LOG,
	    "Error in db library: `%s'\nfor function `%s'." );
msgText( cHisStatus, E_DB_LIB, P_LOG,
	    "Error in db library: `%s'." );
msgText( cHisStatus, E_DEBUG_COMMAND, P_LOG,
	    "Invalid debug level: `%s'." );
msgText( cHisStatus, E_DUPLICATE_CHANNEL, P_LOG,
	    "Duplicate channel in configuration file: `%s'." );
msgText( cHisStatus, E_FCLOSE, P_LOG,
	    "Could not close file: `%s'." );
msgText( cHisStatus, E_FOPEN, P_LOG,
	    "Could not open file: `%s'." );
msgText( cHisStatus, E_FREAD, P_LOG,
	    "Could not read file: `%s'." );
msgText( cHisStatus, E_FREMOVE, P_LOG,
	    "Could not remove file: `%s'." );
msgText( cHisStatus, E_FRENAME, P_LOG,
	    "Could not rename file: `%s' to file `%s'." );
msgText( cHisStatus, E_FSEEK, P_LOG,
	    "Could not seek within file: `%s'." );
msgText( cHisStatus, E_FSTAT, P_LOG,
	    "Could not stat file: `%s'." );
msgText( cHisStatus, E_FWRITE, P_LOG,
	    "Could not write to file: `%s'." );
msgText( cHisStatus, E_HISLOGWRITE, P_LOG,
	    "Already processing hislogWrite command. Command refused. " );
msgText( cHisStatus, E_MSG, P_LOG,
	    "Error from DHS : `%s'." );
msgText( cHisStatus, E_NEWLABEL, P_LOG,
	    "dataServer re-labelled history file with label: `%s'. "
	    "New label is `%s'. Check db before re-running History Server." );
msgText( cHisStatus, E_PATH, P_LOG,
	    "Configured working directory not useable: `%s'." );
msgText( cHisStatus, E_PTHREAD, P_LOG,
	    "Failed to initialize event queue processing thread." );
msgText( cHisStatus, E_PUT, P_LOG,
	    "Error put-ing file `%s' to dataServer." );
msgText( cHisStatus, E_RUNNING, P_LOG,
	    "Another History server is running on this system." );
msgText( cHisStatus, E_SIMULATE_COMMAND, P_LOG,
	    "Invalid simulate level: `%s'." );
msgText( cHisStatus, E_UNKNOWN_COMMAND, P_LOG,
	    "Unknown command `%s' received from `%s' on `%s'." );
msgText( cHisStatus, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsHistory [-V] [-debug {NONE|MIN|FULL}] [-simulate]" );
msgText( cHisStatus, E_WORKING_DIR, P_LOG,
	    "Error opening configured working directory: `%s'." );
