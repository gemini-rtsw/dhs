static char rcsid[] = "$Id: stoNotify.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoNotify.C
//
// PURPOSE:
// Contains the implementation of the cStoNotify class.
//
// METHOD NAME(S)
// cStoNotify::~cStoNotify - Destructor for the class.
// cStoNotify::coexist - Determine if this action can coexist with a specified
//			action on the same request.
// cStoNotify::execute - Execute the action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:09  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.2  2000/01/06 18:00:43  nhill
// Changed to not send notify messages when there is no notify client.
//
// Revision 1.1  1999/06/17 00:01:30  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include "globals.H"
#include "stoAction.H"
#include "stoMedia.H"
#include "stoUnit.H"
#include "stoConnection.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNotify::~cStoNotify
//
// INVOCATION:
// N.A.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Clean up when an cStoNotify action object is deleted
//
// DESCRIPTION:
// Set the notify state to IDLE, If this is a complete user request,
// delete the media object.
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

		cStoNotify::~cStoNotify
(
)
{
    cStoStatus	status;

    saPMedia->setNotifyState( STO_FST_IDLE, status );

    if ( saPMedia->getPAmMedia()->ammStatus == AMM_DONE &&
	    streq( saPMedia->getPAmMedia()->ammMediaRequestType, "USER" ) )
    {
	//
	// Nuke media object, clearing it from EPICS. 
	//

	saPMedia->deref();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoNotify::coexist
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
// Exclude other Notify actions.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoNotify::coexist
(
    STO_ACTION_TYPE
		actionType,	// (in)  The action type tested.
    bool			// (in)  Is the other action a rollback?
)
{
    if ( actionType == STO_AT_NOTIFY )
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
// cStoNotify::execute
//
// INVOCATION:
// notify();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processing for completion-notification.
//
// DESCRIPTION:
// Find the media object in the list of such objects. Set its notify
// state to IN-PROGRESS. Different behaviour for ARCHIVE and USER media
// request types. For ARHIVE, for each unit, compose a notification 
// command which contains a list of all files on that unit. For USER, 
// just send the media request ID. After the commands have been sent, 
// check the database to ensure we can reset the notify-required flag
// back to false. If so, do it. Set the notify state back to IDLE.
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

void		cStoNotify::execute
(
    void
)
{
    DHS_CMD_STATUS	
    		commandStatus;
    cDhsConnection	*pConnect;
    AM_UNIT	amUnit;
    AM_UNIT	*pAmUnit;
    DBPROCESS 	*dbProcess;
    int		amStatus;
    AM_FILE	amFile;
    AM_FILE	*pAmFile;
    char	*commandString;
    int		count;
    char	attributeName[128];
    cStoMedia	*pMedia;		// Ptr to media object. 
    cStoStatus	status;			// Program status. 
    std::list < AM_FILE >
		fileList;		// List of files. 
    std::list < AM_FILE >::iterator
		fileIter;		// File list iterator
    std::list < AM_UNIT >
		unitList;		// List of units. 
    std::list < AM_UNIT >::iterator
		unitIter;		// Unit list iterator
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    boolean	dbBooleanValue;
    int		i;
    cDhsClientCommand	
		*pNotifyCmd;		// Pointer to a notify command.



    //
    // Locate media request object in the list of media requests. 
    // Update EPICS to say that notifying is IN_PROGRESS
    //

    pMedia = cStoMedia::find( getMediaRequestId(), NULL );
    check( pMedia->setNotifyState( STO_FST_IN_PROGRESS, status ),
	    status, return );


    //
    // Different behaviour required for different media request types. 
    // 

    for ( i = 0; i < 1 && status.ok(); i++ )
    {
	if ( streq( pMedia->getPAmMedia()->ammMediaRequestType,
		AM_RT_ARCHIVE ) )
	{
	    //
	    // Get units from db which have AMU_CLEANED_UP status. Add to 
	    // unitList, not to the class's list. 
	    //

	    dbProcess = cStoDbManager::acquire();
	    for ( amStatus = amUnitGet( AM_UG_NOTIFIABLE, &amUnit, dbProcess,
		    pMedia->getPAmMedia() );
		    amStatus == AM_SUCCESS;
		    amStatus = amUnitGet( AM_UG_NEXT, &amUnit, dbProcess ) )
	    {
		unitList.push_back( amUnit );
	    }
	    cStoDbManager::release();
	    if ( amStatus != AM_NO_UNITS )
	    {
		checkAm( amStatus, status, break );
	    }

	    
	    if ( pMedia->getPAmMedia()->ammSourceId != NULL &&
		    strlen( pMedia->getPAmMedia()->ammSourceId ) != 0 )
	    {
		//
		// Connect up to source of media request. 
		//

		pConnect = NULL;
		pConnect = cStoConnection::find(
		    pMedia->getPAmMedia()->ammSourceId, NULL );
		if ( pConnect == NULL )
		{
		    status.E_UNABLE_TO_CONN( status,
			    pMedia->getPAmMedia()->ammSourceId );
		    break;
		}
		else
		{
		    checkDhs( pConnect->open( dhsStatus ), 
			    dhsStatus, status, break );
		}
	    }


	    //
	    // Process list of units.
	    //

	    for ( unitIter = unitList.begin(); 
		    status.ok() && unitIter != unitList.end(); 
		    unitIter++ )
	    {
		pAmUnit = &(*unitIter);


		//
		// Get list of files which are on this unit. 
		//

		dbProcess = cStoDbManager::acquire();
		for ( amStatus = amFileGet( AM_FG_MEDIA_UNIT, &amFile, 
			dbProcess, pAmUnit );
			amStatus == AM_SUCCESS;
			amStatus = amFileGet( AM_FG_NEXT, &amFile, dbProcess ) )
		{
		    fileList.push_back( amFile );
		}
		cStoDbManager::release();
		if ( amStatus != AM_NO_FILE )
		{
		    checkAm( amStatus, status, break );
		}

		if ( pMedia->getPAmMedia()->ammSourceId != NULL &&
			strlen( pMedia->getPAmMedia()->ammSourceId ) != 0 )
		{
		    //
		    // Compose notification command.
		    //

		    pNotifyCmd = new cDhsClientCommand( STO_CMD_ARCH_NOTIFY ); 

		    checkDhs( pNotifyCmd->add( STO_CA_SEGREGATION_DIR, 
			    DHS_DT_STRING, 
			    pMedia->getPAmMedia()->ammMediaRequestName, 
			    dhsStatus ), dhsStatus, status, break );


		    for ( count = 0, fileIter = fileList.begin(); 
			    fileIter != fileList.end();
			    fileIter++, count++ )
		    {
			if ( count > 50 )
			{
			    //
			    // Send command. Wait until it completes. 
			    // Check command status. 
			    //

			    checkDhs( pNotifyCmd->apply( *pConnect, dhsStatus ),
				    dhsStatus, status, break );
			    pNotifyCmd->wait( dhsStatus );
			    checkDhs( ( commandStatus = pNotifyCmd->status( 
				    commandString, dhsStatus ) ), 
				    dhsStatus, status, break );
			    if ( commandStatus != DHS_CS_DONE )
			    {
				status.E_NOTIFY_ARCH( status, commandString );
				break;
			    }
			    count = 0;
			    delete pNotifyCmd;
			    pNotifyCmd = new cDhsClientCommand( 
				    STO_CMD_ARCH_NOTIFY );
			    checkDhs( pNotifyCmd->add( STO_CA_SEGREGATION_DIR, 
				    DHS_DT_STRING, 
				    pMedia->getPAmMedia()->ammMediaRequestName, 
				    dhsStatus ), dhsStatus, status, break );
			}
			pAmFile = &(*fileIter);
			
			//
			// Add file Id to notification command. 
			//

			(void) sprintf( attributeName, STO_CA_NUMBERED_FILE_ID, 
				count );
			checkDhs( pNotifyCmd->add( attributeName, DHS_DT_STRING, 
				pAmFile->amfFileId, dhsStatus), 
				dhsStatus, status, break );
		    }


		    if ( count > 0 )
		    {
			//
			// Send command. Wait until it completes. Check command 
			// status. 
			//

			checkDhs( pNotifyCmd->apply( *pConnect, dhsStatus ),
				dhsStatus, status, break );
			pNotifyCmd->wait( dhsStatus );
			checkDhs( ( commandStatus = pNotifyCmd->status( 
				commandString, dhsStatus ) ), dhsStatus, 
				status, break );
			if ( commandStatus != DHS_CS_DONE )
			{
			    status.E_NOTIFY_ARCH( status, commandString );
			    break;
			}
		    }
		    delete pNotifyCmd;
		}


		//
		// Update unit to done. 
		//
		
		pAmUnit->amuStatus = AMU_DONE;
		dbProcess = cStoDbManager::acquire();
		amStatus = amUnitPut(  pAmUnit, dbProcess );
		cStoDbManager::release();
		checkAm( amStatus, status, break );
	    }


	    checkDhs( pConnect->close( dhsStatus ), dhsStatus, status, break );


	    //
	    // Lock dbprocess, (lock list?) see if there's more CLEANED-UP 
	    // units. If not, update EPICS to false. 
	    //

	    dbProcess = cStoDbManager::acquire();
	    amStatus = amNotifiable( dbProcess, 
		    pMedia->getPAmMedia()->ammMediaRequestId,
		    pMedia->getPAmMedia()->ammMediaRequestType, 
		    &dbBooleanValue );
	    cStoDbManager::release();
	    checkAm( amStatus, status, break );
	    if ( dbBooleanValue == FALSE )
	    {
		pMedia->setNotify( false );
	    }
	}
	else if ( streq( pMedia->getPAmMedia()->ammMediaRequestType,
		AM_RT_USER ) )
	{
	    if ( pMedia->getPAmMedia()->ammSourceId != NULL &&
		    strlen( pMedia->getPAmMedia()->ammSourceId ) != 0 )
	    {
		//
		// Connect up to source of media request. 
		//

		pConnect = NULL;
		pConnect = cStoConnection::find(
		    pMedia->getPAmMedia()->ammSourceId,  NULL );
		if ( pConnect == NULL )
		{
		    status.E_UNABLE_TO_CONN( status,
			    pMedia->getPAmMedia()->ammSourceId );
		    break;
		}
		else
		{
		    checkDhs( pConnect->open( dhsStatus ), dhsStatus, status,
			    break );
		}


		//
		// Compose notification command.
		//

		cDhsClientCommand	notifyCmd( STO_CMD_USER_NOTIFY ); 

		checkDhs( notifyCmd.add( STO_CA_MEDIA_REQUEST_ID, DHS_DT_INT32, 
			pMedia->getMediaRequestId(), dhsStatus ), 
			dhsStatus, status, break );


		//
		// Send command. Wait until it completes. Check command status. 
		//

		checkDhs( notifyCmd.apply( *pConnect, dhsStatus ),
			dhsStatus, status, break );
		notifyCmd.wait( dhsStatus );
		checkDhs( ( commandStatus = notifyCmd.status( commandString, 
			dhsStatus ) ), dhsStatus, status, break );

		checkDhs( pConnect->close( dhsStatus ), dhsStatus, status, 
			break );
		if ( commandStatus != DHS_CS_DONE )
		{
		    status.E_NOTIFY_USER( status, commandString );
		    break;
		}
	    }


	    //
	    // Set units' db status for media request to AMU_DONE all at once.
	    // Set media status to AMM_DONE in db.
	    //

	    dbProcess = cStoDbManager::acquire();
	    amStatus = amUnitUpdate( AM_UU_COMPLETE_USER,
		    dbProcess, pMedia->getPAmMedia()->ammMediaRequestId ); 
	    if ( amStatus != AM_SUCCESS )
	    {
		checkAm( amStatus, status, break );
	    }
	    else
	    {
		pMedia->getPAmMedia()->ammStatus = AMM_DONE;
		checkAm( amPut( pMedia->getPAmMedia(), dbProcess ), 
			status, break );
	    }
	    cStoDbManager::release();


	    //
	    // Update EPICS. 
	    //

	    pMedia->setNotify( false );
	}
	else
	{
	    status.E_MEDIA_REQUEST_TYPE( status, 
		    pMedia->getPAmMedia()->ammMediaRequestType,
		    pMedia->getMediaRequestId() );
	}
    }

    cStoUnit::init( status );


    return;
}
