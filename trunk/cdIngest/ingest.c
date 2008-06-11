/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
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
*   Module Name:	cdIngest/src/ingest.c
*
*   Purpose:
*	Contains the routines to ingest a cd-rom.
*
*   Routines:
*	int		ingest		: The ingest procedure.
*	static int	ingestDisk	: Ingests a disk.
*	static int	ingestFiles	: Ingests files on a disk.
*	static void	onControlC	: Handle a control-c exception.
*
*   Date		: Dec 09, 1996
*
*   Field SCCS data	: @(#)
*	Module Name	: ingest.c
*	Version Number	: 1.10
*	Release Number	: 1
*	Last Updated	: 05/19/99
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/19 JSD  :	Removed renaming of disks.
*	97/04/04 SEC  :	Mod'ed for change to ad lib.
*	97/06/20 SEC  :	Mod'ed for change to ad lib: added back in
*			diskname_orig and fname becomes file_id.
*	97/07/02 SEC  :	Mod'ed ingestFile() to get uncompressed filesize.
*	97/11/04 SJG  :	Mod'ed ingestFile() to use file_id from ad_file_info.
*	98/06/23 sjg  : Added use of gUpper test for file ids.
*	99/05/19 sjg	: Changed gUpper to tri-state AD_CASE value.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "press.h"
#include "db.h"
#include "cd.h"
#include "ad.h"

#include "globals.h"

static int	ingestDirectory( char * );
static int	ingestDisk( void );
static int	ingestFile( char *, struct stat * );
static void	onControlC();

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
*	int	CI_SUCCESS	: All went well.
*	int	CI_QUIT		: Quit at users request.
*	int	CI_E_AD		: Error in the ad library.
*	int	CI_E_DB_LIB	: Error in the cadc db library.
*	int	CI_E_LOG	: Log file error.
*	int	CI_E_PATH_LEN	: Path is longer than the alocated length.
*	int	CI_E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

int	ingest()
{
    int		currentPath;
    boolean	done;
    char	logname[64];
    int		status;
    AD_VOLUME	volume;


    currentPath = 0;
    done = FALSE;
    do
    {

	/* 
	 *  Get a suitable disk.
	 */

	if ( globals.gPathList == NULL )
	{
	    CI_CHECK( diskLoad() );
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
	    
	    if ( cd_check_mnt( globals.gMountPoint, globals.gDiskname,
		    &globals.gCreateDate ) != CD_SUCCESS )
	    {
		printMessage( cd_msg, MESS_ERROR, CR );
		continue;
	    }

	}


	/*
	 *  Try to find a disk with the same diskname and
	 *  creation date in the archive.
	 */

	status = ad_volume_get( AD_VG_NAME, &volume, globals.gDbproc,
		AD_MED_CD, globals.gDiskname, globals.gCreateDate );
	CI_CHECK_DB( dbcancel( globals.gDbproc ) );

	if ( status == AD_NO_VOLUME )
	{
	    /*
	     *  Figure out the default name for the disk.
	     */

	    if ( strlen( globals.gDiskname ) == 0 )
	    {
		(void) strcpy( globals.gDiskname, 
			strtail( globals.gMountPoint ) );
	    }

	    globals.gReingest = FALSE;
	}
	else
	{
	    CI_CHECK_AD( status );

	    (void) strcpy( globals.gDiskname, volume.adv_volume_name );

	    formatMessage( CI_RE_INGEST, volume.adv_volume_name );
	    printMessage( msg, MESS_INFO, CR );

	    if ( globals.gAllowReingest || 
		    ( ! globals.gBatch && promptUser( REINGEST ) ) )
	    {
		globals.gReingest = TRUE;
	    }
	    else
	    {
		formatMessage( CI_NOT_REINGEST, volume.adv_volume_name );
		printMessage( msg, MESS_INFO, CR );

		continue;
	    }
	}


	/*
	 *  Look for the disk in the archive database by name.
	 */

	status = ad_volume_get( AD_VG_NAME, &volume, 
		globals.gDbproc, AD_MED_CD, globals.gDiskname );
	CI_CHECK_DB( dbcancel( globals.gDbproc ) );

	if ( status == AD_NO_VOLUME || 
		volume.adv_vol_info.cd.cdv_creation_date == 0 ||
		( streq( globals.gDiskname,
		    volume.adv_volume_name  ) &&
		    globals.gCreateDate == 
		    volume.adv_vol_info.cd.cdv_creation_date ) )
	{
	    /*
	     *  This is either a volume name that does not exist in 
	     *  the archive, or it is already asigned to this disk.
	     */

	    formatMessage( CI_DISK_NEWNAME, globals.gDiskname );
	    printMessage( msg, MESS_PROMPT, CR );

	    if ( status != AD_NO_VOLUME )
	    {
		(void) strcpy( globals.gArchive, volume.adv_archive );
	    }

	}
	else
	{
	    /*
	     *  The volume name is assigned to another disk and cannot
	     *  be used for this disk.
	     */

	    formatMessage( CI_DISK_NAMEUSED, globals.gDiskname );
	    printMessage( msg, MESS_ERROR, CR );

	}


	(void) sprintf( logname, LOGNAME_FORMAT, globals.gDiskname );
	if ( ( globals.gLogFp = fopen( logname, "w" ) ) == NULL )
	{
	    formatMessage( MSG_ERRNO, logname );
	    printMessage( msg, MESS_ERROR, CR );
	    return( CI_E_LOG );
	}


	/*
	 *  Process the disk.
	 */

	CI_CHECK( ingestDisk() );


	/*
	 *  Close the log file.
	 */

	(void) fclose( globals.gLogFp );
	globals.gLogFp = NULL;


	if ( ! globals.gBatch )
	{
	    if ( !promptUser( NEW_DISK ) )
	    {
		done = TRUE;
	    }
	}
    }
    while ( !done );
    
    return( CI_SUCCESS );
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
*	int	CI_SUCCESS	: Normal completion.
*	int	CI_E_PATH_LEN	: Path is longer than the alocated length.
*	int	CI_E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

static int	ingestDirectory
(
    char	*path		/* (in)  The directory to ingest.	*/
)
{
    DIR		*currentDir;	/* The current directory.		*/
    char	currentFile[PATH_MAX];	
				/* The current file path.		*/
    struct dirent	
		*dirent;	/* The current directory entry.		*/
    struct stat	statBuf;


    currentDir = opendir( path );

    if ( currentDir == NULL )
    {
	CI_CHECK_SYSTEM( -1, path );
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


	CI_CHECK_SYSTEM( stat( currentFile, &statBuf ), currentFile );

	if ( S_ISDIR( statBuf.st_mode ) )
	{
	    CI_CHECK( ingestDirectory( currentFile ) );
	}
	else
	{
	    CI_CHECK( ingestFile( currentFile, &statBuf ) );
	}
    }

    (void) closedir( currentDir );

    return( CI_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	static int	ingestDisk()
*
*   Purpose:
*	Ingest the given disk.
*
*   Parameters:
*	None.
*
*   Values Returned:
*	int	CI_SUCCESS	: All went well.
*	int	CI_E_AD		: Error in the ad library.
*	int	CI_E_DB_LIB	: Database error.
*	int	CI_E_PATH_LEN	: Path is longer than the alocated length.
*	int	CI_E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

static int	ingestDisk()
{
    char	*dbname;
    char	*fileTable;
    char	*volTable;
    AD_VOLUME	volume;

    
    (void) signal( SIGINT, onControlC );


    globals.gNumFiles = 0;
    globals.gNumKbytes = 0;


    /*
     *  if the disk is being re-ingested, remove all files that used to
     *  be on the disk.
     */
    
    if ( globals.gReingest )
    {
	/*
	 *  Delete any existing files from the archive.
	 */

	CI_CHECK_AD( ad_dbinfo_get( AD_MED_CD, NULL, &dbname, &fileTable,
		&volTable ) )

	CI_CHECK_DB_LIB( db_simple_query( globals.gDbproc,
		"delete \n"
		"    %s..%s \n"
		"from \n"
		"    %s..%s vol, \n"
		"    %s..%s file \n"
		"where \n"
		"    vol.diskname = '%s' \n"
		"    and file.diskname = '%s' \n",
		dbname, fileTable, dbname, volTable, 
		dbname, fileTable, globals.gDiskname, 
		globals.gDiskname ) );
    }


    /*
     *  Write a new volume entry to the cds table.
     */

    CI_CHECK_AD( ad_volume_init( &volume, AD_MED_CD ) );
    (void) strcpy( volume.adv_volume_name, globals.gDiskname );
    (void) strcpy( volume.adv_vol_info.cd.cdv_diskname_orig, 
	    globals.gDiskname );
    volume.adv_vol_info.cd.cdv_creation_date = globals.gCreateDate;
    (void) strcpy( volume.adv_archive, globals.gArchive );
    volume.adv_vol_info.cd.cdv_ingest_date = time( NULL );


    formatMessage( CI_DISK_PUT, volume.adv_volume_name );
    printMessage( msg, MESS_INFO, CR );

    CI_CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );


    CI_CHECK( ingestDirectory( globals.gMountPoint ) );


    /*
     *  If end of directory was reached, add the disk to the disk table.
     */

    volume.adv_vol_info.cd.cdv_nfiles = globals.gNumFiles;
    volume.adv_vol_info.cd.cdv_nkbytes = globals.gNumKbytes;

    formatMessage( CI_DISK_UPDATE, volume.adv_volume_name,
	    globals.gNumFiles, globals.gNumKbytes );
    printMessage( msg, MESS_INFO, CR );

    CI_CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );


    /*
     *  Restore default CONTROL-C handler.
     */

    (void) signal( SIGINT, SIG_DFL );

    return( CI_SUCCESS );
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
*	int	CI_SUCCESS	: Normal completion.
*	int	CI_E_AD		: Error in the ad library.
*	int	CI_E_PATH_LEN	: Path is longer than the alocated length.
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
    int		fileDesc;	/* File descriptor of the file. 	*/


    /* 
     *  Initialize file struct, check if filename will fit. 
     */

    CI_CHECK_AD( ad_file_init( &adFile, AD_MED_CD ) );

    if ( (int) ( strlen( path ) - strlen( globals.gMountPoint ) ) - 1 > 
	    AD_PATH_LEN )
    {
	formatMessage( CI_E_PATH_LEN, AD_PATH_LEN, path );
	printMessage( msg, MESS_ERROR, CR );
	return( CI_E_PATH_LEN );
    }


    /*
     *  Fill in file struct. 
     */
  
    adFile.adf_filesize = statBuf->st_size;
    (void) strcpy( adFile.adf_volume_name, globals.gDiskname );
    (void) strcpy( adFile.adf_location.cd.cdl_filename, 
	    path + strlen( globals.gMountPoint ) + 1 );
    CI_CHECK_AD( ad_file_info( strtail( path ), adFile.adf_file_id, 
	    &adFile.adf_compression, NULL ) );
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
     *  possible copy errors in cdrom production), and if so, get
     *  uncompressed size.
     */

    if ( adFile.adf_compression != AD_COMP_UNKNOWN && 
	    adFile.adf_filesize > 0 )
    {
	CI_CHECK_SYSTEM( ( fileDesc = open( path, O_RDONLY ) ), "open" );

	if ( adFile.adf_compression == AD_COMP_GZIP )
	{
		CI_CHECK_PRESS( unpress_fsize( fileDesc, 
			&adFile.adf_filesize_uncomp, PR_GZIP ) );
	}
	else if ( adFile.adf_compression == AD_COMP_UNIX )
	{
		CI_CHECK_PRESS( unpress_fsize( fileDesc, 
			&adFile.adf_filesize_uncomp, PR_UNIX ) );
	}

	CI_CHECK_SYSTEM( close( fileDesc ), "close" );
    }


    /*
     *  Put file into archive tables. 
     */

    formatMessage( CI_FILE_PUT, adFile.adf_location.cd.cdl_filename, 
	    adFile.adf_filesize );
    printMessage( msg, MESS_INFO, CR );

    CI_CHECK_AD( ad_file_put( &adFile, globals.gDbproc ) );

    globals.gNumFiles ++;
    globals.gNumKbytes += ( adFile.adf_filesize + 1023 ) / 1024;

    return( CI_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	static void	onControlC() 
*
*   Purpose:
*	Handle a control-c from the console.
*
*   Parameters:
*	int	sig	: (in)  The signal number.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

static void	onControlC( int sig )
{
    (void) signal( SIGINT, SIG_IGN );
    (void) signal( SIGINT, onControlC );
}
