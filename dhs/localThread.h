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
 * dhsLibrary/h/localThread.h
 *
 * PURPOSE:
 * Header file for the thread creation functions and macros.
 * This file will be replaced for the VxWorks version of the DHS library.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:50  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.5  2000/08/23 21:35:05  nhill
 * Changed this file name from thread.h.
 *
 * Revision 1.4  1997/03/26 16:59:52  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.3  1997/01/20 17:15:30  nhill
 * checkpoint.
 *
 * Revision 1.2  1997/01/13 17:59:20  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef	LOCALTHREAD_H
#define	LOCALTHREAD_H


/*
 ************************************************************************
 *  Type definitions.
 ************************************************************************
 */

/*
 *  Type for a thread function pointer.
 */

typedef void	*(*tDhsThreadFn)( void * );


/*
 ************************************************************************
 *  Macro definitions.
 ************************************************************************
 */

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsThreadSelf
 *
 * INVOCATION:
 * threadId = dhsThreadSelf
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * None.
 *
 * MACRO VALUE:
 * (DHS_THREAD_ID) Thread id of the current thread.
 *
 * PURPOSE:
 * Return the thread id of the current thread
 *
 * DESCRIPTION:
 * This macro is implemented with the POSIX pthread_self function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * dhsThread.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsThreadSelf()	 pthread_self()


/*
 *  External function declarations.
 */

extern DHS_THREAD	dhsThread( tDhsThreadFn, void *, DHS_STATUS * );
extern int		dhsThreadEqual( DHS_THREAD, DHS_THREAD );

#endif /* LOCALTHREAD_H */
