static char rcsid[] = "$Id: msg.C,v 1.1.1.1 2002-11-24 20:31:40 brighton Exp $";
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
// dhsStorage/src/msg.C
//
// PURPOSE:
// Contains the DHS Storage Server messages.
//
// METHOD NAME(S)
// None
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.17  2001/03/09 22:51:41  nhill
// Added the E_ACTION_INVALID and E_PARAM_TYPE messages.
//
// Revision 1.16  2000/02/03 00:56:10  nhill
// Added message E_USER_APP_OPTIONS.
//
// Revision 1.15  2000/01/06 17:58:29  nhill
// Added E_FILE_PATH message.
//
// Revision 1.14  1999/10/12 22:46:22  nhill
// Added some new messages.
//
// Revision 1.13  1999/06/18 22:29:30  nhill
// added the E_NO_MEDIA_TYPE message.
//
// Revision 1.12  1999/06/16 23:52:51  nhill
// added messages, deleted messages, added the alert status property.
//
// Revision 1.11  1999/02/24 23:48:17  jaeger
// Added E_UNABLE_TO_CONN message.
//
// Revision 1.10  1998/06/25 18:08:46  cockayne
// S_DEBUG_MSG *must* have P_NULL as its property set.
//
// Revision 1.9  1998/06/24 15:58:48  cockayne
// Added S_DEBUG_MSG, S_LOST_CONN, S_SHUTDOWN, modified E_PRESS messages.
//
// Revision 1.8  1998/05/25 19:07:19  cockayne
// Added S_ACTION_COMPLETE msgText, renamed S_START_ACTION and
// S_STOPPING_ACTION to S_ACTION_STARTING and S_ACTION_STOPPING, Modified
// some msgText P_??? properties.
//
// Revision 1.7  1998/05/19 16:26:34  cockayne
// Removed S_CONFIG_READ. It's defined in the library.
//
// Revision 1.6  1998/05/15 17:02:59  cockayne
// Mod'ed message properties, added E_WRITE_LOST_UNIT, E_UNIT_NOT_WRITTEN,
// E_UNIT_NOT_PREPARED, E_STARTUP_CMD, E_RMDIR, E_PRESS, E_PREP_LOST_UNIT,
// E_NUMNODES, E_NO_ROLLBACK_WRITE, E_NO_ROLLBACK_QUEUE, E_MEDIA_REQUEST_TYPE,
// E_CFG_LOCALDATASERVER, E_CFG_NOTIFYDATASERVER. Removed E_CFG_DATASERVER.
//
// Revision 1.5  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.4  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 21:00:34  cockayne
// Added E_ARRAY_SIZES message.
//
// Revision 1.2  1998/03/26 21:22:07  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
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

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"


//
// Define the dhsStorage message strings.
//

msgText( cStoStatus, S_ACTION_COMPLETE, P_LOG | P_DEBUG_MIN,
	    "Storage Server has completed action `%s' "
	    "on media request id `%d'." );
msgText( cStoStatus, S_ACTION_DISABLED, P_LOG | P_ALERT | P_DEBUG_MIN,
	    "Action `%s' is disabled due to other ongoing actions." );
msgText( cStoStatus, S_ACTION_STARTING, P_LOG | P_DEBUG_MIN,
	    "Storage Server starting action `%s' on media request id `%d'." );
msgText( cStoStatus, S_ACTION_STOPPING, P_LOG | P_DEBUG_MIN,
	    "Storage Server stopping action `%s' on media request id `%d'." );
msgText( cStoStatus, S_CMD_RECEIVED, P_LOG | P_DEBUG_MIN,
	    "Received `%s' command from `%s' on `%s'." );
msgText( cStoStatus, S_CMD_TEST_SUCCESS, P_LOG | P_DEBUG_MIN,
	    "Test command succeeded." );
msgText( cStoStatus, S_CONNECT_REQ, P_LOG | P_DEBUG_MIN,
	    "Connect request from `%s' on `%s'." );
msgText( cStoStatus, S_DEBUG_LEVEL, P_LOG | P_DEBUG_MIN,
	    "Debug level set to `%s'." );
msgText( cStoStatus, S_DEBUG_MSG, P_NULL,
	    "Debug message from DHS : `%s'." );
msgText( cStoStatus, S_DISCONNECT_REQ, P_LOG | P_DEBUG_MIN,
	    "Disconnect request from `%s' on `%s'." );
msgText( cStoStatus, S_DUPLICATE_FILE, P_LOG,
	    "Duplicate user file ID: `%s' for request `%s' id %d." );
msgText( cStoStatus, S_EXITING, P_LOG | P_DEBUG_MIN,
	    "Storage server exiting." );
msgText( cStoStatus, S_INITIALIZING, P_LOG | P_DEBUG_MIN,
	    "Storage server intializing." );
msgText( cStoStatus, S_LOST_CONN, P_LOG | P_DEBUG_MIN,
            "Storage server lost connection to `%s' at `%s'." );
msgText( cStoStatus, S_NO_CLEANUPABLE, P_LOG | P_ALERT | P_DEBUG_MIN,
	    "No media units to clean up for media request ID: %d." );
msgText( cStoStatus, S_NO_PREPARABLE, P_LOG | P_ALERT | P_DEBUG_MIN,
	    "No media units to prepare for media request ID: %d." );
msgText( cStoStatus, S_NO_WRITEABLE, P_LOG | P_ALERT | P_DEBUG_MIN,
	    "No media units to write for media request ID: %d." );
msgText( cStoStatus, S_NO_STAGE_DIRECTORIES, P_LOG | P_ALERT | P_DEBUG_MIN,
	    "No staging directories available to prepare\n"
	    "media unit `%s' for media request ID: %d." );
msgText( cStoStatus, S_RETRIEVE_LIMIT_REACHED, P_LOG | P_DEBUG_MIN,
	    "The RETRIEVE process for media ID `%d' has reached "
	    "user-set limit of `%d' Mbytes." );
msgText( cStoStatus, S_ROLLBACK_DISABLED, P_LOG | P_ALERT | P_DEBUG_MIN,
	    "Rollback `%s' is disabled due to other ongoing actions." );
msgText( cStoStatus, S_RUNNING, P_LOG | P_DEBUG_MIN,
	    "Storage server running." );
msgText( cStoStatus, S_SHUTDOWN, P_LOG,
            "Storage Server shutting down." );
msgText( cStoStatus, S_SIMULATE_LEVEL, P_LOG | P_DEBUG_MIN,
	    "Simulation level set to `%s'." );
msgText( cStoStatus, S_VERSION, P_NULL,
	    "%s\n%s" );

msgText( cStoStatus, E_AM, P_LOG | P_HEALTH_BAD,
	    "Error in the cadc am library." );
msgText( cStoStatus, E_ACTION_INVALID, P_LOG,
	    "Error - Invalid action type: %s." );
msgText( cStoStatus, E_ACTION_OVERFLOW, P_LOG | P_HEALTH_WARN,
	    "Warning - The number of actions exceeds the size of the EPICS "
	    "action array." );
msgText( cStoStatus, E_ARRAY_SIZES, P_LOG,
	    "Error processing User Media Request Command - the file arrays' "
	    "sizes do not match - %s." );
msgText( cStoStatus, E_CLEANUP_LOST_UNIT, P_LOG | P_HEALTH_WARN,
	    "After the Cleanup process was run for unit `%s', the unit was "
	    "un-findable in the database. Please investigate." );
msgText( cStoStatus, E_CMD_BAD_ATTR_VALUE, P_LOG,
	    "Error - `%s' command has invalid value of `%s' attribute. "
	    "Value: `%s'." );
msgText( cStoStatus, E_CMD_CONNECTION, P_LOG,
	    "Error - Received command `%s' from invalid connector "
	    "`%s' at `%s'." );
msgText( cStoStatus, E_CMD_FORMAT, P_LOG,
	    "Error - Invalid command format for command `%s'." );
msgText( cStoStatus, E_CMD_NO_ATTRIBUTE, P_LOG,
	    "Error - `%s' command has no `%s' attribute." );
msgText( cStoStatus, E_CMD_PROCESSING, P_LOG,
	    "Error processing `%s' command from `%s' at `%s'." );
msgText( cStoStatus, E_CMD_UNKNOWN, P_LOG,
	    "Unknown command `%s' received from `%s' on `%s'." );
msgText( cStoStatus, E_CFG_CONNECTION, P_LOG,
	    "Error - Duplicate connection identity: `%s'." );
msgText( cStoStatus, E_CFG_LOCALDATASERVER, P_LOG,
	    "Error - Local dataServer identity `%s' not in connection list." );
msgText( cStoStatus, E_CFG_NOTIFYDATASERVER, P_LOG,
	    "Error - Identity of dataServer to notify of startup `%s' "
	    "not in connection list." );
msgText( cStoStatus, E_DB_LIB, P_LOG,
	    "Error in db library: `%s'." );
msgText( cStoStatus, E_DEV_NOT_BOOKED, P_LOG | P_ALERT,
	    "Error - Could not book device `%s'. It is probably in use." );
msgText( cStoStatus, E_DEVICE_FIND, P_LOG | P_ALERT,
	    "Error - Could not find device with logical name `%s'\n"
	    "and type `%s'." );
msgText( cStoStatus, E_DEVICE_OVERFLOW, P_LOG | P_HEALTH_BAD,
	    "Error - The number of logical devices exceeds the size of the\n"
	    "EPICS device array." );
msgText( cStoStatus, E_DUPLICATE_FILE, P_LOG,
	    "Duplicate archive file notification for file ID: `%s'." );
msgText( cStoStatus, E_EXEC_ABNORMAL_EXIT, P_LOG | P_ALERT | P_HEALTH_WARN,
	    "Error: Storage application `%s' exited abnormally." );
msgText( cStoStatus, E_EXEC_NONZERO_EXIT, P_LOG | P_ALERT | P_HEALTH_WARN,
	    "Error: Storage application `%s' exited with error status: %d." );
msgText( cStoStatus, E_FCLOSE, P_LOG | P_HEALTH_BAD,
	    "Error closing file: `%s'." );
msgText( cStoStatus, E_FILE_GET, P_LOG | P_HEALTH_WARN,
	    "Error retrieving label `%s' for request %d from data server." );
msgText( cStoStatus, E_FILE_PATH, P_LOG,
	    "Invalid file path `%s' in stoUserMediaRequest command." );
msgText( cStoStatus, E_FIVESTATE, P_LOG | P_HEALTH_WARN,
	    "Error setting fivestate variable: `%s'." );
msgText( cStoStatus, E_FOPEN, P_LOG | P_HEALTH_BAD,
	    "Error opening file: `%s'." );
msgText( cStoStatus, E_FORK_FAILED, P_LOG | P_ALERT | P_HEALTH_BAD,
	    "Error - fork1() failed to create a process to execute `%s'." );
msgText( cStoStatus, E_FPATH, P_LOG | P_HEALTH_BAD,
	    "Error creating directory path: `%s'." );
msgText( cStoStatus, E_FSIZE, P_LOG,
	    "File arrived with incorrect size: `%s'." );
msgText( cStoStatus, E_FSTAT, P_LOG | P_HEALTH_BAD,
	    "Error executing stat() on file: `%s'." );
msgText( cStoStatus, E_FWRITE, P_LOG | P_HEALTH_BAD,
	    "Error writing to file: `%s'." );
msgText( cStoStatus, E_MEDIA_REQUEST_TYPE, P_LOG | P_HEALTH_WARN,
	    "Error - Invalid media request type `%s' "
	    "for media request id `%d'." );
msgText( cStoStatus, E_MSG, P_LOG,
	    "Error from DHS : `%s'." );
msgText( cStoStatus, E_MULTIPLE_REQUEST, P_LOG,
	    "More than one request qualified to be re-used for the "
	    "user request named '%s'." );
msgText( cStoStatus, E_NO_DEVICE, P_LOG | P_DEBUG_MIN,
	    "No device available to write media type `%s'." );
msgText( cStoStatus, E_NO_MEDIA_TYPE, P_LOG | P_HEALTH_BAD,
	    "There is no media type in the database with the name `%s'." );
msgText( cStoStatus, E_NO_ROLLBACK_PREP, P_LOG | P_ALERT,
	    "There are no units to rollback PREPARE for media Id `%d'. " );
msgText( cStoStatus, E_NO_ROLLBACK_QUEUE, P_LOG | P_ALERT,
	    "There are no units to rollback QUEUE for media Id `%d'. " );
msgText( cStoStatus, E_NO_ROLLBACK_WRITE, P_LOG | P_ALERT,
	    "There are no units to rollback WRITE for media Id `%d'. " );
msgText( cStoStatus, E_NOTIFY_ARCH, P_LOG | P_HEALTH_WARN,
	    "Notification for an archive request failed. The response was:\n%s" );
msgText( cStoStatus, E_NOTIFY_USER, P_LOG | P_HEALTH_WARN,
	    "Notification for a user request failed. The response was:\n%s" );
msgText( cStoStatus, E_NUMNODES, P_LOG | P_HEALTH_WARN,
	    "Error - Configured number of nodes is not an integer - `%s'. " );
msgText( cStoStatus, E_PARAM_TYPE, P_LOG,
	    "Error processing command `%s'.\n"
	    "Incorrect data type on parameter `%s'." );
msgText( cStoStatus, E_PATH, P_LOG,
	    "Configured working directory not useable: `%s'." );
msgText( cStoStatus, E_PREP_LOST_UNIT, P_LOG | P_HEALTH_WARN,
	    "After the Prepare process was run for unit `%s', the unit was "
	    "un-findable in the database. Please investigate." );
msgText( cStoStatus, E_PREP_ROLLBACK, P_LOG,
	    "After the prepare rollback was run for unit `%s', the unit was "
	    "NOT rolled back. Please investigate." );
msgText( cStoStatus, E_PUT_TYPE, P_LOG,
	    "A put for file id `%s' was received with the \n"
	    "unsupported put type of `%d'." );
msgText( cStoStatus, E_REQUEST_OVERFLOW, P_LOG | P_HEALTH_WARN,
	    "Warning - The number of requests exceeds the size of the EPICS "
	    "request array." );
msgText( cStoStatus, E_REUSE_REQ_STATUS, P_LOG,
	    "Cannot reuse the request named '%s' because it has status `%c'" );
msgText( cStoStatus, E_STAGES_OVERFLOW, P_LOG | P_HEALTH_BAD,
	    "Error - The number of staging directories exceeds the size "
	    "of the\nEPICS staging directory array." );
msgText( cStoStatus, E_WRITE_ROLLBACK, P_LOG,
	    "After the write rollback was run for unit `%s', the unit was "
	    "NOT rolled back. Please investigate." );
msgText( cStoStatus, E_PRESS, P_LOG | P_HEALTH_BAD,
	    "Error in the cadc press library, handling file `%s'." );
msgText( cStoStatus, E_PTHREAD, P_LOG,
	    "Failed to initialize event queue processing thread." );
msgText( cStoStatus, E_RMDIR, P_LOG | P_HEALTH_WARN,
	    "Error - could not remove directory `%s'." );
msgText( cStoStatus, E_STA, P_LOG | P_HEALTH_WARN,
	    "Error - Status Server could not set EPICS record: `%s'." );
msgText( cStoStatus, E_STARTUP_CMD, P_LOG | P_HEALTH_WARN,
	    "Error - Startup command not completed. Return string from "
	    "Data Server : `%s'." );
msgText( cStoStatus, E_UNABLE_TO_CONN, P_LOG | P_HEALTH_WARN | P_ALERT,
	    "Unable to connect to `%s'." );
msgText( cStoStatus, E_UNIT_NOT_CLEANED_UP, P_LOG | P_ALERT,
	    "After the cleanup process was run for unit `%s', the unit was "
	    "NOT cleaned up. Please investigate." );
msgText( cStoStatus, E_UNIT_NOT_PREPARED, P_LOG | P_ALERT,
	    "After the Prepare process was run for unit `%s', the unit was "
	    "NOT prepared. Please investigate." );
msgText( cStoStatus, E_UNIT_NOT_WRITTEN, P_LOG | P_ALERT,
	    "After the Write process was run for unit `%s', the unit was "
	    "NOT written. Please investigate." );
msgText( cStoStatus, E_UNIT_OVERFLOW, P_LOG | P_HEALTH_WARN,
	    "Warning - The number of units exceeds the size of the EPICS "
	    "units array." );
msgText( cStoStatus, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsStorage [-V] [-debug {NONE|MIN|FULL}] [-simulate]" );
msgText( cStoStatus, E_UNWANTED_PUT, P_LOG,
	    "A put was received containing data for label `%s', \n"
	    "which is not are required data label." );
msgText( cStoStatus, E_USER_APP_OPTIONS, P_LOG | P_HEALTH_WARN, 
	    "Only one set of options is permitted for each AM application." );
msgText( cStoStatus, E_WRITE_LOST_UNIT, P_LOG | P_ALERT | P_HEALTH_WARN,
	    "After the Write process was run for unit `%s', the unit was "
	    "un-findable in the database. Please investigate." );

msgPropertyDef( cStoStatus, P_ALERT, cStoStatus::alert );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStatus::alert
//
// INVOCATION:
// N.A. Called automatically for messages with the alert status enabled.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (cStoStatus &) The status that caused the callback
// (>) prop (cDhsStatus::cStatusProperty &) The property that caused the 
//		callback
// (>) string (const char *) The string being set.
//
// FUNCTION VALUE:
// (bool) True if processing should continue (always true).
//
// PURPOSE:
// Send an alert message to the EPICS database.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoStatus::alert
(
    cDhsStatus	&,		// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
    		&,		// (in)  The property that caused the callback.
    const char	*string		// (in)  The string being set.
)
{
    cStoStatus	thisStatus;


    thisStatus.update( "alert", string, thisStatus );
    thisStatus.flush( thisStatus );


    return( true );
}
