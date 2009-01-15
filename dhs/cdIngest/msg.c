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
*   Module Name:	cdIngest/src/msg.c
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
*   Date		: Dec 09, 1996
*
*   Field SCCS data	: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 06/23/98
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/18 JSD  :	Added CI_E_AL msg.
*	97/07/03 SEC  :	Added CI_E_PRESS msg. 
*	98/01/21 gz   : Added CI_E_NO_DEFAULT msg.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <stdarg.h>
#include <ctype.h>
#include "gen_types.h"
#include "gen_msg.h"

#include "ad.h"

#include "globals.h"

#define	CI_PREFIX	"ci"

char msg[MSG_MAX_LEN];

static MSG	msgs[] = {
    {	CI_DISK_DONE,
	"Disk %s has already been ingested." }, 
    {	CI_DISK_NAMED,
	"Disk inserted has the volume id '%s'." },
    {   CI_DISK_NAMEUSED,
	"Disk name '%s' is used by another disk." },
    {	CI_DISK_NEWNAME,
	"The volume will be named '%s'." },
    {	CI_DISK_PUT,
	"%s - Inserted" },
    {	CI_DISK_UPDATE,
	"%s\t%d\t%10d\t - Updated" },
    {	CI_FILE_PUT,
	"%s\t%10d - Inserted" },
    {	CI_MUST_RENAME,
	"Volume must be renamed." },
    {   CI_NOT_REINGEST,
	"Volume %s not re-ingested." },
    {	CI_QUIT,
	"Quit at user's request." },
    {	CI_RE_INGEST,
	"Volume already exists in the archive with the name '%s'." },
    {	CI_VERSION,
	"%s" },
    {	CI_E_AD_FATAL,
	"Fatal error in ad library." }, 
    {	CI_E_AD_NOREC,
	"File '%s' not found in archive directory." }, 
    {	CI_E_AL,
	"Fatal error in al library." }, 
    {	CI_E_ARCHIVE,
	"Invalid archive name %s" },
    {	CI_E_CD,
	"Error in the cd library." },
    {	CI_E_DB,
	"Database error, check log file." }, 
    {	CI_E_DB_LIB,
	"Database library error, check log file." }, 
    {	CI_E_DIR_READ,
	"Invalid directory entry; verify by other means." },
    {	CI_E_LOG,
	"Error opening log file '%s'.\n" },
    {	CI_E_MEMORY,
	"Out of memory." }, 
    {   CI_E_NO_DEFAULT,
        "No default archive found." },
    {   CI_E_PATH_LEN,
	"Maximum ad path length of %d characters was exceeded by file : \n%s" },
    {   CI_E_PRESS,
	"Error in the press library." },
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
    msg_format( msg, CI_PREFIX, numMsgs, msgs, status, args );
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
    int		type, 		/* (in)  Type of message.		*/
    boolean	cr 		/* (in)  Add carriage return or not	*/
)
{
    /*
     *  If the log file is open, by default all messages are written
     *  to it.
     */

    if ( globals.gLogFp != NULL )
    {
         (void) fprintf( globals.gLogFp, "%s", message );
         if ( cr )
         {
             (void) putc( '\n', globals.gLogFp );
         }
        (void) fflush( globals.gLogFp );
    }


    /*
     *  If the log file is not open, or the message type is not an
     *  information message or we are in verbose mode, print to stdout.
     */

    if ( globals.gLogFp == NULL || 
            ( type != MESS_REPLY && type != MESS_INFO ) ||
            ( globals.gVerbose && type == MESS_INFO ) )
    {
        (void) printf( "%s", message );
        if ( cr )
        {
             (void) putchar( '\n' );
        }
        (void) fflush( stdout );
    }

    msg_clear( message );
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

boolean	promptUser( type )
int	type;
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

	case NEW_DISK:
	    (void) sprintf( buffer,
		    "Would you like to load another disk (Y/N)? " );
	    break;

	case QUIT:
	    (void) sprintf( buffer, "Quit (Y/N)? " );
	    break;

	case READY:
	    (void) sprintf( buffer, "Ready (Y/N)? " );
	    break;

	case REINGEST:
	    (void) sprintf( buffer,
		    "Would you like to re-ingest this disk (Y/N)? " );
	    break;


	case RETRY:
	    (void) sprintf( buffer, "Retry (Y/N)? " );
	    break;
    }

    printMessage( buffer, MESS_WARNING, NO_CR );


    /*
     *  Retrieve the user's input.
     */

    fgets( inputStr, sizeof(inputStr), stdin ); // XXX allan: replaced gets
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
