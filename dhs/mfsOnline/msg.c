/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999
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
*   Module Name:	mfsOnline/src/msg.c
*
*   Purpose:
*	Contains message handling routines for mfsOnline.
*
*   Routines:
*	void	formatMessage	: Format a message.
*	void	printMessage	: Print a message.
*	boolean	promptUser	: Prompt user for information.
*
*   Date		: Feb 23, 1999
*
*   SCCS data           : @(#)
*       Module Name     : msg.c
*       Version Number  : 1.3
*       Release Number  : 1
*       Last Updated    : 04/10/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/02/23 SDJ  	: Copied from cdOnline.
*	99/03/22 SDJ	: Cosmetic changes to match other executables.
*     2000/03/21 SDJ	: Switched from dvdOnline to mfsOnline.
*     2000/04/10 SDJ	: Added promptUser().
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
#include <ctype.h>

/*
 ************************************************************************
 * SYBASE include files
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
#include "db.h"
#include "ad.h"


/*
 ************************************************************************
 *  mfsOnline include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Constant definitions
 ************************************************************************
 */

#define	PREFIX		"mfso"
#define MESS_LEN	128


/*
 ************************************************************************
 *  data types 
 ************************************************************************
 */

/*
 * Create a place to store the mfsOnline messages.
 */

char	msg[MSG_MAX_LEN];


/*
 * Define the status messages.
 */

static MSG	msgs[] = {
    {	S_MFSO_SIGNAL,
	"mfsOnfline handling signal." },
    {   S_QUIT,
	"Quit mfsOnline." },
    {   S_USAGE,
	"Usage: mfsOnline -u <volume> [options] - Un-mount the volume.\n"
	"       mfsOnline [options] <path>      - Mount MFS at the path.\n\n"
	"                 [-a <archive>]        - Archive MFS belongs too\n"
	"                 [-d <date>]           - Date volume was created\n"
	"                                         (YYYY/MM/DD HH:MM:SS).\n"
	"                 [-t <volumeType>]     - File system type on volume\n"
	"                                         (MD, CD, or DVD)\n"
	"                 [-v]                  - Verbose mode.\n"
        "                 [-V]                  - Print version and exit." },
    {	S_VERSION,
	"%s" },
    {	S_VOLUME_LIST_ENTRY,
	"%-30.30s   %-10.10s   %4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d" },
    {	S_VOLUME_LIST_FTR,
	"------------------------------   ----------   -------------------" },
    {	S_VOLUME_LIST_HDR,
	"           NAME                     TYPE         CREATION DATE\n"  
	"------------------------------   ----------   -------------------" },
    { 	S_VOLUME_MOUNTED,
	"Volume %s mounted at %s." },
    {	S_VOLUME_UNMOUNTED,
	"Volume %s unmounted." },

    {   S_SUCCESS,
	"mfsOnline has successfully complete." },
	
    {	E_AD,
	"Error: the AD library has reported an error." },
    {   E_ARCHIVE,
	"Erorr: Invalid archive name %s" },
    {	E_DB_LIB,
	"Error: the CADC db library has reported an error." },
    {   E_LOG,
        "Error: Unable to open log file '%s'.\n" },
    {	E_MEMORY,
	"Error: memory allocation failure." },
    {	E_MFS,
	"Error: the MFS library has reported an error." },
    {   E_NO_DEFAULT,
	"Error: No default archive found." },
    {	E_NO_VOLNAME,
	"Error: There is no volume named %s in the MFS archive." },
    {	E_NO_VOLUME,
	"Error: There is no volume in archive %s with label %s,\n"
	"       volume type %s and creation date %s." },
    {	E_USAGE,
	"Error: Command line usage error." }
};


static int	numMsgs = sizeof( msgs ) / sizeof( MSG );

/*+
************************************************************************
*
*   Function:	formatMessage
*
*   Purpose:
*	Formats messages from the contents of the argument list.
*
*   Values Returned:
*	none
*
************************************************************************
-*/

void		formatMessage
(
    int		status,		/* (in)  The message number to format.	*/
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
*	Prints the message in msg to stdout.
*
*   Values Returned:
*	none
*
************************************************************************
-*/

void		printMessage
(
    char	*msg,		/* (in)  The message to print.		*/
    int		type		/* (in)  The message type.		*/
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
	    (void) fputs( msg, globals.gLogFp );
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
	(void) fputs( msg, stdout );
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
    int	type			/* (in)  Type of prompt.		*/
)
{
    char	buffer[MESS_LEN];	/* String buffer for output.	*/
    char	inputStr[MESS_LEN];	/* String buffer for input.	*/
    char	reply[2];		/* Answer to question.		*/

    
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

	case QUIT:
	    (void) sprintf( buffer, "Quit (Y/N)? " );
	    break;

	case READY:
	    (void) sprintf( buffer, "Ready (Y/N)? " );
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
