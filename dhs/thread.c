static char rcsid[] = "$Id: thread.c,v 1.1.1.1 2002-11-24 20:20:49 brighton Exp $";
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
 * dhsLibrary/src/thread.c
 *
 * PURPOSE:
 * Contains a routine to start a new thread.
 * This file will have to be replaced with VxWorks specific code for
 * the VxWorks version of the DHS library.
 *
 * FUNCTION NAME(S)
 * dhsThread - Start a function in a new thread.
 * dhsThreadEqual - Compare the value of two thread ids.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.7  1998/03/05 23:30:06  nhill
 * Bug fix.
 *
 * Revision 1.6  1997/08/28 20:49:14  nhill
 * Documentation fixes.
 *
 * Revision 1.5  1997/06/09 21:18:51  nhill
 * Changed the hp unix symbol to hpux from HPUX.
 *
 * Revision 1.4  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.3  1997/01/20 17:15:11  nhill
 * Checkpoint.
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

#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsThread
 *
 * INVOCATION:
 * threadId = dhsThread( fnPointer, arg, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (tDhsThreadFn) Pointer to function to start in the new thread.
 * (>) arg (void *) Argument to pass to the new function.
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_THREAD) Identifier of the thread started.
 *
 * PURPOSE:
 * Create a new thread and execute the spcified function in the thread
 *
 * The function returns a thread id of the thread created.  This is not
 * a part of the public DHS library interface.
 *
 * DESCRIPTION:
 * This function is implemented with the POSIX pthread_create function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - macro dhsThreadSelf()
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_THREAD	dhsThread
(
    tDhsThreadFn
   		fn,		/* (in)  Pointer to function to start.	*/
    void	*arg,		/* (in)  Argument to the function.	*/
    DHS_STATUS	*status		/* (out) Function return status.	*/
)
{
    int			s;	/* pthread_create return status.	*/
    DHS_THREAD		thread;	/* Id of the new thread.		*/


    fnEntry( "dhsThread" );
    statCheck( *status, DHS_THREAD_NULL );


    /* 
     *  Start execute the function in a new thread.
     */

#ifdef hpux
    s = pthread_create( &thread, pthread_attr_default, fn, arg );	
#else /* hpux */
    s = pthread_create( &thread, NULL, fn, arg );	
#endif /* hpux */


    /*
     *  Check that the thread was started successfully.
     */

    if ( s != 0 )
    {
	dhsMsgFmt( MSG_ERRNO, DHS_DEBUG_OFF, "pthread_create"  );
	dhsMsgFmt( DHS_E_THREAD_CREATE, DHS_DEBUG_OFF, s );
	*status = DHS_E_THREAD_CREATE;
	fnReturn( thread );
    }


#ifdef hpux
    s = pthread_detach( &thread );
#else /* hpux */
    s = pthread_detach( thread );
#endif /* hpux */
    if ( s != 0 )
    {
	dhsMsgFmt( MSG_ERRNO, DHS_DEBUG_OFF, "pthread_detach"  );
	dhsMsgFmt( DHS_E_THREAD_CREATE, DHS_DEBUG_OFF, s );
	*status = DHS_E_THREAD_CREATE;
	fnReturn( thread );
    }


    /*
     *  Return the id of the new thread.
     */

    fnReturn( thread );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsThreadEqual
 *
 * INVOCATION:
 * areEqual = dhsThreadEqual( thread1, thread2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * thread1 (DHS_THREAD_ID) - The first thread to compare.
 * thread2 (DHS_THREAD_ID) - The second thread to compare.
 *
 * FUNCTION VALUE:
 * (boolean) True if the threads are equal, false otherwise.
 *
 * PURPOSE:
 * Compare the value of two threads.
 *
 * DESCRIPTION:
 * This function is implemented with the POSIX pthread_equal function.
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

int	dhsThreadEqual
(
    DHS_THREAD	thread1,	/* (in)  First thread to compare.	*/
    DHS_THREAD	thread2		/* (in)  Second thread to compare.	*/
)
{
    return( pthread_equal( thread1, thread2 ) );
}
