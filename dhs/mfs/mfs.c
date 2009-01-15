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
*   Module Name:	mfs/src/mfs.c
*
*   Purpose:
*	Checks the mount point of a mountable file system.
*
*   Routines:
*	int		mfs_check_mnt	: Checks a mount point.
*	static int	find_raw_device	: Finds type of raw device.
*	static time_t	parse_iso9660_date : Parses a raw ISO9660 date.
*	static int	read_iso9660	: Reads info from ISO9660 disk.
*	static int	read_udf	: Reads info from UDF disk.
*
*   Date		: March 9, 1990
*
*   SCCS data           : @(#)
*       Module Name     : mfs.c
*       Version Number  : 1.4
*       Release Number  : 1
*       Last Updated    : 05/04/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*     2000/03/06 SDJ	: Changed to "mfs" library from "dvd" library.
*			  Re-organized code to support both ISO9660 and
*			  UDF, in particular added find_raw_device(),
*			  read_iso9660(), and read_udf().
*     2000/04/07 SDJ	: Added MFS_ISO9660_JB.  CD's in the juke box
*			  need to be treated differently than those
*			  in a CD-ROM drive.
* 	2000/04/17 sjg	: Added reading of date to read_iso9660.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
 *  If you for whatever reason don't have <sys/fsctl.h>, the
 *  following define seemed to be all that's needed - but there is
 *  no guarantie that it will work - uncomment it at your own risk !
 *
 *  #define CDFS_VOL_ID 6
 */

#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"

#include "mfs.h"
#include "local_mfs.h"


/*
 * Location definitions.
 */

#define BUFFER_SIZE 	1024
#define BLOCK_SIZE 	2048
#define MAXPATHLEN	1024

/*
 * Local function prototypes.
 */

static	time_t	parse_iso9660_date( char * );

/*
 * XXX allan: 11/21/02: removed obsolete HPUX version and added check for 
 * HAVE_SYS_MNTTAB_H (solaris) and HAVE_MNTENT_H (linux).
 * (The solaris version is out of date, but should still work).
 */

#if defined(HAVE_SYS_MNTTAB_H)

/* Solaris/mnttab.h version */

#include <sys/mnttab.h>

static int	find_raw_device( struct mnttab *, char * );
static int	read_iso9660( char *, struct mnttab *, char *, time_t * );
static int	read_udf( char *, struct mnttab *, char *, time_t * );



/*+
************************************************************************
*
*   Function:	mfs_check_mnt (HAVE_SYS_MNTTAB_H version)
*
*   Purpose:
*	Checks that a specified path is on a mountable optical
*	medium, gets the actual mount point and the name of the mfs.
*	This function knows about the hrfs supplied by tracer, and the
*	system files tracer uses to track the file systems.
*
*	It is expected this 
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_NOT_MFS		: Mount point is not a
*					  recognized file system..
*	int	MFS_E_SYSTEM		: Error in a system call.
*	int	MFS_E_VSTAB		: Error in the vstab file format.
*
************************************************************************
-*/

int		mfs_check_mnt
(
    char	*mount_point,	/* (mod) The path to check.  On exit,	*/
				/*       the actual mount point of the 	*/
				/*       mfs.				*/
    char	*volume,	/* (out) The volume name of the mfs.	*/
    time_t	*create_date,	/* (out) Volume creation date.		*/
    char	*fs		/* (out) File system (format).		*/
)
{
    time_t	date_raw;	/* Date from raw disk.			*/
    struct mnttab mnttab;	/* Mount table information.		*/
    FILE	*mnttab_fp;	/* Mount table file pointer.		*/
    dev_t	path_dev;	/* The dev of the starting path.	*/
    struct stat	stat_buf;	/* Output from the stat function.	*/
    int		status;

    CHECK_SYSTEM( stat( mount_point, &stat_buf ), mount_point );
    path_dev = stat_buf.st_dev;

    
    /*
     *  Open the mnttab file.
     */

    mnttab_fp = fopen( MNTTAB, "r" );
    if ( mnttab_fp == NULL )
    {
	CHECK_SYSTEM( -1, MNTTAB );
    }


    while ( ( status = getmntent( mnttab_fp, &mnttab ) ) == 0 )
    {
	status = stat( mnttab.mnt_mountp, &stat_buf );
	if ( status != 0 )
	{
	    (void) fclose( mnttab_fp );
	    CHECK_SYSTEM( status, mnttab.mnt_mountp );
	}


	if ( stat_buf.st_dev == path_dev )
	{
	    /*
	     *  The required mount entry has been found.
	     */

	    break;
	}
    }
    (void) fclose( mnttab_fp );
    

    if ( status != 0 )
    {
	CHECK_SYSTEM( -1, MNTTAB );
    }
	

    /*
     *  Determine the volume.
     */

    if ( streq( mnttab.mnt_fstype, MFS_ISO9660 ) ||
	    streq( mnttab.mnt_fstype, MFS_ISO9660_JB ) )
    {
	CHECK( read_iso9660( mount_point, &mnttab,
		volume, &date_raw ) );
	(void) strcpy( fs, mnttab.mnt_fstype );
    }
    else if ( streq( mnttab.mnt_fstype, MFS_UDF_JB ) )
    {
	CHECK( read_udf( mount_point, &mnttab,
		volume, &date_raw ) );
	(void) strcpy( fs, MFS_UDF_JB );
    }
    else
    {
	(void) strcpy( fs, MFS_UNKNOWN );
	mfs_format_message( MFS_E_NOT_MFS, mnttab.mnt_fstype );
	return( MFS_E_NOT_MFS );
    }
    (void) strcpy( mount_point, mnttab.mnt_mountp );


    /*
     *  The creation date of the mount point is the disk creation
     *  date.  If the date is 0, then use the date from the raw disk.
     *  If both dates are 0, abort!
     */

    if ( stat_buf.st_ctime == 0 )
    {
	if ( date_raw == 0 )
	{
	    (void) fprintf( stderr, 
		    "mfs_check_mnt: %s: both stat_buf.st_ctime and date_raw are 0\n",
		    mount_point );
	    abort();
	}

	*create_date = date_raw;
    }
    else
    {
	*create_date = stat_buf.st_ctime;
    }

    return( MFS_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	find_raw_device
*
*   Purpose:
*	Finds the raw device in the vstab table.
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_SYSTEM		: Error in a system call.
*	int	MFS_E_VSTAB		: Error in the vstab file format.
*
*
************************************************************************
-*/
int	find_raw_device
(
    struct mnttab
    		*mnttab,	/* (in)  Mount table information.	*/
    char	*raw_device	/* (out) raw device.			*/
)
{
    char	buffer[1024];
    int		num_tokens;	/* Number of tokens in vstab lines.	*/
    char	*tokens[3];	/* tokens from vstab lines.		*/
    FILE	*vstab_fp;	/* file pointer to vstab file.		*/

    raw_device[0]='\0';


    /*
     * Find the volume in the vstab table.
     */

    if ( streq( mnttab->mnt_fstype, MFS_ISO9660_JB ) ||
	    streq( mnttab->mnt_fstype, MFS_UDF_JB ) )
    {
	vstab_fp = fopen ( VSTAB, "r" );
	if ( vstab_fp == NULL )
	{
	    CHECK_SYSTEM( -1, VSTAB );
	}
	

	while ( fgets( buffer, 1023, vstab_fp ) != NULL )
	{
	    if ( strlen( buffer ) == 0 )
	    {
		continue;
	    }


	    /*
	     * Tokenize the information from the vstab table.
	     */
	
	    strtokens( buffer, 3, tokens, &num_tokens );
	    
	    if ( streq( tokens[0], mnttab->mnt_special ) )
	    {
		/*
		 *  this is the device sought.
		 */
		break;
	    }
	}
	
	
	if ( num_tokens != 2 )
	{
	    mfs_format_message( MFS_E_VSTAB, mnttab->mnt_special );
	    return( MFS_E_VSTAB );
	}
	(void)sprintf( raw_device, "%s", tokens[1] );
    }
    else if ( streq( mnttab->mnt_fstype, MFS_ISO9660 ) )
    {
        (void)sprintf( raw_device, "%s", mnttab->mnt_special );
    }

    return( MFS_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	read_iso9660  (HAVE_SYS_MNTTAB_H version )
*
*   Purpose:
*       To read the volume name and date from an optical media written
*	with the ISO9660 file system format.
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_SYSTEM		: Error in a system call.
*
************************************************************************
-*/

int	read_iso9660
(
    char	*mount_point,	/* (in)  Raw device to read from.	*/
    struct mnttab
    		*mnttab,	/* (in)  Mount table information.	*/
    char        *volume,	/* (out) The volume name of the media. */
    time_t	*date		/* (out) Date of the media.		*/
)
{
    char	buffer[BLOCK_SIZE]; /* Buffer read from the mfs.	*/
    char	date_str[15];	/* Date string.				*/
    int		fd;		/* Generic file descriptor.		*/
    char	raw_device[MAXPATHLEN]; /* The raw device name.		*/

    
    CHECK( find_raw_device( mnttab, raw_device ) );
    

    /*
     *  Open the raw device and extract the name and possibly the date.
     *  Seeks and reads must be in 2048 blocks.  The name and date are
     *  in the 17th block so seek 16 * 2048 (=32768) and read 2048.
     */

    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 16 * BLOCK_SIZE, SEEK_SET ) != 16 * BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buffer, BLOCK_SIZE ) != BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );


    /*
     *  The name is at offset 40 of the buffer and the date at offset 813.
     */

    (void) sscanf( buffer + 40, "%32s", volume );
    (void) sscanf( buffer + 813, "%14s", date_str );
    *date = parse_iso9660_date( date_str );

    return( MFS_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	read_udf  (HAVE_SYS_MNTTAB_H version )
*
*   Purpose:
*	To read the volume from an optical media written with the
*	UDF file system format.
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_SYSTEM		: Error in a system call.
*
************************************************************************
-*/

int	read_udf
(
    char	*mount_point,	/* (in)  Raw device to read from.	*/
    struct mnttab
    		*mnttab,	/* (in)  Mount table information.	*/
    char        *volume,	/* (out) The name of the disk.		*/
    time_t	*date		/* (out) The date of the disk.		*/
)
{
    char	buffer[BLOCK_SIZE];	/* Buffer read from the mfs.	*/
    int		fd;		/* Generic file descriptor.		*/
    char	raw_device[MAXPATHLEN];
    				/* The raw device name.			*/
    
    CHECK( find_raw_device( mnttab, raw_device ) );
    

    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 20 * BLOCK_SIZE, SEEK_SET ) != 20 * BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buffer, BLOCK_SIZE ) != BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );


    /*
     *  The name is at offset 117 of the buffer and the date at offset ???.
     *  The date extraction is not yet implemented.
     */

    (void) sscanf( buffer + 117, "%32s", volume );
    *date = 0;
    
    return( MFS_SUCCESS );
}	
#elif defined(HAVE_MNTENT_H)


/* This is the Linux/mntent.h version */
#include <mntent.h>

static int	find_raw_device( struct mntent *, char * );
static int	read_iso9660( char *, struct mntent *, char *, time_t * );
static int	read_udf( char *, struct mntent *, char *, time_t * );



/*+
************************************************************************
*
*   Function:	mfs_check_mnt (HAVE_MNTENT_H version)
*
*   Purpose:
*	Checks that a specified path is on a mountable optical
*	medium, gets the actual mount point and the name of the mfs.
*	This function knows about the hrfs supplied by tracer, and the
*	system files tracer uses to track the file systems.
*
*	It is expected this 
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_NOT_MFS		: Mount point is not a
*					  recognized file system..
*	int	MFS_E_SYSTEM		: Error in a system call.
*	int	MFS_E_VSTAB		: Error in the vstab file format.
*
************************************************************************
-*/

int		mfs_check_mnt
(
    char	*mount_point,	/* (mod) The path to check.  On exit,	*/
				/*       the actual mount point of the 	*/
				/*       mfs.				*/
    char	*volume,	/* (out) The volume name of the mfs.	*/
    time_t	*create_date,	/* (out) Volume creation date.		*/
    char	*fs		/* (out) File system (format).		*/
)
{
    time_t	date_raw;	/* Date from raw disk.			*/
    struct mntent* mntent;	/* Mount table information.		*/
    FILE	*mntent_fp;	/* Mount table file pointer.		*/
    dev_t	path_dev;	/* The dev of the starting path.	*/
    struct stat	stat_buf;	/* Output from the stat function.	*/
    int		status;

    CHECK_SYSTEM( stat( mount_point, &stat_buf ), mount_point );
    path_dev = stat_buf.st_dev;

    
    /*
     *  Open the mntent file.
     */

    mntent_fp = setmntent( MNTTAB, "r" );
    if ( mntent_fp == NULL )
    {
	CHECK_SYSTEM( -1, MNTTAB );
    }


    while ( ( mntent = getmntent( mntent_fp ) ) != NULL )
    {
	status = stat( mntent->mnt_dir, &stat_buf );
	if ( status != 0 )
	{
	    (void) fclose( mntent_fp );
	    CHECK_SYSTEM( status, mntent->mnt_dir );
	}


	if ( stat_buf.st_dev == path_dev )
	{
	    /*
	     *  The required mount entry has been found.
	     */

	    break;
	}
    }
    (void) fclose( mntent_fp );
    

    if ( status != 0 )
    {
	CHECK_SYSTEM( -1, MNTTAB );
    }
	

    /*
     *  Determine the volume.
     */

    if ( streq( mntent->mnt_type, MFS_ISO9660 ) ||
	    streq( mntent->mnt_type, MFS_ISO9660_JB ) )
    {
	CHECK( read_iso9660( mount_point, mntent,
		volume, &date_raw ) );
	(void) strcpy( fs, mntent->mnt_type );
    }
    else if ( streq( mntent->mnt_type, MFS_UDF_JB ) )
    {
	CHECK( read_udf( mount_point, mntent,
		volume, &date_raw ) );
	(void) strcpy( fs, MFS_UDF_JB );
    }
    else
    {
	(void) strcpy( fs, MFS_UNKNOWN );
	mfs_format_message( MFS_E_NOT_MFS, mntent->mnt_type );
	return( MFS_E_NOT_MFS );
    }
    (void) strcpy( mount_point, mntent->mnt_dir );


    /*
     *  The creation date of the mount point is the disk creation
     *  date.  If the date is 0, then use the date from the raw disk.
     *  If both dates are 0, abort!
     */

    if ( stat_buf.st_ctime == 0 )
    {
	if ( date_raw == 0 )
	{
	    (void) fprintf( stderr, 
		    "mfs_check_mnt: %s: both stat_buf.st_ctime and date_raw are 0\n",
		    mount_point );
	    abort();
	}

	*create_date = date_raw;
    }
    else
    {
	*create_date = stat_buf.st_ctime;
    }

    return( MFS_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	find_raw_device
*
*   Purpose:
*	Finds the raw device in the vstab table.
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_SYSTEM		: Error in a system call.
*	int	MFS_E_VSTAB		: Error in the vstab file format.
*
*
************************************************************************
-*/
int	find_raw_device
(
    struct mntent
    		*mntent,	/* (in)  Mount table information.	*/
    char	*raw_device	/* (out) raw device.			*/
)
{
    char	buffer[1024];
    int		num_tokens;	/* Number of tokens in vstab lines.	*/
    char	*tokens[3];	/* tokens from vstab lines.		*/
    FILE	*vstab_fp;	/* file pointer to vstab file.		*/

    raw_device[0]='\0';


    /*
     * Find the volume in the vstab table.
     */

    if ( streq( mntent->mnt_type, MFS_ISO9660_JB ) ||
	    streq( mntent->mnt_type, MFS_UDF_JB ) )
    {
	vstab_fp = fopen ( VSTAB, "r" );
	if ( vstab_fp == NULL )
	{
	    CHECK_SYSTEM( -1, VSTAB );
	}
	

	while ( fgets( buffer, 1023, vstab_fp ) != NULL )
	{
	    if ( strlen( buffer ) == 0 )
	    {
		continue;
	    }


	    /*
	     * Tokenize the information from the vstab table.
	     */
	
	    strtokens( buffer, 3, tokens, &num_tokens );
	    
	    if ( streq( tokens[0], mntent->mnt_fsname ) )
	    {
		/*
		 *  this is the device sought.
		 */
		break;
	    }
	}
	
	
	if ( num_tokens != 2 )
	{
	    mfs_format_message( MFS_E_VSTAB, mntent->mnt_fsname );
	    return( MFS_E_VSTAB );
	}
	(void)sprintf( raw_device, "%s", tokens[1] );
    }
    else if ( streq( mntent->mnt_type, MFS_ISO9660 ) )
    {
        (void)sprintf( raw_device, "%s", mntent->mnt_fsname );
    }

    return( MFS_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	read_iso9660  (HAVE_MNTENT_H version )
*
*   Purpose:
*       To read the volume name and date from an optical media written
*	with the ISO9660 file system format.
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_SYSTEM		: Error in a system call.
*
************************************************************************
-*/

int	read_iso9660
(
    char	*mount_point,	/* (in)  Raw device to read from.	*/
    struct mntent
    		*mntent,	/* (in)  Mount table information.	*/
    char        *volume,	/* (out) The volume name of the media. */
    time_t	*date		/* (out) Date of the media.		*/
)
{
    char	buffer[BLOCK_SIZE]; /* Buffer read from the mfs.	*/
    char	date_str[15];	/* Date string.				*/
    int		fd;		/* Generic file descriptor.		*/
    char	raw_device[MAXPATHLEN]; /* The raw device name.		*/

    
    CHECK( find_raw_device( mntent, raw_device ) );
    

    /*
     *  Open the raw device and extract the name and possibly the date.
     *  Seeks and reads must be in 2048 blocks.  The name and date are
     *  in the 17th block so seek 16 * 2048 (=32768) and read 2048.
     */

    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 16 * BLOCK_SIZE, SEEK_SET ) != 16 * BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buffer, BLOCK_SIZE ) != BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );


    /*
     *  The name is at offset 40 of the buffer and the date at offset 813.
     */

    (void) sscanf( buffer + 40, "%32s", volume );
    (void) sscanf( buffer + 813, "%14s", date_str );
    *date = parse_iso9660_date( date_str );

    return( MFS_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	read_udf  (HAVE_MNTENT_H version )
*
*   Purpose:
*	To read the volume from an optical media written with the
*	UDF file system format.
*
*   Values Returned:
*	int	MFS_SUCCESS		: Normal completion.
*	int	MFS_E_SYSTEM		: Error in a system call.
*
************************************************************************
-*/

int	read_udf
(
    char	*mount_point,	/* (in)  Raw device to read from.	*/
    struct mntent
    		*mntent,	/* (in)  Mount table information.	*/
    char        *volume,	/* (out) The name of the disk.		*/
    time_t	*date		/* (out) The date of the disk.		*/
)
{
    char	buffer[BLOCK_SIZE];	/* Buffer read from the mfs.	*/
    int		fd;		/* Generic file descriptor.		*/
    char	raw_device[MAXPATHLEN];
    				/* The raw device name.			*/
    
    CHECK( find_raw_device( mntent, raw_device ) );
    

    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 20 * BLOCK_SIZE, SEEK_SET ) != 20 * BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buffer, BLOCK_SIZE ) != BLOCK_SIZE )
    {
	CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );


    /*
     *  The name is at offset 117 of the buffer and the date at offset ???.
     *  The date extraction is not yet implemented.
     */

    (void) sscanf( buffer + 117, "%32s", volume );
    *date = 0;
    
    return( MFS_SUCCESS );
}	
#endif /* HAVE_SYS_MNTENT_H */


/*+
************************************************************************
*
*   Function:	parse_iso9660_date
*
*   Purpose:
*	Converts a ISO 9660 date into a time number.
*
*   Values Returned:
*	time_t	time		: Time in Unix time.
*
************************************************************************
-*/
static time_t	parse_iso9660_date
(
    char	*date	/* (in)	ISO 9660 date from raw disks		*/
)
{
    time_t	time;
    struct tm	tm;

    gen_zero( &tm, sizeof( tm ) );
    (void) sscanf( date, "%4d%2d%2d%2d%2d%2d%2d", 
	    &(tm.tm_year),
	    &(tm.tm_mon),
	    &(tm.tm_mday), 
	    &(tm.tm_hour), 
	    &(tm.tm_min), 
	    &(tm.tm_sec) );

    if ( tm.tm_year > 1900 )
    {
	tm.tm_year -= 1900;
    }

    tm.tm_mon--;	/* Jan == 0 */
    tm.tm_isdst = 1;

    time = mktime( &tm );


    /*
     *  Subtract 8 hours for local time.
     */

    time -= 28800;

    return( time );
}
