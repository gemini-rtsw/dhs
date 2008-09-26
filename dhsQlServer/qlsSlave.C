static char rcsid[] = "$Id: qlsSlave.C,v 1.1.1.1 2002-11-24 20:29:23 brighton Exp $";
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
// dhsQlServer/src/qlsSlave.C
//
// PURPOSE:
// Contains the implementation of the cQlsSlave class
//
// METHOD NAME(S)
// cQlsSlave::cQlsSlave - Constructor for the cQlsSlave class.
// cQlsSlave::~cQlsSlave - Destructor for the cQlsSlave class.
// cQlsSlave::checkForward - Check to see if a dataset should be forwarded
//		to the slave QLS.
// cQlsSlave::dsStream - Send a qlsDsStream command to a slave QLS.
// cQlsSlave::forward - Forward a dataset to a slave QLS.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/04/15 23:00:48  nhill
// Updated to the newest version of the cDhsStatus class.
// Updated the documentation.
//
// Revision 1.3  1997/11/30 21:31:03  nhill
// Changed to use the cListOf class to keep a list of all cQlsSlave objects.
//
// Revision 1.2  1997/10/30 18:06:04  nhill
// Fixed a typo.
//
// Revision 1.1  1997/08/20 21:25:19  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_GET
#include "dhs++.H"

#include "globals.H"
#include "qlsDataset.H"
#include "qlsStream.H"
#include "qlsSlave.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSlave::cQlsSlave
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (DHS_CONNECT) The connection associated with the slave.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a new slave connection
//
// DESCRIPTION:
// Add the new slave to the list of all slaves.
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

		cQlsSlave::cQlsSlave
(
    DHS_CONNECT	connection	// (in)  The connection associated with the 
				//		slave.
):	cQlsSubscriber( connection ),
	cListOf< cQlsSlave, DHS_CONNECT, cQlsStatus >( connection )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSlave::~cQlsSlave
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
// Destructor for the cQlsSlave class
//
// Unsubscribes all streams and removes the slave from the list of slaves.
//
// DESCRIPTION:
// - Unsubscribe All streams subscribe by the slave.
// - Remove the object from the list of all cQlsSlave objects.
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

		cQlsSlave::~cQlsSlave
(
    void
)
{
    cQlsStatus	status;


    //lock();


    //
    // Unsubscribe all subscribed streams.
    //

    unSubscribeAll( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSlave::checkForward
//
// INVOCATION:
// slave.checkForward( arg, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pArg (void *) The argument passed to the forEach function.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Check to see if a dataset should be forwarded to the Quick Look Slave
//
// If the slave requires the dataset, the dataset will be forwarded.
// This funtion is intended to be called by the forEach function.
//
// DESCRIPTION:
// - Check each stream subscribed by the slave QLT.
// - If a stream is also assigned to the dataset, forward the data to the slave.
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

void		cQlsSlave::checkForward
(
    void	*pArg,		// (in)  The argument passed to the forEach
    				//	 function.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cQlsStream::iStreamList
    		i;
    tCfArg	*pCfArg( (tCfArg *) pArg );
    				// This is the argument passed to the forEach
				// function. It allows two pointers to be
				// passed, using the STL pair class.
    cQlsDataset	*pDataset;	// The dataset being examined.
    cDhsPutRequest		// Put request originally received.
		*pPutRequest;
    cQlsStream	*pStream;	// A stream subscribed by the slave.


    checkStat( status, return );


    //
    // Get the dataset and put request pointers from the "pArg" argument.
    //

    pDataset = pCfArg->first;
    pPutRequest = pCfArg->second;


    //
    // Everything else is done with the mutex locked to ensure the list
    // of streams is not changing as we look at it.
    //

    lock();


    //
    // check each stream subscribed by the slave.
    //

    for ( i = streamList().begin(); i != streamList().end(); i++ )
    {
	pStream = (*i).second;


	//
	// Check to see if the stream is associated with the dataset.
	//

	if ( strcmp( pStream->name(), "*" ) == 0 || 
		pDataset->hasStream( *pStream ) )
	{
	    //
	    // The stream is associated with the dataset, so forward the
	    // data to the slave.
	    //

	    dsStreams( *pDataset, status );
	    forward( pPutRequest, status );
	    break;
	}
    }
    unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSlave::dsStreams
//
// INVOCATION:
// slave.dsStreams( dataset, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataset (cQlsDataset &) The dataset being processed.
// (>) status (cQlsStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a qlsDsStreams command to a slave qls, notifying it of the
// streams assigned to a dataset
//
// DESCRIPTION:
// - Create a command object.
// - Add the dataset streams to the command object.
// - Add the dataset name to the command object.
// - Send the command.
// - Wait for the command to complete.
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

void		cQlsSlave::dsStreams
(
    cQlsDataset	&dataset,	// (in)  The dataset being processed.
    cQlsStatus	&status		// (in)  The function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cDhsClientCommand		// Command to send to the slave QLS.
		dsStreamCmd( "qlsDsStream" );


    checkStat( status, return );


    dataset.rLock();
    dataset.streamAdd( dsStreamCmd, status );
    dataset.unlock();

    dsStreamCmd.add( "dataset", DHS_DT_STRING, dataset.datasetName(), 
	    dhsStatus ); 

    dsStreamCmd.apply( connect(), dhsStatus );
    checkDhs( dsStreamCmd.wait( dhsStatus ), dhsStatus, status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSlave::forward
//
// INVOCATION:
// slave.forward( pDataset, status * );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pPutRequest (cQlsPutRequest *) Pointer to the dataset to forward.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Forward a dataset to a slave quick look server
//
// DESCRIPTION:
// Create a bulk data put containing the data and send it to the slave.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// This function waits for the put to complete. It would be more efficient
// to send the data without waiting.
//-
//***********************************************************************
//

void		cQlsSlave::forward
(
    cDhsPutRequest		// (in)  The original put request.
    		*pPutRequest,
    cQlsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );	
				// DHS function return status.
    cDhsPutDataset		// The dataset to be sent.
    		dataset( *pPutRequest );


    checkStat( status, return );


    //
    // Send the data.
    //

    checkDhs( dataset.put( connect(), pPutRequest->label(), 
	    pPutRequest->last(), dhsStatus ), dhsStatus, status, return );


    //
    // Wait for the put to complete.
    //

    checkDhs( dataset.wait( dhsStatus ), dhsStatus, status, return );
    status.S_QLS_NOTIFY( status, connect().name(), pPutRequest->label() );
}
