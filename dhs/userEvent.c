static char rcsid[] = "$Id: userEvent.c,v 1.1.1.1 2002-11-24 20:20:56 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997				(c) 1997
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
 * All rights reserved			Tous droits reserves
 * 					
 * NRC disclaims any warranties,	Le CNRC denie toute garantie
 * expressed, implied, or statu-	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.	fortuit, resultant de l'utili-
 * 					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * dhsLibrary/src/userEvent.c
 *
 * PURPOSE:
 * Contains routines to handle IMP user events for the DHS library.
 *
 * FUNCTION NAME(S)
 * dhsUserEvent - Function to handle user IMP messages.
 * msgApply - Handle apply messages.
 * msgBdGet - Handle bulk data get messages.
 * msgBdPut - Handle bulk data put request messages.
 * msgBdReply - Handle a bulk data reply message.
 * msgCmdReply - Handle a command reply message.
 * msgConAccept - Handle a connection accept message from a server.
 * msgConRefuse - Handle a connection refuse message from a server.
 * msgConVerify - Handle a connection verify message from a server.
 * msgDisconnect - Handle a disconnect message from a client.
 * msgElExit - Cause the IMP event loop to exit.
 * msgServerExit - Handle a server exit message.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.31  2001/04/04 16:31:49  nhill
 * Fixed some potential locking problems.
 *
 * Revision 1.30  2001/03/27 20:30:24  nhill
 * Added a check for messages from unkown senders.
 *
 * Revision 1.29  1998/08/04 16:15:45  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.28  1998/06/24 23:09:01  nhill
 * Added check to ensure connetion ids are not null.
 *
 * Revision 1.27  1998/06/02 18:45:34  nhill
 * Removed function msgHeartBeat.
 * Added code to ignore command not found messages when the error applies
 * to a connection that isn't open.
 *
 * Revision 1.26  1998/05/11 16:33:19  nhill
 * Added a reconnect message.
 *
 * Revision 1.25  1998/03/05 23:36:47  nhill
 * Changed to prevent usage of freed connect structures while
 * connections are being closed.
 *
 * Revision 1.24  1998/02/24 01:09:10  nhill
 * Bug fixes.
 *
 * Revision 1.23  1998/02/16 23:45:04  nhill
 * Added a void message.
 * Changed to use dhsConnectUnlock to unlock the connection mutex.
 * Bug fixes.
 *
 * Revision 1.22  1998/01/20 18:23:38  nhill
 * Added a call to the connection callback routine whenever the
 * connection state changes.
 *
 * Revision 1.21  1997/12/22 18:11:43  nhill
 * Fixed the DHS_S_SHUTDOWN message.
 *
 * Revision 1.20  1997/12/18 22:15:34  nhill
 * Made the changes required to allow automatic reconnection attempts
 * after a crash.
 *
 * Revision 1.19  1997/12/11 21:32:29  nhill
 * Added code to terminate commands when a connection is closed.
 *
 * Revision 1.18  1997/08/28 23:52:11  nhill
 * Fixed up the documentation and headers.
 *
 * Revision 1.17  1997/08/19 19:46:41  nhill
 * Added an automatic response to be sent to clients sending messages when
 * the server is not set up to receive messages.
 *
 * Revision 1.16  1997/07/29 22:17:14  nhill
 * Added checking to ensure a connection isn't disconnected more than once.
 *
 * Revision 1.15  1997/07/24 21:28:40  nhill
 * Changed to use dhsMsgCallback to call the error handler.
 *
 * Revision 1.14  1997/07/23 19:38:49  nhill
 * Bug fix.
 *
 * Revision 1.13  1997/07/14 23:18:02  nhill
 * Fixed a typo in a fnEntry string.
 *
 * Revision 1.12  1997/06/09 21:19:55  nhill
 * Changed the disconnnect handler to closs all copies of a connection
 * when multiple connections are open to a single server.
 *
 * Revision 1.11  1997/06/04 18:20:29  nhill
 * Changed some dataLabel terminology to datasetName.
 *
 * Revision 1.10  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.9  1997/02/17 20:14:32  nhill
 * Added error callback on close connect.
 *
 * Revision 1.8  1997/02/04 19:41:34  nhill
 * Checkpoint for install.
 *
 * Revision 1.7  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.6  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.5  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.4  1997/01/20 17:15:11  nhill
 * Checkpoint.
 *
 * Revision 1.3  1997/01/14 20:25:07  nhill
 * Stable Check in.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include "localDhs.h"

#include <time.h>
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif


static void	msgApply( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgBdGet( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgBdPut( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgBdReply( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgCmdReply( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgConAccept( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgConRefuse( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgConVerify( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgDisconnect( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgElExit( int, void *, int, tDhsConnect *, DHS_STATUS * );
static void	msgServerExit( int, void *, int, tDhsConnect *, DHS_STATUS * );


static		tDhsCond	timerCond;
static		boolean		condInit = FALSE;

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsUserEvent
 *
 * INVOCATION:
 * dhsUserEvent( tag, type, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) type (tDhsUsrMsg) The message type.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Function to handle user messages
 *
 * DESCRIPTION:
 * Call an appropriate function for the message type.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	dhsUserEvent
(
    int		tag,		/* (in)  The IMP message tag.		*/
    tDhsUsrMsg	type,		/* (in)  The message type.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "dhsUserEvent" );
    statCheck( *pStatus, VOID );

    dhsMsgFmt( DHS_S_USER_EVENT, DHS_DEBUG_ON, type, tag );

    if ( pSender == NULL && type != DHS_MSG_VOID && 
	    type != DHS_MSG_HEART_BEAT && type != DHS_MSG_EL_EXIT )
    {
	dhsMsgFmt( DHS_E_MSG_SENDER, DHS_DEBUG_OFF, type );
	dhsMsgCallback( dhsConnectId( pSender ), tag );
    }


    switch( type )
    {
	case	DHS_MSG_APPLY:
	    msgApply( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_BD_GET:
	    msgBdGet( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_BD_PUT:
	    msgBdPut( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_BD_REPLY:
	    msgBdReply( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_CMD_REPLY:
	    msgCmdReply( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_CON_ACCEPT:
	    msgConAccept( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_CON_OK:
	    /* This is ignored. */
	    break;
	case	DHS_MSG_CON_REFUSE:
	    msgConRefuse( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_CON_VERIFY:
	    msgConVerify( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_DISCONNECT:
	    msgDisconnect( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_EL_EXIT:
	    msgElExit( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_SERVER_EXIT:
	    msgServerExit( tag, message, length, pSender, pStatus );
	    break;
	case	DHS_MSG_VOID:
	    break;
	default:
	    dhsMsgFmt( DHS_E_IMP_USER_MSG, DHS_DEBUG_OFF, type );
	    dhsMsgCallback( dhsConnectId( pSender ), tag );
    }


    /*
     *  check the status and report any errors through the error callback.
     */

    if ( *pStatus != DHS_S_SUCCESS && *pStatus != DHS_S_EL_EXIT )
    {
	dhsMsgCallback( dhsConnectId( pSender ), tag );
	*pStatus = DHS_S_SUCCESS;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgApply
 *
 * INVOCATION:
 * msgApply( tag, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle apply messages
 *
 * DESCRIPTION:
 * - Decode the message into individual components. 
 * - Execute the command callback function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgApply
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* Attribute value list.		*/
    ulong	bulkLength;	/* Length of the bulk data buffer.	*/
    DHS_CMD_STATUS
    		cmdState;	/* The new command state.		*/
    char	*name;		/* Command name.			*/
    byte	*pBulkData;	/* Pointer to bulk data buffer.		*/


    fnEntry( "msgApply" );
    statCheck( *pStatus, VOID );


    if ( pSender == NULL )
    {
	fnReturn( VOID );
    }


    /*
     *  Decode the message into individual components.
     */

    check( dhsCmdDecode( message, length, &name, &avList, &cmdState,
	    &pBulkData, &bulkLength, pStatus ), *pStatus, VOID );


    if ( dhsLocal.dlCBFCommand == NULL )
    {
	/*
	 *  No command callback function was installed. Send a response
	 *  back to the client indicating this task cannot respond to 
	 *  commands.
	 */

	dhsMsgFmt( DHS_E_CB_NULL, DHS_DEBUG_OFF, "command" );
	*pStatus = DHS_E_CB_NULL;
	dhsMsgCallback( dhsConnectId( pSender ), tag );

	*pStatus = DHS_S_SUCCESS;
	dhsCmdResponse( dhsConnectId( pSender ), tag, DHS_CS_ERROR, 
		"Application cannot receive commands", DHS_AV_LIST_NULL, 
		pStatus );
    }
    else
    {
	/*
	 *  Execute the command callback function.
	 */

	dhsLocal.dlCBFCommand( pSender, tag, name, avList );
    }


    /*
     *  Free the command attribute value list.
     */

    check( dhsAvListFree( avList, pStatus ), *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgBdGet
 *
 * INVOCATION:
 * msgBdGet( tag, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle bulk data get messages
 *
 * DESCRIPTION:
 * - Decode the message into its components.
 * - Extract the get type from the message av list.
 * - Execute the server get callback function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgBdGet
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_ID	avId;		/* Id of an attribute.			*/
    DHS_AV_LIST	avList;		/* Command av list.			*/
    ulong	bulkLength;	/* Length of the bulk data.		*/
    DHS_CMD_STATUS
    		cmdState;	/* The new command state.		*/
    char	*name;		/* Name of the command.			*/
    byte	*pBulkData;	/* Pointer to bulk data buffer.		*/
    DHS_BD_GET_TYPE
    		*pGetType;	/* Type for for returned data.		*/


    fnEntry( "msgBdGet" );
    statCheck( *pStatus, VOID );


    if ( pSender == NULL )
    {
	fnReturn( VOID );
    }


    /*
     *  Decode the message into its components.
     */

    check( dhsCmdDecode( message, length, &name, &avList, &cmdState,
	    &pBulkData, &bulkLength, pStatus ), *pStatus, VOID );



    /*
     * Get the get type from the avlist.
     */

    avId = dhsAvFind( avList, "getType", pStatus );
    dhsAvInfo( avId, NULL, NULL, NULL, NULL, &pGetType, pStatus );

    if ( *pStatus == DHS_S_SUCCESS )
    {
	check( dhsAvListFree( avList, pStatus ), *pStatus, VOID );

	if ( dhsLocal.dlCBFServerGet == NULL )
	{
	    /*
	     *  No message callback was installed.
	     */

	    dhsMsgFmt( DHS_E_CB_NULL, DHS_DEBUG_OFF, "server get" );
	    *pStatus = DHS_E_CB_NULL;
	    dhsMsgCallback( dhsConnectId( pSender ), tag );

	    *pStatus = DHS_S_SUCCESS;
	    dhsCmdResponse( dhsConnectId( pSender ), tag, DHS_CS_ERROR, 
		    "Application cannot receive bulk data get requests", 
		    DHS_AV_LIST_NULL, pStatus );
	}
	else
	{
	    /*
	     *  Execute the server get callback function.
	     */

	    dhsLocal.dlCBFServerGet( pSender, tag, name, *pGetType );
	}
    }
    else
    {
	*pStatus = DHS_S_SUCCESS;
	dhsCmdResponse( dhsConnectId( pSender ), tag, DHS_CS_ERROR, 
		"Invalid bulk data get command received", 
		DHS_AV_LIST_NULL, pStatus );
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgBdPut
 *
 * INVOCATION:
 * msgBdPut( tag, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle bulk data put request messages
 *
 * DESCRIPTION:
 * - Decode the message
 * - Extract information about the bulk daa from the av list.
 * - Execute the put callback function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgBdPut
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_ID	av;		/* Attribute id.			*/
    DHS_AV_LIST	avList;		/* Attribute value list.		*/
    ulong	bulkLength;	/* Length of the bulk data segment.	*/
    DHS_CMD_STATUS
    		cmdState;	/* The new command state.		*/
    char	*datasetName;	/* Data label for the data.		*/
    boolean	*last;		/* True if this is last data for the ds.*/
    char	*name;		/* Command name.			*/
    byte	*pBulkData;	/* Pointer to the bulk data.		*/
    DHS_BD_PUT_TYPE
    		*putType;	/* Type of the incoming data.		*/



    fnEntry( "msgBdPut" );
    statCheck( *pStatus, VOID );


    if ( pSender == NULL )
    {
	fnReturn( VOID );
    }


    /*
     *  Decode the message into its components.
     */
     
    check( dhsCmdDecode( message, length, &name, &avList, &cmdState,
	    &pBulkData, &bulkLength, pStatus ), *pStatus, VOID );


    /*
     *  Make sure an av list was found.
     */

    if ( avList == DHS_AV_LIST_NULL )
    {
	dhsMsgFmt( DHS_E_IMP_MSG, DHS_DEBUG_OFF, "bulk data put" );
	dhsMsgCallback( dhsConnectId( pSender ), tag );

	*pStatus = DHS_S_SUCCESS;
	dhsCmdResponse( dhsConnectId( pSender ), tag, DHS_CS_ERROR, 
		"Invalid bulk data put command received", 
		DHS_AV_LIST_NULL, pStatus );

	fnReturn( VOID );
    }
    else
    {
	/*
	 *  Extract information about the dataset from the av list.
	 */

	av = dhsAvFind( avList, "datasetName", pStatus );
	dhsAvInfo( av, NULL, NULL, NULL, NULL, &datasetName, pStatus );
	av = dhsAvFind( avList, "putType", pStatus );
	dhsAvInfo( av, NULL, NULL, NULL, NULL, &putType, pStatus );
	av = dhsAvFind( avList, "last", pStatus );
	dhsAvInfo( av, NULL, NULL, NULL, NULL, &last, pStatus );
    }


    if ( *pStatus == DHS_S_SUCCESS )
    {
	if ( dhsLocal.dlCBFServerPut == NULL )
	{
	    /*
	     *  This application isn't set up to handle bulk data puts.
	     */

	    dhsMsgFmt( DHS_E_CB_NULL, DHS_DEBUG_OFF, "server put" );
	    *pStatus = DHS_E_CB_NULL;
	    dhsMsgCallback( dhsConnectId( pSender ), tag );

	    *pStatus = DHS_S_SUCCESS;
	    dhsCmdResponse( dhsConnectId( pSender ), tag, DHS_CS_ERROR, 
		    "Application cannot receive bulk data put requests", 
		    DHS_AV_LIST_NULL, pStatus );
	}
	else
	{
	    /*
	     *  Execute the bulk data put callback.
	     */

	    dhsLocal.dlCBFServerPut( pSender, tag, datasetName, *putType, 
		    *last, pBulkData, bulkLength );
	}
    }
    else
    {
	*pStatus = DHS_S_SUCCESS;
	dhsCmdResponse( dhsConnectId( pSender ), tag, DHS_CS_ERROR, 
		"Invalid bulk data put command received", 
		DHS_AV_LIST_NULL, pStatus );
    }

    check( dhsAvListFree( avList, pStatus ), *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgBdReply
 *
 * INVOCATION:
 * msgBdReply( tag, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a bulk data reply message
 *
 * DESCRIPTION:
 * - find the associated command.
 * - decode the message into its components.
 * - execute the dhsBdResp function to handle the data.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgBdReply
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* Attribute value list.		*/
    ulong	bulkLength;	/* Length of the bulk data buffer.	*/
    tDhsCmd	*cmd;		/* The original command structure.	*/
    DHS_CMD_STATUS		/* The new state for the command.	*/
    		cmdState;	/* The new state of the command.	*/
    char	*name;		/* Command name.			*/
    byte	*pBulkData;	/* Pointer to the bulk data buffer.	*/


    fnEntry( "msgBdReply" );
    statCheck( *pStatus, VOID );


    if ( pSender == NULL )
    {
	fnReturn( VOID );
    }


    /*
     *  Find the command associated with the response.
     */

    cmd = dhsCmdPtr( tag );
    dhsCmdRef( cmd, pStatus );


    if ( *pStatus == DHS_E_CMD_NOT_FOUND )
    {
	/*
	 *  Only report not found messages from open connections.
	 */

	if ( pSender->dcState == DHS_CON_CONNECTED )
	{
	    /*
	     *  No command could be found. This is an error.
	     */

	    dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	    dhsMsgCallback( dhsConnectId( pSender ), tag );
	}
	*pStatus = DHS_S_SUCCESS;
    }
    else if ( *pStatus == DHS_S_SUCCESS )
    {
	/*
	 *  Decode the message into its compoents.
	 */

	check( dhsCmdDecode( message, length, &name, &avList, &cmdState,
		&pBulkData, &bulkLength, pStatus ), *pStatus, VOID );


	/*
	 *  Process the response.
	 */

	dhsBdResp( cmd, cmdState, avList, name,  bulkLength, pBulkData, 
		pStatus );


	/*
	 *  Free some resources allocated during the processing.
	 */

	check( dhsAvListFree( avList, pStatus ), *pStatus, VOID );
	dhsCmdDeref( cmd, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgCmdReply
 *
 * INVOCATION:
 * msgCmdReply( tag, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a command reply message
 *
 * DESCRIPTION:
 * - Find the command associated with the reply.
 * - Decode the message into its compoents.
 * - Execute function dhsCmdResp to process the reply.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgCmdReply
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* the response av list.		*/
    ulong	bulkLength;	/* Length of the bulk data.		*/
    tDhsCmd	*pCmd;		/* The orginal command.			*/
    DHS_CMD_STATUS		/* New state of the command.		*/
    		cmdState;
    char	*name;		/* Response string.			*/
    byte	*pBulkData;	/* Pointer to bulk data.		*/


    fnEntry( "msgCmdReply" );
    statCheck( *pStatus, VOID );


    /* 
     *  Find the command associated with the reply.
     */

    pCmd = dhsCmdPtr( tag );
    dhsCmdRef( pCmd, pStatus );

    if ( *pStatus == DHS_E_CMD_NOT_FOUND )
    {
	/*
	 *  Only report not found messages from open connections.
	 */

	if ( pSender != NULL && pSender->dcState == DHS_CON_CONNECTED )
	{
	    /*
	     *  No command could be found. This is an error.
	     */

	    dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	    dhsMsgCallback( dhsConnectId( pSender ), tag );
	}
	*pStatus = DHS_S_SUCCESS;
    }
    else if ( *pStatus == DHS_S_SUCCESS && pSender != NULL )
    {
	/*
	 *  Decode the message into it's components.
	 */

	check( dhsCmdDecode( message, length, &name, &avList, &cmdState,
		&pBulkData, &bulkLength, pStatus ), *pStatus, VOID );


	/*
	 *  Process the response.
	 */

	dhsCmdResp( pCmd, cmdState, avList, name, pStatus );


	/*
	 *  Free some resources.
	 */

	check( dhsAvListFree( avList, pStatus ), *pStatus, VOID );
	dhsCmdDeref( pCmd, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgConAccept
 *
 * INVOCATION:
 * msgConAccept( &msgInfo, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message buffer.
 * (>) length (int) Length of the message buffer.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a connection accept message from a server.
 *
 * DESCRIPTION:
 * Change the state of the connection to "DHS_CON_CONNECTED".
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Connection process must be initiated with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgConAccept
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "msgConAccept" );
    statCheck( *pStatus, VOID );


    if ( pSender != NULL )
    {
	pSender->dcTimeout = tag;
	pSender->dcState = DHS_CON_CONNECTED;
	connectCallback( pSender, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgConRefuse
 *
 * INVOCATION:
 * msgConRefuse( &msgInfo, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message buffer.
 * (>) length (int) Length of the message buffer.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a connection refuse message from a server.
 *
 * DESCRIPTION:
 * Change the state of the connection to "DHS_CON_REFUSE".
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Connection process must be initiated with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgConRefuse
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "msgConRefuse" );
    statCheck( *pStatus, VOID );


    if ( pSender != NULL )
    {
	dhsConnectBreak( pSender, pStatus );
	pSender->dcState = DHS_CON_REFUSED;
	connectCallback( pSender, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgConVerify
 *
 * INVOCATION:
 * msgConVerify( tag, message, length, pSender, pStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message buffer.
 * (>) length (int) Length of the message buffer.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * This function is called when a message verifying the connection is
 * received
 *
 * DESCRIPTION:
 * Send a DHS_MSG_CON_OK back to the sender.
 *
 * EXTERNAL VARIABLES:
 * None
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgConVerify
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* Pointer to the response command.	*/
    DHS_STATUS	s2;


    fnEntry( "msgConVerify" );
    statCheck( *pStatus, VOID );


    if ( pSender != NULL )
    {
	pCmd = dhsCmdNew( NULL, DHS_AV_LIST_NULL, NULL, DHS_MSG_CON_OK, 
		DHS_CF_PRIORITY, DHS_TAG_NULL, DHS_CS_DONE, pStatus );
	dhsConnectLock( pSender, pStatus );
	dhsConnectSend( pSender, pCmd, pStatus );
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( pSender, &s2 );
	if ( *pStatus == DHS_S_SUCCESS )
	{
	    *pStatus = s2;
	}
	s2 = DHS_S_SUCCESS;
	dhsCmdDeref( pCmd, &s2 );
	if ( *pStatus == DHS_S_SUCCESS )
	{
	    *pStatus = s2;
	}
    }
    

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgDisconnect
 *
 * INVOCATION:
 * msgDisconnect( &msgInfo, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message buffer.
 * (>) length (int) Length of the message buffer.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a disconnect message from a client.
 *
 * DESCRIPTION:
 * Delete the connection structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Connection process must be initiated with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgDisconnect
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    IMP_ConnectInfo
    		*conInfo;	/* Pointer to the imp connetion info.	*/

    
    fnEntry( "msgDisconnect" );
    statCheck( *pStatus, VOID );


    if ( pSender != NULL && pSender->dcState != DHS_CON_NOT_CONNECTED )
    {
	/*
	 *  execute the error callback.
	 */
	
	conInfo = &( pSender->dcConnectInfo );

	if ( pSender->dcState == DHS_CON_DISCONNECTING )
	{
	    dhsMsgFmt( DHS_E_DISCONNECT, DHS_DEBUG_OFF, conInfo->TaskName, 
		    conInfo->TaskID.Machine, pSender->dcImpConnect );
	    dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	}
	else
	{
	    dhsMsgFmt( DHS_S_DISCONNECT, DHS_DEBUG_OFF, conInfo->TaskName, 
		    conInfo->TaskID.Machine, pSender->dcImpConnect );

	    pSender->dcState = DHS_CON_DISCONNECTING;
	    dhsCmdConnectLost( pSender, pStatus );
	    pSender->dcState = DHS_CON_NOT_CONNECTED;
	    connectCallback( pSender, pStatus );
	    
	    dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	    
	    dhsConnectDeref( pSender, pStatus );
	}
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgElExit
 *
 * INVOCATION:
 * msgElExit( tag, message, length, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) The IMP message tag.
 * (>) message (void *) Pointer to the message.
 * (>) length (int) The length of the message.
 * (>) pSender (tDhsConnect *) Connection to the message sender.
 * (>) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Cause the IMP event loop to exit
 *
 * DESCRIPTION:
 * Set the function return status to DHS_S_EL_EXIT. This status will cause the
 * event loop to stop processing events.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgElExit
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "msgElExit" );
    statCheck( *pStatus, VOID );


    *pStatus = DHS_S_EL_EXIT;

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * msgServerExit
 *
 * INVOCATION:
 * msgServerExit( tag, message, length, pSender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (int) IMP tag associated with the message.
 * (>) message (void *) Pointer to the message data.
 * (>) length (int) Length of the message.
 * (>) pSender (tDhsConnect *) Pointer to the connection sending the message.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a server exit message
 *
 * DESCRIPTION:
 * Close the connection to the server.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	msgServerExit
(
    int		tag,		/* (in)  The imp message tag.		*/
    void	*message,	/* (in)  Pointer to the message.	*/
    int		length,		/* (in)  Length of the message.		*/
    tDhsConnect	*pSender,	/* (in)  Connection to sender.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    struct timespec	
		absTime;
    boolean	connected;	/* Has the connection be re-connected?	*/
    int		i;
    boolean	isOpen;		/* True if the connection is still open.*/
    DHS_STATUS	s;		/* Temporary return status.		*/
    DHS_STATUS	s2;		/* Temporary return status.		*/


    fnEntry( "msgServerExit" );
    statCheck( *pStatus, VOID );

    if ( pSender == NULL )
    {
	fnReturn( VOID );
    }


    if ( dhsLocal.dlCBFError != NULL )
    {
	dhsMsgFmt( DHS_S_SHUTDOWN, DHS_DEBUG_OFF, pSender->dcName, 
		pSender->dcIpAddress );
	dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
    }

    s = DHS_S_SUCCESS;
    dhsConnectLock( pSender, &s );
    if ( s == DHS_S_SUCCESS )
    {
	if ( pSender->dcState == DHS_CON_CONNECTED )
	{
	    isOpen = TRUE;
	}
	else
	{
	    pSender->dcState = DHS_CON_DISCONNECTING;
	    dhsConnectUnlock( pSender, &s );
	    connectCallback( pSender, pStatus );
	    isOpen = FALSE;
	}
    }
    else if ( *pStatus == DHS_E_CON_NOT_FOUND )
    {
	isOpen = FALSE;
	dhsConnectUnlock( pSender, &s );
    }
    else
    {
	fnReturn( VOID );
    }
    
    if ( isOpen )
    {
	dhsConnectBreak( pSender, pStatus );
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( pSender, &s2 );
	if ( *pStatus == DHS_S_SUCCESS )
	{
	    *pStatus = s2;
	}
	connected = FALSE;
	pSender->dcState = DHS_CON_DISCONNECTING;
	dhsCmdConnectLost( pSender, pStatus );
	dhsElBroadcast( pStatus );

	dhsConnectLock( pSender, pStatus );
	for ( i = 0; *pStatus == DHS_S_SUCCESS && 
		i < pSender->dcTimeout && pSender->dcOpenCount > 0; i++ )
	{
	    if ( ! condInit )
	    {
		dhsCondInit( &timerCond, pStatus );
		condInit = TRUE;
	    }

	    clock_gettime( CLOCK_REALTIME, &absTime );
	    absTime.tv_sec ++;
	    dhsCondTimedWait( &timerCond, &absTime, pStatus );
	    if ( *pStatus == DHS_S_COND_TIMEOUT )
	    {
		*pStatus = DHS_S_SUCCESS;
	    }
	    pSender->dcState = DHS_CON_NOT_CONNECTED;
	    s = DHS_S_SUCCESS;
	    dhsConnectMake( pSender, pStatus );
	    if ( s != DHS_S_SUCCESS )
	    {
		break;
	    }

	    if ( *pStatus == DHS_S_SUCCESS && 
		    pSender->dcState == DHS_CON_DUPLICATE )
	    {
		connected = TRUE;
		pSender->dcOpenCount --;
		pSender->dcState = DHS_CON_CONNECTED;
		break;
	    }
	    else if ( *pStatus == DHS_S_SUCCESS && 
		    pSender->dcState == DHS_CON_CONNECTED )
	    {
		connected = TRUE;
		break;
	    }
	    else
	    {
		*pStatus = DHS_S_SUCCESS;
	    }
	}
	dhsConnectUnlock( pSender, pStatus );
	connectCallback( pSender, pStatus );

	if ( ! connected && pSender != NULL )
	{
	    pSender->dcState = DHS_CON_LOST;
	    connectCallback( pSender, pStatus );
	    s = DHS_S_SUCCESS;
	    dhsMsgFmt( DHS_E_CON_LOST, DHS_DEBUG_OFF, pSender->dcName, 
		    pSender->dcIpAddress );
	    dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	    dhsConnectDeref( pSender, &s );
	} else {
	    dhsMsgFmt( DHS_S_RECONNECT, DHS_DEBUG_OFF, pSender->dcName, 
		    pSender->dcIpAddress );
	    dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	}
    }

    fnReturn( VOID );
}
