static char rcsid[] = "$Id: sta.C,v 1.1.1.1 2002-11-24 20:30:18 brighton Exp $";
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
// dhsSta/src/sta.C
//
// PURPOSE:
// Contains the implementation of the cStaStatus class.
//
// MEHTOD NAME(S)
// cStaCommand::done - This method is execute when a command to the status
//		server is complete.
// cStaCommand::isDone - Returns true when all commands to the status
//		server are complete.
// cStaStatus::config - Read the status server configuration file.
// cStaStatus::connect - Open a connection to the status server.
// cStaStatus::disconnect - Close the connection to the status server.
// cStaStatus::healthSet - Callback routine for the health properties.
// cStaStatus::messageLog - Callback routine for the P_LOG property.
// cStaStatus::flush - Flush all status updates to the status server.
// cStaStatus::init - Initialize the status server.
// cStaStatus::statusServer - Callback routine to parse status server
//		information from the configuration file.
// cStaStatus::update - Update a status value in the status server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.16  2001/03/12 21:00:25  nhill
// Changed to allow the status class to be used before the connection to the status server
// is made.
//
// Revision 1.15  1998/05/25 23:06:12  nhill
// Fixed the code for ignoring recursive calls.
//
// Revision 1.14  1998/05/04 20:48:29  nhill
// Changed to not allow health to change for BAD to WARN.
// Fixed some of the checking for recursive execution.
//
// Revision 1.13  1998/04/24 21:21:48  nhill
// Moved this file from the dhsGen library.
//
// Revision 1.12  1998/04/23 22:21:10  nhill
// Removed the use of copy constructors.
// Changed E_DHS errors to change the health of the system.
//
// Revision 1.11  1998/04/21 05:33:33  nhill
// Pretty much a complete rewrite. The cStaClass is now the cStaStatus
// class, and inherits from the cDhsStatus class. The new class uses the
// cDhsStatus properties to log messages and update health to the status
// server.
//
// Revision 1.10  1998/02/24 01:24:15  nhill
// Bug fix.
//
// Revision 1.9  1997/12/04 19:29:34  nhill
// Changed the locking around commands in the flush method.
//
// Revision 1.8  1997/10/30 22:20:07  nhill
// Added code to ensure the class has been initialized.
//
// Revision 1.7  1997/10/01 16:34:36  nhill
// Fixed the disconnect method so that it will close connections not opened
// by the connect method.
//
// Revision 1.6  1997/09/26 18:07:24  nhill
// Added method cStaCommand::isDone.
//
// Revision 1.5  1997/09/18 17:43:36  nhill
// Fixed up the error checking for the DHS library.
//
// Revision 1.4  1997/09/04 20:38:45  nhill
// Added a check to prevent recursive messages from being displayed
// when the connection to the status server failes.
//
// Revision 1.3  1997/07/10 19:28:29  nhill
// *** empty log message ***
//
// Revision 1.2  1997/06/13 21:17:14  nhill
// Added functions to read status server info from configuration file.
//
// Revision 1.1  1997/06/11 19:50:35  nhill
// Initial revision
//
// Revision 1.4  1997/05/01 17:11:23  nhill
// Changed the message string output to const.
//
// Revision 1.3  1997/04/24 19:35:21  nhill
// Removed the cStaCommand::response method.
//
// Revision 1.2  1997/04/16 16:35:06  nhill
// Changed the name of the include files.
//
// Revision 1.1  1997/04/15 23:06:56  nhill
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

#include "genRwLock.H"
#include "dhsSta.H"
#define	DHS_NO_BULK
#define	DHS_NO_SERVER
#include "dhs++.H" 

#include "localCSta.H"


int			cStaCommand::scCounter = 0;
char			*cStaStatus::ssAddress = NULL;
bool			cStaStatus::ssConnectAlloc = false;
char			*cStaStatus::ssId = NULL;
bool			cStaStatus::ssIsInit = false;
cDhsConnection		*cStaStatus::ssPConnection = NULL;
cMutex			cStaStatus::ssMutex;
const char		*cStaStatus::ssPrefix = NULL;
cStaCommand		*cStaStatus::ssPStaCommand = NULL;
const cStaStatus::cStatusProperty
			*cStaStatus::ssHealth = NULL;

msgPropertyDef( cStaStatus, P_LOG, cStaStatus::messageLog );
msgPropertyDef( cStaStatus, P_HEALTH_BAD, cStaStatus::healthSet );
msgPropertyDef( cStaStatus, P_HEALTH_WARN, cStaStatus::healthSet );

msgText( cStaStatus, E_SS_OPEN, NULL, 
	"Could not open a connection to the status server" );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaCommand::done
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) - (DHS_CMD_STATUS) Command completion status.
// (>) - (char *) Completion string.
// (>) - (const cDhsAvList) Response av list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when a cStaCommand is complete
//
// DESCRIPTION:
// Decrement the command counter and broadcast to the condition variable.
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

void	cStaCommand::done
(
    DHS_CMD_STATUS,
    char *,
    const cDhsAvList &
)
{
    delete this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaCommand::isDone
//
// INVOCATION:
// dhsEventLoop( DHS_ELT_COND, isDone, NULL, &dhsStatus );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg (void *) Unused argument.
//
// FUNCTION VALUE:
// (bool) True if all commands are complete.
//
// PURPOSE:
// Test to see if all commands to the status server are complete
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

bool		cStaCommand::isDone
(
    void	*
)
{
    return( scCounter == 0 );
}
//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::config
//
// INVOCATION:
// cStaStatus::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the status server configuration information
//
// DESCRIPTION:
// Execute the config_get function to interpret the statusServer line in
// the configuration file.
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

void		cStaStatus::config
(
    cStaStatus	&status		// (mod) The function return status.
)
{
    checkStat( status, return );

    checkConfig( config_get( "statusServer", statusServer ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::connect
//
// INVOCATION:
// cStaStatus::connect( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a DHS connection to the status server
//
// DESCRIPTION:
// - Ensure the class is initialized.
// - If neccessary, create a new connection object.
// - Open the connection.
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

void		cStaStatus::connect
(
    cStaStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    checkStat( status, return );


    //
    // Ensure that the class has been initialized.
    //

    if ( ! ssIsInit )
    {
	status.E_INIT( status, "cStaStatus" );
	return;
    }


    if ( ssPConnection == NULL )
    {
	ssPConnection = new cDhsConnection;
	ssConnectAlloc = true;
    }

    checkDhs( ssPConnection->open( ssAddress, ssId, dhsStatus ), 
	    dhsStatus, status, NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::disconnect
//
// INVOCATION:
// cStaStatus::disconnect( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Disconnect from the status server.
//
// DESCRIPTION:
// - If a connection exists, disconnect.
// - If the connection was allocated by the connect method, delete the
//	connection.
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

void		cStaStatus::disconnect
(
    cStaStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    checkStat( status, return );


    if ( ssPConnection != NULL )
    {
	cStaCommand::waitDone();
	checkDhs( ssPConnection->close( dhsStatus ), dhsStatus, status, VOID );
    }

    if ( ssConnectAlloc )
    {
	delete ssPConnection;
    }
    ssPConnection = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::healthSet
//
// INVOCATION:
// N.A. executed as a callback routine
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (cDhsStatus &) The status that caused the callback
// (>) prop (cDhsStatus::cStatusProperty &) The property that caused the 
//		callback
// (>) string (const char *) The string being set.
//
// FUNCTION VALUE:
// (bool) True if processing should continue.
//
// PURPOSE:
// Set the health as appropriate
//
// DESCRIPTION:
// - Ensure the method isn't being called recursivly.
// - Update the health record.
// - Update the healthDesc record.
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

bool		cStaStatus::healthSet
(
    cDhsStatus	&status,	// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
    		&prop,		// (in)  The property that caused the callback.
    const char	*		// (in)  The string being set.
)
{
    cDhsStatus	status2;
    static pthread_t
		thread = 0;
    pthread_t	oldThread;


    //
    // Ensure this function isn't called recursivly, which could happen if
    // the connection to the status server is bad.
    //

    if ( thread == pthread_self() )
    {
	return( true );
    }
    oldThread = thread;
    thread = pthread_self();

    if ( &prop == &( cStaStatus::P_HEALTH_BAD ) )
    {
	update( "health", "BAD", status2 );
	ssHealth = &prop;
    }
    else if ( &prop == &( cStaStatus::P_HEALTH_WARN ) && 
	    ssHealth != &( cStaStatus::P_HEALTH_BAD ) )
    {
	update( "health", "WARNING", status2 );
	ssHealth = &prop;
    }
    update( "healthDesc", status.message() + status.maxCodeLen() + 1, 
	    status2 );
    flush( status2 );

    thread = oldThread;

    return( true );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::messageLog
//
// INVOCATION:
// N.A. executed as a callback routine
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (cDhsStatus &) The status that caused the callback
// (>) prop (cDhsStatus::cStatusProperty &) The property that caused the 
//		callback
// (>) string (const char *) The string being set.
//
// FUNCTION VALUE:
// (bool) True if processing should continue.
//
// PURPOSE:
// Send a log message to the status server.
//
// DESCRIPTION:
// - Make sure the class is initialized.
// - Ensure the connection is open.
// - Prevent recursive execute that may be caused by errors in the
//	connection to the status server.
// - Send the message to the status server.
// - If the connection is dead, close the connection.
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

bool		cStaStatus::messageLog
(
    cDhsStatus	&status,	// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
    		&,		// (in)  The property that caused the callback.
    const char	*string		// (in)  The string being set.
)
{
    cStaCommand	*command;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    bool	isDhs;
    cDhsStatus	status2;
    pthread_t	oldThread;
    static pthread_t
    		thread = 0;


    //
    // The test for DHS messages is a hack. There is a problem which occurs
    // when a DHS debug or error message occurs as the result of logging
    // a message. The apply method below could cause new debug or error
    // messages, which cause new applys, which cause new message, ect.
    // The ideal solution would be to not log any messages that are produced
    // as a result of the apply method, but to log messages produced for
    // any other reason. Unfortunately, because the apply method could
    // run the event loop and process event unrelated to the apply, and
    // because there isn't enough information to distinguish between 
    // messages that should be displayed and messages that shouldn't be
    // displayed, some messages that should be displayed may be missed.
    //

    if ( status == status.S_DHS || status == status.E_DHS )
    {
	isDhs = true;
    }
    else
    {
	isDhs = false;
    }


    //
    // Ensure that the class has been initialized.
    //

    if ( ! ssIsInit )
    {
	return( true );
    }


    if ( ssPConnection != NULL && ssPConnection->isOpen() )
    {
	//
	// Prevent recursive execution of this function that might be caused by
	// a failure in the connection to the status server.
	//

	if ( thread == pthread_self() && isDhs )
	{
	    return( true );
	}
	oldThread = thread;
	thread = pthread_self();


	command = new cStaCommand( "staEventLog" );
	checkDhs( command->add( "message", DHS_DT_STRING, string, dhsStatus ), 
		dhsStatus, status, delete command; return( true ) );
	command->apply( *ssPConnection, dhsStatus );


	//
	// Close the connection if it appears to be dead.
	//

	if ( dhsStatus == DHS_E_CON_LOST || dhsStatus == DHS_E_CON_INVALID || 
		dhsStatus == DHS_E_CON_NOT_FOUND )
	{
	    ssPConnection = NULL;
	}

	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    const char *msg;
	    DHS_STATUS	s( DHS_S_SUCCESS );

	    status.E_DHS( status, 
		    "command->apply( *ssPConnection, dhsStatus )" );
	    while ( s == DHS_S_SUCCESS )
	    {
		msg = cDhs::message( s );
		if ( s == DHS_S_SUCCESS )
		{
		    status += msg;
		    cDhs::messageClear( s );
		}
	    }
	    delete command;
	}

	thread = oldThread;
    }
    return( true );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::flush
//
// INVOCATION:
// cStaStatus::flush( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDhsStatus &) Funtion return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send the status update command to the status server
//
// DESCRIPTION:
// If there is an unsend status update command, send it to the status
// server.
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

void		cStaStatus::flush
(
    cDhsStatus	&status		// (mod) Funtion return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStaCommand	*pTmpCommand;


    checkStat( status, return );


    //
    // Ensure that the class has been initialized.
    //

    if ( ! ssIsInit )
    {
	status.E_INIT( status, "cStaStatus" );
	return;
    }


    ssMutex.lock();
    if ( ssPStaCommand != NULL )
    {
	pTmpCommand = ssPStaCommand;
	ssPStaCommand = NULL;
	ssMutex.unlock();
	if ( ssPConnection != NULL && ssPConnection->isOpen() )
	{
	    checkDhs( pTmpCommand->apply( *ssPConnection, dhsStatus ), 
		    dhsStatus, status, VOID );
	    if ( ! status.ok() )
	    {
		delete pTmpCommand;
		status( status.S_SUCCESS );
	    }
	}
	else
	{
	    delete pTmpCommand;
	}
    }
    else
    {
	ssMutex.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::init
//
// INVOCATION:
// cStaStatus::init( connection, prefix, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (const cDhsConnection &) Connection to the status server.
// (>) prefix (const char *) Prefix for this application.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set up the connection to the status server
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

void		cStaStatus::init
(
    cDhsConnection		// (in)  Connection to the status server.
    		&connection,
    const char	*prefix,	// (in)  Prefix for this applictaion
    cStaStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    if ( ssPConnection != NULL && ssConnectAlloc )
    {
	delete ssPConnection;
	ssPConnection = NULL;
    }

    ssConnectAlloc = false;
    ssPConnection = &connection;
    init( prefix, status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::init
//
// INVOCATION:
// cSta::init( prefix, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) prefix (const char *) Prefix for this application.
// (!) status (cStaStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set up the connection to the status server
//
// DESCRIPTION:
// - Add the status server properties to the status codes defined in the
//	cDhsStatus class.
// - Set the application prefix.
// - Indicate the class is initialized.
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

void		cStaStatus::init
(
    const char	*prefix,	// (in)  Prefix for this application.
    cStaStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    cStaStatus::S_CONFIG_READ += P_LOG;
    cStaStatus::E_CONFIG += P_LOG | P_HEALTH_BAD;
    cStaStatus::E_CONFIG_KWD += P_LOG | P_HEALTH_BAD;
    cStaStatus::E_DHS += P_LOG | P_HEALTH_WARN;
    cStaStatus::E_INIT += P_LOG;
    cStaStatus::E_MEMORY += P_LOG | P_HEALTH_BAD;
    cStaStatus::E_SYSTEM += P_LOG | P_HEALTH_WARN;
    ssPrefix = prefix;
    ssIsInit = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::statusServer
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of configuration file tokens.
// (>) tokens (char **) List of configuration file tokens.
//
// FUNCTION VALUE:
// (boolean) True if the line parsed correctly, false otherwise.
//
// PURPOSE:
// Parse a statusServer line from the configuration file
//
// DESCRIPTION:
// - Check that the correct number of tokens were on the line.
// - Save the status server name.
// - Save the status server address.
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

boolean		cStaStatus::statusServer
(
    int		numTokens,	// (in)  Number of configuration file tokens.
    char	**tokens	// (in)  List of configuration file tokens.
)
{
    //
    //  Check that the number of tokens is correct.
    //

    if ( numTokens != 3 )
    {
	return( false );
    }

    if ( ssId != NULL )
    {
	free( ssId );
	ssId = NULL;
    }

    if ( ssAddress != NULL )
    {
	free( ssAddress );
	ssAddress = NULL;
    }

    ssId = strsav( tokens[1] );
    ssAddress = strsav( tokens[2] );

    if ( ssId == NULL || ssAddress == NULL )
    {
	return( FALSE );
    }
    else
    {
	return( TRUE );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStatus::update
//
// INVOCATION:
// cStaStatus::update( record, value, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) record (const char *) SAD record to update.
// (>) value (const char *) New value of the record.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add an attribute to update the specified record to the update command
//
// DESCRIPTION:
// - If no unsent update command exists, create one.
// - Add an attribute to the command updating the record.
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

void		cStaStatus::update
(
    const char	*record,	// (in)  SAD record to update.
    const char	*value,		// (in)  New value of the record.
    cDhsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    // Ensure that the class has been initialized.
    //

    if ( ! ssIsInit )
    {
	status.E_INIT( status, "cStaStatus" );
	return;
    }


    if ( ssPConnection != NULL && ssPConnection->isOpen() )
    {
	//
	// The mutex prevents to threads from creating a new command at the
	// same time, or from adding attributes to the command at the same time.
	//

	ssMutex.lock();


	//
	// If there isn't already an unsent command, create a new command.
	//

	if ( ssPStaCommand == NULL )
	{
	    ssPStaCommand = new cStaCommand( "staUpdate" );


	    //
	    // Add an attribute specifying the subsystem.
	    //

	    checkDhs( ssPStaCommand->add( "subSystem", DHS_DT_STRING, ssPrefix, 
		    dhsStatus), dhsStatus, status, ssMutex.unlock(); return );
	}


	//
	// Add an attribute to the command updated the status record.
	//

	checkDhs( ssPStaCommand->add( record, DHS_DT_STRING, value, dhsStatus ),
		dhsStatus, status, VOID );

	ssMutex.unlock();
    }
}
