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
*   Module Name:	ad/src/od.c
*
*   Purpose:
*	Contains routines to access the od archive.
*
*   Routines:
*	int	ad_od_file_get	: Get information about a file or files.
*	int	ad_od_file_put	: Insert a file to the od archive tables..
*	int	ad_od_volume_get: Get information about volumes.
*	int	ad_od_volume_put: Insert a volume into the ods table.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: od.c
*	Version Number	: 1.10
*	Release Number	: 1
*	Last Updated	: 09/07/99
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/17 JSD  :	Added archive.
*	97/06/20 SEC  :	Mod'ed fname to file_id, Added compression and
*			format, Removed sever_name and host_name.
*       97/07/16 SEC  : Moved db and db server names to local_ad struct.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"

#include "ad.h"
#include "local_ad.h"

/*+
************************************************************************
*
*   Function:	ad_od_file_get
*
*   Purpose:
*	Gets information about files on optical disk.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_NO_FILE	: No file was available.
*
************************************************************************
-*/

int	ad_od_file_get
(
    AD_FILEGET	select_type,	/* (in)  The selection type.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbproc to use.		*/
    va_list	args		/* (in)  The arguments to use.		*/
)
{
    char	*archive;
    int		dirsectoff;
    char	*diskname;
    char	*filename;


    AD_CHECK_DB( dbfcmd( dbproc, 
	    "select \n"
	    "    %d, \n"
	    "    odf.fsize, \n"
	    "    odf.file_id, \n"
	    "    odf.status, \n"
	    "    odf.diskname, \n"
	    "    ods.archive, \n"
	    "    ods.dtype, \n"
	    "    odf.compression, \n"
	    "    odf.format, \n"
	    "    odf.dirsectoff, \n"
	    "    odf.fname \n"
	    "from \n"
	    "    %s..%s odf, \n"
	    "    %s..%s ods \n"
	    "where \n"
	    "    odf.diskname = ods.diskname \n",
	    AD_MED_OD,
	    local_ad.la_database_name, 
	    local_ad.la_config_od.amc_file_table_name,
	    local_ad.la_database_name,
	    local_ad.la_config_od.amc_vol_table_name ) );

    switch ( select_type )
    {
	case AD_FG_LOCATION:
	    diskname = (char *) va_arg( args, char * );
	    dirsectoff = (int) va_arg( args, int );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and odf.diskname = '%s' \n"
		    "    and odf.dirsectoff = %d \n",
		    diskname, dirsectoff ) );
	    break;

	case AD_FG_NAME:
	    if ( (archive = (char *) va_arg( args, char * ) ) == NULL )
	    {
		archive = local_ad.la_archive;
	    }
	    filename = (char *) va_arg( args, char * );;

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and ods.archive = '%s' \n"
		    "    and odf.file_id like '%s' \n"
		    "    and odf.status = '%c' \n",
		    archive, filename, AD_FILE_CURRENT ) );
	    break;

	case AD_FG_VOLUME:
	    diskname = (char *) va_arg( args, char * );
	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    and odf.diskname = '%s' \n",
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
*   Function:	ad_od_file_put
*
*   Purpose:
*	Insert a file into the od archive tables.  If the file already
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

int	ad_od_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @file_id		varchar(255) \n"
	    "declare @dirsectoff	int \n"
	    "declare @diskname		varchar(255) \n"

	    "select \n"
	    "    @diskname = '%s', \n"
	    "    @file_id = '%s', \n"
	    "    @dirsectoff = %d \n"

	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    dirsectoff = @dirsectoff \n"
	    "    and diskname = @diskname \n"

	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    diskname, \n"
	    "    dirsectoff, \n"
	    "    file_id, \n"
	    "    fname, \n"
	    "    fsize, \n"
	    "    status, \n"
	    "    compression, \n"
	    "    format \n"
	    ") \n"
	    "values ( \n"
	    "    @diskname, \n"
	    "    @dirsectoff, \n"
	    "    @file_id, \n"
	    "    '%s', \n"
	    "    %d, \n"
	    "    '%c', \n"
	    "    '%c', \n"
	    "    '%c' \n"
	    ") \n",
	    ad_file->adf_volume_name,
	    ad_file->adf_file_id,
	    ad_file->adf_location.od.odl_dirsectoff,
	    local_ad.la_database_name, 
	    local_ad.la_config_od.amc_file_table_name, 
	    local_ad.la_database_name, 
	    local_ad.la_config_od.amc_file_table_name, 
	    ad_file->adf_location.od.odl_filename,
	    ad_file->adf_filesize,
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
*   Function:	ad_od_volume_get
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

int		ad_od_volume_get
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
	    "    diskname, \n"
	    "    creation_date + 315532800,\n"
	    "    dirsectoff, \n"
	    "    diskname_orig, \n"
	    "    ingest_date + 315532800, \n"
	    "    flag, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    reverse, \n"
	    "    dtype \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n",
	    AD_MED_OD,
	    local_ad.la_database_name,
	    local_ad.la_config_od.amc_vol_table_name ) );

    
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
	    volume = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    diskname like '%s' \n",
		    volume ) );
	    break;

	case AD_VG_NAME_ORIG:
	    volume = (char *) va_arg( args, char * );

	    AD_CHECK_DB( dbfcmd( dbproc, 
		    "    diskname_orig like '%s' \n",
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
*   Function:	ad_od_volume_put
*
*   Purpose:
*	Insert a volume into the ods table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB		: Error in the cadc db library.
*
************************************************************************
-*/

int		ad_od_volume_put
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
	    local_ad.la_config_od.amc_vol_table_name,
	    ad_volume->adv_volume_name ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    AD_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    accesses, \n"
	    "    archive, \n"
	    "    creation_date, \n"
	    "    dirsectoff, \n"
	    "    diskname, \n"
	    "    diskname_orig, \n"
	    "    dtype, \n"
	    "    error_count, \n"
	    "    flag, \n"
	    "    ingest_date, \n"
	    "    nfiles, \n"
	    "    nkbytes, \n"
	    "    remark, \n"
	    "    reverse, \n"
	    "    shelf \n"
	    ") \n"
	    "values ( \n"
	    "   0, \n"			/* Accesses			*/
	    "	'%s', \n"		/* Archive			*/
	    "   %d, \n"			/* Creation date.		*/
	    "   %d, \n"			/* Dirsectoff			*/
	    "   '%s', \n"		/* Diskname.			*/
	    "   '%s', \n"		/* Diskname orig.		*/
	    "   '%s', \n"		/* Disk type.			*/
	    "   0, \n"			/* Error count.			*/
	    "   '%c', \n"		/* verify flag.			*/
	    "   %d, \n"			/* Ingest date.			*/
	    "   %d, \n"			/* nfiles.			*/
	    "   %d, \n"			/* nkbytes.			*/
	    "   '', \n"			/* remark.			*/
	    "   '%s', \n"		/* reverse.			*/
	    "   '' \n"			/* shelf.			*/
	    ") \n"

	    "commit tran \n",
	    local_ad.la_database_name,
	    local_ad.la_config_od.amc_vol_table_name,
	    ad_volume->adv_archive,
	    ad_volume->adv_vol_info.od.odv_creation_date - 315532800L, 
	    ad_volume->adv_vol_info.od.odv_dirsectoff,
	    ad_volume->adv_volume_name,
	    ad_volume->adv_vol_info.od.odv_diskname_orig,
	    ad_volume->adv_vol_info.od.odv_type,
	    ad_volume->adv_vol_info.od.odv_verified,
	    ad_volume->adv_vol_info.od.odv_ingest_date - 315532800L, 
	    ad_volume->adv_vol_info.od.odv_nfiles,
	    ad_volume->adv_vol_info.od.odv_nkbytes,
	    ad_volume->adv_vol_info.od.odv_reverse_name ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}
