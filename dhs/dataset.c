static char rcsid[] = "$Id: dataset.c,v 1.1.1.1 2002-11-24 20:20:14 brighton Exp $";
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
 * dhs/src/dataset.c
 *
 * PURPOSE:
 * Contains routines used by a application using the ICD 3 interface
 * as either a client or server.
 *
 * FUNCTION NAME(S)
 * dhsBdFrameFind - Find a frame given its name.
 * dhsBdFrameIndex - Find a frame given its index.
 * dhsBdFrameInfo - Get various information about a frame.
 * dhsBdFrameNew - Create a new frame.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.6  1998/08/04 16:13:55  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.5  1997/06/13 21:05:21  nhill
 * Typo fix.
 *
 * Revision 1.4  1997/04/24 16:49:39  nhill
 * Added some "const" keywords.
 *
 * Revision 1.3  1997/02/24 19:17:17  nhill
 * Checkin for install.
 *
 * Revision 1.2  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.1  1997/01/06 17:24:10  nhill
 * Initial revision
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include <stdarg.h>
#include <stdlib.h>

#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdFrameFind
 *
 * INVOCATION:
 * frameId = dhsBdFrameFind( object, name, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) object (DHS_BD_OBJECT) object to search (a frame or dataset).
 * (>) name (char *) Name to look for.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_BD_FRAME) The frame id of the matching frame.
 *
 * PURPOSE:
 * Find a frame given it's name
 *
 * DESCRIPTION:
 * Check each member of the object for a frame with the given name.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The object must be created with the dhsBdDsNew or dhsBdFrameNew functions,
 * or must have been supplied to a callback function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_BD_FRAME	dhsBdFrameFind
(
    DHS_BD_OBJECT		/* (in)  The object to search.		*/
    		object,
    const char	*name,		/* (in)  Name to look for.		*/
    DHS_STATUS	*status		/* (out) Function return status.	*/
)
{
    DHS_AV_ID	attrib;		/* Temporary attribute id.		*/
    DHS_BD_FRAME		/* Id of the frame located.		*/
    		frame;
    char	*frameName;	/* Name of the frame being examined.	*/
    int		i;


    fnEntry( "dhsBdFrameFind" );
    statCheck( *status, DHS_BD_FRAME_NULL );
    isInit( *status, DHS_BD_FRAME_NULL );
    

    /*
     *  Look at every attribute in the object.
     */

    for ( i = 0;; i++ )
    {
	/*
	 *  Get the name of the attribute.  If there is no frame with
	 *  the specified name, then eventually these functions will 
	 *  cause the function to return when no more attributes are
	 *  available.
	 */

	attrib = dhsBdAttribIndex( object, i, status );
	check( dhsBdAttribInfo( attrib, &frameName, NULL, NULL, NULL, 
		NULL, status ), *status, DHS_BD_FRAME_NULL );


	/*
	 *  Check to see if the attribute is a frame.
	 */

	if ( strncmp( frameName, "_frame_", 7 ) == 0 )
	{
	    /*
	     *  If the attribute is a frame, get the frame name out
	     *  of the frame.
	     */

	    dhsBdAttribInfo( attrib, NULL, NULL, NULL, NULL, &frame, status );
	    attrib = dhsAvFind( frame, "_name", status );
	    dhsAvInfo( attrib, NULL, NULL, NULL, NULL, 
		    (void **) &frameName, status );
	    if ( *status == DHS_S_NO_ATTRIB )
	    {
		/*
		 *  This frame has no name attribute, so it is badly 
		 *  formed.
		 */

		*status = DHS_S_NO_FRAME;
		break;
	    }
	    else 
	    {
		/*
		 *  Check to see if the name of the frame matches the
		 *  target name.
		 */

		check( VOID, *status, DHS_BD_FRAME_NULL );
		if ( strcmp( frameName, name ) == 0 )
		{
		    /*
		     *  The frame is found.
		     */

		    break;
		} 
	    }
	}
    }

    fnReturn( frame );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdFrameIndex
 *
 * INVOCATION:
 * How to call the function or proceedure
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) object (DHS_BD_OBJECT) object to search (a frame or dataset).
 * (>) index (int) Index of the frame to find.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_BD_FRAME) The frame id of the matching frame.
 *
 * PURPOSE:
 * Find a frame given it's index
 *
 * DESCRIPTION:
 * Calculate the frame structure name from the index and search for 
 * an attribute with that name.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The object must be created with the dhsBdDsNew or dhsBdFrameNew functions,
 * or must have been supplied to a callback function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */


DHS_BD_FRAME	dhsBdFrameIndex
(
    DHS_BD_OBJECT		/* (in)  The object to search.		*/
    		object,
    int		index,		/* (in)  Index to look for.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	avList;
    DHS_BD_FRAME
    		frame;
    char	frameName[17];


    fnEntry( "dhsBdFrameIndex" );
    statCheck( *pStatus, DHS_BD_FRAME_NULL );
    isInit( *pStatus, DHS_BD_FRAME_NULL );
    

    /*
     *  Generate a name for the frame object.
     */

    (void) sprintf( frameName, "_frame_%d", index );


    /*
     *  Try to find the frame structure in the object.
     */

    avList = dhsAvFind( object, frameName, pStatus );
    if ( *pStatus == DHS_S_NO_ATTRIB )
    {
	/*
	 *  The frame couldn't be found.
	 */

	*pStatus = DHS_S_NO_FRAME;
    }


    /*
     *  If the frame was found, get the frame id.
     */

    check( dhsAvInfo( avList, NULL, NULL, NULL, NULL, &frame, pStatus ), 
	    *pStatus, DHS_BD_FRAME_NULL );

    fnReturn( frame );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdFrameInfo
 *
 * INVOCATION:
 * char 		*name;
 * DHS_DATA_TYPE	type;
 * int			ndims;
 * ulong		dims[7];
 * void			*array;
 * DHS_STATUS		status;
 * dhsBdFrameInfo( frame, &name, &type, &ndims, dims, &array, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) frame (DHS_BD_FRAME) The frame to query.
 * (<) pName (char **) The name of the frame.
 * (<) pType (DHS_DATA_TYPE *) The type of the data in the data array.
 * (<) pNdims (int *) Number of dimensions of the data array.
 * (<) dims (ulong[]) Dimensions of the data array.
 * (<) pArray (void **) Pointer to the data array.
 * (<) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Return information about a frame
 *
 * DESCRIPTION:
 * Use the dhsAvInfo function to extract information about the frame.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The frame must be created with the dhsBdFrameNew function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsBdFrameInfo
(
    DHS_BD_FRAME
		frame,		/* (in)  The index of the frame.	*/
    char	**pName,	/* (out) The name of the frame.		*/
    DHS_DATA_TYPE		/* (out) The type of the data array.	*/
    		*pType,
    int		*pNdims,	/* (out) Number of dims of the array.	*/
    ulong	dims[],		/* (out) Dimensions of the array.	*/
    void	**pArray,	/* (out) Pointer to the data array.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	attrib;


    fnEntry( "dhsBdFrameInfo" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  If the name of the frame is required, get it from the "_name" 
     *  attribute in the frame.
     */

    if ( pName != NULL )
    {
	attrib = dhsAvFind( frame, "_name", pStatus );
	dhsAvInfo( attrib, NULL, NULL, NULL, NULL, (void **) pName, pStatus );
    }


    /*
     *  Get the information about the data array from the "_dataArray"
     *  attribute in the frame.
     */

    attrib = dhsAvFind( frame, "_dataArray", pStatus );
    check( dhsAvInfo( attrib, NULL, pType, pNdims, dims, pArray, pStatus ), 
	    *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdFrameNew
 *
 * INVOCATION:
 * void		*dataPointer;
 * frame = dhsBdFrameNew( object, name, index, dataType, ndims, dims, 
 *		&dataPointer, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) object (DHS_BD_OBJECT) The parent object (frame or dataset).
 * (>) name (char *) The name for the object.
 * (>) index (int) The index to assign to the object.
 * (>) dataType (DHS_DATA_TYPE) The data type for the data array.
 * (>) ndims (int) The number of dimensions for the data array.
 * (>) dims (ulong[]) The dimensions of the data array.
 * (<) pDataPointer (void **) Pointer to the data array.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_BD_FRAME) Frame id of the new frame.
 *
 * PURPOSE:
 * Create a new frame in the specified object
 *
 * DESCRIPTION:
 * Create an attribute value list for the frame. Add the frame to the
 * parent object. Add a _name attribute to the frame to contain the
 * name of the frame. Add a _dataArray object to the frame to contain
 * the data array.  Get a pointer to the actual data array for returning
 * to the calling function.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The object must be a frame or dataset created with the dhsBdFrameNew
 * or dhsBdDsNew functions.
 *
 * DEFICIENCIES:
 * Should probably check for an existing frame with the same name or
 * index.
 *-
 ************************************************************************
 */

DHS_BD_FRAME	dhsBdFrameNew
(
    DHS_BD_OBJECT
    		object,		/* (in)  The parent object.		*/
    const char	*name,		/* (in)  The name of the new frame.	*/
    int		index,		/* (in)  The index number for the frame.*/
    DHS_DATA_TYPE
    		dataType,	/* (in)  The data type.			*/
    int		ndims,		/* (in)  Number dimentsions.		*/
    const ulong	dims[],		/* (in)  Dimensions of the data array.	*/
    const void	**pDataPointer,	/* (out) Pointer to the data array.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_BD_FRAME
		frame;		/* The new frame.			*/
    char	frameName[17];	/* The name of the frame.		*/
    DHS_AV_ID	av;


    fnEntry( "dhsBdFrameNew" );
    statCheck( *pStatus, DHS_BD_FRAME_NULL );
    isInit( *pStatus, DHS_BD_FRAME_NULL );


    /*
     *  Produce the attribute name for the frame based on the index.
     */

    (void) sprintf( frameName, "_frame_%d", index );


    /*
     *  Create an attribute list for the frame.
     */

    frame = dhsAvListNew( pStatus );


    /* 
     *  Add the frame to the parent object.
     */

    dhsAvAdd( object, frameName, DHS_DT_AVLIST, 0, NULL, frame, pStatus );


    /*
     *  Add the _name attribute to the frame.
     */

    dhsAvAdd( frame, "_name", DHS_DT_STRING, 0, NULL, name, pStatus );


    /*
     *  Add the _dataArray attribute to the frame.
     */

    dhsAvAdd( frame, "_dataArray", dataType, ndims, dims, NULL, pStatus );


    /*
     *  Get a pointer to the data array.
     */

    av = dhsAvFind( frame, "_dataArray", pStatus );
    dhsAvInfo( av, NULL, NULL, NULL, NULL, pDataPointer, pStatus );

    fnReturn( frame );
}
