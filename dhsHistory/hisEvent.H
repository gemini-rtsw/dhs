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
// dhsHistory/h/hisCa.H
//
// PURPOSE:
// Contains the interface to the cHisCa class
//
// CLASSES:
// cHisCa - Takes care of the EPICS Channel Access stuff.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:22  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1997/10/20 21:48:07  cockayne
// Check-in for install.
//
// Revision 1.1  1997/10/06 20:16:37  cockayne
// Initial revision
//
// Revision 1.2  1997/08/20 21:19:48  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef HISEVENT_H
#define	HISEVENT_H

#include <string>
#include <list>
#include <map>

// #include "queue.h" XXX allan: can't find header file
#include <queue>

#include "stack.h"
#include "genMutex.H"
#include "genCond.H"



//
//***********************************************************************
//+
// CLASS NAME:
// cHisEvent
//
// PUBLIC METHODS: 
// 		cHisEvent	- Contructor.
// (void)	filepathCreate	- Creates the filepath within the class. 
// (void) 	fileOpen	- Opens the file. 
// (void) 	fileClose	- Closes the file.
// (void)	fileRename	- Renames the file. 
// (void)	queueLock	- Locks the event queue. 
// (void)	queueUnlock	- Unlocks the event queue.
// (void) 	queueWait	- Waits for a queueBroadcast. 
// (void)	queueBroadcast	- Broadcasts. 
// (bool) 	queueEmpty	- Returns whether or not the queue is empty.
// (cHisEvent *)queueFront	- Returns ptr to event at front of queue.
// (void)	queuePop	- Pops the first event off the queue.
// (void) 	stopQueueLoop	- Stops the event queue processing loop. 
// (void)	startQueueLoop	- Starts the event queue processing loop.
// (void)	execThread	- Routine containing the processing loop.
// (void)	queuePush	- Pushes the event onto the end of the queue. 
// (time_t)	date		- Returns event's date.
// (long)	nsec		- Returns event's nanosecond count. 
// (char *)	SIR		- Returns event's SIR record label.
// (char *) 	value		- Returns event's SIR record value.
// (char *)	writeRecord	- Writes an event to file. 
// (char *)	readRecord	- Reads an event record from file. 
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Stores information about an event occurence. 
//
//-
//***********************************************************************
//

class	cHisEvent
{
    public:
	//
	// Constructors, Deconstructors.
	//

	inline          cHisEvent( void )
			{
			    return;
			};
	inline          cHisEvent( time_t date,
				    long nsec,
				    char *SIR,
				    char *value )
			{
			    heDate = date;
			    heNsec = nsec;
			    strncpy( heSIR, SIR, 40 );
			    strncpy( heValue, value, 40 );
			};
	
	//
	// Static Methods.
	//
	
	static void	filepathCreate ( cHisStatus & );
	static void     fileOpen( cHisStatus & );
	static void     fileClose( cHisStatus & );
	static void	fileRename( char *, cHisStatus & );
	static inline void
			queueLock( void ) 
			{ 
			    heCondQueue.lock(); 
			};
	static inline void
			queueUnlock( void ) 
			{
			    heCondQueue.unlock(); 
			};
	static inline void
			queueWait( void ) 
			{ 
			    heCondQueue.wait(); 
			};
	static inline void
			queueBroadcast( void ) 
			{
			    heCondQueue.broadcast(); 
			};
	static inline bool
			queueEmpty( void ) 
			{ 
			    return( heQueue.empty() ); 
			};
	static inline cHisEvent *
			queueFront( void ) 
			{ 
			    return( heQueue.front() ); 
			};
	static inline void
			queuePop( void ) 
			{ 
			    heQueue.pop(); 
			};
	static inline void
			stopQueueLoop( void )
			{
			    heQueueLoopDone = true;
			    heCondQueue.broadcast();
			};

	static void	startQueueLoop( cHisStatus & );
	static void     *execThread( void * );


	//
	// Non-static methods.
	//
	
	inline void	queuePush( void ) 
			{ 
			    heQueue.push( this ); 
			};
	inline time_t	date( void ) 
			{ 
			    return( heDate ); 
			};
	inline long	nsec( void ) 
			{ 
			    return( heNsec ); 
			};
	inline char	*SIR( void ) 
			{ 
			    return( &(heSIR[0]) ); 
			};
	inline char	*value( void ) 
			{ 
			    return( &(heValue[0]) ); 
			};

	void		writeRecord( void );
	void		readRecord( int, bool & );


	//
	// Typedefs
	//

        typedef queue< cHisEvent *, list< cHisEvent * > >
			tEventQueue;        // The queue of events.

	
    protected:
    private:
	//
	// Private Data.
	//
    
	static char	*heFilepath;
	static int	heFileDesc;	// File descriptor for history log.
	static cCond	heCondQueue;	// hcEventQueue Condition Variable.
	static tEventQueue
			heQueue;	// Queue of History 'Events'.
	static bool	heQueueLoopDone;
					// Set when event loop should exit.
		
	
	time_t		heDate;		// Date of the event
	long		heNsec;		// Nanoseconds of the event. 
	char		heSIR[40];	// SIR we are recording. 
	char		heValue[40];	// SIR's value. 
};
#endif // HISEVENT_H
