static char rcsid[] = "$Id: seq.C,v 1.1.1.1 2002-11-24 20:27:32 brighton Exp $";
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
// dhsData/src/seq.C
//
// PURPOSE:
// Class methods to handle the sequence commands.
//
// METHOD NAME(S)
// cDtsDhsSequence::debugLevel	- Set the debug level.
// cDtsDhsSequence::exec	- Called by command handler
// cDtsDhsSequence::exit	- Called by command handler
// cDtsDhsSequence::initReset	- Restart the dataServer.
// cDtsDhsSequence::resetHealth	- Reset health.
// cDtsDhsSequence::simulateLevel
//				- Set the simulate level.
// cDtsDhsSequence::test	- Start a test.
// 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.5  2000/07/27 20:10:37  dunn
// Added standalone features.
//
// Revision 1.4  1998/05/04 23:03:20  dunn
// Added status changes.
//
// Revision 1.3  1997/09/17 22:15:23  dunn
// Code cleanup.
//
// Revision 1.2  1997/08/29 23:05:14  dunn
// Added abort, reset, init, etc.
//
// Revision 1.1  1997/08/19 22:31:56  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


#include "globals.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "gen_eptr.h"

}

#include "dtsDhs.H"
#include "store.H"
#include "dbm.H"
#include "con.H"
#include "seq.H"

//
//  Local Defines.
//

#define DTS_DEBUG_LEVEL		"level"


bool		cDtsDhsSequence::cdsContTest = FALSE;

msgText( cDtsDhsSequence::sStatus,  E_SEQ_CMD, P_NULL,
    "Error, invalid datasetName `%s', cannot set %s." );
msgText( cDtsDhsSequence::sStatus,  E_DEBUG_COMMAND, P_NULL,
	"Error, invalid debugLevel command: `%s'." );
msgText( cDtsDhsSequence::sStatus,  E_SIMULATE_COMMAND, P_NULL,
	"Error, invalid simulateLevel command: `%s'." );
msgText( cDtsDhsSequence::sStatus,  S_TEST_FAIL, P_NULL,
	"Test %s failed." );
msgText( cDtsDhsSequence::sStatus,  S_TEST_SUCCESS, P_NULL,
	"Test %s succeeded." );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::debugLevel
//
// INVOCATION:
// cmd.debugLevel( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the debugging level of the program
//
// DESCRIPTION:
// Set the debugging level of the program.
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

void		cDtsDhsSequence::debugLevel
(
    cDtsStatus	&status		// (mod) Function return status.
)
{

    cDhsAttrib	levelAttrib;	// The current attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Dhs status.
    char	*level;		// New debug level.


    checkStat( status, return );


    //
    // Find the "level" attribute. And get the value.
    //

    levelAttrib = find( DTS_DEBUG_LEVEL, dhsStatus );

    if ( dhsStatus == DHS_S_SUCCESS )
    {
	//
	// Get the new debug level.
	//

	levelAttrib.info( (void **) &level, dhsStatus );


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
	    seqStatus.E_DEBUG_COMMAND( status, "Invalid level" );
	    status.E_DEBUG_COMMAND( status, "Invalid level", sourceName(), 
		    sourceAddr() );
	    dhsStatus = DHS_S_SUCCESS;
	}


	if ( status == status.S_SUCCESS )
	{
	    //
	    //  Set the debug level of this application.
	    //

	    status.debugLevel( level );


	    //
	    // Log the change in the debug level, response and update
	    // the status server.
	    //

	    cStaStatus::update( "debug", status.debugString(), status );
	    cStaStatus::flush( status );
	    status.S_DEBUG_LEVEL( status, level );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else
	{
	    //
	    //  Invalid debug level sent.
	    //
	    
	    dhsStatus = DHS_S_SUCCESS;
	    response( DHS_CS_ERROR, seqStatus.message(), dhsStatus );
	}
    }
    else
    {
	//
	//  Missing the level attribute.
	//

	seqStatus.E_DEBUG_COMMAND( status, "Could not find `level' attribute" );
	status.E_DEBUG_COMMAND( status, "Could not find `level' attribute", 
		sourceName(), sourceAddr() );
	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, seqStatus.message(), dhsStatus );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::exec
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
// To handle commands such as init, reset, test, debugLevel, simulateLevel, exit
//
// DESCRIPTION:
// To handle commands such as init, reset, test, debugLevel, simulateLevel, 
// exit.
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

void		cDtsDhsSequence::exec
(
    void
)
{
    char	*address = NULL;	 // Ip address of the client.
    char	*clientName = NULL;	 // Name of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    					// Dhs status.
    cDtsStatus	status;			// Function return status.



    response( DHS_CS_BUSY, "working", dhsStatus );
    seqStatus.displayStop();
 

    //
    //  Get information about who sent the command.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    if ( cDtsDhs::simulate() && strcmp( name(), DTS_SIM_CMD ) != 0 )
    {
	//
	//  In simulate mode and command is not simulateLevel, so return.
	//

	response( DHS_CS_DONE, dhsStatus );
    }
    else if ( strcmp( name(), DTS_TEST_CMD ) == 0 )
    {
	//
	//  Test request.
	//

	test( status );
    } 
    else if ( strcmp( name(), DTS_PING_CMD ) == 0 )
    {
	//
	//  Ping request.
	//

	response( DHS_CS_DONE, dhsStatus );
    } 
    else if ( strcmp( name(), DTS_INIT_CMD ) == 0 || 
	    strcmp( name(), DTS_RESET_CMD ) == 0 )
    {
	//
	//  Init or reset request.
	//

	initReset( status );
    }
    else if ( strcmp( name(), DTS_RESET_HEALTH_CMD ) == 0 )
    {
	//
	//  Reseting the health.
	//

	resetHealth( status );
    }
    else if ( strcmp( name(), DTS_DEBUG_CMD ) == 0 )
    {
	//
	//  Change the debugging level.
	//

	debugLevel( status );
    }
    else if ( strcmp( name(), DTS_SIM_CMD ) == 0 )
    {
	//
	//  Change the simulate level.
	//

	simulateLevel( status );
    }
    else if ( strcmp( name(), DTS_EXIT_CMD ) == 0 )
    {
	//
	//  Exit the data server.
	//

	exit( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::exit
//
// INVOCATION:
// command.exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Exit the DHS library event loop
//
// DESCRIPTION:
// Exit the DHS library event loop.  Log receipt of the command, set the
// retry flag to false, response and then stop the event loop.
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

void		cDtsDhsSequence::exit
(
    cDtsStatus	&status		// (mod) Function return status.
)
{

    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Log receipt of this command.
    //

    status.S_CMD_RECEIVED( status, "exit", sourceName(), sourceAddr() );


    //
    //  Set the retry flag to false because we are exiting.
    //  Set the ready state of the dataserver to not ready.
    //

    cDtsDhs::retry( false );
    cDtsDhs::ready( status, false );


    //
    // Indicate the command is going to complete and stop the event loop.  
    //

    response( DHS_CS_DONE, dhsStatus );
    cDhs::stop( dhsStatus );


}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::initReset
//
// INVOCATION:
// cmd.initReset( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Cause the program to re-initialize
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

void		cDtsDhsSequence::initReset
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Dhs status.


    //
    //  Log the command.
    //

    status.S_CMD_RECEIVED( status, "init/reset", sourceName(), sourceAddr() );


    //
    //  Response, set the retry flag to true and stop the event loop.
    //

    response( DHS_CS_DONE, dhsStatus );
    cDtsDhs::ready( status, false );
    cDtsDhs::retry( true );
    cDhs::stop( dhsStatus );

}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::resetHealth
//
// INVOCATION:
// N.A. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Reset the health status to good
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

void		cDtsDhsSequence::resetHealth
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Log receipt of this command. Reset health and respond.
    //

    status.S_CMD_RECEIVED( status, "resetHealth", sourceName(), sourceAddr() );

    status.resetHealth( status );

    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::simulateLevel
//
// INVOCATION:
// cmd.simulateLevel( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the simulation level of the program
//
// DESCRIPTION:
// Find the "level" attribute and match it to levels known.
// NONE and VSM set simulate off.  FAST and FULL set simulate on.
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

void		cDtsDhsSequence::simulateLevel
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    
    cDhsAttrib	attrib;		// The current attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*level;


    checkStat( status, return );


    //
    //  Find the "level" attribut.
    //

    attrib = find( "level", dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	attrib.info( (void **) &level, dhsStatus );
	if ( strcmp( level, "NONE" ) == 0 ||
	    strcmp( level, "VSM" ) == 0 )
	{
	    //
	    //  Simulate off.
	    //

	    cDtsDhs::simulateString( FALSE );
	    status.S_SIMULATE_LEVEL( status, "NONE",
		    sourceName(), sourceAddr() );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else if ( strcmp( level, "FULL" ) == 0 ||
		strcmp( level, "FAST" ) == 0 )
	{
	    //
	    //  Simulate on.
	    //

	    cDtsDhs::simulate( TRUE );
	    status.S_SIMULATE_LEVEL( status, "FULL", sourceName(), 
	    		sourceAddr() );
	    response( DHS_CS_DONE, dhsStatus );
	}
	else
	{
	    //
	    //  Invalid level.
	    //

	    seqStatus.E_SIMULATE_COMMAND( status, "Invalid level" );
	    status.E_SIMULATE_COMMAND( status, "Invalid level",
		    sourceName(), sourceAddr() );
	    dhsStatus = DHS_S_SUCCESS;
	    response( DHS_CS_ERROR, seqStatus.message(), dhsStatus );
	}


	//
	//  Update the status server.
	//

	cStaStatus::update( "simulate", cDtsDhs::simulateString(), status );
	cStaStatus::flush( status );
    }
    else
    {
	//
	//  Missing the level attribute in the command.  Error.
	//

	seqStatus.E_SIMULATE_COMMAND( status, 
		"Could not find `level' attribute" );
	status.E_SIMULATE_COMMAND( status, "Could not find `level' attribute",
		sourceName(), sourceAddr() );
	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, seqStatus.message(), dhsStatus );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsSequence::test
//
// INVOCATION:
// cmd.test( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Test the data server and update the health
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

void		cDtsDhsSequence::test
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    bool	failTest;


    //
    //  Change state on status server.
    //

    cStaStatus::update( "state", "TESTING", status );
    cStaStatus::flush( status );


    //
    //  Log the command.
    //

    status.S_CMD_RECEIVED( status, "test", sourceName(), sourceAddr() );


    //
    //  Test the database connections, only if NOT in standalone mode..
    //

    failTest = FALSE;
    continueTesting ( TRUE );

    if ( !status.standAlone() )
    {
	if ( cDtsDbManager::testAll( status ) )
	{
	    status.S_TEST_SUCCESS( status, "database connections");
	    seqStatus.S_TEST_SUCCESS( seqStatus, "database connections");
	}
	else
	{
	    failTest = TRUE;
	    status.S_TEST_FAIL( status, "database connections");
	    seqStatus.S_TEST_FAIL( seqStatus, "database connections");
	}
    }

    status.status( status.S_SUCCESS );


    //
    //  Test that the temp and perm disk storage area are visible.
    //

    if ( continueTesting() )
    {
	if ( cDtsStoreManager::testAll( status ) )
	{
	    status.S_TEST_SUCCESS( status, "disk storage area");
	    seqStatus.S_TEST_SUCCESS( seqStatus, "disk storage area");
	}
	else
	{
	    failTest = TRUE;
	    status.S_TEST_FAIL( status, "disk storage area" );
	    seqStatus.S_TEST_FAIL( seqStatus, "disk storage area" );
	}
    }


    //
    //  Send a command "X" to all server connections current connected.
    //  If some are no longer available, then update the connection information.
    //

    if ( continueTesting() )
    {
	if ( cDtsConManager::testAll( status ) )
	{
	    status.S_TEST_SUCCESS( status, "connections");
	    seqStatus.S_TEST_SUCCESS( seqStatus, "connections");
	}
	else
	{
	    failTest = TRUE;
	    status.S_TEST_FAIL( status, "connections" );
	    seqStatus.S_TEST_FAIL( seqStatus, "connections" );
	}
    }


    //
    //  Update the status server.
    //

    cStaStatus::update( "state", "RUNNING", status );
    cStaStatus::flush( status );
    continueTesting( FALSE );


    if ( !failTest )
    {
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	response( DHS_CS_ERROR, seqStatus.message(), dhsStatus );
    }
}
