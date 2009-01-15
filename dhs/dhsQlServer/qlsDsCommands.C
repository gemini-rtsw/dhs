static char rcsid[] = "$Id: qlsDsCommands.C,v 1.1.1.1 2002-11-24 20:29:10 brighton Exp $";
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
// dhsQlServer/src/qlsDsCommands.C
//
// PURPOSE:
// Contains the implementation of the classes for handling datasets
//
// METHOD NAME(S)
// cQlsDsAbortCmd::exec - Executed to start the processing of a qlsDsAbort 
//			  command.
// cQlsDsGet::config	- Read the configuration file.
// cQlsDsGet::dataServerId	
//			- Read the dataServerId data from the config file.
// cQlsDsGet::exec	- Executed to start the processing of a qlsDsGet 
//			  command
// cQlsPutChunk::exec	- Executed to start the processing of a new chunk.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.9  1999/01/29 17:48:19  nhill
// Fixed the error checking in cQlsDsGet::exec.
//
// Revision 1.8  1999/01/20 23:04:04  nhill
// Changed to wait until all notify commands are sent to the QLT before
// setting a qlsDsGet command state to done.
// Improved the error checking in cQlsDsGet::exec method.
//
// Revision 1.7  1998/04/15 22:50:10  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.6  1997/12/10 21:57:24  nhill
// Made the cQlsDsGet::config method non inline.
//
// Revision 1.5  1997/12/05 00:16:14  nhill
// Now checks to see if the last flag has been set for a dataset, and
// displays an error message if it has.
//
// Revision 1.4  1997/12/01 23:14:11  nhill
// Did some clean up.
//
// Revision 1.3  1997/11/30 21:27:46  nhill
// Changed to let the cQlsDataset class take care of its locking internally.
//
// Revision 1.2  1997/10/30 18:02:45  nhill
// Changed the cQlsDsGet::exec function to use cQlsQlt::notify instead
// of cQlsQlt::checkNotify, since the notification shouldn't depend
// on the quick look stream.
//
// Revision 1.1  1997/10/22 17:45:49  nhill
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

#include "dhs++.H"

#include "globals.H"
#include "qlsDhs.H"
#include "qlsDsCommands.H"
#include "qlsDataset.H"
#include "qlsQlt.H"

#define	checkDhs2( fn, dhsStatus, status, ret )				\
	fn;								\
	if ( dhsStatus != DHS_S_SUCCESS )				\
	{								\
	    const char *msg;						\
	    DHS_STATUS	s( DHS_S_SUCCESS );				\
	    status.E_DHS( status, #fn );				\
	    while ( s == DHS_S_SUCCESS )				\
	    {								\
		msg = cDhs::message( s );				\
		if ( s == DHS_S_SUCCESS )				\
		{							\
		    status += msg;					\
		    cDhs::messageClear( s );				\
		}							\
	    }								\
	    ret;							\
	}


char		*cQlsDsGet::qdgDataServerName = NULL;
char		*cQlsDsGet::qdgDataServerAddr = NULL;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDsAbortCmd::exec
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
// Starts the processing of an qlsDsAbort command
//
// DESCRIPTION:
// - Get the dataset name from the command attributes.
// - Find a cQlsDataset object coresponding to the dataset name.
// - Delete the cQlsDataset object.
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

void		cQlsDsAbortCmd::exec
(
    void
)
{
    cDhsAttrib	datasetAttrib;	// The dataset attribute.
    char	*datasetName;	// Name of the dataset to abort.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsDataset	*pDataset;	// The dataset to abort.
    cQlsStatus	status;


    checkDhs( response( DHS_CS_BUSY, dhsStatus ), dhsStatus, status, return );


    //
    // Get the datsaet name from the command.
    //

    datasetAttrib = find( "dataset", dhsStatus );
    checkDhs( datasetAttrib.info( (void **) &datasetName, dhsStatus ), 
	    dhsStatus, status, VOID );


    if ( status.ok() )
    {
	//
	// Find the cQlsDataset object associated with the dataset name.
	//

	pDataset = cQlsDataset::find( datasetName, &cQlsDataset::deRef );

	
	checkDhs( response( DHS_CS_DONE, dhsStatus ), 
		dhsStatus, status, return );
	status.S_DATASET_ABORT( status, datasetName );
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
// cQlsDsGet::config
//
// INVOCATION:
// cQlsDsGet::config
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the data server location information from the configuration file
//
// DESCRIPTION:
// Execute the config_get function
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with the config_open function.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsDsGet::config
(
    cQlsStatus	&status		// (mod) Function return status
)
{
    checkConfig( config_get( "dataServerId", dataServerId ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDsGet::dataServerId
//
// INVOCATION:
// n.a. Called automatically by the config library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the dataServerId keyword from the configuration file
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

boolean		cQlsDsGet::dataServerId
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cQlsStatus	status;		// Status code.


    //
    // Check the number of tokens on the line is correct.
    //

    if ( numTokens == 3 )
    {
	//
	// Free any existing data server id strings.
	//

	if ( qdgDataServerAddr != NULL )
	{
	    free( qdgDataServerAddr );
	}

	if ( qdgDataServerName != NULL )
	{
	    free( qdgDataServerName );
	}


	//
	// Save the new string.
	//

	checkNull( qdgDataServerName = (char *) strsav( tokens[1] ), status, 
		return( FALSE ) );
	checkNull( qdgDataServerAddr = (char *) strsav( tokens[2] ), status, 
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	//
	// Number of tokens on the line was wrong.
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
// cQlsDsGet::exec
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
// Starts the processing of a qlsDsGet command
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

void		cQlsDsGet::exec
(
    void
)
{
    char	*address;		// Address of the qlt.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    ulong	dims[7];		// Dimensions of the dsname attribute.
    char	*dsName;		// Name of the dataset to retrieve.
    cDhsAttrib	dsNameAttrib;		// Dataset name attribute.
    DHS_CMD_STATUS			// The status returned by the get
    		getStatus;		// request.
    char	*name;			// Imp name of the qlt.
    int		nDims;			// Number of dims of the dsname 
    cQlsDataset	*pDataset;		// Pointer to a dataset object.
    					// attribute
    cQlsQlt	*pQlt;			// Pointer to this qlt's qlt object.
    char	*responseMsg;		// The message returned by the get
    					// request.
    cQlsStatus	status;


    status.displayStop();
    dhsConnectInfo( connect(), &address, &name, &dhsStatus );


    //
    // Get the dataset name from the attributes.
    //

    checkDhs2( dsNameAttrib = find( "dataset", dhsStatus ), 
	    dhsStatus, status, goto error );
    checkDhs2( dsNameAttrib.info( nDims, dims, (void **) &dsName, dhsStatus ),
	    dhsStatus, status, goto error );


    if ( dhsStatus == DHS_S_SUCCESS )
    {
	status.S_DS_GET( status, dsName, name, address );
    }

    checkDhs2( response( DHS_CS_BUSY, dhsStatus ), 
	    dhsStatus, status, goto error );


    //
    // Open the connection to the data server.
    //

    checkDhs2( qdgDataServerConnect.open( dhsStatus ), 
	    dhsStatus, status, goto error );


    //
    // Submit the get request.
    //

    checkDhs2( qdgGet.get( qdgDataServerConnect, dsName, DHS_BD_GT_FITS, 
	    dhsStatus ), dhsStatus, status, goto error );
    checkDhs2( qdgGet.wait( dhsStatus ), dhsStatus, status, goto error );
    checkDhs2( getStatus = qdgGet.status( responseMsg, dhsStatus ), 
	    dhsStatus, status, goto error );
    if ( getStatus != DHS_CS_DONE )
    {
	status.E_DS_GET( status, dsName, responseMsg );
	goto error;
    }

    
    //
    // Close the connection to the data server.
    //

    checkDhs2( qdgDataServerConnect.close( dhsStatus ), 
	    dhsStatus, status, goto error );


    checkNull( pDataset = new cQlsDataset( dsName, false ), 
	    status, goto error );

    pDataset->chunkMerge( qdgGet.buffer(), qdgGet.length(), status );


    pQlt = cQlsQlt::find( connect(), &cQlsQlt::lock );
    if ( pQlt != NULL )
    {
	pDataset->forEachFrame( cQlsQlt::notify, pQlt, status );
	pQlt->unlock();
    }

    pDataset->deleteWait();


    if ( status.ok() )
    {
	checkDhs2( response( DHS_CS_DONE, dhsStatus ), 
		dhsStatus, status, goto error );
	return;
    }


error:
    if ( dhsStatus != DHS_S_SUCCESS )
    {
	dhsStatus = DHS_S_SUCCESS;
    }

    if ( qdgDataServerConnect.isOpen() )
    {
	qdgDataServerConnect.close( dhsStatus );
	dhsStatus = DHS_S_SUCCESS;
    }
    response( DHS_CS_ERROR, status.message(), dhsStatus );
    status.display();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsPutChunk::exec
//
// INVOCATION:
// n.a. Called automatically the the DHS put handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the processing of a new put request
//
// DESCRIPTION:
// - Find the dataset associated with the put.
// - Add the data to the dataset.
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

void		cQlsPutChunk::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsStatus	status;
    cQlsDataset	*pDataset;


    //
    // Set the command status to done immediately so the sender can continue.
    //

    checkDhs( response( DHS_CS_DONE, "Data accepted", dhsStatus ), 
	    dhsStatus, status, return );


    //
    // Display a message indicating the chunk is received.
    //

    status.S_CHUNK_REC( status, label() );


    //
    // Skip the processing if simulation is turned on.
    //

    if ( ! cQlsDhs::simulate() )
    {
	//
	// Find the dataset the data belongs in.
	//

	pDataset = cQlsDataset::findCreate( label(), &cQlsDataset::ref );


	//
	// Add the data to the dataset.
	//

	if ( pDataset != NULL )
	{
	    if ( pDataset->last() )
	    {
		status.E_DS_LAST( status, label() );
	    }
	    else
	    {
		pDataset->chunkAdd( this, status );
	    }
	    pDataset = cQlsDataset::find( label(), &cQlsDataset::deRef );
	}
    }


    //
    // Display a message indicating the processing of the chunk is complete.
    //

    status.S_CHUNK_COMP( status, label(), time( NULL ) );
}
