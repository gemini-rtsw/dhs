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
 * dhs/h/rwLock.h
 *
 * PURPOSE:
 * Implements read/write locks.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:56  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:59  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1997/02/04 20:23:33  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef RWLOCK_H
#define	RWLOCK_H


/*
 ************************************************************************
 *  Type definitions.
 ************************************************************************
 */

typedef struct rwLock
{
    tDhsCond		rwlCond;	/* condition variable for the lock.*/
    int			rwlReaders;	/* Number of readers.		*/
    int			rwlWriters;	/* Number of writers.		*/
} tDhsRwLock;


/*
 ************************************************************************
 *  Macro definitions.
 ************************************************************************
 */

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsRwlDestroy
 *
 * INVOCATION:
 * dhsRwlDestroy( &lock, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pLock (tDhsRwLock *) The lock to Destroy.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Destroy a read/write lock
 *
 * DESCRIPTION:
 * Destroy the condition variable.
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

#define	dhsRwlDestroy( pLock, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    dhsCondDestroy( &( (pLock)->rwlCond ), pStatus );		\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsRwlInit
 *
 * INVOCATION:
 * dhsRwlInit( &lock, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pLock (tDhsRwLock *) The lock to initialize.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Initalize a read/write lock
 *
 * DESCRIPTION:
 * Initialize the condition variable, set both counters to zero.
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

#define	dhsRwlInit( pLock, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    dhsCondInit( &( (pLock)->rwlCond ), pStatus );		\
	    (pLock)->rwlReaders = 0;					\
	    (pLock)->rwlWriters = 0;					\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsRwlRLock
 *
 * INVOCATION:
 * dhsRwlRLock( &lock, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pLock (tDhsRwLock *) The lock to lock.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Get a read lock on a read/write lock
 *
 * DESCRIPTION:
 * Wait for no writers and increment the reader count.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The lock must be initalized with dhsRwlInit.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsRwlRLock( pLock, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    dhsCondLock( &( (pLock)->rwlCond ), pStatus );		\
	    while ( (pLock)->rwlWriters != 0 )				\
	    {								\
		dhsCondWait( &( (pLock)->rwlCond ), pStatus );		\
	    }								\
	    (pLock)->rwlReaders++;					\
	    dhsCondUnlock( &( (pLock)->rwlCond ), pStatus );		\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsRwlUnlock
 *
 * INVOCATION:
 * dhsRwlUnlock( &lock, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pLock (tDhsRwLock *) The lock to unlock.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Unlock a read/write lock
 *
 * DESCRIPTION:
 * Wait for no writers and increment the reader count.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The lock must be initalized with dhsRwlInit.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsRwlUnlock( pLock, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    dhsCondLock( &( (pLock)->rwlCond ), pStatus );		\
	    (pLock)->rwlReaders --;					\
	    if ( (pLock)->rwlWriters > 0 )				\
	    {								\
		(pLock)->rwlWriters--;					\
	    }								\
	    dhsCondUnlock( &( (pLock)->rwlCond ), pStatus );		\
	    dhsCondBroadcast( &( (pLock)->rwlCond ), pStatus );		\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsRwlWLock
 *
 * INVOCATION:
 * dhsRwlWLock( &lock, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pLock (tDhsRwLock *) The lock to lock.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Get a write lock on a read/write lock
 *
 * DESCRIPTION:
 * Wait for no writers and increment the reader count.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The lock must be initalized with dhsRwlInit.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsRwlWLock( pLock, pStatus )					\
	if ( *(pStatus) == DHS_S_SUCCESS )				\
	{								\
	    dhsCondLock( &( (pLock)->rwlCond ), pStatus );		\
	    while ( (pLock)->rwlWriters != 0 || (pLock)->rwlReaders != 0 )\
	    {								\
		dhsCondWait( &( (pLock)->rwlCond ), pStatus );		\
	    }								\
	    (pLock)->rwlWriters++;					\
	    (pLock)->rwlReaders++;					\
	    dhsCondUnlock( &( (pLock)->rwlCond ), pStatus );		\
	}

#endif /* RWLOCK_H */
