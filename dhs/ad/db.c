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
*   Module Name:	ad/src/ad_db.c
*
*   Purpose:
*	Contains routines to access the db archive.
*
*   Routines:
*	int	ad_db_file_get	: Get information about a file or files.
*	int	ad_db_file_put	: Insert a file to the db archive tables..
*	int	ad_db_volume_get: Get volume information from the dbs table.
*	int	ad_db_volume_put: Insert a volume into the dbs table.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: db.c
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 09/07/99
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/17 JSD :	Added archive.
*	97/06/20 SEC  :	Mod'ed common_fname to file_id. Added compression
*			and format. Removed sever name and host name. 
*			Added AD_E_SELECT_TYPE message. 
*       97/07/16 SEC  : Moved db and db server names to local_ad struct.
*       98/06/23 sjg : Changed structure of dbfiles table changing dataset
*		       filename.
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
#if defined(SYBASE_DHS)
#include "db.h"
#endif
#include "gen_eptr.h"

#include "ad.h"
#include "local_ad.h"

/*+
************************************************************************
*
*   Function:	ad_db_file_get
*
*   Purpose:
*	Gets information about files on database.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*
************************************************************************
-*/
#if defined(SYBASE_DHS)
int	ad_db_file_get
(
    AD_FILEGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  Args for the selection.	*/
)
{
    char	*archive;
    char	*db_name;
    char	*file_id;
    char	*filename;
    char	*table_name;
    char	*volume;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    dbf.fsize, \n"
	    "    dbf.file_id, \n"
	    "    '%c', \n"
	    "    dbf.volume, \n"
	    "    dbs.archive, \n"
	    "    ' ', \n"
	    "    dbf.compression, \n"
	    "    dbf.format, \n"
	    "    dbf.fname, \n"
	    "    dbs.database_name, \n"
	    "    dbs.table_name \n"
	    "from \n"
	    "    %s..%s dbf, \n"
	    "    %s..%s dbs \n"
	    "where \n"
	    "    dbf.volume = dbs.volume \n",
	    AD_MED_DB, AD_FILE_CURRENT, 
	    local_ad.la_database_name, 
	    local_ad.la_config_db.amc_file_table_name, 
	    local_ad.la_database_name,
	    local_ad.la_config_db.amc_vol_table_name ) );

    switch ( select_type )
    {
	case AD_FG_LOCATION:
	    db_name = (char *) va_arg( args, char * );
	    table_name = (char *) va_arg( args, char * );
	    file_id = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and dbs.database_name = '%s' \n"
		    "    and dbs.table_name = '%s' \n"
		    "    and dbf.file_id like '%s' \n",
		    db_name, table_name, file_id ) );
	    break;

	case AD_FG_NAME:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    filename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and archive = '%s' \n"
		    "    and dbf.file_id like '%s' \n",
		    archive, filename ) );
	    break;

	case AD_FG_VOLUME:
	    volume = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and dbf.volume = '%s' \n",
		    volume ) );
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
*   Function:	ad_db_file_put
*
*   Purpose:
*	Insert a file into the db archive tables.  If the file already
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

int	ad_db_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @archive		char(6) \n"
	    "declare @file_id		varchar(255) \n"
	    "declare @filename		varchar(255) \n"
	    "declare @volume		varchar(255) \n"
	    "select \n"
	    "    @file_id = '%s', \n"
	    "    @volume = '%s', \n"
	    "    @filename = '%s' \n"
	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    dataset_name = @dataset_name \n"
	    "    and volume = @volume \n"
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    file_id, \n"
	    "    volume, \n"
	    "    fname, \n"
	    "    fsize, \n"
	    "    uncomp_fsize, \n"
	    "    compression, \n"
	    "    format \n"
	    ") \n"
	    "values ( \n"
	    "    @file_id, \n"
	    "    @volume, \n"
	    "    @filename, \n"
	    "    %d, \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    '%c' \n"
	    ") \n",
	    ad_file->adf_file_id,
	    ad_file->adf_volume_name,
	    ad_file->adf_location.db.dbl_filename,
	    local_ad.la_database_name, 
	    local_ad.la_config_db.amc_file_table_name, 
	    local_ad.la_database_name, 
	    local_ad.la_config_db.amc_file_table_name, 
	    ad_file->adf_filesize,
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
*   Function:	ad_db_volume_get
*
*   Purpose:
*	Get information about volumes.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_VOLUME	: No volume available.
*	ing	AD_E_DB		: Error in the sybase db library.
*
************************************************************************
-*/

int		ad_db_volume_get
(
    AD_VOLGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The select arguments.		*/
)
{
    char	*archive;
    char	*volume;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    archive, \n"
	    "    volume, \n"
	    "    database_name, \n"
	    "    table_name \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n",
	    AD_MED_DB,
	    local_ad.la_database_name,
	    local_ad.la_config_db.amc_vol_table_name ) );

    
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
			"    and volume like '%s' \n",
			volume ) );
	    }

	    AD_CHECK_DB( dbcmd( dbproc, 
		    "having \n"
		    "    volume = max( volume ) \n" ) );
	    break;

	case AD_VG_NAME:
	case AD_VG_NAME_ORIG:
	    volume = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    volume like '%s' \n",
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
*   Function:	ad_db_volume_put
*
*   Purpose:
*	Insert a volume into the dbs table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	ing	AD_E_DB		: Error in the sybase db library.
*	ing	AD_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		ad_db_volume_put
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
	    "    volume = '%s' \n",
	    local_ad.la_database_name,
	    local_ad.la_config_db.amc_vol_table_name,
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
	    "    database_name, \n"
	    "    table_name, \n"
	    "    volume \n"
	    ") \n"
	    "values ( \n"
	    "    '%s', \n"
	    "    '%s', \n"
	    "    '%s', \n"
	    "    '%s' \n"
	    ") \n"

	    "commit tran \n",
	    local_ad.la_database_name,
	    local_ad.la_config_db.amc_vol_table_name,
	    ad_volume->adv_archive,
	    ad_volume->adv_vol_info.db.dbv_database_name,
	    ad_volume->adv_vol_info.db.dbv_table_name,
	    ad_volume->adv_volume_name ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );

    return( AD_SUCCESS );
}
#endif

