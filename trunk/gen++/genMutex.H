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
// gen++/h/genMutex.H
//
// PURPOSE:
// Contains the interface to the cMutex class.
//
// CLASSES:
// cMutex - A c++ interface to the pthread mutex class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/05/12 20:54:16  nhill
// Updated the documentation.
//
// Revision 1.2  1997/11/25 17:56:48  cockayne
// Modified declaration of lock() and unlock() to pacify Rose.
//
// Revision 1.1  1997/02/24 19:43:31  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GENMUTEX_H
#define	GENMUTEX_H

#include <pthread.h>

//
//***********************************************************************
//+
// CLASS NAME:
// cMutex
//
// PUBLIC METHODS: 
// cMutex - The constructor.
// ~cMutex - The destructor.
// (void) lock - Lock the mutex.
// (void) unlock - Unlock the mutex.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This is a C++ interface to the posix mutex type
//
// DESCRIPTION:
// Provide lock and unlock methods, and automatic initialization and
// destruction.
//
//-
//***********************************************************************
//

class cMutex
{
    public:
	inline 		cMutex( void );
	inline 		~cMutex( void );
	inline void	lock( void );
	inline void	unlock( void );
    protected:
	pthread_mutex_t	mutex;	//  The mutex.
    //private:
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMutex::cMutex
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

		cMutex::cMutex
(
    void
)
{
    int		s;


    if ( ( s = pthread_mutex_init( &mutex, NULL ) ) != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMutex::~cMutex
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

		cMutex::~cMutex
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
// cMutex::lock
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

void		cMutex::lock
(
    void
)
{
    int		s;


    if ( ( s = pthread_mutex_lock( &mutex ) ) != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMutex::unlock
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

void		cMutex::unlock
(
    void
)
{
    int		s;


    if ( ( s = pthread_mutex_unlock( &mutex ) ) != 0 )
    {
	throw s;
    }
}
#endif // GENMUTEX_H
