static char rcsid[] = "$Id: msg.C,v 1.1.1.1 2002-11-24 20:25:30 brighton Exp $";
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
// dhsCmd/src/msg.C
//
// PURPOSE:
// Contains the definitions of the status messages.
//
// METHOD NAME(S)
// None.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.12  1998/08/04 21:52:02  nhill
// Fixed some bugs.
//
// Revision 1.11  1998/06/28 22:21:03  nhill
// Added some messages. Renamed some messages.
//
// Revision 1.10  1998/06/26 18:59:35  nhill
// Updated and added some messages.
//
// Revision 1.9  1998/05/21 17:31:14  jaeger
// Added CMD_SEND error message.
//
// Revision 1.8  1998/05/21 17:02:56  jaeger
// Added CMD_APPLY error message.
//
// Revision 1.7  1998/04/16 19:11:55  nhill
// Updated to the newest version of the cDhsStatus class.
// Removed the cCmdStatus::msgDisplay method.
// Removed the cCmdStatus::info method.
//
// Revision 1.6  1998/03/13 21:50:47  jaeger
// Added some error messages for Connecting, disconnecting, testing and
// forwarding of commands.
//
// Revision 1.5  1997/12/03 20:00:39  jaeger
// Added checking of attribute dimensions for command values.
//
// Revision 1.4  1997/10/03 23:01:40  nhill
// Added check to prevent display of empty messages.
//
// Revision 1.3  1997/09/05 18:56:06  nhill
// Fixed the version display.
//
// Revision 1.2  1997/06/13 20:46:19  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.1  1997/04/16 21:50:14  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_COMMAND
#define	DHS_NO_BULK
#define	DHS_NO_SERVER
#include "iostream.h"

#include "dhs++.H"

#include "globals.H"
#include "cmdSubsystem.H"

msgText( cCmdStatus, S_CMD_RECEIVED, P_LOG | P_DEBUG_MIN,
	    "%s command received from `%s' on `%s'." );
msgText( cCmdStatus, S_CONNECT_REQ, P_LOG | P_DEBUG_MIN,
	    "Connection request received from `%s' on `%s'." );
msgText( cCmdStatus, S_CONNECTED, P_LOG | P_DEBUG_MIN,
	    "Successfully connected to server %s on %s." );
msgText( cCmdStatus, S_DEBUG_LEVEL, P_LOG,
	    "Debug level set to `%s'." );
msgText( cCmdStatus, S_DISCONNECT_REQ, P_LOG | P_DEBUG_MIN,
	    "Disconnect request received from `%s' on `%s'." );
msgText( cCmdStatus, S_DISCONNECTING, P_LOG | P_DEBUG_MIN,
	    "Disconnecting from server %s on %s." );
msgText( cCmdStatus, S_EXITING, P_LOG,
	    "Command server exiting." );
msgText( cCmdStatus, S_RESET_HEALTH, P_LOG,
	    "Command server Health  set to `%s'." );
msgText( cCmdStatus, S_SIMULATE_LEVEL, P_LOG,
	    "Simulation level set to `%s'." );
msgText( cCmdStatus, S_TEST_FAILURE, P_LOG | P_HEALTH_BAD,
	    "Test command failed." );
msgText( cCmdStatus, S_TEST_SUCCESS, P_LOG | P_DEBUG_MIN,
	    "Test command succeeded." );
msgText( cCmdStatus, S_TESTING, P_LOG | P_DEBUG_MIN,
	    "Testing the command server." );
msgText( cCmdStatus, S_VERSION, P_NULL,
	    "%s\n%s" );
msgText( cCmdStatus, E_ABORTABLE, P_LOG,
	    "Attempt to abort command `%s' which is not abortable." );
msgText( cCmdStatus, E_ATTRIB_DIMS, P_LOG,
	"Dimensions of attribute `%s' is greater than 1.");
msgText( cCmdStatus, E_ATTRIB_MISSING, P_LOG,
	    "Required attribute `%s' missing from command `%s'." );
msgText( cCmdStatus, E_ATTRIB_NAME, P_LOG,
	    "Unrecognized attribute `%s' in command `%s'." );
msgText( cCmdStatus, E_ATTRIB_OPTIONAL, P_LOG,
	    "Unrecognized optional flag `%s' for attribute `%s' of command `%s'." );
msgText( cCmdStatus, E_ATTRIB_TYPE, P_LOG,
	    "Unexpected type `%s' for attribute `%s' of command `%s'." );
msgText( cCmdStatus, E_CMD_APPLY, P_LOG,
	    "DHS error %d:  while sending command." );
msgText( cCmdStatus, E_CMD_SEND, P_LOG,
	    "Error %s occured while sending a command." );
msgText( cCmdStatus, E_CMD_NAME, P_LOG,
	    "Unrecognized command: `%s'." );
msgText( cCmdStatus, E_CONNECTING, P_LOG | P_HEALTH_BAD,
	    "Unsuccessfully connected to server %s on %s." );
msgText( cCmdStatus, E_DEBUG_CMD, P_LOG,
	    "Invalid debugLevel command: `%s'" );
msgText( cCmdStatus, E_DUP_ATTRIB, P_NULL,
	    "Duplicate attribute `%s' for command `%s'." );
msgText( cCmdStatus, E_DUP_CMD, P_NULL,
	    "Duplicate command `%s'." );
msgText( cCmdStatus, E_FORWARD_CMD, P_LOG,
	    "Error occured while forwarding `%s' command to %s." );
msgText( cCmdStatus, E_INVALID_CMD, P_LOG,
	    "Invalid command: `%s'" );
msgText( cCmdStatus, E_SIMULATE_CMD, P_LOG,
	    "Invalid simulateLevel command: `%s'" );
msgText( cCmdStatus, E_SUBSYSTEM, P_LOG,
	    "Error in the subsystem library function:\n%s\n%s" );
msgText( cCmdStatus, E_SUBSYSTEM_NAME, P_LOG,
	    "Unknown subsystem name: `%s'." );
msgText( cCmdStatus, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsCmd [-V] [-debug {NONE|MIN|FULL}] [-simulate]");
