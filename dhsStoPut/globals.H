//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 2000.				(c) 2000.
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
// dhsStoPut/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS put to the data server.
//
// CLASSES:
// cStoPutStatus - put data status.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:30:59  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.5  2001/01/12 22:14:39  jaeger
// Added messages: E_DUP_FILE and E_DUP_TYPE.  Changed message
// E_RET_MT_NOT_FOUND to E_MT_NOT_FOUND.
//
// Revision 1.4  2000/12/19 19:32:45  jaeger
// Added messages: E_HOSTNAME, E_NO_IDENTITY, and E_NUM_COPIES.
//
// Revision 1.3  2000/12/12 20:15:43  jaeger
// Added S_VERSION_SHORT message.
//
// Revision 1.2  2000/12/07 23:48:42  jaeger
// Added message: S_CONFIG_READ.
//
// Revision 1.1  2000/12/04 22:02:36  jaeger
// Initial revision
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
#include "dhs++.H"

#define	APPLICATION	"dhsStoPut"
#define	PREFIX		"stoPut"

//
//***********************************************************************
//+
// CLASS NAME:
// cStoPutStatus
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

class 	cStoPutStatus: public cDhsStatus
{
    public:
	//
	//  The following are the defines for the message in the msg.C file.
	//

	msgPrefix( PREFIX );

	msgId( S_ARCHIVE_WARN	);	// Warn that already archived.
	msgId( S_CONFIG_READ 	);	// List config. file being read.
	msgId( S_DEBUG_MSG	);	// Debug message.
	msgId( S_FILE_AVAIL	);	// stoFileAvail cmd successfully sent.
	msgId( S_HELP 		);	// User has asked for help.
	msgId( S_MEDIATYPE_ADD  );	// Add a media type to the list.
	msgId( S_MEDIATYPE_DEL  );	// Add a media type to the list.
	msgId( S_OPEN_FILE	);	// File opened to read.
	msgId( S_PUT_ADD	);	// Add a filename to the list.
	msgId( S_PUT_DEL	);	// Remove filename from list.
	msgId( S_SENT_FILE	);	// Sent the file to the storage server.
	msgId( S_USAGE		);	// Usage information.
	msgId( S_USR_MEDIA_REQ	);	// User media reuest cmd successful.
	msgId( S_VERSION	);	// Print version information.
	msgId( S_VERSION_SHORT	);	// Print short version info.
	msgId( S_WRITTEN_FILE	);	// File written.

	msgId( E_ARGUMENT_LINE	);	// Error reading 
					// argument line.
	msgId( E_BAD_COMP	);	// Bad compression value.	
	msgId( E_BAD_WRITE	);	// Writing failed.
	msgId( E_CMD_LINE_FILES );	// Cmd-line file specification bad.
	msgId( E_CMD_SRC_FILE 	);	// Bad src file given on cmd-line.
	msgId( E_DUP_FILE	);	// Duplicate file specified.
	msgId( E_DUP_TYPE	);	// Duplicate media type found.	
	msgId( E_FILE_AVAIL	);	// stoFileAvail cmd unsuccessfully sent.
	msgId( E_HOSTNAME 	);	// Can't get local hostname.
	msgId( E_MT_NOT_FOUND );	// Retrieval media type not found.
	msgId( E_NO_FILE	);	// Can't stat the file.
	msgId( E_NO_IDENTITY	);	// Can't find dhsStoPut Identity.
	msgId( E_NO_READ	);	// Can't read the file.
	msgId( E_NUM_COPIES	);	// Number o.f copies specified is bad.
	msgId( E_PUT_FAILED);		// Failed to put dataset.
	msgId( E_USAGE		);	// Command line usage error.
	msgId( E_USR_MEDIA_REQ	);	// User media reuest cmd unsuccessful.
    protected:
    private:
};

    
//
// Global function definitions.
//
    
class cStoPutDhs;
extern void  argInit( int, char *[], cStoPutDhs *, cStoPutStatus & );
extern void  configInit( cStoPutDhs *, cStoPutStatus & );
#endif // GLOBALS_H
