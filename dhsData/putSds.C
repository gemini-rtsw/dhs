static char rcsid[] = "$Id: putSds.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/putSds.C
//
// PURPOSE:
// Handles functions required in bulk data put fits requests.
//
// METHOD NAME(S)
// cDtsPutDs::chunkConvert()	: Convert a single SDS chunk to fits.
// cDtsPutDs::datasetCompose()	: Compose SDS chunks into a FITS file.
// cDtsPutDs::datasetRecovery()	: Tar an SDS dataset.
// cDtsPutDs::datasetRecovery() : Tar an SDS dataset.
// cDtsPutDs::destinationExtract() : Determine where to write a put buffer.
// cDtsPutDs::exec()		: Execution point of a put to SDS.
// cDtsPutDs::fileProcess()	: Process the file - write it somewhere.
// cDtsPutDs::finalDestin()	: Determine final destination of a file.
// cDtsPutDsQl::exec()		: Execution point of a put to QL only.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:30  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.17  2000/08/10 22:11:04  dunn
// Added check in for bad data type.
//
// Revision 1.16  2000/07/27 20:10:28  dunn
// Added standalone features.
//
// Revision 1.15  1998/08/04 22:04:05  dunn
// Removed trailing / from tar path name.
//
// Revision 1.14  1998/06/22 20:49:59  dunn
// Changed popen for the tar command to use fork1 and execlp.
//
// Revision 1.13  1998/06/11 16:39:22  dunn
// Got rid of unused variable and no longer writes to /tmp
// if the write to temp or perm fails.
//
// Revision 1.12  1998/05/26 18:48:41  dunn
// Fixed changing of buf size and correct guess to be multp. of 2880
//
// Revision 1.11  1998/05/14 20:17:42  dunn
// Added return of error if the parsing of sds to fits failed
// for a chunk only.
//
// Revision 1.10  1998/05/04 23:02:04  dunn
// Added changed to return avList with datasetname and status changes.
//
// Revision 1.9  1997/11/25 00:10:12  jaeger
// Fixed buffer size (length) of passed into parseHeader - now is the actual
// size of the composed FITS file.
//
// Revision 1.8  1997/10/17 20:46:27  dunn
//  Changed logic to return an error if there were any warnings.
//
// Revision 1.7  1997/10/03 23:02:38  dunn
// Added mutex around popen call and got rid of cDtsDhsQlPut and replaced
// with cDhsPutDataset.
//
// Revision 1.6  1997/09/29 21:22:45  dunn
// Added fits parsing when dataset is composed.
//
// Revision 1.5  1997/09/17 22:15:52  dunn
// Code cleanup.
//
// Revision 1.4  1997/08/29 23:04:52  dunn
// Added composition of datasets.
//
// Revision 1.3  1997/08/19 22:31:34  dunn
// Added chunks and data composing.
//
// Revision 1.2  1997/07/29 22:03:44  dunn
// Various fixes.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

//
//  Includes
//

#include <iostream>

#include "globals.H"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <wait.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_eptr.h"

#include "sf.h"
}

#include "fh.H"
#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "store.H"
#include "put.H"
#include "dhsGenConnect.H"

//
//  Local defines.
//

// XXX allan: no fork1 on linux        
#ifdef linux
#define fork1 fork
#endif



cMutex	cDtsPutDs::cdlMutexSf;

#define DTS_TAR_EXT	".tar"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutDs::chunkConvert
//
// INVOCATION:
// chunkConvert( status, maxChuunk, pDsList, dsPtr, filePath, ffptr, bufSize);
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) maxChunk (int) Number of chunks that is part of this dataset.
// (!) pDsList (cDtsDatasetList *) Dataset object pointer.
// (!) dsPtr (cDtsDatasetTable *) Pointer to dataset table info.
// (>) filePath (char *) Path to where chunks are found.
// (!) ffPtr (fitsFile *) Buffer pointer.
// (!) bufSize (int &) Final buffer size.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Convert all chunks into a final fits file
//
// DESCRIPTION:
// Convert all chunks into a final fits file.  Read in each chunk, and 
// convert it from sds chunk files to one fits file.
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

void cDtsPutDs::chunkConvert
(
    cDtsStatus	&status,		// (mod) Status passed in.
    int		maxChunk,		// (in)  Maximum num of chunks.
    cDtsDatasetList
    		*pDsList,		// (mod) Dataset object.
    cDtsDatasetTable
    		*dsPtr,			// (in)  Pointer to database info.
    char	*filePath,		// (in)  Path in temp to files.
    fitsfile	*ffPtr,			// (mod) Buffer pointer.
    int		&bufSize		// (mod) Final buffer size.
)
{

    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// Dhs status.
    char	filename[PATH_MAX];
    FILE	*fp;                    // File pointer.
    bool	lastChunk;		// Last chunk flag.
    cDhsPutDataset
    		*pPut;			// Put object to allow access to sds.
    SF_STATUS	sfStatus = SF_S_SUCCESS;// sf Status.
    int		sizeRead;		// Size read.
    struct stat
		stat_buf;		// Output of the stat call.
    void	*tmpBuffer = NULL;	// Pointer to temporary buffer.
    int		tmpBufSize;		// Temporary buffer size;


    //
    //  Check the status.
    //

    checkStat( status, return );
    lastChunk = FALSE;


    //
    //  Read in each chunk and convert.
    //

    for( int i = 1; i <= maxChunk; i++ )
    {

	//
	//  Compose the full path with filename, do a stat to get size of
	//  the chunk, open it.
	//
	
	sprintf( filename, "%s%s.%d", filePath, dsPtr->dstDatasetName, i );

	if ( stat( filename, &stat_buf ) == -1 )
	{
	    //
	    //  File doesn't exist. Error out.
	    //

	    status.E_NO_FILE( status, filename );
	    break;
	}

	if ( ( fp = fopen( filename,  "r" ) ) == NULL )
	{
	    //
	    //  No able to open the file.
	    //

	    status.E_FILE_OPEN( status, filename );
	    status.sysErrno();
	    status.display();
	    break;
	}


	//
	//  Alloc buffer and read in.
	//

	tmpBuffer = gen_alloc( (stat_buf.st_size + 1) );
	sizeRead = fread( tmpBuffer, stat_buf.st_size, 1, fp );

	if ( sizeRead < 1 )
	{
	    //
	    // Nothing read.  Or read error.
	    //
	    
	    status.E_NO_FILE( status, filePath );
	    break;
	}

	(void) fclose( fp );


	//
	//  Import into an sds buffer.  Get the av Id.
	//

	pPut = new cDhsPutDataset( tmpBuffer, (stat_buf.st_size + 1), 
		DHS_BD_PT_DS );
	pPut->access( dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    status.E_ACCESS( status, dsPtr->dstDatasetName );
	    delete pPut;
	    break;
	}


	//
	//  Determine if the last Chunk flag needs to be set, that is passed
	//  to sfSds2Fits.
	//

	if ( i == maxChunk )
	{
	    lastChunk = TRUE;
	}

	lockSf();
	checkSfLib( sfSds2Fits( pPut->objectId(), ffPtr , pDsList->cdlDsInfo, 
		FALSE, &sfStatus ), sfStatus, dsPtr->dstDatasetName, 
		pDsList->cdlDsInfo, status, VOID );
	unlockSf();
	delete pPut;

	if ( tmpBuffer != NULL )
	{
	    free( tmpBuffer );
	    tmpBuffer = NULL;
	}

	if ( !status.ok() )
	{
	    break;
	}
    }


    //
    //  Check for errors, if so, then recover dataset into a tar file.
    //

    if ( !status.ok() )
    {
	status.status( status.S_SUCCESS );
	datasetRecovery( status, filePath, pDsList );
	putStatus.S_DATA_RECOVERY( putStatus, dsPtr->dstDatasetName  );
	cdlRecovery = TRUE;
	if ( tmpBuffer != NULL )
	{
	    free( tmpBuffer );
	}
    }


    //
    //  Close and get the size of the buffer and close the fits file ptr.
    //

    lockSf();
    tmpBufSize = sfFitsFileSize( ffPtr, &sfStatus );
    sfCloseFitsFile( ffPtr, &sfStatus );
    bufSize = tmpBufSize;

    unlockSf();

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutDs::datasetCompose
//
// INVOCATION:
// datasetCompose(status, allSent, maxChunk, pDsList)
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) allSent (bool) All chunks have been sent.
// (>) maxChunk (int) Number of chunks that is part of this dataset.
// (!) pDsList (cDtsDatasetList *) Dataset object pointer.
// (<) BufSize (int*) Size of the buffer, after composition.
//
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Compose a complete dataset
//
// DESCRIPTION:
// Compose a complete dataset.  Convert from sds to fits.
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

void cDtsPutDs::datasetCompose
(
    cDtsStatus	&status,		// (mod) Status passed in.
    bool	allSent,		// (in)  All chunks sent.
    int		maxChunk,		// (in)  Maximum num of chunks.
    cDtsDatasetList
    		*pDsList,		// (mod) Dataset object.
    void	**fitsBuffer,		// (mod) Fits buffer.
    int		*bufSize		// (out) Fits buffer size.
)
{

    cDtsDatasetTable
    			*dsPtr;		// Pointer to database info.
    char		filePath[PATH_MAX];
    char		*finalPath;	// Path to where the file is.
    fitsfile		*ffPtr;
    char		*fname;		// Filename.
    SF_STATUS		sfStatus = SF_S_SUCCESS;
    char		*volName;	// Volume name.


    //
    //  Check the status, and get the table information.
    //

    checkStat( status, return );
    dsPtr = pDsList->getDs();


    //
    //  Increment number of incomplete dataset if necessary.
    //

    if ( !allSent && maxChunk == 1 )
    {
	cDtsLists::lstMutexNumIncomp.lock();
	cDtsDhs::numIncomp( TRUE, status );
	cDtsLists::lstMutexNumIncomp.unlock();
    }
    else if ( allSent && maxChunk > 1 )
    {
	cDtsLists::lstMutexNumIncomp.lock();
	cDtsDhs::numIncomp( FALSE, status );
	cDtsLists::lstMutexNumIncomp.unlock();
    }	    


    //
    //  If all data has not been received, then return.
    //
    
    if ( !allSent )
    {
	return;
    }


    //
    //  Compose the path to the chunk files.
    //

    sprintf( filePath, "%s/%s/", cDtsStoreManager::tempPath(), 
	    dsPtr->dstDatasetName );


    //
    //  If health is sick, perform recovery and return.
    //

    if ( dsPtr->dstHealth == DTS_HL_SICK )
    {
	datasetRecovery( status, filePath, pDsList );
	putStatus.S_DATA_RECOVERY( putStatus, dsPtr->dstDatasetName  );
	cdlRecovery = TRUE;
	return;
    }


    //
    //  If there is no data there, print error and return.
    //

    if ( dsPtr->dstFileSize == 0 )
    {
	putStatus.S_NO_DATA( putStatus, dsPtr->dstDatasetName  );
	status.E_NO_DATA( status, dsPtr->dstDatasetName );
	dsPtr->dstHealth = DTS_HL_SICK;
	cdlRecovery = TRUE;
	return;
    }


    //
    //  Alloc room for final fits buffer.  Check that dsinfo is not null.
    //

    *bufSize = (int)((dsPtr->dstFileSize + 0.6 * dsPtr->dstFileSize));
    *bufSize = (*bufSize / 2880 ) + 1;
    *bufSize = *bufSize * 2880 ;

    checkNull( ( *fitsBuffer = gen_alloc( *bufSize )), status, return );

    if ( pDsList->cdlDsInfo ==  NULL )
    {
	status.E_DSINFO_NULL( status, dsPtr->dstDatasetName );
	return;
    }


    //
    //  Set the memory buffer to point to the malloced buffer.
    //

    lockSf();
    checkSfLib( ( ffPtr = sfSetFitsMem( fitsBuffer, (long *) bufSize, 
	    pDsList->cdlDsInfo, &sfStatus )), sfStatus, dsPtr->dstDatasetName,
	    pDsList->cdlDsInfo, status, VOID );
    unlockSf();
    checkStat( status, return );


    //
    //  Read in each chunk and convert to one fits file.
    //

    chunkConvert( status, maxChunk, pDsList, dsPtr, filePath, ffPtr, 
		*bufSize );


    //
    //  Determine final destination. Write the buffer.  Make retrievable.
    //

    if ( !cdlRecovery )
    {
	cDtsDhsPut::destinationExtract( status, &fname, &finalPath, &volName, 
	    *bufSize, dsPtr );
	fileProcess( status, finalPath, *bufSize, *fitsBuffer, TRUE );
	cDtsStoreManager::fileRemove( status, filePath, 
		cDtsStoreManager::ST_TEMP, *bufSize );
	if ( dsPtr != NULL )
	{
	    //
	    // Update the space listed at the status server.
	    //

	    if ( dsPtr->dstLifetime == DTS_PERM )
	    {
		cDtsStoreManager::updateSpace( status, 
			cDtsStoreManager::ST_PERM, *bufSize );
	    }
	    else
	    {
		cDtsStoreManager::updateSpace( status, 
			cDtsStoreManager::ST_TEMP, *bufSize );
	    }
	}


	//
	//  Write to the archive table to make this file retrievable.
	//
	
	cDtsDbManager::makeRetrievable( status, dsPtr->dstDatasetName, 
		fname, volName, *bufSize );
	if ( status.standAlone() && pDsList != NULL )
	{
	    pDsList->updateObject( status, finalPath, DHS_BD_PT_FITS );
	}
	if ( status.ok() && dsPtr != NULL )
	{
	    dsPtr->dstFormat = DTS_FM_FITS;
	}

	free( finalPath );

    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::datasetRecovery
//
// INVOCATION:
// datasetRecovery ()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) dirName (char *) Dirname that contains files to tar.
// (!) pDs (cDtsDatasetTable *) Dataset table ptr.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To tar all chunks received for this dataset
//
// DESCRIPTION:
// Tar all chunks associated with this dataset to its final destination, make
// the file retrievable and update the database accordingly.
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

void cDtsPutDs::datasetRecovery
(
    cDtsStatus	&status, 		// (mod) Status passed in.
    char	*dirName,		// (in)  Dirname that contains files.
    cDtsDatasetTable
    		*dsPtr			// (mod) Dataset table ptr..
)
{
    char 	arg0[5] = "tar";// Tar argument.
    char 	arg1[5] = "cf";	// Tar argument.
    char	*destin;	// Final destination of the file.
    char	*fname;		// Filename.
    char	*volName;	// Volume name.
    struct stat stat_buf;	// Output of the stat call.
    int		storage;	// Final storage destination.
    int		execStatus;	// Return status from the thread.
    pid_t	cdlProcessThreadId;
    				// Fork thread id.


    status.S_SDS2TAR( status, dsPtr->dstDatasetName );


    //
    //  Update health and format for this item to sick and tar.
    //

    dsPtr->dstHealth = DTS_HL_SICK;
    dsPtr->dstFormat = DTS_FM_TAR;


    //
    //  Determine final destination for this dataset and 
    //  create the arguments for the tar call.
    //

    finalDestin( status, &fname, &destin, &volName, storage, dsPtr );
    checkStat( status, return );
   

    //
    //  Null out last slash in dirName so that tar does not complain.
    //

    dirName[strlen(dirName)-1] = '\0';


    //
    // Spawn process to run tar of the files. Save thread ID in case of exit.
    //
        
    cdlProcessThreadId = -1;
    if ( ( cdlProcessThreadId = fork1() ) == 0 )
    {
	//
	//  Child process only executes the following: it replaces the
	//  child process with the execute command, and then exits.
	//  It executes tar cf destin dirName
	//

	if ( execlp( arg0, arg0, arg1, 
		    destin, dirName, (char *) NULL ) == -1 )
	{
	    std::cout << "execlp() failed to tar file, errno = " << errno << std::endl;
	    _exit( 999 );
	}
        if ( cdlProcessThreadId == -1 )
        {
	    status.E_TAR_FILE( status, destin );
	    status.sysErrno();
	    status.display();
	    return;
	}
    }


    //
    //  Wait for the child process to finish.
    //

    checkSystem( waitpid( cdlProcessThreadId, &execStatus, 0 ), status, VOID );
    cdlProcessThreadId = -1;


    //
    //  Check the return status of the child doing the tar.
    //

    if ( WIFEXITED( execStatus ) == 0 )
    {
	status.E_EXEC_ABNORMAL_EXIT( status, arg0 );
	if ( destin != NULL ) // XXX allan: changed from ( *destin != NULL )
	{
	    free( destin );
	}
	return;
    }
    else if (WEXITSTATUS( execStatus ) != 0 )
    {
	status.E_EXEC_NONZERO_EXIT( status, arg0,
		WEXITSTATUS( execStatus) - 256 );
	if ( destin != NULL ) // XXX allan: changed from ( *destin != NULL )
	{
	    free( destin );
	}
	return;
    }


    //
    //  Do a stat on the tar file to make sure its there and to get the size.
    //

    if ( stat( destin, &stat_buf ) == -1 )
    {
	//
	//  File doesn't exist. Error out.
	//

	status.E_TAR_FILE( status, destin );
	status.sysErrno();
	status.display();
	if ( destin != NULL ) // XXX allan: changed from ( *destin != NULL )
	{
	    free( destin );
	}
	return;
    }


    //
    //  Remove the dir in temp that we just tarred.
    //

    dsPtr->dstFileSize = stat_buf.st_size;
    cDtsStoreManager::fileRemove( status, dirName, cDtsStoreManager::ST_TEMP,
    		0 );

    //
    //  Update space, make the file retrievable and update the database.
    //

    cDtsStoreManager::updateSpace( status, (cDtsStoreManager::tLife) storage, 
	    dsPtr->dstFileSize );
    cDtsDbManager::makeRetrievable( status, dsPtr->dstDatasetName, fname, 
	    volName, stat_buf.st_size );
    free( destin );
    cDtsDatasetList::updateStatus( status, dsPtr->dstDatasetName, dsPtr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::datasetRecovery
//
// INVOCATION:
// datasetRecovery ()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) dirName (char *) Dirname that contains files to tar.
// (!) pDsList (cDtsDatasetList *) Dataset object pointer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To tar all chunks received for this dataset
//
// DESCRIPTION:
// Tar all chunks associated with this dataset to its final destination, make
// the file retrievable and update the database accordingly.
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

void cDtsPutDs::datasetRecovery
(
    cDtsStatus	&status, 		// (mod) Status passed in.
    char	*dirName,		// (in)  Dirname that contains files.
    cDtsDatasetList
    		*pDsList		// (mod) Dataset list.
)
{
    char 	arg0[5] = "tar";// Tar argument.
    char 	arg1[5] = "cf";	// Tar argument.
    char	*destin;	// Final destination of the file.
    cDtsDatasetTable
    		*dsPtr;		// Pointer to database info.
    char	*fname;		// Filename.
    char	*volName;	// Volume name.
    struct stat stat_buf;	// Output of the stat call.
    int		storage;	// Final storage destination.
    int		execStatus;	// Return status from the thread.
    pid_t	cdlProcessThreadId;
    				// Fork thread id.


    dsPtr = pDsList->getDs();
    status.S_SDS2TAR( status, dsPtr->dstDatasetName );


    //
    //  Update health and format for this item to sick and tar.
    //

    dsPtr->dstHealth = DTS_HL_SICK;
    dsPtr->dstFormat = DTS_FM_TAR;


    //
    //  Determine final destination for this dataset and 
    //  create the arguments for the tar call.
    //

    finalDestin( status, &fname, &destin, &volName, storage, dsPtr );
    checkStat( status, return );
   

    //
    //  Null out last slash in dirName so that tar does not complain.
    //

    dirName[strlen(dirName)-1] = '\0';


    //
    // Spawn process to run tar of the files. Save thread ID in case of exit.
    //

    cdlProcessThreadId = -1;
    if ( ( cdlProcessThreadId = fork1() ) == 0 )
    {
	//
	//  Child process only executes the following: it replaces the
	//  child process with the execute command, and then exits.
	//  It executes tar cf destin dirName
	//

	if ( execlp( arg0, arg0, arg1, 
		    destin, dirName, (char *) NULL ) == -1 )
	{
	    std::cout << "execlp() failed to tar file, errno = " << errno << std::endl;
	    _exit( 999 );
	}
        if ( cdlProcessThreadId == -1 )
        {
	    status.E_TAR_FILE( status, destin );
	    status.sysErrno();
	    status.display();
	    return;
	}
    }


    //
    //  Wait for the child process to finish.
    //

    checkSystem( waitpid( cdlProcessThreadId, &execStatus, 0 ), status, VOID );
    cdlProcessThreadId = -1;


    //
    //  Check the return status of the child doing the tar.
    //

    if ( WIFEXITED( execStatus ) == 0 )
    {
	status.E_EXEC_ABNORMAL_EXIT( status, arg0 );
	if ( destin != NULL ) // XXX allan: changed from ( *destin != NULL ) 
	{
	    free( destin );
	}
	return;
    }
    else if (WEXITSTATUS( execStatus ) != 0 )
    {
	status.E_EXEC_NONZERO_EXIT( status, arg0,
		WEXITSTATUS( execStatus) - 256 );
	if ( destin != NULL ) // XXX allan: changed from ( *destin != NULL ) 
	{
	    free( destin );
	}
	return;
    }


    //
    //  Do a stat on the tar file to make sure its there and to get the size.
    //

    if ( stat( destin, &stat_buf ) == -1 )
    {
	//
	//  File doesn't exist. Error out.
	//

	status.E_TAR_FILE( status, destin );
	status.sysErrno();
	status.display();
	if ( destin != NULL ) // XXX allan: changed from ( *destin != NULL ) 
	{
	    free( destin );
	}
	return;
    }


    //
    //  Remove the dir in temp that we just tarred.
    //

    dsPtr->dstFileSize = stat_buf.st_size;
    cDtsStoreManager::fileRemove( status, dirName, cDtsStoreManager::ST_TEMP,
    		0 );

    //
    //  Update space, make the file retrievable and update the database.
    //

    cDtsStoreManager::updateSpace( status, (cDtsStoreManager::tLife) storage, 
	    dsPtr->dstFileSize );
    cDtsDbManager::makeRetrievable( status, dsPtr->dstDatasetName, fname, 
	    volName, stat_buf.st_size );
    if ( status.standAlone() && pDsList != NULL )
    {
	pDsList->updateObject( status, destin, DHS_BD_PT_RAW );
    }
    free( destin );
    cDtsDatasetList::updateStatus( status, dsPtr->dstDatasetName, dsPtr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::destinationExtract
//
// INVOCATION:
// destinationExtract(status, fname, filePath, vol, size, dsPtr, chunkNumber);
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) fname (char **) Filename to write.
// (>) filePath (char **) Complete file and path.
// (>) vol (char **) Volume, either temporary to permanent.
// (>) dsPtr (cDtsDatasetTable *) Pointer to dataset table info.
// (>) chunkNumber (int) Incremental number of the chunk.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To determine where to write the buffer
//
// DESCRIPTION:
// To determine where to write the buffer.  If this is the first chunk then
// it makes the directory to store the files in.  It will also check to see
// if there is enough disk space to write this file.
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

void cDtsPutDs::destinationExtract
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	**fname,		// (mod) Filename to write.
    char	**filePath,		// (mod) Complete filename.
    char	**vol,			// (mod) Volume name.
    int		size,			// (in)  Size to write.
    cDtsDatasetTable
    		*dsPtr,			// (in)  Dataset table information.
    int		chunkNumber		// (in)  Chunk Number.
)
{
    int		spaceAvail;		// Current space avail.
    cDtsStoreManager::tLife		// Temp or perm. storage flag.
    		storeFlag;


    //
    //  Check the status.
    //

    checkStat( status, return );



    //
    //  For SDS always store the chunks in temp.
    //

    checkNull( *filePath = (char *) gen_alloc ( PATH_MAX ), status,
    	    return );

    *vol =  cDtsStoreManager::tempVol();
    if ( chunkNumber == 1 )
    {
	//
	//  This is the first chunk, so make a dir to store it in.
	//

	cDtsStoreManager::makeDir( status, cDtsStoreManager::tempPath(), 
		dsPtr->dstDatasetName );
	checkStat( status, return );
    }


    //
    //  Compose the filePath and fname.
    //

    sprintf( *filePath, "%s/%s/%s.%d", cDtsStoreManager::tempPath(), 
	    dsPtr->dstDatasetName, dsPtr->dstDatasetName, chunkNumber );
    storeFlag =  cDtsStoreManager::ST_TEMP;
    *fname = *filePath + strlen( cDtsStoreManager::tempPath() ) +
	    strlen( dsPtr->dstDatasetName ) + 2;


    //
    //  See if we have enough disk space to write the file.
    //

    spaceAvail = cDtsStoreManager::determineSpace( status, 
	    cDtsStoreManager::ST_TEMP );
    if ( status.ok() && (spaceAvail - ( (size+1023)/1024) )  <  1 ) 
    {
	cDtsStoreManager::updateSpace( status, cDtsStoreManager::ST_TEMP, 0 );
	status.E_NO_SPACE( status, spaceAvail, *fname, ((size+1023)/1024) );
	putStatus.S_NO_SPACE( putStatus, spaceAvail, dsPtr->dstDatasetName  );
	return;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutDs::exec
//
// INVOCATION:
// n.a. This function is called by the put handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called by the put handler to start the processing of a
// put of DS type data request
//
// DESCRIPTION:
// This function is called by the put handler to start the processing of a
// put of DS type data request.
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

void cDtsPutDs::exec
(
)
{
    char	*address = NULL;// Ip address of the client.
    bool	allReceived;	// All data received.
    bool	allSet;		// All data chunks sent.
    bool	badType;	// Bad type match.
    char	*clientName;	// Name of the client.
    int		bufSize;	// Size of the FITS buffer after composition.
    char	*datasetName;	// Datasetname.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Function return status.
    cDtsDatasetTable
    		*dsPtr;		// Pointer to database info.
    char	*filePath;	// Path to where the file is.
    void	*fitsBuffer = NULL;
    				// Fits file buffer.
    char	*fname;		// Filename.
    bool	newName;	// New name generated.
    bool	notContrib;	// Not a contributor.
    int		num;		// Chunk number;
    cDtsDatasetList
    		*pDsList;	// Ptr to new list object.
    cDtsStatus	status;		// Status.
    char	*uniqueName;	// Unique name in the datasetName.
    char	*volName;	// Volume name.


    status.S_PUT_HANDLER( status, type(), label() );
    cdlRecovery = FALSE;
    putStatus.displayStop();


    //
    //  Send a response to the client so it can get on with its life.
    //

    if ( cDtsDhs::simulate() )
    {
	cdnResponseAvList.add( "datasetName", DHS_DT_STRING, label(), 
		dhsStatus );
	status.S_PUT_REQ( status, "N/A", "N/A", label(), "DHS_CS_DONE" );
	response( DHS_CS_DONE, "done", cdnResponseAvList, dhsStatus );
	return;
    }

    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the request.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Check that the label is not empty.
    //

    if ( strlen( label() ) == 0 )
    {
	//
	//  Create a new datasetname and unique name if its empty.
	//

	status.S_LABEL_INVALID( status, sourceName(), sourceAddr() );
	cDtsUniqueList::createUniqueName( status, &datasetName );
	putStatus.S_WARN_NULL_LABEL( putStatus, datasetName );
	initReceived( DTS_DS_NOT_RECEIVED ); 
    }
    else
    {
	datasetName = strsav( label() );
    }


    //
    //  Determine if the segregation dir has been sent in the label.
    //

    setSeg( status, &datasetName );


    //
    //  Verify the unique name, and datasetname.
    //

    cDtsUniqueList::uniqueNameVerify( status, datasetName, &uniqueName );
    pDsList = cDtsDatasetList::datasetNameVerify( status, &datasetName, 
	    uniqueName, sourceName(), type(), length(), allSet, allReceived, 
	    notContrib, badType, newName, last() );
    free( uniqueName );
    if ( allReceived )
    {
	//
	//  All data has been received for this dataset name.
	//

	putStatus.S_DATA_RECEIVED( putStatus );
	status.S_DATA_RECEIVED( status, datasetName, sourceName() );
    }
    else if ( notContrib )
    {
	//
	//  This source is not a listed contributor to the dataset.
	//

	putStatus.S_NOT_CONTRIB ( putStatus );
	status.S_NOT_CONTRIB( status, sourceName(), label() );
    }
    else if ( badType )
    {
	//
	//  Data type set for this dataset does not match.
	//

	char	oldDataType;

	oldDataType = ( dsPtr != NULL )? dsPtr->dstFormat : '?';
	putStatus.S_BAD_TYPE( putStatus, oldDataType, type() );
	status.S_BAD_TYPE( status, datasetName, oldDataType, type() );
    }


    //
    //  Check the last flag, determine where to write the file and write it.
    //

    lastFlagCheck( status, datasetName );
    destinationExtract( status, &fname, &filePath, &volName, length(), 
	    pDsList->getDs(), pDsList->chunkNumber() );
    fileProcess( status, filePath, length(), NULL, FALSE );
    if ( pDsList != NULL && (dsPtr = pDsList->getDs()) != NULL )
    {
	//
	// Update the space listed at the status server.
	//

	if ( dsPtr->dstLifetime == DTS_PERM && type() != DHS_BD_PT_DS )
	{
	    cDtsStoreManager::updateSpace( status, 
		    cDtsStoreManager::ST_PERM, length() );
	}
	else
	{
	    cDtsStoreManager::updateSpace( status, 
		    cDtsStoreManager::ST_TEMP, length() );
	}
    }


    //
    //  Convert to fits and send to Quick Look server.
    //

    qlSend( status, datasetName, allSet, pDsList );
    if ( !status.ok() && dsPtr != NULL )
    {
	//
	//  Set health to sick
	//
	
	dsPtr->dstHealth = DTS_HL_SICK;
    }
    free( filePath );


    //
    //  Get the chunk number.  If all data has been sent, then compose the data
    //  set now, before sending the reply.
    //

    num =  pDsList->chunkNumber();
    status.S_CHUNK2( status, datasetName, num);
    datasetCompose( status, allSet, num, pDsList, &fitsBuffer, &bufSize  ); 
   

    //
    // Update the status in the database if all chunks have been sent or
    // this is the first chunk, or the status is bad.
    //

    if ( allSet || pDsList->chunkNumber() == 1 || !status.ok()  )
    {
	cDtsDatasetList::updateStatus( status, datasetName, pDsList->getDs() );
    }


    //
    //  Parse the fits header.
    //

    char *p = ( char *) putStatus.message();
    bool	warningMsgs;
    warningMsgs = ( p[0] != '\0' ? TRUE : FALSE );


    if ( status.ok() && allSet && status.parseOn() && !warningMsgs )
    {
	cHdrParser	fitsParse;
	cHdrParser::hStatus		hstatus;

	hstatus.displayStop();

	//
	//  Access the buffer, parse the header.
	//

	cDtsPutDs::lockSf();
	fitsParse.parseHeader( hstatus, fitsBuffer, bufSize, datasetName );
	cDtsPutDs::unlockSf();
	if ( !hstatus.ok() )
	{
	    //
	    //  Parse  had errors, give a warning, set health to bad.
	    //

	    dsPtr->dstHealth = DTS_HL_SICK;
	    putStatus.S_HEADER_ERROR( putStatus, hstatus.message() );
	    status.S_HEADER_ERROR( status, datasetName, hstatus.message() );
	    warningMsgs = TRUE;
	}
	else if ( hstatus != hstatus.S_SUCCESS )
	{
	    //
	    //  Warnings parsing header, send back.
	    //

	    putStatus.S_HEADER_WARN( putStatus, hstatus.message() );
	    status.S_HEADER_WARN( status, datasetName, hstatus.message() );
	    warningMsgs = TRUE;
	}
    }

    if ( fitsBuffer != NULL )
    {
	free (fitsBuffer );
    };


    //
    //  Log the event and send a completed response.
    //
   
    dhsStatus = DHS_S_SUCCESS;
    cdnResponseAvList.add( "datasetName", DHS_DT_STRING, datasetName,
	    dhsStatus );

    if ( !status.ok() || warningMsgs )
    {
	if ( newName )
	{
	    //
	    //  A new datasetname was generated because either all the data was 
	    //  received or they were not a contributor.
	    //

	    putStatus.S_NEW_DS_NAME ( putStatus, label(), datasetName );
	    status.S_NEW_DS_NAME ( status, label(), datasetName );
	}
	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_ERROR" );
	response( DHS_CS_ERROR, putStatus.message(), cdnResponseAvList,
		dhsStatus );
    }
    else
    {
	if ( newName )
	{
	    //
	    //  A new datasetname was generated because either all the data was 
	    //  received or they were not a contributor.
	    //

	    putStatus.S_NEW_DS_NAME ( putStatus, label(), datasetName );
	    status.S_NEW_DS_NAME ( status, label(), datasetName );
	    status.display();
	}
	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_DONE");
	response( DHS_CS_DONE, putStatus.message(), cdnResponseAvList,
		dhsStatus );
    }


    //
    //  Notify the oldp and storage servers.
    //

    serverNotify( status, dsPtr );
    pDsList->datasetObjectUnlock();
    free( datasetName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutDs::fileProcess
//
// INVOCATION:
// fileProcess( status, filePath, bufSize);
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) filePath (char *) Complete path where to write the file.
// (>) bufSize (int) Size of the buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To write the data received
//
// DESCRIPTION:
// To write the data received.
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

void cDtsPutDs::fileProcess
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	*filePath,		// (in)  Complete path.
    int		bufSize,		// (in)  Size of the buffer.
    void	*actBuffer,		// (in)  Actual buffer.
    bool	fullFits		// (in)  Is a full fits file.
)
{
    int		actWritten;		// Num bytes actually written.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// Dhs status.
    FILE        *fp;                    // File pointer.
    char	*path;			// Path to write to.
    void	*buf;			// Buffer pointer.
    

    //
    //  If the file is of length zero, return.
    //

    if ( bufSize == 0 )
    {
	return;
    }


    //
    //  Check the status.  If the status is not ok at this point, then
    //  generate a new name and write it there so that at least the data
    //  is somewhere.
    //

    if ( !status.ok() || filePath == NULL )
    {
	putStatus.S_TMP_FILE( putStatus );
	status.E_NOT_WRITTEN( status, label() );
	return;
    }
    else
    {
	path = filePath;
    }
    

    //
    //  Open the file for writing.
    //

    if ( ( fp = fopen( path,  "w" ) ) == NULL )
    {
	status.E_FILE_OPEN( status, path );
	status.sysErrno();
	status.display();
	putStatus.S_FILE_OPEN( putStatus, label() );
	return;
    }


    //
    //  Access the buffer and write the file.
    //

    if ( !fullFits )
    {
	checkDhs( access( dhsStatus ), dhsStatus, status, return );
	buf = buffer();
    }
    else
    {
	buf = actBuffer;
    }

    if ( ( actWritten = fwrite(buf, 1, bufSize, fp ) ) != bufSize )
    {
	status.E_FILE_WRITE( status, path );
	status.sysErrno();
	status.display();
	putStatus.S_FILE_WRITE( putStatus, label() );
	(void) fclose ( fp );
	return;
    }
    (void) fclose ( fp );
    status.S_WROTE_FILE( status, path );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::finalDestin
//
// INVOCATION:
// finalDestin(status,fname,filePath,vol,storeFlag,dsPtr);
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) fname (char **) Filename to write.
// (>) filePath (char **) Complete file and path.
// (>) vol (char **) Volume, either temporary to permanent.
// (!) storeFlag (int &) Storage flag, temp or perm.
// (>) dsPtr (cDtsDatasetTable *) Pointer to dataset table info.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To determine where to write the final fits file
//
// DESCRIPTION:
// To determine where to write the final fits file.
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

void cDtsPutDs::finalDestin
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	**fname,		// (out) Filename.
    char	**filePath,		// (out) Complete filename.
    char	**vol,			// (out) Volume name.
    int		&storeFlag,		// (mod) Storage flag.
    cDtsDatasetTable
    		*dsPtr			// (in)  Dataset table information.
)
{

    //
    //  Check the status.
    //

    checkStat( status, return );



    //
    //  If the lifetime is permanent, then the path is the perm. store
    //  else temporary store.
    //

    checkNull( *filePath = (char *) gen_alloc ( PATH_MAX ), status,
    	    return );

    if ( dsPtr->dstLifetime == DTS_PERM )
    {
	*vol =  cDtsStoreManager::permVol();
	(void) strcpy( *filePath, cDtsStoreManager::permPath() );
	storeFlag =  cDtsStoreManager::ST_PERM;
    }
    else
    {
	*vol =  cDtsStoreManager::tempVol();
	(void) strcpy( *filePath, cDtsStoreManager::tempPath() );
	storeFlag =  cDtsStoreManager::ST_TEMP;
    }


    //
    //  Now append the datasetname + "." + datatype.
    //

    (void) strcat( *filePath, "/" );
    *fname = *filePath + strlen (*filePath );
    (void) strcat( *filePath, dsPtr->dstDatasetName );
    (void) strcat( *filePath, DTS_TAR_EXT );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutDsQl::exec
//
// INVOCATION:
// n.a. This function is called by the put handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called by the put handler to start the processing of a
// put of DS data to quick look only
//
// DESCRIPTION:
// This function is called by the put handler to start the processing of a
// put of DS data to quick look only.
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

void cDtsPutDsQl::exec
(
    void
)
{
    char	*address = NULL;// Ip address of the client.
    char	*datasetName;	// Datasetname.
    char	*clientName;	// Name of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    cDtsStatus	status;		// Function return status.


    status.S_PUT_HANDLER( status, type(), label() );
    putStatus.displayStop();


    //
    //  Send a response to the client so it can get on with its life.
    //

    if ( cDtsDhs::simulate() )
    {
	cdnResponseAvList.add( "datasetName", DHS_DT_STRING, label(), 
		dhsStatus );
	status.S_PUT_REQ( status, "N/A", "N/A", label(), "DHS_CS_DONE" );
	response( DHS_CS_DONE, "done", cdnResponseAvList, dhsStatus );
	return;
    }
    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the request.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Check that the label is not empty.
    //

    if ( strlen( label() ) == 0 )
    {
	//
	//  Create a new datasetname and unique name if its empty.
	//

	status.S_LABEL_INVALID( status, sourceName(), sourceAddr() );
	cDtsUniqueList::createUniqueName( status, &datasetName );
	putStatus.S_WARN_NULL_LABEL( putStatus, datasetName );
    }
    else
    {
	datasetName = strsav( label() );
    }


    //
    //  Convert to Fits and send to Quick Look server.
    //

    qlSend( status, datasetName, last(), NULL );


    //
    //  Log the event and send a completed response.
    //

    dhsStatus = DHS_S_SUCCESS;
    cdnResponseAvList.add( "datasetName", DHS_DT_STRING, datasetName,
	    dhsStatus );

    if ( !status.ok() )
    {
	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_ERROR" );
	response( DHS_CS_ERROR, putStatus.message(), cdnResponseAvList,
		dhsStatus );
    }
    else
    {
	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_DONE");
	response( DHS_CS_DONE, putStatus.message(), cdnResponseAvList,
		dhsStatus );
    }

    free( datasetName );
}
