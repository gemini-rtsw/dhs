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
// dhsHistory/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS Quick Look Server.
//
// CLASSES:
// cHisStatus - Status variable class for the Quick Look Server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:22  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1998/07/31 20:53:56  cockayne
// Modified comments.
//
// Revision 1.6  1998/06/23 19:23:46  cockayne
// Added #defining HIS_CMD_RESETHEALTH and HIS_RSP_ERROR_HLW_IN_PROGRESS.
// Added S_HIS_LOG_WRITE_BEGIN S_HIS_LOG_WRITE_COMPLETE S_LOST_CONN S_NOT_READY,
// and S_SHUTDOWN status message id's.
//
// Revision 1.5  1998/05/05 23:43:20  cockayne
// Added new attribute for return a-v list from put.
//
// Revision 1.4  1998/04/24 22:20:38  nhill
// Changed file dhsGenSta.H to dhsSta.H.
//
// Revision 1.3  1998/04/17 22:49:55  nhill
// Moved the checking macros to the dhsGen library.
// Removed the info and msgDispay methods.
//
// Revision 1.2  1997/10/20 21:47:48  cockayne
// Check-in for install.
//
// Revision 1.1  1997/10/06 20:16:17  cockayne
// Initial revision
//
// Revision 1.3  1997/08/26 21:53:25  nhill
// Added the E_UNKNOWN_COMMAND status.
//
// Revision 1.2  1997/08/20 21:18:50  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <stdarg.h>


//
// Define some DHS parameters.
//

#define	DHS_NO_BULK
#define	DHS_NO_COMMAND
#define	DHS_NO_SERVER

#include "dhs++.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
}

#ifdef HAVE_BOOL_H
#include <bool.h>
#endif

#include "dhsSta.H"
#include "dhsGenTmp.H"


//
// Define application identification strings. 
//

#define	APPLICATION		"dhsHistory"
#define	PREFIX			"HIS"
#define	MSG_PREFIX		"his"


//
// Define strings to pass to status server.
//

#define HIS_STATE		"state"
#define HIS_STATE_RUNNING	"RUNNING"
#define HIS_STATE_TESTING	"TESTING"
#define HIS_STATE_SHUTDOWN	"SHUTTINGDOWN"
#define HIS_STATE_INITIALIZE	"INITIALIZING"

#define HIS_HEALTH		"health"
#define	HIS_HEALTH_GOOD		"GOOD"
#define	HIS_HEALTH_WARNING	"WARNING"
#define	HIS_HEALTH_BAD		"BAD"

#define HIS_DEBUG		"debug"

#define HIS_SIMULATE		"simulate"


//
// Define commands accepted by History Server. 
//

#define HIS_CMD_EXIT		"exit"
#define HIS_CMD_TEST		"test"
#define HIS_CMD_INIT		"init"
#define HIS_CMD_RESET		"reset"
#define HIS_CMD_RESETHEALTH	"resetHealth"
#define HIS_CMD_HISLOGWRITE	"hisLogWrite"
#define HIS_CMD_PING		"ping"
#define HIS_CMD_SIMLEVEL	"simulateLevel"
#define HIS_CMD_DEBUGLEVEL	"debugLevel"
#define HIS_CMD_CMDABORT	"cmdAbort"



//
// Define configuration strings. 
//

#define	HIS_CFG_CHANNEL		"channel"
#define HIS_CFG_DIRECTORY	"directory"
#define HIS_CFG_DATABASE	"database"
#define HIS_CFG_IDENTITY	"identity"
#define HIS_CFG_IMP		"imp"
#define HIS_CFG_DATASERVER	"dataServer"


//
// Define test failure strings.
//

#define HIS_CATEST_FAIL_MSG	"Channel Access Test Failed. "
#define HIS_DBTEST_FAIL_MSG	"Database Test Failed. "
#define HIS_DISKTEST_FAIL_MSG	"Disk Area Test Failed. "


//
// Define DHS response messages.
//

#define HIS_RSP_WORKING_MSG	"Working"
#define HIS_RSP_UNREC_CMD_MSG	"Unrecognized Command"
#define HIS_RSP_BAD_LEVEL_MSG	"Invalid level"
#define HIS_RSP_NO_LEVEL_MSG	"No level attribute"
#define HIS_RSP_ALREADY_HLW_MSG	"Already processing a hisLogWrite command"
#define HIS_RSP_HLW_CANCEL_MSG						\
		"hisLogWrite command over-ridden by exit or init command"
#define HIS_RSP_ERROR_HLW_MSG						\
		"An error occurred processing hisLogWrite command"
#define HIS_RSP_ERROR_HLW_IN_PROGRESS					\
		"Can't reset health - currently processing hisLogWrite command"


//
// Define hisLogWrite command processing steps.
//

#define HLW_ALL_STEPS		1
#define HLW_STEP_GETLABEL	1
#define HLW_STEP_RENAME		1
#define HLW_STEP_INGEST		2
#define HLW_STEP_FITS		3
#define HLW_STEP_PUT		4


//
// Define DHS command attribute strings. 
//

#define HIS_ATTR_LEVEL		"level"
#define HIS_ATTR_NEWLABEL	"datasetName"


//
// Define debug strings.
//

#define HIS_DEBUG_LEVEL_NONE	"NONE"
#define HIS_DEBUG_LEVEL_MIN	"MIN"
#define HIS_DEBUG_LEVEL_FULL	"FULL"


//
// Define simulate strings.
//

#define HIS_SIM_LEVEL_NONE	"NONE"
#define HIS_SIM_LEVEL_VSM	"VSM"
#define HIS_SIM_LEVEL_FAST	"FAST"
#define HIS_SIM_LEVEL_FULL	"FULL"


//
// Define file naming strings. 
//

#define	FNAME_HISTORYLOG	"historyLog.raw"
#define FNAME_SUFFIX_RAW	"raw"
#define FNAME_SUFFIX_FITS	"fits"


//
// Definitions for cfitsio routines. 
//

#define HCF_BITPIX		32
#define HCF_NAXIS		0

#define HCF_INSTR_KWD		"INSTRUME"
#define HCF_INSTR_VAL		"DHS"
#define HCF_INSTR_COM		"Instrument with which data was taken."

#define HCF_TSCOP_KWD		"TELESCOP"
#define HCF_TSCOP_VAL		"GEMINI NORTH"
#define HCF_TSCOP_COM		"Telescope with which data was taken."

#define HCF_DATE_COLNUM		1
#define HCF_NSEC_COLNUM		2
#define HCF_LABEL_COLNUM	3
#define HCF_VALUE_COLNUM	4

//
//***********************************************************************
//+
// CLASS NAME:
// cHisStatus
//
// PUBLIC METHODS: 
// (static void)configInit	- Read information from a cofiguration file.
//
// PURPOSE:
// Contains a status value and defines the status codes.
//
//-
//***********************************************************************
//

class 	cHisStatus: public cStaStatus
{
    public:
	msgPrefix( MSG_PREFIX );
	msgId( S_COMMAND_RECEIVED );	// A command was received.
	msgId( S_CONNECT_REQ );		// Connection request.
	msgId( S_CONFIG_READ );		// Reading configuration.
	msgId( S_DEBUG_LEVEL );		// Debug level set by command.
	msgId( S_DEBUG_MSG );		// Debug message from dhs. 
	msgId( S_DISCONNECT_REQ );	// Disconnect request.
	msgId( S_EXITING );		// Program is exiting.
	msgId( S_FREMOVE );		// Report on files user should remove.
	msgId( S_HIS_LOG_WRITE_BEGIN );	// Beginning hisLogWrite command.
	msgId( S_HIS_LOG_WRITE_COMPLETE);// Completed hisLogWrite command.
	msgId( S_INITIALIZING );	// Program is initializing.
	msgId( S_LOST_CONN );		// Connection lost. 
	msgId( S_NOT_READY );		// Not ready to process commands.
	msgId( S_RECOVERING );		// Program is recovering from 
					// previously unfinished hisLogWrite.
	msgId( S_RESET );		// Resetting server.
	msgId( S_RUNNING );		// Program is running.
	msgId( S_SHUTDOWN );		// Shutting down.
	msgId( S_SIMULATE_LEVEL );	// Simulate level set by command.
	msgId( S_TEST_FAILURE );	// Test command resulted in failure.
	msgId( S_TEST_SUCCESS );	// Test command resulted in success.
	msgId( S_VERSION );		// Print version number.

	msgId( E_ABORT );		// Command abort not permitted.
	msgId( E_CA );			// Error in Channel Access routine. 
	msgId( E_CALLBACK );		// Error in CA callback routine. 
	msgId( E_CFITSIO );		// Error in cfitsio routine. 
	msgId( E_CHANNEL );		// Error connecting to EPICS channel.
	msgId( E_CMD_FORMAT );		// Invalid command format
	msgId( E_DATASERVER_MSG );	// Error message from dataServer. 
	msgId( E_DB );			// Error in the SYBASE db library.
	msgId( E_DB_LIB );		// Error in the CADC db library. 
	msgId( E_DEBUG_COMMAND );	// Invalid debug command.
	msgId( E_DUPLICATE_CHANNEL );	// Duplicate channel in config file.
	msgId( E_FCLOSE );		// Error closing file. 
	msgId( E_FOPEN );		// Error opening file. 
	msgId( E_FREAD );		// Error reading file.
	msgId( E_FREMOVE );		// Error removing file. 
	msgId( E_FRENAME );		// Error renaming file. 
	msgId( E_FSEEK );		// Error seeking within file. 
	msgId( E_FSTAT );		// Error stat-ing file. 
	msgId( E_FWRITE );		// Error writing file. 
	msgId( E_HISLOGWRITE );		// Hislogwrite cmd refused.
	msgId( E_MSG );			// Message from dhs. 
	msgId( E_NEWLABEL );		// Report on re-labelled file.
	msgId( E_PATH );		// Path is not valid. 
	msgId( E_PTHREAD );		// Pthread creation failed. 
	msgId( E_PUT );			// Error put-ing file to dataServer.
	msgId( E_RUNNING );		// Another HS running on this system.
	msgId( E_SIMULATE_COMMAND );	// Invalid simulate command.
	msgId( E_UNKNOWN_COMMAND );	// Unrecognized command received.
	msgId( E_USAGE );		// Command line usage error.
	msgId( E_WORKING_DIR );		// Couldn't open the working dir.
    protected:
    private:
};


//
// Declare the external functions used by the server.
//

extern void	configInit( cHisStatus & );
extern void	argInit( int, char *[], cHisStatus & );
#endif // GLOBALS_H
