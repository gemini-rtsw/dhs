/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	mediaQueues/src/msg.c
*
*   Purpose:
*	Routines for handling message to log file, user's terminal.
*
*   Routines:
*
*	void	formatMessage	: Format a message.
*	void	printMessage	: Print a message.
*
*   Date		: <Jan 28, 1998>
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 01/28/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/09/25 SDJ	: Added support for variable precision in the
*			  media unit names.
*	98/11/23 SDJ	: Made error messages consistent.
*	99/01/06 SDJ	: Added missing msgs; S_QUIT and E_FILE_TOBIG.
*	99/02/24 SDJ	: Changed messages to show percentage full as
*			  a float value rather than an int.
*	99/03/05 NRH	: Added messages related to rollbacks.
*	99/03/29 NRH	: Spelling.
*	99/03/29 SDJ	: Use full file path in msgs instead of file id.
*	99/04/15 SDJ	: Spelling and Grammer.
*     2000/02/03 SDJ	: Added msgs DB_USAGE, E_DB_USAGE, and S_DIFF_ARGS.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <unistd.h>
#include <stdarg.h>


/*
 ************************************************************************
 *  SYBASE DB library include files.
 ************************************************************************
 */

#include <sybfront.h>
#include <sybdb.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "am.h"


/*
 ************************************************************************
 *  Media Queue  include files.
 ************************************************************************
 */

#include "globals.h"

/*
 ************************************************************************
 * Constatns definitions
 ************************************************************************
 */

#define	PREFIX	"mq"


/*
 ************************************************************************
 *  data types 
 ************************************************************************
 */

/*
 * Create a place to store the sf messages.
 */

char	msg[MSG_MAX_LEN];


/*
 * Define the status messages.
 */

static MSG	msgs[] = {
    { S_COMPLETE,
      "Completed queuing %s: %s. Media is %4.2f%% full." },
    { S_DB_USAGE,
      "Usage: [options]                                                   \n" 
      "      [ -i <media ID> ]      - A valid media ID integer.           \n"
      "      [-date <bool>]         - If <bool> is 'Y' then use file dates\n"
      "                               when sorting.  If <bool> is 'N' then\n"
      "                               do not use file dates.              \n"
      "      [-pack <bool>]         - If <bool> is 'Y' then pack the media\n"
      "                               as dense as possible. If <bool> is  \n"
      "                               'N' then stop choosing files as soon\n"
      "                               as the first file does not fit.     \n"
      "      [-size <bool>]         - If <bool> is 'Y' then use file sizes\n"
      "                               when sorting.  If <bool> is 'N' then\n"
      "                               do not use file sizes.              \n"
      "      [-num_units <integer>] - Number of media units to process.   \n"
      "      [-v]                   - Verbose mode.                       \n"},
    { S_DIFF_ARGS,
      "Warning: Command-line arguments and database arguments conflict.\n"
      "         command line: %s\n"
      "         database:      %s" },
    { S_FILE_INFO,
      "\t%s -- SIZE: %.0f DATE: %ld" },
    { S_FORCING,
      "Force-queuing %s:     %s. %4.2f%% full." },
    { S_MQ_SIGNAL,
      "mediaQueue handling signal." },
    { S_NO_FILES,
      "No db media files available to queue for media ID : %d." },
    { S_NOT_ENOUGH,
      "Not able to queue %s:  %s. Media only %4.2f%% full.\n"
      "There is %d unit(s) of media left to queue." },
    { S_NUM_ROLLBACK,
      "There are only %d units that can be rolled back." },
    { S_PREPARING,
      "Preparing to queue %s: %s." },
    { S_PT_TREE_END,
      "===================================================================="
      "==\n" },
    { S_PT_TREE_START,
      "\n\n    Full Directory Tree structure\n"
      "===================================================================="
      "==" },
    { S_PT_UNIT_END,
      "===================================================================="
      "==\n" },
    { S_PT_UNIT_START,
      "\n\n    CONTENTS of Media Unit %s\n"
      "===================================================================="
      "==" },
    { S_QUEUING,
      "Beginning queuing %s: %s." },
    { S_QUIT,
      "Quit a user's request." },
    { S_ROLLBACK,
      "Beginning rollback of queuing %s: %s." },
    { S_ROLLBACK_DONE,
      "Rollback of queuing %s: %s complete." },
    { S_TREE_INFO,
      "**************************************************************"
      "********\n%s -- TREE SIZE: %.0f -- FILE SIZE: %.0f -- DATE: %ld" },
    { S_TREE_UNIT_INFO,
      "**************************************************************"
      "********\n%s -- Date: %ld -- SPLIT: %s" },
    { S_UNITNAME,
      "No remaining unit names available for media ID: %d."  },
    { S_UNITNAME_FMT,
      "Warning: coding/decoding unit name : %s, for media ID : %d." },
    { S_USAGE,
      "Usage: mediaQueue -i <media ID> [options]                          \n" 
      "       -i <media ID>         - A valid media ID integer.           \n"
      "      [-date <bool>]         - If <bool> is 'Y' then use file dates\n"
      "                               when sorting.  If <bool> is 'N' then\n"
      "                               do not use file dates.              \n"
      "      [-pack <bool>]         - If <bool> is 'Y' then pack the media\n"
      "                               as dense as possible. If <bool> is  \n"
      "                               'N' then stop choosing files as soon\n"
      "                               as the first file does not fit.     \n"
      "      [-size <bool>]         - If <bool> is 'Y' then use file sizes\n"
      "                               when sorting.  If <bool> is 'N' then\n"
      "                               do not use file sizes.              \n"
      "      [-num_units <integer>] - Number of media units to process.   \n"
      "      [-force]               - Force queue partial units of media. \n"
      "      [-rollback]            - rollback a previous queuing operation.\n"
      "      [-v]                   - Verbose mode.                       \n"
      "      [-V]                   - Print version and exit." },
    { S_VERSION,
      "%s" },
    
    { S_SUCCESS,
      "Media Queue successfully completed." },
    
    { E_AM,
      "Error: AM library error." },
    { E_ARG_TYPE,
      "Error: Invalid argument parsing type `%d`." },
    { E_DATA_STAGE,
      "Error: Data stage for %d: %s." },
    { E_DB,
      "Error: SYBASE DB library error." },
    { E_DB_LIB,
      "Error: CADC DB library error." },
    { E_DB_USAGE,
      "Error: Database argument usage error: %s." },
    { E_FILE_TOBIG,
      "Error: File size is larger then the media, `%s', size for file\n\t%s"},
    { E_FILESIZE,
      "Error: Actual file size does not match database entry for file\n\t%s." },
    { E_MEDIA_TYPE,
      "Error: Invalid media type '%c' for media ID: %d." },
    { E_MEMORY,
      "Error: Memory allocation failure." },
    { E_NO_MEDIA,
      "Error: No database entry for media ID : %d." },
    { E_NO_PRUNE,
      "Error: Unable to remove `%s' from tree `%s'." },
    { E_NO_UNITS,
      "Error: There are no units to roll back." },
    { E_ONLY_ONE,
      "Error: mediaQueue or mediaIngest already running.\n"
      "Only one is permitted per media ID." },
    { E_OPTIONS,
      "Error: Bad options list `%s`.  Options list may contain any of:\n"
      "       '-nodate', '-nosize', and/or '-nopack'." },
    { E_SYSTEM,
      "Error: in system call `%s'." },
    { E_TREE_TYPE,
      "Error: Bad tree type given: `%d'." },
    { E_USAGE,
      "Error: Command line usage error." },
};


/*
 * Determine the number of messages that can be stored.
 */
static int	numMsgs = sizeof( msgs ) / sizeof( MSG );

/*+
************************************************************************
*
*   Function:	formatMessage
*
*   Purpose:
*	Formats a message from the contents of the argument list.
*
*   Values Returned:
*	none
*
************************************************************************
-*/

void	formatMessage
(
    int		status,		/* (in)  Message status number.		*/
    ...				/* (in)  Message arguments.		*/
)
{
    va_list	args;

    va_start( args, status );
    msg_format( msg, PREFIX, numMsgs, msgs, status, args );
    va_end( args );
}

/*+
************************************************************************
*
*   Function:	printMessage
*
*   Purpose:
*	Prints the message in msg to stdout or stderr, depending on 
*	the type.
*
*   Values Returned:
*	none
*
************************************************************************
-*/

void	printMessage
(
    char	*msg,		/* (in)  The message to display.	*/
    int		type		/* (in)  The type of the message.	*/
)
{
    /*
     *  If the message type is MESS_ERROR, print to stderr and the logfile.
     *  If the message type is MESS_INFO, print to the log file, and if in
     *  verbose mode, to stderr.
     *  If the message type is MESS_DATA, print only to the log file.
     */

    if ( type & MESS_LOG )
    {
	if ( globals.gLogFp != NULL )
	{
	    (void) fputs( msg, globals.gLogFp );
	    if ( ! ( type & MESS_NOCR ) )
	    {
		(void) fputc( '\n', globals.gLogFp );
	    }
	}
    }

    if ( globals.gLogFp == NULL || type & MESS_CON || 
	    ( ( type & MESS_CON_VERB ) && globals.gVerbose ) )
    {
	(void) fputs( msg, stdout );
	if ( ! ( type & MESS_NOCR ) )
	{
	    (void) fputc( '\n', stdout );
	}

	(void) fflush( stdout );
    }

    msg_clear( msg );
}
