/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998				(c) 1998.
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
*   Module Name:	tpIngest/src/msg.c
*
*   Purpose:
*	Contains the main, command line and usage functions for tpingest.
*
*   Routines:
*
*   Date		: Feb 20, 1998
*
*   Field SCCS data	: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 04/15/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/04/09 SDJ	: Added usage msg and others for fork()/exec().
*			  All error msgs now start with "Error:"
*	99/04/15 SDJ	: Fixed usage message.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>

#include "gen_types.h"
#include "gen_msg.h"

#include "ad.h"
#include "tp.h"

#include "globals.h"

#define	TI_PREFIX	"ti"

char msg[MSG_MAX_LEN];

static MSG	msgs[] = {
    {	TI_AUTO_REINGEST,
    	"Auto re-ingesting tape: `%s'."  },
    {	TI_DONE,
    	"Ingest tape : `%s' - is done." },
    {	TI_END_OF_FILE,
    	"End of file `%s' has been reached." },
    {	TI_ENTER_TAPE_NAME,
    	"Enter the tape name please : " },
    {	TI_FILE_PUT,
	"Adding - %s\t%s\t%10d." },
    {	TI_FILE_UPDATE,
	"Updataing -%s\t%s\t%10d." },
    {	TI_QUIT,
	"Quit a user's request." },
    {	TI_READY,
    	"Put the tape in source drive %s\n"
	"Ready(Y/N)? " },
    {	TI_RE_INGEST,
	"Volume already exists in the archive with the name '%s'.\n" 
	"Would you like to re-ingest this tape (Y/N)? " },
    { 	TI_SIGNAL,
	"tpIngest handling signal." },
    {	TI_TAPE_PUT,
	"%s\t%d\t%10d\t - Inserted" },
    {	TI_TAPE_UPDATE,
	"%s\t%d\t%10d\t - Updated" },
    {	TI_USAGE,
	"Usage: tpIngest -d <device> -t <type> [options]                 \n"
	"                <device> Tape devices (eg. /dev/rmt/0n)         \n"
	"                <type>   Tape type from tp lib. config. file    \n"
	"                [-a archive]    - Archive to add tape to.       \n"
	"                [-r]            - Don't prompt on re-ingest.    \n"
	"                [-v]            - Verbose mode.                 \n"
	"                [-V]            - Print version and exit.       \n" },
    {	TI_VERSION,
	"%s" },

    { 	TI_SUCCESS,
	"Successful completion of tpIngest" },

    {	TI_E_AD_FATAL,
	"Error: Fatal error in ad library." }, 
    {	TI_E_DB,
	"Error: SYBASE database error, check log file." }, 
    {	TI_E_DB_LIB,
	"Error: CADC database library error, check log file." },
    
    { 	TI_E_GET_FILE,
    	"Error: unable to process line : `%s'." },
    { 	TI_E_EXEC_ABNORMAL_EXIT,
        "Error: %s returned abnormally." },
    { 	TI_E_EXEC_NONZERO_EXIT,
        "Error: %s returned nonzero status: %d." },
    { 	TI_E_FORK_N_EXEC,
        "Error: fork() and exec() unable to start script:\n"
        "%s %s %s %s" },
    {	TI_E_MEMORY,
	"Error: Out of memory." }, 
    {   TI_E_NO_ARCHIVE,
        "Error: %s is not in the list of configured archives." },
    {   TI_E_NO_DEFAULT,
        "Error: No default archive found." },
    {	TI_E_NO_NAME,
    	"Error: Tape name not found." },
    {   TI_E_PRESS,
	"Error: Press library reports an error." },
    {  	TI_E_SCRIPT,
    	"Error: %s: `%s' - `%s'." },
    {	TI_E_SYSTEM,
	"Error: System cal files: function %s()." },
    {	TI_E_TP,
	"Error: Tp library reports an error." },
    { 	TI_E_USAGE,
	"Error: command line usage error." }
};

static int	numMsgs = sizeof( msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Function: format_message
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	format_message
( 
    int		status, 	/* (in)  The message to print.		*/
    ... 			/* (in)  The message arguments.		*/
)
{
    va_list	args;

    va_start( args, status );
    msg_format( msg, TI_PREFIX, numMsgs, msgs, status, args );
    va_end( args );
}

/*+
************************************************************************
*
*   Function:	print_message
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

void    print_message
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

    if ( globals.g_log_fp != NULL )
    {
         (void) fprintf( globals.g_log_fp, "%s", message );
         if ( cr )
         {
             (void) putc( '\n', globals.g_log_fp );
         }
        (void) fflush( globals.g_log_fp );
    }


    /*
     *  If the log file is not open, or the message type is not an
     *  information message or we are in verbose mode, print to stdout.
     */

    if ( globals.g_log_fp == NULL || 
            ( type != MESS_REPLY && type != MESS_INFO ) ||
            ( globals.g_verbose && type == MESS_INFO ) )
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
*	boolean	prompt_user( type )
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

boolean	prompt_user
(
    int		type 
)
{
    char	inputStr[MESS_LEN];
    char	reply[2];


    /*
     *  Print a prompt.
     */

    switch( type )
    {
	case READY:
	    format_message( TI_READY, globals.g_device ); 
	    break;

	case REINGEST:
	    format_message( TI_RE_INGEST, globals.g_tape_name );
	    break;
    }

    print_message( msg, MESS_WARNING, NO_CR );


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
