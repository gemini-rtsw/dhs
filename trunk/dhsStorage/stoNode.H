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
// dhsStorage/h/stoNode.H
//
// PURPOSE:
// The Storage Server manages a number of processes, up to a configurable
// integer value (see dhsStorage.config). Each process requires a 
// processing node. This file contains code to support processing nodes. 
//
// CLASSES:
// cStoNode - Processing Node and supporting methods. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.9  1999/10/12 23:22:24  nhill
// Updated the documentation.
//
// Revision 1.8  1999/06/16 23:50:40  nhill
// Major re-organization.
//
// Revision 1.7  1999/02/24 23:55:54  jaeger
// Removed environment and toolpath from configuration file.  Added
// queue, prep, and write executable names to the configuration file.
//
// Revision 1.6  1998/05/15 17:19:20  cockayne
// Added #defines and static data members to support starting the tools.
//
// Revision 1.5  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.4  1998/04/08 20:58:34  cockayne
// Added setStop() method.
//
// Revision 1.3  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STONODE_H
#define	STONODE_H

#define NO_FIND_CREATE
#include "genMutex.H"
#include "genCond.H"
#include "genListOf.H"
#include "globals.H"
#include "stoAction.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cStoNode
//
// PUBLIC METHODS: 
// (cStoNode)	cStoNode	- Constructor (two types).
// ()		~cStoNode	- Deconstructor.
// (void)	broadcast	- Broadcast to the node condition variable.
// (void)	config		- Get configurable data from config file. 
// (void)	init		- Initalize list of processing nodes.
// (void)	exit		- Nuke nodes, free resources held by class.
// (void *)	execThread	- pthread_create() exec method. Contains
//				  node processing loop.
// (char *)	queueTask	- Get the name of the queue task.
// (char *)	prepTask	- Get the name of the prep task.
// (char *)	writeTask	- Get the name of the write task.
// (char *)	cleanupTask	- Get the name of the cleanup task.
// (void)	simulate	- Set the simulate level.
// (bool)	simulate	- Get the simulate level.
// (boolean)	checkStopFlag	- Condition method to check from within
//				  dhsEventLoop( DHS_ELT_COND, ... ).
// (bool)	getStop		- Get value of stop flag. 
// (void)	setStop		- Set value of stop flag.
// (void)	setInUse	- Set value of in-use flag.
// (bool)	getInUse	- Get value of in-use flag.
// (bool)	getActive	- Get value of active flag.
// (int)	getNumNodes	- Get the number of nodes. 
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Provides functionality for processing nodes.
//
// Each node creates a loop, which removes actions from the action queue, 
// and executes "execute" method on the action, which causes the action 
// to be processed, and then deletes the action.
//-
//***********************************************************************
//

class cStoNode :	
	public cListOf< cStoNode, int, cStoStatus >
{
    public:
	//
	// Constructors, Deconstructors.
	//

	inline		cStoNode( void )
			:   cListOf< cStoNode, int, cStoStatus >
				( -1, false ),
	                        snProcessThreadId( 0 ), // XXX allan: changed from -1 to 0
			        snLoopThreadId( 0 )  // XXX allan: changed from -1 to 0
			{ 
			};
			cStoNode( int, bool = true );
			~cStoNode( void );


	//
	// Static Methods.
	//
	
	static void	broadcast( void )
			{
			    snCond.broadcast();
			}
	static void	config( cStoStatus & );
	static void	init( cStoStatus & );
	static void	exit( cStoStatus & );
	static void	*execThread( void * );
	static char	*queueTask( void )
			{
			    return( snQueue );
			}
	static char	*prepTask( void )
			{
			    return( snPrep );
			}
	static char	*writeTask( void )
			{
			    return( snWrite );
			}
	static char	*cleanupTask( void )
			{
			    return( snCleanup );
			}
	static void	simulate( bool value )
			{
			    snSimulate = value;
			}
	static bool	simulate( void )
			{
			    return( snSimulate );
			}


	//
	// Non-static methods.
	//
	
	inline static boolean	
			checkStopFlag( void * arg )
			{
			    cStoNode	*pNode;

			    pNode = (cStoNode *) arg;
			    if ( !pNode->getStop() )
			    {
				return( TRUE );
			    }
			    return( FALSE );
			};
	void		processActions( void );
	inline bool	getStop( void )
			{
			    return( snStop );
			};
	inline void	setStop( bool b )
			{
			    snStop = b;
			};
	inline void	setInUse( bool b )
			{
			    snInUse = b;
			};
	inline bool	getInUse( void )
			{
			    return( snInUse );
			};
	inline bool	getActive( void )
			{
			    return( snActive );
			};
	inline static int 
			getNumNodes( void )
			{
			    return( snNumNodes );
			};
    protected:
    private:
	//
	// Private methods.
	//

	static boolean	amApplications( int, char *[] );
	static boolean	numNodes( int, char *[] );


	//
	// Private Data.
	//

	static cCond	snCond;		// Condition variable to control nodes.
	static int	snNumNodes;	// Configured number of nodes.
	static char	*snQueue;	// Configured AM queue application.
	static char	*snPrep;	// Configured AM prepare application.
	static char	*snWrite;	// Configured AM write application.
	static char	*snCleanup;	// Configured AM cleanup application.
	static bool	snSimulate;	// Is simulation turned on?
 

	int		snTotalSize;	// Total size retrieved so far.

	bool		snInUse;	// Is this processing node in use? 	
	pid_t		snProcessThreadId;	
					// ID of processing thread. -1 if not 
					//     in use. 
	pthread_t	snLoopThreadId;	// ID of loop thread.
	bool		snActive;	// Node sets to true when action
					//    is in-progress.
	bool		snKill;		// Set to true when we want to kill the
					//    node - on re-init or exit. 
	bool		snStop;		// Set to true when we want action to
					//    stop.
};
#endif // STONODE_H
