static char rcsid[] = "$Id: get.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/get.C
//
// PURPOSE:
// Handles all bulk data get requests.
//
// METHOD NAME(S)
// cDtsDhsGet::asisRead()   : Read in a buffer as it is stored.
// cDtsDhsGet::decompressRead() : Decompress a file into a buffer.
// cDtsDhsGet::exec()       : Execution point for a get request.
// cDtsDhsGet::fileReadAlone()  : Read in a file, in standalone mode
// cDtsDhsGet::fileRead()   : Read in a file
// cDtsDhsGet::fitsCheck()  : Check that the file is fits.
// cDtsDhsGet::headerRead() : Read in the header only
// cDtsDhsGet::requestRemotely(): Request a file remotely.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:06  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2000/07/27 20:10:03  dunn
// Added standalone features.
//
// Revision 1.7  1998/06/22 20:46:51  dunn
// Added check from strPtr to be null before accessing it.
//
// Revision 1.6  1998/05/25 20:04:05  dunn
// Fixed debug status message and added a reobutsness to returnFname
//
// Revision 1.5  1998/05/04 23:00:03  dunn
// Added the return of an avlist with filename and compression flag.
//
// Revision 1.4  1998/03/06 17:34:35  cockayne
// Modified to return (in addition to the file itself) the name of the file,
// on response to a get.
//
// Revision 1.3  1997/08/29 23:02:13  dunn
// Fixed a lot of leaks.
//
// Revision 1.2  1997/07/29 22:01:52  dunn
// Added lock for the press lib, and
// Fixed it if dhsConnectInfo did not return name and address
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
#include "genMutex.H"

#include "globals.H"
extern "C"
{
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sybfront.h>
#include <sybdb.h>
#include <cstring>
 
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "gen_eptr.h"

#include "db.h"
#include "ad.h"
#include "press.h"
}

#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "con.H"
#include "store.H"
#include "get.H"
#include "dhsGenConnect.H"

//
//  Local defines.
//

cMutex	cDtsDhsGet::cdmMutexPr;

#define DTS_MIN_LINE_LEN	8*sizeof( char )
#define DTS_MAX_LINE_LEN	80*sizeof( char )
#define DTS_END_HDU		"END     "
#define DTS_SIMPLE_LEN		6
#define DTS_SIMPLE_STR		"SIMPLE"

msgText( cDtsDhsGet::gStatus, S_FILENAME, P_NULL,
	    "File name is : %s\n" );
msgText( cDtsDhsGet::gStatus, E_INV_COMP, P_NULL,
	    "Error: File type requested (%d) does not match compression info(%c)." );
msgText( cDtsDhsGet::gStatus, E_INV_MED, P_NULL,
	    "Error: Invalid media type requested(%d) for dataset: `%s'." );
msgText( cDtsDhsGet::gStatus, E_INV_TYPE, P_NULL,
	    "Error: File type requested (%d) does not match stored type(%c)." );
msgText( cDtsDhsGet::gStatus, E_NO_HEADER, P_NULL,
	    "Error: Cannot find end of fits header for : `%s'." );
msgText( cDtsDhsGet::gStatus,  E_NO_FILE, P_NULL,
    "Error: File does not exist given the path on the archive for dataset: `%s'." );
msgText( cDtsDhsGet::gStatus,  E_NOT_FITS, P_NULL,
    "Error: File type of fits requested, file is not a fits file : `%s'." );
msgText( cDtsDhsGet::gStatus,  E_FILE_UNAVAIL, P_NULL,
    "Error: File `%s' not available %s." );
msgText( cDtsDhsGet::gStatus,  E_UNCOMP_FAIL, P_NULL,
    "Error: File `%s' did not uncompress correctly." );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::asisRead
//
// INVOCATION:
// asisRead()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To read a file in as is.
//
// DESCRIPTION:
// To read a file in as is.
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

void cDtsDhsGet::asisRead
(
    cDtsStatus	&status,		// (mod) Status passed in.
    void	**buffer,		// (mod) The data may be uncomp.,
    char	*filename,		// (in)  Filename.
    int		&nbytes			// (mod) Number of bytes in the buffer.
)
{
    int		sizeRead;	// Size read into the buffer.
    FILE	*fp;		// fp of the file.


    //
    //  Check the status.
    //

    checkStat( status, return );
    sizeRead = 0;

    //
    //  Open the file for reading.
    //

    if ( ( fp = fopen( filename, "r" ) ) == NULL ) 
    {
	status.E_NO_FILE( status, filename );
	status.sysErrno();
	status.display();
	getStatus.E_NO_FILE( getStatus, filename );
	getStatus.sysErrno();
	return;
    }


    //
    //  If type is HEADER, then only read in the header, else read all of it.
    //

    if ( type() == DHS_BD_GT_FITS_HEADER )
    {
	headerRead( status, buffer, fp, nbytes );
    }
    else
    {

	//
	//  Alloc room and read in the file.
	//

	*buffer = gen_alloc( (nbytes + 1) );
	sizeRead = fread( *buffer, nbytes, 1, fp );

	if ( sizeRead < 1 )
	{
	    //
	    // Nothing read.  Or read error.
	    //
	    
	    status.E_NO_FILE( status, label() );
	    getStatus.E_NO_FILE( getStatus, label() );
	    status.display();
	}

    }
    (void) fclose( fp );
} 

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::decompressRead
//
// INVOCATION:
// decompressRead()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To decompress a file as reading it in.
//
// DESCRIPTION:
// To decompress a file as reading it in.
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

void cDtsDhsGet::decompressRead
(
    cDtsStatus	&status,		// (mod) Status passed in.
    void	**buffer,		// (mod) The data may be uncomp.,
    char	*filename,		// (in)  Filename.
    int		&nbytes,		// (mod) Number of bytes in the buffer.
    void	**newBuffer,		// (out) Used from mem to mem decomp.
    bool	isFile			// (in)  Indicates decompress from file.
)
{
    int		fd;		// file descriptor.
    int		newBufferSize;	// New buffer size.


    //
    //  Check the status.
    //

    checkStat( status, return );
    newBufferSize = nbytes;


    //
    //  Open the file for reading, if filename is not null.
    //

    if ( isFile  && ( fd = open( filename, O_RDONLY ) ) == 0 ) 
    {
	status.E_NO_FILE( status, filename );
	getStatus.E_NO_HEADER( getStatus, filename );
	return;
    }


    //
    //  Uncompress either from file or from a buffer is the filename is null.
    //

    cdmMutexPr.lock();
    if ( isFile )
    {
	//
	//  Uncompress from a file to memory and then close the file.
	//

	if ( ( unpress_f2m( fd, (byte **) buffer, &nbytes, 
	    cDtsDhs::compressType() ) ) != PR_SUCCESS )
	{		
	    status.E_UNCOMP( status, pr_msg );
	    getStatus.E_UNCOMP_FAIL( getStatus, filename );
	}
	(void) close( fd );
    }
    else if ( !isFile )
    {
	//
	//  If decompression type is gzip, then we can guess the output
	//  buffer size.
	//
	
	if ( streq( cDtsDhs::compressType(), "GZIP" ) )
	{
	    int		tmp;
	    
	    if ( unpress_msize( (byte *) *buffer, nbytes,
		    &tmp, cDtsDhs::compressType() ) == PR_SUCCESS )
	    {
		newBufferSize = tmp;
	    }
	}


	//
	//  Unpress the buffer.
	//

	if ( ( unpress_m2m( (byte *)*buffer, nbytes, (byte **) newBuffer, 
		    &newBufferSize, cDtsDhs::compressType() ) ) != PR_SUCCESS )
	{		
	    status.E_UNCOMP( status, pr_msg );
	    getStatus.E_UNCOMP_FAIL( getStatus, filename );
	}

	nbytes = newBufferSize;
    }

    cdmMutexPr.unlock();


    //
    //  If type is header, then strip off the header.
    //

    if ( type() == DHS_BD_GT_FITS_HEADER )
    {
	headerRead( status, buffer, NULL, nbytes );
    }


} 

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::exec
//
// INVOCATION:
// n.a. This function is called by the get handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called by the get handler to start the processing of a
// request
//
// DESCRIPTION:
// Invent some data to send back to the client.
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

void cDtsDhsGet::exec
(
    void
)
{
    char		*address = NULL;
    					// Ip address of the client.
    void		*buffer = NULL;	// Data buffer to send to the client
    char		*clientName = NULL;
    					// Name of the client.
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
					// Function return status.
    int			dbSize;		// Length of the buffer sent back.
    char		*filename = NULL;	
					// Name of file to return to client.
    cDtsStatus		status;


    
    status.S_GET_HANDLER( status, type(), label() ); 
    pGetRequest = NULL;
    remoteRead = FALSE;
    cdmCompression = AD_COMP_UNKNOWN;
    getStatus.displayStop();


    //
    //  Send a response to the client so it can get on with its life.
    //

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
	status.E_LABEL_INVALID( status, sourceName(), sourceAddr() );
    }


    //
    //  Read the File into a buffer.
    //

    if ( status.standAlone() )
    {
	fileReadAlone( status, &filename, &buffer, dbSize );
    }
    else
    {
	fileRead( status, &filename, &buffer, dbSize );
    }


    //
    //  Log the event and send a completed response.
    //

    cdnResponseAvList.add( "compression", DHS_DT_CHAR, cdmCompression,
	    dhsStatus );
   
    if ( !status.ok() )
    {
	cdnResponseAvList.add( "fileName", DHS_DT_STRING, label(), 
		dhsStatus );
	status.S_GET_REQ( status, sourceName(), sourceAddr(), label(), 
		"DHS_CS_ERROR" );
	response( DHS_CS_ERROR, getStatus.message(), cdnResponseAvList, 
		dhsStatus );
    }
    else
    {
	cdnResponseAvList.add( "fileName", DHS_DT_STRING, filename, 
		dhsStatus );
	getStatus.S_FILENAME( getStatus, filename );
	status.S_GET_REQ( status,  sourceName(), sourceAddr(), label(), 
		"DHS_CS_DONE");
	response( DHS_CS_DONE, getStatus.message(), cdnResponseAvList, type(), 
		buffer, dbSize, dhsStatus );
    }


    if ( pGetRequest != NULL )
    {
	delete pGetRequest;
    }

    if ( !remoteRead )
    {
	free( buffer );
	buffer = NULL;
    }

    gen_free( filename  );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::fileReadAlone
//
// INVOCATION:
// fileReadAlone( status, &filename, &buffer, dbSize );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (<) filename (cDtsStatus &) Function return status.
// (<) buffer (void **) The data read in.
// (<) dbSize (int &) Length of the data buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To locate from the internal list where the file can be found.
//
// DESCRIPTION:
// Return the path to where the file can be found, also set the native
// type of the file.  This is only used in standalone mode.
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

void cDtsDhsGet::fileReadAlone
(
    cDtsStatus		&status,	// (mod) Status passed in.
    char		**returnFname,	// (out) Name of file. 
    void		**buffer,	// (out) The data read in.
    int			&dbSize		// (out) Length of the data buffer.
)
{
    cDtsDatasetList
    			*pListPtr;	// Pointer to item in list.
    struct stat 	stat_buf;	// Output of the stat call.


    //
    //  Check the status and initialize buffer size to 0.
    //

    checkStat( status, return );
    dbSize = 0;


    //
    //  Search the list for information about this dataset.
    //

    pListPtr =  cDtsDatasetList::datasetNameFind( status, (char*)label() );

    if ( status.ok() && status != status.S_NOT_IN_DS_LIST )
    {

	cDtsDatasetTable	*dsPtr = NULL;
	pListPtr->datasetObjectLock();
	dsPtr = pListPtr->getDs();


	//
	//  First check that all the data has been received.
	//

	if ( dsPtr->dstReceived != DTS_DS_ALL_RECEIVED )
	{
	    status.E_NOT_RECVD( status, dsPtr->dstReceived, label() );
	    getStatus.E_FILE_UNAVAIL( getStatus, label() );
	    pListPtr->datasetObjectUnlock();
	    return;
	}


	//
	//  Second check the data type requested against the type stored.
	//

	if ( dsPtr->dstFormat != AD_FMT_FITS  && 
		( type() == DHS_BD_GT_FITS || type() == DHS_BD_GT_FITS_ASIS ||
		type() == DHS_BD_GT_FITS_HEADER ) ) 
	{
	    status.E_INV_TYPE( status, type(), label() );
	    getStatus.E_INV_TYPE( getStatus, type(), dsPtr->dstFormat );
	    pListPtr->datasetObjectUnlock();
	    return;
	}


	//
	// Save filename for return to client.
	//

	*returnFname = strsav( (strtail( pListPtr->datasetPath() ) ) );
		

	//
	//  Do a stat on the file to make sure it exists and get the size.
	//

	if ( stat( pListPtr->datasetPath(), &stat_buf ) == -1 )
	{
	    //
	    //  File doesn't exist. Error out.
	    //

	    status.E_NO_FILE( status, pListPtr->datasetPath() );
	    getStatus.E_NO_HEADER( getStatus, *returnFname );
	    pListPtr->datasetObjectUnlock();
	    return;
	}
	dbSize = stat_buf.st_size;

	
	//
	//  If the file type is AS_IS OR not compressed then
	//  read in entire thing and send it.
	//

	if ( pListPtr->datasetCompression() == AD_COMP_UNKNOWN || 
		( type() == DHS_BD_GT_FITS_ASIS || 
		type() == DHS_BD_GT_RAW_ASIS ) )
	{
	    cdmCompression = pListPtr->datasetCompression();
	    asisRead( status, buffer, pListPtr->datasetPath(), dbSize );
	    
	}

	//
	//  else If file is compressed and type !AS_IS then
	//  decompress read.
	//

	else if ( pListPtr->datasetCompression() != AD_COMP_UNKNOWN && 
		!( type() == DHS_BD_GT_FITS_ASIS ||
		type() == DHS_BD_GT_RAW_ASIS ) )
	{
	    cdmCompression = AD_COMP_UNKNOWN;
	    decompressRead( status, buffer, pListPtr->datasetPath(), 
		    dbSize, NULL, TRUE );

	}
	else
	{
	    status.E_NO_READ( status, pListPtr->datasetPath() );
	    status.display();
	    getStatus.E_INV_COMP( getStatus, type(), 
	    	pListPtr->datasetCompression() ); 
	    pListPtr->datasetObjectUnlock();
	    return;
	}


	//
	//  If the type is fits and NOT AS_IS (because it may be compressed),
	//  then check that the first word is simple.
	//

	fitsCheck( status, buffer, dbSize, AD_COMP_UNKNOWN );
	pListPtr->datasetObjectUnlock();

    }
    else
    {
	status.E_FILE_UNAVAIL( status, label(), sourceName(), sourceAddr(),
		"locally" );
	getStatus.E_FILE_UNAVAIL( getStatus, label(), "locally" );
    }
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::fileRead
//
// INVOCATION:
// fileRead( status, &filename, &buffer, dbSize );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (<) filename (cDtsStatus &) Function return status.
// (<) buffer (void **) The data read in.
// (<) dbSize (int &) Length of the data buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To locate from disk or database where a file_id can be found.
//
// DESCRIPTION:
// Return the path to where the file can be found, also set the native
// type of the file.
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

void cDtsDhsGet::fileRead
(
    cDtsStatus		&status,	// (mod) Status passed in.
    char		**returnFname,	// (out) Name of file. 
    void		**buffer,	// (out) The data read in.
    int			&dbSize		// (out) Length of the data buffer.
)
{
    char		*medFilename;	// Filename on the medium.
    struct stat 	stat_buf;	// Output of the stat call.
    struct ad_file	adFile;		// Ad file structure.
    int			estatus;	// Ad lib status.
    cDtsDbManager	adDbInfo;	// Ad and db information.
    DBPROCESS		*dbProc;	// Database connection.


    //
    //  Check the status and initialize buffer size to 0.
    //

    checkStat( status, return );
    dbSize = 0;
    dbProc = adDbInfo.getDbConn(adDbInfo.DB_AD_LIB);


    //
    //  Query for this datasetName locally.
    //

    estatus = ad_file_get( AD_FG_NAME, &adFile, dbProc, AD_MED_BEST, 
	    NULL, (const char *) label() );


    if ( estatus == AD_NO_FILE )
    {
	//
	//  Request remotely, only if data type is NOT ASIS and the source
	//  of the request is not the storage server.  ASIS means its
	//  coming from a remote dataServer so we don't want to request 
	//  remotely because it may become circular gets.  Storage server
	//  can also ask ASIS.
	//

	if ( ( type() == DHS_BD_GT_RAW_ASIS || type() == DHS_BD_GT_FITS_ASIS )
		&& !streq( sourceName(), cDtsDhs::storageServer() ) )
	{
	    status.E_FILE_UNAVAIL( status, label(), sourceName(), sourceAddr(),
		    "locally" );
	    getStatus.E_FILE_UNAVAIL( getStatus, label(), "locally" );
	    adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );
	    return;
	}
	requestRemotely( status, returnFname, buffer, dbProc, 
		&adFile, estatus, dbSize );

	if ( !status.ok() )
	{
	    //
	    //  No file was found, return an error response.
	    //

	    adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );
	    getStatus.E_FILE_UNAVAIL( getStatus, label(), "locally" );
	    return;
	}
    }
    if ( estatus != AD_SUCCESS )
    {
	status.E_AD( status, ad_msg );
	getStatus.E_FILE_UNAVAIL( getStatus, label(), "locally or remotely" );
	adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );
	return;
    }

    //
    //  Release the db Connection.
    //

    adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );


    //
    //  Read in the file if available locally, otherwise already in buffer.
    //

    if ( adFile.adf_volume_medium != AD_MED_REMOTE )
    {
	AD_PATH	filename;	// Actual path and filename.
	char	*path;		// Path to the file.



	//
	//  First check the data type requested against the type stored.
	//

	if ( adFile.adf_format != AD_FMT_FITS  && 
		( type() == DHS_BD_GT_FITS || type() == DHS_BD_GT_FITS_ASIS ||
		type() == DHS_BD_GT_FITS_HEADER ) ) 
	{
	    status.E_INV_TYPE( status, type(), adFile.adf_file_id );
	    getStatus.E_INV_TYPE( getStatus, type(), adFile.adf_format );
	    return;
	}

	
	//
        //  Determine the path and filename.
	//

	switch( adFile.adf_volume_medium ) 
	{
	    case AD_MED_CD:
		path = adFile.adf_location.cd.cdl_mount_point;
		medFilename = adFile.adf_location.cd.cdl_filename;
		break;

	    case AD_MED_MD:
		path = adFile.adf_location.md.mdl_mount_point;
		medFilename = adFile.adf_location.md.mdl_filename;
		break;

	    case AD_MED_OD:
	    case AD_MED_DB:
	    default:
		//
		//  Not implemented at this time.
		//

		status.E_INV_MED( status, adFile.adf_file_id );
		getStatus.E_INV_MED( getStatus, adFile.adf_volume_medium,
				label() );
		return;
	}



	//
	//  Compose the filename with path. 
	//

	(void) strcpy( filename, path );
	(void) strcat( filename, "/" );
	(void) strcat( filename, medFilename );


	//
	// Save filename for return to client.
	//

	*returnFname = strsav( medFilename );
		

	//
	//  Do a stat on the file to make sure it exists and get the size.
	//

	if ( stat( filename, &stat_buf ) == -1 )
	{
	    //
	    //  File doesn't exit. Error out.
	    //

	    status.E_NO_FILE( status, filename );
	    getStatus.E_NO_HEADER( getStatus, filename );
	    return;
	}
	dbSize = stat_buf.st_size;

	
	//
	//  If the file type is AS_IS OR not compressed then
	//  read in entire thing and send it.
	//

	if ( adFile.adf_compression == AD_COMP_UNKNOWN || 
		( type() == DHS_BD_GT_FITS_ASIS || 
		type() == DHS_BD_GT_RAW_ASIS ) )
	{
	    cdmCompression = adFile.adf_compression;
	    asisRead( status, buffer, filename, dbSize );
	    
	}
	//
	//  else If file is compressed and type !AS_IS then
	//  decompress read.
	//
	else if ( adFile.adf_compression != AD_COMP_UNKNOWN && 
		!( type() == DHS_BD_GT_FITS_ASIS ||
		type() == DHS_BD_GT_RAW_ASIS ) )
	{
	    cdmCompression = AD_COMP_UNKNOWN;
	    decompressRead( status, buffer, filename, dbSize, NULL, TRUE );

	    //
	    //  Strip the compression extension off the filename
	    //

	    char 	*strPtr = NULL;
	    strPtr = strstr( *returnFname, cDtsDhs::compressExtension() );
	    if ( strPtr != NULL )
	    {
		*strPtr = '\0';
	    }

	}
	else
	{
	    status.E_NO_READ( status, filename );
	    status.display();
	    getStatus.E_INV_COMP( getStatus, type(), adFile.adf_compression );
	    return;
	}
    }


    //
    //  If the type is fits and NOT AS_IS (because it may be compressed),
    //  then check that the first word is simple.
    //

    fitsCheck( status, buffer, dbSize, adFile.adf_compression );
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::fitsCheck
//
// INVOCATION:
// fitsCheck()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To check that the first word is "SIMPLE"
//
// DESCRIPTION:
// To check that the first word is "SIMPLE"
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

void cDtsDhsGet::fitsCheck
(
    cDtsStatus	&status,		// (mod) Status passed in.
    void	**buffer,		// (in)  The data to send back.
    int		nbytes,			// (in)  Number of bytes in the buffer.
    char	compFlag		// (in)  Compression flag.
)
{
    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Check that the first word is simple if type if fits if the type is
    //  suppose to be fits and the file is not compressed.
    //

    if ( ( type() == DHS_BD_GT_FITS || type() == DHS_BD_GT_FITS_HEADER
	    || ( type() == DHS_BD_GT_FITS_ASIS && compFlag == AD_COMP_UNKNOWN ))
	    && 
	    (  nbytes > DTS_SIMPLE_LEN ) 
	    && 
	    ( strncmp( (char *)*buffer, DTS_SIMPLE_STR, DTS_SIMPLE_LEN ) != 0 ))
    {
	status.E_NOT_FITS( status, label() );
	getStatus.E_NOT_FITS( getStatus, label() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::headerRead
//
// INVOCATION:
// headerRead()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To make the buffer to send back only point to the header.
//
// DESCRIPTION:
// To make the buffer to send back only point to the header.
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

void cDtsDhsGet::headerRead
(
    cDtsStatus	&status,		// (mod) Status passed in.
    void	**buffer,		// (mod) The data may be uncomp.,
    FILE	*fp,			// (in)  File pointer.
    int		&nbytes			// (mod) Number of bytes in the buffer.
)
{
    boolean	found = FALSE;	// Found end of header.
    int		totalRead;	// Total number of bytes read.
    

    //
    //  Check the status and init total read number of bytes.
    //

    checkStat( status, return );
    totalRead = 0;


    //
    //  If data is coming from a file, then
    //

    if ( fp != NULL )
    {
	int	sizeRead;	// Size read into the buffer.
	char	*aLine;		// At most, 80 charater pointer.


	//
	//  Allocate room for a single line, then start reading till hit END.
	//

	aLine = ( char * ) gen_alloc( 80 * sizeof( char ) );
	while ( 1 )
	{
	    //
	    //  Read in 80 characters.
	    //

	    sizeRead  = 0;
	    if ( ( sizeRead  = fread( aLine, 1, DTS_MAX_LINE_LEN, fp ) )
		    == 0 )
	    {
		//
		//  Problem reading in an entire line, stop.
		//

		break;
	    }

	    totalRead += sizeRead;


	    //
	    //  If read in at least 8 characters and those 8 match END, break.
	    //

	    if ( sizeRead  >= DTS_MIN_LINE_LEN &&
		    strncmp( aLine, DTS_END_HDU, DTS_MIN_LINE_LEN ) == 0 )
	    {
		found = TRUE;
		break;
	    }
	}

	free ( aLine );
	if ( found )
	{
	    (void) rewind ( fp );
	    sizeRead = 0;
	    *buffer = gen_alloc( totalRead );
	    sizeRead  = fread( *buffer, totalRead, 1, fp );
	    nbytes = totalRead;

	    if ( sizeRead < 1 )
	    {
		//
		// Nothing read.  Or read error.
		//
		
		status.E_NO_FILE( status, label() );
		getStatus.E_NO_HEADER( getStatus, label() );
	    }
	}
	else
	{
	    status.E_NO_HEADER( status, label() );
	    getStatus.E_NO_HEADER( getStatus, label() );
	}
    }
    else
    {
	//
	//  Else the file is in the buffer already, so find END in there.
	//

	char	*tmpPtr;		// Temporary pointer into buffer.
	int	numRead;		// Number of characters read.


	tmpPtr = ( char * ) *buffer;
	numRead = 0;
	
	while ( 1 )
	{
	    
	    if ( ( totalRead + DTS_MIN_LINE_LEN ) >  nbytes )
	    {
		//
		//  Problem reading an entire line, stop.
		//

		break;
	    }

	    //
	    //  Calculate how much of the line we can/did read.
	    //

	    if ( ( totalRead + DTS_MAX_LINE_LEN ) <= nbytes )
	    {
		numRead = DTS_MAX_LINE_LEN;
		totalRead += DTS_MAX_LINE_LEN;
	    }
	    else
	    {
		numRead = nbytes - totalRead;
		totalRead = nbytes;

	    }

	    if ( strncmp( tmpPtr, DTS_END_HDU, DTS_MIN_LINE_LEN ) == 0 )
	    {
		found = TRUE;
		break;
	    }
	    tmpPtr += numRead;
	}
	
	if ( found )
	{
	    //
	    //  Found the end of the header, reset the length of the buffer
	    //  to be the position of the end buffer.
	    //

	    nbytes = totalRead;
	}
	else
	{
	    status.E_NO_HEADER( status, label() );
	    getStatus.E_NO_HEADER( getStatus, label() );
	}
    }
} 

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsGet::requestRemotely
//
// INVOCATION:
// n.a. This function is called by exec.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To request for a file remotely.
//
// DESCRIPTION:
// To request for a file remotely.
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

void cDtsDhsGet::requestRemotely
(
    cDtsStatus		&status,	// (mod) Status passed in.
    char		**returnFname,	// (out) Name of file.
    void		**data,		// (out) The data read in.
    DBPROCESS		*dbProc,	// (mod) Database connection.
    struct ad_file	*adFile,	// (mod) Ad file structure.
    int			&estatus,	// (mod) Ad lib status.
    int			&bufSize	// (mod) Buffer size.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    DHS_BD_GET_TYPE
    		newType;		// get type.
    bool	notFound;		// Found the data flag.
    cDhsConnection
    		*pConn;			// Remote connection.
    char	*returnedString;	// Ptr to returned string.
    AD_REMOTEGET
    		selectType;		// Select from ad type.


    //
    //  Check the status.
    //

    checkStat( status, return );
    notFound = TRUE;
    selectType = AD_RG_FILE_ID;



    while( notFound )
    {

	//
	//  Look up in the db remotely.

	estatus = ad_remote_get( selectType, adFile, dbProc, (char *)
		label() );
	if ( estatus == AD_NO_FILE )
	{
	    //
	    //  No file was found, log it and return an error response.
	    //

	    status.E_FILE_UNAVAIL( status, label(), sourceName(), sourceAddr(),
		    "locally or remotely");
	    getStatus.E_FILE_UNAVAIL(getStatus, label(), "locally or remotely");
	    break;
	}
	else if ( estatus != AD_SUCCESS )
	{
	    //
	    //  Else check that an error did not occur.
	    //

	    status.E_AD( status, ad_msg );
	    getStatus.E_FILE_UNAVAIL(getStatus, label(), "locally or remotely");
	    break;
	}

	//
	//  Check the data type requested against the type stored.
	//

	if ( adFile->adf_format != AD_FMT_FITS  && 
		( type() == DHS_BD_GT_FITS || type() == DHS_BD_GT_FITS_ASIS ||
		type() == DHS_BD_GT_FITS_HEADER ) ) 
	{
	    status.E_INV_TYPE( status, type(), adFile->adf_file_id );
	    getStatus.E_INV_TYPE( getStatus, type(), adFile->adf_format );
	    break;
	}

	selectType = AD_RG_NEXT;


	//
	//  Try connecting to this host.
	//

	if ( cDtsConManager::connectTo( status, 
		adFile->adf_location.rs.rsl_server_id, &pConn ) )
	{

	    status.S_REQUEST_REMOTELY( status, label(), 
		    adFile->adf_location.rs.rsl_server_id );


	    //
	    //  Check to see if we need a new type.
	    //

	    if (  adFile->adf_compression == AD_COMP_UNKNOWN ||
		    type() == DHS_BD_GT_FITS_ASIS ||  
		    type() == DHS_BD_GT_RAW_ASIS  ||
		    type() == DHS_BD_GT_FITS_HEADER )
	    {
		newType = type();
	    }
	    else if ( type() == DHS_BD_GT_FITS )
	    {
		newType = DHS_BD_GT_FITS_ASIS;
	    }
	    else if ( type() == DHS_BD_GT_RAW )
	    {
		newType = DHS_BD_GT_RAW_ASIS;
	    }
		

	    //
	    //  Request the data and wait for it.
	    //

	    DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;

	    pGetRequest = new cDhsGetDataset();
	    checkDhs( pGetRequest->get( *pConn, label(), newType, 
		    dhsStatus), dhsStatus, status, VOID );
	    checkDhs( pGetRequest->wait( dhsStatus ), dhsStatus, status, VOID );
	    checkDhs( (cmdStat = pGetRequest->status( dhsStatus )), dhsStatus, 
		    status, VOID );


	    if ( cmdStat == DHS_CS_DONE && status.ok() )
	    {
		//
		// Access the data and determine if it needs to be stripped 
		// or uncomp.
		//

		bufSize = pGetRequest->length();
		notFound = FALSE;

		
		//
		// Save the file name to return to client.
		//
		cDhsAttrib	attrib;

		pGetRequest->status( returnedString, dhsStatus );
		attrib = pGetRequest->find( "fileName", dhsStatus );
		if ( dhsStatus == DHS_S_SUCCESS)
		{
		    void *x = NULL;
		    attrib.info( &x, dhsStatus);
		    if ( dhsStatus == DHS_S_SUCCESS)
		    {
			*returnFname = strsav( (char *) x );
		    }
		}
		else
		{
		    dhsStatus = DHS_S_SUCCESS;
		    pGetRequest->print( dhsStatus );
		    *returnFname = strsav( "Unknown" );
		}

		
		//
		//  If file is compressed and type !AS_IS then
		//  decompress read.
		//

		if(  newType != type() )
		{
		    char	*strPtr = NULL;		// Pointer into string.
		    void	*tmpBuf = NULL;
		    tmpBuf = pGetRequest->buffer();
		    cdmCompression = AD_COMP_UNKNOWN;
		    decompressRead( status, &tmpBuf, 
			    (char *)label(), bufSize, data, FALSE );

		    //
		    //  Strip the compression extension off the filename
		    //

		    strPtr = strstr( *returnFname, 
		    	cDtsDhs::compressExtension() );
		    if ( strPtr != NULL )
		    {
			*strPtr = '\0';
		    }
		}
		else
		{
		    cdmCompression = adFile->adf_compression;
		    *data = pGetRequest->buffer();
		    remoteRead = TRUE;
		}
	    }
	    else
	    {
		//
		//  Get request failed, may be serious, stop.
		//

		getStatus.E_FILE_UNAVAIL( getStatus, label(), 
			"locally or remotely" );
		status.E_GET_FAILED( status, adFile->adf_file_id,
			adFile->adf_location.rs.rsl_server_id );
		break;
	    }
	}
    }
}
