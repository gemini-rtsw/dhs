static char rcsid[] = "$Id: con.C,v 1.1.1.1 2002-11-24 20:26:40 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsData/src/con.C
//
// PURPOSE:
// To handle connections and disconnections.
//
// METHOD NAME(S)
// cDtsConManager::~cDtsConManager()
// cDtsConManager::authorized()
// cDtsConManager::config()
// cDtsConManager::connectors()
// cDtsConManager::connectTo()
// cDtsConManager::close()
// cDtsConManager::disconnectTo()
// cDtsConManager::testAll()
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  2001/03/27 21:09:53  nhill
// Changed to be compatable with workshop 5.0.
//
// Revision 1.5  1998/05/04 22:58:06  dunn
// Changes for status class changes, and change to checkDhs
//
// Revision 1.4  1997/10/06 20:00:52  dunn
// Got rid of unlock and lock around the open connection in connectTo
//
// Revision 1.3  1997/09/29 21:17:45  dunn
// Added wildcard matching in identity string in connection list.
//
// Revision 1.2  1997/09/17 22:11:52  dunn
// Code cleanup
//
// Revision 1.1  1997/08/19 22:28:22  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  Includes
//


#include "globals.H"
extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
}

#include "dhsGenConnect.H"
#include "dtsDhs.H"
#include "cmd.H"
#include "seq.H"
#include "con.H"


//
//  Local Defines
//

#define	KEYWORD_CONNECT		"authorized"


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::~cDtsConManager
//
// INVOCATION:
// n.a. Deconstructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// If there is a server connection open, then that connection is closed
//
// DESCRIPTION:
// If there is a server connection open, then that connection is closed.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cDtsConManager::~cDtsConManager
(
    void
)
{
    if ( serverConnected() )
    {
	//
	//  There is a server connection open, close it.
	//

	DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
	cdcServerConn.close( dhsStatus);
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::authorized
//
// INVOCATION:
// cDtsConManager::authorized()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) identityString (char *) Identity string to look up.
//
// FUNCTION VALUE:
// Returns TRUE if the identity string is an authorized user, otherwise
// FALSE.
//
// PURPOSE:
// Look for an authorized connector using the passed in identity string
//
// DESCRIPTION:
// Look for an authorized connector using the passed in identity string.
// Return a TRUE if found, otherwise false.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// It is successful if the identity string was in the data server config file.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cDtsConManager::authorized
(
    const char	*identityString		// (in)  The identity string to lookup.
)
{
    cDtsConManager
    		*pCon;			// Ptr to the connection.

   
    //
    //  Look up for that identityString.
    //

    pCon = (cDtsConManager *) cConnectList::lookup( identityString, TRUE );
   

    if ( pCon == NULL )
    {
	//
	//  Exact string not found, cycle thru the list and pattern match.
	//

	for ( iterator i = cctConnectList.begin(); 
		i != cctConnectList.end(); i++ )
	{
	    pCon = (cDtsConManager *) (*i).second;

	    if ( strpattern( (char *)identityString, (char *)pCon->identity() ) )
	    {
		//
		//  Found a match, assume they are o.k.
		//

		pCon->cdcMutexConClient.lock();
		pCon->clientConnected(TRUE);
		pCon->cdcMutexConClient.unlock();
		return( TRUE );
	    }
	}

	//
	//  No match found.
	//

	return( FALSE );

    }
    else
    {
	pCon->cdcMutexConClient.lock();
	pCon->clientConnected(TRUE);
	pCon->cdcMutexConClient.unlock();
	return( TRUE );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::config
//
// INVOCATION:
// cDtsConManager::config()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (clStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the authorized connectors from the configuration file
//
// DESCRIPTION:
// Read the authorized connectors from the configuration file.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsConManager::config
(
    clStatus  &status         	// (mod) Function return status.
)
{
    checkStat( status, return );
	 
    exit( status );
	       
    checkConfig( config_get( KEYWORD_CONNECT, (boolean (*) (int, char **))connectors ), status, 
	    return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::connectors
//
// INVOCATION:
// cDtsConManager::connectors()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens in config file.
// (>) tokens (char **) List of tokens in config file.
//
// FUNCTION VALUE:
// TRUE indiates valid line in the configuration file, otherwise false
// is returned.
//
// PURPOSE:
// Read in the tokens, and add to the connection list
//
// DESCRIPTION:
// Check to make sure there are 3 tokens, then create the object that will
// contains these tokens.  The constructor will add it to a list.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cDtsConManager::connectors
(
    int         numTokens,      // (in)  Number of tokens on the configuration
                                //       line.
    char 	*const*tokens        // (in)  List of tokens on the configuration
                                //       line.
)
{
    boolean     ret;	// Function return value.
    cDtsConManager	
                *pConn;	// Pointer to the newly created connection object.
    clStatus	status;	// Error status value.


    if ( numTokens == 3 )
    {
	//
	//  Check that we don't already have this identity string.
	//

	iterator	i;	// Ptr to object in list.

	if ( (i = cctConnectList.find( string( tokens[1] )) ) !=  
		cctConnectList.end() )
	{
	    status.E_MULTI_KEYWORD( status, tokens[1], tokens[0] );
	    ret = FALSE;
	}


	//
	//  This calls the constructor, which adds the tokens to the list.
	//

        checkNull( pConn = new cDtsConManager( tokens ),
		status, return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	//
	//  Wrong number of words in line in config file.
	//

	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::connectTo
//
// INVOCATION:
// cDtsConManager::connectTo()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned function status.
// (>) identityString (char *) Identity string to look up.
// (<) pConn (cDhsConnection **) Pointer to a connection.
//
// FUNCTION VALUE:
// Returns true if connection is made, otherwise false.
//
// PURPOSE:
// Connect to the passed in identity string, if not already connected
//
// DESCRIPTION:
// Connect to the passed in identity string, if not already connected.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cDtsConManager::connectTo
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*identityString,	// (in)  The identity string to lookup.
    cDhsConnection
    		**pConn			// (out) Pointer to a connection.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    					// Dhs status.
    iterator	i;			// Ptr to object in list.
    cDtsConManager
    		*pCon;			// Ptr to the connection.
    bool	ret;			// Return value.



    checkStat( status, return(FALSE) );
  

    //
    //  Lookup the connection in the connect list.
    //

    pCon = ( cDtsConManager *) cConnectList::lookup( identityString, TRUE );
    
    if ( pCon == NULL )
    {
	//
	//  Not found in the valid connectors list from the config file.
	//

	return( FALSE );
    }
    else
    {
	//
	//  Lock the server mutex, as this is a server connection.
	//

	pCon->cdcMutexConServer.lock();

	if ( pCon->serverConnected() )
	{
	    //
	    //  Already connected, unlock and return true.
	    //

	    *pConn = &( pCon->cdcServerConn );
	    pCon->cdcMutexConServer.unlock();
	    return ( TRUE );
	}


	//
	//  Open a connection.
	//

	pCon->cdcServerConn.open( pCon->ipAddr(), identityString, 
		dhsStatus );

	if ( dhsStatus == DHS_S_SUCCESS )
	{
	    //
	    //  Connection was succesful.
	    //

	    pCon->serverConnected(TRUE);
	    *pConn = &( pCon->cdcServerConn );
	    ret = TRUE;
	}
	else
	{
	    //
	    //  Connection was unsuccesful.
	    //

	    pCon->serverConnected(FALSE);
	    *pConn = NULL;
	    ret = FALSE;
	}
	pCon->cdcMutexConServer.unlock();


	//
	//  Print error messages from the open here.  Do outside locks, 
	//  because it sends the message to the status server.
	//

        if ( dhsStatus != DHS_S_SUCCESS )
        {
            const char *msg;
            DHS_STATUS  s( DHS_S_SUCCESS );
            status.E_OPENING( status );
            while ( s == DHS_S_SUCCESS )
            {
                msg = cDhs::message( s );
                if ( s == DHS_S_SUCCESS )
                {
                    status += msg;
                    cDhs::messageClear( s );
                }
            }
        }

	return( ret );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::close
//
// INVOCATION:
// cDtsConManager::close()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To cleanup this connection manager
//
// DESCRIPTION:
// To cleanup this connection manager.  Deletes from the connection list.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsConManager::close
(
    void
)
{

    //
    //  Delete from the list, the delete does an erase.
    //

    while( cctConnectList.size() != 0 )
    {
	delete( (cDtsConManager *)(*cctConnectList.begin()).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::disconnectTo
//
// INVOCATION:
// cDtsConManager::disconnectTo()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status ( cDtsStatus & ) Function return status.
// (>) identityString (char *) Identity string to disconnect to.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To indicate that the identity string passed in has disconnected
//
// DESCRIPTION:
// Set the disconnect flag for the identity string passed in.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Should already be in the connection list.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsConManager::disconnectTo
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*identityString		// (in)  The identity string to lookup.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    iterator	i;			// Ptr to object in list.
    cDtsConManager
    		*pCon;			// Ptr to the connection.



    checkStat( status, return );


    //
    //  Look up the identity string in the connection list.
    //

    pCon = ( cDtsConManager *) cConnectList::lookup( identityString, TRUE );

    
    if ( pCon == NULL )
    {
	//
	//  Did not find this person in our list, ignore them.
	//

	return;
    }
    else
    {
	//
	//  First check if they are connected as a server.
	//

	pCon->cdcMutexConServer.lock();
	pCon->serverConnected(FALSE);
	pCon->cdcMutexConServer.unlock();

	//
	//  Check if it is connected as a client.
	//

	pCon->cdcMutexConClient.lock();
	pCon->clientConnected(FALSE);
	pCon->cdcMutexConClient.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsConManager::testAll
//
// INVOCATION:
// cDtsConManager::testAll()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status ( cDtsStatus & ) Function status.
//
// FUNCTION VALUE:
// TRUE is the test passes, otherwise FALSE.
//
// PURPOSE:
// To check that all connections have correct connection status
//
// DESCRIPTION:
// To check that all connections have correct connection status.
// It they are suppose to be connected, attempt to send a ping
// to them, and update the connection status accordingly.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cDtsConManager::testAll
(
    cDtsStatus	&status 		// (mod) Function return status.
)
{
    DHS_CMD_STATUS	 cmdStat;	//  Returned command status from apply.
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    					//  Dhs status.
    cDtsConManager	*pConn;		//  Connection.
    cDhsClientCommand	 pCmd( DTS_PING_CMD );
    					//  Ping command.


    //
    //  Cycle through the connection list.
    //

    for ( iterator i = cctConnectList.begin(); i != cctConnectList.end(); i++ )
    {
	pConn = (cDtsConManager *) (*i).second;


	//
	//  Check only the server connections.  Only send a ping to them.
	//

	pConn->cdcMutexConServer.lock();

	if ( pConn->serverConnected() )
	{

	    //
	    //  Should be connected, unlock and try a ping to the server.
	    //  Cannot stay locked while sending to the server, it causes
	    //  deadlock because the server lock is used in the connection
	    //  callback, which if that callback is waiting on this lock
	    //  releaseing then both end up deadlocked.
	    //

	    pConn->cdcMutexConServer.unlock();
	    dhsStatus = DHS_S_SUCCESS;
	    cmdStat = DHS_CS_DONE;

	    checkDhs( pCmd.apply( pConn->cdcServerConn, dhsStatus ), 
		dhsStatus, status, VOID );
	    pCmd.wait( dhsStatus );
	    checkDhs( (cmdStat = pCmd.status( dhsStatus )), dhsStatus, status, 
		    VOID );

	    if ( cmdStat != DHS_CS_DONE || !status.ok() || 
		    dhsStatus != DHS_S_SUCCESS )
	    {
		status.S_SERVER_UNAVAIL( status, pConn->identity(),
		    pConn->ipAddr() );
		pConn->cdcMutexConServer.lock();
		pConn->serverConnected( FALSE );
		pConn->cdcMutexConServer.unlock();
	    }

	}
	else
	{
	    pConn->cdcMutexConServer.unlock();
	}
    }

    if ( status.ok() )
    {
	return( TRUE );
    }
    else
    {
	return( FALSE );
    }

	
}
