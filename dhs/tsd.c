static char rcsid[] = "$Id: tsd.c,v 1.1.1.1 2002-11-24 20:20:51 brighton Exp $";
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
 * dhsLibrary/src/tsd.c
 *
 * PURPOSE:
 * Contains routines to maintain the thread specific data structure.
 * This file will have to be replaced for the VxWorks version of the 
 * DHS library.
 *
 * FUNCTION NAME(S)
 * dhsTsdDelete - Delete a thread specific data structure.
 * dhsTsdExit - Clean up the thread specific data for the DHS library.
 * dhsTsdGet - Get the thread specific data structure for the current thread.
 * dhsTsdInit - Initailize the thread specific data.
 * dhsTsdNew - Allocate a thread specific data structure.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.7  1997/08/28 22:43:57  nhill
 * Updated some documentation.
 *
 * Revision 1.6  1997/06/09 21:19:16  nhill
 * Changed the hp unix symbol to hpux from HPUX.
 *
 * Revision 1.5  1997/03/26 17:40:45  nhill
 * Fixed some lint problems.
 *
 * Revision 1.4  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.3  1997/01/22 21:34:52  nhill
 * stable checkin.
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


static 	boolean		initialized = FALSE;
				/* Indicate if the data is intialized.	*/
static	pthread_key_t	key;	/* Store the thread specific data key.	*/

static void	dhsTsdDelete( void * );
static tDhsTsd	*dhsTsdNew( DHS_STATUS * );

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTsdDelete
 *
 * INVOCATION:
 * dhsTsdDelete( tsd );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tsd (void *) Pointer to the thread specific data.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Free the memory allocated to a thread specific data structure
 *
 * This function is passed to the pthread_key_create function and
 * is executed automatically whenever a thread exits.
 *
 * DESCRIPTION:
 * The memory is freed using the free function.
 * Note that the function does not use the function entry/function exit
 * message macros. The reason for this is that the messaging uses the 
 * thread specific data, and an error would occur if any attempt is made
 * to write a message after the data has been freed.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The thread specific data must have been initialized with the dhsTsdInit
 * Function.
 *
 * SEE ALSO:
 * - dhsTsdExit
 * - dhsTsdGet
 * - dhsTsdInit
 * - dhsTsdNew
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	dhsTsdDelete
(
    void	*tsd		/* (in)  Tsd structure to delete.	*/
)
{
    if ( tsd != NULL )
    {
	free( tsd );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTsdExit
 *
 * INVOCATION:
 * dhsTsdExit( &status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Clean up the thread specific data for the DHS library
 *
 * DESCRIPTION:
 * The POSIX thread functions are used to clean up the thread specific
 * data.
 *
 * EXTERNAL VARIABLES:
 * - initialized is set to indicate the data is not initialized.
 * - key is used to delete the thread specific data key.
 *
 * PRIOR REQUIREMENTS:
 * The thread specific data must have been initialized with the 
 * dhsTsdInit function.
 *
 * SEE ALSO:
 * - dhsTsdDelete
 * - dhsTsdGet
 * - dhsTsdInit
 * - dhsTsdNew
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsTsdExit
(
    DHS_STATUS	*status		/* (mod) Function return status.	*/
)
{
    fnEntry( "dhsTsdExit" );
    statCheck( *status, VOID );


    /*
     *  Only delete the key if it has been created.
     */

    if ( initialized )
    {
	/* 
	 *  Set the intialized flag to false.
	 */

	initialized = FALSE;


	/*
	 * Delete the thread specific data key.
	 */

#ifndef hpux
	if ( pthread_key_delete( key ) != 0 )
	{
	    dhsMsgFmt( MSG_ERRNO, DHS_DEBUG_OFF, "pthread_key_delete" );
	    dhsMsgFmt( DHS_E_TSD, DHS_DEBUG_OFF );
	}
#endif /* hpux */
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTsdGet
 *
 * INVOCATION:
 * tsd = dhsTsdGet( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DHS_STATUS * ) Function return status.
 *
 * FUNCTION VALUE:
 * (tDhsTsd *) Pointer to the thread specific data.
 *
 * PURPOSE:
 * Return a pointer to the thread specific data for the current thread
 *
 * DESCRIPTION:
 * This function is implemented with the POSIX thread functions.
 *
 * This fucntion cannot use the dhsMsgFmt function to display any 
 * error messages except DHS_E_TSD since all other messages use this 
 * function to get the message buffer, and so display any other error 
 * message would probably result in an never ending recursion of function 
 * calls.
 *
 * EXTERNAL VARIABLES:
 * - initialized is used to verify the thread specific data has been
 *   initialized.
 * - key is used to identify the thread specific data.
 *
 * PRIOR REQUIREMENTS:
 * The thread specific data must have been initialized with the dhsTsdInit 
 * function.
 *
 * SEE ALSO:
 * - dhsTsdDelete
 * - dhsTsdExit
 * - dhsTsdInit
 * - dhsTsdNew
 *
 * DEFICIENCIES:
 * Error messages are not as complete as they could be since the dhsMsgFmt
 * function cannot be used to display the messages.
 *-
 ************************************************************************
 */

tDhsTsd		*dhsTsdGet
(
    DHS_STATUS	*status		/* (mod) Function return status.	*/
)
{
    tDhsTsd	*tsd;		/* Pointer to the thread specific data.	*/
#ifdef hpux
    int		s;
#endif /* hpux */


    /*
     *  Check that the thread specific data has been initialized.
     */

    if ( ! initialized )
    {
	dhsMsgFmt( DHS_E_TSD, DHS_DEBUG_OFF );
	*status = DHS_E_TSD;
	return( NULL );
    }


    /*
     *  Look for an existing copy of the thread specific data struture.
     */

#ifdef hpux
    s = pthread_getspecific( key, (pthread_addr_t *) &tsd );
    if ( s != 0 )
    {
	dhsMsgFmt( MSG_ERRNO, DHS_DEBUG_OFF, "pthread_getspecific"  );
	dhsMsgFmt( DHS_E_THREAD_CREATE, DHS_DEBUG_OFF, s );
	*status = DHS_E_THREAD_CREATE;
	return( NULL );
    }
#else /* hpux */
    tsd = pthread_getspecific( key );
#endif /* hpux */


    /*
     *  If the thread specific data does not exist for this thread, 
     *  alocate and then initialize it now.
     */

    if ( tsd == NULL )
    {
	tsd = dhsTsdNew( status );


	/*
	 *  Set the pointer to the new thread specific data structure.
	 */

	if ( pthread_setspecific( key, tsd ) != 0 )
	{
	    dhsMsgFmt( DHS_E_TSD, DHS_DEBUG_OFF );
	    dhsTsdDelete( tsd );
	    tsd = NULL;
	}
	    
    }

    return( tsd );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTsdInit
 *
 * INVOCATION:
 * dhsTsdInit( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Intialize the thread specific data
 *
 * DESCRIPTION:
 * The POSIX thread library thread specific data functions are used 
 * to implement this function.
 *
 * EXTERNAL VARIABLES:
 * - initialized is used to indicate the thread specific data has been 
 *   initialized.
 * - key is used to save the thread specific data key.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsTsdDelete
 * - dhsTsdExit
 * - dhsTsdGet
 * - dhsTsdNew
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsTsdInit
(
    DHS_STATUS	*status		/* (mod) Function return status.	*/
)
{
    fnEntry( "dhsTsdInit" );
    statCheck( *status, VOID );


    /* 
     *  Only initialize if it hasn't been done already.
     */

    if ( ! initialized )
    {
	/* 
	 *  Create a thread specific data key.
	 */

#ifdef hpux
	if ( pthread_keycreate( &key, dhsTsdDelete ) == 0 )
#else /* hpux */
	if ( pthread_key_create( &key, dhsTsdDelete ) == 0 )
#endif /* hpux */
	{
	    /*
	     * If the key was created successfully, set the initialzied flag.
	     */

	    initialized = TRUE;
	}
	else
	{
	    dhsMsgFmt( MSG_ERRNO, DHS_DEBUG_OFF, "pthread_key_create" );
	    dhsMsgFmt( DHS_E_TSD, DHS_DEBUG_OFF );
	}
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTsdNew
 *
 * INVOCATION:
 * tsd = dhsTsdNew( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (tDhsTsd *) Pointer to the thread specific data.
 *
 * PURPOSE:
 * Allocate a new thread specific data structure
 *
 * DESCRIPTION:
 * The thread specific data is allocated using malloc.
 *
 * The messages generated by this function are limited since this is called
 * by dhsTsdGet, which is in turn called by the dhsMsgFmt function. Messages
 * sent from this function could result in an infinite recursive loop.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsTsdDelete
 * - dhsTsdExit
 * - dhsTsdGet
 * - dhsTsdInit
 * - dhsTsdNew
 *
 * DEFICIENCIES:
 * The error messages are limited due to the restrictions on using dhsMsgFmt.
 *-
 ************************************************************************
 */

static tDhsTsd	*dhsTsdNew
(
    DHS_STATUS	*status		/* (in)  Function return status.	*/
)
{
    tDhsTsd	*tsd;		/* The new tsd structure.		*/


    /*
     *  Allocate the memory for the tsd structure.
     */

    nullCheck( tsd = malloc( sizeof( tDhsTsd ) ), *status, NULL );
    tsd->dtMsgString[0] = '\0';
    dhsMsgInit( &( tsd->dtMsg ) );
    tsd->dtMsg.dmMsg = tsd->dtMsgString;
    tsd->dtMsgLast = &( tsd->dtMsg );

    return( tsd );
}
