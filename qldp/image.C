static char rcsid[] = "$Id: image.C,v 1.1.1.1 2002-11-24 20:35:07 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998				(c) 1998
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// qldp/src/image.C
//
// PURPOSE:
// Contains the implementation of the cImage class.
//
// METHOD NAME(S)
// cImage::cImage 	- Constructor for the cImage class.
// cImage::regionCopy	- Copy a region from one image to another.
// cImage::toFloat	- Convert an image to floating point values.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:06  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1999/02/09 19:59:36  nhill
// Fixed a bug in regionCopy, and removed some unused code.
//
// Revision 1.3  1999/01/25 20:24:01  nhill
// Added scaling by bscale and bzero.
//
// Revision 1.2  1999/01/20 19:27:41  nhill
// Added method cImage::regionCopy.
// Moved copying of data from method cImage::toFloat to cImage::regionCopy.
//
// Revision 1.1  1998/11/13 23:37:07  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_file.h"
}

#include "arrayMath.H"

#include "FitsIO.h"
#include "localQldp.H"
#include "image.H"


//
// Define a blank value for floating point arrays.
//

#define	FLOAT_BLANK	0.

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::cImage
//
// INVOCATION:
// N.A. Called automatically when a cImage object is created.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) headerFileName (char *) The name of the header file.
// (>) dataFileName (char *) The name of the data file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cImage class
//
// This version of the constructor creates a new cImage from a FITS file
// stored in separate header and data files.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cImage::cImage
(
    char	*headerFileName,// (in)  Comment
    char	*dataFileName	// (in) Comment
) : iData( dataFileName ), iHeader( headerFileName )
{
    iFits = FitsIO::initialize( iHeader, iData );
    iDelete = false;
    strcpy( iHeaderFName, headerFileName );
    strcpy( iDataFName, dataFileName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::cImage
//
// INVOCATION:
// N.A. Called automatically when a cImage object is created.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) headerFileName (char *) The name for the header file.
// (>) headerSize (int) The size to allocate for the header.
// (>) dataFileName (char *) The name for the data file.
// (>) dataSize (char *) The size to allocate for the data.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cImage class
//
// This version of the constructor creates a new cImage with two new, empty
// files for the header and data.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cImage::cImage
(
    char	*headerFileName,// (in)  Name of the header file.
    int		headerSize,	// (in)  Size of the header.
    char	*dataFileName,	// (in)  Name of the data file.
    int		dataSize	// (in)  Size of the data.
) : iData( dataFileName, Mem::FILE_RDWR, 0 ), 
	iHeader( headerFileName, Mem::FILE_RDWR, 0 )
{
    iFits = NULL;
    iDelete = false;
    strcpy( iHeaderFName, headerFileName );
    strcpy( iDataFName, dataFileName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::regionCopy
//
// INVOCATION:
// cImage::regionCopy( source, xStart, xSize, yStart, ySize );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) source (cImage &) The source image to copy from.
// (>) xStart (int) The source image to copy from.
// (>) xSize (int) The source image to copy from.
// (>) yStart (int) The source image to copy from.
// (>) ySize (int) The source image to copy from.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy a region of the source image to the destination image
//
// This method only works if the destination image is float.
// If the span parameters are zero, the whole image is copied.
//
// DESCRIPTION:
// - Create a cArray<float> object for the destination region.
// - Create a cArray<TYPE> object for the source region.
// - Copy the source to the destination.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cImage::regionCopy
(
    cImage	&source,	// (in)  The source image.
    int		xStart,		// (in)  The start of the region in X.
    int		xSpan,		// (in)  The size of the region in X.
    int		yStart,		// (in)  The start of the region in Y.
    int		ySpan		// (in)  The size of the region in Y.
)
{
    cArray<float> dest( (float *) dataPtr(), FLOAT_BLANK, 
	    xSize(), ySize() );	// An array in the floating point image.


    //
    // If the spans are zero, copy the whole image.
    //

    if ( xSpan == 0 )
    {
	xSpan = xSize();
    }

    if ( ySpan == 0 )
    {
	ySpan = ySize();
    }


    //
    // Create a cArray object for the destination region to be copied.
    //

    cArray<float> destRegion( dest, xStart - 1, xSpan, yStart - 1, 
	    ySpan );


    //
    // Copy the data from the original image to the destination array
    //

    if ( source == BYTE_IMAGE )
    {
	//
	// The image is a byte image.
	//

	int	iBlank;
	cArray<unsigned char> sourceData( (unsigned char *) source.dataPtr(),
		0, source.xSize(), source.ySize() );
				    // cArray object for entire source image.


	//
	// Set the blank value from the source.
	//

	if ( source.iFits->get( "BLANK", iBlank ) == 0 )
	{
	    sourceData.blank( iBlank );
	}


	//
	// Create a cArray object for the source region.
	//

	cArray<unsigned char> sourceRegion( sourceData, xStart - 1, xSpan, 
		yStart - 1, ySpan );


	//
	// Copy the data.
	//

	destRegion = sourceRegion;
    }
    else if ( source == SHORT_IMAGE )
    {
	//
	// The image is a short image.
	//
    
	int	iBlank;
	cArray<short> sourceData( (short *) source.dataPtr(), 0, 
		source.xSize(), source.ySize() );
				    // cArray object for entire source image.


	//
	// Set the blank value from the source.
	//

	if ( source.iFits->get( "BLANK", iBlank ) == 0 )
	{
	    sourceData.blank( iBlank );
	}


	//
	// Create a cArray object for the source region.
	//
	
	cArray<short> sourceRegion( sourceData, xStart - 1, xSpan, 
		yStart - 1, ySpan );


	//
	// Copy the data.
	//
	
	destRegion = sourceRegion;
    }
    else if ( source == USHORT_IMAGE )
    {
	//
	// The image is an unsigned short image.
	//
    
	int	iBlank;
	cArray<unsigned short> sourceData( (unsigned short *) source.dataPtr(),
		0, source.xSize(), source.ySize() );
				    // cArray object for entire source image.


	//
	// Set the blank value from the source.
	//

	if ( source.iFits->get( "BLANK", iBlank ) == 0 )
	{
	    sourceData.blank( iBlank );
	}


	//
	// Create a cArray object for the source region.
	//

	cArray<unsigned short> sourceRegion( sourceData, xStart - 1, xSpan, 
		yStart - 1, ySpan );


	//
	// Copy the data.
	//

	destRegion = sourceRegion;
    }
    else if ( source == LONG_IMAGE )
    {
	//
	// The image is a long image.
	//

	int	iBlank;
	cArray<long> sourceData( (long *) source.dataPtr(), 0, 
		source.xSize(), source.ySize() );
				    // cArray object for entire source image.


	//
	// Set the blank value from the source.
	//

	if ( source.iFits->get( "BLANK", iBlank ) == 0 )
	{
	    sourceData.blank( iBlank );
	}


	//
	// Create a cArray object for the source region.
	//

	cArray<long> sourceRegion( sourceData, xStart - 1, xSpan, 
		yStart - 1, ySpan );


	//
	// Copy the data.
	//

	destRegion = sourceRegion;
    }
    else if ( source == FLOAT_IMAGE )
    {
	//
	// The image is a long image.
	//

	int	fBlank;
	cArray<float> sourceData( (float *) source.dataPtr(), 0, 
		source.xSize(), source.ySize() );
				    // cArray object for entire source image.


	//
	// Set the blank value from the source.
	//

	if ( source.iFits->get( "BLANK", fBlank ) == 0 )
	{
	    sourceData.blank( fBlank );
	}


	//
	// Create a cArray object for the source region.
	//
	
	cArray<float> sourceRegion( sourceData, xStart - 1, xSpan, 
		yStart - 1, ySpan );


	//
	// Copy the data.
	//

	destRegion = sourceRegion;
    }
    if ( source.bScale() != 1. )
    {
	destRegion *= source.bScale();
    }
    if ( source.bZero() != 0. )
    {
	destRegion += source.bZero();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::toFloat
//
// INVOCATION:
// image = nonFloatImage.toFloat( path, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) workPath (char *) The directory where files should be created.
// (!) status (cQldpStatus &) The function return status.
//
// FUNCTION VALUE:
// (cImage *) The new floating point image.
//
// PURPOSE:
// Convert a non-floating point image into a floating point image
//
// The actual data is left blank, since this routine doesn't know if only
// a sub-region can be copied.
//
// DESCRIPTION:
// - Create memory maps for the header and data parts of the floating
//   point image.
// - Copy the header data from the original image to the new image.
// - Replace the bitpix and blank header values with values appropriate
//   for a floating point array.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cImage		*cImage::toFloat
(
    char	*workPath,	// (in)  The path for the files.
    cQldpStatus	&status		// (mod) Inherited status value.
)
{
    char	card[81];	// This is used for formatting FITS cards.
    char	dataFileName[ PATH_MAX ];
    				// The name of the data file.
    int		fid;		// File id for a file.
    char	*fName;		// Name for a file.
    char	headerFileName[ PATH_MAX ];
    				// The name of the header file.
    int		i;
    cImage	*newImage;	// The floating point image created.
    int		size;		// The size of the image data.


    //
    // Ensure the status is ok.
    //

    checkStat( status, return( NULL ) );


    //
    // Get a base file name for the temporary files.
    //

    fName = tempnam( workPath, NULL );


    //
    // Create file names for the header and data files.
    //

    strcpy( headerFileName, fName );
    strcat( headerFileName, ".h.fits" );

    strcpy( dataFileName, fName );
    strcat( dataFileName, ".d.fits" );

    free( fName );


    //
    // Create and map a files for header and data, with the correct size.
    //

    checkSystem( fid = creat( headerFileName, S_IRUSR | S_IWUSR | S_IRGRP ),
	    status, return( NULL ) );
    checkSystem( lseek( fid, headerSize() - 1, SEEK_SET ), status, return( NULL ) );
    checkSystem( write( fid, "", 1 ), status, return( NULL ) );
    checkSystem( close( fid ), status, return( NULL ) );
    checkSystem( fid = creat( dataFileName, S_IRUSR | S_IWUSR | S_IRGRP ),
	    status, return( NULL ) );
    size = ( 32 / abs( iFits->bitpix() ) ) * dataSize();
    checkSystem( lseek( fid, size - 1, SEEK_SET ), status, return( NULL ) );
    checkSystem( write( fid, "", 1 ), status, return( NULL ) );
    checkSystem( close( fid ), status, return( NULL ) );


    //
    // Create a new image to contain the floating point array.
    //

    checkNull( newImage = new cImage( headerFileName, headerSize(), 
	    dataFileName, dataSize() ), status, return( NULL ) );
    newImage->deleteFiles( true );


    //
    // Copy the header data from the original image to the floating point
    // copy.
    //

    memcpy( newImage->headerPtr(), headerPtr(), headerSize() );
    newImage->iFits = FitsIO::initialize( newImage->iHeader, newImage->iData );


    //
    // Replace the bitpix, bscale, bzero and blank values in the floating 
    // point copy.
    //

    for ( i = 0; i < newImage->headerSize(); i+= 80 )
    {
	if ( strncmp( ( (char *) newImage->headerPtr() ) + i,  
		"BITPIX  =", 9 ) == 0 )
	{
	    strncpy( ( (char *) newImage->headerPtr() ) + i, 
		    "BITPIX  =        -32                          "
		    "                                  ", 80 );
	}
	if ( strncmp( ( (char *) newImage->headerPtr() ) + i,  
		"BSCALE  =", 9 ) == 0 )
	{
	    strncpy( ( (char *) newImage->headerPtr() ) + i, 
		    "BSCALE  =        1.0                          "
		    "                                  ", 80 );
	}
	if ( strncmp( ( (char *) newImage->headerPtr() ) + i,  
		"BZERO   =", 9 ) == 0 )
	{
	    strncpy( ( (char *) newImage->headerPtr() ) + i, 
		    "BZERO   =        0.0                          "
		    "                                  ", 80 );
	}
	if ( strncmp( ( (char *) newImage->headerPtr() ) + i,  
		"BLANK   =", 9 ) == 0 )
	{
	    sprintf( card, "BLANK   = %-70.10G", FLOAT_BLANK );
	    strncpy( ( (char *) newImage->headerPtr() ) + i, card, 80 );
	}
    }

    return( newImage );
}
