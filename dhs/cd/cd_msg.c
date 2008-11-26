/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	cd/src/cd_msg.c
*
*   Purpose:
*	Routines for handling messages to the log file and the user's
*	terminal.
*
*   Routines:
*	void	cd_format_message	: Formats the message.
*	boolean	prompt_user		: Prompts user to continue or not.
*
*   Date		: Feb 6, 1991
*
*   SCCS data		: @(#)
*	Module Name	: cd_msg.c
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 04/19/95
*
*   Programmer		: Bruce Enns
*			  Severin Gaudet
*			  Robyn Blaber
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
#include <ctype.h>
#include "gen_types.h"
#include "gen_msg.h"

#include "cd.h"
#include "local_cd.h"

#define	CD_PREFIX	"cd"

char cd_msg[MSG_MAX_LEN];

static MSG	cd_msgs[] = {
    {	CD_E_NOT_CDROM,
	"Device has type %s, and is not a cdrom." },
    {	CD_E_VSTAB,
	"vstab entry for volume set %s has an incorrect format.\n" },
};

static int	cd_num_msgs = sizeof( cd_msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Function: cd_format_message
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	cd_format_message
( 
    int		status, 	/* (in)  The message to print.		*/
    ... 			/* (in)  The message arguments.		*/
)
{
    va_list	args;

    va_start( args, status );
    msg_format( cd_msg, CD_PREFIX, cd_num_msgs, cd_msgs, status, args );
    va_end( args );
}
