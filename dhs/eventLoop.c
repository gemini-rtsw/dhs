static char rcsid[] = "$Id: eventLoop.c,v 1.1.1.1 2002-11-24 20:20:30 brighton Exp $";
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
 * dhs/src/eventLoop.c
 *
 * PURPOSE:
 * Contains routines to run the DHS library event loop.
 *
 * FUNCTION NAME(S)
 * dhsEl - Initialize an event loop structure.
 * dhsElNoExit - Returns true if the dhsLocal.dlExitCount is zero.
 * dhsElStart - The DHS library event loop.
 * dhsElStartThread - Function to be executed when a new event loop thread
 *			is created.
 * dhsElStopped - Returns TRUE if the event loop is not running.
 * dhsEventLoop - Start the DHS event loop.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.22  2001/03/27 20:29:17  nhill
 * Added vxWorks.h when compiling for vxworks.
 *
 * Revision 1.21  1998/08/04 16:15:26  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.20  1998/04/23 22:19:53  nhill
 * Bug fixes.
 *
 * Revision 1.19  1998/03/13 21:34:46  nhill
 * Fixed a hole in the locks when the event loop exits.
 *
 * Revision 1.18  1998/03/05 23:34:35  nhill
 * Changed to prevent usage of freed connect structures while
 * connections are being closed.
 *
 * Revision 1.17  1998/02/24 01:07:54  nhill
 * Added function dhsElNoExit.
 * Bug fixes.
 *
 * Revision 1.16  1998/01/09 18:58:43  nhill
 * Changed ImpReadPtr to ImpRead to avoid concurent read problems
 * when reading two messages from the same task.
 *
 * Revision 1.15  1997/09/17 20:33:56  nhill
 * Added some mutex protection around imp functions.
 *
 * Revision 1.14  1997/08/25 23:08:05  nhill
 * Added a call to dhsMsgCallback to ensure any messages produced
 * while processing a message are printed.
 *
 * Revision 1.13  1997/08/19 19:45:19  nhill
 * Changed impCheckNoRet to take a mutex to be unlocked if an error occurs.
 *
 * Revision 1.12  1997/06/09 21:16:52  nhill
 * Changed to use pthread_equal to compare thread ids.
 *
 * Revision 1.11  1997/05/13 17:08:40  nhill
 * Added the dhsElBroadcast function.
 *
 * Revision 1.10  1997/04/15 19:48:26  nhill
 * Changed to allow a thread running the event loop to re-enter the event
 * loop from the event handler routine.
 *
 * Revision 1.9  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
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
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


#ifdef vxWorks
#include <vxWorks.h> /* must come before stdarg.h */
#endif

/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"
static boolean	elExit = FALSE;

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsEl
 *
 * INVOCATION:
 * dhsEl( &el, condFn, pCondArg, TRUE, TRUE, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pEl (tEventLoop *) Event loop structure to initialize.
 * (>) condFn (DHS_COND_FN) The condition function for the event loop
 *			(possibly NULL).
 * (>) pCondArg (void *) Argument to the condition function (possibly NULL).
 * (>) noWait (boolean) Return when no more events to process?
 * (>) thread (boolean) Run the event loop in a new thread?
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Initialize an event loop structure
 *
 * DESCRIPTION:
 * Initialize all fields of the event loop structure.
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

void		dhsEl
(
    tEventLoop	*pEl,		/* (mod) The event loop to initialize.	*/
    DHS_COND_FN	condFn,		/* (in)  Event loop condition function.	*/
    void	*pCondArg,	/* (in)  Condition function argument.	*/
    boolean	noWait,		/* (in)  Don't wait in the event loop.	*/
    boolean	thread,		/* (in)  Event loop in separate thread.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    static tDhsCond		/* Command condition variable.		*/
		cond;
    static boolean		/* Is the static data intiailzied?	*/
		staticInit = FALSE;
    static tDhsMutex		/* Mutex to make sure there is only one	*/
    		mutex;		/* event loop running.			*/
    static DHS_THREAD
    		threadId;	/* Event loop thread id.		*/
    static int	refCount = 0;	/* Number of rentrant event loops running.*/


    fnEntry( "dhsEl" );
    statCheck( *pStatus, VOID );


    /*
     *  Initialize the static items if necessary;
     */

    if ( ! staticInit )
    {
	check( dhsCondInit( &cond, pStatus ), *pStatus, VOID );
	check( dhsMutexInit( &mutex, pStatus ), *pStatus, VOID );
	staticInit = TRUE;
	memset( &threadId, 0, sizeof( DHS_THREAD ) );
    }


    /*
     *  Initialize the structure. Pointers to the static items are put
     *  into the structure to the emulate C++ class static members.
     */

    pEl->elCond = &cond;
    pEl->elCondFn = condFn;
    pEl->elCondArg = pCondArg;
    pEl->elMutex = &mutex;
    pEl->elNoWait = noWait;
    pEl->elRefCount = &refCount;
    pEl->elThread = thread;
    pEl->elThreadId = &threadId;


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsElBroadcast
 *
 * INVOCATION:
 * dhsElBroadCast( &status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Broadcast to the event loop mutex
 *
 * This function can be used by an application callback function to cause
 * any threads waiting on DHS library event to check to see if the event
 * is complete.
 *
 * DESCRIPTION:
 * Trivial.
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

void	dhsElBroadcast
(
    DHS_STATUS	*pStatus	/* (mod) Function return value.		*/
)
{
    IMP_Status	impStatus;	/* IMP function return status.		*/
    IMP_MsgInfo	msgInfo;	/* Imp message info structure.		*/
    tEventLoop	el;


    fnEntry( "dhsElBroadcast" );
    statCheck( *pStatus, VOID );


    dhsEl( &el, NULL, NULL, TRUE, TRUE, pStatus );
    check( dhsCondBroadcast( el.elCond, pStatus ), *pStatus, VOID );


    if ( ! pthread_equal( *( el.elThreadId ), dhsThreadSelf() ) )
    {
	/*
	 *  Intialize the imp status.
	 */

	impStatus = IMP_STATUS_OK;


	/*
	 *  Intialize the msgInfo structure to send a DHS_MSG_EL_EXIT message.  
	 *  The value of msgInfo.Address isn't important, but IMP gets very 
	 *  upset if it is NULL.
	 */

	msgInfo.Address = "void";
	msgInfo.MessageLength = strlen( (char *) msgInfo.Address );
	msgInfo.Tag = DHS_MSG_VOID;
	msgInfo.Type = DHS_MSG_VOID;
	ImpDeltaTime( 0, 0, &msgInfo.DeltaTime );


	/*
	 *  Issue an ImpQueueReminder message to send the message to this task.
	 *  The function is called in a mutex to ensure no two threads ever
	 *  use the same IMP process id at the same time.
	 */

	impCheck( ImpQueueReminder( dhsLocal.dlImpIdIn, &msgInfo, IMP_STATIC, 
		&impStatus ), impStatus, *pStatus, VOID );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsElNoExit
 *
 * INVOCATION:
 * answer = dhsElNoExit( el );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) el (tEventLoop *) Event loop pointer.
 *
 * FUNCTION VALUE:
 * (boolean) True if dhsLocal.dlExitcount is zero.
 *
 * PURPOSE:
 * Condtion function for the event loop
 *
 * This function returns true when all outstanding exit messages have
 * been processed.
 *
 * DESCRIPTION:
 * Trivial
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

boolean		dhsElNoExit
(
    tEventLoop	*el		/* (in)  The event loop object.		*/
)
{
    boolean	response;
    DHS_STATUS	s;

    s = DHS_S_SUCCESS;
    dhsMutexLock( &(dhsLocal.dlImpMutex), &s );
    response = ( dhsLocal.dlExitCount == 0 );
    dhsMutexUnlock( &(dhsLocal.dlImpMutex), &s );

    return( response );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsElStart
 *
 * INVOCATION:
 * dhsElStart( &el, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pEl (tEventLoop *) Structure describing how to run the event loop.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Process IMP events for the DHS library
 *
 * DESCRIPTION:
 * Make sure no other thread is running the event loop, and then loop
 * processing IMP events as they are received. Exit conditions for the
 * event loop depend on the contents of the event loop structure.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpId IMP task id.
 *
 * PRIOR REQUIREMENTS:
 * The event loop structure in pEl should be initialized with the dhsEl
 * function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsElStart
(
    tEventLoop	*eventLoop,	/* (in)  The event loop structure.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return value.		*/
)
{
    IMP_TaskID	*fromTask;	/* Task id of the client.		*/
    IMP_Status	impStatus;	/* Imp Return status.			*/
    IMP_Status	impStatus2;	/* Imp Return status.			*/
    IMP_MsgInfo	msgInfo;	/* Imp message info structure.		*/
    DHS_STATUS	s;		/* Temporary status value.		*/
    tDhsConnect	*sender;	/* Connection the message came from.	*/
    char	msgBuffer[MAX_MESSAGE_LENGTH];
    				/* Buffer to contain the message.	*/


    fnEntry( "dhsElStart" );
    statCheck( *pStatus, VOID );



    /*
     *  Attempt to lock the event loop mutex.
     */

    if ( ! dhsMutexTry( eventLoop->elMutex, pStatus ) || 
	    *pStatus != DHS_S_SUCCESS )
    {
	/* 
	 * The lock failed, another thread must be running the event loop.
	 */

	if ( eventLoop->elThread )
	{
	    /*
	     *  This allows the parent thread to proceed when the event
	     *  loop is started in a thread.
	     */

	    check( dhsCondBroadcast( eventLoop->elCond, pStatus ), *pStatus, 
		    VOID );
	}

	if ( ! pthread_equal( *( eventLoop->elThreadId ), dhsThreadSelf() ) )
	{
	    *pStatus = DHS_E_EL_RUNNING;
	    fnReturn( VOID );
	}
    }

    if ( (*( eventLoop->elRefCount ) ) == 0 )
    {
	elExit = FALSE;
    }


    /*
     *  Set the thread id of the thread running the event loop.
     */

    *( eventLoop->elThreadId ) = dhsThreadSelf();
    (*( eventLoop->elRefCount ))++;


    if ( eventLoop->elThread )
    {
	/*
	 *  This allows the parent thread to proceed when the event
	 *  loop is started in a thread.
	 */

	check( dhsCondBroadcast( eventLoop->elCond, pStatus ), *pStatus, VOID );
    }
    else
    {
	/*
	 *  Unlock the condition variable that should have been locked
	 *  before this function was called.
	 */

	check( dhsCondUnlock( eventLoop->elCond, pStatus ), *pStatus, VOID );
    }


    /*
     *  Start the event loop.
     */

    for ( s = DHS_S_SUCCESS; s == DHS_S_SUCCESS && ! elExit; )
    {
	/*
	 *  If an exit condition is set, check to see if the exit
	 *  condition is met.
	 */

	if ( eventLoop->elCondFn != NULL && 
		eventLoop->elCondFn( eventLoop->elCondArg ) )
	{
	    break;
	}


	impStatus = IMP_STATUS_OK;
	msgInfo.BufferLength = MAX_MESSAGE_LENGTH;
	msgInfo.Address = msgBuffer;
	if ( eventLoop->elNoWait )
	{
	    /*
	     *  Read without blocking.
	     */

	    impCheckNoRet( ImpRead( dhsLocal.dlImpIdIn, &msgInfo, 
		    IMP_NOWAIT, &impStatus ), 
		    NULL, impStatus );
	}
	else
	{
	    /*
	     *  Do a blocking read.
	     */

	    impCheckNoRet( ImpRead( dhsLocal.dlImpIdIn, &msgInfo, 
		    0, &impStatus ), NULL, impStatus );
	}


	/*
	 *  Check for IMP errors.
	 */

	if ( impStatus != IMP_STATUS_OK )
	{
	    dhsMsgCallback( DHS_CONNECT_NULL, DHS_TAG_NULL );
	    *pStatus = DHS_E_IMP;
	    fnReturn( VOID );
	}


	/*
	 *  Check to see if a message was read.
	 */

	if ( ! msgInfo.ReadFlag )
	{
	    /*
	     *  No message was read.
	     */

	    if ( eventLoop->elNoWait )
	    {
		/*
		 *  If the event loop is supposed to return when there
		 *  are no more events, leave the loop now.
		 */

		break;
	    }
	    else
	    {
		/*
		 *  No message was read, so skip the message processing.
		 */

		continue;
	    }
	}


	/*
	 *  Display a message indicating the IMP message was received.
	 */

	dhsMsgFmt( DHS_S_IMP_EVENT, DHS_DEBUG_ON, msgInfo.Type );


	/*
	 *  Get the connection structure of the connection that sent the
	 *  message.
	 */

	fromTask = ImpFromTask( &msgInfo );
	sender = dhsConnectFind( DHS_CF_TASK_ID, &s, fromTask );
	dhsConnectRef( sender, &s );
	if ( s == DHS_S_SUCCESS )
	{
	    impStatus2 = IMP_STATUS_OK;
	    dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus );
	    ImpTimeNow( &(sender->dcLastUsed), &impStatus2 );
	    dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus );
	}
	else if ( s == DHS_E_CON_NOT_FOUND )
	{
	    /*
	     *  Ignore not found return status's since some messages aren't
	     *  associated with connections.
	     */

	    s = DHS_S_SUCCESS;
	    sender = NULL;
	}
	else 
	{
	    /*
	     *  If any other error occured, display the message using
	     *  the error callback.
	     */

	    *pStatus = s;
	    sender = NULL;
	    dhsMsgCallback( DHS_CONNECT_NULL, DHS_TAG_NULL );
	}


	if ( msgInfo.Type < 0 )
	{
	    /*
	     *  Process an IMP system message.
	     */

	    dhsSystemEvent( &msgInfo, sender, &s );
	}
	else
	{
	    /*
	     *  Process an IMP user message.
	     */

	    dhsUserEvent( msgInfo.Tag, (tDhsUsrMsg) msgInfo.Type, 
		    msgInfo.Address, msgInfo.MessageLength, sender, &s );
	}

	if ( sender != NULL )
	{
	    dhsConnectDeref( sender, &s );
	}
	dhsMsgCallback( DHS_CONNECT_NULL, DHS_TAG_NULL );


	/*
	 *  Broadcast to the condition variable so any waiting threads will 
	 *  check their condition functions.
	 */

	check( dhsCondBroadcast( eventLoop->elCond, pStatus ), *pStatus, VOID );

    }
    

    check( dhsCondLock( eventLoop->elCond, pStatus ), *pStatus, VOID );
    (*( eventLoop->elRefCount ))--;


    if ( ( s == DHS_S_EL_EXIT || elExit ) && *( eventLoop->elRefCount ) == 0 )
    {
	dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus )
	if ( dhsLocal.dlExitCount > 0 )
	{
	    dhsLocal.dlExitCount --;
	}
	dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus )
    }


    if ( s == DHS_S_EL_EXIT )
    {
	elExit = TRUE;
    }


    /*
     * Unlock the event loop mutex so another thread can run the event
     * loop.
     */

    if ( *( eventLoop->elRefCount ) == 0 )
    {
	check( dhsMutexUnlock( eventLoop->elMutex, pStatus ), 
		*pStatus, VOID );
    }

    check( dhsCondUnlock( eventLoop->elCond, pStatus ), *pStatus, VOID );


    /*
     *  Broadcast an exit event so any waiting threads can start their
     *  own event loops if they want to.
     */

    check( dhsCondBroadcast( eventLoop->elCond, pStatus ), *pStatus, VOID );


    if ( s != DHS_S_EL_EXIT )
    {
	*pStatus = s;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsElStartThread
 *
 * INVOCATION:
 * dhsThread( dhsElStartThread, &el, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pEl (tEventLoop *) Structure describing the event loop.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Function to run the event loop in a separate thread
 *
 * This function is intended to be passed as an argument to the dhsThread
 * function.
 *
 * DESCRIPTION:
 * Copy the event loop to local memory so it won't be affected when the
 * the memory in the parent thread is reused.
 *
 * Note that the parent thread must wait until the event loop starts,
 * or it is possible for the memory to be reused before it is copied.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The event loop condition variable should have been locked by the 
 * parent thread.
 * The event loop structure should have been initialized with the dhsEl
 * function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	*dhsElStartThread
(
    tEventLoop	*pEl		/* (in)  Event loop structure.		*/
)
{
    DHS_STATUS	status;		/* Event loop return status.		*/
    tEventLoop	el;		/* Local copy of the event loop structure.*/


    fnEntry( "dhsElStartThread" );


    /*
     * Copy the event loop structure.
     */

    el = *pEl;

    /*
     *  Try to run the event loop.
     */

    status = DHS_S_SUCCESS;
    dhsElStart( &el, &status );


    if ( status != DHS_S_SUCCESS )
    {
	dhsMsgCallback( DHS_CONNECT_NULL, DHS_TAG_NULL );
    }


    /*
     *  This should be terminating the thread if the function is used
     *  normally.
     */

    fnReturn( NULL );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsElStopped
 *
 * INVOCATION:
 * isStopped = dhsElStopped( &el );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pEl (tEventLoop *) Structure describing the event loop.
 *
 * FUNCTION VALUE:
 * (boolean) Return TRUE if the event loop is stopped.
 *
 * PURPOSE:
 * Check to see if another thread is running the event loop
 *
 * This function is meant to be used as a condition function passed to 
 * the dhsEventLoop function. 
 *
 * DESCRIPTION:
 * Attempt to lock the event loop mutex. If the attempt succeeds, the 
 * event loop cannot be running.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The event loop structure must be initialized with the dhsEl function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean		dhsElStopped
(
    tEventLoop	*pEl		/* (in)  Event loop structure.		*/
)
{
    DHS_STATUS	status;		/* Function return status.		*/
    boolean	stopped;	/* True when the event loop is stopped.	*/


    fnEntry( "dhsElStopped" );


    /*
     *  Attempt to lock the mutex.
     */

    status = DHS_S_SUCCESS;
    stopped = dhsMutexTry( pEl->elMutex, &status );
    if ( stopped )
    {
	/*
	 *  The lock succeeded, so unlock the mutex.
	 */

	dhsMutexUnlock( pEl->elMutex, &status );
    }


    /*
     *  if the current thread is running the event loop, indicate
     *  the event loop is stopped.
     */

    if ( ! stopped && 
	dhsThreadEqual( * ( pEl->elThreadId ), dhsThreadSelf() ) )
    {
	stopped = TRUE;
    }

    fnReturn( stopped );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsEventLoop
 *
 * INVOCATION:
 * dhsEventLoop( DHS_ELT_COND, condFn, pCondArg, &status );
 * dhsEventLoop( DHS_ELT_NO_WAIT, &status );
 * dhsEventLoop( DHS_ELT_THREADED, &threadId, &status );
 * dhsEventLoop( DHS_ELT_WAIT, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) loopType (DHS_EL_TYPE) Type of event loop to run.
 * (>) condFn (DHS_COND_FN) (optional) Condition function to execute when
 *			the event loop is to exit on a condition.
 * (>) pCondArg (void *) (optional) Argument to pass to the condition 
 *			function.
 * (>) pThreadId (DHS_THREAD *) (optional) Id of thread created for the
 *			event loop.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Start the DHS library event loop
 *
 * There are various ways to start the event loop. The options are:
 * - DHS_ELT_COND: Run the event loop until the supplied function
 *		returns true. The function is executed after each event is 
 *		received by the event loop.  The prototype of the condition 
 *		function is boolean condFn( void * pCondArg ), the event 
 *		loop passes the pCondArg argument as the argument to the
 *		condition function.
 *		If the event loop is already running, the function will
 *		wait until the condition function returns true, but the
 *		event will be processed by the existing event loop.
 * - DHS_ELT_NO_WAIT: Process all pending events and then return.
 *		This is only used when the event loop is not running in
 *		its own thread.
 * - DHS_ELT_THREADED: Create a new thread and run the event loop
 *		in the new thread. The event loop will continue to run until
 *		the dhsEventLoopEnd funcdtion is executed. The parent task
 *		returns as soon as the thread is started. The thread id of
 *		the new thread is returned in the threadId parameter.
 * - DHS_ELT_WAIT: Run the event loop indefinetely. This function
 *		will process events until the dhsEventLoopEnd function
 *		is executed.
 *
 * Note
 * - it is not an error to run this function when the event loop is
 *   already running.
 * - the event loop is stopped while the condition function executes,
 *   so the function should always be as fast as possible.
 *
 * DESCRIPTION:
 * Create a new event loop structure and intialize it to values appropriate
 * for the type of event loop, and then attempt to run the event loop.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 *
 * SEE ALSO:
 * - dhsEventLoopEnd
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsEventLoop
(
    DHS_EL_TYPE	loopType,	/* (in)  The event loop type.		*/
    ...				/* The event loop args.			*/
)
{
    DHS_COND_FN	condFn;		/* Pointer to the condition function.	*/
    tEventLoop	el;		/* Event loop object.			*/
    DHS_STATUS	elStatus;	/* Status from attempt to run event loop.*/
    boolean	noWait;		/* Don't wait in event loop.		*/
    void	*pCondArg;	/* Argument to the condition function.	*/
    DHS_STATUS	*pStatus;	/* Function return status.		*/
    DHS_THREAD	*pThreadId;	/* Thread id parameter.			*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    boolean	thread;		/* Run the event loop in a thread?	*/
    DHS_THREAD	threadId;	/* Id of the thread created.		*/
    va_list	varg;


    fnEntry( "dhsEventLoop" );


    va_start( varg, loopType );


    /*
     *  Intialize the optional argument variables to their default values.
     */

    condFn = NULL;
    pCondArg = NULL;
    noWait = FALSE;
    thread = FALSE;


    /*
     *  Get the optional arguments from the variable argument list. The
     *  arguments present depend on the loop type.
     */

    switch ( loopType )
    {
	case	DHS_ELT_COND:
	    condFn = va_arg( varg, DHS_COND_FN );
	    pCondArg = va_arg( varg, void * );
	    break;
	case	DHS_ELT_NO_WAIT:
	    noWait = TRUE;
	    break;
	case	DHS_ELT_THREADED:
	    thread = TRUE;
	    pThreadId = va_arg( varg, DHS_THREAD * );
	    break;
	case	DHS_ELT_WAIT:
	    break;
	default:
	    dhsMsgFmt( DHS_E_LOOP_TYPE, DHS_DEBUG_OFF, loopType );
	    fnReturn( VOID );
    }
    pStatus = va_arg( varg, DHS_STATUS * );
    va_end( varg );


    /*
     * Do a belated check of the status and to see if the library has been 
     * Initialized. This is usually done at the top of the function, but
     * since the status parameter wasn't available, it had to be delayed.
     */

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Intialize the event loop structure to the specified in the optional
     *  parameters.
     */

    dhsEl( &el, condFn, pCondArg, noWait, thread, pStatus );


    /*
     *  Lock the condition variable mutex. This ensures that two threads 
     *  cannot succeed in running the event loop.  If the thread succeeds
     *  in running the event loop, the condition variable will be unlocked
     *  in the event loop function. If the event loop is not run, the
     *  condition variable is unlocked in this function without too much
     *  delay. If the function gets into the loop in the DHS_ELT_COND 
     *  case below, the condition variable is unlocked while the thread
     *  is waiting in the dhsCondLock function, so in no case should the
     *  lock cause delays in processing.
     */

    check( dhsCondLock( el.elCond, pStatus ), *pStatus, VOID );

    switch ( loopType )
    {
	case	DHS_ELT_COND:
	    /*
	     *  Run the event loop until the condition function returns true.
	     */

	    while( TRUE )
	    {
		/*
		 *  Check to see if the condition function is true, and if
		 *  it is, return.  Note that it is possible the event loop
		 *  won't be run if the condition is already true.
		 */

		if ( condFn( pCondArg ) )
		{
		    break;
		}


		/*
		 *  Set up a separate status variable for the event loop,
		 *  and then attempt to run the event loop.
		 */

		elStatus = DHS_S_SUCCESS;
		dhsElStart( &el, &elStatus );

		if ( elStatus == DHS_E_EL_RUNNING )
		{
		    /*
		     *  Another thread must be running the event loop, so
		     *  just wait for events.  Note that after each event
		     *  it is necessary to try to run the event loop since
		     *  the event may have been another thread leaving the
		     *  event loop.
		     */

		    check( dhsCondWait( el.elCond, pStatus ), *pStatus, VOID );
		}
		else
		{
		    /* 
		     *  An error occured trying to run the event loop.
		     */

		    *pStatus = elStatus;
		    break;
		}
	    }
	    break;
	case	DHS_ELT_NO_WAIT:
	    /*
	     *  run the event loop.
	     */

	    dhsElStart( &el, pStatus );
	    if ( *pStatus == DHS_E_EL_RUNNING )
	    {
		*pStatus = DHS_S_SUCCESS;
	    }
	    break;
	case	DHS_ELT_THREADED:
	    /* 
	     *  Execute function dhsElStartThread in its own thread.
	     */

	    threadId = dhsThread( (tDhsThreadFn) dhsElStartThread, &el, 
		    pStatus );
	    if ( pThreadId != NULL )
	    {
		*pThreadId = threadId;
	    }

	    /*
	     *  The event loop does a broadcast before it starts, so this
	     *  waits until the loop starts. It isn't possible for the 
	     *  broadcast from the event loop to occur before the wait
	     *  since the condition variable is locked here, so the 
	     *  broadcast cannot occur until the wait begins.
	     */

	    dhsCondWait( el.elCond, pStatus );
	    break;
	case	DHS_ELT_WAIT:
	    /*
	     *  run the event loop.
	     */

	    dhsElStart( &el, pStatus );
	    break;
    }


    /*
     *  Unlock the condition variable.
     */

    s = DHS_S_SUCCESS;
    dhsCondUnlock( el.elCond, &s );
    if ( s != DHS_S_SUCCESS && *pStatus == DHS_S_SUCCESS )
    {
	*pStatus = s;
    }


    /*
     *  Clean up the event loop object.
     */

    s = DHS_S_SUCCESS;
    dhsEl_d( &el, &s );
    if ( s != DHS_S_SUCCESS && *pStatus == DHS_S_SUCCESS )
    {
	*pStatus = s;
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsEventLoopEnd
 *
 * INVOCATION:
 * dhsEventLoopEnd( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Case a message to be added to the event queue which will cause the
 * event loop to exit
 *
 * DESCRIPTION:
 * Send a message to this tasks event loop which will cause the event loop
 * to exit, and then wait for the message to take effect.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal.dlImpMutex: IMP function mutex.
 * - dhsLocal.dlImpId = IMP task id.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsEventLoopEnd
(
    DHS_STATUS	*pStatus	/* (mod) function return status.	*/
)
{
    tEventLoop	el;		/* Event loop structure to allow access	*/
    				/* to the event loop mutex.		*/
    IMP_MsgInfo	msgInfo;	/* Imp message info structure.		*/
    IMP_Status	impStatus;	/* Imp function return status.		*/
    

    fnEntry( "dhsEventLoopEnd" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Intialize the imp status.
     */

    impStatus = IMP_STATUS_OK;


    /*
     *  Intialize the msgInfo structure to send a DHS_MSG_EL_EXIT message.  
     *  The value of msgInfo.Address isn't important, but IMP gets very 
     *  upset if it is NULL.
     */

    msgInfo.Address = "Exit time";
    msgInfo.MessageLength = strlen( (char *) msgInfo.Address );
    msgInfo.Tag = DHS_MSG_EL_EXIT;
    msgInfo.Type = DHS_MSG_EL_EXIT;
    ImpDeltaTime( 0, 0, &msgInfo.DeltaTime );


    dhsEl( &el, NULL, NULL, FALSE, FALSE, pStatus );

    dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus );
    dhsLocal.dlExitCount ++;
    dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus );


    /*
     *  Issue an ImpQueueReminder message to send the message to this task.
     *  The function is called in a mutex to ensure no two threads ever
     *  use the same IMP process id at the same time.
     */

    ImpQueueReminder( dhsLocal.dlImpIdIn, &msgInfo, IMP_STATIC, &impStatus );
    if ( impStatus != 0 )
    {
	dhsMutexLock( &(dhsLocal.dlImpMutex), pStatus );
	dhsLocal.dlExitCount --;
	dhsMutexUnlock( &(dhsLocal.dlImpMutex), pStatus );
	impCheckRet( ImpQueueReminder( dhsLocal.dlImpIdIn, &msgInfo, 
		IMP_STATIC, &impStatus ), impStatus, *pStatus, VOID );
    }


    /*
     *  Wait until the event loop exits.  The event loop structure "el" is
     *  only necessary to give the dhsElStopped function access to the 
     *  static value that indicates if the event loop is running.
     */

    if ( ! pthread_equal( *(el.elThreadId), dhsThreadSelf() ) )
    {
	dhsEventLoop( DHS_ELT_COND, dhsElNoExit, &el, pStatus );
    }

    fnReturn( VOID );
}
