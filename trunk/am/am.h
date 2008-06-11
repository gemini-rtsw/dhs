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
*   Include File Name:	am/h/am.h
*
*   Purpose:
*	The am library external definitions.
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: am.h
*	Version Number	: 1.20
*	Release Number	: 1
*	Last Updated	: 08/22/00
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  :	Modifications made to support new 'Storage
*			Server' for the gemini project.
*	80/04/20 SEC  :	Modifications made to return float from 
*			amAmountRetrievable() and amAmountQueueable().
*	98/09/03 JSD  :	Added device booking table information.
*	98/10/20 SDJ  : Added support for split size and unit name
*			precision.  Also made threshold and capacities
*			consistent.
*	98/11/23 JSD  :	Added AM_E_TYPEGET_ENUM, moved split size to
*			AM_MED_TYPE, added AMT_TYPE_GET enum's, changed
*			def. of amMediaTypeGet, increased size of
*			AM_MEDIA_TYPE_LEN to ?.
*	98/20/01 SDJ  : Added new functions, types, ... to support
*			device tables.
*       99/02/23 SDJ  : Added AM_E_UNITNAME message.
*       99/03/05 SDJ  : Added function amFileDelete. Added some search types
*			to amFileGet.
*	99/03/14 SDJ  : Changed the "length" type def's to be one greater
*		  	than the size given in the sql scripts.
*	99/03/18 NRH  : Added function amNumOnUnit.
*	99/04/28 NRH  : Added functions amActionAdd and amActionDelete,
*			and a set of symbols listing allowed actions.
*	99/05/21 NRH  : Added function amLogicalDeviceGet and structure 
*			AM_LOGICAL_DEVICE.
*	99/05/26 NRH  : Added some new error messages. Changed some function
*			arguments to const values.
*	99/06/16 NRH  : Added amLogicalDevice structure. Added function
*			amLogicalDeviceGet.
*	99/10/12 NRH  : Added AM_FG_UNIQUE to the AM_FILE_GET enumerated type.
*			Added AM_MG_REUSE to the AM_MEDIA_GET enumerated type.
*			Changed the mediaFile amfCompressionFlag value to
*			a character.
*     2000/02/03 SDJ  : Added amMediaOptionsGet() and amMediaOptionsPut().
*			Added AM_E_MEDIA_OPTION_GET return status,
*			AM_MEDIA_OPTION_LEN, and AM_MEDIA_OPTIONS structure
*     2000/02/14 SDJ  : Added the lovely amfDayNotified to the AM file struct.
*     2000/08/22 NRH  : Added AMF_COMP_ASIS macro.
*			Made the am_file argument to function amFileDelete
*			"const".
*			
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 * 		 	Return codes.
 ************************************************************************
 */

#define	AM_CANT_MSBOOK		10	/* Can't 'book' with amMSBook().*/
#define	AM_CFILE_OPEN		9	/* Opening config file message.	*/
#define	AM_NO_DEVICE		8	/* No matching device.		*/
#define	AM_NO_FILE		7	/* No matching file.		*/
#define AM_NO_LOGICAL_DEV	6	/* No logical device found.	*/
#define	AM_NO_MEDIA		5	/* No matching media requests.	*/
#define	AM_NO_MEDIA_OPTIONS	4	/* No matching media options.	*/
#define AM_NO_MSTAGE		3	/* No matching media stage.	*/
#define	AM_NO_TYPE		2	/* No media type match.		*/
#define	AM_NO_UNITS		1	/* No matching options.		*/

#define	AM_SUCCESS		0	/* Successful execution.	*/

#define AM_E_BOOKCLEAR_ENUM	(-1)	/* Invalid clear dev. type.	*/
#define	AM_E_CONFIG		(-2)	/* Error in config file.	*/
#define	AM_E_DB			(-3)	/* Database error.		*/
#define	AM_E_DB_LIB		(-4)	/* Database library error.	*/
#define	AM_E_DEVICEGET_ENUM	(-5)	/* Invalid device get type.	*/
#define	AM_E_FILEGET_ENUM	(-6)	/* Invalid file get type.	*/
#define	AM_E_FILEUPDATE_ENUM	(-7)	/* Invalid file update type.	*/
#define	AM_E_INIT		(-8)	/* am library not initialized.	*/
#define AM_E_LOG_DEVGET_ENUM	(-9)	/* Invalid logical device get.	*/
#define AM_E_MEDIA_OPTION_GET	(-10)	/* Error getting media options.	*/
#define	AM_E_MEDIAGET_ENUM	(-11)	/* Invalid amGet type.		*/
#define	AM_E_MEMORY		(-12)	/* Memory allocation failure.	*/
#define	AM_E_MSTAGE_DIR		(-13)	/* Invalid staging directory.	*/
#define AM_E_NOT_BOOKED		(-14)	/* Unable to book a device.	*/
#define	AM_E_SCRIPT_NF		(-15)	/* Script name not found.	*/
#define	AM_E_STATUS		(-16)	/* Error finding status string.	*/
#define	AM_E_TYPEGET_ENUM	(-17)	/* Invalid media type get type.	*/
#define AM_E_UNITNAME		(-18)	/* Invalid unit name.		*/
#define	AM_E_UNITSGET_ENUM	(-19)	/* Invalid units get type.	*/
#define	AM_E_UNITUPDATE_ENUM	(-20)	/* Invalid unit update type.	*/


/*
 ************************************************************************
 *  			     Length constants.
 ************************************************************************
 */

#define	AM_FILE_ID_LEN			46
#define	AM_FILE_NAME_LEN		224
#define AM_MEDIA_OPTION_LEN		256
#define	AM_MEDIA_REQUEST_NAME_LEN	129
#define AM_MEDIA_REQUEST_TYPE_LEN	6
#define AM_MEDIA_TYPE_LEN		11
#define AM_MEDIA_TYPE_DESC_LEN		51
#define	AM_MEDIA_UNIT_NAME_LEN		33
#define	AM_MSTAGE_DIR_LEN		33
#define	AM_SOURCE_ID_LEN		33
#define	AM_STATUS_SHORT_LEN		46
#define	AM_UNIT_NAME_ROOT_LEN		33
#define	AM_DEVICE_NAME_LEN		33
#define AM_DRIVER_NAME_LEN		33
#define AM_DEV_INFO_LEN			224
#define AM_LOGICAL_DEV_NAME_LEN		33
#define AM_PROC_NAME_LEN		256


/*
 ************************************************************************
 *  			     Status Contants
 ************************************************************************
 */

/*
 *  Status in the media table.
 */
 
#define AMM_CREATING		' '	/* Creating User Media Request.	*/
					/*     (Initial state)		*/
#define AMM_DONE		'D'	/* Done. (Final state)		*/
#define AMM_ERROR		'E'	/* Error in User Media Request.	*/
#define AMM_IN_PROG		'I'	/* In Progress.			*/

 
/*
 *  Status in the media_units table.
 */

#define AMU_CLEANED_UP		'C'	/* Media stage is cleaned up.	*/
#define AMU_DONE		'D'	/* Done. (Final state)		*/
#define AMU_PREPARED		'P'	/* Media unit is prepared.	*/
#define AMU_QUEUED		'Q'	/* Media unit is queued.	*/
#define AMU_QUEUEING		' '	/* Media unit is being queued.	*/
					/*     (Initial state)		*/
#define AMU_WRITTEN		'W'	/* All copies of unit have been */
					/*     written.			*/
 

/*
 *  Status in the media_files table.
 */
 
#define AMF_DONE		'D'	/* Done. (Final state)		*/
#define AMF_ERROR		'E'	/* Error retrieving. 		*/
#define AMF_RETRIEVED		'R'	/* Retrieve to Media Stage	*/
#define AMF_UNAVAILABLE		'U'	/* Unavailable.			*/
#define AMF_UNPROCESSED		' '	/* Unprocessed. (Initial state)	*/


/*
 *  Compression flag in the media_files table.
 */

#define	AMF_COMP_ASIS		'A'	/* Don't change compression state.*/
#define AMF_COMP_GZIP		'G'	/* Compress with gzip.		*/
#define AMF_COMP_UNIX		'U'	/* Compress with unix compress.	*/
#define	AMF_COMP_NONE		' '	/* No compression.		*/


/*
 *  Define Media Request Types. 
 */

#define AM_RT_ARCHIVE		"ARCH"	/* Archive Media Request.	*/
#define AM_RT_USER		"USER"	/* User Media Request.		*/


/*
 *  Define the media action types.
 */

#define	AM_MA_INGEST		"I"	/* Ingest.			*/
#define AM_MA_UNINGEST		"IR"	/* Ingest rollback.		*/
#define	AM_MA_QUEUE		"Q"	/* Queue.			*/
#define AM_MA_UNQUEUE		"QR"	/* Queue rollback.		*/
#define	AM_MA_PREPARE		"P"	/* Prepare.			*/
#define AM_MA_UNPREPARE		"PR"	/* Prepare rollback.		*/
#define	AM_MA_WRITE		"W"	/* Write.			*/
#define AM_MA_UNWRITE		"WR"	/* Write rollback.		*/
#define	AM_MA_CLEANUP		"C"	/* Cleanup.			*/
#define AM_MA_UNCLEANUP		"CR"	/* Cleanup rollback.		*/


/*
 *  Special source id definition for internal requests which do not
 *  require storageServer, mediaRetrieve, or mediaNotify. 
 */

#define	AM_SRC_INTERNAL		""
 
/*
 *  This value in the ammUnitNamePrecision field of the amMedia structure
 *  indicates the default precision should be used.
 */

#define	AM_DEFAULT_PRECISION		-1


/*
 ************************************************************************
 *  			     Enumerated types.
 ************************************************************************
 */

/*
 *  Device Clear Types
 */

typedef enum
{ 
    AM_DC_ALL,
    AM_DC_LOGICAL,
    AM_DC_DEVICE
} AM_DEVICE_CLEAR;


/*
 *  Device Get Types
 */

typedef enum
{ 
    AM_DG_ALL,
    AM_DG_NAME,
    AM_DG_NAME_TYPE,
    AM_DG_NEXT,
    AM_DG_TYPE
} AM_DEVICE_GET;


/*
 *  Logical device Get Types
 */

typedef enum
{ 
    AM_LDG_ALL,
    AM_LDG_DNAME,
    AM_LDG_NEXT
} AM_LOGICAL_DEVICE_GET;


/*
 *  Media File Get Types
 */

typedef enum
{ 
    AM_FG_FILE_ID,
    AM_FG_FNAME,
    AM_FG_MEDIA_FILE,
    AM_FG_MEDIA_ID_FNAME,
    AM_FG_MEDIA_REQUEST_ID,
    AM_FG_MEDIA_UNIT_ID,
    AM_FG_MEDIA_UNIT,
    AM_FG_NEXT,
    AM_FG_QUEUEABLE,
    AM_FG_RETRIEVABLE,
    AM_FG_SOURCE_ID,
    AM_FG_STATUS,
    AM_FG_UNIT_FNAME,
    AM_FG_UNWRITTEN,
    AM_FG_UNIQUE
} AM_FILE_GET;


/*
 *  Media File Update Types
 */

typedef enum
{ 
    AM_FU_COMPLETE_ARCH,
    AM_FU_COMPLETE_USER,
    AM_FU_MEDIA_Q_ROLLBACK,
    AM_FU_WRITTEN
} AM_FILE_UPDATE;


/*
 *  Media table Get Types
 */

typedef enum
{ 
    AM_MG_ALL,
    AM_MG_MEDIA_REQUEST_ID,
    AM_MG_NOTIFIABLE,
    AM_MG_NEXT,
    AM_MG_MEDIA_REQUEST_NAME,
    AM_MG_SOURCE_ID,
    AM_MG_STATUS,
    AM_MG_REUSE
} AM_MEDIA_GET;


/*
 *  Media Stage Get Types
 */

typedef enum
{
    AM_SG_ALL,
    AM_SG_DIRECTORY,
    AM_SG_NEXT,
    AM_SG_REQUEST_ID_NAME
} AM_STAGE_GET;
    

/*
 *  Script Types
 */

typedef enum
{ 
    AM_ST_PREP,
    AM_ST_WRITE
} AM_SCRIPT_TYPE;


/*
 *  MediaTypes Table Get Types
 */

typedef enum
{ 
    AM_TG_ALL,
    AM_TG_NEXT,
    AM_TG_TYPE
} AM_TYPE_GET;


/*
 *  Media Unit Get Types
 */

typedef enum
{ 
    AM_UG_MEDIA_REQUEST_ID_LAST,
    AM_UG_MEDIA_REQUEST_ID_NAME,
    AM_UG_MEDIA_REQUEST_ID,
    AM_UG_MEDIA_UNIT_NAME,
    AM_UG_NEXT,
    AM_UG_NOTIFIABLE,
    AM_UG_STATUS_AFTER_NAME,
    AM_UG_STATUS
} AM_UNIT_GET;


/*
 *  Media Unit Update Types.
 */

typedef enum
{
    AM_UU_COMPLETE_USER
} AM_UNIT_UPDATE;


/*
 ************************************************************************
 *  			     Structures
 ************************************************************************
 */


/*
 *  Media table record structure.
 */

typedef struct amMedia
{
    time_t	ammDateCompleted;		/* Date completed.	*/
    time_t	ammDateRequested;		/* Date requested.	*/
    int		ammMediaRequestId;		/* Media request ID.	*/
    char	ammMediaRequestName[AM_MEDIA_REQUEST_NAME_LEN];
						/* Media request name.	*/
    char	ammMediaRequestType[AM_MEDIA_REQUEST_TYPE_LEN];	
						/* Type of request.	*/
    char	ammMediaType[AM_MEDIA_TYPE_LEN];
    						/* Type of media.	*/
    int		ammNumCopies;			/* # of copies to make.	*/
    char	ammUnitNameRoot[AM_UNIT_NAME_ROOT_LEN];	
    						/* Root of unit names.	*/
    int		ammUnitNamePrecision;		/* # of digits appended */
    						/* to the Unit Name.	*/
    char	ammSourceId[AM_SOURCE_ID_LEN];	/* ID of source of req.	*/
    char	ammStatus;			/* Status in media tbl.	*/
} AM_MEDIA;


/*
 *  media_units table record structure.
 */

typedef struct amUnit
{
    int		amuCopiesWritten;		/* Num copies written.	*/
    time_t	amuDateCompleted;		/* Date completed.	*/
    int		amuMediaRequestId;		/* Media request id.	*/
    char	amuMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    						/* Media unit name.	*/
    char	amuStatus;			/* Status.	 	*/
    int		amuTotalSize;			/* Total size.		*/
    int		amuUncompSize;			/* Uncompressed size.	*/
} AM_UNIT;


/*
 *  media_files table record structure.
 */

typedef struct amFile
{
    char	amfCompressionFlag;		/* Compression flag.	*/
    time_t	amfDateNotified;		/* Date notified 	*/
    time_t	amfDayNotified;			/* Day of notification.	*/
    char	amfFileId[AM_FILE_ID_LEN];	/* File id.		*/
    int		amfFileSize;			/* File size (Actual).	*/
    char	amfFilename[AM_FILE_NAME_LEN];	/* File name.		*/
    int		amfMediaRequestId;		/* Media request id.	*/
    char	amfMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
						/* Media unit name.	*/
    char	amfSourceId[AM_SOURCE_ID_LEN];	/* Source id.		*/
    char	amfStatus;			/* Status 		*/
    int		amfUncompFileSize;		/* Uncompressed file size.*/
} AM_FILE;


/*
 *  Media Data Stage record structure.
 */

typedef struct amMedStage
{
    char	amsDirectory[AM_MSTAGE_DIR_LEN];/* Media stage dir name.*/
    int		amsMediaRequestId;		/* Media request id.	*/
    char	amsMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    						/* Media unit name.	*/
} AM_MSTAGE;


/*
 *  Media scripts record structure.
 */

typedef struct amMedScripts
{
    char	*amsPrepScript;		/* Preparation script name.	*/
    char	*amsPrepFile;		/* Preparation file name.	*/
    char	*amsWriteScript;	/* Write script name.		*/
    char	*amsWriteFile;		/* Write file name.		*/
    char        amsTypeName[AM_MEDIA_TYPE_LEN];
					/* Name of Media Type.          */
} AM_MED_SCRIPT;


/*
 *  Media type record structure.
 */

typedef struct amMediaType
{
    int         amtBlockSize;		/* Min. block size of Med. Type.*/
    int		amtDirOverh;		/* Directory overhead.		*/
    int		amtFileOverh;		/* File overhead.		*/
    int		amtMaxSize;		/* Maximum Size of Media Type.  */
    int		amtSplitSize;		/* Min. split size.		*/
    float	amtThreshold;		/* Full Threshold of Media Type.*/
    char        amtTypeName[AM_MEDIA_TYPE_LEN];
					/* Name of Media Type.          */
} AM_MED_TYPE;


/*
 *  Device record structure.
 */

typedef struct amDevice
{
    char	amdDeviceName[AM_DEVICE_NAME_LEN];
    					/* Device name.			*/
    char	amdDeviceDriver[AM_DRIVER_NAME_LEN];
    					/* Device driver.		*/
    char	amdDeviceInfo[AM_DEV_INFO_LEN];
    					/* Device information.		*/
    char	amdType[AM_MEDIA_TYPE_LEN];
    					/* Type of device.		*/
    int		amdRSpeed;		/* Read speed.			*/
    int		amdWSpeed;		/* Write speed.			*/
    boolean	amdOnline;		/* TRUE if device is online.	*/
    boolean	amdInUse;		/* TRUE means in use.		*/
    pid_t	amdProcessId;		/* Process ID using device.	*/
    char	amdProcessName[AM_PROC_NAME_LEN];
					/* Name of process using dev.	*/
    int		amdMediaRequestId;	/* Media request ID.		*/
    char	amdMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    					/* Media unit name.		*/
    time_t	amdTimeStamp;		/* Time when booked.		*/
} AM_DEVICE;


/*
 *  Logical Device record structure.
 */

typedef struct amLogicalDevice
{
    boolean	amldAvailable;		/* Is the logical device available?*/
    int		amldCapacity;		/* Capacity of the device in 	*/
    					/* kBytes.			*/
    boolean	amldInUse;		/* Is the logical device in use?*/
    int		amldMediaRequestId;	/* Media request ID that booked	*/
    					/* this device.		*/
    char	amldMediaType[AM_MEDIA_TYPE_LEN];
					/* One media type supported by	*/
					/* the device.			*/
    char	amldMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    					/* Media unit name that booked	*/
					/* this device.			*/
    char	amldMediaTypeDesc[AM_MEDIA_TYPE_DESC_LEN];
    char	amldName[AM_LOGICAL_DEV_NAME_LEN];
    					/* Logical Device name.		*/
    int		amldNumDevices;		/* Number of physical devices	*/
    					/* Associated with the logical	*/						/* device.			*/
} AM_LOGICAL_DEVICE;


typedef struct amMediaOptions 
{
    int		amoMediaRequestId;	/* Media request ID.		*/
    char	amoProcessName[AM_PROC_NAME_LEN];
					/* Name of process using dev.	*/
    char	amoOptions[AM_MEDIA_OPTION_LEN];
} AM_MEDIA_OPTIONS;


/*
 ************************************************************************
 * 		     Define the global symbols.
 ************************************************************************
 */

extern	char	amMsg[];


/*
 ************************************************************************
 * 		    Global function prototypes
 ************************************************************************
 */

extern	int	amActionAdd( struct dbprocess *, int, char *, char * );
extern	int	amActionDelete( struct dbprocess *, int, char * );
extern	int	amAmountOnUnit( struct dbprocess *, int, char *, int * );
extern	int	amAmountQueueable( struct dbprocess *, int, float * );
extern	int	amAmountRetrievable( struct dbprocess *, int, float * );
extern 	int	amCleanedUp( struct dbprocess *, int, char *, boolean * );
extern	void	amClose( void );
extern	int	amDataStageGet( char **, char **, char ** );
extern	int	amDbInfoGet( char **, char **, char **, char **, 
			char **, char ** );
extern	int	amDeviceBook( const char*, boolean, struct dbprocess *,
			struct amDevice * );
extern	int	amDeviceClear( AM_DEVICE_CLEAR, struct dbprocess *, ... );
extern	int	amDeviceGet( AM_DEVICE_GET, struct amDevice *, 
			struct dbprocess *, ... ); 
extern	int	amDeviceLookup( const char *, struct dbprocess *, const char *, 
			struct eptr ** );
extern	int	amFileDelete( const AM_FILE *, struct dbprocess * );
extern	int	amFileGet( AM_FILE_GET, struct amFile *, struct dbprocess *, 
			... ); 
extern	int	amFilePut( struct amFile *, struct dbprocess * );
extern	int	amFileUpdate( AM_FILE_UPDATE, struct dbprocess *, ... );
extern	int	amGet( AM_MEDIA_GET, struct amMedia *, struct dbprocess *, 
			... );
extern	int	amIngestGet( char ** );
extern 	int	amInit( void );
extern	int	amLogicalDeviceGet( AM_LOGICAL_DEVICE_GET, AM_LOGICAL_DEVICE *,
			struct dbprocess *, ... );
extern	int	amMediaOptionsGet( struct dbprocess *,
			struct amMediaOptions * );
extern	int	amMediaOptionsPut( struct dbprocess *,
			struct amMediaOptions * );
extern	int	amMediaTypeGet( AM_TYPE_GET, struct amMediaType *, 
			struct dbprocess *, ... ); 
extern	int	amMediaIdGet( int *, struct dbprocess * );
extern 	int	amMSBook( struct amMedStage *, struct dbprocess * );
extern	int	amMSClear( struct amMedStage *, struct dbprocess * );
extern 	int	amMSGet( AM_STAGE_GET, struct amMedStage *, struct dbprocess *, 
			... );
extern	int	amNotifiable( struct dbprocess *, int, char *, boolean * );
extern	int	amNumCleanupable( struct dbprocess *, int, int * );
extern	int	amNumCopiesWritten( struct dbprocess *, int, char *, int * );
extern	int	amNumOnUnit( struct dbprocess *, int, char *, int * );
extern	int	amNumPreparable( struct dbprocess *, int, int * );
extern	int	amNumWriteable( struct dbprocess *, int, int * );
extern 	int	amPrecisionGet( int * );
extern	int	amPrepared( struct dbprocess *, int, char *, boolean * );
extern  int	amPut( AM_MEDIA	*, struct dbprocess * );
extern	int	amScript( char *, AM_SCRIPT_TYPE, char **, char ** );
extern	int	amStatusStr( struct dbprocess *, char *, char *, char );
extern	int	amUnitDelete( AM_UNIT *, struct dbprocess * );
extern	int	amUnitGet( AM_UNIT_GET, struct amUnit*, struct dbprocess *, 
			... );
extern	int	amUnitPut( AM_UNIT *, struct dbprocess * );
extern	int	amUnitUpdate( AM_UNIT_UPDATE, struct dbprocess *, ... );

