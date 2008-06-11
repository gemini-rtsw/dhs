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
*   Module Name:	sim/src/msg.c
*
*   Purpose:
*	Routines for handling message to log file, user's terminal.
*
*   Routines:
*	void	formatMessage	: Format a message.
*	void	printMessage	: Print a message.
*
*   Date		: 
*
*   SCCS data		: 
*	Module Name	: 
*	Version Number	: 
*	Release Number	: 
*	Last Updated	: 
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/05/05 SEC  :	Added printing of data label returned from the
*			data server. Ordered messages properly.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/time.h>

#include <unistd.h>
#include <stdarg.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_msg.h"
#include "gen_str.h"

#include "dhs.h"

#include "globals.h"


#define	PREFIX	"sim"

char	msg[MSG_MAX_LEN];

static MSG	msgs[] = {
    { S_RESP_CALLBACK,
        "Response callback called : %s" },
    { S_SENDING,
	"%d : Sending %s under data label `%s'." },
    { S_USAGE,
	"Usage: dhsInstSim -i <FP> -id <ID> -ds <DS> -ip <IP> [-s <time> | -now] [opts]\n"
	"                  <FP>              - Path to input file.           \n"
	"                  <ID>              - Identity to use for simulator.\n"
	"                  <DS>              - Identity of dataServer.       \n"
	"                  <IP>              - IP address of dataServer.     \n"
	"                  <time>            - 24-hr clock time to start.    \n"
	"                                      Format should be HH:MM        \n"
	"                  [-ql]             - Data is for quick look.       \n"
	"                  [-buffs n]        - Number of image buffers.      \n"
	"                  [-debug ON|OFF|FULL] - debug mode.                \n"
	"                  [-v]              - Verbose mode.                 \n"
	"                  [-V]              - Print version and exit." },
    { S_VERSION,
	"%s" },
    { E_DHS,
	"Error in the DHS library." },
    { E_DHS_MESG,
	"Error message from the DHS library : \n%s." },
    { E_ERR_CALLBACK,
	"Error callback called : %s" },
    { E_INPUT,
	"Error in the input file." },
    { E_MEMORY,
	"Memory allocation failure." },
    { E_START_TIME,
	"Start time improperly formatted or already past." },
    { E_SYSTEM,
	"Error in the system" },
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
	if ( globals.g_log_fp != NULL )
	{
	    (void) fputs( msg, globals.g_log_fp );
	    if ( ! ( type & MESS_NOCR ) )
	    {
		(void) fputc( '\n', globals.g_log_fp );
	    }
	}
    }

    if ( globals.g_log_fp == NULL || type & MESS_CON || 
	    ( ( type & MESS_CON_VERB ) ) )
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
