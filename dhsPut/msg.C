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
// cPutStatus::msgDisplay 	- Print a message out.
// cPutStatus::msgDebug		- Print a debug message out.
// cPutStatus::info 		- Format and display an information message.
//
//INDENT-OFF*
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "iostream.h"

#include "dhs++.H"

#include "globals.H"
	msgText( cPutStatus, S_DEBUG_MSG, P_NULL,
	    "Debug message: %s." );
	msgText( cPutStatus, S_LIFE_SET, P_DEBUG_MIN,
	    "Set the lifetime for dataset %s, continuing..." );
	msgText( cPutStatus, S_PUT_ADD, P_DEBUG_MIN,
	    "Added dataset : `%s'." );
	msgText( cPutStatus, S_PUT_DEL, P_DEBUG_MIN,
	    "Removed dataset : `%s'." );
	msgText( cPutStatus, S_OPEN_FILE, P_DEBUG_MIN,
	    "File opened for reading: `%s'." );
	msgText( cPutStatus, S_VERSION, P_NULL,
	    "%s" );
	msgText( cPutStatus, S_WRITTEN_FILE, P_DEBUG_MIN,
	    "File `%s' written, size is : `%d'." );
	msgText( cPutStatus, E_BAD_WRITE, P_NULL,
	    "Error writing the file `%s', errno: `%d'." );
	msgText( cPutStatus, E_ARGUMENT_LINE, P_NULL,
	    "Error reading `%s' in argument line." );
	msgText( cPutStatus, E_DATASETNAME_SET, P_NULL,
	    "Dataset name not set, cannot set lifetime, try using -n option" );
	msgText( cPutStatus, E_LIFE_FAILED, P_NULL,
	    "Failed to set the lifetime for dataset %s, exiting." );
	msgText( cPutStatus, E_NO_FILE, P_NULL,
	    "Error opening the file `%s' for reading, cannot put this file." );
	msgText( cPutStatus, E_NO_READ, P_NULL,
	    "Error reading the file `%s', cannot put this file : ." );
	msgText( cPutStatus, E_PUT_FAILED, P_DEBUG_MIN,
	    "Put failed for dataset : `%s'." );
	msgText( cPutStatus, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsPut \n"
	    "           -d dataServerIdentityString\n"
	    "           -ip dataServerIpAddress\n"
	    "           [-t {RAW|FITS|DS|QL}]\n"
	    "           [-l {PERM|TEMP}]\n"
	    "           [-debug {NONE|MIN|FULL}]\n"
	    "           [-n datasetName ] \n"
	    "           [-id dhsPut_identityString]\n"
	    "           [ [-f filename] ...-f filename ]\n"
	    "           [ [filename] ...filename ]\n"
	    "           [ -noLast ] means do not set last flag to true\n"
	    "           [ -bufZero ] means last put has buff length zero\n"
	    "           [-V]");
