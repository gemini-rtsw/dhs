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
*   Module Name:	am/src/am.c
*
*   Purpose:
*	Routines that access the media, mediaUnits and mediaFiles 
*	tables in the database.
*
*   Routines:
*	int		amActionAdd	: Add an action to the media 
*					  action table.
*	int		amActionDelete	: Delete an action from the 
*					  media action table.
*   	void		amClose		: Frees up the am structures.
*	void		amFree		: Free up the amt structure.
*	int		amDbInfoGet	: Get info about db names.
*	int		amDataStageGet	: Get info about data stage names.
*	int		amFileDelete	: Delete a file from media file tbl.
*	int		amFileGet	: Get info from media files tbl.
*	int		amFilePut	: Put info into media files tbl.
*   	int		amFileUpdate	: Updates the media files table.
*	int		amGet		: Get info from the media table.
*	int		amIngestGet	: Get mediaIngest script name.
*   	int		amInit		: Initializes the am library.
*	int		amMediaIdGet	: Get a new Media Request ID.
*	int		amMediaOptionsGet: Get options from the media options
*					  table.
*	int		amMediaOptionsPut: Update media options in the 
*					  media option table.
*	int		amMediaTypeGet	: Get media type info.
*	int		amPrecisionGet	: Get the unit name precision.
*	int		amPut		: Put info into the media table.
*	int		amStatusStr	: Return status string from db.
*	int		amUnitGet	: Get info from media units tbl.
*	int		amUnitPut	: Put info into media units tbl.
*	int		amUnitUpdate	: Updates the media units tbl.
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: am.c
*	Version Number	: 1.22
*	Release Number	: 1
*	Last Updated	: 08/22/00
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  : Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/02/04 SEC  :	Fixed bug in amGet(). (previously unused code.)
*	98/09/03 JSD  :	Added amDev* routines
*	98/09/25 SDJ  : Added precision and directory split size to media.
*	98/10/27 SDJ  : Fixed all memory leaks, aka made sure cleanup()
*			cleaned everything up. Changed amFree to free
*			mediaTypes.
*	98/11/16 SDJ  : Repaired amUnitGet so it returns the last media_unit
*			this change was needed because of unit names with
*			possible different precision.
*	98/11/23 JSD  :	Added laMedTypesTable reference and use.
*			Changed amDeviceGet to only get the information that
*			now exists in that table.  Change amMediaTypeGet to
*			get from the database, rather then eptr list.
*			Added amIngestGet routine. Re-ordered amClose.
*	98/12/06 SDJ  : Moved device related funstions to amDevice.c
*	    		Added amPrecisionGet().
*	98/12/20 SDJ  : Fixed up comments to match changes.  Made
*			ammUnitNamePrecision not required. Fixed a
*		        bug in amPrecisionGet() added dbcanquery to
*			amStatusStr().
*	99/02/23 SDJ  : Fixed amUnitGet for type AM_UG_STATUS_AFTER_NAME
*			to handle names with variable precision after
*			the unit root name.
*	99/03/05 NRH  : Added amFileDelete(). Added AM_FG_MEDIA_UNIT_ID
*			and AM_FG_MEDIA_ID_FNAME options to amFileGet.
*			Changed to use media_id, file_id, and file_name
*			as the unique index in media files, instead of just
*			media_id and file_id.
*       99/03/24 SDJ  : Changed amGet to check for "DB_SUCCESS" instead of
*			"FAIL" when extracting the media_unit_name_precision.
*	99/04/28 NRH  : Added amActionAdd and amActionDelete.
*	99/06/25 SDJ  : Cleaned up amUnitGet query for type
*			AM_UG_MEDIA_REQUEST_ID_LAST.  It now should only
*			try and get the units whose name are in the correct
*			form.
*	99/10/12 NRH  : Added the AM_FG_UNIQUE option to amFileGet.
*			Added specific compresion types to amFile
*			amfCompressionFlag.
*			Added the AM_MG_REUSE option to amGet.
*     2000/02/03 SDJ  : Added amMediaOptionsGet() and amMediaOptionsPut().
*     2000/02/14 SDJ  : Added the lovely amfDayNotified to the AM file struct.
*     2000/08/22 NRH  : Made amfile argument to amFileDelete "const".
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"

#include "am.h"
#include "localAm.h"

LOCAL_AM	localAm;
boolean		amInitFlag;
void	amFree( AM_MED_SCRIPT * );


/*+
************************************************************************
*
*   Function:	amActionAdd
*
*   Purpose:
*	Add an action to the media_action table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB		: Errro in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		amActionAdd
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    int		requestId,	/* (in)  The media request ID.		*/
    char	*unitName,	/* (in)  The media unit name.		*/
    char	*action		/* (in)  The action to add.		*/
)
{
    int		dbstatus;	/* Return status from the db            */


    if ( unitName == NULL || strlen( unitName ) == 0 )
    {
	AM_CHECK_DB_LIB( db_simple_query( dbproc, 
		"delete \n"
		"from %s..%s \n"
		"where media_request_id = %d and \n"
		"    media_unit_name is NULL \n"
		"insert \n"
		"    into %s..%s \n"
		"    ( media_request_id, media_unit_name, action, time, \n"
		"        process_id ) \n"
		"values ( \n"
		"    %d, NULL, '%s', getdate(), %d ) \n",
		localAm.laDatabase, localAm.laMedActionsTable, requestId,
		localAm.laDatabase, localAm.laMedActionsTable, requestId,
		action, getpid() ) );
    }
    else
    {
	AM_CHECK_DB_LIB( db_simple_query( dbproc, 
		"delete \n"
		"from %s..%s \n"
		"where media_request_id = %d and \n"
		"    media_unit_name ='%s' \n"
		"insert \n"
		"    into %s..%s \n"
		"    ( media_request_id, media_unit_name, action, time, \n"
		"        process_id ) \n"
		"values ( \n"
		"    %d, '%s', '%s', getdate(), %d ) \n",
		localAm.laDatabase, localAm.laMedActionsTable, requestId,
		unitName, localAm.laDatabase, localAm.laMedActionsTable, 
		requestId, unitName, action, getpid() ) );
    }
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AM_CHECK_DB( dbstatus );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amActionDelete
*
*   Purpose:
*	Delete an action from the media_action table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB		: Errro in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		amActionDelete
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    int		requestId,	/* (in)  The media request ID.		*/
    char	*unitName	/* (in)  The media unit name.		*/
)
{
    int		dbstatus;	/* Return status from the db            */

    if ( unitName == NULL || strlen( unitName ) == 0 )
    {
	AM_CHECK_DB_LIB( db_simple_query( dbproc, 
		"delete \n"
		"from %s..%s \n"
		"where media_request_id = %d and \n"
		"    media_unit_name is NULL \n",
		localAm.laDatabase, localAm.laMedActionsTable, requestId ) );
    }
    else
    {
	AM_CHECK_DB_LIB( db_simple_query( dbproc, 
		"delete \n"
		"from %s..%s \n"
		"where media_request_id = %d and \n"
		"    media_unit_name = '%s' \n",
		localAm.laDatabase, localAm.laMedActionsTable, requestId,
		unitName ) );
    }
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AM_CHECK_DB( dbstatus );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amClose
*
*   Purpose:
*	Free up the am structure and eptr lists.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*
************************************************************************
-*/

void		amClose
(
)
{

    if ( localAm.laArchDataStage != NULL )
    {
	gen_free( localAm.laArchDataStage );
	localAm.laArchDataStage = NULL;
    }

    if ( localAm.laBookingTable != NULL )
    {
	gen_free( localAm.laBookingTable );
	localAm.laBookingTable = NULL;
    }

    if ( localAm.laDatabase != NULL )
    {
	gen_free( localAm.laDatabase );
	localAm.laDatabase = NULL;
    }

    if ( localAm.laDeviceTable != NULL )
    {
	gen_free( localAm.laDeviceTable  );
	localAm.laDeviceTable = NULL;
    }

    if ( localAm.laIngestScript != NULL )
    {
	gen_free( localAm.laIngestScript  );
	localAm.laIngestScript = NULL;
    }

    if ( localAm.laLogFileDir != NULL )
    {
	gen_free( localAm.laLogFileDir );
	localAm.laLogFileDir = NULL;
    }

    if ( localAm.laLogicalTable != NULL )
    {
	gen_free( localAm.laLogicalTable );
	localAm.laLogicalTable = NULL;
    }

    if ( localAm.laMedFilesTable != NULL )
    {
	gen_free( localAm.laMedFilesTable );
	localAm.laMedFilesTable = NULL;
    }

    if ( localAm.laMedStagesTable != NULL )
    {
	gen_free( localAm.laMedStagesTable );
	localAm.laMedStagesTable = NULL;
    }

    if ( localAm.laMedTypesTable != NULL )
    {
	gen_free( localAm.laMedTypesTable );
	localAm.laMedTypesTable = NULL;
    }

    if ( localAm.laMedUnitsTable != NULL )
    {
	gen_free( localAm.laMedUnitsTable );
	localAm.laMedUnitsTable = NULL;
    }

    if ( localAm.laMediaDataStage != NULL )
    {
	gen_free( localAm.laMediaDataStage  );
	localAm.laMediaDataStage = NULL;
    }

    if ( localAm.laMediaTable != NULL )
    {
	gen_free( localAm.laMediaTable );
	localAm.laMediaTable = NULL;
    }

    if ( localAm.laMediaScripts != NULL )
    {
	eptr_free( localAm.laMediaScripts, amFree );
	localAm.laMediaScripts = NULL;
    }
    
    if ( localAm.laServer != NULL )
    {
	gen_free( localAm.laServer );
	localAm.laServer = NULL;
    }
    
    if ( localAm.laStatusTable != NULL )
    {
	gen_free( localAm.laStatusTable );
	localAm.laStatusTable = NULL;
    }

    if ( localAm.laUserDataStage != NULL )
    {
	gen_free( localAm.laUserDataStage );
	localAm.laUserDataStage = NULL;
    }
}

/*+
************************************************************************
*
*   Function:	amFree
*
*   Purpose:
*	Free up the amt structure in the eptr list.
*
*   Values Returned:
*	none
*
************************************************************************
-*/

void	amFree
(
    AM_MED_SCRIPT	*amScriptType
)
{
    if ( amScriptType->amsPrepScript != NULL )
    {
	gen_free( amScriptType->amsPrepScript );
	amScriptType->amsPrepScript = NULL;
    }

    if ( amScriptType->amsPrepFile != NULL )
    {
	gen_free( amScriptType->amsPrepFile );
	amScriptType->amsPrepFile = NULL;
    }
    
    if ( amScriptType->amsWriteScript != NULL )
    {
	gen_free( amScriptType->amsWriteScript );
	amScriptType->amsWriteScript = NULL;
    }

    if ( amScriptType->amsWriteFile != NULL )
    {
	gen_free( amScriptType->amsWriteFile );
	amScriptType->amsWriteFile = NULL;
    }

    gen_free( amScriptType );
    return;
}

/*+
************************************************************************
*
*   Function:	amDbInfoGet
*
*   Purpose:
*	Returns the db information from the config file.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int		amDbInfoGet
(
    char	**amServer,	/* (out) The database server.		*/
    char	**amDatabase,	/* (out) The media database.		*/
    char	**amTable,	/* (out) The media table name.		*/
    char	**amUnitTable,	/* (out) The media units table name.	*/
    char	**amFileTable,	/* (out) The media files table name.	*/
    char	**amActionsTable/* (out) The media action table name.	*/
)
{
    AM_CHECK_INIT;


    if ( amServer != NULL )
    {
	*amServer = localAm.laServer;
    }

    if ( amDatabase != NULL )
    {
	*amDatabase = localAm.laDatabase;
    }

    if ( amTable != NULL )
    {
	*amTable = localAm.laMediaTable;
    }

    if ( amUnitTable != NULL )
    {
	*amUnitTable = localAm.laMedUnitsTable;
    }

    if ( amFileTable != NULL )
    {
	*amFileTable = localAm.laMedFilesTable;
    }

    if ( amActionsTable != NULL )
    {
	*amActionsTable = localAm.laMedActionsTable;
    }


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amDataStageGet
*
*   Purpose:
*	Returns the Data Stage information from the config file.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int		amDataStageGet
(
    char	**amArchDStage,	/* (out) The archive data stage.	*/
    char	**amUserDStage,	/* (out) The archive data stage.	*/
    char	**amMediaDStage	/* (out) The archive data stage.	*/
)
{
    AM_CHECK_INIT;

    if ( amArchDStage != NULL )
    {
	*amArchDStage = localAm.laArchDataStage;
    }

    if ( amUserDStage != NULL )
    {
	*amUserDStage = localAm.laUserDataStage;
    }

    if ( amMediaDStage != NULL )
    {
	*amMediaDStage = localAm.laMediaDataStage;
    }

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amFileDelete
*
*   Purpose:
*	Delete a file from the media files table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB		: Error in sybase db library routine.
*       int     AM_E_INIT       : Library is not initialized.
*
************************************************************************
-*/

int		amFileDelete
(
    const AM_FILE
		*amFile,	/* (in)  The file to delete.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;

    AM_CHECK_INIT;

    AM_CHECK_DB( dbfcmd( dbproc, 
	    "delete \n"
	    "    %s..%s \n"
	    "where \n"
	    "	 media_request_id = %d \n"
	    "	 and file_id = '%s' \n"
	    "	 and filename = '%s' \n",
	    localAm.laDatabase, localAm.laMedFilesTable,
            amFile->amfMediaRequestId, amFile->amfFileId, 
	    amFile->amfFilename ) );


    AM_CHECK_DB( dbsqlexec( dbproc ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {}
    AM_CHECK_DB( dbstatus );


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amFileGet
*
*   Purpose:
*       Gets a specified file or list of files from the media_files table.
*       If more than one file matches the specifiecation, the first file is 
*       returned with the first call to this function, and any others should
*       be retrieved by calling this function with a select_type of
*       AM_FG_NEXT.
*
*       char    *fileName;
*       amFileGet( AM_FG_NAME, AM_FILE *, DBPROCESS *, fileName );
*
*       char    *fileId;
*       amFileGet( AM_FG_FILE_ID, AM_FILE *, DBPROCESS *, fileId );
*	
*       char    *sourceId;
*       amFileGet( AM_FG_SOURCE_ID, AM_FILE *, DBPROCESS *, sourceId );
*
*       char    status;
*       amFileGet( AM_FG_STATUS, AM_FILE *, DBPROCESS *, status );
*
*       int    mediaId;
*       char    *fileId;
*       amFileGet( AM_FG_MEDIA_FILE, AM_FILE *, DBPROCESS *, mediaId, 
*		fileId );
*
*       int    mediaId;
*       char    *fileName;
*       amFileGet( AM_FG_MEDIA_ID_FNAME, AM_FILE *, DBPROCESS *, mediaId, 
*		fileName );
*
*	int	*mediaId
*       char    *fileName;
*       char    *fileId;
*	amFileGet( AM_FG_UNIQUE, AM_FILE *, DBPROCESS *, mediaId, 
*		fileId, fileName );
*
*       int    mediaId;
*       amFileGet( AM_FG_MEDIA_REQUEST_ID, AM_FILE *, DBPROCESS *, mediaId );
*
*	int	mediaId
*	char	*unitId
*	amFileGet( AM_FG_MEDIA_UNIT_ID, AM_FILE *, DBPROCESS *, 
*			mediaId, mediaUnitId );
*
*	AM_UNIT	*mediaUnit
*	amFileGet( AM_FG_MEDIA_UNIT, AM_FILE *, DBPROCESS *, mediaUnit );
*
*       int    mediaId;
*       amFileGet( AM_FG_QUEUEABLE, AM_FILE *, DBPROCESS *, mediaId );
*
*       int	mediaId;
*       amFileGet( AM_FG_RETRIEVABLE, AM_FILE *, DBPROCESS *, mediaId );
*
*	AM_UNIT	*mediaUnit
*       char    *fileName;
*	amFileGet( AM_FG_UNIT_FNAME, AM_FILE *, DBPROCESS *, amUnit, fileName );
*
*       int    mediaId;
*	amFileGet( AM_FG_UNWRITTEN, AM_FILE *, DBPROCESS *, mediaId );
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_FILEGET_ENUM
*				: File get type is not valid.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_NO_FILE	: No files to return.
*
************************************************************************
-*/

int	amFileGet
(
    AM_FILE_GET	selectType,	/* (in)  Type of selection.		*/
    AM_FILE	*amFile,	/* (out) File's record.			*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...				/* (in)  The optional arguments.	*/
)
{
    AM_UNIT	*amUnitPtr;	/* Ptr. to media unit struct.		*/
    va_list	args;		/* The variable-length argument list	*/
    char	compressionFlag;	
				/* Compression flag from args if used.	*/
    int		status;		/* Return status from the db		*/
    time_t	day;		/* Num. of seconds in  a day.	*/
    
    struct tm	*time;
    

    AM_CHECK_INIT;
    day = 86400;
    
    if ( selectType != AM_FG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    compression_flag, \n"
		"    datediff( second, 'jan 1 1970', date_notified), \n"
		"    file_id, \n"
		"    filesize, \n"
		"    filename, \n"
		"    media_request_id, \n"
		"    media_unit_name, \n"
		"    source_id, \n"
		"    status, \n"
		"    uncomp_filesize \n"
		"from \n"
		"    %s..%s \n"
		"where \n",
		localAm.laDatabase,
		localAm.laMedFilesTable ) );

	switch ( selectType )
	{
	    case AM_FG_FNAME:
		va_start( args, dbproc );
		(void) strcpy( amFile->amfFilename,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    filename = '%s' \n", amFile->amfFilename ) );
		break;

	    case AM_FG_FILE_ID:
		va_start( args, dbproc );
		(void) strcpy( amFile->amfFileId,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    file_id = '%s' \n", amFile->amfFileId ) );
		break;

	    case AM_FG_SOURCE_ID:
		va_start( args, dbproc );
		(void) strcpy( amFile->amfSourceId,
		        (char *) va_arg( args, char *) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    source_id = '%s' \n", amFile->amfSourceId ) );
		break;

	    case AM_FG_STATUS:
		va_start( args, dbproc );
		amFile->amfStatus = (char) va_arg( args, int );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     status = '%s' \n", amFile->amfStatus ) );
		break;

	    case AM_FG_MEDIA_FILE:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		(void) strcpy( amFile->amfFileId,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and file_id = '%s' \n",
			amFile->amfMediaRequestId, amFile->amfFileId, 
			amFile->amfFilename ) );
		break;

	    case AM_FG_UNIQUE:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		(void) strcpy( amFile->amfFileId,
		        (char *) va_arg( args, char * ) );
		(void) strcpy( amFile->amfFilename,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and filename = '%s' \n"
			"     and file_id = '%s' \n",
			amFile->amfMediaRequestId, 
			amFile->amfFilename,
			amFile->amfFileId ) );
		break;

	    case AM_FG_MEDIA_REQUEST_ID:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc, "     media_request_id = %d \n",
			amFile->amfMediaRequestId ) );
		break;

	    case AM_FG_MEDIA_UNIT_ID:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		strncpy( amFile->amfMediaUnitName,  va_arg( args, char * ), 
			AM_MEDIA_UNIT_NAME_LEN ) ;
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d and \n",
			amFile->amfMediaRequestId ) );
		if ( amFile->amfMediaUnitName[0] == '\0' )
		{
		    AM_CHECK_DB( dbfcmd( dbproc,
			    "     media_unit_name = NULL \n" ) );
		}
		else
		{
		    AM_CHECK_DB( dbfcmd( dbproc,
			    "     media_unit_name = '%s' \n",
			    amFile->amfMediaUnitName ) );
		}
		break;

	    case AM_FG_MEDIA_UNIT:
		va_start( args, dbproc );
		amUnitPtr = (AM_UNIT *) va_arg( args, AM_UNIT * );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and media_unit_name = '%s' \n",
			amUnitPtr->amuMediaRequestId,
			amUnitPtr->amuMediaUnitName ) );
		break;
	
	    case AM_FG_QUEUEABLE:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_request_id = %d \n"
			"    and status = '%c' \n"
			"    and media_unit_name = NULL \n",
			amFile->amfMediaRequestId, AMF_RETRIEVED ) );
		break;
	
	    case AM_FG_RETRIEVABLE:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and  ( status in ( '%c', \n"
			"            '%c', '%c' ) ) \n",
			amFile->amfMediaRequestId, AMF_UNPROCESSED,
			AMF_ERROR, AMF_UNAVAILABLE ) );
		break;

	    case AM_FG_UNIT_FNAME:
		va_start( args, dbproc );
		amUnitPtr = (AM_UNIT *) va_arg( args, AM_UNIT * );
		(void) strcpy( amFile->amfFilename,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and media_unit_name = '%s' \n"
			"     and filename = '%s' \n",
			amUnitPtr->amuMediaRequestId, 
			amUnitPtr->amuMediaUnitName,
			amFile->amfFilename ) );
		break;

	    case AM_FG_MEDIA_ID_FNAME:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		(void) strcpy( amFile->amfFilename,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and filename = '%s' \n",
			amFile->amfMediaRequestId, 
			amFile->amfFilename ) );
		break;

	    case AM_FG_UNWRITTEN:
		va_start( args, dbproc );
		amFile->amfMediaRequestId = (int ) va_arg( args, int ) ;
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     media_request_id = %d \n"
			"     and  ( status not in \n"
			"          ( '%c' ) ) \n",
			amFile->amfMediaRequestId, 
			AMF_DONE ) );
		break;

	    default:
		amFormatMessage( AM_E_FILEGET_ENUM, selectType );
		return( AM_E_FILEGET_ENUM );
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }


    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_FILE );
    }

    AM_CHECK_DB( status );

    AM_CHECK_DB_LIB( db_chardata( dbproc, 1, &compressionFlag ));
    amFile->amfCompressionFlag = compressionFlag;


    /*
     * For backward compatability, look for compression flags of 'Y' and 'N'.
     */

    if ( compressionFlag == 'Y' )
    {
    	amFile->amfCompressionFlag = AMF_COMP_GZIP;
    }
    else if ( compressionFlag == 'N' )
    {
	amFile->amfCompressionFlag = AMF_COMP_NONE;
    }

    if ( ( status = db_intdata( dbproc, 2, 
	    (int *) &( amFile->amfDateNotified ) ) ) == DB_NULL_VALUE )
    {
	amFile->amfDateNotified = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }

    amFile->amfDayNotified = amFile->amfDateNotified + ( day / 2 );
    amFile->amfDayNotified = amFile->amfDayNotified -
	( amFile->amfDateNotified % day );
         

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 3, amFile->amfFileId,
	    AM_FILE_ID_LEN ) );


    AM_CHECK_DB_LIB( db_intdata( dbproc, 4, &amFile->amfFileSize ) );
    
        
    AM_CHECK_DB_LIB( db_stringdata( dbproc, 5, amFile->amfFilename,
	    AM_FILE_NAME_LEN ) );
    
    AM_CHECK_DB_LIB( db_intdata( dbproc, 6, 
	    (int *) &(amFile->amfMediaRequestId ) ) );

    if ( ( status = db_stringdata( dbproc, 7, 
	    amFile->amfMediaUnitName, AM_MEDIA_UNIT_NAME_LEN ) )
	    == DB_NULL_VALUE )
    {
	amFile->amfMediaUnitName[0] = '\0';
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 8, 
	    amFile->amfSourceId, AM_SOURCE_ID_LEN ) );
    AM_CHECK_DB_LIB( db_chardata( dbproc, 9, &amFile->amfStatus ));

    AM_CHECK_DB_LIB( db_intdata( dbproc, 10, &amFile->amfUncompFileSize ) );
    
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amFilePut
*
*   Purpose:
*	Insert a file into the media_files table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB		: Errro in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		amFilePut
(
    AM_FILE	*mediaFile,	/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;	/* Return status from the db            */
    char	mediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    				/* Media unit name.			*/
	    

    AM_CHECK_DB( dbcanquery( dbproc ) );


    /*
     *  Set the compression flag string and media unit name.
     */

    if ( mediaFile->amfMediaUnitName[0] == '\0' )
    {
	(void) sprintf( mediaUnitName, "%s", "NULL" );
    }
    else
    {
	(void) sprintf( mediaUnitName, "'%s'", mediaFile->amfMediaUnitName );
    }

    AM_CHECK_DB_LIB( db_simple_query( dbproc, 
	    "declare @file_id char(%d) \n"
	    "declare @file_name char(%d) \n"
	    "declare @media_request_id int  \n"
	    "select \n"
	    "    @file_id = '%s', \n"
	    "    @file_name = '%s', \n"
	    "    @media_request_id = %d \n"
	    "delete \n"
	    "    %s..%s \n"
	    "where \n"
	    "	 media_request_id = @media_request_id \n"
	    "    and file_id = @file_id \n"
	    "    and filename = @file_name \n"
	    "insert \n"
	    "    %s..%s \n"
	    "( \n"
	    "    compression_flag, \n"
	    "    date_notified, \n"
	    "    file_id, \n"
	    "    filesize, \n"
	    "    filename, \n"
	    "    media_request_id, \n"
	    "    media_unit_name, \n"
	    "    source_id, \n"
	    "    status, \n"
	    "    uncomp_filesize ) \n"
	    "values ( \n"
	    "    '%c', \n"
	    "    '%s',  \n"
	    "    @file_id, \n"
	    "    %d, \n"
	    "    '%s', \n"
	    "    @media_request_id, \n"
	    "    %s, \n"		/* quotes added above if needed */
	    "    '%s', \n"
	    "    '%c', \n"
	    "    %d ) \n",
	    AM_FILE_ID_LEN, 
	    AM_FILE_NAME_LEN, 
	    mediaFile->amfFileId, mediaFile->amfFilename,
	    mediaFile->amfMediaRequestId, localAm.laDatabase, 
	    localAm.laMedFilesTable, localAm.laDatabase, 
	    localAm.laMedFilesTable, mediaFile->amfCompressionFlag, 
	    db_time2str( mediaFile->amfDateNotified, DB_UTC ),
	    mediaFile->amfFileSize, mediaFile->amfFilename,
	    mediaUnitName, mediaFile->amfSourceId,
	    mediaFile->amfStatus, mediaFile->amfUncompFileSize ) );
    
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AM_CHECK_DB( dbstatus );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amFileUpdate
*
*   Purpose:
*	Updates the fields in the media files table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB	 	: Error in the sybase db lib.
*	int	AM_E_FILEUPDATE_ENUM
*				: File update type is not valid.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/
int		amFileUpdate
(
    AM_FILE_UPDATE	selectType,	/* (in)  Type of selection.	 */
    DBPROCESS		*dbproc,	/* (in)  Dbprocess to use.	 */
    ...					/* (in)  The optional arguments. */
)
{
    va_list     args;
    int		mediaRequestId;
    char	mediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    char	sourceId[AM_SOURCE_ID_LEN];


    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );


    /* 
     *      Start the update command.
     */
    
    AM_CHECK_DB( dbfcmd( dbproc, 
            "update \n"
            "   %s..%s \n"
            "set \n",
            localAm.laDatabase, localAm.laMedFilesTable ) );
    

    /*
     *	    Select the type of update.
     */
     
    switch ( selectType )
    {
	case AM_FU_COMPLETE_ARCH:
            va_start( args, dbproc );
            mediaRequestId = (int) va_arg( args, int );
	    (void) strcpy( mediaUnitName, 
		    (char *) va_arg( args, char * ) );
	    (void) strcpy( sourceId, 
		    (char *) va_arg( args, char * ) );
            va_end( args );

	    AM_CHECK_DB( dbfcmd( dbproc,
		    "    status = '%c'         \n"
		    "where                     \n"
		    "    media_request_id = %d         \n"
		    "    and media_unit_name = '%s' \n"
		    "    and source_id = '%s'  \n",
		    AMF_DONE,
		    mediaRequestId, 
		    mediaUnitName,
		    sourceId ) );
	    break;

	case AM_FU_COMPLETE_USER:
            va_start( args, dbproc );
            mediaRequestId = (int) va_arg( args, int );
            va_end( args );

	    AM_CHECK_DB( dbfcmd( dbproc,
		    "    status = '%c'         \n"
		    "where                     \n"
		    "    media_request_id = %d         \n",
		    AMF_DONE,
		    mediaRequestId ) );
	    break;
		    
        case AM_FU_MEDIA_Q_ROLLBACK:
            va_start( args, dbproc );
            mediaRequestId = (int) va_arg( args, int );
	    (void) strcpy( mediaUnitName, 
		    (char *) va_arg( args, char * ) );
            va_end( args );

            AM_CHECK_DB( dbfcmd( dbproc,
                    "     media_unit_name = NULL,     \n" 
                    "     status = '%c'        \n" 
		    "where                     \n"
		    "    media_request_id = %d         \n"
		    "    and media_unit_name = '%s' \n",
                    AMF_RETRIEVED,
		    mediaRequestId,
		    mediaUnitName ) );
            break;
#ifdef OMIT
        case AM_FU_WRITTEN:
            va_start( args, dbproc );
            mediaRequestId = (int) va_arg( args, int );
	    (void) strcpy( mediaUnitName, 
		    (char *) va_arg( args, char * ) );
            va_end( args );

            AM_CHECK_DB( dbfcmd( dbproc,
                    "     status = '%c'        \n" 
		    "where                     \n"
		    "    media_request_id = %d         \n"
		    "    and media_unit_name = '%s' \n",
                    AMF_WRITTEN,
		    mediaRequestId,
		    mediaUnitName ) );
            break;
#endif
        default:
            amFormatMessage( AM_E_FILEUPDATE_ENUM, selectType );
            return( AM_E_FILEUPDATE_ENUM );
    }

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );
        
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amGet
*
*   Purpose:
*       Gets a specified file or list of files from the media table.
*       If more than one file matches the specifiecation, the first file is 
*       returned with the first call to this function, and any others should
*       be retrieved by calling this function with a select_type of
*       AM_NEXT.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB         : Errro in the sybase db library.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_E_MEDIAGET_ENUM
*				: Invalid select type.
*	int	AM_NO_MEDIA	: Request not found in media table.
*
************************************************************************
-*/

int		amGet
(
    AM_MEDIA_GET	selectType,	/* (in)  Type of selection.	*/
    AM_MEDIA		*media,		/* (out) Media information.	*/
    DBPROCESS		*dbproc,	/* (in)  The dbprocess to use.	*/
    ...					/* (in)  The optional arguments.*/
)
{
    va_list	args;
    int		status;

    AM_CHECK_INIT;


    if ( selectType != AM_MG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    datediff( second, 'jan 1 1970', date_completed ), \n"
		"    datediff( second, 'jan 1 1970', date_requested ), \n"
		"    media_request_id, \n"
		"    media_request_name, \n"
		"    media_request_type, \n"
		"    media_type, \n"
		"    num_copies, \n"
		"    source_id, \n"
		"    status, \n"
		"    unit_name_root, \n"
		"    unit_name_precision \n"
		"from \n"
		"    %s..%s \n",
		localAm.laDatabase,
		localAm.laMediaTable ) );

	switch ( selectType )
	{
	    case AM_MG_ALL:
		break;

	    case AM_MG_NOTIFIABLE:
		AM_CHECK_DB( dbfcmd( dbproc,
			"where \n"
			"    ( media_request_type = '%s' ) \n"
			"or \n"
			"    ( media_request_type = '%s' and \n"
			"    status = '%c' ) \n",
			AM_RT_ARCHIVE, AM_RT_USER, 
			AMM_DONE ) );
		break;

	    case AM_MG_MEDIA_REQUEST_ID:
		va_start( args, dbproc );
		media->ammMediaRequestId = (int) va_arg( args, int );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"where \n"
			"    media_request_id = %d \n",
			media->ammMediaRequestId ) );
		break;

	    case AM_MG_SOURCE_ID:
		va_start( args, dbproc );
		(void) strcpy( media->ammSourceId,
		        (char *) va_arg( args, char *) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"where \n"
			"    source_id = '%s' \n",
			media->ammSourceId ) );
		break;

	    case AM_MG_STATUS:
		va_start( args, dbproc );
		media->ammStatus = (char) va_arg( args, int );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"where \n"
			"    status = '%c' \n",
			media->ammStatus ) );
		break;

	    case AM_MG_MEDIA_REQUEST_NAME:
		va_start( args, dbproc );
		(void) strcpy( media->ammMediaRequestName,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"where \n"
			"    media_request_name = '%s' \n",
			media->ammMediaRequestName ) );
		break;

	    case AM_MG_REUSE:
	    	/*
		 * Get a media unit suitable for reuse in place of the 
		 * media unit in the argument.
		 */


		AM_CHECK_DB( dbfcmd( dbproc,
			"where \n"
			"    media_request_name = '%s' and\n"
			"    source_id = '%s' and\n"
			"    num_copies = %d and\n"
			"    media_type = '%s' \n",
			media->ammMediaRequestName,
			media->ammSourceId, media->ammNumCopies, 
			media->ammMediaType ) );
		break;

	    default:
		amFormatMessage( AM_E_MEDIAGET_ENUM, selectType );
		return( AM_E_MEDIAGET_ENUM );
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_MEDIA );
    }

    AM_CHECK_DB( status );

    if ( ( status = db_intdata( dbproc, 1, 
	    (int *) &( media->ammDateCompleted ) ) ) == DB_NULL_VALUE )
    {
	media->ammDateCompleted = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }

    if ( ( status = db_intdata( dbproc, 2, 
	    (int *) &( media->ammDateRequested ) ) ) == DB_NULL_VALUE )
    {
	media->ammDateRequested = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }

    AM_CHECK_DB_LIB( db_intdata( dbproc, 3, &media->ammMediaRequestId ) );
    
    AM_CHECK_DB_LIB( db_stringdata( dbproc, 4, 
	    media->ammMediaRequestName, 
	    sizeof( media->ammMediaRequestName ) ) );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 5, 
	    media->ammMediaRequestType, 
	    sizeof( media->ammMediaRequestType ) ) );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 6, 
	    media->ammMediaType, sizeof( media->ammMediaType ) ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 7, &media->ammNumCopies ) );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 8, 
	    media->ammSourceId, sizeof( media->ammSourceId ) ) );

    AM_CHECK_DB_LIB( db_chardata( dbproc, 9, &media->ammStatus ) );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 10, 
	    media->ammUnitNameRoot, sizeof( media->ammUnitNameRoot ) ) );

    if ( db_intdata( dbproc, 11, &media->ammUnitNamePrecision ) != DB_SUCCESS )
    {
	media->ammUnitNamePrecision = localAm.laDefaultPrec;
    }
    
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amIngestGet
*
*   Purpose:
*	Returns the mediaIngest script name.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_E_MEMORY	: Error allocating memory.
*
************************************************************************
-*/

int		amIngestGet
(
    char	**amIngestName	/* (out) The mediaIngest script name.	*/
)
{
    AM_CHECK_INIT;

    if ( amIngestName == NULL || *amIngestName == NULL )
    {
	return( AM_E_MEMORY );
	
    }

    (void) strcpy( *amIngestName, localAm.laIngestScript );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amInit
*
*   Purpose:
*	Initializes the am library.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_CONFIG	: Error reading the config file.
*
************************************************************************
-*/

int		amInit
(
)
{
    AM_CHECK( amConfigInit() );

        
    amInitFlag = TRUE;


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amMediaIdGet
*
*   Purpose:
*       Gets a new unused media id.
*
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB         : Error in the sybase db library.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*       int     AM_E_INIT       : Library is not initialized.
*
************************************************************************
-*/

int		amMediaIdGet
(
    int		*mediaRequestId,/* (out) The new Media Request ID.	*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		status;

    AM_CHECK_INIT;

    /*
     *  First get the media id from the keys table.
     */

    AM_CHECK_DB( dbcanquery( dbproc ) );
    AM_CHECK_DB( dbfcmd( dbproc,
	    "declare @result int \n"
	    "exec get_medId @result output \n"
	    "select @result \n" ) );
    AM_CHECK_DB( dbsqlexec( dbproc ) );

    status = dbresults( dbproc );
    status = dbresults( dbproc );
    status = dbnextrow( dbproc );

    if ( status != NO_MORE_ROWS && (( status = db_intdata( dbproc, 1, 
	    mediaRequestId ) ) == DB_NULL_VALUE) )
    {
	*mediaRequestId = 0;
    }


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amMediaTypeGet
*
*   Purpose:
*       Gets a specified type or list of types from the mediaTypes table.
*       If more than one type matches the specification, the first type is 
*       returned with the first call to this function, and any others should
*       be retrieved by calling this function with a select_type of
*       AM_NEXT.  Returns filled in AM_MED_TYPE structure.
*
*       amMediaTypeGet( AM_TG_ALL, AM_MED_TYPE *, DBPROCESS * );
*
*	char *medType;
*       amMediaTypeGet( AM_TG_TYPE, AM_MED_TYPE *, DBPROCESS *, medType );
*
*       amMediaTypeGet( AM_TG_NEXT, AM_MED_TYPE *, DBPROCESS * );
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_TYPEGET_ENUM
*				: Media get type is not valid.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_NO_TYPE	: No information to return.
*
************************************************************************
-*/

int	amMediaTypeGet
(
    AM_TYPE_GET
    		selectType,	/* (in)  Type of selection.		*/
    AM_MED_TYPE	*amMediaType,	/* (out) Type's information.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...				/* (in)  The optional arguments.	*/
)
{
    va_list	args;		/* The variable-length argument list	*/
    int		status;		/* Return status from the db		*/

    AM_CHECK_INIT;


    if ( selectType != AM_TG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    media_type, \n"
		"    capacity, \n"
		"    threshold, \n"
		"    splitSize, \n"
		"    blockSize, \n"
		"    fileOverhead, \n"
		"    directoryOverhead\n"
		"from \n"
		"    %s..%s \n",
		localAm.laDatabase,
		localAm.laMedTypesTable ) );

	switch ( selectType )
	{
	    case AM_TG_ALL:
		break;
	    case AM_TG_TYPE:
		va_start( args, dbproc );
		(void) strcpy( amMediaType->amtTypeName, 
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"where media_type = '%s' \n",
			amMediaType->amtTypeName ) );
		break;

	    default:
		amFormatMessage( AM_E_TYPEGET_ENUM, selectType );
		return( AM_E_TYPEGET_ENUM );
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }


    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_TYPE );
    }

    AM_CHECK_DB( status );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, 
	    amMediaType->amtTypeName, sizeof( amMediaType->amtTypeName ) ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 2, 
	    (int *) &(amMediaType->amtMaxSize ) ) );

    AM_CHECK_DB_LIB( db_floatdata( dbproc, 3, 
	    (float *)&(amMediaType->amtThreshold ) ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 4, 
	    (int *) &(amMediaType->amtSplitSize ) ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 5, 
	    (int *) &(amMediaType->amtBlockSize ) ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 6, 
	    (int *) &(amMediaType->amtFileOverh ) ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 7, 
	    (int *) &(amMediaType->amtDirOverh ) ) );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amPrecisionGet
*
*   Purpose:
*	Returns the default unit name precision.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_E_MEMORY	: Error allocating memory.
*
************************************************************************
-*/

int		amPrecisionGet
(
    int		*amPrecision	/* (out) The mediaIngest script name.	*/
)
{
    AM_CHECK_INIT;

    if ( amPrecision == NULL )
    {
	return( AM_E_MEMORY );
    }
    
    *amPrecision = localAm.laDefaultPrec;
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amPut
*
*   Purpose:
*	Delete and Insert a line into the media table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB         : Error in the sybase db library.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*       int     AM_E_INIT       : Library is not initialized.
*
************************************************************************
-*/

int		amPut
(
    AM_MEDIA	*media,		/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;	/* Return status from dbnextrow().	*/

    AM_CHECK_INIT;


    /*
     *  Set the precision to the default value, if required
     */

    if ( media->ammUnitNamePrecision == AM_DEFAULT_PRECISION )
    {
	media->ammUnitNamePrecision = localAm.laDefaultPrec;
    }


    /* 
     *  Start the put command.
     */
    
    
    AM_CHECK_DB( dbfcmd( dbproc, 
	    "delete \n"
	    "    %s..%s \n"
	    "where \n"
	    "	 media_request_id = %d\n"
	    "insert \n"
	    "    %s..%s \n"
	    "( \n"
	    "    date_requested, \n"
	    "    media_request_id, \n"
	    "    media_request_name, \n"
	    "    media_request_type, \n"
	    "    media_type, \n"
	    "    num_copies, \n"
	    "    source_id, \n"
	    "    status, \n"
	    "    unit_name_root, \n"
	    "    unit_name_precision, \n"
	    "    date_completed ) \n"
	    "values ( \n"
	    "    '%s', \n"
	    "     %d, \n"
	    "    '%s', \n"
	    "    '%s', \n"
	    "    '%s', \n"
	    "     %d, \n"
	    "    '%s', \n"
	    "    '%c', \n"
	    "    '%s', \n"
	    "    %d, \n",
	    localAm.laDatabase, localAm.laMediaTable,
	    media->ammMediaRequestId,
	    localAm.laDatabase, localAm.laMediaTable,
	    db_time2str( media->ammDateRequested, DB_UTC ),
	    media->ammMediaRequestId,
	    media->ammMediaRequestName,
	    media->ammMediaRequestType,
	    media->ammMediaType,
	    media->ammNumCopies,
	    media->ammSourceId,
	    media->ammStatus,
	    media->ammUnitNameRoot,
	    media->ammUnitNamePrecision ) );
    

    /*
     *  Complete the put command.
     */

    if ( media->ammDateCompleted == 0 &&
	    media->ammStatus == AMM_DONE )
    {
	AM_CHECK_DB( dbfcmd( dbproc, 
		"    getdate() ) \n" ) );
    }
    else if ( media->ammDateCompleted == 0 )
    {
	AM_CHECK_DB( dbfcmd( dbproc, 
	    "    NULL ) \n" ) );
    }
    else
    {
	AM_CHECK_DB( dbfcmd( dbproc, 
	    "    '%s') \n",
	    db_time2str( media->ammDateCompleted, DB_UTC ) ) );
    }

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {}
    AM_CHECK_DB( dbstatus );


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amMediaActionGet
*
*   Purpose:
*	Retrieve options from the media_options table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB		: Errro in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		amMediaOptionsGet
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    AM_MEDIA_OPTIONS*
    		amMediaOptions	/* (mod) The options retrieved.		*/
)
{
    int		dbstatus;	/* Return status from the db            */
    int		numRows;	/* Number of rows returned by the query.*/

    
    AM_CHECK_NULL( amMediaOptions );

    AM_CHECK_DB( dbcanquery( dbproc ) );
	
    AM_CHECK_DB( dbfcmd( dbproc, 
	    "select options \n"
	    "from %s..%s \n"
	    "where media_request_id = %d and \n"
	    "    processName = '%s' \n",
	    localAm.laDatabase, localAm.laMedOptionsTable,
	    amMediaOptions->amoMediaRequestId,
	    amMediaOptions->amoProcessName ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    if ( ( numRows =  DBROWS( dbproc ) ) < 1 )
    {
	return( AM_NO_MEDIA_OPTIONS );
    }
    else if ( numRows > 1 )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );
	amFormatMessage( AM_E_MEDIA_OPTION_GET, numRows );
	return( AM_E_MEDIA_OPTION_GET );
    }
    

    /*
     * Only one set of option was found so get it.
     */
    
    AM_CHECK_DB( ( dbstatus = dbnextrow( dbproc ) ) );
	    
    AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, amMediaOptions->amoOptions,
	    AM_MEDIA_OPTION_LEN ) );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amMediaOptionsPut
*
*   Purpose:
*       Inserts options to the media_options table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB		: Errro in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*
************************************************************************
-*/

int		amMediaOptionsPut
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    AM_MEDIA_OPTIONS*
    		amMediaOptions	/* (in)	 Media option to be updated.	*/
)
{
    int		dbstatus;	/* Return status from the db            */

    AM_CHECK_NULL( amMediaOptions );
    
    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	    "declare @media_request_id int \n"
	    "declare @processName char(%d) \n"
	    "select \n"
	    "	@media_request_id = %d, \n"
	    "	@processName = '%s' \n"
	    "delete \n"
	    "	%s..%s \n"
	    "where media_request_id = %d and \n"
	    "    processName = '%s' \n"
	    "insert \n"
	    "    %s..%s \n"
	    "    ( media_request_id, processName, options ) \n"
	    "values ( \n"
	    "    %d, '%s', '%s' ) \n",
	    AM_PROC_NAME_LEN - 1,
	    amMediaOptions->amoMediaRequestId, amMediaOptions->amoProcessName,
	    localAm.laDatabase, localAm.laMedOptionsTable,
	    amMediaOptions->amoMediaRequestId, amMediaOptions->amoProcessName,
	    localAm.laDatabase, localAm.laMedOptionsTable,
	    amMediaOptions->amoMediaRequestId, amMediaOptions->amoProcessName,
	    amMediaOptions->amoOptions ) );
    
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AM_CHECK_DB( dbstatus );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amStatusStr
*
*   Purpose:
*	Returns into amString, the short status description from the 
*	status table. 
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB		: Error in the subase db library.
*       int     AM_E_DB_LIB	: Error in the cadc db library.
*       int     AM_E_INIT	: Library is not initialized.
*	int	AM_E_STATUS	: Error finding status string.
*
************************************************************************
-*/

int		amStatusStr
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    char	*amString,	/* (out) The string to return.		*/
    char	*amTable,	/* (in)  table to query on.		*/
    char	amStatus	/* (in)  status character to query on.	*/ 
)
{
    int		status;		/* Return status from dbnextrow().	*/

    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    short_description \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    table_name = '%s' \n"
	    "    and status = '%c' \n"
	    "    and column_name = 'status' \n",
	    localAm.laDatabase,
	    localAm.laStatusTable,
	    amTable,
	    amStatus ) );

    
    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	amFormatMessage( AM_E_STATUS, amTable, amStatus, 
		localAm.laDatabase, localAm.laStatusTable );
        return( AM_E_STATUS );
    }
    AM_CHECK_DB( status );
 

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, amString, 
	    AM_STATUS_SHORT_LEN ) );


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amUnitDelete
*
*   Purpose:
*	Delete a row from the media_units table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB		: Error in sybase db library routine.
*       int     AM_E_INIT       : Library is not initialized.
*
************************************************************************
-*/

int		amUnitDelete
(
    AM_UNIT	*units,		/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;

    AM_CHECK_INIT;

    AM_CHECK_DB( dbfcmd( dbproc, 
	    "delete \n"
	    "    %s..%s \n"
	    "where \n"
	    "	 media_request_id = %d \n"
	    "	 and media_unit_name = '%s' \n",
	    localAm.laDatabase, localAm.laMedUnitsTable,
            units->amuMediaRequestId,
	    units->amuMediaUnitName ) );


    AM_CHECK_DB( dbsqlexec( dbproc ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {}
    AM_CHECK_DB( dbstatus );


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amUnitGet
*
*   Purpose:
*       Gets a specified file or list of files from the media_units table.
*       If more than one file matches the specification, the first file is 
*       returned with the first call to this function, and any others should
*       be retrieved by calling this function with a select_type of
*       AM_UG_NEXT.
*
*       int    mediaId;
*       amUnitGet( AM_UG_MEDIA_REQUEST_ID, AM_UNIT *, DBPROCESS *, mediaId );
*
*       int    mediaId;
*	char   *unitNameRoot
*       amUnitGet( AM_UG_MEDIA_REQUEST_ID_LAST, AM_UNIT *, DBPROCESS *,
*		mediaId, unitNameRoot );
*
*       int    mediaId;
*	char   *unitName
*       amUnitGet( AM_UG_MEDIA_REQUEST_ID_NAME, AM_UNIT *, DBPROCESS *,
*		mediaId, unitName );
*
*       char   *unitName;
*       amUnitGet( AM_UG_MEDIA_UNIT_NAME, AM_UNIT *, DBPROCESS *, unitName );
*
*
*	AM_MEDIA    *media
*	amUnitGet( AM_UN_NOTIFIABLE, AM_UNIT *, DBPROCESS *, media );
*
*       char    status;
*       amUnitGet( AM_UG_STATUS, AM_UNIT *, DBPROCESS *, status );
*
*	int	mediaId;
*       char    status;
*	char	*pattern
*	char    *unitNameRoot
*       amUnitGet( AM_UG_STATUS_AFTER_NAME, AM_UNIT *, DBPROCESS *, mediaId,
*		status, pattern, unitNameRoot );
*
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*       int     AM_E_INIT       : Library is not initialized.
*	int	AM_E_UNITSGET_ENUM
*				: Units get type is not valid.
*	int	AM_NO_UNITS	: Request not found in media_units table.
*
*   Deficiencies
* 	AM_UG_MEDIA_REQUEST_ID_LAST may error.  It may find units
*	whose format is not the expected format.
*
************************************************************************
-*/

int		amUnitGet
(
    AM_UNIT_GET		selectType,	/* (in)  Type of selection.	*/
    AM_UNIT		*units,		/* (out) media_units info.	*/
    DBPROCESS		*dbproc,	/* (in)  The dbprocess to use.	*/
    ...					/* (in)  The optional arguments.*/
)
{
    AM_MEDIA	*amMediaPtr;
    va_list	args;
    char	charStr[7];
    char	inString[60];
    char	pattern[AM_MEDIA_UNIT_NAME_LEN];
    char	unitNameRoot[ AM_UNIT_NAME_ROOT_LEN];
    int		unitNumber;
    int		status;


    AM_CHECK_INIT;


    if ( selectType != AM_UG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    datediff( second, 'jan 1 1970', date_completed), \n"
		"    media_request_id, \n"
		"    media_unit_name, \n"
		"    status, \n"
		"    total_size, \n"
		"    uncomp_size, \n"
		"    copies_written \n"
		"from \n"
		"    %s..%s \n"
		"where \n",
		localAm.laDatabase,
		localAm.laMedUnitsTable ) );

	switch ( selectType )
	{
	    case AM_UG_MEDIA_REQUEST_ID:
		va_start( args, dbproc );
		units->amuMediaRequestId =  (int ) va_arg( args, int );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_request_id= %d \n",
			units->amuMediaRequestId ) );
		break;

	    case AM_UG_MEDIA_REQUEST_ID_LAST:
		va_start( args, dbproc );
		units->amuMediaRequestId =  (int ) va_arg( args, int );
		(void) strcpy( unitNameRoot,
			(char *) va_arg( args, char *) );
		va_end( args );

		(void) sprintf( inString, "convert( int, substring"
			"( media_unit_name, %d, %d ) )",
			strlen( unitNameRoot ) + 1, AM_MEDIA_UNIT_NAME_LEN );
		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_request_id = %d \n"
			"    and media_unit_name like '%s%%'\n"
			"having media_request_id = %d \n"
			"    and media_unit_name like '%s%%'\n"
			"     and %s = max( %s ) \n",
			units->amuMediaRequestId, unitNameRoot,
			units->amuMediaRequestId, unitNameRoot,
			inString, inString ) );
		break;

	    case AM_UG_MEDIA_REQUEST_ID_NAME:
		va_start( args, dbproc );
		units->amuMediaRequestId =  (int ) va_arg( args, int );
		(void) strcpy( units->amuMediaUnitName,
		        (char *) va_arg( args, char *) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_request_id = %d \n"
			"    and media_unit_name = '%s' \n",
			units->amuMediaRequestId, 
			units->amuMediaUnitName ) );
		break;

	    case AM_UG_MEDIA_UNIT_NAME:
		va_start( args, dbproc );
		(void) strcpy( units->amuMediaUnitName,
		        (char *) va_arg( args, char *) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_unit_name = '%s' \n",
			units->amuMediaUnitName ) );
		break;

	    case AM_UG_NOTIFIABLE:
		va_start( args, dbproc );
		amMediaPtr = (AM_MEDIA *) va_arg( args, AM_MEDIA * );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_request_id = %d \n"
			"    and status = '%c' \n",
			amMediaPtr->ammMediaRequestId,
			AMU_CLEANED_UP ) );
		break;

	    case AM_UG_STATUS_AFTER_NAME:
		va_start( args, dbproc );
		units->amuMediaRequestId =  (int ) va_arg( args, int );
		units->amuStatus = (char) va_arg( args, int );
		(void) strcpy( pattern,	(char *) va_arg( args, char *) );
		(void) strcpy( unitNameRoot,
			(char *) va_arg( args, char *) );
		va_end( args );


		AM_CHECK_DB( dbfcmd( dbproc,
			"    media_request_id = %d \n"
			"    and status = '%c' \n",
			units->amuMediaRequestId, units->amuStatus ) );
			
		if ( strne( pattern, "" ) )
		{
		    /*
		     * If pattern is not "" then find all units
		     * which are greater than the given one.
		     */
		     
		    (void) sprintf( inString, "convert( int, substring"
			    "( media_unit_name, %d, %d ) )",
			    strlen( unitNameRoot ) + 1,
			    AM_MEDIA_UNIT_NAME_LEN );
		    if ( !str2int( pattern + strlen( unitNameRoot ) + 1,
			    &unitNumber ) )
		    {
			amFormatMessage( AM_E_UNITNAME, pattern,
				unitNameRoot );
			return( AM_E_UNITNAME );
		    }

		    AM_CHECK_DB( dbfcmd( dbproc,
			    "having media_request_id = %d \n"
			    "    and status = '%c'"
			    "    and %s > %d  \n",
			    units->amuMediaRequestId, units->amuStatus,
			    inString, unitNumber ) );
		}
		break;

	    case AM_UG_STATUS:
		va_start( args, dbproc );
		(void) sprintf( inString, "%s", "(" ); 
		while( ( units->amuStatus = (char) va_arg( args, int ) ) 
			!= 0 )
		{
		    (void) sprintf( charStr, " '%c',", units->amuStatus );
		    strcat( inString, charStr );
		}
		inString[strlen(inString)-1] = '\0';
		strcat( inString, " )" );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"     status in %s \n",
			inString ) );
		break;

	    default:
		amFormatMessage( AM_E_UNITSGET_ENUM, selectType );
		return( AM_E_UNITSGET_ENUM );
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_UNITS );
    }

    AM_CHECK_DB( status );

    if ( ( status = db_intdata( dbproc, 1, 
	    (int *) &( units->amuDateCompleted ) ) ) == DB_NULL_VALUE )
    {
	units->amuDateCompleted = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }

    AM_CHECK_DB_LIB( db_intdata( dbproc, 2, 
	    (int *) &( units->amuMediaRequestId ) ) );
    
    AM_CHECK_DB_LIB( db_stringdata( dbproc, 3, 
	    units->amuMediaUnitName, sizeof( units->amuMediaUnitName ) ) );

    AM_CHECK_DB_LIB( db_chardata( dbproc, 4, &units->amuStatus ) );


    AM_CHECK_DB_LIB( db_intdata( dbproc, 5, &units->amuTotalSize ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 6, &units->amuUncompSize ) );

    AM_CHECK_DB_LIB( db_intdata( dbproc, 7, &units->amuCopiesWritten ) );

    if ( selectType == AM_UG_MEDIA_REQUEST_ID_LAST )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amUnitPut
*
*   Purpose:
*	Insert a file into the media_units table.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*       int     AM_E_INIT       : Library is not initialized.
*
************************************************************************
-*/

int		amUnitPut
(
    AM_UNIT	*units,		/* (in)  The file to insert.		*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		dbstatus;

    AM_CHECK_INIT;

    AM_CHECK_DB( dbfcmd( dbproc, 
	    "declare \n"
	    "    @id int, \n"
	    "    @name varchar(%d) \n"
	    "select \n"
	    "    @id = %d, \n"
	    "    @name = '%s' \n"
	    "delete \n"
	    "    %s..%s \n"
	    "where \n"
	    "	 media_request_id = @id \n"
	    "	 and media_unit_name = @name \n"
	    "insert \n"
	    "    %s..%s \n"
	    "( \n"
	    "	 copies_written, \n"
	    "    media_request_id, \n"
	    "    media_unit_name, \n"
	    "    status, \n"
	    "    total_size, \n"
	    "    uncomp_size, \n"
	    "    date_completed ) \n"
	    "values ( \n"
	    "    %d, \n"
	    "    @id, \n"
	    "    @name, \n"
	    "    '%c', \n"
	    "    %d, \n"
	    "    %d, \n",
	    AM_MEDIA_UNIT_NAME_LEN,
	    units->amuMediaRequestId, 
	    units->amuMediaUnitName, 
	    localAm.laDatabase, localAm.laMedUnitsTable,
	    localAm.laDatabase, localAm.laMedUnitsTable,
	    units->amuCopiesWritten,
	    units->amuStatus,
	    units->amuTotalSize,
	    units->amuUncompSize ) );
    

    /*
     *  Complete the put command.
     */
	       
    if ( units->amuDateCompleted == 0 )
    {
	AM_CHECK_DB( dbfcmd( dbproc,
		"    NULL ) \n" ) );
    }
    else
    {
	AM_CHECK_DB( dbfcmd( dbproc,
		"    '%s') \n",
		db_time2str( units->amuDateCompleted, DB_UTC ) ) );
    }


    AM_CHECK_DB( dbsqlexec( dbproc ) );
    while ( ( dbstatus = dbresults( dbproc ) ) == SUCCEED )
    {}
    AM_CHECK_DB( dbstatus );


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amUnitUpdate
*
*   Purpose:
*	Updates the fields in the media units table. Generally,
*	this routine should be used to update more than one 
*	tuple in the db. For updating one record, amUnitPut 
*	should be used. 
*
*       int	mediaId;
*       amUnitUpdate( AM_UU_COMPLETE_USER, DBPROCESS *, mediaId );
*		- used in mediaNotify to mark units as complete. 
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB	 	: Error in the sybase db lib.
*	int	AM_E_UNITUPDATE_ENUM
*				: Unit update type is not valid.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/
int		amUnitUpdate
(
    AM_UNIT_UPDATE	selectType,	/* (in)  Type of selection.	 */
    DBPROCESS		*dbproc,	/* (in)  Dbprocess to use.	 */
    ...					/* (in)  The optional arguments. */
)
{
    va_list     args;
    int		mediaRequestId;


    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );


    /* 
     *      Start the update command.
     */
    
    AM_CHECK_DB( dbfcmd( dbproc, 
            "update \n"
            "   %s..%s \n"
            "set \n",
            localAm.laDatabase, localAm.laMedUnitsTable ) );
    

    /*
     *	    Select the type of update.
     */
     
    switch ( selectType )
    {
	case AM_UU_COMPLETE_USER:
            va_start( args, dbproc );
            mediaRequestId= (int) va_arg( args, int );
            va_end( args );

	    AM_CHECK_DB( dbfcmd( dbproc,
		    "    status = '%c'         \n"
		    "where                     \n"
		    "    media_request_id = %d         \n",
		    AMU_DONE,
		    mediaRequestId ) );
	    break;

        default:
            amFormatMessage( AM_E_UNITUPDATE_ENUM, selectType );
            return( AM_E_UNITUPDATE_ENUM );
    }

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );
        
    return( AM_SUCCESS );
}

