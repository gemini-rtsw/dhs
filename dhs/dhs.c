static char rcsid[] = "$Id: dhs.c,v 1.1.1.1 2002-11-24 20:20:23 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1996				(c) 1996.
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
 * dhs/src/dhs.c
 *
 * PURPOSE:
 * Contains DHS interface routines common to all modes of use of the
 * library.
 *
 * FUNCTION NAME(S)
 * dhsExit - Clean up the DHS library.
 * dhsInit - Initialize the DHS library.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.26  1998/08/04 16:14:42  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.25  1998/03/13 21:34:04  nhill
 * Removed the IMP_PTR_READ flag from the calls to ImpRegister.
 *
 * Revision 1.24  1998/03/05 23:33:21  nhill
 * Changed to prevent usage of freed connect structures while
 * connections are being closed.
 *
 * Improved handling of event loop shutdown.
 *
 * Revision 1.23  1998/02/24 01:03:55  nhill
 * Bug fixes.
 *
 * Revision 1.22  1998/02/16 23:40:22  nhill
 * Changed to use dhsConnectUnlock to unlock connection mutex.
 *
 * Revision 1.21  1998/01/20 18:21:47  nhill
 * Added the connection callback.
 *
 * Revision 1.20  1997/12/24 19:29:53  nhill
 * Now setting the lockout variable from the dhsInit and dhsExit functions.
 *
 * Revision 1.19  1997/12/18 21:48:47  nhill
 * Added a call to dhsConnectReqClose to function dhsExit.
 *
 * Revision 1.18  1997/09/17 20:32:49  nhill
 * Added saving of the imp name.
 *
 * Revision 1.17  1997/08/27 22:02:24  nhill
 * Added some error messages when ImpRegister fails.
 *
 * Revision 1.16  1997/08/06 16:16:50  nhill
 * Added an ers error handler.
 *
 * Revision 1.15  1997/07/02 20:11:36  greimel
 * typecasts for IMP call
 *
 * Revision 1.14  1997/06/04 18:00:07  nhill
 * Changed dhsInit to allow number of connections in addition to buffer size.
 *
 * Revision 1.13  1997/02/24 19:17:17  nhill
 * Checkin for install.
 *
 * Revision 1.12  1997/02/13 21:59:13  nhill
 * Checkin for install.
 *
 * Revision 1.11  1997/02/04 19:41:34  nhill
 * Checkpoint for install.
 *
 * Revision 1.10  1997/01/30 17:18:54  nhill
 * Checkin for install.
 *
 * Revision 1.9  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.8  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.6  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.5  1997/01/20 19:01:42  nhill
 * Checking for install.
 *
 * Revision 1.4  1997/01/20 17:15:11  nhill
 * Checkpoint.
 *
 * Revision 1.3  1997/01/13 17:58:34  nhill
 * Reasonably stable checkpoint.
 *
 * Revision 1.2  1997/01/06 17:24:10  nhill
 * *** empty log message ***
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
 */


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>


/*
 ************************************************************************
 *  DRAMA ERS system include files.
 ************************************************************************
 */

#include "Ers.h"
#include "Ers_Err.h"


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"


/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsExit
 *
 * INVOCATION:
 * dhsExit( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Clean up the DHS library before exiting
 *
 * After this function is executed, no other DHS library function should
 * be executed until dhsInit is executed again.
 *
 * DESCRIPTION:
 * Make sure the event loop is stopped, clean up the imp connections, clean up
 * an thread specific data, and then clean up the dhsLibrary local data 
 * structure.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpMutex: IMP function mutex.
 * - dhsLocal.dlImpId = IMP task id.
 *
 * PRIOR REQUIREMENTS:
 * The library should have been initialized with the dhsInit function.
 *
 * SEE ALSO:
 * - dhsInit
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsExit
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tEventLoop	el;		/* Event loop structure to allow access	*/
    				/* to the event loop mutex.		*/
    StatusType	ersStatus;	/* ERS function return status.		*/
    IMP_Status	impStatus;	/* IMP function return status.		*/


    fnEntry( "dhsExit" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    dhsLocal.dlLockout = TRUE;


    /*
     *  Request all client connections to close.
     */

    dhsServerExit( pStatus );


    /*
     *  Close all server connections.
     */

    dhsConnectCloseAll( pStatus );


    /* 
     *  Stop the event loop if its running.
     */

    dhsEventLoopEnd( pStatus );


    /*
     *  Wait until the event loop exits.  The event loop structure "el" is
     *  only necessary to give the dhsElStopped function access to the 
     *  static value that indicates if the event loop is running.
     */

    dhsEl( &el, NULL, NULL, FALSE, FALSE, pStatus );
    dhsEventLoop( DHS_ELT_COND, dhsElStopped, &el, pStatus );


    /*
     *  Shut down the IMP system.
     */

    impStatus = IMP_STATUS_OK;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    ImpDetach( dhsLocal.dlImpId, &impStatus );
    /* ImpDetach( dhsLocal.dlImpIdIn, &impStatus ); */
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );


    /*
     *  Shutdown the ERS system.
     */

    ersStatus = ERS__OK;
    ErsStop( &ersStatus );


    /*
     * Clean up the thread specific data.
     */

    dhsTsdExit( pStatus );


    /*
     *  Clean up the dhs library global data shared by all threads.
     */

    dhsLocalExit( pStatus );


    /*
     *  Indicate the dhs library is no longer initalized.
     */

    dhsInitSet( FALSE );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsInit
 *
 * INVOCATION:
 * dhsInit( name, numConnect, *status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) name (char *) Name to give to this IMP task.
 * (>) numConnect (int) Number of connections, or size of the IMP buffers 
 *			to allocate to this task.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Initialize the dhs library
 *
 * DESCRIPTION:
 * Intialize the global data structure, initialize the thread specific data,
 * setup the DRAMA ERS system and DRAMA IMP system.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpMutex: IMP function mutex.
 * - dhsLocal.dlImpId = IMP task id.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsExit
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsInit
(
    const char	*name,		/* (in)  Name of the server.		*/
    const int	numConnect,	/* (in)  Number of connections, or size */
    				/*	 of the IMP buffers to allocate */
				/*	 to this task.			*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    unsigned long
    		buffSize;	/* Size of the IMP buffers.		*/
    StatusType	ersStatus;	/* DRAMA ERS library return status.	*/
    IMP_Status	impStatus;	/* IMP function return status.		*/
    IMP_MsgInfo	msgInfo;	/* Imp message info structure.		*/
    char	name2[IMP_MAX_NAME_LEN+1];


    fnEntry( "dhsInit" );
    statCheck( *pStatus, VOID );


    /*
     *  If the library is already initialized, just return success.
     */

    if ( dhsInitTest() )
    {
	fnReturn( VOID );
    }


    /* 
     *  Initialize the local global structure.
     */

    dhsLocalInit( pStatus );
    strcpy( dhsLocal.dlImpName, name );


    /* 
     *  Initialize the thread specific data structure.
     */

    dhsTsdInit( pStatus );


    /*
     *  Initialize the DRAMA ERS library.
     */

    ersStatus = ERS__OK;
    ErsStart( dhsErsHandler, NULL, 0, NULL, &ersStatus );
    if ( ersStatus != ERS__OK )
    {
	/*
	 *  Failed to start error handling system.
	 */

	*pStatus = DHS_E_ERS;
	dhsMsgFmt( DHS_E_ERS, DHS_DEBUG_OFF, ersStatus );

	fnReturn( VOID );
    }


    /*
     *  Initialize the IMP system.
     */

    if ( numConnect < 1024 )
    {
	buffSize = numConnect * DHS_BUFF_PER_CONNECT;
    }
    else
    {
	buffSize = numConnect;
    }
    dhsLocal.dlBuffSize = MAX( buffSize, 
	    MAX_MESSAGE_LENGTH * MAX_MESSAGES * 2 + 1024 );
    impStatus = IMP_STATUS_OK;
    check( dhsMutexLock( &( dhsLocal.dlImpMutex ), pStatus ), *pStatus, VOID );
    ImpRegister( (char *) name, (int) dhsLocal.dlBuffSize, (long) 0, 
	    &dhsLocal.dlImpId, &impStatus );
    check( dhsMutexUnlock( &( dhsLocal.dlImpMutex ), pStatus ), 
	    *pStatus, VOID );
    sprintf( name2, "%sI", name );

    /*
    ImpRegister( name2, dhsLocal.dlBuffSize, 0, 
	    &dhsLocal.dlImpIdIn, &impStatus );
    */
    dhsLocal.dlImpIdIn = dhsLocal.dlImpId;
    if ( impStatus != IMP_STATUS_OK )
    {
	/*
	 *  Failed to register with IMP. 
	 */

	*pStatus = DHS_E_IMP_REGISTER;
	dhsMsgFmt( DHS_E_IMP_REGISTER, DHS_DEBUG_OFF, name );
	impCheckRet( ImpRegister( (char *) name, (int) dhsLocal.dlBuffSize, 
		(long) 0, &dhsLocal.dlImpId, &impStatus ), 
		impStatus, *pStatus, VOID )
    }


    /*
     *  Set up a heartbeat message.
     */

    msgInfo.Address = "thump";
    msgInfo.MessageLength = strlen( (char *) msgInfo.Address );
    msgInfo.Tag = DHS_MSG_HEART_BEAT;
    msgInfo.Type = DHS_MSG_HEART_BEAT;
    ImpDeltaTime( HEART_RATE, 0, &msgInfo.DeltaTime );


    /*
     *  Issue an ImpQueueReminder message to send the message to this task.
     *  The function is called in a mutex to ensure no two threads ever
     *  use the same IMP process id at the same time.
     */

#ifdef OMIT
    impCheck( ImpQueueReminder( dhsLocal.dlImpIdIn, &msgInfo, 
	    IMP_STATIC | IMP_REPEAT, &impStatus ), impStatus, *pStatus, VOID );
#endif
    
    if ( *pStatus == DHS_S_SUCCESS )
    {
	dhsLocal.dlLockout = FALSE;
	dhsInitSet( TRUE );
    }

    fnReturn( VOID );
}
