static char rcsid[] = "$Id: stoCleanup.C,v 1.1.1.1 2002-11-24 20:31:48 brighton Exp $";
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
// dhsStorage/src/stoCleanup.C
//
// PURPOSE:
// Contains the implementation of the cStoCleanup class.
//
// METHOD NAME(S)
// cStoCleanup::coexist	- Test to see if this action can coexist with another.
// cStoCleanup::execute - Execute an cleanup or cleanup rollback action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.3  2001/03/12 22:47:20  nhill
// There was a missing unlock when there were no units to cleanup,
// which caused the media unit list to be perpetually locked.
//
// Revision 1.2  1999/10/12 22:49:39  nhill
// Changed to allow the resulting state of executing cleanup to be done,
// instead of just cleaned up.
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
#include "stoUnit.H"
#include "stoStage.H"
#include "stoNode.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoCleanup::coexist
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
// Exclude other Cleanup actions.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoCleanup::coexist
(
    STO_ACTION_TYPE
		actionType,	// (in)  The action type tested.
    bool			// (in)  Is the other action a rollback?
)
{
    if ( actionType == STO_AT_CLEANUP )
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
// cStoCleanup::execute
//
// INVOCATION:
// cleanup();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processing for cleanup or cleanup rollback.
// 
// DESCRIPTION:
// Find the media object in the list of such objects. Set its cleanup
// state to IN-PROGRESS. Enter a loop which will clean-up staging directories.
// The loop will exit after the first one is cleaned-up, or if the 
// optional number-to-clean is given, when that number have been cleaned
// up. In the loop, fetch the next cleanup-able unit from the list,
// find its stage object, and clean out the staging directory, removing
// all files. Update the object, indicating it is now free to be used 
// by another prepare process. Build the list of files on the unit, and
// remove them from the retrieval staging area, including directories
// if possible. Remove them, updating the database for each. Update the
// database to show the unti as cleaned-up. Delete the unit object, and
// update the media statistics to reflect this. Go on to the unit. When 
// done, update the media object's cleanup state to IDLE.
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

void		cStoCleanup::execute
(
    void
)
{
    AM_UNIT	amUnit;			// am library media unit struct;
    int		execStatus;		// Return status from the thread. 
    cStoMedia	*pMedia;		// Ptr to media object.
    cStoUnit	*pUnit;			// Ptr to media unit object.
    cStoStatus	status;			// Program status.
    int		amStatus;		// am library routine return status.
    DBPROCESS 	*dbProcess;		// Database process. 
    boolean	dbBooleanValue;		// Return boolean value from the db. 
    int		dbIntValue;		// Return int value from the db.
    map < string, char *, less< string > >
		dirList;		// List of files.
    map < string, char *, less< string > >::iterator
		dirListIter;		// File list iterator
    int		num;
    char	*argv[7];
    char	arg2[20];		// Argument number 2.
    int		i;
    cStoStage	*pStageDir;		// The staging directory for the unit.
    

    
    //
    // Locate media request object in list of media requests. 
    // Update EPICS to say cleaning up is IN_PROGRESS
    //

    pMedia = cStoMedia::find( getMediaRequestId(), NULL );
    check( pMedia->setCleanupState( STO_FST_IN_PROGRESS, status ),
	    status, return );


    //
    // Either do the number requested or do only one. 
    //

    for ( num = 0; num < scNumCleanup || num < 1; num++ )
    {
	cStoUnit::listRLock();
	if ( ( pUnit = cStoUnit::findFirstUnit( getMediaRequestId(),
		AMU_WRITTEN ) ) == NULL )
	{
	    //
	    // No Units to cleanup. Quit.
	    //

	    status.S_NO_CLEANUPABLE( status, getMediaRequestId() );
	    cStoUnit::listUnlock();
	    break;
	}
	pUnit->ref();
	cStoUnit::listUnlock();


	//
	// Set up the arguments to the mediaCleanup program.
	//

	i = 0;
	argv[i++] = cStoNode::cleanupTask();
	argv[i++] = "-i";
	(void) sprintf( arg2, "%d", getMediaRequestId() );
	argv[i++] = arg2;
	argv[i++] = "-n";
	argv[i++] = pUnit->getMediaUnitName();
	argv[i++] = NULL;


	//
	// Spawn process to run cleanup. Save thread ID in case of exit. 
	//

	if ( ( saProcessId = fork1() ) == 0 )
	{
	    if ( execvp( cStoNode::cleanupTask(), argv ) == -1 )
	    {
		cout << "execlp() failed to start " << 
			cStoNode::cleanupTask() << ", errno = " << errno << 
			endl;
		_exit( 999 );
	    }
	}
	if ( saProcessId == -1 )
	{
	    status.E_FORK_FAILED( status, cStoNode::cleanupTask() );
	    status.sysErrno();
	    status.display();
	}


	//
	// Wait for mediaCleanup to exit.
	//

	checkSystem( waitpid( saProcessId, &execStatus, 0 ), status, 
		pUnit->deref(); break );
	saProcessId = -1;


	if ( getStopped() )
	{
	    pUnit->deref();
	    break;
	}


	//
	// Check the return status from mediaCleanup
	//

	if ( WIFEXITED( execStatus ) == 0 )
	{
	    status.E_EXEC_ABNORMAL_EXIT( status, cStoNode::cleanupTask() );
	    pUnit->deref();
	    break;
	}
	else if ( WEXITSTATUS( execStatus ) != 0 )
	{
	    if ( NEGEXITSTATUS( execStatus ) )
	    {
		status.E_EXEC_NONZERO_EXIT( status, cStoNode::cleanupTask(), 
			WEXITSTATUS( execStatus ) - 255 );
	    }
	    else
	    {
		status.E_EXEC_NONZERO_EXIT( status, cStoNode::cleanupTask(), 
			WEXITSTATUS( execStatus ) );
	    }
	}
	    

	//
	// Query the DB to find out what cStoNode::cleanupTask() did. 
	// Need to get the media unit in pUnit. 
	//

	pMedia->lock();
	dbProcess = cStoDbManager::acquire();
	amStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID_NAME, &amUnit, dbProcess,
		getMediaRequestId(), pUnit->getMediaUnitName() );
	cStoDbManager::release();
	if ( amStatus == AM_NO_UNITS )
	{
	    pMedia->unlock();
	    pUnit->deref();
	    status.E_CLEANUP_LOST_UNIT( status, pUnit->getMediaUnitName() );
	    break;
	}
	checkAm( amStatus, status, VOID );

	memcpy( pUnit->getPAmUnit(), &amUnit, sizeof( AM_UNIT ) );

	if ( amUnit.amuStatus != AMU_CLEANED_UP && amUnit.amuStatus != AMU_DONE )
	{
	    pMedia->unlock();
	    pUnit->deref();
	    status.E_UNIT_NOT_CLEANED_UP( status, pUnit->getMediaUnitName() );
	    break;
	}
	

	//
	// Update EPICS - delete units from units list and 
	// also update number cleanupable for the media request.
	//

	pStageDir = cStoStage::find( pUnit->getMediaRequestId(), 
		pUnit->getMediaUnitName() );
	if ( pStageDir != NULL )
	{
	    pStageDir->clear();
	}

	dbProcess = cStoDbManager::acquire();
	checkAm( amNumCleanupable( dbProcess, pMedia->getMediaRequestId(), 
		&dbIntValue ), status, VOID );
	cStoDbManager::release();
	
	pMedia->setCleanupNum( dbIntValue );
	pMedia->unlock();


	//
	// For archive media request types, notification is now possible. If
	// user media request, check db to see if notification is possible.
	//

	if ( streq( pMedia->getPAmMedia()->ammMediaRequestType, 
		AM_RT_ARCHIVE ) )
	{
	    if ( amUnit.amuStatus == AMU_CLEANED_UP )
	    {
		pMedia->setNotify( true );
	    }
	}
	else if ( streq( pMedia->getPAmMedia()->ammMediaRequestType, 
		AM_RT_USER ) )
	{
	    dbProcess = cStoDbManager::acquire();
	    checkAm( amNotifiable( dbProcess, 
		    pMedia->getPAmMedia()->ammMediaRequestId,
		    pMedia->getPAmMedia()->ammMediaRequestType, 
		    &dbBooleanValue ), status, pUnit->deref(); break );
	    pMedia->setNotify( ( ( dbBooleanValue == TRUE ) ? true : false ) );
	    cStoDbManager::release();
	}
	pUnit->setCleanedUp( true );
	pUnit->deref();
    }


    return;
}
