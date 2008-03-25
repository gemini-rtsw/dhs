//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998
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
// qldp/h/image.H
//
// PURPOSE:
// This file contains the public interface to the cImage class.
//
// CLASSES:
// cImage - This class stores information about a FITS image stored
//	    with separate header and data segments in shared memory.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/27 17:15:09  brighton
// ported to gcc-3.2.1/linux
//
// Revision 1.1.1.1  2002/11/24 20:35:02  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1999/01/25 20:23:23  nhill
// Added scaling by bscale and bzero.
//
// Revision 1.2  1999/01/20 19:32:40  nhill
// Added some new functions.
//
// Revision 1.1  1998/11/13 23:37:27  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef IMAGE_H
#define	IMAGE_H
#include "Mem.h"
#include "Fits_IO.h"

//
//***********************************************************************
//+
// CLASS NAME:
// cImage
//
// PUBLIC METHODS: 
// cImage( char *, char * )
//			- Constructor to create a new image based on file
//			  names of existing header and data segments.
// cImage( char *, int, char *, int )
//			- Constructor to create a new empty image with the 
//			  specified header and data file names, and with 
//			  the specified size.
// cImage( void )	- Default constructor.
// ~cImage( void )	- Destructor for teh class.
// (bool) operator==( tImageDataType )
//			- Check to see if the type of data for the image
//			  is the specified type.
// (bool) operator==( tImageDataType )
//			- Check to see if the type of data for the image
//			  is not the specified type.
// (void *) headerPtr	- Accessor function to return a pointer to the data
//			  for the FITS header.
// (int) headerSize	- Accessor function to return the size of the FITS
//			  header.
// (void *) dataPtr	- Accessor function to return a pointer to the image	//			  data.
// (int) dataSize	- Accessor function to return the size of the image
//			  data.
// (int) keyword( const char *, int & )
//			- Get the integer value for the specified keyword.
// (int) keyword( const char *, float & )
//			- Get the floating point value for the specified 
//			  keyword.
// (void) regionCopy	- Copy a region from the original data to the working
//			  copy.
// (cImage *) toFloat( cQldpStatus & )
//			- Given an existing image, create an equivalent 
//			  image except with the image data stored as 
//			  floating point values.
// (int) xSize		- Accessor function to return the X size of the image.
// (int) ySize		- Accessor function to return teh Y size of the image.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Store information about an image
//
//-
//***********************************************************************
//

class cImage
{
    public:
	typedef	enum ImageDataType tImageDataType;
			cImage( char *, char * );
			cImage( char *, int, char *, int );
			~cImage( void )
			{
			    delete iFits;
			    if ( iDelete )
			    {
				if ( iHeaderFName != NULL )
				{
				    unlink( iHeaderFName );
				}
				if ( iDataFName != NULL )
				{
				    unlink( iDataFName );
				}
			    }
			}
	inline bool	operator==( tImageDataType );
	inline bool	operator!=( tImageDataType );
	const char	*dataFName( void )
			{
			    return( iDataFName);
			}
	void		*dataPtr( void )
			{
			    return( iData.ptr() );
			}
	int		dataSize( void )
			{
			    return( iData.size() );
			}
	void		deleteFiles( bool flag )
			{
			    iDelete = flag;
			}
	const char	*headerFName( void )
			{
			    return( iHeaderFName );
			}
	void		*headerPtr( void )
			{
			    return( iHeader.ptr() );
			}
	int		headerSize( void )
			{
			    return( iHeader.size() );
			}
	inline int	keyword( const char *, int & );
	inline int	keyword( const char *, float & );
	void		regionCopy( cImage &, int = 1, int = 0, 
				int = 1, int = 0 );
	cImage		*toFloat( char *, cQldpStatus & );
	int		xSize( void )
			{
			    return( iFits->width() );
			}
	int		ySize( void )
			{
			    return( iFits->height() );
			}
	float		bScale( void )
			{
			    return( iFits->bscale() );
			}
	float		bZero( void )
			{
			    return( iFits->bzero() );
			}
    protected:
    private:
	Mem		iData;		// This is the data part of the file.
	Mem		iHeader;	// This is the header part of the file.
	FitsIO		*iFits;		// This is a ESO fits structure for the
					// file.
	char		iHeaderFName[ PATH_MAX ];
					// The name of the header file.
	char		iDataFName[ PATH_MAX ];
					// The name of the data file.
	bool		iDelete;	// Delete the image when done?
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::operator==
//
// INVOCATION:
// cImage	image;
// if ( image == FLOAT_IMAGE ) {}
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type (tImageDataType) Type to be compared against.
//
// FUNCTION VALUE:
// (bool) true if the types are the same.
//
// PURPOSE:
// Compare the data type of the image against the specified data type
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

bool		cImage::operator==
(
    tImageDataType 		// (in) The type to compare with the image
		type		//      type.
)
{
    if ( iFits != NULL )
    {
	return( iFits->bitpix() == type );
    }
    else
    {
	return( false );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::operator!=
//
// INVOCATION:
// cImage	image;
// if ( image != FLOAT_IMAGE ) {}
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type (tImageDataType) The image type to compare against.
//
// FUNCTION VALUE:
// (bool) True if the image does not have the type of data specified.
//
// PURPOSE:
// Compare the image data type against the specified type
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

bool		cImage::operator!=
(
    tImageDataType		// The type to compare against.
		type
)
{
    if ( iFits != NULL )
    {
	return( iFits->bitpix() != type );
    }
    else
    {
	return( false );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::keyword
//
// INVOCATION:
// if ( image.keyword( "KEYWORD", i ) != 0 ) { error }
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) keyword (const char *) The keyword name.
// (<) value (int &) The keyword value.
//
// FUNCTION VALUE:
// (int) status value, 0 if success.
//
// PURPOSE:
// Get an integer value for the specified keyword
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

int		cImage::keyword
( 
    const char	*keyword,	// (in)  The keyword name.
    int		&value		// (out) The value.
)
{
    if ( iFits == NULL )
    {
	return( -1 );
    }
    else
    {
	iFits->get( keyword, value );
	return( 0 );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cImage::keyword
//
// INVOCATION:
// if ( image.keyword( "keyword", f ) != 0 ) { error }
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) keyword (const char *) The name of the keyword.
// (<) value (float &) The value of the keyword.
//
// FUNCTION VALUE:
// (int) Status, 0 for success.
//
// PURPOSE:
// Return the floating point value for the keyword
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

int		cImage::keyword
(
    const char	*keyword,	// (in)  The keyword name.
    float	&value		// (out) The keyword value.
)
{
    if ( iFits == NULL )
    {
	return( -1 );
    }
    else
    {
	iFits->get( keyword, value );
	return( 0 );
    }
}
#endif // IMAGE_H
