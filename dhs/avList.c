static char rcsid[] = "$Id: avList.c,v 1.1.1.1 2002-11-24 20:19:42 brighton Exp $";
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
 * dhs/src/avList.c
 *
 * PURPOSE:
 * Contains DHS library interface routines used by the Gemini ICD 1c 
 * interface for both clients and servers.
 *
 * FUNCTION NAME(S)
 * avFind - Find 
 * dhsAvAdd - Add an attribute to an attribute value list.
 * dhsAvDelete - Delete an attribute from an attribute value list.
 * dhsAvFind - Find an attribute in an av list given its name.
 * dhsAvIndex - Find an attribute in an av list given its index.
 * dhsAvListCopy - copy an attribute value list.
 * dhsAvListFree - Free an attribute value list.
 * dhsAvListNew - Create a new attribute value list.
 * dhsSdsIdAdd - Add a new SDS id to the list of SDS ids
 * getData - Get information about a data value from a vararg structure.
 * sdsFindLong - Search for an attribute with a specific name.
 * updateIdList - Update the sds id list in the attribute value list and 
 * 		all sublists
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.22  2001/03/27 20:23:12  nhill
 * Added the dhsAvEdit function.
 *
 * Revision 1.21  2000/03/04 00:02:29  nhill
 * Added function avFind, and made dhsAvFind into a call to avFind. avFind
 * allows the option to not keep track of the SDS id, which is used in
 * the new dhsAvDelete function.
 *
 * Added function dhsAvDelete, to delete attributes which already exist
 * in an attribute value list.
 *
 * Revision 1.20  1998/08/04 16:11:56  nhill
 * Rearranged to code to be more object-oriented.
 *
 * Revision 1.19  1997/08/29 22:33:03  nhill
 * Changed to allow string arrays of 0 elements.
 *
 * Revision 1.18  1997/08/19 19:40:34  nhill
 * Changed function dhsAvAdd prototype to match the description in icd1c.
 *
 * Revision 1.17  1997/07/29 22:15:30  nhill
 * Added a check to ensure the SDS object an attribute is being added to
 * is really and avList.
 *
 * Revision 1.16  1997/07/23 19:39:54  nhill
 * Added a check to dhsAvFind to allow for null input avlist.
 *
 * Revision 1.15  1997/07/10 19:08:36  nhill
 * Minor bug fixes and style changes.
 *
 * Revision 1.14  1997/06/09 21:17:42  nhill
 * Changed to allow attribute names longer than SDSs 15 character limit.
 *
 * Revision 1.13  1997/06/04 18:00:51  nhill
 * changed the u<type> variables to unsigned <type>.
 * Added a check for failure in SdsCell when getting data from an array of
 * strings.
 *
 * Revision 1.12  1997/05/20 17:38:21  nhill
 * Fixed a bug that resulted in the dims and ndims values returned by
 * dhsAvInfo being incorrect for scalar string values.
 *
 * Revision 1.11  1997/04/24 16:48:12  nhill
 * Added several "const" keywords.
 *
 * Revision 1.10  1997/04/15 19:50:08  nhill
 * Moved the index increment code into a separate function.
 *
 * Revision 1.9  1997/03/26 17:40:45  nhill
 * Fixed some lint problems.
 *
 * Revision 1.8  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.7  1997/02/12 22:05:27  nhill
 * Checkin for install.
 *
 * Revision 1.6  1997/01/30 17:18:54  nhill
 * Checkin for install.
 *
 * Revision 1.5  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.4  1997/01/27 22:35:03  nhill
 * Checkin before install.
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


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#ifdef vxWorks
#include <vxWorks.h> /* must come before stdarg.h */
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"
#include "av.h"


/*
 *  Structure to temporarily store a single value.
 */

union	value
{
    DHS_AV_LIST	valAvList;	/* Temporary storage for an av list id.	*/
    boolean	valBool;	/* Temporary storage for a boolean.	*/
    char	valChar;	/* Temporary storage for a char.	*/
    double	valDouble;	/* Temporary storage for a double.	*/
    float	valFloat;	/* Temporary storage for float vals.	*/
    short	valInt16;	/* Temporary storage for a short.	*/
    int		valInt32;	/* Temporary storage for a int.		*/
    byte	valInt8;	/* Temporary storage for a byte.	*/
    DHS_TAG	valTag;		/* Temporary storage for a tag.		*/
    unsigned short	
		valUint16;	/* Temporary storage for unsigned short.*/
    ulong	valUint32;	/* Temporary storage for unsigned int.	*/
    unsigned char
		valUint8;	/* Temporary storage for unsigned byte.	*/
};


/*
 ************************************************************************
 *  Local function declarations.
 ************************************************************************
 */

static DHS_AV_ID
		avFind ( DHS_AV_LIST, const char *, boolean, DHS_STATUS * );
static void	getData( DHS_DATA_TYPE, int, va_list *, int *, SdsCodeType *,
			void **, union value *, DHS_STATUS * );
static void	sdsFindLong( SdsIdType, const char *, const char *, 
			SdsIdType *, StatusType * );
static void	updateIdList( DHS_AV_LIST, EPTR *, DHS_STATUS * );

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * 
 *
 * INVOCATION:
 * av = avFind( avlist, name, trackIt, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) Attribute value list to search.
 * (>) name (const char *) Name of the attribute to find.
 * (>) trackIt (boolean) True if the avList should track the returned attribute
 *			id, and delete it when the avlist is deleted.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_ID) The id of the attribute.
 *
 * PURPOSE:
 * Find an attribute in an attribute value list given it's name
 *
 * DESCRIPTION:
 * Use the sdsfind function to look for the attribute.
 * - Get the name of the SDS attribute. If the name will fit into the SDS
 *   name size, the SDS name is the same as the attribute name. If not, the
 *   SDS name is the first part of the attribute name, with the crc of the
 *   attribute name appended.
 * - Search for the attribute with the SdsFind function.
 * - If the name is a long name:
 * -   Verify that the long name is the same as the one desired.
 * -   If the long name isn't the one desired, there must be a duplicate, so do
 *     a slow search for the attribute.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The attribute value list must be created with the dhsAvListNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_AV_ID	avFind
(
    DHS_AV_LIST	avList,		/* (in)	 AvList to query.		*/
    const char	*name,		/* (in)  Name to search for.		*/
    boolean	trackIt,	/* (in)  Keep track of the returned 	*/
    				/*       attribute id?			*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    unsigned long
		actLen;
    unsigned long
    		crc;
    char	*pName;
    SdsIdType	sdsId;		/* Sds id of the av pair.		*/
    SdsIdType	sdsId2;		/* Sds id of the av pair.		*/
    char	sdsName[SDS_NAME_SIZE + 1];
    StatusType	sdsStatus;
    int		strLength;
    boolean	longName;


    fnEntry( "avFind" );
    statCheck( *pStatus, DHS_AV_ID_NULL );
    isInit( *pStatus, DHS_AV_ID_NULL );


    if ( avList == DHS_AV_LIST_NULL )
    {
	*pStatus = DHS_S_NO_ATTRIB;
	return( DHS_AV_ID_NULL );
    }


    /*
     *  If the attribute name is longer than the maximum name length allowed,
     *  use the first 7 characters of the name, and set the last 8 characters
     *  to be the crc of the whole name. This should create a unique name
     *  that fits into SDSs limitations, and it will be the name assigned
     *  when an attribute is added to a list.
     */

    strLength = strlen( name ) + 1;
    if ( strLength > SDS_NAME_SIZE + 1 )
    {
	crc = crc_buffer( strLength, (char *) name );
	longName = TRUE;
	(void) sprintf( sdsName, "%7.7s%08x", name, (unsigned) crc );
    }
    else
    {
	longName = FALSE;
	strcpy( sdsName, name );
    }


    /*
     *  Look for the attribute.
     */

    sdsStatus = SDS__OK;
    SdsFind( dhsSdsId( avList ), sdsName, &sdsId, &sdsStatus );
    
    if ( sdsStatus == SDS__OK && longName )
    {
	/*
	 *  Get the complete attribute name and verify it was correct.
	 */

	SdsFind( dhsSdsId( sdsId ), "attName", &sdsId2, &sdsStatus );
	SdsPointer( sdsId2, (void **) &pName, &actLen, &sdsStatus );
	sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), sdsStatus, *pStatus, 
		DHS_AV_ID_NULL );

	if ( strcmp( pName, name ) != 0 )
	{
	    /*
	     * The attribute name doesn't match, so the sds id must have 
	     * mapped into a duplicate id.  Do the long slow search to fine
	     * the real attribute.
	     */

	    sdsCheck( SdsFreeId( sdsId, &sdsStatus ), sdsStatus, *pStatus, 
		    DHS_AV_ID_NULL );
	    sdsFindLong( dhsSdsId( avList ), name, sdsName, &sdsId,  
		    &sdsStatus );
	}
    }


    if ( sdsStatus == SDS__OK )
    {
	if ( trackIt )
	{
	    /*
	     *  The attribute was found, record the new attribute id in the 
	     *  eptr list of attribute ids.
	     */

	    dhsSdsIdAdd( avList, sdsId, pStatus );
	}

	fnReturn( dhsSdsAvList( sdsId ) );
    }
    else if ( sdsStatus == SDS__NOITEM )
    {
	/*
	 * The attribute was not found.
	 */

	*pStatus = DHS_S_NO_ATTRIB;
	fnReturn( DHS_AV_ID_NULL );
    }
    else
    {
	/*
	 *  An error occured.
	 */

	dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF,
		"SdsFind( dhsSdsId( avList ), name, &sdsId, &sdsStatus )", 
		sdsStatus );
	*pStatus = DHS_E_SDS;
    }

    fnReturn( DHS_AV_ID_NULL );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvAdd
 *
 * INVOCATION:
 * dhsAvAdd( avList, name, type, ndims, dims, value, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) Attribute value list to received the value.
 * (>) attName (char *) Name for the attribute.
 * (>) attType (DHS_DATA_TYPE) Type of the data.
 * (>) ndims (int) Number of dimensions for the data (0 if the value
 *			is scalar).
 * (>) dims (const unsigned long[]) Size of each dimension for a data array 
 *			(NULL if the value is scalar).
 * (>) value (?) Data value if the value is scalar, pointer to the data
 *			array if the data is an array.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Add an attribute to an attribute value list
 *
 * DESCRIPTION:
 * Extract the value from the argument list and use SDS functions to
 * add it to the attribute value list SDS structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The attribute value list should have been created with the dhsAvListNew
 * function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsAvAdd
(
    DHS_AV_LIST	avList,		/* (in)  AV list to be extended.	*/
    const char	*attName,	/* (in)  New attribute name.		*/
    DHS_DATA_TYPE
    		attType,	/* (in)  Type of the attribute.		*/
    int		ndims,		/* (in)  Number of dimensions.		*/
    const ulong	dims[],		/* (in)  Size of each dimension.	*/
    ...				/* (in)  This will contain the value and*/
    				/*	 the status parameters.		*/
)
{
    unsigned long actLen;
    SdsIdType	childId;	/* SDS id of a child structure.		*/
    boolean	cont;
    unsigned long		/* crc of the name.			*/
    		crc;
    int		i;
    EPTR	*idList;	/* List of SDS ids.			*/
    ulong	index[7];	/* Index into the array of SDS		*/
    				/* structures used to contain an array	*/
				/* of strings.				*/
    ulong	strLength;	/* Local array dimensions.		*/
    DHS_STATUS	localStatus;	/* Status set locally.			*/
    char	**ppChar;	/* Pointer into an array of strings.	*/
    void	*pVal;		/* Pointer to the data value.		*/
    SdsIdType	sdsId;		/* SDS id of the new attribute.		*/
    SdsIdType	sdsId2;		/* SDS id.				*/
    SdsIdType	sdsId3;		/* SDS id.				*/
    SdsIdType	sdsId4;		/* SDS id.				*/
    char	sdsName[SDS_NAME_SIZE + 1];
				/* Name of an sds item.			*/
    StatusType	sdsStatus;	/* SDS function return status.		*/
    SdsCodeType	sdsType;	/* SDS data type.			*/
    int		size;		/* Size of a single data item.		*/
    DHS_STATUS	*pStatus;	/* Function return status.		*/
    int		totalSize;	/* Size of the whole data array.	*/
    va_list	varg;
    union value	val;		/* Temporary storage for a value.	*/


    fnEntry( "dhsAvAdd" );


    size = 1;
    localStatus = DHS_S_SUCCESS;


    va_start( varg, dims );


    /*
     * Get a pointer to the data item and the size of each element of the
     * type.
     */

    getData( attType, ndims, &varg, &size, &sdsType, &pVal, &val, 
	    &localStatus );


    /*
     *  Get the status pointer from the argument list.
     */

    pStatus = va_arg( varg, DHS_STATUS * );
    va_end( varg );


    /*
     *  Do some delayed checking of the initialization state and 
     *  inherited status value.
     */

    isInit( *pStatus, VOID );
    statCheck( *pStatus, VOID );


    /*
     *  Check to see if any errors occured before the status parameter
     *  was available.
     */

    if ( localStatus != DHS_S_SUCCESS )
    {
	*pStatus = localStatus;
	fnReturn( VOID );
    }


    /*
     *  Create a new SDS structure for the attribute. The structure will
     *  contain a data type and value.
     */

    strLength = strlen( attName ) + 1;
    if ( strLength > SDS_NAME_SIZE + 1)
    {
	crc = crc_buffer( strLength, (char *) attName );
	(void) sprintf( sdsName, "%7.7s%08x", attName, (unsigned) crc );
    }
    else
    {
	strcpy( sdsName, attName );
    }

    sdsStatus = SDS__OK;
    SdsNew( dhsSdsId( avList ), sdsName, 0, NULL, SDS_STRUCT, 0, NULL, 
	    &sdsId, &sdsStatus );


    /*
     *  Add the attribute name to the SDS structure.
     */

    SdsNew( sdsId, "attName", 0, NULL, SDS_CHAR, 1, &strLength, 
	    &sdsId2, &sdsStatus );
    SdsPut( sdsId2, strLength, 0, attName, &sdsStatus );
    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), sdsStatus, *pStatus, VOID )


    /*
     *  Add the data type to the sds structure.
     */

    SdsNew( sdsId, "type", 0, NULL, SDS_INT, 0, NULL, &sdsId2, &sdsStatus );
    SdsPut( sdsId2, sizeof( attType ), 0, &attType, &sdsStatus );
    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), sdsStatus, *pStatus, VOID );

    if ( attType == DHS_DT_STRING )
    {
	/*
	 *  The data type is string, so some special handling is required.
	 */

	if ( ndims == 0 )
	{
	    /*
	     *  It is a single string so just add it to the avlist as an
	     *  array of characters.
	     */

	    strLength = strlen( pVal ) + 1;
	    SdsNew( sdsId, "value", 0, NULL, sdsType, 1, &strLength, 
		    &sdsId2, &sdsStatus );
	    sdsCheck( SdsPut( sdsId2, strLength, 0, pVal, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID )
	}
	else
	{
	    /*
	     * It is an array of strings, so some more work has to be done.
	     * First an array of pointers have to be allocated. This gives
	     * the dhsAvInfo function somewhere to put the character pointer
	     * it needs to create to return the array of pointers. This is
	     * a hack, but it works.
	     */

	    /*
	     *  Calculate the total size required for the array of pointers
	     *  And initialize the index array to 1's.
	     */

	    totalSize = sizeof( char * );
	    for ( i = 0; i < ndims; i++ )
	    {
		totalSize *= dims[i];
		index[i] = 1;
	    }

	    /*
	     *  Add the pointer array to the attribute structure.  The value
	     *  is written to the pointer only to ensure SDS allocates space
	     *  for them.
	     */

	    SdsNew( sdsId, "pointers", 0, NULL, SDS_UINT, ndims, dims, 
		    &sdsId2, &sdsStatus );
	    SdsPut( sdsId2, totalSize, 0, pVal, &sdsStatus );
	    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID )


	    /*
	     * Add the array of strings to the attribute structure. The
	     * value for an array of strings is an array of sds structures, 
	     * one structure coresponding to each string in the array. Each
	     * Structure in the array contains the character array for the 
	     * coresponding string.
	     */

	    sdsCheck( SdsNew( sdsId, "value", 0, NULL, SDS_STRUCT, ndims, dims, 
		    &sdsId2, &sdsStatus ), sdsStatus, *pStatus, VOID );

	    if ( pVal != NULL && totalSize != 0  )
	    {
		/*
		 *  This loop processes each string in the array of strings.
		 *  The strings are accessed by incrementing the pVal
		 *  pointer after each iteration. The cell in the array
		 *  of values is index by incrementing the appropriate 
		 *  column in the index array.
		 */

		for ( cont = TRUE; cont; )
		{
		    /*
		     *  Get the cell from the array of values indicated
		     *  by the values in the index array.
		     */

		    SdsCell( sdsId2, ndims, index, &sdsId3, &sdsStatus );

		    
		    /*
		     *  Write the current string to the cell retrieved above.
		     */

		    strLength = strlen( *( (char **) pVal ) ) + 1;
		    SdsNew( sdsId3, attName, 0, NULL, sdsType, 1, &strLength, 
			    &sdsId4, &sdsStatus );
		    sdsCheck( SdsFreeId( sdsId3, &sdsStatus ), 
			    sdsStatus, *pStatus, VOID )
		    sdsCheck( SdsPut( sdsId4, strLength, 0, 
			    *( (char **) pVal ), &sdsStatus ), 
			    sdsStatus, *pStatus, VOID );
		    sdsCheck( SdsFreeId( sdsId4, &sdsStatus ), 
			    sdsStatus, *pStatus, VOID )

		    /*
		     *  Increment the values in the index array to the next
		     *  cell in the array of values in SDS attribute structure.
		     */

		    cont = dhsIncrIndex( index, dims, ndims );


		    /*
		     *  Increment the character pointer to the next element
		     *  in the string array.
		     */

		    ppChar = (char **) pVal;
		    ppChar++;
		    pVal = (void *) ppChar;
		}
	    }

	    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	}
    }
    else if ( attType == DHS_DT_AVLIST )
    {
	/*
	 *  The value is an attribute value list, so insert the list into
	 *  the current list.
	 */

	if ( pVal == NULL )
	{
	    dhsMsgFmt( DHS_E_NULLVALUE, DHS_DEBUG_OFF );
	    *pStatus = DHS_E_NULLVALUE;
	}
	else
	{
	    SdsInsert( sdsId, * (SdsIdType *) pVal, &sdsStatus );
	    childId = * (SdsIdType *) pVal;
	    sdsCheck( SdsRename( childId, "value", &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	    sdsCheck( SdsGetExtra( childId, sizeof( EPTR * ), 
		    (char *) &idList, &actLen, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	    if ( actLen != 0 && idList != NULL )
	    {
		for ( i = 0; i < eptrlen( idList ); i++ )
		{
		    sdsId2 = (int) eptritem( idList, i );
		    dhsSdsIdAdd( sdsId, sdsId2, pStatus );
		}
		eptr_free( idList, NULL );
	    }
	    dhsSdsIdAdd( avList, childId, pStatus );
	    sdsCheck( SdsGetExtra( dhsSdsId( avList ), sizeof( EPTR * ), 
		    (char *) &idList, &actLen, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	    sdsCheck( SdsPutExtra( childId, sizeof( EPTR * ), 
		    (char *) &idList, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );

	}
    }
    else
    {
	/*
	 *  The parameter is a regular value or array, so just create
	 *  a new SDS primitive item and put the data into it.
	 */


	/*
	 *  Calculate the total size of the array.
	 */

	totalSize = size;
	for ( i = 0; i < ndims; i++ )
	{
	    totalSize *= dims[i];
	}


	/*
	 *  Create a new SDS primitive item.
	 */

	SdsNew( sdsId, "value", 0, NULL, sdsType, ndims, dims, &sdsId2, 
		&sdsStatus );


	/*
	 *  Put the value into the SDS primitive item.
	 */

	if ( pVal != NULL )
	{
	    sdsCheck( SdsPut( sdsId2, totalSize, 0, pVal, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	}
	sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), 
		sdsStatus, *pStatus, VOID );
    }


    sdsCheck( SdsFreeId( sdsId, &sdsStatus ), sdsStatus, *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvDelete
 *
 * INVOCATION:
 * dhsAvDelete( avList, name, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) Attribute value list the attribute is deleted from.
 * (>) attName (char *) Name of the attribute.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Delete an attribute from an attribute value list
 *
 * The function returns the status DHS_S_NO_ATTRIB if the attribute does
 * not exist in the av list.
 *
 * DESCRIPTION:
 * Find the attribute in the avlist, and delete it and any children.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The attribute value list should have been created with the dhsAvListNew
 * function.
 *
 * DEFICIENCIES:
 avFind None.
 *-
 ************************************************************************
 */

void		dhsAvDelete
(
    DHS_AV_LIST	avList,		/* (in)  AV list to be extended.	*/
    const char	*attName,	/* (in)  New attribute name.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_ID	avId;		/* The id of the attribute.		*/


    fnEntry( "dhsAvDelete" );

    isInit( *pStatus, VOID );
    statCheck( *pStatus, VOID );


    /*
     *  Attempt to delete any existing attribute in this avlist with the 
     *  same name.
     */

    avId = avFind( avList, attName, FALSE, pStatus );
    if ( *pStatus == DHS_S_SUCCESS )
    {
	dhsAvListFree( avId, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvEdit
 *
 * INVOCATION:
 * dhsAvEdit( attribute, DHS_DT_INT, &data, sizeof( data ), &data, 
 *           &dhsStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) attribute (DHS_AV_ID) The attribute to modify.
 * (>) pData (void *) Pointer to the new data.
 * (>) dataSize (size_t) Size of the new data.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * This function allows an attributes value to be edited
 *
 * The data pointed to by the pData parameter will be copied into the 
 * SDS structure. The data size and data type must match the values 
 * stored in the attribute.
 *
 * This function only works with simple, scalar values.
 *
 * DESCRIPTION:
 * This function uses the SDS "extra" information to store easily accesed
 * information about the attribute. This shouldn't conflict with other 
 * uses of the extra information, since it is only used for av lists with
 * child attributes.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Create the attribute with dhsAvAdd. Find the attribute id with dhaAvFind.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsAvEdit
(
    DHS_AV_ID	attribute,	/* (in)  The attribute to edit.		*/
    DHS_DATA_TYPE		/* (in)  The expected type of the data.	*/
    		attType,
    ...
)
{
    ulong	actLen;		/* The actual length of the extra info.	*/
    tAvEditInfo	editInfo;	/* Information needed to edit the attribute. */
    int		nDims;		/* Dimensions of the attribute.		*/
    StatusType	sdsStatus;	/* SDS status value.			*/
    DHS_STATUS	localStatus;	/* Function return status.		*/
    DHS_STATUS	*pStatus;	/* Function return status.		*/
    SdsCodeType	sdsType;	/* SDS data type.			*/
    int		size;		/* Size of a single data item.		*/
    union value	val;		/* Temporary storage for a value.	*/
    void	*pVal;		/* Pointer to the data value.		*/
    va_list	varg;


    fnEntry( "dhsAvEdit" );


    va_start( varg, attType );


    /*
     * Get a pointer to the data item and the size of each element of the
     * type.
     */

    localStatus = DHS_S_SUCCESS;
    getData( attType, 0, &varg, &size, &sdsType, &pVal, &val, 
	    &localStatus );


    /*
     *  Get the status pointer from the argument list.
     */

    pStatus = va_arg( varg, DHS_STATUS * );
    va_end( varg );

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Try to get the "extra" information from the attribute.
     */

    sdsStatus = SDS__OK;
    sdsCheck( SdsGetExtra( dhsSdsId( attribute ), sizeof( tAvEditInfo ),
	    (char *) &editInfo, &actLen, &sdsStatus ), 
	    sdsStatus, *pStatus, VOID );
    

    if ( actLen == 0 )
    {
	/*
	 * No Edit information is available, we have to create a new
	 * tAvEditInfo structure.
	 */

	dhsAvInfo( attribute, NULL, &editInfo.aeiDataType, &nDims, NULL, 
		NULL, pStatus );
	if ( *pStatus != DHS_S_SUCCESS )
	{
	    fnReturn( VOID );
	}

	if ( nDims != 0 )
	{
	    dhsMsgFmt( DHS_E_AVEDIT_DIMS, DHS_DEBUG_OFF, nDims );
	    *pStatus = DHS_E_AVEDIT_DIMS;
	    fnReturn( VOID );
	}

	sdsCheck( SdsFind( dhsSdsId( attribute ), "value", &(editInfo.aeiDataID), 
		&sdsStatus ), sdsStatus, *pStatus, VOID );
	editInfo.aeiNullValue = NULL;
	sdsCheck( SdsPutExtra( dhsSdsId( attribute ), sizeof( tAvEditInfo ),
		(char *) &editInfo, &sdsStatus ), sdsStatus, *pStatus, VOID );

    }
    else if ( actLen != sizeof( tAvEditInfo ) )
    {
	dhsMsgFmt( DHS_E_AVEDIT_INFO, DHS_DEBUG_OFF, actLen );
	*pStatus = DHS_E_AVEDIT_INFO;
	fnReturn( VOID );
    }


    /*
     *  Verify that the data types match.
     */

    if ( attType != editInfo.aeiDataType )
    {
	dhsMsgFmt( DHS_E_AVEDIT_TYPE, DHS_DEBUG_OFF );
	*pStatus = DHS_E_AVEDIT_TYPE;
	fnReturn( VOID );
    }

    if ( attType == DHS_DT_STRING || attType == DHS_DT_AVLIST )
    {
	dhsMsgFmt( DHS_E_AVEDIT_TYPE, DHS_DEBUG_OFF );
	*pStatus = DHS_E_AVEDIT_TYPE;
	fnReturn( VOID );
    }
    sdsCheck( SdsPut( editInfo.aeiDataID, size, 0, pVal, &sdsStatus ),
	    sdsStatus, *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvFind
 *
 * INVOCATION:
 * av = dhsAvFind( avlist, name, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) Attribute value list to search.
 * (>) name (const char *) Name of the attribute to find.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_ID) The id of the attribute.
 *
 * PURPOSE:
 * Find an attribute in an attribute value list given it's name
 *
 * DESCRIPTION:
 * Use the avFind function to find the attribute, tracking the attribute.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The attribute value list must be created with the dhsAvListNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_AV_ID	dhsAvFind
(
    DHS_AV_LIST	avList,		/* (in)	 AvList to query.		*/
    const char	*name,		/* (in)  Name to search for.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    return( avFind( avList, name, TRUE, pStatus ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvIndex
 *
 * INVOCATION:
 * av = dhsAvIndex( avList, index, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) The av list to search.
 * (>) index (int) The index to search for.
 * (!) pStatus (DHS_STATUS *) Descripton of the parameter
 *
 * FUNCTION VALUE:
 * (DHS_AV_ID) The id of the attribute.
 *
 * PURPOSE:
 * Find an attribute given its position in the attribute value list
 *
 * DESCRIPTION:
 * Use the sdsIndex function to find the attribute.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The av list must be created with the dhsAvListNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_AV_ID	dhsAvIndex
(
    DHS_AV_LIST	avList,		/* (in)  The av list to query.		*/
    int		index,		/* (in)  The index into the list.	*/
    DHS_STATUS	*pStatus	/* (out) Function return status.	*/
)
{
    SdsIdType	sdsId;		/* Sds id of the av pair.		*/
    StatusType	sdsStatus;


    fnEntry( "dhsAvIndex" );
    statCheck( *pStatus, DHS_AV_ID_NULL );
    isInit( *pStatus, DHS_AV_ID_NULL );


    /*
     *  Find the attribute in the list.
     */

    sdsStatus = SDS__OK;
    SdsIndex( dhsSdsId( avList ), index + 1, &sdsId, &sdsStatus );
    
    if ( sdsStatus == SDS__OK )
    {
	/*
	 *  The attribute was found, return its id.
	 */

	dhsSdsIdAdd( avList, sdsId, pStatus );
	fnReturn( dhsSdsAvList( sdsId ) );
    }
    else if ( sdsStatus == SDS__NOITEM )
    {
	/*
	 *  The attribute is not in the list.
	 */

	*pStatus = DHS_S_NO_ATTRIB;
	fnReturn( DHS_AV_ID_NULL );
    }
    else
    {
	/*
	 *  An error occured.
	 */

	dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF,
		"SdsIndex( dhsSdsId( avList ), index + 1, &sdsId, &sdsStatus )", 
		sdsStatus );
	*pStatus = DHS_E_SDS;
    }

    fnReturn( DHS_AV_ID_NULL );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListAccess
 *
 * INVOCATION:
 * void			*pBuffer;
 * DHS_AV_LIST		avList;
 * DHS_STATUS		status;
 * avList = dhsAvListAccess( pBuffer, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pBuffer (const void *) Buffer to access.
 * (!) pSstatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_LIST) The accessed avList.
 *
 * PURPOSE:
 * Make a data buffer available as an attribute value list
 *
 * The data buffer should be a buffer exported with function dhsAvListExport,
 * or a dataset buffer received in the server bulk data put callback routine.  
 * The avList created by this routine can only be read, not modified.
 *
 * DESCRIPTION:
 * Execute the SdsAccess routine.
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

DHS_AV_LIST	dhsAvListAccess
(
    const void	*pBuffer,	/* (in)  The buffer to access.		*/
    DHS_STATUS	*pStatus	/* (mod) The function return status.	*/
)
{
    DHS_AV_LIST avList;		/* The new avList.			*/
    StatusType	sdsStatus;	/* The SDS function return value.	*/
    EPTR	*pEptr;
    

    fnEntry( "dhsAvListAccess" );

    statCheck( *pStatus, DHS_AV_LIST_NULL );
    isInit( *pStatus, DHS_AV_LIST_NULL );

    sdsStatus = SDS__OK;
    SdsAccess( (void *) pBuffer, &avList, &sdsStatus );
    nullCheck( pEptr = eptr_alloc( 20 ), *pStatus, DHS_AV_LIST_NULL );
    check( updateIdList( avList, pEptr, pStatus ), *pStatus, DHS_AV_LIST_NULL );

    fnReturn( avList );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListCopy
 *
 * INVOCATION:
 * copyId = dhsAvListCopy( origId, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) origId (DHS_AV_LIST) The original attribute value list.
 * (!) pStatus (DHS_STATUS *) The function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_LIST) The av list id of the copy list.
 *
 * PURPOSE:
 * Make a copy of a specified attribute value list.
 *
 * DESCRIPTION:
 * Use the SdsCopy function to copy the structure.
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

DHS_AV_LIST	dhsAvListCopy
(
    DHS_AV_LIST	origId,		/* (in)  The original av list id.	*/
    DHS_STATUS	*pStatus	/* (in)  Function return status.	*/
)
{
    StatusType	sdsStatus;	/* Sds function return status.		*/
    SdsIdType	copyId;		/* The new sds id.			*/
    EPTR	*pEptr;


    fnEntry( "dhsAvListCopy" );
    statCheck( *pStatus, DHS_AV_LIST_NULL );
    isInit( *pStatus, DHS_AV_LIST_NULL );


    /*
     *  Copy the SDS structure.
     */

    sdsStatus = SDS__OK;
    if ( origId == DHS_AV_LIST_NULL )
    {
	copyId = DHS_AV_LIST_NULL;
    }
    else
    {
	sdsCheck( SdsCopy( dhsSdsId( origId ), &copyId, &sdsStatus ), 
		sdsStatus, *pStatus, DHS_AV_LIST_NULL );
	nullCheck( pEptr = eptr_alloc( 20 ), *pStatus, DHS_AV_LIST_NULL );
	check( updateIdList( copyId, pEptr, pStatus ), *pStatus, 
		DHS_AV_LIST_NULL );
    }

    fnReturn( dhsSdsAvList( copyId ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListFree
 *
 * INVOCATION:
 * dhsAvListFree( avList, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) The av list to free.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Free everything associated with an AV list
 *
 * DESCRIPTION:
 * Use the sds routines to free the SDS structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The avlist should have been created with the dhsAvListNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsAvListFree
(
    DHS_AV_LIST	avList,		/* (in)  AvList to free.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    ulong	actLen;
    int		i;
    EPTR	*idList;	/* List of SDS ids.			*/
    StatusType	sdsStatus;	/* Sds function return values.		*/


    fnEntry( "dhsAvListFree" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    if ( avList != DHS_AV_LIST_NULL )
    {
	sdsStatus = SDS__OK;
	sdsCheck( SdsGetExtra( dhsSdsId( avList ), sizeof( EPTR * ), 
		(char *) &idList, &actLen, &sdsStatus ), 
		sdsStatus, *pStatus, VOID );
	if ( actLen == sizeof( EPTR * ) && idList != NULL )
	{
	    for ( i = 0; i < eptrlen( idList ); i++ )
	    {
		sdsCheck( SdsFreeId( (int) eptritem( idList, i ), &sdsStatus ), 
			sdsStatus, *pStatus, VOID );
	    }
	    eptr_free( idList, NULL );
	}

	sdsStatus = SDS__OK;
	SdsDelete( dhsSdsId( avList ), &sdsStatus );
	if ( sdsStatus == SDS__EXTERN )
	{
	    sdsStatus = SDS__OK;
	}
	else if ( sdsStatus != SDS__OK )
	{
	    dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF, 
		    "SdsDelete( dhsSsAvList( avList ), &sdsStatus )", 
			    sdsStatus );
	    *pStatus = DHS_E_SDS;
	    fnReturn( VOID );
	}
	sdsCheck( SdsFreeId( dhsSdsId( avList ), &sdsStatus ), 
		sdsStatus, *pStatus, VOID );
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListNew
 *
 * INVOCATION:
 * avList = dhsAvListNew( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_LIST) The attribute value list created.
 *
 * PURPOSE:
 * Create a new attribute value list
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
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_AV_LIST	dhsAvListNew
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    StatusType	sdsStatus;	/* Sds function return status.		*/
    SdsIdType	id;		/* The new sds id.			*/
    EPTR	*pEptr;


    fnEntry( "dhsAvListNew" );
    statCheck( *pStatus, DHS_AV_LIST_NULL );
    isInit( *pStatus, DHS_AV_LIST_NULL );


    /*
     *  Create the sds structure for the av list.  The eptr list will be
     *  used to keep a list of SDS ids used to allow them to be freed
     *  when the AV list is freed.
     */

    sdsStatus = SDS__OK;
    nullCheck( pEptr = eptr_alloc( 20 ), *pStatus, DHS_AV_LIST_NULL );
    sdsCheck( SdsNew( 0, "avList", sizeof( EPTR *), (char *) &pEptr, 
	    SDS_STRUCT, 0, NULL, &id, &sdsStatus ), sdsStatus, *pStatus, 
	    DHS_AV_LIST_NULL );

    fnReturn( dhsSdsAvList( id ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsSdsIdAdd
 *
 * INVOCATION:
 * dhsSdsIdAdd( avList, sdsId, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) The attribute value list to add the id to.
 * (>) sdsId (SdsIdType) The Sds id to add to the list.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Add a new SDS id to the list of SDS ids
 *
 * DESCRIPTION:
 * - Get the EPTR list of SDS ids from the extra data area of the SDS
 *   structure.
 * - Insert the new SDS id into the EPTR list.
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

void		dhsSdsIdAdd
(
    DHS_AV_LIST	avList,		/* (in)  avList to add the id to.	*/
    SdsIdType	sdsId,		/* (in)  The Sds id to add to the list.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    unsigned long actLen;
    EPTR	*idList;
    StatusType	sdsStatus;	/* SDS function return status.		*/


    sdsStatus = SDS__OK;
    sdsCheck( SdsGetExtra( dhsSdsId( avList ), sizeof( EPTR * ), 
	    (char *) &idList, &actLen, &sdsStatus ), 
	    sdsStatus, *pStatus, VOID );

    if ( actLen == 0 || idList == NULL )
    {
	dhsMsgFmt( DHS_E_NOT_AVLIST, DHS_DEBUG_OFF, avList );
	*pStatus = DHS_E_NOT_AVLIST;
    }
    else
    {
	nullCheck( eptr_insert( idList, eptrlen( idList ), (void *) sdsId ), 
		*pStatus, VOID );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * getData
 *
 * INVOCATION:
 * getData( attType, &varg, &size, &sdsType, &pVal, &val, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) attType (DHS_DATA_TYPE) Type of the value.
 * (>) ndims (int) Number of dimensions of the data.
 * (>) varg (va_list *) Argument list containing the value.
 * (<) pSize (int *) Size of each element in the value.
 * (<) pSdsType (SdsCodeType *) SDS type to be used to store the data.
 * (<) ppVal (void **) Pointer to the data value.
 * (>) pVal (union value *) Temporary storage for scalar values.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Extract information about a data value from a varg list
 *
 * DESCRIPTION:
 * Each data type is handled separately, but virually identically.
 * The function sets the SDS data type used to store the data, sets the
 * size of the data items, and gets the data value or pointer to the
 * data value depending on whether the data is a scalar value or an array.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The calling function should have been pased the data value in a varg list.
 * va_start should have been executed, and the value should be the next
 * thing in the list.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	getData
( 
    DHS_DATA_TYPE	
		attType, 	/* (in)  The type of the attribute.	*/
    int		ndims,		/* (in)  Number of dims in the data.	*/
    va_list	*varg, 		/* (in)  Argument list containg the value.*/
    int *	pSize, 		/* (out) Size of element in the value.	*/
    SdsCodeType *pSdsType,	/* (out) SDS type to use to store the data.*/
    void	**ppVal, 	/* (out) Pointer to the data value.	*/
    union value *pVal,		/* (in)  Temporary storage for a value.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "getData" );
    statCheck( *pStatus, VOID );


    switch ( attType )
    {
	case	DHS_DT_CHAR:
	    /*
	     *  Data item is a single character.
	     */

	    *pSdsType = SDS_CHAR;
	    *pSize = sizeof( char );

	    if ( ndims == 0 )
	    {
		pVal->valChar = va_arg( *varg, int );
		*ppVal = &(pVal->valChar);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, char * );
	    }

	    break;

	case	DHS_DT_INT8:
	    /*
	     *  Data item is a signed 8 bit integer.
	     */

	    *pSdsType = SDS_BYTE;
	    *pSize = sizeof( byte );

	    if ( ndims == 0 )
	    {
		pVal->valInt8 = va_arg( *varg, int );
		*ppVal = &(pVal->valInt8);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, byte * );
	    }

	    break;
	case	DHS_DT_UINT8:
	    /*
	     *  Data item is an unsigned 8 bit integer.
	     */

	    *pSdsType = SDS_UBYTE;
	    *pSize = sizeof( unsigned char );

	    if ( ndims == 0 )
	    {
		pVal->valUint8 = va_arg( *varg, int );
		*ppVal = &(pVal->valUint8);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, unsigned char * );
	    }

	    break;
	case	DHS_DT_INT16:
	    /*
	     *  Data item is a 16 bit signed integer.
	     */

	    *pSdsType = SDS_SHORT;
	    *pSize = sizeof( short );

	    if ( ndims == 0 )
	    {
		pVal->valInt16 = va_arg( *varg, int );
		*ppVal = &(pVal->valInt16);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, short * );
	    }

	    break;
	case	DHS_DT_UINT16:
	    /*
	     *  Data item is an unsigned 16 bit integer.
	     */

	    *pSdsType = SDS_USHORT;
	    *pSize = sizeof( unsigned short );

	    if ( ndims == 0 )
	    {
		pVal->valUint16 = va_arg( *varg, int );
		*ppVal = &(pVal->valUint16);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, unsigned short * );
	    }

	    break;
	case	DHS_DT_INT32:
	    /* 
	     *  Data item is a signed 32 bit integer.
	     */

	    *pSdsType = SDS_INT;
	    *pSize = sizeof( int );

	    if ( ndims == 0 )
	    {
		pVal->valInt32 = va_arg( *varg, int );
		*ppVal = &(pVal->valInt32);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, int * );
	    }

	    break;
	case	DHS_DT_UINT32:
	    /*
	     *  Data item is an unsigned 32 bit integer.
	     */

	    *pSdsType = SDS_UINT;
	    *pSize = sizeof( unsigned int );

	    if ( ndims == 0 )
	    {
		pVal->valUint32 = va_arg( *varg, unsigned int );
		*ppVal = &(pVal->valUint32);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, unsigned int * );
	    }

	    break;
	case	DHS_DT_FLOAT:
	    /*
	     *  Data item is floating point.
	     */

	    *pSdsType = SDS_FLOAT;
	    *pSize = sizeof( float );

	    if ( ndims == 0 )
	    {
		pVal->valFloat = va_arg( *varg, double );
		*ppVal = &(pVal->valFloat);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, float * );
	    }

	    break;
	case	DHS_DT_DOUBLE:
	    /*
	     *  Data type is double.
	     */

	    *pSdsType = SDS_DOUBLE;
	    *pSize = sizeof( double );

	    if ( ndims == 0 )
	    {
		pVal->valDouble = va_arg( *varg, double );
		*ppVal = &(pVal->valDouble);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, double * );
	    }

	    break;
	case	DHS_DT_STRING:
	    /*
	     *  data type is string.
	     */

	    *pSdsType = SDS_CHAR;

	    if ( ndims == 0 )
	    {
		*ppVal = va_arg( *varg, char * );
	    }
	    else
	    {
		*ppVal = va_arg( *varg, char ** );
	    }

	    break;

	case	DHS_DT_TAG:
	    /*
	     *  Data type is a DHS command tag.
	     */

	    *pSdsType = SDS_INT;
	    *pSize = sizeof( DHS_TAG );

	    if ( ndims == 0 )
	    {
		pVal->valTag = va_arg( *varg, DHS_TAG );
		*ppVal = &(pVal->valTag);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, DHS_TAG * );
	    }

	    break;
	case	DHS_DT_AVLIST:
	    /*
	     *  Data type is another av list.
	     */

	    *pSdsType = SDS_STRUCT;
	    *pSize = sizeof( DHS_AV_LIST );

	    if ( ndims == 0 )
	    {
		pVal->valAvList = va_arg( *varg, DHS_AV_LIST );
		*ppVal = &(pVal->valAvList);
	    }
	    else
	    {
		/*
		 * Arrays of avlists aren't currently handled, so this
		 * is an error.
		 */

		dhsMsgFmt( DHS_E_AVLIST_ARRAY, DHS_DEBUG_OFF );
		*pStatus = DHS_E_AVLIST_ARRAY;
		*ppVal = va_arg( *varg, DHS_AV_LIST * );
	    }

	    break;
	case	DHS_DT_BOOLEAN:
	    /*
	     *  Data type is a boolean.
	     */

	    *pSdsType = SDS_BYTE;
	    *pSize = sizeof( boolean );

	    if ( ndims == 0 )
	    {
		pVal->valBool = va_arg( *varg, int );
		*ppVal = &(pVal->valBool);
	    }
	    else
	    {
		*ppVal = va_arg( *varg, boolean * );
	    }

	    break;
	default:
	    dhsMsgFmt( DHS_E_TYPE, DHS_DEBUG_OFF, attType );
	    *pStatus = DHS_E_TYPE;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sdsFindLong
 *
 * INVOCATION:
 * sdsFindLong( parentId, attribName, sdsName, &childId, &sdsStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) parentId (SdsIdType) The SDS structure to search.
 * (>) attribName (const char *) The real name of the object to find.
 * (>) sdsName (const char *) The SDS name of the object.
 * (<) childId (SdsIdType *) The returned attribute id.
 * (!) pSdsStatus (StatusType *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Do a search for an attribute with a specific name
 *
 * This function is only used if the dhsAvFind function determines that there
 * are two attributes with names longer than 15 characters which have mapped
 * into the same 15 character name. The odds of this happening are 1:2**32
 * for any pair of attributes, so this function will be executed very rarely,
 * if at all.
 *
 * DESCRIPTION:
 * Loop through each of the attributes looking for an attribute with the
 * specified name.
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

static void	sdsFindLong
(
    SdsIdType	parentId,	/* (in)  The SDS structure to search.	*/
    const char	*attribName,	/* (in)  The real name of the object to */
				/*	 find.				*/
    const char	*sdsName,	/* (in)  The SDS name of the object to	*/
				/*	 find.				*/
    SdsIdType	*childId,	/* (out) The Id found.			*/
    StatusType	*pSdsStatus	/* (mod) Fucnction return status.	*/
)
{
    unsigned long
    		actLen;
    long	i;
    SdsIdType	id;
    SdsIdType	id2;
    char	sdsAttribName[ATT_NAME_LEN + 1];
    SdsCodeType	type;
    long	ndims;
    unsigned long dims[7];
    char	*pName;


    for(i = 1; *pSdsStatus == SDS__OK; i++)
    {
	SdsIndex( parentId, i, &id, pSdsStatus );
	SdsInfo( id, sdsAttribName, &type, &ndims, dims, pSdsStatus );
	if ( strcmp( sdsAttribName, sdsName ) == 0 )
	{
	    SdsFind( dhsSdsId( id ), "attName", &id2, pSdsStatus );
	    SdsPointer( id2, (void **) &pName, &actLen, pSdsStatus );
	    SdsFreeId( id2, pSdsStatus );
	    if ( *pSdsStatus != SDS__OK )
	    {
		break;
	    }

	    if ( strcmp( pName, attribName ) == 0 )
	    {
		*childId = id;
		return;
	    }
	}
	SdsFreeId( id, pSdsStatus );
    }

    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * updateIdList
 *
 * INVOCATION:
 * updateIdList( avList, pEptr, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) Attribute value list to update
 * (>) pEptr (EPTR *) New list for the SDS id list.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Update the sds id list in the attribute value list and all sublists
 *
 * DESCRIPTION:
 * Update the pointer to the av list, and recursivly update the pointer
 * in any sub lists.
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

static void	updateIdList
(
    SdsIdType	parent,		/* (in)  Attribute value list to update.*/
    EPTR	*pEptr,		/* (in)  New list for the SDS id list.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    SdsIdType	child;
    boolean	cont;
    ulong	dims[7];
    int		i;
    ulong	index[7];
    char	name[ATT_NAME_LEN + 1];
    long	ndims;
    StatusType	sdsStatus;	/* SDS function return status.		*/
    SdsCodeType	type;		/* Type of data in an attribute.	*/


    fnEntry( "updateIdList" );


    sdsStatus = SDS__OK;
    SdsPutExtra( parent, sizeof( EPTR * ), (char *) &pEptr, &sdsStatus );
    if ( sdsStatus == SDS__EXTRA )
    {
	sdsStatus = SDS__OK;
    }
    else if ( sdsStatus!= SDS__OK )
    {
	dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF, 
		"SdsPutExtra( parent, sizeof( EPTR * ), "
		"(char *) &pEptr, &sdsStatus )", sdsStatus );
	*pStatus = DHS_E_SDS;
	return;
    }

    sdsCheck( SdsInfo( parent, name, &type, &ndims, dims, &sdsStatus ),
	    sdsStatus, *pStatus, VOID );
    if ( type == SDS_STRUCT )
    {
	if ( ndims == 0 )
	{
	    for ( i = 1;; i++ )
	    {
		SdsIndex( parent, i, &child, &sdsStatus );
		if ( sdsStatus == SDS__NOITEM )
		{
		    break;
		}
		else if ( sdsStatus != SDS__OK )
		{
		    dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF, 
			    "SdsIndex( parent, i, &child, &sdsStatus )", 
			    sdsStatus );
		    *pStatus = DHS_E_SDS;
		    break;
		}

		check( updateIdList( child, pEptr, pStatus ), *pStatus, VOID );
		sdsCheck( SdsFreeId( child, &sdsStatus ), sdsStatus, 
			*pStatus, VOID );
	    }
	}
	else
	{
	    for( i = 0; i < ndims; i++ )
	    {
		index[i] = 1;
	    }
	    for( cont = TRUE; cont; )
	    {
		SdsCell( parent, ndims, index, &child, &sdsStatus );
		if ( sdsStatus == SDS__INDEXERR )
		{
		    sdsStatus = SDS__OK;
		}
		else if ( sdsStatus != SDS__OK )
		{
		    dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF, 
			    "SdsCell( parent, ndims, index, &child, &sdsStatus )",
			    sdsStatus );
		    *pStatus = DHS_E_SDS;
		    break;
		}
		else
		{
		    check( updateIdList( child, pEptr, pStatus ), *pStatus, VOID );
		    sdsCheck( SdsFreeId( child, &sdsStatus ), sdsStatus, *pStatus, 
			    VOID );
		}
		cont = dhsIncrIndex( index, dims, ndims );
	    }
	}
    }

    fnReturn( VOID );
}
