static char rcsid[] = "$Id: msg.C,v 1.1.1.1 2002-11-24 20:31:07 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 2000				(c) 2000
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
// dhsStoPut/src/msg.C
//
// PURPOSE:
// To handle the formation and printing of messages.
//
// METHOD NAME(S)
// None.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.6  2001/01/12 22:22:28  jaeger
// Added messaged E_DUP_FILE and E_DUP_TYPE.  Changed message E_RET_MT_NOT_FOUND
// to E_MT_NOT_FOUND.
//
// Revision 1.5  2000/12/19 19:30:04  jaeger
// Added messages E_HOSTNAME and E_NO_IDENTITY
//
// Revision 1.4  2000/12/12 20:17:30  jaeger
// Added S_VERSION_SHORT message.  Altered the usage message to hopefully
// make it clearer.
//
// Revision 1.3  2000/12/11 20:04:51  jaeger
// Fixed wording of E_NO_READ message.
//
// Revision 1.2  2000/12/07 23:51:10  jaeger
// Added the file ID to the S_ARCHIVE_WARN message.  ADded the definition
// for the S_CONFIG_READ message.
//
// Revision 1.1  2000/12/05 06:54:38  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "iostream.h"

#include "dhs++.H"

#include "globals.H"
    	msgText( cStoPutStatus, S_ARCHIVE_WARN, P_NULL,
	    "WARNING: The Storage Server was already aware of file: `%s'." );
        msgText( cStoPutStatus, S_CONFIG_READ, P_NULL,
	    "Reading configuration file: `%s'." );
	msgText( cStoPutStatus, S_DEBUG_MSG, P_NULL,
	    "Debug message: %s." );
	msgText( cStoPutStatus, S_FILE_AVAIL, P_DEBUG_FULL,
	    "Storage server is now aware of file: `%s'.  Continuing..." );
        msgText( cStoPutStatus, S_HELP, P_DEBUG_FULL,
	    "-help command-line option was specified." );
	msgText( cStoPutStatus, S_MEDIATYPE_ADD, P_DEBUG_MIN,
	    "Added media type pair: `%s', `%s'." );
	msgText( cStoPutStatus, S_MEDIATYPE_DEL, P_DEBUG_MIN,
	    "Removed media type pair: `%s', `%s'." );
	msgText( cStoPutStatus, S_OPEN_FILE, P_DEBUG_MIN,
	    "File opened for reading: `%s'." );
	msgText( cStoPutStatus, S_PUT_ADD, P_DEBUG_MIN,
	    "Added File : `%s'." );
	msgText( cStoPutStatus, S_PUT_DEL, P_DEBUG_MIN,
	    "Removed File : `%s'." );
	msgText( cStoPutStatus, S_SENT_FILE, P_DEBUG_MIN,
	    "File `%s' sent to %s, size is : `%d'." );
	msgText( cStoPutStatus, S_USAGE, P_NULL,
	    "Usage:\n"
	    "  dhsStoPut [ options ] -archive <stream>\n"
            "       -f <srcFile> [ <destFile> ]\n"
	    "  dhsStoPut [ options ] -user <stream>\n"
	    "       -f <srcFile1> [ <destFile1> ]\n\n"
            "    -archive <stream>      The name of the archive the file(s)\n"
	    "                           are sent to, eg. GEMINI\n"
            "    [-comp ASIS|GZIP|NONE] Compression, if any to be applied to\n"
 	    "                           the file(s).\n"
            "    [-copies <numCopies>]  Number of copies of each media to\n"
            "                           make. (user requests only).\n"
            "    [-debug NONE|MIN|FULL] Sets the amount of debugging details\n"
 	    "                           that are displayed.\n"
            "    -f <srcFile> [<destFile>]\n"
            "                           The file (srcFile) to be sent and where\n"
            "                           it is to reside (destFile) on the media.\n"
            "                           More than one -f may be specified.\n"
            "    [-id <StorageID>]      Indentity string for dhsStoPut\n"
            "    [-ip <storageIP>]      IP Address of the Storage Server\n"
            "    [-media CD|DVD|DAT|EXABYTE]\n"
            "                           Type of media the user data is to be\n"
	    "                           written on. (user requests only.)\n"
            "    [-prefix <unitPrefix>] Prefix for the media unit names given\n"
	    "                           to user media. (user requests only.)\n"
	    "    [-s <storageID>]       Identity string of the Storage Server\n"
            "    -user <stream>         Name of the user stream to send the\n"
	    "                           file(s) to.\n"
	    "    [-V]                   Display version and quit." );
	msgText( cStoPutStatus, S_USR_MEDIA_REQ, P_DEBUG_FULL,
	    "Storage server is now aware of all files.  Continuing..." );
	msgText( cStoPutStatus, S_VERSION, P_NULL,
	    "%s\n%s" );
    	msgText( cStoPutStatus, S_VERSION_SHORT, P_NULL,
	    "%s" );

	msgText( cStoPutStatus, E_ARGUMENT_LINE, P_NULL,
	    "Error reading `%s' in argument line." );
	msgText( cStoPutStatus, E_BAD_COMP, P_NULL,
	    "Error: Bad compression value given: %s." );
	msgText( cStoPutStatus, E_BAD_WRITE, P_NULL,
	    "Error: Unable to send file `%s', to %s." );
    	msgText( cStoPutStatus, E_CMD_LINE_FILES, P_NULL,
	    "Error: Bad command-line file specification: -f `%s' `%s'." );
    	msgText( cStoPutStatus, E_CMD_SRC_FILE, P_NULL,
	    "Error: Bad command-line file specification: -f `%s'." );
        msgText( cStoPutStatus, E_DUP_FILE, P_NULL,
	    "Error: Duplicate file specified: %s." );
        msgText( cStoPutStatus, E_DUP_TYPE, P_NULL,
	    "Error: Duplicate media type: %s." );
    	msgText( cStoPutStatus, E_FILE_AVAIL, P_NULL,
	    "Error: failed to notify storage server of file: `%s'." );
        msgText( cStoPutStatus, E_HOSTNAME, P_NULL,
	    "Error: Unable to get the host dhsStoPut is running on." );
        msgText( cStoPutStatus, E_MT_NOT_FOUND, P_NULL,
	    "Error: Retrieval media type `%s' not found in media type list." );
	msgText( cStoPutStatus, E_NO_FILE, P_NULL,
	    "Error opening the file `%s' for reading, cannot put this file." );
        msgText( cStoPutStatus, E_NO_IDENTITY, P_NULL,
	    "Error: Unable to determine the identity of dhsStoPut." );
	msgText( cStoPutStatus, E_NO_READ, P_NULL,
	    "Error: reading the file `%s', cannot put this file." );
    	msgText( cStoPutStatus, E_NUM_COPIES, P_NULL,
	    "Error: Bad number of copies given: %s");
	msgText( cStoPutStatus, E_PUT_FAILED, P_NULL,
	    "Put failed for file : `%s'." );
    	msgText( cStoPutStatus, E_USAGE, P_NULL,
	    "Error: Command line usage error." );
    	msgText( cStoPutStatus, E_USR_MEDIA_REQ, P_NULL,
	    "Error: failed to notify storage server of user files." );

    
