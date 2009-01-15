/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1995				(c) 1995.
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
*   Module Name:	cd/src/cd.c
*
*   Purpose:
*	Checks the mount point and diskname of a cd.
*
*   Routines:
*	int	cd_check_mnt	: Checks a cd mount point.
*
*   Date		: Apr 6, 1995
*
*   SCCS data		: @(#)
*	Module Name	: cd.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 04/30/98
*
*   Programmer		: Norm Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "cd.h"
#include "local_cd.h"

/*
 * XXX allan: 11/24/02: removed obsolete HPUX version and added check for 
 * HAVE_SYS_MNTTAB_H (solaris) and HAVE_MNTENT_H (linux).
 * (The solaris version is out of date, but should still work).
 */

#if defined(HAVE_SYS_MNTTAB_H)

/* Solaris/mnttab.h version */

#include <sys/mnttab.h>



/*+
************************************************************************
*
*   Function:	cd_check_mnt (solaris version)
*
*   Purpose:
*	Checks that a specified path is on a cd, gets the actual mount
*	point and the name of the cd.  This function knows about the
*	hrfs supplied by tracer, and the system files tracer uses to 
*	track the file systems.
*
*   Values Returned:
*	int	CD_SUCCESS	: Normal completion.
*	int	CD_E_NOT_CDROM	: Mount point is not a cdrom.
*	int	CD_E_SYSTEM	: Error in a system call.
*	int	CD_E_VSTAB	: Error in the vstab file format.
*
************************************************************************
-*/

int		cd_check_mnt
(
    char	*mount_point,	/* (mod) The path to check.  On exit,	*/
				/*       the actual mount point of the 	*/
				/*       cd.				*/
    char	*diskname,	/* (out) The volume name of the cd.	*/
    time_t	*create_date	/* (out) Volume creation date.		*/
)
{
    char	buffer[1024];
    char	buf[2048];	/* Buffer read from the cd.		*/
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


    CD_CHECK_SYSTEM( stat( mount_point, &stat_buf ), mount_point );
    path_dev = stat_buf.st_dev;


    /*
     *  Open the mnttab file.
     */

    mnttab_fp = fopen( MNTTAB, "r" );
    if ( mnttab_fp == NULL )
    {
	CD_CHECK_SYSTEM( -1, MNTTAB );
    }


    while ( ( status = getmntent( mnttab_fp, &mnttab ) ) == 0 )
    {
	status = stat( mnttab.mnt_mountp, &stat_buf );
	if ( status != 0 )
	{
	    (void) fclose( mnttab_fp );
	    CD_CHECK_SYSTEM( status, mnttab.mnt_mountp );
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
	CD_CHECK_SYSTEM( -1, MNTTAB );
    }
	

    /*
     *  Ensure the file system is a hsfs (high sierra file system) or an 
     *  hrfs (hyper rom file system).
     */

    if ( strne( mnttab.mnt_fstype, "hsfs" ) && 
	    strne( mnttab.mnt_fstype, "hrfs" ) )
    {
	cd_format_message( CD_E_NOT_CDROM, mnttab.mnt_fstype );
	return( CD_E_NOT_CDROM );
    }

    (void) strcpy( mount_point, mnttab.mnt_mountp );


    /*
     *  The creation date of the mount point is the disk creation date.
     */

    *create_date = stat_buf.st_ctime;


    /*
     *  Get the raw device of the disk.
     */

    if ( streq( mnttab.mnt_fstype, "hrfs" ) )
    {
	/*
	 *  If this a hrfs, look the volume set name in the vstab table.
	 */

	vstab_fp = fopen ( VSTAB, "r" );
	if ( vstab_fp == NULL )
	{
	    CD_CHECK_SYSTEM( -1, VSTAB );
	}


	while ( fgets( buffer, 1023, vstab_fp ) != NULL )
	{
	    if ( strlen( buffer ) == 0 )
	    {
		continue;
	    }


	    strtokens( buffer, 3, tokens, &num_tokens );

	    if ( streq( tokens[0], mnttab.mnt_special ) )
	    {
		/*
		 *  this is the device sought.
		 */

		 break;
	    }
	}


	if ( num_tokens != 2 )
	{
	    cd_format_message( CD_E_VSTAB, mnttab.mnt_special );
	    return( CD_E_VSTAB );
	}

	raw_device = tokens[1];
    }
    else
    {
	raw_device = mnttab.mnt_special;
    }

    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	CD_CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 16 * 2048, SEEK_SET ) != 16 * 2048 )
    {
	CD_CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buf, 2048 ) != 2048 )
    {
	CD_CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );
    

    (void) strcpy( diskname, buf+40 );
    strfit( diskname );

    return( CD_SUCCESS );
}



#elif defined(HAVE_MNTENT_H)

/* This is the Linux/mntent.h version */
#include <mntent.h>


int		cd_check_mnt
(
    char	*mount_point,	/* (mod) The path to check.  On exit,	*/
				/*       the actual mount point of the 	*/
				/*       cd.				*/
    char	*diskname,	/* (out) The volume name of the cd.	*/
    time_t	*create_date	/* (out) Volume creation date.		*/
)
{
    char	buffer[1024];
    char	buf[2048];	/* Buffer read from the cd.		*/
    int		fd;		/* Generic file descriptor.		*/
    struct mntent* mntent;	/* Mount table information.		*/
    FILE	*mntent_fp;	/* Mount table file pointer.		*/
    int		num_tokens;	/* Number of tokens in vstab lines.	*/
    dev_t	path_dev;	/* The dev of the starting path.	*/
    char	*raw_device;	/* The raw device name.			*/
    struct stat	stat_buf;	/* Output from the stat function.	*/
    int		status;
    char	*tokens[3];	/* tokens from vstab lines.		*/
    FILE	*vstab_fp;	/* file pointer to vstab file.		*/


    CD_CHECK_SYSTEM( stat( mount_point, &stat_buf ), mount_point );
    path_dev = stat_buf.st_dev;


    /*
     *  Open the mntent file.
     */

    mntent_fp = setmntent( MNTTAB, "r" );
    if ( mntent_fp == NULL )
    {
	CD_CHECK_SYSTEM( -1, MNTTAB );
    }


    while ( ( mntent = getmntent( mntent_fp ) ) != NULL )
    {
	status = stat( mntent->mnt_dir, &stat_buf );
	if ( status != 0 )
	{
	    (void) fclose( mntent_fp );
	    CD_CHECK_SYSTEM( status, mntent->mnt_dir );
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
	CD_CHECK_SYSTEM( -1, MNTTAB );
    }
	

    /*
     *  Ensure the file system is a hsfs (high sierra file system) or an 
     *  hrfs (hyper rom file system).
     */

    if ( strne( mntent->mnt_type, "hsfs" ) && 
	    strne( mntent->mnt_type, "hrfs" ) )
    {
	cd_format_message( CD_E_NOT_CDROM, mntent->mnt_type );
	return( CD_E_NOT_CDROM );
    }

    (void) strcpy( mount_point, mntent->mnt_dir );


    /*
     *  The creation date of the mount point is the disk creation date.
     */

    *create_date = stat_buf.st_ctime;


    /*
     *  Get the raw device of the disk.
     */

    if ( streq( mntent->mnt_type, "hrfs" ) )
    {
	/*
	 *  If this a hrfs, look the volume set name in the vstab table.
	 */

	vstab_fp = fopen ( VSTAB, "r" );
	if ( vstab_fp == NULL )
	{
	    CD_CHECK_SYSTEM( -1, VSTAB );
	}


	while ( fgets( buffer, 1023, vstab_fp ) != NULL )
	{
	    if ( strlen( buffer ) == 0 )
	    {
		continue;
	    }


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
	    cd_format_message( CD_E_VSTAB, mntent->mnt_fsname );
	    return( CD_E_VSTAB );
	}

	raw_device = tokens[1];
    }
    else
    {
	raw_device = mntent->mnt_fsname;
    }

    if ( ( fd = open( raw_device, O_RDONLY ) ) == -1 )
    {
	CD_CHECK_SYSTEM( fd, raw_device );
    }

    if ( lseek( fd, 16 * 2048, SEEK_SET ) != 16 * 2048 )
    {
	CD_CHECK_SYSTEM( -1, raw_device );
    }

    if ( read( fd, buf, 2048 ) != 2048 )
    {
	CD_CHECK_SYSTEM( -1, raw_device );
    }

    (void) close( fd );
    

    (void) strcpy( diskname, buf+40 );
    strfit( diskname );

    return( CD_SUCCESS );
}

#endif /* HAVE_MNTENT_H */
