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
*   Module Name:	cdOnline/src/msg.c
*
*   Purpose:
*	Contains message handling routines for cdOnline.
*
*   Routines:
*	void	formatMessage	: Format a message.
*	void	printMessage	: Print a message.
*
*   Date		: Dec 13, 1996
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 06/25/97
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/18 JSD  :	Added CO_E_AL for logging library errors.
*	97/06/20 SEC  :	Fixed message for CO_E_NO_VOLNAME.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <unistd.h>
#include <stdarg.h>

#include "gen_types.h"
#include "gen_msg.h"

#include "globals.h"


#define	CO_PREFIX	"co"

char	msg[MSG_MAX_LEN];

static MSG	msgs[] = {
    {	CO_VERSION,
	"%s" },
    { 	CO_VOLUME_MOUNTED,
	"Volume %s mounted at %s." },
    {	CO_VOLUME_UNMOUNTED,
	"Volume %s unmounted." },
    {	CO_E_AD,
	"Error in the ad library." },
    {	CO_E_AL,
	"Error in the al library." },
    {	CO_E_CD,
	"Error in the cd library." },
    {	CO_E_DB,
	"Error in the sybase db library." },
    {	CO_E_DB_LIB,
	"Error in the cadc db library." },
    {	CO_E_MEMORY,
	"Memory allocation failure." },
    {	CO_E_NO_VOLNAME,
	"There is no volume named %s in the cd archive." },
    {	CO_E_NO_VOLUME,
	"There is no volume with label %s and creation date %s." },
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
    msg_format( msg, CO_PREFIX, numMsgs, msgs, status, args );
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
    (void) fputs( msg, stdout );
    (void) fputc( '\n', stdout );
    (void) fflush( stdout );

    msg_clear( msg );
}
