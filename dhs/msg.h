/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1996				(c) 1996.
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
 * dhs/h/msg.h
 *
 * PURPOSE:
 * Contains the definition of a message structure that will allow error
 * and information messages to be stored in a linked list.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:51  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:59  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.3  1998/08/04 16:16:43  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.2  1997/01/20 19:01:57  nhill
 * Checking for install.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef MSG_H
#define	MSG_H

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * fnEntry
 *
 * INVOCATION:
 * fnEntry( "functionName" );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (char *) Name of the function being entered.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Display message on entry into a function
 *
 * This macro also saves the name of the function so that it can be
 * displayed when the function exits.
 *
 * DESCRIPTION:
 * This function saves a pointer to the function name, formats a message
 * indicating the function has been entered.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * This macro must be used after the last data declaration in a function
 * and before the first executable statement.
 *
 * SEE ALSO:
 * - fnReturn.
 *
 * DEFICIENCIES:
 * Contains both data definition statements and executable statements,
 * excluding use of another macro that does the same thing.
 *-
 ************************************************************************
 */

#define	fnEntry( fn )							\
	const char	*dhsFnName = fn;				\
	dhsMsgFmt( DHS_S_FN_ENTRY, DHS_DEBUG_FULL, dhsFnName );	

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * fnReturn
 *
 * INVOCATION:
 * fnReturn( val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) val (unkown) return value for the function.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Return from a function, printing a message idicating the function is
 * exitting
 *
 * The argument to this macro is the to be returned by the function. A 
 * special macro "VOID" is declared to be used as the return value of
 * void functions.
 *
 * DESCRIPTION:
 * Format a message indicating a function has been entered.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The fnEntry macro must have been used at the top of the function to
 * indicate entry to the function.
 *
 * SEE ALSO:
 * - fnEntry
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	fnReturn( val )							\
	dhsMsgFmt( DHS_S_FN_EXIT, DHS_DEBUG_FULL, dhsFnName );		\
	return val


#define	MAX_MESSAGE_LENGTH MAX( IMP_BULK_MESSAGE_LENGTH, 5 * 1024 )
#define	MAX_MESSAGES	10	/* Most simultaneous messages allowed.	*/


/*
 *  Create a type for messages.  This structure allows a linked list of
 *  messages to be maintained.
 */

typedef struct tDhsMsg_s
{
    DHS_STATUS		dmStatus;	/* Status causing the error.	*/
    DHS_ERR_LEVEL	dmLevel;	/* Error level of the message.	*/
    struct tDhsMsg_s	*dmNext;	/* Pointer to the next message.	*/
    boolean		dmFree;		/* True if the struct is unused.*/
    char		*dmMsg;		/* The message string.		*/
} tDhsMsg;


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern void		dhsErsHandler( void *, unsigned int, 
				const ErsMessageType[], StatusType * );
extern void		dhsMsgFmt( DHS_STATUS, DHS_DEBUG_LEVEL, ... );
extern void		dhsMsgInit( tDhsMsg * );
#endif /* MSG_H */
