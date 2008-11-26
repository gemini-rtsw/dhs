static char rcsid[] = "$Id: extension.c,v 1.1.1.1 2002-11-24 20:35:18 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) <year>				(c) <year>
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6		Ottawa, Canada, K1A 0R6
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
 *					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * sfLibrary/src/eptr.c
 *
 * PURPOSE:
 * Routines that manipulate the eptr list that describes some of the 
 * information for writing FITS files.	This list is used to make the
 * writing of the FITS files as efficient as possible.
 *
 * FUNCTION NAME(S)
 *
 * sfAddExt	:	Add a new extension node to the extension
 *			list in a dataset information structure.
 * sfDelExt	:	Remove an extension node to the extension
 *			list in a dataset information structure.
 * sfDsInfoFree	:	Free a dataset information structure from
 *			memory.
 * sfDsInfoInit:	Create a new dataset informaiton structure.
 * sfExtCompare	:	Compare two extension identifiers for an
 *			"eptr_search" 
 * sfFindExt	:	To find a particular extension in an
 *			extension list.
 *
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.10  1998/05/05 16:23:55  jaeger
 * Added support for sorting the extension list by frameId and by
 * extname + extver.  This requires two EPTR lists which point to
 * the same memory.
 *
 * Revision 1.9  1997/12/02 21:01:45  jaeger
 * Moved error message for not finding and extension out of the find
 * routine, since it isn't always an error if it is not found.
 *
 * Revision 1.8  1997/11/21 00:21:48  jaeger
 * Fixed problem with data being separated from headers during composition.
 *
 * Revision 1.7  1997/11/19 00:12:18  jaeger
 * Added sorting of "extension" list in the dataset information, removed
 * sfPrintMessage, and fixed bug in fits2sds (big dataset did not have
 * the proper frame structure).
 *
 * Revision 1.6  1997/09/05 18:58:07  jaeger
 * Fixed some memory leaks
 *
 * Revision 1.5  1997/08/22 22:10:35  jaeger
 * String conversion
 *
 * Revision 1.4  1997/08/19 19:18:36  jaeger
 * Fixed bugs for install
 *
 * Revision 1.3  1997/08/15 20:43:37  jaeger
 * Checked in for install
 *
 * Revision 1.2  1997/08/12 21:07:38  jaeger
 * Checked in for install
 *
 * Revision 1.1  1997/08/06 18:30:48  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


/*
 ************************************************************************
 *  standard C libararies 
 ************************************************************************
 */

#include <string.h>


/*
 ************************************************************************
 *  CADC Library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_eptr.h"

/*
 ************************************************************************
 *  local sf library specific include files.
 ************************************************************************
 */
#include "sf.h"
#include "localSf.h"
#include "extension.h"


/*
 ************************************************************************
 *  External Function prototypes.
 ************************************************************************
 */

void	sfFormatMessage();

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddExt
 *
 * INVOCATION:
 * sfAddExt( datasetInformation, extensionId, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	dsInfo	    (SF_DS_INFO*)	Dataset information.
 * (>)	frmId	    (char[FLEN__VALUE])	Eextension Identifier.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Add a new extension node to an extension List.
 *
 * DESCRIPTION:
 * 1. Allocate memory for the new extension and initialize it.
 * 2. Add the new extension to the dataset information.
 *
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * The dataset information structure exists.
 *
 * SEE ALSO:
 * - sfDelExt.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

SF_EXT_INFO *sfAddExt
(
    SF_DS_INFO	*dsInfo,	    /* (in)  Dataset information.	*/
    char	frmId[FLEN_VALUE],  /* (in)  Heirarchical frame id.	*/
    SF_STATUS	*sfStatus	    /* (mod) SF library status.		*/
)
{
    SF_EXT_INFO *newExt;	    /* New extension node.		*/
    
    fnEntry( "sfAddExt", *sfStatus );
    statCheck( *sfStatus, NULL );


    /*
     * Allocate memory for the new extension.
     */
    
    nullCheck( newExt = malloc( sizeof( *newExt) ), *sfStatus, NULL );


    /*
     * Fill in the values for the eptr item.
     */

    strnCpy( newExt->frmName, "\0", FLEN_VALUE, *sfStatus, NULL );
    strnCpy( newExt->frmId, frmId, FLEN_VALUE, *sfStatus, NULL );
    strnCpy( newExt->extName, "", FLEN_VALUE, *sfStatus, NULL );
    newExt->extVer = SF_NULL_EXTVER;
    newExt->extWritten = FALSE;
    newExt->numKeywords = NULL_KEYWORDS;
    newExt->type = NULL_EXT_TYPE;
    newExt->fitsIndex = NULL_INDEX;
    newExt->bitpix = 16;
    newExt->naxis = 0;
    newExt->naxes[0] = newExt->naxes[1] = newExt->naxes[2] = newExt->naxes[3]=0;
    newExt->naxes[4] = newExt->naxes[5] = newExt->naxes[6] = 0;
    
    
    /*
     * Add the new extension to the data set extension information
     */
    
    if ( dsInfo->exts == NULL )
    {
	nullCheck( dsInfo->exts = eptr_alloc( 1 ), *sfStatus, NULL );
    }
    nullCheck( dsInfo->exts = eptr_insert( dsInfo->exts,
	    eptrlen( dsInfo->exts ), (pointer) (newExt) ), *sfStatus, NULL );
    nullCheck( dsInfo->exts = eptr_sort( dsInfo->exts, sfExtCompare ),
	    *sfStatus, NULL );

    fnReturn( newExt );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddExtVer
 *
 * INVOCATION:
 * sfAddExtVer( dsInfo, ext, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) dsInfo	(SF_DS_INFO*)	Dataset information struct.
 * (>) ext	(SF_EXT_INFO*)	Extension to add.
 * (!) sfStatus	(SF_STATUS*)	Function return value.
 *
 * FUNCTION VALUE:
 * NULL		: If an error occurs.
 * SF_EXT_INFO*	: The extension that was added.
 *
 * PURPOSE:
 * To add an existing extension information to a datasets list
 * of of extension versions.
 *
 * DESCRIPTION:
 * ???
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * ???
 *
 * SEE ALSO:
 * ???
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

SF_EXT_INFO*	sfAddExtVer
(
    SF_DS_INFO	*dsInfo,	/* (in)  Dataset information.		*/
    SF_EXT_INFO	*ext,		/* (in)  Extension that is to be added.	*/
    SF_STATUS	*sfStatus	/* (mod) Function return value.		*/
)
{
    fnEntry( "sfAddExtVer", *sfStatus );
    statCheck( *sfStatus, NULL );
    

    if ( dsInfo->extVers == NULL )
    {
	nullCheck( dsInfo->extVers = eptr_alloc( 1 ), *sfStatus, NULL );
    }

    ext->extVer = sfFindLastExtVer( dsInfo->extVers, ext->extName, sfStatus)
	+ 1;
    nullCheck( dsInfo->extVers = eptr_insert( dsInfo->extVers,
	    eptrlen( dsInfo->extVers ), (pointer) (ext) ), *sfStatus, NULL );
    nullCheck( dsInfo->extVers = eptr_sort( dsInfo->extVers,
	    sfExtVerCompare ), *sfStatus, NULL );


    fnReturn( ext );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfDelExt
 *
 * INVOCATION:
 * sfDelExt( extensionList, extensionId, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	extList	    (EPTR*)		The extension list.
 * (>)	frmId	    (char[FLEN_VALUE])	Extension identifier.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Remove a particular extension from the extension list in a
 * dataset information structure.
 *
 * DESCRIPTION:
 * 1. If the extension is found delete it from the extensionList
 *    otherwise set sfStatus to SF_E_EXT_NOT_FOUND.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfAddExt.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
void sfDelExt
(
    EPTR	*extList,	    /* (in)  Extension list.		*/
    EPTR	*extVerList,	    /* (in)  Extension Version list.	*/
    SF_EXT_INFO	*ext,		    /* (in)  Extension to be removed.	*/
    SF_STATUS	*sfStatus	    /* (mod) SF library status.		*/
)
{
    int		index;		    /* Index into extension list.	*/
    
    fnEntry( "sfDelExt", *sfStatus );
    statCheck( *sfStatus, VOID );
    
    if ( eptr_search( extList, (pointer)ext, &index, sfExtCompare ) )
    {
	/*
	 *   Found it, so delete it.
	 */

	eptr_del( extList, index, 1, free );
    }
    else
    {
	*sfStatus = SF_E_EXT_NOT_FOUND;
	sfFormatMessage( *sfStatus, ext->frmId );
    }

    
    if ( eptr_search( extList, (pointer)ext, &index, sfExtVerCompare ) )
    {
	/*
	 *   Found it, so delete it.
	 */

	eptr_del( extList, index, 1, free );
    }
    else
    {
	*sfStatus = SF_E_EXT_NOT_FOUND;
	sfFormatMessage( *sfStatus, ext->frmId );
    }

    gen_free( ext );
    ext = NULL;
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfDsInfoFree
 *
 * INVOCATION:
 * sfDsInfoFree( dsInfo );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!)	dsInfo	(SF_DS_INFO*)	Dataset information structure.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To free all of the memory used by a dataset information structure
 * (SF_DS_INFO).
 *
 * DESCRIPTION:
 * Free the dataset's extension list from memory.
 * Free the dataset from memory.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - sfDsInfoInit.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfDsInfoFree
(
    SF_DS_INFO	*dsInfo		/* (mod) Dataset information structure	*/
)
{
    SF_STATUS	sfStatus = SF_S_SUCCESS;
    				/* SF library status (not used).	*/
    
    fnEntry( "sfDsInfoFree", sfStatus );
    
    if ( dsInfo->exts != NULL )
    {	
	eptr_free( dsInfo->exts, free );
	dsInfo->exts = NULL;
    }

    if ( dsInfo->extVers != NULL )
    {
	gen_free( dsInfo->extVers );
	dsInfo->extVers = NULL;
    }

    gen_free( dsInfo );
    dsInfo = NULL;
    
    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfDsInfoInit
 *
 * INVOCATION:
 * dsInfo = sfDsInfoInit( datasetName, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	name	    (const char*)	Dataset name.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * (dhInfo*) : NULL if an error occured, otherwise a pointer to the new
 *		  dataset information structure.
 *
 * PURPOSE:
 * Create a new dataset information structure.
 *
 * DESCRIPTION:
 * Allocate memory for the dataset information structure and initialize
 * it's values.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfDsInfoFree.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

SF_DS_INFO   *sfDsInfoInit
(
    const char	*name, 		/* (in)  Dataset name.			*/
    SF_STATUS	*sfStatus	/* (mod) SF library status.		*/
)
{
    SF_DS_INFO	*dsInfo;	/* New dataset information		*/
    
    fnEntry( "sfDsInfoInit", *sfStatus );

    
    /*
     * Allocate memory.
     */
    
    nullCheck( dsInfo = malloc( sizeof( SF_DS_INFO ) ), *sfStatus, NULL );


    /*
     * Initialize the dataset information.
     */
    
    strnCpy( dsInfo->name, name, NAME_SIZE - 1, *sfStatus, NULL );
    dsInfo->numKeywords = NULL_KEYWORDS ;
    dsInfo->last = NULL_INDEX;
    strnCpy( dsInfo->instrument, "", FLEN_VALUE, *sfStatus, NULL );
    dsInfo->exts = NULL;
    dsInfo->extVers = NULL;
    
    fnReturn( dsInfo );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfExtCompare
 *
 * INVOCATION:
 * difference = sfExtCompare( ext1, ext2 )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output) *
 * (>)	ext1	(SF_EXT_INFO*)		Extension to find.	
 * (>)	ext2	(SF_EXT_INFO*)		The strucutre to compare to.	
 *
 * FUNCTION VALUE:
 * (int) < 0  : if extId1 < extId2
 * (int) 0    : if extId1 == extId2
 * (int) > 0  : if extId1 > extId2
 *
 * PURPOSE:
 * Compare two extension identifiers for eptr_search
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
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

int 	sfExtCompare
(
    SF_EXT_INFO *ext1,	   /* (in) the strucutre to compare to.	*/
    SF_EXT_INFO *ext2	   /* (in) the strucutre to compare to.	*/
)
{
     return( strncmp( ext1->frmId, ext2->frmId, FLEN_VALUE ) );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfExtVerCompare
 *
 * INVOCATION:
 * difference = sfExtVerCompare( ext1, ext2 )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output) *
 * (>)	ext1	(SF_EXT_INFO*)		Extension Version to find.	
 * (>)	ext2	(SF_EXT_INFO*)		The strucutre to compare to.	
 *
 * FUNCTION VALUE:
 * (int) < 0  : if ext1 < ext2
 * (int) 0    : if ext1 == ext2
 * (int) > 0  : if ext1 > ext2
 *
 * PURPOSE:
 * Compare two extension identifiers for eptr_search, find one that
 * matches this particular extension version.
 *
 * DESCRIPTION:
 * Compare the extNames
 * If the extNames are the same then compare the extVers
 *
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
 * None.
 *-
 ************************************************************************
 */

int 	sfExtVerCompare
(
    SF_EXT_INFO *ext1,	/* (in) the strucutre to find.		*/
    SF_EXT_INFO *ext2	/* (in) the strucutre to compare to.	*/
)
{
    int		ret;	   /* Return value.			*/
    
    ret = strncmp( ext1->extName, ext2->extName, FLEN_VALUE );

    if ( ret == 0 )
    {
	ret = ext1->extVer - ext2->extVer;
    }
	
    fnReturn( ret );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFindExt
 *
 * INVOCATION:
 * extensionInfo = sfFindExt( extensionList, extenstionId, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	extList	    (EPTR*)		The extension list.
 * (>)	frmid	    (char[FLEN_VALUE])	Extension identifier.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To find the extension with the given identifier in a extension list.
 *
 * DESCRIPTION:
 * Search the extension list for the desired extension.  If it is found
 * return it, otherwise return NULL.
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
 * None.
 *-
 ************************************************************************
 */

SF_EXT_INFO *sfFindExt
(
    EPTR	*extList,	    /* (in)  The extension list.	*/
    char	frmId[FLEN_VALUE],  /* (in)  Extension identifier.	*/
    SF_STATUS   *sfStatus	    /* (mod) SF library status.		*/
)
{
    int		index;		/* index into extension list		*/
    SF_EXT_INFO	*tmpExt;
        
    fnEntry( "sfFindExt", *sfStatus );
    statCheck( *sfStatus, NULL );


    nullCheck( tmpExt = (SF_EXT_INFO*)malloc( sizeof( SF_EXT_INFO ) ),
	    *sfStatus, NULL );
    
    strnCpy( tmpExt->frmId, frmId, FLEN_VALUE, *sfStatus, NULL );
    if ( extList != NULL && eptr_search( extList, (pointer)tmpExt, &index,
	    sfExtCompare ) )
    {
	/*
	 *   Found it, so return the extension node
	 */

	gen_free( tmpExt );
	tmpExt = NULL;
	nullCheck( tmpExt = eptritem( extList, index ), *sfStatus, NULL );
    }
    else
    {
	gen_free( tmpExt );
	tmpExt = NULL;
	*sfStatus = SF_E_EXT_NOT_FOUND;
    }
    
    fnReturn( tmpExt );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFindExtVer
 *
 * INVOCATION:
 * extensionInfo = sfFindExtVer( extVerList, extName, extVer, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	extVerList	    (EPTR*)		The extension list.
 * (>)	extName	    (char[FLEN_VALUE])	EXTNAME, extension name.
 * (>)	extVer	    (long)		EXTVER, extension version.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * NULL 	: If not found, of if an error occurs.
 * SF_EXT_INFO* : If a matching extension was found.
 *
 * PURPOSE:
 * To find the extension with the given identifier, extName and, extver
 * values in a extension list.
 *
 * DESCRIPTION:
 * Search the extension list for the desired extension.  If it is found
 * return it, otherwise return NULL.
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
 * None.
 *-
 ************************************************************************
 */

SF_EXT_INFO	*sfFindExtVer
(
    EPTR	*extVerList,	    /* (in)  The extension list.	*/
    char	extName[FLEN_VALUE],/* (in)  EXTNAME, extension name.	*/
    long	extVer,		    /* (in)  EXTVER, extension version.	*/
    SF_STATUS   *sfStatus	    /* (mod) SF library status.		*/
)
{
    int		index;		/* index into extension list		*/
    SF_EXT_INFO	*tmpExt;	/* Temprorary extension info.		*/

    fnEntry( "sfFindExtVer", *sfStatus );
    statCheck( *sfStatus, NULL );

    
    nullCheck( tmpExt = (SF_EXT_INFO*)malloc( sizeof( SF_EXT_INFO ) ),
	    *sfStatus, NULL );
    
    strnCpy( tmpExt->extName, extName, FLEN_VALUE, *sfStatus, NULL );
    tmpExt->extVer = extVer;
    
    if ( extVerList != NULL && eptr_search( extVerList, (pointer)tmpExt,
	    &index, sfExtVerCompare ) )
    {
	/*
	 *   Found it, so return the extension node
	 */

	gen_free( tmpExt );
	tmpExt = NULL;
	nullCheck( tmpExt = eptritem( extVerList, index ), *sfStatus, NULL );
    }
    else
    {
	*sfStatus = SF_E_EXT_NOT_FOUND;
	gen_free( tmpExt );
	tmpExt = NULL;
    }
    

    fnReturn( tmpExt );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME: sfFindLastExtVer
 *
 * INVOCATION:
 * extensionInfo = sfFindLastExtVer( extensionList, extName, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	extVerList  (EPTR*)		The extension list.
 * (>)	extName	    (char[FLEN_VALUE])	EXTNAME, extension name.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * SF_NULL_EXTVER : If not found, of if an error occurs.
 * int 		  : The extVer value of the last extension found with
 *		     the given extName.
 *
 * PURPOSE:
 * To find the extension with the larget extver value associated with
 * the given extName.
 *
 * DESCRIPTION:
 * ???
 * 
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
 * None.
 *-
 ************************************************************************
 */

int sfFindLastExtVer
(
    EPTR	*extVerList,	    /* (in)  The extension list.	*/
    char	extName[FLEN_VALUE],/* (in)  EXTNAME, extension name.	*/
    SF_STATUS   *sfStatus	    /* (mod) SF library status.		*/
)
{
    int		index;		/* index into extension list		*/
    boolean	found; 		/* Result of eptr_search.		*/
    SF_EXT_INFO	*tmpExt;	/* Temprorary extension info.		*/

    fnEntry( "sfFindLastExtVer", *sfStatus );
    statCheck( *sfStatus, 0 );

    nullCheck( tmpExt = (SF_EXT_INFO*)malloc( sizeof( SF_EXT_INFO ) ),
	    *sfStatus, 0 );
        
    strnCpy( tmpExt->extName, extName, FLEN_VALUE, *sfStatus, 0 );
    for( tmpExt->extVer = 1;
	 found = eptr_search( extVerList, (pointer)tmpExt, &index,
		 sfExtVerCompare );
	 tmpExt->extVer++ );

    if ( !found && tmpExt->extVer == 1 )
    {
	index = 0;
    }
    else
    {
	index = tmpExt->extVer - 1;
    }
    
    gen_free( tmpExt );
    fnReturn( index );
}
