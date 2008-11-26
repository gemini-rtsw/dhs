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
// gen++/h/genRwLock.H
//
// PURPOSE:
// Contains the interface to the cRwLock class.
//
// CLASSES:
// cRwLock - Implents a reader/writer lock.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.5  1998/05/12 20:54:28  nhill
// Updated the documentation.
//
// Revision 1.4  1997/11/30 21:05:31  nhill
// Added function tryRLock, tryWLock, and preLock.
// Added prelock key to the wLock methods.
//
// Revision 1.3  1997/09/26 18:06:21  nhill
// Added method cRwLock::wLock.
//
// Revision 1.2  1997/08/19 19:37:16  nhill
// Fixed a bug in the wLock method.
// Added the downgrade method.
//
// Revision 1.1  1997/02/24 19:43:31  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GENRWLOCK_H
#define	GENRWLOCK_H

#include "genCond.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cRwLock
//
// PUBLIC METHODS: 
// cRwLock - Constructor for the class.
// (void) downGrade - Downgrade a writer lock to a reader lock.
// (int) preLock - Allow a writer lock to be reserved in advance.
// (void) rLock - Get a reader lock.
// (bool) tryRLock - Attempt to get a reader lock.
// (bool) tryWLock - Attempt to get a writer lock.
// (void) unlock - Release the current lock.
// (void) wLock - Get a writer lock.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Implements a reader/writer lock
//
// This class also has features to allow queue jumping.
//
// DESCRIPTION:
// This class uses the cCond class to implement a reader/writer lock.
// The class allows any number of simultaneous read locks, but only one
// write lock.
//
// The preLock method allows queue jumping, and allows locks to be granted
// in a predictable order.
//
// DEFICIENCIES:
// Other than queue jumpers using preLock, there is no particular order
// in which locks will be granted.
//-
//***********************************************************************
//

class cRwLock: public cCond
{
    public:
	inline		cRwLock( void );
	inline void	downGrade( void );
	inline int	preLock( void );
	inline void	rLock();
	inline bool	tryRLock();
	inline bool	tryWLock( int key = 0 );
	inline void	unlock( void );
	inline void	wLock( void )
			{
			    wLock( 0 );
			}
	inline void	wLock( int key );
	inline bool	wLock( const struct timespec &, int key = 0 );
    protected:
    private:
	int	rwlReaders;	// Number of readers.
	int	rwlWriters;	// Number of writers.
	int	rwlNewKey;	// Next lock key to issue.
	int	rwlNextKey;	// Next lock key to notify.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::cRwLock
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
// Constructor for a read/write lock.
//
// DESCRIPTION:
// Initialize the counters to zero.
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

		cRwLock::cRwLock
(
    void
)
{
    rwlReaders = 0;
    rwlWriters = 0;
    rwlNewKey = 1;
    rwlNextKey = 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::downGrade
//
// INVOCATION:
// rwlock.downGrade();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// DownGrades a writer lock to a reader lock
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

void		cRwLock::downGrade
(
    void
)
{
    lock();
    if ( rwlWriters > 0 )
    {
	rwlWriters --;
    }
    cCond::unlock();
    broadcast();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::preLock
//
// INVOCATION:
// key = rwLock.preLock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (int) Lock Key.
//
// PURPOSE:
// This function allows an application to reserve a priority write lock
// key
//
// Once this function has been called, no new read or write locks will
// be permitted until the application has acquired and freed the lock
// with the key (except for other priority locks where the preLock function
// was called before this thread called it). If more than one thread 
// calls this function, the locks will be granted in the order in which 
// the preLock function was called.
//
// Note that failing to acquire a write lock with the key after calling 
// this method is as bad as failing to unlock the lock.
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

int		cRwLock::preLock
(
    void
)
{
    int		key;


    cCond::lock();
    key = rwlNewKey++;
    if ( rwlNextKey == 0 )
    {
	rwlNextKey = key;
    }
    cCond::unlock();

    return( key );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::rLock
//
// INVOCATION:
// lock.rlock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Aquire a read lock on the read/write lock
//
// DESCRIPTION:
// Wait for no writers and then increment the readers counter.
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

void		cRwLock::rLock
(
    void
)
{
    lock();
    while ( rwlWriters != 0 || rwlNextKey != 0 )
    {
	wait();
    }
    rwlReaders ++;
    cCond::unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::tryRLock
//
// INVOCATION:
// success = rwLock.tryRLock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) - True if the lock was acquired, false otherwise.
//
// PURPOSE:
// Attempt to acquire a reader/writer lock for reading, returning
// true of the lock was acquired
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

bool		cRwLock::tryRLock
(
    void
)
{
    bool	ret( false );


    lock();
    if ( rwlWriters == 0 && rwlNextKey == 0 )
    {
	rwlReaders ++;
	ret = true;
    }
    cCond::unlock();

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::tryWLock
//
// INVOCATION:
// success = rwLock.tryWLock();
// success = rwLock.tryWLock( key );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) key (int) A lock key allocated with the preLock method.
//
// FUNCTION VALUE:
// (bool) True if the lock was aquired, false otherwise.
//
// PURPOSE:
// Attempt to acquire a reader/writer lock for reading, returning
// true of the lock was acquired
//
// DESCRIPTION:
// - lock the condition variable.
// - If there are no readers or writers, and no reserved locks, give
//   this thread the lock and return true. Otherwise return false.
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

bool		cRwLock::tryWLock
(
    int		key		// (in)  A lock key allocated with the
    				//	 preLock method.
)
{
    bool	ret( false );


    lock();
    if ( rwlWriters == 0 && rwlReaders == 0 && key == rwlNextKey )
    {
	rwlReaders ++;
	rwlWriters ++;

	ret = true;

	if ( rwlNextKey != 0 )
	{
	    rwlNextKey++;
	    if ( rwlNextKey >= rwlNewKey )
	    {
		rwlNextKey = 0;
		rwlNewKey = 1;
	    }
	}
    }
    cCond::unlock();

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::unlock
//
// INVOCATION:
// lock.unlock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Release a lock on a read/write lock
//
// DESCRIPTION:
// Decrement the appropriate counter and broadcast to the condition
// variable so any threads waiting for the lock can proceed.
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

void		cRwLock::unlock
(
    void
)
{
    lock();
    rwlReaders --;
    if ( rwlWriters > 0 )
    {
	rwlWriters --;
    }
    cCond::unlock();

    broadcast();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::wLock
//
// INVOCATION:
// lock.wLock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Acquire a write lock on a read/write lock
//
// DESCRIPTION:
// Wait until there are no readers and then get the write lock.
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

void		cRwLock::wLock
(
    int		key			// (in) The lock priority key.
)
{
    lock();
    while ( rwlWriters != 0 || rwlReaders != 0 || key != rwlNextKey )
    {
	wait();
    }

    rwlReaders ++;
    rwlWriters ++;

    if ( rwlNextKey != 0 )
    {
	rwlNextKey++;
	if ( rwlNextKey >= rwlNewKey )
	{
	    rwlNextKey = 0;
	    rwlNewKey = 1;
	}
    }

    cCond::unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cRwLock::wLock
//
// INVOCATION:
// clock_gettime( CLOCK_REALTIME, &absTime );
// absTime.tv_sec += 5;
// timedOut = lock.wLock( &absTime );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) absTime (const struct timespec *) - Maximum time to wait for a lock.
//
// FUNCTION VALUE:
// (bool) True if the wait timed out.
//
// PURPOSE:
// Acquire a write lock on a read/write lock or wait until a timeout occurs
//
// DESCRIPTION:
// Wait until there are no readers and then get the write lock.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// - The timeout value could be longer than the specified value if broadcasts
//   are being done regularly.
//-
//***********************************************************************
//

bool		cRwLock::wLock
(
    const struct timespec
		&timeOut,		// (in)  Timeout value.
    int		key			// (in)  The lock priority key.
)
{
    bool	ret = false;

    lock();
    while ( ! ret && ( rwlWriters != 0 || rwlReaders != 0 || 
	    key != rwlNextKey) )
    {
	ret = timedWait( timeOut );
    }
    if ( ! ret )
    {
	rwlReaders ++;
	rwlWriters ++;
    }

    if ( rwlNextKey != 0 )
    {
	rwlNextKey++;
	if ( rwlNextKey >= rwlNewKey )
	{
	    rwlNextKey = 0;
	    rwlNewKey = 1;
	}
    }

    cCond::unlock();

    return( ret );
}

#endif // GENRWLOCK_H
