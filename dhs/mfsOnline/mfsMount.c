/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999
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
*   Module Name:	mfsOnline/src/mfsMount.c
*
*   Purpose:
*	Contains routines to mount and unmount mountable media.
*
*   Routines:
*	int	mfsMount	: Mount a mountable file system.
*	int	mfsUnmount	: Unmount a mountable file system.
*
*   Date		: Feb 23, 1999
*
*   SCCS data           : @(#)
*       Module Name     : mfsMount.c
*       Version Number  : 1.2
*       Release Number  : 1
*       Last Updated    : 04/10/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/02/23 SDJ  	: Copied from cdOnline.
*     2000/03/23 SDJ	: Switched from dvdOnline to mfsOnline.
*     2000/04/10 SDJ	: Updated to latest CADC software standards.
*			  Added support for using mfs crc's instead
*			  of volume names, as well as code for handling
*			  multiple volumes if many are found.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 * General UNIX include files.
 ************************************************************************
 */

#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/utsname.h>


/*
 ************************************************************************
 * SYBASE include files
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
#include "gen_msg.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "ad.h"
#include "mfs.h"

/*
 ************************************************************************
 *  mfsOnline include files.
 ************************************************************************
 */

#include "globals.h"

/*+
************************************************************************
*
*   Function:	mfsMount
*
*   Purpose:
*	Updates the vds table to indicate a disk has been mounted.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AD		: Error in the ad library.
*	int	E_MFS		: Error in the mfs library.
*	int	E_NO_VOLUME	: Volume not found in the archive.
*
************************************************************************
-*/

int		mfsMount
(
    char	*path		/* (in)  A path on the mfs to mount.	*/
)
{
    time_t	createDate;	/* Time item was created.		*/
    AD_VOLFORMAT
		format;		/* File system format on system.	*/
    char	mountPoint[PATH_MAX];
    				/* Mount point of the file system.	*/
    AD_VOLNAME	name;		/* Name of the mountable file system.	*/
    int		status;		/* Function return status.		*/
    char	*timeStr;	/* Creation time in string format.	*/
    AD_VOLUME	volume;		/* volume information from AD.		*/


    /*
     * Find the name, creation date and volume type of the moutable
     * file system located at the given path.
     */
    
    (void) strcpy( mountPoint, path );
    CHECK_MFS( mfs_check_mnt( mountPoint, name, &globals.gCreateDate,
	    format ) );

    if ( streq( globals.gVolumeType, "" ) )
    {
	CHECK( getType() );
    }


    /*
     *  Look up the volume in the archive.
     */

    CHECK_AD( status = ad_volume_get( AD_VG_CRC, &volume, 
	    globals.gDbproc, AD_MED_MFS, globals.gArchive, name,
	    globals.gVolumeType, globals.gCreateDate ) );
    

    if ( status == AD_NO_VOLUME )
    {
	timeStr = ctime( &createDate );
	timeStr[strlen(timeStr) - 1] = '\0';
	formatMessage( E_NO_VOLUME, globals.gArchive, name,
		globals.gVolumeType, timeStr );
	printMessage( msg, MESS_ERROR );
	return( E_NO_VOLUME );
    }


    /*
     * Set the volumes mount point and host machine.
     */
    
    (void) strcpy( volume.adv_vol_info.mfs.fsv_mount_point, mountPoint );
    CHECK_SYSTEM( gethostname( volume.adv_vol_info.mfs.fsv_hostname,
	    AD_HOSTNAME_LEN ), name );

    
    /*
     *  Update the volume information.
     */

    CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );

    
    formatMessage( S_VOLUME_MOUNTED, volume.adv_volume_name,  mountPoint );
    printMessage( msg, MESS_OPER );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mfsUnmount
*
*   Purpose:
*	Marks a cd as being not mounted in the cds table.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AD		: Error in the ad library.
*	int	E_NO_VOLNAME	: Volume name not found in the archive.
*
************************************************************************
-*/

int		mfsUnmount
(
    AD_VOLNAME	volName		/* (in)  The volume to unmount.		*/
)
{
    boolean	addVolume;	/* Add volume to list?			*/
    int		i;
    int		status;		/* Function return status.		*/
    struct tm	*create_tm;	/* Creation date of volume.		*/
    AD_VOLUME	*volume_ptr;	/* volume pointer.			*/
    EPTR	*volume_list;	/* List of potential volumes.		*/
    


    /*
     *  Get the volume inforation from the archive.
     */

    volume_list = NULL;
    CHECK_NULL( volume_ptr = gen_alloc( sizeof( AD_VOLUME ) ) );
    CHECK_NULL( volume_list = eptr_alloc( 4 ) );
    
    for( status=ad_volume_get( AD_VG_NAME, volume_ptr, globals.gDbproc,
	    AD_MED_MFS, volName );
         status == AD_SUCCESS;
         status=ad_volume_get( AD_VG_NEXT, volume_ptr, globals.gDbproc ) )
    {
	if (  globals.gVolumeType[0] == '\0' &&
		globals.gArchive[0] == '\0' && 
		globals.gCreateDate < 0 )
	{
	    addVolume = TRUE;
	}
	else
	{
	    addVolume = TRUE;
	    if ( ( globals.gVolumeType[0] != '\0' && strne( globals.gVolumeType,
		    volume_ptr->adv_vol_info.mfs.fsv_type ) ) ||
		 ( globals.gArchive[0] != '\0' && strne( globals.gArchive,
			    volume_ptr->adv_archive ) ) ||
		 ( globals.gCreateDate > 0 && globals.gCreateDate !=
		     volume_ptr->adv_vol_info.mfs.fsv_creation_date ) )
	    {
		addVolume = FALSE;
	    }
	}
	

	if ( addVolume )
	{
	    /*
	     * Add the volume to the list.
	     */
	    
	    CHECK_NULL( volume_list = eptr_insert( volume_list,
		    eptrlen(volume_list), volume_ptr ) );
	    CHECK_NULL( volume_ptr = gen_alloc( sizeof( AD_VOLUME ) ) );
	}
    }
    
    if ( eptrlen( volume_list ) <= 0 )
    {
	formatMessage( E_NO_VOLNAME, volName );
	printMessage( msg, MESS_ERROR );
	return( E_NO_VOLNAME );
    }

    if ( status != AD_NO_VOLUME )
    {
	CHECK_AD( status );
    }
    

    if ( eptrlen( volume_list ) == 1 )
    {
	/*
	 * There was only one potential volume found so use it.
	 */

	CHECK_NULL( volume_ptr = eptritem( volume_list, 0 ) );
    }
    else if ( volume_list != NULL && eptrlen( volume_list ) > 1 )
    {
	formatMessage( S_VOLUME_LIST_HDR );
	for( i = 0; i < eptrlen( volume_list ); i++ )
	{
	    CHECK_NULL( volume_ptr = eptritem( volume_list, i ) );
	    create_tm = localtime(
		&(volume_ptr->adv_vol_info.mfs.fsv_creation_date) );
	    formatMessage( S_VOLUME_LIST_ENTRY, volume_ptr->adv_volume_name,
		    volume_ptr->adv_vol_info.mfs.fsv_type,
		    create_tm->tm_year+1900, create_tm->tm_mon,
		    create_tm->tm_mday, create_tm->tm_hour,
		    create_tm->tm_min, create_tm->tm_sec );
	    printMessage( msg, MESS_CON );
	}

	formatMessage( S_VOLUME_LIST_FTR );
	printMessage( msg, MESS_CON );
	eptr_free( volume_list, free );
	return( E_MULTI_VOLUMES );
    }
    
	
    /* 
     *  Set the volume information to indicate the volume is not mounted.
     */

    volume_ptr->adv_vol_info.mfs.fsv_mount_point[0] = '\0';
    volume_ptr->adv_vol_info.mfs.fsv_hostname[0] = '\0';


    /*
     *  Update the volume information.
     */

    CHECK_AD( ad_volume_put( volume_ptr, globals.gDbproc ) );

    
    formatMessage( S_VOLUME_UNMOUNTED, volume_ptr->adv_volume_name );
    printMessage( msg, MESS_OPER );


    return( S_SUCCESS );
}
