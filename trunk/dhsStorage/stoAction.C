static char rcsid[] = "$Id: stoAction.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoAction.C
//
// PURPOSE:
// Contains methods supporting the cStoAction class, which provides
// action queue management. 
//
// CLASSES
// n/a
// 
// METHOD NAME(S)
// cStoAction::cStoAction	- Constructor
// cStoAction::~cStoAction	- Deconstructor
// cStoAction::disabled		- Determine if the action is disabled.
// cStoAction::flush		- Re-send all EPICs data to the EPICS database.
// cStoAction::getNonActiveAction - Fetch next action to be processed.
// cStoAciton::init		- Initialize the action list.
// cStoAction::isProcessable	- Check if action is currently processable.
// cStoAction::findAction 	- Find a specific action.
// cStoAction::stopAction	- Stop the processing of an action.
// cStoAction::str2action	- Convert string to action type enum.
// cStoAction::selectEpicsArrayRow - Pick row in Epics array for new action.
// cStoAction::getNextPosition	- Get next position for Epics array.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:44  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.9  2000/08/22 19:25:29  nhill
// Added class name to a reference to a member class.
//
// Revision 1.8  2000/01/06 17:59:06  nhill
// Changed to use the media reference counter to protect media requests
// when actions are active.
//
// Revision 1.7  1999/10/12 22:47:23  nhill
// Fixed some comments.
//
// Revision 1.6  1999/06/16 23:53:42  nhill
// Major re-organization.
//
// Revision 1.5  1998/07/31 20:59:07  cockayne
// Fixed bug in destructor, which occurred when pMedia->setNotifyState()
// was called after the pMedia object had been destroyed.
//
// Revision 1.4  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.3  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <csignal>


extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoActionCmd.H"
#include "stoAction.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoMedia.H"
#include "stoNode.H"


//
// Define the static data in the cStoAction class.
//

cMutex    	cStoAction::saQueueMutex;    	// Queue control mutex
cStoAction::tActionQueue 	
		cStoAction::saActionQueue;  	// Queue of Storage Actions

cStoEpicsRecord<bool, cStoAction, cStoStatus>
		cStoAction::saEpicsOverflow( STO_ERA_ACTION_OVERFLOW, 0, NULL, NULL );
						// true if EPICS is overflowed.
cStoEpicsRecord<int, cStoAction, cStoStatus>
		cStoAction::saNumActions( STO_ERA_NUM_ACTIONS, 0, NULL, NULL );
						// Number of actions. 
cStoEpicsRecord<int, cStoAction, cStoStatus>
		cStoAction::saArraySize( STO_ERA_ACTION_EPICS_ARRAY, STO_MAX_ACTION_EPICS_ARRAY, NULL, NULL );
						// Action array size.

int		cStoAction::saEpicsFreeList[STO_MAX_ACTION_EPICS_ARRAY];
						// The list of free/non-free 
						// EPICS action array rows.
int		cStoAction::saPositionCounter( 0 );


//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::cStoAction
//
// INVOCATION:
// cStoAction( aType, pMedia, rollback );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) aType	(STO_ACTION_TYPE) - Type of action to construct.
// (>) pMedia	(cStoMedia *)	- Pointer to action's media object.
// (>) rollback	(bool)		- True if the action is a rollback.
// 
// FUNCTION VALUE:
// (cStoAction) new action object.
// 
// PURPOSE:
// Constructor for the cStoAction object class. 
// 
// DESCRIPTION:
// Create action object, initializing internal data and flushing 
// appropriate SAD record values to the Status Server to update EPICS.
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

		cStoAction::cStoAction
(
    STO_ACTION_TYPE	
		aType,		// Action to add.
    cStoMedia	*pMedia,	// Media object to which to apply action. 
    bool	rollback	// True if the action is a rollback.
)
:   saEpicsArrayRow( selectEpicsRow() ),

    saOption( saEpicsArrayRow, STO_ERA_OPTION, "", this, NULL ),
    saActionType( saEpicsArrayRow, STO_ERA_ACTION, 
	    cStoActionType::initializer( aType ), this, NULL ),
    saMediaRequestId( saEpicsArrayRow, STO_ERA_REQUEST_ID, 
	    pMedia->getMediaRequestId(), this, NULL ),
    saActive( saEpicsArrayRow, STO_ERA_ACTIVE, 
	    false, this, NULL ),
    saInUse( saEpicsArrayRow, STO_ERA_IN_USE, 
	    true, this, NULL ),
    saPosition( saEpicsArrayRow, STO_ERA_POSITION, 
	    getNextPosition(), this, NULL ),
    saPMedia( pMedia ),
    saStopped( false )
{
    cStoStatus	status;


    pMedia->ref();

    saRollback = rollback;

    if ( rollback )
    {
	saOption = std::string( " rollback" ) + saOption.value();
    }


    //
    // Add to queue. 
    //
    
    saActionQueue.push_back( this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::~cStoAction
//
// INVOCATION:
// ~cStoAction();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor for the cStoAction class. 
//
// DESCRIPTION:
// Deconstructs the cStoAction object. Sets the appropriate EPICS records
// to indicate that the action has been deconstructed. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// It is assumed that the action queue is locked by the calling routine.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cStoAction::~cStoAction
(
    void
)
{
    iActionQueue
		iter;		// Iterator for the queue.
    bool	reAssigned;	// Has the EPICS record been re-assigned?
    cStoStatus	status;
    cStoAction	*pAction;	// Pointer to the action being tested.


    saInUse = false;
    saEpicsFreeList[saEpicsArrayRow] = false;


    for ( iter = saActionQueue.begin(); 
	    iter != saActionQueue.end(); 
	    iter++ )
    {
	if ( this == (*iter) )
	{
	    saActionQueue.erase( iter );
	    break;
	}
    }


    saPMedia->deref();


    //
    // Look for another action which hasn't been assigned to an EPICS record.
    //


    if ( saEpicsArrayRow != -1 )
    {
	reAssigned = false;
	for ( iter = saActionQueue.begin(); iter != saActionQueue.end(); 
		iter++ )
	{
	    pAction = (*iter);
	    if ( pAction->saEpicsArrayRow == -1 )
	    {
		if ( reAssigned )
		{
		    saEpicsOverflow = true;
		    break;
		}
		else
		{
		    pAction = (*iter);
		    pAction->saEpicsArrayRow = saEpicsArrayRow;
		    saEpicsOverflow = false;
		    pAction->flush();
		    reAssigned = true;
		}
	    }
	}
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::disabled
//
// INVOCATION:
// if ( action.disabled() )
// ...
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) True if the action is disabled.
//
// PURPOSE:
// Test to see if this action is currently disabled
//
// DESCRIPTION:
// Check each of the existing actions for this media request, and return
// true if one of them is excluded.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// It is assumed that the action queue is locked by the calling routine.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoAction::disabled
(
    void
)
{
    iActionQueue
		iter;		// Iterator for the queue. 
    cStoAction	*pAction;	// Pointer to the action being tested.


    //
    // Loop through each action in the aciton queue, checking to see
    // if the action can co-exist with this action.
    //

    for ( iter = saActionQueue.begin(); iter != saActionQueue.end(); iter++ )
    {
	pAction = (*iter);
	if ( pAction->getMediaRequestId() == this->getMediaRequestId() &&
		! coexist( pAction->getActionType().getAction(), 
		pAction->rollback() ) )
	{
	    return( false );
	}
    }

    return( true );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::flush
//
// INVOCATION:
// action.flush()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Re-send all action EPICS values to the EPICS database
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoAction::flush
(
    void
)
{
    saNumActions.setIndex( saEpicsArrayRow );
    saEpicsOverflow.setIndex( saEpicsArrayRow );
    saMediaRequestId.setIndex( saEpicsArrayRow );
    saActionType.setIndex( saEpicsArrayRow );
    saOption.setIndex( saEpicsArrayRow );
    saInUse.setIndex( saEpicsArrayRow );
    saActive.setIndex( saEpicsArrayRow );
    saPosition.setIndex( saEpicsArrayRow );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::getNonActiveAction
//
// INVOCATION:
// getNonActiveAction( action );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) action	(cStoAction **)	- Ptr to ptr to action object to return.
//
// FUNCTION VALUE:
// (bool) - true if a non-active action is found, false otherwise. 
//
// PURPOSE:
// If possible, fetch a non-active action from the action queue.
//
// DESCRIPTION:
// Iterate through the action queue, looking for a non-active action. If
// found, return true, and set the action pointer to point to the found
// action. If not, return false. 
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

cStoAction *	cStoAction::getNonActiveAction
(
    void
)
{
    iActionQueue
		iter;		// Iterator for the queue. 
    cStoAction 	*returnValue;	// Value to return from this routine.


    queueLock();
    returnValue = NULL;
    for ( iter = saActionQueue.begin(); iter != saActionQueue.end(); iter++ )
    {
	if ( !(*iter)->getActive() && !(*iter)->getStopped() )
	{
	    returnValue = (*iter);
	    returnValue->setActive( true );
	    break;
	}
    }
    queueUnlock();


    return( returnValue );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::init
//
// INVOCATION:
// cStoAction::init
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the cStoAction class.
//
// DESCRIPTION:
// Set every unused action to not in use.
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

void		cStoAction::init
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    int		num;		// Loop counter.
    char	name[40];	// Name of the in use record.


    checkStat( status, return );


    //
    // Loop through the list of EPICS array rows.
    //

    for ( num = 0; num < STO_MAX_ACTION_EPICS_ARRAY; num++ )
    {
	if ( saEpicsFreeList[num] == false )
	{
	    sprintf( name, STO_ERA_IN_USE, num );
	    cStaStatus::update( name, "0", status );
	    cStaStatus::flush( status );
	}
    }
    saEpicsOverflow = false;
    saArraySize = saArraySize.value();


    if ( ! status.ok() )
    {
	status.E_STA( status, name );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::isProcessable
//
// INVOCATION:
// action.isProcessable();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool)   True if the action can currently be processed, ie. if it makes
// 	    sense to currently process the action. False otherwise. 
//
// PURPOSE:
// Evaluate an action in terms of the current system state, and return a
// falg telling whether or not it makes sense to process the action. 
//
// DESCRIPTION:
// The action/media objects are evaluated to see if the action 
// should be processed.
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

bool		cStoAction::isProcessable
(
    void
)
{
    bool 	returnValue;	// Value to return from this routine.


    returnValue = false;
    

    switch( getActionType().getAction() )
    {
	case STO_AT_RETRIEVE: 
	    returnValue = ( saPMedia->getRetrieveSize() > 0 ? true : false );
	    break;

	case STO_AT_QUEUE: 
	    if ( rollback() )
	    {
		returnValue = ( saPMedia->getPrepNum() > 0 ? true : false );
	    }
	    else
	    {
		returnValue = ( saPMedia->getQueueSize() > 0 ? true : false );
	    }
	    break;

	case STO_AT_PREPARE: 
	    if ( rollback() )
	    {
		returnValue = ( saPMedia->getWriteNum() > 0 ? true : false );
	    }
	    else
	    {
		returnValue = ( saPMedia->getPrepNum() > 0 ? true : false );
	    }
	    break;

	case STO_AT_WRITE: 
	    if ( rollback() )
	    {
		returnValue = ( saPMedia->getCleanupNum() > 0 ? true : false );
	    }
	    else
	    {
		returnValue = ( saPMedia->getWriteNum() > 0 ? true : false );
	    }
	    break;

	case STO_AT_CLEANUP: 
	    returnValue = ( saPMedia->getCleanupNum() > 0 ? true : false );
	    break;

	case STO_AT_NOTIFY: 
	    returnValue = saPMedia->getNotify();
	    break;

	case STO_AT_NONE:
	default:
	    break;

    }


    return( returnValue );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::findAction
//
// INVOCATION:
// pAction = findAction( actionType, mediaId );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) aType	(STO_ACTION_TYPE) - Action type to match.
// (>) mediaId	(int)		- Media request identifier to match.
//
// FUNCTION VALUE:
// (pAction) The action if it is found, NULL otherwise.
//
// PURPOSE:
// Find a specific action in the action queue. 
//
// DESCRIPTION:
// The action queue is searched for an action object which matches the 
// passed-in parameters. If one is found, the action pointer is returned,
// otherwise NULL is returned.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// PRIOR REQUIREMENTS:
// It is assumed that the action queue is locked by the calling routine.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cStoAction	*cStoAction::findAction
(
    STO_ACTION_TYPE	
		aType,		// (in)  Action to match.
    int		mediaId		// (in)  MediaId to match.
)
{
    iActionQueue
    		iter;		// Iterator. 


    for ( iter = saActionQueue.begin(); iter != saActionQueue.end(); iter++ )
    {
	if ( (*iter)->getMediaRequestId() == mediaId &&
		(*iter)->getActionType().getAction() == aType )
	{
	    return( *iter );
	}
    }


    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::stopAction
//
// INVOCATION:
// stopAction();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
// 
// PURPOSE:
// Stop processing of an action. 
// 
// DESCRIPTION:
// Set the stop flag, and if the node is using exec() to run a process, 
// kill that process. 
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

void		cStoAction::stopAction
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cStoStatus	status;


    //
    // Set flag and broadcast so that currently-in-progress actions stop.
    //

    setStopped( true );
    if ( getActive() )
    {
	switch( getActionType().getAction() )
	{
	    case STO_AT_QUEUE:
	    case STO_AT_PREPARE:
	    case STO_AT_WRITE:
	    case STO_AT_CLEANUP:
		if ( saProcessId != -1 )
		{
		    checkSystem( kill( saProcessId, SIGINT ), 
			    status, VOID );
		}
		break;

	    case STO_AT_RETRIEVE:
	    case STO_AT_NOTIFY:
	    default:
		break;
	}
    }
    else
    {
	delete this;
    }


    status.S_ACTION_STOPPING( status, getActionType().getActionTypeString(), 
	    getMediaRequestId() );

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::str2action
//
// INVOCATION:
// str2Action( action );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) action	(char *) - String to convert into an action enum. 
//
// FUNCTION VALUE:
// (STO_ACTION_TYPE) Action type enum, which the action string maps to.
//
// PURPOSE:
// Convert the action string into an action type enum value.
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

STO_ACTION_TYPE	cStoAction::str2action
(
    char	*action		// String to convert to enumerated type.
)
{
    STO_ACTION_TYPE
		retValue;	// Method return value. 


    retValue = STO_AT_NONE;

    if ( streq( action, STO_AS_RETRIEVE ) )
    {
	retValue = STO_AT_RETRIEVE;
    }
    else if ( streq( action, STO_AS_QUEUE ) )
    {
	retValue = STO_AT_QUEUE;
    }
    else if ( streq( action, STO_AS_PREPARE ) )
    {
	retValue = STO_AT_PREPARE;
    }
    else if ( streq( action, STO_AS_WRITE) )
    {
	retValue = STO_AT_WRITE;
    }
    else if ( streq( action, STO_AS_CLEANUP) )
    {
	retValue = STO_AT_CLEANUP;
    }
    else if ( streq( action, STO_AS_NOTIFY ) )
    {
	retValue = STO_AT_NOTIFY;
    }


    return( retValue );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::selectEpicsArrayRow
//
// INVOCATION:
// selectEpicsArrayRow()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (int) Epics array row number to assign to an action object.
//
// PURPOSE:
// Assign an Epics array row number.
//
// DESCRIPTION:
// This routine assigns an Epics array number. If there are no rows
// left to assign (ie. they are all in use), the overflow flag is set,
// and a warning is issued. 
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

int		cStoAction::selectEpicsRow
(
    void
)
{
    int		num;		// Loop counter.
    cStoStatus	status;


    //
    // Loop through the list of EPICS array rows, try to find a free one.
    //

    saNumActions = saNumActions.value() + 1;
    for ( num = 0; num < STO_MAX_ACTION_EPICS_ARRAY; num++ )
    {
	if ( saEpicsFreeList[num] == false )
	{
	    saEpicsFreeList[num] = true;
	    return( num );
	}
    }


    //
    // If get to here, must have full EPICS array. Set health to WARNING.
    //

    status.E_ACTION_OVERFLOW( status );
    saEpicsOverflow = true;

    return( -1 );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoAction::getNextPosition
//
// INVOCATION:
// getNextPosition();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (int) next action position to hand out. 
//
// PURPOSE:
// Returns the next action position.
//
// DESCRIPTION:
// The action queue being a queue, there needs to be some 'positional'
// information available to allow it to be viewed as a 'queue'. The
// problem with this is that the EPICS array is just an array. Therefore,
// the array can be sorted by its position field to be viewed as a queue.
// This routine returns and increments the position counter to be put
// into a new action object's position field.
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

int		cStoAction::getNextPosition
(
    void
)
{
    saPositionCounter++;
    return( saPositionCounter );
}
