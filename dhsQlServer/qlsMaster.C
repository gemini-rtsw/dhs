static char rcsid[] = "$Id: qlsMaster.C,v 1.1.1.1 2002-11-24 20:29:19 brighton Exp $";
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
// dhsQlServer/src/qlsMaster.C
//
// PURPOSE:
// Contains the implementation of the cQlsMaster class
//
// METHOD NAME(S)
// cQlsMaster::config - Read the master QLS information from the configuration
//		file.
// cQlsMaster::masterId - Process a masterId line from the configuration file.
// cQlsMaster::open - Open the connection to the master QLS.
// cQlsMaster::queryStreamsCmd - Get a list of QL streams from the maseter QLS.
// cQlsMaster::updateStreams - Send an updated subscription list to the
//		master QLS.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.5  1998/04/15 22:56:22  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.4  1997/11/30 21:29:30  nhill
// Typo fixes and changed the call to addStreams.
//
// Revision 1.3  1997/10/01 17:24:50  nhill
// Second alpha release.
//
// Revision 1.2  1997/08/20 21:24:10  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define	DHS_NO_BULK
#define	DHS_NO_SERVER
#include "dhs++.H"

#include "globals.H"
#include "qlsMaster.H"
#include "qlsStream.H"

class cQlsMaster *cQlsMaster::qlmMaster = NULL;
bool		cQlsMaster::qlmIsSlave = TRUE;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsMaster::config
//
// INVOCATION:
// cQlsMaster::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the master QLS server information from the configuration file.
//
// DESCRIPTION:
// Call the config_get function to read the masterId configuration line.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must have been opened with the config_open function.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsMaster::config
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    checkConfig( config_get( "masterId", masterId ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsMaster::masterId
//
// INVOCATION:
// n.a. Called automatically by the config_get routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (boolean) True if the function succeeded, false otherwise.
//
// PURPOSE:
// Parses a masterId line from the configuration file
//
// This function should only be called once.
//
// The format of the masterId line is:
//	masterId	<internet address>	<imp name>
//
// DESCRIPTION:
// Create a new cQlsMaster object to represent the master QLS.
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

boolean		cQlsMaster::masterId
(
    int		numTokens,	// (in)  Number of tokens on the configuration 
				//	 line.
    char	**tokens	// (in)  Tokens on the configuration line.
)
{
    boolean	ret;		// The return value.
    cQlsStatus	status;		// Status code.


    //
    // Check the line had the right number of tokens.
    //

    if ( numTokens == 3 )
    {
	if ( qlmMaster != NULL )
	{
	    //
	    // Delete an existing master QLS. (this really shouldn't happen).
	    //

	    delete qlmMaster;
	    qlmMaster = NULL;
	}


	//
	// Create a new cQlsMaster object.
	//

	qlmMaster = new cQlsMaster( tokens[1], tokens[2] );
	ret = TRUE;
    }
    else
    {
	//
	// Command line was wrong, print an error.
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
// cQlsMaster::open
//
// INVOCATION:
// cQlsMaster::open( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Open the connection to the master QLS
//
// DESCRIPTION:
// Check that the connection information has been set up, and then use the
// dhs++ library cDhsConnection::open function to open the connection.
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

void		cQlsMaster::open
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    //
    // Make sure there is a master QLS.
    //

    if ( qlmMaster != NULL )
    {
	//
	// Open the connection.
	//

	checkDhs( qlmMaster->cDhsConnection::open( 
		dhsStatus ), dhsStatus, status, 
		return );
    }
    else
    {
	//
	// There isn't a QLS to open.
	//

	status.E_NO_MASTER( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsMaster::queryStreams
//
// INVOCATION:
// cQlsMaster::queryStreams( avList, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) avList (cDhsAvList &) Attribute value list to contain the streams.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a command to the master QLS requesting a list of quick look streams
//
// The returned list of quick look streams is added to the avList parameter.
//
// DESCRIPTION:
// - Send a qlsQueryStreams command to the master QLS.
// - Wait for the command to complete
// - Extract the streams attribute from the response.
// - Copy the streams attribute into the output av list.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The connection to the master QLS should be open.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsMaster::queryStreams
(
    cDhsAvList	&avList,	// (mod) The updated av list.
    cQlsStatus	&status		// (mod) The function return status.
)
{
    ulong	dims[7];	// Dimensions of the streams array.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    int		i;
    int		nDims;		// Dimensions of the streams array.
    char	**streams;	// Array of stream names.
    cDhsAttrib	streamsAttrib;	// Attibute containing stream list.
    cDhsClientCommand		// Command to send to the master QLS.
    		queryStreamsCmd( "qlsQueryStreams" );
    cDhsAvList	responseList( DHS_AV_LIST_NULL );
    				// The response AV list.
    const char	*msg;		// DHS library error message.
    DHS_STATUS	s( DHS_S_SUCCESS );


    //
    // Send the command to the maseter QLS and wait for the command to 
    // complete.
    //

    queryStreamsCmd.apply( *qlmMaster, dhsStatus );
    queryStreamsCmd.wait( dhsStatus );
    if ( queryStreamsCmd.status( dhsStatus ) == DHS_CS_ERROR )
    {
	status.E_SEND( status, "qlsQueryStreams" );
    }


    //
    // Look at all of the responses received for the command.
    //

    for ( i = 0; dhsStatus == DHS_S_SUCCESS; i++ )
    {
	//
	// Get the response list from the i'th response.
	//

	checkDhs( responseList = queryStreamsCmd.responseGet( i, dhsStatus ),
		dhsStatus, status, break );


	//
	// Look for the "steams" attribute in the response av list.
	//

	streamsAttrib = responseList.find( "streams", dhsStatus );


	//
	// If the streams attribute was found, copy the attribute to
	// the output av list.
	//

	if ( dhsStatus == DHS_S_SUCCESS )
	{
	    //
	    // Get the attribute value.
	    //

	    checkDhs( streamsAttrib.info( nDims, dims, (void **) &streams, 
		    dhsStatus ), dhsStatus, status, return );


	    //
	    // copy the attribute to the output av list.
	    //

	    checkDhs( avList.add( "streams", DHS_DT_STRING, nDims, dims, 
		    streams, dhsStatus ), dhsStatus, status, return );
	    break;
	}
	else if ( dhsStatus == DHS_S_NO_ATTRIB )
	{
	    //
	    // There was no streams attribute in the av list, which is not
	    // a big deal.
	    //

	    dhsStatus = DHS_S_SUCCESS;
	}
	else
	{
	    //
	    // An error occured, so print a message.
	    //

	    status.E_DHS( status, "streamsAttrib = responseList.find( "
		    "\"streams\", dhsStatus )" );
	    s = DHS_S_SUCCESS;
	    while ( s == DHS_S_SUCCESS )
	    {
		msg = cDhs::message( s );
		if ( s == DHS_S_SUCCESS )
		{
		    status += msg;
		}
	    }
	    break;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsMaster::updateStreams
//
// INVOCATION:
// cQlsMaster::updateStreams( status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// If necessary, send an updated stream subscription list to the master server
//
// DESCRIPTION:
// - Send a qlsServerStream command to the master QLS.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// A connection must be opened to the master QLS.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsMaster::updateStreams
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cDhsClientCommand		// The command to send to the master QLS.
    		cmd( "qlsServerStream" );
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    // Don't send the command unless this is a slave QLS.
    //

    if ( ! isSlave() )
    {
	return;
    }


    //
    // Create a list of stream names.
    //

    cQlsStream::addStreams( cmd, true, status );


    //
    // Send the command to the master QLS.
    //

    checkDhs( cmd.apply( *qlmMaster, dhsStatus ), dhsStatus, status, return );


    //
    // Wait for the command to complete.
    //

    checkDhs( cmd.wait( dhsStatus ), dhsStatus, status, return );

    if ( cmd.status( dhsStatus ) == DHS_CS_ERROR )
    {
	status.E_SEND( status, "qlsServerStream" );
    }
}
