/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999.
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
*   Module Name:	mfsIngest/src/msg.c
*
*   Purpose:
*	Routines for handling messages to the log file and the user's
*	terminal.
*
*   Routines:
*	void	formatMessage		: Formats the message.
*	void	printMessage		: Prints the message.
*	boolean	promptUser		: Prompts user to continue or not.
*
*   Date		: Mar 10, 1996
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 05/24/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/03/10 SDJ	: Copied from cdIngest.
*     2000/03/27 SDJ	: Updated to new CADC software standards.  Switched
*			  from dvdIngest to mfsIngest.  Added/renamed
*			  messages to support dvd to mfs change.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/
/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>


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
#include "ad.h"
#include "mfs.h"


/*
 ************************************************************************
 *  MFS ingest include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Constatns definitions
 ************************************************************************
 */

#define	PREFIX	"mi"


/*
 ************************************************************************
 *  data types 
 ************************************************************************
 */

/*
 * Create a place to store the mfsIngest messages.
 */

char msg[MSG_MAX_LEN];


/*
 * Define the status messages.
 */

static MSG	msgs[] = {
    {	S_FILE_PUT,
	"%s\t%10d - Inserted" },
    {	S_MFSI_SIGNAL,
	"mfsIngest handling signal." },
    {   S_NOT_REINGEST,
	"Volume %s not re-ingested." },
    {	S_QUIT,
	"Quit at user's request." },
    {	S_RE_INGEST,
	"Volume already exists in the archive with the name '%s'." },
    { 	S_USAGE,
	"Usage: mfsIngest [options] [path ...]       \n"
	"                 [ -a <archive> ] - Archive to add the media's contents to.\n"
	"                 [-b]             - Run in batch mode.                     \n"
	"                 [-r]             - Don't prompt on re-ingest.             \n"
	"                 [-t CD|DVD|MD ]  - Media type to be ingested.             \n"
	"                 [-v]             - Verbose mode.                          \n"
	"                 [-V]             - Print version and exit.                " },
    {	S_VERSION,
	"%s" },
    {	S_VOLUME_NAMED,
	"Volume inserted has the volume id '%s'." },
    {   S_VOLUME_NAMEUSED,
	"Volume name '%s' is used by another volume." },
    {	S_VOLUME_NEWNAME,
	"The volume will be named '%s'." },
    {	S_VOLUME_PUT,
	"%s - Inserted" },
    {	S_VOLUME_UPDATE,
	"%s\t%d\t%10d\t - Updated" },

    {	E_AD_FATAL,
	"Error: Fatal error occured in ad library." }, 
    {	E_ARCHIVE,
	"Erorr: Invalid archive name %s" },
    {	E_DB,
	"Error: SYBASE database reported an error." }, 
    {	E_DB_LIB,
	"Error: CADC Database library reported an error." }, 
    {   E_FILE_ID_LEN,
	"Error: Maximum ad file ID length of %d characters was exceeded by file: %s" },
    {	E_LOG,
	"Error: Unable to open log file '%s'.\n" },
    {	E_MEMORY,
	"Error: Out of memory." }, 
    {	E_MFS,
	"Error: mfs library reported and error." },
    {   E_NO_DEFAULT,
        "Error: No default archive found." },
    {   E_PATH_LEN,
	"Error: Maximum ad path length of %d characters was exceeded by file: %s" },
    {   E_PRESS,
	"Error: press library reports an error on file %s." },
    { 	E_USAGE,
	"Error: command line usage error." },
    { 	E_VOL_FORMAT,
	"Error: Invalid file system format: %s.  Should be one of MD, CD or DVD." },
    { 	E_VOL_VERIFY,
	"Error: Volume '%s' not verified." },
};

static int	numMsgs = sizeof( msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Function: formatMessage
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	formatMessage
( 
    int		status, 	/* (in)  The message to print.		*/
    ... 			/* (in)  The message arguments.		*/
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
*	Prints the 'message' to the log file and, depending
*	on the type, to stdout.  The message is then cleared.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void    printMessage
( 
    char	*message, 	/* (in)  Buffer containing message string*/
    int		type 		/* (in)  Type of message.		*/
)
{
    /*
     * Log the message to the log file if the message type is, MESS_LOG,
     * MESS_ERROR, MESS_INFO, or MESS_OPER.
     */
	
    if ( type & MESS_LOG )
    {
        if ( globals.gLogFp != NULL )
        {
            (void) fputs( message, globals.gLogFp );
            if ( ! ( type & MESS_NOCR ) )
            {
                (void) fputc( '\n', globals.gLogFp );
            }
        }
    }


    /*
     * Write the message to stdout if the message type is: MESS_CON,
     * MESS_CON_VERB, MESS_ERROR, MESS_INFO, or MESS_OPER.
     */

    if ( globals.gLogFp == NULL || type & MESS_CON ||
            ( ( type & MESS_CON_VERB ) && globals.gVerbose ) )
    {
        (void) fputs( message, stdout );
        if ( ! ( type & MESS_NOCR ) )
        {
            (void) fputc( '\n', stdout );
        }
        (void) fflush( stdout );
    }

    msg_clear( msg );
}

/*+
************************************************************************
*
*   Synopsis:
*	boolean	promptUser( type )
*
*   Purpose:
*	Prints a message to stdout warning the user about some irregularity
*	and prompts him/her whether or not to continue.
*
*   Parameters:
*	int	type		: The type of prompt message.
*
*   Values Returned:
*	boolean	TRUE		: The user wants to continue.
*	boolean	FALSE		: The user wants to stop.
*
************************************************************************
-*/

boolean	promptUser
(
    int	type		/* (in)  Type of prompt.		*/
)
{
    char	buffer[MESS_LEN];
    char	inputStr[MESS_LEN];
    char	reply[2];

    
    /*
     *  Print a prompt.
     */

    switch( type )
    {
	case CONTINUE:
	    (void) sprintf( buffer, "Continue (Y/N)? " );
	    break;

	case CORRECT:
	    (void) sprintf( buffer, "Is this correct (Y/N)? " );
	    break;

	case NEW_VOLUME:
	    (void) sprintf( buffer,
		    "Would you like to load another volume (Y/N)? " );
	    break;

	case QUIT:
	    (void) sprintf( buffer, "Quit (Y/N)? " );
	    break;

	case READY:
	    (void) sprintf( buffer, "Ready (Y/N)? " );
	    break;

	case REINGEST:
	    (void) sprintf( buffer,
		    "Would you like to re-ingest this volume (Y/N)? " );
	    break;


	case RETRY:
	    (void) sprintf( buffer, "Retry (Y/N)? " );
	    break;
    }

    printMessage( buffer, MESS_PROMPT );


    /*
     *  Retrieve the user's input.
     */

    (void) gets( inputStr );
    if ( sscanf( inputStr, "%1s", reply ) < 1 )
    {
	reply[0] = 'N';
    }
    else
    {
	reply[0] = toupper( reply[0] );
    }


    /*
     *  User does not wish to continue so acknowledge and return
     *  FALSE.
     */

    if ( reply[0] != 'Y' )
    {
	return( FALSE );
    }

    return( TRUE );
}
