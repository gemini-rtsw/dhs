static char rcsid[] = "$Id: stoNode.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoNode.C
//
// PURPOSE:
// Contains methods of the cStoNode class.
//
// METHOD NAME(S)
// cStoNode::cStoNode		- Constructor.
// cStoNode::~cStoNode		- Deconstructor.
// cStoNode::init		- Initialize list of processing nodes.
// cStoNode::exit		- Exit, freeing resources held by the class.
// cStoNode::execThread		- Thread callable function to start the node's
//				  processing loop.
// cStoNode::processActions	- The node's processing loop.
// cStoNode::config		- Configuration method.
// cStoNode::amApplications	- Parses am applications from the config file.
// cStoNode::numNodes		- Parses number-of-nodes configuration lines. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:07  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.13  1999/06/16 23:58:52  nhill
// Major re-organization of the code. Most of this file moved to the
// action specific action classes.
//
// Revision 1.12  1999/03/01 17:27:37  jaeger
// Ensured number of copies written never is below zero.
//
// Revision 1.11  1999/02/25 00:10:20  jaeger
// Removed environment and toolpath information from configuration file.
// Added name of queue, prep, and write executables to the configuration
// file.  Ensured pConnect is initialized to NULL before using STL find and
// after STL find check to see if it is NULL.  Fixed retrieve so it does
// not try to retrieve indefinitely for archive media requests.  Updated
// to work with new AM library and new AM applications.  Use gen_file library
// to deal with verification and deletion of files.
//
// Revision 1.10  1998/07/31 21:07:07  cockayne
// Fixed bug in how readdir_r() is called. The man page suggests that to use
// readdir_r(), you must pass in a buffer which has been allocated to be the
// size of the dirent struct plus the length of the filename. This was causing
// a segmentation fault in the optimized executable.
//
// Revision 1.9  1998/06/24 16:04:16  cockayne
// Added code to create top-level directory if required, in retrieve().
//
// Revision 1.8  1998/05/25 19:10:21  cockayne
// Added further error checking when attempting to fork a process.
// Also added more message printing and logging for the case of
// action starting, stopping, and completion.
//
// Revision 1.7  1998/05/15 17:12:39  cockayne
// Many modifications, for error checking, program flow.
//
// Revision 1.6  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.5  1998/04/20 20:56:41  cockayne
// Added casting to take care or integer overflow problem.
//
// Revision 1.4  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 21:03:28  cockayne
// Moved dhsElBroadcast() call to execThread() instead of the
// individual action processing methods.
//
// Revision 1.2  1998/03/26 21:21:15  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <csignal>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <cerrno>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
#include "gen_file.h"	
}

#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoAction.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoFile.H"
#include "stoMedia.H"
#include "stoUnit.H"
#include "stoStage.H"
#include "stoGet.H"
#include "stoConnection.H"
#include "stoNode.H"

//
// Define check for negative exit status.
//

#define NEGEXITSTATUS(stat) 	( ((int)(((stat)>>8)&0x1)) == 0 )
    
//
// Define configuration strings.
//
 
#define STO_CFG_AMAPPS		"amApps"
#define STO_CFG_NUMNODES        "numNodes"

//
// Define static data in cStoNode class.
//

cCond		cStoNode::snCond;	// Condition variable to protect nodes.
int		cStoNode::snNumNodes;	// Configured number of nodes.
char		*cStoNode::snQueue=NULL;
    					// Configured AM queue app.
char		*cStoNode::snPrep=NULL;	// Configured AM queue app.
char		*cStoNode::snWrite=NULL;// Configured AM queue app.    
char		*cStoNode::snCleanup=NULL;// Configured AM cleanup app.    
bool		cStoNode::snSimulate=false;// Simulate mode.
    


//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::cStoNode
//
// INVOCATION:
// cStoNode( nodeNumber, addToList );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) nodeNumber	(int)	- Number to index the list of nodes.
// (>) addToList	(bool)	- Set to true if adding to list.
//
// FUNCTION VALUE:
// (cStoNode) new processing node object.
//
// PURPOSE:
// Constructor for the cStoNode class. 
//
// DESCRIPTION:
// Construct all the node's data members, and create a processing
// loop for the node in its own thread. 
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

		cStoNode::cStoNode
(
    int		nodeNumber,	// Number to index the list. 
    bool	addToList	// Set to true if adding to list. 
)
:   cListOf< cStoNode, int, cStoStatus >( nodeNumber, addToList ),
    snInUse( false ),
    snProcessThreadId( 0 ), // XXX allan: was -1
    snLoopThreadId( 0 ), // XXX allan: was -1
    snActive( true ),
    snKill( false ),
    snStop( false )
{
    pthread_attr_t
    		attr;		// POSIX thread attributes.
    int		pthread_status;	// pthread_create function return status;
    
    
    //
    // Start a threaded processing loop for the node.
    //

    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    pthread_status = pthread_create( &snLoopThreadId, &attr, 
	    execThread, this );
    pthread_attr_destroy( &attr );
    if ( pthread_status != 0 )
    {
	throw "Failed to create a thread.";
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::~cStoNode
//
// INVOCATION:
// ~cStoNode();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Deconstructor for the cStoNode class. 
//
// DESCRIPTION:
// If the node is currently running an exec() to process an action, 
// kill that process. Broadcast to cause the node's processing loop 
// to exit. 'Join' to the node's processing thread, in effect waiting
// until the thread dies.
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

		cStoNode::~cStoNode
(
    void
)
{
    cStoStatus	status;			// Program status. 


    //
    // If this node is currently running a process, kill it.
    //
    
    if ( snProcessThreadId != -1 )
    {
	checkSystem( kill( snProcessThreadId, SIGINT ), status, VOID );
    }
    snActive = false;
    snCond.lock();
    snKill = true;
    snCond.unlock();
    snCond.broadcast(); 


    //
    // Wait until the processing loop ends. 
    //

    if ( snLoopThreadId != -1 )
    {
	pthread_join( snLoopThreadId, NULL );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::init
//
// INVOCATION:
// init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cstoStatus &)	- Program status. 
//
// FUNCTION VALUE:
// None.
// 
// PURPOSE:
// Initialize the list of processing nodes.
//
// DESCRIPTION:
// Pretty basic. 
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

void		cStoNode::init
(
    cStoStatus	&status		// (mod) Program status.
)
{
    int		count;		// A counter. 


    checkStat( status, return );

    
    //
    // Create configured number of nodes.
    //

    for ( count = 0; count < snNumNodes; count++ )
    {
	checkNull ( ( new cStoNode( count ) ), status, break );
    }
    
    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::exit
//
// INVOCATION:
// exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &)	- Program status. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Exit method for cStoNode class. 
//
// DESCRIPTION:
// Delete all nodes, free space held by the class. 
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

void		cStoNode::exit
(
    cStoStatus	&status		// (mod) Program status.
)
{
    checkStat( status, return );

    
    //
    // Halt any processing on the nodes and deconstruct them. 
    //

    deleteAll();


    //
    // Free space used by configuration items.
    //
    
    if ( snQueue != NULL )
    {
	gen_free( snQueue );
	snQueue = NULL;
    }

    if ( snPrep != NULL )
    {
	gen_free( snPrep );
	snPrep = NULL;
    }

    if ( snWrite != NULL )
    {
	gen_free( snWrite );
	snWrite = NULL;
    }
    
    snNumNodes = 0;
			      
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode:execThread
//
// INVOCATION:
// n/a. invoked by pthread_create() function, never directly. 
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg  (void *)	- pthread_create() passes this arg to execcThread().
//			  In this case, it is a pointer to the node object.
//
// FUNCTION VALUE:
// (void *) NULL
//
// PURPOSE:
// The node's processing loop. 
//
// DESCRIPTION:
// The node enters its loop, and stays there until killed. It locks and 
// waits to be awoken to process an action. When it is awoken, it calls
// the appropriate action processing method. On completion, it resets
// the stop flag and broadcasts to stop the 'extra' DHS event loop if
// necessary, then deletes the action from the action queue, then 
// waits again, until it is awoken for more proccessing. 
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

void		*cStoNode::execThread
(
    void *	arg
)
{
    cStoNode	*pNode;		// ptr to this node. 


    pNode = (cStoNode *) arg;
    pNode->processActions();

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::processActions
//
// INVOCATION:
// node.processActions();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Process actions forever.
//
// DESCRIPTION:
// Try to get an non-active action. 
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

void		cStoNode::processActions
(
    void
)
{
    cStoAction	*pAction;	// ptr to action object. 
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
    cStoStatus	status;


    for ( ; ; )
    {
	//
	// Try to get an action to process.
	//

	pAction = NULL;
	snCond.lock();
	while ( ( pAction = cStoAction::getNonActiveAction() ) == NULL && 
		! snKill )
	{
	    snCond.wait();
	}
	snCond.unlock();


	if ( snKill )
	{
	    break;
	}


	//
	// Investigate what we're supposed to do and do it. 
	//

	status.S_ACTION_STARTING( status, 
		pAction->getActionType().getActionTypeString(), 
		pAction->getMediaRequestId() );


	pAction->execute();

	//
	// If the action was stopped by a cancel command, there is an 
	// event loop being run which needs to stop. Setting the flag 
	// and dhsELBroadcast() accomplish this.
	//

	if ( getStop() )
	{
	    setStop( false );
	    checkDhs( dhsElBroadcast( &dhsStatus ), 
		    dhsStatus, status, VOID );
	}
	status.S_ACTION_COMPLETE( status, 
		pAction->getActionType().getActionTypeString(), 
		pAction->getMediaRequestId() );


	cStoAction::queueLock();
	delete pAction;
	cStoAction::queueUnlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::config
//
// INVOCATION:
// cStoNode::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read configuration parameters
//
// DESCRIPTION:
// - Read the "numNodes" line from the configuration file.
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

void            cStoNode::config
(
    cStoStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );


    checkConfig( config_get( STO_CFG_NUMNODES, numNodes ),
	    status, return );
    checkConfig( config_get( STO_CFG_AMAPPS, amApplications ), 
	    status, return );    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNode::amApplications
//
// INVOCATION:
// n/a. Invoked only by the config_get() routine, never directly. 
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of 
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse configuration file lines with keyword "amApps" from the
// configuration file.
//
// DESCRIPTION:
// Save the name of the queue, prep, and write applications for use later.
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

boolean		cStoNode::amApplications
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean 	ret;		// Function return value. 
    cStoStatus	status;		// Status code. 


    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 5 )
    {
	if ( snQueue != NULL )
	{
	    gen_free( snQueue );
	    snQueue = NULL;
	}
	checkNull( ( snQueue = strsav( tokens[1] ) ), status, ret = FALSE );


	if ( snPrep != NULL )
	{
	    gen_free( snPrep );
	    snPrep = NULL;
	}
	checkNull( ( snPrep = strsav( tokens[2] ) ), status, ret = FALSE );


	if ( snWrite != NULL )
	{
	    gen_free( snWrite );
	    snWrite = NULL;
	}
	checkNull( ( snWrite = strsav( tokens[3] ) ), status, ret = FALSE );


	if ( snCleanup != NULL )
	{
	    gen_free( snCleanup );
	    snCleanup = NULL;
	}
	checkNull( ( snCleanup = strsav( tokens[4] ) ), status, ret = FALSE );
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
// cStoNode::numNodes
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of 
//           tokens is correctly parseable. FALSE otherwise. 
//
// PURPOSE:
// To parse configuration file lines with keyword "numNodes" from the
// configuration file.
//
// DESCRIPTION:
// Ensure the number of tokens on the configuration line is correct, then
// save the number of nodes. 
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

boolean		cStoNode::numNodes
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean 	ret;		// Function return value. 
    cStoStatus	status;		// Status code. 


    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 2 )
    {
	//
	// Store the number of nodes. 
	//

	if ( !str2int( tokens[1], &snNumNodes ) )
	{
	    status.E_NUMNODES( status, tokens[1] );
	    status.E_CONFIG_KWD( status, tokens[0] );
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
