static char rcsid[] = "$Id: msg.C,v 1.1.1.1 2002-11-24 20:29:02 brighton Exp $";
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
// dhsQlServer/src/msg.C
//
// PURPOSE:
// Contains code for the QLS messages.
//
// METHOD NAME(S)
// None.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.14  1999/01/20 22:59:37  nhill
// Added the E_DS_GET message.
//
// Revision 1.13  1998/08/04 23:12:41  nhill
// Changed the logging of some messages and added some new messages.
//
// Revision 1.12  1998/06/16 22:03:29  nhill
// Changed slave disconnect messages to only display when debug level is MIN.
//
// Revision 1.11  1998/04/27 21:06:51  nhill
// Added some new messages.
//
// Revision 1.10  1998/04/15 22:35:47  nhill
// Removed the cQlsStatus::info and cQlsStatus::msgDisplay methods.
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.9  1997/12/04 23:51:57  nhill
// Added the E_DATASET_LAST message.
//
// Revision 1.8  1997/11/18 23:11:00  nhill
// Fixed the usage message.
//
// Revision 1.7  1997/10/22 17:42:11  nhill
// Added some new messages.
//
// Revision 1.6  1997/10/01 17:22:07  nhill
// Updated message text.
//
// Revision 1.5  1997/09/02 16:12:28  nhill
// fixed a typo.
//
// Revision 1.4  1997/08/27 22:06:28  nhill
// Fixed some message strings.
//
// Revision 1.3  1997/08/26 21:50:59  nhill
// Fixed the version number message.
// Added the E_UNKNOWN_COMMAND status.
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
// Define the dhsQlServer message strings.
//

msgText( cQlsStatus, S_CHUNK_COMP, P_DEBUG_MIN,
	    "Chunk for dataset %s completed." );
msgText( cQlsStatus, S_CHUNK_REC, P_DEBUG_MIN | P_LOG,
	    "Chunk for dataset %s received." );
msgText( cQlsStatus, S_CLIENT_CON, P_LOG | P_DEBUG_MIN,
	    "QLT `%s' on `%s' connected to QLS." );
msgText( cQlsStatus, S_CLIENT_DISCON, P_LOG | P_DEBUG_MIN,
	    "QLT `%s' on `%s' disconnected from QLS." );
msgText( cQlsStatus, S_CLIENT_SUBSC, P_DEBUG_MIN,
	    "QLT `%s' subscribed to stream `%s'." );
msgText( cQlsStatus, S_CON_REQ, P_LOG | P_DEBUG_MIN,
	    "Connection request received from `%s' on `%s'." );
msgText( cQlsStatus, S_DATASET, P_LOG | P_DEBUG_MIN,
	    "Starting new dataset `%s'." );
msgText( cQlsStatus, S_DATASET_ABORT, P_LOG | P_DEBUG_MIN,
	    "Aborting dataset `%s'." );
msgText( cQlsStatus, S_DEBUG_LEVEL, P_LOG | P_DEBUG_MIN,
	    "Debug level set to `%s'." );
msgText( cQlsStatus, S_DISCON_REQ, P_LOG | P_DEBUG_MIN,
	    "Disconnect request received from `%s' on `%s'." );
msgText( cQlsStatus, S_DS_GET, P_DEBUG_MIN,
	    "Dataset get for dataset `%s' received from qlt %s on %s." );
msgText( cQlsStatus, S_DS_STREAMS, P_DEBUG_MIN,
	    "Adding stream `%s' to dataset `%s'." );
msgText( cQlsStatus, S_EXIT_CMD, P_LOG | P_DEBUG_MIN,
	    "Received exit command." );
msgText( cQlsStatus, S_EXITING, P_LOG | P_DEBUG_MIN,
	    "Quick Look Server exiting." );
msgText( cQlsStatus, S_INIT, P_LOG | P_DEBUG_MIN,
	    "Initializing Quick Look Server." );
msgText( cQlsStatus, S_PING_CMD, P_LOG | P_DEBUG_MIN,
	    "Ping command received." );
msgText( cQlsStatus, S_QLS_NOTIFY, P_DEBUG_MIN,
	    "QLS `%s' forwarded data for dataset `%s'." );
msgText( cQlsStatus, S_QLT_NOTIFY, P_DEBUG_MIN,
	    "Notified QLT `%s' of a change in dataset `%s' frame `%s'." );
msgText( cQlsStatus, S_QUERY_STREAMS, P_LOG | P_DEBUG_MIN,
	    "Received query streams command from `%s' on `%s'." );
msgText( cQlsStatus, S_RESET, P_LOG | P_DEBUG_MIN,
	    "Resetting Quick Look Server." );
msgText( cQlsStatus, S_RESET_HEALTH, P_LOG | P_DEBUG_MIN,
	    "Resetting Health state." );
msgText( cQlsStatus, S_SIM_LEVEL, P_LOG,
	    "Simulation level set to `%s'." );
msgText( cQlsStatus, S_SLAVE_CON, P_LOG | P_DEBUG_MIN,
	    "Slave QLS `%s' on `%s' connected to QLS." );
msgText( cQlsStatus, S_SLAVE_DISCON, P_LOG | P_DEBUG_MIN,
	    "Slave QLS `%s' on `%s' disconnected from QLS." );
msgText( cQlsStatus, S_SLAVE_SUBSC, P_LOG | P_DEBUG_MIN,
	    "Slave QLS `%s' subscribed to stream `%s'." );
msgText( cQlsStatus, S_TEST_CMD, P_LOG | P_DEBUG_MIN,
	    "Test command received." );
msgText( cQlsStatus, S_TEST_FAIL, P_LOG | P_HEALTH_BAD,
	    "Test command failed." );
msgText( cQlsStatus, S_TEST_SUC, P_LOG | P_DEBUG_MIN,
	    "Test command succeeded." );
msgText( cQlsStatus, S_VERSION, P_NULL,
	    "%s\n%s" );

msgText( cQlsStatus, E_BUF_OVER, P_LOG,
	    "Frame %s overflowed its image buffer." );
msgText( cQlsStatus, E_CARDNUM, P_LOG,
	    "Attempt to overwrite invalid card image number %d in \n"
	    "frame %s of dataset %s." );
msgText( cQlsStatus, E_CFITSIO, P_LOG,
	    "Error in a cfitsio routine:\n%s\n%s" );
msgText( cQlsStatus, E_CHUNK_MATCH, P_LOG,
	    "Attributes for a chunk don't match attributes for dataset `%s'.\n"
	    "The chunk will be ignored." );
msgText( cQlsStatus, E_CMD_FORMAT, P_LOG,
	    "Invalid command format for command `%s'." );
msgText( cQlsStatus, E_CMD_NAME, P_LOG,
	    "Unrecognized command: `%s'." );
msgText( cQlsStatus, E_COMMAND, P_LOG | P_HEALTH_WARN,
	    "ICD 1c '%s' command failed." );
msgText( cQlsStatus, E_CONNECTION, P_LOG | P_HEALTH_WARN,
	    "Connection to `%s' on `%s' has failed." );
msgText( cQlsStatus, E_DS_GET, P_LOG,
	    "Failed to retrieve dataset `%s' Response message was:\n%s" );
msgText( cQlsStatus, E_DS_LAST, P_LOG,
	    "Last flag has alread been seen for dataset `%s'.\n"
	    "This chunk will be ignored." );
msgText( cQlsStatus, E_DEBUG_CMD, P_LOG,
	    "Invalid debugLevel command: `%s'" );
msgText( cQlsStatus, E_DUP_FITS_KW, P_LOG,
	    "Duplicate FITS keyword in config file: %s %d" );
msgText( cQlsStatus, E_INVALID_CMD, P_LOG,
	    "Invalid command: `%s'" );
msgText( cQlsStatus, E_NAXIS, P_LOG,
	    "Cannot display an image with %d axes for dataset `%s'." );
msgText( cQlsStatus, E_NO_MASTER, P_LOG,
	    "No master Quick Look Server was defined in the configuration "
	    "file" );
msgText( cQlsStatus, E_QLS, P_LOG | P_HEALTH_WARN,
	    "Could not find cQlsSlave object matching connection %d." );
msgText( cQlsStatus, E_QLT, P_LOG | P_HEALTH_WARN,
	    "Could not find cQlsQlt object matching connection %d." );
msgText( cQlsStatus, E_RUNNING, P_NULL,
	    "Another Quick Look server is running on this system." );
msgText( cQlsStatus, E_SEND, P_NULL,
	    "%s command to the master QLS failed." );
msgText( cQlsStatus, E_SIM_CMD, P_LOG,
	    "Invalid simulateLevel command: `%s'" );
msgText( cQlsStatus, E_SLAVE_TEST, P_LOG,
	    "Slave `%s' on `%s' failed it's test. Test response is:\n%s" );
msgText( cQlsStatus, E_UNKNOWN_CMD, P_LOG,
	    "Unknown command `%s' received from `%s' on `%s'" );
msgText( cQlsStatus, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsQlServer [-V] [-debug {NONE|MIN|FULL}] [-simulate] [-master]" 
	    );
