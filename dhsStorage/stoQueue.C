static char rcsid[] = "$Id: stoQueue.C,v 1.1.1.1 2002-11-24 20:32:12 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1999				(c) 1999
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
// dhsStorage/src/stoQueue.C
//
// PURPOSE:
// Contains the implementation of the cStoQueue class.
//
// METHOD NAME(S)
// cStoQueue::coexist - Determine if queue action can coexist with the
//			specified action on the same request.
// cStoQueue::execute - Execute the queue action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1  1999/06/17 00:01:30  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "globals.H"
#include "stoAction.H"
#include "stoNode.H"
#include "stoUnit.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoQueue::coexist
//
// INVOCATION:
// if ( coexist( action.getActionType().getAction(), action.rollback() ) )
// ...
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) actionType (STO_ACTION_TYPE) The type of the action being tested.
// (>) isRollback (bool) Is the action being tested a rollback?
//
// FUNCTION VALUE:
// (boolean) True if the actions coexist.
//
// PURPOSE:
// Determine if this action can coexist with another
//
// DESCRIPTION:
// Exclude other Queue actions.
// If this is a rollback action, exclude prepare actions.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoQueue::coexist
(
    STO_ACTION_TYPE
		actionType,	// (in)  The action type tested.
    bool	isRollback	// (in)  Is the other action a rollback?
)
{
    if ( actionType == STO_AT_QUEUE ||
	    ( rollback() && ( actionType == STO_AT_PREPARE && !isRollback ) ) )
    {
	return( false );
    }
    else
    {
	return( true );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoQueue::execute
//
// INVOCATION:
// action->execute();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processing for queueing. 
//
// DESCRIPTION:
// Find the media object in the list of such objects. Set its queueing 
// state to IN-PROGRESS. Save last unit name from the unit list for later. 
// Spawn a process using exec() to run the mediaQueue executable. When
// it completes, check the database to figure out what it did. For each
// new media unit it created, create a new unit object, add it to the list, 
// and update its statistics so the console display is correct. Also
// update the media object statistics so they are correct. 
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

void		cStoQueue::execute
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    DBPROCESS	*dbProcess;		// Database process. 
    int		execStatus;		// Return status from the thread. 
    char	*argv[8];
    char	arg2[10];		// Argument number 2.
    cStoMedia	*pMedia;		// Ptr to media object.
    cStoStatus	status;			// Program status.
    float	dbFloatValue;		//  Float return value from the db.
    					// Saved previous last unit name.
    float	oldQueueSize;		// Amount queuable before queuing.
    
    int		dbIntValue;		// Return value from database.
    int		i;
    

    //
    // Locate media request object in the list of media requests. 
    // Update EPICS to say that queueing is IN_PROGRESS
    //

    pMedia = cStoMedia::find( getMediaRequestId(), NULL );
    if ( rollback() )
    {
	check( pMedia->setQueueState( STO_FST_ROLLBACK_IN_PROGRESS, status ),
		status, return );
    }
    else
    {
	check( pMedia->setQueueState( STO_FST_IN_PROGRESS, status ),
		status, return );
    }


    //
    // Initialize some strings. 
    //

    i = 0;
    argv[i++] = cStoNode::queueTask();
    argv[i++] = "-i";
    sprintf( arg2, "%d", getMediaRequestId() );
    argv[i++] = arg2;
    argv[i++] = "-num_units";
    argv[i++] = "1";
    if ( sqForce )
    {
	argv[i++] = "-force";
    }

    if ( rollback() )
    {
	argv[i++] = "-rollback";
    }
    argv[i++] = NULL;
    

    for ( i = 0; i < sqNumUnits || sqNumUnits == 0; i++ )
    {
	//
	// Spawn process to run mediaQueue. Save thread ID in case of exit. 
	//

	if ( !getStopped() && status.ok() )
	{
	    if ( ( saProcessId = fork1() ) == 0 )
	    {
		if ( execvp( cStoNode::queueTask(), argv ) == -1 )
		{
		    cout << "execlp() failed to start mediaQueue, errno = " << 
			    errno << endl;
		    _exit( 999 );
		}
	    }
	    if ( saProcessId == -1 )
	    {
		status.E_FORK_FAILED( status, cStoNode::queueTask() );
		status.sysErrno();
		status.display();
	    }
	    checkSystem( waitpid( saProcessId, &execStatus, 0 ),
		    status, VOID );
	    saProcessId = -1;
	}


	//
	// Check return value from exec.
	//

	if ( status.ok() )
	{
	    if ( ( WIFEXITED( execStatus ) == 0 ) )
	    {
		status.E_EXEC_ABNORMAL_EXIT( status, cStoNode::queueTask() );
	    }
	    else
	    {
		if ( WEXITSTATUS( execStatus ) != 0 &&
		     NEGEXITSTATUS( execStatus ) )
		{
		    status.E_EXEC_NONZERO_EXIT( status, cStoNode::queueTask(), 
			    WEXITSTATUS( execStatus ) - 255 );
		}
	    }
	    

	    //
	    // Reload the list of units in order to get all the changes caused
	    // by this execution of mediaQueue.
	    //

	    check( cStoUnit::init( status ), status, return );


	    pMedia->lock();
	    dbProcess = cStoDbManager::acquire();
	    checkAm( amNumPreparable( dbProcess, pMedia->getMediaRequestId(), 
		    &dbIntValue ), status, VOID );

	    pMedia->setPrepNum( dbIntValue );
	    checkAm( amAmountQueueable( dbProcess, pMedia->getMediaRequestId(), 
		    &dbFloatValue ), status, VOID );
	    cStoDbManager::release();

	    oldQueueSize = pMedia->getQueueSize();
	    pMedia->setQueueSize( (long long) dbFloatValue );


	    if ( WEXITSTATUS( execStatus ) != 0 || 
		    oldQueueSize == pMedia->getQueueSize() ||
		    ( rollback() && pMedia->getPrepNum() <= 0 ) )
	    {
		pMedia->unlock();
		break;
	    }
	    pMedia->unlock();
	}
    }


    return;
}
