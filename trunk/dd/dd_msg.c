static char rcsid[] = "$Id: dd_msg.c,v 1.1.1.1 2002-11-24 20:24:13 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) <year>				(c) <year>
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
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
 * 					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * dd/src/ddMsg.c
 *
 * PURPOSE:
 *	Routines for handling messages to the log file and the user's
 *	terminal.
 *
 * FUNCTION NAME(S)
*	void	ddFormatMessage	: Formats the message.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.9  1997/12/02 20:00:57  jaeger
 * Changed one error message, added space where it was needed.
 *
 * Revision 1.8  1997/08/22 17:51:21  jaeger
 * Allowable array types changed to "EXTEN", "INDEX" and "NONE"
 * Keywords are converted to uppercase, warning if they are not
 *
 * Revision 1.7  1997/07/29 20:04:19  dunn
 * Lost of changes.
 *
 * Revision 1.6  1997/07/17 21:51:51  dunn
 * Fixed order of messages and add missing messages.
 *
 * Revision 1.5  1997/07/11 22:38:09  jaeger
 * Added "ANY" to datatype
 *
 * Revision 1.4  1997/07/09 16:35:34  jaeger
 *  Converted library to use dhs instead of bdt
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
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


/*
 ************************************************************************
 *  dd library include files.
 ************************************************************************
 */

#include "dd.h"
#include "local_dd.h"



#define	DD_PREFIX	"dd"

/* ??? Need to ensure that the messages don't exceed the size of
 *  ddMsg, ie. either remove a message from the stack and add the
 *  latest one, or allocate more memory
 */

char		ddMsg[MSG_MAX_LEN];
static MSG	ddMsgs[] = {
    {
	DD_S_LOWER,
	"Warning: '%s' is not an uppercase string." },
    {   DD_S_NOT_FOUND,
	"No %s found for %s pair %s/%s, trying again." },
    { 	DD_S_STRING_SIZE,
	"Only %d characters of string \"%s\" have been used." },
    {   DD_E_ARRAY,
	"Error, invalid array column type : %s." },
    {   DD_E_CFILE_OPEN,
	"Configuration file %s has been opened." },
    {	DD_E_CONFIG,
	"Error while parsing configuration file." }, 
    {	DD_E_FORMAT,
	"Error while getting token %s of type '%s'." }, 
    {	DD_E_FORMAT_NUM,
	"Error, invalid number of columns, %d, expected %d." }, 
    {	DD_E_MEMORY,
	"Out of memory." }, 
    {	DD_E_NOT_FOUND,
	"No %s found for %s pair%s/%s." }, 
    {   DD_E_TYPE,
	"Error for data type of %s, cannot have other values filled in." },
    {   DD_E_UK_KEY,
	"Error, %s pair %s/%s not unique in config file." },
};

static int	ddNumMsgs = sizeof( ddMsgs ) / sizeof( MSG );



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddFormatMessage
 *
 * INVOCATION:
 * ddFormatMessage( status, ... );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) paramName (int) Descripton of the parameter
 * (!) paramName (float) Descripton of the parameter
 * (<) paramName (long) Descripton of the parameter
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 *	Formats a message from the contents of the va_alist.
 *
 * DESCRIPTION:
 * Description of the algorithm.
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

void	ddFormatMessage
(
    int  *status,         /* (in)  The message being formatted.   */
    ...                         /* (in)  The message arguments.         */

)
{
    va_list	args;

    va_start( args, status );
    msg_format( ddMsg, DD_PREFIX, ddNumMsgs, ddMsgs, status, args );
    va_end( args );
}
