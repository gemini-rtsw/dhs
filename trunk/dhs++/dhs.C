static char rcsid[] = "$Id: dhs.C,v 1.3 2003-01-15 18:23:35 brighton Exp $";
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
// dhs++/src/dhs.C
//
// PURPOSE:
// Contains the implementation of the cDhs class, the cDhsError class,
// the cDhsErrorHandlerBase class, the cDhsHandler class, the 
// cErrHandlerKey class, cDhsAttrib class and the cDhsError class.
//
// METHOD NAME(S)
// cDhsAvList::operator= - Assignment operator for the cDhsAvList class.
// cDhsHandler::cDhsHandler - Constructor for the cDhsHandler class.
// cDhsHandler::~cDhsHandler - Destructor for the cDhsHandler class.
// cDhsHandler::disable - Disable a handler.
// cDhsHandler::enable - Enable a handler.
// cDhsHandler::forEach - Execute a function for each existing handler.
// cDhsHandler::threadCreate - Execute a function in a new thread.
// cDhsHandler::threadDestroy - Decrement the thread counter
// cDhsHandler::threadJoin - Wait until all threads have completed.
// tryLockFn - Attempt to lock a reader/writter lock.
// cErrHandlerKey::operator< - Compare to cErrHandlerKey objects.
// cDhsError::error - default error handler method.
// cDhsError::init - Initiaize and error object.
// cDhs::cDhs - Constructor for the cDhs class.
// cDhs::~cDhs - Destructor for the cDhs class.
// cDhs::commonConst - The common part of the constructors for the class.
// cDhs::config - Read the configuration file.
// cDhs::getCallback - DHS library callback function for get responses.
// cDhs::identity - Read the identity configuration line.
// cDhs::imp - Read the imp configuration line.
// cDhs::putCallback - DHS library callback function for put responses.
// cDhs::responseCallback - DHS library callback function for command 
//				responses.
// cDhs::Stop - Stop the DHS event loop.
// cDhsErrorHandlerBase::cDhsErrorHandlerBase - Constructor.
// cDhsErrorHandlerBase::~cDhsErrorHandlerBase - Destructor.
// cDhsErrorHandlerBase::errorCallback - DHS library error callback function.
// cDhsAttrib::typeMap - Turn a data type into a string.
// cDhsAttrib::typeMap - Turn a string into a data type.
// 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/27 17:15:08  brighton
// ported to gcc-3.2.1/linux
//
// Revision 1.1.1.1  2002/11/24 20:24:57  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.26  2001/03/12 20:54:25  nhill
// Changed to work with the workshop 5 compilers.
//
// Revision 1.25  1998/06/22 19:16:07  nhill
// Added method cDhsAvList::exportDataset.
// Moved cDhs::stop from dhs++Dhs.H.
//
// Revision 1.24  1998/05/04 20:50:10  nhill
// Added a timout to the dhs++ configuration.
//
// Revision 1.23  1998/04/23 23:16:07  nhill
// Updated the documentation.
// Added methods to read dhs configuration information from a configuration
// file.
//
// Revision 1.22  1998/04/02 19:22:00  nhill
// Moved the cDhsAvList::operator function from file dhs++Command.H
// Added avList to the get a put callback handling.
//
// Revision 1.21  1998/03/05 23:44:58  nhill
// Added method enable to the cDhsHandler class.
//
// Revision 1.20  1998/02/24 01:21:37  nhill
// Added functions threadCreate, threadDestroy, threadJoin, and tryLockFn.
//
// Revision 1.19  1998/01/20 19:02:07  nhill
// Added the connection callback routine.
//
// Revision 1.18  1997/11/30 21:13:55  nhill
// Changed the locking durring error handling callbacks.
//
// Revision 1.17  1997/11/12 18:19:07  nhill
// Changed cDhsHandler::foreach function to copy the list to a temporary
// list before starting to iterate. This eliminiates a crash when a handler
// is added or deleted by another thread while this function is executing.
//
// Revision 1.16  1997/09/22 23:54:47  nhill
// Re arranged the code so that error handlers cannot be partially destroyed
// when they are used.
//
// Revision 1.15  1997/08/29 22:34:47  nhill
// Fixed a typo bug.
//
// Revision 1.14  1997/08/27 23:00:18  nhill
// Fixed an uninitialized memory read.
//
// Revision 1.13  1997/08/26 21:16:33  nhill
// Changed references to dcdTag to dtTag.
//
// Revision 1.12  1997/07/14 23:23:57  nhill
// Backed out of the r1.11 changes.
//
// Revision 1.10  1997/06/12 16:52:59  nhill
// Fixed running the library without an event loop.
//
// Revision 1.9  1997/05/13 17:25:30  nhill
// Added the broadcast method to the cDhs class.
// Added Code to the callbacks to wait until the tag member has been set by
// the apply/put/get functions.
//
// Revision 1.8  1997/04/24 16:58:00  nhill
// Added some "const" keywords.
//
// Revision 1.7  1997/04/15 21:06:15  nhill
// Added a handler class that is the base class for all handlers, and
// code to keep a list of the handlers.
//
// Revision 1.6  1997/03/27 23:19:07  nhill
// *** empty log message ***
//
// Revision 1.5  1997/03/25 00:02:02  nhill
// Checkin for install.
//
// Revision 1.4  1997/03/06 19:16:04  nhill
// Cleaned up the header file dependancies.
//
// Revision 1.3  1997/02/28 00:54:01  nhill
// Checkin for install.
//
// Revision 1.2  1997/02/24 19:22:58  nhill
// Checkin for install.
//
// Revision 1.1  1997/02/21 21:45:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <map>
#include <list>
#include <algorithm>
#include <iostream>

#define DHS_NO_SERVER
#include <dhs++.H>
#include <localDhs++.H>

#include "genRwLock.H"
#include "genCond.H"
extern "C"
{
#include "gen_config.h"
}


typedef std::list< cDhsHandler * >
		tHandlerList;
typedef tHandlerList::iterator
		iHandlerList;
typedef std::map< cErrHandlerKey, cDhsErrorHandlerBase *, std::less<cErrHandlerKey> >
		tErrHandlerList;
typedef tErrHandlerList::iterator
		iErrHandlerList;

#define	dhHandlerList	(*(tHandlerList *) dhPHandlerList)
#define	dehHandlerList	(*(tErrHandlerList *) dehPHandlerList)
#define dhRwLock	(*((cRwLock *) dhPRwLock))


static cRwLock	threadLock;
static cMutex	mutex;
static pthread_t
		threadLockId = 0;
void		*cDhsHandler::dhPHandlerList = new tHandlerList;

void		*cDhsHandler::dhPRwLock = new cRwLock;
bool		cDhsErrorHandlerBase::dehCbInit = false;
DHS_CB_FN_PTR	cDhsErrorHandlerBase::dehOldCallback = NULL;
void		*cDhsErrorHandlerBase::dehPHandlerList = new tErrHandlerList;
char		*cDhs::dIdentity = NULL;
int		cDhs::dNumConnect = 0;
int		cDhs::dTimeout = 10;
static cDhsStatus	
		*dhsStatusClass;


static cCond	cond;

static bool tryLockFn( void * );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::operator=
//
// INVOCATION:
// avlist = origAvList;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) origAvList (cDhsAvList &) The original Av list.
//
// FUNCTION VALUE:
// (cDhsAvList &) The av list being created.
//
// PURPOSE:
// Overloaded assignment operator for av lists
//
// DESCRIPTION:
// Ensure the existing av list is deleted if necessary. Copy the SDS 
// structure and set up the new av list.
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

cDhsAvList	&cDhsAvList::operator=
(
    const cDhsAvList		// (in)  The original Av list.
		&origAvList
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    if ( dalFreeIt )
    {
	dhsAvListFree( dalAvList, &status );

	if ( status != DHS_S_SUCCESS )
	{
	    throw status;
	}
    }

    dalAvList = dhsAvListCopy( origAvList.dalAvList, &status );
    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }

    dalFreeIt = true;

    return( *this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::exportDataset
//
// INVOCATION:
// avList.exportDataset( fileName, dhsStatus );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) fileName (char *) Name of the exported file.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Save the attribute value list to the named file.
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

void		cDhsAvList::exportDataset
(
    char	*fileName,	// (in)   Name of the exported file.
    DHS_STATUS	&status		// (mod)  Function return status.
) const
{
    int		fd;
    void	*buffer;
    int		size;


    if ( status != DHS_S_SUCCESS )
    {
	return;
    }

    if ( dalAvList != DHS_AV_LIST_NULL )
    {
	size = dhsBdDsSize( dalAvList, &status );
	if ( status != DHS_S_SUCCESS )
	{
	    return;
	}
	buffer = malloc( size );
	dhsBdDsExport( dalAvList, buffer, size, &status );
	if ( status != DHS_S_SUCCESS )
	{
	    free( buffer );
	    return;
	}
	fd = open( fileName, O_CREAT | O_TRUNC | O_WRONLY, 
		S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR );
	if ( fd != 0 )
	{
	    write( fd, buffer, size );
	    close( fd );
	}
	free( buffer );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::cDhsHandler
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
// Constructor for the cDhsHandler object
//
// DESCRIPTION:
// Add the handler to the list of handlers.
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

		cDhsHandler::cDhsHandler
(
    void
)
{
    dhRwLock.wLock();
    dhHandlerList.push_back( this );
    dhRwLock.unlock();
    dhOldCallback = NULL;
    dhEnabled = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::~cDhsHandler
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
// Destructor for the cDhsHandler object
//
// DESCRIPTION:
// Remove the handler from the list of handlers.
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

		cDhsHandler::~cDhsHandler
(
    void
)
{
    iHandlerList	i;

    dhRwLock.wLock();
    if ( ( i = find( dhHandlerList.begin(), dhHandlerList.end(), this) ) != 
	    dhHandlerList.end() )
    {
	dhHandlerList.erase( i );
    }
    dhRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::disable
//
// INVOCATION:
// handler.disable();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Disables a handler
//
// This function is executed just before a handler is disabled.
//
// DESCRIPTION:
// Clear the enable flag.
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

void		cDhsHandler::disable
(
    void
)
{
    dhRwLock.wLock();
    dhEnabled = false;
    dhRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::enable
//
// INVOCATION:
// handler.disable();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Re-enable a handler
//
// This function is called after disable to re-enable the handler.
//
// DESCRIPTION:
// Set the enabled flag.
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

void		cDhsHandler::enable
(
    void
)
{
    dhRwLock.wLock();
    dhEnabled = true;
    dhRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::forEach
//
// INVOCATION:
// cDhsHandler::forEach( functionToCall );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) function (tForEachFn) The funtion to call for each handler.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute the specified function for each handler
//
// The specified funtion will get a pointer to the cDhsHandler object
// as its only parameter.
//
// DESCRIPTION:
// Use the stl foreach function to execute the specified function.
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

void		cDhsHandler::forEach
(
    tForEachFn	function	// (in)  The function to call.
)
{
    tHandlerList tempList;

    tempList = dhHandlerList;
    for_each( tempList.begin(), tempList.end(), function );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::threadCreate
//
// INVOCATION:
// threadCreate( fn, arg );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threadFn (tThreadFn) Function to execute in the thread.
// (>) arg (void *) Pointer to the argument to the function.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Execute the specified function in a detached thread.
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
// SEE ALSO:
// - cDhsHandler::threadJoin
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsHandler::threadCreate
(
    tThreadFn	threadFn,	// (in)  The function to execute
    void	*pArg		// (in)  The argument to the function.
)
{
    pthread_t   thread;         // XXX allan: added
    pthread_attr_t		// Thread attributes.
		attr;
    int		s;		// pthread_create function return status.


    threadLock.rLock();

    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    s = pthread_create( &thread, &attr, threadFn, pArg );
    pthread_attr_destroy( &attr );

    if ( s != 0 )
    {
	throw "failed to create thread";
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::threadDestroy
//
// INVOCATION:
// cDhsHandler::threadDestroy
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Dectrement the thread counter
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
// SEE ALSO:
// - cDhsHandler::threadCreate
// - cDhsHandler::threadJoin
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsHandler::threadDestroy
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    threadLock.unlock();
    dhsElBroadcast( &dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsHandler::threadJoin
//
// INVOCATION:
// threadJoin();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Wait until all threads started by handlers have completed
//
// DESCRIPTION:
// Run the event loop until the tryLock function returns true.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhsHandler::threadCreate
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsHandler::threadJoin
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    while ( dhsStatus == DHS_S_SUCCESS && threadLockId != pthread_self() )
    {
	dhsEventLoop( DHS_ELT_COND, tryLockFn, &threadLock, &dhsStatus );
    }

    if ( dhsStatus == DHS_S_SUCCESS )
    {
	threadLockId = 0;
	threadLock.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// tryLockFn
//
// INVOCATION:
// N.A.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg (void *) Contains a pointer to a reader/writer lock object.
//
// FUNCTION VALUE:
// (bool) Returns true if the lock was acquired, false otherwise.
//
// PURPOSE:
// This is a function usable as a condition callback function by the 
// dhsEventLoop function
//
// The function returns true if a write lock could be acquired on the
// lock, false otherwise.
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

static bool tryLockFn
( 
    void 	*arg 		// (in)  A pointer to a reader/writer lock
    				//       object.
)
{
    if ( ((cRwLock *)arg)->tryWLock() )
    {
	threadLockId = pthread_self();
	return( true );
    }
    else
    {
	return( false );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cErrHandlerKey::operator<
//
// INVOCATION:
// isLess = ( keyI < keyJ )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) rhs (cErrHandlerKey &) The right hand side of the expression.
//
// FUNCTION VALUE:
// (bool) True if ths < rhs.
//
// PURPOSE:
// Test to see if one key is less than the other
//
// DESCRIPTION:
// Compare the error status and then the error level.  The actual order of
// comparisons isn't important since this is used for searching, not
// sorting.
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

bool		cErrHandlerKey::operator<
(
    const cErrHandlerKey	// (in)  The right hand side of the expression.
    		&rhs
) const
{
    if ( ( ehkErrStatus < rhs.ehkErrStatus ) || 
	    ( ehkErrStatus == rhs.ehkErrStatus && 
	    ehkErrLevel < rhs.ehkErrLevel ) )
    {
	return( true );
    }
    else 
    {
	return( false );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::error
//
// INVOCATION:
// obj.error();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deal with the error discribed in this object
//
// DESCRIPTION:
// Default behaviour is to print the message.
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

void		cDhsError::error
(
    void
)
{
    std::cout << deMessage << std::endl;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::init
//
// INVOCATION:
// obj.init( connect, errStatus, errLevel, msg, tag, thread, pUserData );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) The connection causing the error.
// (>) errStatus (DHS_STATUS) The error status number.
// (>) errLevel (DHS_ERR_LEVEL) The error level.
// (>) msg (char *) The error message.
// (>) tag (DHS_TAG) The command tag associated with the message.
// (>) thread (pthread_t) The thread the message occured in.
// (>) pUserData (void *) The user data pointer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the error parameters in teh cDhsError class
//
// DESCRIPTION:
// Initialize the members of the object.
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

void		cDhsError::init
(
    DHS_CONNECT	connect, 	// (in)  Connection associated with the error.
    DHS_STATUS	errStatus, 	// (in)  The error number.
    DHS_ERR_LEVEL		// (in)  The error level.
		errLevel,
    char	*msg,		// (in)  The error mesasge.
    DHS_TAG	tag, 		// (in)  The tag associated with the mesasge.
    pthread_t	thread,		// (in)  The thread the error occured in.
    void	*pUserData	// (in)  User data pointer associated with 
				//	 the tag.
) 
{
    deConnect = connect;
    deLevel = errLevel;
    deMessage = msg;
    deStatus = errStatus;
    deTag = tag;
    deThread = thread;
    dePUserData = pUserData;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::cDhs
//
// INVOCATION:
// cDhs		obj( "imp name", size );
// cDhs		obj( "imp name", size, DHS_DEBUG_ON );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) The name of the task.
// (>) numConnect (int) Number of connections to allow.
// (>) debug (DHS_DEBUG_LEVEL) Debug level to use.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the DHS class
//
// This method initializes the DHS library.
//
// DESCRIPTION:
// Use the dhs library dhsInit function to inialize the library.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - commonConst - The destructor.
// - ~cDhs - The destructor.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cDhs::cDhs
(
    const char	*name,		// (in)  Task name.
    int		numConnect,	// (in)  Number of connections to allow.
    DHS_DEBUG_LEVEL		// (in)  Initial debug level.
		debugLevel 
)
{
    if ( dIdentity != NULL )
    {
	gen_free( dIdentity );
    }
    dIdentity = strsav( name );
    dNumConnect = numConnect;
    
    commonConst( debugLevel );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::~cDhs
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
// Destructor for the DHS class
//
// This class cleans up the DHS library.
//
// DESCRIPTION:
// Use the dhs library dhsExit function to clean up the library.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs - The constructor.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cDhs::~cDhs
(
    void
)
{
    DHS_STATUS	status;


    mutex.lock();
    cDhsHandler::threadJoin();
    status = DHS_S_SUCCESS;
    dhsExit( &status );
    mutex.unlock();

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::commonConst
//
// INVOCATION:
// obj.commonConst( debugLevel );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) debug (DHS_DEBUG_LEVEL) Debug level to use.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method contains the common part of the cDhs class constructors
//
// This method initializes the DHS library.
//
// DESCRIPTION:
// Use the dhs library dhsInit function to inialize the library.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs - The constructors.
// - ~cDhs - The destructor.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhs::commonConst
(
    DHS_DEBUG_LEVEL		// (in)  Initial debug level.
		debugLevel 
)
{
    DHS_STATUS	status;


    status = DHS_S_SUCCESS;
    dhsInit( dIdentity, dNumConnect, &status );
    dhsSetTimeout( dTimeout );
    dhsDebugLevel( debugLevel, &status );
    dhsCallbackSet( DHS_CBT_GET, (DHS_CB_FN_PTR) cDhs::getCallback, &status );
    dhsCallbackSet( DHS_CBT_PUT, (DHS_CB_FN_PTR) cDhs::putCallback, &status );
    dhsCallbackSet( DHS_CBT_RESPONSE, (DHS_CB_FN_PTR) cDhs::responseCallback, 
	    &status );
    dhsCallbackSet( DHS_CBT_CONNECT, 
	    (DHS_CB_FN_PTR) cDhsConnection::connectCallback, &status );

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::config
//
// INVOCATION:
// cDhs::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the DHS library configuration parameters
//
// DESCRIPTION:
// - Read the "idenity" line from the configuration file.
// - Read the "imp" line from the configuration file.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhs::config
(
    cDhsStatus	&status		// (mod) Function return status.
)
{
    if ( ! status.ok() )
    {
	return;
    }

    dhsStatusClass = &status;
    checkConfig( config_get( "identity", identity ), status, return );
    checkConfig( config_get( "imp", imp ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::getCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_GET, (DHS_CB_FN_PTR) cDhs::getCallback, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection data was sent to.
// (>) tag (DHS_TAG) Tag associated with the get.
// (>) label (char *) Data label sent.
// (>) type (DHS_BD_GET_TYPE) Type of data being retrieved.
// (>) status (DHS_CMD_STATUS) The return status of the command.
// (>) string (char *) The string describing the status.
// (>) avList (DHS_AV_LIST) The attribute value list with the get.
// (>) pData (void *) Pointer to the data buffer.
// (>) length (int) Length of the data buffer.
// (>) pUserData (void *) User data pointer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This is called whenever a get resonse is received from a server
//
// DESCRIPTION:
// Dispatch the callback to the appropriate callback function.
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

void		cDhs::getCallback
(
    DHS_CONNECT	, 		// (in)  Connection data was sent to.
    DHS_TAG	tag, 		// (in)  Tag associated with the get.
    char	*label,		// (in)  Data label sent.
    DHS_BD_GET_TYPE 		// (in)  Type of data being retrieved.
		type,
    DHS_CMD_STATUS		// (in)  The return status of the command.
		cmdStatus,
    char	*string, 	// (in)  The string describing the status.
    DHS_AV_LIST	avList,		// (in)  The attribute value list.
    void	*pData,		// (in)  Pointer to the data buffer.
    int		length,		// (in)  Length of the data buffer.
    void	*pUserData	// (in)  User data pointer.
)
{
    cDhsGetDataset		// Pointer to the dataset for the response.
		*pDs;
    DHS_STATUS	status( DHS_S_SUCCESS );
    cDhsAvList	cAvList( avList );


    pDs = (cDhsGetDataset *) pUserData;
    pDs->ddsPBuffer = pData;
    pDs->ddsLength = length;
    pDs->dgdType = type;
    pDs->label( label );

    if ( pDs->dtTag == DHS_TAG_NULL )
    {
	pDs->dtTag = tag;
    }

    pDs->done( cmdStatus, string, cAvList );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::identity
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the identity line from a configuration file and store it.
//
// DESCRIPTION:
// Save the identity string.
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

boolean		cDhs::identity
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.


    //
    // Check the number of tokens on the line is correct.
    //

    if ( numTokens == 2 )
    {
	//
	// Free any existing identity string.
	//

	if ( dIdentity != NULL )
	{
	    free( dIdentity );
	}


	//
	// Save the new string.
	//

	checkNull( dIdentity = (char *) strsav( tokens[1] ), *dhsStatusClass, 
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	//
	// Number of tokens on the line was wrong.
	//

	dhsStatusClass->E_CONFIG_KWD( *dhsStatusClass, tokens[0] );
	dhsStatusClass->display();
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::imp
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the imp line from a configuration file and store it.
//
// DESCRIPTION:
// Save the number of connections.
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

boolean		cDhs::imp
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.


    //
    // Check that the number of tokens on the line is correct.
    //

    if ( numTokens == 2 || numTokens == 3 )
    {
	//
	// Decode the number of connetions allowed.
	//

	ret = TRUE;
	dNumConnect = atoi( tokens[1] );

	if ( numTokens == 3 )
	{
	    //
	    // Decode the timeout for the server.
	    //

	    dTimeout = atoi( tokens[2] );
	}
    }
    else
    {
	dhsStatusClass->E_CONFIG_KWD( *dhsStatusClass, tokens[0] );
	dhsStatusClass->display();
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::putCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_PUT, (DHS_CB_FN_PTR) cDhs::putCallback, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection data was sent to.
// (>) tag (DHS_TAG) Tag associated with the put.
// (>) status (DHS_CMD_STATUS) The return status of the command.
// (>) string (char *) The string describing the status.
// (>) label (char *) Data label sent.
// (>) pUserData (void *) User data pointer.
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// This function is executed when put requests are received by a server
//
// DESCRIPTION:
// Get the cDhsPutDataset associated with the request and then execute the
// "done" method.
//
// EXTERNAL VARIABLES:
// Any external variables used by this function.
//
// PRIOR REQUIREMENTS:
// Operations that must be performed before calling this function.
//
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// Any known problems with the function.
//-
//***********************************************************************
//

void		cDhs::putCallback
(
    DHS_CONNECT	, 		// (in)  Connection data was sent to.
    DHS_TAG	tag, 		// (in)  Tag associated with the put.
    DHS_CMD_STATUS		// (in)  The return status of the command.
		cmdStatus,
    char	*string, 	// (in)  The string describing the status.
    char	*label, 	// (in)  Data label sent.
    void	*pUserData	// (in)  User data pointer.
)
{
    cDhsPutDataset		// Pointer to the dataset for the response.
		*pDs;
    DHS_STATUS	status( DHS_S_SUCCESS );
    DHS_AV_LIST	avList( dhsResponseGet( tag, -1, &status ) );
    cDhsAvList	cAvList( avList );


    pDs = (cDhsPutDataset *) pUserData;

    if ( pDs->dtTag == DHS_TAG_NULL )
    {
	pDs->dtTag = tag;
    }

    pDs->label( label );

    pDs->done( cmdStatus, string, cAvList );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::responseCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_RESPONSE, (DHS_CB_FN_PTR) cDhs::responseCallback, 
//	    &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection data was sent to.
// (>) tag (DHS_TAG) Tag associated with the put.
// (>) command (char *) Name of command executed.
// (>) status (DHS_CMD_STATUS) The return status of the command.
// (>) string (char *) The string describing the status.
// (>) avList (DHS_AV_LIST) The attribute value list with the get.
// (>) pUserData (void *) User data pointer.
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// This method is executed when command responses are received by a server
//
// DESCRIPTION:
// - Find the command associated with the response.
// - Execute the put or done method as appropriate.
//
// EXTERNAL VARIABLES:
// None
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhs::responseCallback
(
    DHS_CONNECT	, 		// (in)  Connection data was sent to.
    DHS_TAG	tag, 		// (in)  Tag associated with the put.
    char	*, 		// (in)  Name of command executed.
    DHS_CMD_STATUS		// (in)  The return status of the command.
		cmdStatus,
    char	*string, 	// (in)  The string describing the status.
    DHS_AV_LIST	avList,		// (in)  The attribute value list.
    void	*pUserData	// (in)  User data pointer.
)
{
    cDhsClientCommand		// Pointer to the command for the response.
		*pCmd;
    DHS_STATUS	status( DHS_S_SUCCESS );
    				// Function return status.
    cDhsAvList	cAvlist( avList );


    pCmd = (cDhsClientCommand *) pUserData;

    if ( pCmd->dtTag == DHS_TAG_NULL )
    {
	pCmd->dtTag = tag;
    }


    if ( dhsTagDone( tag, &status ) )
    {
	pCmd->done( cmdStatus, string, cAvlist );
    }
    else
    {
	pCmd->response( cmdStatus, string, cAvlist );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::stop
//
// INVOCATION:
// cDhs::stop( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Stop the DHS library event loop
//
// DESCRIPTION:
// Call the dhsEventLoopEnd function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs::start
// - cDhs::startThreaded
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhs::stop
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    mutex.lock();
    dhsEventLoopEnd( &status );
    mutex.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsErrorHandlerBase::cDhsErrorHandlerBase
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
// Constructor for the cDhsErrorHandlerBase class
//
// DESCRIPTION:
// Set up the DHS library error callback function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// A cDhs object must have been created.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cDhsErrorHandlerBase::cDhsErrorHandlerBase
(
    DHS_STATUS		errStatus,	// Error status to handle.
    DHS_ERR_LEVEL	errLevel	// Error level to handle.
)
{
    iErrHandlerList i;		// Iterator into the handler list.
    DHS_STATUS status( DHS_S_SUCCESS );


    //
    // If necessary set up the callback function.
    //

    if ( ! dehCbInit )
    {
	dehOldCallback = dhsCallbackSet( DHS_CBT_ERROR, 
		(DHS_CB_FN_PTR) errorCallback, &status );
	dehCbInit = true;
    }


    //
    // Save the error information.
    //

    dehErrStatus = errStatus;
    dehErrLevel = errLevel;


    //
    // Add the handler to the list of handlers.
    //

    dhRwLock.wLock();

    if ( ( i = dehHandlerList.find( cErrHandlerKey( errStatus, errLevel ) ) ) 
	    == dehHandlerList.end() )
    {
	dehOldHandler = NULL;
    }
    else
    {
	dehOldHandler = (*i).second;
	dehHandlerList.erase( cErrHandlerKey( errStatus, errLevel ) );
    }


    dehHandlerList[ cErrHandlerKey( errStatus, errLevel ) ] = this;
    dhRwLock.unlock();
    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsErrorHandlerBase::~cDhsErrorHandlerBase
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
// Destructor for the cDhsErrorHandlerBase class
//
// DESCRIPTION:
// Reset the DHS library error callback function and the current
// handler pointer.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// A cDhs object must have been created.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cDhsErrorHandlerBase::~cDhsErrorHandlerBase
(
    void
)
{
    DHS_STATUS status;


    dhRwLock.wLock();
    dehHandlerList.erase( cErrHandlerKey( dehErrStatus, dehErrLevel ) );

    if ( dehOldHandler != NULL )
    {
	dehHandlerList[ cErrHandlerKey( dehOldHandler->dehErrStatus, 
		dehOldHandler->dehErrLevel ) ] = dehOldHandler;
    }


    //
    // Reduce the writer lock to a reader lock.
    //

    dhRwLock.downGrade();


    status = DHS_S_SUCCESS;
    if ( dehHandlerList.size() == 0 )
    {
	(void) dhsCallbackSet( DHS_CBT_ERROR, dehOldCallback, &status );
	dehCbInit = false;
    }
    dhRwLock.unlock();


    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsErrorHandlerBase::errorCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_ERROR, cDhsErrorHandlerBase::errorCallback, 
//		&status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection associated with the error.
// (>) errStatus (DHS_STATUS) The error number.
// (>) errLevel (DHS_ERR_LEVEL) The error level.
// (>) msg (char *) The error mesasge.
// (>) tag (DHS_TAG) The tag associated with the mesasge.
// (>) pUserData (void *) User data pointer associated with the tag.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This is assigned as the error callback function for the dhs library
//
// DESCRIPTION:
// Execute the "exec" function in a thread or otherwise.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// A cDhs object must have been created.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsErrorHandlerBase::errorCallback
(
    DHS_CONNECT	connect, 	// (in)  Connection associated with the error.
    DHS_STATUS	errStatus, 	// (in)  The error number.
    DHS_ERR_LEVEL		// (in)  The error level.
		errLevel,
    char	*msg,		// (in)  The error mesasge.
    DHS_TAG	tag, 		// (in)  The tag associated with the mesasge.
    void	*pUserData	// (in)  User data pointer associated with 
				//	 the tag.
)
{
    cDhsError	*errorObject;
    iErrHandlerList i;		// Iterator into the handler list.
    cDhsErrorHandlerBase	// Pointer to the appropriate handler.
    		*pHandler;


    dhRwLock.rLock();
    if ( dehPHandlerList == NULL )
    {
	dhRwLock.unlock();
	return;
    }

    if ( ( i = dehHandlerList.find( cErrHandlerKey( errStatus, 
	    DHS_EL_NULL ) ) ) == dehHandlerList.end() || 
	    ! ((*i).second)->enabled() )
    {
	if ( ( i = dehHandlerList.find( cErrHandlerKey( DHS_S_NULL, 
		errLevel ) ) ) == dehHandlerList.end() || 
		! ((*i).second)->enabled() )
	{
	    if ( ( i = dehHandlerList.find( cErrHandlerKey( DHS_S_NULL, 
		    DHS_EL_NULL ) ) ) == dehHandlerList.end() ||
		    ! ((*i).second)->enabled() )
	    {
		dhRwLock.unlock();
		return;
	    }
	}
    }

    pHandler = (*i).second;

    errorObject = pHandler->newError();
    errorObject->init( connect, errStatus, errLevel, msg, tag, 
	    pthread_self(), pUserData );

    errorObject->error();

    delete errorObject;

    dhRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::typeMap
//
// INVOCATION:
// typeString = cDhsAttrib::typeMap( type );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type (DHS_BD_TYPE) Type to map into a string.
//
// FUNCTION VALUE:
// (char *) the type string.
//
// PURPOSE:
// Turn a data type into a string
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

const char	*cDhsAttrib::typeMap
(
    DHS_DATA_TYPE	type	// (in)  The data type.
)
{
    static char	*strings[] = { "DHS_DT_CHAR", "DHS_DT_INT8", "DHS_DT_UINT8",
	    "DHS_DT_INT16", "DHS_DT_UINT16", "DHS_DT_INT32", "DHS_DT_UINT32",
	    "DHS_DT_FLOAT", "DHS_DT_DOUBLE", "DHS_DT_STRING", "DHS_DT_TAG",
	    "DHS_DT_AVLIST", "DHS_DT_BOOLEAN" };
    if ( type >= DHS_DT_CHAR && type <= DHS_DT_BOOLEAN )
    {
	return( strings[type] );
    }
    else
    {
	return( NULL );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::typeMap
//
// INVOCATION:
// type = cDhsAttrib::typeMap( string );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string (char *) String containing a type name.
//
// FUNCTION VALUE:
// (DHS_BD_TYPE) They type matching the string.
//
// PURPOSE:
// Turn a string into a data type
//
// DESCRIPTION:
// Find the string in the list of data types, and return the associated 
// data type.
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

DHS_DATA_TYPE	cDhsAttrib::typeMap
(
    const char	*string	// (in)  The data type string.
)
{
    int		i;
    static char	*strings[] = { "DHS_DT_CHAR", "DHS_DT_INT8", "DHS_DT_UINT8",
	    "DHS_DT_INT16", "DHS_DT_UINT16", "DHS_DT_INT32", "DHS_DT_UINT32",
	    "DHS_DT_FLOAT", "DHS_DT_DOUBLE", "DHS_DT_STRING", "DHS_DT_TAG",
	    "DHS_DT_AVLIST", "DHS_DT_BOOLEAN" };

    for ( i = 0; i < sizeof( strings ) / sizeof( char * ); i++ )
    {
	if ( strcmp( string, strings[i] ) == 0 )
	{
	    return( (DHS_DATA_TYPE) i );
	}
    }
    return( (DHS_DATA_TYPE) DHS_DT_NONE );
}
