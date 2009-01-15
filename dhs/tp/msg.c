/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	tp/src/msg.c
*
*   Purpose:
*	Routines for handling messages to the log file and the user's
*	terminal.
*
*   Routines:
*	void	tp_format_message	: Formats the message.
*
*   Date		: Feb 11, 1998
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 03/23/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/03/23 SDJ	: Standardized the messages. Removed TP_E_SYSTEM.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
#include <ctype.h>
#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "tp.h"
#include "local_tp.h"

#define	TP_PREFIX	"tp"

char tp_msg[MSG_MAX_LEN];

static MSG	tp_msgs[] = {
    {   TP_CFILE_OPEN,
        "Configuration file `%s' has been opened." },
    {   TP_E_BUSY,
        "Error: %s device is busy." },
    {   TP_E_CONFIG,
        "Error: parsing configuration file %s." },
    {   TP_E_DEVICE,
        "Error: device %s not in `/dev/rmt/*n' format." },
    {   TP_E_INIT,
        "Error: Tp library is not initialized." },
    {   TP_E_MEMORY,
        "Error: Memory allocation failure." },
    {   TP_E_PERM,
        "Error: Permission to use device %s denied." },
    {   TP_E_TYPE,
        "Error: Invalid tape script type type : `%s' ." },
    {   TP_E_UNLOAD,
        "Error: No tape loaded in device %s or it is offline." },
};

static int	tp_num_msgs = sizeof( tp_msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Function: tp_format_message
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void		tp_format_message
( 
    int		status, 	/* (in)  The message to print.		*/
    ... 			/* (in)  The message arguments.		*/
)
{
    va_list	args;

    va_start( args, status );
    msg_format( tp_msg, TP_PREFIX, tp_num_msgs, tp_msgs, status, args );
    va_end( args );
}
