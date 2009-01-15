static char rcsid[] = "$Id: stoFile.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoFile.C
//
// PURPOSE:
// Holds methods for the cStoFile class. 
//
// METHOD NAME(S)
// cStoFile::setPAmFile	- Sets pointer to am library file struct.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:56  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.7  2000/08/22 20:17:34  nhill
// Removed some debugging messages.
//
// Revision 1.6  2000/08/22 19:27:49  nhill
// Fixed a bug which caused file names to be wrong when data is retrieved
// through a get.
//
// Revision 1.5  1999/10/13 18:27:12  nhill
// Fixed the DHS++ include files.
//
// Revision 1.4  1999/10/12 22:58:47  nhill
// Added the cStoFile::save method.
//
// Revision 1.3  1998/05/15 17:08:39  cockayne
// Added memory error checking to setPAmFile().
//
// Revision 1.2  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


extern "C"
{
#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_config.h"
#include "gen_file.h"
#include "press.h"
}

#define DHS_NO_COMMAND
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"
#include "stoDbm.H"
#include "stoFile.H"
#include "stoActionType.H"
#include "stoMedia.H"


//
// Define static data members of the cStoFile class. 
//

cMutex		pressLibMutex;



//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoFile::setPAmFile
//
// INVOCATION:
// setPAmFile( pAmFile );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmFile	(AM_FILE *)	- Pointer to am library file struct.
// 
// FUNCTION VALUE:
// (AM_FILE *) pointer to saved am file struct.
// 
// PURPOSE:
// Saves a copy of the am file struct in the class data. 
// 
// DESCRIPTION:
// Allocate memory and copy the struct to class data. 
// 
// EXTERNAL VARIABLES:
// None.
// 
// PRIOR REQUIREMENTS:
// None.
// 
// SEE ALSO:
// None.
// 
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

AM_FILE         *cStoFile::setPAmFile
( 
    AM_FILE 	*pAmFile	// Ptr to am library file struct. 
)
{
    AM_FILE	*ptr;		// Ptr to am library file struct. 


    //
    // Allocate space and save a copy of am library file structure.
    //

    ptr = (AM_FILE *) gen_alloc( sizeof( AM_FILE ) );
    if ( ptr != NULL )
    {
	gen_cpy( ptr, pAmFile, sizeof( AM_FILE ) );
    }


    //
    // Return pointer to allocated stuff.
    //

    return( ptr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoFile::save
//
// INVOCATION:
// stoFile.save( dataset, dataLabel, compressionFlag, oldAmFile, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataset (cDhsDataset &) The dataset containing the buffer.
// (>) dataLabel (const char *) The data label for the dataset.
// (>) compressionFlag (char) The compression of the data.
// (>) oldAmFile (const AM_FILE &) The AM file before modification.
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Save the buffer as the content of the specified file
//
// DESCRIPTION:
// Description of the algorithm.
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

void		cStoFile::save
(
    cDhsDataset	&dataset,	// (in)  The dataset containing the buffer.
    const char	*dataLabel,	// (in)  The datalabel for the dataset.
    char	compressionFlag,// (in)  The compression type of the data.
    const AM_FILE		// (in)  The AM_FILE structure before mods.
    		&oldAmFile,
    cStoStatus	&status		// (mod) Function return status.
)
{
    void	*bufPtr;	// Pointer to the data buffer.
    DBPROCESS	*dbProcess;	// The dbProcess to use.
    char	*ext;		// The file name extension.
    int		fd;		// File descriptor of the file to write.
    char	filePath[PATH_MAX];
    				// The path to the file to be written.
    char	*mediaRequestType;
    				// Temporary pointer to media request type.
    bool	mustFreeBuffer( false );
    				// True when freeing the data buffer is 
				// required.
    ssize_t	numBytesWritten;// The number of bytes actually written.
    void	*oldBufPtr;	// Pointer to the original data buffer.
    char	pathHead[PATH_MAX];
    				// Path to file's directory.
    cStoMedia	*pMedia;	// The media request associated with this file.
    struct stat	statStruct;


    //
    // Compression handling. Expect bufPtr to point to the *correct*
    // buffer after this. Also expect am library struct filesize
    // and uncompressed filesize to be correct. Save old am lib
    // filesize to subtract from media object's total later on. 
    // If filename needs fixing (add or subtract .gz) fix it. 
    //

    if ( getPAmFile()->amfCompressionFlag == AMF_COMP_ASIS )
    {
	if ( compressionFlag == STO_CAV_COMP_GZIP )
	{
	    getPAmFile()->amfCompressionFlag = AMF_COMP_GZIP;
	}
	else
	{
	    getPAmFile()->amfCompressionFlag = AMF_COMP_NONE;
	}
    }

    if ( getPAmFile()->amfCompressionFlag == AMF_COMP_GZIP )
    {
	if ( compressionFlag == STO_CAV_COMP_GZIP )
	{
	    //
	    // Want compressed. Is compressed. Don't fix filename.
	    //

	    pressLibMutex.lock();
	    bufPtr = dataset.buffer();
	    getPAmFile()->amfFileSize = dataset.length();
	    checkPress( unpress_msize( (unsigned char *) bufPtr, 
		    dataset.length(), &( getPAmFile()->amfUncompFileSize ), 
		    PR_GZIP ), status, dataLabel, VOID );
	    pressLibMutex.unlock();
	}
	else
	{
	    //
	    // Want compressed. Is uncompressed. Fix filename.
	    //
	    
	    pressLibMutex.lock();
	    mustFreeBuffer = true;
	    oldBufPtr = dataset.buffer();
	    getPAmFile()->amfUncompFileSize = dataset.length();
	    checkPress( press_m2m( (unsigned char *) oldBufPtr, 
		    dataset.length(), (unsigned char **) &bufPtr, 
		    &( getPAmFile()->amfFileSize ), PR_GZIP ),
		    status, dataLabel, VOID );
	    pressLibMutex.unlock();
	    if ( status.ok() && 
		    ( ( ext = strext( getPAmFile()->amfFilename ) ) == NULL || 
		    strne( ext, "gz" ) ) )
	    {
		strcat( getPAmFile()->amfFilename, ".gz" );
	    }
	}
    }
    else if ( getPAmFile()->amfCompressionFlag == AMF_COMP_NONE )
    {
	if ( compressionFlag == STO_CAV_COMP_GZIP )
	{
	    //
	    // Want uncompressed. Is compressed. Fix filename.
	    //
	    
	    pressLibMutex.lock();
	    mustFreeBuffer = true;
	    oldBufPtr = dataset.buffer();
	    checkPress( unpress_m2m( (unsigned char *) oldBufPtr, 
		    dataset.length(), (unsigned char **) &bufPtr, 
		    &( getPAmFile()->amfFileSize ), PR_GZIP ),
		    status, dataLabel, VOID );
	    pressLibMutex.unlock();
	    if ( status.ok() )
	    {
		getPAmFile()->amfUncompFileSize = getPAmFile()->amfFileSize;

		if ( ( ext = strext( getPAmFile()->amfFilename ) ) != NULL && 
			streq( ext, "gz" ) )
		{
		    strroot( getPAmFile()->amfFilename );
		}
	    }
	}
	else
	{
	    //
	    // Want uncompressed. Is uncompressed. Don't fix filename.
	    //

	    bufPtr = dataset.buffer();
	    getPAmFile()->amfUncompFileSize = dataset.length();
	    getPAmFile()->amfFileSize = dataset.length();
	}
    }
    else if ( getPAmFile()->amfCompressionFlag == AMF_COMP_NONE )
    {
	if ( compressionFlag == STO_CAV_COMP_GZIP )
	{
	    //
	    // Is compressed. Want compressed. Don't fix filename.
	    //

	    pressLibMutex.lock();
	    bufPtr = dataset.buffer();
	    getPAmFile()->amfFileSize = dataset.length();
	    checkPress( unpress_msize( (unsigned char *) bufPtr, 
		    dataset.length(), &( getPAmFile()->amfUncompFileSize ), 
		    PR_GZIP ), status, dataLabel, VOID );
	    pressLibMutex.unlock();
	}
	else
	{
	    //
	    // Is uncompress, want uncompressed. Dont filx file name.
	    //

	    bufPtr = dataset.buffer();
	    getPAmFile()->amfUncompFileSize = dataset.length();
	    getPAmFile()->amfFileSize = dataset.length();
	}
    }


    if ( status.ok() )
    {
	//
	// Get the media request associated with this file.
	//

	pMedia = cStoMedia::findById( getPAmFile()->amfMediaRequestId );


	//
	// Compose complete filepath.
	//

	mediaRequestType = pMedia->getPAmMedia()->ammMediaRequestType;
	if ( streq( mediaRequestType, AM_RT_ARCHIVE ) )
	{
	    (void) sprintf( filePath, "%s/%d.%s/%s", 
		    cStoDbManager::getAmArchiveRetrievalStage(),
		    pMedia->getPAmMedia()->ammMediaRequestId,
		    pMedia->getPAmMedia()->ammMediaRequestName,
		    getPAmFile()->amfFilename );
	}
	else if ( streq( mediaRequestType, AM_RT_USER ) )
	{
	    (void) sprintf( filePath, "%s/%d.%s/%s", 
		    cStoDbManager::getAmUserRetrievalStage(),
		    pMedia->getPAmMedia()->ammMediaRequestId,
		    pMedia->getPAmMedia()->ammMediaRequestName,
		    getPAmFile()->amfFilename );
	}
	else
	{
	    status.E_MEDIA_REQUEST_TYPE( status, mediaRequestType,
		    pMedia->getMediaRequestId() );
	}
		

	//
	// Stat path to file. If it doesn't exist, create it. Need to do
	// this because the returned filename may contain a path component. 
	//

	(void) sprintf( pathHead, "%s", filePath );
	strhead( pathHead );
	if ( stat( pathHead, &statStruct ) == -1 )
	{
	    if ( ! file_dir_mk( pathHead, 
		    S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH ) )
	    {
		status.E_FPATH( status, pathHead );
	    }
	}

	
	//
	// Open file for writing. 
	//

	if ( status.ok() && 
		( fd = open( filePath, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC,
		S_IRUSR | S_IWUSR | S_IRGRP ) ) == -1 )
	{
	    status.E_FOPEN( status, filePath );
	    status.sysErrno();
	}


	//
	// Write file. Free buffer if necessary.
	//
	
	if ( status.ok() && ( numBytesWritten = write( fd, bufPtr, 
		getPAmFile()->amfFileSize ) ) != getPAmFile()->amfFileSize )
	{
	    status.E_FWRITE( status, filePath );
	    status.sysErrno();
	}

	if ( mustFreeBuffer )
	{
	    gen_free( bufPtr );
	}

	
	//
	// Close file. 
	//

	if ( fd != -1 && close( fd ) == -1 )
	{
	    status.E_FCLOSE( status, filePath );
	    status.sysErrno();
	}


	//
	// Ensure file arrived on magnetic disk.
	//

	if ( status.ok() && stat( filePath, &statStruct ) == 0 )
	{
	    if ( statStruct.st_size != getPAmFile()->amfFileSize )
	    {
		status.E_FSIZE( status, filePath );
	    }
	}
	else
	{
	    status.E_FSTAT( status, filePath );
	    status.sysErrno();
	}
    }


    //
    // Set am library file structure status.
    //

    if ( status.ok() )
    {
	getPAmFile()->amfStatus = AMF_RETRIEVED;
    }
    else
    {
	//
	// Revert to the original AM file structure.
	//

	memcpy( getPAmFile(), &oldAmFile, sizeof( AM_FILE ) );
	getPAmFile()->amfStatus = AMF_ERROR;
    }


    //
    // Update database.
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amFileDelete( &oldAmFile, dbProcess ), status, VOID );
    checkAm( amFilePut( getPAmFile(), dbProcess ), status, VOID );
    cStoDbManager::release();


    if ( status.ok() )
    {
	//
	// Update EPICS. Increase queueable, decrease retrievable. 
	//
	
	pMedia->lock();
	pMedia->setRetrieveSize( pMedia->getRetrieveSize() - 
		oldAmFile.amfFileSize );

	pMedia->setQueueSize( pMedia->getQueueSize() + 
		getPAmFile()->amfFileSize );
	pMedia->unlock();
    }
}
