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
// dhsCommand/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS command server.
//
// CLASSES:
// cCmdStatus - Status class for the command server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:08  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.11  1998/06/28 22:21:35  nhill
// Added some messages. Renamed some messages.
//
// Revision 1.10  1998/06/26 19:01:33  nhill
// Added a message.
//
// Revision 1.9  1998/05/21 17:30:44  jaeger
// Added CMD_SEND error message.
//
// Revision 1.8  1998/05/21 17:03:12  jaeger
// Added CMD_APPLY error message.
//
// Revision 1.7  1998/04/24 22:09:03  nhill
// Changed file dhsGenSta.H to dhsSta.H.
//
// Revision 1.6  1998/04/16 22:48:47  nhill
// Updated to use the newest version of the cDhsStatus class.
//
// Revision 1.5  1998/03/13 22:10:48  jaeger
// Added messages for connecting, resetHealh, testing and forwarding commands.
//
// Revision 1.4  1997/12/03 20:01:20  jaeger
// Added checking of attribute dimensions for command values.
//
// Revision 1.3  1997/06/13 20:46:51  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.2  1997/04/24 19:40:19  nhill
// Changed the application name.
//
// Revision 1.1  1997/04/16 21:51:10  nhill
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

#define	DHS_NO_BULK
#define	APPLICATION	"dhsCommand"
#define	PREFIX		"CMD"
#define DTS_PREFIX	"DTS"
#define STA_PREFIX	"STA"

class	cCmdSubsystem;

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdStatus
//
// PUBLIC METHODS: 
// (type) method - description
//
// PURPOSE:
// Contains a status value.
//
//-
//***********************************************************************
//

class 	cCmdStatus: public cStaStatus
{
    public:
	msgPrefix( "cmd" );
	msgId( S_CMD_RECEIVED );	// A command was received.
	msgId( S_CONNECT_REQ );		// Connection request.
	msgId( S_CONNECTED );		// Connecting to a server.
	msgId( S_DEBUG_LEVEL );		// Debug level set by command.
	msgId( S_DISCONNECT_REQ );	// Disconnect request.
        msgId( S_DISCONNECTING );	// Disconnecting from a server.
	msgId( S_EXITING );		// Program is exiting.
	msgId( S_RESET_HEALTH );	// Health set by command.
	msgId( S_SIMULATE_LEVEL );	// Simulate level set by command.
	msgId( S_TEST_FAILURE );	// Test command resulted in failure.
	msgId( S_TEST_SUCCESS );	// Test command resulted in success.
	msgId( S_TESTING );		// Testing the command server. 
	msgId( S_VERSION );		// Print version number.

	msgId( E_ABORTABLE );		// Attempt to abort non 
					// abortable command.
        msgId( E_ATTRIB_DIMS );		// Attribute has more than 1 dimension.
	msgId( E_ATTRIB_MISSING );	// Required attribute missing.
	msgId( E_ATTRIB_NAME );		// Unrecognized attribute name.
	msgId( E_ATTRIB_OPTIONAL );	// Invalid attribute optinal flag.
	msgId( E_ATTRIB_TYPE );		// Invalid attribute type.
	msgId( E_CMD_APPLY );		// Error while doing an apply cmd.
	msgId( E_CMD_SEND );		// Error while sending a command.
	msgId( E_CMD_NAME );		// Unrecognized command name.
	msgId( E_CONNECTING );		// Unsuccessfull connecting to a server.
	msgId( E_DEBUG_CMD );		// Invalid debug command.
	msgId( E_DUP_ATTRIB );		// Duplicate attribute in config file.
	msgId( E_DUP_CMD );		// Duplicate command in config file.
	msgId( E_FORWARD_CMD );		// Error forwarding command.
	msgId( E_INVALID_CMD );		// Invalid command received.
	msgId( E_SIMULATE_CMD );	// Invalid simulate command.
	msgId( E_SUBSYSTEM );		// Error in the subsytem lib.
	msgId( E_SUBSYSTEM_NAME );	// Unknown subsystem name.
	msgId( E_USAGE );		// Command line usage error.
    protected:
    private:
};


extern void	configInit( cCmdStatus & );
extern void	argInit( int, char *[], cCmdStatus & );
#endif // GLOBALS_H
