static char rcsid[] = "$Id: msg.c,v 1.1.1.1 2002-11-24 20:32:56 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) <year>				(c) <year>
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6		Ottawa, Canada, K1A 0R6
 * All rights reserved			Tous droits reserves
 *					
 * NRC disclaims any warranties,	Le CNRC denie toute garantie
 * expressed, implied, or statu-	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.	fortuit, resultant de l'utili-
 *					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * fits2sdsApplication/src/msg.c
 *
 * PURPOSE:
 *	Message handling routines for the fits2sds application.
 *
 * FUNCTION NAME(S)
 *	void	f2sFormatMessage	: Formats a message.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:38  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.5  1998/04/30 06:04:53  jaeger
 * Changed /n to a \n.
 *
 * Revision 1.4  1998/01/29 21:25:46  jaeger
 * Added proper printing of version information.
 *
 * Revision 1.3  1997/12/02 21:26:16  jaeger
 * Add a new line in front of the message being written.
 *
 * Revision 1.2  1997/11/19 00:13:59  jaeger
 * Fixed messages so they are always printed and use the prefix "f2s"
 *
 * Revision 1.1  1997/09/05 19:25:37  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdarg.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"

/*
 ************************************************************************
 *  fits2sds application include files.
 ************************************************************************
 */

#include "fits2sds.h"



/*
 ************************************************************************
 *  Constant definitions.
 ************************************************************************
 */

#define	SF_PREFIX	"f2s"


/*
 ************************************************************************
 *  data types 
 ************************************************************************
 */

/*
 * Create a place to store the sf messages.
 */

char f2sMsg[MSG_MAX_LEN];

/*
 * Set the error messages.
 */

static MSG	f2sMsgs[] = {
    {
	F2S_S_CHUNK_FILE,
	"Writing chunk %d to a %s."
    },
    {
	F2S_S_COMPOSE_FILE,
	"Composing chunks into a %s '%s'."
    },
    {
	F2S_S_FULL_FILE,
	"Writing complete SDS structure to a %s."
    },
    {
	F2S_S_HELP,
	"-array\tAdd multi-dimensional array(s) to the SDS structures.\n"
	"-dd\t\tUse 'libdd.config' when forming the SDS structure.\n"
	"-ddMake\tCreate a 'libdd.config' in the current directory.\n"
	"-disk\t\tGenerate FITS disk files.\n"
	"-help\t\tDisplay this message.\n"
	"-i\t\tInput FITS filename.\n"
	"-mem\t\tGenerage FITS memory buffers.\n"
	"-sds\t\tGenerage SDS memory buffers.\n"
	"-v\t\tVerbose mode.\n"
	"-V\t\tPrint the version number.\n"
	"-xc <integer>\tNumber of chunks along the x-axis.\n"
	"-yc <integer>\tNumber of chunks along the y-axis.\n\n"
	"Note: The flags expecting values must have a space"
	"between the\n\t flag and the value.\n\n" 
    },
    {
	F2S_S_READ_FILE,
	"Reading file '%s'."
    },
    {
	F2S_S_QUIT,
	"Quiting the fits2sds application prematurely."
    },
    {
	F2S_S_USAGE,
	"Usage: fits2sds [-array] [-dd|-ddMake] [-disk] [-help] [-V] [-v]\n\t"
	"[-mem] [-sds] [-xc <integer>] [-yc <integer>] -i <filename>"
    },
    {
	F2S_S_VERSION,
	"%s\n%s    cfitsio      %s\n"
    },
    {
	F2S_S_SUCCESS,
	"Successful completion of the 'fits2sds' application."
    },
    {
	F2S_E_BAD_CMD_ARGS,
	"Bad commandline argument given. \n\n"
	"Usage: fits2sds [-array] [-dd|-ddMake] [-disk] [-help] [-V] [-v]\n\t"
	"[-mem] [-sds] [-xc <integer>] [-yc <integer>] -i <filename>"
    },
    {
	F2S_E_DHS,
	" Error in the DHS library." 
    },    {
	F2S_E_MEMORY,
	"Ran out of memory." 
    },
    {
	F2S_E_SF,
	"Error in the SF library." 
    },
    {
	F2S_E_USAGE,
	"Usage: fits2sds [-array] [-dd|-ddMake] [-disk] [-help] [-V] [-v]\n\t"
	"[-mem] [-sds] [-xc <integer>] [-yc <integer>] -i <filename>"
    }
};


/*
 * Determine the number of messages that can be stored.
 */

static int	f2sNumMsgs = sizeof( f2sMsgs ) / sizeof( MSG );



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sFormatMessage
 *
 * INVOCATION:
 * f2sFormatMessage( va_alist);
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	status	(int)		The message being formatted.	
 * (>)	 ...	(unknown)	The message arguments.		
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Formats a message from the contents of the va_alist.
 *
 * DESCRIPTION:
 * For each argument ( message ) format it and add it to the buffer.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	f2sFormatMessage
(
    int		*status,/* (in)	 The message being formatted.	*/
    ...			/* (in)	 The message arguments.		*/
) 

{
    va_list	args;  

    va_start( args, status );
    msg_format( f2sMsg, SF_PREFIX, f2sNumMsgs, f2sMsgs, status, args );
    va_end( args );
}



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sPrintMessage
 *
 * INVOCATION:
 * f2sPrintMessage( message, type )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	message	(char)		Buffer containing message string.
 * (>)	type	(int)		Type of message.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Prints the 'message' to the log file and, depending
 * on the type, to stdout.  The message is then cleared.
 *
 * DESCRIPTION:
 * 1. Write the message to the file stream (stdout).
 * 2. Clear the stream and list of messages.
 *
 * EXTERNAL VARIABLES:
 * f2sMsg
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	f2sPrintMessage
(
    void
)
{

    (void) printf( "\n%s\n", f2sMsg );
    (void) fflush( stdout );

    msg_clear( f2sMsg );
}
