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
*   Module Name:	ad/src/remote.c
*
*   Purpose:
*	Contains routines to access the remote archive.
*
*   Routines:
*	int	ad_remote_file_put	: Insert a file to the cdfile table.
*	int	ad_remote_volume_put: Insert a volume into the cds table.
*
*   Date		: Aug 25, 1997
*
*
*   Field SCCS data	: @(#)
*	Module Name	: remote.c
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/12/17 JSD  :	Added new medium types to ad_remote_volume_put and
*			made it return AD_E_MEDIUM_ENUM if it was an invalid
*			type.
*     2000/03/23 SDJ  : Removed type AD_MED_VD, added AD_MED_MFS.
*     2000/04/05 SDJ  : Removed mfs_mounted from MFS_VOL_INFO struct.
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
#if defined(SYBASE_DHS)
#include "db.h"
#endif
#include "gen_eptr.h"

#include "ad.h"
#include "local_ad.h"

/*+
************************************************************************
*
*   Function:	ad_remote_file_put
*
*   Purpose:
*	Insert a file into the archive remote tables. If the file already
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
#if defined(SYBASE_DHS)
int	ad_remote_file_put
(
    AD_FILE	*ad_file,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int			dbstatus;


    AD_CHECK_DB_LIB( db_simple_query( dbproc,
	    "declare @file_id    varchar(255) \n"
	    "declare @media_id   varchar(255) \n"
	    "declare @site_id  varchar(255) \n"

	    "select \n"
	    "    @file_id = '%s', \n"
	    "    @media_id = '%s', \n"
	    "    @site_id = '%s' \n"

	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    file_id = @file_id \n"
	    "    and media_id = @media_id \n"
	    "    and site_id = @site_id \n"

	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    file_id, \n"
	    "    media_id, \n"
	    "    site_id, \n"
	    "    status, \n"
	    "    compression, \n"
	    "    format ) \n"
	    "values ( \n"
	    "    @file_id, \n"
	    "    @media_id, \n"
	    "    @site_id, \n"
	    "    '%c', \n"
	    "    '%c', \n"
	    "    '%c' \n"
	    ") \n",
	    ad_file->adf_file_id,
	    ad_file->adf_volume_name,
	    local_ad.la_site_id,
	    local_ad.la_database_name,
	    local_ad.la_files_table,
	    local_ad.la_database_name,
	    local_ad.la_files_table,
	    ad_file->adf_status,
	    ad_file->adf_compression,
	    ad_file->adf_format ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	ad_remote_volume_put
*
*   Purpose:
*	Insert a volume into the remote media table.
*
*   Values Returned:
*	int	AD_SUCCESS	: Normal completion.
*	int	AD_E_DB		: Error in the sybase db library.
*	int	AD_E_DB_LIB	: Error in the cadc db library.
*	int	AD_E_MEDIUM_ENUM: Not a valid medium. 
*
************************************************************************
-*/

int		ad_remote_volume_put
(
    AD_VOLUME	*ad_volume,	/* (in)  The volume to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;	/* Return status from dbresults().	*/
    boolean	mounted;	/* Is the volume mounted or not?	*/
 
 
    switch( ad_volume->adv_medium_type )
    {
	case AD_MED_CD:
	case AD_MED_MD:
	case AD_MED_MFS:
	    mounted = ad_volume->adv_mounted;
	    break;

	case AD_MED_DB:
	    mounted = TRUE;
	    break;

	case AD_MED_OD:
	    mounted = FALSE;
	    break;

	case AD_MED_TP:
	    mounted = FALSE;
	    break;

	default :
	    ad_format_message( AD_E_MEDIUM_ENUM, ad_volume->adv_medium_type );
	    return( AD_E_MEDIUM_ENUM );
    }

    AD_CHECK_DB_LIB( db_simple_query( dbproc,
	    "declare @media_id varchar(255) \n"
	    "declare @site_id  varchar(255) \n"
	    "declare @archive  varchar(255) \n"

	    "select \n"
	    "    @media_id = '%s', \n"
	    "    @site_id = '%s', \n"
	    "    @archive = '%s' \n"

	    "delete from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_id = @media_id \n"
	    "    and site_id = @site_id \n"
	    "    and archive = @archive \n"

	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    media_id, \n"
	    "    site_id, \n"
	    "    archive, \n"
	    "    mounted ) \n"
	    "values ( \n"
	    "    @media_id, \n"
	    "    @site_id, \n"
	    "    @archive, \n"
	    "    '%c' \n"
	    ") \n",
	    ad_volume->adv_volume_name,
	    local_ad.la_site_id,
	    ad_volume->adv_archive,
	    local_ad.la_database_name,
	    local_ad.la_media_table,
	    local_ad.la_database_name,
	    local_ad.la_media_table,
	    mounted ? 'Y' : 'N' ) );

    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AD_CHECK_DB( dbstatus );


    return( AD_SUCCESS );
}
#endif
