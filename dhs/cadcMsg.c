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
*	void		dhs_msg_append	: Appends a message to another.
*	void		dhs_msg_clear	: Clears the message string.
*	MSG	 	*dhs_msg_find	: Returns the message structure
*	void		dhs_msg_format	: Formats and appends the message.
*	static void	msg_indent	: Indent passed in buffer.
*	static void	msg_indent_cnt	: Count number of indents needed.
*
*   Date		: Aug 23, 1991
*
*   SCCS data		: @(#)
*	Module Name	: msg.c
*	Version Number	: 1.12
*	Release Number	: 1
*	Last Updated	: 07/24/96
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
*	97/01/02 nrh :  Modified for use with Gemini software.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <errno.h>
#include <stdarg.h>
#include "gen_types.h"
#include "gen_str.h"
#include "cadcMsg.h"

#include "localDhs.h"

#define MSG_INDENT_LENGTH	10
#define	MSG_NOT_FOUND	"Message not found."

static int	compare_status( const void *, const void *);
static void	msg_indent(char *, char *, int);
static void	msg_indent_cnt(char *, int *);


/*
 *  Macro to get the prefix and errno.
 */

#define MSG_MK_CODE( str, sign, prefix, errno )			\
    {	if( sign == '+' ) 					\
	    (void) sprintf( str, "(%s+%d)", prefix, errno );	\
	else							\
	    (void) sprintf( str, "(%s%d)", prefix, errno ); }

/*
 * Macro to get the thread ID
 */

#ifdef PTHREAD_D4
/* #define	MSG_THREAD_ID	(int) dhsThreadSelf().field2 */
#define	MSG_THREAD_ID (int) 10
#else	/* PTHREAD_D4 */
#define	MSG_THREAD_ID	dhsThreadSelf()
#endif	/* PTHREAD_D4 */

/*+
************************************************************************
*
*   Synopsis:
*	static int	compare_status( p, q )
*
*   Purpose:
*	Compares the status values in 2 msg structures.  This function
*	is used be bsearch().
*
*   Parameters:
*	pointer	p		: (in)	Item to be compared.
*	pointer	q		: (in)	Item to be compared.
*
*   Values Returned:
*	int	0		: Same status value.
*	int	< 0		: p is greater than q.
*	int	> 0		: q is greater than p.
*
************************************************************************
-*/

static int	compare_status( p, q )
const void *	p;
const void *	q;
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
*	void	dhs_msg_append( msg1, msg2 ) *
*   Purpose:
*	Appends msg2, with indents to each line of msg2, to msg1.
*	This is useful for concatenating messages
*	from different sources.  Make sure that the total length will
*	not exceed the maximum message length.  A newline character is
*	used as the delimiter between messages.
*
*   Parameters:
*	char	*msg1		: (mod)	Message to be appended to.
*	char	*msg2		: (in)	Message to be added.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	dhs_msg_append( msg1, msg2 )
char	*msg1;
char	*msg2;
{
    char	buff[MSG_MAX_LEN];	/*  Temporary buffer.		*/
    int		indent_len;		/*  Number of spaces in indent.	*/
    int         len1;			/*  Length of msg1.		*/
    int         len2;			/*  Length of msg2.		*/
    int         len2_max;		/*  Maximum Length of msg2.	*/
    int         num_msgs1;		/*  Number of msgs.		*/
    int         num_msgs2;		/*  Number of msgs.		*/

    indent_len = MSG_INDENT_LENGTH;
    num_msgs1= 0;
    num_msgs2 = 0; 
    len1 = strlen( msg1 );
    len2 = strlen( msg2 );


    /*
     *  Determine the number of lines or messages in the first
     *  message - assuming each line contains a new message.
     *  The amount each line in msg2 gets indented = num_msgs1*indent_len.
     */

    if ( len1 == 0 )
    {
	/*
	 *  msg1 is empty.
	 */

	num_msgs1 = 1;
    }
    else
    {
	 
	msg_indent_cnt( msg1, &num_msgs1 );
    }


    /*
     *  Determine the number of messages in msg2 to determine the total
     *  amount of space required for msg2 because each of the lines
     *  in msg2 will need to be indented too.
     */
     
    msg_indent_cnt( msg2, &num_msgs2 );


    /*
     *  Determine if we have enough room to make the string.
     *  The minimum of total room for message 2, or maximum line
     *  length minus the room for message 1 plus the carriage return.
     */
     
    len2_max = MIN( ( len2 + num_msgs1 * num_msgs2 * indent_len ),
	    ( MSG_MAX_LEN - (len1 + 2) ) );
    if ( len2_max > 0 )
    {
	/*
	 *  If msg1 is not empty, add a carriage return to end of msg1.
	 */

	if( len1 != 0 )
	{
	    (void) strcat(msg1, "\n");
	}


	/*
	 *  Now add the number of indents needed 
	 *  before each line in msg2, then cat it on to msg1.
	 */

	msg_indent( msg2, buff, num_msgs1 * indent_len );
	(void) strncat( msg1, buff, len2_max );

    }

}

/*+
************************************************************************
*
*   Synopsis:
*	void	dhs_msg_clear( msg )
*
*   Purpose:
*	Clears the message string by setting the first character to
*	the end-of-string character.
*
*   Parameters:
*	char	*msg		: (mod)	String to be cleared.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	dhs_msg_clear( msg )
char	*msg;
{
    msg[0] = '\0';
}

/*+
************************************************************************
*
*   Synopsis:
*	MSG	*dhs_msg_find( status, msgs, num_msgs )
*
*   Purpose:
*	Returns the format string of the message corresponding to the
*	given status.
*
*   Parameters:
*	int	status		: (in)	Status searching for.
*	MSG	*msgs		: (in)	Array of message structures.
*	int	num_msgs	: (in)	Number of messages in array.
*
*   Values Returned:
*	char	*format		: Format string.
*
************************************************************************
-*/

MSG		*dhs_msg_find( status, msgs, num_msgs )
int		status;
MSG		*msgs;
int		num_msgs;
{
    MSG		temp_m;


    temp_m.m_status = status;
    return( (MSG *) bsearch( (char *)(&temp_m), (char *) msgs,
	    (unsigned) num_msgs, sizeof( MSG ), compare_status ) );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	dhs_msg_format( buffer, prefix, num_msgs, msgs, status, args )
*
*   Purpose:
*	Formats the buffer according to the status using the message
*	array and arguments provided.
*
*   Parameters:
*	va_list			: (in)  Variable length arguments.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	dhs_msg_format( buffer, prefix, num_msgs, msgs, status, args )
char	*buffer;
char	*prefix;
int	num_msgs;
MSG	*msgs;
int	status;
va_list	args;
{
    char	*errno_arg;
    int		i;
    char	*t1;
    char	*t2;
    char	temp_buffer1[MSG_MAX_LEN];
    char	temp_buffer2[MSG_MAX_LEN];
    char	code_buffer[MSG_MAX_LEN];	/* Prefix code & errno.	*/
    int		len1;
    int		len2;
    MSG		*msg;			/* The message structure.	*/

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
	    (void) sprintf( temp_buffer2, "%-10s%s %3d: %s",
		    code_buffer, errno_arg, MSG_THREAD_ID,
		    strerror(errno) );
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

	if ( ( msg = dhs_msg_find( status, msgs, num_msgs ) ) == NULL )
	{
	    (void) sprintf( temp_buffer1, "%s", MSG_NOT_FOUND );
	}
	else
	{
	    (void) vsprintf( temp_buffer1, msg->m_format, args );
	}

	if ( strchr( temp_buffer1, '\n' ) == NULL )
	{
	    MSG_MK_CODE( code_buffer, '-', prefix, status );
	    (void) sprintf( temp_buffer2, "%-10s %3d %s", code_buffer, 
		    MSG_THREAD_ID, temp_buffer1 );
	}
	else
	{
	    MSG_MK_CODE( code_buffer, '-', prefix, status );
	    (void) sprintf( temp_buffer2, "%-10s %3d", code_buffer,
		    MSG_THREAD_ID );
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

    len2 = MIN( len2, ( MSG_MAX_LEN - len1 ) );
    
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
*	static void	msg_indent( buff1, buff2, indent_len )
*
*   Purpose:
*	Indent each line of buff1 by indent_len spaces by copying it
*	to buff2.  buff2 is returned.  If the last character is a 
*	carriage return, it will not indent after that.
*
*   Parameters:
*	char	*buff1		: (in)	Original message.
*	char	*buff2		: (mod)	Message returned.
*	int	indent_len	: (mod) Length of the indent.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

static void	msg_indent( buff1, buff2, indent_len )
char		*buff1;			/*  Original message .		*/
char		*buff2;			/*  New message with indents.	*/
int		indent_len;		/*  Number of spaces in indent.	*/
{
    int		i;			/*  Counter.			*/
    int		j;			/*  Counter.			*/
    int		k;			/*  Counter.			*/
    int		len1;			/*  Length of buff1.		*/


    len1 = strlen(buff1);


    /*
     *  Put the indent in the first line.
     */

    for( k = 0; k < indent_len; k++ )
    {
	buff2[k] = ' ';
    }


    /*
     *  Now for each line, indent .
     */

    for( i = 0, j = k; i < len1 && j < ( MSG_MAX_LEN - 1 ); i++, j++ )
    {
	buff2[j] = buff1[i];


	/*
	 *  If we hit a carriage return and it is not the
	 *  carriage return at the end of the message, then add indent.
	 */

	if ( buff1[i] == '\n' && i != ( len1 - 1 ) )
	{
	    for( k = 0; k < indent_len && j < ( MSG_MAX_LEN - 1 ); k++ )
	    {
		buff2[++j] = ' ';
	    }
	}
    }

    buff2[j] = '\0';

}

/*+
************************************************************************
*
*   Synopsis:
*	static void	msg_indent_cnt( buff1, indent_len )
*
*   Purpose:
*	To determine the number of lines in the message passed in.
*	Each line is assumed to contain 1 message.
*
*   Parameters:
*	char	*buff		: (in)	Message to be appended to.
*	int	*num_msgs	: (mod)	Number of messages.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

static void	msg_indent_cnt( buff, num_msgs )
char		*buff;			/*  Original message.	*/
int		*num_msgs;		/*  Number of messages.	*/
{
    char	*buff_ptr1;		/*  Temporary pointer.		*/
    int         num_msg;		/*  Number of message in buff.	*/

    num_msg = 1;


    /*
     *  Determine the number of messages by counting
     *  the number of carriage returns in buff + 1 for the last one.
     */
     
    buff_ptr1 = buff;
    while( ( buff_ptr1 = strpbrk( buff_ptr1, "\n" ) ) != NULL )
    {
	num_msg++;
	buff_ptr1++;
    }
    *num_msgs = num_msg ;
}
