/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1998.				(c) 1998
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
 * dhs/h/eventLoop.h
 *
 * PURPOSE:
 * Contains the interface to the event loop stuff.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:47  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:59  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1998/08/04 16:17:41  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef EVENTLOOP_H
#define	EVENTLOOP_H

#define	dhsEl_d( el, s )	/* Dummy destructor for an event loop	*/
				/* structure.				*/

/*
 *  Create an enumerated type specifying IMP user message types.
 */

typedef enum
{
    DHS_MSG_APPLY,		/* Apply command.			*/
    DHS_MSG_BD_GET,		/* Bulk data get command.		*/
    DHS_MSG_BD_PUT,		/* Bulk data put command.		*/
    DHS_MSG_BD_REPLY,		/* Bulk data get command.		*/
    DHS_MSG_BULK_HANDLE,	/* Handling a bulk data transfer.	*/
    DHS_MSG_BULK_READ,		/* Bulk read message.			*/
    DHS_MSG_CMD_REPLY,		/* Reply to a command.			*/
    DHS_MSG_CON_ACCEPT,		/* Accept a connection.			*/
    DHS_MSG_CON_OK,		/* Connection verifies as being ok.	*/
    DHS_MSG_CON_REFUSE,		/* Refuse a connection.			*/
    DHS_MSG_CON_VERIFY,		/* Send a connection verification message.*/
    DHS_MSG_DISCONNECT,		/* Client wants to disconnect.		*/
    DHS_MSG_EL_EXIT,		/* Exit event loop.			*/
    DHS_MSG_HEART_BEAT,		/* Heart beat message.			*/
    DHS_MSG_SERVER_EXIT,	/* Server request exit.			*/
    DHS_MSG_TIMEOUT,		/* Timeout message.			*/
    DHS_MSG_VOID		/* VOID message.			*/
} tDhsUsrMsg;


/*
 * Create a type to contain information about an invocation of the event
 * loop.
 */

typedef struct eventLoop
{
    tDhsCond		*elCond;	/* Event loop conditio variable.*/
    DHS_COND_FN		elCondFn;	/* Condition function.		*/
    void		*elCondArg;	/* Condition function argument.	*/
    tDhsMutex		*elMutex;	/* Event loop mutex.		*/
    boolean		elNoWait;	/* Exit as soon as a block occurs.*/
    int			*elRefCount;	/* Number of rentrant event 	*/
					/*  loops running.		*/
    boolean		elThread;	/* Run the event loop in a thread?*/
    DHS_THREAD		*elThreadId;	/* Event loop thread id.	*/
} tEventLoop;


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern void		dhsEl( tEventLoop *, DHS_COND_FN, void *, 
				boolean, boolean, DHS_STATUS * );
extern boolean		dhsElNoExit( tEventLoop * );
extern boolean		dhsElStopped( tEventLoop * );
extern void		dhsElStart( tEventLoop *, DHS_STATUS * );
extern void		*dhsElStartThread( tEventLoop * );
extern void		dhsSystemEvent( IMP_MsgInfo *, tDhsConnect *,
				DHS_STATUS * );
extern void		dhsUserEvent( int, tDhsUsrMsg, void *, int,
				tDhsConnect *, DHS_STATUS * );
#endif /* EVENTLOOP_H */
