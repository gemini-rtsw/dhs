/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	mfsIngest/src/ingest.c
*
*   Purpose:
*	Contains the routines to ingest a MFS.
*
*   Routines:
*	int		ingest		: The ingest procedure.
*	static int	ingestVolume	: Ingests a volume.
*	static int	ingestFile	: Ingests files on a volume.
*
*   Date		: Mar 10, 1999
*
*   SCCS data		: @(#)
*	Module Name	: ingest.c
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 05/24/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/03/10 SDJ	: Copied from mfsIngest.
*	99/10/14 SDJ	: Changed gUpper to tri-state AD_CASE value.
*     2000/03/27 SDJ    : Updated to new CADC software standards.  Switched
*                         from dvdIngest to mfsIngest.  Use volume CRC to
*                         retrieve info rather than name.  Added prompt to
*                         determine vol. type if not already given.
*			  Removed onControlC().  Changed logic of ingest,
*			  only read from the database once, at this point
*			  it determines whether we are re-ingesting.
*     2000/04/05 SDJ	: Added ad_volume_init() to initialize the volume
*			  structure (should be added to cdIngest probably).
*			  Updated call to ad_volume_get do to changes in
*			  the ad library.
*     2000/04/10 SDJ	: Switched mfs volume crc from int to binary data
*			  type and used new functions in crc, and db
*			  libs for crcs.  Added MFS_ISO9660_JB since
*			  CD/DVD in jukeboxes are handled different.
*			  New volume prompt is shown only when no volumes
*			  are given on cmd-line and not in batch mode.
*	2000/05/04 sjg	: Added log file open code and fixed other parts.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/
/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>


/*
 ************************************************************************
 *  SYBASE DB library include files.
 ************************************************************************
 */

#include <sybfront.h>
#include <sybdb.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "press.h"
#include "db.h"
#include "mfs.h"
#include "crc.h"
#include "ad.h"


/*
 ************************************************************************
 *  Local definitions
 ************************************************************************
 */ 

#define LOGNAME_FORMAT		"mfsIngest.%s.log"


/*
 ************************************************************************
 *  mfsIngest include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Local function prototypes
 ************************************************************************
 */
 
static int	ingestDirectory( char * );
static int	ingestVolume( void );
static int	ingestFile( char *, struct stat * );

/*+
************************************************************************
*
*   Synopsis:
*	int	ingest()
*
*   Purpose:
*	The actual ingest procedure.
*
*   Parameters:
*	None.
*
*   Values Returned:
*	int	S_SUCCESS	: All went well.
*	int	S_QUIT		: Quit at users request.
*	int	E_AD		: Error in the ad library.
*	int	E_DB_LIB	: Error in the cadc db library.
*	int	E_LOG		: Log file error.
*	int	E_PATH_LEN	: Path is longer than the alocated length.
*	int	E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

int	ingest()
{
    int		currentPath;	/* Current path being checked.		*/
    boolean	done;		/* Done ingesting?			*/
    char	logname[PATH_MAX];	/* Log file name.		*/
    boolean	promptType;	/* TRUE if need to prompt for vol. type.*/
    int		status;		/* Function return status.		*/
    AD_VOLUME	volume;		/* Volume information.			*/


    currentPath = 0;
    done 	= FALSE;
    promptType	= FALSE;
    
    
    do
    {
	/*
	 * Initialize the volume information.
	 */

	CHECK_AD( ad_volume_init( &volume, AD_MED_MFS ) );
	
		
	/* 
	 *  Get a suitable volume.
	 */

	if ( globals.gPathList == NULL )
	{
	    CHECK( volumeLoad() );
	}
	else
	{
	    if ( currentPath + 1 > eptrlen( globals.gPathList ) )
	    {
		done = TRUE;
		continue;
	    }


	    (void) strcpy( globals.gMountPoint, 
		    eptritem( globals.gPathList, currentPath ) );
	    currentPath++;
	    
	    if ( mfs_check_mnt( globals.gMountPoint, globals.gVolumename,
		    &globals.gCreateDate, globals.gMfsFormat ) != MFS_SUCCESS )
	    {
		printMessage( mfs_get_message(), MESS_ERROR );
		continue;
	    }

	}


	/*
	 * Open the log file.
	 */

	if ( globals.gLogFp != NULL )
	{
	    (void) fclose( globals.gLogFp );
	}

	(void) sprintf( logname, LOGNAME_FORMAT, globals.gVolumename );

	if ( ( globals.gLogFp = fopen( logname, "w" ) ) == NULL )
	{
	    formatMessage( MSG_ERRNO, logname );
	    printMessage( msg, MESS_ERROR );
	    return( E_LOG );
	}

	
	/*
	 * Get the media type of the volume.
	 */

	if ( streq( globals.gVolumeType, "" ) || promptType )
	{
	    promptType = TRUE;
	    CHECK( getType() );
	}
	(void) strcpy( volume.adv_vol_info.mfs.fsv_type, globals.gVolumeType );
	

	/*
	 * Verify type/format of the volume.
	 */

	if ( streq( globals.gMfsFormat, MFS_ISO9660 ) ||
		streq( globals.gMfsFormat, MFS_ISO9660_JB ) )
	{
	    (void) strcpy( globals.gVolumeFormat, AD_ISO9660 );
	    (void) strcpy( volume.adv_vol_info.mfs.fsv_format, AD_ISO9660 );
	}
	else if ( streq( globals.gMfsFormat, MFS_UDF_JB ) )
	{
	    (void) strcpy( globals.gVolumeFormat, AD_UDF );
	    (void) strcpy( volume.adv_vol_info.mfs.fsv_format, AD_UDF );
	}
	else if ( streq( globals.gMfsFormat, MFS_UFS ) )
	{
	    (void) strcpy( globals.gVolumeFormat, AD_UFS );
	    (void) strcpy( volume.adv_vol_info.mfs.fsv_format, AD_UFS );
	}
	else
	{
	    formatMessage( E_VOL_FORMAT, globals.gMfsFormat );
	    printMessage( msg, MESS_ERROR );
	    return( E_VOL_FORMAT );
	}

	volume.adv_medium_type = AD_MED_MFS;
	if ( ! ad_volume_verify( &volume ) )
	{
	    formatMessage( E_VOL_VERIFY, globals.gVolumename );
	    msg_append( msg, ad_msg );
	    printMessage( msg, MESS_ERROR );
	    return( E_VOL_VERIFY );
	}

		
	/*
	 *  Try to find a volume with the same volumename, type, and
	 *  creation date in the archive.
	 */

	status = ad_volume_get( AD_VG_CRC, &volume, globals.gDbproc,
		AD_MED_MFS, globals.gArchive, globals.gVolumename,
		globals.gVolumeType, globals.gCreateDate );
	CHECK_DB( dbcancel( globals.gDbproc ) );
	globals.gCrc = volume.adv_crc;

	
	if ( status != AD_NO_VOLUME )
	{
	    /*
	     * Volume is already in the database check to see if
	     * the user wishes to re-ingest the volume.
	     */

	    CHECK_AD( status );

	    (void) strcpy( globals.gVolumename, volume.adv_volume_name );

	    formatMessage( S_RE_INGEST, volume.adv_volume_name );
	    printMessage( msg, MESS_INFO );

	    if ( globals.gAllowReingest || 
		    ( ! globals.gBatch && promptUser( REINGEST ) ) )
	    {
		globals.gReingest = TRUE;
	    }
	    else
	    {
		formatMessage( S_NOT_REINGEST, volume.adv_volume_name );
		printMessage( msg, MESS_INFO );
		continue;
	    }
	}
	
 
	/*
	 *  Process the volume.
	 */

	CHECK( ingestVolume() );

	if ( ! globals.gBatch &&
		( globals.gPathList == NULL && ! promptUser( NEW_VOLUME ) ) )
	{
	    done = TRUE;
	}
    }
    while ( !done );
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ingestDirectory
*
*   Purpose:
*	Ingests a directory into the archive database, recursivly calling
*	itself to ingest sub-direcotries.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_PATH_LEN	: Path is longer than the alocated length.
*	int	E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

static int	ingestDirectory
(
    char	*path			/* (in)  Directory to ingest.	*/
)
{
    DIR			*currentDir;	/* Current directory.		*/
    char		currentFile[PATH_MAX];	/* Current file path.	*/
    struct dirent	*dirent;	/* Current directory entry.	*/
    struct stat		statBuf;


    currentDir = opendir( path );
    if ( currentDir == NULL )
    {
	formatMessage( MSG_ERRNO, path );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }


    /*
     *  Process every file in the directory.
     */

    while ( ( dirent = readdir( currentDir ) ) != NULL )
    {
	if ( streq( dirent->d_name, "." ) || streq( dirent->d_name, ".." ) )
	{
	    continue;
	}

	(void) sprintf( currentFile, "%s/%s", path, dirent->d_name );


	CHECK_SYSTEM( stat( currentFile, &statBuf ), currentFile );

	if ( S_ISDIR( statBuf.st_mode ) )
	{
	    CHECK( ingestDirectory( currentFile ) );
	}
	else
	{
	    CHECK( ingestFile( currentFile, &statBuf ) );
	}
    }

    (void) closedir( currentDir );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	static int	ingestVolume()
*
*   Purpose:
*	Ingest the given volume.
*
*   Parameters:
*	None.
*
*   Values Returned:
*	int	S_SUCCESS	: All went well.
*	int	E_AD		: Error in the ad library.
*	int	E_DB_LIB	: Database error.
*	int	E_PATH_LEN	: Path is longer than the alocated length.
*	int	E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

static int	ingestVolume()
{
    char	crcSrchStr[11];	/* CRC search string.			*/
    char	*dbName;	/* Name of the mfs database.		*/
    char	dbStatus;	/* Status from db routine.		*/
    char	*fileTable;	/* Tbl name for the mfs files.		*/
    int		i;
    int		numFiles;	/* Number of files to delete.		*/
    char	*volTable;	/* Tbl name for the mfs volumes.	*/
    AD_VOLUME	volume;		/* The volume to be ingested.		*/

    
    globals.gNumFiles = 0;
    globals.gNumKbytes = 0;


    /*
     *  if the volume is being re-ingested, remove all files that used to
     *  be on the volume.
     */
    
    if ( globals.gReingest )
    {
	/*
	 *  Delete any existing files from the archive.
	 */

	CHECK_AD( ad_dbinfo_get( AD_MED_MFS, NULL, &dbName, &fileTable,
		&volTable ) );

	/*
	 * First find out how much we need to remove.
	 */

	CHECK_DB( dbfcmd( globals.gDbproc,
		"select count(*) \n"
		"from %s..%s files \n"
		"where files.crc = %s \n",
		dbName, fileTable, crc322str( globals.gCrc, crcSrchStr ) ) );
	
	CHECK_DB( dbsqlexec( globals.gDbproc ) );
	CHECK_DB( dbresults( globals.gDbproc ) );
	CHECK_DB( dbnextrow( globals.gDbproc ) );

	
	
	dbStatus = db_intdata( globals.gDbproc, 1, &numFiles );
	if ( dbStatus == DB_NULL_VALUE )
	{
	    numFiles = 0;
	}
	else
	{
	    CHECK_DB_LIB( dbStatus );
	}


	/*
	 * Now delete the files, 500 at a time.
	 */
 
	for( i = 0; i < numFiles; i+= 500 )
	{
	    CHECK_DB_LIB( db_simple_query( globals.gDbproc,
		    "set ROWCOUNT 500 \n" ) );

	    CHECK_DB_LIB( db_simple_query( globals.gDbproc,
		    "begin transaction" ) );
	    CHECK_DB_LIB( db_simple_query( globals.gDbproc,
		    "delete \n"
		    "    %s..%s \n"
		    "from \n"
		    "    %s..%s vol, \n"
		    "    %s..%s file \n"
		    "where \n"
		    "    vol.crc = %s \n"
		    "    and file.crc = %s \n",
		    dbName, fileTable, dbName, volTable, 
		    dbName, fileTable, crc322str( globals.gCrc, crcSrchStr ),
			    crcSrchStr ) );
	    while ( ( dbStatus = dbresults( globals.gDbproc ) ) == SUCCEED )
	    {
	    }
	    CHECK_DB( dbStatus );
	    CHECK_DB_LIB( db_simple_query( globals.gDbproc,
		    "commit transaction" ) );
	}
    }


    /*
     *  Write a new volume entry to the vds table.
     */

    CHECK_AD( ad_volume_init( &volume, AD_MED_MFS ) );
    (void) strcpy( volume.adv_volume_name, globals.gVolumename );
    volume.adv_vol_info.mfs.fsv_creation_date = globals.gCreateDate;
    (void) strcpy( volume.adv_archive, globals.gArchive );
    volume.adv_vol_info.mfs.fsv_ingest_date = time( NULL );
    (void) strcpy( volume.adv_vol_info.mfs.fsv_type, globals.gVolumeType );
    (void) strcpy( volume.adv_vol_info.mfs.fsv_format, globals.gVolumeFormat );

    
    formatMessage( S_VOLUME_PUT, volume.adv_volume_name );
    printMessage( msg, MESS_INFO );

    CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );
    globals.gCrc = volume.adv_crc;

    CHECK( ingestDirectory( globals.gMountPoint ) );


    /*
     *  If end of mfsirectory was reached, add the volume to the volume table.
     */

    volume.adv_vol_info.mfs.fsv_nfiles = globals.gNumFiles;
    volume.adv_vol_info.mfs.fsv_nkbytes = globals.gNumKbytes;

    formatMessage( S_VOLUME_UPDATE, volume.adv_volume_name,
	    globals.gNumFiles, globals.gNumKbytes );
    printMessage( msg, MESS_INFO );

    CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ingestFile
*
*   Purpose:
*	Ingest a file into the database.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AD		: Error in the ad library.
*	int	E_PATH_LEN	: Path length too long.
*
************************************************************************
-*/

static int	ingestFile
(
    char	*path,		/* (in)  The path of the file to ingest.*/
    struct stat	*statBuf	/* (in)  The stat buf previously done.	*/
)
{
    AD_FILE	adFile;		/* The file to insert.			*/
    char	compression;	/* Type of compression on file.		*/
    int		fileDesc;	/* File descriptor of the file. 	*/
    char	fileId[PATH_MAX];	/* File ID.				*/
    int		status;		/* Status variable.			*/


    /* 
     *  Initialize file struct.
     */

    CHECK_AD( ad_file_init( &adFile, AD_MED_MFS ) );
    adFile.adf_status = AD_FILE_CURRENT;


    /*
     *  Check if filename and file ID will fit. 
     */

    if ( (int) ( strlen( path ) - strlen( globals.gMountPoint ) ) - 1 > 
	    AD_PATH_LEN )
    {
	formatMessage( E_PATH_LEN, AD_PATH_LEN, path );
	printMessage( msg, MESS_ERROR );
	return( E_PATH_LEN );
    }

    CHECK_AD( ad_file_info( strtail( path ), fileId, &compression, NULL ) );
    if ( strlen( fileId ) > AD_FILE_ID_LEN )
    {
	formatMessage( E_FILE_ID_LEN, AD_FILE_ID_LEN, fileId );
	printMessage( msg, MESS_ERROR );
	(void) strncpy( adFile.adf_file_id, fileId, AD_FILE_ID_LEN );
	adFile.adf_status = AD_FILE_ERROR;
    }
    else
    {
	(void) strcpy( adFile.adf_file_id, fileId );
    }


    /*
     *  Fill in the rest of the file struct. 
     */
  
    adFile.adf_filesize = statBuf->st_size;
    adFile.adf_crc = globals.gCrc;
    (void) strcpy( adFile.adf_volume_name, globals.gVolumename );
    (void) strcpy( adFile.adf_location.mfs.fsl_filename, 
	    path + strlen( globals.gMountPoint ) + 1 );
    adFile.adf_compression = compression;

    switch( globals.gCase )
    {
	case AD_CASE_LOWER:
	    str2lower( adFile.adf_file_id, adFile.adf_file_id );
	    break;

	case AD_CASE_UPPER:
	    str2upper( adFile.adf_file_id, adFile.adf_file_id );

	default:
	    break;
    }

    
    /* 
     *  Check if file is compressed and is non-zero (because of
     *  possible copy errors in MFS production), and if so, get
     *  uncompressed size.
     */

    if ( ( adFile.adf_compression == AD_COMP_GZIP
	    || adFile.adf_compression == AD_COMP_UNIX ) 
	    && adFile.adf_filesize > 0 )
    {
	CHECK_SYSTEM( ( fileDesc = open( path, O_RDONLY ) ), "open" );

	if ( adFile.adf_compression == AD_COMP_GZIP )
	{
	    status = unpress_fsize( fileDesc, 
		    &adFile.adf_filesize_uncomp, PR_GZIP );
	}
	else if ( adFile.adf_compression == AD_COMP_UNIX )
	{
	    status = unpress_fsize( fileDesc, 
		    &adFile.adf_filesize_uncomp, PR_UNIX );
	}
	CHECK_SYSTEM( close( fileDesc ), "close" );

	if ( status != PR_SUCCESS )
	{
	    formatMessage( E_PRESS, path );
	    msg_append( msg, pr_msg );
	    msg_clear( pr_msg );
	    printMessage( msg, MESS_ERROR );
	    adFile.adf_filesize_uncomp = -1;
	    adFile.adf_status = AD_FILE_ERROR;
	}
    }


    /*
     *  Put file into archive tables. 
     */

    formatMessage( S_FILE_PUT, adFile.adf_location.mfs.fsl_filename, 
	    adFile.adf_filesize );
    printMessage( msg, MESS_INFO );

    CHECK_AD( ad_file_put( &adFile, globals.gDbproc ) );

    globals.gNumFiles ++;
    globals.gNumKbytes += ( adFile.adf_filesize + 1023 ) / 1024;

    return( S_SUCCESS );
}
