/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Module Name:	ad/src/tp.c	
*
*   Purpose:
*	Contains routines to access the tp archive.
*
*   Routines:
*	int	ad_tp_file_get	: Get information about a file or files.
*	int	ad_tp_file_put	: Insert a file to the od archive tables..
*	int	ad_tp_volume_get: Get information about volumes.
*	int	ad_tp_volume_put: Insert a volume into the tps table.
*
*   Date		: Mar 4, 1998 
*
*   SCCS data		: @(#)
*	Module Name	: tp.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 09/07/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/02/24 SDJ	: Added "AD_VG_NAME_ORIG" changed name to tp.c
*	99/04/16 NRH	: Added uncompressed files size to the values 
*			  extracted from the database in ad_tp_file_get.
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
*   Function:	ad_tp_file_get
*
*   Purpose:
*	Gets information about tape files.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_FILE	: No file was available.
*
************************************************************************
-*/

#if defined(SYBASE_DHS)
int	ad_tp_file_get
(
    AD_FILEGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbproc to use.		*/
    va_list	args		/* (in)  The arguments to use.		*/
)
{
    char	*archive;
    char	*tapename;
    char	*filename;
    char	*location;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    tpf.fsize, \n"
	    "    tpf.file_id, \n"
	    "    tpf.status, \n"
	    "    tpf.tapename, \n"
	    "    tps.archive, \n"
	    "    tps.type, \n"
	    "    tpf.compression, \n"
	    "    tpf.format, \n"
	    "    tpf.location, \n"
	    "    tpf.fname, \n"
	    "    tpf.uncomp_fsize \n"
	    "from \n"
	    "    %s..%s tpf, \n"
	    "    %s..%s tps \n"
	    "where \n"
	    "    tpf.tapename = tps.tapename \n",
	    AD_MED_TP,
	    local_ad.la_database_name, 
	    local_ad.la_config_tp.amc_file_table_name,
	    local_ad.la_database_name,
	    local_ad.la_config_tp.amc_vol_table_name ) );

    switch ( select_type )
    {
	case AD_FG_LOCATION:
	    tapename = (char *) va_arg( args, char * );
	    filename = (char *) va_arg( args, char * );
	    location = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and tpf.tapename = '%s' \n"
		    "	 and tpf.fname = '%s' \n"
		    "    and tpf.location = '%s' \n",
		    tapename, filename, location ) );
	    break;

	case AD_FG_NAME:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    filename = (char *) va_arg( args, char * );;

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and tps.archive = '%s' \n"
		    "    and tpf.file_id like '%s' \n"
		    "    and tpf.status = '%c' \n",
		    archive, filename, AD_FILE_CURRENT ) );
	    break;

	case AD_FG_VOLUME:
	    tapename = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and tpf.tapename = '%s' \n",
		    tapename ) );
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
*   Function:	ad_tp_file_put
*
*   Purpose:
*	Insert a file into the tp archive tables.  If the file already
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

int	ad_tp_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @file_id		varchar(255) \n"
	    "declare @location		varchar(32) \n"
	    "declare @tapename		varchar(255) \n"

	    "select \n"
	    "    @tapename = '%s', \n"
	    "    @file_id = '%s', \n"
	    "    @location = '%s' \n"

	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    location = @location \n"
	    "    and tapename = @tapename \n"
	    "    and file_id = @file_id \n"

	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    tapename, \n"
	    "    file_id, \n"
	    "    location, \n"
	    "    fname, \n"
	    "    fsize, \n"
	    "    uncomp_fsize, \n"
	    "    status, \n"
	    "    compression, \n"
	    "    format \n"
	    ") \n"
	    "values ( \n"
	    "    @tapename, \n"
	    "    @file_id, \n"
	    "    @location, \n"
	    "    '%s', \n"
	    "    %d, \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    '%c', \n"
	    "    '%c' \n"
	    ") \n",
	    ad_file->adf_volume_name,
	    ad_file->adf_file_id,
	    ad_file->adf_location.tp.tpl_location,
	    local_ad.la_database_name, 
	    local_ad.la_config_tp.amc_file_table_name, 
	    local_ad.la_database_name, 
	    local_ad.la_config_tp.amc_file_table_name, 
	    ad_file->adf_location.tp.tpl_filename,
	    ad_file->adf_filesize,
	    ad_file->adf_filesize_uncomp,
	    ad_file->adf_status,
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
*   Function:	ad_tp_volume_get
*
*   Purpose:
*	Get information about volumes.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_VOLUME	: No volume available.
*	int	AD_E_DB		: Error in the sybase db library.
*
************************************************************************
-*/

int		ad_tp_volume_get
(
    AD_VOLGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    va_list	args		/* (in)  The arguements to the query.	*/
)
{
    char	*archive;
    char	*volume;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    archive, \n"
	    "    tapename, \n"
	    "    datediff(second, 'jan 1 1970', ingest_date), \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    script, \n"
	    "    type, \n"
	    "    verified \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n",
	    AD_MED_TP,
	    local_ad.la_database_name,
	    local_ad.la_config_tp.amc_vol_table_name ) );

    
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
			"    and tapename like '%s' \n",
			volume ) );
	    }

	    AD_CHECK_DB( dbcmd( dbproc, 
		    "having \n"
		    "    tapename = max( tapename ) \n" ) );
	    break;

	case AD_VG_NAME:
	case AD_VG_NAME_ORIG:
	    volume = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    tapename like '%s' \n",
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
*   Function:	ad_tp_volume_put
*
*   Purpose:
*	Insert a volume into the tps table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB		: Error in the cadc db library.
*
************************************************************************
-*/

int		ad_tp_volume_put
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
	    "    tapename = '%s' \n",
	    local_ad.la_database_name,
	    local_ad.la_config_tp.amc_vol_table_name,
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
	    "    tapename, \n"
	    "    ingest_date, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    script, \n"
	    "    type, \n"
	    "    verified \n"
	    ") \n"
	    "values ( \n"
	    "	'%s', \n"		/* Archive			*/
	    "   '%s', \n"		/* Tapename.			*/
	    "   dateadd( second, %d, 'jan 1 1970' ), \n" /*Ingest date.	*/
	    "   %d, \n"			/* nfiles.			*/
	    "   %d, \n"			/* nkbytes.			*/
	    "   '%s', \n"		/* Tape script.			*/
	    "   '%s', \n"		/* Tape type.			*/
	    "   '%c' \n"		/* verify flag.			*/
	    ") \n"

	    "commit tran \n",
	    local_ad.la_database_name,
	    local_ad.la_config_tp.amc_vol_table_name,
	    ad_volume->adv_archive,
	    ad_volume->adv_volume_name,
	    ad_volume->adv_vol_info.tp.tpv_ingest_date, 
	    ad_volume->adv_vol_info.tp.tpv_nfiles,
	    ad_volume->adv_vol_info.tp.tpv_nkbytes,
	    ad_volume->adv_vol_info.tp.tpv_tp_script,
	    ad_volume->adv_vol_info.tp.tpv_type,
	    ad_volume->adv_vol_info.tp.tpv_verified ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}
#endif
