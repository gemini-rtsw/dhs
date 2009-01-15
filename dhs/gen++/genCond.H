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
// gen++/h/genCond.H
//
// PURPOSE:
// Whatever
//
// CLASSES:
// <class> - Description
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/25 10:32:33  brighton
// added genBool.H
//
// Revision 1.1.1.1  2002/11/24 20:33:07  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1997/11/30 21:08:52  nhill
// Updated the documentation.
//
// Revision 1.2  1997/09/26 18:06:04  nhill
// Added method cCond::timedWait.
//
// Revision 1.1  1997/02/24 19:43:31  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GENCOND_H
#define	GENCOND_H

#ifdef HAVE_SYNCH_H
#include <synch.h>
#endif

#include <cerrno>
#include <genMutex.H>

#include "genBool.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cCond
//
// PUBLIC METHODS: 
// 		cCond - Constructor.
//		~cCond - Destructor.
// void		broadcast - Broadcast to the condition variable.
// bool		timedWait - Wait for the condition variable with a timeout.
// void		wait - Wait for the condition variable.
//
// PURPOSE:
// Wrapper for a posix threads condition variable
//
// DESCRIPTION:
// Provide a condition variable class.
//
//***********************************************************************
//

class 	cCond : public cMutex
{
    public:
	inline		cCond( void );
	inline		~cCond( void );
	inline void	broadcast( void );
	inline bool	timedWait( const struct timespec & );
	inline void	wait( void );
    protected:
    private:
	pthread_cond_t	cond;	//  The condition variable.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCond::cCond
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
// Constructor for a condition variable
//
// DESCRIPTION:
// Initialize the pthread condition variable.
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

		cCond::cCond
(
    void
)
{
    int		s;


    if ( ( s = pthread_cond_init( &cond, NULL ) ) != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCond::~cCond
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
// Destructor for a condition variable
//
// DESCRIPTION:
// Destroy the pthread condition variable.
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

		cCond::~cCond
(
    void
)
{
    int		s;


    if ( ( s = pthread_cond_destroy( &cond ) ) != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCond::broadcast
//
// INVOCATION:
// cond.broadcast();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Broadcast to a condition variable
//
// DESCRIPTION:
// Use the POSIX thread function to broadcast to the condition variable.
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

void		cCond::broadcast
(
    void
)
{
    int		s;


    lock();
    s = pthread_cond_broadcast( &cond );
    unlock();

    if ( s != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCond::wait
//
// INVOCATION:
// cond.wait();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Wait for a condition variable broadcast
//
// DESCRIPTION:
// Use the pthread cond wait function to wait for the broadcast.
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

void		cCond::wait
(
    void
)
{
    int		s;


    if ( ( s = pthread_cond_wait( &cond, &mutex ) ) != 0 )
    {
	throw s;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCond::timedWait
//
// INVOCATION:
// clock_gettime( CLOCK_REALTIME, &absTime );
// absTime.tv_sec += 5;
// timedOut = cond.wait( &absTime );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) absTime (const struct timespec *) - Timeout for the condition variable.
//
// FUNCTION VALUE:
// (bool) true if a timeout occurred.
//
// PURPOSE:
// Wait for a condition variable broadcast or a timeout
//
// DESCRIPTION:
// Use the pthread cond timedwait function to wait for the broadcast.
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

bool		cCond::timedWait
(
    const struct timespec &		// (in)  Maximum time to wait.
		absTime
)
{
    int		s;


    s = pthread_cond_timedwait( &cond, &mutex, &absTime );
    if ( s == ETIME || s == ETIMEDOUT )
    {
	return( true );
    }
    else if ( s == 0 )
    {
	return( false );
    }
    else
    {
	throw s;
    }
}
#endif /* GENCOND_H */
