static char rcsid[] = "$Id: bulkTransfer.c,v 1.1.1.1 2002-11-24 20:19:45 brighton Exp $";
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
 * dhs/src/bulkTransfer.c
 *
 * PURPOSE:
 * Contains routines to keep track of all bulk transfers currently in
 * progress.
 *
 * FUNCTION NAME(S)
 * btCompare - Compare two bulk transfer structures so they can be inserted
 * 		in the correct place in the eptr list.
 * dhsBtDelete - Remove a bulk transfer structure from the eptr list and
 *		delete its memory.
 * dhsBtfind - Find a bulk transfer structure in the eptr list, searching
 *		by various criteria.
 * dhsBtNew - Create a new bulk transfer structure and add it to the eptr 
 *		list.
 * init - Initialize the static items in this file.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.4  2001/03/27 20:24:01  nhill
 * Added vxWorks.h when compiling for vxworks.
 *
 * Revision 1.3  1998/05/27 16:28:17  nhill
 * Fixed a bug.
 *
 * Revision 1.2  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.1  1997/01/22 21:36:02  nhill
 * Initial revision
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


/*
 ************************************************************************
 *  Static variables.
 ************************************************************************
 */

static EPTR		btList;		/* List of all bulk transfer 	*/
					/* structures.			*/
static boolean		initialized = FALSE;
					/* Have the static variables	*/
					/* been initialized?		*/
static tDhsMutex	mutex;		/* Mutex to limit access to	*/
					/* the bdList eptr list.	*/


/*
 ************************************************************************
 *  Static functions.
 ************************************************************************
 */

static int		btCompare( tDhsBt *, tDhsBt * );
static void		init( DHS_STATUS * );

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * btCompare
 *
 * INVOCATION:
 * diff = btCompare( pDhsBt1, pDhsBt2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pDhsBt1 (tDhsBt *) First structure.
 * (>) pDhsBt2 (tDhsBt *) Second structure.
 *
 * FUNCTION VALUE:
 * (int) < 0 if *pDhsBt1 < *pDhsBt2, 0 if *pDhsBt1 == *pDhsBt2,
 *		> 0 if *pDhsBt1 > *pDhsBt2.
 *
 * PURPOSE:
 * Compare two tDhsBt structures for eptrSearch
 *
 * DESCRIPTION:
 * Get the tags of the structures and return the difference.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Both bulk data structures should have been created with dhsBtNew.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static int	btCompare
(
    tDhsBt	*pDhsBt1,	/* (in)  First structure to compare.	*/
    tDhsBt	*pDhsBt2	/* (in)  Second structure to compare.	*/
)
{
    return( dhsBtTag( pDhsBt1 ) - dhsBtTag( pDhsBt2 ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBtDelete
 *
 * INVOCATION:
 * dhsBtDelete( pDhsBt, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pDhsBt (tDhsBt *) Pointer to be deleted.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Delete a tDhsBt structure created with dhsBtNew
 *
 * DESCRIPTION:
 * If the shared memory is in use, free it, and then free the memory
 * allocated to the structure and remove it from the list of structures.
 *
 * EXTERNAL VARIABLES:
 * - btList: List of all bulk transfer structures.
 * - mutex: Used to limit access to the static variables.
 *
 * PRIOR REQUIREMENTS:
 * The structure must have been created with the dhsBtNew function.
 *
 * DEFICIENCIES:
 * Doesn't bother to check to make sure things have been initialized.
 * This shouldn't be required if the function is used correctly.
 *-
 ************************************************************************
 */

void		dhsBtDelete
(
    tDhsBt	*pDhsBt,	/* (in)  The structure to delete.	*/
    DHS_STATUS	*pStatus	/* (in)  Function return status.	*/
)
{
    IMP_Status	impStatus;	/* Imp function return status.		*/
    int		index;		/* Index in the list.			*/


    fnEntry( "dhsBtDelete" );
    statCheck( *pStatus, VOID );


    /*
     *  If the shared memory is in use, free it.
     */

    if ( pDhsBt->dbSharedMemUsed )
    {
	impStatus = IMP_STATUS_OK;
	impCheck( ImpReleaseShared( &( pDhsBt->dbSharedMem ), TRUE, 
		&impStatus ), impStatus, *pStatus, VOID );
    }


    /*
     *  Find the object in the eptr list and remove it. This is protected
     *  by the mutex to ensure no other thread is modifying the list while
     *  this occurs.
     */

    dhsMutexLock( &mutex, pStatus );
    if ( eptr_search( &btList, pDhsBt, &index, btCompare ) )
    {
	eptr_del( &btList, index, 1, NULL );
    }
    dhsMutexUnlock( &mutex, pStatus );


    /*
     *  Free the memory allocated to the structure.
     */

    free( pDhsBt );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBtFind
 *
 * INVOCATION:
 * dhsBt = dhsBtFind( type, &status, ... );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) type (tDhsBtFind) How to look for the structure.
 * (!) pStatus (DHS_STATUS *) Function return status.
 * (>) ... () Optional arguments.
 *
 * FUNCTION VALUE:
 * (tDhsBt) The object found.
 *
 * PURPOSE:
 * Search the list of tDhsBt structures
 *
 * DESCRIPTION:
 * Look through the list of structures until one is found.
 *
 * EXTERNAL VARIABLES:
 * - btList: List of all bulk transfer structures.
 * - intialized: Indicates if the static variables are initialized.
 * - mutex: Used to limit access to the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

tDhsBt		*dhsBtFind
(
    tDhsBtFind	type,		/* (in)  Type of search to do.		*/
    DHS_STATUS	*pStatus,	/* (mod) Function return status.	*/
    ...				/*	 values to search for.		*/
)
{
    va_list	args;		/* The variable arguments.		*/
    tDhsBtTag	btTag;		/* The bulk transfer tag.		*/
    int		index;		/* Position of the tag in the list.	*/
    tDhsBt	*pDhsBt;	/* The located structure.		*/
    DHS_STATUS	tStatus;	/* A temporary status value.		*/


    fnEntry( "dhsBtFind" );
    statCheck( *pStatus, NULL );


    /*
     *  Make sure the static variables are initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, NULL );
    }


    /*
     *  How the search is done depends on the search type.
     */

    va_start( args, pStatus );
    switch( type )
    {
	case	DHS_BF_BT_TAG:
	    /*
	     *  Search by tag. Extract the tag from the argument list.
	     */

	    btTag = va_arg( args, tDhsBtTag );


	    /*
	     * eptr_search can be used to look for a structure with a matching
	     * tag.  The search is protected by the mutex to ensure another
	     * thread doesn't modify the list durring the search.
	     */

	    tStatus = DHS_S_SUCCESS;
	    dhsMutexLock( &mutex, &tStatus );
	    if ( eptr_search( &btList, dhsBtPtr( btTag ), &index, 
		    btCompare ) )
	    {
		/*
		 * Matching object was found.
		 */

		pDhsBt = eptritem( &btList, index );
	    }
	    else
	    {
		/*
		 * No matching object was found.
		 */

		*pStatus = DHS_E_BT_NOT_FOUND;
		pDhsBt = NULL;
	    }
	    dhsMutexUnlock( &mutex, &tStatus );
	    if ( tStatus != DHS_S_SUCCESS )
	    {
		*pStatus = tStatus;
	    }
	    break;
	default:
	    /*
	     *  Unrecognized search type.
	     */

	    dhsMsgFmt( DHS_E_BT_FIND, DHS_DEBUG_OFF, type );
	    pDhsBt = NULL;
    }
    va_end( args );

    fnReturn( pDhsBt );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBtNew
 *
 * INVOCATION:
 * pDhsBt = dhsBtNew( pConnect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) Pointer to the connection for the transfer.
 * (!) pStatus (DHS_STATUS *) function return status.
 *
 * FUNCTION VALUE:
 * (tDhsBt *) Pointer to the new structure.
 *
 * PURPOSE:
 * Allocate and initialize a new tDhsBt structure
 *
 * DESCRIPTION:
 * Allocate the new structure and add it to the eptr list.
 *
 * EXTERNAL VARIABLES:
 * - btList: List of all bulk transfer structures.
 * - intialized: Indicates if the static variables are initialized.
 * - mutex: Used to limit access to the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

tDhsBt		*dhsBtNew
(
    tDhsConnect	*pConnect,	/* (in)  Connetion for the transfer.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		index;		/* Position of the structure in the list*/
    tDhsBt	*pBt;		/* The new structure.			*/
    DHS_STATUS	tStatus;	/* Temporary status value.		*/

    fnEntry( "dhsBtNew" );
    statCheck( *pStatus, NULL );


    /*
     *  Make sure the static variables are initialized.
     */

    if ( ! initialized )
    {
	check( init( pStatus ), *pStatus, NULL );
    }


    /*
     *  Allocate memory for a new structure.
     */

    nullCheck( pBt = malloc( sizeof( tDhsBt ) ), *pStatus, NULL );


    /*
     *  Initialize the structure members.
     */

    pBt->dbSharedMemUsed = FALSE;
    pBt->dbConnect = pConnect;


    /*
     *  Add the structure to the eptr list. This is protected by the mutex
     *  to ensure no other thread is modifying or accessing the list while
     *  the insert occurs.
     */

    tStatus = DHS_S_SUCCESS;
    dhsMutexLock( &mutex, &tStatus );
    (void) eptr_search( &btList, pBt, &index, btCompare );
    nullCheck( eptr_insert( &btList, index, pBt ), *pStatus, NULL );
    dhsMutexUnlock( &mutex, &tStatus );
    if ( tStatus != DHS_S_SUCCESS )
    {
	*pStatus = tStatus;
    }

    fnReturn( pBt );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * init
 *
 * INVOCATION:
 * init( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Initialize the static variables in this file
 *
 * DESCRIPTION:
 * Initialize the mutex and eptr list.
 *
 * EXTERNAL VARIABLES:
 * - btList: List of all bulk transfer structures.
 * - intialized: Indicates if the static variables are initialized.
 * - mutex: Used to limit access to the static variables.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	init
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "init" );

    nullCheck( eptr_init( &btList, 5 ), *pStatus, VOID )
    dhsMutexInit( &mutex, pStatus );

    initialized = TRUE;

    fnReturn( VOID );
}
