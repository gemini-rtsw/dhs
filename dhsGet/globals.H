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
// dhsGet/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS get from data server.
//
// CLASSES:
// cGetStatus - get data status.
//
//INDENT-OFF*
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

#include "genBool.H"

#include "dhsGenStatus.H"

#define	DHS_NO_COMMAND
#define DHS_NO_SERVER
#define DHS_NO_PUT
#define	APPLICATION	"dhsGet"
#define	PREFIX		"get"

//
//***********************************************************************
//+
// CLASS NAME:
// cGetStatus
//
// PUBLIC METHODS: 
// (void) msgId - Declaration of messages.
// (void) info - Print information message.
//
// PURPOSE:
// Contains a status value.
//
//-
//***********************************************************************
//

class 	cGetStatus: public cDhsStatus
{
    public:
	//
	//  The following are the defines for the message in the msg.C file.
	//

	msgPrefix( PREFIX );

	msgId( S_DEBUG_MSG	);	// Debug message.
	msgId( S_GET_ADD	);	// Add a dataset to the list.
	msgId( S_GET_DEL	);	// Remove dataset from list.
	msgId( S_NULL_BUFFER	);	// Null buffer returned.
	msgId( S_OPEN_FILE	);	// File opened to write.
	msgId( S_VERSION	);	// Print version number.
	msgId( S_WRITTEN_FILE	);	// File written.

	msgId( E_BAD_WRITE	);	// Writing failed.
	msgId( E_ARGUMENT_LINE	);	// Error reading 
					// argument line.
	msgId( E_GET_FAILED	);	// Couldn't retrieve the dataset.
	msgId( E_NO_WRITE	);	// Can't write the file.
	msgId( E_USAGE		);	// Command line usage error.
    protected:
    private:
};


extern void	argInit( int, char *[], cGetStatus & );
#endif // GLOBALS_H
