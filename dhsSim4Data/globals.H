//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
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
// <filename>
//
// PURPOSE:
// Whatever
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:37  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/05/07 16:31:20  dunn
// Changed storage server command from fileArchive to stoArchiveFileAvailable.
//
// Revision 1.2  1998/04/20 20:54:36  nhill
// Updated to use the latest version of the cDhsStatus class.
// Removed some macros available from the dhsGenStatus.H file.
// Removed methods cSimStatus::info cSimStatus::msgDebug, c
// cSimStatus::msgDisplay and cSimStatus::defDhsDebug.
//
// Revision 1.1  1997/10/03 23:08:32  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GLOBALS_H
#define GLOBALS_H

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

#define APPLICATION	"dhsSim4Data"
#define PREFIX		"sim"


//
//  Some more #defines. Commands, attribute names, etc.
//

#define STV_CMD_OLDPS	"fileAvail"
#define STV_CMD_SS	"stoArchiveFileAvailable"
#define STV_CMD_QLS	"qlsDsStream"

#define STV_OLDPS_FILEID	"fileId"

#define STV_QLS_DATASET		"dataset"
#define STV_QLS_STREAMS		"streams"

#define STV_SS_FILEID		"fileId"
#define STV_SS_FILESIZE		"fileSize"
#define STV_SS_SEGREGATIONDIR	"segregationDir"
#define STV_SS_NOTIFY		"fileNotify"

#define STV_DATASET_NAME_LEN	45


//
//  Error checking macros. 
//

#define checkTrue( fn, status, ret )					\
    {									\
	if ( ( fn ) == FALSE )						\
	{								\
	    status.E_FALSE( status, #fn );				\
	    ret;							\
	}								\
    }



//
//***********************************************************************
//+
// CLASS NAME:
// cSimStatus
//
// PUBLIC METHODS:
// (void) msgId - Declaration of messages.
// (void) info - Print information message.
// (void) defDhsDebug - set default debug mode for the DHS.
//
// PURPOSE:
// Contains a status value.
//
//-
//***********************************************************************
//

class   cSimStatus: public cDhsStatus
{
    public:
	msgPrefix( PREFIX );

	msgId( S_CMD_OLDPS	);	// Successful OLDPS cmd message.
	msgId( S_CMD_OLDPS_AV	);	// OLDPS attribute-value message.
	msgId( S_CMD_QLS	);	// Successful QLS cmd message.
	msgId( S_CMD_QLS_AV	);	// QLS attribute-value message.
	msgId( S_CMD_SS		);	// Successful SS cmd message.
	msgId( S_CMD_SS_AV	);	// SS attribute-value message.
	msgId( S_CONFIG_OPEN	);	// Config. file opened.
	msgId( S_CONNECT_REQ	);	// Connection request.
	msgId( S_DEBUG_MSG	);	// Debug message. 
	msgId( S_DISCONNECT_REQ	);	// Disconnection request.
	msgId( S_PUT		);	// Successful file put message.
	msgId( S_VERSION	);	// Print version number.

	msgId( E_CONTROL_C	);	// Control-C was pressed.
	msgId( E_FALSE		);	// Something was ususpectedly false.
	msgId( E_FILE_OPEN	);	// Error opening the file.
	msgId( E_FILE_WRITE	);	// Error writing the file.
	msgId( E_INVALID_COMMAND );	// Received invalid command.
	msgId( E_INVALID_PUT	);	// Received invalid put.
	msgId( E_INVALID_PUT_LABEL );	// Invalid put label.
	msgId( E_USAGE		);	// Command line usage error.
    protected:
    private:
};


//
//  Function prototypes
//

void	argInit( int, char *[], cSimStatus & );
void	configInit( cSimStatus & );


#endif // GLOBALS_H
