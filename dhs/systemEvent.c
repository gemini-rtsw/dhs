static char rcsid[] = "$Id: systemEvent.c,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
 * dhs/src/systemEvent.c
 *
 * PURPOSE:
 * Contains routines to handle IMP system events.
 *
 * FUNCTION NAME(S)
 * dhsSystemEvent - Process an IMP system message.
 * bulkHandle - Handle an IMP_SYS_BULK_DATA message.
 * bulkTransfer - Handle an IMP_SYS_BULK_TRANSFER message.
 * bulkWaiting - Handle an IMP_SYS_BULK_WAITING message.
 * connectLost - Called to handle lost connectios detected by the crash
 *		or machineLost functions.
 * connectReject - Handle an IMP_SYS_REJECT message.
 * connectReply - Handle an IMP_CONNECT_REQ message.
 * connectRequest - Handle an IMP_SYS_CONNECT message.
 * crash - Handle an IMP_SYS_CRASH message.
 * disconnect - Handle IMP_SYS_DISC and IMP_SYS_CONN_CLOSE messages.
 * locateRequest - Handle IMP_LOCATE_REQ messages.
 * machineLost - Handles machine lost messages.
 * notify - Handle IMP_SYS_NOTIFY messages.
 * sendErr - Handle IMP_SYS_SEND_ERR messages.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/11/24 20:20:48  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.46  2001/03/27 20:30:01  nhill
 * Fixed several errors in the connection sequence.
 *
 * Revision 1.45  2000/02/15 22:26:12  nhill
 * Separated function crash into three functions: crash which handles
 * IMP_SYS_CRASH messages, machineLost which handles IMP_SYS_MACHINE_LOST
 * messages, and connectLost which contains the code shared by both crash
 * and machineLost. This was necessary because the "context" of machine
 * lost messages is different from crash messages in that machine lost
 * messages indicate that a host has crashed without any specific
 * connection information, where crash messages only apply to a specific
 * connection.
 *
 * Revision 1.44  1999/02/12 18:15:38  nhill
 * Fixed a bug that prevented a connection from going to the
 * DHS_CON_NOT_CONNECTED state.
 *
 * Revision 1.43  1999/02/01 21:04:50  nhill
 * Fixed some error reporting in bulkTransfer.
 *
 * Revision 1.42  1998/08/06 19:52:46  nhill
 * Fixed some bugs.
 *
 * Revision 1.41  1998/08/04 16:15:39  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.40  1998/06/28 23:41:01  nhill
 * Fixed a bug that allowed systems shutting down to cause a connecting
 * task to hang.
 *
 * Revision 1.39  1998/06/26 18:54:12  nhill
 * Fixed a bug that occured when a server system crashed and then re-started.
 *
 * Revision 1.38  1998/06/24 23:09:41  nhill
 * Added locks to protect a mutex in the crash routine.
 *
 * Revision 1.37  1998/06/16 21:54:07  nhill
 * Added a call to dhsCmdConnectLost when a connection disconnects.
 *
 * Revision 1.36  1998/06/02 18:48:26  nhill
 * Added code to ignore close messages that occur when the connection is not
 * open.
 *
 * Revision 1.35  1998/05/11 16:32:28  nhill
 * Added a reconnect message.
 *
 * Revision 1.34  1998/04/23 22:20:00  nhill
 * Changed the names of the temporary files to make it more obvious
 * what the files are, given their name.
 *
 * Revision 1.33  1998/03/05 23:35:56  nhill
 * Changed to prevent usage of freed connect structures while
 * connections are being closed.
 *
 * Revision 1.32  1998/02/24 01:08:50  nhill
 * Bug fixes.
 *
 * Revision 1.31  1998/02/16 23:43:48  nhill
 * Changed to use dhsConnectUnlock to unlock the connection mutex.
 * Bug fixes.
 *
 * Revision 1.30  1998/01/20 18:23:17  nhill
 * Added a call to the connection callback routine whenever the
 * connection state changes.
 *
 * Revision 1.29  1997/12/24 19:30:47  nhill
 * Added handling of the IMP_SYS_REJECT message.
 *
 * Revision 1.28  1997/12/18 22:10:45  nhill
 * Removed the machineLost function. crash is used instead.
 * Made the changes required to allow automatic reconnection attempts
 * after a server crash.
 *
 * Revision 1.27  1997/11/30 21:12:16  nhill
 * Changed the behaviour when a connection is lost.
 *
 * Revision 1.26  1997/09/26 18:02:42  nhill
 * Improved handling of connection close messages.
 *
 * Revision 1.25  1997/09/17 20:37:01  nhill
 * Added handling for system crash messages.
 * Changed to removed shared memory blocks as soon as a response is received.
 *
 * Revision 1.24  1997/08/28 20:46:16  nhill
 * Fixed up the documentation and headers.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include <netinet/in.h>
#include <arpa/inet.h>

#include "gen_types.h"
#include "gen_util.h"
#include "localDhs.h"

#include <time.h>
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

static void	bulkHandle( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *, 
			DHS_STATUS * );
static void	bulkTransfer( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *, 
			DHS_STATUS * );
static void	bulkWaiting( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	connectLost( tDhsConnect *, DHS_STATUS * );
static void	connectReject( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	connectReply( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	connectRequest( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	crash( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	disconnect( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	locateRequest( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	machineLost( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	notify( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );
static void	sendErr( IMP_MsgInfo *, IMP_SysInfo *, tDhsConnect *,
			DHS_STATUS * );

static		tDhsCond	timerCond;
static		boolean		condInit = FALSE;		
tDhsConnect	*tmpConnect = NULL;

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsSystemEvent
 *
 * INVOCATION:
 * dhsSystemEvent( &msgInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pMsgInfo (IMP_MsgInfo *) Pointer to the msgInfo structure.
 * (>) pSender (tDhsConnect *) Pointer to the connection sending the msg.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handles an IMP system message received by the IMP system
 *
 * DESCRIPTION:
 * - Try to let IMP handle the message.
 * - Call a function to handle the specific message.
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

void		dhsSystemEvent
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		handled;	/* True if IMP handled the message.	*/
    IMP_Status	impStatus;	/* IMP function return status.		*/
    IMP_SysInfo	sysInfo;	/* IMP_SysInfo structure for the message.*/


    fnEntry( "dhsSystemEvent" );
    statCheck( *pStatus, VOID );


    /*
     *  Let imp have a chance to process the message.
     */

    impStatus = IMP_STATUS_OK;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpSystemMessage( dhsLocal.dlImpIdIn, msgInfo, &handled, 
	    &sysInfo, &impStatus ), &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    if ( impStatus != IMP_STATUS_OK )
    {
	dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	*pStatus = DHS_E_IMP;
	fnReturn( VOID );
    }


    dhsMsgFmt( DHS_S_SYS_EVENT, DHS_DEBUG_ON, sysInfo.SysMsgType, 
	    msgInfo->Tag );


    /*
     *  Check for each of the system messages that we can handle.
     */

    switch( sysInfo.SysMsgType )
    {
	case	IMP_SYS_REJECT:
	    connectReject( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_CONNECT_REQ:
	    connectReply( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_LOCATE_REQ:
	    locateRequest( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_BULK_DATA:
	    bulkHandle( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_BULK_INTERNAL:
	    /* These messages are ignored. */
	    break;
	case	IMP_SYS_BULK_TRANSFERRED:
	    bulkTransfer( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_BULK_WAITING:
	    bulkWaiting( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_CONN_CLOSE:
	    disconnect( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_CONNECT:
	    connectRequest( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_CRASH:
	    crash( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_DISC:
	    disconnect( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_MACHINE_LOST:
	    machineLost( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_NOTIFY:
	    notify( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_REPORT:
	    /* These messages are ignored. */
	    break;
	case	IMP_SYS_SEND_ERR:
	    sendErr( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	case	IMP_SYS_SYNCH:
	    notify( msgInfo, &sysInfo, pSender, pStatus );
	    break;
	default:
	    dhsMsgFmt( DHS_E_IMP_SYS_MSG, DHS_DEBUG_OFF, sysInfo.SysMsgType );
	    dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
    }
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	*pStatus = DHS_S_SUCCESS;
    }
	
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * bulkHandle
 *
 * INVOCATION:
 * bulkHandle( &msgInfo, &sysInfo, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a IMP_SYS_BULK_DATA message
 *
 * DESCRIPTION:
 * - Get the message from the IMP bulk data buffer.
 * - Handle the message as if it was a normal user level message.
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

static void	bulkHandle
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    IMP_BulkReportInfo
    		bulkReport;	/* Report to send back to the sender.	*/
    IMP_Status	impStatus;	/* Imp function return status.		*/
    long	messageSize;	/* Size of the message.			*/
    byte	*pMessage;	/* Address of the IMP bulk buffer.	*/
    IMP_SharedMemInfo
    		sharedMemInfo;	/* IMP shared memory information.	*/
    tDhsUsrMsg	type;		/* Type of the command.			*/


    fnEntry( "bulkHandle" );
    statCheck( *pStatus, VOID );

    impStatus = IMP_STATUS_OK;


    /*
     *  Get the information about the bulk data.
     */

    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    msgInfo->NotifyBytes = 0;
    msgInfo->RefNumber = -1;
    impCheckNoRet( ImpHandleBulk( dhsLocal.dlImpIdIn, msgInfo, &sharedMemInfo, 
	    (void **) &pMessage, &messageSize, &impStatus ), 
	    &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    if ( impStatus != STATUS__OK )
    {
	dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	fnReturn( VOID );
    }


    /*
     *  Initialize the bulkReport structure.
     */

    bulkReport = sysInfo->BulkReport;
    bulkReport.TransferredBytes = bulkReport.TotalBytes;
    bulkReport.Released = FALSE;
    bulkReport.TargetTag = -1;
    bulkReport.TargetRef = DHS_MSG_BULK_HANDLE;


    /*
     *  Report back to the sending task that the transfer is complete.
     */

    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpBulkReport( dhsLocal.dlImpIdIn, &(sysInfo->TaskID), 
	    &bulkReport, &impStatus ), &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    if ( impStatus != STATUS__OK )
    {
	impStatus = STATUS__OK;
	dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus );
	ImpReleaseShared( &sharedMemInfo, FALSE, &impStatus );
	dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus );
	dhsMsgCallback( dhsConnectId( pSender ), bulkReport.SenderTag );
	fnReturn( VOID );
    }


    /*
     *  Handle data in the same way a normal user message would be handled.
     */

    type = *pMessage;
    dhsUserEvent( bulkReport.SenderTag, type, pMessage + 4, messageSize - 4, 
	    pSender, pStatus );


    /*
     *  Release the shared memory buffer.
     */

    dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus );
    ImpReleaseShared( &sharedMemInfo, FALSE, &impStatus );


    bulkReport.Released = TRUE;
    impCheckNoRet( ImpBulkReport( dhsLocal.dlImpIdIn, &(sysInfo->TaskID), 
	    &bulkReport, &impStatus ), &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );


    /*
     *  Display an error messages.
     */

    if ( impStatus != IMP_STATUS_OK )
    {
	dhsMsgCallback( dhsConnectId( pSender ), bulkReport.SenderTag );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * bulkTransfer
 *
 * INVOCATION:
 * bulkTransfer( &msgInfo, &sysInfo, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a IMP_SYS_BULK_TRANSFER message
 *
 * DESCRIPTION:
 * When the data transfer is complete, process the message if it was an
 * incomming message, or free the shared memory if it was an outgoing message.
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

static void	bulkTransfer
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* The command originating the transfer.*/
    IMP_Status	impStatus;	/* IMP function return status.		*/
    tDhsBt	*pBt;		/* Pointer to bulk transfer struct.	*/
    tDhsUsrMsg	type;		/* The type of message received.	*/


    fnEntry( "bulkTransfer" );
    statCheck( *pStatus, VOID );


    /*
     *  Do nothing until the bulk transfer is complete.
     */

    if ( sysInfo->BulkReport.Released )
    {
	if ( sysInfo->BulkReport.TargetRef == DHS_MSG_BULK_READ )
	{
	    /*
	     *  The bulk transfer was a bulk data get function. Attempt to 
	     *  find the information about the bulk transfer, and then 
	     *  process the completion of the get as if is was a normal
	     *  user message.
	     */

	    pBt = dhsBtFind( DHS_BF_BT_TAG, pStatus, 
		    sysInfo->BulkReport.TargetTag );

	    if ( *pStatus == DHS_E_BT_NOT_FOUND )
	    {
		/*
		 *  Could not find the bulk transfer information.
		 */

		dhsMsgFmt( DHS_E_BT_NOT_FOUND, DHS_DEBUG_OFF, 
			sysInfo->BulkReport.SenderTag );
		dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );
	    }
	    else
	    {
		/*
		 *  Process the message as if it was a normal user message.
		 */

		type = *( pBt->dbAddress );
		dhsUserEvent( sysInfo->BulkReport.SenderTag, type, 
			pBt->dbAddress + 4, pBt->dbSize - 4, 
			pBt->dbConnect, pStatus );
		dhsBtDelete( pBt, pStatus );
	    }
	}
	else 
	{
	    /*
	     *  The message was the result of a put. Simply free the shared
	     *  memory.
	     */

	    pCmd = dhsCmdPtr( sysInfo->BulkReport.SenderRef );
	    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, pCmd );

	    if ( *pStatus == DHS_E_CMD_NOT_FOUND )
	    {
		dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, 
			sysInfo->BulkReport.SenderRef );
		dhsMsgCallback( dhsConnectId( pSender ), 
			sysInfo->BulkReport.SenderRef);
	    }
	    else
	    {
		impStatus = IMP_STATUS_OK;
		if ( pCmd->dcSharedMemUsed )
		{
		    impCheck( ImpReleaseShared( &(pCmd->dcSharedMem), TRUE, 
			    &impStatus ), impStatus, *pStatus, VOID );
		    pCmd->dcSharedMemUsed = FALSE;
		    dhsCmdDeref( pCmd, pStatus );
		}
	    }
	}
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * bulkWaiting
 *
 * INVOCATION:
 * bulkWaiting( &msgInfo, &sysInfo, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a IMP_SYS_BULK_WAITING message
 *
 * DESCRIPTION:
 * - Create a bulk transfer structure to receive the data.
 * - Identify the bulk data area to IMP so it can copy the data into the
 *   buffer.
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

static void	bulkWaiting
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    byte	*address;	/* Address of shared memory area.	*/
#ifndef		vxWorks
    char	baseName[PATH_MAX];
    char	*fname;		/* MMAP file name.			*/
#endif		/* !vxWorks */
    IMP_Status	impStatus;	/* IMP function return status.		*/
    tDhsBt	*pBt;		/* Bulk transfer structure.		*/
    tDhsConnect	*pConnect;	/* DHS connection structure.		*/
    IMP_Status	smStat;		/* Shared memory allocation status.	*/


    fnEntry( "bulkWaiting" );
    statCheck( *pStatus, VOID );


    /* 
     *  locate the connection structure based on the task id.
     */

    pConnect = dhsConnectFind( DHS_CF_TASK_ID, pStatus, &(sysInfo->TaskID) );


    /* 
     *  Create a new bulk transfer structure.
     */

    check( pBt = dhsBtNew( pConnect, pStatus ), *pStatus, VOID );


    /*
     *  Create a new share memory area for the transfer.
     */

    smStat = IMP_STATUS_OK;
#ifdef vxWorks
    ImpDefineShared( IMP_SHARE_GLOBAL, NULL, 0, sysInfo->BulkReport.TotalBytes,
	    TRUE, (void **) &address, &( pBt->dbSharedMem ), &smStat );
#else	/* vxWorks */
    fname = gen_tempnam( "/tmp", "dhsImp-" );
    sprintf( baseName, "%s-bulkWaiting-%s", fname, dhsLocal.dlImpName );
    dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus );
    ImpDefineShared( IMP_SHARE_MMAP, baseName, 0, 
	    sysInfo->BulkReport.TotalBytes, TRUE, (void **) &address, 
	    &( pBt->dbSharedMem ), &smStat );
    dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus );
    free( fname );
#endif
    if ( smStat == IMP_STATUS_OK )
    {
	pBt->dbSharedMemUsed = TRUE;
	pBt->dbAddress = address;
	pBt->dbSize = sysInfo->BulkReport.TotalBytes;
    }


    /*
     *  Send the new shared area to IMP so it can put the data into it.
     */

    msgInfo->RefNumber = DHS_MSG_BULK_READ;
    msgInfo->Tag = dhsBtTag( pBt );
    msgInfo->NotifyBytes = 0;
    impStatus = IMP_STATUS_OK;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpReadBulk( dhsLocal.dlImpIdIn, &( sysInfo->TaskID ), 
	    &( pBt->dbSharedMem ), msgInfo, smStat, 0, &impStatus ), 
	    &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    if ( impStatus != IMP_STATUS_OK )
    {
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	*pStatus = DHS_E_IMP;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * connectLost
 *
 * INVOCATION:
 * connectLost( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pConnect (tDhsConnect) The connection that is lost.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * This is called to indicate the the specified connection is lost
 *
 * This is called by machineLost or crash to close connections.
 *
 * DESCRIPTION:
 * Call the dhsConnectLost function, and if this is a connection to a server
 * attempt to re-connect.
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

static void	connectLost
(
    tDhsConnect	*pConnect,	/* (in)  The connection lost.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    char	*address;	/* Address of the server.		*/
    struct timespec	
		absTime;
    DHS_CONNECT	connectId;	/* Id of the connection being broken.	*/
    int		i;
    boolean	lost;
    char	*name;		/* Name of the server.			*/

    fnEntry( "connectLost" );
    statCheck( *pStatus, VOID );


    pConnect->dcState = DHS_CON_LOST;
    pConnect->dcImpConnect = -1;

    dhsCmdConnectLost( pConnect, pStatus );

    connectId = dhsConnectId( pConnect );
    dhsConnectInfo( connectId, &address, &name, pStatus );


    /*
     *  If this is a connection to a server, attempt to re-connect.
     */

    lost = TRUE;
    if ( ! pConnect->dcClientConnect )
    {
	tmpConnect = pConnect;
	dhsConnectLock( pConnect, pStatus );
	for ( i = 0; *pStatus == DHS_S_SUCCESS && 
		i < pConnect->dcTimeout; i++ )
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

	    pConnect->dcState = DHS_CON_NOT_CONNECTED;
	    dhsConnectMake( pConnect, pStatus );
	    if ( *pStatus == DHS_S_SUCCESS && 
		    pConnect->dcState == DHS_CON_DUPLICATE )
	    {
		lost = FALSE;
		pConnect->dcOpenCount --;
		pConnect->dcState = DHS_CON_CONNECTED;
		break;
	    }
	    else if ( *pStatus == DHS_S_SUCCESS && 
		    pConnect->dcState == DHS_CON_CONNECTED )
	    {
		lost = FALSE;
		break;
	    }
	    else
	    {
		*pStatus = DHS_S_SUCCESS;
	    }
	}
	dhsConnectUnlock( pConnect, pStatus );
	tmpConnect = NULL;
	if ( lost )
	{
	    pConnect->dcState = DHS_CON_LOST;
	    pConnect->dcImpConnect = -1;
	}
    }

    if ( lost )
    {
	dhsMsgFmt( DHS_E_CON_LOST, DHS_DEBUG_OFF, name, address );
	dhsMsgCallback( connectId, DHS_TAG_NULL );

	connectCallback( pConnect, pStatus );
	dhsConnectDeref( pConnect, pStatus );
    }
    else
    {
	dhsMsgFmt( DHS_S_RECONNECT, DHS_DEBUG_OFF, name, address );
	dhsMsgCallback( connectId, DHS_TAG_NULL );
	connectCallback( pConnect, pStatus );
    }



    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
    }
    

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * connectReject
 *
 * INVOCATION:
 * connectReject( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handles the connect reject message
 *
 * This message is set to the client when the server rejects the connection.
 * The response is to mark the connection as being refused.
 *
 * DESCRIPTION:
 * - Find the connection structure.
 * - Mark the connection as being refused.
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

static void	connectReject
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsConnect	*connect;	/*  The connection being completed.	*/

    fnEntry( "connect_reject" );


    /* 
     *  locate the connection based on the server task id.
     */

    connect = dhsConnectFind( DHS_CF_TASK_ID, pStatus, &(sysInfo->TaskID) );
    if ( *pStatus == DHS_S_SUCCESS )
    {
	/*
	 *  Mark the connection as being refused.
	 */

	connect->dcState = DHS_CON_REFUSED;
	dhsConnectBreak( connect, pStatus );
	connectCallback( connect, pStatus );
    }


    *pStatus = DHS_S_SUCCESS;

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * connectReply
 *
 * INVOCATION:
 * connectReply( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handles the connect reply message
 *
 * This message is set to the client when the server accepts the connection.
 * The response is to mark the connection as being open but not accepted.
 *
 * DESCRIPTION:
 * - Find the connection structure.
 * - Mark the connection as being open but not accepted.
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

static void	connectReply
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsConnect	*connect;	/*  The connection being completed.	*/
    DHS_STATUS	s;

    fnEntry( "connect_reply" );


    /* 
     *  locate the connection based on the server task id.
     */

    s = DHS_S_SUCCESS;
    connect = dhsConnectFind( DHS_CF_TASK_ID, &s, &(sysInfo->TaskID) );
    if ( s != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CON_NOT_FOUND, DHS_DEBUG_OFF );
	dhsMsgCallback( dhsConnectId( connect ), DHS_TAG_NULL );
	dhsMsgFmt( DHS_E_CONNECT_REPLY, DHS_DEBUG_OFF, sysInfo->TaskID.Machine, 
		sysInfo->TaskID.Pid );
	dhsMsgCallback( dhsConnectId( connect ), DHS_TAG_NULL );
	s = DHS_S_SUCCESS;
	while ( ( connect = dhsConnectFind( DHS_CF_CONNECTING, &s ) ) != NULL ) 
	{
	    connect->dcState = DHS_CON_ERROR;
	    connectCallback( connect, pStatus );
	}
	fnReturn( VOID );
    }


    /*
     *  Make sure the IMP connection succeeded.
     */

    if ( sysInfo->SysMsgStat != 0 )
    {
	dhsMsgFmt( DHS_E_IMP_CONNECT, DHS_DEBUG_OFF, sysInfo->SysMsgStat,
		ImpErrorText( sysInfo->SysMsgStat ) );
	dhsMsgCallback( dhsConnectId( connect ), DHS_TAG_NULL );
	if ( connect != NULL )
	{
	    connect->dcState = DHS_CON_ERROR;
	    connectCallback( connect, pStatus );
	}
	fnReturn( VOID );
    }

    if ( connect != NULL )
    {
	/*
	 *  Mark the connection as being open but not accepted.
	 */

	connect->dcState = DHS_CON_NOT_ACCEPTED;
	connectCallback( connect, pStatus );
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * connectRequest
 *
 * INVOCATION:
 * connectRequest( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Called when a client requests a connection
 *
 * The response is to add the connection to the list of connections
 * and accept the connection.
 *
 * DESCRIPTION:
 * - Accept the imp connection.
 * - Execute the dhs error callback.
 * - Send a message back to the client indicating if the connection was
 *   accepted or not. The connection is not accepted if the connection
 *   is closed in the error callback routine.
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

static void	connectRequest
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* Response to client system.		*/
    tDhsConnect	*pConnect;	/* The connection being made.		*/
    IMP_ConnectInfo
    		*conInfo;	/* Imp connection info.			*/
    IMP_Status	impStatus;	/* Imp return status.			*/
    struct in_addr
    		ipStruct;	/* Ip address structure.		*/
    DHS_STATUS	s2;		/* Temporary function status.		*/
    char	*serverAddress;	/* Server address string.		*/
    IMP_ConnectInfo
    		tmpConInfo;	/* Imp connection info.			*/
    int		tmpConnect;

    fnEntry( "connectRequest" );
    statCheck( *pStatus, VOID );

    impStatus = IMP_STATUS_OK;


    /*
     *  Create a connect structure to represent the connection.
     */

    ipStruct = inet_makeaddr( sysInfo->TaskID.Machine, 0 );
    serverAddress = inet_ntoa( ipStruct );

    pConnect = dhsConnectNew( sysInfo->TaskName, serverAddress, 
	    NULL, TRUE, pStatus );
    dhsConnectRef( pConnect, pStatus );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	tmpConInfo.MessageBytes = 0;
	tmpConInfo.MaxMessages = 0;
	tmpConInfo.TaskID = sysInfo->TaskID;
	s2 = DHS_S_SUCCESS;
	impCheck( ImpAcceptConnect( dhsLocal.dlImpIdIn, msgInfo, &tmpConInfo, 
		0, IMP_FALSE, &tmpConnect, &impStatus ), impStatus, s2, VOID );
	fnReturn( VOID );
    }
    pConnect->dcOpenCount ++;

    s2 = DHS_S_SUCCESS;
    pConnect->dcState = DHS_CON_REQUESTED;
    conInfo = &( pConnect->dcConnectInfo );
    conInfo->MessageBytes = 0;
    conInfo->MaxMessages = 0;
    conInfo->TaskID = sysInfo->TaskID;


    /*
     *  Accept the connection from the IMP point of view.
     */

    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpAcceptConnect( dhsLocal.dlImpIdIn, msgInfo, conInfo, 
	    0 /*IMP_FLOW_CONTROL | IMP_NO_FULL_MSG*/, IMP_TRUE, 
	    &(pConnect->dcImpConnect), 
	    &impStatus ), &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    if ( impStatus != IMP_STATUS_OK )
    {
	dhsConnectDeref( pConnect, pStatus );
	dhsConnectDeref( pConnect, pStatus );
	dhsMsgCallback( DHS_CONNECT_NULL, DHS_TAG_NULL );
	fnReturn( VOID );
    }


    /*
     *  Execute the error callback to indicate someone is trying to connect.
     */

    dhsMsgFmt( DHS_S_CONNECT, DHS_DEBUG_OFF, conInfo->TaskName, 
	    conInfo->TaskID.Machine, pConnect->dcImpConnect );
    dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );


    if ( pConnect->dcState == DHS_CON_REFUSED )
    {
	/*
	 *  If the error callback refuses the connection, (by closing the
	 *  connection from the error callback), then send a message to the
	 *  client idicating the connection was refused.
	 */

	pConnect->dcState = DHS_CON_CONNECTED;
	pCmd = dhsCmdNew( NULL, DHS_AV_LIST_NULL, NULL, DHS_MSG_CON_REFUSE, 
		DHS_CF_NONE, DHS_TAG_NULL, DHS_CS_DONE, pStatus );
    }
    else
    {
	/* 
	 *  Send a message indicating the connection is accepted.
	 */

	pConnect->dcState = DHS_CON_CONNECTED;
	pCmd = dhsCmdNew( NULL, DHS_AV_LIST_NULL, NULL, DHS_MSG_CON_ACCEPT, 
		DHS_CF_NONE, dhsLocal.dlTimeout, DHS_CS_DONE, pStatus );
    }

    dhsConnectSend( pConnect, pCmd, pStatus );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	*pStatus = DHS_S_SUCCESS;
	impStatus = IMP_STATUS_OK;
	check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), 
		*pStatus, VOID );
	impCheckNoRet( ImpCloseConnect( dhsLocal.dlImpId, 
		pConnect->dcImpConnect, &impStatus ), 
		&( dhsLocal.dlImpMutex ), impStatus );
	pConnect->dcImpConnect = -1;
	pConnect->dcState = DHS_CON_LOST;
	*pStatus = DHS_S_SUCCESS;
	check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
		*pStatus, VOID );
	dhsConnectDeref( pConnect, pStatus );
    }
    dhsCmdDeref( pCmd, pStatus );

    if ( pConnect->dcState != DHS_CON_REFUSED )
    {
	connectCallback( pConnect, pStatus );
    }
    dhsConnectDeref( pConnect, pStatus );
    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * crash
 *
 * INVOCATION:
 * crash( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Called when the connection to a task is lost.
 *
 * The response should be to close all connections to the lost task.
 *
 * DESCRIPTION:
 * Call the dhsConnectLost function.
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

static void	crash
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsConnect	*pConnect;	/* The connection being handled.	*/

    fnEntry( "crash" );
    statCheck( *pStatus, VOID );


    pConnect = dhsConnectFind( DHS_CF_TASK_ID, pStatus, 
	    &(sysInfo->TaskID) );
    dhsConnectRef( pConnect, pStatus );
    connectLost( pConnect, pStatus );
    if ( dhsIsConnected( dhsConnectId( pConnect ), pStatus ) )
    {
	dhsConnectDeref( pConnect, pStatus );
    }
    if ( *pStatus == DHS_E_CON_NOT_FOUND )
    {
	*pStatus = DHS_S_SUCCESS;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * disconnect
 *
 * INVOCATION:
 * disconnect( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handles a connection close from the client.
 *
 * Response is to remove the connection from the list of connections.
 *
 * DESCRIPTION:
 * Description of the algorithm.
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

static void	disconnect
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    IMP_ConnectInfo
    		*conInfo;	/* Pointer to the imp connetion info.	*/


    fnEntry( "disconnect" );
    statCheck( *pStatus, VOID );


    /*
     *  If the connection isn't being dropped already by a user level message,
     *  delete the connection.
     */

    if ( pSender == NULL )
    {
	pSender = dhsConnectFind( DHS_CF_DISCONNECTING, pStatus, 
		&(sysInfo->TaskID) ) ;
	*pStatus = DHS_S_SUCCESS;
    }

    if ( pSender != NULL )
    {
	if ( ! pSender->dcClientConnect )
	{
	    if ( pSender->dcState == DHS_CON_CONNECTED )
	    {
		pSender->dcState = DHS_CON_DISCONNECTING;
		connectCallback( pSender, pStatus );
	    } 
	    else if ( pSender->dcState == DHS_CON_DISCONNECTING )
	    {
		pSender->dcState = DHS_CON_NOT_CONNECTED;
	    }
	}
	else
	{
	    if ( pSender->dcState != DHS_CON_DISCONNECTING &&
		    pSender->dcState != DHS_CON_NOT_CONNECTED )
	    {
		/*
		 *  execute the error callback.
		 */

		pSender->dcState = DHS_CON_DISCONNECTING;
		dhsCmdConnectLost( pSender, pStatus );
		pSender->dcState = DHS_CON_NOT_CONNECTED;
		connectCallback( pSender, pStatus );
		
		conInfo = &( pSender->dcConnectInfo );

		dhsMsgFmt( DHS_S_DISCONNECT, DHS_DEBUG_OFF, conInfo->TaskName, 
			conInfo->TaskID.Machine, pSender->dcImpConnect );
		dhsMsgCallback( dhsConnectId( pSender ), DHS_TAG_NULL );

		dhsConnectDeref( pSender, pStatus );
	    }
	}
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * locateRequest
 *
 * INVOCATION:
 * locateRequest( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle an IMP_LOCATE_REQ message
 *
 * If the locate request succeeded, the response is to attempt to
 * initiate a connection.
 *
 * DESCRIPTION:
 * - Ensure the connection is in the expected state.
 * - Attempt to complete the connection with the ImpConnect function.
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

static void	locateRequest
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsConnect	*pConnect;	/* The connection being handled.	*/
    tDhsConnect	*pDupConnect;	/* Duplicate connection pointer.	*/
    int		known;		/* Is the task known?			*/
    IMP_Status	impStatus;	/* Imp return status.			*/
    int		wait;		/* Will the connection have to be	*/
    				/* completed via messages?		*/
    IMP_ConnectInfo
    		*conInfo;
    DHS_STATUS	s;


    fnEntry( "locateRequest" );
    statCheck( *pStatus, VOID );


    /*
     *  Try to locate connection based on the imp connection id sent in 
     *  the imp message tag.
     */

    s = DHS_S_SUCCESS;
    pConnect = dhsConnectFind( DHS_CF_CON_ID, &s, msgInfo->Tag ) ;
    dhsConnectRef( pConnect, &s );
    if ( s != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CON_NOT_FOUND, DHS_DEBUG_OFF );
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	*pStatus = s;
	fnReturn( VOID );
    }


    /* 
     *  Make sure the connection is in an appropriate state for being
     *  connected.
     */

    if ( pConnect->dcState != DHS_CON_NOT_CONNECTED )
    {
	dhsMsgFmt( DHS_E_LOCATE_MSG, DHS_DEBUG_OFF );
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	*pStatus = DHS_E_LOCATE_MSG;
	dhsConnectDeref( pConnect, &s );
	fnReturn( VOID );
    }


    /*
     *  Make sure the IMP locate succeeded.
     */

    if ( sysInfo->SysMsgStat != 0 )
    {
	pConnect->dcState = DHS_CON_ERROR;
	dhsMsgFmt( DHS_E_LOCATE, DHS_DEBUG_OFF, sysInfo->SysMsgStat );
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	connectCallback( pConnect, pStatus );
	dhsConnectDeref( pConnect, &s );
	fnReturn( VOID );
    }


    /* 
     *  Check to see if this is an attempt to open a duplicate connection.
     */

    pDupConnect = dhsConnectFind( DHS_CF_TASK_ID, &s, &(sysInfo->TaskID) );
    dhsConnectRef( pDupConnect, &s );
    if ( s == DHS_E_CON_NOT_FOUND || pDupConnect == pConnect )
    {
	/*
	 *  This is not a duplicate connection, proceed with the connection
	 *  process.
	 */

	pConnect->dcState = DHS_CON_LOCATED;
	connectCallback( pConnect, pStatus );
	conInfo = &( pConnect->dcConnectInfo );
	conInfo->TaskID = sysInfo->TaskID;
	dhsConnectDeref( pDupConnect, &s );
    }
    else
    {
	/* 
	 *  This is a duplicate connection, do not continue to connect.
	 */

	pConnect->dcDupConnect = pDupConnect;
	pDupConnect->dcOpenCount++;
	pConnect->dcState = DHS_CON_DUPLICATE;
	connectCallback( pConnect, pStatus );


	conInfo = &( pConnect->dcConnectInfo );
	conInfo->TaskID = sysInfo->TaskID;
	dhsConnectDeref( pDupConnect, &s );
	s = DHS_S_SUCCESS;
	dhsConnectDeref( pConnect, &s );
	fnReturn( VOID );
    }


    /*
     *  Try to complete the connection.
     */

    impStatus = IMP_STATUS_OK;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpConnect( dhsLocal.dlImpIdIn, sysInfo->TaskName, 
	    &(pConnect->dcConnectInfo), 
	    IMP_TWO_WAY /* | IMP_FLOW_CONTROL | IMP_NO_FULL_MSG*/, 
	    &(pConnect->dcImpConnect), &known, &wait, &impStatus ), 
	    &( dhsLocal.dlImpMutex ), impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    if ( impStatus != IMP_STATUS_OK )
    {
	pConnect->dcState = DHS_CON_ERROR;
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	s = DHS_S_SUCCESS;
	dhsConnectDeref( pConnect, &s );
	*pStatus = DHS_E_IMP;
	fnReturn( VOID );
    }
    

    /*
     *  If the task wasn't known, set the connection as being in error and
     *  return.
     */

    if ( ! known )
    {
	pConnect->dcState = DHS_CON_ERROR;
	dhsMsgFmt( DHS_E_TASK_UNKNOWN, DHS_DEBUG_OFF, sysInfo->TaskName );
	dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	connectCallback( pConnect, pStatus );
	s = DHS_S_SUCCESS;
	dhsConnectDeref( pConnect, &s );
	fnReturn( VOID );
    }


    if ( ! wait )
    {
	/*
	 *  Connection was completed immediately.
	 */

	pConnect->dcState = DHS_CON_NOT_ACCEPTED;
	connectCallback( pConnect, pStatus );
    }

    s = DHS_S_SUCCESS;
    dhsConnectDeref( pConnect, &s );


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * machineLost
 *
 * INVOCATION:
 * machineLost( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a IMP_SYS_MACHINE_LOST message
 *
 * DESCRIPTION:
 * This function calls connectLost for every connection to a task on the
 * machine.
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

static void	machineLost
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		connections[IMP_MAX_TASKS];
    				/* The list of lost connections.	*/
    int		i;
    IMP_Status	impStatus;
    int		numLost;	/* The number of lost connections.	*/
    char	taskNames[IMP_MAX_TASKS][IMP_MAX_NAME_LEN+1];
    tDhsConnect	*pConnect;	/* The lost connection.			*/


    fnEntry( "machineLost" );
    statCheck( *pStatus, VOID );


    impStatus = IMP_STATUS_OK;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    impCheckNoRet( ImpLostMachine( dhsLocal.dlImpIdIn, &(sysInfo->TaskID), 
	    IMP_MAX_TASKS, &numLost, connections, (char *) taskNames, 
	    IMP_MAX_NAME_LEN + 1, &impStatus ), &( dhsLocal.dlImpMutex ), 
	    impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    for ( i = 0; i < numLost; i++ )
    {
	pConnect = dhsConnectFind( DHS_CF_IMP_ID, pStatus, connections[i] );
	dhsConnectRef( pConnect, pStatus );
	connectLost( pConnect, pStatus );
	if ( dhsIsConnected( dhsConnectId( pConnect ), pStatus ) )
	{
	    dhsConnectDeref( pConnect, pStatus );
	}
	if ( *pStatus == DHS_E_CON_NOT_FOUND )
	{
	    *pStatus = DHS_S_SUCCESS;
	}
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * notify
 *
 * INVOCATION:
 * notify( &msgInfo, &sysInfo, pSender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) psender (tDhsConnect *) The connection sending the message.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle a IMP_SYS_NOTIFY message
 *
 * DESCRIPTION:
 * Set the notified flag for the connection.
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

static void	notify
(
    IMP_MsgInfo	*msgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*sysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "notify" );

    if ( pSender != NULL )
    {
	pSender->dcNotified = TRUE;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sendErr
 *
 * INVOCATION:
 * connectRequest( &msgInfo, &sysInfo, &sender, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) msgInfo (IMP_MsgInfo *) The IMP msginfo structure.
 * (>) sysInfo (IMP_SysInfo) The IMP sysinfo structure.
 * (>) pSender (tDhsConnect) The connection the message came on.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Function called when a send fails.
 *
 * The response is to retry the transmission.
 *
 * ???? this function should be changed to figure out why the message
 * failed and take appropriate action: i.e. if there is no space in
 * the receivers buffers, wait for space to become available.
 *
 * DESCRIPTION:
 * Description of the algorithm.
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

static void	sendErr
(
    IMP_MsgInfo	*pMsgInfo,	/* (in)  The IMP msginfo structure.	*/
    IMP_SysInfo	*pSysInfo,	/* (in)  The IMP sysinfo structure.	*/
    tDhsConnect	*pSender,	/* (in)  Connection the message came from.*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* Pointer to the command.		*/
    tDhsConnect	*pConnect;	/* Pointer to the connection.		*/
    DHS_STATUS	s;		/* Temporary return status.		*/
    fnEntry( "sendErr" );


printf ("IMP sendErr message received\n" );
    /*
     *  Check to see if the tag is a connection id.
     */

    s = DHS_S_SUCCESS;
    pConnect = dhsConnectFind( DHS_CF_CON_ID, &s, (DHS_CONNECT) pSysInfo->Tag );
    

    if ( s == DHS_S_SUCCESS )
    {
	pConnect->dcState = DHS_CON_ERROR;
	connectCallback( pConnect, pStatus );
    }
    else if ( s == DHS_E_CON_NOT_FOUND )
    {
	s = DHS_S_SUCCESS;
	pCmd = dhsCmdFind( DHS_CF_PTR, &s, 
		dhsCmdPtr( (DHS_TAG) pSysInfo->Tag ) );
	if ( s == DHS_S_SUCCESS )
	{
	    /*
	     *  Re-send the command.
	     */

	    dhsConnectSend( pCmd->dcSentTo, pCmd, pStatus );
	}
	else
	{
	    dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, 
		    (DHS_TAG) pSysInfo->Tag );
	    dhsMsgCallback( dhsConnectId( pConnect ), DHS_TAG_NULL );
	}
    }
    else
    {
	*pStatus = s;
    }


    fnReturn( VOID );
}
