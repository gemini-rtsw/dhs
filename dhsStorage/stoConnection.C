static char rcsid[] = "$Id: stoConnection.C,v 1.1.1.1 2002-11-24 20:31:49 brighton Exp $";
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
// dhsStorage/src/stoConnection.C
//
// PURPOSE:
// Contains methods for connection management. 
//
// CLASSES
// n/a
// 
// METHOD NAME(S)
// cStoConnection::config	- cStoConnection class configuration routine.
// cStoConnection::localDataServer
// 				- To parse configuration file lines with 
//				  keyword "localDataServer" from the 
//				  configuration file. 
// cStoNode::authorized		- To parse configuration file lines with 
//				  keyword "connection" from the configuration 
//				  file.
// cStoNode::notifyDataServer	- To parse configuration file lines with 
//				  keyword "notifyDataServer" from the
//				  configuration file. 
// cStoConnection::aliveCommands- This method sends startup commands to each 
//				  of the dataservers in the configuration file
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.11  2000/08/22 19:25:54  nhill
// Added class name to a reference to a member class.
//
// Revision 1.10  1999/10/12 22:51:19  nhill
// Updated the documentation.
//
// Revision 1.9  1999/06/18 22:29:42  nhill
// Added the explicit deletion of the command.
//
// Revision 1.8  1999/06/16 23:55:04  nhill
// Removed the check to see if the connection is already open, and
// added closing the connection.
//
// Revision 1.7  1999/02/24 23:50:06  jaeger
// Ensured pConnect pointer is NULL before calling STL find function.
//
// Revision 1.6  1998/06/25 18:09:23  cockayne
// Modified to match data server's config file syntax.
//
// Revision 1.5  1998/06/24 16:00:57  cockayne
// Modified 'initial-commands' sending to be more robust, & not set health
// to warning if not successful.
//
// Revision 1.4  1998/05/15 17:05:20  cockayne
// Mod'ed to get list of dataservers to notify of startup, and to send
// startup commands to those dataservers.
//
// Revision 1.3  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.2  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"

#include "stoConnection.H"


//
// Define configuration strings.
//

#define	STO_CFG_AUTHORIZED		"authorized"
#define	STO_CFG_LOCALDATASERVER		"localDataServer"
#define	STO_CFG_NOTIFYDATASERVER	"notifyDataServer"


//
// Define the static data in the cStoStage class.
//

cDhsConnection	*cStoConnection::scPDataServer( NULL );
cStoConnection::notL	
		cStoConnection::scNotifyList;



//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoConnection::config
//
// INVOCATION:
// config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &)	- Program status. 
// 
// FUNCTION VALUE:
// None.
// 
// PURPOSE:
// Perform runtime configuration of some cStoConnection parameters.
// 
// DESCRIPTION:
// Get the 'known connections' list, and the dataserver connection 
// configurable information. 
// 
// EXTERNAL VARIABLES:
// None.
// 
// PRIOR REQUIREMENTS:
// None.
// 
// SEE ALSO:
// None.
// 
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoConnection::config
(
    cStoStatus	&status
)
{
    checkStat( status, return );

	
    checkConfig( config_get( STO_CFG_AUTHORIZED, authorized ),
	    status, return );
    checkConfig( config_get( STO_CFG_LOCALDATASERVER, localDataServer ),
	    status, return );
    checkConfig( config_get( STO_CFG_NOTIFYDATASERVER, notifyDataServer ),
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoConnection::localDataServer
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)	  - Number of tokens in the tokens array.
// (>) tokens	(char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of 
//           tokens is correctly parseable. FALSE otherwise. 
//
// PURPOSE:
// To parse configuration file lines with keyword "localDataServer" from the
// configuration file. 
//
// DESCRIPTION:
// Ensure the second token is in the list of connection objects. Save a 
// pointer to that connection object. 
// 
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
 
boolean         cStoConnection::localDataServer
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStoStatus	status;		// Status code.


    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 2 )
    {
	//
	// Find dataserver connection in list. 
	//

	scPDataServer = cStoConnection::find( tokens[1] );
	if ( scPDataServer == NULL )
	{
	    status.E_CFG_LOCALDATASERVER( status, tokens[1] );
	    ret = FALSE;
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }


    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::authorized
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse configuration file lines with keyword "connection" from the
// configuration file.
//
// DESCRIPTION:
// Ensure the second token is not already in the list of connection objects. 
// Create a new connection object with the second token as a connection name,
// and the third token as the address (name or dotted quad).
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//
//-
//***********************************************************************
//
 
boolean         cStoConnection::authorized
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStoStatus	status;		// Status code.
    cStoConnection
    		*pConnect;	// Ptr. to connection object. 


    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 3 )
    {
	//
	// If identity isn't already in list, create new connection object. 
	//

	pConnect = NULL;
	pConnect = cStoConnection::find( tokens[1] );
	if ( pConnect != NULL )
	{
	    status.E_CFG_CONNECTION( status, tokens[1] );
	    ret = FALSE;
	}
	else
	{
	    pConnect = NULL;
	    checkNull( ( pConnect = 
		    new cStoConnection( tokens[1], tokens[2] ) ),
		    status, ret = FALSE );
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }


    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::notifyDataServer
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)	  - Number of tokens in the tokens array.
// (>) tokens	(char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of 
//           tokens is correctly parseable. FALSE otherwise. 
//
// PURPOSE:
// To parse configuration file lines with keyword "notifyDataServer" from the
// configuration file. 
//
// DESCRIPTION:
// Ensure the second token is in the list of connection objects. Save a 
// pointer to that connection object in the notify list. 
// 
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
 
boolean         cStoConnection::notifyDataServer
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStoStatus	status;		// Status code.

    cStoConnection
    		*pConnection;

    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 2 )
    {
	//
	// Find dataserver connection in list. 
	//

	pConnection= NULL;
	pConnection = cStoConnection::find( tokens[1] );
	if ( scPDataServer == NULL )
	{
	    status.E_CFG_NOTIFYDATASERVER( status, tokens[1] );
	    ret = FALSE;
	}
	else
	{
	    scNotifyList.push_back( pConnection );
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }


    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoConnection::aliveCommands
//
// INVOCATION:
// cStoConnection::aliveCommands( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method sends startup commands to each of the dataservers in
// the configuration file
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

void		cStoConnection::aliveCommands
( 
    cStoStatus	&status
)
{
    cStoStartCmd
		*cmd;			//
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					//
    notIt	connListIter;		//
    cStoConnection
		*pConnect;		//

    checkStat( status, return );
    for ( connListIter = scNotifyList.begin();
	    connListIter != scNotifyList.end();
	    connListIter++ )
    {
	pConnect = (*connListIter);
	pConnect->open( dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    dhsStatus = DHS_S_SUCCESS;
	    continue;
	}
	checkNull( ( cmd = new cStoStartCmd( STO_CMD_STARTUP ) ), 
		status, break );
	checkDhs( cmd->apply( *pConnect, dhsStatus ), 
		dhsStatus, status, VOID );
	checkDhs( cmd->wait( dhsStatus ), dhsStatus, status, VOID );
	if ( !status.ok() )
	{
	    status.S_SUCCESS( status );
	}
	delete cmd;
	pConnect->close( dhsStatus );
    }
};

