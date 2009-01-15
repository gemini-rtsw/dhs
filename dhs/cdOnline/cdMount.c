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
*   Module Name:	cdOnline/src/cdMount.c
*
*   Purpose:
*	Contains routines to mount and unmount cds.
*
*   Routines:
*	int	cdMount		: Mount a cd.
*	int	cdUnmount	: Unmount a cd.
*
*   Date		: Dec 13, 1996
*
*   SCCS data		: @(#)
*	Module Name	: cdMount.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 02/05/97
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/05 JSD :	Got rid of append to mount_point the machine.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <time.h>
#include <stdlib.h>
#include <sys/utsname.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "ad.h"
#include "cd.h"

#include "globals.h"

/*+
************************************************************************
*
*   Function:	cdMount
*
*   Purpose:
*	Updates the cds table to indicate a disk has been mounted.
*
*   Values Returned:
*	int	CO_SUCCESS	: Normal completion.
*	int	CO_E_AD		: Error in the ad library.
*	int	CO_E_CD		: Error in the cd library.
*	int	CO_E_NO_VOLUME	: Volume not found in the archive.
*
************************************************************************
-*/

int		cdMount
(
    char	*path		/* (in)  A path on the cd to mount.	*/
)
{
    time_t	createDate;	/* Creation time of the cd.		*/
    AD_VOLNAME	diskname;	/* Name of the cd.			*/
    char	mountPoint[PATH_MAX];
    int		status;
    char	*timeStr;
    AD_VOLUME	volume;		/* volume information from AD.		*/


    (void) strcpy( mountPoint, path );

    CO_CHECK_CD( cd_check_mnt( mountPoint, diskname, &createDate ) );


    /*
     *  Look up the volume in the archive.
     */

    CO_CHECK_AD( status = ad_volume_get( AD_VG_NAME_ORIG, &volume, 
	    globals.gDbproc, AD_MED_CD, diskname, createDate ) );


    if ( status == AD_NO_VOLUME )
    {
	timeStr = ctime( &createDate );
	formatMessage( CO_E_NO_VOLUME, diskname, timeStr );
	printMessage( msg, MESS_ERROR );
	return( CO_E_NO_VOLUME );
    }

    (void) strcpy( volume.adv_vol_info.cd.cdv_mount_point, mountPoint );
    //volume.adv_vol_info.cd.cdv_mounted = TRUE; XXX allan: not defined


    /*
     *  Update the volume information.
     */

    CO_CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );

    
    formatMessage( CO_VOLUME_MOUNTED, volume.adv_volume_name, 
	    mountPoint );
    printMessage( msg, MESS_INFO );

    return( CO_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	cdUnmount
*
*   Purpose:
*	Marks a cd as being not mounted in the cds table.
*
*   Values Returned:
*	int	CO_SUCCESS	: Normal completion.
*	int	CO_E_AD		: Error in the ad library.
*	int	CO_E_NO_VOLNAME	: Volume name not found in the archive.
*
************************************************************************
-*/

int		cdUnmount
(
    AD_VOLNAME	volName	/* (in)  The volume to unmount.		*/
)
{
    int		status;
    AD_VOLUME	volume;		/* volume information from AD.		*/


    /*
     *  Get the volume inforation from the archive.
     */

    CO_CHECK_AD( status = ad_volume_get( AD_VG_NAME, &volume, 
	    globals.gDbproc, AD_MED_CD, volName ) );

    if ( status == AD_NO_VOLUME )
    {
	formatMessage( CO_E_NO_VOLNAME, volName );
	printMessage( msg, MESS_ERROR );
	return( CO_E_NO_VOLNAME );
    }


    /* 
     *  Set the volume information to indicate the volume is not mounted.
     */

    volume.adv_vol_info.cd.cdv_mount_point[0] = '\0';
    //volume.adv_vol_info.cd.cdv_mounted = FALSE; XXX allan: not defined


    /*
     *  Update the volume information.
     */

    CO_CHECK_AD( ad_volume_put( &volume, globals.gDbproc ) );

    
    formatMessage( CO_VOLUME_UNMOUNTED, volume.adv_volume_name );
    printMessage( msg, MESS_INFO );


    return( CO_SUCCESS );
}
