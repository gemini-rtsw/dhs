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
// dhsPut/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS put to the data server.
//
// CLASSES:
// cPutStatus - put data status.
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

#ifdef HAVE_BOOL_H
#include <bool.h>
#endif

#include "dhsGenStatus.H"

#define DHS_NO_SERVER
#define DHS_NO_GET
#define	APPLICATION	"dhsPut"
#define	PREFIX		"put"

#define PUT_PERM	'P'
#define PUT_TEMP	'T'

#define DP_LIFE_CTL	2
#define	DP_BDCTL	"bdCtl"
#define DP_CTL		"ctl"
#define DP_LIFE		"lifeTime"
#define DP_DATASETNAME	"datasetName"

//
//***********************************************************************
//+
// CLASS NAME:
// cPutStatus
//
// PUBLIC METHODS: 
// (void) msgId - Declaration of messages.
//
// PURPOSE:
// Contains a status value.
//
//-
//***********************************************************************
//

class 	cPutStatus: public cDhsStatus
{
    public:
	//
	//  The following are the defines for the message in the msg.C file.
	//

	msgPrefix( PREFIX );

	msgId( S_DEBUG_MSG	);	// Debug message.
	msgId( S_LIFE_SET	);	// Lifetime set.
	msgId( S_OPEN_FILE	);	// File opened to read.
	msgId( S_PUT_ADD	);	// Add a filename to the list.
	msgId( S_PUT_DEL	);	// Remove filename from list.
	msgId( S_VERSION	);	// Print version number.
	msgId( S_WRITTEN_FILE	);	// File written.

	msgId( E_BAD_WRITE	);	// Writing failed.
	msgId( E_DATASETNAME_SET);	// Dataset name not set.
	msgId( E_ARGUMENT_LINE	);	// Error reading 
					// argument line.
	msgId( E_LIFE_FAILED	);	// Setting lifetime failed.
	msgId( E_NO_FILE	);	// Can't stat the file.
	msgId( E_NO_READ	);	// Can't read the file.
	msgId( E_PUT_FAILED);		// Failed to put dataset.
	msgId( E_USAGE		);	// Command line usage error.
    protected:
    private:
};


extern void	argInit( int, char *[], cPutStatus & );
#endif // GLOBALS_H
