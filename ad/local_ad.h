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
*   Include File Name:	ad/h/local_ad.h
*
*   Purpose:
*	Contains the local definitions for the ad library.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: local_ad.h
*	Version Number	: 1.14
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/31 JSD  :	Changed name to have capital A.
*	97/02/05 JSD  :	Added macros.
*	97/02/17 JSD  :	Added archive to localAd.
*	97/04/01 SEC  :	Added database server name to amc_ and apc_.
*	97/06/20 SEC  : Removed priority config for cd only, added 
*			for all media, added structures for holding
*			compression and format config information. 
*			Added prototypes for ad_*_compare_*(). 
*	97/07/16 SEC  :	Moved db and db server names to local_ad struct.
*	97/08/25 SEC  :	Mod'ed local_ad struct def'n. 
*	98/04/02 gzg  : Added features to support tape tp.	
*	98/06/23 sjg  : Added concept of supported archives to config.
*	98/08/31 jsd  : Added features to support dvd.
*     2000/03/24 SDJ  : Changed AD_MEDIUM_CONFIG struct slightly.
*			Switched all vd (dvd) to mfs (mountable file
*			system.
*     2000/04/05 SDJ  : Made the AD_CRC_STRING_LEN bigger so the
*			archive could be added.
*     2000/05/10 NRH  : Added function ad_onefile_media.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#define	AD_LIBRARY	"libAd"


#define	AD_CHECK( s )							\
	{   int estatus;						\
	    if ( ( estatus = ( s ) ) != AD_SUCCESS )			\
		return( estatus ); }

#define AD_CHECK_CONFIG(s)						\
	{   if ( ( s ) != GC_SUCCESS ) {				\
		ad_format_message( AD_E_CONFIG );			\
		msg_append( ad_msg, gc_msg );				\
		msg_clear( gc_msg );					\
		return( AD_E_CONFIG ); } }

#define AD_CHECK_DB(s)							\
	{   if ( ( s ) == FAIL ) {                 			\
		ad_format_message( AD_E_DB );   			\
		msg_append( ad_msg, db_msg );				\
		msg_clear( db_msg );					\
		return( AD_E_DB ); } }

#define AD_CHECK_DB_LIB(s)						\
	{   if ( ( s ) != DB_SUCCESS ) {               			\
		ad_format_message( AD_E_DB_LIB );   			\
		msg_append( ad_msg, db_msg );				\
		msg_clear( db_msg );					\
		return( AD_E_DB_LIB ); } }

#define	AD_CHECK_INIT							\
	if ( ! ad_init_flag ) {						\
	    ad_format_message( AD_E_INIT ); 				\
	    return( AD_E_INIT ); }

#define AD_CHECK_MEDIUM_CD						\
	{ 								\
	    if ( local_ad.la_config_cd.amc_medium_type[0] == 0 )	\
	    {								\
		ad_format_message( AD_E_MEDIUM_ENUM, AD_MED_CD );	\
		return( AD_E_MEDIUM_ENUM );				\
	    }								\
	}

#define AD_CHECK_MEDIUM_DB						\
	{ 								\
	    if ( local_ad.la_config_db.amc_medium_type[0] == 0 )	\
	    {								\
		ad_format_message( AD_E_MEDIUM_ENUM, AD_MED_DB );	\
		return( AD_E_MEDIUM_ENUM );				\
	    }								\
	}

#define AD_CHECK_MEDIUM_MD						\
	{ 								\
	    if ( local_ad.la_config_md.amc_medium_type[0] == 0 )	\
	    {								\
		ad_format_message( AD_E_MEDIUM_ENUM, AD_MED_MD );	\
		return( AD_E_MEDIUM_ENUM );				\
	    }								\
	}

#define AD_CHECK_MEDIUM_MFS						\
	{ 								\
	    if ( local_ad.la_config_mfs.amc_medium_type[0] == 0 )	\
	    {								\
		ad_format_message( AD_E_MEDIUM_ENUM, AD_MED_MFS );	\
		return( AD_E_MEDIUM_ENUM );				\
	    }								\
	}

#define AD_CHECK_MEDIUM_OD						\
	{ 								\
	    if ( local_ad.la_config_od.amc_medium_type[0] == 0 )	\
	    {								\
		ad_format_message( AD_E_MEDIUM_ENUM, AD_MED_OD );	\
		return( AD_E_MEDIUM_ENUM );				\
	    }								\
	}

#define AD_CHECK_MEDIUM_TP						\
	{ 								\
	    if ( local_ad.la_config_tp.amc_medium_type[0] == 0 )	\
	    {								\
		ad_format_message( AD_E_MEDIUM_ENUM, AD_MED_TP );	\
		return( AD_E_MEDIUM_ENUM );				\
	    }								\
	}

#define	AD_CHECK_NULL( s )						\
        {   if ( ( s ) == NULL ) {					\
                    ad_format_message( AD_E_MEMORY );			\
                    return( AD_E_MEMORY ); } }				 

#define AD_CHECK_NULL_CONFIG(s)                                         \
        {   if ( (s) == NULL )                                          \
            {   ad_format_message( AD_E_MEMORY );                       \
                return( FALSE ); } }

/*
 *  Define the names of the archive media.
 */

#define	AD_CD_NAME		"CD"	/* Name of the cd archive.	*/
#define	AD_DB_NAME		"DB"	/* Name of the db archive.	*/
#define	AD_MD_NAME		"MD"	/* Name of the md archive.	*/
#define AD_MFS_NAME		"MFS"	/* Name of the mfs archive.	*/
#define	AD_OD_NAME		"OD"	/* Name of the od archive.	*/
#define	AD_TP_NAME		"TP"	/* Name of the tp archive.	*/


/*
 * Define the length of a CRC input string.
 */

#define AD_CRC_STRING_LEN	60


/*
 *  Type definitions. 
 */

typedef struct ad_compression_config
{
    char	acc_type;		/* Type of compression extenshn.*/
    AD_COMP_EXT	acc_ext;		/* The extension itself.	*/
} AD_COMP_CONFIG;


typedef struct ad_format_config
{
    char	afc_type;		/* Type of format extension.	*/
    AD_FMT_EXT	afc_ext;		/* The extension itself.	*/
} AD_FMT_CONFIG;


typedef struct ad_medium_config
{
    AD_SYBNAME	amc_file_table_name;	/* File table name.		*/
    AD_MEDIUM	amc_medium;		/* Enumerated medium type.	*/
    AD_MEDNAME	amc_medium_type;	/* Name of the medium.		*/
    int		amc_priority;		/* Priority of this medium.	*/
    AD_SYBNAME	amc_vol_table_name;	/* Volume table name.		*/
} AD_MEDIUM_CONFIG;


typedef struct ad_mfs_config
{
    AD_VOLTYPE		amfsc_medium_type;
    					/* Type of mediuam. 		*/
    AD_VOLFORMAT	amfsc_format;	/* Which File system format.	*/
} AD_MFS_CONFIG;

    
typedef struct local_ad
{
    EPTR		*la_archive_list;/* List of archives.		*/
    EPTR		*la_comp_list;	/* List of compression info.	*/
    AD_MEDIUM_CONFIG	la_config_cd;	/* CD configuration.		*/
    AD_MEDIUM_CONFIG	la_config_db;	/* DB configuration.		*/
    AD_MEDIUM_CONFIG	la_config_md;	/* MD configuration.		*/
    AD_MEDIUM_CONFIG	la_config_mfs;	/* MFS configuration.		*/
    AD_MEDIUM_CONFIG	la_config_od;	/* OD configuration.		*/
    AD_MEDIUM_CONFIG	la_config_tp;	/* TP configuration.		*/
    AD_SYBNAME		la_database_name;	
    					/* Database tables are in.	*/
    AD_SYBNAME		la_db_server_name;	
    					/* Database server to use.	*/
    AD_ARCHNAME		la_archive;	/* Default archive name.	*/
    EPTR		*la_fmt_list;	/* List of format info.		*/
    AD_SYBNAME		la_files_table;	/* Master files table name.	*/
    AD_SITE_ID		la_site_id;	/* Local site's site ID.	*/
    EPTR		*la_media_list;	/* Priority media list.		*/
    AD_SYBNAME		la_media_table;	/* Master media units table name*/
    boolean		la_remote; 	/* Flag. Do we use remote tbls?	*/
    AD_SYBNAME		la_servers_table;
    EPTR		*la_voltype_list;/* Volume type list.	.	*/
} LOCAL_AD;


int	ad_archive_search( char *, AD_ARCHIVE * );
int	ad_archive_sort( AD_ARCHIVE *, AD_ARCHIVE * );
int	ad_cd_file_get( AD_FILEGET, DBPROCESS *, va_list );
int	ad_cd_file_put( AD_FILE *, DBPROCESS * );
int	ad_cd_volume_get( AD_VOLGET, DBPROCESS *, va_list );
int	ad_cd_volume_put( AD_VOLUME *, DBPROCESS * );
int     ad_comp_compare_srch( char *, AD_COMP_CONFIG * );
int     ad_comp_compare_sort( AD_COMP_CONFIG *, AD_COMP_CONFIG * );
int	ad_config_init( void );
int	ad_db_file_get( AD_FILEGET, DBPROCESS *, va_list );
int	ad_db_file_put( AD_FILE *, DBPROCESS * );
int	ad_db_volume_get( AD_VOLGET, DBPROCESS *, va_list );
int	ad_db_volume_put( AD_VOLUME *, DBPROCESS * );
int     ad_fmt_compare_srch( char *, AD_FMT_CONFIG * );
int     ad_fmt_compare_sort( AD_FMT_CONFIG *, AD_FMT_CONFIG * );
void	ad_format_message( int, ... );
int	ad_get_next_file( DBPROCESS *, AD_FILE * );
int	ad_get_next_volume( DBPROCESS *, AD_VOLUME * );
int	ad_md_file_del( DBPROCESS *, char *, char * );
int	ad_md_file_get( AD_FILEGET, DBPROCESS *, va_list );
int	ad_md_file_put( AD_FILE *, DBPROCESS * );
int	ad_md_volume_get( AD_VOLGET, DBPROCESS *, va_list );
int	ad_md_volume_put( AD_VOLUME *, DBPROCESS * );
int	ad_mfs_compare_sort( AD_MFS_CONFIG *, AD_MFS_CONFIG * );
int	ad_mfs_compare_srch( MFS_VOL_INFO *, AD_MFS_CONFIG * );
int	ad_mfs_file_get( AD_FILEGET, DBPROCESS *, va_list );
int	ad_mfs_file_put( AD_FILE *, DBPROCESS * );
int	ad_mfs_volume_get( AD_VOLGET, DBPROCESS *, va_list );
int	ad_mfs_volume_put( AD_VOLUME *, DBPROCESS * );
boolean	ad_mfs_volume_verify( AD_VOLUME * );
int	ad_od_file_get( AD_FILEGET, DBPROCESS *, va_list );
int	ad_od_file_put( AD_FILE *, DBPROCESS * );
int	ad_od_volume_get( AD_VOLGET, DBPROCESS *, va_list );
int	ad_od_volume_put( AD_VOLUME *, DBPROCESS * );
int	ad_onefile_media( DBPROCESS *, char *, char *, AD_MEDIA_MASK * );
int	ad_remote_file_put( AD_FILE *, DBPROCESS * );
int	ad_remote_volume_put( AD_VOLUME *, DBPROCESS * );
int	ad_tp_file_get( AD_FILEGET, DBPROCESS *, va_list );
int	ad_tp_file_put( AD_FILE *, DBPROCESS * );
int	ad_tp_volume_get( AD_VOLGET, DBPROCESS *, va_list );
int	ad_tp_volume_put( AD_VOLUME *, DBPROCESS * );


/*
 *  Define the globals local to the library.
 */

extern	boolean		ad_init_flag;
extern	LOCAL_AD	local_ad;
