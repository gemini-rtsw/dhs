/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1995.
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
*   Module Name:	mediaCleanup/src/init.c
*
*   Purpose:
*	Initialization routines for mediaCleanup.
*
*   Routines:
*	static	int	checkRegistered	: Ensures registration is OK.
*	static 	int	dataStagesInit	: Initialize data stage paths. 
*	int		globalsInit	: Initialize the globals structure.
*	int		mcInit		: Initialization for mediaCleanup.
*	static	int	mediaInit	: Initialize media struct.
*	static	int	mediaUnitInit	: Initialize media unit struct.
*
*   Date		: Dec 12, 1996
*
*   SCCS data		: @(#)
*	Module Name	: init.c
*	Version Number	: 1.14
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Marc LeBlanc
*			: Steve Cockayne
*			: Shannon Jaeger
*
*   Modification History:
*	97/02/03 SEC  : Fixed db opening bug - incorrect order of functions.
*	97/02/04 SEC  : Fixed bug in checkRegistered().
*	97/02/04 SEC  : Mod'ed log file name.
*	97/02/10 SEC  : Removed extra S_VERSION message from log file.
*	97/02/21 SEC  : Added event logging, fixed some bugs. 
*       97/03/18 SEC  : Mod'ed for change to al, am libs to fetch db
*                       server name in the a*DbInfoGet call.
*	98/08/24 SDJ  : Upgraded to new AM library. Remove Al library.
*	98/10/26 SDJ  : Added initialization of media stage.
*	99/01/07 SDJ  : Switched from UNIX file functions to ones in
*			gen_file.
*	99/02/24 SDJ  : globalsInit now initializes all globals.  No
*			longer an error if no media staging directory
*			is found.
*	99/03/17 NRH  : Added the rollback option.
*	99/04/13 NRH  : Changed to append to the log file. Changed to
*			continue cleanup if there is no staging directory
*			assigned.
*	99/04/29 NRH  : Changed to update the actions table.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <errno.h>
#include <sys/stat.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "gen_file.h"
#include "db.h"
#include "am.h"

#include "globals.h"

static 	int	checkRegistered( void );
static	int	dataStagesInit( void );
static  int     mediaInit( void );
static  int     mediaUnitInit( void );


/*+
************************************************************************
*
*   Function:   checkRegistered
*
*   Purpose:
*	Checks if the unit name and media id are registered to an
*	MDS directory.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal Completion.
*	int	E_AM		: Error in am library.
*	int	S_NO_MSTAGE	: No db tuple matches query
*
************************************************************************
-*/
 
static  int     checkRegistered
(
    void
)
{
    int         amGetStatus;    /* Ret. status from amMSGet.            */
    AM_MSTAGE   amMStage;       /* The media stage structure.           */
 
 
    /*
     *  Check - is this unit & media name 'registered' in a directory. 
     */
    
    amGetStatus = amMSGet( AM_SG_REQUEST_ID_NAME, &amMStage, globals.gDbProc, 
            globals.gMediaId, globals.gMediaUnitName );         

    if ( amGetStatus != AM_NO_MSTAGE )
    {
        CHECK_AM( amGetStatus );
 
 
        /*
         *  amMSGet() returned AM_SUCCESS - get dir name from struct. 
         */
	
        (void) sprintf( globals.gMDSDirectory, "%s", amMStage.amsDirectory );
    }
    else
    {
	formatMessage( S_NO_MSTAGE, globals.gMediaId, globals.gMediaUnitName );
	printMessage( msg, MESS_ERROR );
    }
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   dataStagesInit
*
*   Purpose:
*       Fetches the media & data stage path from the am library.
*
*   Values Returned:
*       int     SUCCESS         : Normal Completion.
*       int     E_AM            : Error in the am library. 
*       int     E_DATA_STAGE    : Error with the data stage. 
*
************************************************************************
-*/
 
static  int     dataStagesInit
(
    void
)
{
    char        *archDataStage; /* Return string from amDataStageGet(). */
    char        *mediaDataStage;/* Return string from amDataStageGet(). */
    int         returnStatus;   /* Status to save and return.           */
    struct stat stat_struct;    /* Return struct from file_dir().	*/
    char        *userDataStage; /* Return string from amDataStageGet(). */
 
 
    CHECK_AM( amDataStageGet( &archDataStage, &userDataStage, 
            &mediaDataStage ) );
 
    if ( streq( globals.gMedia.ammMediaRequestType, AM_RT_ARCHIVE ) )
    {
        (void) sprintf( globals.gDataStage, "%s", archDataStage );
    }
    else if ( streq( globals.gMedia.ammMediaRequestType, AM_RT_USER ) )
    {
        (void) sprintf( globals.gDataStage, "%s", userDataStage );
    }
    else
    {
        formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
                "Invalid queue type" );
        printMessage( msg, MESS_ERROR );
        return( E_DATA_STAGE );
    }
 
 
    /*
     *  Use file_dir() to check the validity of the two paths.
     */
 
    returnStatus = S_SUCCESS;
    if ( ! file_dir( globals.gDataStage, &stat_struct ) )
    {
	formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
		"file_dir() failed for path" );
	if ( errno != 0 )
	{
	    formatMessage( MSG_ERRNO, globals.gDataStage );
	}
        printMessage( msg, MESS_ERROR );
        returnStatus = E_DATA_STAGE;
    }
 
    (void) sprintf( globals.gMediaDataStage, "%s", mediaDataStage );
    if ( ! file_dir( globals.gMediaDataStage, &stat_struct )  )
    {
        formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
                "file_dir() failed for media data stage path" );
	if ( errno != 0 )
	{
	    formatMessage( MSG_ERRNO, globals.gMediaDataStage );
	}
        printMessage( msg, MESS_ERROR );
        returnStatus = E_DATA_STAGE;
    }
    
    return( returnStatus );
}

/*+
************************************************************************
*
*   Function:	globalsInit
*
*   Purpose:
*	Initialize the globals structure.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		globalsInit
(
    void
)
{
    globals.gDbProc 			= NULL;
    globals.gLogFp 			= NULL;

    globals.gDataStage[0]		= '\0';
    globals.gMDSDirectory[0]		= '\0';
    globals.gMediaDataStage[0]		= '\0';

    globals.gMedia.ammMediaRequestId	= -1;
    globals.gMediaId			= -1;

    globals.gMediaUnit.amuMediaUnitName[0] = '\0';
    globals.gMediaUnitName[0]		= '\0';


    globals.gRollback			= FALSE;
    globals.gVerbose 			= FALSE;
    globals.gVersion 			= FALSE;
	
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mcInit
*
*   Purpose:
*	Initialization for mediaCleanup.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in am library.
*	int	E_NO_MEDIA	: No db tuple matches query.
*	int	E_UNIT_STATUS	: Cannot process units with this status.
*	int	E_NO_SUCH_UNIT	: Media unit not found in db.
*	int	E_NO_MSTAGE	: Media stage not found in db.
*
************************************************************************
-*/

int		mcInit
(
    void
)
{
    char        application[17];/* String for application name.         */


    (void) sprintf( globals.gLogFile, "%s.%d.%s.log", APPLICATION, 
	    globals.gMediaId, globals.gMediaUnitName );
    if ( ( globals.gLogFp = fopen( globals.gLogFile, "a" ) ) == NULL )
    {
	formatMessage( MSG_ERRNO, globals.gLogFile );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }

    /*
     *  Write the version information to the log file.
     */

    formatMessage( S_VERSION, version_short );
    printMessage( msg, MESS_LOG );


    /* 
     *  Initialize the CADC db library.
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
     *  Open db connection.  Ensure only one is running. 
     */


    CHECK( checkDbProc( &globals.gDbProc ) );
    CHECK( checkUnique( globals.gDbProc, application ) );

 
    /*
     *  Initialize media structure. 
     */
 
    CHECK( mediaInit( ) );
 
 
    /*
     *  Initialize media unit structure.
     */
 
    CHECK( mediaUnitInit( ) );


    /*
     *  Make sure we're registered. 
     */
     
    if ( ! globals.gRollback )
    {
	CHECK( checkRegistered( ) );
    }


    /*
     *  Initialize the data stage paths.
     */
    
    CHECK( dataStagesInit( ) );


    if ( globals.gRollback )
    {
	/*
	 *  Add the action to the database.
	 */

	CHECK_AM( amActionAdd( globals.gDbProc, globals.gMediaId, 
		globals.gMediaUnitName, AM_MA_UNCLEANUP ) );
    }
    else
    {
	/*
	 *  Add the action to the database.
	 */

	CHECK_AM( amActionAdd( globals.gDbProc, globals.gMediaId, 
		globals.gMediaUnitName, AM_MA_CLEANUP ) );
    }
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   mediaInit
*
*   Purpose:
*       Reads the correct media struct from the media table. 
*
*   Values Returned:
*       int     SUCCESS         : Normal Completion.
*       int     E_AM            : Error in am library function.
*       int     E_NO_MEDIA      : Media not found in table.
*
************************************************************************
-*/
 
static  int     mediaInit
(
    void
)
{
    int         amStatus;       /* Ret. status from amGet().            */
 
 
    amStatus = amGet( AM_MG_MEDIA_REQUEST_ID, &globals.gMedia,
	    globals.gDbProc, globals.gMediaId );
    if ( amStatus == AM_NO_MEDIA )  
    {
        formatMessage( E_NO_MEDIA, globals.gMediaId );
        printMessage( msg, MESS_ERROR );
        return( E_NO_MEDIA );
    }
    CHECK_AM( amStatus );
    

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   mediaUnitInit
*
*   Purpose:
*       Using the command line parameter media unit name, fetches the
*       information from the db and makes sure this one is in an OK
*       state to process.
*
*   Values Returned:
*       int     SUCCESS         : Normal Completion.
*       int     E_AM            : Error in am library routine.
*       int     E_UNIT_STATUS	: Cannot process units with this status. 
*       int     E_NO_SUCH_UNIT  : Media unit not found in db. 
*
************************************************************************
-*/
 
static  int     mediaUnitInit
(
    void
)
{
    int         unitStatus;     /* Return status from amUnitGet().      */
    char        amString[AM_STATUS_SHORT_LEN];
    				/* Return string from amStatusStr().    */
 
 
    /*
     *  Get unit struct from db. 
     */
 
    unitStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID_NAME, &globals.gMediaUnit,
            globals.gDbProc, globals.gMediaId, globals.gMediaUnitName );
    if ( unitStatus == AM_NO_UNITS )
    {
	formatMessage( E_NO_SUCH_UNIT, globals.gMediaId, 
		globals.gMediaUnitName );
	printMessage( msg, MESS_ERROR );
        return( E_NO_SUCH_UNIT );
    }
    CHECK_AM( unitStatus );
 
 
    /*
     *  Unit must have status AMU_WRITTEN, or if rollback is being done,
     *  the status must be AMU_CLEANED_UP or AMU_DONE.
     */
 
    if ( ( globals.gRollback && 
	    ( globals.gMediaUnit.amuStatus != AMU_CLEANED_UP &&
	    globals.gMediaUnit.amuStatus != AMU_DONE ) ) || 
	    ( ! globals.gRollback && 
	    globals.gMediaUnit.amuStatus != AMU_WRITTEN ) )
    {
	CHECK_AM( amStatusStr( globals.gDbProc, amString, "media_units",
		globals.gMediaUnit.amuStatus ) );
        formatMessage( E_UNIT_STATUS, amString );
        printMessage( msg, MESS_ERROR );
        return( E_UNIT_STATUS );
    }
 
    return( S_SUCCESS );
}

