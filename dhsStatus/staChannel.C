static char rcsid[] = "$Id: staChannel.C,v 1.1.1.1 2002-11-24 20:30:37 brighton Exp $";
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
// dhsStatus/src/staChannel.C
//
// PURPOSE:
// Contains the implementation of the cStaChannel class
//
// This class is the interface to the EPICS channel access library.
//
// MEHTOD NAME(S)
// cStaChannel::cStaChannel - Constructor for the cStaChannel class.
// cStaChannel::~cStaChannel - Destructor for the cStaChannel class.
// cStaChannel::close - Close a channel.
// cStaChannel::config - Read channel information from a configuration file.
// cStaChannel::channelConfig - Parse a channel line from the configuration 
//			file.
// cStaChannel::ecpisConfig - Parse the EPICS host information from the 
//			configuration file.
// cStaChannel::locationSiteConfig - Read the location/site information from
//			the configuration file.
// cStaChannel::channelAdd - Add a new channel to the list of channels.
// cStaChannel::channelDelete - Remove a channel from the list of channels.
// cStaChannel::chFlush - Flush all output to the EPICS database.
// cStaChannel::dirtyAll - Mark all channels as being dirty.
// cStaChannel::exit - Delete all channels and exit the channel access library.
// cStaChannel::init - Initialize the channel access library.
// cStaChannel::logMessage - Send a message to the logMessage channel.
// cStaChannel::newIndex - Create a new indexed channel.
// cStaChannel::test - Verify a channel works ok.
// cStaChannel::testAll - Test all channels.
// cStaChannel::find - Find a channel object matching the specified alias 
//			and system.
// cStaChannel::mergeConfig - Create the merge objects for each merge keyword
//			in the configuration file.
// cStaChannel::put - Send a value to a channel.
// cStaChannel::cKey::operator< - < operator allows stl ordered list of keys.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.25  1998/08/05 18:51:46  nhill
// Stopped sending channel access error messages to the history log.
// This could be improved to use a queue to send the messages unless the
// channel is the log message channel, or to send the log messages to the
// DHS service status acceptors only.
//
// Revision 1.24  1998/06/24 23:18:15  nhill
// Improved error recovery after cahnnel access failures.
// fixed some bugs.
//
// Revision 1.23  1998/06/22 22:21:29  jaeger
// Fixed a memory access bug.
//
// Revision 1.22  1998/06/19 18:08:37  nhill
// Changed the cStaStatus class name to cStaStat.
// Changed to update status values via the cStaStat class.
//
// Revision 1.21  1998/06/16 22:20:55  nhill
// Added some error checking.
//
// Revision 1.20  1998/06/02 19:07:48  nhill
// Added a check for messages aliases which exceed the expected length.
//
// Revision 1.19  1998/05/12 20:23:02  nhill
// Changed to allow dhsService status acceptors.
//
// Revision 1.18  1998/04/15 20:59:11  nhill
// Updated the documentation.
// Updated to the latest version of the cDhsStatus class.
//
// Revision 1.17  1998/03/16 21:42:29  cockayne
// Added code to support sonfiguration of location and site.
// Ensured scDoneMutex is unlocked wherever necessary.
//
// Revision 1.16  1998/03/16 05:09:50  jaeger
// Fixed up the scDoneMutext lock, hopefully for th e last time.
//
// Revision 1.15  1998/03/13 21:47:23  jaeger
// Added some bits to ensure scDoneMutex get unlocked if an error occurs.
//
// Revision 1.14  1998/03/06 20:28:01  jaeger
// Reconnects to EPICS when channel lost (check for channel lost added),
// Now optionally does a scDoneMutex.lock, this is because logMessage
// needs this lock too.
//
// Revision 1.13  1998/02/17 00:20:48  jaeger
// Put some locks in so the sending of channel access commands occurs
// in a single thread.
//
// Revision 1.12  1998/02/05 21:21:31  jaeger
// Fixed a bug with indexed EPICS channels (rwlock instead of rwunlock).
// Fixed a bug in put, instead of using check macro did a status.ok() after
// the function call.
//
// Revision 1.11  1998/01/08 21:00:14  jaeger
// Changed a comparison with status.S_SUCCESS to a status.ok().
//
// Revision 1.10  1997/12/23 23:20:45  nhill
// Moved method cStaChannel::chFlush into this file from the header file.
//
// Revision 1.9  1997/11/12 23:15:02  cockayne
// Modified logMessage() method to call ca_pend_event() in a tight loop
// until a callback is called, releasing the loop.
//
// Revision 1.8  1997/10/03 22:59:34  nhill
// Changed to work with the object space stl library.
//
// Revision 1.7  1997/10/01 17:13:10  nhill
// Added record name to the channel access errors.
//
// Revision 1.6  1997/08/19 20:11:30  nhill
// A lint type fix.
//
// Revision 1.5  1997/06/13 20:41:22  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.4  1997/04/28 22:56:33  nhill
// Removed debugging output.
//
// Revision 1.3  1997/04/24 20:04:48  nhill
// Added some "const" keywords.
//
// Revision 1.2  1997/04/16 22:28:28  nhill
// Changed to the of the flush function to chFlush.
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
#include <pthread.h>

#include "genCond.H"

#include "globals.H"

extern "C"
{
#include "gen_config.h"
}

#include "staDhs.H"    
#include "staChannel.H"
#include "subscriber.H"

pthread_t		cStaChannel::scElThread = 0;
bool			cStaChannel::scElExit = false;
bool			cStaChannel::scEpicsActive = false;
void			*cStaChannel::scList = NULL;
cStaChannel		*cStaChannel::scLogChannel = NULL;
cThreadLock		cStaChannel::scMutex;
char			*cStaChannel::scSadHost = NULL;
cRwLock			cStaChannel::scRwLock;
float			cStaChannel::scTimeout = 30.;
cStaChannel::simulateMode cStaChannel::scSimulateMode = 
				cStaChannel::SIMULATE_NONE;
bool			cStaChannel::scPutDone;				
char			*cStaChannel::scEpicsPrefix = NULL;

static cCond		elCond;


static int const	CHANNEL_NAME_LEN = 32;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::cStaChannel
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) system (char *) System the channel is used for (* for all systems).
// (>) name (char *) Channel name.
// (>) alias (char *) Channel alias.
// (>) epics (bool) If true, record should be updated in the epics database.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for an EPICS channel
//
// DESCRIPTION:
// - Intialize the data members.
// - Create a map for the list of channels if necessary.
// - Insert the channel into the map.
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

		cStaChannel::cStaChannel
(
    const char	*system,	// (in)  System the channel applies to.
    const char	*name,		// (in)  Name of the EPICS channel.
    const char	*alias,		// (in)  Alias for the EPICS channel.
    bool	epics		// (in)  If true, record should be updated 
    				//	 in the epics database.
)
{
    tChMap	*pChMap;	// Pointer to the channel map.


    scName = strsav( name );
    scAlias = strsav( alias );
    strncpy( scSystem, system, cDhsSubsystem::SYSTEM_NAME_LENGTH );
    scConnected = false;
    scDataSize = 2048;
    scData = new char[scDataSize];
    scData[0] = '\0';
    scTime = 0;
    scMerger = NULL;
    scMessageSize = 2048;
    scMessage = new char[scMessageSize];
    scMessage[0] = '\0';
    scEpics = epics;
    if ( scEpics )
    {
	scEpicsActive = true;
    }

    if ( strstr( scAlias, "%d" ) == NULL )
    {
	scIndexed = false;
	scDirty = true;
    }
    else
    {
	scIndexed = true;
	scDirty = false;
    }
    
    scRwLock.wLock();
    if ( scList == NULL )
    {
	scList = new tChMap;
    }
    pChMap = (tChMap *) scList;

    (*pChMap).insert( tChMap::value_type( cStaChannel::cKey( scAlias, 
	    scSystem ), this ) );
    scRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::~cStaChannel
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
// Destructor for a channel structure
//
// DESCRIPTION:
// Free the memory allocated to the channel.
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

		cStaChannel::~cStaChannel
(
    void
)
{
    cStaStat	status;		// Status value.


    close( status );

    if ( this == scLogChannel )
    {
	scLogChannel = NULL;
    }

    if ( scName != NULL )
    {
	free( scName );
    }
    if ( scAlias != NULL )
    {
	free( scAlias );
    }

    delete [] scMessage;
    scMessage = NULL;
    scMessageSize = 0;

    delete [] scData;
    scData = NULL;
    scDataSize = 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::close
//
// INVOCATION:
// channel.close( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Close a channel access channel
//
// DESCRIPTION:
// Execute the ca_clear_channel function.
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

void		cStaChannel::close
(
    cStaStat	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    if ( scConnected )
    {
	chFlush( status );

	if ( scEpics )
	{
	    scMutex.lock();
	    checkCa( ca_clear_channel( scValChannel ), status, VOID, scName );
	    checkCa( ca_clear_channel( scMsgChannel ), status, VOID, scName );
	    scMutex.unlock();
	}
	
	chFlush( status );

	scConnected = false;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::config
//
// INVOCATION:
// cStaChannel::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// read the configuration data for the EPICS channels
//
// DESCRIPTION:
// Use the config library config_get function to read the data.
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

void		cStaChannel::config
(
    cStaStat	&status		// (mod) Function return status.
)
{
    int		st;


    checkStat( status, return );


    //
    // Delete any existing channels.
    //

    channelDelete( status );

    checkConfig( config_get( "epics", epicsConfig ), status, return );
    checkConfig( config_get( "locationSite", locationSiteConfig), 
	    status, return );
    checkConfig( config_get( "channel", channelConfig ), status, return );
    st = config_get( "merge", mergeConfig );
    if ( st != GC_SUCCESS && st != GC_E_KEY_NOT_FOUND )
    {
	status.E_CONFIG( status );
	status += gc_msg;	
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::channelConfig
//
// INVOCATION:
// n.a. Called automatically from the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the channel information from the configuration file.
//
// DESCRIPTION:
// Create a new cStaChannel object and add it to the list.
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

boolean		cStaChannel::channelConfig
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStaStat	status;		// Status code.
    cDhsSubsystem::iterator
    		i;
    boolean	epics;


    ret = TRUE;
    if ( numTokens == 5 )
    {
	if ( ! str2bool( tokens[4], &epics ) )
	{
	    status.E_CONFIG_KWD( status, tokens[0] );
	    return( FALSE );
	}

	if ( strcmp( tokens[1], "*" ) == 0 )
	{
	    for ( i = cDhsSubsystem::begin(); i != cDhsSubsystem::end(); i++ )
	    {
		channelAdd( (*i)->name(), tokens[2], tokens[3], (bool) epics, 
			status );
	    }
	}
	else if ( strcmp( tokens[1], "-" ) == 0 )
	{
	    channelAdd( tokens[2], tokens[3], (bool) epics, status );
	}
	else
	{
	    channelAdd( tokens[1], tokens[2], tokens[3], (bool) epics, status );
	}

	if ( !status.ok() )
	{
	    ret = FALSE;
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::epicsConfig
//
// INVOCATION:
// n.a. Called automatically from the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the SAD host information from the configuration file.
//
// DESCRIPTION:
// Store the host address.
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

boolean		cStaChannel::epicsConfig
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStaStat	status;		// Status code.


    ret = TRUE;
    if ( numTokens == 3 )
    {
	if ( scSadHost != NULL )
	{
	    free( scSadHost );
	}

	checkNull( scSadHost = strsav( tokens[1] ), status, ret = FALSE );

	scTimeout = atof( tokens[2] );
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::locationSiteConfig
//
// INVOCATION:
// n.a. Called automatically from the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the Location/Site information from the configuration file.
//
// DESCRIPTION:
// Build the "DH??" string for this Location/Site from the characters
// in the configuration file.
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

boolean		cStaChannel::locationSiteConfig
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStaStat	status;		// Status code.
    char	tempEpicsPrefix[5];
				// Temporary string.

    ret = TRUE;
    if ( numTokens == 3 )
    {
	if ( scEpicsPrefix != NULL )
	{
	    free( scEpicsPrefix );
	}

	(void) sprintf( tempEpicsPrefix, "%s%c%c", "DH",
	*tokens[1], *tokens[2] );

	checkNull( scEpicsPrefix = strsav( tempEpicsPrefix ), 
		status, ret = FALSE );
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::channelAdd
//
// INVOCATION:
// channelAdd( system, name, alias, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) system (const char *) The DHS subsystem id.
// (>) name (const char *) The name of the EPICS record less the system and 
//			subsystem parts.
// (>) alias (const char *) The name that will be used within the dhs.
// (>) epics (bool) If true, record should be updated in the epics database.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a new channel to the list of channels
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

void		cStaChannel::channelAdd
(
    const char	*s,		// (in)  The DHS subsystem name.
    const char	*name,		// (in)  Name of the EPICS record.
    const char	*alias,		// (in)  Alias to be used within the DHS.
    bool	epics,		// (in)  If true, record should be updated 
    				//	 in the epics database.
    cStaStat	&status		// (mod) Function return status.
)
{
    char	workBuff[CHANNEL_NAME_LEN + 1];

    checkStat( status, return );

    (void) sprintf( workBuff, "%s:%s:%s", scEpicsPrefix, s, name );

    checkNull( new cStaChannel( s, workBuff, alias, epics ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::channelAdd
//
// INVOCATION:
// channelAdd( system, name, alias, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (const char *) The name of the EPICS record less the system and 
//			subsystem parts.
// (>) alias (const char *) The name that will be used within the dhs.
// (>) epics (bool) If true, record should be updated in the epics database.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a new channel to the list of channels
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

void		cStaChannel::channelAdd
(
    const char	*name,		// (in)  Name of the EPICS record.
    const char	*alias,		// (in)  Alias to be used within the DHS.
    bool	epics,		// (in)  If true, record should be updated 
    				//	 in the epics database.
    cStaStat	&status		// (mod) Function return status.
)
{
    char	workBuff[CHANNEL_NAME_LEN + 1];

    checkStat( status, return );

    (void) sprintf( workBuff, "%s:%s", scEpicsPrefix, name );
    checkNull( new cStaChannel( "*", workBuff, alias, epics ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::channelDelete
//
// INVOCATION:
// cStaChannel::channelDelete( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all channels from the list of channels.
//
// DESCRIPTION:
// Loop through the list of channels deleting each.
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

void		cStaChannel::channelDelete
(
    cStaStat	&status		// (mod) Function return status.
)
{
    tChMap	*chMap;		// Pointer to the map.
    iChMap	i;		// Iterator for the map.
    cStaChannel	*pChannel;


    checkStat( status, return );

    if ( scList != NULL )
    {
	scRwLock.rLock();
	chMap = (tChMap *) scList;
	for ( i = chMap->begin(); i != chMap->end(); i++ )
	{
	    pChannel = (*i).second;
	    if ( ! pChannel->scIndexed )
	    {
		pChannel->close( status );
	    }
	}
	scRwLock.unlock();
	scRwLock.wLock();
	for ( i = chMap->begin(); i != chMap->end(); i++ )
	{
	    pChannel = (*i).second;
	    delete pChannel;
	}
	delete chMap;
	scList = NULL;
	scRwLock.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::chFlush
//
// INVOCATION:
// cStaChannel::chFlush( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Flush pending operations in the channel access buffer
//
// DESCRIPTION:
// Execute the ca_pend_io function.
//
// This should be an inline function, but it cannot be because of the
// inablility of the sun workshop C compiler to interprete channel access
// status macros in inlined functions.
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

void		cStaChannel::chFlush
(
    cStaStat	&status		// (mod) Function return status.
)
{
    checkStat( status, return );
    tChMap	*pChMap;
    iChMap	i;
    cStaChannel	*pChan;


    scRwLock.rLock();
    pChMap = (tChMap *) scList;

    if ( pChMap != NULL )
    {
	for ( i = pChMap->begin(); i != pChMap->end(); i++ )
	{
	    pChan = (*i).second;
	    if ( pChan->scDirty )
	    {
		pChan->update( status );
	    }
	}
    }
    scRwLock.unlock();


    if ( scEpicsActive )
    {
	scMutex.lock();
	checkCa( ca_pend_io( scTimeout ), status, VOID, "N/A" );
	scMutex.unlock();
	status( status.S_SUCCESS );
    }

    cStaSubscriber::flush( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::eventLoop
//
// INVOCATION:
// N.A. Called by the pthread_create function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dummy (void *) Dummy argument. Ignored.
//
// FUNCTION VALUE:
// (void *) - not used.
//
// PURPOSE:
// Run a channel access event loop periodically
//
// DESCRIPTION:
// - Until the exit flag is set:
//   - Run the channel access event loop.
//   - Sleep for 5 seconds.
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

void		*cStaChannel::eventLoop
(
    void 	*		// (in)  Dummy argument.
)
{
    timespec	time;		// Time for the next condition variable 
    				// timeout.
    int		s;
    cStaStat	status;	


    elCond.lock();

    //
    // Loop until the exit flag is set.
    //

    while ( ! scElExit )
    {
	//
	// Run the channel access event loop.
	//

	scMutex.lock();
	s = ca_pend_event( 0.0001 );
	scMutex.unlock();


	//
	// Check for fatal errors from ca_pend_event.
	//

	if ( s != ECA_NORMAL && s != ECA_TIMEOUT )
	{
	    scMutex.unlock();
	    status.E_CA( status, s, "N.A.", "ca_pend_event( 0.0001 )",
		    ca_message( s ) );
	    return( NULL );
	}


	//
	// Sleep for 5 seconds.  The sleep is done with a condition variable
	// so that a broadcast can stop the wait when it is time to exit.
	//

	clock_gettime( CLOCK_REALTIME, &time );
	time.tv_sec += 5;
	elCond.timedWait( time );
    }
    elCond.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::dirtyAll
//
// INVOCATION:
// cStaChannel::dirtyAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Mark all channel data as dirty
//
// DESCRIPTION:
// - Loop through all known channels, setting the dirty flag to true.
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

void		cStaChannel::dirtyAll
(
    cStaStat	&status		// (mod) Function return status.
)
{
    tChMap	*pChMap;	// Pointer to the channel map.
    iChMap	i;
    cStaChannel	*pChan;


    checkStat( status, return );


    scRwLock.rLock();
    pChMap = (tChMap *) scList;


    //
    // Loop through all known channels, setting the dirty flags to true.
    //

    if ( pChMap != NULL )
    {
	for ( i = pChMap->begin(); i != pChMap->end(); i++ )
	{
	    pChan = (*i).second;
	    if ( ! pChan->scIndexed )
	    {
		pChan->scDirty = true;
	    }
	}
    }
    scRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::exit
//
// INVOCATION:
// staChannel::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Close the EPICS Channel access library.
//
// DESCRIPTION:
// execute the ca_task_initialize function.
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

void		cStaChannel::exit
(
    cStaStat	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    cStaMerger::exit( status );

    channelDelete( status );

    scElExit = true;
    elCond.broadcast();

    pthread_join( scElThread, NULL );

    if ( scEpicsActive )
    {
	scMutex.lock();
	checkCa( ca_task_exit(), status, scMutex.unlock(); return, "N/A" );
	scMutex.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::init
//
// INVOCATION:
// staChannel::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the EPICS Channel access library.
//
// DESCRIPTION:
// execute the ca_task_initialize function.
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

void		cStaChannel::init
(
    cStaStat	&status		// (mod) Function return status.
)
{
    char	buffer[1024];
    tChMap	*chMap;		// Pointer to the map.
    iChMap	i;		// Iterator for the map.
    cStaChannel	*pChannel;	// One of the channels.


    checkStat( status, return );


    if ( scEpicsActive )
    {
	//
	// Set up the channel access environment variables.
	//

	(void) putenv( "EPICS_CA_AUTO_ADDR_LIST=NO" );
	(void) sprintf( buffer, "EPICS_CA_ADDR_LIST=%s", scSadHost );
	(void) putenv( buffer );
	scMutex.lock();
	checkCa( ca_task_initialize(), status, scMutex.unlock(); return, 
		"N/A" );
	scMutex.unlock();


	//
	// Start the channel access event loop.
	//

	scElExit = false;
	checkSystem( pthread_create( &scElThread, NULL, eventLoop, NULL ), 
		status, return );
    }


    //
    // Set the status items to their initial values.
    //

    cStaStat::logStatus( true );
    scRwLock.rLock();
    chMap = (tChMap *) scList;
    for ( i = chMap->begin(); i != chMap->end(); i++ )
    {
	pChannel = (*i).second;
	if ( ! pChannel->scIndexed )
	{
	    pChannel->put( pChannel->scData, pChannel->scMessage, status );
	}
    }
    chFlush( status );


    //
    // Close all of the channels.
    //

    for ( i = chMap->begin(); i != chMap->end(); i++ )
    {
	pChannel = (*i).second;
	if ( ! pChannel->scIndexed )
	{
	    pChannel->close( status );
	}
    }
    scRwLock.unlock();


    //
    //  Set the status server status items to their current values.
    //

    cStaStat::S_INITIALIZING( status );
    cStaStat::update( "state", "INITIALIZING", status );
    cStaStat::resetHealth( status );
    switch ( status.debugLevel() )
    {
	case	status.DEBUG_MIN:
	    cStaStat::update( "debug", "MIN", status );
	    break;
	case	status.DEBUG_FULL:
	    cStaStat::update( "debug", "FULL", status );
	    break;
	default:
	    cStaStat::update( "debug", "NONE", status );
	    break;
    }
    switch ( simulate() )
    {
	case	SIMULATE_VSM:
	    cStaStat::update( "simulate", "VSM", status );
	    break;
	case	SIMULATE_FULL:
	    cStaStat::update( "simulate", "FULL", status );
	    break;
	case	SIMULATE_FAST:
	    cStaStat::update( "simulate", "FAST", status );
	    break;
	default:
	    cStaStat::update( "simulate", "NONE", status );
	    break;
    }
    cStaStat::flush( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::mergeConfig
//
// INVOCATION:
// N.a. Called automatically by the get_config function
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the config line.
// (>) tokens (char **) List of tokens on the config line.
//
// FUNCTION VALUE:
// (boolean) True if the line parsed successfully.
//
// PURPOSE:
// Parse a merge line from the configuration file
//
// DESCRIPTION:
// - Check the number of tokens on the command line.
// - Create a cStaMerger object for each valid merger line.
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


boolean		cStaChannel::mergeConfig
(
    int		numTokens,	// (in)  Number of tokens on the config line.
    char	**tokens	// (in)  List o tokens on the config line.
)
{
    boolean	ret;
    cStaStat	status;

    
    if ( numTokens < 2 )
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    else
    {
	new cStaMerger( tokens[1], numTokens - 2, tokens + 2 );
	ret = TRUE;
    }
    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::newIndex
//
// INVOCATION:
// cStaChannel::newIndex( system, alias, index, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) system (const char *) DHS subsystem the record applies to.
// (>) alias (const char *) The alias name, containing "%d" for the index.
// (>) index (int) The index for the new channel.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// (cStaChannel *) Pointer to the new channel.
//
// PURPOSE:
// Create a new indexed channel
//
// DESCRIPTION:
// - Find the indexed channel.
// - Create the names for the new channel.
// - Create the new channel.
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

cStaChannel	*cStaChannel::newIndex
(
    const char	*system,	// (in)  DHS subsystem the record applies to.
    const char	*alias,		// (in)  The alias name, containing "%d" 
    				//	for the index.
    int		index,		// (in)  The new index number.
    cStaStat	&status		// (mod) Function return status.
)
{
    tChMap	*chMap;		// Pointer to the map.
    cStaChannel	*pChannel;
    pair<iChMap, iChMap>	// Range iterators.
    		range;
    char	tmpAlias[CHANNEL_NAME_LEN + 1];
    char	tmpName[CHANNEL_NAME_LEN + 1];


    checkStat( status, return( NULL ) );


    scRwLock.wLock();
    chMap = (tChMap *) scList;


    //
    // Find the indexed channel.
    //

    range = chMap->equal_range( cStaChannel::cKey( alias, system ) );
    pChannel = (*range.first).second;
    if ( strcmp( pChannel->scAlias, alias ) == 0 && 
	    ( strcmp( pChannel->scSystem, system ) == 0 || 
	      strcmp( pChannel->scSystem, "*" ) == 0 ) )
    {
	//
	// Get the name of the channel after the index has been included.
	//

	(void) sprintf( tmpAlias, alias, index );
	(void) sprintf( tmpName, pChannel->scName, index );
	scRwLock.unlock();


	//
	// Create a new channel for the specific index.
	//

	checkNull( pChannel = new cStaChannel( system, tmpName, tmpAlias, 
		pChannel->scEpics ), status, return( NULL ) );
	pChannel->update( status );
	if ( ! status.ok() )
	{
	    scRwLock.wLock();
	    (*chMap).erase( cStaChannel::cKey( pChannel->scAlias, 
		    pChannel->scSystem ) );
	    scRwLock.unlock();
	    delete pChannel;
	    return( NULL );
	}
    }
    else
    {
	scRwLock.unlock();
	status.E_CHANNEL_MISSING( status, alias, system );
	pChannel = NULL;
    }

    return( pChannel );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::test
//
// INVOCATION:
// isOk = channel.test( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// (bool) True if the channel works ok.
//
// PURPOSE:
// Verify a channel works ok
//
// DESCRIPTION:
// - Send a value to the channel.
// - Replace the channel value with the original value.
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

bool		cStaChannel::test
(
    cStaStat	&status		// (mod) Function return status.
)
{
    bool	retVal( true );
    char	oldMessage[DATA_LENGTH+1];
    char	oldValue[DATA_LENGTH+1];


    checkStat( status, return( false ) );


    //
    // Don't test indexed channels since they don't have coresponding
    // epics records.
    //

    if ( ! scIndexed )
    {
	//
	// Save the original value.
	//

	(void) strcpy( oldValue, scData );
	(void) strcpy( oldMessage, scMessage );


	//
	// Send a new value to the channel.
	//

	put( "testing", "testing", status );
	chFlush( status );


	//
	// Replace the old value in the channel.
	//

	put( oldValue, oldMessage, status );
	chFlush( status );

	if ( !status.ok() )
	{
	    status.status( status.S_SUCCESS );
	    retVal = false;
	}
    }

    return( retVal );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::testAll
//
// INVOCATION:
// isok = cStaChannel::testAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// (bool) True if all channels are ok, false otherwise.
//
// PURPOSE:
// Test all channels to make sure they work.
//
// DESCRIPTION:
// Cycle through all channels and write a value to them.
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

bool		cStaChannel::testAll
(
    cStaStat	&status		// (mod) Function return status.
)
{
    tChMap	*chMap;		// Pointer to the map.
    iChMap	i;		// Iterator for the map.
    bool	retVal( true );	// Value to return.


    checkStat( status, return( false ) );

    scRwLock.rLock();
    chMap = (tChMap *) scList;
    for ( i = chMap->begin(); i != chMap->end(); i++ )
    {
	if( ! ((*i).second)->test( status ) )
	{
	    retVal = false;
	}
    }
    scRwLock.unlock();

    return( retVal );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::find
//
// INVOCATION:
// cStaChannel::find( system, alias, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) system (const char *) The dhs subsystem for the record.
// (>) alias (const char *) The records alias.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// find the channel object matching the specified alias and system
//
// DESCRIPTION:
// Find the correct channel
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

cStaChannel	*cStaChannel::find
(
    const char	*system,	// (in)  The subsystem owning the record.
    const char	*alias,		// (in)  The records alias.
    cStaStat	&status		// (mod) Function return status.
)
{
    tChMap	*chMap;		// Pointer to the map.
    iChMap	i;		// Iterator for the map.
    int		index;		// The index in an indexed record.
    cStaChannel	*pChannel;	// The channel being processed.
    const char	*pIndex;	// Pointer to an index in the alias.
    int		prefixLen;	// Length of the string before the index.
    const char	*pSuffix;	// Pointer to the text following the index in
    				// the alias.
    pair<iChMap, iChMap>	// Range iterators.
    		range;
    char	tmpAlias[CHANNEL_NAME_LEN + 1];	
				// Temporary alias name.


    checkStat( status, return( NULL ) );

    scRwLock.rLock();

    chMap = (tChMap *) scList;
    
    range = chMap->equal_range( cStaChannel::cKey( alias, system ) );

    if ( range.first == range.second )
    {
	//
	// A channel cannot be found exactly matching the alias name, try 
	// to find an index number in the alias name.
	//

	pIndex = alias + strcspn( alias, "0123456789" );
	if ( pIndex - alias  == (long) strlen( alias ) )
	{
	    //
	    // The alias doesn't contain an index, so the channel wasn't
	    // found.
	    //

	    status.E_CHANNEL_MISSING( status, alias, system );
	    pChannel = NULL;
	}
	else
	{
	    //
	    // The alias contains an index number, so translate the index
	    // number into the "%d" stored in the indexed channel alias
	    // names.
	    //

	    //
	    // Calculate the length of the alias before the index number.
	    //

	    prefixLen = pIndex - alias;


	    //
	    // Get a pointer to the part of the alias following the index
	    // number.
	    //

	    pSuffix = pIndex + strspn( pIndex, "0123456789" );


	    //
	    // Decode the index number.
	    //

	    index = atoi( pIndex );
	    

	    //
	    // Calculate a new alias name that has "%d" in place of the index.
	    //

	    if ( prefixLen + strlen( pSuffix ) + 2 > CHANNEL_NAME_LEN )
	    {
		status.E_CHANNEL_NAME( status, alias );
		return( NULL );
	    }
	    (void) strncpy( tmpAlias, alias, prefixLen );
	    tmpAlias[prefixLen] = '\0';
	    (void) strcat( tmpAlias, "%d" );
	    (void) strcat( tmpAlias, pSuffix );


	    //
	    // Create a new channel for the specific instance of the indexed
	    // record. The lock is unlocked to prevent a deadlock, and 
	    // relocked only so the unlock later in the function is balanced
	    // by a lock.
	    //

	    scRwLock.unlock();
	    check( pChannel = newIndex( system, tmpAlias, index, status ), 
		    status, return( NULL ) );
	    scRwLock.rLock();
	}
    } 
    else
    {
	pChannel = (*range.first).second;
    }

    scRwLock.unlock();

    return( pChannel );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::logMessage
//
// INVOCATION:
// cStaChannel::logMessage( message, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) message (const char *) The message to log.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send the message to the logMessage channel
//
// DESCRIPTION:
// Trivail
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

void		cStaChannel::logMessage
(
    const char	*message,	// (in)  The message to log.
    cStaStat	&status		// (mod) Function return status.
)
{
    if ( scLogChannel == NULL )
    {
	scLogChannel = find( "logMessage", status );
    }

    scLogChannel->put( message, status );
    cStaChannel::chFlush( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::put
//
// INVOCATION:
// channel.put( value, message, block, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) value 	(const char *) 	The new value for the channel.
// (>) message 	(const char *)	The new message for the channel.
// (!) status 	(cStaStat &) Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Update the value for a channel.
//
// DESCRIPTION:
// - Save the value in the channel.
// - Save the mesage in the channel.
// - Indicate the channel has been changed since the last flush.
// - If necessary, update a merged channel value.
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

void		cStaChannel::put
(
    const char	*value,		// (in)  The value for the record.
    const char	*message,	// (in)  The message for the record.
    cStaStat	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    scMutex.lock();


    //
    // If necessary, increase the size of the data string.
    //

    if ( scDataSize < strlen( value ) + 1 )
    {
	scDataSize = strlen( value ) + 1;
	delete [] scData;
	scData = new char[scDataSize];
    }


    //
    // Copy the new data to the data string.
    //

    strcpy( scData, value );


    //
    // If necessary, increase the size of the message string.
    //

    if ( scMessageSize < strlen( message ) + 1 )
    {
	scMessageSize = strlen( message ) + 1;
	delete [] scMessage;
	scMessage = new char[scMessageSize];
    }


    //
    // Copy the new message to the message string.
    //

    strcpy( scMessage, message );


    //
    // Set the time the channel was updated.
    //

    scTime = ::time( NULL );


    //
    // Set the dirty flag on the channel to indicate it was changed since
    // the last update.
    //

    scDirty = true;

    scMutex.unlock();


    //
    // If this is a merged channel, update the merged value.
    //

    if ( scMerger != NULL )
    {
	scMerger->put( this, status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::putCallback
//
// INVOCATION:
// N.A. Called automatically by the channel access library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>)  (struct event_handler_args) Argument is not used
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// This function is executed when a put is complete
//
// DESCRIPTION:
// Set the scPutDone flag to true.
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

void		cStaChannel::putCallback
(
    struct event_handler_args	// (in)  Arguments
		args
)
{
    cStaChannel	*pChan( (cStaChannel *) args.usr );
    cStaStat	status;

    scPutDone = true;
    if ( args.status != ECA_NORMAL )
    {
	status.E_CA_LOG( status, args.status, pChan->scName, 
		"ca_array_put_callback", ca_message( args.status ) );
    }
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::update
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a channels value to EPICS if necessary, and to any dhsService
// status acceptors
//
// DESCRIPTION:
// - If neccessary, connect to the channel access channel.
// - Update the channel message
// - Break the data into chunks that EPICS can handle, and send the chunks
//   to EPICS.
// - Send the data to dhsService status acceptors.
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

void		cStaChannel::update
(
    cStaStat	&status		// (mod) Function return status.
)
{
    char	*ptr;
    char	workBuff[ CHANNEL_NAME_LEN + 10];
    int		caStatus;
    char	buffer[ DATA_LENGTH ];


    checkStat( status, return ); 


    //
    // Do nothing if simulate is turned on.
    //

    if ( simulate() == SIMULATE_NONE || simulate() == SIMULATE_VSM )
    {
	scMutex.lock();
	if ( scEpics )
	{
	    //
	    // If the epics channel is not connected, connect to it.
	    //

	    if ( !scConnected || scValChannel->state != cs_conn ||
		    scMsgChannel->state != cs_conn )
	    {
		status.S_CA_CONNECT( status, scName );
		(void) sprintf( workBuff, "%s.VAL", scName );
		checkCa( ca_search_and_connect( workBuff, &scValChannel, 
		      NULL, this ), status, scMutex.unlock(); return, scName );
		
		(void) sprintf( workBuff, "%s.IMSS", scName );
		checkCa( ca_search_and_connect( workBuff, &scMsgChannel, 
			NULL, this ), status, 
			scMutex.unlock(); return, scName );

		checkCa( ca_pend_io( scTimeout ), status, 
			scMutex.unlock(); return, scName );
		scConnected = true;
	    }
	    

	    //
	    // Update the channel message.
	    //

	    checkCa( ca_array_put_callback( DBF_STRING, 1, scMsgChannel, 
		    scMessage, putCallback, this ), status, 
		    scMutex.unlock(); return, scName );


	    //
	    // Update the channel value, breaking the value into message sizes
	    // that epics can tolerate.
	    //

	    for ( ptr = scData; ptr < scData + strlen( scData ); 
		    ptr += DATA_LENGTH - 1 )
	    {
		//
		// Get a chunk of the message.
		//

		strncpy( buffer, ptr, DATA_LENGTH - 1 );
		buffer[DATA_LENGTH - 1] = '\0';


		//
		// Send this chunk of the message to epics.
		//

		scPutDone = false;
		checkCa( ca_array_put_callback( DBF_STRING, 1, scValChannel, 
			buffer, putCallback, this ), status, 
			scMutex.unlock(); return, scName );


		//
		// Wait for epics to acknowledge recipt of the message.
		//

		while ( !scPutDone )
		{
		    caStatus = ca_pend_event( 0.0001 );
		    if ( caStatus != ECA_TIMEOUT )
		    {
			checkCa( caStatus, status, scMutex.unlock(); return, 
				"N/A" );
		    }
		}
	    }
	}

	if ( ! scIndexed )
	{
	    //
	    // Update the dhsService subscribers.
	    //

	    cStaSubscriber::postAll( *this, status );
	}


	//
	// Change the dirty flag to indicate that everyting is updated.
	//

	scDirty = false;

	scMutex.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::cKey::operator<
//
// INVOCATION:
// key1 < key2
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) key (const cKey & );
//
// FUNCTION VALUE:
// (bool) True if cKey < this.
//
// PURPOSE:
// Allows the multimap to create a less function
//
// DESCRIPTION:
// Compare the subsystem and alias fields, allowing for the subsystem
// field to be '*'.
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

bool		cStaChannel::cKey::operator<
(
    const cKey	&key		// (in)  The right hand side key.
) const
{
    int		v;
    


    if ( ( v =  strcmp( kAlias, key.kAlias ) ) == 0  )
    {
	if ( strcmp( key.kSystem, "*" ) == 0 || strcmp( kSystem, "*" ) == 0 )
	{
	    v = 0;
	}
	else
	{
	    v =  strcmp( kSystem, key.kSystem );
	}
    }

    return( v < 0 );
}
