static char rcsid[] = "$Id: stoSeqCmd.C,v 1.1.1.1 2002-11-24 20:32:25 brighton Exp $";
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
// dhsStorage/src/stoSeqCmd.C
//
// PURPOSE:
// Contains the implementation of the cStoSeqCmd class.
//
// CLASSES
// n/a.
// 
// METHOD NAME(S)
// cStoSeqCmd::exec		- Process a sequence command.
// cStoSeqCmd::execDefault	- Process an unrecognized command.
// cStoSeqCmd::execDebug	- Process a debug command.
// cStoSeqCmd::execExit		- Process an exit command.
// cStoSeqCmd::execInit		- Process an init or reset command.
// cStoSeqCmd::execPing		- Process a ping command.
// cStoSeqCmd::execResetHealth	- Process a reset-health command.
// cStoSeqCmd::execSimulate	- Process a simulate command.
// cStoSeqCmd::execTest		- Process a test command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  2001/03/12 20:39:30  nhill
// Added type checking of command parameters. Changed some response
// formatting to use the status messages.
//
// Revision 1.5  1999/06/16 23:14:32  nhill
// Added the VSM simulation level to turn simulation of media write on.
//
// Revision 1.4  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.3  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
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

#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoAction.H"
#include "stoSeqCmd.H"
#include "stoNode.H"


//
// Define and initialize the static data in the cStoDhs class.
//



//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::exec
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
// This function is called when a sequence command is received
//
// DESCRIPTION:
// Call the appropriate command handling method. 
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

void		cStoSeqCmd::exec
(
    void
)
{
    //
    // If we're not ready to receive commands, respond with ERROR. 
    //
    
    if ( !cStoDhs::readyToReceiveCommands() )
    {
	DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

	response( DHS_CS_ERROR, STO_RSP_NOT_READY, dhsStatus );
    }


    //
    // Depending on the command, pick a handling routine. 
    //

    if ( streq( name(), STO_CMD_EXIT ) )
    {
	execExit();
    }
    else if ( streq( name(), STO_CMD_TEST ) )
    {
	execTest();
    }
    else if ( streq( name(), STO_CMD_INIT ) || 
	    streq( name(), STO_CMD_RESET ) )
    {
	execInit();
    }
    else if ( streq( name(), STO_CMD_PING ) )
    {
	execPing();
    }
    else if ( streq( name(), STO_CMD_SIMLEVEL ) )
    {
	execSimulate();
    }
    else if ( streq( name(), STO_CMD_DEBUGLEVEL ) )
    {
	execDebug();
    }
    else if ( streq( name(), STO_CMD_RESETHEALTH ) )
    {
	execResetHealth();
    }
    else
    {
	execDefault();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execDefault
//
// INVOCATION:
// execDefault();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method is called when an unknown command is received
//
// DESCRIPTION:
// Log receipt of the command.
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

void		cStoSeqCmd::execDefault
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS Library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    cStoStatus	status;		// Status. 


    //
    // Get connection info. Log error. Respond to client with ERROR. 
    //
    
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.displayStop();
    status.E_CMD_UNKNOWN( status, name(), id, ip );

    response( DHS_CS_ERROR, status.message(), dhsStatus );
    status.display();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoCmd::execDebug
//
// INVOCATION:
// execDebug();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts the processing of a debug command
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoSeqCmd::execDebug
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
    int		nDims;			// The dimmensions of an attribute.
    cStoStatus	status;			// Status. 
    DHS_DATA_TYPE			// The data type of an attribute.
    		type;

    char* dummy;                        // dummy arg to attribute.info() (XXX allan)

    //
    // Respond to let client continue. Log receipt of command. 
    //
    
    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // If in simulation mode, just return DONE. 
    // 

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    } 

    status.displayStop();


    //
    // Find debug level attribute. Get value.
    //

    levelAttrib = find( STO_CA_LEVEL, dhsStatus );
    levelAttrib.info( dummy, type, nDims, NULL, 
	    (void **) &level, dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type != DHS_DT_STRING || nDims != 0 )
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_LEVEL );
	    response( DHS_CS_ERROR, status.message(), dhsStatus );
	    status.display();
	    return;
	}


	//
	// Set DHS library debug level.
	//

	if ( strcmp( level, STO_DEBUG_LEVEL_NONE ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_OFF, dhsStatus );
	}
	else if ( strcmp( level, STO_DEBUG_LEVEL_MIN ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_ON, dhsStatus );
	}
	else if ( strcmp( level, STO_DEBUG_LEVEL_FULL ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_FULL, dhsStatus );
	}
	else
	{
	    //
	    // Invalid debug level. 
	    //
	    
	    status.E_CMD_BAD_ATTR_VALUE( status,  name(), STO_CA_LEVEL, 
		    level );
	    response( DHS_CS_ERROR, status.message(), dhsStatus );
	    status.display();
	    return;
	}


	//
	// Set HS debug level. Send response. Update HS state.
	//
	
	status.debugLevel( level );
	response( DHS_CS_DONE, dhsStatus );
	cStoDhs::setDebug( level );
	status.S_DEBUG_LEVEL( status, level );
	status.display();
    }
    else
    {
	//
	// No debug level attribute. 
	//

	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_NO_ATTRIBUTE( status, name(), STO_CA_LEVEL );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execExit
//
// INVOCATION:
// execExit();
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoSeqCmd::execExit
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cStoStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // Set retry flag for exit. 
    // Stop queue processing loop.
    //

    cStoDhs::retry( false );
    checkDhs( cDhs::stop( dhsStatus ), dhsStatus, status, VOID );
    status.S_EXITING( status );


    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execInit
//
// INVOCATION:
// execInit();
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoSeqCmd::execInit
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cStoStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // If in simulation mode, just return DONE. 
    //

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    }


    //
    // Set retry flag for re-init. Stop DHS loop.
    //

    cStoDhs::retry( true );
    checkDhs( cDhs::stop( dhsStatus ), dhsStatus, status, VOID );


    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execPing
//
// INVOCATION:
// execPing();
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoSeqCmd::execPing
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cStoStatus  status;                 // Status.


    //
    // Ping requires quick response of "DONE". Log receipt of command.
    //

    response( DHS_CS_DONE, STO_RSP_PING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execResetHealth
//
// INVOCATION:
// execResetHealth();
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

void		cStoSeqCmd::execResetHealth
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cStoStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // Reset health.
    //

    cStaStatus::resetHealth( status );


    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execSimulate
//
// INVOCATION:
// execSimulate();
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoSeqCmd::execSimulate
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cDhsAttrib  levelAttrib;            // Level attribute.
    char        *level;                 // Level string.
    int		nDims;			// The dimensions of an attribute.
    cStoStatus  status;                 // Status.
    DHS_DATA_TYPE			// The type of an attribute.
    		type;

    char* dummy;                        // dummy arg to attribute.info() (XXX allan)

    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    status.displayStop();


    //
    // Get the simulation level from the attribute list.
    //

    levelAttrib = find( STO_CA_LEVEL, dhsStatus );
    levelAttrib.info( dummy, type, nDims, NULL, (void **) &level, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type != DHS_DT_STRING || nDims != 0 )
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_LEVEL );
	    response( DHS_CS_ERROR, status.message(), dhsStatus );
	    status.display();
	    return;
	}


	//
	// Levels "FULL" and "FAST" turn all simulation on. Level VSM
	// truns simulation of media writing on. Level NONE turns 
	// simulation off. Others are invalid .
	//

	if ( strcmp( level, STO_SIM_LEVEL_FULL ) == 0 )
	{
	    cStoDhs::simulate( true );
	    cStoNode::simulate( true );
	}
	else if ( strcmp( level, STO_SIM_LEVEL_FAST ) == 0 )
	{
	    cStoDhs::simulate( true );
	    cStoNode::simulate( true );
	}
	else if ( strcmp( level, STO_SIM_LEVEL_VSM ) == 0 )
	{
	    cStoDhs::simulate( false );
	    cStoNode::simulate( true );
	}
	else if ( strcmp( level, STO_SIM_LEVEL_NONE ) == 0 )
	{
	    cStoDhs::simulate( false );
	    cStoNode::simulate( false );
	}
	else
	{
	    //
	    // Invalid simulation level. 
	    //
	    
	    status.E_CMD_BAD_ATTR_VALUE( status, name(), STO_CA_LEVEL, 
		    level );
	    response( DHS_CS_ERROR, status.message(), dhsStatus );
	    status.display();
	    return;
	}


	//
	// Send response. Update HS state. history-log command.
	//

	response( DHS_CS_DONE, dhsStatus );
	cStoDhs::setSimulate( level );
	status.S_SIMULATE_LEVEL( status, level );
	status.display();
    }
    else
    {
	//
	// No simulation level attribute.
	//

	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_NO_ATTRIBUTE( status, name(), STO_CA_LEVEL );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoSeqCmd::execTest
//
// INVOCATION:
// execTest();
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoSeqCmd::execTest
(
    void
)
{
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
    char	*id;
    char	*ip;
    cStoStatus  status;
    bool	testCAPass( true );
    bool	testDBPass( true );
    bool	testDiskPass( true );
    char	*msgCA;
    char	*msgDB;
    char	*msgDisk;
    char	returnMsg[128];


    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );


    //
    // If in simulation mode, just respond DONE. 
    //

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    }

    
    //
    // Get information about who sent the command. Log it. 
    //
	     
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
	    dhsStatus, status, VOID );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // Change state to TESTING.
    //

    cStoDhs::setState( STO_STATE_TESTING );


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


    //
    // Test Database connection. 
    //


    //
    // Test disk area.
    //


    //
    // Change state to RUNNING.
    //

    cStoDhs::setState( STO_STATE_RUNNING );


    //
    // Print message about test success. 
    //

    if ( testCAPass && testDBPass && testDiskPass )
    {
	status.S_CMD_TEST_SUCCESS( status );
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	response( DHS_CS_ERROR, returnMsg, dhsStatus );
    }
}
