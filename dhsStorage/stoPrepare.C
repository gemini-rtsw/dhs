static char rcsid[] = "$Id: stoPrepare.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoPrepare.C
//
// PURPOSE:
// Contains the implementation of the cStoPrepare class.
//
// METHOD NAME(S)
// cStoPrepare::coExist - Determine if the prepare action can co-exist with
// 				the specified action on the same request.
// cStoPrepare::execute - Execute this prepare action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:10  brighton
// Imported sources
//
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "globals.H"
#include "stoAction.H"
#include "stoStage.H"
#include "stoUnit.H"
#include "stoNode.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPrepare::coexist
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
// Exclude other prepare actions.
// If this is a rollback action, exclude write actions.
// If this is a forward action, exclude queue rollback actions.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoPrepare::coexist
(
    STO_ACTION_TYPE
		actionType,	// (in)  The action type tested.
    bool	isRollback	// (in)  Is the other action a rollback?
)
{
    if ( actionType == STO_AT_PREPARE ||
	    ( rollback() && ( actionType == STO_AT_WRITE && ! isRollback ) ) ||
	    ( ! rollback() && ( actionType == STO_AT_QUEUE && isRollback ) ) )
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
// cStoPrepare::execute
//
// INVOCATION:
// action.execute();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processing for preparing. 
//
// DESCRIPTION:
// Find the media object in the list of such objects. Set its prepare
// state to IN-PROGRESS. Enter a loop which will run until we've done
// preparing physical images to write to CDROM. Fetch the next prep'able
// unit from the list, and fetch a usable staging directory from the list
// of staging directories. Use exec() to spawn a process to run cdPrep
// which will prepare the unit. On its exit, query the database to 
// discover what it did. Update the unit and media objects to reflect the
// new state of the unit. Go on to the next unit. When all preparing has
// been done. On the way out, set the prepare state back to IDLE for the 
// media object. 
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

void		cStoPrepare::execute
(
    void
)
{
    cStoStage::iObjectList
    		dirIter;		// Iterator for the stage dir list. 
    cStoMedia	*pMedia;		// Ptr to media object.
    cStoStatus	status;			// Program status.
    cStoUnit	*pUnit;			// Ptr to media unit object.
    AM_UNIT	amUnit;			// am library media unit struct;
    cStoStage	*pStageDir;		// Ptr to staging directory object. 
    cStoStage	*pStageDir2;		// Ptr to staging directory object. 
    int		availToProcess;		// Number available to prepare. 
    int		execStatus;		// Return status from the thread. 
    int		count;			// A counter. 
    char	*argv[8];
    char	arg2[20];		// Argument number 2.
    int		amStatus;		// am library routine return status.
    int		dbIntValue;		// Int value from database.
    DBPROCESS 	*dbProcess;		// Database process. 
    int		i;
    int		i1;

    
    //
    // Locate media request object in the list of media requests. 
    // Update EPICS to say that preparing is IN_PROGRESS
    //

    pMedia = cStoMedia::find( getMediaRequestId(), NULL );
    if ( rollback() )
    {
	check( pMedia->setPrepareState( STO_FST_ROLLBACK_IN_PROGRESS, status ),
		status, return );
    }
    else
    {
	check( pMedia->setPrepareState( STO_FST_IN_PROGRESS, status ),
		status, return );
    }


    //
    // Initialize some strings. 
    //

    i = 0;
    argv[i++] = cStoNode::prepTask();
    argv[i++] = "-i";
    (void) sprintf( arg2, "%d", getMediaRequestId() );
    argv[i++] = arg2;
    if ( rollback() )
    {
	argv[i++] = "-rollback";
    }


    //
    // Either do the number requested or do the number available to prep.
    //

    if ( rollback() )
    {
	availToProcess = pMedia->getWriteNum();
    }
    else
    {
	availToProcess = pMedia->getPrepNum();
    }
    for ( count = 0; status.ok() && count < availToProcess && 
	    ( spNumPrepare == 0 || count < spNumPrepare ); 
	    count++ )
    {
	i1 = i;


	//
	// Get next preparable unit from units list. 
	//

	if ( rollback() )
	{
	    if ( ( pUnit = cStoUnit::findLastUnit( 
		    getMediaRequestId(), AMU_PREPARED ) ) == NULL )
	    {
		//
		// No Units to rollback. Quit.
		//

		status.E_PREP_ROLLBACK( status, getMediaRequestId() );
		return;
	    }
	}
	else
	{
	    if ( ( pUnit = cStoUnit::findFirstUnit( 
		    getMediaRequestId(), AMU_QUEUED ) ) == NULL )
	    {
		//
		// No Units to prepare. Quit.
		//

		status.S_NO_PREPARABLE( status, getMediaRequestId() );
		return;
	    }
	}
	

	//
	// See if this unit is already using a staging directory.
	//

	pStageDir = NULL;
	cStoStage::listRLock();
	for ( dirIter = cStoStage::begin(); 
		dirIter != cStoStage::end(); 
		dirIter++ )
	{
	    pStageDir2 = ((*dirIter).second);
	    if ( pStageDir2->getInUse() && 
		    streq( pStageDir2->getMediaUnitName(),
		    pUnit->getMediaUnitName() ) && 
		    pStageDir2->getMediaRequestId() == 
		    pUnit->getMediaRequestId() )
	    {
		pStageDir = pStageDir2;
		break;
	    }
	}
	cStoStage::listUnlock();


	if ( ! rollback() )
	{
	    //
	    // See if there's an available staging directory. 
	    //

	    if ( pStageDir == NULL )
	    {
		cStoStage::listRLock();
		for ( dirIter = cStoStage::begin(); 
			dirIter != cStoStage::end(); 
			dirIter++ )
		{
		    if ( !((*dirIter).second)->getInUse() )
		    {
			pStageDir = ( (*dirIter).second );
			break;
		    }
		}
		cStoStage::listUnlock();
		if ( pStageDir == NULL )
		{
		    //
		    // No free directories, Quit. 
		    //

		    status.S_NO_STAGE_DIRECTORIES( status, 
			    pUnit->getMediaUnitName(), 
			    getMediaRequestId() );
		    break;
		}
	    }
	    argv[i1++] = "-d";
	    argv[i1++] = pStageDir->getDirectory();
	}


	//
	// Set up the arguments to pass to cdPrep in the execle() call. 
	//

	argv[i1++] = "-n";
	argv[i1++] = pUnit->getMediaUnitName();
	argv[i1++] = NULL;

	
	//
	// Spawn process to run cdPrep. Save thread ID in case of exit. 
	//

	if ( ( saProcessId = fork1() ) == 0 )
	{
	    if ( execvp( cStoNode::prepTask(), argv ) == -1 )
	    {
		std::cout << "execlp() failed to start cdPrep, errno = " <<
			errno << std::endl;
		_exit( 999 );
	    }
	}
	if ( saProcessId == -1 )
	{
	    status.E_FORK_FAILED( status, cStoNode::prepTask() );
	    status.sysErrno();
	    status.display();
	    break;
	}


	if ( rollback() )
	{
	    if ( pStageDir != NULL )
	    {
		pStageDir->clear();
	    }
	}
	else
	{
	    //
	    // Process spawned OK. Update EPICS. Update am library struct. 
	    // Wait for process to finish.
	    //

	    pStageDir->setMediaRequestId( getMediaRequestId() );
	    pStageDir->setMediaUnitName( pUnit->getMediaUnitName() );
	    pStageDir->getPAmStage()->amsMediaRequestId = 
		    getMediaRequestId();
	    (void) sprintf( pStageDir->getPAmStage()->amsMediaUnitName, "%s",  
		    pUnit->getMediaUnitName() );
	    pStageDir->setInUse( true );
	}

	checkSystem( waitpid( saProcessId, &execStatus, 0 ), 
		status, break );
	saProcessId = -1;


	//
	// If process was stopped by cancel command, break loop here. 
	//

	if ( getStopped() )
	{
	    break;
	}


	//
	// Check return value from exec.
	//

	if ( !WIFEXITED( execStatus ) )
	{
	    status.E_EXEC_ABNORMAL_EXIT( status, cStoNode::prepTask() );
	    continue;
	}
	else if ( WEXITSTATUS( execStatus ) < 0 &&
    		 NEGEXITSTATUS( execStatus ) )
	{
	    status.E_EXEC_NONZERO_EXIT( status, cStoNode::prepTask(), 
		    WEXITSTATUS( execStatus ) - 255 );
	    continue;
	}


	//
	// Query the DB to find out what cdPrep did to the unit. 
	//

	dbProcess = cStoDbManager::acquire();
	amStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID_NAME, &amUnit, 
		dbProcess, getMediaRequestId(), pUnit->getMediaUnitName() );
	cStoDbManager::release();
	if ( amStatus == AM_NO_UNITS )
	{
	    status.E_PREP_LOST_UNIT( status, pUnit->getMediaUnitName() ); 
	    continue;
	}
	checkAm( amStatus, status, VOID );
	memcpy( pUnit->getPAmUnit(), &amUnit, sizeof( AM_UNIT ) );
	if ( rollback() )
	{
	    if ( amUnit.amuStatus != AMU_QUEUED )
	    {
		status.E_PREP_ROLLBACK( status, pUnit->getMediaUnitName() ); 
		continue;
	    }
	    

	    //
	    // Set prepared to true for the unit. 
	    //

	    pUnit->setPrepared( false );
	}
	else
	{
	    if ( amUnit.amuStatus != AMU_PREPARED )
	    {
		status.E_UNIT_NOT_PREPARED( status, pUnit->getMediaUnitName() ); 
		continue;
	    }
	    

	    //
	    // Set prepared to true for the unit. 
	    //

	    pUnit->setPrepared( true );
	}


	//
	// Get the number of preparable and writable units.
	//

	dbProcess = cStoDbManager::acquire();
	checkAm( amNumPreparable( dbProcess, 
		pMedia->getPAmMedia()->ammMediaRequestId,
		&dbIntValue ), status, break );
	pMedia->setPrepNum( dbIntValue );
	checkAm( amNumWriteable( dbProcess, 
		pMedia->getPAmMedia()->ammMediaRequestId,
		&dbIntValue ), status, break );
	pMedia->setWriteNum( dbIntValue );
	cStoDbManager::release();
    }


    return;
}
