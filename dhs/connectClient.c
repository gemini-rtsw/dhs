static char rcsid[] = "$Id: connectClient.c,v 1.1.1.1 2002-11-24 20:20:12 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1998				(c) 1998
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
 * dhs/src/connectClient.c
 *
 * PURPOSE:
 * Contains the connection routines that would only be used by a task
 * behaving as a client.
 *
 * FUNCTION NAME(S)
 * dhsConnect - Create a new connection to a server.
 * dhsConUserDataGet - Get the user data for a connection.
 * dhsConUserDataSet - Set the user data for a connection.
 * dhsDisconnect - Break a connection to a server.
 * dhsIsConnected - Return true if a connection is connected.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:57  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.3  1999/01/27 18:50:02  nhill
 * Bug fix.
 *
 * Revision 1.2  1999/01/20 22:52:42  nhill
 * Changed dhsConUserDataSet to check for the existance of a connection.
 *
 * Revision 1.1  1998/08/04 16:16:11  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include "localDhs.h"


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnect
 *
 * INVOCATION:
 * connect = dhsConnect( "salish.dao.nrc.ca", "serverName", pUserData,
 *			&status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) ipAddress (const char *) Address of the server.
 * (>) name (const char *) Name of the server.
 * (>) userData (void *) User data pointer for the connection.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_CONNECT) New connection id.
 *
 * PURPOSE:
 * Create and return a new connection to the specified server
 *
 * DESCRIPTION:
 * Create an new connection structure, start the process of making an
 * IMP connection, and then wait for the process to either complete
 * or fail.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpId - IMP task id.
 *
 * PRIOR REQUIREMENTS:
 * The dhs library must have been initialized with the dhsInit function.
 * The server being connected to must be running.
 * If the server is not on the current machine, the IMP master task must
 * be running on both the server machine and the current machine.
 *
 * SEE ALSO:
 * - dhsDisconnect
 * - dhsConnectInfo
 * - dhsConUserData
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_CONNECT	dhsConnect
(
    const char	*ipAddress,	/* (in)  Address of the server.		*/
    const char	*name,		/* (in)  Name of the server.		*/
    void	*userData,	/* (in)  Connections user data pointer.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsConnect	*pConnection;	/* The new connection.			*/
    tDhsConnect	*pConnect2;	/* A second connection pointer.		*/
    DHS_STATUS	s;		/* Temporary function return status.	*/


    fnEntry( "dhsConnect" );

    statCheck( *pStatus, DHS_CONNECT_NULL );
    isInit( *pStatus, DHS_CONNECT_NULL );


    /*
     * Create a new connection.
     */

    check( pConnection = dhsConnectNew( name, ipAddress, userData, FALSE, 
	    pStatus ), *pStatus, DHS_CONNECT_NULL );


    dhsConnectLock( pConnection, pStatus );
    pConnection->dcOpenCount ++;
    dhsConnectMake( pConnection, pStatus );

    if ( *pStatus != DHS_S_SUCCESS )
    {
	s = DHS_S_SUCCESS;
	if ( *pStatus != DHS_E_CON_NOT_FOUND )
	{
	    dhsConnectUnlock( pConnection, &s );
	    check( dhsConnectDeref( pConnection, &s ), s, DHS_CONNECT_NULL );
	}
	fnReturn( DHS_CONNECT_NULL );
    }


    /*
     *  Check to see if the connection succeeded.
     */

    s = DHS_S_SUCCESS;
    if ( pConnection->dcState == DHS_CON_DUPLICATE )
    {
	pConnect2 = pConnection->dcDupConnect;
	dhsConnectUnlock( pConnection, &s );
	check( dhsConnectDeref( pConnection, pStatus ), 
		*pStatus, DHS_CONNECT_NULL );
	pConnection = pConnect2;
	dhsConnectLock( pConnection, &s );
    }
    else if ( pConnection->dcState != DHS_CON_CONNECTED )
    {
	dhsConnectUnlock( pConnection, &s );
	check( dhsConnectDeref( pConnection, pStatus ), 
		*pStatus, DHS_CONNECT_NULL );
	dhsMsgFmt( DHS_E_CONNECT, DHS_DEBUG_OFF, name, ipAddress );
	*pStatus = DHS_E_CONNECT;
	fnReturn( DHS_CONNECT_NULL );
    }
    s = DHS_S_SUCCESS;
    dhsConnectUnlock( pConnection, &s );

    fnReturn( dhsConnectId( pConnection ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConUserDataGet
 *
 * INVOCATION:
 * pUserData = dhsConUserDataGet( connect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) The connection to query.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (void *) The user data pointer.
 *
 * PURPOSE:
 * Return the user data pointer assigned to a connection
 *
 * DESCRIPTION:
 * Get the user data pointer from the connection structure and return it.
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
 * - dhsConnectInfo
 * - dhsDisconnect
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		*dhsConUserDataGet
(
    DHS_CONNECT	connect,	/* (in)  connection to query.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    void	*ptr;


    fnEntry( "dhsConUserDataGet" );

    statCheck( *pStatus, NULL );
    isInit( *pStatus, NULL );

    ptr = dhsConnectPtr( connect )->dcUserData;
    fnReturn( ptr );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConUserDataSet
 *
 * INVOCATION:
 * dhsConUserDataSet( connect, pUserData, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) The connection to query.
 * (>) pUserData (void *) The new user data pointer.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Set the user data pointer assigned to a connection
 *
 * This function can be used to change the user data pointer, the 
 * prefered method for setting the user data pointer is with the 
 * dhsConnect function to avoid the gap between a call to dhsConnect 
 * and a call to this function. If this functon is used, mutexes 
 * will probably be required to prevent attempts to use the user 
 * data pointer before it is set.
 *
 * DESCRIPTION:
 * Set the user data pointer for the connection structure.
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
 * - dhsConnectInfo
 * - dhsDisconnect
 * - dhsConUserDataGet
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsConUserDataSet
(
    DHS_CONNECT	connect,	/* (in)  connection to query.		*/
    void	*pUserData,	/* (in)  The new user data pointer.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "dhsConUserDataSet" );

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );

    dhsConnectRef( dhsConnectPtr( connect ), pStatus );
    if ( *pStatus == DHS_S_SUCCESS )
    {
	dhsConnectPtr( connect )->dcUserData = pUserData;
    }
    check( dhsConnectDeref( dhsConnectPtr( connect ), pStatus ), 
	    *pStatus, VOID );
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsDisconnect
 *
 * INVOCATION:
 * dhsDisconnect( connect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to break.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Close a connection created with function dhsConnect
 *
 * This function can also be used by a server to refuse a connection
 * requested by a client. In this case, dhsDisconnect must be called
 * in the error callback when it is called indicating a client requests
 * a connection.
 *
 * DESCRIPTION:
 * Find the connection structure, if this is a connection from a client
 * and if it is not yet complete, mark the connection as refused.  If
 * the connection is a clients connection to a server, close the IMP
 * connection, delete any commands on the connection and then delete
 * the connection structure.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpId The IMP task id.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The connection must have been opened with the dhsConnect function, or
 * the function must be called from a servers error callback function
 * when it is notified a client is requesting a connection.
 *
 * SEE ALSO:
 * - dhsConnect
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsDisconnect
(
    DHS_CONNECT	connection,	/* (in)  Connection to break.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return state.		*/
)
{
    tDhsConnect *pConnect;	/* Pointer to connection structure.	*/


    fnEntry( "dhsDisconnect" );

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Find the connection structure.
     */

    pConnect = dhsConnectPtr( connection );
    dhsConnectLock( pConnect, pStatus );
    if ( *pStatus == DHS_E_CON_NOT_FOUND )
    {
	dhsMsgFmt( DHS_E_CON_NOT_FOUND, DHS_DEBUG_OFF );
	fnReturn( VOID );
    }
    else if ( *pStatus != DHS_S_SUCCESS )
    {
	fnReturn( VOID );
    }

    if ( pConnect->dcState == DHS_CON_REQUESTED )
    {
	/*
	 *  Connection is from a client, mark it as refused and return.
	 */

	pConnect->dcState = DHS_CON_REFUSED;
	dhsConnectUnlock( pConnect, pStatus );
    }
    else if ( ! pConnect->dcClientConnect )
    {
	pConnect->dcOpenCount --;
	if ( pConnect->dcOpenCount == 0 )
	{
	    dhsConnectBreak( pConnect, pStatus );
	    dhsConnectUnlock( pConnect, pStatus );


	    /*
	     *  This destroys any commands associated with the connection.
	     */

	    check( dhsCmdConnectLost( pConnect, pStatus ), *pStatus, VOID );
	    *pStatus = DHS_S_SUCCESS;


	    /*
	     *  Wait for the connection to be closed.
	     */

	    dhsEventLoop( DHS_ELT_COND, dhsDisconnected, pConnect, pStatus );
	    *pStatus = DHS_S_SUCCESS;


	    /*
	     *  Delete the connection structure.
	     */

	    check( dhsConnectDeref( pConnect, pStatus ), *pStatus, VOID );
	    *pStatus = DHS_S_SUCCESS;
	}
	else
	{
	    dhsConnectUnlock( pConnect, pStatus );
	}

	while ( *pStatus == DHS_S_SUCCESS )
	{
	    dhsMessageClear( pStatus );
	}

	*pStatus = DHS_S_SUCCESS;
    }
    else
    {
	dhsConnectUnlock( pConnect, pStatus );
    }

    dhsElBroadcast( pStatus );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsIsConnected
 *
 * INVOCATION:
 * result = dhsIsConnected( connection );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) The connection to check.
 * (!) pStatus (DHS_STATUS &) The function return status.
 *
 * FUNCTION VALUE:
 * (DHS_BOOLEAN) True if the connection is open, false otherwise.
 *
 * PURPOSE:
 * Check to see if the connection is open.
 *
 * DESCRIPTION:
 * Trivial.
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

DHS_BOOLEAN	dhsIsConnected
(
    DHS_CONNECT	connect,	/* (in)  The connection to check.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return stauts.	*/
)
{
    tDhsConnect	*pConnect;
    DHS_STATUS	status;

    fnEntry( "dhsIsConnected" );

    statCheck( *pStatus, DHS_FALSE );
    isInit( *pStatus, DHS_FALSE );

    status = DHS_S_SUCCESS;

    pConnect = dhsConnectFind( DHS_CF_CON_ID, &status, connect );
    if ( status == DHS_S_SUCCESS && pConnect->dcState == DHS_CON_CONNECTED )
    {
	fnReturn( DHS_TRUE );
    }
    else
    {
	fnReturn( DHS_FALSE );
    }
}
