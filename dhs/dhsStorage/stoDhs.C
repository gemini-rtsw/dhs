static char rcsid[] = "$Id: stoDhs.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoDhs.C
//
// PURPOSE:
// Contains the implementation of the cStoDhs class, and the classes required
// to handle commands and data being received from the DHS++ library.
//
// CLASSES
// n/a
// 
// METHOD NAME(S)
// cStoDhsError::error	- Process an error message.
// cStoConnect::error	- Process a connect message.
// cStoDisconnect::error- Process a disconnect message.
//
// cStoDhs::cStoDhs	- Constructor for DHS object.
// cStoDhs::exit	- Clean up DHS and DHS++ libraries before exit.
// cStoDhs::init	- Initialize DHS and DHS++ libraries.
// cStoDhs::start	- Start the DHS library event loop.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:55  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.12  2001/03/09 22:52:44  nhill
// Moved the initialization of the status class to main, to avoid
// problems with crashes during resets, and when problems are
// detected during startup.
//
// Revision 1.11  1999/10/12 22:57:06  nhill
// Changed to call specific functions in the cStoRequestCmd for archive file
// and user media requests, instead of calling the default exec function.
//
// Revision 1.10  1999/06/16 23:56:58  nhill
// Changed the request commands to call the appropriate function directly.
// Changed the info and refresh commands to run in a separate thread.
//
// Revision 1.9  1999/02/24 23:48:53  jaeger
// Added the stoRefresh command.
//
// Revision 1.8  1998/08/03 23:03:13  cockayne
// Added case where reporting a lost connection but dhsConnectInfo() returns
// an error.
//
// Revision 1.7  1998/06/24 16:01:31  cockayne
// Modified DHS Error callback.
//
// Revision 1.6  1998/05/15 17:07:18  cockayne
// Mod'ed cStoDhsError::error(), Mod'ed to send startup commands.
//
// Revision 1.5  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.4  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 21:01:08  cockayne
// Added new dhs command handler for user media request commands.
//
// Revision 1.2  1998/03/26 21:21:15  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_GET

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoConnection.H"
#include "stoSeqCmd.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoAction.H"
#include "stoActionCmd.H"
#include "stoRequestCmd.H"
#include "stoInfoCmd.H"
#include "stoRefreshCmd.H"
#include "stoMedia.H"
#include "stoPut.H"

#include "stoDhs.H"


//
// Define and initialize the static data in the cStoDhs class.
//

bool		cStoDhs::sdReadyToReceiveCommands = false;
bool		cStoDhs::sdRetry = false;
bool		cStoDhs::sdSimulateFlag = false;
cStoEpicsRecord<char *, cStoDhs, cStoStatus>
		cStoDhs::sdState( STO_ERD_STATE, NULL, NULL );
cStoEpicsRecord<char *, cStoDhs, cStoStatus>
		cStoDhs::sdSimulate( STO_ERD_SIMULATE, NULL, NULL );
cStoEpicsRecord<char *, cStoDhs, cStoStatus>
		cStoDhs::sdDebug( STO_ERD_DEBUG, NULL, NULL );


extern "C" 
{ 
    long	gethostid( void ); 
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDhs::cStoDhs
//
// INVOCATION:
// cStoDhs();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cStoDhs) A new cStoDhs object.
//
// PURPOSE:
// Constructor for objects of the cStoDhs class.
//
// DESCRIPTION:
// Set the pointer to the cDhs object to NULL.
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

		cStoDhs::cStoDhs
(
    void
)
{
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// delhandler
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
// FUNCTION NAME:
// cStoDhsError::error
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDhsError::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS library status.
    char	*id;			// Identity of client.
    char	*ip;			// Ip address of client.
    cStoStatus	sStatus;		// Status. 


    if ( level() == DHS_EL_DEBUG || level() == DHS_EL_INFO )
    {
	sStatus.S_DEBUG_MSG( sStatus, message() );
    }
    else
    {
	if ( status() == DHS_E_CON_LOST )
	{
	    //
	    // See if we just lost the Status Server connection.
	    //

	    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
	    if ( connect() == cStoStatus::connection().dhsConnect() )
	    {
		//
		// Shut down the Storage Server.
		//

		sStatus.S_LOST_CONN( sStatus, id, ip );
		sStatus.S_SHUTDOWN( sStatus );
		cStoDhs::retry( false );
		cStoDhs::readyToReceiveCommands( false );
		dhsStatus = DHS_S_SUCCESS;
		cDhs::stop( dhsStatus );
	    }
	    else
	    {
		//
		// Just log the fact we lost connection to connect().
		//

		if ( dhsStatus == DHS_S_SUCCESS )
		{
		    sStatus.S_LOST_CONN( sStatus, id, ip );
		}
		else
		{
		    sStatus.S_LOST_CONN( sStatus, "unknown server", 
			    "unknown address");
		}
	    }
	}
	else
	{
	    //
	    // Plain old error message.
	    //

	    sStatus.E_MSG( sStatus, message() );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoConnect::error
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
// Make sure connection is from an authorized connector. 
// Log the connection. 
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

void		cStoConnect::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    cStoStatus	status;		// status. 


    //
    // Get connection info. Log connection.
    //

    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CONNECT_REQ( status, id, ip );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDisconnect::error
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
// Log the disconnect 
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

void		cStoDisconnect::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS Library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    cStoStatus	status;		// Status. 


    if ( connect() != 0 )
    {
	//
	// Get connection info. Log disconnection. 
	//

	dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
	status.S_DISCONNECT_REQ( status, id, ip );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDhs::exit
//
// INVOCATION:
// cStoDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
void		cStoDhs::exit
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    // Depending if this is an init or exit, update the program state.
    //

    if ( sdRetry )
    {
	setState( STO_STATE_INITIALIZE );
    }
    else
    {
	setState( STO_STATE_SHUTDOWN );
    }

    
    //
    // Disconnect from DHS Status Server.
    // Delete DHS error handlers.
    //

    cStaStatus::disconnect( status );
    cDhsHandler::forEach( delHandler );


    //
    // Delete the cDhs object.
    //

    delete sdPDhs;
    sdPDhs = NULL;


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDhs:init
//
// INVOCATION:
// cStoDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDhs::init
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    sdRetry = false;
    checkStat( status, return );


    //
    // Select a debug level for the DHS library based on the current debug
    // level of the application.
    //

    switch ( cStoStatus::debugLevel() )
    {
	case	cStoStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;

	case	cStoStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;

	case	cStoStatus::DEBUG_FULL:
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

    checkNull( new cDhsErrorHandler<cStoDhsError>, status, return );
    checkNull( new cDhsErrorHandler<cStoConnect>( DHS_S_CONNECT ), 
	    status, return );
    checkNull( new cDhsErrorHandler<cStoDisconnect>( DHS_S_DISCONNECT ), 
	    status, return );


    //
    // Connect to DHS Status Server.
    //

    check( cStaStatus::connect( status ), status, return );


    //
    // Initialize health, state etc. through the DHS Status Server. 
    //

    setState( STO_STATE_INITIALIZE );
    status.S_INITIALIZING( status );
    check( cStoStatus::update( "health", "GOOD", status ), status, VOID );
    setDebug( status.debugString() );
    if ( cStoDhs::simulate() )
    {
	setSimulate( STO_SIM_LEVEL_FAST );
    }
    else
    {
	setSimulate( STO_SIM_LEVEL_NONE );
    }


    //
    // Set flag which causes commands to be rejected until we're ready.
    //

    sdReadyToReceiveCommands = false;

    
    //
    // Set up the sequence command handlers.
    //

    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_EXIT ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_TEST ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_INIT ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_RESET ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_PING ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_SIMLEVEL ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false, STO_CMD_DEBUGLEVEL ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cDhsAbortCmd> ( false, STO_CMD_CMDABORT ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoSeqCmd> ( false ), status, return );
    

    //
    // Set up the media request command handlers.
    //

    checkNull( new cDhsCmdHandler<cStoRequestCmd> ( true, 
	    STO_CMD_USER_MEDIA, &cStoRequestCmd::execUserMediaRequest ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoRequestCmd> ( true, 
	    STO_CMD_ARCHIVE_FILE, &cStoRequestCmd::execStoArchiveFileAvailable ),
	    status, return );


    //
    // Set up the bulk data put handler.
    //

    checkNull( new cDhsPutHandler<cStoPut>( true ), status, return );


    //
    // Set up the action and rollback command handlers.
    //

    checkNull( new cDhsCmdHandler<cStoActionCmd> ( true, 
	    STO_CMD_ACTION_REQUEST, &cStoActionCmd::execActionRequest ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoActionCmd> ( true, 
	    STO_CMD_ROLLBACK_REQUEST, &cStoActionCmd::execRollbackRequest ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cStoActionCmd> ( true, 
	    STO_CMD_ACTION_CANCEL, &cStoActionCmd::execActionCancel ), 
	    status, return );


    //
    // Set up the info command handler.
    //

    checkNull( new cDhsCmdHandler<cStoInfoCmd> ( true, 
	    STO_CMD_INFO ), status, return );

    
    //
    // Set up the refresh command handler
    //

    checkNull( new cDhsCmdHandler<cStoRefreshCmd> ( true,
	    STO_CMD_REFRESH ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDhs::start
//
// INVOCATION:
// stoDhsObject.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDhs::start
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    checkStat( status, return );


    //
    // Set stuff to indicate we're running. 
    //

    sdReadyToReceiveCommands = true;
    setState( STO_STATE_RUNNING );
    status.S_RUNNING( status );


    //
    // Send "alive" command to dataservers in the notify data servers list.
    //

    cStoConnection::aliveCommands( status );

    
    //
    // Run event loop. 
    //

    checkDhs( sdPDhs->start( dhsStatus ), dhsStatus, status, return );


    return;
}
