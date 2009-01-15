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
// dhsStorage/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS Storage Server.
//
// CLASSES:
// cStoStatus - Status variable class for the Storage Server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/25 10:32:33  brighton
// added genBool.H
//
// Revision 1.1.1.1  2002/11/24 20:31:17  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.15  2001/03/12 20:40:47  nhill
// Removed some now unused STO_RSP_* messages. Added new status messages
// E_ACTION_INVALID and E_PARAM_TYPE.
//
// Revision 1.14  2000/02/03 00:51:25  nhill
// Added the E_USER_APP_OPTIONS message.
//
// Revision 1.13  2000/01/05 20:40:04  nhill
// Added E_FILE_PATH message.
//
// Revision 1.12  1999/10/12 23:20:07  nhill
// Added some new messages.
//
// Revision 1.11  1999/06/18 22:26:47  nhill
// Added the E_NO_MEDIA_TYPE message.
//
// Revision 1.10  1999/06/16 23:44:25  nhill
// Major upgrade.
//
// Revision 1.9  1999/02/24 23:54:35  jaeger
// Added messages: E_WRITE_LOST_UNIT, E_UNABLE_TO_CONN, and
// E_WRITE_LOST_UNIT.  Added command stoRefresh.
//
// Revision 1.8  1998/06/24 16:05:16  cockayne
// Added msgId's S_DEBUG_MSG S_LOST_CONN S_SHUTDOWN.
//
// Revision 1.7  1998/05/25 19:06:29  cockayne
// Added S_ACTION_COMPLETE, E_FORK_FAILED msgId's, renamed S_START_ACTION and
// S_STOPPING_ACTION to S_ACTION_COMPLETE and S_ACTION_STARTING msgId's,
// added STO_RSP_NO_ROLLBACK_WRITE and STO_RSP_NO_ROLLBACK_QUEUE response
// messages.
//
// Revision 1.6  1998/05/19 16:26:08  cockayne
// Removed S_CONFIG_READ. It's defined in the library.
//
// Revision 1.5  1998/05/15 17:15:16  cockayne
// Modifications include adding command and attribute #defines, and
// mods to response format strings, and status msgId()'s.
//
// Revision 1.4  1998/04/23 16:15:50  cockayne
// Modified to use new status and health.
//
// Revision 1.3  1998/04/08 20:57:02  cockayne
// Added command attributes for user media command. Added response
// string for a specific error in user media request processing,
// Added E_ARRAY_SIZES program status.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <cstdarg>

#include "dhs++.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
}

#include "genBool.H"

#include "dhsSta.H"
#include "dhsGenTmp.H"


//
// Define application identification strings. 
//

#define	APPLICATION		"dhsStorage"
#define	PREFIX			"STO"
#define	MSG_PREFIX		"sto"


//
// Define sequence commands accepted and sent by DHS Storage Server. 
//

#define STO_CMD_EXIT		"exit"
#define STO_CMD_TEST		"test"
#define STO_CMD_INIT		"init"
#define STO_CMD_RESET		"reset"
#define STO_CMD_PING		"ping"
#define STO_CMD_SIMLEVEL	"simulateLevel"
#define STO_CMD_DEBUGLEVEL	"debugLevel"
#define STO_CMD_CMDABORT	"cmdAbort"
#define STO_CMD_RESETHEALTH	"resetHealth"
#define STO_CMD_STARTUP		"storageStart"


//
// Define commands accepted and sent by DHS Storage Server.
// Action and Rollback, media request, notification, informational.  
//

#define STO_CMD_REFRESH			"stoRefresh"
#define STO_CMD_ACTION_REQUEST		"stoRequestAction"
#define	STO_CMD_ACTION_CANCEL		"stoCancelAction"
#define	STO_CMD_ROLLBACK_REQUEST	"stoRequestRollback"
#define STO_CMD_ARCHIVE_FILE		"stoArchiveFileAvailable"
#define	STO_CMD_USER_MEDIA		"stoUserMediaRequest"
#define	STO_CMD_ARCH_NOTIFY		"archive_file_notify"
#define	STO_CMD_USER_NOTIFY		"user_notify"
#define	STO_CMD_INFO			"stoInfo"


//
// Define command-attribute names of commands. 
//

#define	STO_CA_ACTION		"action"
#define STO_CA_COMPRESSION	"compression"
#define STO_CA_COMPRESSION_FLAG	"compressionFlag"
#define	STO_CA_DEVICE_NAME	"logicalDev"
#define	STO_CA_FILE_ID		"fileId"
#define	STO_CA_FILENAME		"fileName"
#define	STO_CA_FILEPATH		"filePath"
#define	STO_CA_FILESIZE		"fileSize"
#define	STO_CA_FORCE		"force"
#define	STO_CA_LEVEL		"level"
#define	STO_CA_MEDIA_REQUEST_ID	"mediaId"
#define	STO_CA_MEDIA_UNIT_NAME	"mediaUnitName"
#define	STO_CA_NUMBER		"number"
#define	STO_CA_REUSE		"reuse"
#define	STO_CA_SIZE		"size"
#define	STO_CA_SEGREGATION_DIR	"segregationDir"

#define	STO_CA_REQUEST_NAME	"requestName"
#define	STO_CA_NOTIFY_CLIENT	"notifyClient"
#define	STO_CA_UNIT_NAME_ROOT	"unitNameRoot"
#define	STO_CA_NUM_COPIES	"numCopies"
#define	STO_CA_MEDIA_TYPE	"mediaType"

#define STO_CA_NUMBERED_FILE_ID			"fileId%d"
#define STO_CA_NUMBERED_FILESIZE		"fileSize%d"
#define STO_CA_NUMBERED_COMPRESSION_FLAG	"compressionFlag%d"
#define STO_CA_NUMBERED_FILEPATH		"filePath%d"

#define	STO_CA_INFO_CMD_TYPE		"type"
#define	STO_CA_DATA			"stoData"
#define	STO_CA_DATA_HEADER		"stoDataHeader"

#define STO_CA_FILE_ID_ARRAY	"fileIdArray"
#define STO_CA_FILE_SIZE_ARRAY	"fileSizeArray"
#define STO_CA_COMP_FLAG_ARRAY	"compFlagArray"
#define STO_CA_COMP_FLAG	"compFlag"
#define STO_CA_FILE_PATH_ARRAY	"filePathArray"


//
// Define command-attribute values of action and rollback commands. 
//

#define STO_CAV_RETRIEVE	"RETRIEVE"
#define STO_CAV_QUEUE		"QUEUE"
#define STO_CAV_PREPARE		"PREPARE"
#define STO_CAV_WRITE		"WRITE"
#define STO_CAV_CLEANUP		"CLEANUP"
#define STO_CAV_NOTIFY		"NOTIFY"

#define STO_CAV_RETRIEVABLE	"RETRIEVABLE"
#define STO_CAV_QUEUEABLE	"QUEUEABLE"
#define STO_CAV_PREPARABLE	"PREPARABLE"
#define STO_CAV_WRITEABLE	"WRITEABLE"
#define STO_CAV_CLEANUPABLE	"CLEANUPABLE"
#define STO_CAV_NOTIFYABLE	"NOTIFYABLE"

#define STO_CAV_COMP_GZIP	'G'	// Represents gzip compression type, 
					// from the ad library. The ad 
					// library is not used by the 
					// storage server, so this is 
					// defined here. 


//
// Define test failure strings.
//

#define STO_DBTEST_FAIL_MSG	"Database Test Failed. "
#define STO_DISKTEST_FAIL_MSG	"Disk Area Test Failed. "


//
// Define DHS response messages.
//

#define STO_RSP_ACTION_ALREADY	"There is already a %s action "		\
				"requested or in-progress for media "	\
				"request ID %d."
#define	STO_RSP_DB_ERR		"Database error in Storage Server."
#define	STO_RSP_DUPLICATE_FILE	"File with file ID: `%s' already archived."
#define	STO_RSP_INFO_ERR	"Error occurred processing info command."
#define	STO_RSP_MEDIAID_INVALID	"Media ID invalid: `%d'."
#define STO_RSP_PING_MSG	"Ping! Ding-a-ling! Wa-bing!"
#define	STO_RSP_NOT_READY	"Server Initializing. Can't do this right now."
#define STO_RSP_CMD_UNRECOGNIZED					\
				"Unrecognized Command: `%s'."
#define STO_RSP_WORKING_MSG	"Working ..."
#define STO_RSP_UNAUTHORIZED	"`%s' at `%s' is not authorized to " 	\
				"send commands to the DHS Storage Server."
#define	STO_RSP_DB_ERROR	"An error has prevented the DHS "	\
				"Storage Server from processing "	\
				"the `%s' command you sent. "
#define STO_RSP_ACTION_UNROLLBACKABLE					\
				"Cannot Rollback - Retrieve, Cleanup, "	\
				"and Notify are not rollback-able. "

//
//***********************************************************************
//+
// CLASS NAME:
// cStoStatus
//
// PUBLIC METHODS: 
// (void)	info		- Send info msg to DHS Status Server.
//
// PURPOSE:
// Contains a status value and defines the status codes.
//
//-
//***********************************************************************
//

class 	cStoStatus: public cStaStatus
{
    public:
	msgPrefix( MSG_PREFIX );
	msgId( S_ACTION_COMPLETE );	// Action Complete message.
	msgId( S_ACTION_DISABLED );	// An action is disabled.
	msgId( S_ACTION_STARTING );	// Action Starting message.
	msgId( S_ACTION_STOPPING );	// Action Stopping message.
	msgId( S_CMD_RECEIVED );	// A command was received.
	msgId( S_CMD_TEST_SUCCESS );	// Test command resulted in success.
	msgId( S_CONNECT_REQ );		// Connection request.
	msgId( S_DEBUG_LEVEL );		// Debug level set by command.
	msgId( S_DEBUG_MSG );		// Debug message from DHS libraries.
	msgId( S_DISCONNECT_REQ );	// Disconnect request.
	msgId( S_DUPLICATE_FILE );	// Duplicate file ignored in a user 
					// media request command.
	msgId( S_EXITING );		// Program is exiting.
	msgId( S_INITIALIZING );	// Program is initializing.
	msgId( S_LOST_CONN );		// Connection Lost.
	msgId( S_NO_CLEANUPABLE );	// No units to cleanup.
	msgId( S_NO_PREPARABLE );	// No units to prepare.
	msgId( S_NO_WRITEABLE );	// No units to write.
	msgId( S_NO_STAGE_DIRECTORIES );// No stage dirs in which to prepare.
	msgId( S_RETRIEVE_LIMIT_REACHED );
	msgId( S_ROLLBACK_DISABLED );	// A rollback is disabled.
					// Retrieve limit reached. 
	msgId( S_RUNNING );		// Program is running.
	msgId( S_SHUTDOWN );		// Shutting down.
	msgId( S_SIMULATE_LEVEL );	// Simulate level set by command.
	msgId( S_VERSION );		// Print version number.

	msgId( E_ACTION_INVALID );	// Invalid action type received.
	msgId( E_ACTION_OVERFLOW );	// Number of actions exceeded EPICS 
					// array size.
	msgId( E_AM );			// Error in the CADC am library.
	msgId( E_ARRAY_SIZES );		// User req cmd array size problem.
	msgId( E_CLEANUP_LOST_UNIT );	// Unit disapeared during cleanup.
	msgId( E_CMD_BAD_ATTR_VALUE );	// Bad value for a command's attribute.
	msgId( E_CMD_CONNECTION );	// Error - invalid connector.
	msgId( E_CMD_FORMAT );		// Invalid command format
	msgId( E_CMD_NO_ATTRIBUTE );	// Command is missing an attribute.
	msgId( E_CMD_PROCESSING );	// Error processing a command. 
	msgId( E_CMD_UNKNOWN );		// Unrecognized command received.
	msgId( E_CFG_CONNECTION );	// Error configuring a connection. 
	msgId( E_CFG_LOCALDATASERVER );	// Error cfg-ing local dataserver conn.
	msgId( E_CFG_NOTIFYDATASERVER ); // Error cfg-ing dataserver conn list.
	msgId( E_DB_LIB );		// Error in the CADC db library. 
	msgId( E_DEV_NOT_BOOKED );	// A device could not be booked.
	msgId( E_DEVICE_FIND );		// Could not find device.
	msgId( E_DEVICE_OVERFLOW );	// Number of logical devices exceeded
					// EPICS array size.
	msgId( E_DUPLICATE_FILE );	// Duplicate file notification.
	msgId( E_EXEC_ABNORMAL_EXIT );	// execle() call terminated abnormally.
	msgId( E_EXEC_NONZERO_EXIT );	// execle'd application exited negative.
	msgId( E_FCLOSE );		// Error closing file. 
	msgId( E_FILE_GET );		// Error Getting a file.
	msgId( E_FILE_PATH );		// Invalid file path in user request.
	msgId( E_FIVESTATE );		// Error Setting a fivestate variable.
	msgId( E_FOPEN );		// Error opening file. 
	msgId( E_FORK_FAILED );		// Error forking off a new process. 
	msgId( E_FPATH );		// Error creating file path. 
	msgId( E_FSIZE );		// Error with file's size. 
	msgId( E_FSTAT );		// Error stat-ing file. 
	msgId( E_FWRITE );		// Error writing file. 
	msgId( E_MEDIA_REQUEST_TYPE );	// Invalid media request type.
	msgId( E_MSG );			// Message from dhs. 
	msgId( E_MULTIPLE_REQUEST );	// Cannot reuse requests when more than
					// one matches.
	msgId( E_NO_DEVICE );		// No device available for writing.
	msgId( E_NO_MEDIA_TYPE );	// No media type with matching name.
	msgId( E_NO_ROLLBACK_PREP );	// Nothing to rollback prep for. 
	msgId( E_NO_ROLLBACK_QUEUE );	// Nothing to rollback queue for. 
	msgId( E_NO_ROLLBACK_WRITE );	// Nothing to rollback write for. 
	msgId( E_NOTIFY_ARCH );		// Notify of an archive request failed.
	msgId( E_NOTIFY_USER );		// Notify of a user request failed.
	msgId( E_NUMNODES );		// Error in config - number of nodes.
	msgId( E_PATH );		// Path is not valid. 
	msgId( E_PARAM_TYPE );		// Invalid parameter type on a command.
	msgId( E_PREP_LOST_UNIT );	// Prepare process 'lost' a unit.
	msgId( E_PREP_ROLLBACK );	// Prepare rollback failed.
	msgId( E_PUT_TYPE );		// Unsupported put type received.
	msgId( E_WRITE_ROLLBACK );	// Write rollback failed.
	msgId( E_PRESS );		// Error in the CADC press library.
	msgId( E_PTHREAD );		// Pthread creation failed. 
	msgId( E_REQUEST_OVERFLOW );	// Number of requests exceeded EPICS 
					// array size.
	msgId( E_REUSE_REQ_STATUS );	// Attempt to reuse a request with an
					// incorrect status.
	msgId( E_RMDIR );		// Error removing directory.
	msgId( E_STA );			// Error setting EPICS record. 
	msgId( E_STAGES_OVERFLOW );	// Number of staging directories 
					// exceeded EPICS array size.
	msgId( E_STARTUP_CMD );		// Error sending startup commands.
	msgId( E_UNABLE_TO_CONN );	// Unable to connect to a server.
	msgId( E_UNIT_NOT_CLEANED_UP );	// Cleanup process did not cleanup unit.
	msgId( E_UNIT_NOT_PREPARED );	// Prep process did not prep unit.
	msgId( E_UNIT_NOT_WRITTEN );	// Write process did not prep unit.
	msgId( E_UNIT_OVERFLOW );	// Number of units exceeded EPICS array
					// size.
	msgId( E_UNWANTED_PUT );	// A put was received containing 
					// unneeded data.
	msgId( E_USAGE );		// Command line usage error.
	msgId( E_USER_APP_OPTIONS );	// Error reading user application 
					// options.
	msgId( E_WRITE_LOST_UNIT );	// Write process 'lost' a unit.

	msgProperty( P_ALERT );		// Property to alert console about a 
					// problem.
    protected:
    private:
	static bool	alert( cDhsStatus &, 
				const cDhsStatus::cStatusProperty &,
				const char * );
};


//
// Declare the external functions used by the server.
//

extern void	configInit( cStoStatus & );
extern void	argInit( int, char *[], cStoStatus & );

// XXX allan: no fork1 on linux        
#ifdef linux
#define fork1 fork
#endif

#endif // GLOBALS_H
