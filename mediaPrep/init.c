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
*   Module Name:	mediaPrep/src/init.c
*
*   Purpose:
*	Contains initialization routines for mediaPrep.
*
*   Routines:
*	static 	int 	dataStageInit	: Initializes data stage info.
*	int		globalsInit	: Initializes mediaPrep's global vars.
*	static 	int 	mediaUnitInit	: Initializes media unit info.
*	static 	int 	mediaInit	: Initializes media info.
*	int		mpInit		: Initializes mediaPrep.
*	static	int	scriptInit	: Initialized prep script info.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: init.c
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/09/09 SDJ	: Copied from cdPrep
*	98/09/11 SDJ	: Added code to use scripts names in AM config file.
*       98/12/06 SDJ	: Upgraded to new AM, and added check for 
*                         "CD" devices.
*	98/12/17 SDJ	: Changed AM_DV_TYPE to AM_DG_TYPE.
*       99/01/06 SDJ  	: Use gen_file instead of stat() and other UNIX
*			  file functions.
*	99/02/24 SDJ	: Fixed scritpInit(). 
*	99/03/08 NRH	: Added support for rollback.
*	99/03/14 SDJ	: Changed the name of the log file.
*	99/03/25 SDJ	: scriptInit() now closes the config. file.
*	99/04/28 NRH	: Changed to update the actions table.
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
 *  Media prep  include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypes.
 ************************************************************************
 */

static 	int	mpRegister( void );
static 	int	mpGetDirectory( void );
static 	int	dataStagesInit( void );
static	int	mediaInit( void );
static  int     mediaUnitInit( void );
static	int	scriptInit( void );


/*+
************************************************************************
*
*   Function:	mpInit
*
*   Purpose:
*	Initializes the Media Prep program.
*
*   Values Returned:
*	int	SUCCESS		: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*	int	E_DB_LIB	: Error in the CADC db library.
*	int	E_SYSTEM	: Error in system call. 
*
************************************************************************
-*/

int		mpInit
(
    void
)
{
    char	logFile[PATH_MAX];	/* log file name.		*/
    char	application[17];	/* String for application name.	*/
 

    /*
     * Open log file,  APPLICATON_mediaId_mediaUnitName.log
     */
	
    (void) sprintf( logFile, "%s.%d.%s.log", APPLICATION, globals.gMediaId,
	    globals.gMediaUnitName );
    if ( ( globals.gLogFp = fopen( logFile, "a" ) ) == NULL )
    {
        if ( errno == 0 )
        {
            formatMessage( E_STAGE_DIR, logFile );
        }	
        else	
        {
            formatMessage( MSG_ERRNO, logFile );
        }
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
     *	Initialize the prepare script information.
     */

    CHECK( scriptInit( ) );

 
    if ( globals.gRollback )
    {
	/*
	 *  Get the directory from the database.
	 */

	CHECK( mpGetDirectory( ) );


	/*
	 *  Add the action to the database.
	 */

	CHECK_AM( amActionAdd( globals.gDbProc1, globals.gMediaId, 
		globals.gMediaUnitName, AM_MA_UNPREPARE ) );
    }
    else
    {
	/*
	 *  Add the action to the database.
	 */

	CHECK_AM( amActionAdd( globals.gDbProc1, globals.gMediaId, 
		globals.gMediaUnitName, AM_MA_PREPARE ) );


	/*
	 *  Register the media ID and media unit name to the MDS directory. 
	 */
	 
	CHECK( mpRegister( ) );
    }


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mpGetDirectory
*
*   Purpose:
*	Gets the media directory from the MDS directory.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*
************************************************************************
-*/

static int	mpGetDirectory
(
    void
)
{
    CHECK_AM( amMSGet( AM_SG_REQUEST_ID_NAME, &globals.gMediaStage, 
	    globals.gDbProc1, globals.gMediaId, globals.gMediaUnitName ) );
    (void) strcpy( globals.gMDSDirectory, globals.gMediaStage.amsDirectory );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mpRegister
*
*   Purpose:
*	Commits media ID and media unit name to the MDS directory.
*
*   Values Returned:
*	int	SUCCESS		: Normal Completion.
*
************************************************************************
-*/

static	int	mpRegister
(
    void
)
{
    int		amBookStatus;	/* Ret. status from amMSBook. 		*/
    int		amGetStatus;	/* Ret. status from amMSGet. 		*/
    char	directory[PATH_MAX];	
    				/* Full path to the MDS directory.	*/
    struct stat stat_struct;	/* Return struct from file_dir().	*/


    /*
     *  Check - does the directory actually exist?
     */

    (void) sprintf( directory, "%s/%s", globals.gMediaStagePath,
	    globals.gMDSDirectory );
    if ( ! file_dir( directory, &stat_struct ) )
    {
        if ( errno == 0 )
        {
            formatMessage( E_STAGE_DIR, directory );
        }
        else
        {
            formatMessage( MSG_ERRNO, directory );
        }
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }


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

	globals.gMediaStage.amsMediaRequestId = globals.gMediaId;
	(void) strcpy( globals.gMediaStage.amsMediaUnitName,
		globals.gMediaUnitName );
	(void) strcpy( globals.gMediaStage.amsDirectory,
		globals.gMDSDirectory );
	amBookStatus = amMSBook( &globals.gMediaStage, globals.gDbProc1 );
	if ( amBookStatus == AM_CANT_MSBOOK )
	{
	    formatMessage( E_OTHER_UNIT, globals.gMDSDirectory );
	    printMessage( msg, MESS_ERROR );
	    return( E_OTHER_UNIT );
	}
	else
	{
	    CHECK_AM( amBookStatus );


	    /*
	     *  amMSPut successfully 'registered' unit & name in dir.
	     */
	    
	    formatMessage( S_BEGINNING, globals.gMediaUnitName );
	    printMessage( msg, MESS_OPER );
	    return( S_SUCCESS );
	}
    }
    else 
    {
	CHECK_AM( amGetStatus );


	/*
	 *  amMSGet returned AM_SUCCESS - inspect return struct. 
	 */

	if ( streq( globals.gMediaStage.amsDirectory, globals.gMDSDirectory ) )
	{
	    formatMessage( S_CONTINUING, globals.gMediaUnitName );
	    printMessage( msg, MESS_OPER );
	    return( S_SUCCESS );
	}
	else
	{
	    formatMessage( E_WRONG_DIR, globals.gMediaStage.amsDirectory );
	    printMessage( msg, MESS_ERROR );
	    return( E_WRONG_DIR );
	}
    }
}

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
    char        *mediaDataStage;/* Return string from amDataStageGet(). */
    int		returnStatus;	/* Status to save and return.		*/
    struct stat stat_struct;	/* Return struct from file_dir().	*/
    char        *userDataStage;	/* Return string from amDataStageGet(). */
 
 
    CHECK_AM( amDataStageGet( &archDataStage, &userDataStage, 
	    &mediaDataStage ) );
 
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
 
 
    /*
     *  Use stat() to check the validity of the two paths.
     */
 
    returnStatus = S_SUCCESS;
    if ( ! file_dir( globals.gRetrievalStagePath, &stat_struct ) )
    {
	if ( errno == 0 )
        {
            formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
                    "file_dir() failed for path" );
        }
        else
        {
            formatMessage( MSG_ERRNO, globals.gRetrievalStagePath );
        }
	printMessage( msg, MESS_ERROR );
	returnStatus = E_DATA_STAGE;
    }
 
    (void) sprintf( globals.gMediaStagePath, "%s", mediaDataStage );
    if ( ! file_dir( globals.gMediaStagePath, &stat_struct ) )
    {
	if ( errno == 0 )
        {
            formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
                    "file_dir() failed for media data stage path" );
        }
        else
        {
            formatMessage( MSG_ERRNO, globals.gMediaStagePath );
        }       
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
    
    globals.gScriptConfigFPath[0]	= '\0'; /* may not need */
    globals.gScriptLogFileName[0]	= '\0';
    globals.gScriptProcessId		= (pid_t) -1;
    globals.gScript 			= NULL; /* may not need */
    globals.gScriptKill			= FALSE;

    globals.gMediaStagePath[0] 		= '\0';
    globals.gRetrievalStagePath[0] 	= '\0';
    globals.gMDSDirectory[0] 		= '\0';
    globals.gMediaUnitName[0] 		= '\0';

    globals.gMediaId			= 0;
    globals.gNumFiles			= 0;
    globals.gNumInRS			= 0;
    globals.gNumQueued			= 0;
    globals.gNumNeedReRetrieving	= 0;
    globals.gNumBadStatus		= 0;

    globals.gVerbose 			= FALSE;
    globals.gVersion 			= FALSE;
    globals.gRollback			= FALSE;


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mediaInit
*
*   Purpose:
*	Reads the media struct from the media table. Also verify there
*	is a device for the given media.
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
    int		amStatus;	/* Ret. status from amGet().		*/
    AM_DEVICE	amDevice;	/* AM media Device structure.		*/
    


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
     *  globals.gMediaUnit.amuStatus != AMU_PREPARED &&
     */
 
    if ( ( ! globals.gRollback && 
	    globals.gMediaUnit.amuStatus != AMU_QUEUED ) ||
	    ( globals.gRollback && 
	    globals.gMediaUnit.amuStatus != AMU_PREPARED ) )
    {
	CHECK_AM( amStatusStr( globals.gDbProc1, amString, "media_units",
		globals.gMediaUnit.amuStatus ) );
	formatMessage( E_BAD_U_STATUS, amString );
	printMessage( msg, MESS_ERROR );
	return( E_BAD_U_STATUS );
    }


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	scriptInit
*
*   Purpose:
*	To get the script name and configuration file from AM.
*
*   Values Returned:
*	int	SUCCESS		: Normal Completion.
*	int	E_AM		: Error in am library routine.
*
*
************************************************************************
-*/

int	scriptInit
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
    CHECK_AM( amScript( globals.gMedia.ammMediaType, AM_ST_PREP,
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

    (void) sprintf( globals.gScriptLogFileName, "%s/%s/MP.%d.%s.log", 
	    globals.gMediaStagePath, globals.gMDSDirectory, 
	    globals.gMediaId, globals.gMediaUnitName );

    if ( configFile != NULL )
    {
	gen_free( configFile );
	configFile = NULL;
    }

    return( S_SUCCESS );
}
