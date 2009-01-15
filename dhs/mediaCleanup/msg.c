/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
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
*   Module Name:	mediaCleanup/src/msg.c
*
*   Purpose:
*	Routines for handling message to the log file and the users 
*	terminal.
*
*   Routines:
*	void	formatMessage		: Format a message.
*	void	printMessage		: Print a message.
*	boolean	promptUser		: Prompt for a user response.
*
*   Date		: Dec 13, 1996
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.11
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmers		: Marc LeBlanc
*			: Steve Cockayne
*			: Shannon Jaeger
*
*   Modification History:
*	97/02/21 SEC  : Added S_CLEANED_UP message, mod'ed some others.
*	98/08/24 SDJ  : Upgraded to new AM library.
*	98/12/14 SDJ  : Made error msg's consistent, added E_SYSTEM msg.
*	99/01/07 SDJ  : Added msg's for file removal.
*	99/02/24 SDJ  : Switched E_NO_MSTAGE to S_NO_MSTAGE.
*	99/03/17 NRH  : Added the messages required for the rollback option.
*	99/04/13 NRH  : Added the S_MC_SIGNAL message.
*	99/04/29 NRH  : Added the E_DB message.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"
#include "am.h"

#include "globals.h"


#define	PREFIX		"mc"

char	msg[MSG_MAX_LEN];

static MSG	msgs[] = {
    { S_CLEANED_UP,
	"mediaCleanup of unit %s in media request %d successful." },
    { S_NO_MSTAGE,
	"Warning: No MDS directory 'booked' for media ID : %d, unit: %s." },
    { S_QUIT,
        "Quit a user's request." },
    { S_REMOVED,
        "Removed file `%s' from retrieval staging area." },
    { S_ROLLBACK,
	"Rollback of cleanup of unit %s in media request %d started." },
    { S_ROLLBACK_DONE,
	"Rollback of cleanup of unit %s in media request %d complete." },
    { S_MC_SIGNAL,
	"mediaCleanup handling signal." },
    { S_USAGE,
	"Usage: mediaCleanup -i <media ID> -n <unit name> [options]          \n"
	"                       <media ID>    - A valid media ID integer.    \n"
	"                       <unit name>   - A valid media unit name.     \n"
	"                       [-rollback]   - Rollback a cleanup.          \n"
	"                       [-v]          - Verbose mode.                \n"
	"                       [-V]          - Print version and exit." },
    { S_VERSION,
	"%s" },
    
    { S_SUCCESS,
      "mediaCleanup has successfully completed." },
    
    { E_AM,
	"Error: in the am library." },
    { E_CLEAN_RSTAGE,
        "Error: Unable to remove all files from retrieval stage,\n\t%s" },
    { E_DATA_STAGE,
	"Error: Data stage for %d: %s." },
    { E_DB,
	"Error: in the SYBASE db library." },
    { E_DB_LIB,
	"Error: in the cadc db library." },
    { E_DIR_EMPTY,
        "Error: Unable to empty directory `%s'." },
    { E_DIR_RM,
        "Error: Unable to remove directory `%s'." },
    { E_FILE_RM,
        "Error: Unable to remove file `%s'." },
    { E_MEMORY,
	"Error: Memory allocation failure." },
    { E_NO_MEDIA,
	"Error: No db entry for media ID : %d." },
    { E_NO_SUCH_UNIT,
	"Error: No db entry for media ID : %d, unit: %s." },
    { E_ONLY_ONE,
	"Error: mediaCleanup already running. Only one permitted per media "
        "unit." },
    { E_SYSTEM,
        "Error: in system call `%s'." },
    { E_UNIT_STATUS,
	"Error: mediaCleanup cannot process unit with status '%s'." },
    { E_UNITNAME_FMT,
	"Error: decoding unit name : %s, for media ID : %d." },
    { E_USAGE,
	"Command line usage error." },
};

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
*   Function:   printMessage
*
*   Purpose:
*       Prints the message in msg to stdout or stderr, depending on 
*       the type.
*
*   Values Returned:
*       none
*
************************************************************************
-*/
 
void    printMessage
(
    char        *msg,           /* (in)  The message to display.        */
    int         type            /* (in)  The type of the message.       */
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
