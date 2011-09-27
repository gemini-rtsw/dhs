static char rcsid[] = "$Id: qlsFrame.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
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
// dhsQlServer/src/qlsFrame.C
//
// PURPOSE:
// Contains the implementation of the cQlsFrame class.
//
// This class manages a FITS file containing the data for a frame.
//
// METHOD NAME(S)
// cQlsFrame::cQlsFrame - Constructor for the cQlsFrame class.
// cQlsFrame::operator< - Comparison of the frame ids.
// cQlsFrame::allocData - Allocate enough memory to contain the image data
//		for a frame.
// cQlsFrame::cardAdd - Add a new FITS card to the FITS header.
// cQlsFrame::dataCopy - Copy the image data for a chunk into a frame.
// cQlsFrame::dataMerge - Merge the data from a FITS HDU into a frame.
// cQlsFrame::headerCopy - Copy the header data from on cQlsFrame structure 
//		to another.
// cQlsFrame::headerMerge - Merge the header part of a HDU into a frame.
// cQlsFrame::modified - Adds a modified region to the frame.
// cQlsFrame::modifiedClear - Clears the list of modified regions.
// cQlsFrame::regionsAdd - Add the list of modified regions to an av list.
// cQlsFrame::resizeHeader - Increase the size allocated to the frame header.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:29:17  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.17  2001/03/27 20:49:16  nhill
// Added locks to the cqlsFrame::modifiedClear method.
//
// Revision 1.16  2000/01/05 18:32:35  nhill
// Changed to use snprintf instead of the IMP ssprintf function.
//
// Revision 1.15  1999/02/01 21:11:34  nhill
// Removed the unused qdfModified flag.
//
// Revision 1.14  1999/01/25 20:21:51  nhill
// Removed debug statement.
//
// Revision 1.13  1999/01/20 23:15:14  nhill
// Removed spaces from data file names.
// Added methods modified, modifiedClear, and regionsAdd to support
// sending a list of the image regions that have changed to the QLT.
//
// Revision 1.12  1998/06/16 22:06:44  nhill
// Changed to allow frame parameters to be set to "null" values until they
// are known.
// Fixed the detection of the "last" frame.
//
// Revision 1.11  1998/05/27 16:34:27  nhill
// Added code to overwrite previously existing header keywords with
// new values.
//
// Revision 1.10  1998/04/27 21:09:16  nhill
// Changed to use separate header and data files.
//
// Revision 1.9  1998/04/15 22:54:45  nhill
// Updated the documentation.
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.8  1997/12/05 00:17:02  nhill
// Added a counter to the file name to help avoid problems when the
// same dataset is sent more than once.
//
// Revision 1.7  1997/11/30 21:28:21  nhill
// Move the headerMerge method into the cQlsFrame class from the cQlsDataset
// class.
// Changed the format of the frame ids.
//
// Revision 1.6  1997/10/30 18:04:40  nhill
// Removed the unused "operator=" function.
// Changed the functions that manipulate the FITS file to open and then close
// the file, instead of leaving the file open (to prevent too many files
// from being open at the same time.
//
// Revision 1.5  1997/10/22 17:44:21  nhill
// Checkpoint check in.
//
// Revision 1.4  1997/10/01 17:24:27  nhill
// Second alpha release.
//
// Revision 1.3  1997/08/28 17:14:41  nhill
// Added the BLANK keyword to the output files.
//
// Revision 1.2  1997/08/27 22:07:13  nhill
// Changed the default axis origins to 1 from 0.
//
// Revision 1.1  1997/08/20 21:25:12  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <sys/stat.h>

#include "fitsio.h"
#define	FITSFILE	fitsfile

#include "globals.H"
#include "qlsDataset.H"
#include "qlsFrame.H"
#include "qlsBufIndex.H"
#include "qlsFitsKeyword.H"
int	cQlsFrame::qfdCounter = 0;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::cQlsFrame
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataset (cQlsDataset) Dataset to which the frame belongs.
// (>) frameId (char *) The frame id.
// (>) extName (char *) The extension name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize a cQlsFrame object
//
// DESCRIPTION:
// - Set up default values for all data members.
// - Create the name of a file to store the frames FITS data.
// - Create the file.
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

		cQlsFrame::cQlsFrame
( 
    cQlsDataset	*dataset, 	// (in)  Dataset to which the frame belongs.
    char	*frameId, 	// (in)  The frame Id.
    char	*extName	// (in)  The extension name.
)
{
    int         i;
    int		fd;
    char	frameFile[PATH_MAX];
    cQlsStatus	status;

    qfdBytePix = 0;
    qfdData = NULL;
    qfdDataset = dataset;
    qfdDataSize = 0;


    //
    // Create file names for the FITS files.
    //

    sprintf( frameFile, "%s:%s.%d.h.fits", dataset->datasetName(), frameId, 
	    qfdCounter );
    for ( i = 0; i < strlen( frameFile ); i++ )
    {
	if ( frameFile[i] == ' ' )
	{
	    frameFile[i] = '_';
	}
    }
    tmpDir.mkPath( qfdHeaderFName, "datasets", frameFile, NULL );

    sprintf( frameFile, "%s:%s.%d.d.fits", dataset->datasetName(), frameId, 
	    qfdCounter );
    for ( i = 0; i < strlen( frameFile ); i++ )
    {
	if ( frameFile[i] == ' ' )
	{
	    frameFile[i] = '_';
	}
    }
    tmpDir.mkPath( qfdDataFName, "datasets", frameFile, NULL );

    qfdCounter = ( qfdCounter + 1 ) % 1000;


    //
    // Create the FITS files.
    //

    checkSystem( fd = creat( qfdHeaderFName, 
	    S_IRUSR | S_IWUSR |S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ),
	   status, VOID );
    checkSystem( close( fd ), status, VOID );
    checkSystem( fd = creat( qfdDataFName, 
	    S_IRUSR | S_IWUSR |S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH ),
	   status, VOID );
    checkSystem( close( fd ), status, VOID );


    qfdFrameId = strsav( frameId );
    qfdHeader = NULL;
    qfdHeaderSize = 0;
    qfdLastIsEnd = false;
    qfdNaxes = 0;
    for ( i = 0; i < 7; i++ )
    {
	qfdNaxis[i] = 0;
    }
    qfdNextCard = 0;
    qfdNumCards = 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::operator<
//
// INVOCATION:
// rel = frame1 < frame2;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) otherFrame (const cQlsFrame &) The comparison frame.
//
// FUNCTION VALUE:
// (bool) true if this frame < frameOrig.
//
// PURPOSE:
// Comparison of the frame ids
//
// The comparison indicates which of the two frames is closes to being
// the best frame to display.
//
// DESCRIPTION:
// - Frames at the second or lower level are always lower priority.
// - If the frames are both at the top level, the higher numbered frame
//   should be displayed.
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

bool		cQlsFrame::operator<
(
    const cQlsFrame		// (in)  The comparison frame.
		&otherFrame
) const
{
    int		thisFirstLevel;
    int		thisNumCon;
    int		thisSecondLevel;
    int		otherFirstLevel;
    int		otherNumCon;
    int		otherSecondLevel;

    thisNumCon = sscanf( frameId(), "%d.%d", &thisFirstLevel, 
	    &thisSecondLevel );
    otherNumCon = sscanf( otherFrame.frameId(), "%d.%d", &otherFirstLevel, 
	    &otherSecondLevel );

    if ( thisNumCon != 1 )
    {
	return( true );
    }
    else if ( otherNumCon != 1 )
    {
	return( false );
    }
    else
    {
	return( thisFirstLevel < otherFirstLevel );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::allocData
//
// INVOCATION:
// fitsData.allocData( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Allocate enough space for the data buffer at the end of the file.
//
// DESCRIPTION:
// - Calculate the size of the image array.
// - Round the size up to the next complete fits block.
// - Increase the size of the disk file.
// - Re-map the disk file.
// - Zero the data.
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

void		cQlsFrame::allocData
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    int		fd;		// File descriptor for the mapped file.
    int		i;
    size_t	oldSize;	// The size of the existing FITS file.


    checkStat( status, return );


    //
    // Calculate the size of the data buffer.
    //

    oldSize = qfdDataSize;
    qfdDataSize = qfdBytePix;
    for ( i = 0; i < qfdNaxes; i++ )
    {
	qfdDataSize *= qfdNaxis[i];
    }


    //
    // round up to the nearest FITS block.
    //

    if ( ( qfdDataSize % 2880 ) != 0 )
    {
	qfdDataSize += 2880 - ( qfdDataSize % 2880 );
    }


    //
    // Open the file.
    //

    checkSystem( fd = open( qfdDataFName, O_RDWR ), status, VOID );


    //
    // Increase the size of the memory mapped file.
    //

    checkSystem( lseek( fd, qfdDataSize - 1, SEEK_SET ), 
	    status, close( fd ); return );
    checkSystem( write( fd, " ", 1 ), status, close( fd ); return );


    //
    // re-map the FITS buffers.
    //

    if ( qfdData != NULL )
    {
	checkSystem( munmap( qfdData, oldSize ), status, return );
	qfdData = NULL;
    }
    if ( ( qfdData = (char*)mmap( NULL, qfdDataSize, PROT_WRITE | PROT_READ, 
	    MAP_SHARED, fd, 0 ) ) == MAP_FAILED )
    {
	status.sysErrno();
	checkSystem( close( fd ), status, return );
	return;
    }
    checkSystem( close( fd ), status, return );


    //
    // Zero the data array.
    //

    memset( qfdData, 0, qfdDataSize );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::cardAdd
//
// INVOCATION:
// header.cardAdd( card, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) card  (char *) The card image to add to the header.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a new card onto the FITS header
//
// DESCRIPTION:
// - If the card overfills the header, increase the size of the header.
// - Copy the Card into the header.
// - Ensure that any existing "END" card is overwritten by new cards. It
//   is assumed that the last card from any chunk will be another END card.
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

void		cQlsFrame::cardAdd
(
    char	*card,		// (in)  The card image to add to the header.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    char	*pChar;		// Generic character pointer.
    iKeywordList iKeyword;
    char	keyword[9];


    checkStat( status, return );


    //
    // If the keyword is not HISTORY, COMMENT, or END, and if the keyword
    // is already in the header, overwrite the previous version of the
    // keyword.
    //


    //
    // Remove trailing NULLs and change them back to spaces.
    //

    for ( pChar = card + 79; pChar >= card && *pChar == '\0' ; pChar-- )
    {
	*pChar = ' ';
    }


    strncpy( keyword, card, 8 );
    keyword[8] = '\0';
    if ( strncmp( card, "HISTORY ", 8 ) == 0 || 
	    strncmp( card, "COMMENT ", 8 ) == 0 || 
	    strncmp( card, "END     ", 8 ) == 0 || 
	    ( iKeyword = qfKeywordList.find( keyword ) ) == 
	    qfKeywordList.end() )
    {
	//
	// If the header buffer is full, Extend the header buffer.
	//

	if ( qfdNextCard >= qfdHeaderSize )
	{
	    check( resizeHeader( 1, status ), status, return );
	}


	//
	// Copy the card into the header.
	//

	memcpy( qfdHeader + qfdNextCard, card, 80 );
	qfKeywordList[ keyword ] = qfdNumCards;


	if ( ! qfdLastIsEnd )
	{
	    //
	    // Increment the number of cards in the header. This is only done
	    // if the previous card in the header was not an END card, since
	    // an end card would be overwritten, and so not change the number
	    // of cards in the header.
	    //

	    qfdNumCards++;
	}


	if ( strncmp( card, "END     ", 8 ) == 0 )
	{
	    //
	    // Card was an END card, so don't increment the pointers. This
	    // results in the END card being overwritten by the next card 
	    // inserted.
	    //

	    qfdLastIsEnd = true;
	}
	else
	{
	    //
	    // Increment the pointer which indicates where to put the next card.
	    //

	    qfdNextCard += 80;
	    qfdLastIsEnd = false;
	}
    }
    else
    {
	cardAdd( card, (*iKeyword).second, status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::cardAdd
//
// INVOCATION:
// header.cardAdd( card, cardNum, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) card  (char *) The card image to add to the header.
// (>) cardNum  (int) The card number to replace (0 is the first card).
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Replace an existing card in a FITS header
//
// The first card is card 0.
//
// DESCRIPTION:
// - Copy the Card into the header.
// - If the last card was overwritten, ensure the last is end flag is
//   updated correctly.
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

void		cQlsFrame::cardAdd
(
    char	*card,		// (in)  The card image to add to the header.
    int		cardNum,	// (in)  The card number to replace.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    char	*pChar;		// Generic character pointer.


    checkStat( status, return );


    //
    // Ensure that the card number is one that has already been written.
    //

    if ( cardNum < 0 || cardNum >= qfdNumCards )
    {
	status.E_CARDNUM( status, cardNum, frameId(), dataset().datasetName() );
	return;
    }


    //
    // Remove trailing NULLs and change them back to spaces.
    //

    for ( pChar = card + 79; pChar >= card && *pChar == '\0' ; pChar-- )
    {
	*pChar = ' ';
    }


    //
    // Copy the card into the header.
    //

    memcpy( qfdHeader + cardNum * 80, card, 80 );


    if ( cardNum == qfdNumCards - 1 )
    {
	if ( strncmp( card, "END     ", 8 ) == 0 )
	{
	    qfdLastIsEnd = true;
	}
	else
	{
	    qfdLastIsEnd = false;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::dataCopy
//
// INVOCATION:
// fitsData.dataCopy( fptr, naxis, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fptr (fitsFile *) Pointer to the fits file structure.
// (>) naxis (long []) Array of axis sizes.
// (>) origin (long []) Origin of the chunk in the complete image.
// (>) pBuffer (void *) Pointer to the buffer containing the data.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy the data from the fits file into the data buffer
//
// DESCRIPTION:
// - Use an index of type cQlsBufIndex to calculate the address of each row
//   of pixels in the output buffer.
// - Copy each row of pixels from the chunk data buffer into the output buffer.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// This could be made more efficient by checking to see if more than one row
// can be copied at the same time.
// This function has never been tested with > 2d data.
// This function may significantly benifit from careful optimization.
//-
//***********************************************************************
//

void		cQlsFrame::dataCopy
(
    fitsfile	*fptr,		// (in)  Pointer to the fits file structure.
    long	naxis[],	// (in)  Image chunk size.
    long	origin[],	// (in)  Image chunk orign.
    void	*pBuffer,	// (in)  Pointer to the buffer containing the 
    				//	 data.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    long	hduAdr;		// The offset of the HDU in the data buffer.
    long	nextHduAdr;	// Offset of the next HDU. Not used.
    int		keysExist;	// Number of keywords in the HDU.
    int		moreKeys;	// Number of empty keywords in the HDU.
    int		cfStatus( 0 );	// cfitsio function return status.
    cQlsBufIndex		// Index object to calculate output addresses.
		index( naxis, origin, this );
    char	*inAddr;	// The input address of a row of pixels.
    ulong	dataStart;	// Calcualted starting point of data in a HDU.
    long	dataEnd;	// Calcualted end point of data in a HDU.
    size_t	bufSize;	// The number of bytes in a row of pixels.


    checkStat( status, return );


    //
    // Calculate the starting point of the data buffer based in information
    // supplied by the cfitsio library.
    //

#if defined(CFITSIO_OUTDATED)
    fits_get_hduaddr( fptr, &hduAdr, &nextHduAdr );
#else
    fits_get_hduaddr( fptr, &hduAdr, &nextHduAdr, &dataEnd, &cfStatus );
#endif   
    
    fits_get_hdrspace( fptr, &keysExist, &moreKeys, &cfStatus );
    dataStart = hduAdr + 2880 * ( ( moreKeys + keysExist + 35 ) / 36 );


    //
    // Copy each row of pixles from the input data.
    //

    bufSize = index.bufSize();
    for( index.start(), inAddr = (char *) pBuffer + dataStart; 
	    !index.end() && cfStatus == 0; index ++, inAddr += bufSize )
    {
	//
	// Make sure the data from the input buffer won't extend past the
	// end of the output data buffer.
	//

	if ( index.outAddr() + bufSize - qfdData > qfdDataSize )
	{
	    status.E_BUF_OVER( status, frameId() );
	    return;
	}


	//
	// Copy the row of pixels.
	//

	memcpy( index.outAddr(), inAddr, bufSize );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::dataMerge
//
// INVOCATION:
// dataset.dataMerge( fptr, true, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fptr (fitsfile *) Fits file pointer.
// (>) isChunk (bool) Is the data a chunk?
// (>) pBuffer (void *) Pointer to the buffer containing the data.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add the data part of a FITS HDU to a frame
//
// DESCRIPTION:
// - Write appropriate NAXIS and BITPIX keywords to the frame header.
// - use the cQlsFrame::dataCopy function to copy the data to the frame.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The cfitio file pointer must be on the correct HDU.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsFrame::dataMerge
(
    fitsfile	*fptr,		// (in)  Fits file pointer.
    bool	isChunk,	// (in)  Is the data a chunk.
    void	*pBuffer,	// (in)  Pointer to the buffer containing the 
    				//	 data.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    long	bitpix;		// Value read from the BITPIX card.
    char	card[81];	// Source of a FITS keyword card.
    int		cfStatus( 0 );	// cfitsio status value.
    bool	error;
    int		imageDims;	// Dimentions of the image.
    long	imageSize[7];	// Size of the chunk.
    int		i;
    cQlsDataset::iFrameList	iFrame;		// Iterator in the frame list.
    int		naxes;		// Number of axes in the data.
    long	naxis[7];	// The size of each axis.
    int		nfound;
    long	origin[7];	// Origin of the chunk.


    checkStat( status, return );


    //
    // Get the number of axes.
    //

    checkFitsio( fits_read_keys_lng( fptr, "NAXIS", 1, 7, naxis, 
	    &naxes, &cfStatus ), cfStatus, status, VOID );

    if ( cfStatus == KEY_NO_EXIST )
    {
	naxes = 0;
	cfStatus = 0;
    }
    else if ( cfStatus != 0 )
    {
	//
	// An error occured, display a message.
	//

	checkFitsioNX( fits_read_keys_lng( fptr, "NAXIS", 1, 7, naxis, 
		&naxes, &cfStatus ), cfStatus, status, return );
    }

    for ( i = 0; i < naxes; i++ )
    {
	if ( naxis[i] == 0 )
	{
	    naxes = 0;
	    break;
	}
    }


    //
    // Ensure it is a 2d image.
    //

    if ( naxes != 2 && naxes != 0 )
    {
	status.E_NAXIS( status, naxes, dataset().datasetName() );
	return;
    }


    //
    // Read the image data from the header.
    //

    if ( isChunk )
    {
	checkFitsio( fits_read_keys_lng( fptr, "AXISSZ", 1, 7, imageSize, 
		&imageDims, &cfStatus ), cfStatus, status, VOID );
	if ( cfStatus == KEY_NO_EXIST )
	{
	    imageDims = 0;
	    cfStatus = 0;
	}
	else if ( cfStatus != 0 )
	{
	    checkFitsioNX( fits_read_keys_lng( fptr, "AXISSZ", 1, 7, 
		    imageSize, &imageDims, &cfStatus ), 
		    cfStatus, status, return );
	}
    }
    else
    {
	imageDims = naxes;
	for ( i = 0; i < naxes; i++ )
	{
	    imageSize[i] = naxis[i];
	}
    }


    if ( qfdNaxes == 0 && imageDims != 0 )
    {
	qfdNaxes = imageDims;
	(void) sprintf( card, "NAXIS   = %10d%60s", qfdNaxes, "" );
	check( cardAdd( card, 2, status ), status, VOID );
	for ( i = 0; i < imageDims; i++ )
	{
	    qfdNaxis[i] = imageSize[i];
	    (void) sprintf( card, "NAXIS%-3d= %10d%60s", i + 1, 
		    qfdNaxis[i], "" );
	    check( cardAdd( card, 3 + i, status ), status, VOID );
	}
    }


    //
    // Get the size of each data element (BIPIX).
    //

    checkFitsio( fits_read_key( fptr, TLONG, "BITPIX", &bitpix, NULL, 
	    &cfStatus ), cfStatus, status, VOID );

    if ( qfdBytePix == 0 && naxes != 0 )
    {
	qfdBytePix = abs( bitpix / 8 );
	(void) sprintf( card, "BITPIX  = %10d%60s", bitpix, "" );
	check( cardAdd( card, 1, status ), status, VOID );
    }


    if ( qfdDataSize == 0 && qfdBytePix != 0 && qfdNaxes != 0 )
    {
	//
	// Put the BLANK, OBJECT and END cards into the frame header.
	//

	(void) sprintf( card, "BLANK   = %10d%60s", 0, "" );
	check( cardAdd( card, status ), status, VOID );
	(void) snprintf( card, 80, "OBJECT  = '%s:%s'%60s", 
		dataset().datasetName(), frameId(), "" );
	check( cardAdd( card, status ), status, VOID );
	(void) sprintf( card, "END%77s", "" );
	check( cardAdd( card, status ), status, VOID );


	//
	// Allocate a data array big enough for the complete image.
	//

	allocData( status );
    }


    //
    // Ensure the chunk has the image parameters as the first chunk.
    //

    if ( imageDims != 0 )
    {
	error = false;
	if ( qfdNaxes == imageDims )
	{
	    for ( i = 0; i < imageDims; i++ )
	    {
		if ( qfdNaxis[i] != imageSize[i] )
		{
		    fprintf(stderr, "XXX: cQlsFrame::dataMerge: qfdNaxis[i] != imageSize[i]: %d != %d\n", qfdNaxis[i], imageSize[i]);
		    error = true;
		    break;
		}
	    }
	}
	else
	{
	    fprintf(stderr, "XXX: cQlsFrame::dataMerge: qfdNaxes != imageDims: %d != %d\n", qfdNaxes, imageDims);
	    error = true;
	}

	if ( naxes != 0 && qfdBytePix != abs( bitpix ) / 8 )
	{
	    fprintf(stderr, "XXX: cQlsFrame::dataMerge: naxes != 0 && qfdBytePix != abs( bitpix ) / 8:  %d != 0 && %d != abs(%d)/8\n", naxes, qfdBytePix, bitpix);
	    error = true;
	}


	if ( error )
	{
	    status.E_CHUNK_MATCH( status, dataset().datasetName() );
	    return;
	}
    }


    if ( naxes != 0 )
    {
	//
	// Get the size and location of the data chunk.
	//

	checkFitsio( fits_read_keys_lng( fptr, "ORIGINS", 1, qfdNaxes, origin, 
		&nfound, &cfStatus ), cfStatus, status, VOID );


	//
	// If no ORIGINS keyword was found, set the default origins to 0.
	//

	if ( nfound == 0 )
	{
	    for ( i = 0; i < qfdNaxes; i++ )
	    {
		origin[i] = 1;
	    }
	}


	//
	// Copy the data into the buffer.
	//
	
	dataCopy( fptr, naxis, origin, pBuffer, status );


	//
	// Set the modified flag to ensure interested Quick Look Clients 
	// will be notified of the change.
	//

	modified( origin, naxis, status );


	//
	// If the frame is "newer" than the current "lastFrame", 
	// replace the last frame with the new frame.
	//

	if ( &(qfdDataset->lastFrame()) == NULL || 
	     ! (*this < qfdDataset->lastFrame()) ) // XXX allan: replaced ">" with "!<"
	{
	    qfdDataset->lastFrame( *this );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::headerCopy
//
// INVOCATION:
// fitsData.headerCopy( sourceData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) sourceData (cQlsFrame &) Source fits data structure.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy the header data from one cQlsFrame structure into another
//
// DESCRIPTION:
// - Increase the size of the output header enough to contain the input header.
// - Copy the source header to the target header.
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

void		cQlsFrame::headerCopy
(
    cQlsFrame	&sourceData,	// (in)  Source fits data structure.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    int		increment;	// Number of block to increase header by.


    checkStat( status, return );


    //
    // Calculate the number of cards in the merged header.
    //

    qfdNumCards += sourceData.qfdNumCards;
    if ( qfdLastIsEnd )
    {
	qfdNumCards--;
    }


    //
    // Calculate the required increment in the header size.
    //

    increment = ( ( qfdNumCards * 80 + 2879 ) / 2880 ) - 
	    ( qfdHeaderSize / 2880 );


    //
    // Resize the target header.
    //

    if ( increment > 0 )
    {
	check( resizeHeader( increment, status ), status, return );
    }


    //
    // Copy the source header to the target header.
    //

    memcpy( qfdHeader + qfdNextCard, sourceData.qfdHeader, 
	    sourceData.qfdNumCards * 80 );


    //
    // Set up the data members describing how new cards should be added.
    //

    qfdLastIsEnd = sourceData.qfdLastIsEnd;
    if ( qfdLastIsEnd )
    {
	qfdNextCard = ( qfdNumCards - 1 ) * 80;
    }
    else
    {
	qfdNextCard = qfdNumCards * 80;
    }

    qfKeywordList = sourceData.qfKeywordList;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::headerMerge
//
// INVOCATION:
// frame.headerMerge( fptr, true, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fptr (fitsfile *) Fits file pointer to open FITS file.
// (>) isMain (bool) Is this the main HDU?
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Merge the header part of a HDU into a frame
//
// DESCRIPTION:
// - Loop through the cards in the HDU, and determine if they should be
//   ignored or added to the frame.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// - Should be called with the frame mutex write locked to ensure two threads
//   don't modify the same frame.
// - Should be called with the cfitsio mutex locked to ensure two threads
//   don't use the cfitsio library.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsFrame::headerMerge
(
    fitsfile	*fptr,		// (in)  The fits file to read from.
    bool	isMain,		// (in)  Is this the main HDU?
    cQlsStatus	&status		// (mod) function return status.
)
{
    char	card[81];	// A FITS card image.
    int		cfStatus( 0 );	// cfitsio function return status.
    int		i;
    char	kwdName[9];	// Fits keyword name.
    cQlsFitsKeyword		// A fits keyword object.
    		*pKwd;
    char	value[81];	// Value field from the card.


    checkStat( status, return );


    //
    // Process every FITS card in the HDU.
    //

    for ( i = 1; cfStatus == 0; i++ )
    {
	//
	//  Read the next card.
	//

	fits_read_record( fptr, i, card, &cfStatus );


	//
	// Check for the end of the header.
	//

	if ( cfStatus == KEY_OUT_BOUNDS )
	{
	    cfStatus = 0;
	    fits_clear_errmsg();
	    break;
	}


	//
	// If an error occured, print a message.
	//

	checkFitsioNX( fits_read_record( fptr, i, card, &cfStatus ), 
		cfStatus, status, break );


	//
	// Get the FITS keyword name.
	//

	checkFitsio( fits_read_keyn( fptr, i, kwdName, value, NULL, &cfStatus ),
		cfStatus, status, break );


	//
	// Find the control information for the keyword.
	//

	check( pKwd = cQlsFitsKeyword::find( kwdName, isMain, status ),
		status, break );


	if ( pKwd->isRequired( dataset().qdsFirst ) )
	{
	    if ( isMain )
	    {
		//
		// This card is in the main HDU, so add the card to all 
		// existing frames.
		//


		dataset().cardAdd( card, status );
	    }
	    else
	    {
		//
		// Add to only the current frame.
		//

		check( cardAdd( card, status ), status, break );


#ifdef OMIT	
		//
		// Indicate the frame has been modified.
		//

		if ( qfdDataSize != 0 )
		{
		    modified( true ); 
		}
#endif // OMIT
	    }
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::modified
//
// INVOCATION:
// frame.modified( origin, size )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) origin (long[]) The origin of the modified region.
// (>) size (long[]) The size of the modified region.
// (!) status (cQlsStatus &) The function return status.
//
// FUNCTION VALUE:
// (bool) true, indicating the region has been modified.
//
// PURPOSE:
// Adds a modified region to the frame
//
// DESCRIPTION:
// - Create a list of 4 longs containing the start point and size of
// the region.
// - Add the region to the qfdModifiedRegions list.
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

bool		cQlsFrame::modified
(
    long	origin[],	// (in)  The origin of the modified region.
    long	size[],		// (in)  This size of the modified region.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    int		i;
    long	*region;	// This is the modified region, in the form
    				// X origin, X size, Y origin, Y size,
				// Z origin, ...


    checkStat( status, return( modified() ) );


    checkNull( region = new (long [qfdNaxes*2]), status, 
	    return( modified() ) );
    for ( i = 0; i < qfdNaxes; i++ )
    {
	region[i*2] = origin[i];
	region[i*2+1] = size[i];
    }

    qfdModifiedRegions.push_back( region );

    return( modified() );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::modifiedClear
//
// INVOCATION:
// frame.modifiedClear( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Clears all modified regions from the qfdModifiedRegions vector
//
// DESCRIPTION:
// Pop each region from the qfdModifiedRegions list, and free the memory
// allocated to the region.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// cQlsFrame::modified( long *, long *, cQlsStatus & )
// cQlsFrame::modified()
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsFrame::modifiedClear
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    long	*region;


    checkStat( status, return );


    wLock();
    while( qfdModifiedRegions.size() > 0 )
    {
	region = qfdModifiedRegions.back();
	qfdModifiedRegions.pop_back();
	delete [] region;
    }
    unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::regionsAdd
//
// INVOCATION:
// frame.regionsAdd( avList, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) avList (cDhsAvList &) The attribute value list to which the regions
//		attribute should be added.
// (!) status (cQlsStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method adds a "regions" attribute to the avList
//
// DESCRIPTION:
// - allocate an array of strings for the region description strings.
// - allocate a string for each region, and format the region information
//   into it.
// - Add the string to the AV list.
// - delete the memory allocated to create the list.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// The regions are being passed as TCL lists. It would be better to pass
// the list of regions as a two dimensional array, but this isn't supported
// by OCSWish.
//
//-
//***********************************************************************
//

void		cQlsFrame::regionsAdd
(
    cDhsAvList	&avList,	// (mod) The avList to received the new 
    				//	 attribute.
    cQlsStatus	&status		// (mod) The function return status.
)
{
    char	**array;	// This is the array of character strings
    				// that will be used to contain the region
				// descriptions.
    ulong	dims[7];
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    std::vector<long *>::iterator
    		i;
    int		j;
    long	*region;	// The region being formatted.
    char	string[60];


    //
    // Create an array of character pointers to be passed to the add method.
    //

    dims[0] = qfdModifiedRegions.size();
    if ( dims[0] == 1 )
    {
	region = qfdModifiedRegions.front();
	sprintf( string, "{ %d %d %d %d }", 
		region[0], region[1], region[2], region[3] );
	checkDhs( avList.add( "regions", DHS_DT_STRING, 0, 0, string, 
		dhsStatus ), dhsStatus, status, VOID );
    }
    else
    {
	array = new char* [dims[0]];


	//
	// Add a string to the array for each region.
	//

	for ( j = 0, i = qfdModifiedRegions.begin(); 
		i != qfdModifiedRegions.end(); j++, i++ )
	{
	    //
	    // Get the region data from the i'th position in the vector.
	    //

	    region = *i;


	    //
	    // Allocate a new string for the region.
	    //

	    array[j] = new char[60];


	    //
	    // Format a string containing the region information.
	    //
	    
	    sprintf( array[j], "{ %d %d %d %d }", 
		    region[0], region[1], region[2], region[3] );
	}


	//
	// Add the array of strings to the av list.
	//

	checkDhs( avList.add( "regions", DHS_DT_STRING, 1, dims, array, 
		dhsStatus ), dhsStatus, status, VOID );


	//
	// Delete the memory allocated above.
	//

	for ( j = 0; j < dims[0]; j++ )
	{
	    delete [] array[j];
	}
	delete [] array;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::resizeHeader
//
// INVOCATION:
// fitsData.resizeHeader( increment, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) increment (int) Size to increase the header by.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Increase the amount of space available for header records
//
// DESCRIPTION:
// - Increase the size of the fits file.
// - Re-map the fits buffer.
// - Copy the image data to it's new location.
// - Fill the new header area with spaces.
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

void		cQlsFrame::resizeHeader
(
    int		increment,	// (in)  Size to increate the header by.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    int		fd;		// Map file file descriptor.
    size_t	oldSize;	// Size of the original buffer.


    checkStat( status, return );


    if ( increment == 0 )
    {
	return;
    }

    oldSize = qfdHeaderSize;


    //
    // Open the file.
    //

    checkSystem( fd = open( qfdHeaderFName, O_RDWR ), status, VOID );


    //
    // Increase the size of the memory mapped file.
    //

    checkSystem( lseek( fd, qfdHeaderSize + increment * 2880 - 1, SEEK_SET ), 
	    status, close( fd ); return );
    checkSystem( write( fd, " ", 1 ), status, close( fd ); return );


    //
    // re-map the FITS buffers.
    //

    if ( qfdHeader != NULL )
    {
	checkSystem( munmap( qfdHeader, oldSize ), status, return );
	qfdHeader = NULL;
    }


    if ( ( qfdHeader = (char*)mmap( NULL, qfdHeaderSize + increment * 2880, 
	    PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0 ) ) == 
	    MAP_FAILED )
    {
	status.sysErrno();
	checkSystem( close( fd ), status, return );
	return;
    }


    checkSystem( close( fd ), status, return );


    qfdHeaderSize += increment * 2880;


    //
    // Fill the new header area with spaces.
    //

    memset( qfdHeader + qfdNextCard, ' ', qfdHeaderSize - qfdNextCard );
}
