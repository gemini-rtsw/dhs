static char rcsid[] = "$Id: mutex.c,v 1.1.1.1 2002-11-24 20:20:39 brighton Exp $";
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
 * dhsLibrary/src/mutex.c
 *
 * PURPOSE:
 * Contains functions to manipulate mutexes.
 *
 * FUNCTION NAME(S)
 * dhsMutexTry - Attempt to lock a mutex.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.4  1998/06/28 22:18:17  nhill
 * Added a debug message to dhsMutexTrylock.
 *
 * Revision 1.3  1997/08/28 19:23:26  nhill
 * Fixed some comments.
 *
 * Revision 1.2  1997/01/13 17:58:34  nhill
 * Reasonably stable checkpoint.
 *
 * Revision 1.1  1997/01/06 17:24:10  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include <errno.h>

#include "gen_types.h"

#include "localDhs.h"


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsMutexTry
 *
 * INVOCATION:
 * dhsMutexTry( &mutex, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pMutex (tDhsMutex *) The mutex to attemp to lock.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (boolean) Returns TRUE if the mutex could be locked, false otherwise.
 *
 * PURPOSE:
 * Attempt to lock a mutex
 * This function attempts to lock a mutex defined as a tDhsMutex type.
 * If the function acquires the lock, the function returns true, otherwise
 * it returns false.
 *
 * This is not a public function of the DHS library.
 *
 * DESCRIPTION:
 * This function uses the posix threads mutex functions to implement the 
 * mutex. This function is not a macro because a macro would have to be
 * implemented with nested conditional operators and thats just too ugly.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - Header file "mutex.h" must be included.
 * - The mutex must be initialized with the dhsMutexInit macro.
 * - The status parameter must be initialized to DHS_S_SUCCESS.
 *
 * SEE ALSO:
 * - Macro dhsMutexDestroy.
 * - Macro dhsMutexInit.
 * - Macro dhsMutexLock.
 * - Macro dhsMutexUnlock.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean	dhsMutexTry
(
    tDhsMutex	*pMutex,	/* (mod) The mutex to try.		*/
    DHS_STATUS	*pStatus	/* (out) Function return status.	*/
)
{
    int		s;		/* pthread_mutex_trylock return status.	*/
    boolean	ret;		/* Value to return.			*/


    fnEntry( "dhsMutexTry" );
    statCheck( *pStatus, FALSE );


    /*
     *  Attempt to acquire the lock.
     */

    s = pthread_mutex_trylock( pMutex );
    if ( s == EBUSY )
    {
	/*
	 *  Failed to acquire the lock because it is busy.
	 */

	ret = FALSE;
    }
    else if ( s != 0 )
    {
	/*
	 *  Failed to acquire the lock for some other reason.
	 */

	dhsMsgFmt( DHS_E_MUTEX, DHS_DEBUG_OFF, s );
	*pStatus = DHS_E_MUTEX;
	ret = FALSE;
    }
    else
    {
	dhsMsgFmt( DHS_S_MUTEX_LOCK, DHS_DEBUG_FULL, "pMutex", pMutex );
	ret = TRUE;
    }

    fnReturn( ret );
}
