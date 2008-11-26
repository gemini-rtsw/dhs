static char rcsid[] = "$Id: streamCommands.C,v 1.1.1.1 2002-11-24 20:29:29 brighton Exp $";
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
// dhsQlServer/src/streamCommands.C
//
// PURPOSE:
// Contains the implementation of the classes to handle stream commands
//
// METHOD NAME(S)
// cQlsClientStreamCmd::exec 
//			- Executed to start the processing of a 
//			  qlsClientStream command.
// cQlsDsStreamCmd::exec 
//			- Executed to start the processing fo a qlsDsStream
//			  command.
// cQlsQueryStreamsCmd::exec 
//			- Executed to start the processsing of a 
//			  qlsQueryStreams command.
// cQlsServerStreamCmd::exec 
//			- Executed to start the processing of a qlsServerStream
//			  command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/15 23:06:24  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.2  1997/11/30 21:33:05  nhill
// Changed the cQlsDataset, cQlsSlave, and cQlsQlt classes to take care of
// list locking internally.
//
// Revision 1.1  1997/10/22 17:46:00  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_CLIENT
#define	DHS_NO_BULK
#include "dhs++.H"

#include "globals.H"
#include "qlsQlt.H"
#include "qlsDataset.H"
#include "qlsMaster.H"
#include "qlsSlave.H"
#include "streamCommands.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsClientStreamCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts the processing of an clientStream command
//
// DESCRIPTION:
// - Find the cQlsQlt object associated with the connection.
// - Unsubscribe all existing streams from the cQlsQlt.
// - Subscribe to the new list of streams.
// - Update the list of streams required from a master QLS.
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

void		cQlsClientStreamCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    ulong	dims[7];		// Dimensions of the streams attribute.
    ulong	i;
    int		nDims;			// Num dimensions of the streams 
    					// attribute.
    void	*pAttribData;		// Pointer to an attributes data.
    cQlsQlt	*pQlsQlt;		// Pointer to the cQlsQlt object.
    cQlsStatus	status;
    cDhsAttrib	streamAttrib;		// The streams attribute.
    char	**streamList;		// The new list of streams.


    response( DHS_CS_BUSY, dhsStatus );


    status.displayStop();


    //
    // Find the cQlsQlt object associated with the client.
    //

    pQlsQlt = cQlsQlt::find( connect(), &cQlsQlt::lock );
    if ( pQlsQlt != NULL )
    {
	//
	// Find the "streams" attribute in the command.
	//

	streamAttrib = find( "streams", dhsStatus );


	//
	// The the information about the streams attribute.
	//

	checkDhs( streamAttrib.info( nDims, dims, (void **) &pAttribData, 
		dhsStatus ), dhsStatus, status, VOID );


	//
	// Unsubscribe from all existing streams.
	//

	pQlsQlt->unSubscribeAll( status );


	//
	// Subscribe to the new list of streams.
	//

	if ( nDims == 1 )
	{
	    streamList = (char **) pAttribData;
	    for ( i = 0; i < dims[0] && status.ok(); i++ )
	    {
		pQlsQlt->subscribe( streamList[i], status );
		status.S_CLIENT_SUBSC( status, pQlsQlt->connect().name(), 
			streamList[i] );
	    }
	}
	else if ( nDims == 0 && pAttribData != NULL )
	{
	    pQlsQlt->subscribe( (char *) pAttribData, status );
	    status.S_CLIENT_SUBSC( status, pQlsQlt->connect().name(), 
		    (char *) pAttribData );
	}
	else
	{
	    status.E_CMD_FORMAT( status, name () );
	}

	pQlsQlt->unlock();


	//
	// If necessary, update the stream requested of the master server.
	//

	cQlsMaster::updateStreams( status );
    }


    //
    // Check for error messages.
    //

    if ( status.ok() )
    {
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	//
	// An error occured, so return an error status and display the
	// messages.
	//

	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDsStreamCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts the processing of an dsStream command
//
// DESCRIPTION:
// - Get the datset name from the command.
// - Find the cQlsDataset object matching the command.
// - Add the streams to the cQlsDataset object.
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

void		cQlsDsStreamCmd::exec
(
    void
)
{
    cDhsAttrib	datasetAttrib;	// The dataset attribute.
    char	*datasetName;	// Name of the dataset.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    ulong	dims[7];	// Dimensions of the array.
    int		nDims;		// Number of dimensions.
    cQlsDataset	*pDataset;	// The dataset object describing the dataset.
    cQlsStatus	status;
    char	**streams;	// list of stream names.
    cDhsAttrib	streamsAttrib;	// The streams attribute.


    checkDhs( response( DHS_CS_BUSY, dhsStatus ), dhsStatus, status, return );


    //
    // Get the dataset name from the command.
    //

    datasetAttrib = find( "dataset", dhsStatus );
    datasetAttrib.info( (void **) &datasetName, dhsStatus );
    

    //
    // Get the list of streams from the command.
    //

    streamsAttrib = find( "streams", dhsStatus );
    checkDhs( streamsAttrib.info( nDims, dims, (void **) &streams, 
	    dhsStatus ), dhsStatus, status, VOID );
    if ( ! status.ok() )
    {
	checkDhs( response( DHS_CS_ERROR, dhsStatus ), 
		dhsStatus, status, VOID );
	return;
    }


    //
    // Find the dataset with the specified name, creating one if necessary.
    //

    pDataset = cQlsDataset::findCreate( datasetName, &cQlsDataset::wLock );


    if ( pDataset != NULL )
    {
	//
	// Set the streams for the dataset.
	//

	pDataset->setQlStream( streams, dims[0], status );

	pDataset->unlock();
    }


    //
    // Send the response.
    //

    if ( status.ok() )
    {
	checkDhs( response( DHS_CS_DONE, dhsStatus ), 
		dhsStatus, status, return );
    }
    else
    {
	checkDhs( response( DHS_CS_ERROR, dhsStatus ), 
		dhsStatus, status, return );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQueryStreamsCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the execution of the queryStreams command
//
// DESCRIPTION:
// - If this is a slave QLS, get the list of streams from the master QLS.
// - If this is a master QLS, add the complete list of streams to the response.
// - Send the response containing the list of streams.
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

void		cQlsQueryStreamsCmd::exec
(
    void
)
{
    char	*address;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*name;
    cDhsAvList	responseAvList;		// Avlist to contain the response.
    cQlsStatus	status;


    checkDhs( response( DHS_CS_BUSY, "working", dhsStatus ), 
	    dhsStatus, status, return );


    dhsConnectInfo( connect(), &address, &name, &dhsStatus );

    status.S_QUERY_STREAMS( status, name, address );


    if ( cQlsMaster::isSlave() )
    {
	//
	// This is a slave QLS, forward the request to the master. 
	//
	check( cQlsMaster::queryStreams( responseAvList, status ), 
		status, VOID );
    }
    else
    {
	//
	// This is a master QLS, get the list of ql streams from the 
	// cQlsStream class.
	//

	check( cQlsStream::addStreams( responseAvList, false, status ), 
		status, VOID );
    }


    if ( status.ok() )
    {
	//
	// Everything worked ok, send the response back.
	//

	checkDhs( response( DHS_CS_BUSY, "done", responseAvList, dhsStatus ), 
		dhsStatus, status, VOID );
	checkDhs( response( DHS_CS_DONE, "done", dhsStatus ), 
		dhsStatus, status, VOID );
    }
    else
    {
	//
	// Something went wrong, send an error back.
	//

	checkDhs( response( DHS_CS_ERROR, "oops", dhsStatus ), 
		dhsStatus, status, VOID );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsServerStreamCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts the processing of a serverStream command
//
// DESCRIPTION:
// - Find the cQlsSlave object associated with the connection.
// - Remove all streams currently subcribed by the slave.
// - Add the new list of streams to the slave.
// - Send an updated list of streams to the master QLS.
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

void		cQlsServerStreamCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    ulong	dims[7];	// Dimensions of the streams attribute.
    ulong	i;
    int		nDims;		// Number of dimensions of the streams 
    				// attribute.
    cQlsSlave	*pQlsSlave;	// The slave associated with the connection.
    cQlsStatus	status;
    cDhsAttrib	streamAttrib;	// The streams attribute.
    char	**streamList;	// String array containing the list of stream 
    				// names.


    response( DHS_CS_BUSY, dhsStatus );


    status.displayStop();


    //
    // Find the cQlsSlave object associated with the slave.
    //

    pQlsSlave = cQlsSlave::find( connect(), &cQlsSlave::lock );
    if ( pQlsSlave != NULL )
    {
	//
	// Get the "streams" attribute from the command.
	//

	streamAttrib = find( "streams", dhsStatus );
	streamAttrib.info( nDims, dims, (void **) &streamList, dhsStatus );


	//
	// Make sure the streams value was extracted correctly.
	//

	if ( dhsStatus == DHS_S_SUCCESS && nDims == 1 )
	{
	    //
	    // Unsubscribe all existing streams.
	    //

	    pQlsSlave->unSubscribeAll( status );


	    //
	    // Subscribe the new list of streams.
	    //

	    for ( i = 0; i < dims[0] && status.ok(); i++ )
	    {
		pQlsSlave->subscribe( streamList[i], status );
		status.S_SLAVE_SUBSC( status, 
			pQlsSlave->connect().name(), streamList[i] );
	    }


	    pQlsSlave->unlock();


	    //
	    // If necessary, update the stream requested of the master server.
	    //

	    cQlsMaster::updateStreams( status );
	}
	else
	{
	    //
	    // Streams attribute could not be found, so the command format
	    // is invalid.
	    //

	    pQlsSlave->unlock();
	    status.E_CMD_FORMAT( status, name() );
	}
    }


    if ( status.ok() )
    {
	//
	// Everthing worked ok, set command state to done.
	//

	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	//
	// Something went wrong. Set the command state to error and display
	// the messages.
	//

	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
    }
}
