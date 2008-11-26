static char rcsid[] = "$Id: qlsStream.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsQlServer/src/qlsStream.C
//
// PURPOSE:
// Contains the implementation of the cQlsStream class
//
// METHOD NAME(S)
// cQlsStream::cQlsStream - Constructor for the cQlsStream class.
// cQlsStream::~cQlsStream - Destructor for the cQlsStream class.
// cQlsStream::addStreams - Add all streams to a streams attribute in a 
//		DHS library av list.
// cQlsStream::config - Read the default streams from the configuration file.
// cQlsStream::exit - Delete all cQlsStream objects.
// cQlsStream::find - Find a stream by stream name.
// cQlsStream::qlStream - Process a qlStream line from the configuration file.
// cQlsStream::subscribe - Add a new cQlsSubscriber object to the subscriber 
//		list for this stream.
// cQlsStream::unSubscribe - Unsubscribe the specified subscriber.
// cQlsSubscriber::~cQlsSubscriber - Destructor for the cQlsSubscriber class.
// cQlsSubscriber::connectCallback - Called when the connection state changes.
// cQlsSubscriber::disconnect - Called when a subscriber disconnects.
// cQlsSubscriber::subscribe - Subscribe the subscriber to the specified stream.
// cQlsSubscriber::test - Test the subscriber and its connections.
// cQlsSubscriber::unSubscribeAll - Unsubscribe all streams from the
//		subscriber.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:29:26  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2001/03/27 20:52:14  nhill
// Fixed bugs when disconnecting clients.
//
// Revision 1.7  1998/06/16 22:10:41  nhill
// Added a callback routine to disconnect a subscriber when the connection
// state changes.
// Added locks to protect the list of subscribers when a subscriber is
// deleted.
//
// Revision 1.6  1998/04/27 21:11:01  nhill
// Added methods cQlsSubscriber::test and cQlsSubscriber::disconnect.
//
// Revision 1.5  1998/04/15 23:03:11  nhill
// Updated to the newest version of the cDhsStatus class.
// Updated the documentation.
//
// Revision 1.4  1997/11/30 21:32:02  nhill
// Added the forQls argument to the addStreams method.
//
// Revision 1.3  1997/10/22 17:45:23  nhill
// Checkpoint check in.
//
// Revision 1.2  1997/08/20 21:24:51  nhill
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
#include "qlsStream.H"
#include "qlsQlt.H"
#include "qlsDataset.H"
#include "qlsSlave.H"
#include "qlsMaster.H"


//
// Define the static data members for the cQlsStream class.
//

cRwLock		cQlsStream::qlsRwLock;
cQlsStream::tStreamList	
		cQlsStream::qlsStreamList;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::cQlsStream
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the new stream.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cQlsStream class
//
// DESCRIPTION:
// Copy the stream name to the qlsName data member. Add the new object to
// the list of all cQlsStream objects.
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

		cQlsStream::cQlsStream
(
    const char	*name		// (in)  The name of the new stream.
)
{
    qlsName = strsav( name );
    qlsRefCount = 0;
    qlsPubRefCount = 0;


    //
    //  Add the stream to the list of streams.
    //
    
    qlsStreamList[std::string( name )] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::~cQlsStream
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
// Destructor for a cQlsStream object
//
// DESCRIPTION:
// Remove the object from the list of all cQlsStream objects.
// Free memory allocated to the object.
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

		cQlsStream::~cQlsStream
(
    void
)
{
    iStreamList	i;


    //
    // Find the objects and remove it from the list of all cQlsStream objects.
    //

    i = qlsStreamList.find( qlsName );
    if ( i != qlsStreamList.end() )
    {
	qlsStreamList.erase( i );
    }


    //
    // Free the memory allocated the the qlsName data member.
    //

    gen_free( qlsName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::addStreams
//
// INVOCATION:
// cQlsStream::addStreams( avList, true, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) avList (cDhsAvList &) The avlist to modify.
// (>) forQls (bool) Is the list to be sent to a master QLS?
// (!) status (cQlsStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a "streams" attribute to an av list containing the list of all stream
// names
//
// DESCRIPTION:
// - Create an array of strings.
// - Populate the array of strings with the stream names from the qlsStreamList
//   list of streams.
// - Put a streams attribute into the av list, with the stream names as the
//   attribute value.
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

void		cQlsStream::addStreams
(
    cDhsAvList	&avList,	// (mod) The avlist to modify.
    bool	forQls,		// (in)  Is the list to be sent to a master
    				//	 QLS?
    cQlsStatus	&status		// (mod) The function return status.
)
{
    iStreamList	i;
    int		j;
    ulong	numStreams;	// Number of streams in qlsStreamList.
    cQlsStream	*p;		// The stream being examined.
    const char	**streamArray;	// An array of stream names.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat(status, return );


    //
    // Everything is done with the list locked to ensure no other thread
    // is modifying the list  of streams while we are looking at it.
    //

    listRLock();


    //
    //  Get the number of streams in the list.
    //

    numStreams = qlsStreamList.size();


    //
    // Allocate an array of string pointers large enough for all of the
    // stream names.
    //

    checkNull( streamArray = new const char * [numStreams], status, return );


    //
    // Put the pointer to each stream name into the array of stream names.
    //

    for ( i = qlsStreamList.begin(), j = 0; i != qlsStreamList.end(); i++ )
    {
	p = (*i).second;
	if ( ( ! forQls && p->qlsPubRefCount > 0 ) ||
		( forQls && p->qlsSubscriberList.size() > 0 ) )
	{
	    streamArray[j] = p->name();
	    j++;
	}
    }
    numStreams = j;


    //
    // Create a new attribute in the av list with the stream names as its
    // value.
    //

    checkDhs( avList.add( "streams", DHS_DT_STRING, 1, &numStreams, 
	    streamArray, dhsStatus ), dhsStatus, status, return );
    listUnlock();

    delete streamArray;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::config
//
// INVOCATION:
// cQlsStream::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the default set of quick look streams from the configuration file
//
// DESCRIPTION:
// Use the config_get function to parse all qlStream lines in the configuration
// file.
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

void		cQlsStream::config
(
    cQlsStatus	&status	// (mod) Function return status.
)
{
    checkStat( status, return );

    checkConfig( config_get( "qlStream", qlStream ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::exit
//
// INVOCATION:
// cQlsStream::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all cQlsStream objects
//
// DESCRIPTION:
// Free each cQlsStream object in the list of all cQlsStream objects.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function assumes all cQlsStream objects are allocated on the heap.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsStream::exit
(
    cQlsStatus	&		// (mod) Function return status.
)
{
    iStreamList	i;


    for ( i = qlsStreamList.begin(); i != qlsStreamList.end(); 
	    i = qlsStreamList.begin() )
    {
	delete (*i).second;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::find
//
// INVOCATION:
// pStream = find( "stream name", status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) streamName (const char *) Name of the stream to find.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// (cQlsStream *) Pointer to the stream found.
//
// PURPOSE:
// Locates a quick look stream given its name
//
// If the stream cannot be found, a new one is created.
//
// DESCRIPTION:
// - Use the STL map::find function to attemp to find the cQlsStream object.
// - If the object is not found, create a new one.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function should be called after locking the class mutex with the
// cQlsStream:::listWLock function.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cQlsStream	*cQlsStream::find
(
    const char	*streamName,	// (in)  Name of the stream to find.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    iStreamList	i;		// Iterator into the list of streams.
    cQlsStream	*pStream;	// Pointer to the stream to return.


    //
    // Try to find an existing stream in the list of streams.
    //

    if ( ( i = qlsStreamList.find( std::string( streamName ) ) ) == 
	    qlsStreamList.end() )
    {
	//
	// The stream could not be found, create a new one.
	//

	checkNull( pStream = new cQlsStream( streamName ), 
		status, return( NULL ) );
    }
    else
    {
	//
	// A stream was found, return a pointer to it.
	//

	pStream = (*i).second;
    }

    return( pStream );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::qlStream
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (boolean) True if the function succeeded, false otherwise.
//
// PURPOSE:
// Parse a qlStream line from the configuration file
//
// This function is called once for each qlStream line in the file.
//
// DESCRIPTION:
// Get the stream name from the second token, and create a new cQlsStream
// object with that name.
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

boolean		cQlsStream::qlStream
(
    int		numTokens,	// (in)  Number of tokens on the configuration 
				//	 line.
    char	**tokens	// (in)  Tokens on the configuration line.
)
{
    boolean	ret;		// The return value.
    cQlsStatus	status;		// Status code.
    cQlsStream	*pStream;	// The newly created stream.


    //
    // check the format of the config line.
    //

    if ( numTokens == 2 )
    {
	//
	// Create a new cQlsStream object.
	//

	pStream = new cQlsStream( tokens[1] );


	//
	// Incrementing the reference counter on the new stream ensures
	// it won't be deleted until all qlsStream objects are deleted.
	//

	pStream->ref();
	ret = TRUE;
    }
    else
    {
	//
	// The configuration line was incorrect, display an error.
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
// cQlsStream::subscribe
//
// INVOCATION:
// stream.subscribe( pQlt, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) subscriber (cQlsSubscriber &) The QLT to add to the subscriber list.
// (<) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a new cQlsSubscriber object to the subscriber list for this stream
//
// DESCRIPTION:
// Add the stream to the subscribers subscriber list.
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

void		cQlsStream::subscribe
(
    cQlsSubscriber		// (in)  The subscriber to add to the 
		&subscriber,	//	 subscriber list.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    listWLock();
    qlsSubscriberList[ subscriber.connect().dhsConnect() ] = &subscriber;
    ref( false );
    listUnlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsStream::unSubscribe
//
// INVOCATION:
// stream.unSubscribe( subscriber, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) subscriber (cQlsSubscriber &) The subscriber to remove.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Unsubscribe the specified subscriber from this stream
//
// DESCRIPTION:
// Attempt to find the subscriber in the list of subscriber's, and if 
// found, remove it. It is not an error if the qlt is not in the list.
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

void		cQlsStream::unSubscribe
(
    cQlsSubscriber		// (in)  The cQlsQlt object to unsubscribe.
		&subscriber,
    cQlsStatus	&status		// (mod) Function return status.
)
{
    iSubscriberList	
		i;

    checkStat( status, return );


    listWLock();
    i = qlsSubscriberList.find( subscriber.connect().dhsConnect() );
    if ( i != qlsSubscriberList.end() )
    {
	qlsSubscriberList.erase( i );
	if ( deRef( false ) == 0 )
	{
	    delete this;
	}
    }
    listUnlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSubscriber::~cQlsSubscriber
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
// Destructor for the cQlsSubscriber class
//
// DESCRIPTION:
// Unsubscribe all subscribed streams.
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

		cQlsSubscriber::~cQlsSubscriber
(
    void
)
{
    cQlsStatus	status;

    //
    // Disable the connection state change callbacks.
    //

    qsConnection.callbackSet( NULL, NULL );

    unSubscribeAll( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSubscriber::connectCallback
//
// INVOCATION:
// N.A. Called automatically when the connection state changes.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pConnect (cDhsConnect *) The connection that changed state.
// (!) paramName (float) Descripton of the parameter
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called whenever the connection to a subscriber changes
// state
//
// The function ensures the subscriber object is deleted when the connection is
// lost.
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

void		cQlsSubscriber::connectCallback
(
    cDhsConnection		// (in)  The connection to the subscriber.
		*pConnect,
    void	*ptrSubscriber	// (mod) Pointer to the cQlsSubscriber object.
)
{
    cQlsStatus	status;
    cQlsSubscriber
		*pSubscriber( (cQlsSubscriber *) ptrSubscriber );


    if ( ! pConnect->isOpen() )
    {
	pSubscriber->disconnect( pConnect->dhsConnect(), status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSubscriber::disconnect
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) The connection being closed.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when a disconnect request is received by the DHS library.
//
// DESCRIPTION:
// Log the disconnect and remove any cQlsQlt or cQlsSlave object associated
// with the connection.
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

void		cQlsSubscriber::disconnect
(
    DHS_CONNECT	connect,		// (in)  The connection closed.
    cQlsStatus	&status			// (mod) Function return status.
)
{
    char	*address;		// Ip address of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    char	*name;			// Name of the client.
    cQlsQlt	*pQlt;			// Pointer to a cQlsQlt object.
    cQlsSlave	*pSlave;		// Pointer to a cQlsSlave object.
    bool	exitNow;

    if ( connect != 0 )
    {
	lock();
	if ( qsDeleting )
	{
	    exitNow = true;
	}
	else
	{
	    exitNow = false;
	}
	qsDeleting = true;
	unlock();
	if ( exitNow )
	{
	    return;
	}


	//
	// Get the connection information from the DHS library.
	//

	dhsConnectInfo( connect, &address, &name, &dhsStatus );


	//
	// Test to see if this is a quick look client.
	//

	if ( strncmp( "dhsQlc", name, strlen( "dhsQlc" ) ) == 0 )
	{
	    //
	    // Find and delete the cQlsQlt object associated with the 
	    // connection.
	    //

//	    pQlt = cQlsQlt::find( connect );


	    if ( this != NULL )
	    {
		cQlsQlt::listWLock();
		delete this;
		cQlsQlt::listUnlock();
	    }


	    //
	    // If necessary, update the stream requested of the master server.
	    //

	    cQlsMaster::updateStreams( status );


	    //
	    // Check for errors.
	    //

	    if ( status.ok() )
	    {
		status.S_CLIENT_DISCON( status, name, address );
	    }


	}
	else if ( strncmp( "qlServer", name, strlen( "qlServer" ) ) == 0 )
	{
	    //
	    // This is a slave QL server, so find and delete the cQlsSlave 
	    // object associated with the connection.
	    //

	    pSlave = cQlsSlave::find( connect );
	    if ( pSlave != NULL )
	    {
		cQlsSlave::listWLock();
		delete pSlave;
		cQlsSlave::listUnlock();
	    }


	    //
	    // If necessary, update the stream requested of the master server.
	    //

	    cQlsMaster::updateStreams( status );


	    //
	    // Check for errors.
	    //

	    if ( status.ok() )
	    {
		status.S_SLAVE_DISCON( status, name, address );
	    }
	}
	else
	{
	    status.S_DISCON_REQ( status, name, address );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSubscriber::subscribe
//
// INVOCATION:
// subscriber.subscribe( streamName, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) streamName (char *) Name of the stream to subscribe to.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Subscribe a subscriber to the specified stream
//
// DESCRIPTION:
// - Find the cQlsStream object coresponding to the stream name.
// - Subscribe to the stream.
// - Add the stream to the subscribers list of streams.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function should be called with the subscriber mutex locked.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void          cQlsSubscriber::subscribe
(
    char	*streamName,    // (in)  Name of the stream to subscribe to.
    cQlsStatus  &status         // (mod) Function return status.
)
{
    cQlsStream  *pStream;


    checkStat( status, return );


    //
    //  Find a stream object with the specified name.
    //

    check( pStream = cQlsStream::find( streamName, status ), status, return );

    pStream->subscribe( *this, status );
    qsStreamList[streamName] = pStream;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSubscriber::test
//
// INVOCATION:
// cQlsQlt::forEach( test, void, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>)  (void *) Dummy parameter to make this function compatable with the
//		foreach function.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Test the connection to the subscriber
//
// DESCRIPTION:
// - Send a ping command to the subscriber.
// - Send a test command to the subscriber.
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

void		cQlsSubscriber::test
(
    void	*,		// (in)  Dummy argument
    cQlsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*message;	// Message returned by the test command.
    cDhsClientCommand
    		ping( "ping" );
    DHS_CMD_STATUS		// Command state returned by the ping command.
    		state;
    cDhsClientCommand		// Test command to send to the slave.
    		test( "test" );


    //
    // Send a ping command to the subscriber.
    //

    ping.apply( qsConnection, dhsStatus );
    ping.wait( dhsStatus );
    if ( dhsStatus != DHS_S_SUCCESS )
    {
	status.E_CONNECTION( status, connect().name(), connect().address() );
    }


    //
    // Send a test command to the subscriber.
    //

    test.apply( connect(), dhsStatus );
    checkDhs( test.wait( dhsStatus ), dhsStatus, status, return );

    state = test.status( message, dhsStatus );
    if ( state != DHS_CS_DONE )
    {
	status.E_SLAVE_TEST( status, connect().name(), connect().address(),
		message );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSubscriber::unSubscribeAll
//
// INVOCATION:
// subscriber.unSubscribeAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Unsubscribe the subscriber from all streams
//
// DESCRIPTION:
// Unsubscribe the subscriber from all existing streams.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function should be called with the subscriber mutex locked.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsSubscriber::unSubscribeAll
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cQlsStream::iStreamList	i;


    checkStat( status, return );

    for ( i = qsStreamList.begin(); i != qsStreamList.end(); 
	    i = qsStreamList.begin() )
    {
	((*i).second)->unSubscribe( *this, status );
	qsStreamList.erase( qsStreamList.begin() );
    }
}
