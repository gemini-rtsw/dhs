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
// dhsQlServer/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS Quick Look Server.
//
// CLASSES:
// cQlsStatus - Status variable class for the Quick Look Server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:49  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.12  1999/01/20 23:18:29  nhill
// Added E_DS_GET message.
//
// Revision 1.11  1998/08/04 23:16:12  nhill
// Added some new messages.
//
// Revision 1.10  1998/05/14 20:24:59  nhill
// Added code to ignore non error error status's from cfitsio.
//
// Revision 1.9  1998/04/27 20:51:03  nhill
// Added some new messages.
//
// Revision 1.8  1998/04/15 23:09:36  nhill
// Updated to the newest version of the cDhsStatus class.
// Moved the check, checkSystem, checkConfig, and checkNull macros to the
// dhsGen library.
// Removed cQlsStatus::config, info, msgDisplay and statusServer methods.
//
// Revision 1.7  1997/12/04 23:51:44  nhill
// Added the E_DATASET_LAST message.
//
// Revision 1.6  1997/11/18 23:11:29  nhill
// Changed to application name to dhsQls.
//
// Revision 1.5  1997/10/22 17:46:26  nhill
// Added some new messages.
//
// Revision 1.4  1997/10/01 17:25:43  nhill
// Added the S_QUERY_STREAMS status.
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

#include "genBool.H"

#include "dhsSta.H"
#include "dhsGenTmp.H"

#define	APPLICATION	"dhsQls"
#define	PREFIX		"QLS"


//
//  Define the status checking macros.
//

#define	checkFitsioNX( fn, cfitsioStatus, status, ret )			\
    {									\
	char	cfitsioErrText[81];					\
	if ( cfitsioStatus != 0 && cfitsioStatus != USE_MEM_BUFF )	\
	{								\
	    fits_get_errstatus( cfitsioStatus, cfitsioErrText );	\
	    status.E_CFITSIO( status, cfitsioErrText, #fn );		\
	    while ( fits_read_errmsg( cfitsioErrText ) != 0 )		\
	    {								\
		status += cfitsioErrText;				\
	    }								\
	    ret;							\
	}								\
    }

#define	checkFitsio( fn, cfitsioStatus, status, ret )			\
    {									\
	fn;								\
	checkFitsioNX( fn, cfitsioStatus, status, ret );		\
    }

class	cQlsSubsystem;

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsStatus
//
// PUBLIC METHODS: 
//
// PURPOSE:
// Contains a status value and defines the status codes.
//
//-
//***********************************************************************
//

class 	cQlsStatus: public cStaStatus
{
    public:
	msgPrefix( "qls" );
	msgId( S_CHUNK_COMP );		// Processing of a chunk is complete.
	msgId( S_CHUNK_REC );		// Processing of a chunk has started.
	msgId( S_CLIENT_CON );		// A Client has connected.
	msgId( S_CLIENT_DISCON );	// A Client has disconnected.
	msgId( S_CLIENT_SUBSC );	// Client subscribes to QL streams.
	msgId( S_CON_REQ );		// Connection request.
	msgId( S_DATASET );		// Creating new dataset.
	msgId( S_DATASET_ABORT );	// Aborted dataset.
	msgId( S_DEBUG_LEVEL );		// Debug level set by command.
	msgId( S_DISCON_REQ );		// Disconnect request.
	msgId( S_DS_GET );		// Dataset get request received.
	msgId( S_DS_STREAMS );		// Setting streams for a dataset
	msgId( S_EXIT_CMD );		// Program received exit command.
	msgId( S_EXITING );		// Program is exiting.
	msgId( S_INIT );		// Program is initalizing.
	msgId( S_PING_CMD );		// Ping command received.
	msgId( S_QLS_NOTIFY );		// Slave QLS has been sent a dataset.
	msgId( S_QLT_NOTIFY );		// QLT has been notified about a frame.
	msgId( S_QUERY_STREAMS );	// Received a query streams command.
	msgId( S_RESET );		// Resetting server.
	msgId( S_RESET_HEALTH );	// Resetting health.
	msgId( S_SIM_LEVEL );		// Simulate level set by command.
	msgId( S_SLAVE_CON );		// Slave QLS connects to QLS.
	msgId( S_SLAVE_DISCON );	// Slave QLS disconnects from QLS.
	msgId( S_SLAVE_SUBSC );		// Slave QLS subscribes to streams.
	msgId( S_TEST_FAIL );		// Test command resulted in failure.
	msgId( S_TEST_SUC );		// Test command resulted in success.
	msgId( S_TEST_CMD );		// Received test command.
	msgId( S_VERSION );		// Print version number.

	msgId( E_BUF_OVER );		// An image overflowed its buffer.
	msgId( E_CARDNUM );		// Invalid fits card image number.
	msgId( E_CFITSIO );		// Error in the cfitsio routines.
	msgId( E_CHUNK_MATCH );		// Chunk parameters don't match.
	msgId( E_CMD_FORMAT );		// Invalid command format
	msgId( E_CMD_NAME );		// Unrecognized command name.
	msgId( E_COMMAND );		// Failed to send a command.
	msgId( E_CONNECTION );		// Lost a connection.
	msgId( E_DS_GET );		// An error ocurred doing a dsget.
	msgId( E_DS_LAST );		// Last flag has been seen for the ds.
	msgId( E_DEBUG_CMD );		// Invalid debug command.
	msgId( E_DUP_FITS_KW );		// Duplicate FITS keyword in the
					// configuration file.
	msgId( E_INVALID_CMD );		// Invalid command received.
	msgId( E_NAXIS );		// Number of axis of image is incorrect.
	msgId( E_NO_MASTER );		// No master server is available.
	msgId( E_QLS );			// Specified quick look slave not found.
	msgId( E_QLT );			// Specified quick look tool not found.
	msgId( E_RUNNING );		// Another QLS is running on this 
					// system.
	msgId( E_SEND );		// Failed to send a command.
	msgId( E_SIM_CMD );		// Invalid simulate command.
	msgId( E_SLAVE_TEST );		// Slave QLS failed its tests.
	msgId( E_UNKNOWN_CMD );		// Unrecognized command received.
	msgId( E_USAGE );		// Command line usage error.
    protected:
    private:
};


//
// Declare a cDhsTmpDir object which will be used to create filenames for
// all of the Quick Look server temporary files.
//

extern cDhsTmpDir	tmpDir;


//
// Declare the external functions used by the server.
//

extern void	configInit( cQlsStatus & );
extern void	argInit( int, char *[], cQlsStatus & );
#endif // GLOBALS_H
