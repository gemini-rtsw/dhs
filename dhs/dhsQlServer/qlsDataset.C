static char rcsid[] = "$Id: qlsDataset.C,v 1.3 2004-08-24 13:57:13 brighton Exp $";
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
// dhsQlServer/src/qlsDataset.C
//
// PURPOSE:
// Contains the implementation of the cQlsDataset class.
//
// METHOD NAME(S)
// cQlsDataset::cQlsDataset - Constructor for the class.
// cQlsDataset::~cQlsDataset - Destructor for the class.
// cQlsDataset::cardAdd - Add a card to every frame in a dataset.
// cQlsDataset::chunkAdd - Add an image chunk onto a dataset.
// cQlsDataset::chunkMerge - Merge a fits buffer into a dataset.
// cQlsDataset::forEachFrame - Execute a function for each frame in the dataset.
// cQlsDataset::frameFind - Find or create a frame for a given FITS HDU.
// cQlsDataset::hasStream - Return true if the dataset has the specified
//		stream.
// cQlsDataset::modifiedClear - Clear the modified flag of a frame.
// cQlsDataset::setQlStream - Add a list of Ql streams to the dataset.
// cQlsDataset::streamAdd - Add all streams to an av list.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/27 17:15:08  brighton
// ported to gcc-3.2.1/linux
//
// Revision 1.1.1.1  2002/11/24 20:29:06  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.14  1999/01/20 23:02:58  nhill
// Changed to not notify QLT's if only header data has changed.
//
// Revision 1.13  1998/08/04 23:15:25  nhill
// Added a message indicating a new dataset has been created.
//
// Revision 1.12  1998/06/16 22:04:06  nhill
// Changed the format of the frame id string.
//
// Revision 1.11  1998/05/14 20:25:50  nhill
// Changed to handle the new extension keywords.
//
// Revision 1.10  1998/05/11 16:25:23  nhill
// Changed to allow chunks containing no data.
//
// Revision 1.9  1998/04/15 22:41:44  nhill
// Updated the documentation.
// Changed to the newest version of the cDhsStatus class.
//
// Revision 1.8  1997/11/30 21:24:29  nhill
// Changed to use the cListOf class to keep track of the list of all
// cQlsDataset objects.
//
// Revision 1.7  1997/10/30 18:01:58  nhill
// Fixed a typo.
//
// Revision 1.6  1997/10/22 17:42:51  nhill
// Checkpoint check in.
//
// Revision 1.5  1997/10/03 23:06:09  nhill
// Removed debug messages.
//
// Revision 1.4  1997/10/01 17:23:06  nhill
// Second alpha release.
//
// Revision 1.3  1997/08/26 21:51:36  nhill
// Added a check to insure only frames with data are considered to be
// modified.
//
// Revision 1.2  1997/08/20 21:23:15  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <cstring>
#include <sys/types.h>
#include <unistd.h>

#include "fitsio.h"
#define	FITSFILE	fitsfile

#define	DHS_NO_GET
#define	DHS_NO_CLIENT
#include "dhs++.H"

#include "globals.H"
#include "qlsDataset.H"
#include "qlsStream.H"
#include "qlsQlt.H"
#include "qlsSlave.H"


cMutex                cQlsDataset::qdsNumWaitsLock;


//
// Create a mutex to protect the cfitsio library.
//

static cMutex	fitsMutex;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::cQlsDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (string) Name of the new dataset.
// (>) addToList (bool) True if the ds should be added to the list of datasets.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cQlsDataset class
//
// DESCRIPTION:
// - Initalize the data members.
// - Add the new datsaet to the dataset list.
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

		cQlsDataset::cQlsDataset
(
    std::string	name,		// (in)  Name of the new dataset.
    bool	addToList	// (in)  True if the ds should be added to the
    				//	 list of datasets.
): cListOf<cQlsDataset, std::string, cQlsStatus >( name, addToList )
{
    cQlsFrame	*pFrame;
    char	card[81];
    cQlsStatus	status;
    int		i;


    //
    // Initialize the data members.
    //

    qdsFirst = true;
    qdsIsChunk = false;
    qdsInList = addToList;
    qdsLast = false;
    qdsPLastFrame = NULL;
    qdsName = (char *) char_alloc( name.size() + 1);
    for ( i = 0; i < name.size(); i++ )
    {
	qdsName[i] = name[i];
    }
    qdsName[i] = '\0';
    qdsNotifyTime = time( NULL );
    qdsNumWaits = 0;
    qdsRefCount = 1;


    //
    // Create a main HDU frame object for the dataset.
    //

    pFrame = new cQlsFrame( this, "Main HDU", "Main HDU" );
    qdsFrameList[ "Main HDU" ] = pFrame;

    qdsPMainFrame = pFrame;


    //
    // Add the simple card.
    //

    (void) sprintf( card, "SIMPLE  = %10s%60s", "T", "" );
    pFrame->cardAdd( card, status );


    //
    // Reserve space for the naxis, bitpix and object keywords.
    //

    (void) sprintf( card, "COMMENT   %10s%60s", "", "" );
    for ( i = 0; i < 10; i++ )
    {
	pFrame->cardAdd( card, status );
    }


    status.S_DATASET( status, qdsName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::~cQlsDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for the cQlsDataset class
//
// DESCRIPTION:
// - Remove the dataset from the list of datasets.
// - Free resources allocated to the object.
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

		cQlsDataset::~cQlsDataset
(
    void
)
{
    iFrameList	iFrame;
    cQlsStream::iStreamList	iStream;



    for ( iStream = qdsStreamList.begin(); iStream != qdsStreamList.end(); 
	    iStream++ )
    {
	(*iStream).second->deRef();
    }


    //
    // Free resources allocated to the object.
    //

    for ( iFrame = qdsFrameList.begin(); iFrame != qdsFrameList.end(); 
	    iFrame++ )
    {
	delete (*iFrame).second;
    }

    gen_free( qdsName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::cardAdd
//
// INVOCATION:
// dataset.cardAdd( card, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) card  (char *) The card image to add to the header.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a new card onto the FITS headers of every frame in a dataset
//
// DESCRIPTION:
// Loop through all of the frames in the dataset, adding the card to each.
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

void		cQlsDataset::cardAdd
(
    char	*card,		// (in)  The card image to add to the header.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    iFrameList	iFrame;		// Iterator into the frame list.
    cQlsFrame	*pFrame;	// Pointer to a frame object.


    for ( iFrame = qdsFrameList.begin(); 
	    status.ok() && iFrame != qdsFrameList.end(); iFrame++ )
    {
	pFrame = (*iFrame).second;

	if ( pFrame != qdsPMainFrame )
	{
	    pFrame->wLock();
	}

	pFrame->cardAdd( card, status );


	if ( pFrame != qdsPMainFrame )
	{
	    pFrame->unlock();
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::chunkAdd
//
// INVOCATION:
// dataset.chunkAdd( pBuffer, length, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pPutRequest (cDhsPutRequest *) Pointer to the put request.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add an image chunk in a put request onto a datset and notify everthing
// that should be notified
//
// DESCRIPTION:
// - Use the chunkMerge function to merge the chunk into the dataset.
// - Notify any interested Quick Look Tools.
// - Forward the data to any interested slave Quick Look Servers.
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

void		cQlsDataset::chunkAdd
(
    cDhsPutRequest	
		*pPutRequest,	// (in)  Pointer to the put request.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cQlsSlave::tCfArg		// Argument to pass to the cQlsSlave::foreach
		arg;		// function.
    int		nw;		// Number of waiting threads.


    checkStat( status, return );


    if ( pPutRequest->length() > 0 )
    {
	//
	// Increment the number of threads processing the dataset.
	//

	qdsNumWaitsLock.lock();
	qdsNumWaits ++;
	qdsNumWaitsLock.unlock();


	//
	// Merge the chunk into the datset.
	//

	chunkMerge( pPutRequest->buffer(), pPutRequest->length(), status );


	//
	// Decrement the number of threads processing the dataset.
	//

	qdsNumWaitsLock.lock();
	qdsNumWaits --;
	qdsNumWaitsLock.unlock();


	qdsNumWaitsLock.lock();
	nw = qdsNumWaits;
	qdsNumWaitsLock.unlock();


	//
	// If there are no other threads waiting to merge chunks into the 
	// dataset, or if it is time to do another update of the displays, 
	// update the quick look displays.
	//

	if ( nw == 0 || abs( qdsNotifyTime - time( NULL ) ) > 3 )
	{
	    //
	    // Notify QLT's that the datset has changed.
	    //

	    cQlsQlt::forEach ( &cQlsQlt::checkNotify, this, status );
	    qdsNotifyTime = time( NULL );


	    //
	    // Change the modified status of each frame to false since all 
	    // quick look tools have been notified.
	    //

	    forEachFrame( modifiedClear, NULL, status );
	}


	//
	// Forward the data to all interested QLS slaves.
	//

	arg.first = this;
	arg.second = pPutRequest;
	cQlsSlave::forEach( &cQlsSlave::checkForward, &arg, status );
    }


    //
    // If this is the last chunk, delete the dataset.  A reference count
    // is kept to ensure no other thread is using the dataset. This will
    // probably never happen in real life, but it does happen while
    // testing, when the last flag is set for every chunk.
    //

    if ( pPutRequest->last() && ! last() )
    {
	last( true );
	deRef();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::chunkMerge
//
// INVOCATION:
// dataset.chunkMerge( pBuffer, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pBuffer (void *) FITS buffer to merge into the dataset.
// (>) bufLen (int) Size of the FITS buffer.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Merge a FITS buffer into the dataset
//
// DESCRIPTION:
// - Figure out if the data is a chunk of a dataset.
// - Merge the header and data for each HDU into the individual frames.
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

void		cQlsDataset::chunkMerge
(
    void	*pBuffer,	// (in)  FITS buffer to merge into the dataset.
    size_t	bufLen,		// (in)  Size of the FITS buffer.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    int		cfStatus( 0 );	// cfitsio status value.
    fitsfile	*fptr;		// cfitsio file pointer.
    int	        hasExtend;	// True if the extend keyword is true. (XXX allan: changed from bool, see cfitsio call)
    int		hduType;	// Type of the HDU read.
    int		i;
    int		isAChunk;	// True if the dataset is a chunk.
    bool	isMain;		// True when processing the main HDU.
    cQlsFrame	*pFrame;	// The frame the HDU is being added to.


    checkStat( status, return );


    //
    // Open the FITS buffer.  The mutex protecting the cfitsio library
    // is lock for the entire process. This is a possible source of
    // performance improvment.
    //

    fitsMutex.lock();

#if defined(CFITSIO_OUTDATED)
    checkFitsio( fits_set_mem_buff( &fptr, &pBuffer, &bufLen, 0, NULL, 
	    &cfStatus ), cfStatus, status, fitsMutex.unlock(); return );
    checkFitsio( fits_open_file( &fptr, "", READONLY, &cfStatus ), 
	    cfStatus, status, fitsMutex.unlock(); return );
#else
   checkFitsio( ffomem( &fptr, "", READONLY, &pBuffer, (size_t*)&bufLen, 2880, realloc, &cfStatus ), 
      cfStatus, status, fitsMutex.unlock(); return );
#endif   

    fits_read_key( fptr, TLOGICAL, "EXTEND", &hasExtend, NULL, &cfStatus );
    if ( cfStatus == KEY_NO_EXIST )
    {
	hasExtend = false;
	cfStatus = 0;
    }
    else
    {
	//
	// An error occured, print a message.
	//

	checkFitsioNX( fits_read_key( fptr, TLOGICAL, "EXTEND", &hasExtend, 
		NULL, &cfStatus ), cfStatus, status, VOID );
    }


    //
    // Figure out if the FITS file contains a chunk or not.
    //

    if ( qdsFirst )
    {
	fits_read_key( fptr, TLOGICAL, "CHUNK", &isAChunk, NULL, &cfStatus );
	if ( cfStatus == 0 && isAChunk )
	{
	    qdsIsChunk = true;
	}
	else if ( ( cfStatus == 0 && ! isAChunk ) || cfStatus == KEY_NO_EXIST )
	{
	    qdsIsChunk = false;
	}
	else
	{
	    //
	    // An error occured, print a message.
	    //

	    checkFitsioNX( fits_read_key( fptr, TLOGICAL, "CHUNK", &isAChunk, 
		    NULL, &cfStatus ), cfStatus, status, VOID );
	}
    }
    if ( status.ok() )
    {
	cfStatus = 0;
    }


    //
    // Process each of the HDUs in the input buffer. The isMain flag is set
    // to true when processing the first HDU.
    //

    for ( hduType = IMAGE_HDU, isMain = true, i = 1; 
	    cfStatus == 0 && status.ok(); 
	    fits_movrel_hdu( fptr, 1, &hduType, &cfStatus ), 
	    isMain = false, i++ ) 
    {
	//
	// Only process image HDUs, and ignore anything else.
	//

	if ( hduType == IMAGE_HDU )
	{
	    //
	    // Find an appropriate frame object.
	    //

	    pFrame = frameFind( fptr, isMain, i, status );


	    if ( pFrame != NULL )
	    {
		//
		// Lock the frame so no other thread can be modifying it.
		//

		pFrame->wLock();


		//
		// Merge the header from the HDU.
		//

		pFrame->headerMerge( fptr, isMain, status );


		//
		// Merge the data from the HDU.
		//

		pFrame->dataMerge( fptr, qdsIsChunk, pBuffer, status );
		status.status( status.S_SUCCESS );


		//
		// Unlock the frame.
		//

		pFrame->unlock();
	    }
	}

	if ( ! hasExtend )
	{
	    break;
	}
    }


    if ( cfStatus == END_OF_FILE )
    {
	//
	// The error was an end of file, which isn't really an error, so
	// clear the cfitsio message buffer and move back to the start of the
	// file.
	//

	fits_clear_errmsg();
	cfStatus = 0;
	checkFitsio( fits_movabs_hdu( fptr, 1, &hduType, &cfStatus ), 
		cfStatus, status, VOID );
    }
    else
    {
	//
	// The error status indicates a real error, so display the message
	// returned from the cfitsio library.
	//

	checkFitsioNX( fits_movrel_hdu( fptr, 1, &hduType, &cfStatus ), 
		cfStatus, status, VOID );
    }


    //
    // Close the FITS buffer.
    //

    checkFitsio( fits_close_file( fptr, &cfStatus ), 
	    cfStatus, status, VOID );
    fitsMutex.unlock();

    qdsFirst = false;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::forEachFrame
//
// INVOCATION:
// dataset.forEachFrame( function, arg, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) function (tForEachFrameFn) The Function to execute.
// (>) arg (void *) Argument to pass to the function.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Execute the specified functon for each frame in the dataset
//
// The prototype of the function called is:
// function( cQlsFrame & frame, void * arg, cQlsStatus & status ).
//
// The frame parameter is set to each frame in turn, the arg parameter
// is passed the arg value, and the status is the inherited function status.
//
// DESCRIPTION:
// Iterate though the list of frames executing the specified function
// for each frame.
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

void		cQlsDataset::forEachFrame
(
    tForEachFrameFn		// (in)  The function to execute.
    		function,
    void	*arg,		// (in)  Argument to pass to the function.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    iFrameList	i;


    for ( i = qdsFrameList.begin(); i != qdsFrameList.end(); i++ )
    {
	function( *((*i).second), arg, status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::frameFind
//
// INVOCATION:
// pFrame = frameFind( fptr, isMain, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fptr (fitsfile *) Fits file pointer to open FITS file.
// (>) isMain (bool) Is this the main HDU?
// (>) index (int) Index of the HDU in the file.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// (cQlsFrame *) A Pointer the located cQlsFrame object.
//
// PURPOSE:
// Find or create a frame for the current FITS HDU
//
// DESCRIPTION:
// - If this HDU is the main HDU, return the pre-allocated main frame.
// - otherwise, get the extension name from the HDU.
// - If frame name isn't available, use the frame index to form an extension 
//   name.
// - Get the frame ID from the HDU. If frame ID isn't present, use the
//   extension name and version in its place.
// - Look for an existing frame matching the frame id. If not found, create
//   a new frame.
// - Return a pointer to the matching or created frame.
// 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// - Should be called with the cfitsio mutex locked to ensure two threads
//   don't use the cfitsio library.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cQlsFrame	*cQlsDataset::frameFind
(
    fitsfile	*fptr,		// (in)  The fits file to read from.
    bool	isMain,		// (in)  Is this the main HDU?
    int		index,		// (in)  index of the HDU in the file.
    cQlsStatus	&status		// (mod) function return status.
)
{
    int		cfStatus( 0 );	// cfitsio function return status.
    char	extName[ 81 ];	// Extension name of a frame.
    char	extVer[81];	// Extension version of a frame.
    char	frameId[ 81 ];	// Frame id.
    iFrameList	iFrame;		// Iterator in the frame list.
    cQlsFrame	*pFrame;	// The frame object found or created.


    checkStat( status, return( NULL ) );


    if ( isMain )
    {
	//
	// If this is the main HDU, get the pointer to the main frame 
	// from the static variable in the class.
	//

	pFrame = qdsPMainFrame;
    }
    else
    {
	//
	// This is not the main HDU, so find the frame based on extension
	// name and extension version.
	//


	//
	// Get the frame name.
	//

	fits_read_key( fptr, TSTRING, "FRMNAME", extName, NULL, &cfStatus );
	if ( cfStatus == VALUE_UNDEFINED || cfStatus == KEY_NO_EXIST )
	{
	    //
	    // Extension version does not have to be defined, so if it isn't
	    // set the default value to 0.
	    //

	    sprintf( extName, "Extension %d", index );
	    cfStatus = 0;
	}
	else
	{
	    //
	    // An error occured while trying to get the extension version,
	    // print the message.
	    //

	    checkFitsioNX( fits_read_key( fptr, TSTRING, "FRMNAME", extName, 
		    NULL, &cfStatus ), cfStatus, status, VOID );
	}


	//
	// Get the extension version.
	//

	fits_read_key( fptr, TSTRING, "FRMID", extVer, NULL, &cfStatus );
	if ( cfStatus == VALUE_UNDEFINED || cfStatus == KEY_NO_EXIST )
	{
	    //
	    // Extension version does not have to be defined, so if it isn't
	    // set the default value to 0.
	    //

	    cfStatus = 0;
	    extVer[0] = '\0';
	}
	else
	{
	    //
	    // An error occured while trying to get the extension version,
	    // print the message.
	    //

	    checkFitsioNX( fits_read_key( fptr, TSTRING, "FRMID", extVer, 
		    NULL, &cfStatus ), cfStatus, status, VOID );
	}


	//
	// Get the frame id name.
	//

	fits_read_key( fptr, TSTRING, "FRAMEID", frameId, NULL, &cfStatus );
	
	if ( cfStatus == VALUE_UNDEFINED || cfStatus == KEY_NO_EXIST )
	{
	    //
	    // If the frame id isn't define, use the extension name and 
	    // version.
	    //

	    (void) sprintf( frameId, "%s:%s", extVer, extName );
	    cfStatus = 0;
	}
	else
	{
	    //
	    // If an error occured while trying to get the frame id,
	    // print the message.
	    //

	    checkFitsioNX( fits_read_key( fptr, TSTRING, "FRAMEID", frameId, 
		    NULL, &cfStatus ), cfStatus, status, VOID );
	}


	if ( cfStatus != 0 )
	{
	    return( NULL );
	}


	//
	// Look for the matching frame.
	//

	iFrame = qdsFrameList.find( frameId );

	if ( iFrame == qdsFrameList.end() )
	{
	    //
	    // The frame could not be found, so create a new one. A pointer
	    // to the frame is kept for later use.
	    //

	    pFrame = new cQlsFrame( this, frameId, extName );
	    qdsFrameList[ frameId ] = pFrame;
	    

	    //
	    // Copy all exising keyword from the main header into the frame
	    // header.
	    //

	    pFrame->headerCopy( *qdsPMainFrame, status );
	}
	else
	{
	    //
	    // The frame already exists, so get a pointer to it.
	    //

	    pFrame = (*iFrame).second;
	}
    }

    return( pFrame );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::hasStream
//
// INVOCATION:
// hasStream = dataset.hasStream( stream );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) stream (cQlsStream &) The stream to look for.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Check to see if a specified stream is associated with a dataset
//
// DESCRIPTION:
// Attempt to find the stream in the list of streams assigned to a dataset.
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

bool		cQlsDataset::hasStream
(
    cQlsStream	&stream		// (in)  The stream to look for.
)
{
    cQlsStream::iStreamList
		i;


    //
    // Search for the stream by name.
    //

    if ( qdsStreamList.find( stream.name() ) == qdsStreamList.end() )
    {
	return( false );
    }
    else
    {
	return( true );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::modifiedClear
//
// INVOCATION:
// n.a. Called by the forEachFrame function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) frame (cQlsFrame &) The frame to change.
// (>) arg (void *) Input argument.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Function to change the modified status of a frame
//
// This function is designed to be called by the cQlsDataset::forEachFrame
// function.
//
// DESCRIPTION:
// Executes the cQlsFrame::modified method.
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

void		cQlsDataset::modifiedClear
(
    cQlsFrame	&frame,		// (in)  The frame to cahnge.
    void	*,		// (in)  The input argument.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    frame.modifiedClear( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::setQlStream
//
// INVOCATION:
// dataset.setQlStream( pStreams, numStreams, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pStreams (char **) List of names of streams add.
// (>) numStreams (uint) Number of streams in the list.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a list of quick look streams to a dataset
//
// DESCRIPTION:
// For each stream name in the input list, find or create a cQlsStream object,
// and add it to the datasets list of streams.
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

void		cQlsDataset::setQlStream
(
    char	**pStreams,	// (in)  Pointer to the list of stream names.
    uint	numStreams,	// (in)  Number of streams in the list.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    uint	i;
    cQlsStream	*pStream;	// The stream object to add.


    checkStat( status, return );


    //
    // Remove any existing streams.
    //

    qdsStreamList.clear(); // XXX allan: was erase()

    //
    // Loop through the stream names in the pStreams list.
    //

    for ( i = 0; i < numStreams; i++ )
    {
	//
	// Lock the list of streams so that it won't change while I'm looking
	// at it.
	//

	cQlsStream::listWLock();


	//
	// Find an existing stream or create a new one.
	//

	pStream = cQlsStream::find( pStreams[i], status );


	//
	// Increment the stream reference counter so it won't be deleted
	// until this dataset is done with it.
	//

	pStream->ref();


	//
	// Unlock the list of streams.
	//

	cQlsStream::listUnlock();


	//
	// Add the stream to the list of streams associated with the dataset.
	//

	qdsStreamList[pStream->name()] = pStream;

	status.S_DS_STREAMS( status, pStreams[i], datasetName() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::streamAdd
//
// INVOCATION:
// dataset.streamAdd( avlist, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) avList (cDhsAvList &) The av list to add the streams to.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add all of the streams assigned to the dataset to an attribute value list
//
// DESCRIPTION:
// - Create an array of character pointers large enough to hold the names of
//   all streams in the list of streams.
// - Assign the stream names to the array of pointers.
// - Add a new array of strings to the avList with the name "streams", and
//   with the array of stream names as its value.
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

void		cQlsDataset::streamAdd
(
    cDhsAvList	&avList,	// (mod) The av list to add the streams to.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cQlsStream::iStreamList	i;
    int		j;
    ulong	numStreams;	// Number of streams in qlsStreamList.
    cQlsStream	*p;		// The stream being examined.
    const char	**streamArray;	// An array of stream names.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Get the number of streams in the list.
    //

    numStreams = qdsStreamList.size();


    //
    // Allocate an array of string pointers large enough for all of the
    // stream names.
    //

    checkNull( streamArray = new const char * [numStreams], status, return );


    //
    // Put the pointer to each stream name into the array of stream names.
    //

    for ( i = qdsStreamList.begin(), j = 0; i != qdsStreamList.end(); i++, j++ )
    {
	p = (*i).second;
	streamArray[j] = p->name();
    }
    numStreams = j;


    //
    // Create a new attribute in the av list with the stream names as its
    // value.
    //

    checkDhs( avList.add( "streams", DHS_DT_STRING, 1, &numStreams, 
	    streamArray, dhsStatus ), dhsStatus, status, return );

    delete streamArray;
}
