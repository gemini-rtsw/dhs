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
*   Module Name:	ad/src/local_ad.c
*
*   Purpose:
*	Local miscelaneous routines for the ad library.
*
*   Routines:
*	int	ad_archive_search	: Compares archive structures.
*	int	ad_archive_sort		: Compares name with structure.
*	int	ad_comp_compare_srch	: Compares ext with structure.
*	int	ad_comp_compare_sort	: Compare compression structs.
*	int	ad_fmt_compare_srch	: Compares ext. with structure.
*	int	ad_fmt_compare_sort	: Compares file format structs.
*	int	ad_mfs_compare_sort	: Compares volume file system structs.
*	int	ad_mfs_compare_srch	: Compares volume file system structs.
*	int	ad_get_next_file	: Get the next file from the db.
*	int	ad_get_next_volume	: Get the next volume from the db.
*	int	ad_onefile_media	: Get the media flags for a single file.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: local_ad.c
*	Version Number	: 1.19
*	Release Number	: 1
*	Last Updated	: 07/28/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/04 JSD  :	Removed reference to diskname_orig for cd only.
*	97/02/17 JSD  :	Added archive.
*	97/06/20 SEC  :	Added ad_*_compare_*() routines. Mod'ed 
*			ad_get_next_file() and  ad_get_next_volume() to 
*			get compression and format and diskname_orig and
*			to not get server_id and host name. Added 
*			section to get remote file info. 
*	98/03/10 gzg  : Added features to handle tape tp.
*	98/06/23 sjg  : Added archive_search and archive_sort.
*	99/04/14 NRH  : Added separate uncompressed file size to tape.
*     2000/03/24 SDJ  : Added in missing comments. Added
*			ad_mfs_compare_sort() and ad_mfs_compare_ssrch()
*			ad_get_next_file() altered, for MFS table the 5th
*			column is the "crc" column for all other tables it
*			is "volumename".  All references to VD tables/type
*			changed to MFS.
*     2000/04/05 SDJ  : Added hostname to MFS structures.  Removed
*			volume_orig from MFS volume struct.
*     2000/04/10 SDJ  : Use db_crc32data() instead of db_intdata() to
*			retrieve the mfs crc's from the database.
*     2000/05/04 sjg  : Fixed extraction of mfs volume date in file get.
*     2000/05/10 NRH  : Added function ad_onefile_media.
*     2000/05/15 NRH  : Removed the volume structure initialization from
*			ad_get_next_volume.
*     2000/07/13 JSN  : Fixed bug in ad_get_next_file.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>

#if defined(SYBASE_DHS)
#include <sybfront.h>
#include <sybdb.h>
#endif

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "ad.h"
#if defined(SYBASE_DHS)
#include "db.h"
#endif
#include "local_ad.h"
#if defined(SYBASE_DHS)
#include "crc.h"
#endif

LOCAL_AD	local_ad;
boolean		ad_init_flag = FALSE;


/*+
************************************************************************
*
*   Function:	ad_archive_search
*
*   Purpose:
*	eptr_search compare routine for archive structures.
*
*   Values Returned:
*	int	>1	: Name is greater than name in structure.
*	int	0	: Name matches name in structure.
*	int	<1	: Name is less than name in structure.
*
************************************************************************
-*/

int		ad_archive_search
(
    char	*name,		/* (in)  Name to compare.		*/
    AD_ARCHIVE	*arch		/* (in)  Structure to compare to.	*/
)
{
    return( strcmp( name, arch->ada_name ) );
}

/*+
************************************************************************
*
*   Function:	ad_archive_sort
*
*   Purpose:
*	eptr_sort compare routine for archive structures.
*
*   Values Returned:
*	int	>1	: Name is greater than name in structure.
*	int	0	: Name matches name in structure.
*	int	<1	: Name is less than name in structure.
*
************************************************************************
-*/

int		ad_archive_sort
(
    AD_ARCHIVE	*arch1,		/* (in)  First structure to compare.	*/
    AD_ARCHIVE	*arch2		/* (in)  Second structure to compare.	*/
)
{
    return( strcmp( arch1->ada_name, arch2->ada_name ) );
}

/*+
************************************************************************
*
*   Function:	ad_comp_compare_srch
*
*   Purpose:
*	eptr_search compare routine for compression config info.
*
*   Values Returned:
*	int	>1	: String is greater than compression config info.
*	int	0	: String matches the compression config info.
*	int	<1	: String is less than compression config info.
*
************************************************************************
-*/

int		ad_comp_compare_srch
(
    char	*ext,		/* (in)  Extension to compare with.	*/
    AD_COMP_CONFIG	
		*ad_comp_cfg	/* (in)  Compression config info.	*/
)
{
    return( strcmp( ext, ad_comp_cfg->acc_ext ) );
}

/*+
************************************************************************
*
*   Function:	ad_comp_compare_sort
*
*   Purpose:
*	eptr_sort compare routine for compression config info.
*
*   Values Returned:
*	int	>1	: String is greater than compression config info.
*	int	0	: String matches the compression config info.
*	int	<1	: String is less than compression config info.
*
************************************************************************
-*/

int		ad_comp_compare_sort
(
    AD_COMP_CONFIG	
		*ad_comp_cfg1,	/* (in)  Compression config info.	*/
    AD_COMP_CONFIG	
		*ad_comp_cfg2	/* (in)  Compression config info.	*/
)
{
    return( strcmp( ad_comp_cfg1->acc_ext, ad_comp_cfg2->acc_ext ) );
}

/*+
************************************************************************
*
*   Function:	ad_fmt_compare_srch
*
*   Purpose:
*	eptr_search compare routine for format config info.
*
*   Values Returned:
*	int	>1	: String is greater than format config info.
*	int	0	: String matches the format config info.
*	int	<1	: String is less than format config info.
*
************************************************************************
-*/

int		ad_fmt_compare_srch
(
    char	*ext,		/* (in)  Extension to compare with.	*/
    AD_FMT_CONFIG	
		*ad_fmt_cfg	/* (in)  Compression config info.	*/
)
{
    return( strcmp( ext, ad_fmt_cfg->afc_ext ) );
}

/*+
************************************************************************
*
*   Function:	ad_fmt_compare_sort
*
*   Purpose:
*	eptr_sort compare routine for format config info.
*
*   Values Returned:
*	int	>1	: String is greater than format config info.
*	int	0	: String matches the format config info.
*	int	<1	: String is less than format config info.
*
************************************************************************
-*/

int		ad_fmt_compare_sort
(
    AD_FMT_CONFIG	
		*ad_fmt_cfg1,	/* (in)  Compression config info.	*/
    AD_FMT_CONFIG	
		*ad_fmt_cfg2	/* (in)  Compression config info.	*/
)
{
    return( strcmp( ad_fmt_cfg1->afc_ext, ad_fmt_cfg2->afc_ext ) );
}

/*+
************************************************************************
*
*   Function:	ad_mfs_compare_srch
*
*   Purpose:
*	eptr_sort compares mountable file system (MFS) struct. with a
*	MFS config information.
*
*   Values Returned:
*	int	>1	: String is greater than format config info.
*	int	0	: String matches the format config info.
*	int	<1	: String is less than format config info.
*
************************************************************************
-*/

int		ad_mfs_compare_srch
(
    MFS_VOL_INFO
    		*vol_info,	/* (in)  MFS volume info.		*/
    AD_MFS_CONFIG	
		*ad_mfs_cfg	/* (in)  MFS config info.		*/
)
{
    int		ret;

    ret = strcmp( vol_info->fsv_type, ad_mfs_cfg->amfsc_medium_type );

    if ( ret == 0 )
    {
	ret = strcmp( vol_info->fsv_format, ad_mfs_cfg->amfsc_format );
    }
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	ad_mfs_compare_sort
*
*   Purpose:
*	eptr_sort compare routine for mountable file system config info.
*
*   Values Returned:
*	int	>1	: String is greater than format config info.
*	int	0	: String matches the format config info.
*	int	<1	: String is less than format config info.
*
************************************************************************
-*/

int		ad_mfs_compare_sort
(
    AD_MFS_CONFIG	
		*ad_mfs_cfg1,	/* (in)  Compression config info.	*/
    AD_MFS_CONFIG	
		*ad_mfs_cfg2	/* (in)  Compression config info.	*/
)
{
    int		ret;

    ret = strcmp( ad_mfs_cfg1->amfsc_medium_type,
	    ad_mfs_cfg2->amfsc_medium_type );

    if ( ret == 0 )
    {
	ret = strcmp( ad_mfs_cfg1->amfsc_format, ad_mfs_cfg2->amfsc_format );
    }
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	ad_get_next_file
*
*   Purpose:
*	Gets the next file from the database.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_FILE	: No files available.
*	int	AD_E_DB		: Error in the sybase db lib.
*	int	AD_E_DB_LIB	: Error in the CADC db lib.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_get_next_file
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    AD_FILE	*ad_file	/* (out) The file read.			*/
)
{
    int		medium_int;	/* Integer version of the media type.	*/
    char	mount_flag;
    int		status;


    if ( ( status = dbnextrow( dbproc ) ) == NO_MORE_ROWS )
    {
	return( AD_NO_FILE );
    }

    AD_CHECK_DB( status );


    /*
     *  Get the information common to all media.
     */

    AD_CHECK_DB_LIB( db_intdata( dbproc, 1, & medium_int ) );
    ad_file->adf_volume_medium = medium_int;
    AD_CHECK_DB_LIB( db_intdata( dbproc, 2, &( ad_file->adf_filesize ) ) );
    AD_CHECK_DB_LIB( db_stringdata( dbproc, 3, ad_file->adf_file_id,
	    sizeof( ad_file->adf_file_id ) ) );
    AD_CHECK_DB_LIB( db_chardata( dbproc, 4, &( ad_file->adf_status ) ) );
    AD_CHECK_DB_LIB( db_stringdata( dbproc, 6, ad_file->adf_archive,
	    sizeof( ad_file->adf_archive ) ) );
    AD_CHECK_DB_LIB( db_stringdata( dbproc, 7, ad_file->adf_volume_type,
	    sizeof( ad_file->adf_volume_type ) ) );
    AD_CHECK_DB_LIB( db_chardata( dbproc, 8, &( ad_file->adf_compression ) ) );
    AD_CHECK_DB_LIB( db_chardata( dbproc, 9, &( ad_file->adf_format ) ) );

    /*
     *  Get the archive specific location.
     */

    switch ( ad_file->adf_volume_medium )
    {
	case	AD_MED_CD:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 10,
		    ad_file->adf_location.cd.cdl_filename,
		    sizeof( ad_file->adf_location.cd.cdl_filename ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 11, 
		    &( ad_file->adf_filesize_uncomp ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 12,
		    ad_file->adf_location.cd.cdl_mount_point,
		    sizeof( ad_file->adf_location.cd.cdl_mount_point ) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 13, &mount_flag ) );
	    if ( mount_flag == 'Y' )
	    {
		ad_file->adf_mounted = TRUE;
	    }
	    else
	    {
		ad_file->adf_mounted = FALSE;
	    }
	    break;

	case	AD_MED_DB:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 10,
		    ad_file->adf_location.db.dbl_filename,
		    sizeof( ad_file->adf_location.db.dbl_filename ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 11,
		    ad_file->adf_location.db.dbl_database_name,
		    sizeof( ad_file->adf_location.db.dbl_database_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 12,
		    ad_file->adf_location.db.dbl_table_name,
		    sizeof( ad_file->adf_location.db.dbl_table_name ) ) );
	    ad_file->adf_mounted = TRUE;
	    break;

	case	AD_MED_MD:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 10,
		    ad_file->adf_location.md.mdl_filename,
		    sizeof( ad_file->adf_location.md.mdl_filename ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 11, 
		    &( ad_file->adf_filesize_uncomp ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 12,
		    ad_file->adf_location.md.mdl_mount_point,
		    sizeof( ad_file->adf_location.md.mdl_mount_point ) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 13, &mount_flag ) );
	    if ( mount_flag == 'Y' )
	    {
		ad_file->adf_mounted = TRUE;
	    }
	    else
	    {
		ad_file->adf_mounted = FALSE;
	    }
	    break;

	case	AD_MED_MFS:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 10,
		    ad_file->adf_location.mfs.fsl_filename,
		    sizeof( ad_file->adf_location.mfs.fsl_filename ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 11, 
		    &( ad_file->adf_filesize_uncomp ) ) );
	    if ( ( status = db_stringdata( dbproc, 12,
		    ad_file->adf_location.mfs.fsl_mount_point,
		    sizeof( ad_file->adf_location.mfs.fsl_mount_point ) ) ) ==
		    DB_NULL_VALUE )
	    {
		ad_file->adf_location.mfs.fsl_mount_point[0] = '\0';
		ad_file->adf_mounted = FALSE;
	    }
	    else
	    {
		AD_CHECK_DB_LIB( status );
		ad_file->adf_mounted = TRUE;
	    }
	    
	    if ( ( status = db_stringdata( dbproc, 13,
		    ad_file->adf_location.mfs.fsl_hostname,
		    sizeof( ad_file->adf_location.mfs.fsl_hostname ) ) ) ==
		    DB_NULL_VALUE )
	    {
		ad_file->adf_location.mfs.fsl_hostname[0] = '\0';
	    }
	    else
	    {
		AD_CHECK_DB_LIB( status );
	    }
	    
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 14,
		    ad_file->adf_volume_format,
		    sizeof( ad_file->adf_volume_format ) ) );
	    AD_CHECK_DB_LIB( db_crc32data( dbproc, 15,
		    (crc32_t*) &(ad_file->adf_crc) ) );
	    break;

	case	AD_MED_OD:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 10, 
		    &( ad_file->adf_location.od.odl_dirsectoff ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 11,
		    ad_file->adf_location.od.odl_filename,
		    sizeof( ad_file->adf_location.od.odl_filename ) ) );
	    ad_file->adf_filesize_uncomp = ad_file->adf_filesize;
	    ad_file->adf_mounted = FALSE;
	    break;

	case	AD_MED_TP:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 10, 
	            ad_file->adf_location.tp.tpl_location,
		    sizeof( ad_file->adf_location.tp.tpl_location ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 11,
		    ad_file->adf_location.tp.tpl_filename,
		    sizeof( ad_file->adf_location.tp.tpl_filename ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 12, 
		    &( ad_file->adf_filesize_uncomp ) ) );
	    ad_file->adf_mounted = FALSE;
	    break;

	case 	AD_MED_REMOTE:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5, ad_file->adf_volume_name,
		    sizeof( ad_file->adf_volume_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 9,
		    ad_file->adf_location.rs.rsl_server_id,
		    sizeof( ad_file->adf_location.rs.rsl_server_id ) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 10, &mount_flag ) );
	    if ( mount_flag == 'Y' )
	    {
		ad_file->adf_mounted = TRUE;
	    }
	    else
	    {
		ad_file->adf_mounted = FALSE;
	    }
	    break;
    }

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_get_next_volume
*
*   Purpose:
*	Gets the next volume from the database.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_VOLUME	: No volume available.
*	int	AD_E_DB		: Error in the sybase db lib.
*	int	AD_E_DB_LIB	: Error in the CADC db lib.
*
************************************************************************
-*/

int		ad_get_next_volume
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    AD_VOLUME	*ad_volume	/* (out) The volume read.		*/
)
{
    int		medium_int;	/* Integer version of the media type.	*/
    char	mount_flag;
    int		status;


    if ( ( status = dbnextrow( dbproc ) ) == NO_MORE_ROWS )
    {
	return( AD_NO_VOLUME );
    }

    AD_CHECK_DB( status );


    /*
     *  Get the information common to all media.
     */

    AD_CHECK_DB_LIB( db_intdata( dbproc, 1, & medium_int ) );
	    ad_volume->adv_medium_type = medium_int;
    AD_CHECK_DB_LIB( db_stringdata( dbproc, 2, ad_volume->adv_archive,
                sizeof( ad_volume->adv_archive ) ) );
    AD_CHECK_DB_LIB( db_stringdata( dbproc, 3, ad_volume->adv_volume_name,
	    sizeof( ad_volume->adv_volume_name ) ) );


    /*
     *  Get the archive specific location.
     */

    switch ( ad_volume->adv_medium_type )
    {
	case	AD_MED_CD:
            AD_CHECK_DB_LIB( db_stringdata( dbproc, 4,
		    ad_volume->adv_vol_info.cd.cdv_diskname_orig,
		    sizeof( ad_volume->adv_vol_info.cd.cdv_diskname_orig ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 5,
		    (int *) &(ad_volume->adv_vol_info.cd.cdv_creation_date) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 6,
		    (int *) &(ad_volume->adv_vol_info.cd.cdv_ingest_date) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 7, &mount_flag ) );
	    if ( mount_flag == 'Y' )
	    {
		ad_volume->adv_mounted = TRUE;
	    }
	    else
	    {
		ad_volume->adv_mounted = FALSE;
	    }
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 8,
		    ad_volume->adv_vol_info.cd.cdv_mount_point,
		    sizeof( ad_volume->adv_vol_info.cd.cdv_mount_point ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 9,
		    &( ad_volume->adv_vol_info.cd.cdv_nfiles ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 10,
		    &( ad_volume->adv_vol_info.cd.cdv_nkbytes ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 11,
		    ad_volume->adv_vol_info.cd.cdv_type,
		    sizeof( ad_volume->adv_vol_info.cd.cdv_type ) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 12,
		    &ad_volume->adv_vol_info.cd.cdv_verified ) );
	    break;

	case	AD_MED_DB:
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 4,
		    ad_volume->adv_vol_info.db.dbv_database_name,
		    sizeof( ad_volume->adv_vol_info.db.dbv_database_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5,
		    ad_volume->adv_vol_info.db.dbv_table_name,
		    sizeof( ad_volume->adv_vol_info.db.dbv_table_name ) ) );
	    ad_volume->adv_mounted = TRUE;
	    break;

	case	AD_MED_MD:
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 4, &mount_flag ) );
	    if ( mount_flag == 'Y' )
	    {
		ad_volume->adv_mounted = TRUE;
	    }
	    else
	    {
		ad_volume->adv_mounted = FALSE;
	    }
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 5,
		    ad_volume->adv_vol_info.md.mdv_mount_point,
		    sizeof( ad_volume->adv_vol_info.md.mdv_mount_point ) ) );
	    break;

	case	AD_MED_MFS:
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 4,
		    (int *) &(ad_volume->adv_vol_info.mfs.fsv_creation_date) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 5,
		    (int *) &(ad_volume->adv_vol_info.mfs.fsv_ingest_date) ) );
	    if ( ( status = db_stringdata( dbproc, 6,
		    ad_volume->adv_vol_info.mfs.fsv_hostname,
		    sizeof( ad_volume->adv_vol_info.mfs.fsv_hostname ) ) ) ==
		    DB_NULL_VALUE )
	    {
		ad_volume->adv_vol_info.mfs.fsv_hostname[0] = '\0';
	    }
	    else
	    {
		AD_CHECK_DB_LIB( status );
	    }

	    status = db_stringdata( dbproc, 7,
		    ad_volume->adv_vol_info.mfs.fsv_mount_point,
		    sizeof( ad_volume->adv_vol_info.mfs.fsv_mount_point ) );
	    if ( status == DB_SUCCESS )
	    {
		ad_volume->adv_mounted = TRUE;
	    }
	    else if ( status == DB_NULL_VALUE )
	    {
		ad_volume->adv_vol_info.mfs.fsv_mount_point[0] = '\0';
		ad_volume->adv_mounted = FALSE;
	    }
	    else
	    {
		AD_CHECK_DB_LIB( status );
	    }

	    AD_CHECK_DB_LIB( db_intdata( dbproc, 8,
		    &( ad_volume->adv_vol_info.mfs.fsv_nfiles ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 9,
		    &( ad_volume->adv_vol_info.mfs.fsv_nkbytes ) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 10,
		    &ad_volume->adv_vol_info.mfs.fsv_verified ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 11,
		    ad_volume->adv_vol_info.mfs.fsv_type,
		    sizeof( ad_volume->adv_vol_info.mfs.fsv_type ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 12,
		    ad_volume->adv_vol_info.mfs.fsv_format,
		    sizeof( ad_volume->adv_vol_info.mfs.fsv_format ) ) );
	    AD_CHECK_DB_LIB( db_crc32data( dbproc, 13,
		    (crc32_t*) &(ad_volume->adv_crc ) ) );
	    break;

	case	AD_MED_OD:
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 4,
		    (int *) &(ad_volume->adv_vol_info.od.odv_creation_date) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 5,
		    (int *) &(ad_volume->adv_vol_info.od.odv_dirsectoff) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 6,
		    ad_volume->adv_vol_info.od.odv_diskname_orig,
		    sizeof( ad_volume->adv_vol_info.od.odv_diskname_orig ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 7,
		    (int *) &(ad_volume->adv_vol_info.od.odv_ingest_date) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 8,
		    &ad_volume->adv_vol_info.od.odv_verified ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 9,
		    &( ad_volume->adv_vol_info.od.odv_nfiles ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 10,
		    &( ad_volume->adv_vol_info.od.odv_nkbytes ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 11,
		    ad_volume->adv_vol_info.od.odv_reverse_name,
		    sizeof( ad_volume->adv_vol_info.od.odv_reverse_name ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 12,
		    ad_volume->adv_vol_info.od.odv_type,
		    sizeof( ad_volume->adv_vol_info.od.odv_type ) ) );
	    break;

	case	AD_MED_TP:
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 4,
		    (int *) &(ad_volume->adv_vol_info.tp.tpv_ingest_date) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 5,
		    &( ad_volume->adv_vol_info.tp.tpv_nfiles ) ) );
	    AD_CHECK_DB_LIB( db_intdata( dbproc, 6,
		    &( ad_volume->adv_vol_info.tp.tpv_nkbytes ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 7,
		    ad_volume->adv_vol_info.tp.tpv_tp_script,
		    sizeof( ad_volume->adv_vol_info.tp.tpv_tp_script ) ) );
	    AD_CHECK_DB_LIB( db_stringdata( dbproc, 8,
		    ad_volume->adv_vol_info.tp.tpv_type,
		    sizeof( ad_volume->adv_vol_info.tp.tpv_type ) ) );
	    AD_CHECK_DB_LIB( db_chardata( dbproc, 9,
		    &ad_volume->adv_vol_info.tp.tpv_verified ) );
	    break;
    }

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_onefile_media
*
*   Purpose:
*	Get the media flags for a single file id.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		ad_onefile_media
(
    DBPROCESS	*dbproc,	/* (in)  The db process to use.		*/
    char	*archive,	/* (in)  The archive containing the file.*/
    char	*file_id,	/* (in)  The file id to check.		*/
    AD_MEDIA_MASK		/* (out) The media mask for the file.	*/
    		*file_mask
)
{
    char	media_name[AD_MEDNAME_LEN+1];
    int		s;


    /*
     *  Get the list of media types which contain the file.
     */

    AD_CHECK_DB_LIB( db_simple_query( dbproc,
	    "archive..get_file_media '%s', '%s'", file_id, archive ) );


    /*
     * Set the appropriate bit of the file mask when the file is on a given
     * media type.
     */

    *file_mask = 0;
    do
    {
	if ( DBROWS( dbproc ) != FAIL )
	{
	    while ( ( s = dbnextrow( dbproc ) ) != NO_MORE_ROWS )
	    {
		AD_CHECK_DB_LIB( db_stringdata( dbproc, 1, media_name, 
			sizeof( media_name ) ) );
		if ( strcmp( media_name, AD_MEDNAME_CD ) == 0 )
		{
		    *file_mask |= AD_MEDIA_CD;
		}
		else if ( strcmp( media_name, AD_MEDNAME_DB ) == 0 )
		{
		    *file_mask |= AD_MEDIA_DB;
		}
		else if ( strcmp( media_name, AD_MEDNAME_MD ) == 0 )
		{
		    *file_mask |= AD_MEDIA_MD;
		}
		else if ( strcmp( media_name, AD_MEDNAME_OD ) == 0 )
		{
		    *file_mask |= AD_MEDIA_OD;
		}
		else if ( strcmp( media_name, "DVD" ) == 0 )
		{
		    *file_mask |= AD_MEDIA_DVD;
		}
		else if ( strcmp( media_name, "EXA" ) == 0 )
		{
		    *file_mask |= AD_MEDIA_EXA;
		}
		else if ( strcmp( media_name, "DLT" ) == 0 )
		{
		    *file_mask |= AD_MEDIA_DLT;
		}
		else
		{
		    ad_format_message( AD_E_MEDIUM_TYPE, media_name );
		    AD_CHECK_DB( dbcanquery( dbproc ) );
		    return( AD_E_MEDIUM_TYPE );
		}
	    }
	}
    }
    while ( ( s = dbresults( dbproc ) ) == SUCCEED );

    if ( s == FAIL )
    {
	AD_CHECK_DB( s );
    }

    return( AD_SUCCESS );
}
#endif
