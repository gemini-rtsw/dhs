static char rcsid[] = "$Id: fits2sds.c,v 1.1.1.1 2002-11-24 20:35:29 brighton Exp $";
/*
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) <year>                            (c) <year>
* National Research Council             Conseil national de recherches
* Ottawa, Canada, K1A 0R6               Ottawa, Canada, K1A 0R6
* All rights reserved                   Tous droits reserves
*                                       
* NRC disclaims any warranties, Le CNRC denie toute garantie
* expressed, implied, or statu- enoncee, implicite ou legale,
* tory, of any kind with respect        de quelque nature que se soit,
* to the software, including            concernant le logiciel, y com-
* without limitation any war-           pris sans restriction toute
* ranty of merchantability or           garantie de valeur marchande
* fitness for a particular pur- ou de pertinence pour un usage
* pose.  NRC shall not be liable        particulier.  Le CNRC ne
* in any event for any damages, pourra en aucun cas etre tenu
* whether direct or indirect,           responsable de tout dommage,
* special or general, consequen-        direct ou indirect, particul-
* tial or incidental, arising           ier ou general, accessoire ou
* from the use of the software. fortuit, resultant de l'utili-
*                                       sation du logiciel.
*
************************************************************************
*
* FILENAME
* sfLibrary/src/sf.c
*
* PURPOSE:
* Library to convert from FITS to SDS formats.
* 	
*
* FUNCTION NAME(S)
* int	    compareExtver	: Compare function used for sorting the
*				  EPTR list containting the extver list.
* void	    sfAddArray		: Adds a array or random size and value
*				  to a DHS object.
* void	    sfAddDhsKeywords	: Adds keywords required by DHS, Gemini,
*				  to a dataset or frame object.
* void      sfAddFrames		: Adds a frames to a dataset to create a
*				  structure that matches the frames id.
* void	    sfAddGeminiDdEntries: Add the Gemini required keywords to the data
*				: dictionary.
* void	    sfAddImgData	: Add Image data, frame, to a dataset or frame.
* void	    sfAddKeyword	: Add a keyword to the SDS structures.
* void	    sfAddKeywordWithDd	: Add a keyword to the SDS structure, making
*				  sure that the information matches what is
*				  stored in the data dictionary.
* void	    sfAddOptKeywords	: Adds optional keywords to a dataset or
*                             	  a frame object.
* int       sfCalcCorners   	: Find the bottom, left and top, right corners
*				  of a data chunk.
* int	    sfCalcPos		: Find data chunking positions.
* void	    sfCreatePrimeFrame	: Create a frame for the "prime" data.
* void	    sfDoImgChunking	: Split an image into several chunks and
*				: create SDS structures for them.
* void	    sfFindFrameId	: Calculate a frame Id based on the frame number
*				  or the values of the "EXTNAME" and "EXTVER"
*				  keywords.
* int       sfFits2Sds      	: Converts a FITS file to several SDS objects.
*
*INDENT-OFF*
* $Log: not supported by cvs2svn $
* Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
*
*
* Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
* Initial install into CVS of dhs-0.16
*
* Revision 1.18  1999/02/08 22:09:06  jaeger
* Fixed calcualtion of corners to start at 1,1 .
*
* Revision 1.17  1999/01/26 06:50:03  jaeger
* Made sure origin is initialized to 1,1,1.  Changed references to
* long ints to int since they are the same under solaris.  Move the
* string functions "str2unit" and others to gen library, this now uses
* the gen library ones.
*
* Revision 1.16  1998/06/17 18:16:35  jaeger
* Changed output to dd file, so dataType, extensionVersion, frameName, and frameId all have an array type of NONE.
*
* Revision 1.15  1998/05/14 18:50:33  jaeger
* Added functionality for ensuring AXISSZ, ORIGIN are on all frames,
* when a frame has data.  Also made sure the correct frameName was
* given to the frames (tricky when there is a hierarcy of frames).
*
* Revision 1.14  1998/05/12 23:11:17  nhill
* Eliminated some compiler warnings.
*
* Revision 1.13  1998/05/05 16:25:56  jaeger
* Fixed the random array generation.  Mapped FITS keywords as discussed:
* EXTNAME -> dataType attribute.  EXTVER not placed on SDS structure
* since it will be calculated by sds2fits, same with FRMID and FRMNAME.
*
* Revision 1.12  1998/01/14 17:54:40  jaeger
* Fixed the truncating of comments in the DD, added instrument to the DD.
*
* Revision 1.11  1997/12/02 21:05:38  jaeger
* Added a routine to make sure required Gemini keywords are in the dd.
*
* Revision 1.10  1997/11/21 00:21:48  jaeger
* Fixed problem with data being separated from headers during composition.
*
* Revision 1.9  1997/11/19 00:12:18  jaeger
* Added sorting of "extension" list in the dataset information, removed
* sfPrintMessage, and fixed bug in fits2sds (big dataset did not have
* the proper frame structure).
*
* Revision 1.8  1997/10/06 16:08:17  jaeger
* Add quotes around comment string for the data dictionary.
*
* Revision 1.7  1997/09/05 18:58:07  jaeger
* Fixed some memory leaks
*
* Revision 1.6  1997/08/22 22:10:35  jaeger
* Added support for multi-dimensional arrays.
*
* Revision 1.5  1997/08/19 19:03:45  jaeger
* Checked in for install
*
* Revision 1.4  1997/08/15 20:43:37  jaeger
* Checked in for install
*
* Revision 1.3  1997/08/12 21:07:38  jaeger
* Checked in for install
*
* Revision 1.2  1997/08/06 18:30:48  jaeger
* Checked in for install and testing with dhs
*
* Revision 1.1  1997/07/09 16:33:19  jaeger 
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "gen_msg.h"

/*
 ************************************************************************
 *  cfits library include files.
 ************************************************************************
 */

#include "fitsio2.h"


/*
 ************************************************************************
 *  sf library include files.
 ************************************************************************
 */

#include "sf.h"
#include "localSf.h"
#include "frameInfo.h"


/*
 ************************************************************************
 * Constatns definitions
 ************************************************************************
 */

#define AXISSZ_COMM	"\"Data array dimensions.&\""
#define DT_COMM		"\"Name assigned to a FITS extenstion.\""
#define EXTVER_COMM	"\"Number assigned to a FITS extenstion.\""
#define FRMID_COMM	"\"Gemini frame identifier\""
#define FRMNAME_COMM	"\"Name assigned to a frame of data.\""
#define INSTR_COMM	"\"Instrument used to acquire data.\""
#define MAX_SIZE 	1024
#define NO_FILE		NULL
#define NULL_VERSION	( -1 )
#define ORIGIN_COMM	"\"A frames data origin.&\""


/*
 ************************************************************************
 * Structure definitions
 ************************************************************************
 */

typedef struct
{
    int	extver;
    int	last;
} EXTVER_INFO;


/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */

static	int	compareExtver( int*, EXTVER_INFO* );
static	void	sfAddArray( DHS_BD_OBJECT, DHS_BD_OBJECT, SF_STATUS* );
static 	void 	sfAddDhsKeywords( DHS_BD_OBJECT, int, long[], long[],
			char[],	SF_STATUS* );
static 	void	sfAddFrames( DHS_BD_DATASET, boolean, char[], EPTR*,
			DHS_BD_FRAME*, SF_STATUS* );
static  void 	sfAddGeminiDdEntries( FILE*, char[], SF_STATUS* );
static 	void	sfAddImgData( fitsfile*, DHS_BD_OBJECT, char[], EPTR*, int,
			int, long[], long[], long[], DHS_BD_FRAME*,
			SF_STATUS* );
static	void	sfAddKeyword( DHS_BD_OBJECT, char[], char[], char[], FILE*,
			SF_STATUS* );
static	void	sfAddKeywordWithDd( DHS_BD_OBJECT, char[], char[], char[],
			SF_STATUS* );
static 	void 	sfAddOptKeywords(DHS_BD_OBJECT, DHS_BD_OBJECT, fitsfile*,
			char[], boolean, FILE*, SF_STATUS* );
static 	void 	sfCalcCorners( long[], int, int, int, int, long[], long[],
			SF_STATUS* );
static 	void 	sfCalcPos( long[], int, int, long*, long*, SF_STATUS* );
static 	void	sfCreatePrimeFrame( EPTR*, DHS_BD_OBJECT, char[], int, long[],
			EPTR*, SF_STATUS* );
static 	void	sfAddPrimeFrame( EPTR*, DHS_BD_OBJECT, char[], SF_STATUS* );
static 	void	sfDoImgChunking( fitsfile*, DHS_BD_OBJECT, EPTR*, int, int,
			long[], char[], int, int, boolean, char[], EPTR*,
			char[], long, SF_STATUS* );
static 	SF_FRAME_INFO
		*sfFindFrameId( char[], long, int, SF_STATUS* );

void	sfFormatMessage();

/*
 ************************************************************************
 * Global 
 ************************************************************************
 */


/*
 * A list containing the "EXTVER" values and a counter.
 */

static	EPTR		*extverList;


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * compareExtver
 *
 * INVOCATION:
 * compareExtver( extver1, extver2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) extver	    (int*)	  	The value of an "EXTVER" keyword.
 * (>) extverInfo   (EXTVER_INFO*)	Information about a particular
 *					"EXTVER" keyword.
 * FUNCTION VALUE:
 * (int) 1  : if *extver > extverInfo->extver
 * (int) 0  : if *extver == extverInfo->extver
 * (int) -1 : if *extver > extverInfo->extver
 *
 * PURPOSE:
 * Sorting routine for the eptr_sort and eptr_search calls.
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

int compareExtver
(
    int		*extver,
    EXTVER_INFO	*extverInfo2
)
{
    return( *extver - extverInfo2->extver );
}


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddArray
 *
 * INVOCATION:
 * sfAddArray( objec1, object2, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	object1	    (DHS_BD_OBJECT)	First DHS object.	
 * (>)	object2	    (DHS_BD_OBJECT)	Second DHS object.	
 * (!) 	sfStatus    (SF_STATUS*)	SF library status
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Add an arbitrary array to two DHS objects.
 *
 * DESCRIPTION:
 * 1. Randomly generate the data type of the array.
 * 2. Randomly generate the number of dimension and the size of each
 *    dimension.
 * 3. Add the array, full of garbage, to the DHS objects.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfAddArray
(
    DHS_BD_OBJECT   object1,	/* (in)  DHS frame or dataset object.	*/
    DHS_BD_OBJECT   object2,	/* (in)	 DHS frame or dataset object.	*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    char	    *charArray;		/* Array of character strings.	*/
    DHS_STATUS      dhsStatus;		/* DHS library status.		*/
    unsigned long   dims[SF_MAXDIM];	/* Size each axis, array's.	*/
    double	    *doubleArray;	/* Array of double values.	*/
    float	    *floatArray;	/* Array of float values.	*/
    int		    i;
    short	    *int16Array;	/* Array of short values.	*/
    long	    *int32Array;	/* Array of long values.	*/
    int		    *int8Array;		/* Array of int values.		*/
    int		    ndims;		/* Number of dimensions.	*/
    unsigned long   size;		/* Size of the array.		*/
    DHS_DATA_TYPE   type;		/* Data type of the array.	*/
    unsigned short  *uint16Array;	/* Array of unsigned shorts.	*/
    unsigned long   *uint32Array;	/* Array of unsigned longs.	*/
    unsigned int    *uint8Array;	/* Array of unsigned ints.	*/
    unsigned char   *ucharArray;


    fnEntry( "sfAddArray", *sfStatus );
    statCheck( *sfStatus, VOID );

    /*
     * Randomsly find the type, 
     */
    
    do{
	type = rand() % 14;
    } while( type == DHS_DT_TAG || type == DHS_DT_AVLIST ||
	    type == DHS_DT_NONE || type == DHS_DT_STRING );

    
    /*
     * Randomly find the number of dimension and the size of each.
     */
#ifdef OMIT    
    ndims = ( rand() % SF_MAXDIM ) + 1;
#endif
    ndims = ( rand() % 3 ) + 1;
    size = 1;
    
    for( i = 0; i < ndims; i++ )
    {
	size *= ( dims[i] = rand() % ( MAX_SIZE / ndims ) );
    }


    /*
     * Add the array to the DHS objects.
     */
    
    dhsStatus = DHS_S_SUCCESS;
    switch( type )
    {
	case DHS_DT_BOOLEAN:
	    if ( ( ucharArray = calloc( size, sizeof( unsigned char ) ) )
		    == NULL )
	    {
		return;
	    }
	    
	    dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		    dims, ucharArray, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		    dims, ucharArray, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    free( ucharArray );
	    int8Array = NULL;
	    break;
	    
	case DHS_DT_CHAR:
	   if ( ( int8Array = calloc( size, sizeof( char ) ) ) == NULL )
	   {
	       return;
	   }
	   dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		   dims, charArray, &dhsStatus ), dhsStatus, *sfStatus, VOID );
	   dhsCheck( dhsBdAttribAdd( object2, "multiArray", type,
		   ndims, dims, charArray, &dhsStatus ), dhsStatus,
		   *sfStatus, VOID );
	   free( int8Array );
	   int8Array = NULL;
	   break;
	    
	case DHS_DT_INT8:
	    if ( ( int8Array = calloc( size, sizeof( int ) ) ) == NULL )
	    {
		return;
	    }
	    dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		    dims, int8Array, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		    dims, int8Array, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    free( int8Array );
	    int8Array = NULL;
	    break;
	    
	case DHS_DT_UINT8:
	    if ( ( uint8Array = calloc( size, sizeof( unsigned int ) ) )
		    == NULL )
	    {
		return;
	    }
	    dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		    dims, uint8Array, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		    dims, uint8Array, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    free( uint8Array );
	    uint8Array = NULL;
	    break;
	    
	case DHS_DT_INT16:
	   if ( ( int16Array = calloc( size, sizeof( short ) ) ) == NULL )
	   {
	       return;
	   }
	   dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		   dims, int16Array, &dhsStatus ), dhsStatus, *sfStatus,
		   VOID );
	   dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		   dims, int16Array, &dhsStatus ), dhsStatus, *sfStatus,
		   VOID );	    
	   free( int16Array );
	   int16Array = NULL;
	   break;
	   
	case DHS_DT_UINT16:
	   if ( ( uint16Array = calloc( size, sizeof( unsigned short ) ) )
		   == NULL )
	   {
	       return;
	   }
	   dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		   dims, uint16Array, &dhsStatus ), dhsStatus, *sfStatus,
		   VOID  );
	   dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		   dims, uint16Array, &dhsStatus ), dhsStatus, *sfStatus,
		   VOID  );
	   free( uint16Array );
	   uint16Array = NULL;
	   break;
	    
	case DHS_DT_INT32:
	   if ( ( int32Array = calloc( size, sizeof( long ) ) ) == NULL )
	   {
	       return;
	   }
	   dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		   dims, int32Array, &dhsStatus ), dhsStatus, *sfStatus,
		   VOID  );
	   dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		   dims, int32Array, &dhsStatus ), dhsStatus, *sfStatus,
		   VOID  );
	   free( int32Array );
	   int32Array = NULL;
	   break;
	    
	case DHS_DT_UINT32:
	    if ( ( uint32Array = calloc( size, sizeof( unsigned long ) ) )
		    == NULL )
	    {
		return;
	    }
	    dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		    dims,  uint32Array, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID  );
	    dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		    dims,  uint32Array, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID  );
	    free( uint32Array );
	    uint32Array = NULL;
	    break;    	    

    	case DHS_DT_FLOAT:
	    if ( ( floatArray = calloc( size, sizeof( float ) ) ) == NULL )
	    {
		return;
	    }
	    dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		    dims, floatArray, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID  );
	    dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		    dims, floatArray, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID  );
	    free( floatArray );
	    floatArray = NULL;
	    break;

	case DHS_DT_DOUBLE:
	    if ( ( doubleArray = calloc( size, sizeof( double ) ) ) == NULL )
	    {
		return;
	    }
	    dhsCheck( dhsBdAttribAdd( object1, "multiArray", type, ndims,
		    dims, doubleArray, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID  );
	    dhsCheck( dhsBdAttribAdd( object2, "multiArray", type, ndims,
		    dims, doubleArray, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID  );
	    free( doubleArray );
	    doubleArray = NULL;
	    break;
    }

    fnReturn( VOID );
    
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddDhsKeywords
 *
 * INVOCATION:
 * sfAddDhsKeywords( dhsObject, naxis, naxes, blc, trc, instrument, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	dhsObject   (DHS_BD_OBJECT)	Dataset or frame object.
 * (>)	naxis	    (int)		Number of axies, dimensions.
 * (>)	naxes	    (long[SF_MAXDIM])	Size of each axis.
 * (>)	blc	    (long[SF_MAXDIM])	Data's Bottom, left corner.
 * (>)  instrument  (char[FLEN_VALUE])	The instrument.
 * (!)	sfStatus   (SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To add all of the keywords required by Gemini, DHS, to the
 * DHS object (dataset or a frame).
 *
 * DESCRIPTION:
 * 1. Initialize status variables.
 * 2. Add "insturment" to the DHS object.
 * 2. Add "origin" to the dataset
 * 3. Add "axissz" to the dataset
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
*/

void sfAddDhsKeywords
( 
    DHS_BD_OBJECT   dhsObject,	/* (in)  Dataset or frame object.	*/
    int		    naxis,	/* (in)  Number of axies, dimensions.	*/
    long	    naxes[SF_MAXDIM],
				/* (in)  Size of each axis. 		*/
    long	    blc[SF_MAXDIM],
				/* (in)  Data's Bottom, left corner. 	*/
    char	    instrument[FLEN_VALUE],
				/* (in)  The instrument. 		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    long	    axissz[SF_MAXDIM];
				    /* Axis size of the complete image. */
    DHS_STATUS	    dhsStatus;	    /* Status from DHS routines.	*/
    unsigned long   dims[SF_MAXDIM];/* Size of each attribute axis.	*/
    int		    	i;
    

    fnEntry ( "sfAddDhsKeywords", *sfStatus );
    statCheck( *sfStatus,  VOID );

	
    /*
     * Initialize variables.
     */

    dhsStatus = DHS_S_SUCCESS;



    /*
     * Add "origin" and "axissz" to the DHS object.
     */
	
    for( i = 0; i < naxis; i++ )
    {
	axissz[i] = naxes[i];
    }

    dims[0]=naxis;
    dhsCheck( dhsBdAttribAdd( dhsObject, "origin", DHS_DT_INT32, 1, dims,
			blc, &dhsStatus ), dhsStatus, *sfStatus, VOID );

    dhsCheck( dhsBdAttribAdd( dhsObject, "axisSize", DHS_DT_INT32, 1, dims,
			axissz, &dhsStatus ), dhsStatus, *sfStatus, VOID );

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddFrames
 *
 * INVOCATION:
 * sfAddFrames( fitsfilePtr, dataset, xtensionType, frameId, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	dataset	    (DHS_BD_DATASET)	Dataset the framses are added to.
 * (>)	data	    (boolean)		Data frame?
 * (>)	frameId	    (char[NAME_SIZE])	The frame's identifier.
 * (>)  frameName   (char[???])		
 * (<)	frame	    (DHS_BD_FRAME*)	Bottom most frame created.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To Add frames to a dataset in such a way that the resultant structure
 * correspond to the frames Identifier.
 *
 * For example if a frames identifier is "2.1" the two frames will be added.
 * First a frame with index "2" is added to the dataset, then a frame
 * with index "1" is added to the first frame.
 *
 * DESCRIPTION:
 * For each "." found in the frame Identier add a new frame to the
 * dataset (for the 1st frame created) or the previously created frame.
 * Create the frame with an index based on the framed id and name
 * "DHS-test-data".
 * Set "frame" to the last (bottom most) frame created.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfAddFrames
(
    DHS_BD_DATASET  dataset,	/* (in)  Dataset to add frames to.	*/
    boolean	    dataFrame,
    char	    frameId[NAME_SIZE],
				/* (in)  The frame's identifier. 	*/
    EPTR	    *frameList, /* (in)  Lisf of frame descriptions.	*/
				/* Frame's name.		 	*/
    DHS_BD_FRAME    *frame,	/* (out) Bottom most frame.   		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    DHS_BD_OBJECT   dhsObject;	/* DHS dataset or frame.		*/
    DHS_STATUS	    dhsStatus; 	/* Status from dhs routines.    	*/
    SF_FRAME_INFO   *frameInfo; /* Frame description.			*/
    char	    *id ;	/* A copy of the frame's identifier.	*/
    char	    *idCpy ;	/* A copy of the frame's identifier.	*/
    int		    index;	/* A particular frame's index.		*/
    char	    *tmpId;	/* Temporary frame identifier.		*/


    fnEntry( "sfAddFrames", *sfStatus );

    
    /*
     * Initizlize variables.
     */

    dhsStatus = DHS_S_SUCCESS;
    dhsObject = dataset;

    
    /*
     * frameId 0 is the primary header unit, so no frames need to
     * be added.  If no frameId then return.
     */
    
    if ( streq( frameId, "0" ) || strlen( frameId ) < 1 )
    {
	*frame = dhsObject;
	fnReturn( VOID );
    }
    
		
    /*
     * Create a new frame for every occurence of '.' in the frame's
     * identifier.
     */
    
    if ( strlen( frameId ) > 0 ) 
    {
	idCpy = id = strsav( frameId );
	tmpId = strstr( id, ".");  

	while ( tmpId != NULL )
	{
	    *tmpId = '\0';
	    str2int( id, &index );
	    nullCheck( frameInfo = sfFrameFind( frameList, idCpy,
		    sfStatus ), *sfStatus, VOID );
	    dhsCheck( dhsObject = dhsBdFrameNew( dhsObject, frameInfo->frmName,
		    index, DHS_DT_BOOLEAN, 0, NULL, NULL, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );

	    if ( tmpId != NULL )
	    {
		*tmpId = '.';
		id = tmpId + 1;
		tmpId = strstr( tmpId + 1, "." );
	    }
	} 


	if ( !dataFrame )
	{
	    str2int( id, &index );
	    nullCheck( frameInfo = sfFrameFind( frameList, frameId,
		    sfStatus ), *sfStatus, VOID );
	    dhsCheck( dhsObject = dhsBdFrameNew( dhsObject, frameInfo->frmName,
		    index, DHS_DT_BOOLEAN, 0, NULL, NULL, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );
	}
		     
	
	/*
	 * Free memory.
	 */

	if ( idCpy != NULL )
	{
	    free( idCpy );
	    id = NULL;
	}
    }
    
	
    /*
     * Set frame to the last object (frame) created.
     */
	
    *frame = dhsObject;
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddGeminiDdEntries
 *
 * INVOCATION:
 * sfAddGeminiDdEntries( ddFile, instrument, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) 	ddFile		(FILE*)			File pointer to data Dict.
 * (>)  instrument	(char[FLEN_VALUE])	The instrument.
 * (!)	sfStatus	(SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To Add the required keywords/attributes for Gemini data to the data
 * dictionary being created.
 *
 * DESCRIPTION:
 * 1. Add origin to the data dictioanary
 * 2. Add axissz to the data dictioanary
 * 3. Add frame name to the data dictioanary
 * 4. Add frame id to the data dictioanary
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

void	sfAddGeminiDdEntries
(
    FILE	    *ddFile,	    /* (in)  File pointer to data dict.	*/
    char	    instrument[FLEN_VALUE],
    				    /* (in)  The instrument. 		*/
    SF_STATUS	    *sfStatus	    /* (mod) SF library status.		*/

)
{
    fprintf( ddFile, "OK %-9s\tinstrument      \tSTRING\tINSTRUME\tF\tNULL"
	    "\tNONE\tNULL\t%s\n", instrument, INSTR_COMM );
    fprintf( ddFile, "OK %-9s\torigin          \tINT   \tORIGINS  \tF\tNULL"
	    "\tINDEX\tNULL\t%s\n", instrument, ORIGIN_COMM );
    fprintf( ddFile, "OK %-9s\taxisSize        \tINT   \tAXISSZ   \tF\tNULL"
	    "\tINDEX\tNULL\t%s\n", instrument, AXISSZ_COMM );

    fprintf( ddFile, "OK %-9s\tdataType        \tSTRING\tEXTNAME  \tF\tNULL"
	    "\tNONE\tNULL\t%s\n", instrument, DT_COMM );
    fprintf( ddFile, "OK %-9s\textensionVersion\tINT   \tEXTVER   \tF\tNULL"
	    "\tNONE\tNULL\t%s\n", instrument, EXTVER_COMM );
    fprintf( ddFile, "OK %-9s\tframeName       \tSTRING\tFRMNAME  \tF\tNULL"
	    "\tNONE\tNULL\t%s\n", instrument, FRMNAME_COMM );
    fprintf( ddFile, "OK %-9s\tframeId         \tSTRING\tFRMID    \tF\tNULL"
	    "\tNONE\tNULL\t%s\n", instrument, FRMID_COMM );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddImgData
 *
 * INVOCATION:
 * sfAddImgData( fitsfilePointer, dhsObject, frameName, index, bitpix,
 *		   naxis, *naxes, blc, trc, inc, frame, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		FITS file pointer.
 * (>)	dhsOjbect   (DHS_BD_OBJECT)	Object the data frame is added to.
 * (>)	frameName   (char[NAME_SIZE])	Frame's name.
 * (>)	index	    (int)		Frames index (id).
 * (>)	bitpix	    (int)		Number of bits per pixel.
 * (>)	naxis	    (int)		Number of dimensions.
 * (>)	naxes	    (long[SF_MAXDIM])	Size of each axis.
 * (>)	blc	    (long[SF_MAXDIM])	Data's bottom, left corner.
 * (>)	trc	    (long[SF_MAXDIM])	Data's top, right corner.
 * (<)	frame	    (DHS_BD_FRAME*)	Frame containing the data.
 * (!)	sfStatus    (SF_STATUS*)	SF library status
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Add a DHS frame, with the data read from the FITS file, to a DHS
 * object. The DHS object must be either a frame or dataset.
 *
 *
 * DESCRIPTION:
 * 1. Initialize the status indicators and fits values.	 
 * 2. Convert the naxes to unsigned for DHS routines.
 * 3. Add frames to make sure data appears at the right depth.
 * 4. Based on the type of data (bitpix) create a new frame and
 * 	  read the data from the file into the frames' data array.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfAddImgData
( 
    fitsfile	    *fptr,	/* (in)	 FITS file pointer. 		*/
    DHS_BD_OBJECT   dhsObject,	/* (in)	 Object the data frame is	*/
				/*	 added to.			*/
    char	    frameId[NAME_SIZE],
    				/* (in)  Frame identifier.		*/
    EPTR	    *frameList,	/* (in)  List of frame identifiers.	*/
    int		    bitpix,	/* (in)	 Number of bits per pixel. 	*/
    int		    naxis,	/* (in)	 Number of dimensions.		*/
    long	    naxes[SF_MAXDIM],
    				/* (in)	 Size of each axis.		*/
    long	    blc[SF_MAXDIM],
    				/* (in)	 Data's bottom, left corner. 	*/
    long	    trc[SF_MAXDIM],
    				/* (in)	 Data's top, right corner. 	*/
    DHS_BD_FRAME    *frame,	/* (out) Frame containing the data.	*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    int		    anynull;	/* Return value from cfitsio get image	*/
				/*   routines (not used).		*/
    const void	    *data;	/* Buffer for image data. 		*/
    DHS_STATUS	    dhsStatus;	/* Status from DHS routines.		*/
    int		    fitsStatus;	/* Status from cfitsio routines. 	*/
    SF_FRAME_INFO   *frameInfo; /* Frame description.			*/
    long	    group;	/* Fits file group variable (always 1).	*/
    int		    i;
    long	    inc[SF_MAXDIM];
    				/* Increment to be applied in each dim. */
    int		    index;	/* The frame's index.			*/
    short	    nullval;	/* Check for nulls in the image. 	*/
    DHS_BD_FRAME    tmpFrame;	/* The frame the data frame is to be	*/
				/* added to.				*/
    char	    tmpFrameId[NAME_SIZE];
    				/* Frame Id of the frame 1 level above  */
				/* the current frame.			*/
    unsigned long   uNaxes[SF_MAXDIM];
    				/* Array giving size of each axis.	*/


    fnEntry ( "sfAddImgData", *sfStatus );
    statCheck( *sfStatus,  VOID );


    /* 
     *Initialiaze status indicators.
     */
    
    fitsStatus = FITS_S_SUCCESS;
    dhsStatus = DHS_S_SUCCESS;

	
    /*
     * nullval = 0 prevents checking for null values in the image 
     * and the group must always be 1 (the data is in one group).
     * increment is always 1 since, we always want every pixel.
     */

    group = 1;
    nullval  = 0;
    for( i = 0; i < naxis; i++ )
    {
	inc[i] = 1;
    }


    /*
     * Make sure the images corners are defined.
     */
	
    if ( blc == NULL || trc == NULL )
    {
	*sfStatus = SF_E_CORNER;
	sfFormatMessage( *sfStatus );
	fnReturn( VOID );
    }

    /*
     * Make the dimensions of the axes into unsigned for dhs routines.
     */
    
    for ( i = 0; i < naxis; i++ )
    {
	uNaxes[i] = ( unsigned long ) ( (trc[i] - blc[i] + 1 ) );
    }



    /*
     * Add all but the last frame.
     */

    strnCpy( tmpFrameId, frameId, NAME_SIZE, *sfStatus, VOID );
    strroot( tmpFrameId );
    if ( strcmp( tmpFrameId, frameId ) == 0 ) {
	strnCpy( tmpFrameId, "", NAME_SIZE, *sfStatus, VOID );
    }
    check( sfAddFrames( dhsObject, TRUE, frameId, frameList, &tmpFrame,
	    sfStatus ), *sfStatus, VOID );
    dhsObject = tmpFrame;

    
    /*
     * Find the index for the frame.
     */
			
    if ( strext( frameId ) == NULL )
    {
	if( !str2int( frameId, &index ) )
	{
	    *sfStatus = SF_E_FRAMEID;
	    sfFormatMessage( *sfStatus, frameId );
	    fnReturn( VOID );
	}
    }
    else
    {
	if( !str2int( strext( frameId ), &index ) )
	{
	    *sfStatus = SF_E_FRAMEID;
	    sfFormatMessage( *sfStatus, strext( frameId ) );
	    fnReturn( VOID );
	}
    }

 
    nullCheck( frameInfo = sfFrameFind( frameList, frameId, sfStatus ),
	    *sfStatus, VOID );

    /* 
     * Create the new frame and add the data to them.
     */

    switch( bitpix )
    {
	case BYTE_IMG:
	    dhsCheck( *frame = dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_UINT8, naxis, uNaxes, &data, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );
			
	    fitsCheck ( ffgsvb( fptr, group, naxis, naxes, blc, trc, inc,
		    nullval, (unsigned char*)data, &anynull, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    break;
			
	case USHORT_IMG:   
	    dhsCheck( *frame = dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_UINT16, naxis,  uNaxes, &data, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );

	    fitsCheck( ffgsvui( fptr, group, naxis, naxes, blc, trc, inc,
		    nullval, (unsigned short*)data, &anynull, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    break;
			
	case SHORT_IMG:	  
	    dhsCheck( *frame = dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_INT16, naxis,  uNaxes, &data, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );
	    
	    fitsCheck( ffgsvi( fptr, group, naxis, naxes, blc, trc, inc,
		    nullval, ( short* )data, &anynull, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    break;

	case ULONG_IMG:	  
	    dhsCheck( *frame=dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_UINT32, naxis, uNaxes, &data, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );
			
	    fitsCheck( ffgsvuj( fptr, group, naxis, naxes, blc, trc, inc,
		    (long)nullval, (unsigned long*)&data, &anynull,
		    &fitsStatus ), fitsStatus, *sfStatus, VOID);
	    break;
			
	case LONG_IMG:	 
	    dhsCheck( *frame=dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_INT32, naxis, uNaxes, &data, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );

	    fitsCheck( ffgsvj( fptr, group, naxis, naxes, blc, trc, inc,
		    (long)nullval, (long*)&data, &anynull, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID);
	    break;

	case FLOAT_IMG:	    
	    dhsCheck( *frame=dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_FLOAT, naxis, uNaxes, &data, &dhsStatus),
		    dhsStatus, *sfStatus, VOID );

	    fitsCheck( ffgsve( fptr, group, naxis, naxes, blc, trc, inc,
		    (float)nullval, (float*)data, &anynull, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    break;
			
	case DOUBLE_IMG:   
	    dhsCheck( *frame=dhsBdFrameNew( dhsObject, frameInfo->frmName, index,
		    DHS_DT_DOUBLE, naxis, uNaxes, &data, &dhsStatus ),
		    dhsStatus, *sfStatus, VOID );

	    fitsCheck( ffgsvd( fptr, group, naxis, naxes, blc, trc, inc,
		    (double)nullval, (double*)data, &anynull, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID);
	    break; 

	default:
	    *sfStatus = SF_E_BITPIX;
	    sfFormatMessage( *sfStatus, bitpix );
	    break;
    }
	
    fnReturn ( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddKeyword
 *
 * INVOCATION:
 * sfAddKeyword( dhsObject, instrument, keyword, card, dataDictionaryFile,
 *	sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	object		(DHS_BD_OBJECT)		A dataset or frame object.
 * (>)  instrument	(char[FLEN_VALUE])	The instrument.
 * (>)	keyword		(char[FLEN_KEYWORD])	The keyword.
 * (>) 	card		(char[FLEN_CARD])	A cfitsio card.
 * (>) 	ddFile		(FILE*)			File pointer to data Dict.
 * (!)	sfStatus	(SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To Add a keyword to a DHS object.  No checking with the data dictionary
 * is performed.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Find the keyword's value.
 * 3. Make sure "COMMENT" and "HISTORY" keywords retain there values.
 * 4. Add the keyword/value pair to the SDS structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * sfAddKeywordWithDd.
 *
 * DEFICIENCIES:
 * Attributes that are added may not conform to the information in the
 * data dictionary, which may case problems later.
 *-
 ************************************************************************
 */

void	sfAddKeyword
(
    DHS_BD_OBJECT   object,    	    /* (mod) First dataset.		*/
    char	    instrument[FLEN_VALUE],
    				    /* (in)  The instrument. 		*/
    char	    keyword[FLEN_KEYWORD],
       				    /* (in)  The keyword.		*/
    char	    card[FLEN_CARD],/* (in)  The card from FITS file.	*/
    FILE	    *ddFile,	    /* (in)  File pointer to data dict.	*/
    SF_STATUS	    *sfStatus	    /* (mod) SF library status.		*/
)
{
    char	comm[FLEN_COMMENT];
				/* Comment field of a keyword.		*/
    char        charValue[FLEN_VALUE];
                                /* Temporary string value.              */
    double	dblValue;	/* Temporary double value.		*/
    DHS_STATUS	dhsStatus;	/* Status from DHS routines.		*/
    int		fitsStatus;    	/* Status from cfitsio routines.	*/
    char	fitsType;	/* Cfitsio data Type.			*/
    long	intValue;	/* Temporary integer value.		*/
    int		length;		/* Lenght of comment string.		*/
    int		logicalValue;	/* Temporary logical value.		*/
    char	svalue[FLEN_VALUE];
				/* Temporary value from card.		*/
    
    fnEntry( "sfAddKeyword", *sfStatus );
    statCheck( *sfStatus, VOID );

    
    /*
     * Initialize variables.
     */

    dhsStatus = DHS_S_SUCCESS;
    fitsStatus = 0;
    
    
    /*
     * Find the value for the keyword and it's type.
     */
	    
    fitsCheck( ffpsvc( card, svalue, comm, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID );
    fitsCheck( ffdtyp( svalue, &fitsType, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID);

    /*
     * EXTVER doesn't get placed on the SDS structure.  This value
     * is computed by sds2fits.
     */
    
    if ( streq( keyword, "EXTVER" ) )
    {
	fnReturn( VOID );
    }

    
    /*
     * Store cards with "/" as the first character, and no keyword,
     * as comments.
     */

    if ( strlen( keyword ) < 1 )
    {
	strnCpy( keyword, "COMMENT", FLEN_KEYWORD, *sfStatus, VOID );
    }


    /*
     * A little hack to make sure COMMENT's are kept.  The value is
     * stored in the comment field instead of the value. 
     */
    
    if ( streq( keyword, "HISTORY" ) || streq( keyword, "COMMENT" ) )
    {
	fitsType = 'C';
	if ( ( comm[0] == '=' ) || ( comm[0] == ' ' ) )
	{
	    strnCpy( svalue, comm+1, FLEN_VALUE, *sfStatus, VOID );
	}
	else
	{
	    strnCpy( svalue, comm, FLEN_VALUE, *sfStatus, VOID );	
	}
    }


    /*
     *	Make sure EXTNAME is converted to dataType
     */
    
    if ( streq( keyword, "EXTNAME" ) )
    {
	strnCpy( keyword, "dataType", FLEN_KEYWORD - 1, *sfStatus, VOID );
    }


    /*
     * Make sure the comment begins and ends with double quotes.
     */

    if ( (length = strlen( comm ) ) > 0 )
    {
	
	if ( comm[0] != '"' )
	{
	    charValue[0] = '"';
	    charValue[1] = '\0';
	    nullCheck( strcat( charValue, comm ), *sfStatus, VOID );
	    strnCpy( comm, charValue, FLEN_VALUE, *sfStatus, VOID );
	}

	if ( comm[length-1] != '"' )
	{
	    if ( length == FLEN_VALUE - 1 )
	    {
		comm[length-1] = '"';
	    }
	    else 
	    {
		comm[length+1] = '"';
		comm[length+2] = '\0';
	    }
	}
    }
    else
    {
	comm[0] = '"';
	comm[1] = '"';
	comm[2] = '\0';
    }
    
    
#ifdef DEBUG
    printf( "Adding %s to SDS structure.\n", keyword );
#endif

	    
    /*
     * Now, knowing the type, do the right conversion to svalue
     * and add the attribute.
     */
	    
    switch ( fitsType )
    {
	case 'C': 
	    fitsCheck( ffc2s( svalue, charValue, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    dhsCheck( dhsBdAttribAdd( object, keyword, DHS_DT_STRING, 0,
		    NULL, charValue, &dhsStatus ), dhsStatus, *sfStatus, VOID );
	    if( ddFile != NO_FILE )
	    {
		fprintf( ddFile, "OK %-9s\t%-18s\tSTRING\t%-9s\tF\tNULL\t"
			"NONE\tNULL\t%s\n", instrument, keyword, keyword,
			comm );
	    }
	    break;

	    
	case 'I': 
	    fitsCheck( ffc2ii( svalue, &intValue, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    dhsCheck( dhsBdAttribAdd( object, keyword, DHS_DT_INT32, 0,
		    NULL, intValue, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    if( ddFile != NO_FILE )
	    {
		fprintf( ddFile, "OK %-9s\t%-18s\tINT   \t%-9s\tF\t0\tNONE\t"
			"NULL\t%s\n", instrument, keyword, keyword, comm );
	    }
	    break;

	case 'F':
	    fitsCheck( ffc2dd( svalue, &dblValue, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    dhsCheck( dhsBdAttribAdd( object, keyword, DHS_DT_DOUBLE, 0, 
		    NULL, dblValue, &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    if( ddFile != NO_FILE )
	    {
		fprintf( ddFile, "OK %-9s\t%-18s\tDOUBLE\t%-9s\tF\t0.0\t"
			"NONE\tNULL\t%s\n", instrument, keyword, keyword,
			comm );
	    }
	    break;

	case 'L':
	    fitsCheck( ffc2ll( svalue, &logicalValue, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	    dhsCheck( dhsBdAttribAdd( object, keyword, DHS_DT_BOOLEAN, 0,
		    NULL, logicalValue, &dhsStatus ), dhsStatus,
		    *sfStatus, VOID );
	    if( ddFile != NO_FILE )
	    {
		fprintf( ddFile,  "OK %-9s\t%-18s\tBOOLEAN\t%-9s\tF\t0\t"
			"NONE\tNULL\t%s\n", instrument, keyword, keyword,
			comm );
	    }
	    break;
		    
	default:
	    *sfStatus = SF_E_FITS_TYPE;
	    sfFormatMessage( *sfStatus, fitsType );
	    break;
    }	

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddKeywordWithDd
 *
 * INVOCATION:
 * sfAddKeywordWithDd( dhsObject, instrument, keyword, card, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) 	object		(DHS_BD_OBJECT)		A dataset or frame object.
 * (>)  instrument	(char[FLEN_VALUE])	The instrument.
 * (>)	keyword		(char[FLEN_KEYWORD])	The keyword.
 * (>) 	card		(char[FLEN_CARD])	A cfitsio card.
 * (!)	sfStatus	(SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To add a keyword to the DHS objects, ensuring that the information
 * matches what is in the data dictionary.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Find the keyword's value.
 * 3. Make sure "COMMENT" and "HISTORY" keywords retain there values.
 * 4. Find the attribute and type from the data dictionary.
 * 4. Add the attribut/value pair to the SDS structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has value greater than or equal to SF_S_SUCCESS.
 *
 * "sfInit" has been executed.
 *
 * SEE ALSO:
 * sfAddKeyword.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfAddKeywordWithDd
(
    DHS_BD_OBJECT   object, 		/* (mod) DHS object.    	*/
    char	    instrument[FLEN_VALUE],
    					/* (in)  The instrument. 	*/
    char	    keyword[FLEN_KEYWORD],
       					/* (in)  The keyword.		*/
    char	    card[FLEN_CARD],	/* (in)  The card from FITS file*/
    SF_STATUS	    *sfStatus		/* (mod) SF library status.	*/
)
{
    char	    comm[FLEN_COMMENT];
				/* Comment field of a keyword.		*/
    DD_STATUS	    ddStatus;	/* Status from dd routines.		*/
    DHS_STATUS	    dhsStatus;	/* Status from DHS routines.		*/
    char	    *dPos;	/* Where the 'D' char is in a dbl.	*/
    int		    fitsStatus; /* Status from cfitsio routines.	*/
    char	    fitsType;	/* Cfitsio data Type.			*/
    DD_ROW          *row;	/* DD keyword/attribute object.		*/
    char	    svalue[FLEN_VALUE];
				/* Temporary value from card.		*/
    void	    *value;	/* Attribute's value.			*/
    
    
    fnEntry( "sfAddKeywordWithDd", *sfStatus );
    statCheck( *sfStatus, VOID );
    isInit( *sfStatus, VOID );
    


    /*
     * Initialize variables.
     */

    ddStatus = DD_S_SUCCESS;
    dhsStatus = DHS_S_SUCCESS;
    fitsStatus = 0;
    value = NULL;

    
    /*
     * Find the value for the keyword and it's type.
     */
	    
    fitsCheck( ffpsvc( card, svalue, comm, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID );
    fitsCheck( ffdtyp( svalue, &fitsType, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID);

    
    /*
     * Store cards with "/" as the first character, and no keyword,
     * as comments.
     */

    if ( strlen( keyword ) < 1 )
    {
	strnCpy( keyword, "COMMENT", FLEN_KEYWORD, *sfStatus, VOID );
    }


    /*
     * A little hack to make sure COMMENT's are kept.  The value is
     * stored in the comment field instead of the value. 
     */
    
    if ( streq( keyword, "HISTORY" ) || streq( keyword, "COMMENT" ) )
    {
	fitsType = 'C';
	if ( ( comm[0] == '=' ) || ( comm[0] == ' ' ) )
	{
	    strnCpy( svalue, comm+1, FLEN_VALUE, *sfStatus, VOID );
	}
	else
	{
	    strnCpy( svalue, comm, FLEN_VALUE, *sfStatus, VOID );	
	}
    }
	
    /* 
     * determine the datatype of a keyword value string
     */
	    
    ddCheck( ddAttribGet( instrument, keyword, &row, &ddStatus ),
	    ddStatus, *sfStatus, VOID );

#ifdef DEBUG
    printf( "Adding %s to SDS structure.\n", row->ddrAttrib );
#endif

    switch( row->ddrType )
    {
	case DD_ANY_TYPE:
	    check( sfAddKeyword( object, instrument, keyword, card,
		    NO_FILE, sfStatus ), *sfStatus, VOID );
	    break;
	    
	case DHS_DT_CHAR:
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, svalue[0], &dhsStatus ), dhsStatus, *sfStatus, VOID );
	    break;
	    
	case DHS_DT_STRING:
	    svalue[ strlen(svalue) - 1 ] = '\0';
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, svalue+1, &dhsStatus ), dhsStatus, *sfStatus, VOID );
	    break;

	case DHS_DT_BOOLEAN:
	    nullCheck( value = malloc( sizeof( boolean ) ), *sfStatus, VOID );
	    if( !str2bool( svalue, (boolean*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "boolean" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((int*)value), &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    break;

	case DHS_DT_INT8:
	    nullCheck( value = malloc( sizeof( int ) ), *sfStatus,
		    VOID );
	    if( !str2int( svalue, (int*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "8 bit integer" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((int*)value), &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    break;

	case DHS_DT_UINT8:
	    nullCheck( value = malloc( sizeof( unsigned int ) ), *sfStatus,
		    VOID );
	    if( !str2uint( svalue, (unsigned int*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "8 bit unsigned integer" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((unsigned int*)value), &dhsStatus ), dhsStatus,
		    *sfStatus, VOID );
	    break;

	case DHS_DT_INT16:
	    nullCheck( value = malloc( sizeof( short ) ), *sfStatus, VOID );
	    if( !str2short( svalue, (short*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "16 bit integer" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((short*)value), &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    break;

	case DHS_DT_UINT16:
	    nullCheck( value = malloc( sizeof( unsigned short) ), *sfStatus,
		    VOID );
	    if( !str2ushort( svalue, (unsigned short*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "16 bit unsigned integer" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((unsigned short*)value), &dhsStatus ), dhsStatus,
		    *sfStatus, VOID );
	    break;

	case DHS_DT_INT32:
	    nullCheck( value = malloc( sizeof( int ) ), *sfStatus, VOID );
	    if( !str2int( svalue, (int*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "32 bit integer" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((long*)value), &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    break;

	case DHS_DT_UINT32:
	    nullCheck( value = malloc( sizeof( unsigned int ) ), *sfStatus,
		    VOID );
	    if( !str2uint( svalue, (unsigned int*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "32 bit unsigned integer" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((unsigned long*)value), &dhsStatus ), dhsStatus,
		    *sfStatus, VOID );
	    break;

	case DHS_DT_FLOAT:
	    nullCheck( value = malloc( sizeof( float ) ), *sfStatus, VOID );
	    if( !str2float( svalue, (float*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "float" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((float*)value), &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    break;

	case DHS_DT_DOUBLE:
	    nullCheck( value = malloc( sizeof( double ) ), *sfStatus, VOID );

	    
	    /*
	     * FITS files may use 'D' exponent.  In order to make these value
	     * compatible with c libraries.
	     */

	    if( dPos = strchr( svalue, 'D' ) )
	    {
		*dPos = 'E';
	    }
	    if( !str2double( svalue, (double*)value ) )
	    {
		*sfStatus = SF_E_STR_CONV;
		sfFormatMessage( *sfStatus, "double" );
	    }
	    dhsCheck( dhsBdAttribAdd( object, row->ddrAttrib, row->ddrType, 0,
		    NULL, *((double*)value), &dhsStatus ), dhsStatus, *sfStatus,
		    VOID );
	    break;

	default:
	    *sfStatus = SF_E_DHS_TYPE;
	    sfFormatMessage( *sfStatus, row->ddrType );
	    break;
    }

    if( value != NULL )
    {
	gen_free( value );
	value = NULL;
    }
    
    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfAddOptKeywords
 *
 * INVOCATION:
 * sfAddOptKeywords( bigDataset, smallDataset, fitsfilePointer, instrument,
 *			checkDd, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	bigDs		(DHS_BD_OBJECT)		A dataset or frame object.
 * (>) 	smallDs		(DHS_BD_OBJECT)		A dataset or frame object.
 * (>)	fptr		(fitsfile*)		FITS file to read.
 * (>)	insrument	(char[FLEN_VALUE])	The instrument.
 * (>) 	checkDd		(boolean)		Use Dd to create attributes.
 * (>) 	ddFile		(FILE*)			File pointer to data Dict.
 * (!)	sfStatus	(SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To add all of the keywords, that aren't required by FITS or DHS
 * (Gemini).  In other words add all the remaining keywords to the
 * SDS structure.  These attribute are added to two DHS objects at
 * the same time.
 *
 * DESCRIPTION:
 * 1. Initialize status variables.
 * 2. For each keyword add to the dhs
 *	If it is a COMMENT or HISTORY keyword do some special handling.
 *	Convert the value read in to a value with appropriate type.
 *	Add the value to the data object ( frame or dataset )
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfAddOptKeywords
( 
    DHS_BD_OBJECT   bigDs,	/* (in)  A dataset or frame object.	*/
    DHS_BD_OBJECT   smallDs,	/* (in)  A dataset or frame object.	*/
    fitsfile	    *fptr,	/* (in)  FITS file to read.	 	*/
    char	    instrument[FLEN_VALUE],
    				/* (in)  The instrument.      	  	*/
    boolean	    checkDd,	/* (in)  Check keywords with dd.	*/
    FILE	    *ddFile,	/* (in)  File pointer to data dict.	*/
    SF_STATUS	    *sfStatus	/* (mod) Function return status.        */
)
{
    char	card[FLEN_CARD];/* Standard string defined in cfitsio. 	*/

    int		fitsStatus;    	/* Status from cfitsio routines.	*/ 
    int		i;
    int		keypos;		/* Next keyword to be read (not used).	*/
    char	*keyword;	/* FITS keyword.			*/
    int		nkeys;		/* Number of FITS keywords.		*/


    fnEntry ( "sfAddOptKeywords", *sfStatus );
    statCheck( *sfStatus,  VOID );


    /*
     * Initialize variables.
     */
	
    fitsStatus = 0;
    keyword = NULL;


    /* 
     * get the number of keywords 
     */
    
    fitsCheck( ffghps( fptr, &nkeys, &keypos, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID );
    
    for ( i = 1; i <= nkeys; i++ )  
    {
	/*
	 * Get the card from the file and find the keyword.
	 */
	
	fitsCheck( ffgrec( fptr, i, card, &fitsStatus ), fitsStatus,
		*sfStatus, VOID );

	check( keyword = sfExtractKeyword( card, sfStatus ), *sfStatus,
		VOID );

	/*
	 * Ignore blank lines.
	 */
	
	if ( strlen( card ) < 1 )
	{
	    if( keyword != NULL )
	    {
		gen_free( keyword );
		keyword = NULL;
	    }
	    continue;
	}


	/*
	 * Skip the unnecessary keywords.
	 */

	if ( streq( keyword, "BITPIX" ) ||
		streq( keyword, "END" ) ||
		streq( keyword, "EXTEND" ) ||
		streq( keyword, "EXTNAME" ) ||
		streq( keyword, "EXTVER" ) ||
		streq( keyword, "FRAMEID" ) |
		streq( keyword, "GCOUNT" ) ||
		streq( keyword, "INSTRUME" ) ||
		streq( keyword, "PCOUNT" ) ||
		streq( keyword, "SIMPLE" ) ||
		streq( keyword, "XTENSION" ) ||		
		strncmp( keyword, "NAXIS", 5 ) == 0 )
	{
	    if( keyword != NULL )
	    {
		gen_free( keyword );
		keyword = NULL;
	    }
	    continue;
	}
	

	/*
	 * Check value with data dictionary if desired.
	 */

	if ( checkDd )
	{
	    check( sfAddKeywordWithDd( bigDs, instrument, keyword, card,
		    sfStatus ), *sfStatus, VOID );
	    check( sfAddKeywordWithDd( smallDs, instrument, keyword, card,
		    sfStatus ), *sfStatus, VOID );
	}
	else
	{
	    check( sfAddKeyword( bigDs, instrument, keyword, card, ddFile,
		    sfStatus ), *sfStatus, VOID );
	    check( sfAddKeyword( smallDs, instrument, keyword, card,
		    NO_FILE,  sfStatus ), *sfStatus, VOID );
	}
	if( keyword != NULL )
	{
	    gen_free( keyword );
	    keyword = NULL;
	}
    }	
    
    fnReturn( VOID );
}

/***********************************************************************
 *+
 * FUNCTION NAME:
 * sfCalcCorners
 *
 * INVOCATION:
 * sfCalcCorners( naxes, numxc, numyc, whichx, whichy, blc, trc, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	naxes	    (long[SF_MAXDIM])	Array giving size of each axis.
 * (>) 	numxc	    (int)		Number of divisions on x axis.
 * (>)	numyc	    (int)		Number of divisions on y axis.
 * (>)	whichx	    (int)    		Which x division we are using.
 * (>) 	whichy      (int)		Number of divisions on y axis.
 * (<)	blc	    (long[SF_MAXDIM])	Bottom left corner of the data.
 * (<)	trc	    (long[SF_MAXDIM])	Top right corner of the data.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Calculate the bottom left corner, and the top right corner of the
 * data chunk.
 *
 * DESCRIPTION:
 * 1. determine the corners for the first axis (x axis).
 * 2. determine the corners for the second axis (y axis).
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * sfCalcPos.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfCalcCorners
(
    long	naxes[SF_MAXDIM],
				/* (in)  Array giving size of each axis.*/
    int		numxc,		/* (in)  Number of divisions on x axis.	*/
    int		numyc,		/* (in)  Number of divisions on y axis.	*/
    int		whichx,		/* (in)  Which x division we are using.	*/
    int		whichy,		/* (in)  Number of divisions on y axis.	*/
    long	blc[SF_MAXDIM],	/* (out) Bottom left corner.		*/
    long  	trc[SF_MAXDIM],	/* (out) Top right corner.		*/
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)
{

    long    	*xPos;  /* Division points of the chunks along the x axis.*/
    long    	*yPos;  /* Division points of the chunks along the x axis.*/
    
    fnEntry ( "sfCalcCorners", *sfStatus );
    statCheck( *sfStatus,  VOID );


    /*
     * Calculate the division points on both of the axis.
     */
    
    nullCheck( xPos = long_alloc ( numxc + 1 ), *sfStatus, VOID );
    nullCheck( yPos = long_alloc ( numyc + 1 ), *sfStatus, VOID ); 
    check( sfCalcPos( naxes, numxc, numyc, xPos, yPos, sfStatus ),
	    *sfStatus, VOID );

    
    /* 
     * Find the points along the x axis
     */
    
    if ( numxc == 1 )
    {
	blc[0] = xPos[0];
	trc[0] = xPos[1];
    } 
    else
    {
	blc[0] = xPos[whichx % ( numxc + 1 )];
	if ( whichx == ( numxc - 1 ) )
	{
	    trc[0] = xPos[ numxc ];
	}
	else
	{
	    trc[0] = xPos[( whichx % ( numxc + 1 ) ) + 1] - 1;
	}
    }


    /* 
     * Find the points along the y axis
     */
    
    if ( numyc == 1 )
    {
	blc[1] = yPos[0];
	trc[1] = yPos[1];
    } 
    else
    {
	blc[1] = yPos[whichy % ( numyc + 1 )];
	if ( whichy == ( numyc - 1) )
	{
	    trc[1] = yPos[ numyc ];
	}
	else
	{
	    trc[1] = yPos[( whichy % ( numyc + 1 ) ) + 1] - 1;
	}
    }	


    /*
     * Free memory
     */
	
    gen_free( xPos );
    xPos = NULL;
    
    gen_free( yPos );
    yPos = NULL;
    
    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfCalcPos
 *
 * INVOCATION:
 * sfCalcPos( naxis, naxes, numPieces, positions, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) 	naxes	(long[SF_MAXDIM])	Array giving size of each axis.
 * (>)	numxc	(int)			Number of divisions on x axis.
 * (>) 	numyc	(int)			Number of divisions on y axis.
 * (<)	xPos	(long*)			List of division points on x axis.
 * (<)	yPos	(long*)			List of division points on y axis.
 * (!)	sfStatus( SF_STATUS*)	   	SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Calculate the x and y positions for breaking up the data into chunks.
 * This routine is intended for 2-D blocks of data.
 *
 * DESCRIPTION:
 * For each dimension (i) 
 *     For each division on dimension i
 *	   Position[i] is assigned the list of positions.
 *    
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * sfCalcCorners.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfCalcPos
( 
    long	naxes[SF_MAXDIM],   
			   /* (in)  Array giving size of each axis.	*/
    int		numxc,	   /* (in)  Number of divisions on x axis.	*/
    int		numyc,	   /* (in)  Number of divisions on y axis.	*/
    long	*xPos,	   /* (out) List of division points on x axis.	*/
    long	*yPos,	   /* (out) List of division points on y axis.	*/
    SF_STATUS	*sfStatus  /* (mod) Function return status.	 	*/
)
{
    int		i;	
    
    fnEntry ( "sfCalcPos", *sfStatus );
    statCheck( *sfStatus,  VOID );

	
    /*
     * Find the positions along the x axis.
     */

    xPos[0] = 1;
    for( i=1; i<=numxc; i++ )
    {
	if ( i == 1 )
	{
	    xPos[i] = naxes[0] / ( unsigned long )numxc;
	}
	else
	{
	    xPos[i] = xPos[i-1] + ( naxes[0] / ( unsigned long )numxc );
	}
    }
    if ( xPos[numxc] != naxes[0] ) 
    {
	xPos[numxc] = naxes[0];
    }

	
    /*
     * Find the positions along the y axis
     */

    yPos[0] = 1;
    for( i=1; i<=numyc; i++ )
    {
	if ( i == 1 )
	    yPos[i] =  naxes[1] / ( unsigned long )numyc;
	else
	    yPos[i] = yPos[i-1] + ( naxes[1] / ( unsigned long )numyc );
    }
    if ( yPos[numyc] != naxes[1] ) 
    {
	yPos[numyc] = naxes[1];
    }
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfCreatePrimeFrame
 *
 * INVOCATION:
 * sfCreatePrimeFrame ???
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!)	objectList  (EPTR*)		List of DHS objects created.
 * (!)	bigDs	    (DHS_BD_OBJECT) 	Dataset containing full FITS file.
 * (>)	instrument  (char[FLEN_VALUE])	Instrument.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To create a frame with the necessary keywords for the the prime
 * header unit.
 *
 * DESCRIPTION:
 * ????
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

void	sfCreatePrimeFrame
(
    EPTR	    *objectList,    /* (mod) List of DHS objects created*/
    DHS_BD_OBJECT   bigDs,	    /* (mod) SDS object of FULL file.	*/
    char	    instrument[FLEN_VALUE],
				    /* (in)  Instrument			*/
    int		    naxis,	    /* (in)  Number of axies, dimensions.*/
    long	    naxes[SF_MAXDIM],
				    /* (in)  Size of each axis. 	*/
    EPTR	    *frameList,	    /* (in)  List of frame descriptions.*/
    SF_STATUS  	    *sfStatus	    /* (mod) SF library status.		*/
)
{
    DHS_BD_FRAME    bigFrame;	/* Frame for the big dataset.		*/
    long	    blc[SF_MAXDIM];
				/* Data's Bottom, left corner. 		*/
    DHS_STATUS	    dhsStatus;	/* Status from DHS routines.		*/
    int		    extVer;
    int		    i;
    SF_OBJECT	    *object; 	/* New object and information.		*/
    DHS_BD_OBJECT   smallDs;	/* Small dataset, contains 1 chunk.	*/
    DHS_BD_FRAME    smallFrame;	/* Frame for the small dataset.		*/
    
    fnEntry( "sfCreatePrimeFrame", *sfStatus );
    statCheck( *sfStatus, VOID );


    dhsStatus = DHS_S_SUCCESS;
    extVer = 1;
    for ( i = 0; i < naxis; i++ ) {
	blc[i] = 1;
    }
    
    

    /*
     * Add the Prime Frame to the big dataset.
     */
    
    check( sfAddFrames( bigDs, FALSE, "1", frameList, &bigFrame, sfStatus ),
	    *sfStatus, VOID );
    dhsCheck( dhsBdAttribAdd( bigFrame, "dataType", DHS_DT_STRING, 0, NULL,
	    "SCI", &dhsStatus ), dhsStatus, *sfStatus, VOID );
    dhsCheck( dhsBdAttribAdd( bigFrame, "extensionVersion", DHS_DT_INT32, 0,
	    NULL, extVer, &dhsStatus ), dhsStatus, *sfStatus, VOID );

    check( sfAddDhsKeywords( bigFrame, naxis, naxes, blc, instrument,
	    sfStatus ), *sfStatus, VOID );
 
    /*
     * Create a dataset with the Prime Frame info only.
     */ 

    dhsCheck( smallDs = dhsBdDsNew( &dhsStatus ), dhsStatus, *sfStatus,
		VOID );
    check( sfAddFrames( smallDs, FALSE, "1", frameList, &smallFrame, sfStatus ), *sfStatus, VOID );
    dhsCheck( dhsBdAttribAdd( smallDs, "instrument", DHS_DT_STRING, 0, NULL,
	    (char*)instrument, &dhsStatus), dhsStatus, *sfStatus, VOID );
    dhsCheck( dhsBdAttribAdd( smallFrame, "dataType", DHS_DT_STRING, 0, NULL,
	    "SCI", &dhsStatus ), dhsStatus, *sfStatus, VOID );
    dhsCheck( dhsBdAttribAdd( smallFrame, "extensionVersion", DHS_DT_INT32, 0,
	    NULL, extVer, &dhsStatus ), dhsStatus, *sfStatus, VOID );
    
    check( sfAddDhsKeywords( smallFrame, naxis, naxes, blc, instrument,
	    sfStatus ), *sfStatus, VOID );
        

    /*
     * Add the small Dataset to the list of SDS objects.
     */

    nullCheck( object = malloc( sizeof( SF_OBJECT ) ), *sfStatus, VOID );
    object->dhsObject = smallDs;
    nullCheck( object->id = strnsav( "1", NAME_SIZE - 1 ), *sfStatus, VOID );
    nullCheck( objectList = eptr_insert( objectList, eptrlen( objectList ),
	    object ), *sfStatus, VOID );
    
    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfDoImgChunking
 *
 * INVOCATION:
 * sfDoImgChunking( fitsfilePointer, bigDataset, objectList, bitpix,
 *	naxis, naxes, instrument, numxy, numyc, frameId, array, sfStatus )
 *
 * Parameters: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)	    	FITS file pointer.
 * (!)	bigDs	    (DHS_BD_OBJECT) 	Dataset containing full FITS file.
 * (!)	objectList  (EPTR*)		List of DHS objects created.
 * (>)	bitpix	    (int)		Number of bits per pixel.
 * (>)	naxis	    (int)		Number of dimensions.
 * (>)	naxes	    (long[SF_MAXDIM])	Size of each dimension.
 * (>)	instrument  (char[FLEN_VALUE])	Instrument.
 * (>)	numxc	    (int)		Number of divisions on x axis.
 * (>)	numyc	    (int)		Number of divisions on y axis.
 * (>)	frameId	    (char[NAME_SIZE])	Current frame identifier.
 * (>)	array	    (boolean)		Add Random arrays (yes/no)?
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To break a block of data into small chunks and store each data chunk
 * in a separate frames.  The data chunks are stored on the large
 * dataset containing the full file information and small datasets are
 * created for each chunk to simulate data chunkss from an instrument.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Make sure there is at least one chunk.
 * 3. Find the positions of the data chunks.
 * 4. For each chunk
 * 	Find the "index" for the frame.
 *	Add the new data frame to the big dataset, contains full FITS file.
 *	Create a small dataset for this data chunk only.
 *	Add the data and necessary keywords to the small dataset.
 *	Add the small dataset to the object list.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfDoImgChunking
(
    fitsfile	    *fptr,	/* (in)  FITS file pointer.		*/
    DHS_BD_OBJECT   bigDs,	/* (mod) Dataset containing full file.	*/
    EPTR	    *objectList,/* (mod) List of DHS objects created. 	*/
    int		    bitpix,	/* (in)  Number of bits per pixel.	*/
    int		    naxis,	/* (in)  Number of dimensions.		*/
    long	    naxes[SF_MAXDIM],
    				/* (in)  Size of each dimension.	*/
    char	    instrument[FLEN_VALUE],
				/* (in)  Instrument			*/
    int		    numxc,	/* (in)  Number of divisions on x axis.	*/
    int		    numyc,	/* (in)  Number of divisions on y axis.	*/
    boolean	    array,	/* (in)  Add random arrays.		*/
    char	    frameId[NAME_SIZE],
    				/* (in)  Frame identifier.		*/
    EPTR	    *frameList,	/* (in)  List of frame descriptions. 	*/
    char	    extname[NAME_SIZE],
    long	    extver,
    SF_STATUS  	    *sfStatus	/* (mod) SF library status.		*/
)
{
    DHS_BD_FRAME    bigFrame;	/* Frame for the big dataset.		*/
    long    	    blc[SF_MAXDIM];
    				/* Bottom left corner.			*/
    int		    chunkIndex;	/* Chunk counter.			*/
    DHS_STATUS	    dhsStatus;	/* Status from DHS routines.		*/
    SF_OBJECT	    *object; 	/* New object and information.		*/
    DHS_BD_OBJECT   smallDs;	/* Small dataset, contains 1 chunk.	*/
    DHS_BD_FRAME    smallFrame;	/* Frame for the small dataset.		*/
    long	    trc[SF_MAXDIM];
    				/* Top right corner.			*/
    int		    xc;		/* x axis counter.			*/
    int		    yc;		/* y axis ocunter.			*/
    
    fnEntry( "sfDoImgChunking", *sfStatus );
    statCheck( *sfStatus, VOID );


    /*
     * Initialize variables.
     */
	
    dhsStatus = DHS_S_SUCCESS;

	
    /*
     * Make sure we have at least one chunk.
     */

    if ( ( numxc * numyc ) < 1 )
    {
	sfFormatMessage( SF_E_CHUNK, numxc * numyc );
	*sfStatus = SF_E_CHUNK;
	fnReturn( VOID );
    }


    /* 
     * Determine the positions of the data chuncks. 
     */

    for( xc = 0, chunkIndex = 0; xc < numxc; xc++ )
    {
	for( yc = 0; yc < numyc; yc++ )
	{
	    chunkIndex++;

			
	    /*
	     * Calculate the bottom-left corner, top-right corner
	     * and increment for this data chunk.  Note that the
	     * increment is always 1, we always want every pixel.
	     */

	    sfCalcCorners( naxes, numxc, numyc, xc, yc, blc, trc,
		    sfStatus );

			
	    /* 
	     * Add a frame with the data to the big dataset.
	     */
	    
	    check( sfAddImgData( fptr, bigDs, frameId, frameList, bitpix,
		    naxis, naxes, blc, trc, &bigFrame, sfStatus ),
		    *sfStatus, VOID );
		
	    check( sfAddDhsKeywords( bigFrame, naxis, naxes, blc, 
		    instrument, sfStatus ), *sfStatus, VOID );

	    
	    /* 
	     * Create a new small dataset to contain this chunk only.
	     */
		
	    dhsCheck( smallDs = dhsBdDsNew( &dhsStatus ), dhsStatus,
		    *sfStatus, VOID );

	    dhsCheck( dhsBdAttribAdd( smallDs, "instrument", DHS_DT_STRING,
		    0, NULL, instrument, &dhsStatus), dhsStatus, *sfStatus,
		    VOID );

	    check( sfAddImgData( fptr, smallDs, frameId, frameList, bitpix,
		    naxis, naxes, blc, trc, &smallFrame, sfStatus), *sfStatus,
		    VOID );
	    
	    check( sfAddDhsKeywords( smallFrame, naxis, naxes, blc,
		    instrument, sfStatus ), *sfStatus, VOID );

	    
	    /*
	     * Randomly decide if an array should be added.
	     */

	    if( array && ( rand() % 10 ) )
	    {
		check( sfAddArray( bigFrame, smallFrame, sfStatus ),
			*sfStatus, VOID );
	    }
		    
	    
	    /*
	     * Add the small dataset, with one chunk, to the objectList.
	     */
		
	    nullCheck( object = malloc( sizeof( SF_OBJECT ) ), *sfStatus,
		    VOID );
	    object->dhsObject = smallDs;
	    nullCheck( object->id = strnsav( frameId, NAME_SIZE - 1 ),
		    *sfStatus, VOID );
	    nullCheck( objectList = eptr_insert( objectList,
		    eptrlen( objectList ), object ), *sfStatus, VOID );

	}
    }

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfFindFrameId
 *
 * INVOCTAION:
 * frameInfo = sfFindFrameId( extensionName, extensionVersion, frameNumber );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	extname	    (char[FLEN_VALUE)	Value of the "EXTNAME" keyword.
 * (>)	extver	    (long)		Value of the "EXTVER" keyword.
 * (>) 	frameNum    (int)		Number of the current frame.
 *
 * FUNCTION VALUE:
 * NULL 	    : If an error occurs.
 * SF_FRAME_INFO*   : Frame information containing the frameId and frameName 
 *
 * PURPOSE:
 * To create a unique frame identifier.
 *
 * The unique frame identifier is either
 * extver.x where x is a number value associated with the extname
 *						OR
 * frameNum if there are no values for the extname and extver variables.
 *
 * DESCRIPTION:
 * 1. Find the unique frame identifier.
 * 2. Find the name of the frame.
 *
 * EXTERNAL VARIABLES:
 * extverList - List of extversions and a counter for each.
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

SF_FRAME_INFO *sfFindFrameId
(
    char	extname[FLEN_VALUE],	/* (in)  EXTNAME keyword value.	*/
    long	extver,			/* (in)  EXTVER keyword value.	*/
    int		frameNum,		/* (in)  Current frame number.	*/
    SF_STATUS  	*sfStatus		/* (mod) SF library status.	*/
)
{
    int			index;
    char		parentId[ FLEN_VALUE ];
    					/* Parent frame Identifier.	*/
    char		frameId[ FLEN_VALUE ];
    					/* Frame Identifier. 		*/
    EXTVER_INFO		*extverInfo;	/* EXTVER value.		*/
    SF_FRAME_INFO	*newFrame;	/* Description of the frame.	*/
    long 		tmpExtver;	/* Tmporary extension. 		*/
        
    fnEntry( "sfFindFrameId", *sfStatus );
    statCheck( *sfStatus, NULL );

    if (  extver <= 0 )
    {
	/*
	 * If "extver" doesn't have a value the frameid is the frame Number.
	 */

	sprintf( frameId, "_%d", frameNum - 1 );
	nullCheck( newFrame = sfNewFrameInfo( "", frameId, 0, sfStatus ),
		*sfStatus, NULL );
	sprintf( newFrame->frmName, "%s:$d", "SCI", frameNum - 1);
    }
    else {
	/*
	 * Adjust for indexing by 1.
	 */
	
	tmpExtver = extver;

	if ( eptr_search( extverList, (pointer)&tmpExtver, &index,
		compareExtver ) )
	{
	    /*
	     * If "extver" was found in the extverList then use it.
	     */

	    extverInfo = eptritem( extverList, index );	
	    sprintf( parentId, "%d", extverInfo->extver );
	    sprintf( frameId, "_%d", ++extverInfo->last );
	    nullCheck( newFrame = sfNewFrameInfo( parentId, frameId, 0,
		    sfStatus ), *sfStatus, NULL );
	    sprintf( newFrame->frmName, "%s_%d", extname, extverInfo->extver );
	}
	else
	{
	    /*
	     * If extver was NOT found craete a new one.
	     */

	    extverInfo = malloc( sizeof( EXTVER_INFO ) );
	    extverInfo->extver = tmpExtver;
	    extverInfo->last = 0;
	    extverList = eptr_insert( extverList, eptrlen( extverList),
		    extverInfo );
	    sprintf( frameId, "_%d", extverInfo->extver );
	    nullCheck( newFrame = sfNewFrameInfo( "", frameId, 0, sfStatus ),
		    *sfStatus, NULL );
	    sprintf( newFrame->frmName, "%s_%d", extname, extverInfo->extver );
	}
    }
    

    fnReturn( newFrame );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfFits2Sd
 *
 * INVOCATION:
 * sfFits2Sds( fitsfilePtr, numxc, numyc, checkDd, array, makeDd,
 *	objectList, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fitsFilename	(char*)		Filename of the input FITS file
 * (>)	numxc		(int)		Number of data divisions on the
 *					x axis.
 * (>)	numyc		(int)		Number of data divisions on the
 *					y axis.
 * (>)	checkDd		(boolean)	Check data with data dictionary.
 * (>)	ddMake		(boolean)	Generate a data dictionary.
 * (>)	array		(boolean)	Add random arrays to objects.
 *					is to be created.
 * (!)	objectList	(EPTR*)		List of all datasets created.
 * (!)	sfStatus	(SF_STATUS)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Function to convert a FITS file to a dhs dataset (SDS object).
 *
 * DESCRIPTION:
 * 1.  Initialize status variables
 * 2.  Open the FITS file for reading
 * 3.  Create a new, empty dataset to hold all of the FITS file information.
 * 4.  Create a new, empty dataset to hold chunks of information from the
 *     FITS file.
 * 5.  Read the FITS required keywords.
 * 6.  Add the required keywords to both of the datasets.
 * 7.  Read the remainder of the keywords
 * 8.  Add the remainder of the keywords to the datasets.
 * 9.  If there is any data chunk it.
 * 10. Add the small dataset to the object list.
 * 11. For each FITS extension found
 *	 Create a small dataset for this extensions keywords.
 *	 Add any keywords found to the dataset and find the Frame identifier.
 *  	 Add a frame with the chunk of data to the small dataset and
 *  		to the big dataset.
 *	 If there is any data "chunk it".
 *	 Add the small dataset to the object list.
 * 12. Add the dataset with the full file information to the object list.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfAddFrames
 * - sfAddOptKeywords
 * - sfDoImgchunking
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfFits2Sds
( 
    char        *fitsFilename,	/* (in)  FITS filname.			*/
    int	        numxc,	    	/* (in)  Number of divisions on the
				   x axis for chunking.			*/ 
    int	        numyc,	    	/* (in)  Number of divisions on the
				   y axis for chunking.			*/
    boolean	checkDd,	/* (in)  Check with data dictionary.	*/
    boolean	makeDd,		/* (in)  Generate a data dictionary.	*/
    boolean	array,		/* (in)  Add random arrays.		*/
    EPTR        *objectList,	/* (mod) List of all datasets created.	*/
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)					
{
    int		    bitpix;	/* Number of bits per pixel.		*/
    DHS_BD_OBJECT   bigDs;	/* Sds object contain full FITS file.	*/
    DHS_BD_FRAME    bigFrame;   /* Data frame structure.		*/
    long	    blc[SF_MAXDIM];
    				/* Coords of bottom left corner.	*/
    char	    comm[FLEN_COMMENT];
				/* Comment field of a keyword attribute.*/
    FILE	    *ddFile;	/* Generated DD disk file.		*/
    char	    *ddFilename;/* File name of the generated DD.	*/
    DHS_STATUS	    dhsStatus;  /* Status from DHS routines.    	*/
    boolean	    doExt;	/* TRUE if extension are to be process.	*/
    int		    extend;	/* May extensions be present?		*/
    char	    extname[FLEN_VALUE];
				/* Value of the "EXTNAME" keyword. 	*/
    int		    extver;	/* Value of the "EXTVER"  keyword.	*/
    char	    extverString[FLEN_VALUE];
       				/* String value of "EXTVER" keyword.	*/
    int		    fitsStatus; /* Status from cfitsio routines.	*/ 
    fitsfile	    *fptr;		/* FITS file pointer (input).	*/
    int		    frameNum;	/* Frame Number.			*/
    char	    frameId[NAME_SIZE];
				/* A unique frame identifier. 		*/
    SF_FRAME_INFO   *frameInfo; /* Frame description.			*/
    EPTR	    *frameList; /* List of frame information.		*/
    long	    gcount;	/* Number of random groups.		*/
    int		    hdutype;/* Return value from ffmahd (not used).	*/
    int		    i;
    char	    instrument[FLEN_VALUE];
				/* Value of the instrument keyword.	*/
    long	    naxes[SF_MAXDIM];
    				/* Array giving size of each axis.	*/
    int		    naxis;	/* Number of axes in the data array.	*/
    SF_OBJECT	    *object;	/* Dhs object and information.		*/
    long	    pcount;	/* Number of group parameters.		*/
    int		    simple;	/* Is the file fits standard?		*/
    DHS_BD_DATASET  smallDs;	/* Small dataset, contains one chunk.	*/
    DHS_BD_FRAME    smallFrame; /* Frame for the small dataset.		*/
    char	    *temp;	/* Temporary char pointer.		*/
    
					   
    fnEntry ( "sfFits2Sds", *sfStatus );
    statCheck( *sfStatus,  VOID );

    
    /*
     * Initialize variables
     */

    ddFilename = NULL;
    dhsStatus = DHS_S_SUCCESS;
    doExt = TRUE;
    nullCheck( extverList = eptr_alloc( 10 ), *sfStatus, VOID );
    fitsStatus = 0;
    frameNum = 0;
    nullCheck( frameList = sfFrameListCreate( 10, sfStatus ), *sfStatus,
		VOID );
    hdutype = 0;
    extver = NULL_VERSION;
    srand( time( NULL ) );
    
        
    /*
     * Open the inFile for reading 
     */
    
    check( fptr = sfOpenFitsFile( fitsFilename, "READONLY", sfStatus ),
			*sfStatus, VOID );

    /*
     * Open the file for the data dictionary if we are required to
     * make one.
     */
    
    if( makeDd )
    {
	nullCheck( ddFilename = strnsav( strtail( fitsFilename ),
		strlen( strtail( fitsFilename) ) + 4 ), *sfStatus, VOID );
	temp = strrchr( ddFilename, '.' );
	if ( temp == NULL )
	{
	    ddFilename[strlen( ddFilename ) ] = '.';
	    ddFilename[strlen( ddFilename ) + 1 ] = 'd';
	    ddFilename[strlen( ddFilename ) + 2 ] = 'd';
	    ddFilename[strlen( ddFilename ) + 3 ] = '\0';
	}
	else
	{
	    *(temp + 1) = 'd';
	    *(temp + 2) = 'd';
	    *(temp + 3) = '\0';
	}

	if( ( ddFile = fopen( ddFilename, "w") ) == NULL )
	{
	    *sfStatus = SF_E_DD_FILE;
	    sfFormatMessage( *sfStatus, "opening", ddFilename );
	}
    }
    else
    {
	ddFile = NO_FILE;
    }

    
    /*
     * Create a new, empty dataset to hold primary header
     * details only.
     */

    dhsCheck( bigDs = dhsBdDsNew( &dhsStatus ), dhsStatus, *sfStatus, VOID );

    
    /*
     * Create a new, empty dataset to hold primary header
     * details only.
     */

    dhsCheck( smallDs = dhsBdDsNew( &dhsStatus ), dhsStatus, *sfStatus,
			VOID );


    /*
     * Read the required header keywords.
     */

    
    if ( ffgkys( fptr, "INSTRUME", instrument, comm, &fitsStatus ) )
    {
        #ifdef DEBUG		
	    printf( "Error reading instrument keyword, so using instr = *\n" );
        #endif
        strnCpy( instrument, "*", FLEN_VALUE, *sfStatus, VOID );
	fitsStatus = 0;
    }

    if ( ddFile != NO_FILE )
    {
	check ( sfAddGeminiDdEntries ( ddFile, instrument, sfStatus ), *sfStatus,
		VOID );
    }
    

    /*
     * Add the instrument keyword to the datasets.
     */

    dhsCheck( dhsBdAttribAdd( smallDs, "instrument", DHS_DT_STRING, 0,
	    NULL, (char*)instrument, &dhsStatus), dhsStatus, *sfStatus,
	    VOID );
	
    dhsCheck( dhsBdAttribAdd( bigDs, "instrument", DHS_DT_STRING, 0,
	    NULL, (char*)instrument, &dhsStatus), dhsStatus, *sfStatus,
	    VOID );
	
    fitsCheck( ffghpr( fptr, SF_MAXDIM, &simple, &bitpix, &naxis, naxes,
	    &pcount, &gcount, &extend, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID );


    /*
     * Get the keywords.
     */

    check( sfAddOptKeywords( bigDs, smallDs, fptr, instrument, checkDd,
	    ddFile, sfStatus ), *sfStatus, VOID );	  


    /*
     * Write the small fits file with the primary header information
     * and add it to our list of datasets.
     */

    nullCheck( object = malloc( sizeof( SF_OBJECT ) ), *sfStatus, VOID );
    object->dhsObject = smallDs;
    nullCheck( object->id = strnsav( "", NAME_SIZE - 1 ), *sfStatus,
			VOID );
    nullCheck( objectList = eptr_insert( objectList, eptrlen( objectList ),
			object ), *sfStatus, VOID );

    
    /*
     * If there is data after the primary HDU create chunks.  If there
     * is data the extensions are not processed.
     */

    if( sfHasData( naxis, naxes ) )
    {
	/*
	 * ??? Create a proper frame header for this data.
	 */

	sprintf( frameId, "_%d", 1 );
	nullCheck( frameInfo = sfNewFrameInfo( "", frameId, 0, sfStatus ),
		*sfStatus, VOID );
	strnCpy( frameInfo->frmName, "SCI_1", FLEN_VALUE, *sfStatus, VOID );
	check( sfFrameListInsert( &frameList, frameInfo, sfStatus ),
		*sfStatus, VOID );
	nullCheck( frameList = eptr_sort( frameList, sfFrameCompare ),
		*sfStatus, VOID );

	check( sfCreatePrimeFrame( objectList, bigDs, instrument, naxis,
		naxes, frameList, sfStatus ), *sfStatus, VOID );
	check( sfDoImgChunking( fptr, bigDs, objectList, bitpix, naxis, naxes,
		instrument, numxc, numyc, array, "1", frameList,
		"SCI", 1, sfStatus), *sfStatus, VOID );
	doExt = FALSE;
    }


    /*
     * Find the remaining HDU's, if there are any.
     */

    
    for ( frameNum = 2; !( ffmahd( fptr, frameNum, &hdutype, &fitsStatus ) )
	      && fitsStatus == 0 && doExt == TRUE ;  frameNum++ )
    {
	/*
	 * Find the "EXTNAME" and "EXTVER" keywords and use them
	 * to calculate the frame identifier.
	 */
	
	ffgkey( fptr, "EXTNAME", extname, comm, &fitsStatus );
	if ( fitsStatus != FITS_S_SUCCESS )
	{
	    strnCpy( extname, "SCI", FLEN_VALUE, *sfStatus, VOID );

	    fitsStatus = FITS_S_SUCCESS;
	} else {
	    fitsCheck( ffc2s( extname, extname, &fitsStatus ),
		    fitsStatus, *sfStatus, VOID );
	}
	
	ffgkey( fptr, "EXTVER", extverString, comm, &fitsStatus );
	if ( fitsStatus != FITS_S_SUCCESS )
	{
	    extver = frameNum;
	    fitsStatus = FITS_S_SUCCESS;
	}
	else
	{
	    if ( ! str2int( extverString, &extver ) )
	    {
		extver =  -1;
	    }
	}
	

	nullCheck ( frameInfo = sfFindFrameId( extname, extver, frameNum,
		sfStatus ), *sfStatus, VOID );
	check( sfFrameListInsert( &frameList, frameInfo, sfStatus ),
		*sfStatus, VOID );
	nullCheck( frameList = eptr_sort( frameList, sfFrameCompare ),
		*sfStatus, VOID );	

	/*
	 * Find the bitpix, naxis, and naxes values from the FITS file.
	 */
	
	fitsCheck( ffghpr( fptr, SF_MAXDIM, &simple, &bitpix, &naxis, naxes,
		&pcount, &gcount, &extend, &fitsStatus ), fitsStatus,
		*sfStatus, VOID );
	for ( i = 0; i < naxis; i++ ) {
	    blc[i] = 1;
	}

	
	/*
	 * Create a new, empty dataset to hold the extension
	 * header details only.
	 */

	dhsCheck( smallDs = dhsBdDsNew( &dhsStatus ), dhsStatus, *sfStatus,
		VOID );
	check( sfAddFrames( smallDs, FALSE, frameInfo->frmId, frameList,
		&smallFrame, sfStatus ), *sfStatus, VOID );
	dhsCheck( dhsBdAttribAdd( smallDs, "instrument", DHS_DT_STRING, 0,
		NULL, (char*)instrument, &dhsStatus), dhsStatus, *sfStatus,
		VOID );
	dhsCheck( dhsBdAttribAdd( smallFrame, "dataType", DHS_DT_STRING,
		0, NULL, extname, &dhsStatus), dhsStatus, *sfStatus, VOID );
	dhsCheck( dhsBdAttribAdd( smallFrame, "extensionVersion", DHS_DT_INT32,
		0, NULL, extver, &dhsStatus), dhsStatus, *sfStatus, VOID );
	check( sfAddDhsKeywords( smallFrame, naxis, naxes, blc, instrument,
		sfStatus ), *sfStatus, VOID );

	
	/*
	 * Create a new frame for the big dataset, with the DHS
	 * keywords.
	 */
	
	check( sfAddFrames( bigDs, FALSE, frameInfo->frmId, frameList, &bigFrame,
		sfStatus ), *sfStatus, VOID );
	dhsCheck( dhsBdAttribAdd( bigFrame, "dataType", DHS_DT_STRING,
		0, NULL, extname, &dhsStatus), dhsStatus, *sfStatus, VOID );
	dhsCheck( dhsBdAttribAdd( bigFrame, "extensionVersion",	DHS_DT_INT32,
		0, NULL, extver, &dhsStatus), dhsStatus, *sfStatus, VOID );
	check( sfAddDhsKeywords( bigFrame, naxis, naxes, blc, instrument,
		sfStatus ), *sfStatus, VOID );


	/*
	 * Add the optional keywords to both the big and small frames.
	 */

	check( sfAddOptKeywords( bigFrame, smallFrame, fptr, instrument,
		checkDd, ddFile, sfStatus ), *sfStatus, VOID );


	/*
	 * Write the small fits file with the header information.
	 */

	nullCheck( object = malloc( sizeof( SF_OBJECT ) ), *sfStatus, VOID );
	object->dhsObject = smallDs;
	nullCheck( object->id = strnsav( frameInfo->frmId, NAME_SIZE - 1 ),
		*sfStatus, VOID );
	nullCheck( objectList = eptr_insert( objectList, eptrlen( objectList ),
		object ), *sfStatus, VOID );
    
	/*
	 * If there is data after the HDU create chunks
	 */
	
	if( sfHasData( naxis, naxes ) )
	{
	    check( sfDoImgChunking( fptr, bigDs, objectList, bitpix, naxis,
		    naxes, instrument, numxc, numyc, array, frameInfo->frmId,
		    frameList, extname, extver, sfStatus ), *sfStatus, VOID );
	}
	
    }
    


    if ( fitsStatus != END_OF_FILE && doExt == TRUE)
    {
	sfFormatMessage( SF_E_FITS_FILE);
	*sfStatus = SF_E_FITS;
    }
    else
    {
	fitsStatus = FITS_S_SUCCESS;
    }
    
    
    /*
     * Add the full file information to the object list.
     */
	 
    fitsCheck( ffclos( fptr, &fitsStatus ), fitsStatus, *sfStatus, VOID );

    nullCheck( object = malloc( sizeof( SF_OBJECT ) ), *sfStatus, VOID );
    object->dhsObject = bigDs;
    nullCheck( object->id = strnsav( "", NAME_SIZE - 1 ), *sfStatus, VOID );
    nullCheck( objectList = eptr_insert( objectList, eptrlen( objectList ),
			object ), *sfStatus, VOID );

    if ( makeDd )
    {
	fclose( ddFile );
    }
    
    check( sfFrameListDelete( frameList, sfStatus ), *sfStatus, VOID );

    eptr_free( extverList, free );
    if( ddFilename != NULL )
    {
	gen_free( ddFilename );
	ddFilename = NULL;
    }
    	
    fnReturn( VOID );
}
