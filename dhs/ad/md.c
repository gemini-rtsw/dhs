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
*   Module Name:	ad/src/md.c
*
*   Purpose:
*	Contains routines to access the md archive.
*
*   Routines:
*	int	ad_md_file_del	: Delete a file for the archive.
*	int	ad_md_file_get	: Get information about a file or files.
*	int	ad_md_file_put	: Insert a file to the md archive tables..
*	int	ad_md_volume_get: Get volume info from the mds table.
*	int	ad_md_volume_put: Insert a volume into the mds table.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: md.c
*	Version Number	: 1.10
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/04/15 GJF  : Fixed ad_md_file_put so it wouldn't truncate names.
*	97/06/20 SEC  : Mod'ed common_fname to file_id. Remove references 
*			to server_name, host_name, Added message for
*			AD_E_SELECT_TYPE. 
*       97/07/16 SEC  : Moved db and db server names to local_ad struct.
*	2000/05/10 sjg	: Moved mounted flag to volume structure.
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
#if defined(SYBASE_DHS)
#include "db.h"
#endif
#include "ad.h"
#include "local_ad.h"

/*+
************************************************************************
*
*   Function:	ad_md_file_del
*
*   Purpose:
*	Delete a file from the md archive.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int		ad_md_file_del
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    char	*volname,	/* (in)  The volume the file is on.	*/
    char	*filename	/* (in)  The name of the file on disk.	*/
)
{
    
    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "delete \n"
	    "from %s..%s \n"
	    "where \n"
	    "    diskname = '%s' \n"
	    "    and fname = '%s' \n",
	    local_ad.la_database_name, 
	    local_ad.la_config_md.amc_file_table_name, 
	    volname, filename ) );

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_md_file_get
*
*   Purpose:
*	Gets information about files on magnetic disk.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int	ad_md_file_get
(
    AD_FILEGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbproc to use.		*/
    va_list	args		/* (in)  The args for the select.	*/
)
{
    char	*archive;
    char	*diskname;
    char	*filename;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    mdf.fsize, \n"
	    "    mdf.file_id, \n"
	    "    mdf.status, \n"
	    "    mdf.diskname, \n"
	    "    mds.archive, \n"
	    "    ' ', \n"
	    "    mdf.compression, \n"
	    "    mdf.format, \n"
	    "    mdf.fname, \n"
	    "    mdf.uncomp_fsize, \n"
	    "    mds.mount_point, \n"
	    "    mds.mounted \n"
	    "from \n"
	    "    %s..%s mdf, \n"
	    "    %s..%s mds \n"
	    "where \n"
	    "    mdf.diskname = mds.diskname \n",
	    AD_MED_MD, 
	    local_ad.la_database_name, 
	    local_ad.la_config_md.amc_file_table_name, 
	    local_ad.la_database_name,
	    local_ad.la_config_md.amc_vol_table_name ) );

    switch ( select_type )
    {
	case AD_FG_LOCATION:
	    diskname = (char *) va_arg( args, char * );
	    filename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and mdf.diskname = '%s' \n"
		    "    and mdf.fname = '%s' \n",
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
		    "    and mdf.file_id like '%s' \n"
		    "    and mdf.status = '%c' \n",
		    archive, filename, AD_FILE_CURRENT ) );
	    break;

	case AD_FG_VOLUME:
	    diskname = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and mdf.diskname = '%s' \n",
		    diskname ) );
	    break;
 
        default:
            ad_format_message( AD_E_SELECT_TYPE );
            return( AD_E_SELECT_TYPE );
    }

    AD_CHECK_DB( dbsqlexec( dbproc ) );
    AD_CHECK_DB( dbresults( dbproc ) );


    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_md_file_put
*
*   Purpose:
*	Insert a file into the md archive tables.  If the file already
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

int	ad_md_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @file_id		varchar(255) \n"
	    "declare @fname		varchar(255)\n"
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
	    ad_file->adf_location.md.mdl_filename,
	    local_ad.la_database_name, 
	    local_ad.la_config_md.amc_file_table_name, 
	    local_ad.la_database_name, 
	    local_ad.la_config_md.amc_file_table_name, 
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
*   Function:	ad_md_volume_get
*
*   Purpose:
*	Get information about volumes.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_VOLUME	: No volume available.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		ad_md_volume_get
(
    AD_VOLGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The arguments to the query.	*/
)
{
    char	*archive;
    char	*volume;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    archive, \n"
	    "    diskname, \n"
	    "    mounted, \n"
	    "    mount_point \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n",
	    AD_MED_MD,
	    local_ad.la_database_name,
	    local_ad.la_config_md.amc_vol_table_name ) );

    
    switch ( select_type )
    {
	case AD_VG_LAST:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    volume = (char *) va_arg( args, char * );

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
	case AD_VG_NAME_ORIG:
	    volume = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    diskname like '%s' \n",
		    volume ) );
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


    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_md_volume_put
*
*   Purpose:
*	Insert a volume into the mds table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the SYBASE db library.
*	int	AD_E_DB_LIB	: Error in the CADC db library.
*
************************************************************************
-*/

int		ad_md_volume_put
(
    AD_VOLUME	*ad_volume,	/* (in)  The volume to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "begin tran \n"

	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    diskname = '%s' \n",
	    local_ad.la_database_name,
	    local_ad.la_config_md.amc_vol_table_name,
	    ad_volume->adv_volume_name ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    archive, \n"
	    "    diskname, \n"
	    "    mounted, \n"
	    "    mount_point \n"
	    ") \n"
	    "values ( \n"
	    "	'%s', \n"
	    "   '%s', \n"
	    "   '%c', \n"
	    "   '%s' \n"
	    ") \n"

	    "commit tran \n",
	    local_ad.la_database_name,
	    local_ad.la_config_md.amc_vol_table_name,
	    ad_volume->adv_archive,
	    ad_volume->adv_volume_name,
	    ad_volume->adv_mounted ? 'Y' : 'N',
	    ad_volume->adv_vol_info.md.mdv_mount_point ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );

    return( AD_SUCCESS );
}
#endif
