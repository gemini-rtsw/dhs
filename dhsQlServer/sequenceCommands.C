static char rcsid[] = "$Id: sequenceCommands.C,v 1.1.1.1 2002-11-24 20:29:27 brighton Exp $";
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
// dhsQlServer/src/sequenceCommands.C
//
// PURPOSE:
// Contains the implementation of the gemini sequence command handlers
//
// METHOD NAME(S)
// cQlsSequenceCmd::cQlsSequenceCmd
//			- Constructor for the class.
// cQlsSequenceCmd::cmdAbort
//			- Executed when a command is aborted.
// cQlsSequenceCmd::debugLevel	
//			- Executed to start the processing of a debug command.
// cQlsSequenceCmd::exit- Executed to start the processing of an exit command.
// cQlsSequenceCmd::init- Executed to start the processing of an init or reset
//			  command.
// cQlsSequenceCmd::resetHealth
//			- Executed to start the processing of a resetHealth 
//			  command.
// cQlsSequenceCmd::simulateLevel 
//			- Executed to start the processing of a simulate 
//			  command.
// cQlsSequenceCmd::test- Executed to start the processing of a test command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/27 21:11:52  nhill
// Implemented the test command.
//
// Revision 1.2  1998/04/15 23:04:47  nhill
// Updated to the newest version of the cDhsStatus class.
// Updated to use method specific callbacks in the command handlers.
//
// Revision 1.1  1997/10/22 17:45:55  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <unistd.h>

#define	DHS_NO_BULK
#include "dhs++.H"

#include "globals.H"
#include "qlsDhs.H"
#include "qlsStream.H"
#include "sequenceCommands.H"
#include "qlsDataset.H"
#include "qlsQlt.H"
#include "qlsSlave.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::cQlsSequenceCmd
//
// INVOCATION:
// n.a. Called automatically when an object is created.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initalize the test command class
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

		cQlsSequenceCmd::cQlsSequenceCmd
(
    void
)
{
    qscAbort = false;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::cmdAbort
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) True if the command was aborted, false otherwise.
//
// PURPOSE:
// Abort the test command
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

bool		cQlsSequenceCmd::cmdAbort
(
    void
)
{
    qscAbort = true;
    return true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::debugLevel
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

void		cQlsSequenceCmd::debugLevel
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cDhsAttrib	levelAttrib;		// Debug level attribute.
    char	*level;			// Debug level string.
    cQlsStatus	status;


    response( DHS_CS_BUSY, dhsStatus );


    //
    // Find the debug level attribute.
    //

    levelAttrib = find( "level", dhsStatus );


    //
    // Get the value of the debug level string.
    //

    levelAttrib.info( (void **) &level, dhsStatus );

    if ( dhsStatus == DHS_S_SUCCESS )
    {
	status.debugLevel( level );


	//
	// Set the DHS library debug level.
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
	}
	

	checkDhs( response( DHS_CS_DONE, dhsStatus ), 
		dhsStatus, status, return );


	//
	// Update the QLS state.
	//

	cQlsStatus::update( "debug", level, status );
	cQlsStatus::flush( status );
	status.S_DEBUG_LEVEL( status, level );
    }
    else
    {
	//
	// An error occured, return an error status and display a message.
	//

	dhsStatus = DHS_S_SUCCESS;
	checkDhs( response( DHS_CS_ERROR, dhsStatus ), 
		dhsStatus, status, return );
	status.E_CMD_FORMAT( status, name() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::exit
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

void		cQlsSequenceCmd::exit
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsStatus	status;

    status.S_EXIT_CMD( status );

    response( DHS_CS_DONE, dhsStatus );

    cQlsDhs::retry( false );
    checkDhs( cDhs::stop( dhsStatus ), dhsStatus, status, VOID );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::init
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

void		cQlsSequenceCmd::init
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsStatus	status;

    status.S_INIT( status );

    response( DHS_CS_DONE, dhsStatus );

    cQlsDhs::retry( true );
    checkDhs( cDhs::stop( dhsStatus ), dhsStatus, status, VOID );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::resetHealth
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

void		cQlsSequenceCmd::resetHealth
(
)
{
    cQlsStatus	status;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkDhs( response( DHS_CS_BUSY, dhsStatus ), dhsStatus, status, return );

    status.S_RESET_HEALTH( status, "GOOD" );
    status.resetHealth( status );

    checkDhs( response( DHS_CS_DONE, dhsStatus ), dhsStatus, status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::simulateLevel
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

void		cQlsSequenceCmd::simulateLevel
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cDhsAttrib	levelAttrib;
    char	*level;
    cQlsStatus	status;


    response( DHS_CS_BUSY, dhsStatus );


    //
    // Get the simulation level from the attribute list.
    //

    levelAttrib = find( "level", dhsStatus );
    levelAttrib.info( (void **) &level, dhsStatus );


    //
    // If the level was found, set the simulation level.
    //

    if ( dhsStatus == DHS_S_SUCCESS )
    {
	//
	// Simulation levels "VSM" and "FAST" turn simulation on. All
	// other modes are the same as normal operation.
	//

	if ( strcmp( level, "VSM" ) == 0 || strcmp( level, "FAST" ) == 0 )
	{
	    cQlsDhs::simulate( true );
	}
	else
	{
	    cQlsDhs::simulate( false );
	}


	//
	// Send the response.
	//

	checkDhs( response( DHS_CS_DONE, dhsStatus ), 
		dhsStatus, status, return );


	//
	// Update the QLS state.
	//

	cQlsStatus::update( "simulate", level, status );
	cQlsStatus::flush( status );


	//
	// Log a message indicating the simulation mode has changed.
	//

	status.S_SIM_LEVEL( status, level );
    }
    else
    {
	//
	// Simulation level could not be found, so the command format
	// is invalid.
	//

	dhsStatus = DHS_S_SUCCESS;
	checkDhs( response( DHS_CS_ERROR, dhsStatus ), 
		dhsStatus, status, return );
	status.E_CMD_FORMAT( status, name() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSequenceCmd::test
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

void		cQlsSequenceCmd::test
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsStatus	status;


    status.displayStop();

    status.S_TEST_CMD( status );

    cQlsQlt::testAll( status );
    cQlsSlave::testAll( status );

    if ( status.ok() )
    {
	response( DHS_CS_DONE, dhsStatus );
	status.S_TEST_SUC( status );
    }
    else
    {
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.S_TEST_FAIL( status );
    }
    status.display();
}
