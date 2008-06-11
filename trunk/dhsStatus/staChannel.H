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
// dhsStatus/h/staChannel.H
//
// PURPOSE:
// Contains the definition of the cStaChannel class.
//
// CLASSES:
// cStaChannel - The interface to the channel access library.
// cStaChannel::cKey - A key class for the list of channels.
// cStaChannel::simulate - Set the simulation mode for the cStaChannel class.
// cStaChannel::simulate - Get the simulation mode for the cStaChannel class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:30:22  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.15  1998/06/24 23:05:52  nhill
// Changed the mutex protecting the epics functins to a cThreadLock type.
//
// Revision 1.14  1998/06/19 18:09:55  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.13  1998/05/12 20:19:51  nhill
// Added class cStaMerger.
// Changed class cStaChannel to allow dhsService status acceptors.
//
// Revision 1.12  1998/04/15 22:03:22  nhill
// Updated the documentation.
//
// Revision 1.11  1998/03/16 21:43:20  cockayne
// Added code to support sonfiguration of location and site.
//
// Revision 1.10  1998/03/06 19:22:06  jaeger
// Added a parameter to put method.
//
// Revision 1.9  1998/02/17 00:23:09  jaeger
// Add a mutex lock so channel access commands occur in a single thread.
//
// Revision 1.8  1997/12/23 23:19:46  nhill
// Moved method cStaChannel::chFlush to the C file and made it non-inlined.
//
// Revision 1.7  1997/11/12 23:17:40  cockayne
// Added staPutCallback() and scPutDone, supporting modifications in
// staChannel.C.
//
// Revision 1.6  1997/10/01 17:13:42  nhill
// Added record name to the channel access errors.
//
// Revision 1.5  1997/08/19 20:10:46  nhill
// Some lint type fixes.
//
// Revision 1.4  1997/06/13 20:41:49  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.3  1997/04/24 20:04:02  nhill
// Added some "const" keywords.
//
// Revision 1.2  1997/04/16 22:32:24  nhill
// Changed the name of the flush function to chFlush.
//
// Revision 1.1  1997/03/27 22:16:14  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STACHANNEL_H
#define	STACHANNEL_H

#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>

#include "genThreadLock.H"
#include "genRwLock.H"
#define	NO_FIND_CREATE
#include "genListOf.H"

#include "cadef.h"
#include "tsDefs.h"

class	cStaMerger;

//
//***********************************************************************
//+
// CLASS NAME:
// cStaChannel
//
// PUBLIC CLASSES: 
// cKey - A key to be used in an STL multimap.
//
// PUBLIC METHODS: 
// (void) close - Close the channel access copnnection.
// (void) chFlush - Flush all updates to the EPICS database.
// (void) config - Read the channel access information from the configuration
//		file.
// (void) dirtyAll - Mark all channels dirty.
// (void) exit - Clean up this class before exiting.
// (char *) data - Return the data associated with the channel.
// (cStaChannel *) find - Locate a channel given its alias and subsystem.
// (void) init - Initialize the channel access library.
// (void) logMessage - Log a message to the log record.
// (char *) name - Return the name of the channel.
// (void) put - set a channel access record value.
// (simulateMode) simulate - Set or get the simulate mode.
// (bool) testAll - Test all channel access connections.
// (time_t) time - Return the time the channel was last modified.
// (void) update - Send the value of a channel to appropriate subscribers.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// (void *) scList - list of all channels.
//
// PURPOSE:
// Stores a list of EPCIS channels.
//
//-
//***********************************************************************
//

class 	cStaChannel
{
    public:
    class	cKey;

    friend class cStaSubscribe;
    friend class cStaMerger;

    typedef std::multimap<cStaChannel::cKey, cStaChannel *, 
			    std::less<cStaChannel::cKey> > 
			    tChMap;
    typedef std::multimap<cStaChannel::cKey, cStaChannel *, 
			    std::less<cStaChannel::cKey> >::iterator 
				iChMap;

    public:
	class cKey
	{
	    public:
		inline		cKey( ) {};
		inline		cKey( const char *, const char * );
		bool		operator<( const cKey & ) const;
	    private:
		const char	*kAlias;
		char		kSystem[cDhsSubsystem::SYSTEM_NAME_LENGTH+1];
	};

	typedef enum
	{
	    SIMULATE_NONE,
	    SIMULATE_VSM,
	    SIMULATE_FULL,
	    SIMULATE_FAST
	} simulateMode;

	enum { DATA_LENGTH = 40 };	// Length of the data buffer.

	void		close( cStaStat & );
	static void	chFlush( cStaStat & );
	static void	config( cStaStat & );
	char		*data( void )
			{
			    return( scData );
			};
	static void	dirtyAll( cStaStat & );
	static void	exit( cStaStat & );
	static cStaChannel
			*find( const char *, const char *, cStaStat & );
	static inline cStaChannel
			*find( const char * alias, cStaStat &status )
				{ return( find( "*", alias, status ) ); };
	static void	init( cStaStat & );
	static void	logMessage( const char *, cStaStat & );
	char		*name( void )
			{
			    return( scName );
			};
	void		put( const char*, const char*, cStaStat & );
	inline void	put( const char * message, cStaStat & status )
				{ put( message, "", status ); };
	static inline simulateMode
			simulate( simulateMode );
	static inline simulateMode
			simulate( void );
	static bool	testAll( cStaStat & );
	time_t		time( void )
			{
			    return( scTime );
			}
	void		update( cStaStat & );

    protected:
			~cStaChannel( void );
    private:
    			cStaChannel( const char *, const char *, const char *, 
				bool );
	inline		cStaChannel( const cStaChannel & ) { throw "a fit"; };
	inline cStaChannel
			&operator = ( const cStaChannel & ) { throw "a fit"; };
	static void	channelAdd( const char *, const char *, const char *, 
				bool, cStaStat & );
	static void	channelAdd( const char *, const char *, bool, 
				cStaStat & );
	static void	channelDelete( cStaStat & );
	static boolean	channelConfig( int, char ** );
	static boolean	epicsConfig( int, char ** );
	static void	*eventLoop( void * );
	static boolean	locationSiteConfig( int, char ** );
	static boolean	mergeConfig( int, char ** );
	static cStaChannel
			*newIndex( const char*, const char *, int, 
				cStaStat & );
	static void	putCallback( struct event_handler_args );
	bool		test( cStaStat & );

	char		*scAlias;	// Alias for the channel.
	bool		scConnected;	// Is the channel connected?
	char		*scData;	// Stores the data currently in the
					// channel.
	int		scDataSize;	// Size of the data buffer.
	bool		scDirty;	// True if the channel data is dirty.
	bool		scEpics;	// Send the channel to epics?
	static bool	scEpicsActive;	// Are any channels going to epics?
	static pthread_t		// Thread id of the event loop thread.
			scElThread;
	static bool	scElExit;	// True when the event loop should exit.
	bool		scIndexed;	// Is the record indexed?
    	static void	*scList;	// List of all channels.
	static cStaChannel		// Channel for the log messages.
			*scLogChannel;
	char		*scMessage;	// Stores the message currently in the
					// channel.
	int		scMessageSize;	// Size of the message buffer.
	chid		scMsgChannel;	// Channel for the message field.
	static cThreadLock		// Mutex prevents misuse of EPICS.
			scMutex;
	char		*scName;	// Name of the channel.
	static cRwLock	scRwLock;	// Reader/writer lock for scList.
	static char	*scSadHost;	// SAD host address.
	static simulateMode		// Simulation mode.
			scSimulateMode;
	char		scSystem[cDhsSubsystem::SYSTEM_NAME_LENGTH+1];	
					// System the status applies to.
	time_t		scTime;		// Time the channel was updated.
	static float	scTimeout;	// Timeout.
	chid		scValChannel;	// Channel for the value field.

	static bool	scPutDone;	// Flag to signify callback has been
					// called.
	static char	*scEpicsPrefix;	// The epics prefix.
	cStaMerger	*scMerger;	// The merged record associated with
					// this record.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStaMerger
//
// PUBLIC METHODS: 
// cStaMerger - Constructor for the class.
// ~cStaMerge - Destructor for the class.
// (void) exit - Delete all objects of class cStaMerger.
// (int) priority - Prioritize a string.
// (void) put - Update the current value of a merged channel.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Keeps track of a set of channels to be merged into a single record
//
// The cListOf class is used to maintain a list of all known cStaMerger
// objects.
//
//-
//***********************************************************************
//

class cStaMerger: 
	public cListOf< cStaMerger, std::string, cStaStat >
{
    public:
			cStaMerger( char *, int, char ** );
			~cStaMerger( void );
	static void	exit( cStaStat & );
	int		priority( const char * );
	void		put( cStaChannel *, cStaStat & );
    protected:
    private:
	cStaChannel	*smPChannel;	// The merged channel.
	cStaChannel::tChMap		// list of merged channels.
			smChannelList;
	int		smNumValues;	// Number of possible values.
	char		**smValues;	// List of possible values.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::cKey::cKey
//
// INVOCATION:
// N.A.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) alias (const char *) Alias for the channel.
// (>) subsystem (const char *) The subsystem associated with the channel.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cStaChannel::cKey class
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

			cStaChannel::cKey::cKey
(
    const char	*alias,		// (in)  Alias for the channel.
    const char	*subsystem	// (in)  Subsystem associated with the channel.
)
{
    kAlias = alias;
    strcpy( kSystem, subsystem );
    str2upper( kSystem, kSystem );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::simulate
//
// INVOCATION:
// staChannel::simulate( mode );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) simulate (cStaChannel::simulateMode) The new simulation mode.
//
// FUNCTION VALUE:
// (cStaChannel::simulateMode) The old simulation mode.
//
// PURPOSE:
// Set the simulation mode and return the old simulation mode
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

cStaChannel::simulateMode	
			cStaChannel::simulate
(
    simulateMode mode		// (in)  The new simultion mode.
)
{
    simulateMode oldMode;

    oldMode = scSimulateMode;
    scSimulateMode = mode;
    return( oldMode );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaChannel::simulate
//
// INVOCATION:
// mode = staChannel::simulate();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cStaChannel::simulateMode) The old simulation mode.
//
// PURPOSE:
// Return the simulation mode
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

cStaChannel::simulateMode	
			cStaChannel::simulate
(
    void
)
{
    return( scSimulateMode );
}
#endif // STACHANNEL_H
