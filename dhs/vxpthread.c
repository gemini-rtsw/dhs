static char rcsid[]="$Id: vxpthread.c,v 1.1.1.1 2002-11-24 20:20:59 brighton Exp $";
/*
 * Copyright 1997 Association of Universities for Research In Astronomy, Inc.
 * 
 * Terms And Conditions for Copying, Distribution and Modification of
 * Gemini Software
 * 
 * This software has been developed for the Gemini Project and may be
 * used and copied only by the Association of Universities for Research in
 * Astronomy, Inc., and the governments of the United States of America, the
 * United Kingdom of Great Britain and Northern Ireland, Canada, Argentina,
 * Brazil and Chile, and only for non-commercial research purposes. All
 * copies of this software must contain this copyright and disclaimer.
 * If you make modifications to this software, you must clearly mark the
 * software as having been changed, but must retain this copyright and
 * disclaimer.
 * 
 * Disclaimer of all Warranties
 * 
 * Since this software is available free of charge, there is NO WARRANTY for
 * the software, as limited by applicable law.  This software is available
 * "AS IS"; without warranty of any kind, either expressed or implied,
 * including the implied warranties of merchantability and fitness for a
 * specific purpose.  By using this software you are assuming all risks
 * and costs.  In no event is the Association of Universities for Research
 * in Astronomy, Inc. or the Gemini Project liable for any damages that
 * might result from the use of this software.
 * 
 *
 * FILENAME
 * vxpthread.c
 *
 * FUNCTION NAME(S)
 * pthread_self
 * pthread_mutex_init
 * pthread_mutex_lock
 * pthread_mutex_trylock
 * pthread_mutex_unlock
 * pthread_mutex_destroy
 * pthread_cond_init
 * pthread_cond_destroy
 * pthread_cond_wait
 * pthread_cond_timedwait
 * pthread_cond_broadcast
 * pthread_attr_init
 * pthread_attr_destroy
 * pthread_attr_setdetachstate
 * pthread_create
 * pthread_key_create
 * pthread_key_delete
 * pthread_setspecific
 * pthread_getspecific
 * pthread_detach
 * pthread_equal
 *
 *
 * MODIFICATION HISTORY
 * 01a,bdg,08jan97	created.
 * 01b,bdg,31jan97	
 * 01c,bdg,20feb97	fixed pointers to mutex and cond calls.
 * 01d,bdg,17mar97	fixed semGive() return (in VxWorks it's always -1!)
 * 01e,nrh,26mar97	added pthread_detach function.
 * 01f,bdg,13may97	added pthread_equal function.
 * 01g,nrh,05mar01	Fixed the pthread_mutex_trylock function to
 *			return the correct error value.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2002/03/08 19:37:21  tpaz
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.10  2001/03/06 00:01:53  nhill
 * Changed pthread_mutex_tryclock to have the correct return values.
 *
 * Revision 1.9  1998/02/16 23:52:53  nhill
 * Added the body of pthread_cond_timedwait.
 *
 * Revision 1.8  1998/02/05 19:24:28  nhill
 * Changed the name of the vxpthread.h header file.
 *
 * Revision 1.7  1998/02/03 21:14:07  nhill
 * Added a stub for function pthread_cond_timedwait.
 *
 * Revision 1.6  1997/07/10 19:09:17  nhill
 * Spelling changes.
 *
 * Revision 1.5  1997/06/04 18:21:10  nhill
 * added function pthread_equal.
 *
 * Revision 1.4  1997/03/26 17:37:03  nhill
 * Changed -k option on rcs.
 *
 * Revision 1.3  1997/03/26 17:10:02  nhill
 * Added the pthread_detach function.
 *
 *INDENT-ON*
 */

#ifdef	vxWorks

#include "semLib.h"
#include "taskLib.h"
#include "taskVarLib.h"
#include "taskHookLib.h"
#include "errnoLib.h"

#include "dhsVxPthread.h"


typedef struct _pthread_key
{
    WIND_TCB *tid;
    int init;
    const void *value;
    void (*destructor)(void *);
} pthread_key;

static pthread_key *	pkeylist[PTHREAD_KEYS_MAX];


/*
 *+
 * FUNCTION NAME:
 * pthread_self
 *
 * INVOCATION:
 * pthread_self ()
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *
 * FUNCTION VALUE:
 * pthread_t	pthread type
 *
 * PURPOSE:
 * get calling thread's ID
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It returns the task ID of the calling task.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 *-
 */

pthread_t pthread_self (void)
{
    return (pthread_t) taskIdSelf ();
}


/*
 *+
 * FUNCTION NAME:
 * pthread_mutex_init
 *
 * INVOCATION:
 * pthread_mutex_init (pmv, pmattr)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *	<	pmv	pthread_mutex_t *
 *	>	pmattr	const pthread_mutexattr_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * create a mutual exclusion semaphore
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It creates a mutual exclusion semaphore.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The attribute argument is ignored.
 *-
 */

int pthread_mutex_init (
    pthread_mutex_t *pmv,
    const pthread_mutexattr_t *pmattr)
{
    *pmv = (pthread_mutex_t) semMCreate (SEM_Q_FIFO);
    return *pmv ? 0 : errnoGet ();
}


/*
 *+
 * FUNCTION NAME:
 * pthread_mutex_lock
 *
 * INVOCATION:
 * pthread_mutex_lock (pmv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *	>	pmv	pthread_mutex_t *
 *
 * FUNCTION VALUE:
 * int		Success (0) or failure (-1).
 *
 * PURPOSE:
 * Lock the mutual exclusion semaphore.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It locks the mutual exclusion semaphore.  It will wait forever
 * for the mutex to become available.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The mutex variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The mutex variable cannot be statically allocated, per POSIX.
 *-
 */

int pthread_mutex_lock (
    pthread_mutex_t *pmv)
{
    return semTake ((SEM_ID) *pmv, WAIT_FOREVER);
}


/*
 *+
 * FUNCTION NAME:
 * pthread_mutex_trylock
 *
 * INVOCATION:
 * pthread_mutex_trylock (pmv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pmv     pthread_mutex_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * try to lock a mutual exclusion semaphore
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It attempts to lock a mutual exclusion semaphore.  It does not
 * wait if the mutex is not available.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The mutex variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The mutex variable cannot be statically allocated, per POSIX.
 *-
 */

int pthread_mutex_trylock (
    pthread_mutex_t *pmv)
{
    if ( semTake ((SEM_ID) *pmv, NO_WAIT) != 0 )
    {
	if ( errno == S_objLib_OBJ_UNAVAILABLE )
	{
	    return( EBUSY );
	}

	return(-1);
    }

    return( 0 );
}


/*
 *+
 * FUNCTION NAME:
 * pthread_mutex_unlock
 *
 * INVOCATION:
 * pthread_mutex_unlock (pmv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pmv     pthread_mutex_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * unlock a mutual exclusion semaphore
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It unlocks a mutual exclusion semaphore.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The mutex variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The mutex variable cannot be statically allocated, per POSIX.
 *-
 */

int pthread_mutex_unlock (
    pthread_mutex_t *pmv)
{
    (void) semGive ((SEM_ID) *pmv);	/* VxWorks always returns an error */
    return 0;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_mutex_destroy
 *
 * INVOCATION:
 * pthread_mutex_destroy (pmv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pmv     pthread_mutex_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * destroy a mutual exclusion semaphore
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It destroys a mutual exclusion semaphore.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The mutex variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The mutex variable cannot be statically allocated, per POSIX.
 *-
 */

int pthread_mutex_destroy (
    pthread_mutex_t *pmv)
{
    (void) semDelete ((SEM_ID) *pmv);
    return 0;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_cond_init
 *
 * INVOCATION:
 * pthread_cond_init (pcv, pcattr)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      <       pcv     pthread_cond_t *
 *      >       pcattr  const pthread_condattr_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * create a condition variable
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It creates a condition variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The attribute argument is ignored.
 *-
 */

int pthread_cond_init (
    pthread_cond_t *pcv,
    const pthread_condattr_t *pcattr)
{
    *pcv = (pthread_cond_t) semBCreate (SEM_Q_FIFO, SEM_EMPTY);
    return *pcv ? 0 : -1;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_cond_destroy
 *
 * INVOCATION:
 * pthread_cond_destroy (pcv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pcv     pthread_cond_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * Destroy a condition variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It destroys a condition variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The condition variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 *-
 */

int pthread_cond_destroy (
    pthread_cond_t *pcv)
{
    (void) semDelete ((SEM_ID) *pcv);
    return 0;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_cond_timedwait
 *
 * INVOCATION:
 * pthread_cond_wait (pcv, pmv, time)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pcv     pthread_cond_t *
 *      >       pmv	pthread_mutex_t *
 *      >       time	struct timespec *
 *
 * FUNCTION VALUE:
 * int    Success (0), failure (-1) or ETIME for a timeout.
 *
 * PURPOSE:
 * Wait on a condition variable or timeout if the specified time is passed.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It waits on a condition variable or until the specified time is reached.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The condition variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * This routine is implemented with the VxWorks taskLock() call to
 * provide atomic execution.
 *-
 */

int pthread_cond_timedwait (
    pthread_cond_t *pcv,
    pthread_mutex_t *pmv,
    struct timespec *time)
{
    int status = 0;
    struct timespec now;
    int timeout; 
 
    if (clock_gettime (CLOCK_REALTIME, &now) != OK)
        return -1;
    timeout = ((time->tv_sec - now.tv_sec) * sysClkRateGet ()) +
        ((((time->tv_nsec - now.tv_nsec) / 1000) * sysClkRateGet ()) / 1000000);
    if (timeout < 0)
        timeout = NO_WAIT;
 
    (void) taskLock ();
    if (pthread_mutex_unlock (pmv))
        status = -1;
    else if (semTake ((SEM_ID) *pcv, timeout) == ERROR)
        status = ETIME;
    (void) pthread_mutex_lock (pmv);
    (void) taskUnlock ();
 
    return status;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_cond_wait
 *
 * INVOCATION:
 * pthread_cond_wait (pcv, pmv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pcv     pthread_cond_t *
 *      >       pmv	pthread_mutex_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * Wait on a condition variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It waits on a condition variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The condition variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * This routine is implemented with the VxWorks taskLock() call to
 * provide atomic execution.
 *-
 */

int pthread_cond_wait (
    pthread_cond_t *pcv,
    pthread_mutex_t *pmv)
{
    int status = 0;

    (void) taskLock ();
    if (pthread_mutex_unlock (pmv))
	status = -1;
    else if (semTake ((SEM_ID) *pcv, sysClkRateGet()) == -1)
	status = 0;
    (void) pthread_mutex_lock (pmv);
    (void) taskUnlock ();

    return status;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_cond_broadcast
 *
 * INVOCATION:
 * pthread_cond_broadcast (pcv)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pcv     pthread_cond_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * Broadcast to a condition variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It broadcasts to a condition variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The condition variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * This routine uses the VxWorks semFlush() call.
 *-
 */

int pthread_cond_broadcast (
    pthread_cond_t *pcv)
{
    (void) semFlush ((SEM_ID) *pcv);
    return 0;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_attr_init
 *
 * INVOCATION:
 * pthread_attr_init (pattr)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      <       pattr	pthread_attr_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * Create an attribute.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It creates an attribute for posix thread.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The routine does nothing, since the only attribute used is DETACHED.
 *-
 */

int pthread_attr_init (
    pthread_attr_t *pattr)
{
    return OK;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_attr_destroy
 *
 * INVOCATION:
 * pthread_attr_destroy (pattr)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pattr	pthread_attr_t *
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * Destroy a thread attribute.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It destroys a thread attribute.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The attribute must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * the routine does nothing, since attributes are never created.
 *-
 */

int pthread_attr_destroy (
    pthread_attr_t *pattr)
{
    return OK;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_attr_setdetachstate
 *
 * INVOCATION:
 * pthread_attr_setdetachstate (pattr)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       pattr   	pthread_attr_t *
 *	>	detachstate	int
 *
 * FUNCTION VALUE:
 * int    Success (0) or failure (-1).
 *
 * PURPOSE:
 * Set the detach state of a thread attribute.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It sets the detached state of a thread attribute.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The attribute must have been created..
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The routine does nothing, since the only attribute used is DETACHED.
 *-
 */

int pthread_attr_setdetachstate (
    pthread_attr_t *pattr,
    int detachstate)
{
    return OK;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_create
 *
 * INVOCATION:
 * pthread_create (pthread, pattr, function, parg)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      <       pthread		pthread_t *
 *	>	pattr		const pthread_attr_t *
 *	>	function	void * (*function)(void *)
 *	>	parg		void *
 *
 * FUNCTION VALUE:
 * int		Success (0) or failure (-1).
 *
 * PURPOSE:
 * Create a thread.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It creates a POSIX thread.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The priority is the same as the calling task.
 * The stacksize is fixed.
 * All attributes are ignored.
 *-
 */

int pthread_create (
    pthread_t *pthread,
    const pthread_attr_t *pattr,
    void * (*function)(void *),
    void *parg)
{
    int priority;

    (void) taskPriorityGet (0, &priority);
    *pthread = taskSpawn (NULL, priority, VX_FP_TASK, 100000,
	(FUNCPTR) function, (int) parg, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    return *pthread ? 0 : -1;
}


/*******************************************************************************
*/

static void pthread_key_destruct (
    WIND_TCB *pTcb)
{
}


/*
 *+
 * FUNCTION NAME:
 * pthread_key_create
 *
 * INVOCATION:
 * pthread_key_create (pkey, destructor)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      <       pkey		pthread_key_t *
 *	>	destructor	void (*destructor)(void *))
 *
 * FUNCTION VALUE:
 * int          Success (0) or failure (-1).
 *
 * PURPOSE:
 * Create a thread specific variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It creates a thread specific variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The destructor function does not work.
 *-
 */

int pthread_key_create (
    pthread_key_t *pkey,
    void (*destructor)(void *))
{
    for (*pkey = 0; *pkey < PTHREAD_KEYS_MAX; (*pkey)++)
    {
	if (pkeylist[*pkey] == NULL)
	{
	    pkeylist[*pkey] = (pthread_key *) malloc (sizeof (pthread_key));
	    pkeylist[*pkey]->init = 0;
	    pkeylist[*pkey]->value = NULL;
	    pkeylist[*pkey]->destructor = destructor;
	    taskDeleteHookAdd ((FUNCPTR) pthread_key_destruct);
	    return OK;
	}
    }
    return ERROR;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_key_delete
 *
 * INVOCATION:
 * pthread_key_delete (key)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       key		pthread_key_t
 *
 * FUNCTION VALUE:
 * int          Success (0) or failure (-1).
 *
 * PURPOSE:
 * Delete a thread specific variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It deletes a thread specific variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The thread specific variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 *-
 */

int pthread_key_delete (
    pthread_key_t key)
{
    if (!pkeylist || !pkeylist[key])
	return ERROR;
    (void) free ((void *) pkeylist[key]);
    pkeylist[key] = NULL;
    return OK;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_setspecific
 *
 * INVOCATION:
 * pthread_setspecific (key, value)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       key		pthread_key_t
 *      >       value		const void *
 *
 * FUNCTION VALUE:
 * int          Success (0) or failure (-1).
 *
 * PURPOSE:
 * Set a thread specific variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It sets a thread specific variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The thread specific variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 * The variable is saved as a VxWorks task variable.
 *-
 */

int pthread_setspecific (
    pthread_key_t key,
    const void *value)
{
    if (!pkeylist || !pkeylist[key])
	return ERROR;
					/* test if key is already a taskVar */
    if (pkeylist[key]->init != -1)
    {
	taskVarInit ();
	taskVarAdd (0, (int *) &pkeylist[0]->value);
	pkeylist[key]->init++;
    }
    pkeylist[key]->value = value;
    return OK;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_getspecific
 *
 * INVOCATION:
 * pthread_getspecific (key)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       key		pthread_key_t
 *
 * FUNCTION VALUE:
 * void *	The value of the thread specific variable.
 *
 * PURPOSE:
 * Get a thread specific variable.
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It gets a thread specific variable.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * The thread specific variable must have been created.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 *-
 */

void * pthread_getspecific (
    pthread_key_t key)
{
    if (!pkeylist || !pkeylist[key])
	return NULL;
    return (void *) pkeylist[key]->value;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_detach
 *
 * INVOCATION:
 * pthread_detach (thread)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       thread		pthread_t
 *
 * FUNCTION VALUE:
 * int          Always returns 0.
 *
 * PURPOSE:
 * Emulate the POSIX pthread_detach function
 *
 * DESCRIPTION:
 * This is a no-op under vxworks since VxWorks tasks are never "attached"
 * in the POSIX threads sense.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 *-
 */

int pthread_detach (
    pthread_t key)
{
    return OK;
}


/*
 *+
 * FUNCTION NAME:
 * pthread_equal
 *
 * INVOCATION:
 * pthread_equal (thread1, thread2)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *      >       thread1		pthread_t
 *      >       thread2		pthread_t
 *
 * FUNCTION VALUE:
 * int          0 or 1.
 *
 * PURPOSE:
 * Emulate the POSIX pthread_equal function
 *
 * DESCRIPTION:
 * This is a VxWorks emulation of the POSIX call.
 * It compares the values of two thread IDs.
 *
 * EXTERNAL VARIABLES:
 * none.
 *
 * PRIOR REQUIREMENTS:
 * none.
 *
 * DEFICIENCIES:
 * This is a VxWorks emulation.
 *-
 */

int pthread_equal (
    pthread_t thread1,
    pthread_t thread2)
{
    return (thread1 == thread2);
}


#endif	/* vxWorks */
