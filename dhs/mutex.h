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
 * dhsLibrary/h/mutex.h
 *
 * PURPOSE:
 * This is the header file for the mutex (mutual exclusion) variable
 * manipulation functions. This is based on the POSIX mutex functions
 * and under solaris is implemented using the POSIX functions.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:52  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:59  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.8  1997/12/18 22:17:47  nhill
 * Fixed the message formatting.
 *
 * Revision 1.7  1997/06/09 21:21:35  nhill
 * Changed the hp unix symbol to hpux from HPUX.
 *
 * Revision 1.6  1997/05/13 17:09:31  nhill
 * Fixed a bug in the the code the occurs when the DEBUG_ON define is not set.
 *
 * Revision 1.5  1997/04/15 19:20:54  nhill
 * Added information to mutex error messages.
 *
 * Revision 1.4  1997/03/26 16:59:52  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.3  1997/01/22 21:36:10  nhill
 * Stable Checkin.
 *
 * Revision 1.2  1997/01/13 17:59:20  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef MUTEX_H
#define MUTEX_H


/*
 ************************************************************************
 *  Type definitions.
 ************************************************************************
 */

typedef pthread_mutex_t	tDhsMutex;


/*
 ************************************************************************
 *  Macro definitions.
 ************************************************************************
 */

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsMutexDestroy
 *
 * INVOCATION:
 * dhsMutexDestroy( &pMutex, &pStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pMutex (tDhsMutex*) Pointer to the mutex to destroy.
 * (!) pStatus (DHS_STATUS) Pointer to the return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Macro to free resources allocated to a mutex
 *
 * This is not a public member of the DHS library.
 *
 * DESCRIPTION:
 * This macro uses the pthread_mutex_destroy function to destory the mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - The mutex should have been initialized with the dhsMutexInit macro.
 * - The status value should have been initialized to DHS_S_SUCCESS.
 *
 * SEE ALSO:
 * - Macro dhsMutexInit.
 * - Macro dhsMutexLock.
 * - Macro dhsMutexUnlock.
 * - Function dhsMutexTry.
 *
 * DEFICIENCIES:
 * None.
 *- (XXX allan: added call to dhsMutexUnlock)
 ************************************************************************
 */

#define	dhsMutexDestroy( pMutex, pStatus )				\
        dhsMutexUnlock( pMutex, pStatus );                              \
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    rs = pthread_mutex_destroy( pMutex );			\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsMutexInit
 *
 * INVOCATION:
 * dhsMutexInit( &pMutex, &pStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pMutex (tDhsMutex *) Pointer to the mutex to initialize.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Macro to initialize the mutex
 *
 * This is not a public member of the DHS library. This macro must be used
 * to initialize a mutex before any other mutex access function or macro
 * is used.
 *
 * DESCRIPTION:
 * This macro uses the pthread_mutex_init function to destory the mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - The status value should have been initialized to DHS_S_SUCCESS.
 *
 * SEE ALSO:
 * - Macro dhsMutexDestroy.
 * - Macro dhsMutexLock.
 * - Macro dhsMutexUnlock.
 * - Function dhsMutexTry.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#ifdef hpux
#define	dhsMutexInit( pMutex, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    rs = pthread_mutex_init( pMutex, pthread_mutexattr_default );\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#else /* hpux */
#define	dhsMutexInit( pMutex, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    rs = pthread_mutex_init( pMutex, NULL );			\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#endif /* hpux */


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsMutexLock
 *
 * INVOCATION:
 * dhsMutexLock( &mutex, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pMutex (tDhsMutex *) The mutex to lock.
 * (!) pStatus (DHS_STATUS *) The function return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Acquire a lock on the mutex.
 *
 * This macro will block waiting for the macro to become free if necessary.
 * This is not a public member of the DHS library.
 *
 * DESCRIPTION:
 * This macro uses the pthread_mutex_lock function to lock the mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - The status value should have been initialized to DHS_S_SUCCESS.
 *
 * SEE ALSO:
 * - Macro dhsMutexDestroy.
 * - Macro dhsMutexInit.
 * - Macro dhsMutexUnlock.
 * - Function dhsMutexTry.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#ifdef DEBUG_ON
#define	dhsMutexLock( pMutex, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    dhsMsgFmt( DHS_S_MUTEX_LOCK, DHS_DEBUG_FULL, #pMutex, pMutex );\
	    rs = pthread_mutex_lock( pMutex );				\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#else
#define	dhsMutexLock( pMutex, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    rs = pthread_mutex_lock( pMutex );				\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#endif


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsMutexUnlock
 *
 * INVOCATION:
 * dhsMutexUnlock( &mutex, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pMutex (tDhsMutex *) Pointer to the mutex to unlock.
 * (!) pStatus (DHS_STATUS *) Return status.
 *
 * MACRO VALUE:
 * (void)
 *
 * PURPOSE:
 * Unlock a mutex
 *
 * This is not a public member of the DHS library.
 *
 * DESCRIPTION:
 * This macro uses the pthread_mutex_unlock function to unlock the mutex.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Include header file mutex.h.
 * - The status value should have been initialized to DHS_S_SUCCESS.
 *
 * SEE ALSO:
 * - Macro dhsMutexDestroy.
 * - Macro dhsMutexInit.
 * - Macro dhsMutexlock.
 * - Function dhsMutexTry.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#ifdef DEBUG_ON
#define	dhsMutexUnlock( pMutex, pStatus )				\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    dhsMsgFmt( DHS_S_MUTEX_UNLOCK, DHS_DEBUG_FULL, #pMutex, pMutex );\
	    rs = pthread_mutex_unlock( pMutex );			\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#else
#define	dhsMutexUnlock( pMutex, pStatus )				\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    int	rs;							\
	    rs = pthread_mutex_unlock( pMutex );			\
	    if ( rs != 0 )						\
	    {								\
		*(pStatus) = DHS_E_MUTEX;				\
		dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, rs );		\
	    }								\
	}
#endif


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern boolean		dhsMutexTry( tDhsMutex *, DHS_STATUS * );

#endif /* MUTEX_H */
