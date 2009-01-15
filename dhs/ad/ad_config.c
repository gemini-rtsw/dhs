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
*   Module Name:	ad/src/ad_config.c
*
*   Purpose:
*	Initailizes the configuration for the ad library.
*
*   Routines:
*	 int	ad_config_init	: Initailize the ad library configuration.
* static int	add_archive	: Get the default archive. 
* static int	add_compression	: Get the compression config info.
* static int	add_database	: Get the database config info.
* static int	add_default	: Get the default archive name.
* static int	add_format	: Get the format config info.
* static int	add_medium	: Get the medium config info.
* static int	add_mfs		: Get mountable file system info.
* static int	add_remote	: Get the remote files config info. 
* static int	compare_priorities
*				: Compare cd priorities.
*
*   Date		: Nov 22, 1996.
*
*   Programmer		: Marc LeBlanc
*
*   Field SCCS data	: @(#)
*	Module Name	: ad_config.c
*	Version Number	: 1.18
*	Release Number	: 1
*	Last Updated	: 04/05/00
*
*
*   Modification History:
*	97/01/31 JSD  :	Made remote hosts non-mandatory.
*	97/02/17 JSD  :	Added default archive.
*	97/02/19 JSD  :	Added msg_clear after reading mediums, incase the
*			medium is not there, just ignore the message.
*	97/04/01 SEC  :	Added add_database(), and server name now goes into 
*			the apc_ and amc_ structs.
*	97/06/06 JSD  :	Fixed servername in add_local to get server name rather
*			then cdfiles name.
*	97/06/20 SEC  :	Added add_compression(), add_format(). Removed
*			ad_priority_config, replaced with priority for
*			all media types. Added default archive to config.
*       97/07/16 SEC  : Moved db and db server names to local_ad struct.
*	97/08/25 SEC  :	Added site_id to add_remote(), mod'ed config_get() 
*			call order, initializing local_ad.la_remote.
*	98/03/10 gzg  : Added features to handle tape tp.
*	98/06/23 sjg  : Added add_archive() to configuration info.
*	98/08/31 jsd  : Added features to handle dvd.
*	99/05/19 sjg  : Changed ada_upper to tri-state case conversion.
*     2000/03/24 SDJ  : Added mfs (mountable file system) configuration info.
*			Removed references to dvd (vd) tables and switched
*			to the mfs tables. Added file format SDF
*     2000/04/05 SDJ  : Removed AD_MFS_OD, no longer valid.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>

#include <sybfront.h>
#include <sybdb.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "gen_eptr.h"

#include "ad.h"
#include "local_ad.h"

#define	KEY_ARCHIVE		"archive"
#define KEY_COMPRESSION 	"compression"
#define	KEY_DEFAULT		"default"
#define	KEY_DATABASE		"database"
#define	KEY_FORMAT		"format"
#define	KEY_MEDIUM		"medium"
#define KEY_REMOTE		"remote"
#define KEY_VOL_MFS		"vol_mfs"

static boolean	add_archive( int, char ** );
static boolean	add_compression( int, char ** );
static boolean	add_database( int, char ** );
static boolean	add_default( int, char ** );
static boolean	add_format( int, char ** );
static boolean	add_medium( int, char ** );
static boolean	add_mfs( int, char ** );
static boolean	add_remote( int, char ** );

static int compare_priorities( AD_MEDIUM_CONFIG *, AD_MEDIUM_CONFIG * );


/*+
************************************************************************
*
*   Function:	ad_config_init
*
*   Purpose:
*	Initialize the configuration.
*
*   Values Returned:
*	int	AD_SUCCESS	    : Normal completion.
*	int	AD_E_CONFIG	    : Error reading config file.
*
************************************************************************
-*/

int	ad_config_init
(
    void
)
{
    char	*config_file;
    int		get_status;


    AD_CHECK_CONFIG( config_open( AD_LIBRARY, &config_file ) );
    ad_format_message( AD_CFILE_OPEN, config_file );


    local_ad.la_media_list 		= NULL;
    local_ad.la_fmt_list 		= NULL;
    local_ad.la_comp_list 		= NULL;
    local_ad.la_archive[0] 		= '\0';
    local_ad.la_files_table[0] 		= '\0';
    local_ad.la_media_table[0] 		= '\0';
    local_ad.la_servers_table[0]	= '\0';
    local_ad.la_database_name[0] 	= '\0';
    local_ad.la_db_server_name[0] 	= '\0';
    local_ad.la_site_id[0] 		= '\0';
    local_ad.la_remote			= FALSE;

    local_ad.la_config_cd.amc_medium_type[0]		= '\0';
    local_ad.la_config_cd.amc_file_table_name[0]	= '\0';
    local_ad.la_config_cd.amc_vol_table_name[0]		= '\0';
    local_ad.la_config_cd.amc_priority 			= -1;

    local_ad.la_config_db.amc_medium_type[0] 		= '\0';
    local_ad.la_config_db.amc_file_table_name[0] 	= '\0';
    local_ad.la_config_db.amc_vol_table_name[0] 	= '\0';
    local_ad.la_config_db.amc_priority 			= -1;

    local_ad.la_config_md.amc_medium_type[0] 		= '\0';
    local_ad.la_config_md.amc_file_table_name[0]	= '\0';
    local_ad.la_config_md.amc_vol_table_name[0] 	= '\0';
    local_ad.la_config_md.amc_priority 			= -1;

    local_ad.la_config_mfs.amc_medium_type[0]		= '\0';
    local_ad.la_config_mfs.amc_file_table_name[0] 	= '\0';
    local_ad.la_config_mfs.amc_vol_table_name[0] 	= '\0';
    local_ad.la_config_mfs.amc_priority 		= -1;

    local_ad.la_config_od.amc_medium_type[0] 		= '\0';
    local_ad.la_config_od.amc_file_table_name[0] 	= '\0';
    local_ad.la_config_od.amc_vol_table_name[0] 	= '\0';
    local_ad.la_config_od.amc_priority 			= -1;

    local_ad.la_config_tp.amc_medium_type[0] 		= '\0';
    local_ad.la_config_tp.amc_file_table_name[0] 	= '\0';
    local_ad.la_config_tp.amc_vol_table_name[0] =	 '\0';
    local_ad.la_config_tp.amc_priority 			= -1;


    /*
     *  Parse the file for required keywords.
     */

    AD_CHECK_CONFIG( config_get( KEY_DATABASE, add_database ) );
    AD_CHECK_CONFIG( config_get( KEY_MEDIUM, add_medium ) );
    AD_CHECK_CONFIG( config_get( KEY_VOL_MFS, add_mfs ) );
    AD_CHECK_CONFIG( config_get( KEY_ARCHIVE, add_archive ) );
    AD_CHECK_CONFIG( config_get( KEY_DEFAULT, add_default ) );
    AD_CHECK_CONFIG( config_get( KEY_COMPRESSION, add_compression ) );
    AD_CHECK_CONFIG( config_get( KEY_FORMAT, add_format ) );


    /*
     *  Keyword `remote' is optional. 
     */

    get_status = config_get( KEY_REMOTE, add_remote );
    if ( get_status != GC_E_KEY_NOT_FOUND )
    {
	AD_CHECK_CONFIG( get_status );
    }
    msg_clear( gc_msg );


    /*
     *  Close config file, sort lists. 
     */

    config_close();

    (void) eptr_sort( local_ad.la_media_list, compare_priorities );
    (void) eptr_sort( local_ad.la_comp_list, ad_comp_compare_sort );
    (void) eptr_sort( local_ad.la_fmt_list, ad_fmt_compare_sort );
    (void) eptr_sort( local_ad.la_archive_list, ad_archive_sort );
    (void) eptr_sort( local_ad.la_voltype_list, ad_mfs_compare_sort );
    

    return( AD_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	add_archive
*
*   Purpose:
*	Adds the archive info 
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_archive
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    AD_ARCHIVE	*ad_archive;

    if ( num_tokens != 3 ||
	    strlen( tokens[1] ) > AD_ARCHNAME_LEN ||
	    strlen( tokens[2] ) != 1 )
    {
	return( FALSE );
    }
    AD_CHECK_NULL_CONFIG( ad_archive = gen_alloc( sizeof( AD_ARCHIVE ) ) );
    (void) strcpy( ad_archive->ada_name, tokens[1] );

    if ( streq( tokens[2], "L" ) )
    {
    	ad_archive->ada_case = AD_CASE_LOWER;
    }
    else if ( streq( tokens[2], "N" ) )
    {
    	ad_archive->ada_case = AD_CASE_NONE;
    }
    else if ( streq( tokens[2], "U" ) )
    {
    	ad_archive->ada_case = AD_CASE_UPPER;
    }
    else
    {
	return( FALSE );
    }

    if ( local_ad.la_archive_list == NULL )
    {
	AD_CHECK_NULL_CONFIG( local_ad.la_archive_list = eptr_alloc( 20 ) );
    }

    AD_CHECK_NULL_CONFIG( eptr_insert( local_ad.la_archive_list,
	    eptrlen( local_ad.la_archive_list ), ad_archive ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_compression
*
*   Purpose:
*	Adds the compression info 
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_compression
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    AD_COMP_CONFIG
    		*ad_comp_cfg;

    
    if ( num_tokens != 3 ||
	    (int) strlen( tokens[1] ) != 1 ||
	    (int) strlen( tokens[2] ) > AD_COMP_EXT_LEN )
    {
	return( FALSE );
    }

    AD_CHECK_NULL_CONFIG( ad_comp_cfg = gen_alloc( 
	    sizeof( AD_COMP_CONFIG ) ) );

    switch( tokens[1][0] )
    {
	case AD_COMP_GZIP:
	    ad_comp_cfg->acc_type = AD_COMP_GZIP;
	    break;

	case AD_COMP_CF:
	    ad_comp_cfg->acc_type = AD_COMP_CF;
	    break;

	case AD_COMP_UNIX:
	    ad_comp_cfg->acc_type = AD_COMP_UNIX;
	    break;

	default:
	    ad_format_message( AD_E_COMP );
	    return( FALSE );
    }
	    
    (void) strcpy( ad_comp_cfg->acc_ext, tokens[2] );


    if ( local_ad.la_comp_list == NULL )
    {
	AD_CHECK_NULL_CONFIG( local_ad.la_comp_list = eptr_alloc( 4 ) );
    }
    AD_CHECK_NULL_CONFIG( eptr_insert( local_ad.la_comp_list,
	    eptrlen( local_ad.la_comp_list ), ad_comp_cfg ) );
				     

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_database
*
*   Purpose:
*	Adds the database info to the configuration, for later adding
*	to the ad configuration information.
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_database
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    if ( num_tokens != 3 || 
	    (int) strlen( tokens[1] ) > AD_SYBASENAME_LEN ||
	    (int) strlen( tokens[1] ) == 0 ||
	    (int) strlen( tokens[2] ) > AD_SYBASENAME_LEN ||
	    (int) strlen( tokens[2] ) == 0  )
    {
	return( FALSE );
    }


    (void) strcpy( local_ad.la_db_server_name, tokens[1] );
    (void) strcpy( local_ad.la_database_name, tokens[2] );


    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_default
*
*   Purpose:
*	Adds the default archive configuration info. 
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_default
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    if ( num_tokens != 2 || (int) strlen( tokens[1] ) > AD_ARCHNAME_LEN )
    {
	return( FALSE );
    }

    (void) strcpy( local_ad.la_archive, tokens[1] );


    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_format
*
*   Purpose:
*	Adds the format info 
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_format
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    AD_FMT_CONFIG
    		*ad_fmt_cfg;

    
    if ( num_tokens != 3 ||
	    (int) strlen( tokens[1] ) != 1 ||
	    (int) strlen( tokens[2] ) > AD_FMT_EXT_LEN )
    {
	return( FALSE );
    }

    AD_CHECK_NULL_CONFIG( ad_fmt_cfg = gen_alloc( 
	    sizeof( AD_FMT_CONFIG ) ) );

    switch( tokens[1][0] )
    {
	case AD_FMT_FITS:
	    ad_fmt_cfg->afc_type = AD_FMT_FITS;
	    break;

	case AD_FMT_GSD:
	    ad_fmt_cfg->afc_type = AD_FMT_GSD;
	    break;

	case AD_FMT_SDF:
	    ad_fmt_cfg->afc_type = AD_FMT_SDF;
	    break;

	case AD_FMT_SDS:
	    ad_fmt_cfg->afc_type = AD_FMT_SDS;
	    break;

	case AD_FMT_RAW:
	    ad_fmt_cfg->afc_type = AD_FMT_RAW;
	    break;

	default:
	    ad_format_message( AD_E_FMT );
	    return( FALSE );
    }
	    
    (void) strcpy( ad_fmt_cfg->afc_ext, tokens[2] );


    if ( local_ad.la_fmt_list == NULL )
    {
	AD_CHECK_NULL_CONFIG( local_ad.la_fmt_list = eptr_alloc( 4 ) );
    }
    AD_CHECK_NULL_CONFIG( eptr_insert( local_ad.la_fmt_list,
	    eptrlen( local_ad.la_fmt_list ), ad_fmt_cfg ) );
				     

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_medium
*
*   Purpose:
*	Adds the media to the ad library configuration.
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_medium
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    char	*medium_type;
    AD_MEDIUM_CONFIG	
		*medium_cfg;


    if ( num_tokens != 5 || 
	    (int) strlen( tokens[2] ) > AD_MEDNAME_LEN || 
	    (int) strlen( tokens[2] ) == 0 ||
	    (int) strlen( tokens[3] ) > AD_SYBASENAME_LEN ||
	    (int) strlen( tokens[3] ) == 0 ||
	    (int) strlen( tokens[4] ) > AD_SYBASENAME_LEN ||
	    (int) strlen( tokens[4] ) == 0  )
    {
	return( FALSE );
    }


    medium_type = tokens[2];

    if ( streq( medium_type, AD_CD_NAME ) )
    {
	medium_cfg = &local_ad.la_config_cd;
	medium_cfg->amc_medium = AD_MED_CD;
    }
    else if ( streq( medium_type, AD_DB_NAME ) )
    {
	medium_cfg = &local_ad.la_config_db;
	medium_cfg->amc_medium = AD_MED_DB;
    }
    else if ( streq( medium_type, AD_MD_NAME ) )
    {
	medium_cfg = &local_ad.la_config_md;
	medium_cfg->amc_medium = AD_MED_MD;
    }
    else if ( streq( medium_type, AD_MFS_NAME ) )
    {
	medium_cfg = &local_ad.la_config_mfs;
	medium_cfg->amc_medium = AD_MED_MFS;
    }
    else if ( streq( medium_type, AD_OD_NAME ) )
    {
	medium_cfg = &local_ad.la_config_od;
	medium_cfg->amc_medium = AD_MED_OD;
    }
    else if ( streq( medium_type, AD_TP_NAME ) )
    {
	medium_cfg = &local_ad.la_config_tp;
	medium_cfg->amc_medium = AD_MED_TP;
    }
    else
    {
	ad_format_message( AD_E_MEDIUM_TYPE, medium_type );
	return( FALSE );
    }

    if ( !str2int( tokens[1], &(medium_cfg->amc_priority) ) )
    {
	return( FALSE );
    }
    (void) strcpy( medium_cfg->amc_medium_type, medium_type );
    (void) strcpy( medium_cfg->amc_file_table_name, tokens[3] );
    (void) strcpy( medium_cfg->amc_vol_table_name, tokens[4] );

    if ( local_ad.la_media_list == NULL )
    {
	AD_CHECK_NULL_CONFIG( local_ad.la_media_list = eptr_alloc( 4 ) );
    }
    AD_CHECK_NULL_CONFIG( eptr_insert( local_ad.la_media_list, 
	    eptrlen( local_ad.la_media_list ), medium_cfg ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_mfs
*
*   Purpose:
*	Adds the moutable file system information to the ad
*	library configuration.
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_mfs
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    AD_MFS_CONFIG
		*mfs_cfg;	/* Mountable file system structure.	*/


    /*
     * Make sure the number of tokens is correct and that the strings
     * are not too long.
     */
    
    if ( num_tokens != 3 || 
	    (int) strlen( tokens[1] ) > AD_VOLUMETYPE_LEN || 
	    (int) strlen( tokens[1] ) == 0 ||
	    (int) strlen( tokens[2] ) > AD_VOLUMEFORMAT_LEN || 
	    (int) strlen( tokens[2] ) == 0 )
    {
	return( FALSE );
    }


    /*
     * Add the information if the type of media is a recognized one.
     */

    AD_CHECK_NULL( mfs_cfg = gen_alloc( sizeof( AD_MFS_CONFIG ) ) );
    
    if ( strne( tokens[1], AD_MFS_CD ) &&
	    strne( tokens[1], AD_MFS_DVD ) &&
	    strne( tokens[1], AD_MFS_MD ) )
    {
	ad_format_message( AD_E_MEDIUM_TYPE, tokens[1] );
	return( FALSE );    }
    else
    {
	(void) strcpy( mfs_cfg->amfsc_medium_type, tokens[1] );
	(void) strcpy( mfs_cfg->amfsc_format, tokens[2] );	
    }
    
    if ( local_ad.la_voltype_list == NULL )
    {
	AD_CHECK_NULL_CONFIG( local_ad.la_voltype_list = eptr_alloc( 4 ) );
    }
    AD_CHECK_NULL_CONFIG( eptr_insert( local_ad.la_voltype_list, 
	    eptrlen( local_ad.la_voltype_list ), mfs_cfg ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	add_remote
*
*   Purpose:
*	Adds remote files information to the ad library configuration.
*
*   Values Returned:
*	boolean	TRUE		: Data parsed & inserted correctly.
*	boolean	FALSE		: Data failed to parse.
*
************************************************************************
-*/

static boolean	add_remote
(
    int		num_tokens,	/* (in)  Number of tokens on input line.*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    if ( num_tokens != 5 ||
            (int) strlen( tokens[1] ) > AD_SITE_ID_LEN ||
            (int) strlen( tokens[1] ) == 0 ||
            (int) strlen( tokens[2] ) > AD_SYBASENAME_LEN ||
            (int) strlen( tokens[2] ) == 0 ||
            (int) strlen( tokens[3] ) > AD_SYBASENAME_LEN ||
            (int) strlen( tokens[3] ) == 0 ||
            (int) strlen( tokens[4] ) > AD_SYBASENAME_LEN ||
            (int) strlen( tokens[4] ) == 0  )
    {
	return( FALSE );
    }

    local_ad.la_remote = TRUE;

    (void) strcpy( local_ad.la_site_id, tokens[1] );
    (void) strcpy( local_ad.la_files_table, tokens[2] );
    (void) strcpy( local_ad.la_media_table, tokens[3] );
    (void) strcpy( local_ad.la_servers_table, tokens[4] );


    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	compare_priorities
*
*   Purpose:
*	Compares the priorities of two cd_table structures.
*
*   Values Returned:
*	int	-1		: priority1 < priority2
*	int	0		: priority1 == priority2
*	int	1		: priority1 > priority2
*
************************************************************************
-*/
static int	compare_priorities
(
    AD_MEDIUM_CONFIG	*table1,	/* (in)  The first timer.	*/
    AD_MEDIUM_CONFIG	*table2		/* (in)  The second timer.	*/
)
{
    if ( table1->amc_priority < table2->amc_priority )
    {
	return( -1 );
    }
    else if ( table1->amc_priority == table2->amc_priority )
    {
	return( 0 );
    }
    else
    {
	return( 1 );
    }
}
