static char rcsid[] = "$Id: msg.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsData/src/msg.C
//
// PURPOSE:
// To handle the formation and printing of messages.
//
// METHOD NAME(S)
// None.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:30:07  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:49:42  nhill
// Updated to use the latest version of the cDhsStatus class.
// Removed the cSimStatus::info, cSimStatus::msgDebug and c
// cSimStatus::msgDisplay methods.
//
// Revision 1.1  1997/10/03 23:07:10  cockayne
// Initial revision
//
// Revision 1.1  1997/07/10 18:27:55  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>

#include "dhs++.H"

#include "globals.H"
	msgText( cSimStatus, 	S_CMD_OLDPS, P_NULL,
	    "Received `%s' command from `%s' on %s" );
	msgText( cSimStatus, 	S_CMD_OLDPS_AV, P_NULL,
	    "Attribute `%s' had value `%s'" );
	msgText( cSimStatus, 	S_CMD_QLS, P_NULL,
	    "Received `%s' command from `%s' on %s" );
	msgText( cSimStatus, 	S_CMD_QLS_AV, P_NULL,
	    "Attribute `%s' had value `%s'" );
	msgText( cSimStatus, 	S_CMD_SS, P_NULL,
	    "Received `%s' command from `%s' on %s" );
	msgText( cSimStatus, 	S_CMD_SS_AV, P_NULL,
	    "Attribute `%s' had value `%s'" );
	msgText( cSimStatus,  S_CONFIG_OPEN, P_NULL,
	    "Configuration file\n`%s'\nhas been opened." );
	msgText( cSimStatus,	S_CONNECT_REQ, P_DEBUG_MIN,
	    "Connection request from `%s' on %s" );
	msgText( cSimStatus,	S_DEBUG_MSG, P_NULL,
	    "Debug message: `%s'" );
	msgText( cSimStatus,	S_DISCONNECT_REQ, P_DEBUG_MIN,
	    "Disconnection request from `%s' on %s" );
	msgText( cSimStatus, S_PUT, P_NULL,
	    "Received file `%s', lastFlag = `%c', from `%s' on %s" );
	msgText( cSimStatus,	S_VERSION, P_NULL,
	    "%s" );
	msgText( cSimStatus,	E_CONTROL_C, P_NULL,
	    "Control-C was pressed, exiting." );
	msgText( cSimStatus,	E_FALSE, P_NULL,
	    "Function unsuspectedly returned FALSE:\n%s" );
	msgText( cSimStatus,	E_FILE_OPEN, P_NULL,
	    "Error opening file `%s' for writing :" );
	msgText( cSimStatus,	E_FILE_WRITE, P_NULL,
	    "Error writing file `%s' :" );
	msgText( cSimStatus,	E_INVALID_COMMAND, P_NULL,
	    "Error, Invalid command: `%s' received from `%s' on %s" );
	msgText( cSimStatus,	E_INVALID_PUT, P_NULL,
	    "Error, Invalid put: `%s' received from `%s' on %s" );
	msgText( cSimStatus,	E_INVALID_PUT_LABEL, P_NULL,
	    "Error, Invalid put label received from `%s' on %s" );
	msgText( cSimStatus,	E_USAGE, P_NULL,
	    "Usage:\nNobody should be using this, so this is an error." );
