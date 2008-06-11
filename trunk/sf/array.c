static char rcsid[] = "$Id: array.c,v 1.1.1.1 2002-11-24 20:35:15 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1999				(c) 1999
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
 * sf/src/array.
 *
 * PURPOSE:
 * Routines that are used to manipulate the eptr list of "array" attributes
 * found for a given dataset, frames, and subframes.
 *
 * FUNCTION NAME(S)
 * sfArrayAdd	    :	Add a new "array" element to an array List.
 * sfArrayFind	    :   Find a particular array element in the array list.
 * sfArrayListFree  :	Free an entire array list from memory.
 * sfArrayListNew   :	Create a new array list.
 * sfArrayDel	    : 	Remove an "array" element from an array List.
 * sfFindLastExtVer : 	Find the last extension verions value for a given
 *			extension name.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1999/02/09 20:28:35  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/
/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"


/*
 ************************************************************************
 *  sf library include files.
 ************************************************************************
 */

#include "sf.h"
#include "localSf.h"
#include "array.h"

void        sfFormatMessage();

static long	sfFindLastExtVer( EPTR*, char[], SF_STATUS* );

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfArrayAdd
 *
 * INVOCATION:
 * sfArrayAdd( arrayList, newArray, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) arrayList    (EPTR**)		List of frames.
 * (>) extName	    (char[SF_EXTNAME_SIZE)	An array info structure.
 * (>) sdsId	    (DHS_BD_ATTRIB_ID)	SDS identifier for the attribute.
 * (!) sfStatus	    (SF_STATUS*) 	Function return value.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To add a new array element to an array list.
 *
 * DESCRIPTION:
 * Find the extension version value for this new array element.
 * Insert the array element in the proper position.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * sfArrayDelete
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfArrayAdd
(
    EPTR	**arrayList, 	/* (in)  arrayList to add to.		*/
    DHS_BD_ATTRIB_ID
    		sdsId,		/* (in)  Unique attribute ID.		*/
    char	extName[SF_EXTNAME_SIZE],
    				/* (in)  Name of the extension.		*/
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)
{
    SF_ARRAY_INFO	*newArray;	/* New array element.		*/

    
    fnEntry( "sfArrayAdd", *sfStatus );
    statCheck( *sfStatus, VOID );
    

    /*
     * Allocate memory for the new array element.
     */

    nullCheck( newArray = malloc( sizeof( *newArray ) ), *sfStatus, VOID );


    /*
     * Initialize the values of the array element.
     */

    strnCpy( newArray->extName, extName, SF_EXTNAME_SIZE, *sfStatus, VOID );
    newArray->sdsId = sdsId;
    check( newArray->extVer = sfFindLastExtVer( *arrayList, extName, sfStatus )
	    + 1, *sfStatus, VOID ) ;
    

    /*
     * Add the new array element to the array list..
     */
    
    nullCheck( *arrayList = eptr_insert( *arrayList, eptrlen( *arrayList ),
	    newArray ), *sfStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfArrayFind
 *
 * INVOCATION:
 * array = sfArrayFind( arrayList, extName, extVer, sfStatue );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) arrayList    (EPTR *)		List of array elements.
 * (>) extName	    (char[SF_EXTNAME_SIZE])Extension name to look for.
 * (>) extVer	    (long)		Extension version number
 * (!) sfStatus	    (SF_STATUS*)	SF library Status.
 *
 * FUNCTION VALUE:
 * NULL           : If there is no array element which matches the
 *		    given extName and extVer values.
 * SF_ARRAY_INFO *: The first array element that matches
 *
 * PURPOSE:
 * To find an array element, in the given list, which matches the given
 * extension name and version.
 *
 * If the extseion version value given is "SF_NULL_EXTVERSION" then the
 * first array element which matches the given extname  is returned.
 *
 * DESCRIPTION:
 * Loop through the array list and try and match the given values with
 * the array elements found in the array list.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * An eptr search would be much more efficient but the list needs to be
 * sorted, which it is not.  If the list is sorted then order in which
 * the attributes were found is not maintained.
 *-
 ************************************************************************
 */

SF_ARRAY_INFO *	sfArrayFind
(
    EPTR	*arrayList,	/* (in)  The list to search.		*/
    char	extName[SF_EXTNAME_SIZE],
	    			/* (in)	 The extension name to look for.*/
    long	extVer,		/* (in)	 The extension version to find.	*/
    SF_STATUS   *sfStatus	/* (mod) SF library status.		*/
)  
{
    SF_ARRAY_INFO	*arrayItem;	/* An item found in the array.	*/
    boolean		found;		/* TRUE if match is found.	*/
    int			index;		/* List index.			*/

    fnEntry( "sfArrayFind", *sfStatus );
    statCheck( *sfStatus,  NULL );

    nullCheck( arrayList, *sfStatus, NULL );
    
    
    for ( found = FALSE, index = 0;
	  !found && index < eptrlen( arrayList );
	  index ++ )
    {
	nullCheck( arrayItem = eptritem( arrayList, index ), *sfStatus, NULL );
	if ( streq( arrayItem->extName, extName ) )
	{
	    if ( extVer == SF_EXTVERSION_NULL || arrayItem->extVer == extVer )
	    {
		found = TRUE;
	    }
	}
    }
    

    if ( found )
    {
	return( arrayItem );
    }
    else
    {
	return( NULL );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfArrayListFree
 *
 * INVOCATION:
 * sfArrayListFree( arrayList, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) arrayList    (eptr *)		List of array elements.
  * (!) sfStatus	    (SF_STATUS*)	SF library Status.
  *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To free all memory that is being used by an array list.
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
 * SEE ALSO:
 * sfArrayListNew
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfArrayListFree
(
    EPTR	**arrayList	/* (in)  The list to search.		*/
)
{
    SF_STATUS	sfStatus = SF_S_SUCCESS;
    				/* SF library status (not used).	*/

    fnEntry( "sfArrayListFree", sfStatus );
    
    if ( *arrayList != NULL )
    {
	eptr_free( *arrayList, free );
	*arrayList = NULL;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfArrayListNew
 *
 * INVOCATION:
 * arrayList = sfArrayListNew();
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * None.
 *
 * FUNCTION VALUE:
 * NULL	   : If unable to create a new array List.
 * EPTR *  : If able to create a new array List.
 *
 * PURPOSE:
 * To create a new array List.
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
 * SEE ALSO:
 * sfArrayListFree
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

EPTR	* sfArrayListNew
(
    void
)
{
    SF_STATUS	sfStatus = SF_S_SUCCESS;
    				/* SF library status (not used).	*/
     
     fnEntry( "sfArrayListNew", sfStatus );

     fnReturn( eptr_alloc( 5 ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfArrayDel
 *
 * INVOCATION:
 * sfArrayDel( arrayList, extName, extVer, sfStatue );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) arrayList    (EPTR *)		List of array elements.
 * (>) extName	    (char[SF_EXTNAME_SIZE])Extension name to look for.
 * (>) extVer	    (long)		Extension version number
 * (!) sfStatus	    (SF_STATUS*)	SF library Status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To delete an array element from the given list.  The element must
 * match the given given extension name and version.
 *
 * If the extension version value given is "SF_EXTVERSION_NULL" then the
 * first array element which matches the given extname  is returned.
 *
 * DESCRIPTION:
 * Loop through the array list and try and match the given values with
 * the array elements found in the array list.  If one is found then remove
 * the element from the array.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * An eptr search would be much more efficient but the list needs to be
 * sorted, which it is not.  If the list is sorted then order in which
 * the attributes were found is not maintained.
 *-
 ************************************************************************
 */

void	sfArrayDel
(
    EPTR	**arrayList,	/* (in)  The list to search.		*/
    char	extName[SF_EXTNAME_SIZE],
	    			/* (in)	 The extension name to look for.*/
    long	extVer,		/* (in)	 The extension version to find.	*/
    SF_STATUS   *sfStatus	/* (mod) SF library status.		*/
)  
{
    SF_ARRAY_INFO	*arrayInfo;	/* An item found in the array.	*/
    boolean		found;		/* TRUE if match is found.	*/
    int			index;		/* List index.			*/

    fnEntry( "sfArrayDel", *sfStatus );
    statCheck( *sfStatus, VOID );

    nullCheck( *arrayList, *sfStatus, VOID );
    
    
    for ( found = FALSE, index = 0;
	  !found && index < eptrlen( *arrayList );
	  index ++ )
    {
	nullCheck( arrayInfo = eptritem( *arrayList, index ), *sfStatus, VOID );
	if ( streq( arrayInfo->extName, extName ) )
	{
	    if ( extVer == SF_EXTVERSION_NULL || arrayInfo->extVer == extVer )
	    {
		eptr_del( *arrayList, index, index, free );
	    }
	}
    }
    

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFindLastExtVer
 *
 * INVOCATION:
 * extVer = sfFindLastExtVer( arrayList, extName, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) arrayList    (EPTR**)		List of frames.
 * (>) extName	    (char[SF_EXTNAME_SIZE)	An array info structure.
 * (!) sfStatus	    (SF_STATUS*) 	Function return value.
 *
 * FUNCTION VALUE:
 * SF_EXTVERSION_NULL : If not found.
 * long		  : If found.
 *
 * PURPOSE:
 * To find the greatest extension version number of all the array elements
 * which have the same extension name as the one given.
 *
 * DESCRIPTION:
 * Loop through the list keeping track of the largest extension version 
 * value found.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * Would be nice to use an eptr search but since we want to preserver the
 * order of the array elements we can't.  An ept search would most likely
 * be faster.
 *-
 ************************************************************************
 */

long	sfFindLastExtVer
(
    EPTR	*arrayList, 	/* (in)  arrayList to add to.		*/
    char	extName[SF_EXTNAME_SIZE],
    				/* (in)  Name of the extension.		*/
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)
{
    SF_ARRAY_INFO	*arrayItem;	/* An item found in the array.	*/
    long		lastExtVer;	/* Larget extension ver. found.	*/
    int			index;		/* List index.			*/

    fnEntry( "sfArrayFind", *sfStatus );
    statCheck( *sfStatus, 0 );

    nullCheck( arrayList, *sfStatus, 0 );
    

    
    for( lastExtVer = SF_EXTVERSION_NULL, index = eptrlen( arrayList ) - 1;
	 index >= 0; index -- )
    {
	nullCheck( arrayItem = eptritem( arrayList, index ), *sfStatus, 0 );
	if ( streq( arrayItem->extName, extName ) &&
		arrayItem->extVer > lastExtVer )
	{
	    lastExtVer = arrayItem->extVer;
	}
    }

    fnReturn( lastExtVer );
}
