static char rcsid[] = "$Id: staDhs.C,v 1.1.1.1 2002-11-24 20:30:41 brighton Exp $";
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
// dhsStatus/src/staDhs.C
//
// PURPOSE:
// Contains the ICD1c interface stuff for the status server.
//
// CLASSES
// cStaDhsError - Class to handle DHS library errors.
// cStaDefaultCmd - An object of this type is created to handled unkown 
//		commands.
// cStaEventLog - Object created to handle event log commands.
// cStaSequenceCmd - Object created to handle sequence commands.
// cStaUpdate - Object created to handle update commands.
// cStaGetStatus - Object created to handle staGetStatus commands.
//
// METHOD NAME(S)
// cStaDhsError::error - This function is called to announce an error.
// cStaDefaultCmd::exec - This function is called to start the processing
//		of an unkown command.
// cStaEventLog::exec - This function is called to start the processing
//		of an eventLog command.
// cStaSequenceCmd::debugLevel - This function is called to start the processing
//		of a debugLevel command.
// cStaSequenceCmd::exit - This function is called to start the processing
//		of an exit command.
// cStaSequenceCmd::flush - This function is called to start the processing
//		of a flush command.
// cStaSequenceCmd::init - This function is called to start the processing
//		of an init or reset command.
// cStaSequenceCmd::ping - This function is called to start the processing
//		of a ping command.
// cStaSequenceCmd::resetHealth - This function is called to start the 
//		processing of a resetHealth command.
// cStaSequenceCmd::simulateLevel - This function is called to start the 
//		processing of a simulateLevel command.
// cStaSequenceCmd::test - This function is called to start the processing of 
//		a test command.
// cStaUpdate::exec - This function is called to start the processing of 
//		a statusUpdate command.
// cStaGetStatus::exec - This function is called to start the processing of 
//		a staGetStatus command.
// cStaDhs::~cStaDhs - The destructor for the cStaDhs class.
// cStaDhs::start - Start the DHS event loop.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.20  1999/05/20 22:59:23  nhill
// Added the staGetStatus command.
//
// Revision 1.19  1998/06/24 23:19:01  nhill
// Fixed some bugs.
//
// Revision 1.18  1998/06/22 22:30:41  nhill
// Fixed a bug which caused an infinite loop when debug full was on.
//
// Revision 1.17  1998/06/19 18:07:04  nhill
// Changed the cStaStatus class name to cStaStat.
// Added some message logging.
// Changed to update status values via the cStaStat class.
//
// Revision 1.16  1998/06/16 22:20:22  nhill
// Added some error checking.
//
// Revision 1.15  1998/06/02 19:08:08  nhill
// Added code to wait until all clients disconnect before exiting.
//
// Revision 1.14  1998/05/20 04:57:48  jaeger
// Added appropriate calls to the staMon::simulate method.
//
// Revision 1.13  1998/05/12 20:24:18  nhill
// Updated the documentation.
// Added commands to allow dhsService subscribers.
//
// Revision 1.12  1998/04/15 21:07:55  nhill
// Updated the documentation.
// Updated to the newest version of the cDhsStatus class.
// Changed to use command specific callback routines from the
// dhs++ library.
// Removed the DHS library configuration functions.
//
// Revision 1.11  1998/03/06 19:22:35  jaeger
// Added a paramter to staChannel::put
//
// Revision 1.10  1998/02/17 00:20:05  jaeger
// Moved cStaMon::stop() out and put refusal of connections until initialization
// is complete in.
//
// Revision 1.9  1998/02/05 21:20:32  jaeger
// Added bits for monitoring resources.  Added test command.
//
// Revision 1.8  1998/01/09 19:23:34  nhill
// Added a call to dhsServerExit.
//
// Revision 1.7  1998/01/08 20:49:28  jaeger
// add resetHealth command.
//
// Revision 1.6  1997/09/18 20:48:40  nhill
// Added a ping command.
//
// Revision 1.5  1997/06/13 20:41:22  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.4  1997/04/28 22:56:33  nhill
// Removed debugging output.
//
// Revision 1.3  1997/04/24 20:05:04  nhill
// Added a const keyword.
//
// Revision 1.2  1997/04/16 22:28:55  nhill
// Changed to use the handler list to delete the handlers instead of saving
// specific pointers.
//
// Revision 1.1  1997/03/27 22:15:42  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <unistd.h>

#include "dhs.h"
#include "globals.H"

extern "C"
{
#include "gen_config.h"
}


#include "staDhs.H"
#if defined(EPICS_DHS)
#include "staChannel.H"
#endif
#include "staMonitor.H"    
#include "subscriber.H"

bool	cStaDhs::sdIsInit = false;
bool	cStaDhs::sdRetry = false;
bool	cStaDhs::sdSimulate = false;


static boolean	allClosed( void * );
static void	delHandler( cDhsHandler *handler )
{
    delete handler;
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaDhsError
//
// PUBLIC METHODS: 
// None.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Object to deal with DHS library errors
//
//-
//***********************************************************************
//

class 	cStaDhsError: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaDhsError::error
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

void		cStaDhsError::error 
(
    void
)
{
    cDhsConnection	*CDConnection;	// Dhs Connection.
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    					// DHS function return value.
    char		*address;
    char		*name;
    cStaStat		staStatus;	// Function return value.

    
    if ( status() == DHS_S_CONNECT )
    {
	dhsConnectInfo( connect(), &address, &name, &dhsStatus );
	staStatus.S_CONNECT( staStatus, name, address );
	if ( ! cStaDhs::isInit() && connect() != 0 )
	{
	    //
	    // The system is not initialized so refuse the connection.
	    //

	    CDConnection = new cDhsConnection( connect() );
	    CDConnection->close( dhsStatus );
	    delete ( cDhsConnection*)CDConnection;
	}
    }
    else if ( status() == DHS_S_DISCONNECT )
    {
	dhsConnectInfo( connect(), &address, &name, &dhsStatus );
	staStatus.S_DISCONNECT( staStatus, name, address );
    }
    else
    {
	staStatus.msgDisplay( message() );
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaDefaultCmd
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Default command handler
//
//-
//***********************************************************************
//

class 	cStaDefaultCmd: public cDhsServerCommand
{
    public:
    protected:
    private:
	void		exec( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaDefaultCmd::exec
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
// Created to handle unknown commands
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

void		cStaDefaultCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Return status from the dhs library.
    cStaStat	status;


    status.E_INVALID_CMD( status, name() );
    response( DHS_CS_ERROR, dhsStatus );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaEventLog
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handler for an event log command.
//
//-
//***********************************************************************
//

class 	cStaEventLog: public cDhsServerCommand
{
    public:
    protected:
    private:
	void		exec( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaEventLog::exec
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
// Call to handle an event log request.
//
// DESCRIPTION:
// - Extract the log message from the av list.
// - Set the command state to busy.
// - Send the log message to the log channel.
// - Set the command state to done.
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

void		cStaEventLog::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Return status from the dhs library.
    cDhsAttrib	av;		// The message string attribute.
    char	*message;	// The message to log.
    const char	*str;
    cStaStat	status;
    DHS_STATUS	st;
    DHS_ERR_LEVEL el;
    

    if ( cStaDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	//
	// Extract the log message from the av list.
	//

	av = find( "message", dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS )
	{
	    //
	    // Set the command state to busy.
	    //

	    response( DHS_CS_BUSY, dhsStatus );
	    if ( dhsStatus != DHS_S_SUCCESS )
	    {
		dhsStatus = DHS_S_SUCCESS;
	    }


	    //
	    // Send the message to the log message channel.
	    //

	    av.info( (void **) &message, dhsStatus );
#if defined(EPICS_DHS)
	    cStaChannel::logMessage( message, status );
#endif


	    //
	    // Set the command state to done.
	    //

	    response( DHS_CS_DONE, dhsStatus );
	    if ( dhsStatus != DHS_S_SUCCESS )
	    {
		status.E_DHS( status, "" );
		for ( DHS_STATUS s1( DHS_S_SUCCESS ); s1 == DHS_S_SUCCESS; 
			str = cDhs::message( st, el, s1 ) )
		{
		    status += str;
		    cDhs::messageClear( s1 );
		}
	    }
	}
	else
	{
	    status.E_EVENTLOG_CMD( status, 
		    "Could not find `message' attribute" );
	    dhsStatus = DHS_S_SUCCESS;
	    response( DHS_CS_ERROR, dhsStatus );
	}
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaSequenceCmd
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Command object for sequence commands.
//
//-
//***********************************************************************
//

class 	cStaSequenceCmd: public cDhsServerCommand
{
    public: // XXX allan: changed from private
	void		debugLevel( void );
	void		exit( void );
	void		flush( void );
	void		init( void );

	//
	// This declaration of the init function isn't a real function, it
	// just prevents the compiler warning about hiding the init funciton
	// in the cDhsServerCommand class. (Which is a private function 
	// anyway, so I don't know why it cares.)
	//

//	void		init( unsigned long, unsigned long, char *, long, cDhsCmdHandlerBase & ); XXX allan
	void		ping( void );
	void		resetHealth( void );
	void		simulateLevel( void );
	void		test( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::debugLevel
//
// INVOCATION:
// cmd.debugLevel();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the debugging level of the program
//
// DESCRIPTION:
// - Get the debug level from the av list.
// - Set the dhs library debug leve.
// - Set the cStaStat class debug level.
// - Update the debug level channel.
// - Log the debug level change.
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

void		cStaSequenceCmd::debugLevel
(
    void
)
{
    cDhsAttrib	attrib;		// The current attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*level;
    cStaStat	status;


    response( DHS_CS_BUSY, dhsStatus );

    status.S_CMD_RECEIVED( status, "debugLevel" );


    //
    // Find the "level" attribute.
    //

    attrib = find( "level", dhsStatus );

    if ( dhsStatus == DHS_S_SUCCESS )
    {
	//
	// Get the new debug level.
	//

	attrib.info( (void **) &level, dhsStatus );


	//
	// Set the appropriate DHS library debug level.
	//

	if ( strcmp( level, "NONE" ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_OFF, dhsStatus );
	}
	else if ( strcmp( level, "MIN" ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_ON, dhsStatus );
	}
	else if ( strcmp( level, "FULL" ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_FULL, dhsStatus );
	}
	else
	{
	    status.E_DEBUG_CMD( status, "Invalid level" );
	    dhsStatus = DHS_S_SUCCESS;
	}

	//	if ( status == status.S_SUCCESS )
	if ( status.ok() )
	{
	    //
	    //  Set the debug level of this application.
	    //

	    status.debugLevel( level );


	    //
	    // Write the debug level to the SAD.
	    //

	    cStaStat::update( "debug", level, status );
	    cStaStat::flush( status );


	    //
	    // Log the change in the debug level.
	    //

	    status.S_DEBUG_LEVEL( status, level );

	    response( DHS_CS_DONE, dhsStatus );
	}
	else
	{
	    response( DHS_CS_ERROR, dhsStatus );
	}
    }
    else
    {
	status.E_DEBUG_CMD( status, "Could not find `level' attribute" );
	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::exit
//
// INVOCATION:
// command.exit();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Exit the DHS library event loop
//
// DESCRIPTION:
// Trivial
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

void		cStaSequenceCmd::exit
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStaStat	status;


    response( DHS_CS_DONE, dhsStatus );

    status.S_CMD_RECEIVED( status, "exit" );

    cStaDhs::retry( false );
    cDhs::stop( dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::flush
//
// INVOCATION:
// cmd.flush();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute a flush command
//
// DESCRIPTION:
// Trivial
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

void		cStaSequenceCmd::flush
(
    void
)
{
    cStaStat	status;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    response( DHS_CS_BUSY, dhsStatus );

    status.S_CMD_RECEIVED( status, "flush" );

#if defined(EPICS_DHS)
    cStaChannel::dirtyAll( status );
    cStaChannel::chFlush( status );
#endif

    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::init
//
// INVOCATION:
// cmd.init( );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Cause the program to re-initialize.
//
// DESCRIPTION:
// Set the retry flag and then exit the dhs event loop.  The system
// should exit, re-read the configuration files and the re-enter the
// DHS event loop.
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

void		cStaSequenceCmd::init
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStaStat	status;


    response( DHS_CS_BUSY, dhsStatus );


    status.S_CMD_RECEIVED( status, "init" );


    //
    // Set init value to false so connections are refused until
    // the status server is ready for them.
    //

    cStaDhs::isInit( false );


    //
    // Change EPICS values.
    //

    cStaStat::update( "state", "INITIALIZING", status );
    cStaStat::flush( status );
    status.S_CMD_RECEIVED( status, "init" );
    
    response( DHS_CS_DONE, dhsStatus );
    cStaDhs::retry( true );
    cDhs::stop( dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::ping
//
// INVOCATION:
// cmd->ping()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called to execute a ping command
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

void		cStaSequenceCmd::ping
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStaStat	status;


    status.S_CMD_RECEIVED( status, "ping" );

    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::resetHealth
//
// INVOCATION:
// cmd.resetHealth();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the health of the status server to "GOOD".
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

void		cStaSequenceCmd::resetHealth
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStaStat	status;


    status.S_CMD_RECEIVED( status, "resetHealth" );

    cStaStat::resetHealth( status );

    if ( status.ok() )
    {
	response( DHS_CS_DONE, dhsStatus );
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
// cStaSequenceCmd::simulateLevel
//
// INVOCATION:
// cmd.simulateLevel( );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the simulation level of the program
//
// DESCRIPTION:
// - Get the simulate level from the av list.
// - Find the simulate level channel.
// - Update the simulate level channel.
// - Set the new simulate level.
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

void		cStaSequenceCmd::simulateLevel
(
    void
)
{
    cDhsAttrib	attrib;		// The current attribute.
#if defined(EPICS_DHS)
    cStaChannel	*channel;	// The channel for the simulation level.
#endif
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*level;
    cStaStat	status;


    response( DHS_CS_BUSY, dhsStatus );


    status.S_CMD_RECEIVED( status, "simulateLevel" );

    attrib = find( "level", dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	attrib.info( (void **) &level, dhsStatus );
	if ( strcmp( level, "NONE" ) == 0 )
	{
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_NONE );
#endif
	    cStaStat::update( "simulate", "NONE", status );
	    cStaStat::flush( status );
	    cStaDhs::simulate( false );
	    cStaMon::simulate( false );
	    status.S_SIMULATE_LEVEL( status, "NONE" );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else if ( strcmp( level, "VSM" ) == 0 )
	{
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_NONE );
#endif
	    status.S_SIMULATE_LEVEL( status, "VSM" );
	    cStaStat::update( "simulate", "VSM", status );
	    cStaStat::flush( status );
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_VSM );
#endif
	    cStaDhs::simulate( false );
	    cStaMon::simulate( false );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else if ( strcmp( level, "FULL" ) == 0 )
	{
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_NONE );
#endif
	    status.S_SIMULATE_LEVEL( status, "FULL" );
	    cStaStat::update( "simulate", "FULL", status );
	    cStaStat::flush( status );
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_FULL );
#endif
	    cStaDhs::simulate( true );
	    cStaMon::simulate( true );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else if ( strcmp( level, "FAST" ) == 0 )
	{
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_NONE );
#endif
	    status.S_SIMULATE_LEVEL( status, "FAST" );
	    cStaStat::update( "simulate", "FAST", status );
	    cStaStat::flush( status );
#if defined(EPICS_DHS)
	    cStaChannel::simulate( cStaChannel::SIMULATE_FAST );
#endif
	    cStaDhs::simulate( true );
	    cStaMon::simulate( true );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else
	{
	    status.E_SIMULATE_CMD( status, "Invalid level" );
	    dhsStatus = DHS_S_SUCCESS;
	    response( DHS_CS_ERROR, dhsStatus );
	}
#if defined(EPICS_DHS)
	channel->chFlush( status );
#endif
    }
    else
    {
	status.E_SIMULATE_CMD( status, "Could not find `level' attribute" );
	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSequenceCmd::test
//
// INVOCATION:
// cmd.test( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Test the status server and update the health
//
// DESCRIPTION:
// - Test all channels
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

void		cStaSequenceCmd::test
(
    void
)
{
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    cStaStat		status;


    status.S_CMD_RECEIVED( status, "test" );

    cStaStat::update( "state", "TESTING", status );
    cStaStat::flush( status );

    cStaMon::testAll( status );
#if defined(EPICS_DHS)
    if ( cStaChannel::testAll( status )  && status. ok() )
    {
#endif
	status.S_TEST_SUCCESS( status );
	response( DHS_CS_DONE, dhsStatus );
	cStaStat::update( "state", "RUNNING", status );
#if defined(EPICS_DHS)
    }
    else
    {
	status.S_TEST_FAILURE( status );
	status.S_SUCCESS( status );
	response( DHS_CS_ERROR, dhsStatus );
	cStaStat::update( "state", "RUNNING", status );
    }
#endif
    cStaStat::flush( status );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaUpdate
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handler for an event log command.
//
//-
//***********************************************************************
//

class 	cStaUpdate: public cDhsServerCommand
{
    public:
    protected:
    private:
	void		exec( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaUpdate::exec
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
// Called to handle a status update request.
//
// DESCRIPTION:
// - Get the subsytem from the av list.
// - For each attribute in the avlist:
//   - Find the coresponding channel
//   - Update the channel value.
// - Flush the channel values to epics and the status acceptors.
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

void		cStaUpdate::exec
(
    void
)
{
    cDhsAttrib	attrib;		// The current attribute.
    char	*attribName;	// Name of the attribute.
#if defined(EPICS_DHS)
    cStaChannel	*channel;	// Channel to update.
#endif
    int		i;
    char	*attribValue;	// Pointer to the attributes value.
    cStaStat	status;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*subSystem;	// Subsystem for the data.
    DHS_DATA_TYPE type;		// Type of the data.


    response( DHS_CS_BUSY, dhsStatus );

    if ( cStaDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	//
	// Get the subsystem from the av list.
	//

	attrib = find( "subSystem", dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS )
	{
	    attrib.info( (void **) &subSystem, dhsStatus );


	    //
	    // Get each status value from the av list.
	    //

	    status.displayStop();
	    for ( i = 0; ;i++ )
	    {
		attrib = find( i, dhsStatus );
		if ( dhsStatus != DHS_S_SUCCESS )
		{
		    if ( dhsStatus == DHS_S_NO_ATTRIB )
		    {
			dhsStatus = DHS_S_SUCCESS;
		    }
		    break;
		}
		attrib.info( attribName, type, dhsStatus );
		if ( strcmp( attribName, "subSystem" ) != 0 )
		{
		    attrib.info( (void **) &attribValue, dhsStatus );

		    //
		    // Find the channel associated with the attribute and
		    // updates its value.
		    //

#if defined(EPICS_DHS)
		    channel = cStaChannel::find( subSystem, attribName, 
			    status );
		    channel->put( attribValue, status );
#endif
		}
	    }


	    //
	    // Flush the channel values to the EPICS database and the
	    // status acceptors.
	    //

#if defined(EPICS_DHS)
	    cStaChannel::chFlush( status );

	    if ( status.ok() )
	    {
#endif
		response( DHS_CS_DONE, dhsStatus );
#if defined(EPICS_DHS)
	    }
	    else
	    {
		response( DHS_CS_ERROR, status.message(), dhsStatus );
		status.display();
	    }
#endif
	    status.displayStart();
	}
	else
	{
	    status.E_STAUPDATE_CMD( status,
		    "Could not find `subSystem' attribute" );
	    dhsStatus = DHS_S_SUCCESS;
	    response( DHS_CS_ERROR, dhsStatus );
	}
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaGetStatus
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handler for a get status command.
//
//-
//***********************************************************************
//

class 	cStaGetStatus: public cDhsServerCommand
{
    public:
    protected:
    private:
	void		exec( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaGetStatus::exec
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
// Called to handle a get status request.
//
// DESCRIPTION:
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStaGetStatus::exec
(
    void
)
{
    cDhsAttrib	attrib;		// The current attribute.
    cDhsAvList	avList;
#if defined(EPICS_DHS)
    cStaChannel	*channel;	// Channel to query.
#endif
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStaStat	status;
    char	*subSystemName;	// The name of the subsystem.
    char	*statusItem;	// The name of the status item.


    attrib = find( "subSystem", dhsStatus );
    attrib.info( (void **) &subSystemName, dhsStatus );
    attrib = find( "statusItem", dhsStatus );
    attrib.info( (void **) &statusItem, dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
#if defined(EPICS_DHS)
	channel = cStaChannel::find( subSystemName, statusItem, status );
	if ( status.ok() )
	{
	    avList.add( "statusValue", DHS_DT_STRING, channel->data(), 
		    dhsStatus );
	}
    }
    if ( status.ok() && dhsStatus == DHS_S_SUCCESS )
    {
#endif
	response( DHS_CS_DONE, "done", avList, dhsStatus );
    }
#if defined(EPICS_DHS)
    else
    {
	response( DHS_CS_ERROR, "failed", dhsStatus );
    }
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaDhs::~cStaDhs
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
// Destructor for the cStaDhs class
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

		cStaDhs::~cStaDhs
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
// cStaDhs::start
//
// INVOCATION:
// cStaDhs::start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// start the dhs library event loop
//
// DESCRIPTION:
// - Set up the cDhs object.
// - Set up the error and command handlers.
// - Update the state of the status server.
// - Start monitoring resources.
// - Start the dhs event loop.
// - Flush updated status to the status acceptors.
// - Ask all clients to disconnect.
// - Delete the command handlers.
// - Delete the error handler.
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

void		cStaDhs::start
(
    cStaStat	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );

    sdRetry = false;
    checkStat( status, return );


    switch ( status.debugLevel() )
    {
	case	cStaStat::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cStaStat::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cStaStat::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }

    //
    //  Set up the dhs object.
    //

    checkNull( sdPDhs = new cDhs( level ), status, return );


    //
    //  Set up the error handler.
    //

    checkNull( new cDhsErrorHandler<cStaDhsError>, status, return );


    //
    // Set up the command handlers.
    //

    checkNull( new cDhsCmdHandler<cDhsAbortCmd>( false, "cmdAbort" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStaDefaultCmd>( false ), status, return );
    checkNull( new cDhsCmdHandler<cStaEventLog>( false, "staEventLog" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStaUpdate>( false, "staUpdate" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStaGetStatus>( false, "staGetStatus" ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStaSubscribe>( false, "_dhsSubscribe", 
	    &cStaSubscribe::subscribe ), status, return );
    checkNull( new cDhsCmdHandler<cStaSubscribe>( false, "_dhsUnsubscribe", 
	    &cStaSubscribe::unsubscribe ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "test", 
	    &cStaSequenceCmd::test ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "init",
	    &cStaSequenceCmd::init ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "exit",
	    &cStaSequenceCmd::exit ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "reset",
	    &cStaSequenceCmd::init ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "resetHealth",
	    &cStaSequenceCmd::resetHealth ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "debugLevel", 
	    &cStaSequenceCmd::debugLevel ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "simulateLevel",
	    &cStaSequenceCmd::simulateLevel ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "flush",
	    &cStaSequenceCmd::flush ), status, return );
    checkNull( new cDhsCmdHandler<cStaSequenceCmd>( false, "ping",
	    &cStaSequenceCmd::ping ), status, return );


    //
    //  Indicate that the status server is running.
    //

    cStaStat::update( "state", "RUNNING", status );
    cStaStat::flush( status );
    

    //
    // Start the monitoring of spaces.
    //
    
    cStaMon::start( status );

    
    //
    //  Start the dhs event loop.
    //

    cDhs::start( s );
    if ( ! retry() )
    {
	cStaStat::update( "state", "SHUTTINGDOWN", status );
	cStaStat::flush( status );
	status.S_EXITING( status );


	//
	// Wait for all clients to close their connections.
	//

	while ( ! allClosed( NULL ) )
	{
	    dhsEventLoop( DHS_ELT_COND, allClosed, NULL, &s );
	}
    }
    

    cStaStat::logStatus( false );
    cStaSubscriber::exit( status );
    dhsServerExit( &s );


    //
    //  Delete the command handlers.
    //

    cDhsHandler::forEach( delHandler );


    //
    //  Delete the error handler.
    //
    
    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// allClosed
//
// INVOCATION:
// dhsEventLoop( DHS_ELT_COND, allClosed, NULL, &dhsStatus );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) (void *) Unused parameter passed from the event loop.
//
// FUNCTION VALUE:
// (boolean) True if all connections are closed, false otherwise.
//
// PURPOSE:
// Condition function used by the DHS event loop to wait until all
// connections from clients have closed
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

boolean		allClosed
(
    void	*		// (in)  Unused argument from the event loop.
)
{
    int		clientConnects;
    int		serverConnects;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    dhsConnectCount( &clientConnects, &serverConnects, &dhsStatus );
    if ( clientConnects == 0 )
    {
	return( TRUE );
    }
    else
    {
	return( FALSE );
    }
}
