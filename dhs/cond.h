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
 * dhsLibrary/h/cond.h
 *
 * PURPOSE:
 * This is the header file for the condition variable manipulation functions.
 * The condition variable functionality is based on the POSIX thread 
 * condition variables, and under Solaris is implemented using the posix 
 * thread functions.
 *
 *  A mutex variable has been associated with the condition variables since
 *  a condition variable always needs to be used with a mutex for it to
 *  work correctly.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:42  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.6  1997/12/18 22:16:24  nhill
 * Added macro dhsCondTimedWait.
 *
 * Revision 1.5  1997/06/09 21:20:46  nhill
 * Changed the hp unix symbol to hpux from HPUX.
 *
 * Revision 1.4  1997/03/26 16:59:52  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.3  1997/02/04 19:42:19  nhill
 * Checkin for install.
 *
 * Revision 1.2  1997/01/13 17:59:20  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef	COND_H
#define	COND_H


/*
 ************************************************************************
 *  Type definitions.
 ************************************************************************
 */

typedef struct cond
{
    tDhsMutex		cMutex;		/* Mutex for the cond variable.	*/
    pthread_cond_t	cCond;		/* The condition variable.	*/
} tDhsCond;


/*
 ************************************************************************
 *  Macro definitions.
 ************************************************************************
 */

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCondBroadcast
 *
 * INVOCATION:
 * dhsCondBroadcast( &cond, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (!) pStatus (DHS_STATUS *) Return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Broadcast to all threads waiting on a condition variable
 *
 * All threads waiting on the condition variable with the dhsCondWait
 * macro will resume when a thread executes this macro. This is not part 
 * of the public DHS library interface.
 *
 * DESCRIPTION:
 * The POSIX mutex and condition variable functions are used to implement
 * this macro. The mutex lock and unlock that are required to surround
 * the broadcast have been incorporated into the macro because there was
 * no situation in which the broadcast was not surrounded by a lock and
 * unlock.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 * - The mutex must have been initialized with the dhsCondInit macro.
 *
 * SEE ALSO:
 * - Macro dhsCondInit
 * - Macro dhsCondLock
 * - Macro dhsCondUnlock
 * - Macro dhsCondWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsCondBroadcast( pCond, pStatus )				\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int		rs;						\
	    dhsMutexLock( &( (pCond)->cMutex ), pStatus );		\
	    if ( *(pStatus) == DHS_S_SUCCESS )				\
	    {								\
		rs = pthread_cond_broadcast( &( (pCond)->cCond ) );	\
		if ( rs != 0 )						\
		{							\
		    *(pStatus) = DHS_E_COND;				\
		    dhsMsgFmt( DHS_E_COND, DHS_DEBUG_OFF, rs );		\
		}							\
		dhsMutexUnlock( &( (pCond)->cMutex ), pStatus );	\
	    }								\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCondDestroy
 *
 * INVOCATION:
 * dhsCondDestroy( &cond, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (!) pStatus (DHS_STATUS *) Macro return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Destroy a condition variable
 *
 * This is not part of the public DHS library interface.
 *
 * DESCRIPTION:
 * The POSIX condition variable and mutex functions are used to implement
 * this macro.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 *
 * SEE ALSO:
 * - Macro dhsCondBroadcast
 * - Macro dhsCondLock
 * - Macro dhsCondUnlock
 * - Macro dhsCondWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsCondDestroy( pCond, pStatus )				\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int		rs;						\
	    dhsMutexDestroy( &( (pCond)->cMutex ), pStatus );		\
	    rs = pthread_cond_destroy( &( (pCond)->cCond ), NULL );	\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_COND;				\
		dhsMsgFmt( DHS_E_COND, DHS_DEBUG_OFF, rs );		\
	    }								\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCondInit
 *
 * INVOCATION:
 * dhsCondInit( &cond, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (!) pStatus (DHS_STATUS *) Macro return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Intialize a condition variable
 *
 * This is not part of the public DHS library interface.
 *
 * DESCRIPTION:
 * The POSIX condition variable and mutex functions are used to implement
 * this macro.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 *
 * SEE ALSO:
 * - Macro dhsCondBroadcast
 * - Macro dhsCondLock
 * - Macro dhsCondUnlock
 * - Macro dhsCondWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#ifdef hpux
#define	dhsCondInit( pCond, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int		rs;						\
	    dhsMutexInit( &( (pCond)->cMutex ), pStatus );		\
	    rs = pthread_cond_init( &( (pCond)->cCond ), 		\
		    pthread_condattr_default );				\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_COND;				\
		dhsMsgFmt( DHS_E_COND, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#else /* hpux */
#define	dhsCondInit( pCond, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int		rs;						\
	    dhsMutexInit( &( (pCond)->cMutex ), pStatus );		\
	    rs = pthread_cond_init( &( (pCond)->cCond ), NULL );	\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_COND;				\
		dhsMsgFmt( DHS_E_COND, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#endif /* hpux */

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCondLock
 *
 * INVOCATION:
 * dhsCondLock( &cond, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (!) pStatus (DHS_STATUS *) Macro return status.
 *
 * MACRO VALUE:
 * (void) 
 *
 * PURPOSE:
 * Lock the mutex associated with a condition variable
 *
 * DESCRIPTION:
 * The POSIX mutex functions are used to implement this macro.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 * - The condition variable must be initialized with the dhsCondInit macro.
 *
 * SEE ALSO:
 * - Macro dhsCondBroadcast
 * - Macro dhsCondInit
 * - Macro dhsCondUnlock
 * - Macro dhsCondWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsCondLock( pCond, pStatus )					\
	dhsMutexLock( &( (pCond)->cMutex ), pStatus )

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhscondUnlock
 *
 * INVOCATION:
 * dhsCondUnlock( &cond, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (!) pStatus (DHS_STATUS *) Macro return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Unlock the mutex associated with a condition variable.
 *
 * DESCRIPTION:
 * The POSIX mutex functions are used to implement this macro.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 * - The condition variable must be initialized with the dhsCondInit macro.
 *
 * SEE ALSO:
 * - Macro dhsCondBroadcast
 * - Macro dhsCondInit
 * - Macro dhsCondLock
 * - Macro dhsCondWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsCondUnlock( pCond, pStatus )					\
	dhsMutexUnlock( &( (pCond)->cMutex ), pStatus )

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCondTimedWait
 *
 * INVOCATION:
 * dhsCondTimedWait( &cond, abstime, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (>) pAbsTime (const struct timestruct_t *) time when wait should stop.
 * (!) pStatus (DHS_STATUS *) Macro return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Wait for a broadcast message from another thread or until the time is 
 * exceeded
 *
 * This macro is normally used in the following sequence:
 * 1. Lock the condition variable with the dhsCondLock macro.
 * 2. While some condition is false, use the dhsCondWait macro to wait 
 *    for a broadcast from some other thread.
 * 4. Unlock the condition variable with the dhsCondUnlock macro.
 *
 * DESCRIPTION:
 * The POSIX condition variable functions are used to implement this macro.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 * - The condition variable must be initialized with the dhsCondInit macro.
 * - The condition varialbe must be locked with the dhsCondLock macro.
 *
 * POST REQUIREMENTS:
 * - The condition variable must be unlocked with the dhsCondUnlock macro.
 *
 * SEE ALSO:
 * - Macro dhsCondBroadcast
 * - Macro dhsCondInit
 * - Macro dhsCondLock
 * - Macro dhsCondWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsCondTimedWait( pCond, pAbsTime, pStatus )			\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int		rs;						\
	    rs = pthread_cond_timedwait( &( (pCond)->cCond ), 		\
		    &( (pCond)->cMutex ), (pAbsTime) );			\
	    if ( rs == ETIME || rs == ETIMEDOUT )			\
	    {								\
		*(pStatus) = DHS_S_COND_TIMEOUT;			\
	    }								\
	    else if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_COND;				\
		dhsMsgFmt( DHS_E_COND, DHS_DEBUG_OFF, rs );		\
	    }								\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCondWait
 *
 * INVOCATION:
 * dhsCondWait( &cond, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pCond (tDhsCond *) Pointer to the condition variable.
 * (!) pStatus (DHS_STATUS *) Macro return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Wait for a broadcast message from another thread
 *
 * This macro is normally used in the following sequence:
 * 1. Lock the condition variable with the dhsCondLock macro.
 * 2. While some condition is false, use the dhsCondWait macro to wait 
 *    for a broadcast from some other thread.
 * 4. Unlock the condition variable with the dhsCondUnlock macro.
 *
 * DESCRIPTION:
 * The POSIX condition variable functions are used to implement this macro.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - Include header file cond.h.
 * - The condition variable must be initialized with the dhsCondInit macro.
 * - The condition varialbe must be locked with the dhsCondLock macro.
 *
 * POST REQUIREMENTS:
 * - The condition variable must be unlocked with the dhsCondUnlock macro.
 *
 * SEE ALSO:
 * - Macro dhsCondBroadcast
 * - Macro dhsCondInit
 * - Macro dhsCondLock
 * - Macro dhsCondTimedWait
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsCondWait( pCond, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int		rs;						\
	    rs = pthread_cond_wait( &( (pCond)->cCond ), 		\
		    &( (pCond)->cMutex ) );				\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_COND;				\
		dhsMsgFmt( DHS_E_COND, DHS_DEBUG_OFF, rs );		\
	    }								\
	}

#endif /* COND_H */
