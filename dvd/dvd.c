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
*   Module Name:	dvd/src/dvd.c
*
*   Purpose:
*	Checks the mount point and diskname of a dvd.
*
*   Routines:
*	int	dvd_check_mnt	: Checks a dvd mount point.
*
*   Date		: March 9, 1990
*
*   SCCS data           : @(#)
*       Module Name     : dvd.c
*       Version Number  : 1.1
*       Release Number  : 1
*       Last Updated    : 03/15/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef hpux
#include <mntent.h>
/*
 *  If you for whatever reason don't have <sys/fsctl.h>, the
 *  following define seemed to be all that's needed - but there is
 *  no guarantie that it will work - uncomment it at your own risk !
 *
 *  #define CDFS_VOL_ID 6
 */
#endif
#ifdef solaris
#include <sys/mnttab.h>
#endif

#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"


#include "dvd.h"
#include "local_dvd.h"


/*+
************************************************************************
*
*   Function:	dvd_check_mnt (solaris version)
*
*   Purpose:
*	Checks that a specified path is on a dvd, gets the actual mount
*	point and the name of the dvd.  This function knows about the
*	hrfs supplied by tracer, and the system files tracer uses to 
*	track the file systems.
*
*   Values Returned:
*	int	DVD_SUCCESS	: Normal completion.
*	int	DVD_E_NOT_DVDROM	: Mount point is not a dvdrom.
*	int	DVD_E_SYSTEM	: Error in a system call.
*	int	DVD_E_VSTAB	: Error in the vstab file format.
*
************************************************************************
-*/

#ifdef solaris
int		dvd_check_mnt
(
    char	*mount_point,	/* (mod) The path to check.  On exit,	*/
				/*       the actual mount point of the 	*/
				/*       dvd.				*/
    char	*diskname,	/* (out) The volume name of the dvd.	*/
    time_t	*create_date	/* (out) Volume creation date.		*/
)
{
    char	buffer[1024];
    char	buf[2048];	/* Buffer read from the dvd.		*/
    int		fd;		/* Generic file descriptor.		*/
    struct mnttab mnttab;	/* Mount table information.		*/
    FILE	*mnttab_fp;	/* Mount table file pointer.		*/
    int		num_tokens;	/* Number of tokens in vstab lines.	*/
    dev_t	path_dev;	/* The dev of the starting path.	*/
    char	*raw_device;	/* The raw device name.			*/
    struct stat	stat_buf;	/* Output from the stat function.	*/
    int		status;
    char	*tokens[3];	/* tokens from vstab lines.		*/
    FILE	*vstab_fp;	/* file pointer to vstab file.		*/


    DVD_CHECK_SYSTEM( stat( mount_point, &stat_buf ), mount_point );
    path_dev = stat_buf.st_dev;


    /*
     *  Open the mnttab file.
     */

    mnttab_fp = fopen( MNTTAB, "r" );
    if ( mnttab_fp == NULL )
    {
	DVD_CHECK_SYSTEM( -1, MNTTAB );
    }


    while ( ( status = getmntent( mnttab_fp, &mnttab ) ) == 0 )
    {
	status = stat( mnttab.mnt_mountp, &stat_buf );
	if ( status != 0 )
	{
	    (void) fclose( mnttab_fp );
	    DVD_CHECK_SYSTEM( status, mnttab.mnt_mountp );
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
	DVD_CHECK_SYSTEM( -1, MNTTAB );
    }
	

    /*
     *  Ensure the file system is a ???
     */
#ifdef OMIT
    if ( strne( mnttab.mnt_fstype, "???" ) )
    {
	dvd_format_message( DVD_E_NOT_DVDROM, mnttab.mnt_fstype );
	return( DVD_E_NOT_DVDROM );
    }
#endif
    (void) strcpy( mount_point, mnttab.mnt_mountp );


    /*
     *  The creation date of the mount point is the disk creation date.
     */

    *create_date = stat_buf.st_ctime;


    /*
     *  Get the raw device of the disk.
     */

#ifdef OMIT
    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	DVD_CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 16 * 2048, SEEK_SET ) != 16 * 2048 )
    {
	DVD_CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buf, 2048 ) != 2048 )
    {
	DVD_CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );

    (void) strcpy( diskname, buf+40 );
    strfit( diskname );
#endif    

    return( DVD_SUCCESS );
}
#endif /* solaris */

/*+
************************************************************************
*
*   Function:	dvd_check_mnt (hpux version)
*
*   Purpose:
*	Checks that a specified path is on a dvd, gets the actual mount
*	point and the name of the dvd.  This function knows about the
*	hrfs supplied by tracer, and the system files tracer uses to 
*	track the file systems.
*
*   Values Returned:
*	int	DVD_SUCCESS	: Normal completion.
*	int	DVD_E_NOT_DVDROM	: Mount point is not a dvdrom.
*	int	DVD_E_SYSTEM	: Error in a system call.
*	int	DVD_E_VSTAB	: Error in the vstab file format.
*
************************************************************************
-*/

#ifdef hpux
int		dvd_check_mnt
(
    char	*mount_point,	/* (mod) The path to check.  On exit,	*/
				/*       the actual mount point of the 	*/
				/*       dvd.				*/
    char	*diskname,	/* (out) The volume name of the dvd.	*/
    time_t	*create_date	/* (out) Volume creation date.		*/
)
{
#define BUFFER_SIZE 1024
    char	buffer[BUFFER_SIZE];
				/*    char	buf[VOL_ID_SIZ+1];*/
    char	buf[33];
				/* Buffer for dvd volume ID.		*/
    int		fd;		/* Generic file descriptor.		*/
    struct mntent *mnttab;	/* Mount table information.		*/
    FILE	*mnttab_fp;	/* Mount table file pointer.		*/
    dev_t	path_dev;	/* The dev of the starting path.	*/
    struct stat	stat_buf;	/* Output from the stat function.	*/
    int		status;


    DVD_CHECK_SYSTEM( stat( mount_point, &stat_buf ), mount_point );
    path_dev = stat_buf.st_dev;


    /*
     *  Open the mnttab file.
     */

    mnttab_fp = fopen( MNT_MNTTAB, "r" );
    if ( mnttab_fp == NULL )
    {
	DVD_CHECK_SYSTEM( -1, MNT_MNTTAB );
    }


    while ( ( mnttab = getmntent( mnttab_fp ) ) != NULL )
    {
	if ( streq( mnttab->mnt_type, MNTTYPE_IGNORE ) ) continue;

	status = stat( mnttab->mnt_dir, &stat_buf );
	if ( status != 0 )
	{
	    (void) fclose( mnttab_fp );
	    DVD_CHECK_SYSTEM( status, mnttab->mnt_dir );
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
	DVD_CHECK_SYSTEM( -1, MNT_MNTTAB );
    }
	

    /*
     *  Ensure the file system is a DVD file system
     */
#ifdef OMIT
    if ( strne( mnttab->mnt_type, ??? ) )
    {
	dvd_format_message( DVD_E_NOT_DVDROM, mnttab->mnt_type );
	return( DVD_E_NOT_DVDROM );
    }
#endif
    (void) strcpy( mount_point, mnttab->mnt_dir );


    /*
     *  The creation date of the mount point is the disk creation date.
     */

    *create_date = stat_buf.st_ctime;

    /*
     *  Get the volume name
     */
#ifdef OMIT
    (void) strncpy( buffer, mount_point, BUFFER_SIZE );
    (void) strcat( buffer, "/." );

    if ( ( fd = open( buffer, O_RDONLY ) ) == -1 )
    {
        DVD_CHECK_SYSTEM( fd, buffer );
    }

    if ( fsctl( fd, DVDFS_VOL_ID, &buf, (size_t) 33) == -1 )
    {
        DVD_CHECK_SYSTEM( -1, buffer );
    }

    (void) close( fd );
    

    (void) strcpy( diskname, buf );
    strfit( diskname );
#endif    

    return( DVD_SUCCESS );
}
#endif
