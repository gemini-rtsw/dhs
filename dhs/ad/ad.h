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
*   Include File Name:	ad/h/ad.h
*
*   Purpose:
*	Contains the global definitions for the ad library.
*
*   Date		: Nov 22, 1996.
*
*
*   Field SCCS data	: @(#)
*	Module Name	: ad.h
*	Version Number	: 1.20
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/04/01 SEC  : adDbInfoGet() now returns db server too. 
*	97/04/15 GJF  : Changed AD_PATH_LEN to reflect the db.
*	97/06/20 SEC  :	Added AD_E_COMP and AD_E_FMT error statusses. 
*			Added AD_COMP_* compression types. Added AD_FMT_*
*			format types, and string lengths. Also added 
*			lengths for new tables. Mod'ed filename to file_id
*			Removed server_name and host_name. Added proto's
*			for ad_default_archive() and ad_file_info().
*	97/07/03 SEC  :	Added C++ cleanliness #ifdef.
*	97/08/25 SEC  :	Added AD_E_NO_REMOTE and AD_E_SITE_ID error statusses.
*			Added lengths & typedefs for new table fields. 
*			Mod'ed RS_LOCATION def'n, and added prototype for
*			ad_server_id_get().
*	97/09/10 pdd  : Removed AD_FILENAME and AD_FILENAME_LEN definitions
*	97/11/04 sjg  : Modified second arg of ad_file_info to char *.
*	98/02/13 SJG  : Added ad_file_name function.
*	98/03/10 gzg  : Added features to handle tape tp.
*	98/04/23 sjg  : Added script field to tps table.
*	98/06/23 sjg  : Added ad_archive_info function and AD_ARCHIVE
*			structure.
*	99/02/04 SDJ  : Removed unused defines: AD_E_ARCHIVE,
*			AD_E_CONFIG_VERIFY, and AD_E_GET_NEXT.
*	99/05/19 sjg  : Changed ada_upper to tri-state case conversion.
*     2000/03/24 SDJ  : Changed AD_MEDIUM_CONFIG struct slightly.
*			Switched all vd (dvd) to mfs (mountable file
*			system.  Added various structures and defines
*			to support the mfs tables.
*     2000/05/10 NRH  : Added function ad_file_media and the structures
*			needed to support it.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Status and message codes.
 */

#define	AD_CFILE_OPEN		3	/* Opening configuration file.	*/
#define	AD_NO_FILE		2	/* No file available.		*/
#define	AD_NO_VOLUME		1	/* No volume available.		*/

#define	AD_SUCCESS		0	/* Normal completion.		*/

#define	AD_E_CANNOT_DEL		(-1)	/* Cannot delete from medium.	*/
#define	AD_E_COMP		(-2)	/* Error in comp config info.	*/
#define	AD_E_CONFIG		(-3)	/* Error reading config file.	*/
#define	AD_E_DB			(-4)	/* Error in the db library.	*/
#define	AD_E_DB_LIB		(-5)	/* Error in the cadc db library.*/
#define	AD_E_FM_TYPE_ENUM	(-6)	/* Invalid file media type.	*/
#define	AD_E_FMT		(-7)	/* Error in format config info.	*/
#define	AD_E_INIT		(-8)	/* Library not initialized.	*/
#define	AD_E_MEDIUM_ENUM	(-9)	/* Medium enumerated value ng.	*/
#define	AD_E_MEDIUM_TYPE	(-10)	/* Medium type is invalid.	*/
#define	AD_E_MEMORY		(-11)	/* Error during a system call.	*/
#define AD_E_MFS_VOL_INFO	(-12)	/* Invalid mfs type/format pair.*/
#define AD_E_NO_REMOTE		(-13)	/* No remote tables in use.	*/
#define AD_E_SITE_ID		(-14)	/* Site ID incorrect or missing.*/
#define	AD_E_SELECT_TYPE	(-15)	/* Select type invalid.		*/


/*
 *  Define the moutable media types
 */

#define AD_MFS_CD	"CD"
#define AD_MFS_DVD	"DVD"
#define AD_MFS_MD	"MD"


/*
 *  Define the various file system formats expected on various media..
 */

#define	AD_ISO9660	"ISO9660"
#define	AD_UDF		"UDF"
#define AD_UFS		"UFS"


/*
 *  Define the medium name strings.
 */

#define	AD_MEDNAME_CD	"CD"
#define AD_MEDNAME_DB	"DB"
#define AD_MEDNAME_MFS	"MFS"
#define AD_MEDNAME_MD	"MD"
#define AD_MEDNAME_OD	"OD"
#define AD_MEDNAME_TP	"TP"
#define AD_MEDNAME_UNK	"UM"


/*
 *  Define the compression types. 
 */

#define AD_COMP_GZIP	'G'
#define AD_COMP_CF	'C'
#define AD_COMP_UNKNOWN	' '
#define AD_COMP_UNIX	'U'


/*
 *  Define the file format types. 
 */

#define AD_FMT_FITS	'F'
#define AD_FMT_GSD	'G'
#define AD_FMT_RAW	'R'
#define AD_FMT_SDS	'S'
#define AD_FMT_SDF	'D'
#define AD_FMT_UNKNOWN	' '


/* 
 *  Define the values for the files table status values.
 */

#define	AD_FILE_CURRENT		'C'	/* File is the best file.	*/
#define	AD_FILE_OBSOLETE	'I'	/* File is obsolete.		*/
#define	AD_FILE_ERROR		'E'	/* File cannot be correctly read*/

#define	AD_NOT_VERIFIED		' '
#define	AD_VERIFIED		'Y'


/*
 *  Define the field sizes.
 */

#define	AD_ARCHNAME_LEN		6
#define	AD_COMP_EXT_LEN		5
#define	AD_FILE_ID_LEN		45
#define	AD_FMT_EXT_LEN		5
#define AD_HOSTNAME_LEN		32
#define	AD_MEDNAME_LEN		10
#define AD_MEDIA_ID_LEN		32	/* Not Used Anywhere		*/
#define	AD_PATH_LEN		223
#define	AD_SERVER_ID_LEN	32	/* Not Used Anywhere		*/
#define	AD_SITE_ID_LEN		3
#define	AD_SYBASENAME_LEN	30
#define AD_TP_LOCATION_LEN	32	/* Not Used Anywhere		*/
#define	AD_VOLUMENAME_LEN	32
#define	AD_VOLUMESCRIPT_LEN	10	/* Not Used Anywhere		*/
#define	AD_VOLUMETYPE_LEN	10	/* Not Used Anywhere		*/
#define	AD_VOLUMEFORMAT_LEN	10	/* Not Used Anywhere		*/


/*
 *  Define unknown file size. 
 */

#define AD_FILESIZE_UNKNOWN	-1



/*
 * Define unkown CRC
 */

#define AD_CRC_UNKNOWN		0


/*
 *  The following macros define bit positions for different
 *  media to be used in an archive's complete and incomplete
 *  fields.  The are not used in any ad tables.
 */

#define	AD_MEDIA_DVD	0x0001
#define	AD_MEDIA_CD	0x0002
#define	AD_MEDIA_MD	0x0004
#define	AD_MEDIA_DB	0x0008
#define	AD_MEDIA_EXA	0x0010
#define	AD_MEDIA_DLT	0x0020
#define	AD_MEDIA_OD	0x0040


/*
 *  The following #ifdef makes the interface to C++ cleaner. 
 */

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  Define the low level types used in the archive structures.
 */

typedef	enum
{
    AD_CASE_LOWER,
    AD_CASE_NONE,
    AD_CASE_UPPER
} AD_CASE;
 
typedef	enum	
{ 
    AD_MED_CD, 
    AD_MED_DB,
    AD_MED_MD, 
    AD_MED_MFS,
    AD_MED_OD, 
    AD_MED_REMOTE,
    AD_MED_TP, 
    AD_MED_BEST,
    AD_MED_UNKNOWN
} AD_MEDIUM;

typedef enum
{ 
    AD_FG_LOCATION, 
    AD_FG_NAME, 
    AD_FG_NEXT,
    AD_FG_VOLUME 
} AD_FILEGET;

typedef enum
{ 
    AD_RG_FILE_ID,
    AD_RG_NEXT
} AD_REMOTEGET;

typedef enum
{ 
    AD_VG_ARCHIVE,
    AD_VG_CRC,
    AD_VG_LAST,
    AD_VG_NAME, 
    AD_VG_NAME_ORIG,
    AD_VG_NEXT 
} AD_VOLGET;


/*
 * Type to determine if function ad_file_media is being passed a single 
 * file or an EPTR list of files.
 */

typedef enum
{ 
    AD_FM_FILE, 
    AD_FM_EPTR_LIST 
} AD_FILEMEDIA;



typedef	char	AD_ARCHNAME[ AD_ARCHNAME_LEN + 1 ];
typedef	char	AD_COMP_EXT[ AD_COMP_EXT_LEN + 1 ];
typedef	char	AD_FILE_ID[ AD_FILE_ID_LEN + 1 ];
typedef	char	AD_FMT_EXT[ AD_FMT_EXT_LEN + 1 ];
typedef char	AD_HOSTNAME[ AD_HOSTNAME_LEN + 1];
typedef	char	AD_MEDNAME[ AD_MEDNAME_LEN + 1 ];
typedef char	AD_MEDIA_ID[ AD_MEDIA_ID_LEN + 1 ];
typedef unsigned short 
		AD_MEDIA_MASK;
typedef	char	AD_PATH[ AD_PATH_LEN + 1 ];
typedef	char	AD_SERVER_ID[ AD_SERVER_ID_LEN + 1 ];
typedef	char	AD_SITE_ID[ AD_SITE_ID_LEN + 1 ];
typedef char	AD_SYBNAME[ AD_SYBASENAME_LEN + 1 ];
typedef char	AD_TP_LOCATION[ AD_TP_LOCATION_LEN + 1 ];
typedef unsigned long
		AD_VOLCRC;
typedef char	AD_VOLNAME[ AD_VOLUMENAME_LEN + 1 ];
typedef char	AD_VOLSCRIPT[ AD_VOLUMESCRIPT_LEN + 1 ];
typedef char	AD_VOLTYPE[ AD_VOLUMETYPE_LEN + 1 ];
typedef char	AD_VOLFORMAT[ AD_VOLUMEFORMAT_LEN + 1 ];




/*
 *  Type containing the file location on cd.
 */

typedef struct cd_location
{
    AD_PATH	cdl_filename;
    AD_PATH	cdl_mount_point;
} CD_LOCATION;


/*
 *  Type containing the file location on a mountable file system.
 */

typedef struct mfs_location
{
    AD_PATH	fsl_filename;
    AD_HOSTNAME	fsl_hostname;
    AD_PATH	fsl_mount_point;
} MFS_LOCATION;


/*
 *  Type containing the file location in the database.
 */

typedef struct db_location
{
    AD_SYBNAME		dbl_database_name;
    AD_SYBNAME		dbl_table_name;
    AD_PATH		dbl_filename;
} DB_LOCATION;


/*
 *  Type containing the file location on magnetic disk.
 */

typedef struct md_location
{
    AD_PATH	mdl_filename;
    AD_PATH	mdl_mount_point;
} MD_LOCATION;


/*
 *  Type containing the file location on optical disk.
 */

typedef struct od_location
{
    int		odl_dirsectoff;
    AD_PATH	odl_filename;
} OD_LOCATION;


/*
 *  Type containing the file location on tape. 
 */

typedef struct tp_location
{
    AD_PATH		tpl_filename;
    AD_TP_LOCATION	tpl_location;	/* The offset of the tape.	*/
} TP_LOCATION;


/*
 *  Type containing the file location on a remote server.
 */

typedef struct rs_location
{
    AD_SERVER_ID	rsl_server_id;	/* IMP ident. of file's server.	*/
} RS_LOCATION;


typedef union ad_location
{
    CD_LOCATION	cd;			/* Location on cd.		*/
    MFS_LOCATION mfs;			/* Location on dvd.		*/
    DB_LOCATION	db;			/* Location on db.		*/
    MD_LOCATION	md;			/* Location on md.		*/
    OD_LOCATION od;			/* Location on od.		*/
    TP_LOCATION tp;			/* Location on tp.		*/
    RS_LOCATION rs;			/* Location on remote server.	*/
} AD_LOCATION;


/*
 *  Type for describing a file in the archive.
 */

typedef struct ad_file
{
    AD_VOLCRC	adf_crc;		/* Volumd CRC file belongs to.	*/
    AD_ARCHNAME	adf_archive;		/* Archive the file belongs to.	*/
    char	adf_compression;	/* Compression format.		*/
    AD_FILE_ID	adf_file_id;		/* File identifier. 		*/
    int		adf_filesize;		/* File size.			*/
    int		adf_filesize_uncomp;	/* Uncompressed file size.	*/
    char	adf_format;		/* File format.			*/
    AD_LOCATION	adf_location;		/* Location on the volume.	*/
    boolean	adf_mounted;		/* Is the volume mounted.	*/
    char	adf_status;		/* Current file status.		*/
    AD_VOLNAME	adf_volume_name;	/* Volume name.			*/
    AD_MEDIUM	adf_volume_medium;	/* Medium of volume.		*/
    AD_VOLTYPE	adf_volume_type;	/* Volume type.			*/
    AD_VOLFORMAT adf_volume_format;	/* Volume file system format.	*/
} AD_FILE;


/*
 *  Cd specific information about the volume.
 */

typedef struct cd_vol_info
{
    long	cdv_creation_date;	/* Unix time of creation.	*/
    AD_VOLNAME	cdv_diskname_orig;	/* Original disk name.		*/
    long	cdv_ingest_date;	/* Unix time of ingest.		*/
    AD_PATH	cdv_mount_point;	/* File system mount point?	*/
    int		cdv_nfiles;		/* Number of files on volume.	*/
    int		cdv_nkbytes;		/* Number of kbytes on volume.	*/
    AD_VOLTYPE	cdv_type;		/* Type of cdrom.		*/
    char	cdv_verified;		/* Is the volume verified?	*/
} CD_VOL_INFO;


/*
 *  Mountable file system specific information about the volume.
 */

typedef struct mfs_vol_info
{
    long	fsv_creation_date;	/* Unix time of creation.	*/
    long	fsv_ingest_date;	/* Unix time of ingest.		*/
    AD_HOSTNAME	fsv_hostname;		/* Host where volume resides.	*/
    AD_PATH	fsv_mount_point;	/* File system mount point?	*/
    int		fsv_nfiles;		/* Number of files on volume.	*/
    int		fsv_nkbytes;		/* Number of kbytes on volume.	*/
    AD_VOLTYPE	fsv_type;		/* Volume's media type.		*/
    AD_VOLFORMAT fsv_format;		/* File system format on vol.	*/
    char	fsv_verified;		/* Is the volume verified?	*/
} MFS_VOL_INFO;


/* 
 *  Db specific information about the volume.
 */

typedef struct db_vol_info
{
    AD_SYBNAME	dbv_database_name;	/* Database where data is.	*/
    AD_SYBNAME	dbv_table_name;		/* Table where data is.		*/
} DB_VOL_INFO;


/*
 *  Md specific information about the volume.
 */

typedef struct md_vol_info
{
    AD_PATH	mdv_mount_point;	/* File system mount point?	*/
} MD_VOL_INFO;


/*
 *  Od specific  information about the volume.
 */

typedef struct od_vol_info
{
    long	odv_creation_date;	/* Unix time of creation.	*/
    long	odv_dirsectoff;		/* Sector of directory.		*/
    AD_VOLNAME	odv_diskname_orig;	/* Original disk name.		*/
    long	odv_ingest_date;	/* Unix time of ingest.		*/
    int		odv_nfiles;		/* Number of files on volume.	*/
    int		odv_nkbytes;		/* Number of kbytes on volume.	*/
    AD_VOLNAME	odv_reverse_name;	/* Name of the flip side.	*/
    AD_VOLTYPE	odv_type;		/* Type of optical disk.	*/
    char	odv_verified;		/* Is the volume verified?	*/
} OD_VOL_INFO;


typedef struct tp_vol_info
{
    long	tpv_ingest_date;	/* Unix time of ingest.		*/
    int		tpv_nfiles;		/* Number of files on volume.	*/
    int		tpv_nkbytes;		/* Number of kbytes on volume.	*/
    AD_VOLTYPE	tpv_type;		/* Type of tape.		*/
    AD_VOLSCRIPT tpv_tp_script;		/* Type of tp scripts to use.	*/
    char	tpv_verified;		/* Is the volume verified?	*/
} TP_VOL_INFO;


typedef union ad_vol_info
{
    CD_VOL_INFO	cd;			/* Cd volume information.	*/
    MFS_VOL_INFO mfs;			/* Mountable volume info.	*/
    DB_VOL_INFO	db;			/* Db volume information.	*/
    MD_VOL_INFO	md;			/* Md volume information.	*/
    OD_VOL_INFO	od;			/* Od volume information.	*/
    TP_VOL_INFO	tp;			/* TP volume information.	*/
} AD_VOL_INFO;
   

/*
 *  Type for describing a volume in the archive.
 */

typedef struct ad_volume
{
    AD_ARCHNAME	adv_archive;
    AD_VOLCRC	adv_crc;		/* Unique CRC for the volume.	*/
    AD_MEDIUM	adv_medium_type;	/* Volume type.			*/
    boolean	adv_mounted;		/* Is the file system mounted?	*/
    AD_VOL_INFO	adv_vol_info;		/* Medium specifiec volume info.*/
    AD_VOLNAME	adv_volume_name;	/* Name assigned to the volume	*/	
} AD_VOLUME;


/*
 *  Type for describing an archive.
 */

typedef struct ad_archive
{
    AD_ARCHNAME	ada_name;		/* Name of archive.		*/
    AD_CASE	ada_case;		/* Case conversion?		*/
} AD_ARCHIVE;


/*
 *  Function prototypes and external declarations.
 */

extern	AD_ARCHIVE
		*ad_archive_info( AD_ARCHNAME );
extern	void	ad_close( void );
extern	int	ad_dbinfo_get( AD_MEDIUM, char **, char **, char **, char ** );
#if defined(SYBASE_DHS)
extern	int	ad_file_del( struct dbprocess *, AD_MEDIUM, char *, ... );
extern	int	ad_file_get( AD_FILEGET, AD_FILE *, struct dbprocess *, ... );
#endif
extern	int	ad_file_info( char *, char *, char *, char * );
extern	int	ad_file_init( AD_FILE *, AD_MEDIUM );
#if defined(SYBASE_DHS)
extern	int	ad_file_media( struct dbprocess *, AD_FILEMEDIA, char *, 
			AD_MEDIA_MASK *, AD_MEDIA_MASK *, boolean *, ... );
#endif
extern	char	*ad_file_name( AD_FILE *, char * );
#if defined(SYBASE_DHS)
extern	int	ad_file_put( AD_FILE *, struct dbprocess * );
extern	int	ad_server_id_get( struct dbprocess *, char ** );
#endif
extern	int	ad_init( void );
extern	char	*ad_med_name( AD_MEDIUM );
extern  AD_MEDIUM
		ad_med_type( char * );
#if defined(SYBASE_DHS)
extern	int	ad_remote_get( AD_REMOTEGET, AD_FILE *, struct dbprocess *, char * );
extern	int	ad_volume_get( AD_VOLGET, AD_VOLUME *, 
			struct dbprocess *, ... );
#endif
extern	int	ad_volume_init( AD_VOLUME *, AD_MEDIUM );
#if defined(SYBASE_DHS)
extern	int	ad_volume_put( AD_VOLUME *, struct dbprocess * );
#endif
extern	boolean	ad_volume_verify( AD_VOLUME * );
extern	char	ad_msg[];


/*
 *  Ending the C++ compatibility from above.
 */

#ifdef    __cplusplus
}
#endif

