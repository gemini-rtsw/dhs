static char rcsid[] = "$Id: stoWrite.C,v 1.1.1.1 2002-11-24 20:32:31 brighton Exp $";
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
// dhsStorage/src/stoWrite.C
//
// PURPOSE:
// Whatever
//
// METHOD NAME(S)
// cStoWrite::cStoWrite - Constructor for the class.
// cStoWrite::coexist - Determine if this action can coexist with the 
// 			specified action on the same media request.
// cStoWrite::execute - Execute this action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1  1999/06/17 00:01:41  nhill
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
#include "stoMedia.H"
#include "stoUnit.H"
#include "stoNode.H"
#include "stoDevice.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoWrite::cStoWrite
//
// INVOCATION:
// N.A. Called automatically.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pMedia (cStoMedia *) The media being written.
// (>) force (bool) Force writing to a busy logical device?.
// (>) numUnits (int) The number of units to write.
// (>) device (char *) The device to use.
// (>) rollback (bool) Is this a rollback operation.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize a cStoWrite object, selecting an appropriate logical device.
//
// DESCRIPTION:
// Trivia.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cStoWrite::cStoWrite
(
    cStoMedia	*pMedia,	// (in)  The media being written.
    bool	force,		// (in)  Force writing to a busy logical device?
    int		numUnits,	// (in)  The number of units to write.
    char	*device,	// (in)  The device to use.
    bool	rollback	// (in)  Is this a rollback action?
): cStoAction( STO_AT_WRITE, pMedia, rollback )
{
    char	buffer[30];	// Used to create an opton description string.
    cStoStatus	status;		// Function return status.
    cStoStatus	status2;
    cStoDevice	*pDevice;	// The device object to use.


    swNumUnits = numUnits;
    swForce = force;


    //
    // Set the action state.
    //

    if ( rollback )
    {
	pMedia->setWriteState( STO_FST_ROLLBACK_REQUESTED, status );
    }
    else
    {
	pMedia->setWriteState( STO_FST_REQUESTED, status );
    }


    if ( ! rollback )
    {
	if ( device == NULL )
	{
	    //
	    // Get an appropriate device for the operation.
	    //

	    pDevice = cStoDevice::deviceSelect( pMedia->type(), status );
	    if ( pDevice != NULL )
	    {
		swDevice = strsav( pDevice->logicalName() );
	    }
	}
	else
	{
	    //
	    // If a device was given, find the device by name.
	    //

	    pDevice = cStoDevice::find( device, pMedia->type(), status );
	    swDevice = strsav( device );
	    sprintf( buffer, " device: %s", device );
	    saOption = saOption.value() + buffer;
	}


	//
	// If a device was found, book the device.
	//

	if ( pDevice != NULL )
	{
	    pDevice->book( swForce && device != NULL, status );
	    cStoDevice::inUseUpdate( status );
	}
    }
    else
    {
	swDevice = NULL;
    }


    if ( ! status.ok() )
    {
	if ( swDevice != NULL )
	{
	    gen_free( swDevice );
	}
	pMedia->setWriteState( STO_FST_IDLE, status2 );
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoWrite::coexist
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
// Exclude other write actions.
// If this is a forward aciton, exclude prepare rollback actions.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoWrite::coexist
(
    STO_ACTION_TYPE
		actionType,	// (in)  The action type tested.
    bool	isRollback	// (in)  Is the other action a rollback?
)
{
    if ( actionType == STO_AT_WRITE ||
	    ( ! rollback() && ( actionType == STO_AT_PREPARE && isRollback ) ) )
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
// cStoWrite::execute
//
// INVOCATION:
// write();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processing for writing. 
//
// DESCRIPTION:
// Find the media object in the list of such objects. Set its write
// state to IN-PROGRESS. Locate the next writable unit in the unit 
// list. Spawn a process using fork() and exec() to write the unit
// using the cdWrite executable. Query the database to figure out
// cdWrite's results. Update media and unit objects to reflect new
// state of the unit. On the way out, update the write state to IDLE
// for the media object. 
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

void		cStoWrite::execute
(
    void
)
{
    int		execStatus;		// Return status from the thread. 
    char	*argv[13];
    char	arg2[20];		// Argument number 2.
    AM_UNIT	amUnit;			// am library media unit struct;
    cStoMedia	*pMedia;		// Ptr to media object.
    cStoUnit	*pUnit;			// Ptr to media unit object.
    cStoStatus	status;			// Program status.
    int		amStatus;		// am library routine return status.
    DBPROCESS 	*dbProcess;		// Database process. 
    int		dbIntValue;		// Value from database.
    int		i;			// loop counter.
    int		j;			// argument counter.
    cStoDevice	*pStoDevice;		// Pointer to the device object.


    //
    // Locate media request object in the list of media requests. 
    // Update EPICS to say that writing is IN_PROGRESS
    //

    pMedia = cStoMedia::find( getMediaRequestId(), NULL );
    check( pMedia->setWriteState( STO_FST_IN_PROGRESS, status ),
	    status, return );


    //
    // Write each of the units.
    //

    for ( i = 0; i < swNumUnits; i++ )
    {
	//
	// Locate the next writable unit in the list. Save name. 
	//

	if ( rollback() )
	{
	    if ( ( pUnit = cStoUnit::findLastUnit( 
		    getMediaRequestId(), AMU_WRITTEN ) ) == NULL )
	    {
		//
		// No Units to write. Quit.
		//

		status.S_NO_WRITEABLE( status, getMediaRequestId() );
		break;
	    }
	}
	else
	{
	    if ( ( pUnit = cStoUnit::findFirstUnit( 
		    getMediaRequestId(), AMU_PREPARED ) ) == NULL )
	    {
		//
		// No Units to write. Quit.
		//

		status.S_NO_WRITEABLE( status, getMediaRequestId() );
		break;
	    }


	    pStoDevice = cStoDevice::find( swDevice, pMedia->type(), status );
	    if ( pStoDevice != NULL )
	    {
		pStoDevice->setReqUnit( pUnit->getMediaRequestId(), 
			pUnit->getMediaUnitName() );
	    }
	}


	//
	// Initialize some strings. 
	//

	j = 0;
	argv[j++] = cStoNode::writeTask();
	argv[j++] = "-i";
	(void) sprintf( arg2, "%d", getMediaRequestId() );
	argv[j++] = arg2;
	argv[j++] = "-n";
	argv[j++] = pUnit->getMediaUnitName();
	if ( swDevice != NULL )
	{
	    argv[j++] = "-l";
	    argv[j++] = swDevice;
	}
	argv[j++] = "-force";
	if ( cStoNode::simulate() )
	{
	    argv[j++] = "-simulate";
	}
	if ( rollback() )
	{
	    argv[j++] = "-rollback";
	}
	argv[j++] = NULL;

	
	//
	// Spawn process to run cdWrite. Save thread ID in case of exit. 
	//

	if ( ( saProcessId = fork1() ) == 0 )
	{
	    if ( execvp( cStoNode::writeTask(), argv ) == -1 )
	    {
		cout << "execlp() failed to start cdWrite, errno = " <<
			errno << endl;
		_exit( 999 );
	    }
	}
	if ( saProcessId == -1 )
	{
	    status.E_FORK_FAILED( status, cStoNode::writeTask() );
	    status.sysErrno();
	    status.display();
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

	if ( WIFEXITED( execStatus ) == 0 )
	{
	    status.E_EXEC_ABNORMAL_EXIT( status, cStoNode::writeTask() );
	    break;
	}
	else if ( WEXITSTATUS( execStatus ) < 0  &&
		 NEGEXITSTATUS( execStatus ) )
	{
	    status.E_EXEC_NONZERO_EXIT( status, cStoNode::writeTask(), 
		WEXITSTATUS( execStatus ) - 255 );
	    break;
	}


	//
	// Query the DB to find out what cdWrite did. Need to get the media
	// unit in pUnit. 
	//

	dbProcess = cStoDbManager::acquire();
	amStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID_NAME, &amUnit, dbProcess,
		getMediaRequestId(), pUnit->getMediaUnitName() );
	cStoDbManager::release();
	if ( amStatus == AM_NO_UNITS )
	{
	    status.E_WRITE_LOST_UNIT( status, pUnit->getMediaUnitName() );
	    break;
	}
	checkAm( amStatus, status, VOID );

	memcpy( pUnit->getPAmUnit(), &amUnit, sizeof( AM_UNIT ) );

	pUnit->setNumWritten( amUnit.amuCopiesWritten );
	if ( rollback() )
	{
	    if ( amUnit.amuStatus != AMU_PREPARED )
	    {
		status.E_WRITE_ROLLBACK( status, pUnit->getMediaUnitName() );
		break;
	    }
	}
	else
	{
	    if ( amUnit.amuStatus != AMU_WRITTEN )
	    {
		status.E_UNIT_NOT_WRITTEN( status, pUnit->getMediaUnitName() );
		break;
	    }
	}


	//
	// Get the number of writeable and cleanupable units.
	//

	dbProcess = cStoDbManager::acquire();
	checkAm( amNumWriteable( dbProcess, 
		pMedia->getPAmMedia()->ammMediaRequestId,
		&dbIntValue ), status, break );
	pMedia->setWriteNum( dbIntValue );
	checkAm( amNumCleanupable( dbProcess, 
		pMedia->getPAmMedia()->ammMediaRequestId,
		&dbIntValue ), status, break );
	pMedia->setCleanupNum( dbIntValue );
	cStoDbManager::release();
    }

    if ( swDevice != NULL )
    {
	dbProcess = cStoDbManager::acquire();
	checkAm( amDeviceClear( AM_DC_LOGICAL, dbProcess, swDevice ), 
		status, VOID );
	cStoDbManager::release();
	cStoDevice::inUseUpdate( status );
    }


    return;
}
