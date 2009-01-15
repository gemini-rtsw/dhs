/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999.				(c) 1999.
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
*   Module Name:	mfs/src/msg.c
*
*   Purpose:
*	Routines for handling messages to the log file and the user's
*	terminal.
*
*   Routines:
*	statis void	format_message	    : Formats the message.
*	extern char	*mfs_get_message    : Returns the mfs lib. messages.
*
*   Date		: March 9, 1999
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 03/24/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*     2000/03/06 SDJ	: Changed to "mfs" library from "dvd" library.
*			  Updated to new standards.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
#include <ctype.h>
#include "gen_types.h"
#include "gen_msg.h"

#include "mfs.h"
#include "local_mfs.h"

#define	PREFIX	"mfs"

char msg[MSG_MAX_LEN];

static MSG	msgs[] = {
    {	MFS_E_NOT_MFS,
	"Device has type %s, and is not of type udf, or hrfs" },
    {	MFS_E_VSTAB,
	"vstab entry for volume set %s has an incorrect format.\n" },
};

static int	num_msgs = sizeof( msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Function: mfs_format_message
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	mfs_format_message
( 
    int		status, 	/* (in)  The message to print.		*/
    ... 			/* (in)  The message arguments.		*/
)
{
    va_list	args;

    va_start( args, status );
    msg_format( msg, PREFIX, num_msgs, msgs, status, args );
    va_end( args );
}

/*+
************************************************************************
*
*   Function:	mfs_get_message
*
*   Purpose:
*	Returns a pointer to the MFS library messages.
*
*   Values Returned:
*	char    *msg	: Message string is returned.
*	NULL		: Some error has occured.	
*
************************************************************************
-*/

char   	*mfs_get_message
(
    void
)
{
    return( msg );
}
