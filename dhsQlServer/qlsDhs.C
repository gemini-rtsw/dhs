static char rcsid[] = "$Id: qlsDhs.C,v 1.1.1.1 2002-11-24 20:29:08 brighton Exp $";
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
// dhsQlServer/src/qlsDhs.C
//
// PURPOSE:
// Contains the implementation of the cQlsDhs class, and the classes required
// to handle commands and data being received from the DHS++ library.
//
// CLASSES
// cQlsDhsError		- An object of this type is created to handle a generic 
//			  error from the DHS++ library.
// cQlsConnect		- An object of this type is created to handle a 
//			  connection message from the DHS++ library.
// cQlsDefaultCmd	- An object of this type is created to handle unknown
//			  commands.
// 
// METHOD NAME(S)
// delHandler		- Deletes a dhs handler object.
// cQlsDhsError::error	- Executed to start the processing of an error message.
// cQlsConnect::error	- Executed to start the processing of a connect message.
// cQlsDefaultCmd::exec - Executed to start the processing of an unrecognized
//			  command.
// cQlsDhs::exit	- Clean up the DHS and DHS++ libraries before exit.
// cQlsDhs::init	- Initialize the DHS and DHS++ libraries.
// cQlsDhs::start	- Start the DHS library event loop.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.13  2001/03/27 20:46:15  nhill
// Removed the call to disconnect from a subscriber from the error callback.
// This was causing recursive attempts at disconnecting.
//
// Revision 1.12  1998/08/04 23:15:42  nhill
// Fixed the message that indicates a connection is closed.
//
// Revision 1.11  1998/06/16 22:04:22  nhill
// Removed the disconnect error handler.
// Added locks protected the creation of slave QLS and QLT objects.
//
// Revision 1.10  1998/04/27 21:08:00  nhill
// Moved the disconnect processing to a common method in cQlsSubscriber.
//
// Revision 1.9  1998/04/15 22:46:42  nhill
// Updated the documentation.
// Moved the configuration routines to the dhs++ library.
// Updated to the newest version of the cDhsStatus class.
// Updated to use specific methods in sequence command handler callbacks.
//
// Revision 1.8  1997/11/30 21:26:43  nhill
// Changed to let the cQlsQlt and cQlsSlave classes take care of their list
// locking internally.
// Made some comamnds threaded.
//
// Revision 1.7  1997/11/12 18:25:48  nhill
// Added code to cQlsDisconnect::error to update the streams subscribed to
// from a master QLS, when a slave QLS disconnects.
//
// Revision 1.6  1997/10/22 17:43:23  nhill
// Checkpoint check in.
// Moved serveral of the classes to new files.
//
// Revision 1.5  1997/10/01 17:23:57  nhill
// Second alpha release.
//
// Revision 1.4  1997/08/27 22:06:45  nhill
// Changed the way the server name is generated for slave servers.
//
// Revision 1.3  1997/08/26 21:52:08  nhill
// Added a default command handler.
//
// Revision 1.2  1997/08/20 21:23:39  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
}

#include "dhs++.H"

#include "globals.H"
#include "qlsDhs.H"
#include "qlsDataset.H"
#include "qlsStream.H"
#include "qlsSlave.H"
#include "qlsQlt.H"
#include "sequenceCommands.H"
#include "qlsDsCommands.H"
#include "streamCommands.H"
#include "qlsMaster.H"


//
// Define the static data in the cQlsDhs class.
//

bool		cQlsDhs::sdRetry = false;
bool		cQlsDhs::sdSimulate = false;
extern "C" 
{ 
    long	gethostid( void ); 
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// delHandler
//
// INVOCATION:
// n.a. Called by the cDhsHandler::forEach function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pHandler (cDhsHandler *) The handler to delete.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Used to pass to the cDhsHandler::forEach function
//
// This function deletes the handler passed.
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

static void	delHandler
(
    cDhsHandler	*pHandler	// (in)  The handler to delete.
)
{
    delete pHandler;
}

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsDhsError
//
// PUBLIC METHODS: 
// (void) error - Called to start the processing of an error message.
//
// PURPOSE:
// An object of this type is created and executed each time an error 
// occurs in the DHS library. This class defines the error methods 
// originally declared as pure virtual in the cDhsError base class.
//
//-
//***********************************************************************
//

class 	cQlsDhsError: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDhsError::error
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when an error occurs in the DHS library
//
// DESCRIPTION:
// Print the error messages.
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

void		cQlsDhsError::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsStatus	status;


    //
    // If this is anything except a debug message, log it to the DHS status
    // server.
    //

    if ( this->status() != DHS_S_DISCONNECT )
    {
	if ( level() == DHS_EL_INFO || level() == DHS_EL_DEBUG )
	{
	    status.S_DHS( status, message() );
	}
	else
	{
	    status.E_DHS( status, message() );
	}
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsConnect
//
// PUBLIC METHODS: 
// (void) error - Called to start the processing of a connection message.
//
// PURPOSE:
// Object to deal with DHS library connection requests.
//
//-
//***********************************************************************
//

class 	cQlsConnect: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsConnect::error
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when a connection request is received by the DHS library.
//
// DESCRIPTION:
// Log the connection. If the connector is a QLT, create a cQlsQlt object
// associated with the connection. If the connector is a slave QLS, create
// a cQlsSlave object associated with the connection.
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

void		cQlsConnect::error
(
    void
)
{
    char	*address;		// Ip address of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    char	*name;			// Name of the client.
    cQlsQlt	*pQlsQlt;		// A new QLT object.
    cQlsStatus	status;


    //
    // Get the connection information from the DHS library.
    //

    dhsConnectInfo( connect(), &address, &name, &dhsStatus );


    //
    // Test to see if this is a quick look client.
    //

    if ( strncmp( "dhsQlc", name, strlen( "dhsQlc" ) ) == 0 )
    {
	//
	// Create a new cQlsQlt object.
	//

	cQlsQlt::listWLock();
	checkNull( pQlsQlt = new cQlsQlt( connect() ), status, VOID );
	cQlsQlt::listUnlock();


	//
	// set up "*" as the default stream.
	//

	pQlsQlt->lock();
	pQlsQlt->subscribe( "*", status );
	pQlsQlt->unlock();


	//
	// Display a message.
	//

	status.S_CLIENT_CON( status, name, address );


	//
	// If necessary, update the stream requested of the master server.
	//

	cQlsMaster::updateStreams( status );
    }
    else if ( strncmp( "qlServer.", name, strlen( "qlServer." ) ) == 0 )
    {
	//
	// Create a new cQlsSlave object.
	//

	cQlsSlave::listWLock();
	checkNull( new cQlsSlave( connect() ), status, VOID );
	cQlsSlave::listUnlock();


	//
	// Display a message.
	//

	status.S_SLAVE_CON( status, name, address );
    }
    else
    {
	//
	// Generic connection request.
	//

	status.S_CON_REQ( status, name, address );
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsDefaultCmd
//
// PUBLIC METHODS: 
// (void) exec - called to start the processing of an unknown command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles unknown command received by the dhsQlServer
//
//-
//***********************************************************************
//

class 	cQlsDefaultCmd: public cDhsServerCommand
{
    public:
	void		exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDefaultCmd::exec
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
// This function is called when an unknown command is received
//
// DESCRIPTION:
// Print a message and return.
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

void		cQlsDefaultCmd::exec
(
    void
)
{
    char	*address;		// Ip address of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    char	*clientName;		// Name of the client.
    cQlsStatus	status;


    if ( strcmp( name(), "ping" ) == 0 )
    {
	status.S_PING_CMD( status );
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	dhsConnectInfo( connect(), &address, &clientName, &dhsStatus );
	status.E_UNKNOWN_CMD( status, name(), clientName, address );

	response( DHS_CS_ERROR, "Unrecognized command", dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDhs::exit
//
// INVOCATION:
// cQlsDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of all DHS handlers and the cDhs object.
//
// DESCRIPTION:
// - Delete all error handlers created in the init function.
// - Delete the cDhs object.
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
void		cQlsDhs::exit
(
    cQlsStatus	&status		// (mod) Function return status.
)
{

    checkStat( status, return );


    //
    // Delete the error handlers.
    //

    cDhsHandler::forEach( delHandler );


    //
    // Delete the cDhs object.
    //

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDhs::init
//
// INVOCATION:
// cQlsDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the dhs library
//
// DESCRIPTION:
// - Create a new cDhs object to iterface with the DHS library.
// - Set up the error handlers.
// - Set up the command and bulk data handlers.
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

void		cQlsDhs::init
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    char	impName[32];
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    sdRetry = false;
    checkStat( status, return );


    //
    // Select a debug level for the DHS library based on the current debug
    // level of the application.
    //

    switch ( cQlsStatus::debugLevel() )
    {
	case	cQlsStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cQlsStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cQlsStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }


    //
    // generate an IMP name for this QLS.
    //

    if ( cQlsMaster::isSlave() )
    {
	//
	// gethostid isn't POSIX compliant, but I can't think of a better
	// way of doing this.
	//

	(void) sprintf( impName, "%s.%lx", cDhs::identity(), gethostid() );
    }
    else
    {
	(void) sprintf( impName, "%s", cDhs::identity() );
    }


    //
    //  Set up the dhs object.
    //


    checkNull( sdPDhs = new cDhs( impName, cDhs::numConnect(), level ), 
	    status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDhsErrorHandler<cQlsDhsError>, status, return );
    checkNull( new cDhsErrorHandler<cQlsConnect>( DHS_S_CONNECT ), 
	    status, return );


    //
    // Set up the command handlers.
    //

    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( false, "exit", 
	    &cQlsSequenceCmd::exit ), status, return );
    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( true, "test",
	    &cQlsSequenceCmd::test ), status, return );
    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( false, "init",
	    &cQlsSequenceCmd::init ), status, return );
    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( false, "reset",
	    &cQlsSequenceCmd::init ), status, return );
    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( false, "simulateLevel",
	    &cQlsSequenceCmd::simulateLevel ), status, return );
    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( false, "debugLevel",
	    &cQlsSequenceCmd::debugLevel ), status, return );
    checkNull( new cDhsCmdHandler<cQlsSequenceCmd> ( false, "resetHealth",
	    &cQlsSequenceCmd::resetHealth ), status, return );

    checkNull( new cDhsCmdHandler<cDhsAbortCmd> ( false, "cmdAbort" ), 
	    status, return );

    checkNull( new cDhsCmdHandler<cQlsClientStreamCmd> ( true, 
	    "qlsClientStream" ), status, return );
    checkNull( new cDhsCmdHandler<cQlsServerStreamCmd> ( true, 
	    "qlsServerStream" ), status, return );
    checkNull( new cDhsCmdHandler<cQlsQueryStreamsCmd> ( true, 
	    "qlsQueryStreams" ), status, return );
    checkNull( new cDhsCmdHandler<cQlsDsStreamCmd> ( true, "qlsDsStream" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cQlsDsAbortCmd> ( false, "qlsDsAbort" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cQlsDsGet> ( true, "qlsDsGet" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cQlsDefaultCmd> ( false ), 
	    status, return );


    //
    // Set up a put handler.
    //

    checkNull( new cDhsPutHandler<cQlsPutChunk> ( true ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the DHS library event loop
//
// DESCRIPTION:
// Call the cDhs::start function.
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

void		cQlsDhs::start
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    checkStat( status, return );


    checkDhs( sdPDhs->start( dhsStatus ), dhsStatus, status, return );

    dhsServerExit( &dhsStatus );
}
