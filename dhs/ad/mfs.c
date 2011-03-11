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
*   Module Name:	ad/src/mfs.c
*
*   Purpose:
*	Contains routines to access the moutable file system (mfs)
*	archive.
*
*   Routines:
*	int	ad_mfs_file_get	    : Get information about a file or files.
*	int	ad_mfs_file_put	    : Insert a file to the mfsfile table.
*	int	ad_mfs_volume_get   : Get information about volumes.
*	int	ad_mfs_volume_put   : Insert a volume into the mfs table.
*	int	ad_mfs_volume_verify: Verifies volume information.
*
*   Date		: August 28, 1998
*
*
*   Field SCCS data	: @(#)
*	Module Name	: mfs.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	98/08/31  jsd	: Copied from cd.c.
*     2000/03/24  SDJ	: Changed from dvd.c to mfs.c. Supports moutable
*			  file system not just DVD. Added
*			  ad_mfs_volume_verify().  mfs tables use a crc
*			  where dvd tables use a volume name.
*     2000/04/05  SDJ	: Removed mounted and volume_orig from mfs table.
*			  Added hostname to mfs table.  Added the archive
*			  to the volume CRC creation.  Allow the hostname
*			  and mount_point to be "NULL" instead of the
*			  empty string.
*    2000/04/10  sdj	: Switched mfs crc from an int to a SYBASE
*			  binary type.  Used appropriate crc functions
*			  for using the crc.
*	2000/05/04 sjg  : Fixed extraction of mfs volume date in file get.
*	2000/05/10 sjg	: Moved mounted flag to volume structure and fixed
*			  mfs_file_get by location.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 *		General UNIX include files.
 ************************************************************************
 */

#include <stdarg.h>


/*
 ************************************************************************
 *		   Sybase include files.
 ************************************************************************
 */
#if defined(SYBASE_DHS)
#include <sybfront.h>
#include <sybdb.h>
#endif

/*
 ************************************************************************
 *		 CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_str.h"
#include "gen_eptr.h"
#if defined(SYBASE_DHS)
#include "db.h"
#include "crc.h"
#endif


/*
 ************************************************************************
 *		  AD library include files.
 ************************************************************************
 */

#include "ad.h"
#include "local_ad.h"


/*
 ************************************************************************
 *		Some definitions.
 ************************************************************************
 */

#define	MAX_CD_SIZE	( 650 * 1024 )
#define MAX_DVD_SIZE	( 4.3 * 1024 * 1024 )

/*+
************************************************************************
*
*   Function:	ad_mfs_file_get
*
*   Purpose:
*	Gets information about files on mountable file system.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int	ad_mfs_file_get
(
    AD_FILEGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The arguments for the select.	*/
)
{
    char	*archive;	/* Archive file belongs too.		*/
    char	*filename;	/* Name of the file sought.		*/
    char	*volume;	/* Name of volume.			*/
    

    if ( local_ad.la_config_mfs.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS );
    }

    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    mfsf.fsize, \n"
	    "    mfsf.file_id, \n"
	    "    mfsf.status, \n"
	    "    mfss.volume, \n"
	    "    mfss.archive, \n"
	    "    mfss.type, \n"
	    "    mfsf.compression, \n"
	    "    mfsf.format, \n"
	    "    mfsf.fname, \n"
	    "    mfsf.uncomp_fsize, \n"
	    "    mfss.mount_point, \n"
	    "    mfss.hostname, \n"
	    "	 mfss.format, \n"
	    "	 mfss.crc \n"
	    "from \n"
	    "    %s..%s mfsf, \n"
	    "    %s..%s mfss \n"
	    "where \n"
	    "    mfsf.crc = mfss.crc \n",
	    AD_MED_MFS,
	    local_ad.la_database_name, 
	    local_ad.la_config_mfs.amc_file_table_name, 
	    local_ad.la_database_name,
	    local_ad.la_config_mfs.amc_vol_table_name ) );

    switch ( select_type )
    {
	case AD_FG_LOCATION:
	    volume = (char *) va_arg( args, char * );
	    filename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and mfss.volume = '%s' \n"
		    "    and mfsf.fname = '%s' \n",
		    volume, filename ) );
	    break;

	case AD_FG_NAME:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    filename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and archive = '%s' \n"
		    "    and mfsf.file_id like '%s' \n"
		    "    and status = '%c' \n",
		    archive, filename, AD_FILE_CURRENT ) );
	    break;

	case AD_FG_VOLUME:
	    volume = (char *) va_arg( args, char * );
	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and mfss.volume = '%s' \n",
		    volume ) );
	    break;


	default:
	    ad_format_message( AD_E_SELECT_TYPE );
	    return( AD_E_SELECT_TYPE );
    }

    AD_CHECK_DB( dbsqlexec( dbproc ) );
    AD_CHECK_DB( dbresults( dbproc ) );

    if ( DBROWS( dbproc ) == SUCCEED )
    {
	return( AD_SUCCESS );
    }

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_mfs_file_put
*
*   Purpose:
*	Insert a file into the dmfs archive tables.  If the file already
*	exists, and if it is not the same as the new file, update the
*	existing entry.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int	ad_mfs_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;	/* Database fnct. return status.	*/
    char	crc_srch_str[11];
    			       /* CRC search string.			*/

    if ( local_ad.la_config_mfs.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS );
    }

    AD_CHECK_NULL( crc322str( ad_file->adf_crc, crc_srch_str ) )

    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @file_id		varchar(255) \n"
	    "declare @fname		varchar(255) \n"
	    "declare @crc		binary(4) \n"
	    "select \n"
	    "    @file_id = '%s', \n"
	    "    @crc = %s, \n"
	    "    @fname = '%s' \n"
	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    fname = @fname \n"
	    "    and crc = @crc \n"
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    crc, \n"
	    "    file_id, \n"
	    "    fname, \n"
	    "    fsize, \n"
	    "    format, \n"
	    "    uncomp_fsize, \n"
	    "    compression, \n"
	    "    status \n"
	    ") \n"
	    "values ( \n"
	    "    @crc, \n"
	    "    @file_id, \n"
	    "    @fname, \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    '%c' \n"
	    ") \n",
	    ad_file->adf_file_id,
	    crc_srch_str,
	    ad_file->adf_location.mfs.fsl_filename,
	    local_ad.la_database_name, 
	    local_ad.la_config_mfs.amc_file_table_name, 
	    local_ad.la_database_name, 
	    local_ad.la_config_mfs.amc_file_table_name,
	    ad_file->adf_filesize,
	    ad_file->adf_format,
	    ad_file->adf_filesize_uncomp,
	    ad_file->adf_compression,
	    ad_file->adf_status ) );



    while ( ( dbstatus = dbresults( dbproc ) )  == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_mfs_volume_get
*
*   Purpose:
*	Get information about volumes.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*
************************************************************************
-*/

int		ad_mfs_volume_get
(
    AD_VOLGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The arguments for the query.	*/
)
{
    char	*archive;	/* Archive volume is added to.		*/
    crc32_t	crc;		/* CRC for the volume			*/
    char	crc_ins_str[11];
    				/* CRC insert string.			*/
    char	crc_string[AD_CRC_STRING_LEN];
    				/* String the CRC is created from.	*/
    time_t	creation_date;	/* Date volume was created.		*/
    char	*volume_name;	/* Name of the volume.			*/
    char	*volume_type;	/* Type of volume (CD, DVD, or MD)	*/


    if ( local_ad.la_config_mfs.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS );
    }

    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    archive, \n"
	    "    volume, \n"
	    "    datediff( second, 'jan 1 1970', creation_date ), \n"
	    "    datediff( second, 'jan 1 1970', ingest_date ), \n"
	    "    hostname, \n"
	    "    mount_point, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    verified, \n"
	    "    type, \n"
	    "	 format, \n"
	    "	 crc \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n",
	    AD_MED_MFS, 
	    local_ad.la_database_name, 
	    local_ad.la_config_mfs.amc_vol_table_name  ) );

    switch ( select_type )
    {
	case AD_VG_CRC:
	    archive = (char *) va_arg( args, char* );
	    volume_name = (char *) va_arg( args, char *);
	    volume_type = (char *) va_arg( args, char *);
	    creation_date = (time_t) va_arg( args, time_t );
		    
	    (void) sprintf( crc_string, "%s%s%s%10.10d", archive, volume_name,
		    volume_type, creation_date );
	    crc = crc_buffer( strlen( crc_string ), crc_string );
	    crc322str( crc, crc_ins_str );
	    
	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    crc =  %s \n", crc_ins_str ) );
	    break;

	case AD_VG_LAST:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    volume_name = (char *) va_arg( args, char *);
	    
	    AD_CHECK_DB( dbfcmd( dbproc,
			"    archive = '%s' \n",
			archive ) );

	    if ( volume_name != NULL )
	    {
		AD_CHECK_DB( dbfcmd( dbproc, 
			"    and volume like '%s' \n",
			volume_name ) );
	    }

	    AD_CHECK_DB( dbcmd( dbproc, 
		    "having \n"
		    "    volume = max( volume ) \n" ) );
	    break;

	case AD_VG_NAME:
	    volume_name = (char *) va_arg( args, char *);

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    volume like '%s' \n",
		    volume_name ) );
	    break;
	    
	case AD_VG_ARCHIVE:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }

	    AD_CHECK_DB( dbfcmd( dbproc,
			"    archive = '%s' \n",
			archive ) );
	    break;

	default:
	    ad_format_message( AD_E_SELECT_TYPE );
	    return( AD_E_SELECT_TYPE );
    }

    AD_CHECK_DB( dbsqlexec( dbproc ) );
    AD_CHECK_DB( dbresults( dbproc ) );

    if ( DBROWS( dbproc ) == SUCCEED )
    {
	return( AD_SUCCESS );
    }
    
    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_mfs_volume_put
*
*   Purpose:
*	Insert a volume into the mfss table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		ad_mfs_volume_put
(
    AD_VOLUME	*ad_volume,	/* (in)  The volume to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    char	crc_ins_str[11];
    				/* CRC insert string.			*/
    char	crc_string[AD_CRC_STRING_LEN];
    				/* String the CRC is created from.	*/
    int		dbstatus;	/* DB fnct() return status.		*/

 
    if ( local_ad.la_config_mfs.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS  );
    }


    if ( ad_volume->adv_crc == AD_CRC_UNKNOWN )
    {
	(void) sprintf( crc_string, "%s%s%s%10.10d",
		ad_volume->adv_archive,
		ad_volume->adv_volume_name,
		ad_volume->adv_vol_info.mfs.fsv_type,
		ad_volume->adv_vol_info.mfs.fsv_creation_date );
	ad_volume->adv_crc = crc_buffer( strlen( crc_string ), crc_string );
    }
    crc322str( ad_volume->adv_crc, crc_ins_str );
    
    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "begin tran \n"
	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "	 archive = '%s' \n"
	    "    and volume = '%s' \n"
	    "    and type = '%s' \n"
	    "    and ( creation_date = \n"
	    "        dateadd( second, %d, 'jan 1 1970' ) \n"
	    "        or creation_date = 'jan 1 1970' ) \n",
	    local_ad.la_database_name, 
	    local_ad.la_config_mfs.amc_vol_table_name,
	    ad_volume->adv_archive,
	    ad_volume->adv_volume_name,
	    ad_volume->adv_vol_info.mfs.fsv_type,
	    ad_volume->adv_vol_info.mfs.fsv_creation_date ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "	 crc, \n"
	    "    archive, \n"
	    "    volume, \n"
	    "    creation_date, \n"
	    "    ingest_date, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    type, \n"
	    "	 format, \n"
	    "    verified, \n"
	    "    hostname, \n"
	    "    mount_point \n"
	    ") \n"
	    "values ( \n"
	    "	%s, \n"
	    "   '%s', \n"
	    "   '%s', \n"
	    "	dateadd( second, %d, 'jan 1 1970' ), \n"
	    "	dateadd( second, %d, 'jan 1 1970' ), \n"
	    "   %d, \n"
	    "   %d, \n"
	    "   '%s', \n"
	    "   '%s', \n"
	    "   '%c', \n",
	    local_ad.la_database_name, 
	    local_ad.la_config_mfs.amc_vol_table_name,
	    crc_ins_str,
	    ad_volume->adv_archive,
	    ad_volume->adv_volume_name,
	    ad_volume->adv_vol_info.mfs.fsv_creation_date, 
	    ad_volume->adv_vol_info.mfs.fsv_ingest_date,
	    ad_volume->adv_vol_info.mfs.fsv_nfiles, 
	    ad_volume->adv_vol_info.mfs.fsv_nkbytes,
	    ad_volume->adv_vol_info.mfs.fsv_type,
	    ad_volume->adv_vol_info.mfs.fsv_format,
	    ad_volume->adv_vol_info.mfs.fsv_verified ) );
    

    /*
     * If the volume is not mounted insert "NULL" value in hostname
     * and mount point.
     */

    if ( ad_volume->adv_vol_info.mfs.fsv_hostname[0] == '\0' ||
	    ad_volume->adv_vol_info.mfs.fsv_mount_point == '\0' )
    {
	AD_CHECK_DB( dbfcmd( dbproc, 	
	    "   NULL, \n"
	    "   NULL \n"
    	    ") \n"
	    "commit tran \n" ) );
    }
    else
    {
	AD_CHECK_DB( dbfcmd( dbproc, 	
	    "   '%s', \n"
	    "   '%s' \n"
    	    ") \n"
	    "commit tran \n",
    	    ad_volume->adv_vol_info.mfs.fsv_hostname,
	    ad_volume->adv_vol_info.mfs.fsv_mount_point ) );
    }
    

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
*   Function:	ad_mfs_volume_verify
*
*   Purpose:
*	Checks the validity of a volume information.  In particular
*	if the volume format, volume type pair are valid.
*
*   Values Returned:
*	boolean	TRUE	: Volume is valid.
*	boolean FALSE	: Volume is not valid.
*
************************************************************************
-*/

boolean	ad_mfs_volume_verify
(
    AD_VOLUME	*ad_volume	/* (in)  The volume to insert.		*/
)
{
    int 	index;
    
    if ( ! eptr_search( local_ad.la_voltype_list,
	    &(ad_volume->adv_vol_info.mfs), &index, ad_mfs_compare_srch ) )
    {
	ad_format_message( AD_E_MFS_VOL_INFO,
		ad_volume->adv_vol_info.mfs.fsv_type,
		ad_volume->adv_vol_info.mfs.fsv_format );
	return( FALSE );
    }
    else
    {
	if ( ( streq( ad_volume->adv_vol_info.mfs.fsv_type, AD_MFS_CD ) &&
		ad_volume->adv_vol_info.mfs.fsv_nkbytes > MAX_CD_SIZE ) ||
	     ( streq( ad_volume->adv_vol_info.mfs.fsv_type, AD_MFS_DVD ) &&
		ad_volume->adv_vol_info.mfs.fsv_nkbytes > MAX_DVD_SIZE ) )
	{
	    return( FALSE );
	}
	
	return( TRUE );
    }
}
