static char rcsid[] = "$Id: sds2fits.c,v 1.1.1.1 2002-11-24 20:35:46 brighton Exp $";
/************************************************************************
****  C A N A D I A N	A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1997				(c) 1997
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
*					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.	 NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
*					sation du logiciel.
*
************************************************************************
*
* FILENAME
* sf/src/sds2fits.c
*
* PURPOSE:
* Library to convert between FITS and SDS file formats.
*
*
* FUNCTION NAME(S)
* sfCreateCard		: Turn a "bad" keyword/value pair into a FITS
*			  comment.
* sfDoChunkFrame	: Write the frames found in a DHS object as
*			  FITS extensions in a FITS file composed of
*			  exactly 1 DHS object.
* sfDoChunkPrimeHdu	: Write the primary header unit to a FITS file 
*			  composed from exactly 1 DHS object.
* sfDoFrame		: Write the frames found in a DHS object as
*			  FITS extensions in a FITS file composed of
*			  1 or more DHS objects.
* sfDoPrimeHdu		: Write the primary header unit to a FITS file 
*			  composed from 1 or more DHS objects.
* sfGetDhsKeywords	: Find the values of the keywords required for
*			  DHS.
* sfGetImgKeywords	: Get the required keywords for an image extension.
* sfIsData		: Determine if a partiuclar attribute is a data.
* sfIsFrame		: Determine if a particular attribute is a frame
*			  a frame attribute.
* sfIsDataset		: Determine whether DHS object is a DHS frame or
*			  a dataset.
* sfNewBinHdu		: Creat a new binary header unit in a FITS file.
* sfNewImgHdu		: Create a new image header unit in a FITS file.
* sfSds2Fits   		: Converts an DHS dataset to a FITS file.
* sfUpdateImgKeywords	: Make sure the IMG keywords are correct in the
*			  extension information.
* sfWriteBadIndexedArray: Write out an indexed array as comments.
* sfWriteBinData	: Write the data in a binary extension.
* sfWriteBinExtension	: Writes keyword and stores info to create a new
*			: FITS binary extension later.
* sfWriteBinKeywords	: Write the required keywords of a binary
*			  extension to a FITS file.
* sfWriteDhsKeywords	: Write all of the keywords required by DHS.
* sfWriteFrame		: Write a frame's keywords and data to a FITS file.
* sfWriteImgData	: Write a block of image data to a FITS file.
* sfWriteImgKeywords	: Write the FITS required keywords for an Image
*			  extension or the FITS primary header unit.
* sfWriteIndexedArray	: Write an indexed array to a given FITS file.
* sfWriteOptKeywords	: Writes all of the keywords not required by
*			  the DHS library or FITS files to a given
*			  FITS file.
* sfWriteScalarVal	: Write a scalar value to a given FITS file.
*
*INDENT-OFF*
* $Log: not supported by cvs2svn $
* Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
*
*
* Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
* Initial install into CVS of dhs-0.16
*
* Revision 1.19  1999/02/12 18:10:55  jaeger
* Added checks and messages for handling data sizes which are larger than
* the "axisSz" values given.
*
* Revision 1.18  1999/02/09 20:38:27  jaeger
* Removed "." in sprintf's in sfCreateCard since it is no longer limiting the size.
* Added "EXTVER" keyword to binary tables.  Removed unnecessary code (OMITs).
* Added sfWriteBinExtension().  sfWriteDhsKeywords() now returns the number
* of keywords it puts in the Hdr. Fixed various type bugs - using wrong types
* between FITS and DHS.
*
* Revision 1.17  1999/01/26 18:28:46  jaeger
* Added axis boundry checks.  Display error messages with the attribute
* value rather than the keyword.  Corrected bugs in sfWriteBadIndexedArray.
* Fixed bugs in "char" indexed arrays.
*
* Revision 1.16  1998/05/25 16:12:33  jaeger
* Fixed a bug, sfCreateCard had a sprintf that was missing an argument.
*
* Revision 1.15  1998/05/14 18:49:40  jaeger
* Added functionality to ensure the AXISSZ and ORIGIN values always get
* written, even when there is no data.
*
* Revision 1.14  1998/05/05 16:33:00  jaeger
* Added proper creation of Binary extensions for attributes that are
* multi-dimensional arrays.  FRMID, FRMNAME, and EXTVER are all
* created from the information gathered from the SDS structure.
* EXTNAME is determined by the "dataType" attribute.
*
* Revision 1.13  1997/12/02 21:08:01  jaeger
* Fixed problems with strncat to make sure the string isn't longer than
* it is supposed to me.  Fixed problems in sfCreateCard with respect to
* floating point numbers overrunning the character buffer.
*
* Revision 1.12  1997/11/21 00:21:48  jaeger
* Fixed problem with data being separated from headers during composition.
*
* Revision 1.11  1997/11/19 00:12:18  jaeger
* Added sorting of "extension" list in the dataset information, removed
* sfPrintMessage, and fixed bug in fits2sds (big dataset did not have
* the proper frame structure).
*
* Revision 1.10  1997/09/17 22:55:49  jaeger
* Fixed bugs - frmName not intialiazed and absence of EXTNAME and
* FRMNAME in the "composed" FITS file.
*
* Revision 1.9  1997/09/05 23:04:53  jaeger
* fixed remaining memory leaks.
*
* Revision 1.8  1997/09/05 18:58:07  jaeger
* Fixed some memory leaks
*
* Revision 1.7  1997/08/29 23:15:42  dunn
* Added free of attribData in sfWriteOptKeywords.
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
*  Checked in for install and testing with dhs
*
* Revision 1.1	1997/07/09 16:33:19  jaeger
* Initial revision
*
*INDENT-ON*
*
****  C A N A D I A N	A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdio.h> 
#include <unistd.h>
#include <math.h>


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
#include "frameInfo.h"
#include "extension.h"
#include "array.h"

/*
 ************************************************************************
 * local definitions
 ************************************************************************
 */

#define DATA		    "_dataArray" 	
#define DHS_COMMENT         "Internal, temporary DHS quicklook keyword.\0"
#define FRAME 		    "_frame" 
#define NAME 	   	    "_name"
#define SF_BAD_COMM  	    "BAD:"


/*
 ************************************************************************
 *  Local function declarations.
 ************************************************************************
 */

static 	char	*sfCreateCard( char*, DHS_DATA_TYPE, void*, char[],
			SF_STATUS* );
static	void	sfDoChunkFrame( fitsfile*, SF_FRAME_INFO*, SF_DS_INFO*, char[],
			SF_STATUS* );
static	void	sfDoChunkPrimeHdu( fitsfile*, DHS_BD_DATASET, SF_DS_INFO*,
			char[], SF_STATUS* );
static	void	sfDoFrame( fitsfile*, SF_FRAME_INFO*, SF_DS_INFO*, char[],
			SF_STATUS* );
static	void	sfDoPrimeHdu( fitsfile*, DHS_BD_DATASET, SF_DS_INFO*, char[],
			SF_STATUS* );
static	void	sfGetDhsKeywords( DHS_BD_OBJECT, char[], int*, long[], long[],
			char[], char[], SF_STATUS*);
static	void	sfGetImgKeywords( DHS_BD_OBJECT, char[], int*, int*, long[],
			SF_STATUS* );
static	boolean	sfIsData( char* );
static	boolean sfIsDataset( DHS_BD_OBJECT );
static	boolean	sfIsFrame( char* );
static	void	sfNewBinHdu( fitsfile*, SF_ARRAY_INFO*, char[], boolean,
			SF_DS_INFO*, SF_STATUS* );
static	boolean	sfNewImgHdu( fitsfile*, DHS_BD_OBJECT, char[], boolean,
			boolean, SF_DS_INFO*, SF_EXT_INFO*, SF_STATUS* );
static	void	sfUpdateDhsKeywords( SF_DS_INFO*, SF_FRAME_INFO*,
			SF_EXT_INFO*, SF_STATUS* );
static	void	sfUpdateImgKeywords( int, int, long[], SF_EXT_INFO*,
			SF_STATUS* );
static 	void	sfWriteBadIndexedArray( fitsfile*, char[], DHS_DATA_TYPE,
			int, unsigned long[], char[], void*, SF_STATUS* );
static 	void	sfWriteBinData( fitsfile*, int, unsigned long[], DHS_DATA_TYPE,
			void*, SF_STATUS* );
static	void	sfWriteBinExtension( fitsfile*, EPTR**, char[],
			DHS_BD_ATTRIB_ID, char*, char[], DHS_DATA_TYPE,
			char[], SF_STATUS* );
static 	void	sfWriteBinKeywords( fitsfile*, char[], DHS_DATA_TYPE, 
			SF_ARRAY_INFO*, SF_STATUS* );
static 	int	sfWriteDhsKeywords( fitsfile*, SF_FRAME_INFO, SF_EXT_INFO*,
			boolean, int, char[], long[], long[], SF_STATUS*);
static	int	sfWriteFrame( fitsfile*, SF_EXT_INFO*, SF_FRAME_INFO*,
			SF_DS_INFO*, boolean, boolean, char[], SF_STATUS*);
static	void	sfWriteImgData( fitsfile*, DHS_BD_FRAME, boolean, long[],
			long[], SF_STATUS*);
static	void	sfWriteImgKeywords( fitsfile*, int, int, long[], boolean,
			SF_STATUS* );
static	void	sfWriteIndexedArray( fitsfile*, char[], char*, DHS_DATA_TYPE,
		int, unsigned long[], char[], void*, SF_STATUS* );
static	int	sfWriteOptKeywords( fitsfile*, SF_FRAME_INFO*, SF_DS_INFO*,
			char[], boolean, EPTR*, EPTR**, SF_STATUS* );
// there is a lot of confusion happening here between these two arguments
// opt for that order:
//    char *attribute
//    char keyword[FLEN_KEYWORD]
static	void	sfWriteScalarVal( fitsfile*, char*, char[], DHS_DATA_TYPE,
			char[],	void*, SF_STATUS * );

void		sfFormatMessage();

/*
************************************************************************
*+
* FUNCTION NAME:
* sfCreateCard
*
* INVOCATION:
* comment = sfCreateCard( keyword, type, value, comment, status );
*
* PARAMETERS: (">" input, "!" modified, "<" output)
* (>)	attribute(char*)		SDS attribute Name
* (>) 	type	 (DHS_DATA_TYPE)	The keyword's type.
* (>)	value	 (void*)		The keyword's value.
* (>)	comm	 (char[FLEN_COMMENT])	The keyeword's comment.
* (!)  sfStatus (SF_STATUS*)		Function status for SF library.	
*
* FUNCTION VALUE:
* (char*) : String composed as follows "keyword value", if no errors
*	     occured.
* (char*) : NULL, if errors occured.
*
* PURPOSE:
* To create a string to be used as a value for a FITS COMMENT keyword.
* Typically used when a problem has been found with the given keyword,
* its value, or its type.
*
* DESCRIPTION:
* 1. Copy the keyword to the leftmost portion of the commentString.
* 2. Add the keyword's value 4 spaces after the keyword.
* 3. Add the keyword's comment 4 spaces after the value.
*
* EXTERNAL VARIABLES:
* None.
*
* PRIOR REQUIREMENTS:
* "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS. .
*
* SEE ALSO:
* None.
*
* DEFICIENCIES:
* None.
*-
************************************************************************
*/

char	*sfCreateCard
(
    char	    *attribute,	/* (in)  SDS attribute name.		*/
    DHS_DATA_TYPE   type,	/* (in)  The keyword's type.		*/
    void	    *data,	/* (in)  The keyword's value.		*/
    char	    comm[FLEN_COMMENT],
				/* (in)  The keyword's commment. 	*/
    SF_STATUS	    *sfStatus	/* (mod) Function status for SF library.*/
)
{
    char	    *charData;	/* The string constructed from the	*/
				/* keyword and its value.		*/
		
    fnEntry( "sfCreateCard", *sfStatus );
    statCheck( *sfStatus, NULL );


    /*
     * Allocate memory and add the keyword's value to the comment string.
     */
    
    nullCheck( charData = char_alloc( FLEN_CARD ), *sfStatus, NULL );

    if ( data == NULL )
    {
	(void) sprintf( charData, "%-s = BINARY EXTENSION / ", attribute );
    }
    else
    {
	switch( type )
	{	
	    case DHS_DT_BOOLEAN:
		if ( *( ( DHS_BOOLEAN* )data ) )
		{
		    (void) sprintf( charData, "%-s = %20c / ", attribute, 'T' );
		}
		else
		{
		    (void) sprintf( charData, "%-s = %20c / ", attribute, 'F' );
		}
		break;
			
	    case DHS_DT_CHAR:
		(void) sprintf( charData, "%-s = '%18s' / ", attribute, data );
		break;
		
	    case DHS_DT_STRING:
		(void) sprintf( charData, "%-s = '", attribute );
		strnCat( charData, (char*)data, FLEN_CARD - 4, *sfStatus,
		    NULL );
		strnCat( charData, "' / ", FLEN_CARD, *sfStatus, NULL );
		break;
			
	    case DHS_DT_INT8:
		(void) sprintf( charData, "%-s = %20d / ", attribute,
			*((char*)data) );
		break;
			
	    case DHS_DT_INT16:
		(void) sprintf( charData, "%-s = %20d / ", attribute,
			*((short*) data) );
		break;
	    
	    case DHS_DT_INT32:
		(void) sprintf( charData, "%-s = %20d / ", attribute,
			*((int*)data) );
		break;
			
	    case DHS_DT_UINT8:
		(void) sprintf( charData, "%-s = %20u / ", attribute,
			*((unsigned char*)data) );
		break;
			
	    case DHS_DT_UINT16:
		(void) sprintf( charData, "%-s = %20hu / ", attribute,
			*((unsigned short*)data) );
		break;
		
	    case DHS_DT_UINT32:
		(void) sprintf( charData, "%-s = %20lu / ", attribute,
			*((unsigned int*)data) );
		break;
			
	    case DHS_DT_FLOAT:
		/*
		 * For some unknown reason there are memory errors if the
		 * precision is more than 14.
		 */
		(void) sprintf( charData, "%-s = %19E / ", attribute,
			*( ( float* )data ) );
		break;
			
	    case DHS_DT_DOUBLE:
		(void) sprintf( charData, "%-s = %19E / ", attribute,
			*( ( double* )data ) );
		break;
			
	    default:
		*sfStatus = SF_E_DHS_TYPE;
		sfFormatMessage( *sfStatus, type );
		fnReturn( NULL );
	}
    }
    

    strnCat( charData, comm, FLEN_CARD, *sfStatus, NULL ); 
    fnReturn( charData );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfDoChunkFrame
 *
 * INVOCATION:
 * sfDoChunkFrame( fitsfilePointer, frameInformation, datasetInformation,
 *			instrument, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fptr	    (fitsfile*)		FITS file pointer.
 * (>) frameInfo    (SF_FRAME_INFO*)	Details about the image.
 * (>) dsInfo	    (SF_DS_INFO*)	Details about the dataset.
 * (>) instrument   (char[FLEN_VALUE])	The instrument used to get the image.
 * (!) sfStatus	    (SF_STATUS*)	Function status for SF library.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Write the partial or entire information for some image data.
 *
 * This information is stored in a single FITS file that contains
 * one "chunk" of information for an observation.  An observation
 * "chunk" can be all of the information associated with a particular
 * observation or some subset of it.
 *
 * DESCRIPTION:
 * 1. Check if there is an extension associated with this frame in
 *	  the dataset information, if there isn't create one.
 * 2. Create a new extension header in the FITS file for this frame,
 *    with the keywords required by FITS.
 * 3. Find and write the keywords that are required by DHS, Gemini
 *	  standards.
 * 4. Write any other keywords, if any, to the FITS file.
 * 5. If this frame has data then write the data to the FITS file.
 * 6. Process any subframe, if any, found for this frame.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS. .
 *
 * SEE ALSO:
 * - sfDoPrimeHdu
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfDoChunkFrame
(
    fitsfile	    *fptr,	/* (in)  Fits file pointer.		*/
    SF_FRAME_INFO   *frameInfo,	/* (in)  Details about the image.	*/
    SF_DS_INFO	    *dsInfo,	/* (in)  Details about the dataset.	*/
    char	    instrument[FLEN_VALUE],
				/* (in)  The instrument. 		*/
    SF_STATUS	    *sfStatus	/* (mod) Function return status.	*/
)
{
    SF_EXT_INFO	*ext;  		/* Extension information.		*/

    fnEntry( "sfDoChunkFrame", *sfStatus);
    statCheck( *sfStatus, VOID );


    /*
     * Initialize variables.
     */

    ext = NULL;

	
    /*
     * Try and find the extension information, if one is not found
     * then create a new one.
     */

    if ( dsInfo->exts != NULL )
    {	
	ext = sfFindExt( dsInfo->exts, frameInfo->frmId, sfStatus );
    }
    
    if ( *sfStatus == SF_E_EXT_NOT_FOUND || ext == NULL )
    {
	*sfStatus = sfFnStatus;
	nullCheck( ext = sfAddExt( dsInfo, frameInfo->frmId, sfStatus ),
		*sfStatus, VOID );
	ext->type = SF_IMAGE;
    }


    /*
     * Create a new header unit in the FITS file.
     */

    check( frameInfo->hasData = sfNewImgHdu( fptr, frameInfo->sdsId,
	    instrument, TRUE, FALSE, dsInfo, ext, sfStatus ),
	    *sfStatus, VOID );

    

    /*
     * Write the contents of the frame to a FITS file.
     */
    
    check( ext->numKeywords += sfWriteFrame( fptr, ext, frameInfo, dsInfo,
	    TRUE, FALSE, instrument, sfStatus ), *sfStatus, VOID );

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfDoChunkPrimeHdu
 *
 * INVOCATION:
 * sfDoChunkPrimeHdu( fitsfilePointer, dataset, datasetInformation,
 *                    instrument, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fptr		(fitsfile*)		The fits file pointer.	
 * (>) dataset  	(DHS_BD_DATASET)	DHS dataset.
 * (!) dsInfo		(SF_DS_INFO*)		Information on the dataset.
 * (>) instrument  	(char[FLEN_VALUE])	The instrument.
 * (!) sfStatus		( SF_STATUS*)		Sf library status indicator.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle writing the primary Header for a FITS file containing one
 * "chunk" of an observation.
 *
 * An observation "chunk" can be all of the information associated
 * with a partiuclar observation or some subset of it.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Move to the top of the FITS file.
 * 3. Create the primary header unit with the required FITS keywords.
 * 4. Add "CHUNK", gemini required keywords, to the FITS file.
 * 5. Write the remaining keywords, if any, to the FITS file.
 * 6. Make sure there was no data found.
 * 7. Process the frames found in the dataset provided.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS. .
 *
 * SEE ALSO:
 * - sfDoChunkFrame.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfDoChunkPrimeHdu
(
    fitsfile	    *fptr,	/* (in)	 The fits file pointer.		*/
    DHS_BD_DATASET  dataset,	/* (in)	 dataset or frame object.	*/
    SF_DS_INFO	    *dsInfo,	/* (mod) dataset information.		*/
    char	    instrument[FLEN_VALUE],
				/* (in)	 instrument.			*/
    SF_STATUS	    *sfStatus	/* (mod) sf library status indicator.	*/
)
{
    FITS_STATUS		fitsStatus;	/* From cfitsio routines.	*/
    SF_FRAME_INFO	*frameInfo;	/* Information on a frame.	*/
    int			hdutype;	/* Type of the current hdu.	*/
    
    fnEntry( "sfDoChunkPrimeHdu", *sfStatus );
    statCheck( *sfStatus, VOID );


    /*
     * Initialize the variables.
     */

    fitsStatus = FITS_S_SUCCESS;
	

    /*
     * Move to the top of the FITS file.
     */

    fitsCheck( ffmahd( fptr, 1, &hdutype, &fitsStatus ), fitsStatus, *sfStatus,
	    VOID );


    /*
     * Create the primary header with the require FITS keywords.
     */
    
    check( sfNewImgHdu( fptr, dataset, instrument, TRUE, TRUE, dsInfo, NULL,
	   sfStatus ), *sfStatus, VOID );


    /*
     * Write "CHUNK" keyword to FITS file with value "TRUE".
     */


    fitsCheck( ffpkyl( fptr, "CHUNK", TRUE, DHS_COMMENT, &fitsStatus ),
            fitsStatus, *sfStatus, VOID );


    /*
     * Create a dummy frame Information structure to be used with,
     * "sfWriteFrame".
     */

    nullCheck( frameInfo = sfNewFrameInfo( "", "", dataset, sfStatus ),
	    *sfStatus, VOID );

	
    /*
     * Write the contents of the frame to a FITS file.
     */
    
    check( dsInfo->numKeywords += sfWriteFrame( fptr, NULL, frameInfo,
	    dsInfo, TRUE, TRUE, instrument, sfStatus ), *sfStatus, VOID );
	
    gen_free( frameInfo );
	

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfDoFrame
 *
 * INVOCATION:
 * sfDoframe( fptr, frameInfo, dhsObject, dsInfo, instrument, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		FITS file pointer.
 * (>)	frameInfo   (SF_FRAME_INFO*)	Information on the frame.
 * (>) 	dsInfo	    (SF_DS_INFO*)	Information on the dataset.
 * (>)  instrument  (char[FLEN_VALUE])	The instrument.
 * (!)	sfStatus    (SF_STATUS*)	Function return status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To write the information contained in a frame into a FITS
 * extension.  The information may be all or some subset of the
 * frames information.
 *
 * The FITS file this will be written is composed of 1 or more
 * observations chunks. An observation "chunk" can be all of the
 * information associated with a partiuclar observation or some
 * subset of it.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Find the extension information for the frame.
 * 3. Create a new FITS extension for this frame if there isn't one,
 *    otherwise move to the appropriate location in the FITS file.
 * 4. Write any frame attributes found as keywords in the FITS file.
 * 5. Get the list of subframes for this frame and its data identifier.
 * 6. Write the DHS, Gemini, required keywords FITS file.
 * 7. Write the data to the FITS file.
 * 8. Process the subframes that were found, if any.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * The dataset information is complete.  The number of keywords for
 * each header have been filled out.  The dataset information is filled
 * in when writing the observation chunks, one-by-one, to separate
 * FITS files.  Thus, the composing of observation chunks must occur
 * after each of the observations chunks have been written to their
 * own FITS files.
 *
 * SEE ALSO:
 * - sfDoPrimeHdu.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfDoFrame
(
    fitsfile	    *fptr,	/* (in)  The fits file pointer.		*/
    SF_FRAME_INFO   *frameInfo,	/* (in)  Information on the frame.	*/
    SF_DS_INFO	    *dsInfo,	/* (mod) Information on the dataset.	*/
    char	    instrument[FLEN_VALUE],
				/* (in)  The instrument. 		*/
    SF_STATUS	    *sfStatus   /* (mod) Function return status.	*/
)
{
    int		    bitpix;	/* Number of bits per pixel.		*/
    SF_EXT_INFO	    *ext;	/* Extension information.		*/
    FITS_STATUS	    fitsStatus; /* From cfitsio routines.		*/
    int		    hdutype;	/* Type of the current hdu.		*/
    int	    	    naxis;	/* Number of dimension of data.		*/
    long	    naxes[SF_MAXDIM];
    				/* Size of each data dimension.		*/
    int             numKeywords;/* Num. of keywords found (not used).   */    

    fnEntry( "sfDoFrame", *sfStatus );
    statCheck( *sfStatus, VOID );
    

    /*
     * Initialize variables.
     */

    fitsStatus = FITS_S_SUCCESS;
    ext = NULL;
    
    
    /*
     * Find the extension information associated with this frame.
     */

    ext = sfFindExt( dsInfo->exts, frameInfo->frmId, sfStatus );
    if ( *sfStatus == SF_E_EXT_NOT_FOUND || ext == NULL )
    {
    	sfFormatMessage( *sfStatus, frameInfo->frmId );
	return VOID;
    }

    
    /*
     * Either create a new FITS extension header or move to the
     * correct extension in the FITS file.
     */

    if ( ext->fitsIndex == NULL_INDEX )
    {
	check( frameInfo->hasData = sfNewImgHdu( fptr, frameInfo->sdsId,
		instrument, FALSE, FALSE, dsInfo, ext,
		sfStatus ), *sfStatus, VOID );
    }
    else
    {
	check( sfGetImgKeywords( frameInfo->sdsId, instrument, &bitpix,
		&naxis, naxes, sfStatus ), *sfStatus, VOID );
	if ( naxis > 0 )
	{
	    frameInfo->hasData = TRUE;
	}
	else
	{
	    frameInfo->hasData = FALSE;
	}
	fitsCheck( ffmahd( fptr, ext->fitsIndex, &hdutype, &fitsStatus ),
		fitsStatus, *sfStatus, VOID );
    }
	

    /*
     * Write the contents of the frame to a FITS file.
     */
    
    check( numKeywords = sfWriteFrame( fptr, ext, frameInfo, dsInfo, FALSE,
	    FALSE, instrument, sfStatus ), *sfStatus, VOID );

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfDoPrimeHdu
 *
 * INVOCATION:
 * sfDoPrimeHdu( fitsfilePointer, dataset, datasetInformation, instrument,
 *			sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		The fits file pointer.		
 * (>)  dataset	    (DHS_BD_DATASET)  	One piece of observation data.
 * (!)	dsInfo	    (SF_DS_INFO*)	Information on the dataset.	
 * (>)	instrument  (char[FLEN_VALUE])	Instrument that gathered the data. 
 * (!)	sfStatus    (SF_STATUS*)	SF library status indicator.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To write the information contained in this observation "chunk" to a
 * FITS file composed of all the observation "chunks".  Thus, ths FITS
 * file will contain all of the information for one observation.
 *
 * An observation "chunk" can be all of the information associated with
 * a partiuclar observation or some subset of it.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Move to the top of the FITS file.
 * 3. Create the FITS primary header unit, if it doesn't already exist.
 * 4. Write any attributes found to the FITS file as keywords.
 * 5. Make sure there is no data following the primary header unit.
 * 6. Process the frames found, if any.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * The dataset information is complete.  The number of keywords for
 * each header have been filled out.  The dataset information is filled
 * in when writing the observation chunks, one-by-one, to separate
 * FITS files.  Thus, the composing of observation chunks must occur
 * after each of the observations chunks have been written to their
 * own FITS files.
 *
 * SEE ALSO:
 * - sfDoFrame.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfDoPrimeHdu
(
    fitsfile	    *fptr,	/* (in)	 The fits file pointer.		*/
    DHS_BD_DATASET  dataset,	/* (in)	 One piece of observation data.	*/
    SF_DS_INFO	    *dsInfo,	/* (mod) Information on the dataset.	*/
    char	    instrument[FLEN_VALUE],
				/* (in)	 The instrument that gathered	*/
				/*       the data.			*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status indicator.	*/
)
{
    FITS_STATUS	    fitsStatus; /* From cfitsio routines.		*/
    SF_FRAME_INFO   *frameInfo; /* Details concerining this frame.	*/
    int		    hdutype;    /* Type of the current hdu.		*/
    int             numKeywords;/* Num. of keywords found (not used).   */
   
    fnEntry( "sfDoPrimeHdu", *sfStatus );
    statCheck( *sfStatus, VOID );


    /*
     * Initialize the variables.
     */
	
    fitsStatus = FITS_S_SUCCESS;


    /*
     * Move to the top of the FITS file.
     */
	
    fitsCheck( ffmahd( fptr, 1, &hdutype, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID );


    /*
     * If the primary header unit doesn't already exist then create it.
     */

    if ( dsInfo->last == NULL_INDEX )
    {
	check( sfNewImgHdu( fptr, dataset, instrument, FALSE, TRUE, dsInfo,
		NULL, sfStatus ), *sfStatus, VOID );
    }
    

    /*
     * Set up a dummy frame inforamtion to be used by "sfWriteFrame".
     */


    nullCheck( frameInfo = sfNewFrameInfo( "", "", dataset, sfStatus ),
	    *sfStatus, VOID );
    
	
    /*
     * Write the contents of the frame to a FITS file.
     */
    
    check( numKeywords = sfWriteFrame( fptr, NULL, frameInfo, dsInfo,
	    FALSE, TRUE, instrument, sfStatus ),  *sfStatus, VOID );
    	    
    gen_free( frameInfo );
	
	
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfGetDhsKeywords
 *
 * INVOCATION:
 * sfGetDhsKeywords( dhsObject, instrument, naxis, bottomLeftCorner,
 *		axisSize, frmName, extName, extVer, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * 	(>)	dhsObject   (DHS_BD_OBJECT)	DHS dataset or frame object.
 * 	(>) 	instrument  (char[FLEN_VALUE])	The instrument.
 * 	(>)	naxis	    (mod)		Number of dimensions.
 * 	(<)	blc	    (long[SF_MAXDIM])  	The origin, bottom left corner,
 *						of this image piece in the full.
 *	(<) 	axissz	    (long[SF_MAXDIM])	Axis size of the full image.
 * 	(<)	frmName	    (char[FLEN_VALUE])	Frame name.
 * 	(<)	extName	    (char[FLEN_VALUE])	EXTNAME keyword value.
 *	(<)	extVer	    (*long)		EXTVER keyword value.
 * 	(!)	sfStatus    (SF_STATUS*)	Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To Get the Gemini, DHS, required keywords from the dataset.
 *
 * DESCRIPTION:
 * 1. Find the "origin".
 * 2. Find the "axisSize".
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" must have value SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfWriteDhsKeywords.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfGetDhsKeywords
(
    DHS_BD_OBJECT   dhsObject,	/* (in)	 DHS frame object.		*/
    char	    instrument[FLEN_VALUE],
				/* (in)  The instrument.		*/
    int	    	    *naxis,	/* (in)	 Number of dimensions.		*/
    long	    blc[SF_MAXDIM],
				/* (mod) Bottom left corner.		*/
    long	    axissz[SF_MAXDIM],
				/* (mod) Top right corner.		*/
    char	    frmName[FLEN_VALUE],
    char	    extName[FLEN_VALUE],
    SF_STATUS	    *sfStatus	/* (mod) Function return status.	*/
)
{
    unsigned long   dims[SF_MAXDIM];
				/* Array of dims of attribute data. 	*/
    int		    i;
    int		    ndims;	/* Number of dimensions.	    	*/
    DHS_DATA_TYPE   type;	/* Type of dhs attrib data object.  	*/
    void	    *value;	/* Value found for an attribute.	*/
    

    fnEntry( "sfGetDhsKeywords", *sfStatus );
    statCheck( *sfStatus, VOID );


    /*
     * Find the frameName
     */

    check( sfGetValue( dhsObject, TRUE, "_name", &type, &ndims, dims,
	    (void**)&value, sfStatus ), *sfStatus, VOID );
    if ( ndims != 0 )
    {
	*sfStatus = SF_E_ATTRIB_DIMS;
	sfFormatMessage( *sfStatus, ndims, "_name" );
	frmName[0]='\0';
    }
    else
    {
	strnCpy( frmName, (char*)value, FLEN_VALUE, *sfStatus, VOID );
    }


    /*
     * Find attribute "origin", the bottom left corner of the data chunk.
     */

    if ( blc != NULL )
    {
	sfGetValue( dhsObject, FALSE, "origin", &type, &ndims, dims,
		(void**)&value, sfStatus );
	if ( *sfStatus < SF_S_SUCCESS  )
	{
	    *sfStatus = sfFnStatus;
	    *naxis = 0;
	    blc = NULL;
	}
	else {
	    if ( *naxis == 0 && dims[0] != 0 )
	    {
		*naxis = dims[0];
	    }
	    else if ( dims[0] != *naxis || ndims != 1 )
	    {
		*naxis = 0;
		blc = NULL;
		*sfStatus = SF_E_ORIGIN_DIMS;
		sfFormatMessage( *sfStatus, ndims, dims[0] );
		fnReturn( VOID );
	    }
	    memcpy( blc, value, sizeof( long ) *  dims[0] );
	}
    }


    /*
     * Find attribute "axisSize", the dimension of the full data array.
     */

    if ( axissz != NULL  )
    {
	sfGetValue( dhsObject, FALSE, "axisSize", &type, &ndims, dims,
		(void**)&value, sfStatus);
	if ( *sfStatus < SF_S_SUCCESS  )
	{
	    *sfStatus = sfFnStatus;
	    *naxis = 0;
	    blc = NULL;
	    fnReturn( VOID );
	}
	else
	{
	    if ( *naxis == 0 && dims[0] != 0 )
	    {
		*naxis = dims[0];
	    }
	    else if (  dims[0] != *naxis || ndims != 1 )
	    {
		*naxis = 0;
		blc = NULL;
		*sfStatus = SF_E_AXISSZ_DIMS;
		sfFormatMessage( *sfStatus, ndims, dims[0] );
		fnReturn( VOID );
	    }

	    memcpy( axissz, value, sizeof( long ) *  dims[0] );
	    for( i = dims[0]; i < SF_MAXDIM; i++ )
	    {
		axissz[i] = 0;
	    }
	}
    }


    /*
     * Verify correctness of the "origin"
     */
    
    for ( i = 0; i < dims[0] && blc != NULL; i++ )
    {
	if ( blc[i] < 1 || blc[i] > axissz[i]  )
	{
	    *naxis = 0;
	    if ( dims[0] == 2 )
	    {
		*sfStatus = SF_E_ORIGIN_2D;
		sfFormatMessage( *sfStatus, axissz[0], axissz[1], blc[0],
			blc[1] );
		blc = NULL;
		fnReturn( VOID );
	    }
	    else if ( dims[0] == 3 )
	    {
		*sfStatus = SF_E_ORIGIN_3D;
		sfFormatMessage( *sfStatus, axissz[0], axissz[1], axissz[2],
			blc[0], blc[1], blc[2] );
		blc = NULL;
		fnReturn( VOID );
	    }
	    else 
	    {
		*sfStatus = SF_E_ORIGIN;
		sfFormatMessage( *sfStatus, i+1, axissz[i] );
		blc = NULL;
		fnReturn( VOID );
	    }
	}
    }


    sfGetValue( dhsObject, FALSE, "dataType", &type, &ndims, dims,
	    (void**)&value, sfStatus );
    if ( *sfStatus < SF_S_SUCCESS )
    {
	strnCpy( extName, "", FLEN_VALUE, *sfStatus, VOID );
	*sfStatus = sfFnStatus;
    }
    else if ( ndims != 0 ) {
	*sfStatus = SF_E_ATTRIB_DIMS;
	sfFormatMessage( *sfStatus, ndims, "dataType" );
    }
    else
    {	
	strnCpy( extName, (char*)value, FLEN_VALUE, *sfStatus, VOID );
    }

   fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfGetImgKeyword
 *
 * INVOCATION:
 * sfGetImgKeywords( dataset, instrument, bitpix, naxis, naxes, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	dhsObject   (DHS_BD_OBJECT)	DHS dataset or frame.
 * (>)	instrument  (char[FLEN_VALUE])	Name of the attribute.
 * (<)	bitpix	    (int*)		Number of bits per pixel.
 * (<)	naxis	    (int*)		Number of dimensions.
 * (<)	naxes	    (long[SF_MAXDIM])	FITS primary attribute.
 * (!)	sfStatus    (SF_STATUS)		SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To determine the bitpix, naxis and naxes for an image extension
 * or the primary HDU.
 *
 * DESCRIPTION:
 * 1. Initaliaze variables to default value.
 * 2. Find the frame information, if there is any.
 * 3. Set bitpix, naxis, naxes if the frame information was found.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfWriteImgKeywords.
 *
 * DEFICIENCIES:
 * The dimensions, size, of each axis is stored as an unsigned long
 * integer in the SDS structure but is converted to a long integer
 * for "cfitsio".  This could cause problems with large sets of data.
 *-
 ************************************************************************
 */

void sfGetImgKeywords
( 
    DHS_BD_OBJECT   dhsObject,	/* (in)	 DHS dataset or frame.		*/
    char	    instrument[FLEN_VALUE],
				/* (in)	 Name of the attribute.		*/
    int		    *bitpix,	/* (out) Number of bits per pixel.	*/
    int		    *naxis,	/* (out) Number of dimensions.		*/
    long	    naxes[SF_MAXDIM],
				/* (out) FITS primary attribute.	*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    void	    *attribData;/* Attribute values (may be an array).	*/
    char	    *attribName;/* Name of the attribute.	    	*/
    DHS_STATUS	    dhsStatus;	/* From dhs routines.	    		*/
    unsigned long   dims[SF_MAXDIM];
				/* Array of dims of attribute data.    	*/
    int		    i;
    int		    ndims;	/* Number of dimensions.	    	*/
    DHS_DATA_TYPE   type;	/* Type of dhs attrib data object.	*/


    fnEntry ( "sfGetImgKeywords", *sfStatus );
    statCheck( *sfStatus,  VOID );
    
	
    /*
     * Initialize variables.
     */
	
    dhsStatus = DHS_S_SUCCESS;
    *bitpix = SHORT_IMG;
    *naxis = 0;
    naxes[0]=naxes[1]=naxes[2]=naxes[3]=naxes[4]=naxes[5]=naxes[6]=0;
    

    /*
     * Find the frame information, if there is any.
     */

    if( !sfIsDataset( dhsObject ) )
    {
	dhsBdFrameInfo( dhsObject, &attribName, &type, &ndims,
		dims, (void**)&attribData, &dhsStatus );
	    
    }
    else
    {
	dhsBdAttribInfo( dhsObject, &attribName, &type, &ndims,
		dims, (void**)&attribData, &dhsStatus );
    }
    
	
    /*
     * A frame with data was found so set the values appropriately.
     */
	
    if ( dhsStatus != DHS_S_NO_ATTRIB  && ndims != 0 )
    {
	*naxis = ndims;

		
	/*
	 * Convert from the DHS_DATA_TYPE to a valid bitpix value.
	 */

	dhs2bitpix( type, *bitpix, *sfStatus, VOID );
	if ( *sfStatus < SF_S_SUCCESS )
	{
	    sfFormatMessage( *sfStatus, type );
	    fnReturn( VOID );
	}
	
		
	/*
	 * Convert naxes to long integers from unsigned long.
	 */
	    
	for( i = 0; i < (*naxis); i++ )
	{
	    naxes[i] = (long)dims[i];
	}
    }

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfIsData
 *
 * INVOCATION:
 * sfIsData( attributeName );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) 	attribName   (char*)	Name of the attribute 
 *
 * FUNCTION VALUE:
 * boolean : TRUE,  if the attribute name is "_dataArray"
 * boolean : FALSE, if the attribute name is NOT "_dataArray"
 *
 * PURPOSE:
 * To determine if the attribute name corresponds to data.
 *
 * DESCRIPTION:
 * if the attribute name contains the string "_dataArray"
 * then return TRUE, otherwise return FALSE.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * It relies on dhs to use "_dataArray" to indicate if it is data frame.  A
 * cleaner way of doing this would be prefered.
 *
 *-
 ************************************************************************
 */

boolean sfIsData
(
    char      *attribName   /*(in) name of the attribute.      		*/
)
{
    if ( strstr( attribName, DATA ) != NULL )
    {
	return( TRUE );
    }
    else
    {
	return( FALSE );
    }
}



/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfIsDataset
 *
 * INVOCATION:
 * booleanValue = sfIsDataset( sdsObject );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) dhsObject    (DHS_BD_OBJECT)	A DHS dataset, frame or attribute.
 *
 * FUNCTION VALUE:
 * (boolean) TRUE  : if dhsObject has attribute "_name" 
 * (boolean) FALSE " if dhsObject doesnt have attribute "_name"
 *
 * PURPOSE:
 * Determine if a DHS object (frame or dataset) is a frame or
 * a dataset.
 *
 * DESCRIPTION:
 * If the attribute "_name" is one of the objects attributes then
 * it is a dataset so return TRUE, otherwise return FALSE.
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
 * Relies on the existence or absense of "_name" as an attribute.  All
 * frames are given this attribute, but datasets are not.  However,
 * there is nothing preventing someone from adding "_name" to a dataset.
 *-
 ************************************************************************
 */

boolean	sfIsDataset
(
    DHS_BD_OBJECT   dhsObject	 /* (in)  Dhs object to be checked	*/
)
{
    DHS_STATUS	dhsStatus;	/* Status of DHS commands.		*/  
    SF_STATUS	sfStatus = SF_S_SUCCESS;
    
    fnEntry( "sfIsDataset", sfStatus );
    
    dhsStatus = DHS_S_SUCCESS;

    dhsBdAttribFind( dhsObject, NAME, &dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	fnReturn( FALSE );
    }
    else
    {
	fnReturn( TRUE );
    }
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfIsFrame
 *
 * INVOCATION:
 * booleanValue = sfIsFrame( attributeName );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) attribName   (char*)	Name of the attribute.
 *
 * FUNCTION VALUE:
 * boolean : TRUE,  if the attribute name is "_frame"
 * boolean : FALSE, if the attribute name is NOT "_frame"
 *
 * PURPOSE:
 * To determine if the attribute name corresponds to frame.
 *
 * DESCRIPTION:
 * if the attribute name contains the string "_frame"
 * then return TRUE, otherwise return FALSE.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * It relies on dhs to use "_frame" to indicate if it is data frame.  A
 * cleaner way of doing this would be prefered.
 *
 *-
 ************************************************************************
 */

boolean sfIsFrame
(
    char    *attribName   /*(in) name of the attribute.			*/
)
{
    if ( strncmp( attribName, FRAME, 6 ) == 0 )
    {
	return( TRUE );
    }
    else
    {
	return( FALSE );
    }
}


/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfNewBinHdu
 *
 * INVOCATION:
 * sfNewBinHdu( fitsfilePointer, arrayInformation, instrument, oneChunk,
 *		datasetInformation, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		The fits file pointer.
 * (>)	arrayInfo   (SF_ARRAY_INFO*) 	Information about the array.
 * (>) 	instrument  (char[FLEN_VALUE])	Instrument.
 * (>)	oneChunk    (boolean)		Processing an observation chunk
 *		      			for Quick Look or not.
 * (!)	dsInfo	    (SF_DS_INFO*)	Dataset, observation,  information.
 * (!)	sfStatus    (SF_STATUS*)	Sf library status indicator.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To add a binary new header unit to a FITS file.
 *
 * DESCRIPTION:
 * 1. Move to the bottom of the file.
 * 2. Find the required keywords to write.
 * 3. Write the requireed keywords and the data.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS. 
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * Does not create an extension for an array of strings.
 *-
 ************************************************************************
 */

void	sfNewBinHdu
(
    fitsfile	    *fptr,	/* (in)	 The fits file pointer.		*/
    SF_ARRAY_INFO   *arrayInfo,	/* (in)  Information about the array.	*/
    char	    instrument[FLEN_VALUE],
				/* (in)	 Instrument.			*/
    boolean	    oneChunk,	/* (in)  Indicates if processing an	*/
				/*       observation chunk.		*/
    SF_DS_INFO	    *dsInfo,	/* (mod) dataset information.		*/
    SF_STATUS	    *sfStatus	/* (mod) sf library status indicator.	*/
)
{
    DD_ARRAY_TYPE   array;	/* Attribute array type.		*/
    void	    *attribData;/* Attribute values (may be an array).	*/
    char       	    *attribName;/* Name of the attribute		*/
    int		    bitpix;	/* Number of bits per pixel.		*/
    long	    blc[SF_MAXDIM];
    				/* The data's origin or coordinates	*/
				/* for the bottom, left corner.		*/
    char	    comm[FLEN_COMMENT];
			        /* Comment for FITS file. 		*/
    DHS_STATUS	    dhsStatus;	/* From dhs routines.	    		*/
    unsigned long   dims[SF_MAXDIM];
    				/* array of dims of attrib data.	*/
    FITS_STATUS	    fitsStatus;	/* From cfitsio routines.		*/
    int		    hdutype;	/* Type of the current hdu.		*/
    int		    i;
    char	    keyword[FLEN_KEYWORD];
			     	/* Keyword for the FITS file. 		*/
    int	    	    naxis;	/* Number of dimension of data.		*/
    long	    naxes[SF_MAXDIM];
    int		    maxHdu;
        			/* Size of each data dimension.		*/
    int		    ndims;	/* Number of dimensions.		*/
    DHS_DATA_TYPE   type;	/* Type of attrib data object.		*/
    boolean	    writeIt;	/* TRUE if the keyword is written to the
				   FITS file.				*/
    
    fnEntry( "sfNewBinHdu", *sfStatus );
    statCheck( *sfStatus, VOID );

    /*
     * Initialize variables.
     */

    dhsStatus = DHS_S_SUCCESS;
    fitsStatus = FITS_S_SUCCESS;


    /*
     * Move to the bottom of the file.
     */

    fitsCheck( ffthdu( fptr, &maxHdu, &fitsStatus ), fitsStatus,
	    *sfStatus, VOID);
    fitsCheck( ffmahd( fptr, maxHdu, &hdutype, &fitsStatus ),
	    fitsStatus, *sfStatus, VOID );


    /*
     * Find the type, dimensions, and size of the data.
     */

    dhsCheck( dhsBdAttribInfo ( arrayInfo->sdsId, &attribName, &type, &ndims,
	    dims, &attribData, &dhsStatus ), dhsStatus, *sfStatus,
	    VOID );

    if ( *sfStatus == SF_E_CONV_TYPE )
    {
	/*
	 * The only type that requires a binaray extension
	 * is Arrays of strings and this is currently no
	 * implemented.
	 */

	check( sfWriteBinKeywords( fptr, instrument, type, arrayInfo, sfStatus ),
		*sfStatus, VOID );
	check( sfWriteBinData( fptr, ndims, dims, type, attribData, sfStatus),
		*sfStatus, VOID );
    }
    else
    {
	check( sfWriteBinKeywords( fptr, instrument, type, arrayInfo, sfStatus ),
		*sfStatus, VOID );
	check( sfWriteBinData( fptr, ndims, dims, type, attribData, sfStatus),
		*sfStatus, VOID );
    }


    /*
     * If we are not processing a chunk we need to increment
     * the last index into the FITS file.
     */
    
    if( !oneChunk )
    {
	dsInfo->last++;
    }
    
    fnReturn( VOID );
}



/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfNewImgHdu
 *
 * INVOCATION:
 * sfNewImgHdu( fitsfilePointer, datasetObject, instrument, oneChunk,
 *	isPrimeHdu, datasetInformation, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		The fits file pointer.
 * (>)	dhsObject   (DHS_BD_OBJECT) 	Dataset or frame object.
 * (>) 	instrument  (char[FLEN_VALUE])	Instrument.
 * (>)	oneChunk    (boolean)		Processing an observation chunk
 *		      			for Quick Look or not.
 * (>)	isPrimeHdu  (boolean)	    	Creating primary HDU?
 * (!)	dsInfo	    (SF_DS_INFO*)	Dataset, observation,  information.
 * (!)	extAddr	    (SF_EXT_INFO*)	Information about this extension.
 * (!)	sfStatus    (SF_STATUS*)	Sf library status indicator.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To add a new header unit to a FITS file and to reserve space for
 * the keywords.
 *
 * DESCRIPTION:
 * 1. Initialize variables
 * 2. Determine the type of header unit to create.
 * 3. Create the header unit at the bottom of the file.
 * 4. Reserve space for the keywords.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS. 
 *
 * The "ext.type" has been assigned a value or (exclusive or) 
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean	sfNewImgHdu
(
    fitsfile	    *fptr,	/* (in)	 The fits file pointer.		*/
    DHS_BD_OBJECT   dhsObject,	/* (in)	 dataset or frame object.	*/
    char	    instrument[FLEN_VALUE],
				/* (in)	 Instrument.			*/
    boolean	    oneChunk,	/* (in)  Indicates if processing an	*/
				/*       observation chunk.		*/
    boolean	    isPrimeHdu,	/* (in)	 Creating primary HDU?		*/
    SF_DS_INFO	    *dsInfo,	/* (mod) dataset information.		*/
    SF_EXT_INFO     *ext,       /* (mod) Extension information.		*/
    SF_STATUS	    *sfStatus	/* (mod) sf library status indicator.	*/
)
{
    long	axissz[SF_MAXDIM];  /* Axis sizes of the full image.	*/
    int		bitpix;		    /* Number of bits per pixel.	*/
    long	blc[SF_MAXDIM];	    /* The data's origin or coordinates	*/
				    /* for the bottom, left corner.	*/
    char	extName[FLEN_VALUE];
    FITS_STATUS	fitsStatus;	    /* Cfitesio status indicator.	*/
    char	frmName[FLEN_VALUE];
    boolean	hasData;	    /* Indicates if the frame has data. */
    int		hdutype;	    /* Type of the current hdu.		*/
    int		maxHdu;
    int    	naxis;		    /* Number of dimension of data.	*/
    long	naxes[SF_MAXDIM];   /* Size of each data dimension.	*/
    int		tmpNaxis;	    /* Temporary variable for n axis.	*/
    

    fnEntry( "sfNewImgHdu", *sfStatus );
    statCheck( *sfStatus, FALSE );

    
    /*
     * Initialize variables.
     */

    hasData = FALSE;
    fitsStatus = FITS_S_SUCCESS;
    if ( !oneChunk )
    {
	dsInfo->last++;
	if( ext != NULL )
	{
	    ext->fitsIndex = dsInfo->last;
	}
    }
    

    /*
     * Move to the last HDU in the fits file;
     */

    fitsCheck( ffthdu( fptr, &maxHdu, &fitsStatus ), fitsStatus,
	    *sfStatus, FALSE );
    
    /* only move forward if possible */
    if (maxHdu) {
      fitsCheck( ffmahd( fptr, maxHdu, &hdutype, &fitsStatus ),
	      fitsStatus, *sfStatus, FALSE );
    }
	
    /*
     * Find the FITS required Keywords and create the header unit.
     */

    if ( isPrimeHdu )
    {
	naxis = 0;
	bitpix = 16;
    }
    else
    {
	check( sfGetImgKeywords( dhsObject, instrument, &bitpix, &naxis,
		naxes, sfStatus ), *sfStatus, FALSE );
	if ( naxis > 0 )
	{
	    hasData = TRUE;
	}
    }
    

    if ( isPrimeHdu )
    {
	check( sfWriteImgKeywords( fptr, bitpix, naxis, naxes, isPrimeHdu,
		sfStatus), *sfStatus, FALSE );
    }
    else if ( oneChunk )
    {
	tmpNaxis = naxis;
	check( sfGetDhsKeywords( dhsObject, instrument,	&tmpNaxis, blc,
		axissz,	frmName, extName, sfStatus ), *sfStatus, FALSE );
	if ( naxis != tmpNaxis  && hasData )
	{
	    *sfStatus = SF_E_DHS_KEYS;
	    sfFormatMessage( *sfStatus, frmName );
	}
			
	check( sfUpdateImgKeywords( bitpix, naxis, axissz, ext, sfStatus ),
		*sfStatus, FALSE );
	check( sfWriteImgKeywords( fptr, bitpix, naxis, naxes, isPrimeHdu,
		sfStatus ), *sfStatus, FALSE );
    }
    else
    {
	bitpix = ext->bitpix;
	naxis = ext->naxis;
	nullCheck( memcpy( naxes, ext->naxes, sizeof ( long ) * SF_MAXDIM ),
		    *sfStatus, FALSE );
	check( sfWriteImgKeywords( fptr, bitpix, naxis, naxes,
		isPrimeHdu, sfStatus), *sfStatus, FALSE );
    }
   
	    
    /*
     * Reserve space for the keywords.
     */
    
    statCheck( *sfStatus,  FALSE );
    if ( isPrimeHdu && !oneChunk )
    {
	fitsCheck( ffhdef( fptr, dsInfo->numKeywords + 1, &fitsStatus ),
		fitsStatus, *sfStatus, FALSE );
    }
    else if ( !isPrimeHdu && !oneChunk )
    {
	fitsCheck( ffhdef( fptr, ext->numKeywords, &fitsStatus ),
		fitsStatus, *sfStatus, FALSE );
    }

    fnReturn( hasData );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfSds2Fits
 *
 * INVOCATION:
 * sfSds2Fits (sdsObject, fitsfilePointer, datasetInfo, oneChunkFlag,
 *	 sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) 	sdsObject   (DHS_BD_OBJECT)	SDS object to be converted to FITS.
 * (>)	fptr	    (fitsfile*)		Pointer to a FITS file on disk
 *					or a FITS file in memory.
 * (!)	dsInfo	    (SF_DS_INFO*)	Description of SDS object
 * (>)	oneChunk    (boolean)	   	TRUE if one and only one SDS
 *					(observation) chunk will be
 *					written to the FITS file.  False
 *					if a complete observation
 *					(several chunks) are to be
 *					 written to the FITS file.
 * (!)	sfStatus	(SF_STATUS*)    SF library status indicator.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Function that converts a fits from an SDS structure to a FITS
 * file.
 *
 * DESCRIPTION:
 * 1. Make sure the fits file pointer is not NULL.
 * 2. Make sure the data set information is not NULL.
 * 3. Make sure the DHS object is a dataset.
 * 4. Find the instrument attribute and its value.
 * 5. Process the conversion from SDS to FITS.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * "dsInfo" has been intialized.
 *
 * SEE ALSO:
 * -sfDoChunkPrimeHdu
 * -sfDoPrimeHdu
 *
 * DEFICIENCIES:
 * None. *-
 ************************************************************************
 */

void sfSds2Fits
( 
    DHS_BD_OBJECT   sdsObject,	/* (in)  The SDS to be converted to FITS.*/
    fitsfile	    *fptr,	/* (in)  Fits file pointer (disk or
					  memory.			*/
    SF_DS_INFO	    *dsInfo,	/* (mod) Inforamtion describing the SDS
					 object.			*/
    boolean	    oneChunk,	/* (in)  True if one SDS (observation)
				   chunk will be writen to the FITS
				   file.  False otherwise.		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status indicator.	*/
)    
{
    char	    instrument[FLEN_VALUE];
				/* Name of the attribute.		*/
    void	    *value;	/* Attribute values.			*/

    
    fnEntry ( "sfSds2Fits", *sfStatus );
    statCheck( *sfStatus,  VOID );


    /*
     * Make sure we have a fits file pointer.
     */
	
    if ( fptr == NULL )
    {
	*sfStatus = SF_E_FPTR;
	sfFormatMessage( *sfStatus );
	fnReturn( VOID );
    }


    /*
     * Make sure we have the dataset information.
     */
	
    if ( dsInfo == NULL )
    {
	*sfStatus = SF_E_DS_INFO;
	sfFormatMessage( *sfStatus );
	fnReturn( VOID );
    }


    /*
     * Make sure we have a DHS dataset.
     */
	
    if ( !sfIsDataset( sdsObject ) )
    {
	*sfStatus = SF_E_NOT_DATASET;
	sfFormatMessage( *sfStatus );
	fnReturn( VOID );
    }


    /*
     * Find the instrument attribute, if there is one get its value.
     * If there isn't use "*" for the instrument and set a warning
     * message.
     */
	
    sfGetValue( sdsObject, FALSE, "instrument", NULL, NULL, NULL, &value,
	    sfStatus );
    if ( *sfStatus < SF_S_SUCCESS )
    {
	strnCpy( instrument, "*", FLEN_VALUE, *sfStatus, VOID );
	*sfStatus = SF_S_INST_NOT_FOUND;
	sfFormatMessage( *sfStatus );
    }
    else
    {
    	strnCpy( instrument, (char*)value, FLEN_VALUE, *sfStatus, VOID );
    }

    /*
     * Process the conversion from SDS to FITS differently depending on
     * if the FITS file will contain an observations "chunk" or all of
     * the observation information.
     */

    if ( oneChunk )
    {
	check( sfDoChunkPrimeHdu( fptr, sdsObject, dsInfo, instrument,
		sfStatus ), *sfStatus, VOID );
     }	
    else
    {	
	check( sfDoPrimeHdu( fptr, sdsObject, dsInfo, instrument,
		sfStatus ), *sfStatus, VOID );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfUpdateDhsKeywords
 *
 * INVOCATION:
 * check( sfUpdateDhsKeywords( dsInfo, frameInfo, ext, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) dsInfo	(SF_DS_INFO*)		Informaiton about the dataset.
 * (>) framInfo	(SF_FRAME_INFO*)	Information about the frame.
 * (>) ext	(SF_EXT_INFO*)		Information on the FITS extension.
 * (!) sfStatus	(SF_STATUS*)		Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To check and/or update the FITS extension information with the
 * current Frame information.
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

void	sfUpdateDhsKeywords
(
    SF_DS_INFO		*dsInfo,	/* (in)  Dataset information.	*/
    SF_FRAME_INFO	*frameInfo,	/* (in)  Frame information.	*/
    SF_EXT_INFO		*ext,		/* (in)  FITS extension info.	*/
    SF_STATUS		*sfStatus	/* (mod) Function return value.	*/
)
{
    fnEntry( "sfUpdateDhsKeywords", *sfStatus );
    statCheck( *sfStatus, VOID );

    if ( ext != NULL && strne( frameInfo->frmName, ""  ) &&
	strne( ext->frmName, "" ) )
    {
	if ( strne( ext->frmName, frameInfo->frmName ) )
	{
	    *sfStatus = SF_S_FRMNAME;
	    sfFormatMessage( *sfStatus, ext->frmName, frameInfo->frmName,
		    ext->frmId );
	}
	ext->numKeywords += 2;
    }


    if ( ext != NULL && strne( frameInfo->extName, "" )  &&
	    strne( ext->extName, "" ) )
    {
	if ( strne( ext->extName, frameInfo->extName ) )
	{
	    *sfStatus = SF_S_EXTNAME;
	    sfFormatMessage( *sfStatus );
	}
    }
    else if ( ext != NULL && strne( frameInfo->extName, "" ) )
    {
	strnCpy( ext->extName, frameInfo->extName, FLEN_VALUE, *sfStatus,
		VOID );
	ext->numKeywords ++;
    }
        

    if ( ext != NULL && frameInfo->extVer != SF_NULL_EXTVER &&
	    ext->extVer != SF_NULL_EXTVER )
    {
	if ( ext->extVer !=  frameInfo->extVer )
	{
	    *sfStatus = SF_S_EXTVER;
	    sfFormatMessage( *sfStatus );
	}
    }
    else if (  ext != NULL && ext->extVer == SF_NULL_EXTVER &&
	    strne( ext->extName, "" ) )
    {
	check( sfAddExtVer( dsInfo, ext, sfStatus ), *sfStatus, VOID );
	frameInfo->extVer = ext->extVer;
    }
    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfUpdatImgKeywords
 *
 * INVOCATION:
 * sfUpdateImgKeywords( bitpix, naxis, naxes, ext, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (<)	bitpix	    (int*)		Number of bits per pixel.
 * (<)	naxis	    (int*)		Number of dimensions.
 * (<)	naxes	    (long[SF_MAXDIM])	FITS primary attribute.
 * (!)	ext	    (SF_EXT_INFO*)	Information about this extension.
 * (!)	sfStatus    (SF_STATUS*)	Sf library status indicator.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To make sure the bitpix, naxis and naxes value for the extension
 * are correct.
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

void	sfUpdateImgKeywords
(
    int		    bitpix,	/* (in) Number of bits per pixel.	*/
    int		    naxis,	/* (in) Number of dimensions.		*/
    long	    naxes[SF_MAXDIM],
    SF_EXT_INFO	    *ext,	/* (mod) the extension information.     */
				/* (in) FITS primary attribute.		*/
    SF_STATUS	    *sfStatus	/* (mod) sf library status indicator.	*/
)
{

    int		i;
    
    
    fnEntry( "sfUpdateImgKeywords", *sfStatus );
    statCheck( *sfStatus, VOID );


    if ( ext == NULL )
    {
	*sfStatus = SF_E_NULL_EXT;
	sfFormatMessage( *sfStatus);
    }


    /*
     * If there is no data then values do not need updating.
     */
    
    if ( naxis == 0 )
    {
	fnReturn( VOID );
    }
    
	
    /*
     * Verify bitpix
     */
	
    if ( ext->bitpix != 16 &&
	    ext->bitpix != bitpix ) {
	/*
	 * ERROR
	 */

	*sfStatus = SF_E_BITPIX_VAL;
	sfFormatMessage( *sfStatus, ext->bitpix, bitpix, ext->frmName );
	
    }	
    else
    {
	ext->bitpix = bitpix;
    }


    /*
     * Verify naxis
     */
	
    if ( ext->naxis != 0 && ext->naxis != naxis ) {
	/*
	 * ERROR
	 */

	*sfStatus = SF_E_NAXIS;
	sfFormatMessage( *sfStatus, ext->naxis, naxis, ext->frmName );
    }	
    else
    {
	ext->naxis = naxis;
    }

    
    /*
     * Verify naxes
     */

    for( i = 0; i < naxis; i++ )
    {
	if ( ext->naxes[i] != naxes[i] && ext->naxes[i] != 0 )
	{
	    /*
	     * ERROR
	     */

	    *sfStatus = SF_E_NAXES;
	    sfFormatMessage( *sfStatus, ext->naxes[i], naxes[i], i-1,
		    ext->frmName );
	}	
	else
	{
	    ext->naxes[i] = naxes[i];
	}
    }

    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteBadIndexedArray
 *
 * INVOCATION:
 * sfWriteBadIndexedArray
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	(fitsfile*)		    FITS file pointer.
 * (>)	keyword	(char[FLEN_KEYWORD]	    FITS keyword to be written.
 * (>)	type	(DHS_DATA_TYPE)		    The keyword's DHS data type.
 * (>)	ndims	(int) 			    Number of dimension the data is.
 * (>)	dims	(unsigned long[SF_MAXDIM])  The size of each axis.
 * (>)	comm	(comm[FLEN_COMMENT])	    Comment for the FITS file. 
 * (>)	data	(void*)			    The keyword's value.	
 * (!) 	sfStatus(SF_STATUS*)		    SF library status.
 * 
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Write the contents of an indexed array as a number of comments
 * in the FITS file.
 *
 * This is done when some problem, but not a serious problem, has
 * been found with the data.
 *
 * DESCRIPTION:
 * For each item in the indexed array write it as a scalar value
 * with the index value added to the end of the keyword.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
void	sfWriteBadIndexedArray
(
    fitsfile	    *fptr,	/* (in)  FITS file pointer.		*/
    char	    *attribute, /* (in)  SDS attribute name.	*/
    DHS_DATA_TYPE   type,	/* (in)  The keyword's DHS data type.	*/
    int		    ndims,	/* (in)  Number of dimension.		*/
    unsigned long   dims[SF_MAXDIM],
				/* (in)  The size of each axis. 	*/
    char	    comm[FLEN_COMMENT],
				/* (in)  Comment for the FITS file. 	*/
    void	    *data,	/* (in)  Value to be written.		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    int		    i;
    void	    *indexedData;    	/* Data for one indexed keyword.*/
    char	    *indexedAttribute; 	/* attribute plus index number.	*/
    char	    indexedKeyword[FLEN_KEYWORD];
    					/* indexec Keyword truncated.	*/

    fnEntry( "sfWriteBadIndexedArray", *sfStatus );

    nullCheck( indexedAttribute = char_alloc( strlen( attribute ) +
	    log10( dims[0] ) + 2 ), *sfStatus, VOID );
    
    
    for( i=1; i <= dims[0]; i++ )
    {
	(void) sprintf( indexedAttribute, "%s%u", attribute, i );
    
	switch ( type )
	{
	    case DHS_DT_CHAR:
		nullCheck( indexedData = (void*)char_alloc( 2 ),
			*sfStatus, VOID );
		((char*)indexedData)[0] = ((char*)data)[i-1];
		((char*)indexedData)[1] = '\0';
		break;
		
	    case DHS_DT_STRING:
		indexedData = ((char**)data)[i-1];
		break;

	    case DHS_DT_BOOLEAN:
	    case DHS_DT_INT8:
		indexedData = &(((char*)data)[i-1]);
		break;

	    case DHS_DT_UINT8:
		indexedData = &(((unsigned char*)data)[i-1]);
		break;

	    case DHS_DT_INT16:
		indexedData = &(((short*)data)[i-1]);
		break;

	    case DHS_DT_UINT16:
		indexedData = &(((unsigned short*)data)[i-1]);
		break;

	    case DHS_DT_INT32:
		indexedData = &(((int*)data)[i-1]);
		break;

	    case DHS_DT_UINT32:
		indexedData = &(((unsigned int*)data)[i-1]);
		break;

	    case DHS_DT_FLOAT:
		indexedData = &(((float*)data)[i-1]);
		break;

	    case DHS_DT_DOUBLE:
		indexedData = &(((double*)data)[i-1]);
		break;
		
	    case DHS_DT_AVLIST:
	    case DHS_DT_TAG:			
	    default:
		*sfStatus = SF_E_DHS_TYPE;
		sfFormatMessage( *sfStatus, type );
		fnReturn( VOID );
	}

	(void) sprintf( indexedKeyword, "%-.*s", FLEN_KEYWORD - 1,
		indexedAttribute );
   // fixme -- mdcb 24Mar08
   // the order of attribute and keyword is swapped
   // this is an old bug that effectively defeat the sds/fits translation from libdd.config
   // fixing it now is bound to break more things (GSA etc.) so leave it for now
	check( sfWriteScalarVal( fptr, indexedKeyword, indexedAttribute,
		type, comm, indexedData, sfStatus ), *sfStatus, VOID );
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteBinExtension
 *
 * INVOCATION:
 * sfWriteBinExtension( )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) paramName (int) Descripton of the parameter
 * (!) paramName (float) Descripton of the parameter
 * (<) paramName (long) Descripton of the parameter
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Statement of purpose  
 * First line should be a summary not terminated with a period.
 *
 * DESCRIPTION:
 * Description of the algorithm.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfWriteBinExtension
(
    fitsfile	    *fptr,	/* (in)  FITS file pointer.		*/
    EPTR	    **arrayList,/* (in)  List of array elements.	*/
    char	    frameId[FLEN_VALUE],
    				/* (in)  Frame identifier.		*/
    DHS_BD_ATTRIB_ID
    		    attribId,   /* (in)  attribute identifier.		*/
    char	    *attribute,	/* (in)  SDS attribute name.		*/
    char	    keyword[FLEN_KEYWORD],
				/* (in)  FITS keyword. 			*/
    DHS_DATA_TYPE   type,	/* (in)  The keyword's type.		*/
    char	    comm[FLEN_COMMENT],
				/* (in)  The keyword's comment.		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    char	    extName[SF_EXTNAME_SIZE];
    				/* Extension name for this bin. ext.	*/
    FITS_STATUS	    fitsStatus;	/* From cfitsio routines.		*/
    SF_ARRAY_INFO   *newArrayInfo;
    				/* Multi-dim. array information.	*/
    char	    *newCard;	/* Keyword + keyword's value string.	*/
    char	    *tmpCard;   /* Temporary card.			*/

    
    fnEntry( "sfWriteBinExtension", *sfStatus );

    fitsStatus = FITS_S_SUCCESS;
    if ( *sfStatus != SF_S_SUCCESS )
    {
	/*
	 * Didn't pass data dictionary checks.
	 */

	nullCheck( newCard = strnsav( SF_BAD_COMM "    ", FLEN_CARD ),
		*sfStatus, VOID );
	nullCheck( tmpCard = sfCreateCard( attribute, type, NULL, comm,
		sfStatus ), *sfStatus, VOID );
	strnCat( newCard, tmpCard, FLEN_CARD, *sfStatus, VOID );
	fitsCheck( ffpcom( fptr, newCard, &fitsStatus ), fitsStatus,
		*sfStatus, VOID );
	if( newCard != NULL )
	{
	    gen_free( newCard );
	    newCard = NULL;
	}
	if( tmpCard != NULL )
	{
	    gen_free( tmpCard );
	    tmpCard = NULL;
	}	
	fnReturn( VOID );
    }
    statCheck( *sfStatus, VOID );


#if DEBUG
    printf ( "name = %s, type = %d, comm = %s\n", keyword, type, comm);
#endif

#ifdef OMIT
    /*
     * Create the multi-dimension array object.  The extension
     * name of the array will be "attributeName:frameId"
     */
    
    nullCheck( newArrayInfo = malloc( sizeof( SF_ARRAY_INFO ) ),
	    *sfStatus, NULL );
    newArrayInfo->sdsId = attribId;

    strnCpy( newArrayInfo->extName, keyword, NAME_SIZE+9, *sfStatus, NULL );
    strnCat( newArrayInfo->extName, ":",  NAME_SIZE+9, *sfStatus, NULL );
    strnCat( newArrayInfo->extName, frameId, NAME_SIZE+9, *sfStatus, NULL );
#endif /* OMIT */
    /*
     * Add the new binary extension to our list.
     */
    
    strnCpy( extName, keyword, NAME_SIZE+9, *sfStatus, VOID );
    strnCat( extName, ":",  NAME_SIZE+9, *sfStatus, VOID );
    strnCat( extName, frameId, NAME_SIZE+9, *sfStatus, VOID );
    check( sfArrayAdd( arrayList, attribId, extName, sfStatus ), *sfStatus,
	    VOID );
    
    
    /*
     * Write the keyword to the current HDU with
     * value "keyword:frameId"
     */
    
    fitsStatus = 0;
    fitsCheck( ffpkys( fptr, keyword, extName, comm, &fitsStatus ),
	    fitsStatus, *sfStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteBinData
 *
 * INVOCATION:
 * sfWriteBinData( fptr, arrayInformation, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		The fits file pointer.		
 * (>)	ndims	    (int)		Number of dimensions, in data.
 * (>)  dims	    (long[SF_MAX_DIM])	Size of each dimension.
 * (>)	type	    (DHS_DATA_TYPE)	Type of data.
 * (>)	data	    (void*)		The data.
 * (!)	sfStatus    (SF_STATUS*)	SF library status indicator.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Write the data of a binary extension.
 *
 * DESCRIPTION:
 * Trivial
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * sfWriteBinKeywords
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfWriteBinData
(
    fitsfile	    *fptr,	/* (in)	 The fits file pointer.		*/
    int		    ndims,	/* (in)  Number of dimensions.		*/
    unsigned long   dims[SF_MAXDIM],
    				/* (in)  Array of dims of attrib data.	*/
    DHS_DATA_TYPE   type,	/* (in)  Type of attrib data object.	*/
    void	    *data,	/* (in)  The data to be written.	*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status indicator.	*/
)
{
    FITS_STATUS	    fitsStatus;	/* From cfitsio routines.		*/
    int		    fitsType;	/* FITS data type.			*/
    int		    i;
    long	    size;	/* Number of elements in array.		*/

    
    fnEntry( "sfWriteBinData", *sfStatus );
    statCheck( *sfStatus, VOID );

    fitsStatus = FITS_S_SUCCESS;
    
    dhs2fits( type, fitsType, *status, VOID );

    if( ndims < 0 )
    {
	 /* Error */
	fnReturn( VOID );
    }

    size = 1;
    for( i = 0; i < ndims; i++ )
    {
	size *= dims[i];
    }

    switch( fitsType )
    {
	case TLOGICAL:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( char *)data, &fitsStatus ), fitsStatus,
		    *sfStatus, VOID );
	    break;
	case TBYTE:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( unsigned char *)data, &fitsStatus ), fitsStatus,
		    *sfStatus, VOID );
	    break;
	case TSTRING:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( char *)data, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
	case TSHORT:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( short *) data, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
	case TUSHORT:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( unsigned short *) data, &fitsStatus ), fitsStatus,
		    *sfStatus, VOID );
	    break;
	case TINT:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( int *) data, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
	case TLONG:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( long *) data, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
	case TULONG:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( unsigned long *) data, &fitsStatus ), fitsStatus,
		    *sfStatus, VOID );
	    break;
	case TFLOAT:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( float *) data, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
	case TDOUBLE:
	    fitsCheck( ffpcl( fptr, fitsType, 1, 1, 1, size,
		    ( double *) data, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
	default:
	    *sfStatus = SF_E_DHS_TYPE;
	    sfFormatMessage( *sfStatus, type );
	    break;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteBinKeywords
 *
 * INVOCATION:
 * numNewWords = sfWriteBinKeywords( fptr, instrument, type, extName,
 *	sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		The fits file pointer.		
 * (>)  instrument  (char[FLEN_VALUE])  The instrument used to get the image.
 * (>)	type	    (DHS_DATA_TYPE)	Type of data in the extension.
 * (>)	arrayInfo   (SF_ARRAY_INFO*) 	Information about the array.
 * (!)	sfStatus    (SF_STATUS*)	SF library status indicator.
 *
 * FUNCTION VALUE:
 * int	: Number of keywords written.
 *
 * PURPOSE:
 * To create a new binary extension at the bottom of the FITS file.
 *
 * DESCRIPTION:
 * This function is currently not used since the SF library is
 * currently not supporting binary extension, image extensions
 * are used for all data.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void 	sfWriteBinKeywords
(
    fitsfile	    *fptr,	/* (in)	 The fits file pointer.		*/
    char            instrument[FLEN_VALUE],
                                /* (in)  The instrument.                */
    DHS_DATA_TYPE   type,	/* (in)  Type of attrib data object.	*/
    SF_ARRAY_INFO   *arrayInfo,	/* (in)  Information about the array.	*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status indicator.	*/
)
{
    DD_ARRAY_TYPE   array;      /* Attribute array type.            	*/
    char    	    comm[FLEN_COMMENT];
			            /* Comment for FITS file. 		*/
    FITS_STATUS	    fitsStatus;	/* From FITS routines. 			*/
    char            keyword[FLEN_KEYWORD];
                                /* Keyword for the FITS file.      	*/
    boolean         writeIt;    /* TRUE if the keyword is written to the*/
    char            *tform[1];
    char            *ttype[1]= { "\0" };
    				/* Type of data in each column.		*/
    char            *tunit[] = { "\0" };
    
    

    fnEntry( "sfWriteBinKeywords", *sfStatus );
    statCheck( *sfStatus, VOID );

    fitsStatus = FITS_S_SUCCESS;


    switch( type )
    {
	case DHS_DT_BOOLEAN:
	    tform[0] = "1L";
	    break;
	case DHS_DT_CHAR:
	    tform[0] = "1B";
	    break;
	case DHS_DT_STRING:
	    tform[0] = "1PA";
	    break;
	case DHS_DT_INT8:
	case DHS_DT_UINT8:
	    tform[0] = "1I";
	    break;
	case DHS_DT_INT16:
	case DHS_DT_UINT16:
	case DHS_DT_INT32:
	case DHS_DT_UINT32:
	    tform[0] = "1J";
	    break;		 		 
	case DHS_DT_FLOAT:
	    tform[0] = "1E";
	    break;
	case DHS_DT_DOUBLE:
	    tform[0] = "1D";
	    break;
	default:
	    *sfStatus = SF_E_DHS_TYPE;
	    sfFormatMessage( *sfStatus, type );
	    fnReturn( VOID );
    }

    fitsCheck( ffcrtb( fptr, BINARY_TBL, 1, 1, ttype, tform, tunit,
	    arrayInfo->extName, &fitsStatus ), fitsStatus, *sfStatus,
	    VOID );

    check( sfCheckWithDd( instrument, DHS_DT_INT32, 0,
	    "extensionVersion",	&array, comm, keyword, &writeIt,
	    sfStatus ), *sfStatus, VOID );
    fitsCheck( ffpkyj( fptr, keyword, arrayInfo->extVer, comm, &fitsStatus ),
	    fitsStatus, *sfStatus, VOID );
    
    
    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteDhsKeywords
 *
 * INVOCATION:
 * sfWriteDhsKeywords( fitsfilePtr, frameInformation, extension, oneChunk,
 *	naxis, instrument, bottomLeftCorner, axisSize, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		FITS file pointer.
 * (>)	frameInfo   (SF_FRAME_INFO) 	Details about the frame.
 * (!)  extension   (SF_EXT_INFO*)	Extension information.
 * (>)	oneChunk    (boolean)		TRUE if FITS file contains
 *					one observation chunk.
 * (>)	naxis	    (int)		Number of dimensions.
 * (>) instrument   (char[FLEN_VALUE])  The instrument used to get the image.
 * (!)	blc	    (long[SF_MAXDIM])	The data's origin.
 * (!)	axissz	    (long[SF_MAXDIM])	Sizes of each axis for the
 *					full image.
 * (!)	sfStatus    (SF_STATUS*)	SF library status indicator.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To write all of the keywords that are required by the DHS library,
 * Gemini, into the FITS file.
 *
 * DESCRIPTION:
 * 1. If we are putting one observation "chunk" into the FITS file
 * 	  then write the origin and axisSize attributes.
 * 2. Write the EXTNAME and FRMNAME keywords.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or eqaul to SF_S_SUCCESS.
 *
 * "frameInfo" contains the frames identifier, name and the
 * frame itself.
 *
 * SEE ALSO:
 * - sfGetDhsKeywords
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

int sfWriteDhsKeywords
(
    fitsfile	    *fptr,	/* (in)	 FITS file pointer.		*/
    SF_FRAME_INFO   frameInfo,	/* (in)  Details about the frame.	*/
    SF_EXT_INFO	    *ext,	/* (mod) the extension information.	*/
    boolean	    oneChunk,	/* (in)	 TRUE if the FITS file contains */
				/*	 one observation chunk.		*/
    int		    naxis,	/* (in)	 Number of dimensions.		*/
    
    char            instrument[FLEN_VALUE],
                                /* (in)  The instrument.                */
    long	    blc[SF_MAXDIM],
				/* (mod) The data's origin.		*/
    long	    axissz[SF_MAXDIM],
				/* (mod) Size of each axis in the full	*/
				/*	 image.				*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status indicator.	*/
)
{
    DD_ARRAY_TYPE   array;      /* Attribute array type.            	*/
    char	    *comm;	/* Comment for keywords.    		*/
    FITS_STATUS	    fitsStatus;	/* From FITS routines. 			*/
    char            keyword[FLEN_KEYWORD];
                                /* Keyword for the FITS file.      	*/
    int		    numKeywords;/* Number of keywords to write.		*/
    boolean         writeIt;    /* TRUE if the keyword is written to the
				   FITS file.       			*/

        
    fnEntry( "sfWriteDhsKeywords", *sfStatus );
    statCheck( *sfStatus, 0 );

    fitsStatus = FITS_S_SUCCESS;
    numKeywords = 0;
    comm = NULL;
    

    /*
     * Initialize the comment string.
     */

    nullCheck( comm = char_alloc( FLEN_COMMENT ), *sfStatus,
		numKeywords );
    

    /*
     * Write the "ORIGIN" and AXISSZ keywords to the FITS file if
     * we are writing a chunk of the oberservation information.
     */
    
    if ( oneChunk )
    {
	if ( blc != NULL )
	{
	    strnCpy( comm, DHS_COMMENT, FLEN_COMMENT - 1, *sfStatus,
		    numKeywords );
	    (void) sprintf( comm, "%s&\0", comm);

	    fitsCheck( ffpknj( fptr, "ORIGINS", 1, naxis, blc, (char**)&comm,
                    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	    numKeywords += naxis;
	}
	
	if ( axissz != NULL )
	{
	    strnCpy( comm, DHS_COMMENT, FLEN_COMMENT - 1, *sfStatus,
		    numKeywords );
	    (void) sprintf( comm, "%s&\0", comm);

	    fitsCheck( ffpknj( fptr, "AXISSZ", 1, naxis, axissz, (char**)&comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	    numKeywords += naxis;
	}


	/*
	 * Write the frameName and frameId.
	 */ 

	if ( strne( frameInfo.frmName, "" ) )
	{
	    check( sfCheckWithDd( instrument, DHS_DT_STRING, 0, "frameName",
		    &array, comm, keyword, &writeIt, sfStatus ), *sfStatus,
		    numKeywords );
	    fitsCheck( ffpkys( fptr, keyword, frameInfo.frmName, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	}
	
	if ( strne( frameInfo.frmId, "" ) )
	{
	    check( sfCheckWithDd( instrument, DHS_DT_STRING, 0, "frameId",
		    &array, comm, keyword, &writeIt, sfStatus ), *sfStatus,
		    numKeywords );
	    fitsCheck( ffpkys( fptr, keyword, frameInfo.frmId, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	}
	


	/*
	 * Write the EXTNAME and EXTVER keywords.
	 */

	if ( strne( frameInfo.extName, "" ) )
	{
	    check( sfCheckWithDd( instrument, DHS_DT_STRING, 0, "dataType",
		    &array, comm, keyword, &writeIt, sfStatus ), *sfStatus,
		    numKeywords );
	    fitsCheck( ffpkys( fptr, keyword, frameInfo.extName, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	}
	

	if ( frameInfo.extVer != SF_NULL_EXTVER )
	{
	    check( sfCheckWithDd( instrument, DHS_DT_INT32, 0,
		    "extensionVersion",	&array, comm, keyword, &writeIt,
		    sfStatus ), *sfStatus, numKeywords );
	    fitsCheck( ffpkyj( fptr, keyword, ext->extVer, comm, &fitsStatus ),
		    fitsStatus, *sfStatus, numKeywords );
	}
    }
    else
    {
	/*
	 * Write the frame name and id.
	 */
	
	if ( ext != NULL && streq(ext->frmName,"" ) )
	{
	    strnCpy( ext->frmName, frameInfo.frmName, FLEN_VALUE, *sfStatus,
		    numKeywords );
	    check( sfCheckWithDd( instrument, DHS_DT_STRING, 0, "frameName",
		    &array, comm, keyword, &writeIt, sfStatus ), *sfStatus,
		    numKeywords );
	    fitsCheck( ffpkys( fptr, keyword, frameInfo.frmName, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	    
	    check( sfCheckWithDd( instrument, DHS_DT_STRING, 0, "frameId",
		    &array, comm, keyword, &writeIt, sfStatus ), *sfStatus,
		    numKeywords );
	    fitsCheck( ffpkys( fptr, keyword, frameInfo.frmId, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	}
	else if ( ext != NULL && strne( frameInfo.frmName, ext->frmName ) )
	{	
	    *sfStatus = SF_S_FRMNAME;
	    sfFormatMessage( *sfStatus, frameInfo.frmName, ext->frmName,
		    ext->frmId );
	}
	
		
    	/*
	 * Write the EXTNAME keyword
	 */

	if ( ext != NULL && ! ext->extWritten )
	{
	    strnCpy( ext->extName, frameInfo.extName, FLEN_VALUE, *sfStatus,
		    numKeywords );
	    check( sfCheckWithDd( instrument, DHS_DT_STRING, 0, "dataType",
		    &array, comm, keyword, &writeIt, sfStatus ), *sfStatus,
		    numKeywords );
	    fitsCheck( ffpkys( fptr, keyword, frameInfo.extName, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	    
	    check( sfCheckWithDd( instrument, DHS_DT_INT32, 0,
		    "extensionVersion", &array, comm, keyword, &writeIt,
		    sfStatus ), *sfStatus, numKeywords );
	    fitsCheck( ffpkyj( fptr, keyword, ext->extVer, comm,
		    &fitsStatus ), fitsStatus, *sfStatus, numKeywords );
	    ext->extWritten = TRUE;
	}
    }
        
    if ( comm != NULL )
    {
	gen_free( comm );
    }
    
    fnReturn( numKeywords );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteFrame
 *
 * INVOCATION:
 * sfWriteFrame( fitsFilePointer, extensionInformation, frameInformation,
 * 	datasetInformation, instrument, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		Fits file pointer.
 * (!)	ext	    (SF_EXT_INF*)	Extension information.
 * (>)	frameInfo   (SF_FRAME_INFO*)	Details about the image.
 * (!)	dsInfo	    (SF_DS_INFO)	Details about the dataset.
 * (>)	oneChunk    (boolean)		Indicates if processing an
 *					observation chunk.
 * (>)	isPrime	    (boolean)		Indicates if prime hdu.
 * (>)	instrument  (char[FLEN_VALUE])  The instrument.
 * (!)	sfStatus    (SF_STATUS*)	SF library status indicator.	
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Write a frame's attributes and data to a FITS file.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Write the optional keywords and find any frames and/or
 *    data array's
 * 3. Write this frame's data, if there was any.
 * 4. Write all of the array, if any were found, in separate
 *    image extensions.
 * 5. Write all of the frames, if any were found, in separate
 *    image extensions.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

int	sfWriteFrame
(
    fitsfile	    *fptr,	/* (in)  Fits file pointer.		*/
    SF_EXT_INFO	    *ext,       /* (mod) Extension information.		*/
    SF_FRAME_INFO   *frameInfo,	/* (in)  Details about the image.	*/
    SF_DS_INFO	    *dsInfo,	/* (mod) Details about the dataset.	*/
    boolean	    oneChunk,	/* (in)  Indicates if processing an	*/
				/*       observation chunk.		*/
    boolean	    isPrime,	/* (in)  Indicates if prime hdu.	*/
    char	    instrument[FLEN_VALUE],
				    /* (in)  The instrument. 		*/
    SF_STATUS	    *sfStatus	/* (mod) Function return status.	*/
)
{
    EPTR	*arrayList;
    long	axissz[SF_MAXDIM];
				/* Axis size of the full image. 	*/
    int		bitpix;		/* Number of bits per pixel.		*/
    long	blc[SF_MAXDIM];
				/* The origin, bottom left corner,	*/
				/*   of this image piece in the full.	*/
    EPTR	*frameList;
    				/* The id's of all the frames.		*/
    int		i;
    int		last;		/* Index of the the last item in a	*/
				/* e-pointer list.			*/
    int		naxis;		/* Number of dimensions.		*/
    long	naxes[SF_MAXDIM];
    int		numKeywords;	/* Number of keywords (not used).	*/
				/* NAXES FITS primary attribute.	*/
    
    fnEntry( "sfWriteFrame", *sfStatus );
    statCheck( *sfStatus, 0 );


    /*
     * Initialize variables.
     */

    numKeywords = 0;
    
    nullCheck( frameList = sfFrameListCreate( 5, sfStatus ), *sfStatus,
	    numKeywords );
    nullCheck( arrayList = sfArrayListNew(), *sfStatus, numKeywords );
    
    
    /*
     * Write the optional keywords. The number of keywords in the
     * dataset information is NOT updatad since it should already
     * contain the total number.
     */
    
    check( numKeywords += sfWriteOptKeywords( fptr, frameInfo, dsInfo,
	    instrument, oneChunk, frameList, &arrayList, sfStatus ),
	    *sfStatus, numKeywords );

    
    /*
     * Write the data, if there is any found to the FITS file.
     */
	
    if ( frameInfo->hasData )
    {
	check( sfGetImgKeywords( frameInfo->sdsId, instrument, &bitpix,
		&naxis, naxes, sfStatus ), *sfStatus, numKeywords );
	check( sfGetDhsKeywords( frameInfo->sdsId, instrument, &naxis,
		blc, axissz, frameInfo->frmName, frameInfo->extName,
		sfStatus ), *sfStatus, numKeywords );
	check( sfUpdateDhsKeywords( dsInfo, frameInfo, ext, sfStatus ),
		*sfStatus, numKeywords );
	check( numKeywords += sfWriteDhsKeywords( fptr, *frameInfo, ext,
		oneChunk, naxis, instrument, blc, axissz, sfStatus ),
		*sfStatus, numKeywords );
	check( sfWriteImgData( fptr, frameInfo->sdsId, oneChunk, blc,
		axissz, sfStatus ), *sfStatus, numKeywords );
    }
    else
    {
	if ( !isPrime )
	{
	    naxis = 0;
	    check( sfGetDhsKeywords( frameInfo->sdsId, instrument, &naxis,
		    blc, axissz, frameInfo->frmName, frameInfo->extName,
		    sfStatus ), *sfStatus, numKeywords );
	    check( sfUpdateDhsKeywords( dsInfo, frameInfo, ext, sfStatus ),
		    *sfStatus, FALSE );
	    check( numKeywords += sfWriteDhsKeywords( fptr, *frameInfo, ext,
		    oneChunk, naxis, instrument, blc, axissz, sfStatus ),
		    *sfStatus, numKeywords );
	}
	else
	{
	    check( numKeywords += sfWriteDhsKeywords( fptr, *frameInfo, ext,
		    oneChunk, 0, instrument, NULL, NULL, sfStatus ),
		    *sfStatus, numKeywords );
	}
    }


    /*
     * Process the list of multi-dimensional arrays.
     */

    if( ( last = eptrlen( arrayList ) ) > 0 && isPrime )
    {
	*sfStatus = SF_E_PRIME_DATA;
	sfFormatMessage( *sfStatus );
    }
    for( i = 0; i < last; i++ )
    {
	check( sfNewBinHdu( fptr, eptritem( arrayList, i), instrument,
		oneChunk, dsInfo, sfStatus ), *sfStatus, numKeywords );
    }
    sfArrayListFree( &arrayList );

	
    /*
     * Process the subframes that were found by calling this function
     * recursively.
     */

    last = eptrlen( frameList );
    for( i = 0; i < last; i++ )
    {
	if( oneChunk )
	{
	    check( sfDoChunkFrame( fptr, eptritem( frameList, i), dsInfo,
		    instrument, sfStatus ), *sfStatus, numKeywords );
	}
	else
	{
	    check( sfDoFrame( fptr, eptritem( frameList, i), dsInfo,
		    instrument, sfStatus ), *sfStatus, numKeywords );
	}
    }

    check( sfFrameListDelete( frameList, sfStatus ), *sfStatus, numKeywords );
    
    fnReturn( numKeywords );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteImgData
 *
 * INVOCATION:
 * sfWriteImgData( filePointer, frame, oneChunk, bottomLeftCorner,
 *		axisSize, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)		FITS file pointer.
 * (>)	frame	    (DHS_BD_FRAME)	Frame object, with data.
 * (>)	oneChunk    (boolean)		TRUE if the FITS file contains
 *					one observation chunk.
 * (>)	blc	    (long[SF_MAXDIM])	The data's origin (bottom, left
 *					corner).
 * (>) 	axissz	    (long[SF_MAXDIM])	Size of each axis for the full
 *					image.
 * (!)	sfStatus    (SF_STATUS*)	SF library status indicator.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To write a block of image data to a FITS file.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Get the data and it's size and type from the DHS frame.
 * 3. If more than one obsrvation chunk is written to this FITS file
 *	  then use "axissz" for the dimensions fo the data.
 * 4. Find the size of the data to be written and the coordinates for
 *    the top, right corner of the data.
 * 5. Write the data to the FITS file based on the image data type.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfSds2Fits
 *
 * DEFICIENCIES:
 * Size of each axis is converted from an unsigned long to a long
 * value.  Since cfitsio uses long values but SDS uses unsigned value.
 *-
 ************************************************************************
 */

void sfWriteImgData
( 
    fitsfile	    *fptr,	/* (in)	 FITS file pointer.	 	*/
    DHS_BD_FRAME    frame,	/* (in)	 Frame object, with data. 	*/
    boolean	    oneChunk,	/* (in)	 TRUE if FITS file contains	*/
				/*   one observation chunk.	 	*/
    long	    blc[SF_MAXDIM],
				/* (in)	 Bottome, left corner.		*/
    long	    axissz[SF_MAXDIM],
				/* (in)	 Size of each axis.	 	*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status. 		*/
)
{
    void	    *attribData;/* Attribute values (may be an array)	*/
    char	    *attribName;/* Name of the attribute.		*/
    DHS_STATUS	    dhsStatus;  /* From DHS routines		    	*/
    unsigned long   dims[SF_MAXDIM];
    				/* Array of dims of attrib data.	*/
    int		    first;	/* Where the first pixel is.		*/
    FITS_STATUS	    fitsStatus; /* From cfitsio routines.		*/
    int		    fitsType;	/* FITS data type.			*/
    long	    group;	/* FITS group variable (always 1).	*/
    int		    i;
    int		    ndims;	/* Number of dimensions.		*/
    long	    size;	/* Size of the data.			*/
    long	    trc[SF_MAXDIM];
    				/* Top right position of the data.	*/
    DHS_DATA_TYPE   type;	/* DHS data type.			*/


    fnEntry ( "sfWriteImgData", *sfStatus );
    statCheck( *sfStatus,  VOID );



    /*
     * Initialize the variables.
     */
	 
    dhsStatus = DHS_S_SUCCESS;
    fitsStatus = FITS_S_SUCCESS;
    group = 1;
    size = 1;

    /*
     * Find the dimensions and type of the data.
     */
    
    dhsBdFrameInfo( frame, &attribName, &type, &ndims, dims,
	    (void**)&attribData, &dhsStatus );
    if ( dhsStatus != DHS_S_SUCCESS )
    {
	dhsStatus = DHS_S_SUCCESS;
	dhsCheck( dhsBdAttribInfo( frame, &attribName, &type, &ndims, dims,
		(void**)&attribData, &dhsStatus ), dhsStatus, *sfStatus,
		VOID );
    }
    
	
    
    /*
     * Ensure image has less than MAX dimensions and greater than 1.
     */
    
    if ( ndims < 1 || ndims > SF_MAXDIM )
    {
	
	*sfStatus = SF_E_DIMS ;
	sfFormatMessage( *sfStatus, ndims );
	fnReturn( VOID );
    }

    
    /*
     * Set the top, right corner values and verify correctness of the
     * origin.
     */

    for( i = 0; i < ndims; i++ )
    {
	if ( blc[i] < 1 || blc[i] > axissz[i] )
	{
	    if ( ndims == 2 )
	    {
		*sfStatus = SF_E_ORIGIN_2D;
		sfFormatMessage( *sfStatus, axissz[0], axissz[1], blc[0],
			blc[1] );
		fnReturn( VOID );
	    }
	    else if ( ndims == 3 )
	    {
		*sfStatus = SF_E_ORIGIN_3D;
		sfFormatMessage( *sfStatus, axissz[0], axissz[1], axissz[2],
			blc[0], blc[1], blc[2] );
		fnReturn( VOID );
	    }
	    else
	    {
		*sfStatus = SF_E_ORIGIN;
		sfFormatMessage( *sfStatus );
		fnReturn( VOID );
	    }
	}
	else if ( blc[i] == 1 )
	{
	    trc[i] = (long)dims[i];
	}
	else
	{
	    trc[i] = blc[i] + (long)dims[i] - 1;
	}
    }	
    

    /*
     * Ensure that ths size of the data is less than what is expected.
     * Also adjust dims, if necessary, and calculate the size of the
     * image.
     */


    for( i = 0; i < ndims; i++ )
    {
	if ( dims[i] > axissz[i] + 1 )
	{
	    if ( ndims == 2 )
	    {
		*sfStatus = SF_E_AXISSZ_2D;
		sfFormatMessage( *sfStatus, dims[0], dims[1], axissz[0]+1,
			axissz[1]+1 );
		fnReturn( VOID );
	    }
	    else if ( ndims == 3 )
	    {
		*sfStatus = SF_E_AXISSZ_3D;
		sfFormatMessage( *sfStatus, dims[0], dims[1], dims[2],
			axissz[0]+1, axissz[1]+1, axissz[2]+1 );
		fnReturn( VOID );
	    }
	    else
	    {
		*sfStatus = SF_E_AXISSZ;
		sfFormatMessage( *sfStatus, i+1, dims[i], axissz[i]+1 );
		fnReturn( VOID );
	    }
	}

	
	/*
	 *	Need to adjust NAXES if NOT oneChunk, should be the
	 *	size of the whole image.
	 */

	if ( !oneChunk )
	{
	    dims[i] = axissz[i];
	}


	/*
	 * Calculate the size of the image
	 */

	size *= (long)dims[i];
    }
    
	
    /*
     * Convert from a DHS data type to a cfitsio data type.
     */
	
    dhs2fits( type, fitsType, *status, VOID );


    /*
     * Write the data to the FITS file.
     */

    if ( oneChunk )
    {
	    first = 1;
	    if ( type == DHS_DT_BOOLEAN )
	    {
		fitsType = TBYTE;
	    }
	    fitsCheck( ffppr( fptr, fitsType, first, size,
		    attribData, &fitsStatus ), fitsStatus, *sfStatus, VOID );
    }	 
    else
    {
	/* 
	 * Call the appropriate cfitsio function based on data type
	 */

	if ( type == DHS_DT_BOOLEAN )
	{
	    fitsType = TBYTE;
	}    
	switch( fitsType )
	{
	    case TBYTE:
		fitsCheck( ffpssb( fptr, group, ndims, (long*)dims, blc, trc,
			(unsigned char *)attribData, &fitsStatus ), fitsStatus,
			*sfStatus,  VOID );
		break;

	    case TINT:
		fitsCheck( ffpssk( fptr, group, ndims, (long*)dims, blc, trc,
			(int *)attribData, &fitsStatus ), fitsStatus, *sfStatus,
			VOID );
		break;

	    case TUSHORT:
		fitsCheck( ffpssui( fptr, group, ndims,	(long*)dims, blc, trc,
			(unsigned short*) attribData, &fitsStatus ), fitsStatus,
			*sfStatus, VOID );
		break;

	    case TSHORT:
		fitsCheck( ffpssi( fptr, group, ndims, (long*)dims, blc, trc,
			(short *)attribData, &fitsStatus ), fitsStatus,
			*sfStatus, VOID );
		break;

	    case TULONG:
		fitsCheck( ffpssuj( fptr, group, ndims,	(long*)dims, blc, trc,
			(unsigned long*) attribData, &fitsStatus ), fitsStatus,
			*sfStatus, VOID );
		break;
		
	    case TLONG:
		fitsCheck( ffpssj( fptr, group, ndims, (long*)dims, blc, trc,
			(long*)attribData, &fitsStatus ), fitsStatus, *sfStatus,
			VOID );
		break;
		
	    case TFLOAT:
		fitsCheck( ffpsse( fptr, group, ndims, (long*)dims, blc, trc,
			(float*)attribData, &fitsStatus ), fitsStatus,
			*sfStatus, VOID );
		break;
		
	    case TDOUBLE:
		fitsCheck( ffpssd( fptr, group, ndims, (long*)dims, blc, trc,
			(double*)attribData, &fitsStatus ), fitsStatus,
			*sfStatus, VOID );
		break;
		
	    default:
		*sfStatus = SF_E_FITS_TYPE;
		sfFormatMessage( *sfStatus, type );
		fnReturn( VOID );
	}
    }

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteImgKeywords
 *
 * INVOCATION:
 * sfWriteImgKeywords( fptr, bitpix, naxis, naxes, isPrimeHdu, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fptr	    (fitsfile*)		File pointer to the FITS file.
 * (>) bitpix	    (int)		Number of bits per pixel.
 * (>) naxis	    (int)	    	Number of dimensions.
 * (>) naxes	    (long[SF_MAXDIM)	Size of each axis.
 * (>) isPrimeHdu   (boolean)	    	TRUE if writing the keywords
 *					to the FITS primaray header unit.
 * (!) sfStatus	    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To write the bitpix, naxis, and naxes into a FITS image extension
 * or the primary HDU of a FITS file.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. Write the keywords to the FITS header unit.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - sfGetImgKeywords.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfWriteImgKeywords
( 
    fitsfile	*fptr,		/* (in)  File pointer to the fits file.	*/
    int		bitpix,		/* (in)  Number of bits per pixel	*/
    int	    	naxis,		/* (in)  Number of dimensions.		*/
    long	naxes[],	/* (in)	 Size of each axis.		*/
    boolean	isPrimeHdu,	/* (in)  TRUE if writing the keywords to*/
				/*       the FITS primaray header unit.	*/
    SF_STATUS	*sfStatus	/* (mod) SF library status.		*/
)
{
    DD_ARRAY_TYPE   array;	/* Attribute array type.		*/
    char	    comm[FLEN_COMMENT];
			        /* Comment for FITS file. 		*/
    FITS_STATUS	    fitsStatus;	/* From cfitsio routines.		*/
    char	    keyword[FLEN_KEYWORD];
			     	/* Keyword for the FITS file. 		*/
    double	    value; 	/* Value of BSCALE & BZERO.		*/
    boolean	    writeIt;	/* TRUE if the keyword is written to the
				   FITS file.				*/


    fnEntry ( "sfWriteImgKeywords", *sfStatus);
    statCheck( *sfStatus,  VOID );


    /*
     * Initialize variables.
     */
	
    fitsStatus = FITS_S_SUCCESS;

	
    /* 
     * Write the primary header keywords to the FITS file
     * and create the new primary header or extension header.
     */

    if ( isPrimeHdu )
    {
	fitsCheck( ffphps( fptr, bitpix, (int)(naxis), naxes,
		&fitsStatus ), fitsStatus, *sfStatus, VOID );
    }		
    else	
    {	
	fitsCheck( ffcrim( fptr, bitpix, (int)(naxis), naxes,
		&fitsStatus ), fitsStatus, *sfStatus, VOID );
    }		

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteIndexedArray
 *
 * INVOCATION:
 * sfWriteIndexedArrray( fitsfilePointer, keyword, type, numberDimensions,
 *			dimensions, comment, data, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	(fitsfile*)		    FITS file pointer.
 * (>)	keyword	(char[FLEN_KEYWORD]	    FITS keyword to be written.
 * (>)	type	(DHS_DATA_TYPE)		    The keyword's DHS data type.
 * (>)	ndims	(int) 			    Number of dimension the data is.
 * (>)	dims	(unsigned long[SF_MAXDIM])  The size of each axis.
 * (>)	comm	(comm[FLEN_COMMENT])	    Comment for the FITS file. 
 * (>)	data	(void*)			    The keyword's value.	
 * (!) 	sfStatus(SF_STATUS*)		    SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Write an indexed array to a FITS file.  They are written as multiple
 * keywords in the FITS header unit, one for each value in the array.
 *
 * When the value are placed in the FITS the index number is added to the
 * right of the keyword.  An example of an indexed keyword is "NAXIS".
 *
 * DESCRIPTION:
 * 1. Make sure the array is an indexed array.
 * 2. If there was a problem with the data, for example a bad type, then
 *	  write the keyword and it's value as a "COMMENT".
 * 3. Write the keywords to the FITS file.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfWriteIndexedArray
(
    fitsfile	    *fptr,	/* (in)  FITS file pointer.		*/
    char	    keyword[FLEN_KEYWORD],
				/* (in)  FITS keyword to be written.	*/
    char	    *attribute,	/* (in)  SDS attribute name.		*/
    DHS_DATA_TYPE   type,	/* (in)  The keyword's DHS data type.	*/
    int		    ndims,	/* (in)  Number of dimension.		*/
    unsigned long   dims[SF_MAXDIM],
				/* (in)  The size of each axis. 	*/
    char	    comm[FLEN_COMMENT],
				/* (in)  Comment for the FITS file. 	*/
    void	    *data,	/* (in)  Value to be written.		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    FITS_STATUS     fitsStatus;	    /* Cfitsio status indicator.	*/
    int		    i;
    char	    *index;	    /* Character representation of index*/
    void	    *indexedData;    /* Data for one indexed keyword.	*/
    char	    indexedKeyword[FLEN_KEYWORD];
    				    /* Keyword plux index number.	*/
    int	    	    *intVals;	    /* Array of longs.			*/
    long	    *longVals;	    /* Array of longs.			*/
    char	    *charVal;       /* A character Value.		*/
		

    fnEntry( "sfWriteIndexedArray", *sfStatus );

    /*
     * Initialize variables.
     */

    fitsStatus = FITS_S_SUCCESS;
    longVals = NULL;
    intVals = NULL;
    charVal = NULL;
    index = NULL;
    indexedData = NULL;
    

	
    /*
     * Make sure the array is an indexed array, has one dimension.
     */
	
    if ( ndims != 1 )
    {
	*sfStatus = SF_E_DIMS;
	sfFormatMessage( *sfStatus, ndims );
	fnReturn( VOID );
    }

    
    if ( *sfStatus < SF_S_SUCCESS )
    {
	check( sfWriteBadIndexedArray( fptr, attribute, type, ndims, dims,
		comm, data, sfStatus ), *sfStatus, VOID );
	fnReturn( VOID );
    }



    /*
     * Based on the type the data is write it to the FITS file.
     */
	
    switch( type )
    {
	case DHS_DT_CHAR:
	    /*
	     * Convert from characters to string values since no
	     * cfitsio function exist for character arrays.
	     */
			
	    nullCheck( charVal = char_alloc( 2 ), *sfStatus, VOID );
	    nullCheck( index = char_alloc( FLEN_KEYWORD ), *sfStatus, VOID );
	    
	    for( i = 1; i <= dims[0]; i++ )
	    {
		indexedKeyword[0]='\0';
		strnCat( indexedKeyword, keyword, FLEN_KEYWORD, *sfStatus,
			VOID );
		(void) sprintf( index, "%u", i );
		strnCat( indexedKeyword, index, FLEN_KEYWORD, *sfStatus,
			VOID );

		charVal[0] = ( (char*)data )[i-1];
		charVal[1] = '\0';


		fitsCheck( ffpkys( fptr, indexedKeyword, charVal, comm,
			&fitsStatus ), fitsStatus, *sfStatus, VOID );
	    }
	    break;
			
	case DHS_DT_STRING:
	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpkns( fptr, keyword, 1, dims[0], (char**)data,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    
	    break;
	    
	case DHS_DT_BOOLEAN:
	    /*
	     * Convert from DHS_BOOLEAN type to FITS logical type.
	     */
	    
	    nullCheck( intVals = malloc( sizeof(int) * dims[0] ),
		    *sfStatus, VOID );
	    for( i = 1; i <= dims[0]; i++ )
	    {
		if ( ((unsigned char*)data)[i-1] )
		{
		    intVals[i-1] = 1;
		}
		else
		{
		    intVals[i-1] = 0;
		}
	    }
	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpknl( fptr, keyword, 1, dims[0], intVals,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    
	    break;

	case DHS_DT_INT8:
	    /*
	     * Convert from bytes to integers since we need to use
	     * the cfitsio routine that uses long values.
	     */

	    nullCheck( longVals = malloc( sizeof(long) * dims[0] ),
		    *sfStatus,  VOID );
	    for( i = 1; i <= dims[0]; i++ )
	    {
		longVals[i-1] = ((char*)data)[i-1];
	    }

	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpknj( fptr, keyword, 1, dims[0], longVals,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    break;
			
	case DHS_DT_UINT8:
	    nullCheck( index = char_alloc( FLEN_KEYWORD ), *sfStatus, VOID );
	    for( i=1; i <= dims[0]; i++ )
	    {
		indexedKeyword[0]='\0';
		strnCat( indexedKeyword, keyword, FLEN_KEYWORD, *sfStatus,
			VOID );
		(void) sprintf( index, "%u", i );
		strnCat( indexedKeyword, index, FLEN_KEYWORD, *sfStatus,
			VOID );
		
		indexedData = &(((unsigned char*)data)[i-1]);
		check( sfWriteScalarVal( fptr, attribute, indexedKeyword, 
			type, comm, indexedData, sfStatus ), *sfStatus,
			VOID );
	    }
	    break;

	case DHS_DT_INT16:
	    /*
	     * Convert from short ints to integers since we need to use
	     * the cfitsio routine that uses ints values.
	     */

	    nullCheck( longVals = malloc( sizeof(long) * dims[0] ),
		    *sfStatus, VOID );
	    for( i = 1; i <= dims[0]; i++ )
	    {
		longVals[i-1] = ((short*)data)[i-1];
	    }

	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpknj( fptr, keyword, 1, dims[0], longVals,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );

	    break;
	    
	case DHS_DT_UINT16:
	    nullCheck( index = char_alloc( FLEN_KEYWORD ), *sfStatus, VOID );
	    for( i=1; i <= dims[0]; i++ )
	    {
		indexedKeyword[0]='\0';
		strnCat( indexedKeyword, keyword, FLEN_KEYWORD, *sfStatus,
			VOID );
		(void) sprintf( index, "%u", i );
		strnCat( indexedKeyword, index, FLEN_KEYWORD, *sfStatus,
			VOID );
		indexedData = &(((unsigned short*)data)[i-1]);
		check( sfWriteScalarVal( fptr, attribute, indexedKeyword, 
			type, comm, indexedData, sfStatus ), *sfStatus,
			VOID );
	    }
	    break;
	    
	case DHS_DT_INT32:
	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpknj( fptr, keyword, 1, dims[0], (long*)data,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    
	    break;

	case DHS_DT_UINT32:
	    nullCheck( index = char_alloc( FLEN_KEYWORD ), *sfStatus, VOID );
	    for( i=1; i <= dims[0]; i++ )
	    {
		indexedKeyword[0]='\0';
		strnCat( indexedKeyword, keyword, FLEN_KEYWORD, *sfStatus,
			VOID );
		(void) sprintf( index, "%u", i );
		strnCat( indexedKeyword, index, FLEN_KEYWORD, *sfStatus,
			VOID );
		indexedData = &(((unsigned int*)data)[i-1]);
		check( sfWriteScalarVal( fptr, attribute, indexedKeyword, 
			type, comm, indexedData, sfStatus ), *sfStatus,
			VOID );
	    }
	    break;
	    
	case DHS_DT_FLOAT:
	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpknf( fptr, keyword, 1, dims[0], (float*)data, 6,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    
	    break;

	case DHS_DT_DOUBLE:
	    comm[ strlen( comm ) ] = '&';
	    comm[ strlen( comm ) ] = '\0';
	    fitsCheck( ffpknd( fptr, keyword, 1, dims[0], (double*)data, 6,
		    (char**)&comm, &fitsStatus ), fitsStatus, *sfStatus,
		    VOID );
	    
	    break;

	case DHS_DT_AVLIST:
	case DHS_DT_TAG:			
	default:
	    *sfStatus = SF_E_DHS_TYPE;
	    sfFormatMessage( *sfStatus, type );
	    fnReturn( VOID );
    }


    if ( longVals != NULL )
    {
	gen_free( longVals );
	longVals = NULL;
    }

    if ( intVals != NULL )
    {
	gen_free( intVals );
	intVals = NULL;
    }

    if ( charVal != NULL )
    {
	gen_free( charVal );
	charVal = NULL;
    }

    if ( index != NULL )
    {
	gen_free( index );
	index = NULL;
    }

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteOptKeywords
 *
 * INVOCATION:
 * sfWriteOptKeywords( fitsfilePtr, CurrentFrameInfo, datasetInfo,
 *	instrumnet, oneChunk, frameList, arrayList, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fptr	    (fitsfile*)	    	FITS File pointer.
 * (>)	curInfo	    (SF_FRAME_INFO*)	Current frame information.
 * (!)	dsInfo	    (SF_DS_INFO*)	Dataset iformation.
 * (>)	instrument  (char[FLEN_VALUE])	The instrument.
 * (>)	oneChunk    (boolean)	   	TRUE if one and only one SDS
 *					(observation) chunk will be.
 * (<)	frameList   (EPTR*)		Id's of all the subframes.
 * (<)	arrayList   (EPTR**)		Id's of all the multi-dimensional
 *					    arrays that were found.
 * (!)	sfStatus    (SF_STATUS*)	SF library status.
 *
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To write all of the keywords not required by the DHS library or
 * FITS files into the FITS file.  Or in otherwords, write all of the
 * remaining keywords.
 *
 * Also find any subframes and multi-dimensional arrays that may be
 * attached to the current frame.
 *
 * DESCRIPTION:
 * Initialize variables.
 * For each attribute found on the SDS structure.
 *	 If it is a data array ignore it
 *	 If it is a subframe the add it to the list of subframes.
 * 	 If it is the frames name then add it to the current frames info.
 *   Otherwise check the validity of the attribute with the data
 *		dictionary and write it to the FITS file if it gets written.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

int sfWriteOptKeywords
(
    fitsfile	    *fptr,	/* (in) FITS File pointer.		*/
    SF_FRAME_INFO   *curInfo,   /* (in) Current frame information.	*/
    SF_DS_INFO	    *dsInfo,    /* (mod) dataset information.		*/
    char	    instrument[FLEN_VALUE],
    				/* (in)  The instrument.		*/
    boolean	    oneChunk,	/* (in)  Indicates if processing an	*/
				/*       observation chunk.		*/
    EPTR	    *frameList, /* (out) Id's of all the subframes.	*/
    EPTR	    **arrayList, /* (out) Id's of multi-dim. arrays.	*/
    SF_STATUS	    *sfStatus   /* (mod) Function return status.	*/
)
{
    DD_ARRAY_TYPE	array;	    /* Attribute array type.		*/
    void		*attribData;/* Attribute values (may be an
				       array).			  	*/
    DHS_BD_ATTRIB_ID	attribId;   /* attribute identifier.		*/
    char	      	*attribName;/* Name of the attribute		*/
    char	    	comm[FLEN_COMMENT];
			            /* Comment for FITS file. 		*/
    DHS_STATUS	      	dhsStatus;  /* From DHS routines	  	*/
    long	      	dhsIndex;   /* Index of the DHS object.		*/
    unsigned long     	dims[SF_MAXDIM];
    				    /* array of dims of attrib data.*/
    FITS_STATUS		fitsStatus;
    int		 	i;
    char	      	keyword[FLEN_KEYWORD];
				     /* Keyword for the FITS file. 	*/
    SF_STATUS		localStatus; /* Local status value.		*/
    int		      	ndims;	     /* Number of dimensions.		*/
    SF_FRAME_INFO       *newFrameInfo;/* Frame information.             */
    int		      	numKeywords; /* Number of keywords found.	*/
    DHS_DATA_TYPE     	type;	     /* Type of attrib data object.	*/
    boolean	      	writeIt;     /* TRUE if the keyword is
					written to the FITS file.	*/
    

    fnEntry( "sfWriteOptKeywords", *sfStatus );
    statCheck( *sfStatus, 0 );


    /*
     * Initialize variables.
     */
	
    dhsStatus = DHS_S_SUCCESS;
    localStatus = SF_S_SUCCESS;
    numKeywords = 0;
    strnCpy( comm, "", FLEN_COMMENT, *sfStatus, numKeywords );
    attribName = NULL;
    	
    /*
     * Find all of the attributes for the dhsObject.
     */
	
    for( dhsIndex = 0, numKeywords = 0; dhsStatus != DHS_S_NO_ATTRIB
	 && dhsStatus >= DHS_S_SUCCESS; dhsIndex++ )
    {
	attribId=dhsAvIndex( curInfo->sdsId, dhsIndex, &dhsStatus );
	if ( dhsStatus == DHS_S_NO_ATTRIB )
	{
	    /*
	     * Found the last attribute so stop.
	     */
	    break;
	}

		
	/*
	 * Find the detailed information about the attribute.
	 */

	dhsCheck( dhsBdAttribInfo ( attribId, &attribName, &type, &ndims,
		dims, &attribData, &dhsStatus ), dhsStatus, *sfStatus,
		numKeywords );
	

	if ( streq( attribName, "dataType" ) ||
		streq( attribName, "extensionVersion" ) )
	{
	    /*
	     * skip these are written in sfWriteDhsKeywords.
	     */
	}
	else if ( sfIsFrame( attribName ) )
	{    
	    /*
	     * Store the frame's SDS id and FITS id to be
	     * used later when processing the frame.
	     * the frames.  Note that the Sds is the value of
	     * the current id.
	     */

	    nullCheck( newFrameInfo = sfNewFrameInfo( curInfo->frmId,
		    attribName, (DHS_BD_FRAME)attribData, sfStatus ),
		    *sfStatus, numKeywords );


	    check( sfFrameListInsert( &frameList, newFrameInfo, sfStatus ),
		    *sfStatus, numKeywords );
	}
	else if ( streq( attribName, NAME ) )
	{
	    /*
	     * Store the current frame's name.
	     */

	    strnCpy( curInfo->frmName, (char*)attribData, NAME_SIZE, *sfStatus,
		    numKeywords );
	}
	else if ( !oneChunk && streq( attribName, "instrument" ) && strlen(
	    dsInfo->instrument ) > 0 )
	{
	    if ( strne( (char*)attribData, dsInfo->instrument ) )
	    {
		*sfStatus = SF_S_INST;
		sfFormatMessage( *sfStatus );
	    }
	    continue;
	}
	else
	{
	    /*
	     * Check validity with the data dictionary.
	     */

	    localStatus = SF_S_SUCCESS;
	    sfCheckWithDd( instrument, type, ndims, attribName, &array, comm,
		    keyword, &writeIt, &localStatus );

	    if ( !oneChunk && streq( attribName, "instrument" ) )
	    {
		if ( array != DD_NO_ARRAY )
		{
		    *sfStatus = SF_S_INST_ARRAY;
		    sfFormatMessage( *sfStatus, array );
		}
		else
		{
		    strnCpy( dsInfo->instrument, (char*)attribData,
			    FLEN_VALUE,	*sfStatus, numKeywords );
		}
	    }

	    
	    
	    if ( writeIt == TRUE )
	    {
		switch ( array )
		{
		    case DD_EXTENSION:
			/* 
			 * Keep track of the id for the data
			 */

			sfWriteBinExtension( fptr, arrayList, curInfo->frmId,
				attribId, attribName, keyword, type, comm,
				&localStatus );
			break;

		    case DD_INDEX:
			sfWriteIndexedArray( fptr, keyword, attribName,
				type, ndims, dims, comm, attribData,
				&localStatus );
			for( i = 0; localStatus > SF_S_SUCCESS && i < ndims;
			     i++ )
			{
			    numKeywords += dims[i];
			}
			break;

		    case DD_NO_ARRAY:
         // fixme -- mdcb 24Mar08
         // the order of attribute and keyword is swapped
         // this is an old bug that effectively defeat the sds/fits translation from libdd.config
         // fixing it now is bound to break more things (GSA etc.) so leave it for now
         sfWriteScalarVal( fptr, keyword, attribName, type,
			   comm, attribData, &localStatus );
			numKeywords++;
			break;

		    default:
			*sfStatus = SF_E_ARRAY_TYPE;
			sfFormatMessage( *sfStatus, array );
			fnReturn( numKeywords );
		}
		if ( localStatus != SF_S_SUCCESS
			&& *sfStatus == SF_S_SUCCESS )
		{
		    *sfStatus = localStatus;
		}
	    }
	}
    }

    fnReturn( numKeywords );
}


/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteScalarVal
 *
 * INVOCATION:
 * sfWriteScalarVal( fitsfilePointer, attribute, keyword, type, comment, data, status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fptr	(fitsfile*)		FITS file pointer.
 * (>) keyword	(char[FLEN_KEYWORD])	FITS keyword.
 * (>) type	(DHS_DATA_TYPE)		The keyword's type.
 * (>) comm	(char[FLEN_COMMENT])	The keyword's comment.
 * (>) data	(void*)			The keyword's value.
 * (!) sfStatus	(SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Write a scalar value to a given FITS file.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. If an error occured when checking with the data dictionary,
 *	  "sfStatus" is less than SF_S_SUCCESS, then write the keyword
 *	  and its value as a FITS comment.
 * 3. Otherwise write the keyword, value and comment.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfWriteScalarVal
(
    fitsfile	    *fptr,	/* (in)  FITS file pointer.		*/
    char	    *attribute,	/* (in)  SDS attribute name.		*/
    char	    keyword[FLEN_KEYWORD],
				/* (in)  FITS keyword. 			*/
    DHS_DATA_TYPE   type,	/* (in)  The keyword's type.		*/
    char	    comm[FLEN_COMMENT],
				/* (in)  The keyword's comment.		*/
    void	    *data,	/* (in)  The keyword's value.		*/
    SF_STATUS	    *sfStatus	/* (mod) SF library status.		*/
)
{
    unsigned char   byteValue;	/* DHS logical value.			*/
    char	    charValue;  /* Single character value.		*/
    int		    intValue;	/* Single integer value.		*/
    FITS_STATUS	    fitsStatus; /* From cfitsio routines.		*/
    int		    fitsType;	/* FITS data type.			*/
    char	    *newCard;	/* Keyword + keyword's value string.	*/
    char	    *tmpCard;	/* Temporary card.			*/
    void	    *tmpData;	/* Temporary data.			*/
    
    fnEntry( "sfWriteScalarVal", *sfStatus );

    
    /*
     * Initialize variables.
     */
	
    fitsStatus = FITS_S_SUCCESS;

    if ( *sfStatus == SF_S_NOT_FOUND || *sfStatus < SF_S_SUCCESS )
    {
	/*
	 * Didn't pass check with data dictionary so write it as
	 * FITS comment.
	 */

	nullCheck( newCard = strnsav( SF_BAD_COMM "    ", FLEN_CARD ),
		*sfStatus, VOID );
	nullCheck( tmpCard = sfCreateCard( attribute, type, data, comm,
		sfStatus ), *sfStatus, VOID );
	strnCat( newCard, tmpCard, FLEN_CARD, *sfStatus, VOID );
	fitsCheck( ffpcom( fptr, newCard, &fitsStatus ), fitsStatus,
		*sfStatus, VOID );
	if( newCard != NULL )
	{
	    gen_free( newCard );
	    newCard = NULL;
	}
	if( tmpCard != NULL )
	{
	    gen_free( tmpCard );
	    tmpCard = NULL;
	}	
	fnReturn( VOID );
    }
    statCheck( *sfStatus, VOID );
		

#if DEBUG
    printf ( "name = %s, type = %d, comm = %s\n", keyword, type, comm);
#endif

	
    /*
     * Write the keyword data to the FITS file.
     */

    switch( type )
    {
	case DHS_DT_CHAR:
	    charValue = ((char*)data)[0];
	    tmpData = (void*)char_alloc( 2 );
	    ((char*)tmpData)[0] = charValue;
	    ((char*)tmpData)[1] = '\0';
	    type = DHS_DT_STRING;
	    break;

	case DHS_DT_BOOLEAN:
	    byteValue = *((unsigned char*)data);
	    tmpData = malloc( sizeof( int ) );
	    if ( *((unsigned char*)data) )
	    {
		*((int*)tmpData) = 1;
	    }
	    else
	    {
		*((int*)tmpData) = 0;
	    }
	    break; 

	case DHS_DT_INT8:
	    intValue = *((char*)data);
	    tmpData = malloc( sizeof( int) );
	    *((int*)tmpData) = intValue;
	    break;

	default:
	    tmpData = data;
	    break;
    }

    dhs2fits( type, fitsType, *status, VOID );

#if !defined(CFITSIO_OUTDATED)
    // fixme -- mdcb 24Mar08
    // preserve the old cfitsio behiviour to avoid the HIERARCH convention.
    // keyword is truncated to 8 char, cfitsio takes care of upper-casing
    // .. let's be wild, uncomment if you want (FLEN_KEYWORD used to be 8, as of cfitio3.0.6 it's 72)
    // assert (FLEN_KEYWORD>8);
    keyword[8]='\0';
#endif

    fitsCheck( ffpky( fptr, fitsType, keyword, tmpData, comm, &fitsStatus ),
	    fitsStatus, *sfStatus, VOID );

    switch( type )
    {
	case DHS_DT_CHAR:
	case DHS_DT_BOOLEAN:
	case DHS_DT_INT8:
	    if( tmpData != NULL )
	    {
		gen_free( tmpData );
	    }
    }
}
