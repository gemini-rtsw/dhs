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
*   Module Name:	ad/src/cd.c
*
*   Purpose:
*	Contains routines to access the cd archive.
*
*   Routines:
*	int	ad_cd_file_get	: Get information about a file or files.
*	int	ad_cd_file_put	: Insert a file to the cdfile table.
*	int	ad_cd_volume_get: Get information about volumes.
*	int	ad_cd_volume_put: Insert a volume into the cds table.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: cd.c
*	Version Number	: 1.14
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/31 JSD  :	Changed all gets to return success.
*	97/02/04 JSD  :	Removed reference to diskname_orig and fixed 
*		 	varargs.
*	97/02/17 JSD  :	Added archive.
*	97/06/20 SEC  :	Removed ad_priority_config. Added compression and
*			format to files table. Added diskname_orig back. 
*       97/07/16 SEC  : Moved db and db server names to local_ad struct.
*     2000/03/23 SDJ  : Changed mc_medium_name to amc_medium_type, due
*			to changed in AD_MEDIUM_CONFIG structure.
*	2000/05/10 sjg	: Moved mounted flag to volume structure.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "db.h"
#include "gen_eptr.h"

#include "ad.h"
#include "local_ad.h"

/*+
************************************************************************
*
*   Function:	ad_cd_file_get
*
*   Purpose:
*	Gets information about files on cdrom.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*
************************************************************************
-*/

int	ad_cd_file_get
(
    AD_FILEGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The arguments for the select.	*/
)
{
    char		*archive;
    char		*diskname;
    char		*filename;


    if ( local_ad.la_config_cd.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS );
    }

    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    cdf.fsize, \n"
	    "    cdf.file_id, \n"
	    "    cdf.status, \n"
	    "    cdf.diskname, \n"
	    "    cds.archive, \n"
	    "    cds.type, \n"
	    "    cdf.compression, \n"
	    "    cdf.format, \n"
	    "    cdf.fname, \n"
	    "    cdf.uncomp_fsize, \n"
	    "    cds.mount_point, \n"
	    "    cds.mounted \n"
	    "from \n"
	    "    %s..%s cdf, \n"
	    "    %s..%s cds \n"
	    "where \n"
	    "    cdf.diskname = cds.diskname \n",
	    AD_MED_CD,
	    local_ad.la_database_name, 
	    local_ad.la_config_cd.amc_file_table_name, 
	    local_ad.la_database_name,
	    local_ad.la_config_cd.amc_vol_table_name ) );

    switch ( select_type )
    {
	case AD_FG_LOCATION:
	    diskname = (char *) va_arg( args, char * );
	    filename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and cdf.diskname = '%s' \n"
		    "    and cdf.fname = '%s' \n",
		    diskname, filename ) );
	    break;

	case AD_FG_NAME:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    filename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and archive = '%s' \n"
		    "    and cdf.file_id like '%s' \n"
		    "    and status = '%c' \n",
		    archive, filename, AD_FILE_CURRENT ) );
	    break;

	case AD_FG_VOLUME:
	    diskname = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and cdf.diskname = '%s' \n",
		    diskname ) );
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
*   Function:	ad_cd_file_put
*
*   Purpose:
*	Insert a file into the cd archive tables.  If the file already
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

int	ad_cd_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int			dbstatus;


    if ( local_ad.la_config_cd.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS );
    }

    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @file_id		varchar(255) \n"
	    "declare @fname		varchar(255) \n"
	    "declare @diskname		varchar(255) \n"

	    "select \n"
	    "    @file_id = '%s', \n"
	    "    @diskname = '%s', \n"
	    "    @fname = '%s' \n"

	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    fname = @fname \n"
	    "    and diskname = @diskname \n"

	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    file_id, \n"
	    "    diskname, \n"
	    "    fname, \n"
	    "    fsize, \n"
	    "    status, \n"
	    "    uncomp_fsize, \n"
	    "    compression, \n"
	    "    format \n"
	    ") \n"
	    "values ( \n"
	    "    @file_id, \n"
	    "    @diskname, \n"
	    "    @fname, \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    '%c' \n"
	    ") \n",
	    ad_file->adf_file_id,
	    ad_file->adf_volume_name,
	    ad_file->adf_location.cd.cdl_filename,
	    local_ad.la_database_name, 
	    local_ad.la_config_cd.amc_file_table_name, 
	    local_ad.la_database_name, 
	    local_ad.la_config_cd.amc_file_table_name, 
	    ad_file->adf_filesize,
	    ad_file->adf_status,
	    ad_file->adf_filesize_uncomp,
	    ad_file->adf_compression,
	    ad_file->adf_format ) );

    while ( ( dbstatus = dbresults( dbproc ) )  == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_cd_volume_get
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

int		ad_cd_volume_get
(
    AD_VOLGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The arguments for the query.	*/
)
{
    char		*archive;
    time_t		creation_date;
    char		*diskname;
    char		*volume;


    if ( local_ad.la_config_cd.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS );
    }

    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    archive, \n"
	    "    diskname, \n"
	    "    diskname_orig, \n"
	    "    datediff( second, 'jan 1 1970', creation_date ), \n"
	    "    datediff( second, 'jan 1 1970', ingest_date ), \n"
	    "    mounted, \n"
	    "    mount_point, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    type, \n"
	    "    verified \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n",
	    AD_MED_CD, 
	    local_ad.la_database_name, 
	    local_ad.la_config_cd.amc_vol_table_name  ) );

    switch ( select_type )
    {
	case AD_VG_LAST:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    volume = (char *) va_arg( args, char *);

	    AD_CHECK_DB( dbfcmd( dbproc,
			"    archive = '%s' \n",
			archive ) );

	    if ( volume != NULL )
	    {
		AD_CHECK_DB( dbfcmd( dbproc, 
			"    and diskname like '%s' \n",
			volume ) );
	    }


	    AD_CHECK_DB( dbcmd( dbproc, 
		    "having \n"
		    "    diskname = max( diskname ) \n" ) );
	    break;

	case AD_VG_NAME:
	    diskname = (char *) va_arg( args, char *);

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    diskname like '%s' \n",
		    diskname ) );
	    break;

	case AD_VG_NAME_ORIG:
	    diskname = (char *) va_arg( args, char *);
	    creation_date = (time_t) va_arg( args, time_t );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    diskname_orig like '%s' \n"
		    "    and creation_date = \n"
		    "    dateadd( second, %d, 'jan 1 1970' ) \n",
		    diskname, creation_date ) );
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
*   Function:	ad_cd_volume_put
*
*   Purpose:
*	Insert a volume into the cds table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		ad_cd_volume_put
(
    AD_VOLUME	*ad_volume,	/* (in)  The volume to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int			dbstatus;
 
 
    if ( local_ad.la_config_cd.amc_medium_type[0] == 0 )
    {
	return( AD_SUCCESS  );
    }
 
 
    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "begin tran \n"
	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    diskname = '%s' \n"
	    "    and ( creation_date = \n"
	    "        dateadd( second, %d, 'jan 1 1970' ) \n"
	    "        or creation_date = 'jan 1 1970' ) \n",
	    local_ad.la_database_name, 
	    local_ad.la_config_cd.amc_vol_table_name,
	    ad_volume->adv_volume_name,
	    ad_volume->adv_vol_info.cd.cdv_creation_date ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    archive, \n"
	    "    creation_date, \n"
	    "    diskname, \n"
	    "    diskname_orig, \n"
	    "    ingest_date, \n"
	    "    mounted, \n"
	    "    mount_point, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    type, \n"
	    "    verified \n"
	    ") \n"
	    "values ( \n"
	    "   '%s', \n"
	    "	dateadd( second, %d, 'jan 1 1970' ), \n"
	    "   '%s', \n"
	    "   '%s', \n"
	    "	dateadd( second, %d, 'jan 1 1970' ), \n"
	    "   '%c', \n"
	    "   '%s', \n"
	    "   %d, \n"
	    "   %d, \n"
	    "   '%s', \n"
	    "   '%c' \n"
	    ") \n"
	    "commit tran \n",
	    local_ad.la_database_name, 
	    local_ad.la_config_cd.amc_vol_table_name,
	    ad_volume->adv_archive,
	    ad_volume->adv_vol_info.cd.cdv_creation_date, 
	    ad_volume->adv_volume_name,
	    ad_volume->adv_vol_info.cd.cdv_diskname_orig, 
	    ad_volume->adv_vol_info.cd.cdv_ingest_date, 
	    ad_volume->adv_mounted ? 'Y' : 'N',
	    ad_volume->adv_vol_info.cd.cdv_mount_point,
	    ad_volume->adv_vol_info.cd.cdv_nfiles, 
	    ad_volume->adv_vol_info.cd.cdv_nkbytes,
	    ad_volume->adv_vol_info.cd.cdv_type,
	    ad_volume->adv_vol_info.cd.cdv_verified ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}
