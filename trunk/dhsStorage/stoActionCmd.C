static char rcsid[] = "$Id: stoActionCmd.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoActionCmd.C
//
// PURPOSE:
// Contains methods to handle action commands. 
//
// CLASSES
// N/A
// 
// METHOD NAME(S)
// cStoActionCmd::execActionRequest - Handle action requests.
// cStoActionCmd::execActionCancel - Handle action cancel commands.
// cStoActionCmd::execRollbackRequest - Handle rollback requests.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:46  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.11  2001/03/27 17:51:14  jaeger
// Changed behaviour of cStoActionCmd::execActionRequest().  It would
// report an error if an optional parameters was the empty string, an
// optional parameter that was not needed given the action type.  This
// method was changed to reflect what is stated in ICD3.2, doesn't fail
// for optional parameters that are not required for a given action type
// if they are the empty string.
//
// Revision 1.10  2001/03/12 20:34:20  nhill
// Added type checking on the command input parameters.
//
// Revision 1.9  1999/06/16 23:54:21  nhill
// Major re-organization.
//
// Revision 1.8  1999/02/24 23:49:30  jaeger
// Ensured pMedia pointer are NULL before calling STL find function.
//
// Revision 1.7  1998/06/24 15:59:50  cockayne
// Modified command AV list parsing to be more robust.
//
// Revision 1.6  1998/05/25 19:09:05  cockayne
// Mod'ed the way an option value is converted to an int. Now it is a string
// which is a float. Also removed a deadlock condition.
//
// Revision 1.5  1998/05/15 17:04:20  cockayne
// Added Memory error checking, better formatting of returned command
// error response string.
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
#include <unistd.h>
#include <cmath>

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
#include "stoDhs.H"
#include "stoAction.H"
#include "stoFiveState.H"
#include "stoDbm.H"
#include "stoMedia.H"
#include "stoNode.H"

#include "stoActionCmd.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoActionCmd::execActionRequest
//
// INVOCATION:
// execActionRequest();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts processing an action request command
//
// DESCRIPTION:
// Log receipt of command, parse out command attributes, add new action
// object to the action queue.
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

void		cStoActionCmd::execActionRequest
(
    void
)
{
    cStoAction	*pAction;		// The action being created.
    char	actionString[16];	// action string.
    STO_ACTION_TYPE
    		actionType;		// action type.
    cDhsAttrib	attribute;		// attribute object.
    char	*device;		// Device for writing.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status. 
    char        *id;			// Identity of client.
    char        *ip;			// Ip address of client.
    void	*pData;			// Pointer to attribute data.
    bool	force;			// Force queuing?
    int		mediaId;		// media id.
    cStoMedia	*pMedia;		// Ptr to media object.
    int		nDims;			// Number of dimensions for an
    					// attribute.
    int		number;			// Number fo prepare, or write.
    char	responseString[256];	// String to format response into.
    cStoStatus	status;			// Status. 
    int		size;			// Size to retrieve.
    DHS_DATA_TYPE			// Type of data in an attribute.
		type;

    char* dummy; // XXX allan: needed to fill ref param in attribute.info call



    //
    // Respond to let client continue. Log receipt of command. 
    //
    
    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    // 
    // What should we really be doing with simulation???
    //

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    } 


    //
    // Find required attributes. Get their value.
    //

    status.displayStop();
    attribute = find( STO_CA_ACTION, dhsStatus );
    attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type == DHS_DT_STRING && nDims == 0 )
	{
	    strncpy( actionString, (char *) pData, sizeof( actionString ) );
	    actionString[ sizeof( actionString ) - 1 ] = '\0';
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_ACTION );
	}
    }

    attribute = find( STO_CA_MEDIA_REQUEST_ID, dhsStatus );
    attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type == DHS_DT_STRING && nDims == 0 && 
		strne( (char *) pData, "" ) )
	{
	    mediaId = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_MEDIA_REQUEST_ID );
	}
    }

    if ( dhsStatus != DHS_S_SUCCESS )
    {
	//
	// Invalid command format. 
	//

	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_FORMAT( status, name() );
    }


    if ( ! status.ok() )
    {
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Convert action string to action type. 
    //

    if ( ( actionType = cStoAction::str2action( actionString ) ) == 
	    STO_AT_NONE )
    {
	status.E_ACTION_INVALID( status, actionString );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Find optional attributes if they exist.
    //

    attribute = find( STO_CA_SIZE, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB || actionType != STO_AT_RETRIEVE )
    {
	size = 0;
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
		dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS && type == DHS_DT_STRING && 
		nDims == 0 && strne( (char *) pData, "" ) )
	{
	    size = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_SIZE );
	}
    }


    attribute = find( STO_CA_FORCE, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB || actionType != STO_AT_QUEUE )
    {
	force = false;
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
		dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS && type == DHS_DT_STRING &&
		nDims == 0 && strne( (char *) pData, "" ) )
	{
	    force = atoi( (char *) pData ) == 0 ? false: true;
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_FORCE );
	}
    }


    attribute = find( STO_CA_NUMBER, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB || actionType == STO_AT_RETRIEVE
	|| actionType == STO_AT_NOTIFY )
    {
	number = 0;
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
		dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS && type == DHS_DT_STRING && 
		nDims == 0 && strne( (char *) pData, "" ) )
	{
	    number = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_NUMBER );
	}
    }


    attribute = find( STO_CA_DEVICE_NAME, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB ||  actionType != STO_AT_WRITE )
    {
	device = NULL;
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
		dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS && type == DHS_DT_STRING && 
		nDims == 0 && strne( (char *) pData, "" ) )
	{
	    device = strsav( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_FORCE );
	}
    }


    //
    // Check for errors.
    //

    if ( dhsStatus != DHS_S_SUCCESS || ! status.ok() )
    {
	dhsStatus = DHS_S_SUCCESS;
	status.E_ACTION_INVALID( status, actionString );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Check if media ID is valid.
    //

    pMedia = NULL;
    pMedia = cStoMedia::find( mediaId, NULL );
    if ( pMedia == NULL )
    {
	(void) sprintf( responseString, STO_RSP_MEDIAID_INVALID, mediaId );
	response( DHS_CS_ERROR, responseString, dhsStatus );
	return;
    }


    //
    // Lock action queue. 
    //

    cStoAction::queueLock();


    //
    // If state of action is not IDLE, refuse command.
    //

    if ( pMedia->getActionState( actionType ) != STO_FST_IDLE )
    {
	cStoAction::queueUnlock();
	(void) sprintf( responseString, STO_RSP_ACTION_ALREADY, 
		actionString, mediaId );
	response( DHS_CS_ERROR, responseString, dhsStatus );
	return;
    }


    //
    // Create action object, adding it to queue. 
    //

    pAction = NULL;
    switch ( actionType )
    {
	case STO_AT_RETRIEVE:
	    checkNull( pAction = new  cStoRetrieve( pMedia, size ), 
		    status, VOID );
	    break;

	case STO_AT_QUEUE:
	    checkNull( pAction = new  cStoQueue( pMedia, force, number ), 
		    status, VOID );
	    break;

	case STO_AT_PREPARE:
	    checkNull( pAction = new cStoPrepare( pMedia, number ), 
		    status, VOID );
	    break;

	case STO_AT_WRITE:
	    try
	    {
		checkNull( pAction = new cStoWrite( pMedia, force, number, 
			device ), status, VOID );
	    }
	    catch( const cStoStatus & s )
	    {
		status = s;
	    }
	    break;

	case STO_AT_CLEANUP:
	    checkNull( pAction = new  cStoCleanup( pMedia, number ), 
		    status, VOID );
	    break;
	case STO_AT_NOTIFY:
	    checkNull( pAction = new  cStoNotify( pMedia ), status, VOID );
	    break;
    }


    //
    // Check to see if the action is enabled.
    //

    if ( pAction != NULL && pAction->disabled() )
    {
	delete pAction;
	status.S_ACTION_DISABLED( status, actionString );
    }


    //
    // Unlock the action queue.
    //

    cStoAction::queueUnlock();


    //
    // Broadcast to the cStoNode class to allow a processing node to 
    // get the new action.
    //

    cStoNode::broadcast();

    if ( device != NULL )
    {
	free( device );
	device = NULL;
    }


    //
    // Respond that command is DONE. 
    //

    dhsStatus = DHS_S_SUCCESS;
    if ( status.ok() )
    {
	response( DHS_CS_DONE, dhsStatus );
    }
    else
    {
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
    }
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoActionCmd::execActionCancel
//
// INVOCATION:
// execCancelAction();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts processing an action cancel command
//
// DESCRIPTION:
// Log receipt of command, parse out command attributes, and either
// remove the action from the action queue or stop its processing
// node.
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

void		cStoActionCmd::execActionCancel
(
    void
)
{
    char	actionString[20];	// action string.
    STO_ACTION_TYPE
    		actionType;		// action type.
    cDhsAttrib	attribute;		// attribute object.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status. 
    char        *id;			// Identity of client.
    char        *ip;			// Ip address of client.
    int		mediaId;		// media id.
    int		nDims;			// Number of dimensions in an attribute.
    cStoAction	*pAction;		// Ptr to media object.
    void	*pData;			// Pointer to attribute data.
    cStoMedia	*pMedia;		// Ptr to media object.
    char	responseString[256];	// String to format response into.
    cStoStatus	status;			// Status. 
    DHS_DATA_TYPE			// Type of data in an attribute.
    		type;

    char* dummy; // XXX allan: needed to fill ref param in attribute.info call

    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    // 
    // What should we really be doing with simulation???
    //

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    } 


    //
    // Find required attributes. Get their values.
    //

    status.displayStop();
    attribute = find( STO_CA_ACTION, dhsStatus );
    attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type == DHS_DT_STRING && nDims == 0 )
	{
	    strncpy( actionString, (char *) pData, sizeof( actionString ) );
	    actionString[ sizeof( actionString ) - 1 ] = '\0';
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_ACTION );
	}
    }

    attribute = find( STO_CA_MEDIA_REQUEST_ID, dhsStatus );
    attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type == DHS_DT_STRING && nDims == 0 )
	{
	    mediaId = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_MEDIA_REQUEST_ID );
	}
    }

    if ( dhsStatus != DHS_S_SUCCESS || !status.ok() )
    {
	//
	// Invalid command format. 
	//

	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_FORMAT( status, name() );
    }

    if ( ! status.ok() )
    {
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Convert the action string into an action type. 
    //
    
    if ( ( actionType = cStoAction::str2action( actionString ) ) == 
	    STO_AT_NONE )
    {
	status.E_ACTION_INVALID( status, actionString );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Check if media ID is valid.
    //

    pMedia = NULL;
    pMedia = cStoMedia::find( mediaId, NULL );
    if ( pMedia == NULL )
    {
	(void) sprintf( responseString, STO_RSP_MEDIAID_INVALID, mediaId );
	response( DHS_CS_ERROR, responseString, dhsStatus );
	return;
    }


    //
    // Find the action and stop it.
    //

    cStoAction::queueLock();
    pAction = cStoAction::findAction( actionType, mediaId );
    if ( pAction != NULL )
    {
	pAction->stopAction();
    }
    cStoAction::queueUnlock();
    cStoNode::broadcast();


    //
    // Respond with DONE.
    //
    
    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoActionCmd::execRollbackRequest
//
// INVOCATION:
// execRollbackRequest();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Starts processing a rollback request command
//
// DESCRIPTION:
// Log receipt of command, parse out command attributes, create and queue
// an action for the rollback request.
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

void		cStoActionCmd::execRollbackRequest
(
    void
)
{
    char	actionString[16];	// action string.
    STO_ACTION_TYPE
    		actionType;		// action type.
    cDhsAttrib	attribute;		// attribute object.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status. 
    char        *id;			// Identity of client.
    char        *ip;			// Ip address of client.
    void	*pData;			// Pointer to attribute data.
    int		mediaId;		// media id.
    cStoMedia	*pMedia;		// Ptr to media object.
    char	responseString[256];	// String to format response into.
    char	rollbackString[256];	// return string from rollback().
    cStoStatus	status;			// Status. 
    int		number;			// Number of units to rollback.
    cStoAction	*pAction;		// The action created.
    DHS_DATA_TYPE			// Data type of an attribute.
    		type;
    int		nDims;			// Number of dimensions of an attribute.

    char* dummy; // XXX allan: needed to fill ref param in attribute.info call

    //
    // Respond to let client continue. Log receipt of command. 
    //
    
    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    // 
    // What should we really be doing with simulation???
    //

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    } 


    //
    // Find required attributes. Get their value.
    //

    status.displayStop();
    attribute = find( STO_CA_ACTION, dhsStatus );
    attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type == DHS_DT_STRING && nDims == 0 )
	{
	    strncpy( actionString, (char *) pData, sizeof( actionString ) );
	    actionString[ sizeof( actionString ) - 1 ] = '\0';
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_ACTION );
	}
    }

    attribute = find( STO_CA_MEDIA_REQUEST_ID, dhsStatus );
    attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
	    dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	if ( type == DHS_DT_STRING && nDims == 0 && 
		strne( (char *) pData, "" ) )
	{
	    mediaId = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_MEDIA_REQUEST_ID );
	}
    }


    if ( dhsStatus != DHS_S_SUCCESS || ! status.ok() )
    {
	//
	// Invalid command format. 
	//

	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_FORMAT( status, name() );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    if ( ! status.ok() )
    {
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Convert action string to action type. 
    //

    if ( ( actionType = cStoAction::str2action( actionString ) ) == 
	    STO_AT_NONE )
    {
	status.E_ACTION_INVALID( status, actionString );
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    if ( actionType == STO_AT_RETRIEVE || 
	    actionType == STO_AT_CLEANUP || 
	    actionType == STO_AT_NOTIFY )
    {
	(void) sprintf( responseString, STO_RSP_ACTION_UNROLLBACKABLE, 
		actionString );
	response( DHS_CS_ERROR, responseString, dhsStatus );
	return;
    }


    //
    // Check if media ID is valid.
    //

    pMedia = NULL;
    pMedia = cStoMedia::find( mediaId, NULL );
    if ( pMedia == NULL )
    {
	(void) sprintf( responseString, STO_RSP_MEDIAID_INVALID, mediaId );
	response( DHS_CS_ERROR, responseString, dhsStatus );
	return;
    }


    attribute = find( STO_CA_NUMBER, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB )
    {
	number = 0;
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	attribute.info( dummy, type, nDims, (unsigned long*)NULL, (void **) &pData, 
		dhsStatus );
	if ( dhsStatus == DHS_S_SUCCESS && type == DHS_DT_STRING && 
		nDims == 0 && strne( (char *) pData, "" ) )
	{
	    number = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_NUMBER );
	}
    }
    else
    {
	dhsStatus = DHS_S_SUCCESS;
	status.E_ACTION_INVALID( status, actionString );
    }


    if ( ! status.ok() )
    {
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }


    //
    // Lock action queue. 
    //

    cStoAction::queueLock();


    //
    // If state of action is not IDLE, refuse command.
    // 

    if ( pMedia->getActionState( actionType ) != STO_FST_IDLE )
    {
	cStoAction::queueUnlock();
	(void) sprintf( responseString, STO_RSP_ACTION_ALREADY, 
		actionString, mediaId );
	response( DHS_CS_ERROR, responseString, dhsStatus );
	return;
    }


    //
    // Create an action request and add it to the queue.
    //

    pAction = NULL;
    switch ( actionType )
    {
	case	STO_AT_QUEUE:
	    checkNull( pAction = new cStoQueue( pMedia, false, number, true ), 
		    status, VOID );
	    break;

	case	STO_AT_PREPARE:
	    checkNull( pAction = new cStoPrepare( pMedia, number, true ), 
		    status, VOID );
	    break;

	case	STO_AT_WRITE:
	    checkNull( pAction = new cStoWrite( pMedia, false, number, NULL, 
		    true ), status, VOID );
	    break;
    }


    //
    // Check to see if the action is disabled.
    //

    if ( pAction != NULL && pAction->disabled() )
    {
	delete pAction;
	status.S_ROLLBACK_DISABLED( status, actionString );
    }


    //
    // Unlock the action queue.
    //

    cStoAction::queueUnlock();


    //
    // Broadcast to the nodes to allow an idle not to process this request.
    //

    cStoNode::broadcast();


    if ( !status.ok() )
    {
	//
	// Respond that the rollback failed.
	//

	response( DHS_CS_ERROR, rollbackString, dhsStatus );
    }
    else
    {
	//
	// Respond that command is DONE. 
	//

	response( DHS_CS_DONE, dhsStatus );
    }


    return;
}
