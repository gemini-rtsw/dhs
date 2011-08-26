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
*   Module Name:	ad.c
*
*   Purpose:
*	Contains the external routines for the ad library.
*
*   Routines:
*	AD_ARCHIVE *ad_archive_info
*	       			: Returns archive structure.
*	void	ad_close	: Frees memory that is used by the AD
*				  libarary.
*	int	ad_dbinfo_get	: Gets the database information for a 
*				  specified archive.
*	int	ad_file_del	: Removes a file from an archive.
*	int	ad_file_get	: Gets the file information for a specified 
*				  local file or files.
*	int	ad_file_info	: Returns info about compression and format.
*	int	ad_file_init	: Sets a file structure to its default 
*				  values.
*	int	ad_file_media	: Calcualtes the media mask flags for either
*				  a single file, or for a list of files.
*	char *	ad_file_name	: Returns the name of the file on medium.
*	int	ad_file_put	: Insert a file into the archive directories.
*	int	ad_init		: Initializes the ad library.
*	char *	ad_med_name	: Gets the name of a medium.
*	AD_MEDIUM
*	        ad_med_type	: Gets the enumerated type of a medium.
*	int	ad_remote_get	: Gets information about a remote file. 
*	int	ad_server_id_get: Gets the local dataServer's identity.
*	int	ad_volume_get	: Gets a volume from the archive.
*	int	ad_volume_init	: Sets a volume structure to its default
*				  values.
*	int	ad_volume_put	: Writes a volume to the volume table.
*	boolean	ad_volume_verify: Verifies correctness of volume info.
*static int	mediumFree	: Frees the medium information.
*
*   Date		: Nov 22, 1996.
*
*   Programmer		: Marc LeBlanc
*
*   Field SCCS data	: @(#)
*	Module Name	: ad.c
*	Version Number	: 1.21
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*
*   Modification History:
*	97/02/04 JSD  :	Removed reference to diskname_orig for cd only.
*	97/02/17 JSD  :	Added archive as an input param.
*	97/04/01 SEC  :	adDbInfoGet() now returns db server name too. 
*	97/06/20 SEC  : Many mods. Removed ref's to priority config struct.
*			Added ad_default_archive(). Mod'ed ad_file_get() to
*			use config list for local file table priority.
*			added ad_file_info(). Mod'ed all references to 
*			adf_filename to adf_file_id. Added ad_remote_get(),
*			Removed ref's to adv_server_name and adv_host_name.
*	97/07/16 SEC  : Moved db and db server names to local_ad struct.
*	97/08/25 SEC  :	Added remote table insertion to ad_file_put() and
*			ad_volume_put(). Mod'ed ad_remote_get() for table 
*			changes. Added ad_server_get().
*	97/10/06 SEC  :	Fixed bug in ad_file_info().
*	97/11/04 SJG  :	Modified second arg of ad_file_info to char *.
*	98/02/13 SJG  :	Added ad_file_name function.
*	98/03/10 gzg  : Added features to handle tape tp.
*	98/06/23 sjg  : Added ad_archive_info function.
*	98/08/31 jsd  : Added features to handle dvd.
*	98/02/03 SDJ  : Added ad_close() and updated code documentation!
*	99/02/24 SDJ  : Cleaned up documentation. Added ad_colse().
*     2000/03/24 SDJ  : Changed from vd (dvd) media type to mfs (mountable
*			file system).  mfs tables use crc rather than
*			a volume name.  Added ad_volume_verify() which
*			really only verifies mfs volumes.
*    2000/04/05 SDJ   : Removed fsv_mounted, fsv_volume_orig but added
*			fsv_hostname to the MFS_VOL_INFO structure.
*    2000/04/10 SDJ   : Used AD_CRC_UNKNOWN to initialiaze crc values.
*    2000/05/10 NRH   : Added the ad_file_media function.
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
#include "gen_str.h"
#include "gen_eptr.h"

#if defined(SYBASE_DHS)
#include "db.h"
#endif

#include "ad.h"
#include "local_ad.h"


/*+
************************************************************************
*
*   Function:	ad_close
*
*   Purpose:
*	Free any memory used by AD.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	ad_close
(
    void
)
{
    /*
     * free eptr lists.
     */

    if ( local_ad.la_archive_list != NULL )
    {
	eptr_free( local_ad.la_archive_list, free );
	local_ad.la_archive_list = NULL;
    }

    if ( local_ad.la_comp_list != NULL )
    {
	eptr_free( local_ad.la_comp_list, free );
	local_ad.la_comp_list = NULL;
    }

    if ( local_ad.la_fmt_list != NULL )
    {
	eptr_free( local_ad.la_fmt_list, free);
	local_ad.la_fmt_list = NULL;
    }
}

/*+
************************************************************************
*
*   Function:	ad_dbinfo_get
*
*   Purpose:
*	Gets the database information for a specified archive.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_INIT	: The ad library is not initialized.
*	int	AD_E_MEDIUM_ENUM: Not a valid medium. 
*
************************************************************************
-*/

#if defined(SYBASE_DHS)
int		ad_dbinfo_get
(
    AD_MEDIUM	medium,		/* (in)  The medium to return.		*/
    char	**server,	/* (out) The database server name.	*/
    char	**database,	/* (out) The database name.		*/
    char	**file_table,	/* (out) The file table name.		*/
    char	**vol_table	/* (out) The volume table name.		*/
)
{
    AD_MEDIUM_CONFIG	
		*medium_config;	/* the configuration for a medium.	*/


    medium_config = NULL;

    AD_CHECK_INIT;

    switch ( medium )
    {
	case	AD_MED_CD:
	    AD_CHECK_MEDIUM_CD;
	    medium_config = &( local_ad.la_config_cd );
	    break;

	case	AD_MED_DB:
	    AD_CHECK_MEDIUM_DB;
	    medium_config = &( local_ad.la_config_db );
	    break;

	case	AD_MED_MD:
	    AD_CHECK_MEDIUM_MD;
	    medium_config = &( local_ad.la_config_md );
	    break;

	case	AD_MED_MFS:
	    AD_CHECK_MEDIUM_MFS;
	    medium_config = &( local_ad.la_config_mfs );
	    break;

	case	AD_MED_OD:
	    AD_CHECK_MEDIUM_OD;
	    medium_config = &( local_ad.la_config_od );
	    break;

	case	AD_MED_TP:
	    AD_CHECK_MEDIUM_TP;
	    medium_config = &( local_ad.la_config_tp );
	    break;

	default:
	    ad_format_message( AD_E_MEDIUM_ENUM, medium );
	    return( AD_E_MEDIUM_ENUM );
    }

    if ( server != NULL )
    {
	*server = local_ad.la_db_server_name;
    }

    if ( database != NULL )
    {
	*database = local_ad.la_database_name;
    }

    if ( file_table != NULL )
    {
	*file_table = medium_config->amc_file_table_name;
    }

    if ( vol_table != NULL )
    {
	*vol_table = medium_config->amc_vol_table_name;
    }

    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_archive_info
*
*   Purpose:
*	Returns the archive structure corresponding to the name passed in.
*   If the name is NULL, then the default archive is returned.
*
*   Values Returned:
*	AD_ARCHIVE *		: The address of the archive structure.
*	NULL			: No matching archive found.
*
************************************************************************
-*/

AD_ARCHIVE	*ad_archive_info
(
    char	*name		/* Name (or NULL) to search for.	*/
)
{
    AD_ARCHIVE	*archive;
    int		index;
    char	*search_name;

    if ( name == NULL && local_ad.la_archive == NULL )
    {
	return( NULL );
    }

    if ( name == NULL )
    {
    	search_name = local_ad.la_archive;
    }
    else
    {
    	search_name = name;
    }

    if ( eptr_search( local_ad.la_archive_list, search_name,
	    &index, ad_archive_search ) )
    {
    	archive = (AD_ARCHIVE *) eptritem( local_ad.la_archive_list, index );
    }
    else
    {
    	archive = NULL;
    }

    return( archive );
}

/*+
************************************************************************
*
*   Function:	ad_file_del
*
*   Purpose:
*	Deletes a file from an archive.
*
*	char	*volname;
*	char	*file_name;
*	ad_file_del( DBPROCESS *, AD_MED_CD | AD_MED_DB | AD_MED_MD |
*			AD_MED_OD | AD_MED_TP | AD_MED_MFS, volname );
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_CANNOT_DEL	: Cannot delete from specified medium type.
*	int	AD_E_MEDIUM_ENUM: Invalid medium type.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_file_del
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    AD_MEDIUM	medium,		/* (in)  The medium to delete from.	*/
    char	*volname,	/* (in)  The volume name.		*/
    ...				/* (in)  Medium dependant location info.*/
)
{
    va_list	args;
    char	*filename;


    switch ( medium )
    {
	case AD_MED_CD:
	case AD_MED_DB:
	case AD_MED_MFS:
	case AD_MED_OD:
	case AD_MED_TP:
	    ad_format_message( AD_E_CANNOT_DEL, medium );
	    break;
	case AD_MED_MD:
	    AD_CHECK_MEDIUM_MD;
	    va_start( args, volname );
	    filename = (char *) va_arg( args, char * );
	    va_end( args );
	    AD_CHECK( ad_md_file_del( dbproc, volname, filename ) );
	    break;
	default:
	    ad_format_message( AD_E_MEDIUM_ENUM, medium );
	    return( AD_E_MEDIUM_ENUM );
    }

    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_file_get
*
*   Purpose:
*	Gets a specified file or list of files from the archive directories.
*	If more than one file matches the specifiecation, the first file is 
*	returned with the first call to this function, and any others should
*	be retrieved by calling this function with a select_type of
*	AD_FG_NEXT.
*
*	char	*disk_name;
*	char	*file_name;
*	ad_file_get( AD_FG_LOCATION, AD_FILE *, DBPROCESS *, 
*			AD_MED_CD | AD_MED_MD | AD_MED_MFS, 
*			disk_name, file_name );
*
*	char	*database_name;
*	char	*table_name;
*	char	*dataset_name;
*	ad_file_get( AD_FG_LOCATION, AD_FILE *, DBPROCESS *, AD_MED_DB,
*			database_name, table_name, dataset_name );
*
*	char	*disk_name;
*	int	offset;
*	ad_file_get( AD_FG_LOCATION, AD_FILE *, DBPROCESS *, AD_MED_OD, 
*			disk_name, offset );
*
*	char	*tape_name;
*	char	*file_name;
*	char	*location;
*	ad_file_get( AD_FG_LOCATION, AD_FILE *, DBPROCESS *, AD_MED_TP, 
*			tape_name, file_name, location );
*
*	char	*archive;
*	char	*file_name;	
*	ad_file_get( AD_FG_NAME, AD_FILE *, DBPROCESS *, AD_MED_*, archive,
*			file_name );
*
*	char	*volume_name;
*	ad_file_get( AD_FG_VOLUME, AD_FILE *, DBPROCESS *, AD_MED_*, 
*		volume_name );
*
*	ad_file_get( AD_FG_NEXT, AD_FILE *, DBPROCESS * );
*
*	In the case of AD_FG_NAME, file_name can contain sybase wildcard
*	characters.
*	
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_FILE	: No file available.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_INIT	: Library not initialized.
*	int	AD_E_MEDIUM_ENUM: Invalid medium type.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_file_get
(
    AD_FILEGET	select_type,	/* (in)  The type of selction.		*/
    AD_FILE	*ad_file,	/* (out) The adfile to fill in.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...				/* (in)  The optional qualifiers for the*/
				/*	 selection type.		*/
)
{
    va_list	args;
    static int	current_medium_index;
    AD_MEDIUM	medium;
    AD_MEDIUM	current_medium;
    AD_MEDIUM_CONFIG	
		*current_medium_cfg;
    int		status;


    AD_CHECK_INIT;

    if ( select_type != AD_FG_NEXT )
    {
	va_start( args, dbproc );
	medium = (AD_MEDIUM) va_arg( args, AD_MEDIUM );
	va_end( args );

	if ( medium == AD_MED_BEST && 
		local_ad.la_media_list != NULL )
	{
	    current_medium_index = 0;
	    current_medium_cfg = (AD_MEDIUM_CONFIG *) eptritem( 
		    local_ad.la_media_list, current_medium_index );
	    current_medium = current_medium_cfg->amc_medium;
	}
	else
	{
	    current_medium = medium;
	}

	AD_CHECK_DB( dbcanquery( dbproc ) );
    }
    else
    {
	medium = AD_MED_UNKNOWN;
	current_medium = AD_MED_UNKNOWN;
    }


    for ( ;; )
    {
	if ( select_type != AD_FG_NEXT )
	{
	    va_start( args, dbproc );


	    /*
	     *  Ignore the first argument.
	     */

	    (void) va_arg( args, AD_MEDIUM );
	    switch ( current_medium ) 
	    {
		case AD_MED_CD:
		    AD_CHECK( ad_cd_file_get( select_type, dbproc, args ) );
		    break;

		case AD_MED_DB:
		    AD_CHECK( ad_db_file_get( select_type, dbproc, args ) );
		    break;

		case AD_MED_MD:
		    AD_CHECK( ad_md_file_get( select_type, dbproc, args ) );
		    break;

		case AD_MED_MFS:
		    AD_CHECK( ad_mfs_file_get( select_type, dbproc, args ) );
		    break;

		case AD_MED_OD:
		    AD_CHECK( ad_od_file_get( select_type, dbproc, args ) );
		    break;

		case AD_MED_TP:
		    AD_CHECK( ad_tp_file_get( select_type, dbproc, args ) );
		    break;

		case AD_MED_UNKNOWN:
		case AD_MED_BEST:
		default:
		    ad_format_message( AD_E_MEDIUM_ENUM, current_medium );
		    return( AD_E_MEDIUM_ENUM );
	    }
	    va_end( args );
	}

	status = ad_get_next_file( dbproc, ad_file );


	/*
	 *  If no file was found in the queried medium.
	 */

	if ( status == AD_NO_FILE )
	{
	    if ( medium == AD_MED_BEST && 
		    ++current_medium_index < 
		    eptrlen( local_ad.la_media_list ) )
	    {
		/*
		 *  If we are looking for the best medium, and there are other
		 *  media to check, set the next best media as the one to check.
		 */
		
		current_medium_cfg = (AD_MEDIUM_CONFIG *) eptritem( 
			local_ad.la_media_list, current_medium_index );
		current_medium = current_medium_cfg->amc_medium;
	    }
	    else
	    {
		return( AD_NO_FILE );
	    }
	}
	else
	{
	    /*
	     *  If the status check passes, a file was found, return it.
	     */

	    AD_CHECK( status );

	    return( AD_SUCCESS );
	}
    }
}
#endif

/*+
************************************************************************
*
*   Function:	ad_file_info
*
*   Purpose:
*	Return information about the suspected compression and format
*	of the file_id. 
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		ad_file_info
(
    char	*file_id,	/* (in)  The file_id to examine.	*/
    char	*root,		/* (out) The root of the file_id.	*/
    char	*compression,	/* (out) The file_id's compression type.*/
    char	*format		/* (out) The file_id's format type.	*/
)
{
    AD_COMP_CONFIG
    		*ad_comp_cfg;	/* Compression config information struct*/
    AD_FMT_CONFIG
    		*ad_fmt_cfg;	/* Format config information struct.	*/
    char	copy[PATH_MAX+1];	
    				/* Buffer to take copy of file_id.	*/
    char 	ext[PATH_MAX+1];	
    				/* Buffer to take extension of file_id.	*/
    char	*ext_ptr;	/* Pointer to buffer of extension.	*/
    int		index;		/* Index into eptr list.		*/


    /*
     *	Set up the default compression and format. Take copy of file_id. 
     */

    if ( compression != NULL )
    {
	*compression = AD_COMP_UNKNOWN;
    }

    if ( format != NULL )
    {
	*format = AD_FMT_UNKNOWN;
    }

    (void) strncpy( copy, file_id, PATH_MAX );
    
    /*
     *  Strip first extension.
     */
    
    ext_ptr = strext( copy );
    if ( ext_ptr == NULL )
    {
	/*
	 *  No extensions to strip. Set the root and return.
	 */

	if ( root != NULL )
	{
	    (void) strcpy( root, copy );
	}
	return( AD_SUCCESS );
    }
    (void) strncpy( ext, ext_ptr, PATH_MAX );

    
    /*
     *  Try to match the extension to a compression type.
     */

    if ( eptr_search( local_ad.la_comp_list, ext, &index, 
	    ad_comp_compare_srch ) )
    {
	ad_comp_cfg = eptritem( local_ad.la_comp_list, index );
	if ( compression != NULL )
	{
	    *compression = ad_comp_cfg->acc_type;
	}

	/*
	 *  See if there's a second extension to strip.
	 */

	strroot( copy );
	ext_ptr = strext( copy );
	if ( ext_ptr == NULL )
	{
	    /*
	     *  No more extensions to strip. Set the root and return.
	     */

	    if ( root != NULL )
	    {
		(void) strcpy( root, copy );
	    }
	    return( AD_SUCCESS );
	}
	(void) strncpy( ext, ext_ptr, PATH_MAX );
    }

    /*
     *  If we get to this point, we must have either not matched the
     *  first extension, or there was a second extension to check. 
     */
     
    if ( eptr_search( local_ad.la_fmt_list, ext, &index, 
	    ad_fmt_compare_srch ) )
    {
	ad_fmt_cfg = eptritem( local_ad.la_fmt_list, index );
	if ( format != NULL )
	{
	    *format = ad_fmt_cfg->afc_type;
	}
	strroot( copy );
    }


    if ( root != NULL )
    {
	(void) strcpy( root, copy );
    }
    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_file_init
*
*   Purpose:
*	Initializes a file structure for a specified archive directory.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		ad_file_init
(
    AD_FILE	*ad_file,	/* (in)  The file to initialize.	*/
    AD_MEDIUM	medium		/* (in)  The medium the file is on.	*/
)
{
    ad_file->adf_crc 			= 0;
    ad_file->adf_archive[0] 		= '\0';
    ad_file->adf_compression 		= AD_COMP_UNKNOWN;
    ad_file->adf_file_id[0] 		= '\0';
    ad_file->adf_filesize 		= AD_FILESIZE_UNKNOWN;
    ad_file->adf_filesize_uncomp	= AD_FILESIZE_UNKNOWN;
    ad_file->adf_format 		= AD_FMT_UNKNOWN;
    ad_file->adf_mounted 		= FALSE;
    ad_file->adf_status 		= AD_FILE_CURRENT;
    ad_file->adf_volume_name[0]		= '\0';
    ad_file->adf_volume_medium 		= medium;
    ad_file->adf_volume_type[0] 	= '\0';
    ad_file->adf_volume_format[0]	= '\0';


    switch( medium )
    {
	case AD_MED_CD:
	    AD_CHECK_MEDIUM_CD;
	    ad_file->adf_location.cd.cdl_filename[0] = '\0';
	    ad_file->adf_location.cd.cdl_mount_point[0] = '\0';
	    break;
	case AD_MED_DB:
	    AD_CHECK_MEDIUM_DB;
	    ad_file->adf_location.db.dbl_database_name[0] = '\0';
	    ad_file->adf_location.db.dbl_table_name[0] = '\0';
	    ad_file->adf_location.db.dbl_filename[0] = '\0';
	    break;
	case AD_MED_MD:
	    AD_CHECK_MEDIUM_MD;
	    ad_file->adf_location.md.mdl_filename[0] = '\0';
	    ad_file->adf_location.md.mdl_mount_point[0] = '\0';
	    break;
	case AD_MED_MFS:
	    AD_CHECK_MEDIUM_MFS;
	    ad_file->adf_location.mfs.fsl_filename[0] = '\0';
	    ad_file->adf_location.mfs.fsl_mount_point[0] = '\0';
	    break;
	case AD_MED_OD:
	    AD_CHECK_MEDIUM_OD;
	    ad_file->adf_location.od.odl_dirsectoff = -1;
	    ad_file->adf_location.od.odl_filename[0] = '\0';
	    break;
	case AD_MED_TP:
	    AD_CHECK_MEDIUM_TP;
	    ad_file->adf_location.tp.tpl_location[0] = '\0';
	    ad_file->adf_location.tp.tpl_filename[0] = '\0';
	    break;
	case AD_MED_REMOTE:
	    ad_file->adf_location.rs.rsl_server_id[0] = '\0';
	    break;
	default:
	    ad_format_message( AD_E_MEDIUM_ENUM, medium );
	    return( AD_E_MEDIUM_ENUM );
    }

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_file_media
*
*   Purpose:
*	Calculate the media mask flags for either a single file, or for
*	a list of files. The media mask flags indicate which media have
*	the entire set of files being examined (parameter complete), and 
*	which media have an incomplete subset of the files being examined 
*	(parameter incomplete).
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_FM_TYPE_ENUM: Invalid file media type enumerated value.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_file_media
(
    DBPROCESS	*dbproc,	/* (in)  The db process to use.		*/
    AD_FILEMEDIA 		/* (in)  Is this a single file or a list*/
    		type,		/*	 of files?			*/
    char	*archive,	/* (in)  The archive being examined.	*/
    AD_MEDIA_MASK		/* (out) The mask of media containing	*/
    		*complete,	/*	 the complete set of files.	*/
    AD_MEDIA_MASK		/* (out) The mask of media containing	*/
    		*incomplete,	/*	 an incomplete set of files.	*/
    boolean	*all_available,	/* (out) Are all of the files available	*/
    				/*	 on at least one media?		*/
    ...				/* (in)  The varargs will contain either*/
    				/*	 a char * to a file id, or an	*/
				/*	 eptr list of files ids,	*/
				/*	 depending on the value of the	*/
				/*	 type parameter.		*/
)
{
    int		i;
    va_list	args;		/* The variable argument list.		*/
    char	*file_id;	/* A single file id.			*/
    EPTR_P	id_list;	/* The list of file ids.		*/
    AD_MEDIA_MASK		/* Temporary complete mask.		*/
    		present;
    AD_MEDIA_MASK		/* Mask of media with at least one file	*/
    		at_least_one;


    AD_CHECK_INIT;


    if ( type == AD_FM_FILE )
    {
	/*
	 * Only one file is being check, get the file id.
	 */

	va_start( args, all_available );
	file_id = (char *) va_arg( args, char * );
	va_end( args );


	/*
	 * Get the mask for the single file.
	 */

	AD_CHECK( ad_onefile_media( dbproc, archive, file_id, complete ) );


	/*
	 * If it is complete on any media, then all files are available.
	 */

	*all_available = ( *complete != 0 );

	
	/*
	 * A set of one file can never be incomplete.
	 */

	*incomplete = 0;
	return( AD_SUCCESS );
    }
    else if ( type == AD_FM_EPTR_LIST )
    {
	/*
	 * A list of files is being checked, get the eptr list.
	 */

	va_start( args, all_available );
	id_list = (EPTR_P) va_arg( args, EPTR_P );
	va_end( args );


	/*
	 * Check each file in the eptr list. The complete flag is initialized
	 * to 0xFFFF, and bits are cleared as files fail to appear on each
	 * media. The "at_least_one" flag is initialized to zero, and bits
	 * are set as files apear on each media.
	 */

	for ( i = 0, *all_available = TRUE, *complete = 0xFFFF, 
		at_least_one = 0x0000 ; i < eptrlen( id_list ); i++ )
	{
	    /*
	     * Get the file id.
	     */

	    file_id = eptritem( id_list, i );


	    /*
	     * Get the mask for the single file.
	     */

	    AD_CHECK( ad_onefile_media( dbproc, archive, file_id, &present ) );

	    
	    /*
	     * And the present mask with the complete mask to clear any bits
	     * for media on which this file doesn't appear.
	     */

	    *complete &= present;

	    
	    /*
	     * Or the present mask with the at_least_one mask to set any
	     * bits for media on which this file does appear.
	     */

	    at_least_one |= present;

	    
	    /*
	     *  If this file isn't present on at least one media, then all
	     *  of the files are not available.
	     */

	    *all_available = ( *all_available && ( present != 0 ) );
	}


	/*
	 * And the complete mask with the at_least_one mask. This is only
	 * necessary when there are 0 files in the list, and stops complete
	 * from being returned as 0xffff.
	 */

	*complete &= at_least_one;


	/*
	 * Figure out the incomplete mask. This is any media where there is
	 * at least one file available, but the media does not contain the
	 * complete set.
	 */

	*incomplete = at_least_one ^ *complete;
    }
    else
    {
	ad_format_message( AD_E_FM_TYPE_ENUM, type );
	return( AD_E_FM_TYPE_ENUM );
    }

    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_file_name
*
*   Purpose:
*	Returns a pointer to the full name on the medium.
*
*   Values Returned:
*	char	*fname	: Name of file on medium.
*	char	*NULL	: No name.
*
************************************************************************
-*/

char		*ad_file_name
(
    AD_FILE	*ad_file,	/* (in)  The file to process.		*/
    char	*fname		/* (out) The string for the name.	*/
)
{
    char	*name;
    char	*path;

    name = NULL;
    path = NULL;

    switch( ad_file->adf_volume_medium )
    {
	case AD_MED_CD:
	    path = ad_file->adf_location.cd.cdl_mount_point;
	    name = ad_file->adf_location.cd.cdl_filename;
	    break;

	case AD_MED_DB:
	    name = ad_file->adf_location.db.dbl_filename;
	    break;

	case AD_MED_MD:
	    path = ad_file->adf_location.md.mdl_mount_point;
	    name = ad_file->adf_location.md.mdl_filename;
	    break;

	case AD_MED_MFS:
	    path = ad_file->adf_location.mfs.fsl_mount_point;
	    name = ad_file->adf_location.mfs.fsl_filename;
	    break;

	case AD_MED_OD:
	    name = ad_file->adf_location.od.odl_filename;
	    break;

	case AD_MED_TP:
	    name = ad_file->adf_location.tp.tpl_filename;
	    break;

	default:
	    break;
    }

    if ( name == NULL )
    {
	return( NULL );
    }

    if ( path == NULL )
    {
	(void) strcpy( fname, name );
    }
    else
    {
	(void) sprintf( fname, "%s/%s", path, name );
    }

    return( fname );
}

/*+
************************************************************************
*
*   Function:	ad_file_put
*
*   Purpose:
*	Inserts a file into the archive directories.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_INIT	: Library not initialized.
*	int	AD_E_MEDIUM_ENUM: Medium type is not valid.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to write.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;	/* Return status from dbresults().	*/


    AD_CHECK_INIT;


    /*
     *  Open a transaction
     */

    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "begin transaction \n" ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    /*
     *  Put to `local' table. 
     */
     
    switch ( ad_file->adf_volume_medium )
    {
	case	AD_MED_CD:
	    AD_CHECK_MEDIUM_CD;
	    AD_CHECK( ad_cd_file_put( ad_file, dbproc ) );
	    break;
	case	AD_MED_DB:
	    AD_CHECK_MEDIUM_DB;
	    AD_CHECK( ad_db_file_put( ad_file, dbproc ) );
	    break;
	case	AD_MED_MD:
	    AD_CHECK_MEDIUM_MD;
	    AD_CHECK( ad_md_file_put( ad_file, dbproc ) );
	    break;
	case	AD_MED_MFS:
	    AD_CHECK_MEDIUM_MFS;
	    AD_CHECK( ad_mfs_file_put( ad_file, dbproc ) );
	    break;
	case	AD_MED_OD:
	    AD_CHECK_MEDIUM_OD;
	    AD_CHECK( ad_od_file_put( ad_file, dbproc ) );
	    break;
	case	AD_MED_TP:
	    AD_CHECK_MEDIUM_TP;
	    AD_CHECK( ad_tp_file_put( ad_file, dbproc ) );
	    break;
	default:
	    ad_format_message( AD_E_MEDIUM_ENUM, ad_file->adf_volume_medium );
	    return( AD_E_MEDIUM_ENUM );
    }

    if ( local_ad.la_remote == TRUE )
    {
	/*
	 *  Put to `remote' table. 
	 */
	  
	AD_CHECK( ad_remote_file_put( ad_file, dbproc ) );
    }

    /*
     *  Commit transaction.
     */

    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "commit tran \n" ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_init
*
*   Purpose:
*	Initialize the ad library.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_CONFIG	: Error from the gen_config library.
*
************************************************************************
-*/

int	ad_init
(
    void
)
{
    AD_CHECK( ad_config_init() );


    ad_init_flag = TRUE;

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_med_name
*
*   Purpose:
*	Gets the name of a medium.
*
*   Values Returned:
*	char	*name		: The name of the medium.
*
************************************************************************
-*/

char		*ad_med_name	
(
    AD_MEDIUM	medium		/* (in)  The medium to name.		*/
)
{
/********** FIX, POTENTIALLY WILL FAIL, if medium is not configured.*/
    switch ( medium )
    {
	case AD_MED_CD:
	    return( AD_MEDNAME_CD );
	case AD_MED_DB:
	    return( AD_MEDNAME_DB );
	case AD_MED_MD:
	    return( AD_MEDNAME_MD );
	case AD_MED_MFS:
	    return( AD_MEDNAME_MFS );
	case AD_MED_OD:
	    return( AD_MEDNAME_OD );
	case AD_MED_TP:
	    return( AD_MEDNAME_TP );
	default:
	    return( AD_MEDNAME_UNK );
    }
}

/*+
************************************************************************
*
*   Function:	ad_med_type
*
*   Purpose:
*	Gets the enumerated type of a medium (string).
*
*   Values Returned:
*	AD_MEDIUM	medium	: The enumerated type for medium.
*
************************************************************************
-*/

AD_MEDIUM	ad_med_type
(
    char	*medium_str	/* (in)  Medium string passed in.	*/
)
{
    if ( streq( medium_str, AD_MEDNAME_CD ) )
    {
	return( AD_MED_CD );
    }
    if ( streq( medium_str, AD_MEDNAME_DB ) )
    {
	return( AD_MED_DB );
    }
    if ( streq( medium_str, AD_MEDNAME_MD ) )
    {
	return( AD_MED_MD );
    }
    if ( streq( medium_str, AD_MEDNAME_MFS ) )
    {
	return( AD_MED_MFS );
    }
    if ( streq( medium_str, AD_MEDNAME_OD ) )
    {
	return( AD_MED_OD );
    }
    if ( streq( medium_str, AD_MEDNAME_TP ) )
    {
	return( AD_MED_TP );
    }
    else
    {
	return( AD_MED_UNKNOWN );
    }
}

/*+
************************************************************************
*
*   Function:	ad_remote_get
*
*   Purpose:
*	Fetches file information from the master tables containing 
*	information about remotely-stored files. 
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in sybase db library.
*	int	AD_E_DB_LIB	: Error in cadc db library.
*	int	AD_E_INIT	: Library not initialized.
*	int	AD_E_NO_REMOTE	: No remote tables in use.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_remote_get
(
    AD_REMOTEGET	
		select_type,	/* (in)  The select type. 		*/
    AD_FILE	*ad_file,	/* (out) The file structure to return.	*/
    DBPROCESS 	*dbproc,	/* (in)  The dbprocess to use.		*/
    char	*file_id	/* (in)  The file_id to search for.	*/
)
{
    AD_CHECK_INIT;


    if ( local_ad.la_remote == FALSE )
    {
	ad_format_message( AD_E_NO_REMOTE );
	return( AD_E_NO_REMOTE );
    }

    if ( select_type != AD_RG_NEXT )
    {
	AD_CHECK_DB( dbcanquery( dbproc ) );

	switch ( select_type )
	{
	    case AD_RG_FILE_ID:	    
		AD_CHECK_DB( dbfcmd( dbproc,
			"select \n"
			"    %d, \n"
			"    %d, \n"
			"    adf.file_id, \n"
			"    adf.status, \n"
			"    adf.media_id, \n"
			"    adm.archive, \n"
			"    adf.compression, \n"
			"    adf.format, \n"
			"    ads.server_id, \n"
			"    adm.mounted \n"
			"from \n"
			"    %s..%s adf, \n"
			"    %s..%s adm, \n"
			"    %s..%s ads \n"
			"where \n"
			"    adf.media_id = adm.media_id \n"
			"    and adf.site_id = adm.site_id \n"
			"    and adm.site_id = ads.site_id \n"
			"    and ads.site_id != '%s' \n"
			"    and adm.mounted = 'Y' \n"
			"    and adf.file_id = '%s' \n"
			"    and adm.archive = '%s' \n"
			"    and adf.status = '%c' \n"
			"order by \n"
			"    ads.rank \n",
			AD_MED_REMOTE,
			AD_FILESIZE_UNKNOWN,
			local_ad.la_database_name,
			local_ad.la_files_table,
			local_ad.la_database_name,
			local_ad.la_media_table,
			local_ad.la_database_name,
			local_ad.la_servers_table,
			local_ad.la_site_id,
			file_id, 
			local_ad.la_archive,
			AD_FILE_CURRENT ) );
		break;
	    
	    default:
		ad_format_message( AD_E_SELECT_TYPE );
		return( AD_E_SELECT_TYPE );
	}

	AD_CHECK_DB( dbsqlexec( dbproc ) );
	AD_CHECK_DB( dbresults( dbproc ) );
    }

    if ( DBROWS( dbproc ) != SUCCEED )
    {
	return( AD_NO_FILE );
    }

    AD_CHECK( ad_get_next_file( dbproc, ad_file ) );


    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_server_id_get
*
*   Purpose:
*	Fetches the server_id of the local dataserver. This is the 
*	IMP identity string which shares the same record in the 
*	ad_servers table with the config file's site_id string.
*	
*	The configured site_id string is the string which identifys
*	the local site in the remote ad tables. It is used to indicate
*	to remote servers that a specified file is located 'here'.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_INIT	: Library not initialized.
*	int	AD_E_DB_LIB	: Error in cadc db library.
*	int	AD_E_DB		: Error in sybase db library.
*	int	AD_E_NO_REMOTE	: No remote tables in use.
*	int	AD_E_SITE_ID	: Error with the site ID.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int	ad_server_id_get
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    char	**server_id_ptr	/* (mod) address of str ptr to return.	*/
)
{
    static AD_SERVER_ID
		server_id_str;	/* String to receive identity from db.	*/
    int		status;		/* Return status from the dbnextrow().	*/


    AD_CHECK_INIT;


    if ( local_ad.la_remote == FALSE )
    {
	ad_format_message( AD_E_NO_REMOTE );
	return( AD_E_NO_REMOTE );
    }

    AD_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    server_id \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    site_id = '%s' \n",
	    local_ad.la_database_name,
	    local_ad.la_servers_table,
	    local_ad.la_site_id ) );

    AD_CHECK_DB( dbsqlexec( dbproc ) );
    AD_CHECK_DB( dbresults( dbproc ) );

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	ad_format_message( AD_E_SITE_ID, local_ad.la_site_id );
	return( AD_E_SITE_ID );
    }
    AD_CHECK_DB( status );

    AD_CHECK_DB_LIB( db_stringdata( dbproc, 1, 
	    server_id_str, sizeof( server_id_str ) ) );

    *server_id_ptr = &server_id_str[0];


    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_volume_get
*
*   Purpose:
*	Gets a specified volume or list of volumes from the archive.
*	The selection type can be:
*	    AD_VG_ARCHIVE	- get all volumes from an archive.
*	    AD_VG_LAST		- get the last volume 
*				  matching the specified diskname pattern. 
*           AD_VG_NAME		- get volume by name. 
*	    AD_VG_NAME_ORIG	- get volumes with the specified original 
*				  volume name.
*	    AD_VG_NEXT		- get the next volume in the current sequence.
*
*	The synopses for the various select types are:
*
*	char		*archive;
*	ad_volume_get( AD_VG_ARCHIVE, AD_VOLUME *, DBPROCESS *, AD_MEDIUM,
*		archive );
*
*	char		*archive;
*	char		*volume_name;
*	ad_volume_get( AD_VG_LAST, AD_VOLUME *, DBPROCESS *, AD_MEDIUM,
*		archive, volume_name );
*
*	char		*volume_name;
*	ad_volume_get( AD_VG_NAME, AD_VOLUME *, DBPROCESS *, AD_MEDIUM,
*		volume_name );
*
*	char		*orig_volume_name;
*	ad_volume_get( AD_VG_NAME_ORIG, AD_VOLUME *, DBPROCESS *, 
*		AD_MED_DB | AD_MED_MD | AD_MED_TP | AD_MED_OD, 
*		orig_volume_name )
*
*	char		*orig_volume_name;
*	time_t		creation_date;
*	ad_volume_get( AD_VG_NAME_ORIG, AD_VOLUME *, DBPROCESS *, 
*		AD_MED_CD | AD_MED_MFS, orig_volume_name, creation_date );
*
*	ad_volume_get( AD_VG_NEXT, AD_VOLUME *, DBPROCESS * );
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_VOLUME	: No volume matched.
*	int	AD_E_INIT	: Library not initialized.
*	int	AD_E_MEDIUM_ENUM: Invalid medium type.
*
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int	ad_volume_get
(
    AD_VOLGET	select_type,	/* (in)  The selection type.		*/
    AD_VOLUME	*ad_volume,	/* (out) The volume to get.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbproc to use.		*/
    ...				/* (in)  Optional arguments.		*/
)
{
    va_list	args;
    AD_MEDIUM	medium;


    AD_CHECK_INIT;

    if ( select_type != AD_VG_NEXT )
    {
	AD_CHECK_DB( dbcanquery( dbproc ) );

	va_start( args, dbproc );
	medium = (AD_MEDIUM) va_arg( args, AD_MEDIUM );
	switch ( medium )
	{
	    case AD_MED_CD:
		AD_CHECK_MEDIUM_CD;
		AD_CHECK( ad_cd_volume_get( select_type, dbproc, args ) );
		break;

	    case AD_MED_DB:
		AD_CHECK_MEDIUM_DB;
		AD_CHECK( ad_db_volume_get( select_type, dbproc, args ) );
		break;

	    case AD_MED_MD:
		AD_CHECK_MEDIUM_MD;
		AD_CHECK( ad_md_volume_get( select_type, dbproc, args ) );
		break;

	    case AD_MED_MFS:
		AD_CHECK_MEDIUM_MFS;
		AD_CHECK( ad_mfs_volume_get( select_type, dbproc, args ) );
		break;

	    case AD_MED_OD:
		AD_CHECK_MEDIUM_OD;
		AD_CHECK( ad_od_volume_get( select_type, dbproc, args ) );
		break;

	    case AD_MED_TP:
		AD_CHECK_MEDIUM_TP;
		AD_CHECK( ad_tp_volume_get( select_type, dbproc, args ) );
		break;

	    default:
		ad_format_message( AD_E_MEDIUM_ENUM, medium );
		return( AD_E_MEDIUM_ENUM );
	}
	va_end( args );
    }


    return( ad_get_next_volume( dbproc, ad_volume ) );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_volume_init
*
*   Purpose:
*	Initializes a volume structure for a specified archive directory.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		ad_volume_init
(
    AD_VOLUME	*ad_volume,	/* (in)  The volume to initialize.	*/
    AD_MEDIUM	medium		/* (in)  The medium the volume is on.	*/
)
{
    ad_volume->adv_archive[0] = '\0';
    ad_volume->adv_crc = AD_CRC_UNKNOWN;
    ad_volume->adv_medium_type = medium;
    ad_volume->adv_mounted = FALSE;
    ad_volume->adv_volume_name[0] = '\0';

    switch( medium )
    {
	case AD_MED_CD:
	    AD_CHECK_MEDIUM_CD;
	    ad_volume->adv_vol_info.cd.cdv_creation_date = 0;
	    ad_volume->adv_vol_info.cd.cdv_diskname_orig[0] = '\0';
	    ad_volume->adv_vol_info.cd.cdv_ingest_date = 0;
	    ad_volume->adv_vol_info.cd.cdv_mount_point[0] = '\0';
	    ad_volume->adv_vol_info.cd.cdv_nfiles = 0;
	    ad_volume->adv_vol_info.cd.cdv_nkbytes = 0;
	    strcpy( ad_volume->adv_vol_info.cd.cdv_type, AD_ISO9660 );
	    ad_volume->adv_vol_info.cd.cdv_verified = AD_NOT_VERIFIED;
	    break;
	case AD_MED_DB:
	    AD_CHECK_MEDIUM_DB;
	    ad_volume->adv_vol_info.db.dbv_database_name[0] = '\0';
	    ad_volume->adv_vol_info.db.dbv_table_name[0] = '\0';
	    break;
	case AD_MED_MD:
	    AD_CHECK_MEDIUM_MD;
	    ad_volume->adv_vol_info.md.mdv_mount_point[0] = '\0';
	    break;
	case AD_MED_MFS:
	    AD_CHECK_MEDIUM_MFS;
	    ad_volume->adv_vol_info.mfs.fsv_creation_date = 0;
	    ad_volume->adv_vol_info.mfs.fsv_ingest_date = 0;
	    ad_volume->adv_vol_info.mfs.fsv_hostname[0] = '\0';
	    ad_volume->adv_vol_info.mfs.fsv_mount_point[0] = '\0';
	    ad_volume->adv_vol_info.mfs.fsv_nfiles = 0;
	    ad_volume->adv_vol_info.mfs.fsv_nkbytes = 0;
	    ad_volume->adv_vol_info.mfs.fsv_type[0] = '\0';
	    ad_volume->adv_vol_info.mfs.fsv_format[0] = '\0';
	    ad_volume->adv_vol_info.mfs.fsv_verified = AD_NOT_VERIFIED;
	    break;
	case AD_MED_OD:
	    AD_CHECK_MEDIUM_OD;
	    ad_volume->adv_vol_info.od.odv_creation_date = -1;
	    ad_volume->adv_vol_info.od.odv_dirsectoff = -1;
	    ad_volume->adv_vol_info.od.odv_diskname_orig[0] = '\0';
	    ad_volume->adv_vol_info.od.odv_ingest_date = -1;
	    ad_volume->adv_vol_info.od.odv_nfiles = 0;
	    ad_volume->adv_vol_info.od.odv_nkbytes = 0;
	    ad_volume->adv_vol_info.od.odv_reverse_name[0] = '\0';
	    ad_volume->adv_vol_info.od.odv_type[0] = '\0';
	    ad_volume->adv_vol_info.od.odv_verified = AD_NOT_VERIFIED;
	    break;
	case AD_MED_TP:
	    AD_CHECK_MEDIUM_TP;
	    ad_volume->adv_vol_info.tp.tpv_ingest_date = -1;
	    ad_volume->adv_vol_info.tp.tpv_nfiles = 0;
	    ad_volume->adv_vol_info.tp.tpv_nkbytes = 0;
	    ad_volume->adv_vol_info.tp.tpv_tp_script[0] = '\0';
	    ad_volume->adv_vol_info.tp.tpv_type[0] = '\0';
	    ad_volume->adv_vol_info.tp.tpv_verified = AD_NOT_VERIFIED;
	    break;
	default:
	    ad_format_message( AD_E_MEDIUM_ENUM, medium );
	    return( AD_E_MEDIUM_ENUM );
    }

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_volume_put
*
*   Purpose:
*	Writes a the volume information into the appropriate volume table.
*
*   Values Returned:
*	int	AD_SUCCESS 	: Normal completion.
*	int	AD_E_INIT	: Library not initialized.
*	int	AD_E_MEDIUM_ENUM: Invalid volume medium.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_volume_put
(
    AD_VOLUME	*ad_volume,	/* (in)  The volume to write.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;	/* Return status from dbresults().	*/


    AD_CHECK_INIT;


    /*
     *  Open an transaction
     */

    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "begin transaction \n" ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    /*
     *  Put to `local' tables. 
     */

    switch ( ad_volume->adv_medium_type )
    {
	case	AD_MED_CD:
	    AD_CHECK_MEDIUM_CD;
	    AD_CHECK( ad_cd_volume_put( ad_volume, dbproc ) );
	    break;
	case	AD_MED_DB:
	    AD_CHECK_MEDIUM_DB;
	    AD_CHECK( ad_db_volume_put( ad_volume, dbproc ) );
	    break;
	case	AD_MED_MD:
	    AD_CHECK_MEDIUM_MD;
	    AD_CHECK( ad_md_volume_put( ad_volume, dbproc ) );
	    break;
	case	AD_MED_MFS:
	    AD_CHECK_MEDIUM_MFS;
	    AD_CHECK( ad_mfs_volume_put( ad_volume, dbproc ) );
	    break;
	case	AD_MED_OD:
	    AD_CHECK_MEDIUM_OD;
	    AD_CHECK( ad_od_volume_put( ad_volume, dbproc ) );
	    break;
	case	AD_MED_TP:
	    AD_CHECK_MEDIUM_TP;
	    AD_CHECK( ad_tp_volume_put( ad_volume, dbproc ) );
	    break;
	default:
	    ad_format_message( AD_E_MEDIUM_ENUM, ad_volume->adv_medium_type );
	    return( AD_E_MEDIUM_ENUM );
    }

    if ( local_ad.la_remote == TRUE )
    {
	/*
	 *  Put to `remote' tables.
	 */
	 
	AD_CHECK( ad_remote_volume_put( ad_volume, dbproc ) );
    }


    /*
     *  Commit transaction.
     */

    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "commit transaction \n" ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}
#endif

/*+
************************************************************************
*
*   Function:	ad_volume_verify.
*
*   Purpose:
*	Verify correctness of volume information.
*
*   Values Returned:
*	boolean	TRUE	: Volume is valid.
*	boolean FALSE	: Volume is not valid.
*
************************************************************************
-*/

boolean	ad_volume_verify
(
    AD_VOLUME	*ad_volume	/* (in)	 Volume to be verified.		*/
)
{
    boolean 	retValue;	/* Return value.			*/

    switch ( ad_volume->adv_medium_type )
    {
	case	AD_MED_CD:
	    retValue = AD_SUCCESS;
	    break;
	case	AD_MED_DB:
	    retValue = AD_SUCCESS;
	    break;
	case	AD_MED_MD:
	    retValue = AD_SUCCESS;
	    break;
	case	AD_MED_MFS:
	    retValue = ad_mfs_volume_verify( ad_volume );
	    break;
	case	AD_MED_OD:
	    retValue = AD_SUCCESS;
	    break;
	case	AD_MED_TP:
	    retValue = AD_SUCCESS;
	    break;

	default:
	    retValue = FALSE;
    }

    return( retValue );
}


