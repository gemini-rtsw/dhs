static char rcsid[] = "$Id: msg.C,v 1.1.1.1 2002-11-24 20:30:30 brighton Exp $";
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
// dhsStatus/src/msg.C
//
// PURPOSE:
// Contains the definitions of the status messages.
//
// MEHTOD NAME(S)
// cStaStat::flush - Flush all status values to the status receivers.
// cStaStat::init - Initialize the cStaStat class.
// cStaStat::healthSet - Set the health record for the status server.
// cStaStat::messageLog - Log a message for the status server.
// cStaStat::update - Update a status item for the status server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.14  1998/06/24 23:17:39  nhill
// Added code to not flush the status to the channels before the channel
// structures have been initialized.
//
// Revision 1.13  1998/06/19 21:16:33  nhill
// Changed the name of the cStaStatus class to cStaStat.
// Added functions to make the cStaStat class emulate the cStaStatus class,
// but updating status channels directly instead of sending commands to
// the status server.
//
// Revision 1.12  1998/06/02 19:07:07  nhill
// Added message E_CHANNEL_NAME.
//
// Revision 1.11  1998/05/20 04:57:22  jaeger
// Changed the text of the "NO MONITORING" message.
//
// Revision 1.10  1998/05/12 20:22:28  nhill
// Shortened some message ids.
//
// Revision 1.9  1998/04/15 20:57:48  nhill
// Removed the cStaStatus::info and cStaStatus::msgDisplay methods.
// Updated the message definitions to the latest version of the cDhsStatus
// class.
//
// Revision 1.8  1998/03/06 18:23:03  jaeger
// Added a NO_MONITOR message.
//
// Revision 1.7  1998/02/05 21:19:21  jaeger
// Added messages for monitoring resource and a info method.
//
// Revision 1.6  1998/01/08 21:01:05  jaeger
// Added a message for the resetting health.
//
// Revision 1.5  1997/10/01 17:12:50  nhill
// Added record name to the E_CA message.
//
// Revision 1.4  1997/09/05 18:55:05  nhill
// Fixed the version display.
//
// Revision 1.3  1997/06/13 20:41:22  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.2  1997/04/16 22:28:06  nhill
// Added some information to the channel access error message.
//
// Revision 1.1  1997/03/27 22:15:42  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "dhsGenStatus.H"

#include "globals.H"
#include "staChannel.H"

msgText( cStaStat, S_CA_CONNECT, P_DEBUG_FULL,
	    "Creating channel access connection to record `%s'." );
msgText( cStaStat, S_CMD_RECEIVED, P_LOG | P_DEBUG_MIN,
	    "%s command received." );
msgText( cStaStat, S_CONNECT, P_LOG | P_DEBUG_MIN,
	    "Connection request received from `%s' on `%s'." );
msgText( cStaStat, S_DEBUG_LEVEL, P_LOG | P_DEBUG_MIN,
	    "Debug level set to `%s'." );
msgText( cStaStat, S_DISCONNECT, P_LOG | P_DEBUG_MIN,
	    "Disconnect request received from `%s' on `%s'." );
msgText( cStaStat, S_EXITING, P_LOG | P_DEBUG_MIN,
	    "Status server exiting." );
msgText( cStaStat, S_INITIALIZING, P_LOG | P_DEBUG_MIN,
	    "Initalizing Status server." );
msgText( cStaStat, S_MONITORING, P_LOG | P_DEBUG_MIN,
	    "The monitoring of spaces has started." );
msgText( cStaStat, S_NO_MONITORING, P_LOG | P_HEALTH_WARN,
	    "The monitoring of spaces was NOT started" );
msgText( cStaStat, S_SET_HEALTH, P_LOG | P_DEBUG_MIN,
	    "Health set to `%s'." );
msgText( cStaStat, S_SIMULATE_LEVEL, P_LOG | P_DEBUG_MIN,
	    "Simulation level set to `%s'." );
msgText( cStaStat, S_STOP_MONITOR, P_LOG | P_DEBUG_MIN,
	    "The monitoring of spaces has stopped." );
msgText( cStaStat, S_TEST_FAILURE, P_LOG | P_HEALTH_BAD,
	    "Test command failed." );
msgText( cStaStat, S_TEST_SUCCESS, P_LOG | P_DEBUG_MIN,
	    "Test command succeeded." );
msgText( cStaStat, S_VERSION, P_NULL,
	    "%s\n%s" );
msgText( cStaStat, E_BAD_CHALIAS, P_LOG | P_HEALTH_WARN,
	    "Bad channel alias, `%s' given in config file." );
msgText( cStaStat, E_BAD_EPICS_VAL, P_LOG,
	    "Bad Epics value given, data of type." );
msgText( cStaStat, E_CA, P_LOG,
	    "Error %d on EPICS record %s\n"
	    "in channel access function:\n%s\n%s" );
msgText( cStaStat, E_CA_LOG, P_NULL,
	    "Error %d on EPICS record %s\n"
	    "in channel access function:\n%s\n%s" );
msgText( cStaStat,  E_CDB, P_LOG,
	   "Error in db library: `%s' \nfor function `%s'." );
msgText( cStaStat, E_CHANNEL_NAME, P_LOG,
	    "Invalid channel name: `%s'" );
msgText( cStaStat, E_CHANNEL_MISSING, P_LOG,
	    "Could not find a channel matching alias `%s' for system '%s'." );
msgText( cStaStat, E_CONFIG_TYPE, P_NULL,
	    "Error reading `%s' configuration keyword, expected '%s' data" );
msgText( cStaStat, E_CWD, P_LOG | P_HEALTH_BAD,
	    "Error finding the current working directory." );
msgText( cStaStat, E_DB_TEST_FAIL, P_LOG | P_HEALTH_BAD,
	    "Database test failed." );
msgText( cStaStat, E_DEBUG_CMD, P_LOG,
	    "Invalid debugLevel command: `%s'" );
msgText( cStaStat, E_DF_CMD, P_LOG | P_HEALTH_WARN,
	    "Warning, Error in the df command :\n`%s%s'.");
msgText( cStaStat, E_EVENTLOG_CMD, P_LOG,
	    "Invalid event log command: `%s'" );
msgText( cStaStat, E_GEN, P_LOG,
	    "Error in gen library for function `%s.'" );
msgText( cStaStat, E_INVALID_CMD, P_LOG,
	    "Invalid command: `%s'" );
msgText( cStaStat, E_MD_TEST_FAIL, P_LOG | P_HEALTH_WARN,
	    "Magnetic disk test failed." );
msgText( cStaStat, E_MON_CONFIG, P_LOG | P_HEALTH_WARN,
	    "Failed to read monitoring record names from\nconfiguration file.\n"
	    "Records `resourceArraySize', `resource%%d:inUse', `resource%%d:max',\n"
	    "`resource%%d:name', `resource%%d:fullname', `resource%%d:type',\n"
	    "`resource%%d:used', and `resource%%d:units' must all be present.\n" );
msgText( cStaStat, E_MON_TEST_FAIL, P_LOG | P_HEALTH_WARN,
	    "Monitoring test failed." );
msgText( cStaStat, E_MON_THREAD, P_LOG,
	    "Error in monitoring pthread function." );
msgText( cStaStat, E_NO_ALIASES, P_LOG | P_HEALTH_BAD,
	    "No channel aliases defined for reources `%s'." );
msgText( cStaStat, E_PATH, P_LOG | P_HEALTH_BAD,
	    "Invalid path in config file: `%s'" );
msgText( cStaStat, E_SDB, P_LOG | P_HEALTH_BAD,
	    "Error in the SYBASE db library: `%s'.\nfor function `%s'." );
msgText( cStaStat, E_SIMULATE_CMD, P_LOG,
	    "Invalid simulateLevel command: `%s'" );
msgText( cStaStat, E_STAUPDATE_CMD, P_LOG,
	    "Invalid status update command: `%s'" );
msgText( cStaStat, E_SUBSCRIBE_CMD, P_LOG,
	    "Invalid `_dhsSubscribe' command." );
msgText( cStaStat, E_SUBSYSTEM, P_LOG,
	    "Error in the dhsGen subsystem class method:\n%s" );
msgText( cStaStat, E_UNSUBSCRIBE_CMD, P_LOG,
	    "Invalid `_dhsUnsubscribe' command." );
msgText( cStaStat, E_USAGE, P_NULL,
	    "Usage:\n"
	    "    dhsStatus [-V] [-debug {NONE|MIN|FULL}]\n"
	    "                   [-simulate {NONE|VSM|FULL|FAST}]" );


bool		cStaStat::ssLogStatus = false;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStat::flush
//
// INVOCATION:
// cStaStat::flush
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStatus &) The status to change.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Flush all status values to the status receiver
//
// DESCRIPTION:
// Use the cStaChannel::chFlush to send all status values.
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

void		cStaStat::flush
(
    cStaStat	&status		// (mod) Inherited function status.
)
{
    pthread_t	oldThread;
    static pthread_t	
		thread = 0;


    if ( thread != pthread_self() && ssLogStatus )
    {
	oldThread = thread;
	thread = pthread_self();

	cStaChannel::chFlush( status );

	thread = oldThread;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStat::init
//
// INVOCATION:
// cStaStat::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function replaces the callback functions assigned to the P_LOG
// and P_HEALTH* properties with ones taylored for the dhsStatus servers
// needs
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

void		cStaStat::init
(
    cStaStat	&		// (mod) Inherited function status.
)
{
    cStaStat::P_LOG.callback( cStaStat::messageLog );
    cStaStat::P_HEALTH_BAD.callback( cStaStat::healthSet );
    cStaStat::P_HEALTH_WARN.callback( cStaStat::healthSet );

    cDhsSubsystem::cStatus::E_NOT_FOUND += P_LOG;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStat::healthSet
//
// INVOCATION:
// N.A. This is a property callback function.
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
// Change the health of the status server.
//
// DESCRIPTION:
// If the property is P_HEALTH_BAD, set the health channel to bad.
// If the property is P_HEALTH_WARN, set the health channel to warning.
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

bool		cStaStat::healthSet
(
    cDhsStatus	&,		// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
    		&prop,		// (in)  The property that caused the callback.
    const char	*string		// (in)  The string being set.
    
)
{
    cStaStat	status;


    if ( &prop == &( cStaStatus::P_HEALTH_BAD ) )
    {
	update( "health", "BAD", status );
	status.S_SET_HEALTH( status, "BAD" );
    }
    else if ( &prop == &( cStaStatus::P_HEALTH_WARN ) )
    {
	update( "health", "WARNING", status );
	status.S_SET_HEALTH( status, "WARNING" );
    }
    update( "healthDesc", string, status );
    flush( status );

    return( true );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStat::messageLog
//
// INVOCATION:
// N.A. This is a property callback function.
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
// Log a message to the EPICS database or DHS status acceptors.
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

bool		cStaStat::messageLog
(
    cDhsStatus	&,		// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
    		&,		// (in)  The property that caused the callback.
    const char	*string		// (in)  The string being set.
    
)
{
    cStaStat	status;
    bool	oldStatus;
    pthread_t	oldThread;
    static pthread_t	
		thread = 0;


    if ( thread != pthread_self() && ssLogStatus )
    {
	oldThread = thread;
	thread = pthread_self();
	oldStatus = logStatus( false );
	cStaChannel::logMessage( string, status );
	logStatus( oldStatus );
	thread = oldThread;
    }

    return( true );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaStat::update
//
// INVOCATION:
// char	*channelName;
// char *channelValue;
// cStaStat::status;
// cStaStat::update( channelName, channelValue, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) channelName (const char *) Name of the channel to update.
// (>) channelValue (const char *) Value of the channel to update.
// (!) status (cStaStat) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Update a status parameter
//
// DESCRIPTION:
// Find the channel and updates its value.
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

void		cStaStat::update
(
    const char	*channelName,	// (in)  Name of the channel to update.
    const char	*channelValue,	// (in)  Value of the channel.
    cStaStat	&status		// (mod) Function return status.
)
{
    cStaChannel	*pChannel;
    pthread_t	oldThread;
    static pthread_t	
		thread = 0;


    if ( thread != pthread_self() && ssLogStatus )
    {
	oldThread = thread;
	thread = pthread_self();

	pChannel = cStaChannel::find( PREFIX, channelName, status );
	pChannel->put( channelValue, status );

	thread = oldThread;
    }
}
