/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	gen/src/msg.c
*
*   Purpose:
*	Message handling routines.  When more than one message is
*	found in the string, the delimiting character is a newline
*	character.
*
*   Routines:
*	static int	compare_status	: Compare function for bsearch.
*	void		msg_append	: Appends a message to another.
*	void		msg_clear	: Clears the message string.
*	static char	*msg_find	: Returns the format string.
*	void		msg_format	: Formats and appends the message.
*	static void	msg_indent	: Indent passed in buffer.
*
*   Date		: Aug 23, 1991
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.20
*	Release Number	: 1
*	Last Updated	: 12/14/99
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	92/08/11 nrh :	Added check for vms specific errors to
*				msg_format.
*	96/06/18 jsd :	Changed msg_append to be smart enough to indent
*			any next messages.  Also changed msg_format to
*			save 10 spaces for the opening codes.  Also
*			removed the ifdef VMS stuff.
*	96/07/17 jsd :	Added prototype of compare_status at top of file
*			and changed types of pointer to const void *.
*			Added msg_indent and msg_indent_cnt to take care
*			of the second message being multi-lined.
*	97/02/05 jsd :	Fixed msg_indent to not overrun maxmsglen.
*			and fixed it to print the + signs.
*	97/10/20 nrh :	Changed to use ssprintf and svsprintf instead
*			of sprintf and vsprintf.
*	97/11/17 sjg :	Fixed indentation on append.
*	99/12/13 nrh :  Changed to use snprintf and vsnprintf instead of 
*			ssprintf and vsprintf for all platforms except hp.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <errno.h>
#ifndef vxWorks
#include <stdarg.h>
#endif

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"

/*
 *  Globally define errno.
 */

int		sys_nerr;
//extern	char	*sys_errlist[];

#define MSG_INDENT_LENGTH	10
#define	MSG_NOT_FOUND		"Message not found."

static int	compare_status( const void *, const void * );
static char	*msg_find( int status, MSG *msgs, int num_msgs );
static void	msg_indent( char *, char * );


/*
 *  Macro to get the prefix and errno.
 */

#define MSG_MK_CODE( str, sign, prefix, errno )			\
    {	if ( sign == '+' ) 					\
	    (void) sprintf( str, "(%s+%d)", prefix, errno );	\
	else							\
	    (void) sprintf( str, "(%s%d)", prefix, errno ); }


/*+
************************************************************************
*
*   Synopsis:
*	static int	compare_status
*
*   Purpose:
*	Compares the status values in 2 msg structures.  This function
*	is used be bsearch().
*
*   Values Returned:
*	int	0		: Same status value.
*	int	< 0		: p is greater than q.
*	int	> 0		: q is greater than p.
*
************************************************************************
-*/

static int	compare_status(
const void	*p,		/* (in)  Item to be compared.		*/
const void	*q		/* (in)  Item to be compared.		*/
)
{
    MSG		*m1;
    MSG		*m2;

    m1 = (MSG *) p;
    m2 = (MSG *) q;
    return( m2->m_status - m1->m_status );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	msg_append
*   Purpose:
*	Appends msg2, with indents to each line of msg2, to msg1.
*	This is useful for concatenating messages
*	from different sources.  Make sure that the total length will
*	not exceed the maximum message length.  A newline character is
*	used as the delimiter between messages.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	msg_append(
char	*msg1,			/* (mod) Message to be appended to.	*/
char	*msg2			/* (in)  Message to be added.		*/
)
{
    char	buff[MSG_MAX_LEN];	/*  Temporary buffer.		*/
    int         len1;			/*  Length of msg1.		*/
    int         len2;			/*  Length of msg2.		*/
    int         len2_max;		/*  Maximum Length of msg2.	*/


    len1 = strlen( msg1 );


    /*
     *  Add the number of indents needed 
     *  before each line in msg2.
     */

    msg_indent( buff, msg2 );
    len2 = strlen( buff );


    /*
     *  Determine if we have enough room to make the string.
     *  The minimum of total room for message 2 plus a carriage return
     *  or maximum line length minus the room for message 1.
     */
     
    len2_max = MIN( ( len2 + 1 ), ( ( MSG_MAX_LEN - 1 ) - len1 ) );


    if ( len2_max > 0 )
    {
	if ( len1 != 0 )
	{
	    /*
	     *  If msg1 is not empty, add a carriage return to end of msg1.
	     */

	    (void) strcat(msg1, "\n");
	    len2_max--;
	}

	(void) strncat( msg1, buff, len2_max );
    }
}

/*+
************************************************************************
*
*   Synopsis:
*	void	msg_clear
*
*   Purpose:
*	Clears the message string by setting the first character to
*	the end-of-string character.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	msg_clear(
char	*msg				/* (mod) String to be cleared.	*/
)
{
    msg[0] = '\0';
}

/*+
************************************************************************
*
*   Synopsis:
*	static char	*msg_find( status, msgs, num_msgs )
*
*   Purpose:
*	Returns the format string of the message corresponding to the
*	given status.
*
*   Values Returned:
*	char	*format		: Format string.
*
************************************************************************
-*/

static char	*msg_find(
int		status,		/* (in)  Status searching for.		*/
MSG		*msgs,		/* (in)  Array of message structures.	*/
int		num_msgs	/* (in)  Number of messages in array.	*/
)
{
    MSG		*m;
    MSG		temp_m;


    temp_m.m_status = status;
    m = (MSG *) bsearch( (char *)(&temp_m), (char *) msgs,
	    (unsigned) num_msgs, sizeof( MSG ), compare_status );
    if ( m == NULL )
    {
	return( MSG_NOT_FOUND );
    }
    else
    {
	return( m->m_format );
    }
}

/*+
************************************************************************
*
*   Synopsis:
*	void	msg_format
*
*   Purpose:
*	Formats the buffer according to the status using the message
*	array and arguments provided.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	msg_format(
char	*buffer,			/* (mod) Formattd message.	*/
char	*prefix,			/*				*/
int	num_msgs,			/*				*/
MSG	*msgs,				/*				*/
int	status,				/*				*/
va_list	args
)
{
    char	*errno_arg;
    char	*format;
    int		i;
    char	*t1;
    char	*t2;
    char	temp_buffer1[MSG_MAX_LEN];
    char	temp_buffer2[MSG_MAX_LEN];
    char	code_buffer[MSG_MAX_LEN];	/* Prefix code & errno.	*/
    int		len1;
    int		len2;

    if ( status == MSG_ERRNO )
    {
	/*
	 *  Errno message will be used and then reset to 0.
	 *  The '+' character is used to separate the prefix from
	 *  the error number.  There will be an indent of 10 char.
	 */

	errno_arg = va_arg( args, char * );

	if ( errno != 0 )
	{
	    MSG_MK_CODE( code_buffer, '+', prefix, errno );
#ifdef NO_SNPRINTF
	    (void) ssprintf( MSG_MAX_LEN, temp_buffer2, "%-10s%s: %s",
		    code_buffer, errno_arg, strerror(errno) );
#else /* NO_SNPRINTF */
	    (void) snprintf( temp_buffer2, MSG_MAX_LEN, "%-10s%s: %s",
		    code_buffer, errno_arg, strerror(errno) );
#endif /* NO_SNPRINTF */
	    errno = 0;
	}
    }
    else
    {
	/*
	 *  Message array will be used and the prefix-number
	 *  separator will be a '-' (implicit since the number
	 *  is negative).  Since this is a programmer generated
	 *  message, parse it for newline characters and indent
	 *  10 spaces accordingly.
	 */

	format = msg_find( status, msgs, num_msgs );
#ifdef NO_SNPRINTF
	(void) svsprintf( MSG_MAX_LEN, temp_buffer1, format, args );
#else /* NO_SNPRINTF */
	(void) vsnprintf( temp_buffer1, MSG_MAX_LEN, format, args );
#endif /* NO_SNPRINTF */

	if ( strchr( temp_buffer1, '\n' ) == NULL )
	{
	    MSG_MK_CODE( code_buffer, '-', prefix, status );
#ifdef NO_SNPRINTF
	    (void) ssprintf( MSG_MAX_LEN, temp_buffer2, "%-10s%s", 
		    code_buffer, temp_buffer1 );
#else /* NO_SNPRINTF */
	    (void) snprintf( temp_buffer2, MSG_MAX_LEN, "%-10s%s", 
		    code_buffer, temp_buffer1 );
#endif /* NO_SNPRINTF */
	}
	else
	{
	    MSG_MK_CODE( code_buffer, '-', prefix, status );
	    (void) sprintf( temp_buffer2, "%-10s", code_buffer );
	    t1 = temp_buffer1;
	    t2 = temp_buffer2 + MSG_INDENT_LENGTH;
	    for ( ; *t1; t1++ )
	    {
		*t2++ = *t1;
		if ( *t1 == '\n' )
		{
		    for ( i = 0; i < MSG_INDENT_LENGTH; i++ )
		    {
			*t2++ = ' ';
		    }
		}
	    }
	    *t2 = '\0';
	}
    }

    
    len1 = strlen( buffer ) + 1;
    len2 = strlen( temp_buffer2);


    /*
     *  Find min length, lenght of temp_buffer2 or MAX length plus already
     *  existing length of message plus 1 for the carriage return.
     */

    len2 = MIN( len2, ( MSG_MAX_LEN - len1 - 1  ) );
    
    if ( len2 > 0 )
    {
	if ( len1 == 1 )
	    (void) strcpy( buffer, temp_buffer2);
	else
	{
	    (void) strcat( buffer, "\n" );
	    (void) strncat( buffer, temp_buffer2, len2 );
	}
    }
     
}
/*+
************************************************************************
*
*   Synopsis:
*	static void	msg_indent
*
*   Purpose:
*	Indent each line of buff1 by indent_len spaces by copying it
*	to buff2.  buff2 is returned.  If the last character is a 
*	carriage return, it will not indent after that.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

static void	msg_indent(
char		*buff2,		/*  (out) New message with indents.	*/
char		*buff1		/*  (in) Original message .		*/
)
{
    int		i;			/*  Counter.			*/
    int		j;			/*  Counter.			*/
    int		k;			/*  Counter.			*/
    int		len1;			/*  Length of buff1.		*/


    len1 = strlen(buff1);


    /*
     *  Put the indent in the first line.
     */

    for( j = 0; j < MSG_INDENT_LENGTH; j++ )
    {
	buff2[j] = ' ';
    }


    /*
     *  Now for each line, indent and copy.
     */

    for( i = 0 ; i < len1 && j < ( MSG_MAX_LEN - 1 ) ; i++, j++ )
    {
	buff2[j] = buff1[i];


	/*
	 *  If we hit a carriage return and it is not the
	 *  carriage return at the end of the message, then add indent.
	 */

	if ( buff1[i] == '\n' && i != ( len1 - 1 ) )
	{
	    for ( k = 0;
		    k < MSG_INDENT_LENGTH && j < ( MSG_MAX_LEN - 1 );
		    k++ )
	    {
		buff2[++j] = ' ';
	    }
	}
    }

    buff2[j] = '\0';
}
