static char rcsid[] = "$Id: av.c,v 1.1.1.1 2002-11-24 20:19:35 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1998				(c) 1998
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
 * dhs/src/av.c
 *
 * PURPOSE:
 * Contains the functions for manipulating attributes.
 *
 * FUNCTION NAME(S)
 * dhsAvInfo - Get various information about an attribute.
 * dhsIncrIndex - Increment an index array.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:57  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1998/08/04 16:16:03  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include "localDhs.h"
#include "av.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvInfo
 *
 * INVOCATION:
 * char			*name;
 * DHS_DATA_TYPE	type;
 * int			ndims;
 * int[7]		dims;
 * void			*value;
 * dhsAvInfo( avId, &name, &type, &ndims, dims, &value, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) id (DHS_AV_ID) The attribute id to query.
 * (<) pName (char **) Pointer to the name of the attribute.
 * (<) pType (DHS_DATA_TYPE *) Data type of the attribute.
 * (<) pNdims (int *) Number of dimensions of the attribute.
 * (<) dims (ulong []) Dimensions of the attribute.
 * (<) pValue (void **) Pointer to the data.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Get various information about an attribute
 *
 * DESCRIPTION:
 * - Get the type SDS field associated with the attribute and extract the data
 *   type.
 * - Find the value SDS field of the attribute.
 * - Find the attribute name SDS field of the attribute and extract the name.
 * - Get the dimensions of the value from the value structure.
 * - Extract the value for the attribute.
 * -   If the attribute type is an string array, then an array of pointers
 *     to the strings is created. This allows the value returned to be 
 *     the type of value passed to the dhsAvAdd function.
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

void		dhsAvInfo
(
    DHS_AV_ID	id,		/* (in)  Index of attribute to query.	*/
    char	**pName,	/* (out) Name of the attribute.		*/
    DHS_DATA_TYPE
    		*pType,		/* (out) Type of the attribute.		*/
    int		*pNdims,	/* (out) Number of dimensions.		*/
    unsigned long		/* (out) Size of each dimension.	*/
		dims[],
    void	*pValue,	/* (out) Pointer to the data.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    boolean	cont;
    boolean	freeId;		/* Free the SDS id when done?.		*/
    ulong	length;		/* Length of the data buffer.		*/
    SdsIdType	sdsId;		/* Id of structure members.		*/
    SdsIdType	sdsId2;		/* Second sds structure id.		*/
    SdsIdType	sdsId3;		/* Thid sds structure id.		*/
    StatusType	sdsStatus;	/* Sds function return status.		*/
    SdsCodeType	sdsType;	/* The sds type of the data.		*/
    ulong	actLen;		/* Actual length of data read.		*/
    char	tAttName[ATT_NAME_LEN + 1];
    				/* Name of an attribute.		*/
    long	tndims;
    ulong	tdims[7];
    char	**ppChar;
    int		i;
    ulong	index[7];
    int		nelem;
    DHS_DATA_TYPE
    		tType;


    fnEntry( "dhsAvInfo" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Find the type field of the attribute and extract the type.
     */

    sdsStatus = SDS__OK;
    SdsFind( dhsSdsId( id ), "type", &sdsId, &sdsStatus );
    SdsGet( sdsId, sizeof( tType ), 0, &tType, &actLen, &sdsStatus );
    sdsCheck( SdsFreeId( sdsId, &sdsStatus ), sdsStatus, *pStatus, VOID );
    if ( pType != NULL )
    {
	*pType = tType;
    }


    /*
     *  Get the attribute name from the attribute structure.
     */

    if ( pName != NULL )
    {
	SdsFind( dhsSdsId( id ), "attName", &sdsId2, &sdsStatus );
	SdsPointer( sdsId2, (void **) pName, &actLen, &sdsStatus );
	sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), sdsStatus, *pStatus, VOID );
    }


    /*
     *  Find the value vield of the attribute.
     */

    SdsFind( dhsSdsId( id ), "value", &sdsId, &sdsStatus );


    /*
     *  Get the value dimensions from the value structure.
     */

    sdsCheck( SdsInfo( sdsId, tAttName, &sdsType, &tndims, 
	    tdims, &sdsStatus ), sdsStatus, *pStatus, VOID );
    freeId = TRUE;
    if ( pNdims != NULL )
    {
	if ( tType != DHS_DT_STRING || 
		( tndims != 0 && sdsType == SDS_STRUCT ) )
	{
	    *pNdims = tndims;
	}
	else
	{
	    *pNdims = 0;
	}
    }

    if ( dims != NULL )
    {
	for ( i = 0; i < tndims; i++ )
	{
	    dims[i] = tdims[i];
	}
    }


    /*
     *  Get the value of the object.
     */

    if ( pValue != NULL )
    {
	if ( tType == DHS_DT_STRING && tndims != 0 && sdsType == SDS_STRUCT )
	{
	    /*
	     *  This is a multi dimensional string array, so lots of work
	     *  has to be done to return a pointer to an array of string
	     *  pointers.
	     */

	    /*
	     *  This assumes a pointer is a 4 byte integer, so verify
	     *  that it really is.
	     */

	    if ( sizeof( char * ) != 4 )
	    {
		dhsMsgFmt( DHS_E_PTR_SIZE, DHS_DEBUG_OFF );
		*pStatus = DHS_E_PTR_SIZE;
		fnReturn( VOID );
	    }


	    /*
	     *  Get the total number of elements in the array and initialize
	     *  the index array to all 1s.
	     */

	    nelem = 1;
	    for ( i = 0; i < tndims; i++ )
	    {
		nelem *= tdims[i];
		index[ i ] = 1;
	    }


	    /*
	     *  Find the pointer structure in the attribute structure.
	     *  This will be the array of pointers returned to the calling
	     *  program.
	     */

	    SdsFind( dhsSdsId( id ), "pointers", &sdsId2, &sdsStatus );
	    SdsPointer( sdsId2, pValue, &length, &sdsStatus );
	    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), 
		    sdsStatus, *pStatus, VOID );
	    ppChar = *( (char ***) pValue );


	    /*
	     *  Set up the pointer structure to point at the data for
	     *  each of the strings in the array.
	     */

	    if ( nelem != 0 )
	    {
		for ( cont = TRUE; cont; )
		{
		    /*
		     *  Get the SDS id for the the cell identified by the
		     *  index array.
		     */

		    SdsCell( sdsId, tndims, index, &sdsId2, &sdsStatus );


		    /* 
		     *  Get the SDS id of the only thing in the cells structure,
		     *  the string for the cell.
		     */

		    SdsIndex( sdsId2, 1, &sdsId3, &sdsStatus );
		    sdsCheck( SdsFreeId( sdsId2, &sdsStatus ), 
			    sdsStatus, *pStatus, VOID );


		    /*
		     *  Get a pointer to the string in the cell and copy it
		     *  into the pointer array.
		     */

		    sdsCheck( SdsPointer( sdsId3, (void **) ppChar, &length, 
			    &sdsStatus ), sdsStatus, *pStatus, VOID );
		    sdsCheck( SdsFreeId( sdsId3, &sdsStatus ), 
			    sdsStatus, *pStatus, VOID );


		    /*
		     *  Increment the index array.
		     */

		    cont = dhsIncrIndex( index, tdims, tndims );


		    /*
		     *  Increment the pointer into the array of char pointers
		     *  to point to the next position in the array.
		     */

		    ppChar ++;
		}
	    }
	}
	else if ( tType == DHS_DT_AVLIST )
	{
	    /* 
	     *  The attribute was a attribute value list, so return its
	     *  attribute id.
	     */

	    freeId = FALSE;
	    *(DHS_AV_LIST *) pValue = dhsSdsAvList( sdsId );
	}
	else
	{
	    /*
	     *  This is where the value pointer for most data types is 
	     *  assigned.
	     */

	    sdsCheck( SdsPointer( sdsId, pValue, &length, 
		    &sdsStatus ), sdsStatus, *pStatus, VOID );
	}
    }


    if ( freeId )
    {
	sdsCheck( SdsFreeId( sdsId, &sdsStatus ), sdsStatus, *pStatus, VOID );
    }
    else
    {
	dhsSdsIdAdd( sdsId, sdsId, pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsIncrIndex
 *
 * INVOCATION:
 * dhsIncrIndex( index, dims, ndims )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) index (ulong[]) The index array to increment.
 * (>) dims ( const ulong[]) The dimensions of the array.
 * (>) ndims (long) The number of dimensions.
 *
 * FUNCTION VALUE:
 * (boolean) FALSE if the index has overflowed.
 *
 * PURPOSE:
 * Increment an index array, carrying the increment over if necessary
 *
 * This function is used when going through the list of strings. The 
 * function allows steping through a multi dimensional array,one item 
 * at a time. The function returns false when the index was already
 * at the last index in the array, true otherwise.
 *
 * DESCRIPTION:
 * - increment the the last dimension of the index array.
 * - If the increment caused the index to overflow, set the index to zero
 *   and increment the next index.
 * - repeat the above step until no overflow occurs or no more indexes are 
 *   available.
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

boolean		dhsIncrIndex
(
    ulong	index[],	/* (mod) The index array to increment.	*/
    const ulong	dims[],		/* (in)  The dimensions of the array.	*/
    long	ndims		/* (in)  The number of dimensions.	*/
)
{
    long	i;


    /*
     *  Increment the values in the index array to the next cell in the 
     *  array of values in SDS attribute structure. This is done by 
     *  incrementing the last index in the index array, and propegating 
     *  any overflow down the index array, intil the first index overflows.
     */

    index[ndims - 1]++;


    /*
     *  Check for overflows in the index array, starting with the last one.
     */

    for ( i = ndims - 1; i >= 0; i-- )
    {
	if ( index[i] > dims[i] )
	{
	    /*
	     *  An overflow has occured so set the current dimension to one.
	     */

	    index[i] = 1;
	    if ( i == 0 )
	    {
		/*
		 *  If i is the first dimension, and it has just overflowed, 
		 *  then we have just processed the last element in the array
		 *  and so we should exit this loop and set the flag that 
		 *  will indicate the outer loop should exit as well.
		 */

		return( FALSE );
	    }
	    else
	    {
		/*
		 *  If i isn't the first dimension, increment the i-1'th 
		 *  dimension.  The next iteration of the loop will check 
		 *  to see if it overflowed.
		 */

		index[i-1]++;
	    }
	}
	else
	{
	    /*
	     *  No overflow occured, so it isn't necessary to check the 
	     *  other dimensions.
	     */

	    break;
	}
    }

    return( TRUE );
}
