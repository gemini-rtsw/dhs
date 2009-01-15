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
*   Module Name:	mediaWrite/src/init.c
*
*   Purpose:
*	Contains initialization routines for mediaWrite.
*
*   Routines:
*	static 	int 	dataStageInit	: Initializes data stage info.
*	int		globalsInit	: Initializes mediaWrite's global vars.
*	static 	int 	mediaUnitInit	: Initializes media unit info.
*	static 	int 	mediaInit	: Initializes media info.
*	int		mwInit		: Initializes mediaWrite.
*	int		mwGetRegister	: Get the staging area.
*     	static  int     scriptInit      : Initializes script names.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: init.c
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Shannon jaeger
*
*   Modification History:
*	98/09/09 SDJ    : Copied from cdWrite
*	98/10/22 SDJ	: Get script names from AM instead of config file.
*       98/11/23 SDJ  	: Made sure all pending queries are cancelled on
*                         start-up and shut-down.  Added device support.
*	98/12/17 SDJ	: Changed AM_DV_TYPE to AM_DG_TYPE.
*	98/01/24 SDJ	: Added initialization of the retrieval stage.
*	99/02/24 SDJ	: Fixed scriptInit()
*	99/03/08 NRH    : Added support for rollback.
*	99/03/14 SDJ	: Changed log file name.
*	99/04/14 NRH	: Added updating the action table.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */
 
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
 
 
/*
 ************************************************************************
 *  SYBASE DB libarary include files.
 ************************************************************************
 */
 
#include <sybfront.h>
#include <sybdb.h>
 
 
/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */
 
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "gen_config.h"
#include "gen_file.h"
#include "db.h"
#include "am.h"
 
 
/*
 ************************************************************************
 *  Media Write include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */


static 	int	dataStagesInit( void );
static	int	mediaInit( void );
static  int     mediaUnitInit( void );
static 	int	mwGetRegister( void );
static  int     scriptInit( void );

/*+
************************************************************************
*
*   Function:	dataStagesInit
*
*   Purpose:
*	Fetches the data stage paths for the media ID from the am
*	library.
*
*   Values Returned:
*	int	SUCCESS		: Normal Completion.
*	int	E_AM		: Error in the am library. 
*	int	E_DATA_STAGE	: Error with the data stage. 
*
************************************************************************
-*/

static	int	dataStagesInit
(
    void
)
{
    char        *archDataStage;	/* Return string from amDataStageGet(). */
    char        *mediaStagePath;/* Return string from amDataStageGet(). */
    int		returnStatus;	/* Status to save and return.		*/
    struct stat stat_struct;	/* Return struct from file_dir().	*/
    char        *userDataStage;	/* Return string from amDataStageGet(). */
 

    returnStatus = S_SUCCESS;
    
    CHECK_AM( amDataStageGet( &archDataStage, &userDataStage, 
	    &mediaStagePath ) );

    if ( streq( globals.gMedia.ammMediaRequestType, AM_RT_ARCHIVE ) )
    {
	(void) sprintf( globals.gRetrievalStagePath, "%s", archDataStage );
    }
    else if ( streq( globals.gMedia.ammMediaRequestType, AM_RT_USER ) )
    {
	(void) sprintf( globals.gRetrievalStagePath, "%s", userDataStage );
    }
    else
    {
	formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
		"Invalid queue type" );
	printMessage( msg, MESS_ERROR );
	return( E_DATA_STAGE );
    }
 
    (void) sprintf( globals.gMediaStagePath, "%s", mediaStagePath );
    if ( ! file_dir( globals.gMediaStagePath, &stat_struct ) )
    {
	formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
		"file_dir() failed for media data stage path" );
	printMessage( msg, MESS_ERROR );
	returnStatus = E_DATA_STAGE;
    }
    CHECK( returnStatus );
 

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	globalsInit
*
*   Purpose:
*	Initialize mediaQueue's global variables.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Error - out of memory.
*
************************************************************************
-*/

int		globalsInit
(
    void
)
{
    globals.gDbProc1 			= NULL;
    globals.gDbProc2 			= NULL;

    globals.gLogFp 			= NULL;

    globals.gScriptConfigFPath[0]       = '\0'; /* may not need */
    globals.gScriptLogFileName[0]       = '\0';
    globals.gScript                     = NULL; /* may not need */
    globals.gPidList			= NULL;
    
    globals.gMediaStagePath[0] 		= '\0';
    globals.gMDSDirectory[0] 		= '\0';
    globals.gMediaUnitName[0] 		= '\0';
    globals.gMediaId			= 0;
    globals.gNumCopies			= 0;

    globals.gLogicalDeviceName[0]	= '\0';
    globals.gDeviceList			= NULL;
    
    globals.gForce			= FALSE;
    globals.gRollback			= FALSE;
    
    globals.gNumDirs			= 0;
    globals.gNumFiles			= 0;
    globals.gNumInMDS			= 0;
    globals.gNumInDS			= 0;
    globals.gNumNeedReRetrieving	= 0;
    globals.gNumBadStatus		= 0;

    globals.gVerbose 			= FALSE;
    globals.gVersion 			= FALSE;


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mediaInit
*
*   Purpose:
*	Reads the media struct from the media table and verify that
*	there is at least one device of the given media type.
*
*   Values Returned:
*	int	SUCCESS		: Normal Completion.
*	int	E_AM		: Error in am library function.
*	int	E_NO_MEDIA	: Media not found in table.
*
************************************************************************
-*/

static	int	mediaInit
(
    void
)
{
    AM_DEVICE	amDevice;	/* AM. device structure.		*/
    int		amStatus;	/* Ret. status from amGet().		*/


    amStatus = amGet( AM_MG_MEDIA_REQUEST_ID, &globals.gMedia,
	    globals.gDbProc1, globals.gMediaId  );
    if ( amStatus == AM_NO_MEDIA )
    {
        formatMessage( E_NO_MEDIA, globals.gMediaId );
        printMessage( msg, MESS_ERROR );
        return( E_NO_MEDIA );
    }
    CHECK_AM( amStatus );

    
    /*
     * Make sure there is at least one device of the given type.
     */
	
    amStatus = amDeviceGet( AM_DG_TYPE, &amDevice, globals.gDbProc1,
	    globals.gMedia.ammMediaType  );

    if ( amStatus == AM_NO_DEVICE )
    {
	formatMessage( E_NO_DEVICE, globals.gMedia.ammMediaType );
	printMessage( msg, MESS_ERROR );
	return( E_NO_MEDIA );
    }
    CHECK_AM( amStatus );

    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mediaUnitInit
*
*   Purpose:
*	Using the command line parameter media unit name, fetches the
*	information from the db and makes sure this one is in an OK
*	state to process.
*
*   Values Returned:
*	int	SUCCESS		: Normal Completion.
*	int	E_AM		: Error in am library routine.
*	int	E_UNIT_STATUS	: Cannot process units with this status. 
*	int	E_NO_ROLLBACK	: Rollback isn't possible.
*	int	E_NO_UNIT	: Media unit not found in db. 
*
************************************************************************
-*/

static	int	mediaUnitInit
(
    void
)
{
    int		unitStatus;	/* Return status from amUnitGet().	*/
    char        amString[AM_STATUS_SHORT_LEN];  
				/* Return string from amStatusStr().    */


    /*
     *  Get unit struct from db. 
     */
 
    unitStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID_NAME, &globals.gMediaUnit,
            globals.gDbProc1, globals.gMediaId, globals.gMediaUnitName );
    if ( unitStatus == AM_NO_UNITS )
    {
	formatMessage( E_NO_UNIT, globals.gMediaId, 
		globals.gMediaUnitName );
	printMessage( msg, MESS_ERROR );
        return( E_NO_UNIT );
    }
    CHECK_AM( unitStatus );
 

    /*
     *  Is the unit in a correct state to continue?
     */
 
    if ( globals.gRollback )
    {
	if ( ( globals.gMediaUnit.amuStatus != AMU_PREPARED && 
		globals.gMediaUnit.amuStatus != AMU_WRITTEN ) ||
		globals.gMediaUnit.amuCopiesWritten == 0 )
	{
	    formatMessage( E_NO_ROLLBACK, globals.gMediaUnit.amuMediaRequestId,
		    globals.gMediaUnit.amuMediaUnitName );
	    printMessage( msg, MESS_ERROR );
	    return( E_NO_ROLLBACK );
	}
    }
    else
    {
	if ( globals.gMediaUnit.amuStatus != AMU_PREPARED )
	{
	    CHECK_AM( amStatusStr( globals.gDbProc1, amString, "media_units",
		    globals.gMediaUnit.amuStatus ) );
	    formatMessage( E_BAD_U_STATUS, amString );
	    printMessage( msg, MESS_ERROR );
	    return( E_BAD_U_STATUS );
	}
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mwGetRegister
*
*   Purpose:
*	Finds the MDS directory registered to the given  media ID
*   	and media unit name.
*
*   Values Returned:
*	int	SUCCESS		: Normal Completion.
*
************************************************************************
-*/

static	int	mwGetRegister
(
    void
)
{
    int		amGetStatus;	/* Ret. status from amMSGet. 		*/
    char	directory[PATH_MAX];	
    				/* Full path to the MDS directory.	*/
    struct stat stat_struct;	/* Return struct from file_dir().	*/


    /*
     *  Check - is this unit & media name 'registered' in a directory. 
     */

    amGetStatus = amMSGet( AM_SG_REQUEST_ID_NAME, &globals.gMediaStage,
	    globals.gDbProc1, globals.gMediaId, globals.gMediaUnitName );
    if ( amGetStatus == AM_NO_MSTAGE )
    {
	/* 
	 *  Unit & media name 'un-registered'. Try to 'register'.
	 */

	formatMessage( E_UNIT_UNREG, globals.gMediaUnitName, 	
		globals.gMediaId );
	printMessage( msg, MESS_ERROR );
	return( E_UNIT_UNREG );
    }
    else 
    {
	CHECK_AM( amGetStatus );


	/*
	 *  Check - does the directory actually exist?
	 */

	(void) sprintf( globals.gMDSDirectory, "%s", 
		globals.gMediaStage.amsDirectory );
	(void) sprintf( directory, "%s/%s", globals.gMediaStagePath,
		globals.gMDSDirectory );

	if ( ! file_dir( directory, &stat_struct ) )
	{
	    formatMessage( MSG_ERRNO, directory );
	    printMessage( msg, MESS_ERROR );
	    return( E_SYSTEM );
	}


	formatMessage( S_BEGINNING, globals.gMediaUnitName );
	printMessage( msg, MESS_OPER );
	return( S_SUCCESS );
    }
}

/*+
************************************************************************
*
*   Function:	mwInit
*
*   Purpose:
*	Initializes the Media Write program.
*
*   Values Returned:
*	int	SUCCESS		: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*	int	E_DB_LIB	: Error in the CADC db library.
*	int	E_SYSTEM	: Error in system call. 
*
************************************************************************
-*/

int		mwInit
(
    void
)
{
    char	application[17];	/* String for application name.	*/
    char	logFile[PATH_MAX];	/* Log file name.		*/
    

    /*
     * Open script log,  APPLICATON_mediaId_mediaUnitName.log
     */
	
    (void) sprintf( logFile, "%s.%d.%s.log", APPLICATION,
	    globals.gMediaId, globals.gMediaUnitName );
    if ( ( globals.gLogFp = fopen( logFile, "a" ) ) == NULL )
    {
        formatMessage( MSG_ERRNO, logFile );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }
 

    /*
     *  Write the version information to the log file.
     */
 
    formatMessage( S_VERSION, version_short );
    printMessage( msg, MESS_LOG );
 


    /* 
     *  Initialize the CADC db library, print library init message.
     */

    CHECK( composeDbAppName( application, APP_SHORT, 
	    globals.gMediaId, globals.gMediaUnitName ) );
    CHECK_DB_LIB( db_init( application ) );


    /* 
     *  Initialize the astoMed library, print library init message.
     */

    CHECK_AM( amInit( ) );
    msg_append( msg, amMsg );
    msg_clear( amMsg );
    printMessage( msg, MESS_INFO );

 
    /*
     *  Ensure only one application is connected to db
     *  for this media ID. If OK - open db connection 2
     */

    CHECK( checkDbProc( &globals.gDbProc1 ) );
    CHECK( checkUnique( globals.gDbProc1, application ) );
    CHECK( checkDbProc( &globals.gDbProc2 ) );
    CHECK_DB( dbcanquery( globals.gDbProc1 ) );
    

    /*
     *  Fetch the Media table information. 
     */

    CHECK( mediaInit( ) );


    /*
     *  Initialize media unit structure.
     */

    CHECK( mediaUnitInit( ) );



    /*
     *  Initialize the data stage paths. 
     */

    CHECK( dataStagesInit( ) );


    /*
     *  Get the MDS directory the media ID and media unit name are
     *	registered with.
     */
     
    CHECK( mwGetRegister( ) );


    /*
     * Initialize the write script information.
     */

    CHECK( scriptInit( ) );


    /*
     *  Add the action to the action table.
     */

    if ( globals.gRollback )
    {
	CHECK_AM( amActionAdd( globals.gDbProc1, globals.gMediaId, 
		globals.gMediaUnitName, AM_MA_UNWRITE ) );
    }
    else
    {
	CHECK_AM( amActionAdd( globals.gDbProc1, globals.gMediaId, 
		globals.gMediaUnitName, AM_MA_WRITE ) );
    }

    
    return( S_SUCCESS );
}

/*+
 ************************************************************************
 *
 *   Function: scriptInit
 *
 *   Purpose:
 *     To get the script name for writing and its configuration file
 *     from AM.
 *
 *   Values Returned:
 *     int     SUCCESS         : Normal Completion.
 *     int     E_AM            : Error in am library routine.
 *
 ************************************************************************
 -*/


int   scriptInit
(
    void
)
{
    char    *configName;   /* Name of the configuration file.      	*/
    char    configRootName[ PATH_MAX ];
    			   /* Root name of the configuration file.	*/
    char    *configFile;   /* Name/Path of the configuration file.	*/
    int	    status;	   /* Status from config functions.		*/
    
    configFile = NULL;

	
    /*
     * Get the name of the script to execute and its configuration file.
     */
	      
    CHECK_AM( amScript( globals.gMedia.ammMediaType, AM_ST_WRITE,
	    &globals.gScript, &configName ) );

    if ( configName != NULL && strlen( configName ) > 0 &&
	    strne( configName, "none" ) )
    {
	(void) sprintf( configRootName, "%s", configName );
	strroot( configRootName );
	status = config_open( configRootName, &configFile );
	if ( status != GC_SUCCESS )
	{
	    formatMessage( E_CONFIG );
	    printMessage( msg, MESS_ERROR );
	    return( E_CONFIG );
	}
	else
	{
	    (void) sprintf( globals.gScriptConfigFPath, "%s", configFile );
	}
	

	/*
	 * Close configuration file.
	 */
    
	config_close( );
    }
    

    /*
     *  Find the log file path.
     */
					  
    (void) sprintf( globals.gScriptLogFileName, "%s/%s/MW.%d.%s.log",
	    globals.gMediaStagePath, globals.gMDSDirectory,
	    globals.gMediaId, globals.gMediaUnitName );


    if ( configFile != NULL )
    {
	gen_free( configFile );
	configFile = NULL;
    }

    return( S_SUCCESS );
}
