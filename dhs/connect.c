static char rcsid[] = "$Id: connect.c,v 1.1.1.1 2002-11-24 20:20:09 brighton Exp $";
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
 * dhs/src/connect.c
 *
 * PURPOSE:
 * Contains routines to manage connections.
 *
 * FUNCTION NAME(S)
 * connectCompare - Compare the tags of two connections.
 * dhsConnectAllClosed - Test to see if all client initiated connections
 *		have been closed.
 * dhsConnectBreak - Break a connection.
 * dhsConnectCloseAll - Close all connections to outside servers.
 * dhsConnectCount - Return the number of connections currently in existance.
 * dhsConnectDeref - Dereference a connection structure.
 * dhsConnected - Test to see if a connection is connected.
 * dhsConnectFind - Locate a connection by various search criteria.
 * dhsConnectInfo - Get information about a connection.
 * dhsConnectLock - Lock a connections mutex.
 * dhsConnectLost - Called when a connection is lost to close the connections.
 * dhsConnectMake - Make a connection to another application.
 * dhsConnectNew - Create a new connection structure.
 * dhsConnectRef - Reference a connection structure.
 * dhsConnectSend - Send a message to a connection.
 * dhsConnectUnlock - Unlock the mutex for a connection.
 * dhsDisconnected - Test to see if a connection is connected.
 * dhsServerExit - Request all clients close their connections.
 * init - Initialize the static variables.
 * mutexTry - Try to lock a mutex.
 * notified - Check to see if a message was received indicating space
 *		is available on the connection.
 * requestNotify - Send an Send a request notify message and wait until
 *		the notification is received.
 * sendBulk - Send a message using the IMP bulk transfer routines.
 * sendMsg - Send a message as a normal IMP message.
 * 
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.40  2001/03/27 20:27:23  nhill
 * Added DHS_CF_CONNECTING search type to dhsConnectFind.
 * Changed the DHS_CF_TASK_ID search type of dhsConnect find to use the
 * ImpSameMachine function, rather than directly comparing machine ids.
 *
 * Revision 1.39  1999/06/16 22:19:02  nhill
 * Fixed a bug in dhsConnect lock that could have caused deadlocks when
 * many commands are issued at the same time.
 *
 * Revision 1.38  1999/02/13 00:31:23  nhill
 * Fixed a bug that caused disconnect after a lost connection to hang.
 *
 * Revision 1.37  1999/02/12 18:14:20  nhill
 * Added search type DHS_CF_CON_ID2 to dhsConnectFind, which finds connections
 * by connection id, event those which are disconnected.
 * Changed dhsDisconnected to use search type DHS_CF_CON_ID2 when looking
 * for the connection.
 *
 * Revision 1.36  1998/08/04 22:20:43  nhill
 * Removed some debugging statements.
 *
 * Revision 1.35  1998/08/04 16:13:34  nhill
 * Rearranged to code to be more object-oriented.
 *
 * Revision 1.34  1998/06/02 18:47:00  nhill
 * Updated some documentation.
 * Removed function dhsConnectVerify.
 * Added function dhsConnectCount.
 *
 * Revision 1.33  1998/05/04 20:40:34  nhill
 * Added code to allow null datasets to be sent.
 *
 * Revision 1.32  1998/04/23 22:18:09  nhill
 * Changed the temporary file names to make it more obvious what
 * the files are given their names.
 *
 * Revision 1.31  1998/03/05 23:32:18  nhill
 * Changed to prevent usage of freed connect structures while
 * connections are being closed.
 *
 * Revision 1.30  1998/02/24 01:01:59  nhill
 * Added function dhsConnectCloseAll.
 * Bug fixes to allow the library to run again after being exited and
 * re-initialized.
 *
 * Revision 1.29  1998/02/16 23:39:21  nhill
 * Added function mutexTry and connectUnlock.
 * Changed to avoid deadlocking.
 *
 * Revision 1.28  1998/01/20 18:21:21  nhill
 * Added the connect callback executed when the connection state changes.
 *
 * Revision 1.27  1998/01/09 18:57:41  nhill
 * Added code to clear the taskId associated with a connection when ithe
 * connection is closed. This prevents a closed connetion from being
 * detected when looking for duplicate connections.
 *
 * Revision 1.26  1997/12/24 19:29:35  nhill
 * Changed the lockout variable to global.
 *
 * Revision 1.25  1997/12/18 21:35:26  nhill
 * Added functions dhsConnectMake and dhsConnectBreak.
 * Added the name and ip address to the connection structure.
 * Made changes required to allow automatic reconnection attempts.
 *
 * Revision 1.24  1997/11/30 21:11:51  nhill
 * *** empty log message ***
 *
 * Revision 1.23  1997/09/26 18:00:16  nhill
 * Improved checking for active messages while disconnecting.
 *
 * Revision 1.22  1997/09/17 20:30:45  nhill
 * Added more information to the temporary file names.
 * Improved handling of lost connections.
 * Added some missing mutex locks.
 *
 * Revision 1.21  1997/08/19 19:43:36  nhill
 * Changed impCheckNoRet to take a mutex to be unlocked if an error occurs.
 *
 * Revision 1.20  1997/07/24 21:27:14  nhill
 * Changed a call to the error callback to use dhsMsgCallback.
 * Removed the check for diconnecting connections in the connectLock function.
 *
 * Revision 1.19  1997/07/10 19:08:08  nhill
 * Changed some spelling.
 *
 * Revision 1.18  1997/05/20 17:37:37  nhill
 * Fixed a bug where a number at the start of a response was decoded as
 * part of the command status value in the header.
 *
 * Revision 1.17  1997/04/15 19:23:39  nhill
 * Changed the connect routines to use the original connection if a duplicate
 * connection is attempted.
 *
 * Revision 1.16  1997/03/26 17:40:45  nhill
 * Fixed some lint problems.
 *
 * Revision 1.15  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.14  1997/02/17 20:14:32  nhill
 * Added some debugging information.
 *
 * Revision 1.13  1997/02/04 20:22:55  nhill
 * Lint fixes.
 *
 * Revision 1.12  1997/02/04 19:41:34  nhill
 * Checkpoint for install.
 *
 * Revision 1.11  1997/01/30 17:18:54  nhill
 * Checkin for install.
 *
 * Revision 1.10  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.9  1997/01/28 18:59:50  nhill
 * Checking before install.
 *
 * Revision 1.8  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.7  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.6  1997/01/20 19:01:42  nhill
 * Checking for install.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


#include <netinet/in.h>
#include <arpa/inet.h>

#include "gen_types.h"
#include "gen_str.h"

/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"
#include "rwLock.h"


/*
 ************************************************************************
 *  Static variables.
 ************************************************************************
 */

static EPTR		connectList;	/* List of all connections.	*/
static boolean		initialized = FALSE;
					/* Are the static variables 	*/
					/* initialized?			*/
static tDhsRwLock	rwLock;		/* Used to limit access to the	*/
					/* connectList variable.	*/


/*
 ************************************************************************
 *  Static functions.
 ************************************************************************
 */

static int	connectCompare( tDhsConnect *, tDhsConnect * );
static void	init( DHS_STATUS * );
static boolean	mutexTry( tDhsMutex * );
static boolean	notified( tDhsConnect * );
static void	requestNotify( tDhsConnect *, DHS_STATUS * );
static void	sendBulk( tDhsConnect *, tDhsCmd *, ulong, ulong, ulong,
			DHS_STATUS * );
static void	sendMsg( tDhsConnect *, tDhsCmd *, ulong, ulong, ulong, 
			DHS_STATUS * );

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * connectCompare
 *
 * INVOCATION:
 * diff = connectCompare( pConnect1, pConnect2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect1 (tDhsConnect *) Pointer to the first connection.
 * (>) pConnect2 (tDhsConnect *) Pointer to the second connection.
 *
 * FUNCTION VALUE:
 * (int) > 0 if *pConnect1 > *pConnect2, 0 if *pConnect1 == *pConnect2,
 *		< 0 if *pConnect1 < *pConnect2
 *
 * PURPOSE:
 * Compare two connection by their connection id's
 *
 * DESCRIPTION:
 * Compare the two connection id's.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Both connection should have been created with the dhsConnectNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static int	connectCompare
(
    tDhsConnect	*pConnect1,	/* (in)  First connection.		*/
    tDhsConnect	*pConnect2	/* (in)  Second Connection.		*/
)
{
    return( dhsConnectId( pConnect2 ) - dhsConnectId( pConnect1 ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectAllClosed
 *
 * INVOCATION:
 * dhsConnectAllClosed( &timedOut );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pTimedOut (boolean *) Has a timeout occured.
 *
 * FUNCTION VALUE:
 * (boolean) True if all client connections are closed or a timeout 
 *		has occured.
 *
 * PURPOSE:
 * Test to see if all client connections have closed or a timeout
 * has occured
 *
 * The only thing this function is good for is as a condition function 
 * passed to the dhsEventLoop function used when waiting for clients
 * to comply with the messages sent by the dhsServerExit function.
 *
 * DESCRIPTION:
 * Check to see if there are any client connection left, or if a timeout
 * has occured.
 *
 * EXTERNAL VARIABLES:
 * - connectList: Eptr list of all connections.
 * - initialized: Indicates if the static variables have been initialized.
 * - rwLock: Ensure only one thread manipulates the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean		dhsConnectAllClosed
(
    boolean	*pTimedOut	/* (in)  Has a timeout occured.		*/
)
{
    int		i;
    tDhsConnect	*pConnect;	/* The current connection.		*/
    boolean	response;	/* The value to return.			*/
    DHS_STATUS	s;		/* Temporary function return status.	*/


    if ( *pTimedOut )
    {
	response = TRUE;
    }
    else
    {
	response = TRUE;
	s = DHS_S_SUCCESS;
	dhsRwlRLock( &rwLock, &s );
	for ( i = 0; i < eptrlen( &connectList ); i++ )
	{
	    pConnect = eptritem( &connectList, i );
	    if ( pConnect->dcClientConnect )
	    {
		response = FALSE;
		break;
	    }
	}
	dhsRwlUnlock( &rwLock, &s );
    }

    return( response );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectBreak
 *
 * INVOCATION:
 * dhsConnectBreak( pConnect, pStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pConnect (tDhsConnect *) The connection to break.
 * (!) pStatus (DHS_STATUS *) The function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Break the imp connection for a DHS connection
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The pConnect object must be locked.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConnectBreak
(
    tDhsConnect	*pConnect,	/* (mod) The connection to break.	*/
    DHS_STATUS	*pStatus	/* (mod) The connection to break.	*/
)
{
    IMP_Status	impStatus;	/* Imp function return status.		*/
    tDhsCmd	*pCmd; 		/* Pointer to a command structure.	*/
    

    fnEntry( "dhsConnectBreak" );
    statCheck( *pStatus, VOID );


    pCmd = dhsCmdNew( NULL, DHS_AV_LIST_NULL, NULL, DHS_MSG_DISCONNECT, 
	    DHS_CF_NONE, DHS_TAG_NULL, DHS_CS_DONE, pStatus );
    dhsConnectSend( pConnect, pCmd, pStatus );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	*pStatus = DHS_S_SUCCESS;
	pConnect->dcState = DHS_CON_NOT_CONNECTED;
    }
    else
    {
	pConnect->dcState = DHS_CON_DISCONNECTING;
    }
    dhsCmdDeref( pCmd, pStatus );
    connectCallback( pConnect, pStatus );


    impStatus = IMP_STATUS_OK;
    if ( pConnect->dcImpConnect != -1 )
    {
	check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), 
		*pStatus, VOID );
	impCheckNoRet( ImpCloseConnect( dhsLocal.dlImpId, 
		pConnect->dcImpConnect, &impStatus ), 
		&( dhsLocal.dlImpMutex ), impStatus );
	pConnect->dcImpConnect = -1;
	*pStatus = DHS_S_SUCCESS;
	check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
		*pStatus, VOID );
    }
    /*memset( &( pConnect->dcConnectInfo.TaskID ), 0, 
	    sizeof( pConnect->dcConnectInfo.TaskID ) );*/

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectCloseAll
 *
 * INVOCATION:
 * dhsConnectCloseAll( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Close all connections to clients
 *
 * DESCRIPTION:
 * - Copy all connections to clients to a separate list. The separate list 
 *   avoids deadlocks caused when this routine locks the rwLock mutex,
 *   and the dhsConnectDeref routine trys to lock it to delete the connection.
 * - Loop through the list of connections breaking the connections.
 *
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

void		dhsConnectCloseAll
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		i;
    EPTR	tmpList;
    tDhsConnect	*pConnect;


    fnEntry( "dhsConnectBreak" );
    statCheck( *pStatus, VOID );


    /*
     * Copy the connections to be closed to a temporary list of connections.
     */

    eptr_init( &tmpList, 5 );
    dhsRwlRLock( &rwLock, pStatus );
    for ( i = 0; i < eptrlen( &connectList ); i++ )
    {
	pConnect = eptritem( &connectList, i );
	if ( ! pConnect->dcClientConnect )
	{
	    nullCheck( eptr_insert( &tmpList, eptrlen( &tmpList ), pConnect ), 
		    *pStatus, VOID );
	}
    }
    dhsRwlUnlock( &rwLock, pStatus );


    /*
     * Close all connections in the temporary list.
     */

    for ( i = 0; i < eptrlen( &tmpList ); i++ )
    {
	pConnect = eptritem( &tmpList, i );
	dhsConnectRef( pConnect, pStatus );
	dhsConnectLock( pConnect, pStatus );
	if ( *pStatus == DHS_S_SUCCESS )
	{
	    if ( pConnect->dcOpenCount > 0 )
	    {
		dhsConnectBreak( pConnect, pStatus );
		dhsConnectUnlock( pConnect, pStatus );
		dhsConnectDeref( pConnect, pStatus );
	    }
	    else
	    {
		dhsConnectUnlock( pConnect, pStatus );
	    }

	    dhsConnectDeref( pConnect, pStatus );
	}
    }

    eptr_clear( &tmpList, NULL );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectCount
 *
 * INVOCATION:
 * int		clientConnects;
 * int		serverConnects;
 * DHS_STATUS	status;
 * dhsConnectCount( &clientConnects, &serverConnects, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (<) clientConnects (int *) The number of clients connected to this process.
 * (<) serverConnects (int *) The number of servers this process is 
 *			      connected to.
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Count the number of connections currently in existance
 *
 * A separate count is returned for connections made to other servers
 * and connections accepted by the current application. All existing
 * connections are counter, no matter what their state is.
 *
 * DESCRIPTION:
 * Loop throught the list of connections incrementing the appropriate counter
 * for each connection.
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

void		dhsConnectCount
(
    int		*pClientCon,	/* (out) The number of clients which	*/
    				/*	 have connected to this process.*/
    int		*pServerCon,	/* (out) The number of servers this	*/
    				/*	 process is connected to.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		i;
    tDhsConnect	*pConnect;	/* Pointer to the current connection.	*/


    fnEntry( "dhsConnectCount" );
    statCheck( *pStatus, VOID );

    *pClientCon = 0;
    *pServerCon = 0;
    check( dhsRwlRLock( &rwLock, pStatus ), *pStatus, VOID );
    for ( i = 0, pConnect = NULL; i < eptrlen( &connectList ); i++ )
    {
	/*
	 *  Get the current connection from the list.
	 */

	pConnect = (tDhsConnect *) eptritem( &connectList, i );


	if ( pConnect->dcClientConnect )
	{
	    (*pClientCon) ++;
	}
	else
	{
	    (*pServerCon) ++;
	}
    }
    check( dhsRwlUnlock( &rwLock, pStatus ), *pStatus, VOID );
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectDeref
 *
 * INVOCATION:
 * dhsConnectDeref( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to delete.
 * (!) pStatus (DHS_STATUS *) function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Dereference a connection and delete it if the reference count it zero
 *
 * DESCRIPTION:
 * Remove the connection from the list of connection and clean up and free
 * the connection.
 *
 * EXTERNAL VARIABLES:
 * - connectList: Eptr list of all connections.
 * - rwLock: reader writer lock to ensure only one thread manipulates the 
 *		static variables.
 *
 * PRIOR REQUIREMENTS:
 * The connection must have been created with the dhsConnectNew function.
 *
 * DEFICIENCIES:
 * Doesn't check that the static variables are initialized. This isn't
 * necessary if the function is used correctly.
 *-
 ************************************************************************
 */

void	dhsConnectDeref
(
    tDhsConnect *pConnect,	/* (in)  The connection to delete.	*/
    DHS_STATUS	*pStatus	/* (mod) The function return status.	*/
)
{
    int		index;
    DHS_STATUS	s;


    fnEntry( "dhsConnectDeref" );
    statCheck( *pStatus, VOID );

    
    s = DHS_S_SUCCESS;
    dhsRwlWLock( &rwLock, &s );
    pConnect->dcRefCount --;
    if ( pConnect->dcRefCount <= 0 )
    {
	/*
	 *  Remove the connection from the list of connections. This is done
	 *  with the mutex locked to ensure no other thread is accessing the
	 *  list.
	 */

	if ( eptr_search( &connectList, pConnect, &index, connectCompare ) )
	{
	    eptr_del( &connectList, index, 1, NULL );


	    /*
	     *  Free the memory allocated to the connection.
	     */

	    dhsMutexLock( &( pConnect->dcMutex ), pStatus );

	    if ( pConnect->dcName != NULL )
	    {
		free( pConnect->dcName );
	    }

	    if ( pConnect->dcIpAddress != NULL )
	    {
		free( pConnect->dcIpAddress );
	    }

	    free( pConnect );
	    dhsMutexDestroy( &( pConnect->dcMutex ), pStatus );

	}
    }
    dhsRwlUnlock( &rwLock, &s );


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnected
 *
 * INVOCATION:
 * answer = dhsConnected( pConnect );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to test.
 *
 * FUNCTION VALUE:
 * (boolean) True if the connection is completed or failed.
 *
 * PURPOSE:
 * Test to see if the process of making a connection is complete
 *
 * This function exists so that it can be passed to the dhsEventLoop
 * function as a condition function.
 *
 * DESCRIPTION:
 * Test the status of the connection state member to see if is in one
 * of the final states.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The connection must have been created with the dhsConnectNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean		dhsConnected
(
    tDhsConnect	*connect	/* (in)  The connection in question.	*/
)
{
    return( connect->dcState == DHS_CON_CONNECTED || 
	    connect->dcState == DHS_CON_DUPLICATE ||
	    connect->dcState == DHS_CON_ERROR || 
	    connect->dcState == DHS_CON_REFUSED ||
	    connect->dcState == DHS_CON_TIMEOUT || 
	    connect->dcState == DHS_CON_LOST || 
	    connect->dcState == DHS_CON_DISCONNECTING );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectFind
 *
 * INVOCATION:
 * pConnect = dhsConnectfind( DHS_CF_CON_ID, &status, connectId );
 * pconnect = dhsConnectFind( DHS_CF_TASK_ID, &status, pTaskId );
 * pconnect = dhsConnectFind( DHS_CF_CONNECTING, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) type (tDhsConFind) Type of the search criteria.
 * (!) pStatus (DHS_STATUS *) Function return status.
 * (>) connectId (ulong) The connection id to search for (optional).
 * (>) pTaskId (IMP_TaskID *) The task to search for (optional).
 *
 * FUNCTION VALUE:
 * (tDhsConnect *) The connection found.
 *
 * PURPOSE:
 * Search for a connection based on various search criteria
 *
 * Options are to search for connections based on IMP task id or DHS
 * connection id.
 *
 * DESCRIPTION:
 * Extract the appropriate arguments from the varargs list and perform
 * the appropriate search.
 *
 * EXTERNAL VARIABLES:
 * - connectList: Eptr list of all connections.
 * - initialized: Indicates if the static variables have been initialized.
 * - rwLock: Reader Writer lock to ensure only one thread manipulates the
 *   static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

tDhsConnect	*dhsConnectFind
(
    tDhsConFind type,		/* (in)  Type of data to look for.	*/
    DHS_STATUS	*pStatus,	/* (out) Function return status.	*/
    ...				/* (in)  Data to look for.		*/
)
{
    va_list	args;		/* The varargs list.			*/
    ulong	conId;		/* Connection id to find.		*/
    boolean	found;		/* Was the connection found?		*/
    int		i;
    int		impId;		/* IMP connetion id number.		*/
    int		index;		/* Index in the eptr list.		*/
    IMP_ConnectInfo		/* Imp connection info structure.	*/
    		*pConInfo;
    tDhsConnect	*pConnect;	/* The connection found.		*/
    boolean	sameMachine;	/* Are two machines equivalent?		*/
    IMP_TaskID	*taskId;	/* IMP task id to find.			*/


    fnEntry( "dhsConnectFind" );
    statCheck( *pStatus, NULL );


    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, NULL );
    }


    /*
     *  The type of search depends on the type parameter.
     */

    va_start( args, pStatus );
    switch ( type )
    {
	case	DHS_CF_CON_ID:
	case	DHS_CF_CON_ID2:
	    /*
	     *  Locate the connection by connection id.
	     */

	    conId = va_arg( args, int );


	    /*
	     *  Lock the mutex to ensure no other thread is manipulating
	     *  the eptr list.
	     */

	    dhsRwlRLock( &rwLock, pStatus );
	    if ( found = eptr_search( &connectList, dhsConnectPtr( conId ), 
		    &index, connectCompare ) )
	    {
		/*
		 *  The connection was found.
		 */

		pConnect = eptritem( &connectList, index );
	    }
	    dhsRwlUnlock( &rwLock, pStatus );


	    if ( ! found || ( type == DHS_CF_CON_ID && 
		    pConnect->dcState == DHS_CON_DISCONNECTING ) )
	    {
		*pStatus = DHS_E_CON_NOT_FOUND;
		pConnect = NULL;
	    }

	    break;
	case DHS_CF_CONNECTING:
	    /*
	     *  Check every connection in the list. The mutex is locked
	     *  to ensure no other thread is manipulating the list
	     *  while it is being searched.
	     */

	    check( dhsRwlRLock( &rwLock, pStatus ), *pStatus, NULL );
	    for ( i = 0, pConnect = NULL, found = FALSE; 
		    i < eptrlen( &connectList ); i++ )
	    {
		/*
		 *  Get the current connection from the list.
		 */

		pConnect = (tDhsConnect *) eptritem( &connectList, i );


		if ( pConnect->dcState == DHS_CON_LOCATED || 
			pConnect->dcState == DHS_CON_NOT_ACCEPTED ||
			pConnect->dcState == DHS_CON_NOT_CONNECTED ||
			pConnect->dcState == DHS_CON_REQUESTED )
		{
		    found = TRUE;
		    break;
		}
	    }
	    check( dhsRwlUnlock( &rwLock, pStatus ), *pStatus, NULL );


	    /*
	     *  Verify the connection was found.
	     */

	    if ( ! found )
	    {
		*pStatus = DHS_E_CON_NOT_FOUND;
		pConnect = NULL;
	    }
	    break;
	case	DHS_CF_IMP_ID:
	    /*
	     *  Find the connection by the IMP connection id.
	     */

	    impId = va_arg( args, int );

	
	    /*
	     *  Check every connection in the list. The mutex is locked
	     *  to ensure no other thread is manipulating the list
	     *  while it is being searched.
	     */

	    check( dhsRwlRLock( &rwLock, pStatus ), *pStatus, NULL );
	    for ( i = 0, pConnect = NULL, found = FALSE; 
		    i < eptrlen( &connectList ); i++ )
	    {
		/*
		 *  Get the current connection from the list.
		 */

		pConnect = (tDhsConnect *) eptritem( &connectList, i );


		/*
		 *  Extract the IMP connection information structure.
		 */

		if ( pConnect->dcImpConnect == impId )
		{
		    /*
		     *  If the IMP ids match, the connection is found.
		     */

		    found = TRUE;
		    break;
		}
	    }
	    check( dhsRwlUnlock( &rwLock, pStatus ), *pStatus, NULL );


	    /*
	     *  Verify the connection was found.
	     */

	    if ( ! found )
	    {
		*pStatus = DHS_E_CON_NOT_FOUND;
		pConnect = NULL;
	    }
	    break;
	case	DHS_CF_TASK_ID:
	case	DHS_CF_DISCONNECTING:
	    /*
	     *  Find the connection by the task id of the other side of the
	     *  connection.
	     */

	    taskId = va_arg( args, IMP_TaskID * );

	
	    /*
	     *  Check every connection in the list. The mutex is locked
	     *  to ensure no other thread is manipulating the list
	     *  while it is being searched.
	     */

	    check( dhsRwlRLock( &rwLock, pStatus ), *pStatus, NULL );
	    for ( i = 0, pConnect = NULL, found = FALSE; 
		    i < eptrlen( &connectList ); i++ )
	    {
		/*
		 *  Get the current connection from the list.
		 */

		pConnect = (tDhsConnect *) eptritem( &connectList, i );


		/*
		 *  Extract the IMP connection information structure.
		 */

		pConInfo = &( pConnect->dcConnectInfo );


		/*
		 *  Compare the task id in the conneciton information
		 *  structure with the task id passed into this function.
		 */

		sameMachine = ImpSameMachine( dhsLocal.dlImpId, 
			taskId->Machine, pConInfo->TaskID.Machine );

		if ( pConInfo != NULL && 
			sameMachine &&
			taskId->Pid == pConInfo->TaskID.Pid &&
			( ( type == DHS_CF_TASK_ID && 
			pConnect->dcState != DHS_CON_DISCONNECTING ) ||
			( type == DHS_CF_DISCONNECTING &&
			pConnect->dcState == DHS_CON_DISCONNECTING ) ) )
		{
		    /*
		     *  If the task ids match, the connection is found.
		     */

		    found = TRUE;
		    break;
		}
	    }
	    check( dhsRwlUnlock( &rwLock, pStatus ), *pStatus, NULL );


	    /*
	     *  Verify the connection was found.
	     */

	    if ( ! found )
	    {
		*pStatus = DHS_E_CON_NOT_FOUND;
		pConnect = NULL;
	    }
	    break;
	default:
	    dhsMsgFmt( DHS_E_CON_FIND, DHS_DEBUG_OFF, type );
	    *pStatus = DHS_E_CON_FIND;
	    pConnect = NULL;
    }
    va_end( args );

    fnReturn( pConnect );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsconnectInfo
 *
 * INVOCATION:
 * char		*serverAddress;
 * char		*serverName;
 * dhsConnectInfo( dhsConnect, &serverAddress, &serverName, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to query.
 * (!) pServerAddress (char **) Address of the connection.
 * (!) pServerName (char **) Name of the connection.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Get information about a connection
 *
 * DESCRIPTION:
 * - Extract the name and address of the remote task from teh IMP strcutres.
 * - Turn the address into an IP address in a readable form.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The connection must be made with the dhsConnect function.
 *
 * SEE ALSO:
 * - dhsConnect
 * - dhsConUserDataGet
 * - dhsDisconnect
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConnectInfo
(
    DHS_CONNECT	connect,	/* (in)  Connection to query.		*/
    char	**pServerAddress,/* (mod) Address of the connection.	*/
    char	**pServerName,	/* (mod) Name of the connection.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    struct in_addr
    		ipStruct;	/* Ip address structure.		*/
    tDhsConnect	*pConnect;	/* Pointer to the connection.		*/


    fnEntry( "dhsConnectInfo" );
    
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    pConnect = dhsConnectPtr( connect );

    check( dhsConnectRef( pConnect, pStatus ), *pStatus, VOID );

    /*
     *  Get the name of the task.
     */

    *pServerName = pConnect->dcConnectInfo.TaskName;


    /*
     *  Turn the address into a string of numbers.
     */

    ipStruct = inet_makeaddr( pConnect->dcConnectInfo.TaskID.Machine, 0 );
    *pServerAddress = inet_ntoa( ipStruct );

    dhsConnectDeref( pConnect, pStatus );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectLock
 *
 * INVOCATION:
 * dhsConnectLock( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to lock.
 * (!) pStatus (DHS_STATUS *) function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Find a connection in the list of connections and lock its mutex
 *
 * DESCRIPTION:
 * - Get a reader lock on the list of connections.
 * - Search the list of connetions to ensure this connection is still in
 *   the list.
 * - wait to acquire a lock on this connection. The waiting is done in
 *   the DHS event loop to avoid deadlocks.
 *
 * EXTERNAL VARIABLES:
 * - connectList: Eptr list of all connections.
 * - rwLock: Reader Writer lock to ensure only one thread manipulates the
 *   static variables.
 *
 * PRIOR REQUIREMENTS:
 * The connection must have been created with the dhsConnectNew function.
 *
 * DEFICIENCIES:
 * Doesn't check that the static variables are initialized. This isn't
 * necessary if the function is used correctly.
 *-
 ************************************************************************
 */

void	dhsConnectLock
(
    tDhsConnect *pConnect,	/* (in)  The connection to delete.	*/
    DHS_STATUS	*pStatus	/* (mod) The function return status.	*/
)
{
    int		index;
    DHS_STATUS	s;		/* Temporary function return status.	*/


    fnEntry( "dhsConnectLock" );
    statCheck( *pStatus, VOID );


    /*
     *  Locate the connection from the list of connections. This is done
     *  with the mutex locked to ensure no other thread is accessing the
     *  list.
     */

    s = DHS_S_SUCCESS;
    check( dhsRwlWLock( &rwLock, pStatus ), *pStatus, VOID );
    if ( eptr_search( &connectList, pConnect, &index, connectCompare ) )
    {
	pConnect->dcRefCount ++;
	dhsRwlUnlock( &rwLock, &s );
	dhsEventLoop( DHS_ELT_COND, mutexTry, &( pConnect->dcMutex ), 
		pStatus );
	dhsConnectDeref( pConnect, &s );
    }
    else
    {
	*pStatus = DHS_E_CON_NOT_FOUND;
	dhsRwlUnlock( &rwLock, &s );
    }

    if ( *pStatus == DHS_S_SUCCESS && s != DHS_S_SUCCESS )
    {
	*pStatus = s;
	s = DHS_S_SUCCESS;
	dhsConnectUnlock( pConnect, &s );
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectLost
 *
 * INVOCATION:
 * dhsConnectLost( &taskId, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pTaskId (IMP_TASK_ID *) Task id that was lost.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Closes all connections to a machine when a connection is lost.
 *
 * DESCRIPTION:
 * - Get a list of lost connections using the ImpLostMachine function.
 * - Loop through the list of connections deleting the associated DHS
 *   connection structures.
 *
 * EXTERNAL VARIABLES:
 * - connectList:  Eptr list of all connections.
 * - initialized:  Indicates if the static variables have been initialized.
 * - rwLock: Reader Writer lock to ensure only one thread manipulates the
 *   static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConnectLost
(
    IMP_TaskID	*pTaskId,	/* (in)  The task that was lost.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    char	*address;	/* Address of the lost server.		*/
    IMP_ConnectInfo
    		*conInfo;	/* Pointer to the imp connetion info.	*/
    int		*connectIds;	/* List of broken connection ids.	*/
    DHS_CONNECT	connectId;	/* Id of the connection being broken.	*/
    int		i;
    IMP_Status	impStatus;	/* Status returned by imp routines.	*/
    char	*name;		/* Name of the lost server.		*/
    int		numBroken;	/* Number of broken connections.	*/
    int		numConnect;	/* Number of active connections.	*/
    tDhsConnect	*pConnect;	/* Connection being processed.		*/
    char	*taskNames;	/* Required for ImpLostMachine.		*/


    fnEntry( "dhsconnectLost" );


    /*
     *  Make sure the static variables have been initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, VOID );
    }


    /*
     *  Get a list of lost machines. The list is put into an array, which
     *  can't be larger than the number of open connections.
     */

    check( dhsRwlRLock( &rwLock, pStatus ), *pStatus, VOID );
    numConnect = eptrlen( &connectList );
    check( dhsRwlUnlock( &rwLock, pStatus ), *pStatus, VOID );

    nullCheck( connectIds = int_alloc( numConnect ), *pStatus, VOID );
    nullCheck( taskNames = gen_alloc( IMP_MAX_NAME_LEN + 1 * numConnect ), 
	    *pStatus, VOID );
    impStatus = IMP_STATUS_OK;
    impCheck( ImpLostMachine( dhsLocal.dlImpId, pTaskId, numConnect, 
	    &numBroken, connectIds, taskNames, IMP_MAX_NAME_LEN + 1, 
	    &impStatus ), impStatus, *pStatus, VOID );
    gen_free( taskNames );

    
    /*
     *  Go through the list of broken connections deleting them.
     */

    for ( i = 0; i < numBroken; i++ )
    {
	/*
	 *  Try to find the connection by IMP connection id.
	 */

	pConnect = dhsConnectFind( DHS_CF_IMP_ID, pStatus, connectIds[i] );
	if ( *pStatus == DHS_E_CON_NOT_FOUND )
	{
	    /* 
	     *  The connection was not found, it must have been deleted
	     *  already.
	     */

	    continue;
	}


	/*
	 * Delete all commands associated with the connection.
	 */

	dhsCmdConnectLost( pConnect, pStatus );


	/*
	 *  execute the error callback.
	 */

	connectId = dhsConnectId( pConnect );
	dhsConnectInfo( connectId, &address, &name, pStatus );
	dhsMsgFmt( DHS_E_CON_LOST, DHS_DEBUG_OFF, name, address );
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );

	conInfo = &( pConnect->dcConnectInfo );
	dhsMsgFmt( DHS_S_DISCONNECT, DHS_DEBUG_OFF, name, 
		conInfo->TaskID.Machine, pConnect->dcImpConnect );
	dhsMsgCallback( connectId, DHS_TAG_NULL );


	/*
	 *  Delete the connection.
	 */

	dhsConnectDeref( pConnect, pStatus );

	if ( *pStatus != DHS_S_SUCCESS )
	{
	    dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	}
    }


    gen_free( connectIds );


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectMake
 *
 * INVOCATION:
 * dhsConnectMake( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pConnect (tDhsConnect *) The connection to complete.
 * (!) pStatus (DHS_STATUS * ) The function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * This takes an exising tDhsConnect object and completes the connection
 *
 * This function assumes the connection is not currently open.
 *
 * DESCRIPTION:
 * - Start the connection process by calling ImpNetLocate.
 * - Wait until the connection is completed or fails.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The pConnect object must be locked.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConnectMake
(
    tDhsConnect	*pConnect,	/* (mod) The connection to complete.	*/
    DHS_STATUS	*pStatus	/* (mod) The function return status.	*/
)
{
    IMP_Status	impStatus;	/* Imp function return status.		*/
    IMP_MsgInfo	msg_info;	/* Imp message information struture.	*/


    fnEntry( "dhsConnectMake" );
    statCheck( *pStatus, VOID );


    /*
     *  Start the connection process. All of the other imp stuff is handled
     *  as messages.
     */

    msg_info.Tag = dhsConnectId( pConnect );
    impStatus = IMP_STATUS_OK;
    /* dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus ) */
    impCheck( ImpNetLocate( dhsLocal.dlImpId, pConnect->dcIpAddress, 
	    pConnect->dcName, &msg_info, &impStatus ), 
	    impStatus, *pStatus, VOID );
    /* dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus ); */


    /*
     *  Wait for the connection to be completed.
     */

    dhsEventLoop( DHS_ELT_COND, dhsConnected, pConnect, pStatus );
    if ( pConnect->dcState != DHS_CON_CONNECTED )
    {
	pConnect->dcImpConnect = -1;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectNew
 *
 * INVOCATION:
 * pConnect = dhsConnectNew( name, pUserData, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) name (char *) Name of the task on the other end of the connection.
 * (>) ipAddress (char *) Address of the other task.
 * (>) pUserData (void *) Pointer to the user data area.
 * (>) clientConnect (boolean) Was the connection initiated by the client?
 * (>) pStatus (DHS_STATUS *) function return status.
 *
 * FUNCTION VALUE:
 * (tDhsConnect *) Pointer to the new connect structure.
 *
 * PURPOSE:
 * Create and initialize a new connect structure
 *
 * DESCRIPTION:
 * Create a new connect structure, initialize it and add it to the list
 * of connections.
 *
 * EXTERNAL VARIABLES:
 * - connectList:  Eptr list of all connections.
 * - initialized:  Indicates if the static variables have been initialized.
 * - rwLock: Reader Writer lock to ensure only one thread manipulates the
 *   static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

tDhsConnect	*dhsConnectNew
(
    const char	*name,		/* (in)  Name of the other task.	*/
    const char	*ipAddress,	/* (in)  Address of the other task.	*/
    void	*pUserData,	/* (in)  User data pointer for the 	*/
				/*       connection.			*/
    boolean	clientConnect,	/* (in)  Connection initiated by an	*/
    				/*	 external client.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    IMP_ConnectInfo		/* Imp connection information structure.*/
    		*pConInfo;
    tDhsConnect	*pConnect;	/* The new connection.			*/
    int		index;		/* Index in the list of connections.	*/
    IMP_Status	impStatus;	/* IMP function return status.		*/


    fnEntry( "dhsConnectNew" );
    statCheck( *pStatus, NULL );


    /*
     *  Make sure the static variables have been initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, NULL );
    }


    if ( dhsLocal.dlLockout && clientConnect )
    {
	*pStatus = DHS_E_CON_LOCKOUT;
	dhsMsgFmt( DHS_E_CON_LOCKOUT, DHS_DEBUG_OFF, name, ipAddress );
	fnReturn( NULL );
    }


    /*
     *  Alocate a new connection structure.
     */

    nullCheck( pConnect = 
	    (tDhsConnect *) malloc( sizeof( tDhsConnect ) ), *pStatus, NULL );


    /*
     *  Initialize the members of the connection structure.
     */

    pConnect->dcClientConnect = clientConnect;
    pConnect->dcImpConnect = -1;
    pConnect->dcTimeout = 0;
    if ( ipAddress != NULL )
    {
	pConnect->dcIpAddress = strsav( ipAddress );
    }
    else
    {
	pConnect->dcIpAddress = NULL;
    }
    dhsMutexInit( &( pConnect->dcMutex ), pStatus );
    impStatus = IMP_STATUS_OK;
    impCheck( ImpTimeNow( &( pConnect->dcLastUsed), &impStatus ), 
	    impStatus, *pStatus, NULL );
    if ( name != NULL )
    {
	pConnect->dcName = strsav( name );
    }
    else
    {
	pConnect->dcName = NULL;
    }
    pConnect->dcState = DHS_CON_NOT_CONNECTED;
    pConnect->dcUserData = pUserData;
    (void) strcpy( pConnect->dcConnectInfo.TaskName, name );
    pConnect->dcOpenCount = 0;
    pConnect->dcRefCount = 1;
    pConInfo = &( pConnect->dcConnectInfo );
    pConInfo->MessageBytes = MAX_MESSAGE_LENGTH;
    pConInfo->MaxMessages = MAX_MESSAGES;
    pConInfo->ReplyBytes = MAX_MESSAGE_LENGTH;
    pConInfo->MaxReplies = MAX_MESSAGES;
    pConInfo->TaskID.Machine = 0;
    pConInfo->TaskID.Pid = 0;


    /*
     *  Add the connection to the list of connections. This is done with
     *  the mutex locked so no other thread can be accessing the list.
     */

    check( dhsRwlWLock( &rwLock, pStatus ), *pStatus, NULL );
    eptr_search( &connectList, pConnect, &index, connectCompare );
    nullCheck( eptr_insert( &connectList, index, pConnect ), 
	    *pStatus, NULL );
    dhsRwlUnlock( &rwLock, pStatus );

    fnReturn( pConnect );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectRef
 *
 * INVOCATION:
 * dhsConnectRef( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) Pointer to the connection.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Increment the reference counter for a connection
 *
 * DESCRIPTION:
 * Trivial
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

void		dhsConnectRef
(
    tDhsConnect	*pConnect,	/* (in)  The connection.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		index;
    DHS_STATUS	s;


    fnEntry( "dhsConnectRef" );
    statCheck( *pStatus, VOID );


    s = DHS_S_SUCCESS;
    dhsRwlWLock( &rwLock, &s );
    if ( eptr_search( &connectList, pConnect, &index, connectCompare ) )
    {
	pConnect->dcRefCount ++;
    }
    else
    {
	*pStatus = DHS_E_CON_NOT_FOUND;
    }
    dhsRwlUnlock( &rwLock, &s );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectSend
 *
 * INVOCATION:
 * dhsConnectSend( pConnect, pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) Connection to send the message to.
 * (>) pCmd (tDhsCmd *) Command structure describing the message.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send a message to the specified connection
 *
 * DESCRIPTION:
 * Send the message over a connection using a function that is dependant
 * on the message size.
 *
 * EXTERNAL VARIABLES:
 * Any external variables used by this function.
 *
 * PRIOR REQUIREMENTS:
 * - The connection must be opened with the dhsConnect function.
 * - The command structure must have been created with the dhsCmdNew function.
 * - The pConnect object must be locked.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConnectSend
(
    tDhsConnect *pConnect,	/* (in)  Connection to send to.		*/
    tDhsCmd	*pCmd,		/* (in)  The command to send.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    ulong	avListLength;	/* Length of the av list.		*/
    ulong	bulkLength;	/* Length of the bulk data segment.	*/
    ulong	headerLength;	/* Length of the message header.	*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    StatusType	sdsStatus;	/* Sds function return status.		*/


    fnEntry( "dhsconnectSend" );
    statCheck( *pStatus, VOID );


    sdsStatus = SDS__OK;


    if ( pConnect == NULL )
    {
	*pStatus = DHS_E_CON_INVALID;
	dhsMsgFmt( DHS_E_CON_INVALID, DHS_DEBUG_OFF, dhsConnectId( pConnect ) );
	fnReturn( VOID );
    }


    /*
     *  Get the size of the av list.
     */

    if ( pCmd->dcAvList == DHS_AV_LIST_NULL )
    {
	avListLength = 0;
    }
    else
    {
	sdsCheck( SdsSize( dhsSdsId( pCmd->dcAvList ), &avListLength, 
		&sdsStatus ), sdsStatus, *pStatus, VOID );
    }


    /*
     *  Get the size of the bulk data area.
     */

    switch( pCmd->dcBulkType )
    {
	case	DHS_BT_BUFFER:
	    /* 
	     * Bulk data is a raw buffer so the length is easy.
	     */

	    bulkLength = pCmd->dcBulkLen;
	    break;
	case	DHS_BT_SDS:
	    /* 
	     * Bulk data is an sds structure, so use SdsSize to get
	     * its size.
	     */

	    if ( pCmd->dcDataset == DHS_BD_DATASET_NULL )
	    {
		bulkLength = 0;
	    }
	    else
	    {
		sdsCheck( SdsSize( dhsSdsId( pCmd->dcDataset ), &bulkLength,
			&sdsStatus ), sdsStatus, *pStatus, VOID );
	    }
	    break;
	default:
	    bulkLength = 0;
    }


    /*
     *  header length is the size required by the size string plus the
     *  length of the name.
     */

    headerLength = 36;
    if ( pCmd->dcName != NULL )
    {
	 headerLength += strlen( pCmd->dcName );
    }
    headerLength += sizeof( long ) - ( headerLength % sizeof( long ) );


    /*
     *  Calculate the total message size.
     */

    pCmd->dcMsgInfo.MessageLength = avListLength + headerLength + bulkLength;


    pCmd->dcSentTo = pConnect;


    /*
     *  If the total message size exceeds the threshold, use the imp bulk
     *  routines, otherwise use normal IMP message passing routines.
     */

    if ( pCmd->dcSentTo->dcState != DHS_CON_CONNECTED )
    {
	s = DHS_S_SUCCESS;
	*pStatus = DHS_E_CON_LOST;
	dhsMsgFmt( DHS_E_CON_LOST, DHS_DEBUG_OFF, pConnect->dcName, 
		pConnect->dcIpAddress );
    }

    if ( *pStatus == DHS_S_SUCCESS )
    {
	if ( pCmd->dcMsgInfo.MessageLength > MAX_MESSAGE_LENGTH )
	{
	    sendBulk( pConnect, pCmd, headerLength, avListLength, bulkLength,
		    pStatus );
	}
	else
	{
	    sendMsg( pConnect, pCmd, headerLength, avListLength, bulkLength,
		    pStatus );
	}

	s = DHS_S_SUCCESS;
	if ( *pStatus == DHS_S_SUCCESS )
	{
	    *pStatus = s;
	}
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectUnlock
 *
 * INVOCATION:
 * dhsConnectUnlock( &connection, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to unlock.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Unlock the mutex associated with a connection
 *
 * DESCRIPTION:
 * Unlock the mutex.
 * Broadcast to the event loop condition variable.
 * Send a null message to the event loop.
 *
 * The broadcast and the null message cause any threads in the 
 * dhsConnectLock function to stop waiting and attempt to acquire the
 * mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The connection must be created with dhsConnectNew.
 * - The connection must have been locked with the dhsConnectLock function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConnectUnlock
(
    tDhsConnect	*pConnect,	/* (in) The connection to unlock.	*/
    DHS_STATUS	*pStatus	/* (in) Function return status.		*/
)
{
    fnEntry( "dhsConnectUnlock" );
    statCheck( *pStatus, VOID );

    dhsMutexUnlock( &( pConnect->dcMutex ), pStatus );

    dhsElBroadcast( pStatus );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsDisconnected
 *
 * INVOCATION:
 * answer = dhsDisconnected( pConnect );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to test.
 *
 * FUNCTION VALUE:
 * (boolean) True if the connection has been disconnected.
 *
 * PURPOSE:
 * Test to see if the process of closing a connection is complete
 *
 * This function exists so that it can be passed to the dhsEventLoop
 * function as a condition function.
 *
 * DESCRIPTION:
 * Test the status of the connection state member to see if is 
 * DHS_CS_NOT_CONNECTED.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The connection must have been created with the dhsConnectNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean		dhsDisconnected
(
    tDhsConnect	*connect	/* (in)  The connection in question.	*/
)
{
    DHS_STATUS		s;

    s = DHS_S_SUCCESS;
    dhsConnectFind( DHS_CF_CON_ID2, &s, dhsConnectId( connect ) );
    return( s != DHS_S_SUCCESS || 
	    connect->dcState == DHS_CON_NOT_CONNECTED ||
	    connect->dcState == DHS_CON_LOST );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsServerExit
 *
 * INVOCATION:
 * dhsServerExit( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send a message to all client connections requesting that they
 * close the connection
 *
 * DESCRIPTION:
 * Loop through the connection list, sending message to each client
 * connection.
 *
 * EXTERNAL VARIABLES:
 * - connectList: Eptr list of all connections.
 * - initialized: Indicates if the static variables have been initialized.
 * - rwLock: Reader Writer lock to ensure only one thread manipulates the
 *   static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsServerExit
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		i;
    tDhsCmd	*pCmd;		/* Pointer to a command.		*/
    tDhsConnect	*pConnect;	/* Pointer to the current connection.	*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    DHS_STATUS	s1;		/* Temporary function return status.	*/


    fnEntry( "dhsServerExit" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );
    


    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, VOID );
    }


    dhsLocal.dlLockout = TRUE;
    for (;;)
    {
	s = DHS_S_SUCCESS;
	s1 = DHS_S_SUCCESS;
	dhsRwlRLock( &rwLock, &s1 );
	for ( i = 0, pConnect = NULL; 
		s1 == DHS_S_SUCCESS && i < eptrlen( &connectList ); 
		i++ )
	{
	    pConnect = eptritem( &connectList, i );
	    if ( pConnect->dcClientConnect && 
		    pConnect->dcState == DHS_CON_CONNECTED )
	    {
		break;
	    }
	    else
	    {
		pConnect = NULL;
	    }
	}
	dhsRwlUnlock( &rwLock, &s );

	s = DHS_S_SUCCESS;


	if ( pConnect == NULL )
	{
	    break;
	}
	else
	{
	    pCmd = dhsCmdNew( NULL, DHS_AV_LIST_NULL, NULL, 
		    DHS_MSG_SERVER_EXIT, DHS_CF_NONE, DHS_TAG_NULL, 
		    DHS_CS_DONE, &s1 );
	    s = DHS_S_SUCCESS;
	    dhsConnectLock( pConnect, &s );
	    dhsConnectRef( pConnect, &s1 );
	    dhsConnectSend( pConnect, pCmd, &s1 );
	    if ( s1 != DHS_S_SUCCESS )
	    {
		s1 = DHS_S_SUCCESS;
		dhsMessageClear( &s1 );
		pConnect->dcState = DHS_CON_LOST;
		dhsConnectUnlock( pConnect, &s );
		dhsConnectDeref( pConnect, &s1 );
		connectCallback( pConnect, pStatus );
	    }
	    else
	    {
		dhsConnectUnlock( pConnect, &s );
	    }
	    dhsCmdDeref( pCmd, &s1 );
	    dhsEventLoop( DHS_ELT_COND, dhsDisconnected, pConnect, &s1 );
	    dhsConnectDeref( pConnect, &s1 );
	}

	if ( s1 != DHS_S_SUCCESS )
	{
	    *pStatus = s1;
	}
    }


    fnReturn( VOID );
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
 * Initialize the eptr list and mutex.
 *
 * EXTERNAL VARIABLES:
 * - connectList: Eptr list of all connections.
 * - initialized: Indicates if the static variables have been initialized.
 * - rwLock: Reader Writer lock to ensure only one thread manipulates the
 *   static variables.
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
    DHS_STATUS	*pStatus	/* (out) function return status.	*/
)
{
    fnEntry( "init" );


    /*
     *  Initialize the eptr list.
     */

    nullCheck( eptr_init( &connectList, 5 ), *pStatus, VOID );


    /* 
     *  Initialize the mutex.
     */

    dhsRwlInit( &rwLock, pStatus );


    initialized = TRUE;

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * mutexTry
 *
 * INVOCATION:
 * isLocked = mutexTry( &dhsMutex );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pMutex (tDhsMutex *) The mutex to try to lock.
 *
 * FUNCTION VALUE:
 * (boolean) True if the mutex was locked, false otherwise.
 *
 * PURPOSE:
 * Mutex try function callable by the DHS event loop as a condition function
 *
 * DESCRIPTION:
 * Execute DhsMutexTry.
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

boolean		mutexTry
(
    tDhsMutex	*pMutex		/* (in)  The mutex to lock.		*/
)
{
    DHS_STATUS	status;

    status = DHS_S_SUCCESS;
    return( dhsMutexTry( pMutex, &status ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * notified
 *
 * INVOCATION:
 * isNotified = notified( pConnect );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to check.
 *
 * FUNCTION VALUE:
 * (boolean) True if a notify message has been received.
 *
 * PURPOSE:
 * Check to see if a notify message has been received
 *
 * This function only exists so it can be passed to the dhsEventLoop
 * function as a condition testing function.
 *
 * DESCRIPTION:
 * Check the notified member of the structure and return true when it
 * indicates a notify message has been received.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The connection must be opened with the dhsConnect funcion.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static boolean	notified
(
    tDhsConnect *pConnect	/* (in) Connection to check.		*/
)
{
    return( pConnect->dcNotified );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * requestNotify
 *
 * INVOCATION:
 * requestNotify( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to be notified.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Request IMP send a notify message when the buffers for the connection
 * are empty
 *
 * DESCRIPTION:
 * Use the ImpRequestNotify function to request notification when the
 * buffers are empty.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpId: The IMP id.
 * - dhsLocal.dlImpMutex: The IMP mutual exclusion lock.
 *
 * PRIOR REQUIREMENTS:
 * The connection must be opened with dhsConnect.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	requestNotify
(
    tDhsConnect *pConnect,	/* (in)  The connection.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    IMP_Status	impStatus;	/* Imp function return status.		*/

    fnEntry( "requestNotify" );


    pConnect->dcNotified = FALSE;
    impStatus = IMP_STATUS_OK;


    /*
     *  Request the notify message.
     */

    impCheck( ImpRequestNotify( dhsLocal.dlImpId, pConnect->dcImpConnect, 
	    &impStatus ), impStatus, *pStatus, VOID );


    /*
     *  Wait until the notify is received.
     */

    dhsEventLoop( DHS_ELT_COND, notified, pConnect, pStatus );
    
    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sendBulk
 *
 * INVOCATION:
 * sendBulk( pConnect, pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) Pointer to the connection.
 * (>) pCmd (tDhsCmd *) Pointer to the command.
 * (>) headerLength (ulong) Length of the header part.
 * (>) avListLength (ulong) Length of exported av list.
 * (>) bulkLength (ulong) Length of the bulk part.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send a message using the IMP bulk data functions
 *
 * DESCRIPTION:
 * - Create a new shared memory area for the message.
 * - Format the message into the new buffer.
 * - Use the ImpSendBulk command to send the buffer.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpId: The IMP id.
 * - dhsLocal.dlImpMutex: The IMP mutual exclusion lock.
 *
 * PRIOR REQUIREMENTS:
 * Operations that must be performed before calling this function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	sendBulk
(
    tDhsConnect	*pConnect,	/* (in)  The connection to use.		*/
    tDhsCmd	*pCmd,		/* (in)  The command to send.		*/
    ulong	headerLength,	/* (in)  length of the header part.	*/
    ulong	avListLength,	/* (in)  Length of exported av list.	*/
    ulong	bulkLength,	/* (in)  Length of the bulk part.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    byte	*dataPtr;	/* Pointer to shared memory area.	*/
    IMP_Status	impStatus;	/* IMP Function return status.		*/


    fnEntry( "sendBulk" );
    statCheck( *pStatus, VOID );




    impStatus = IMP_STATUS_OK;


    if ( pCmd->dcRetries == 0 )
    {
	/*
	 *  Create a new shared memory area for the bulk data.  The area 
	 *  allocated is slightly larger than the message size to allow 
	 *  the command type to be copied to the first few bytes.
	 */

#ifdef vxWorks
	impCheck (ImpDefineShared( IMP_SHARE_GLOBAL, NULL, 0,
		pCmd->dcMsgInfo.MessageLength + 4, TRUE,
		(void **) &dataPtr, &(pCmd->dcSharedMem), &impStatus ),
		impStatus, *pStatus, VOID );
#else	/* !vxWorks */
	char	*fname;		/* MMAP file name.			*/
	char	baseName[PATH_MAX];

	fname = tempnam( "/tmp", "dhsImp-" );
	sprintf( baseName, "%s-sendBulk-%s", fname, dhsLocal.dlImpName );
	impCheck( ImpDefineShared( IMP_SHARE_MMAP, baseName, 0, 
		pCmd->dcMsgInfo.MessageLength + 4, TRUE, 
		(void **) &dataPtr, &(pCmd->dcSharedMem), &impStatus ), 
		impStatus, *pStatus, VOID );
	free( fname );
#endif	/* vxWorks */


	/*
	 *  Indicate the commands shared memory area is in use.
	 */

	pCmd->dcSharedMemUsed = TRUE;


	/*
	 *  Increment the reference count in the command structure. This 
	 *  prevents the command structure from being deleted while the
	 *  shared memory structure is in use.
	 */

	dhsCmdRef( pCmd, pStatus );


	/*
	 *  Put the commands type into the first few bytes of the buffer.
	 */

	*dataPtr = pCmd->dcType;


	/*
	 *  Format the message into the buffer following the type.
	 */

	dhsCmdMsgFmt( pCmd, dataPtr + 4, headerLength, avListLength, 
		bulkLength, pStatus );
    }


    /*
     *  Execute the sendBulk command.  The tag of the command structure,
     *  is put into the refNumber field, and the assigned tag is put
     *  into the tag field.
     */

    pCmd->dcMsgInfo.Tag = pCmd->dcTag;
    pCmd->dcMsgInfo.RefNumber = dhsCmdTag( pCmd );
    pCmd->dcMsgInfo.NotifyBytes = 0;

    /*
     *  Lock the connection mutex to ensure no other thread attempts to
     *  use the connection.
     */

    pCmd->dcRetries++;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpSendBulk( dhsLocal.dlImpId, pConnect->dcImpConnect, 
	    &(pCmd->dcSharedMem), &( pCmd->dcMsgInfo ), 0, &impStatus ), 
	    &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );



    if ( impStatus == IMP_STATUS_OK )
    {
	dhsMsgFmt( DHS_S_CMD_BULK_SENT, DHS_DEBUG_ON, dhsCmdTag( pCmd ), 
		pCmd->dcType );
    }
    else
    {
	/*
	 *  If the send failed, format a message and release the shared memory.
	 */

	pCmd->dcRetries--;
	*pStatus = DHS_E_IMP;
	impStatus = IMP_STATUS_OK;
	impCheck( ImpReleaseShared( &(pCmd->dcSharedMem), TRUE, &impStatus ), 
		impStatus, *pStatus, VOID );
	pCmd->dcSharedMemUsed = FALSE;
	dhsCmdDeref( pCmd, pStatus );
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sendMsg
 *
 * INVOCATION:
 * sendMsg( pConnect, pCmd, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) Pointer to the connection.
 * (>) pCmd (tDhsCmd *) Pointer to the command.
 * (>) headerLength (ulong) Length of the header part.
 * (>) avListLength (ulong) Length of exported av list.
 * (>) bulkLength (ulong) Length of the bulk part.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send a message as a standard IMP message
 *
 * DESCRIPTION:
 * - The the ImpSendPtr function to get a buffer fo rthe mesasge.
 * - If there isn't enough space for the message wait until there is.
 * - When there is enough space for the message, format the message into
 *   the buffer.
 * - Use ImpSendEnd to send the message.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpId: The IMP id.
 * - dhsLocal.dlImpMutex: The IMP mutual exclusion lock.
 *
 * PRIOR REQUIREMENTS:
 * Operations that must be performed before calling this function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	sendMsg
(
    tDhsConnect	*pConnect,	/* (in)  The connection to use.		*/
    tDhsCmd	*pCmd,		/* (in)  The command to send.		*/
    ulong	headerLength,	/* (in)  length of the header part.	*/
    ulong	avListLength,	/* (in)  Length of exported av list.	*/
    ulong	bulkLength,	/* (in)  Length of the bulk part.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    void	*dataPtr;	/* Pointer to the data buffer.		*/
    long	flags;		/* Flags to use for sending the message.*/
    IMP_Status	impStatus;	/* IMP function return status.		*/
    DHS_STATUS	s;		/* Temporary return status.		*/


    fnEntry( "sendMsg" );
    statCheck( *pStatus, VOID );


    flags = 0;
    if ( ( pCmd->dcFlags & DHS_CF_PRIORITY ) == DHS_CF_PRIORITY )
    {
	flags |= IMP_PRIORITY;
    }


    /*
     *  Try to get space for the message.
     */

    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    for ( ;; )
    {
	/*
	 *  Set up the imp MsgInfo structure in the command.
	 */

	pCmd->dcMsgInfo.Tag = pCmd->dcTag;
	pCmd->dcMsgInfo.Type = pCmd->dcType;

	pConnect->dcNotified = FALSE;
	impStatus = IMP_STATUS_OK;

	impCheckNoRet( ImpSendPtr( dhsLocal.dlImpId, pConnect->dcImpConnect, 
		&( pCmd->dcMsgInfo ), flags, &impStatus ), 
		&( dhsLocal.dlImpMutex ), impStatus );
	if ( impStatus == IMP__NO_SPACE )
	{
		printf( "no-space imp error\n" );
	    /*
	     *  If no space is available, request a notify message when
	     *  the buffers become empty.
	     */

	    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
		    *pStatus, VOID );
	    requestNotify( pConnect, pStatus );
	    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), 
		    *pStatus, VOID );
	    if ( *pStatus == DHS_S_SUCCESS )
	    {
		continue;
	    }
	    else
	    {
		break;
	    }
	}
	else if ( impStatus == IMP__NEED_SYNCH )
	{
		printf( "need-sync imp error\n" );
	    /*
	     *  Wait until the notify is received.
	     */


	    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
		    *pStatus, VOID );
	    dhsEventLoop( DHS_ELT_COND, notified, pConnect, pStatus );
	    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), 
		    *pStatus, VOID );
	}
	else if ( impStatus != IMP_STATUS_OK )
	{
	    /*
	     *  An error occured.
	     */

	    *pStatus = DHS_E_IMP;	
	    break;
	}
	else
	{
	    /*
	     * A message buffer was allocated.
	     */

	    dataPtr = pCmd->dcMsgInfo.Address;
	    break;
	}
    }


    if ( *pStatus == DHS_S_SUCCESS )
    {
	/*
	 *  Format the message into the buffer.
	 */

	dhsCmdMsgFmt( pCmd, dataPtr, headerLength, avListLength, bulkLength, 
		pStatus );


	/*
	 *  Send the message.
	 */

	pCmd->dcRetries ++;
	pCmd->dcMsgInfo.Tag = pCmd->dcTag;
	pCmd->dcMsgInfo.Type = pCmd->dcType;
	impCheckNoRet( ImpSendEnd( dhsLocal.dlImpId, &( pCmd->dcMsgInfo ), 
		IMP_TRUE, &impStatus ), &( dhsLocal.dlImpMutex ), impStatus );
	if ( impStatus == IMP_STATUS_OK )
	{
	    dhsMsgFmt( DHS_S_CMD_MSG_SENT, DHS_DEBUG_ON, dhsCmdTag( pCmd ), 
		    pCmd->dcType );
	}
	else
	{
	    pCmd->dcRetries --;
	    *pStatus = DHS_E_IMP;	
	}
    }


    /*
     *  A separate status value is used to unlock the mutex because it
     *  should be unlocked even if errors ocurred above.
     */

    s = DHS_S_SUCCESS;
    dhsMutexUnlock( &( dhsLocal.dlImpMutex ), &s );
    if ( s != DHS_S_SUCCESS && *pStatus == DHS_S_SUCCESS )
    {
	*pStatus = s;
    }

    fnReturn( VOID );
}
