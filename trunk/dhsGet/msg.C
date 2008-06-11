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
//
//INDENT-OFF*
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>

#include "dhs++.H"

#include "globals.H"
	msgText( cGetStatus, S_DEBUG_MSG, P_NULL,
	    "Debug message: %s." );
	msgText( cGetStatus, S_GET_ADD, P_DEBUG_MIN,
	    "Added dataset : `%s'." );
	msgText( cGetStatus, S_GET_DEL, P_DEBUG_MIN,
	    "Removed dataset : `%s'." );
	msgText( cGetStatus, S_NULL_BUFFER, P_NULL,
	    "Null buffer requested for dataset : `%s'." );
	msgText( cGetStatus, S_OPEN_FILE, P_DEBUG_FULL,
	    "File opened for writing : `%s'." );
	msgText( cGetStatus, S_VERSION, P_NULL,
	    "%s" );
	msgText( cGetStatus, S_WRITTEN_FILE, P_DEBUG_MIN,
	    "File `%s' written, size is : `%d'." );
	msgText( cGetStatus, E_BAD_WRITE, P_NULL,
	    "Error writing the file `%s', errno: `%d'." );
	msgText( cGetStatus, E_GET_FAILED, P_NULL,
	    "Error getting datsaet `%s'." );
	msgText( cGetStatus, E_ARGUMENT_LINE, P_NULL,
	    "Error reading `%s' in argument line." );
	msgText( cGetStatus, E_NO_WRITE, P_NULL,
	    "Error opening the file `%s' for writing: " );
	msgText( cGetStatus, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsGet \n"
	    "           -d dataServer identity string\n"
	    "           -ip ip Address of dataServer\n"
	    "           [-t {RAW|FITS|HEADER}]\n"
	    "           [-debug {NONE|MIN|FULL}]\n"
	    "           [-dest path]\n"
	    "           [-id dhsGet_identityString]\n"
	    "           [ [-n datasetName] ...-n datasetName]\n"
	    "           [-V]");
