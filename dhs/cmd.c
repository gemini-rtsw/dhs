static char rcsid[] = "$Id: cmd.c,v 1.1.1.1 2002-11-24 20:19:53 brighton Exp $";
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
 * dhs/src/cmd.c
 *
 * PURPOSE:
 * This file contains routines to manage command structures used to keep
 * track of all of the active commands issued by the DHS library.  This
 * includes maintaining an eptr list of all current commands.
 *
 * FUNCTION NAME(S)
 * - cmdCompare: Compare to tDhsCmd strucutres for eptrSearch.
 * - dhsBdResp: This function is called when a bulk data response is
 *		received for a command.
 * - dhsCmdConnectLost: Called when a connection is lost.
 * - dhsCmdDecode: Decode a message received on a connection.
 * - dhsCmdDelete: Delete a command and remove it from the eptr list.
 * - dhsCmdDeref: Decrement the command reference count by one, and if
 *		the reference count goes to 0, delete the command.
 * - dhsCmdDone: Test to see if a command is in a terminal state.
 * - dhsCmdFind: Find a command in the eptr list, searching by various
 *		search criteria.
 * - dhsCmdMsgFmt: Format a message to be sent over a connection.
 * - dhsCmdNew: Create a new command structure, initialize it, and add 
 *		it to the eptr list.
 * - dhsCmdRef: Increment the command reference counter.
 * - dhsCmdResp: This function is called when a command response is received
 *		for a command.
 * - dhsCmdStart: Return true if processing of a command has started, false
 *		otherwise.
 * - init: Initialize the static variables in this file.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.29  2001/03/27 20:24:21  nhill
 * Added vxworks.h when compiling for vxworks.
 * Removed some unused variables.
 *
 * Revision 1.28  1999/02/01 21:02:35  nhill
 * Fixed the problem of coordinating deletion of bulk data shared memory
 * by removing it. The memory should still be freed in function bulkTransfer.
 *
 * Revision 1.27  1999/01/27 23:20:59  nhill
 * Bug fix.
 *
 * Revision 1.26  1998/08/04 16:13:00  nhill
 * Rearranged to code to be more object-oriented.
 *
 * Revision 1.25  1998/06/02 21:12:35  nhill
 * Fixed bug that allowed deadlocks when a command is lost.
 *
 * Revision 1.24  1998/05/25 19:51:32  nhill
 * Fixed a bug that caused the wrong bulk data get type to be indicated
 * in the get callback.
 *
 * Revision 1.23  1998/05/13 22:30:06  nhill
 * Changed to ensure a dhsWait will not complete until after the callback
 * has been executed.
 *
 * Revision 1.22  1998/02/16 23:39:00  nhill
 * Changed to ignore all responses received after a command is complete.
 *
 * Revision 1.21  1998/01/20 18:20:23  nhill
 * Changed to ignore all messages setting the command state to DHS_CS_PENDING.
 *
 * Revision 1.20  1997/12/18 21:29:00  nhill
 * Added code to decrement the command reference counter twice when
 * a command is lost and the command incremented the counter because it
 * was using shared memory.
 *
 * Revision 1.19  1997/09/26 18:03:36  nhill
 * Improved handling of command deletion.
 *
 * Revision 1.18  1997/09/17 20:28:37  nhill
 * Changed to free shared memory blocks as early as possible after a
 * response is received.
 * Added the dhsCmdRefCount function.
 *
 * Revision 1.17  1997/08/29 22:32:43  nhill
 * Fixed a memory leak.
 *
 * Revision 1.16  1997/07/10 19:19:20  nhill
 * Bug fix.
 *
 * Revision 1.15  1997/07/10 19:07:51  nhill
 * Fixed a memory leak.
 *
 * Revision 1.14  1997/03/26 17:40:45  nhill
 * Fixed some lint problems.
 *
 * Revision 1.13  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.12  1997/02/24 19:17:17  nhill
 * Checkin for install.
 *
 * Revision 1.11  1997/02/04 19:41:34  nhill
 * Checkpoint for install.
 *
 * Revision 1.10  1997/01/30 17:18:54  nhill
 * Checkin for install.
 *
 * Revision 1.9  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.8  1997/01/28 18:59:50  nhill
 * Checkin before install.
 *
 * Revision 1.7  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.6  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.5  1997/01/20 19:01:42  nhill
 * Checking for install.
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
 
#ifdef vxWorks
#include <vxWorks.h> /* must come before stdarg.h */
#endif
#include <string.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"


/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"


/*
 ************************************************************************
 *  Static variables.
 ************************************************************************
 */

static EPTR		cmdList;	/* List of all active commands.	*/
static boolean		initialized = FALSE;
					/* Have the static variables 	*/
					/* been initalized?		*/
static tDhsMutex	mutex; 		/* Used to limit access to the	*/
					/* cmdList static variable.	*/


/*
 ************************************************************************
 *  Static functions.
 ************************************************************************
 */

static int		cmdCompare( tDhsCmd *, tDhsCmd * );
static void		init( DHS_STATUS * );


/*
 ************************************************************************
 *  Local constants.
 ************************************************************************
 */

#define	RESPONSE_INCR	10	/* Amount to increment command response	*/
				/* list by when it is full.		*/

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * cmdCompare
 *
 * INVOCATION:
 * diff = cmdCompare( &tag, pDhsCmd2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd1 (tDhsCmd *) First command to compare.
 * (>) pCmd2 (tDhsCmd *) Sencond command to compare.
 *
 * FUNCTION VALUE:
 * (int) -1 if *tag < *pmd, 0 if *tag == *pCmd,	1 if *tag > *pCmd
 *
 * PURPOSE:
 * Compare two tDhsCmd structures for eptrSearch
 *
 * DESCRIPTION:
 * Get the tags of the commands and return the difference.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Both commands should have been created with dhsCmdNew.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static int	cmdCompare
(
    tDhsCmd	*pCmd1,		/* (in)  First command to compare.	*/
    tDhsCmd	*pCmd2		/* (in)  Second command to compare.	*/
)
{
    return( pCmd2 - pCmd1 );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdResp
 *
 * INVOCATION:
 * dhsBdResp( pCmd, cmdStatus, avList, string, bulkLength, pBulk, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd (tDhsCmd *) Command associated with the response.
 * (>) cmdStatus (DHS_CMD_STATUS) New status for the command.
 * (>) avList (DHS_AV_LIST) Av list sent with the response.
 * (>) string (char *) Text string sent with the response.
 * (>) bulkLength (ulong) Length of the bulk data buffer.
 * (>) pBulk (void *) Pointer to the bulk data buffer.
 * (!) pstatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Process bulk data response
 *
 * DESCRIPTION:
 * Update the state of the command, add the attribute value list to the
 * list of command responses and then execute the bulk data get callback
 * function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		 dhsBdResp
(
    tDhsCmd	*pCmd,		/* (in)  command response.		*/
    DHS_CMD_STATUS
    		cmdStatus,	/* (in)  The new command state.		*/
    DHS_AV_LIST	avList,		/* (in)  The SDS id.			*/
    char	*string,	/* (in)  Response string.		*/
    ulong	bulkLength,	/* (in)  Length of the bulk data buffer.*/
    void	*pBulk,		/* (in)  Pointer to bulk data buffer.	*/
    DHS_STATUS	*pStatus	/* (mod) Funciton return status.	*/
)
{
    DHS_AV_LIST	copyAvList;	/* Copy of the av list.			*/


    fnEntry( "dhsBdResp" );
    statCheck( *pStatus, VOID );


    /*
     *  Add the response to the list of responses in the command.  The 
     *  command mutex is locked while this is doen to ensure no other 
     *  thread is accessing or modifying the command.
     */

    dhsMutexLock( &( pCmd->dcMutex ), pStatus );


    /*
     *  Set the command response string to the string received with the
     *  response.
     */

    if ( pCmd->dcString != NULL )
    {
	free( pCmd->dcString );
    }

    if ( strlen( string ) == 0 )
    {
	pCmd->dcString = NULL;
    }
    else
    {
	pCmd->dcString = strsav( string );
    }


    /*
     *  Set the command status.
     */

    pCmd->dcState = cmdStatus;


    /*
     *  Add the av list to the list of responses.
     */

    pCmd->dcNumResp ++;
    if ( pCmd->dcNumResp > pCmd->dcRespSize )
    {
	/*
	 *  The list of responses is full, allocate some more space.
	 */

	nullCheck( pCmd->dcResponse = realloc( pCmd->dcResponse, 
		( pCmd->dcRespSize + RESPONSE_INCR ) * 
		sizeof ( DHS_AV_LIST ) ), *pStatus, VOID );
	pCmd->dcRespSize += RESPONSE_INCR;
    }


    /*
     *  Insert the av list into the list of responses.
     */

    if ( avList == DHS_AV_LIST_NULL )
    {
	copyAvList = avList;
    }
    else
    {
	copyAvList = dhsAvListCopy( avList, pStatus );
	if ( *pStatus != DHS_S_SUCCESS )
	{
	    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );
	    return;
	}
	    
    }
    pCmd->dcResponse[ pCmd->dcNumResp - 1 ] = copyAvList;

    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );


    /*
     *  Copy the bulk data.
     */

    if ( pCmd->dcBulkPtr != NULL )
    {
	free( pCmd->dcBulkPtr );
	pCmd->dcBulkPtr = NULL;
    }

    if ( pBulk != NULL )
    {
	nullCheck( pCmd->dcBulkPtr = malloc( bulkLength ), *pStatus, VOID );
	memcpy( pCmd->dcBulkPtr, pBulk, bulkLength );
    }


    /*
     *  Execute the bulk data get callback.
     */

    if ( ( ( pCmd->dcFlags & DHS_CF_CALLBACK ) == DHS_CF_CALLBACK ) && 
	    dhsLocal.dlCBFGet != NULL )
    {
	dhsLocal.dlCBFGet( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		pCmd->dcName, pCmd->dcGetType, cmdStatus, pCmd->dcString, 
		copyAvList, pCmd->dcBulkPtr, bulkLength, pCmd->dcUserData );
    }

    if ( dhsCmdDone( pCmd, pStatus ) )
    {
	pCmd->dcCmdDone = TRUE;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdConnectLost
 *
 * INVOCATION:
 * dhsCmdConnectLost( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection lost.
 * (!) pStatus (DHS_STATUS *) Descripton of the parameter
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Mark all command issued to a connection as being lost
 *
 * DESCRIPTION:
 * - Loop through the list of commands making a list of commands associated
 *   with the lost connection.
 * - Loop through the list of lost commands changing their state to 
 *   DHS_CS_LOST, and executing the appropriate callback function.
 *
 * EXTERNAL VARIABLES:
 * - cmdList: Eptr list of all commands.
 * - mutex: Mutex to ensure only one thread manipulates the static variables.
 * - initialized: Indicates if the static variables have been initialized.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsCmdConnectLost
(
    tDhsConnect	*pConnect,	/* (in)  The connection lost.		*/
    DHS_STATUS	*pStatus	/* (mod) The function return status.	*/
)
{
    int		i;
    IMP_Status	impStatus;	/* IMP function return status.		*/
    tDhsCmd	*pCmd;		/* Command being processed.		*/
    EPTR	tagList;	/* List of tags to delete.		*/


    fnEntry( "dhsCmdConnectLost" );
    statCheck( *pStatus, VOID );


    /*
     *  Ensure the static variables have been initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, VOID );
    }


    /*
     *  Lock the mutex protecting the list of commands.
     */

    dhsMutexLock( &mutex, pStatus );


    /*
     *  Initialize the eptr list of tags to delete.
     */

    if ( eptr_init( &tagList, eptrlen( &cmdList ) ) == NULL )
    {
	dhsMutexUnlock( &mutex, pStatus );
	dhsMsgFmt( DHS_E_MEMORY, DHS_DEBUG_OFF );
	*pStatus = DHS_E_MEMORY;
	return;
    }


    /*
     *  Loop through the list of commands finding those which belong
     *  to the specified connection, and which are not yet complete,
     *  change the command state to DHS_CS_LOST, and add them to the list
     *  of commands to delete. The command reference counter is incremented
     *  to prevent the commands from disapearing.
     */

    for ( i = 0; i < eptrlen( &cmdList ); i++ )
    {
	pCmd = eptritem( &cmdList, i );
	dhsMutexLock( &( pCmd->dcMutex ), pStatus );
	if  ( pCmd->dcSentTo == pConnect && ! dhsCmdDone( pCmd, pStatus )
		&& pCmd->dcRefCount > 0 )
	{
	    pCmd->dcRefCount ++;
	    pCmd->dcState = DHS_CS_LOST;
	    pCmd->dcCmdDone = TRUE;
	    eptr_insert( &tagList, eptrlen( &tagList ), pCmd );
	}
	dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );
    }

    /*
     * Unlock the command list mutex.
     */

    dhsMutexUnlock( &mutex, pStatus );


    /*
     *  Loop throught the list of lost commands, executing the appropriate
     *  callback function to allow the application to clean up.
     */

    for ( i = 0; i < eptrlen( &tagList ); i++ )
    {
	pCmd = eptritem( &tagList, i );
	if ( ( ( pCmd->dcFlags & DHS_CF_CALLBACK ) == DHS_CF_CALLBACK ) && 
		pCmd->dcType == DHS_MSG_APPLY && 
		dhsLocal.dlCBFResponse != NULL )
	{
	    dhsLocal.dlCBFResponse( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		    pCmd->dcName, pCmd->dcState, pCmd->dcString, 
		    DHS_AV_LIST_NULL, pCmd->dcUserData );
	}
	else if ( ( ( pCmd->dcFlags & DHS_CF_CALLBACK ) == DHS_CF_CALLBACK ) && 
		pCmd->dcType == DHS_MSG_BD_GET && dhsLocal.dlCBFGet != NULL )
	{
	    dhsLocal.dlCBFGet( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		    pCmd->dcName, pCmd->dcGetType, pCmd->dcState, 
		    pCmd->dcString, DHS_AV_LIST_NULL, NULL, 0, 
		    pCmd->dcUserData );
	}
	else if ( ( ( pCmd->dcFlags & DHS_CF_CALLBACK ) == DHS_CF_CALLBACK ) && 
		pCmd->dcType == DHS_MSG_BD_PUT && dhsLocal.dlCBFPut != NULL )
	{
	    dhsLocal.dlCBFPut( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		    pCmd->dcState, pCmd->dcString, pCmd->dcName, 
		    pCmd->dcUserData );
	}
	else if ( dhsLocal.dlCBFError != NULL )
	{
	    dhsMsgFmt( DHS_E_CMD_LOST, DHS_DEBUG_OFF );
	    dhsMsgCallback( dhsConnectId( pConnect ), dhsCmdTag( pCmd ) );
	}

	if ( pCmd->dcSharedMemUsed )
	{
	    impStatus = IMP_STATUS_OK;
	    impCheck( ImpReleaseShared( &(pCmd->dcSharedMem), TRUE, 
		    &impStatus ), impStatus, *pStatus, VOID );
	    pCmd->dcSharedMemUsed = FALSE;
	    dhsCmdDeref( pCmd, pStatus );
	}
	    
	/*
	 * Decrement the reference counter to allow the command to be
	 * deleted.
	 */

	dhsCmdDeref( pCmd, pStatus );
    }
    eptr_clear( &tagList, NULL );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdDecode
 *
 * INVOCATION:
 * dhsCmdDecode( address, &name, &avList, &bulkData, &bulkLength );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) address (byte *) Address of the message to decode.
 * (<) pName (char **) Returned pointer to command name.
 * (<) pAvList (DHS_AV_LIST * ) Return attribute value list.
 * (<) pCmdState (DHS_CMD_STATUS *) Returned status of the command.
 * (<) pBulkData (void **) Returned pointer to the bulk data buffer.
 * (<) pBulkLength (ulong *) Returned bulk data length.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Decode a user message received from another system
 *
 * DESCRIPTION:
 * Decode a message encoded with the dhsCmdMsgFmt function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsCmdDecode
(
    byte	*address,	/* (in)  Address of the message to decode*/
    int		length,		/* (in)  Length of the message.		*/
    char	**pName,	/* (out) Returned name of the command.	*/
    DHS_AV_LIST	*pAvList,	/* (out) Returned attribute value list.	*/
    DHS_CMD_STATUS
    		*pCmdState,	/* (out) New state of the command.	*/
    byte	**pBulkData,	/* (out) Pointer to bulk data buffer.	*/
    ulong	*pBulkLength,	/* (out) Length of bulk data buffer.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    ulong	avListLength;	/* Length of the av list.		*/
    ulong	headerLength;	/* Length of the message header.	*/
    int		i;
    

    fnEntry( "dhsCmdDecode" );
    statCheck( *pStatus, VOID );
    

    /*
     *  Get the lengths of the message components.
     */

    if ( length < 35 )
    {
	dhsMsgFmt( DHS_E_MSG_LENGTH, DHS_DEBUG_OFF, length, ">= 35" );
	*pStatus = DHS_E_MSG_LENGTH;
	fnReturn( VOID );
    }


    (void) sscanf( (char *) address, "%10lu%10lu%10lu%5d", &headerLength, 
	    &avListLength, pBulkLength, &i );
    *pCmdState = i;


    if ( length != headerLength + avListLength + *pBulkLength )
    {
	char	l[20];
	sprintf( l, "%lu", headerLength + avListLength + *pBulkLength ); 
	dhsMsgFmt( DHS_E_MSG_LENGTH, DHS_DEBUG_OFF, length, l );
	*pStatus = DHS_E_MSG_LENGTH;
	fnReturn( VOID );
    }


    /* 
     * The name follows the lengths.
     */

    *pName = (char *)( address + 35 );


    /*
     *  Get the attribute value list.
     */

    if ( avListLength == 0 )
    {
	/* 
	 * No av list is present, so set it to the null value.
	 */

	*pAvList = DHS_AV_LIST_NULL;
    }
    else
    {
	/*
	 * An av list is present, so make it available with an dhsAvListAccess.
	 */

	*pAvList = dhsAvListAccess( address + headerLength, pStatus );
    }


    /*
     * The bulk data follows the attribute value list if it is present.
     */

    *pBulkData = address + headerLength + avListLength;

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdDelete
 *
 * INVOCATION:
 * dhsCmdDelete( pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd (tDhsCmd *) The command to delete.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Remove a command from the list of commands and delete the command
 *
 * DESCRIPTION:
 * Remove the command from the eptr list of all commands, destroy the
 * command mutex, reslease any shared memory allocated to the command
 * and free any memory allocated to the command.
 *
 * EXTERNAL VARIABLES:
 * - cmdList: Eptr list of all commands.
 * - mutex: Mutex to ensure only one thread manipulates the static variables.
 *
 * PRIOR REQUIREMENTS:
 * The command should have been created with dhsCmdNew.
 *
 * DEFICIENCIES:
 * This function doesn't check to make sure the static variables have been
 * initialized. This isn't be necessary if the function is used correctly.
 *-
 ************************************************************************
 */

void		dhsCmdDelete
(
    tDhsCmd	*pCmd,		/* (in)  The command to delete.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* A response av list.			*/
    int		i;
    IMP_Status	impStatus;	/* Imp function return status.		*/
    int		index;		/* Index into the eptr list.		*/


    fnEntry( "dhsCmdDelete" );
    statCheck( *pStatus, VOID );


    dhsMsgFmt( DHS_S_CMD_DELETE, DHS_DEBUG_ON, dhsCmdTag( pCmd ) );


    /* 
     *  Lock the mutex to ensure no other thread is manipulating the
     *  eptr list.
     */

    dhsMutexLock( &mutex, pStatus );


    /* 
     *  Find the command in the list and remove it.
     */

    if ( eptr_search( &cmdList, pCmd, &index, cmdCompare ) )
    {
	eptr_del( &cmdList, index, 1, NULL );
    }

    dhsMutexUnlock( &mutex, pStatus );


    /*
     *  if the command is using a shared memory buffer, free it.
     */

    if ( pCmd->dcSharedMemUsed )
    {
	impStatus = IMP_STATUS_OK;
	impCheck( ImpReleaseShared( &( pCmd->dcSharedMem ), TRUE, &impStatus ),
		impStatus, *pStatus, VOID );
	pCmd->dcSharedMemUsed = FALSE;
    }


    /*
     *  Destroy the command mutex.
     */

    dhsMutexDestroy( &( pCmd->dcMutex ), pStatus );


    /*
     *  Free any memory allocated for the command.
     */

    if ( pCmd->dcName != NULL )
    {
	free( pCmd->dcName );
    }

    if ( pCmd->dcNumResp != 0 )
    {
	for ( i = 0; i < pCmd->dcNumResp; i++ )
	{
	    avList = pCmd->dcResponse[ i ];
	    check( dhsAvListFree( avList, pStatus ), *pStatus, VOID )
	}
	free( pCmd->dcResponse );
    }

    if ( pCmd->dcString != NULL )
    {
	free( pCmd->dcString );
    }

    if ( pCmd->dcBulkPtr != NULL && pCmd->dcType == DHS_MSG_BD_GET )
    {
	free( pCmd->dcBulkPtr );
    }

    free( pCmd );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdDeref
 *
 * INVOCATION:
 * dhsCmdDeref( pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCmd (tDhsCmd *) The command to change.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Decrement the reference count for a command and delete it when it hits 0
 *
 * DESCRIPTION:
 * Lock the mutex, decrement the reference count, unlock the mutex, and if 
 * the reference count is zero, delete the command.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The command must have been created with dhsCmdNew.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsCmdDeref
(
    tDhsCmd	*pCmd,		/* (mod) The command to change.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		count;		/* Number of references.		*/


    fnEntry( "dhsCmdDeref" );
    statCheck( *pStatus, VOID );


    /*
     *  Decrement the reference count. This is done with the command
     *  mutex lock to ensure no other thread is manipulating the reference
     *  count.
     */

    dhsMutexLock( &( pCmd->dcMutex ), pStatus );
    pCmd->dcRefCount--;
    count = pCmd->dcRefCount;
    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );

    if ( count <= 0 )
    {
	dhsCmdDelete( pCmd, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdDone
 *
 * INVOCATION:
 * isDone = dhsCmdDone( pcmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd (tDhsCmd *) Pointer to the command to test.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (boolean) True if the command is in a end state, false otherwise.
 *
 * PURPOSE:
 * Test to see if a command is in an end state
 *
 * DESCRIPTION:
 * Test the status value against all possible end states.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean		dhsCmdDone
(
    tDhsCmd 	*pCmd,		/* (in)  Pointer to the command to test.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    return( pCmd->dcState == DHS_CS_ERROR || pCmd->dcState == DHS_CS_DONE ||
	    pCmd->dcState == DHS_CS_LOST || pCmd->dcState == DHS_CS_ABORTED );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdFind
 *
 * INVOCATION:
 * pDhsCmd = dhsCmdFind( DHS_CF_PTR, &status, tag );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) type (tDhsCmdFind) Indicates the search criteria type.
 * (!) pStatus (DHS_STATUS *) Function return status.
 * (>) ... (?) Search criteria arguments.
 *
 * FUNCTION VALUE:
 * (tDhsCmd *) The command structure located.
 *
 * PURPOSE:
 * Locate a specified command structure
 *
 * DESCRIPTION:
 * Get the search criteria from the argument list and search for it.
 *
 * EXTERNAL VARIABLES:
 * - cmdList: Eptr list of all commands.
 * - initialized: Indicates if the static variables have been initialized.
 * - mutex: Mutex to ensure only one thread manipulates the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

tDhsCmd		*dhsCmdFind
(
    tDhsCmdFind	type,		/* (in)  Type of data to look for.	*/
    DHS_STATUS	*pStatus,	/* (mod) Function return status.	*/
    ...
)
{
    va_list	args;
    int		index;		/* Index into the eptr list.		*/
    tDhsCmd	*pCmd;		/* The command found.			*/
    DHS_STATUS	s;		/* Temporary function return status.	*/


    fnEntry( "dhsCmdFind" );
    statCheck( *pStatus, NULL );


    /*
     *  Ensure the static variables have been initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, NULL );
    }


    /*
     *  Search for the value depending on the type of search.
     */

    va_start( args, pStatus );
    switch ( type )
    {
	case	DHS_CF_PTR:
	    /*
	     *  Search by command tag.
	     */

	    pCmd = va_arg( args, tDhsCmd * );


	    /*
	     *  Lock the mutex to ensure no other thread is manipulating the
	     *  eptr list.
	     */

	    dhsMutexLock( &mutex, pStatus );
	    if ( ! eptr_search( &cmdList, pCmd, &index, cmdCompare ) )
	    {
		/*
		 *  The command was not found.
		 */

		*pStatus = DHS_E_CMD_NOT_FOUND;
		pCmd = NULL;
	    }
	    s = DHS_S_SUCCESS;
	    dhsMutexUnlock( &mutex, &s );
	    if ( s != DHS_S_SUCCESS && *pStatus == DHS_S_SUCCESS )
	    {
		*pStatus = s;
	    }
	    break;
	default:
	    /*
	     *  Invalid search type.
	     */

	    dhsMsgFmt( DHS_E_CMD_FIND, DHS_DEBUG_OFF, type );
	    pCmd = NULL;
    }
    va_end( args );

    fnReturn( pCmd );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdMsgFmt
 *
 * INVOCATION:
 * dhsCmdMsgFmt( &cmd, dataPtr, headerLen, avListLen, bulkLength, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd (tDhsCmd *) The command to format.
 * (!) dataPtr (void *) Pointer to the buffer to receive the data.
 * (>) headerLength (ulong) Length of the header part.
 * (>) avListLength (ulong) Length of exported av list.
 * (>) bulkLength (ulong) Length of the bulk part.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Format a command into a buffer
 *
 * DESCRIPTION:
 * - Format the lengths of the message elements into the message header.
 * - Write the attribute value list to the message.
 * - Write any bulk data to the message.
 *
 * The current format of the message is:
 * - A message header containing:
 *   - The length of the message header. (Formated as a 10 character string.)
 *   - The length of the attribute value list part of the message. (Formatted
 *     as a 10 character string.)
 *   - The length of the bulk data part of the message. (Formatted as a 10
 *     character string.)
 *   - A new state for the command. (An integer value formatted as a 5
 *     character string.)
 *   - The command name.
 * - An attribute value list part of the messages, which is an exported
 *   SDS structure.
 * - A bulk data part of the message.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The pCmd parameter msut point to a valid command structure.
 * The pStatus parameter must contain DHS_S_SUCCESS.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsCmdMsgFmt
(
    tDhsCmd	*pCmd,		/* (in)  The command to format.		*/
    void	*dataPtr,	/* (mod) Pointer to the data buffer.	*/
    ulong	headerLength,	/* (in)  length of the header part.	*/
    ulong	avListLength,	/* (in)  Length of exported av list.	*/
    ulong	bulkLength,	/* (in)  Length of the bulk part.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    StatusType	sdsStatus;	/* Sds function return status.		*/


    fnEntry( "dhsCmdMsgFmt" );
    statCheck( *pStatus, VOID );


    sdsStatus = SDS__OK;


    /*
     *  Format the lengths of the various elements of the message into the
     *  header.
     */

    if ( pCmd->dcName == NULL )
    {
	(void) sprintf( dataPtr, "%010lu%010lu%010lu%05d", 
		headerLength, avListLength, bulkLength, pCmd->dcState );
    }
    else
    {
	(void) sprintf( dataPtr, "%010lu%010lu%010lu%05d%s", 
		headerLength, avListLength, bulkLength, pCmd->dcState, 
		pCmd->dcName );
    }


    /*
     *  Write the attribute value list into the message.
     */

    if ( pCmd->dcAvList != DHS_AV_LIST_NULL )
    {
	SdsExport( dhsSdsId( pCmd->dcAvList ), avListLength, 
		( (char *) dataPtr) + headerLength, &sdsStatus );
    }


    /*
     *  Write the bulk data into the message.*
     */

    if ( pCmd->dcBulkType == DHS_BT_SDS )
    {
	/*
	 *  Bulk data is an SDS structure, so use SdsExport.
	 */

	if ( pCmd->dcDataset != DHS_BD_DATASET_NULL )
	{
	    sdsCheck( SdsExport( dhsSdsId( pCmd->dcDataset ), bulkLength, 
		    ( (char *) dataPtr) + headerLength + avListLength, 
		    &sdsStatus ), sdsStatus, *pStatus, VOID );
	}
    }
    else if ( pCmd->dcBulkType == DHS_BT_BUFFER )
    {
	/*
	 *  Bulk data is a buffer, so just copy it.
	 */

	memcpy( ( (char *) dataPtr) + headerLength + avListLength, 
		pCmd->dcBulkPtr, bulkLength );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdNew
 *
 * INVOCATION:
 * pDhsCmd = dhsCmdNew( command, avList, pUserData, type, TRUE, DHS_TAG_NULL,
 *		DHS_CS_PENDING, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) command (char *) The name of the command.
 * (>) avList (DHS_AV_LIST) The av list to send with the command.
 * (>) pUserData (void *) User data pointer for the command.
 * (>) type (tDhsUsrMsg) Type of the command.
 * (>) flags (tCmdFlag) Flags associated with the command.
 * (>) tag (DHS_TAG) Specific tag to assign to the command.
 * (>) cmdStatus (DHS_CMD_STATUS) Initial command status.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (tDhsCmd *) Pointer to the new command structure.
 *
 * PURPOSE:
 * Create a new command structure with specified initial values
 *
 * DESCRIPTION:
 * Create and initialialize a command structure, and then add it to the
 * list of all commands.
 *
 * EXTERNAL VARIABLES:
 * - cmdList: Eptr list of all commands.
 * - initialized: Indicates if the static variables have been initialized.
 * - mutex: Mutex to ensure only one thread manipulates the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

tDhsCmd		*dhsCmdNew
(
    const char	*command,	/* (in)  Command name.			*/
    DHS_AV_LIST	avList,		/* (in)  Commands av list.		*/
    void	*pUserData,	/* (in)  User data pointer.		*/
    tDhsUsrMsg	type,		/* (in)  The type of message.		*/
    tCmdFlag	flags,		/* (in)  Flags associated with the cmd.	*/
    DHS_TAG	tag,		/* (in)  The tag id.			*/
    DHS_CMD_STATUS
    		cmdStatus,	/* (in)  Initial command pStatus.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* The command created.			*/
    int		index;		/* Index into the eptr list.		*/


    fnEntry( "dhsCmdNew" );
    statCheck( *pStatus, NULL );


    /*
     *  Ensure the static variables have been initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, NULL );
    }


    /* 
     * Create a new command structure.
     */

    nullCheck( pCmd = (tDhsCmd * ) malloc( sizeof( tDhsCmd ) ), 
	    *pStatus, NULL );

    dhsMsgFmt( DHS_S_CMD_NEW, DHS_DEBUG_ON, dhsCmdTag( pCmd ) );

    
    /*
     * Initialize the fields of the command structure.
     */

    pCmd->dcBulkType = DHS_BT_NONE;
    pCmd->dcBulkPtr = NULL;
    pCmd->dcBulkLen = 0;
    pCmd->dcCmdDone = FALSE;
    pCmd->dcDataset = DHS_BD_DATASET_NULL;
    pCmd->dcUserData = pUserData;
    pCmd->dcAvList = avList;
    pCmd->dcGetType = DHS_BD_GT_UNKNOWN;
    pCmd->dcMsgSize = 0;
    pCmd->dcState = cmdStatus;
    pCmd->dcType = type;
    pCmd->dcFlags = flags;
    pCmd->dcSentTo = NULL;
    pCmd->dcResponse = NULL;
    pCmd->dcRetries = 0;
    pCmd->dcNumResp = 0;
    pCmd->dcRefCount = 1;
    pCmd->dcRespSize = 0;
    pCmd->dcString = NULL;
    pCmd->dcSharedMemUsed = FALSE;
    dhsMutexInit( &( pCmd->dcMutex ), pStatus );
    if ( command != NULL )
    {
	nullCheck( pCmd->dcName = malloc( strlen( command ) + 1 ), 
		*pStatus, NULL );
	(void) strcpy( pCmd->dcName, command );
    }
    else
    {
	pCmd->dcName = NULL;
    }


    /*
     *  If no specific tag was assigned, assign one.
     */

    if ( tag == DHS_TAG_NULL )
    {
	pCmd->dcTag = dhsCmdTag( pCmd );
    }
    else
    {
	pCmd->dcTag = tag;
    }


    /*
     *  Add the command to the list of commands.  This is done with the 
     *  mutex locked to ensure no other thread is manipulating the list.
     */

    dhsMutexLock( &mutex, pStatus );
    (void) eptr_search( &cmdList, pCmd, &index, cmdCompare );
    nullCheck( eptr_insert( &cmdList, index, pCmd ), *pStatus, NULL );
    dhsMutexUnlock( &mutex, pStatus );

    fnReturn( pCmd );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdRef
 *
 * INVOCATION:
 * dhsCmdRef( pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCmd (tDhsCmd *) The command to change.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Increment the reference count for a command
 *
 * DESCRIPTION:
 * Lock the mutex, increment the reference count, and the unlock the
 * mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The command must have been created with dhsCmdNew.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsCmdRef
(
    tDhsCmd	*pCmd,		/* (mod) The command to change.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		index;
    DHS_STATUS	s;


    fnEntry( "dhsCmdRef" );
    statCheck( *pStatus, VOID );


    dhsMutexLock( &( mutex ), pStatus );
    if ( ! eptr_search( &cmdList, pCmd, &index, cmdCompare ) )
    {
	dhsMutexUnlock( &( mutex ), pStatus );
	*pStatus = DHS_E_CMD_NOT_FOUND;
	fnReturn( VOID );
    }

    dhsMutexLock( &( pCmd->dcMutex ), pStatus );
    if ( pCmd->dcRefCount == 0 )
    {
	dhsMsgFmt( DHS_E_CMD_DELETED, DHS_DEBUG_OFF, pCmd );
	*pStatus = DHS_E_CMD_DELETED;
    }
    else
    {
	pCmd->dcRefCount ++;
    }

    s = DHS_S_SUCCESS;
    dhsMutexUnlock( &( pCmd->dcMutex ), &s );
    dhsMutexUnlock( &( mutex ), &s );
    if ( *pStatus == DHS_S_SUCCESS )
    {
	*pStatus = s;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdRefCount
 *
 * INVOCATION:
 * count = dhsCmdRefCount( pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCmd (tDhsCmd *) The command to check.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Return the value of a commands reference counter.
 *
 * DESCRIPTION:
 * Lock the mutex, get the reference count value, return the value. and
 * then unlock the mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The command must have been created with dhsCmdNew.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

int		dhsCmdRefCount
(
    tDhsCmd	*pCmd,		/* (mod) The command to check.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		index;
    int		count;
    DHS_STATUS	s;


    fnEntry( "dhsCmdRefCount" );
    statCheck( *pStatus, 0 );


    dhsMutexLock( &( mutex ), pStatus );
    if ( ! eptr_search( &cmdList, pCmd, &index, cmdCompare ) )
    {
	dhsMutexUnlock( &( mutex ), pStatus );
	*pStatus = DHS_E_CMD_NOT_FOUND;
	fnReturn( 0 );
    }

    dhsMutexLock( &( pCmd->dcMutex ), pStatus );
    count = pCmd->dcRefCount;

    s = DHS_S_SUCCESS;
    dhsMutexUnlock( &( pCmd->dcMutex ), &s );
    dhsMutexUnlock( &( mutex ), &s );

    if ( *pStatus == DHS_S_SUCCESS )
    {
	*pStatus = s;
    }

    fnReturn( count );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdResp
 *
 * INVOCATION:
 * dhsCmdResp( pCmd, cmdStatus, avList, string, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd (tDhsCmd *) Pointer to the command structure.
 * (>) cmdStatus (DHS_CMD_STATUS) The new command status.
 * (>) avList (DHS_AV_LIST) The av list given with the response.
 * (>) string (char *) The response text string.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a response to a command
 *
 * DESCRIPTION:
 * Update the state of the command, add the attribute value list to the
 * list of command responses and then execute the command response
 * callback function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The command structure should have been create with the dhsCmdNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		 dhsCmdResp
(
    tDhsCmd	*pCmd,		/* (in)  command response.		*/
    DHS_CMD_STATUS
    		cmdStatus,	/* (in)  The new command state.		*/
    DHS_AV_LIST	avList,		/* (in)  The SDS id.			*/
    char	*string,	/* (in)  Response string.		*/
    DHS_STATUS	*pStatus	/* (mod) Funciton return status.	*/
)
{
    DHS_AV_LIST	copyAvList;	/* Copy of the av list.			*/


    fnEntry( "dhsCmdResp" );
    statCheck( *pStatus, VOID );


    if ( cmdStatus == DHS_CS_PENDING || dhsCmdDone( pCmd, pStatus ) )
    {
	fnReturn( VOID );
    }


    /*
     *  Lock the command mutex to ensure no other thread is manipulating
     *  the command.
     */

    dhsMutexLock( &( pCmd->dcMutex ), pStatus );


    /*
     *  Update the command string.
     */

    if ( pCmd->dcString != NULL )
    {
	free( pCmd->dcString );
    }

    if ( strlen( string ) == 0 )
    {
	pCmd->dcString = NULL;
    }
    else
    {
	pCmd->dcString = strsav( string );
    }


    /*
     *  Update the command state.
     */

    pCmd->dcState = cmdStatus;


    /*
     *  Add the attribute value list to the list of responses.
     */

    pCmd->dcNumResp ++;
    if ( pCmd->dcNumResp > pCmd->dcRespSize )
    {
	/*
	 *  If the list of responses is full, extend the list.
	 */

	nullCheck( pCmd->dcResponse = realloc( pCmd->dcResponse, 
		( pCmd->dcRespSize + RESPONSE_INCR ) * 
		sizeof ( DHS_AV_LIST ) ), *pStatus, VOID );
	pCmd->dcRespSize += RESPONSE_INCR;
    }
    if ( avList == DHS_AV_LIST_NULL )
    {
	copyAvList = DHS_AV_LIST_NULL;
    }
    else
    {
	copyAvList = dhsAvListCopy( dhsSdsAvList(  avList ), pStatus );
	if ( *pStatus != DHS_S_SUCCESS )
	{
	    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );
	    return;
	}
    }

    pCmd->dcResponse[ pCmd->dcNumResp - 1 ] = copyAvList;

    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );


    /*
     *  Execute the command response callback.
     */

    if ( ( ( pCmd->dcFlags & DHS_CF_CALLBACK ) == DHS_CF_CALLBACK ) )
    {
	if ( pCmd->dcType == DHS_MSG_APPLY && dhsLocal.dlCBFResponse != NULL )
	{
	    dhsLocal.dlCBFResponse( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		    pCmd->dcName, cmdStatus, pCmd->dcString, copyAvList, 
		    pCmd->dcUserData );
	}
	else if ( pCmd->dcType == DHS_MSG_BD_PUT && dhsLocal.dlCBFPut != NULL )
	{
	    dhsLocal.dlCBFPut( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		    pCmd->dcState, pCmd->dcString, pCmd->dcName, 
		    pCmd->dcUserData );
	}
	else if ( pCmd->dcType == DHS_MSG_BD_GET && dhsLocal.dlCBFGet != NULL )
	{
	    dhsLocal.dlCBFGet( pCmd->dcSentTo, dhsCmdTag( pCmd ), 
		    pCmd->dcName, pCmd->dcGetType, pCmd->dcState, 
		    pCmd->dcString, copyAvList, NULL, 0, pCmd->dcUserData );
	}
    }

    if ( dhsCmdDone( pCmd, pStatus ) )
    {
	pCmd->dcCmdDone = TRUE;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdStart
 *
 * INVOCATION:
 * started = dhsCmdStart( pCmd );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pCmd (tDhsCmd *) The command to check.
 *
 * FUNCTION VALUE:
 * (boolean) True if the execution of the command has started.
 *
 * PURPOSE:
 * Check to see if the execution of the command has started
 *
 * This function is only here to make it available as a condition function
 * to the event loop.
 *
 * DESCRIPTION:
 * Check the command state and return.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The command should have been created with dhsCmdNew.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean		dhsCmdStart
(
    tDhsCmd	*pCmd		/* (in)  Command to check.		*/
)
{
    return( pCmd->dcState != DHS_CS_PENDING );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * init
 *
 * INVOCATION:
 * init( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Initialize the static variables
 *
 * DESCRIPTION:
 * Initialize all static variables in this file.
 *
 * EXTERNAL VARIABLES:
 * - cmdList: Eptr list of all commands.
 * - initialized: Indicates if the static variables have been initialized.
 * - mutex: Mutex to ensure only one thread manipulates the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	init
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "init" );


    /*
     *  Initialize the eptr list.
     */

    nullCheck( eptr_init( &cmdList, 5 ), *pStatus, VOID );


    /*
     *  Initialize the mutex.
     */

    dhsMutexInit( &mutex, pStatus );

    initialized = TRUE;

    fnReturn( VOID );
}
