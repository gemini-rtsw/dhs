static char rcsid[] = "$Id: connect.C,v 1.1.1.1 2002-11-24 20:27:49 brighton Exp $";
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
// dhsGen/src/connect.C
//
// PURPOSE:
// Whatever
//
// METHOD NAME(S)
// cConnectList::authorized() 	- Returns true if authorized.
// cConnectList::cConnectList()	- Constructor.  Does additions to list
// cConnectList::~cConnectList()- Deconstructor.  Deletes from list.
// cConnectList::config() 	- Reads the authorized users from a config file.
// cConnectList::connectors()	- Adds an identity string & ipAddress to a list.
// cConnectList::exit() 	- Delete all connection objects.
// cConnectList::init() 	- Save the debug level and set init flag.
// cConnectList::lookup() 	- Looks up the ip addr of identity passed in.
// cConnectList::lookup() 	- Looks up the identity string passed in.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2001/03/27 23:54:57  nhill
// Increased the constness of the argument to the cConnectList constructor.
//
// Revision 1.7  2000/08/17 23:51:09  nhill
// Fixed a C++ compatability problem.
//
// Revision 1.6  1998/04/27 20:52:01  dunn
// Fixed a log message
//
// Revision 1.5  1998/04/24 20:55:03  dunn
// Fixed status message
//
// Revision 1.4  1998/04/20 21:19:59  nhill
// Updated to the latest version of the cDhsStatus class.
//
// Revision 1.3  1997/08/19 19:59:57  dunn
// Cleaned up the file.
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
}

#include "localDhsSubsystem.H"


//
//  Define locals
//

cConnectList::tConnectList
		cConnectList::cctConnectList;	// List of auth. connectors.

msgText( cConnectList::clStatus, S_CONN_DEL, P_DEBUG_FULL,
	    "Deleting from list conn identity:%s, ipaddr:%s\n" );
msgText( cConnectList::clStatus, S_CONN_ADD, P_DEBUG_FULL,
	    "Adding to list conn identity:%s, ipaddr:%s\n" );
msgText( cConnectList::clStatus, E_MULTI_KEYWORD, P_DEBUG_MIN,
	    "Error, identity string %s must be unique in configuration keyword `%s'" );

//
//  Define Keywords
//

#define GEN_KEYWORD_CONNECT	"authorized"


//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::authorized
//
// INVOCATION:
// cConnectList::authorized()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) identityString (char *) Identity string to look up.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Look for an authorized connector using the passed in identity string.
//
// DESCRIPTION:
// Look for an authorized connector using the passed in identity string.
// Return a TRUE if found, otherwise false.
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

boolean		cConnectList::authorized
(
    const char	*identityString		// (in)  The identity string to lookup.
)
{
    iterator	i;			// Ptr to object in list.

    i = cctConnectList.find( string( identityString ) );
    if ( i == cctConnectList.end() )
    {
	return( FALSE );
    }
    else
    {
	return( TRUE );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::cConnectList
//
// INVOCATION:
// cConnectList::cConnectList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) tokens (char **) Lost of tokens in config file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor.  Add to the authorized connectors list.
//
// DESCRIPTION:
// Save the identity and ip address strings in the token string to the
// object created.  Then add the object to the connector list.
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

		cConnectList::cConnectList
(
    const char  *const	*tokens // (in)  List of tokens on the configuration
                                //      line.
)
{
    clStatus		status;	// Status code.



    //
    //  Save the identity and ip address strings to the object.
    //

    checkNull( cctIdentityString = strsav( tokens[1] ), status, return );
    checkNull( cctIpAddress = strsav( tokens[2] ) , status, return  );
    cctConnected = FALSE;


    status.S_CONN_ADD( status, cctIdentityString, cctIpAddress);

    //
    //  Now add this authorized user to the connection list.
    //

     cctConnectList[ string( cctIdentityString ) ] = this;
    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::~cConnectList
//
// INVOCATION:
// cConnectList::cConnectList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor. Erase all objects in the list. 
//
// DESCRIPTION:
// Erase all objects in the list and free both chars.
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

		cConnectList::~cConnectList
(
    void 
)
{
    clStatus		status;	// Status code.


    //
    //  Erase all objects in the list.
    //
    
    status.S_CONN_DEL( status,  cctIdentityString, cctIpAddress);

    cctConnectList.erase( cctIdentityString );
    free( cctIdentityString );
    free( cctIpAddress );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::config
//
// INVOCATION:
// cConnectList::config()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (clStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the authorized connectors from the configuration file.
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

void		cConnectList::config
(
    clStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );
	 
    exit( status );
	       
    checkConfig( config_get( GEN_KEYWORD_CONNECT, connectors ), status, 
	    return );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::connectors
//
// INVOCATION:
// cConnectList::connectors()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens in config file.
// (>) tokens (char **) Lost of tokens in config file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read in the tokens, and add to the connection list.
//
// DESCRIPTION:
// Check to make sure there are 3 tokens, then create the object that will
// contains these tokens.
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

boolean		cConnectList::connectors
(
    int         numTokens,      // (in)  Number of tokens on the configuration
                                //      line.
    char  	**tokens        // (in)  List of tokens on the configuration
                                //      line.
)
{
    boolean     ret;		// Return value.
    cConnectList		// Pointer to the newly created connection object.
                *pConn;
    clStatus	status;		// Error status value.


    if ( numTokens == 3 )
    {
	//
	//  Check that we don't already have this identity string.
	//

	iterator	i;			// Ptr to object in list.

	if ( (i = cctConnectList.find( string( tokens[1] )) ) !=  
		cctConnectList.end() )
	{
	    status.E_MULTI_KEYWORD( status, tokens[1], tokens[0] );
	    ret = FALSE;
	}
	
	//
	//  This calls the constructor, which adds the tokens to the list.
	//

        checkNull( pConn = new cConnectList( tokens ),
		status, return( FALSE ) );
	ret = TRUE;
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
// cConnectList::exit
//
// INVOCATION:
// cConnectList::exit()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (clStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all connection objects.
//
// DESCRIPTION:
// While the size of the list is not 0, delete from the list.
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

void		cConnectList::exit
(
    clStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );
	
    while( cctConnectList.size() != 0 )
    {
	delete( (*cctConnectList.begin()).second );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::init
//
// INVOCATION:
// cConnectList::init()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) level (tDebugLevel) Current debug level.
// (!) status (clStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Pass on the debug level
//
// DESCRIPTION:
// Pass on the debug level
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

void		cConnectList::init
(
    cDhsStatus::tDebugLevel
		level,			// (in)  Current debug level.
    clStatus	&status         	// (mod) Function return status.
)
{
    checkStat( status, return );
	
    cDhsStatus::debugLevel( level );
 
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::lookup
//
// INVOCATION:
// cConnectList::lookup()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) identityString (char *) Identity string to look up.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Look for an authorized connector's ip address using the passed in 
// identity string.
//
// DESCRIPTION:
// Look in the list given the identity string.  Make sure its not the end
// of the list and if it is then return a null otherwise
// return the ip address.
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

char		*cConnectList::lookup
(
    const char	*identityString		// (in)  The identity string to lookup.
)
{
    iterator	i;			// Ptr to object in list.

    i = cctConnectList.find( string( identityString ) );
    if ( i == cctConnectList.end() )
    {
	return( NULL );
    }
    else
    {
	return( (*i).second->cctIpAddress );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cConnectList::lookup
//
// INVOCATION:
// cConnectList::lookup()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) identityString (char *) Identity string to look up.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Look for an authorized connector using the passed in 
// identity string.
//
// DESCRIPTION:
// Look in the list given the identity string.  Make sure its not the end
// of the list and if it is then return a null otherwise
// return a pointer to that item.
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

cConnectList	*cConnectList::lookup
(
    const char	*identityString,	// (in)  The identity string to lookup.
    bool	tmp			// (in)  Nothing.
)
{
    iterator	i;			// Ptr to object in list.

    i = cctConnectList.find( string( identityString ) );
    if ( i == cctConnectList.end() )
    {
	return( NULL );
    }
    else
    {
	return( (*i).second );
    }
}
