static char rcsid[] = "$Id: cmdDhs.C,v 1.1.1.1 2002-11-24 20:25:28 brighton Exp $";
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
// dhsCommand/src/cmdDhs.C
//
// PURPOSE:
// Contains the definition of the cCmdDhs class
//
// This class is the interface to the dhs library for the DHS Command Server.
//
// CLASS NAME(S)
// cCmdDhsError - An object of this class is created to handle each error.
// cCmdConnect - An object of this class is created to handle each connection
//		request.
// cCmdDisconnect - An object of this class is created to handle each 
//		disconnect request.
//
// METHOD NAME(S)
// (void) delHandler - Delete a DHS++ command or error handler.
// (void) cCmdDhsError::error - Executed to start the processing of an error.
// (void) cCmdConnect::error - Executed to start the processing of a connetion
//		request.
// (void) cCmdDisconnect::error - Executed to start the processing of a 
//		disconnect request.
// (bool) cCmdServerCmd::cmdAbort - Called when a command is aborted.
// (bool) cCmdServerCmd::exec - Called to start the processing of a command.
// cCmdDhs::~cCmdDhs - Destructor for the cCmdDhs class.
// (void) cCmdDhs::exit - Clean up the cCmdDhs class before exit.
// (void) cCmdDhs::init - Initialize the cCmdDhs class.
// (void) cCmdDhs::start - Start the DHS event loop.
// (void) cCmdClientCmd::cCmdClientCmd - Constructor for the cCmdClientCmd 
//		class.
// (void) cCmdClientCmd::done - Executed when a command to a subsystem is 
//		complete.
// (void) cCmdClientCmd::response - Executed when a command to a subsystem 
//		returns a response.
// (bool) cCmdServerCmd::forward - Forward a command to a subsystem.
// (bool) cCmdServerCmd::riForward - Forward a reset or init command to a 
//		subsystem.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.15  1998/08/04 21:52:21  nhill
// Fixed a duplicate message bug.
//
// Revision 1.14  1998/06/28 22:20:07  nhill
// Fixed some bugs in the riForward function.
//
// Revision 1.13  1998/06/26 18:59:00  nhill
// Bug fixes.
//
// Revision 1.12  1998/05/21 18:45:58  jaeger
// Fixed a bug: The CMD_SEND error was getting set when it shouldn't have
// been.
//
// Revision 1.11  1998/05/21 17:31:45  jaeger
// Added the error message from the server to the error message when
// a command is sent.
//
// Revision 1.10  1998/05/21 17:02:14  jaeger
// Made better error messages when command forwarding fails.
//
// Revision 1.9  1998/04/16 18:57:23  nhill
// Updated the documentation.
// Updated to use the latest version of the cDhsStatus class.
// Moved the configuration of the cCmdDhs class to the dhs++ library.
//
// Revision 1.8  1998/03/13 22:00:12  jaeger
// Added mucho code to handle reset and init commands probably.  This required
// sequencing the order the command is sent to the subsystems.  Also needed
// to wait until subsystem indicated it was shutting down.  Also added bits
// so the command server attempts to reconnect to a subsystem before
// sending the command, if the connection has been lost.
//
// Revision 1.7  1997/10/03 23:00:52  nhill
// Added check to avoid iterative calls to the status server.
//
// Revision 1.6  1997/06/13 20:46:19  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.5  1997/05/01 23:37:51  nhill
// *** empty log message ***
//
// Revision 1.4  1997/04/28 22:53:47  nhill
// Added shutdown of client systems on exit.
//
// Revision 1.3  1997/04/24 19:50:32  nhill
// Fixed compile time error.
//
// Revision 1.2  1997/04/24 19:45:03  nhill
// Some minor changes to names & things.
//
// Revision 1.1  1997/04/16 21:50:14  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>
#include <unistd.h>

#include "genMutex.H"
#include "globals.H"

#include "genCond.H"
#include "cmdDhs.H"
#include "cmdCommandInfo.H"

cDhsHandler	*cCmdDhs::cmdDhsHandler;
bool		cCmdDhs::sdRetry = false;
bool		cCmdDhs::sdSimulate = false;
cMutex		cCmdServerCmd::cscMutex;


//
//***********************************************************************
//+
// FUNCTION NAME:
// delHandler
//
// INVOCATION:
// n.a. called by the dhs++ library cDhsHandler::forEach method.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) handler (cDhsHandler *) The handler to delete.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function deletes a handler
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
    cDhsHandler	*handler	// (in)  The handler to delete.
)
{
    delete handler;
}


//
//***********************************************************************
//+
// CLASS NAME:
// cCmdDhsError
//
// PUBLIC METHODS: 
// (void) cdeLock - Lock the codition variable.
// (void) cdeUnlock - Unlock the condition variable.
// (void) cdeBroadcast - Broadcast to the condition variable.
// (void) cdeWait - Wait for the conition variable.
// (void) shutdown - Set or get the shutdown flag.
//
// PURPOSE:
// An object of this type is created and executed each time an error 
// occurs in the DHS library
//
// This class defines the error methods originally declared as pure virtual 
// in the cDhsError base class.
//
//-
//***********************************************************************
//

class 	cCmdDhsError: public cDhsError
{
    public:
	static inline void
			cdeLock( void )
			{
				cdeCondSD.lock();
			}
	static inline void
			cdeUnlock( void )
			{
				cdeCondSD.unlock();
			}
	static inline void
			cdeBroadcast( void )
			{
				cdeCondSD.broadcast();
			}
	static inline void
			cdeWait( void )
			{
				cdeCondSD.wait();
			}
	static inline bool 
			shutdown( void )
			{
				return ( cdeShutdown );
			}
	static inline void
			shutdown( bool value )
			{
				cdeShutdown = value;
			}

    private:
	void		error( void );

	static 	bool	cdeShutdown;	// A server has shutdown?
	static	cCond	cdeCondSD;	// Cond. variable for above.
};

cCond		cCmdDhsError::cdeCondSD;
bool		cCmdDhsError::cdeShutdown = false;

    

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhsError::error
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
// If the status message indicates that a subsystem is shutting down then
// close the connection to the subsystem and broadcast that it is
// shutting down.  Otherwise print the message.
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

void		cCmdDhsError::error
(
    void
)
{
    cCmdStatus		cmdStatus;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    
    if ( status() == DHS_S_SHUTDOWN )
    {
	//
	// Disconnect from the server that is shutting down.
	//

	if ( connect() != 0 )
	{
	    dhsDisconnect( connect(), &dhsStatus );
	}

	
	//
	// If a system has shutdown then set the shutdown flag
	// and broadcast the change.
	//
	
	cdeLock();
	shutdown( true );
	cdeUnlock();
	cdeBroadcast();
    }


    //
    // If this is anything except a debug message, log it to the DHS status
    // server.
    //

    switch ( level() )
    {
	case DHS_EL_SEVERE:
	case DHS_EL_ERROR:
	case DHS_EL_WARNING :
	    if ( status() != DHS_E_LOCATE )
	    {
		cmdStatus.E_DHS( cmdStatus, message() );
	    }
	    break;
	case DHS_EL_INFO:
	case DHS_EL_DEBUG:
	    cmdStatus.S_DHS( cmdStatus, message() );
	    break;
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdConnect
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Object to deal with DHS library connection requests.
//
//-
//***********************************************************************
//

class 	cCmdConnect: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdConnect::error
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
// Log the connection.
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

void		cCmdConnect::error
(
    void
)
{
    char	*address;		// Ip address of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    char	*name;			// Name of the client.
    cCmdStatus	status;

    dhsConnectInfo( connect(), &address, &name, &dhsStatus );
    status.S_CONNECT_REQ( status, name, address );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdDisconnect
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Object to deal with DHS library disconnect requests.
//
//-
//***********************************************************************
//

class 	cCmdDisconnect: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDisconnect::error
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
// Called when a disconnect request is received by the DHS library.
//
// DESCRIPTION:
// Log the disconnect.
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

void		cCmdDisconnect::error
(
    void
)
{
    char	*address;		// Ip address of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    char	*name;			// Name of the client.
    cCmdStatus	status;

    if ( connect() != 0 )
    {
	dhsConnectInfo( connect(), &address, &name, &dhsStatus );
	status.S_DISCONNECT_REQ( status, name, address );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdServerCmd::cmdAbort
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Called when a command is aborted.
//
// DESCRIPTION:
// Set the aborted flag.
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

bool		cCmdServerCmd::cmdAbort
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    bool	ret( true );
    cCmdStatus	status;


    cscAborted = true;

    cscMutex.lock();
    if ( cscPCmdInfo != NULL && cscPCmdInfo )

    if ( cscPCmdInfo->abortable() )
    {
	if ( cscPClientCmd != NULL )
	{
	    cscPClientCmd->cmdAbort( dhsStatus );
	}
    }
    else
    {
	status.E_ABORTABLE( status, name() );
    }
    cscMutex.unlock();

    if ( dhsStatus != DHS_S_SUCCESS || !status.ok() )
    {
	ret = false;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdServerCmd::exec
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
// Called to exectute a command.
//
// DESCRIPTION:
// - Log the reception of the command.
// - Get the information describing the command.
// - Execute the command as specified in the information.
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

void		cCmdServerCmd::exec
(
    void
)
{
    char	*address;	// Address of the command sender.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Return status from the dhs library.
    char	*clientName;		// Name of the command sender.
    cCmdStatus	status;


    dhsConnectInfo( connect(), &address, &clientName, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), clientName, address );


    //
    //  Find the information for the command.
    //

    cscPCmdInfo = cCmdCommandInfo::find( name(), status );

    if ( status.ok() )
    {
	//
	// Execute the command.
	//

	cscPCmdInfo->exec( *this, status );
    }
    else
    {
	response( DHS_CS_ERROR, dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhs::~cCmdDhs
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
// Destructor for the cCmdDhs class
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

		cCmdDhs::~cCmdDhs
(
    void
)
{
    if ( sdPDhs != NULL )
    {
	delete (cDhs *) sdPDhs;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhs::exit
//
// INVOCATION:
// cCmdDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of all DHS handlers and the cDhs object.
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
void		cCmdDhs::exit
(
    cCmdStatus	&status		// (mod) Function return status.
)
{

    checkStat( status, return );

    cDhsHandler::forEach( delHandler );

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhs::init
//
// INVOCATION:
// cCmdDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the dhs library
//
// DESCRIPTION:
// - Create a cDhs object to handle the DHS connections.
// - Set up the error handlers for this application.
// - Set up the command handler for this application.
// - Set up the handler for abort commands.
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

void		cCmdDhs::init
(
    cCmdStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );

    sdRetry = false;
    checkStat( status, return );


    switch ( cCmdStatus::debugLevel() )
    {
	case	cCmdStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cCmdStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cCmdStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }


    //
    //  Set up the dhs object.
    //

    checkNull( sdPDhs = new cDhs( level ), status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDhsErrorHandler<cCmdDhsError>, status, return );
    checkNull( new cDhsErrorHandler<cCmdConnect>( DHS_S_CONNECT ), 
	    status, return );
    checkNull( new cDhsErrorHandler<cCmdDisconnect>( DHS_S_DISCONNECT ), 
	    status, return );


    //
    // Set up the command handler.
    //

    checkNull( cmdDhsHandler = new cDhsCmdHandler<cCmdServerCmd>( true ),
	status, return );


    //
    // Set up the abort handler.
    //

    checkNull( new cDhsCmdHandler<cDhsAbortCmd>( false, "cmdAbort" ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the DHS library event loop
//
// DESCRIPTION:
// - Enable processing of commands.
// - Start the event loop.
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

void		cCmdDhs::start
(
    cCmdStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    checkStat( status, return );

    enable();
    checkDhs( sdPDhs->start( dhsStatus ), dhsStatus, status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdClientCmd::cCmdClientCmd
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) serverCommand (cDhsServerCommand &) The server command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cCmdClientCmd class
//
// The constructor creates a client command based on a server command,
// and records a pointer to the server command.
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

		cCmdClientCmd::cCmdClientCmd
(
    cDhsServerCommand			// (in)  The server command.
		&serverCommand
): cDhsClientCommand( serverCommand )
{
    cccServerCommand = &serverCommand;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdClientCmd::done
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) The command status.
// (>) string (char *) The command status string.
// (>) avList (cDhsAvList &) The response av list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Callback executed when a command has competed
//
// DESCRIPTION:
// Send the av list on to the client system.
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

void		cCmdClientCmd::done
(
    DHS_CMD_STATUS,		// (in)  The command status.
    char	*string,	// (in)  The command status string.
    const cDhsAvList		// (in)  The response av list.
		&avList	
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    if ( ! avList.isNull() )
    {
	cccServerCommand->response( DHS_CS_BUSY, string, avList, dhsStatus );
    }
    cCmdDhsError::cdeBroadcast();
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdClientCmd::response
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) The command status.
// (>) string (char *) The command status string.
// (>) avList (cDhsAvList &) The response av list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Callback executed when a command receives a response
//
// DESCRIPTION:
// Send the av list on to the client system.
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

void		cCmdClientCmd::response
(
    DHS_CMD_STATUS		// (in)  The command status.
		cmdStatus,
    char	*string,	// (in)  The command status string.
    const cDhsAvList		// (in)  The response av list.
		&avList	
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    cccServerCommand->response( cmdStatus, string, avList, dhsStatus );
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdServerCmd::forward
//
// INVOCATION:
// command.forward( connection, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (cDhsConnection &) Connection to forward the command to.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// (bool) True if one of the sub-commands returns an error, false otherwise.
//
// PURPOSE:
// Forward the server command to the specified connection
//
// DESCRIPTION:
// - If the command hasn't been aborted
// -  Create a client command with the same parameters as the server command.
// -  Send the client command to the specified connection.
// -  Wait for the command to complete.
// -  Check for errors.
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

bool		cCmdServerCmd::forward
(
    cDhsConnection	// (in)  Connection to forward the command to.
		&connect,
    cCmdStatus	&status		// (mod) Function return status.
)
{
    bool	error = false;
    char	*errorMsg;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    DHS_STATUS	dhsS1( DHS_S_SUCCESS );


    checkStat( status, return( true ) );


    //
    // The mutex ensures there are not undesirable interactions between the
    // running of the sub command and command aborts.
    //

    cscMutex.lock();

    if ( ! cscAborted )
    {
	cscPClientCmd = new cCmdClientCmd( *this );
	cscPClientCmd->apply( connect, dhsStatus );

	if ( dhsStatus == DHS_E_CON_NOT_FOUND )
	{
	    //
	    // Attempt to reconnect and send the command since connection
	    // was not found.
	    //

	    dhsStatus = DHS_S_SUCCESS;
	    connect.open( dhsStatus );
	    if ( dhsStatus == DHS_S_SUCCESS )
	    {
		cscPClientCmd->apply( connect, dhsStatus ); 
		if ( dhsStatus != DHS_S_SUCCESS )
		{
		    status.E_CMD_SEND( status, "Could not forward command." );
		    status.E_DHS( status, "cscPClientCmd->apply( connect, dhsStatus )" );
		    error = true;
		}
	    }
	    else
	    {
		status.E_CONNECTING( status, connect.name(), 
			connect.address() );
		error = true;
	    }
	}
	else if ( dhsStatus != DHS_S_SUCCESS )
	{
	    const char *msg;
	    DHS_STATUS	s( DHS_S_SUCCESS );

	    status.E_CMD_SEND( status, "Could not forward command." );
	    status.E_DHS( status, "cscPClientCmd->apply( connect, dhsStatus )" );
	    while ( s == DHS_S_SUCCESS )
	    {	
		msg = cDhs::message( s );
		if ( s == DHS_S_SUCCESS )
		{
		    status += msg;
		    cDhs::messageClear( s );
		}
	    }
	    
	    delete cscPClientCmd;
	    cscPClientCmd = NULL;
	    cscMutex.unlock();
	    return( true );
	}

	//
	//  Unlock the mutex and wait for the command to complete.
	//

	cscMutex.unlock();
 	if ( !error )
	{
	    cscPClientCmd->wait( dhsStatus );
	

	    //
	    // check the function return status.
	    //

	    if ( cscPClientCmd->status( dhsS1 ) != DHS_CS_DONE &&
		    status.ok() )
	    {
		cscPClientCmd->status( errorMsg, dhsS1 );
		status.E_CMD_SEND( status, errorMsg );
		error = true;
	    }
	}


	//
	// Lock the mutex again to ensure another thread cannot attempt to
	// abort the command as it is being deleted.
	//

	cscMutex.lock();


	//
	// Delete the sub command object.
	//

	delete cscPClientCmd;
	cscPClientCmd = NULL;
    }

    cscMutex.unlock();
    

    return( error );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdServerCmd::riForward
//
// INVOCATION:
// command.riForward( connection, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (cDhsConnection &) Connection to forward the command to.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// (bool) True if one of the sub-commands returns an error, false otherwise.
//
// PURPOSE:
// Forward the reset or init command to the specified connection
//
// DESCRIPTION:
// Send the "reset" or "init" command to the specified connection
// (subsystem).  Then wait until we receive the "shutdown" message
// from the subsystem.  Once the message has been received wait for
// the "reset" or "init" command to complete, then delete the command.
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

bool		cCmdServerCmd::riForward
(
    cDhsConnection		// (mod)  Connection to forward the command to.
		&connect,
    cCmdStatus	&status		// (mod) Function return status. 
)
{
    bool	error;
    char	*errorMsg;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    checkStat( status, return( true ) );

    //
    // The mutex ensures there are not undesirable interactions between the
    // running of the sub command and command aborts.
    //

    cscMutex.lock();
    error = false;
    if ( ! cscAborted )
    {

	cscPClientCmd = new cCmdClientCmd( *this );


	//
	// Make sure shutdown is set to false before the command is executed.
	//

	cCmdDhsError::cdeLock();
	cCmdDhsError::shutdown( false );
	cCmdDhsError::cdeUnlock();
    
	cscPClientCmd->apply( connect, dhsStatus );
	if ( dhsStatus == DHS_E_CON_NOT_FOUND )
	{
	    //
	    // Attempt to reconnect and send the command since connection
	    // was not found.
	    //

	    dhsStatus = DHS_S_SUCCESS;
	    connect.open( dhsStatus );
	    if ( dhsStatus == DHS_S_SUCCESS )
	    {
		cscPClientCmd->apply( connect, dhsStatus );
	    }
	    else
	    {
		status.E_CONNECTING( status, connect.name(), 
			connect.address() );
		error = true;
	    }
	}
	


	//
	//  Unlock the mutex and wait for the subsystem to disconnect
	// from the command server.
	//

	cscMutex.unlock();


	//
	// Wait for the subsystem to shutdown and close the connection.
	//
	//
	cCmdDhsError::cdeLock();
	while (  status.ok() && ( dhsStatus == DHS_S_SUCCESS &&
		( ! cscPClientCmd->isDone( dhsStatus ) ||
		cscPClientCmd->status( dhsStatus ) == DHS_CS_DONE ) ) &&
		! cCmdDhsError::shutdown() )
	{
	    cCmdDhsError::cdeWait();
	}
	cCmdDhsError::cdeUnlock();



	//
	// Lock the mutex again to ensure another thread cannot attempt to
	// abort the command as it is being deleted or while we are
	// waiting for it to complete.
	//

	cscMutex.lock();


	//
	// Wait for the completion of the reset command.
	//

	cscPClientCmd->wait( dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS && status.ok() )
	{
	    error = true;
	    status.E_CMD_APPLY( status, dhsStatus );
	}
	else if ( cscPClientCmd->status( dhsStatus ) != DHS_CS_DONE &&
    		status.ok() )
	{
	    cscPClientCmd->status( errorMsg, dhsStatus );
	    status.E_CMD_SEND( status, errorMsg );
	    error = true;
	}



	//
	// Delete the sub command object and close the connection.
	//

	delete cscPClientCmd;
	cscPClientCmd = NULL;
    }

    cscMutex.unlock();
    

    return( error );
}
