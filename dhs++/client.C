static char rcsid[] = "$Id: client.C,v 1.1.1.1 2002-11-24 20:24:51 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
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
// dhs++/src/client.C
//
// PURPOSE:
// Contains implementation code used by all clients.
//
// MEHTOD NAME(S)
// cDhsConnection::cDhsConnection - constructor for the cDhsConnection class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:01  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.13  1999/01/20 22:53:43  nhill
// Fixed a bug that allowed a callback for a connection object that had been
// deleted.
//
// Revision 1.12  1998/08/04 16:18:11  nhill
// Fixed a bug which occured when a connection object was destroyed.
//
// Revision 1.11  1998/06/24 23:12:26  nhill
// Added setting of the user data pointer to null in the connection
// destructor.
//
// Revision 1.10  1998/05/11 16:35:14  nhill
// Added a callback function to be executed when the connection state
// changes.
//
// Revision 1.9  1998/01/20 19:01:49  nhill
// Added a connection callback routine.
//
// Revision 1.8  1997/11/12 18:21:13  nhill
// Changed cDhsConnection::open to set the open state to true only if the
// dhsConnect function succeeds.
//
// Revision 1.7  1997/08/20 21:29:06  nhill
// Changed the cQlsConnect destructor to not close connections created
// from existing connections.
//
// Revision 1.6  1997/08/19 19:51:46  nhill
// Added function cDhsConnect to create a connection from an existing
// DHS_CONNECT.
//
// Revision 1.5  1997/07/23 19:43:14  nhill
// *** empty log message ***
//
// Revision 1.4  1997/05/01 18:31:33  nhill
// Move the check for NULL address from the constructor to the open.
//
// Revision 1.3  1997/04/30 23:47:48  nhill
// *** empty log message ***
//
// Revision 1.2  1997/04/15 20:02:41  nhill
// Moved the open function here from the header.
// Added freeing of allocated memory before pointers are reused.
//
// Revision 1.1  1997/03/06 19:20:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <stdlib.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
}

#define	DHS_NO_SERVER
#define DHS_NO_BULK
#define	DHS_NO_COMMAND
#include "dhs++.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::cDhsConnection
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) ipAddress (char *) Address of the server.
// (>) name (char *) Name of teh server.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a connection
//
// DESCRIPTION:
// Description of the algorithm.
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

		cDhsConnection::cDhsConnection
(
    char	*ipAddress,	// (in)  Address of the server.
    char	*name		// (in)  Name of the server.
)
{
    dcName = strsav( name );

    dcIpAddress = strsav( ipAddress );
    dcConnection = DHS_CONNECT_NULL;
    dcOpen = false;
    dcCallbackFn = NULL;
    dcCallbackArg = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::cDhsConnection
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (DHS_CONNECT) The existing connection.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a connection for an existing DHS_CONNECT id
//
// DESCRIPTION:
// Trivial.
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

		cDhsConnection::cDhsConnection
(
    DHS_CONNECT	connection	// (in)  The connection to set.
)
{
    char	*ipAddress;	// Address of the other end.
    char	*name;		// Imp Name of the other end.
    DHS_STATUS	status( DHS_S_SUCCESS );


    dcConnection = connection;
    dhsConnectInfo( connection, &ipAddress, &name, &status );
    dhsConUserDataSet( connection, this, &status );
    dcName = strsav( name );
    dcIpAddress = strsav( ipAddress );
    dcOpen = false;
    dcCallbackFn = NULL;
    dcCallbackArg = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::~cDhsConnection
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for a connection
//
// DESCRIPTION:
// Description of the algorithm.
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

		cDhsConnection::~cDhsConnection
(
    void
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );

    if ( dcConnection != DHS_CONNECT_NULL )
    {
	dhsConUserDataSet( dcConnection, NULL, &s );
	if ( dcOpen )
	{
	    dhsDisconnect( dcConnection, &s );
	}
    }

    if ( dcName != NULL )
    {
	free( dcName );
    }

    if ( dcIpAddress != NULL )
    {
	free( dcIpAddress );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::connectCallback
//
// INVOCATION:
// n.a. callback routine executed by the DHS library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (DHS_CONNECT) The connection that changed.
// (>) state (DHS_CON_STATE) The new state of the connection.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is executed whenever a connection state changes
//
// This funtion keeps the dcConnected flag up to date.
//
// DESCRIPTION:
// Description of the algorithm.
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

void	cDhsConnection::connectCallback
(
    DHS_CONNECT	connection,	// (in)  The connection that changed.
    DHS_CON_STATE		// (in)  The new connection state.
    		state
)
{
    cDhsConnection
    		*pConnect;
    DHS_STATUS	status( DHS_S_SUCCESS );


    pConnect = (cDhsConnection *) dhsConUserDataGet( connection, &status );

    if ( status == DHS_S_SUCCESS && pConnect != NULL )
    {
	if ( state == DHS_CON_CONNECTED )
	{
	    pConnect->dcOpen = true;
	}
	else
	{
	    pConnect->dcOpen = false;
	}
    }

    if ( pConnect != NULL && pConnect->dcCallbackFn != NULL )
    {
	pConnect->dcCallbackFn( pConnect, pConnect->dcCallbackArg );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::open
//
// INVOCATION:
// connect.open( ipAddress, name, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) ipAddress (char *) Address of the server.
// (>) name (char *) Name of teh server.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Open a connection to a server
//
// DESCRIPTION:
// Description of the algorithm.
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

void		cDhsConnection::open
(
    char	*ipAddress,	// (in)  Address of the server.
    char	*name,		// (in)  Name of the server.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( dcIpAddress != NULL )
    {
	free( dcIpAddress );
    }
    dcIpAddress = strsav( ipAddress );

    if ( dcName != NULL )
    {
	free( dcName );
    }
    dcName = strsav( name );

    dcConnection = ::dhsConnect( dcIpAddress, dcName, this, &status );
    if ( status == DHS_S_SUCCESS )
    {
	dcOpen = true;
    }
}
