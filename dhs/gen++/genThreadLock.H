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
// gen++/h/genThreadLock.H
//
// PURPOSE:
// Contains the interface to the cThreadLock class.
//
// CLASSES:
// cThreadLock - A c++ interface to a mutex like construct.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.1  1998/06/24 23:14:16  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GENTHREADLOCK_H
#define	GENTHREADLOCK_H

#include <pthread.h>

//
//***********************************************************************
//+
// CLASS NAME:
// cThreadLock
//
// PUBLIC METHODS: 
// cThreadLock - The constructor.
// ~cThreadLock - The destructor.
// (void) lock - Lock the mutex.
// (void) unlock - Unlock the mutex.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This is a C++ interface to a mutex like construct
//
// This construct differes from the cMutex class in that the same thread
// can succeed in locking the mutex more than once.
//
// DESCRIPTION:
// Provide lock and unlock methods, and automatic initialization and
// destruction.
//
//-
//***********************************************************************
//

class cThreadLock
{
    public:
	inline 		cThreadLock( void );
	inline 		~cThreadLock( void );
	inline void	lock( void );
	inline void	unlock( void );
    protected:
	pthread_mutex_t	mutex;	//  The mutex.
    private:
	pthread_t	tlThread;	// The thread that has the mutex.
	int		tlCount;	// Number of times the thread has the
					// mutex.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cThreadLock::cThreadLock
//
// INVOCATION:
// N.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize a mutex
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

		cThreadLock::cThreadLock
(
    void
)
{
    int		s;


    if ( ( s = pthread_mutex_init( &mutex, NULL ) ) != 0 )
    {
	throw s;
    }

    tlThread = 0;
    tlCount = 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cThreadLock::~cThreadLock
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
// Destroy a mutex
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

		cThreadLock::~cThreadLock
(
    void
)
{
    int		s;


    if ( ( s = pthread_mutex_destroy( &mutex ) ) != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cThreadLock::lock
//
// INVOCATION:
// mutex.lock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Lock a mutex
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

void		cThreadLock::lock
(
    void
)
{
    int		s;
    pthread_t	self;


    self = pthread_self();

    if ( tlThread == self )
    {
	tlCount++;
    }
    else
    {
	if ( ( s = pthread_mutex_lock( &mutex ) ) != 0 )
	{
	    throw s;
	}
	tlThread = self;
	tlCount = 1;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cThreadLock::unlock
//
// INVOCATION:
// mutex.unlock();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Unlock a mutex
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

void		cThreadLock::unlock
(
    void
)
{
    int		s;


    tlCount --;
    if ( tlCount == 0 )
    {
	tlThread = 0;
	if ( ( s = pthread_mutex_unlock( &mutex ) ) != 0 )
	{
	    throw s;
	}
    }
}
#endif // GENTHREADLOCK_H
