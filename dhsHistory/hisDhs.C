static char rcsid[] = "$Id: hisDhs.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsHistory/src/hisDhs.C
//
// PURPOSE:
// Contains the implementation of the cHisDhs class, and the classes required
// to handle commands and data being received from the DHS++ library.
//
// CLASSES
// cHisDhsError - An object of this type is created to handle a generic 
//		error from the DHS++ library.
// cHisConnect - An object of this type is created to handle a connection 
//		message from the DHS++ library.
// cHisDisconnect - An object of this type is created to handle a disconnect 
//		message from the DHS++ library.
// cHisDefaultCmd - An object of this type is created to handle unknown
//		commands.
// cHisDebugCmd - An object of this type is create to handle a debug command.
// cHisExitCmd - An object of this type is create to handle an exit command.
// cHisInitCmd - An object of this type is create to handle an init or reset 
//		command.
// cHisSimulateCmd - An object of this type is created to handle a simulate 
//		command.
// cHisTestCmd - An object of this type is created to handle a test command.
// 
// METHOD NAME(S)
// cHisDhsError::error - Executed to start the processing of an error message.
// cHisConnect::error - Executed to start the processing of a connect message.
// cHisDisconnect::error - Executed to start the processing of a disconnect 
//		message.
// cHisDefaultCmd::exec - Executed to start the processing of an unrecognized
//		command.
// cHisDebugCmd::exec - Executed to start the processing of a debug command.
// cHisExitCmd::exec - Executed to start the processing of an exit command.
// cHisInitCmd::exec - Executed to start the processing of an init or reset
//		command.
// cHisSimulateCmd::exec - Executed to start the processing of a simulate 
//		command.
// cHisTestCmd::exec - Executed to start the processing  of a test command.
// cHisDhs::~cHisDhs - Destructor for the cHisDhs class.
// cHisDhs::exit - Clean up the DHS and DHS++ libraries before exit.
// cHisDhs::init - Initialize the DHS and DHS++ libraries.
// cHisDhs::start - Start the DHS library event loop.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:18  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:21  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1998/07/24 16:52:16  cockayne
// Fixed BUG simulate processing was printing debug message.
//
// Revision 1.6  1998/06/23 19:19:05  cockayne
// Modified error callback. Added reset health command handler. Modified call
// to cHisStatus::init().
//
// Revision 1.5  1998/05/05 23:46:45  cockayne
// Mod'ed to use #defined string instead of the string itself.
//
// Revision 1.4  1998/04/17 22:45:54  nhill
// Updated to use the latest version of the cDhsStatus class.
// Moved the configuration methods to teh dhs++ library.
//
// Revision 1.3  1997/10/20 21:46:46  cockayne
// Check-in for install.
//
// Revision 1.2  1997/10/06 20:15:14  cockayne
// Check-in for code review
//
// Revision 1.1  1997/09/02 21:13:35  cockayne
// Initial revision
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

#include <iostream>
#include <sys/types.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define	DHS_NO_CLIENT
#define	DHS_NO_GET
#include "dhs++.H"

#include "cadef.h"

#include "globals.H"
#include "hisDhs.H"
#include "hisEvent.H"
#include "hisCa.H"
#include "hisDbm.H"
#include "hisPutFile.H"
#include "hisLogWrite.H"


//
// Define and initialize the static data in the cHisDhs class.
//

bool		cHisDhs::hdRetry = false;
bool		cHisDhs::hdReady = false;
bool		cHisDhs::hdSimulate = false;
extern "C" 
{ 
    long	gethostid( void ); 
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
// cHisDhsError::error
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

void		cHisDhsError::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS library status.
    char	*id;			// Identity of client.
    char	*ip;			// Ip address of client.
    cHisStatus  hStatus;		// Status. 


    if ( level() == DHS_EL_DEBUG || level() == DHS_EL_INFO )
    {
	hStatus.S_DEBUG_MSG( hStatus, message() );
    }
    else
    {
	if ( status() == DHS_E_CON_LOST )
	{
	    //
	    // See if we just lost the Status Server connection.
	    //

	    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
	    if ( connect() == cHisStatus::connection().dhsConnect() )
	    {
		//
		// Shut down the History Server.
		//
		
		hStatus.S_LOST_CONN( hStatus, id, ip );
		hStatus.S_SHUTDOWN( hStatus );
		cHisDhs::retry( false );
		cHisDhs::ready( false );
		cHisCa::channelLoopStop();
	    }
	    else
	    {
		//
		// Just log the fact we lost connection to connect().
		//

		hStatus.S_LOST_CONN( hStatus, id, ip );
	    }
	}
	else
	{
	    //
	    // Plain old error message.
	    //
	    
	    hStatus.E_MSG( hStatus, message() );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisConnect::error
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisConnect::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    cHisStatus	status;		// status. 


    //
    // Get connection info. Log connection.
    //

    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CONNECT_REQ( status, id, ip );


    //
    // Check if the History Server is yet ready to accept connections.
    //

    if ( !cHisDhs::ready() )
    {
	//
	// History Server not ready for connections. Disconnect connector. 
	//
	
	dhsDisconnect( connect(), &dhsStatus );
	status.S_NOT_READY( status, id, ip );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisDisconnect::error
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisDisconnect::error
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS Library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    cHisStatus	status;		// Status. 


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
// cHisDefaultCmd::exec
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

void		cHisDefaultCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS Library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    cHisStatus	status;		// Status. 


    //
    // Get connection info. Log error. Respond to client with ERROR. 
    //
    
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.E_UNKNOWN_COMMAND( status, name(), id, ip );
    response( DHS_CS_ERROR, HIS_RSP_UNREC_CMD_MSG, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisDebugCmd::exec
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
// Starts the processing of an debug command
//
// DESCRIPTION:
// Get the debug level from the command attributes and set the DHS library
// and Quick Look server debug levels.
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

void		cHisDebugCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status. 
    char        *id;			// Identity of client.
    char        *ip;			// Ip address of client.
    cDhsAttrib	levelAttrib;		// Debug level attribute.
    char	*level;			// Debug level string.
    cHisStatus	status;			// Status. 


    //
    // Respond to let client continue. Log receipt of command. 
    //
    
    response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    //
    // If in simulation mode, just return DONE. 
    // 

    if ( cHisDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    } 


    //
    // Find debug level attribute. Get value.
    //

    levelAttrib = find( HIS_ATTR_LEVEL, dhsStatus );
    levelAttrib.info( (void **) &level, dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	//
	// Set DHS library debug level.
	//

	if ( strcmp( level, HIS_DEBUG_LEVEL_NONE ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_OFF, dhsStatus );
	}
	else if ( strcmp( level, HIS_DEBUG_LEVEL_MIN ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_ON, dhsStatus );
	}
	else if ( strcmp( level, HIS_DEBUG_LEVEL_FULL ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_FULL, dhsStatus );
	}
	else
	{
	    //
	    // Invalid debug level. 
	    //
	    
	    status.E_DEBUG_COMMAND( status, HIS_RSP_BAD_LEVEL_MSG );
	    response( DHS_CS_ERROR, HIS_RSP_BAD_LEVEL_MSG, dhsStatus );
	    status.E_DEBUG_COMMAND( status, level );
	    return;
	}


	//
	// Set HS debug level. Send response. Update HS state.
	//
	
	status.debugLevel( level );
	response( DHS_CS_DONE, dhsStatus );
	cHisStatus::update( HIS_DEBUG, level, status );
	cHisStatus::flush( status );
	status.S_DEBUG_LEVEL( status, level );
    }
    else
    {
	//
	// No debug level attribute. 
	//

	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, HIS_RSP_NO_LEVEL_MSG, dhsStatus );
	status.E_CMD_FORMAT( status, name() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisExitCmd::exec
//
// INVOCATION:
// n.a. Called by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Begin the processing of the exit command
//
// DESCRIPTION:
// - Ensure the retry flag is set to false.
// - Execute the cDhs::stop function.
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

void		cHisExitCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cHisStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    //
    // Stop channel access, set retry flag for exit. Stop HLW cmd.  
    //

    cHisCa::channelLoopStop();
    //cHisLogWrite::stopHLW();
    cHisDhs::retry( false );


    response( DHS_CS_DONE, dhsStatus );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisInitCmd::exec
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
// Starts the processing of an init command
//
// DESCRIPTION:
// - Ensure the retry flag is set to true.
// - Execute the cDhs::stop function.
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

void		cHisInitCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cHisStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    //
    // If in simulation mode, just return DONE. 
    //

    if ( cHisDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    }


    //
    // Stop HLW cmd. Stop channel access. Set retry flag for re-init. 
    //

    //cHisLogWrite::stopHLW();
    cHisDhs::retry( true );
    cHisCa::channelLoopStop();


    response( DHS_CS_DONE, dhsStatus );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisPingCmd::exec
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
// Starts the processing of an ping command
//
// DESCRIPTION:
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

void		cHisPingCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cHisStatus  status;                 // Status.


    //
    // Ping requires quick response of "DONE". Log receipt of command.
    //

    response( DHS_CS_DONE, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisResetHealthCmd::exec
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
// Starts the processing of a reset health command
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

void		cHisResetHealthCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cHisStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    //
    // Reset health, unless a hisLogWrite command is being processed. .
    //

    if ( cHisLogWrite::HLWRunning() )
    {
	response( DHS_CS_ERROR, HIS_RSP_ERROR_HLW_IN_PROGRESS, dhsStatus );
    }
    else
    {
	cHisStatus::resetHealth( status );
	response( DHS_CS_DONE, dhsStatus );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisSimulateCmd::exec
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
// Starts the processing of an simulate command
//
// DESCRIPTION:
// - Get the simulation level from the command attributes.
// - If the simulation level is "VSM" or "FAST" turn simulation on,
//   otherwise turn simulation off.
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

void		cHisSimulateCmd::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cDhsAttrib  levelAttrib;            // Level attribute.
    char        *level;                 // Level string.
    cHisStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    //
    // Get the simulation level from the attribute list.
    //

    levelAttrib = find( HIS_ATTR_LEVEL, dhsStatus );
    levelAttrib.info( (void **) &level, dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	//
	// Levels "FULL" and "FAST" turn simulation on. Levels NONE and 
	// VSM turn simulation off. Others are invalid .
	//

	if ( strcmp( level, HIS_SIM_LEVEL_FULL ) == 0 || 
		strcmp( level, HIS_SIM_LEVEL_FAST ) == 0 )
	{
	    cHisDhs::simulate( true );
	}
	else if ( strcmp( level, HIS_SIM_LEVEL_NONE ) == 0 || 
		strcmp( level, HIS_SIM_LEVEL_VSM ) == 0 )
	{
	    cHisDhs::simulate( false );
	}
	else
	{
	    //
	    // Invalid simulation level. 
	    //
	    
	    status.E_SIMULATE_COMMAND( status, HIS_RSP_BAD_LEVEL_MSG );
	    response( DHS_CS_ERROR, HIS_RSP_BAD_LEVEL_MSG, dhsStatus );
	    status.E_SIMULATE_COMMAND( status, level );
	    return;
	}


	//
	// Send response. Update HS state. History-log command.
	//

	response( DHS_CS_DONE, dhsStatus );
	cHisStatus::update( HIS_SIMULATE, level, status );
	cHisStatus::flush( status );
	status.S_SIMULATE_LEVEL( status, level );
    }
    else
    {
	//
	// No simulation level attribute.
	//

	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, HIS_RSP_NO_LEVEL_MSG, dhsStatus );
	status.E_CMD_FORMAT( status, name() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisTestCmd::cmdAbort
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
// Starts the processing of a cmdAbort command
//
// DESCRIPTION:
// Records reception of the command to the status server, then refuses it. 
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

bool		cHisTestCmd::cmdAbort
(
    void
)
{
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
				// DHS library status. 
    char	*id;		// Connecting client's identity.
    char	*ip;		// Connecting client's IP address. 
    cHisStatus	status;		// Program status. 


    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
	dhsStatus, status, VOID );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    status.E_ABORT( status, id );


    return( FALSE );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisTestCmd::exec
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
// Starts the processing of an test command
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

void		cHisTestCmd::exec
(
    void
)
{
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
    char	*id;
    char	*ip;
    cHisStatus  status;
    bool	testCAPass( true );
    bool	testDBPass( true );
    bool	testDiskPass( true );
    char	*msgCA;
    char	*msgDB;
    char	*msgDisk;
    char	returnMsg[128];


    response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );


    //
    // If in simulation mode, just respond DONE. 
    //

    if ( cHisDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    }

    
    //
    // Get information about who sent the command. Log it. 
    //
	     
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
	    dhsStatus, status, VOID );
    status.S_COMMAND_RECEIVED( status, name(), id, ip );


    //
    // Change state to TESTING.
    //

    cHisStatus::update( HIS_STATE, HIS_STATE_TESTING, status );
    cHisStatus::flush( status );


    //
    // Reset status for tests. Initialize return strings.
    //
    
    if ( !status.ok() )
    {
	status.status( status.S_SUCCESS );
    }
    msgCA = strsav( "" );
    msgDB = strsav( "" );
    msgDisk = strsav( "" );


    //
    // Test Channel Access. 
    //

    cHisCa::testChannelAccess( status );
    if ( !status.ok() )
    {
	testCAPass = false;
	msgCA = strsav( HIS_CATEST_FAIL_MSG );
	status.status( status.S_SUCCESS );
    }
    

    //
    // Test Database connection. 
    //

    cHisDbManager::testDatabase( status );
    if ( !status.ok() )
    {
	testDBPass = false;
	msgDB = strsav( HIS_DBTEST_FAIL_MSG );
	status.status( status.S_SUCCESS );
    }


    //
    // Test disk area.
    //

    cHisCa::testDiskArea( status );
    if ( !status.ok() )
    {
	testDiskPass = false;
	msgDisk = strsav( HIS_DISKTEST_FAIL_MSG );
	status.status( status.S_SUCCESS );
    }


    //
    // Change state to RUNNING.
    //

    cHisStatus::update( HIS_STATE, HIS_STATE_RUNNING, status );
    cHisStatus::flush( status );


    //
    // Print message about test success. 
    //

    if ( testCAPass && testDBPass && testDiskPass )
    {
	status.S_TEST_SUCCESS( status );
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	(void) sprintf( returnMsg, "%s%s%s", msgCA, msgDB, msgDisk );
	status.S_TEST_FAILURE( status );
	response( DHS_CS_ERROR, returnMsg, dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisDhs::exit
//
// INVOCATION:
// cHisDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cHisStatus &) Function return status.
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
void		cHisDhs::exit
(
    cHisStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    // If the retry flag is true, then it should re-initialize
    // everything, otherwise it is time to exit.  Update the
    // program state appropriately.
    //

    if ( cHisDhs::retry() )
    {
	cHisStatus::update( HIS_STATE, HIS_STATE_INITIALIZE, status );
	status.S_INITIALIZING( status );
    }
    else
    {
	cHisStatus::update( HIS_STATE, HIS_STATE_SHUTDOWN, status );
	status.S_EXITING( status );
    }
    cHisStatus::flush( status );

    //
    // If there is a HLW command being processed, stop it. 
    //

    cHisLogWrite::stopHLW();

    
    //
    // Stop dhs event loop.
    //

    hdPDhs->stop( dhsStatus );


    //
    // Delete the error handlers.
    //

    cDhsHandler::forEach( delHandler );


    // 
    // Disconnect from the status server.
    //

    cHisStatus::disconnect( status );


    //
    // Delete the cDhs object.
    //

    delete hdPDhs;
    hdPDhs = NULL;


    //
    // If the retry flag is set, reset hisLogWrite stopping flag.
    //

    if ( cHisDhs::retry() )
    {
	cHisLogWrite::setHLWStopping( false );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisDhs::init
//
// INVOCATION:
// cHisDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cHisStatus &) Function return status.
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

void		cHisDhs::init
(
    cHisStatus	&status		// (mod) Function return status.
)
{
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    hdRetry = false;
    checkStat( status, return );


    //
    // Select a debug level for the DHS library based on the current debug
    // level of the application.
    //

    switch ( cHisStatus::debugLevel() )
    {
	case	cHisStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cHisStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cHisStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }


    //
    //  Set up the dhs object.
    //

    checkNull( hdPDhs = new cDhs( level ), status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDhsErrorHandler<cHisDhsError>, status, return );
    checkNull( new cDhsErrorHandler<cHisConnect>( DHS_S_CONNECT ), 
	    status, return );
    checkNull( new cDhsErrorHandler<cHisDisconnect>( DHS_S_DISCONNECT ), 
	    status, return );


    //
    // Set up the command handlers.
    //

    checkNull( new cDhsCmdHandler<cHisExitCmd> ( false, HIS_CMD_EXIT ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisTestCmd> ( false, HIS_CMD_TEST ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisInitCmd> ( false, HIS_CMD_INIT ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisInitCmd> ( false, HIS_CMD_RESET ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisResetHealthCmd> ( false, 
	    HIS_CMD_RESETHEALTH ), status, return );
    checkNull( new cDhsCmdHandler<cHisLogWrite> ( true, HIS_CMD_HISLOGWRITE ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisPingCmd> ( false, HIS_CMD_PING ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisSimulateCmd> ( false, HIS_CMD_SIMLEVEL ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisDebugCmd> ( false, HIS_CMD_DEBUGLEVEL ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cDhsAbortCmd> ( false, HIS_CMD_CMDABORT ), 
	    status, return );
    checkNull( new cDhsCmdHandler<cHisDefaultCmd> ( false ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cHisStatus &) Function return status.
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

void		cHisDhs::start
(
    cHisStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    // Connect to Status Server.
    //

    cHisStatus::init( PREFIX, status ); 
    check( cHisStatus::connect( status ), status, return );


    //
    // Update state of history server.
    //

    cHisStatus::update( HIS_HEALTH, HIS_HEALTH_GOOD, status );
    cHisStatus::update( HIS_STATE, HIS_STATE_INITIALIZE, status );
    status.S_INITIALIZING( status );
    cHisStatus::update( HIS_DEBUG, status.debugString(), status );
    if ( cHisDhs::simulate() )
    {
	cHisStatus::update( HIS_SIMULATE, HIS_SIM_LEVEL_FAST, status );
    }
    else
    {
	cHisStatus::update( HIS_SIMULATE, HIS_SIM_LEVEL_NONE, status );
    }
    cHisStatus::flush( status );


    //
    // Start DHS event loop in its own thread.
    //

    checkDhs( hdPDhs->startThread( dhsStatus ), dhsStatus, 
	    status, return );
}
