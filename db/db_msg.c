/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	db/src/db_msg.c
*
*   Purpose:
*	message formatting routines.
*
*   Routines:
*	void	db_format_message	: Formats the message.
*
*   Date		: Sep 14, 1993
*
*   SCCS data		: @(#)
*	Module Name	: db_msg.c
*	Version Number	: 1.12
*	Release Number	: 1
*	Last Updated	: 05/24/96
*
*   Programmer		: Norman Hill
*
*   Modification History:
*	25/05/94 - nrh	: Added dbrc status messages.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "db.h"
#include "local_db.h"

#define	DB_PREFIX	"db"

char	db_msg[MSG_MAX_LEN];
int	db_errno = 0;

MSG	db_msgs[] = {
    {	DB_E_BAD_COLUMN,
	"Column %d specified in call to %s is not valid." },
    {	DB_E_DATE_FORMAT,
	"Date string '%s' has an invalid format." },
    {	DB_E_DB,
	"Error in the db library." },
    {	DB_E_DB_LIBRARY,
	"db library error: %d\n%s" },
    {	DB_E_MEMORY,
	"Memory allocation failure." },
    {	DB_E_NO_USER_INF,
	"Entry not found in .dbrc for server '%s' database '%s'." },
    {	DB_E_PERMIT,
	"Mode or ownership of file .dbrc incorrect." },
    {	DB_E_SQL_SERVER,
	"sql server error: %d\n%s" },
};

int	db_num_msgs = sizeof( db_msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Synopsis:
*	void	db_format_message( status, ... )
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

/* VARARGS 1 */

void	db_format_message
( 
    int 	status, 	/* (in)  The message number to print.	*/
    ...				/* (in)  the message arguments.		*/
)
{
    va_list	args;
    char	db_msg_local[MSG_MAX_LEN];


    va_start( args, status );
    msg_format( db_msg, DB_PREFIX, db_num_msgs, db_msgs, status, args );
    if ( local_db.ld_log_fp != NULL )
    {
	msg_format( db_msg_local, DB_PREFIX, db_num_msgs, db_msgs, 
		status, args );
	(void) fprintf( local_db.ld_log_fp, "%s\n", db_msg_local );
	fflush( local_db.ld_log_fp );
	msg_clear( db_msg_local );
    }

    va_end( args );
}
