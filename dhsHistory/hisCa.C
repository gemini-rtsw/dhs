static char rcsid[] = "$Id: hisCa.C,v 1.1.1.1 2002-11-24 20:28:12 brighton Exp $";
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
// dhsHistory/src/hisCa.C
//
// PURPOSE:
// Contains implementation of cHisCa class, and classes required
// to handle EPICS Channel Access stuff. 
//
// METHOD NAME(S)
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>


extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#include "dhs++.H"
#include "cadef.h"
#include "tsDefs.h"


#include "globals.H"
#include "hisCa.H"
#include "hisDhs.H"
#include "hisEvent.H"


//
// Define static data in cHisCa class.
//

bool		cHisCa::hcChannelLoopDone;
cHisCa::tChanList	cHisCa::hcChannelList;
char *		cHisCa::hcDirectory;



//
//***********************************************************************
//+
// FUNCTION NAME:
// testChannel
//
// INVOCATION:
// testChannel( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Tests the channel
//
// DESCRIPTION:
// Using ca_search, test that the channel may be connected to. 
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

void		cHisCa::testChannel
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    chid	channel_id;	// Channel identifier returned from CA lib.


    //
    //  Test channel by ensuring it's find-able. 
    //

    checkCa( ca_search( hcChannelName, &channel_id ), status, return );
    checkCa( ca_pend_io( 10 ), status, VOID );


    //
    // Ensure channel is connected by inspecting returned struct. 
    //
    
    if ( ca_field_type( channel_id ) == TYPENOTCONN )
    {
	status.E_CHANNEL( status, hcChannelName );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// testChannelAccess
//
// INVOCATION:
// testChannelAccess( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Test Channel Access 
//
// DESCRIPTION:
// Test Channel Access by ca_search-ing all configured channels. 
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

void		cHisCa::testChannelAccess
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    iChanList	chanListIter;	// Channel list iterator. 
    cHisCa	*channel;	// Channel object pointer.


    //
    //  Loop thru channel list, testing each.
    //

    for ( chanListIter = hcChannelList.begin();
	    chanListIter != hcChannelList.end();
	    chanListIter++ )
    {
	channel = (*chanListIter).second;
	channel->testChannel( status );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// testDiskArea
//
// INVOCATION:
// testDiskArea( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Test Disk Area 
//
// DESCRIPTION:
// Test Disk Area by using stat() to get the filemode and testing 
// the read and write bits of the filemode. 
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

void		cHisCa::testDiskArea
(
    cHisStatus	&status		// (mod) Program status.
)
{
    struct stat	statStruct;	// Return struct from stat().

    
    if ( stat( tempDir(), &statStruct ) == -1 )
    {
	status.E_FSTAT( status, tempDir() );
	status.sysErrno();
	return;
    }

    if ( ( statStruct.st_mode & ( S_IRUSR | S_IWUSR ) ) != 
	    ( S_IRUSR | S_IWUSR ) )
    {
	status.E_PATH( status, tempDir() );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// callbackAdd
//
// INVOCATION:
// callbackAdd( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Adds callback for channel 
//
// DESCRIPTION:
// Adds callback for channel by searching for the channel, 
// then adding an event callback. 
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

void		cHisCa::callbackAdd
(
    cHisStatus	&status		// (mod) Program status.
)
{
    evid	event_id;	// Channel access returned event ID. 
    chid	channel_id;	// Channel identifier returned from CA lib.


    checkStat( status, return );


    checkCa( ca_search( hcChannelName, &channel_id ), status, VOID );
    checkCa( ca_pend_io( 10 ), status, VOID );
    checkCa( ca_add_event( DBR_TIME_STRING, channel_id, channelCallback, 
	    hcChannelName, &event_id), status, VOID );

    if ( ca_field_type( channel_id ) == TYPENOTCONN || !status.ok() )
    {
	status.E_CHANNEL( status, hcChannelName );
	return;
    }
    hcEvId = event_id;


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// callbackRemove
//
// INVOCATION:
// callbackRemove( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Remove callback for channel
//
// DESCRIPTION:
// Removes callback by clearing the event from channel access. 
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

void		cHisCa::callbackRemove
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    int         ca_status;      // Channel access library return status.


    checkStat( status, return );


    checkCa( ca_clear_event( hcEvId ), status, return );
    ca_status = ca_pend_io( 10 );
    if ( ca_status != ECA_TIMEOUT )
    {
	checkCa( ca_status, status, return );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// channelLoop
//
// INVOCATION:
// channelLoop( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Channel access event loop.
//
// DESCRIPTION:
// Forms the channel access library event loop by calling ca_pend_event
// in a loop until its time to stop looping. 
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

void		cHisCa::channelLoop
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    int		ca_status;	// Channel access library return status. 


    checkStat( status, return );


    for ( ; ; )
    {
	ca_status = ca_pend_event( 1 ); 
	if ( ca_status != ECA_TIMEOUT )
	{
	    checkCa( ca_status, status, return );
	}

	if ( hcChannelLoopDone == true )
	{
	    break;
	}
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// channelLoopStart
//
// INVOCATION:
// channelLoopStart( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts channel acess event loop.
//
// DESCRIPTION:
// Initializes channel access library, Registers callbacks for all
// channels, runs channel access loop. Upon exit from channel access
// loop, un-registers callbacks, closes channel access library.
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

void		cHisCa::channelLoopStart
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    iChanList	chanListIter;	// Channel list iterator. 
    cHisCa	*channel;	// Channel object pointer.


    checkStat( status, return );


    //
    //  Init channel access task. Start history event queue processing. 
    //

    hcChannelLoopDone = false;
    checkCa( ca_task_initialize(), status, return );
    check( cHisEvent::startQueueLoop( status ), status, return );


    //
    //  Loop thru channel list, adding callback for each. 
    //

    for ( chanListIter = hcChannelList.begin(); 
	    chanListIter != hcChannelList.end(); 
	    chanListIter++ )
    {
	channel = (*chanListIter).second;
	channel->callbackAdd( status );
    }


    //
    //  Update state. Run Channel Access event loop until its time to stop. 
    //

    cHisDhs::ready( true );
    cHisStatus::update( HIS_STATE, HIS_STATE_RUNNING, status );
    cHisStatus::flush( status );
    status.S_RUNNING( status );

    channelLoop( status );
    
    cHisDhs::ready( false );


    //
    //  Loop through channels list, removing callback & clearing list. 
    //

    for ( chanListIter = hcChannelList.begin(); 
	    hcChannelList.size() != 0;
	    chanListIter = hcChannelList.begin() )
    {
	channel = (*chanListIter).second;
	channel->callbackRemove( status );
	delete( channel );
	hcChannelList.erase( chanListIter );
    }


    //
    // Stop history event queue processing loop.
    //
    
    cHisEvent::stopQueueLoop();


    //
    // Exit EPICS Channel Access library.
    //

    checkCa( ca_task_exit(), status, return );


    //
    // Free memory allocated during configuration.
    //

    gen_free( hcDirectory );
    

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// channelCallback
//
// INVOCATION:
// N/A. Called from within channel access library. 
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Provides a callback to handle EPICS channel value changes
//
// DESCRIPTION:
// Get information from the passed event handler argument, create a new
// event, insert into queue. 
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

void		cHisCa::channelCallback
(
    struct event_handler_args
    		args		// EPICS callback structure. 
)
{
    cHisEvent	*event;		// Pointer to event object.
    cHisStatus	status;		// Program status. 


    // XXX allan: this is not defined anymore in recent EPICS versions
#ifndef TS_EPOCH_SEC_PAST_1970
#define TS_EPOCH_SEC_PAST_1970 7305*86400 /* 1/1/90 20 yr (5 leap) of seconds */
#endif

    //
    // Create event with callback argument. 
    //

    event = new cHisEvent( TS_EPOCH_SEC_PAST_1970 + 
	    ((struct dbr_time_string *)args.dbr)->stamp.secPastEpoch, 
	    ((struct dbr_time_string *)args.dbr)->stamp.nsec, 
	    (char *) args.usr,     
	    ((struct dbr_time_string *)args.dbr)->value );
    
    if ( args.status != 1 )
    {
	status.E_CALLBACK( status, args.status );
	return;
    }


    //
    // If simulation is turned on, don't add event to queue. 
    //
    
    if ( cHisDhs::simulate() )
    {
	delete event;
	return;
    }


    //
    // Lock. Put in queue. Unlock. Broadcast. 
    //

    event->queueLock();
    event->queuePush();
    event->queueUnlock();
    event->queueBroadcast();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// config
//
// INVOCATION:
// config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Performs runtime configuration for cHisCa class
//
// DESCRIPTION:
// Performs runtime configuration for cHisCa class
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

void		cHisCa::config
(
    cHisStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );
    
    
    checkConfig( config_get( HIS_CFG_CHANNEL, channel ), 
	    status, return );
    checkConfig( config_get( HIS_CFG_DIRECTORY, directory ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// channel
//
// INVOCATION:
// booleanVariable = channel( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)		Number of tokens in tokens parameter.
// (>) tokens 	 (char **)	Pointer to packed array of token strings. 
//
// FUNCTION VALUE:
// (boolean) TRUE 	- Tokens were successfully parsed. 
// (boolean) FALSE	- Tokens were not successfully parsed. 
//
// PURPOSE:
// Parse "channel" configuration file lines.
//
// DESCRIPTION:
// Parse "channel" configuration file lines by checking number of tokens
// on line, and checking for duplicate channel names. Valid channel 
// names are saved into list of channels. 
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

boolean		cHisCa::channel
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  List of tokens.
)
{
    cHisCa	*pHisCa;	// Pointer to cHisCa object.
    boolean     ret;            // Function return value.
    cHisStatus  status;         // Status code.
    iChanList	i;		// Iterator for hcChannelList.


    //
    // Check number of tokens on config line is correct. 
    //

    if ( numTokens == 2 )
    {
	i = hcChannelList.find( string( tokens[1] ) );
	if ( i != hcChannelList.end() )
	{
	    //
	    // Duplicate keyword.
	    //

	    status.E_DUPLICATE_CHANNEL( status, tokens[1] );
	    ret = FALSE;
	}
	else
	{
	    //
	    // Save channel info in a list. 
	    //

	    pHisCa = new cHisCa( tokens[1] );
	    pHisCa->chanListAdd( status );

	    ret = TRUE;
	}
    }
    else
    {
	//
	// Number of tokens on line was wrong.
	//

	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }


    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// directory
//
// INVOCATION:
// booleanVariable = directory( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)		Number of tokens in tokens paramter.
// (>) tokens 	 (char **)	Pointer to pached array of token strings.
//
// FUNCTION VALUE:
// (boolean) TRUE       - Tokens were successfully parsed.
// (boolean) FALSE      - Tokens were not successfully parsed.
//
// PURPOSE:
// Parse "directory" configuration file lines.
//
// DESCRIPTION:
// Parse "directory" configuration file lines by checking number of tokens
// on line, and saving directory string. 
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

boolean		cHisCa::directory
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  List of tokens.
)
{
    boolean     ret;            // Function return value.
    cHisStatus  status;         // Status code.


    //
    // Check number of tokens on config line is correct. 
    //

    if ( numTokens == 2 )
    {
	hcDirectory = strsav( tokens[1] );
	ret = TRUE;
    }
    else
    {
	//
	// Number of tokens on line was wrong.
	//

	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }


    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// chanListAdd
//
// INVOCATION:
// chanListAdd( status ):
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &)	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add channel name to channel name list 
//
// DESCRIPTION:
// Add channel name to channel name list.
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
 
void     cHisCa::chanListAdd
(
    cHisStatus  &status                 // (mod) Program status.
)
{
    //
    // Check status.
    //

    checkStat( status, return );


    //
    // Add to channel list.
    //

    hcChannelList[ string( hcChannelName ) ] = this;
}
