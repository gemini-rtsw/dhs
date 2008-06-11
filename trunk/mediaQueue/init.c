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
*   Module Name:	mediaQueue/src/init.c
*
*   Purpose:
*	Contains initialization routines for the mediaQueue application.
*
*   Routines:
*	static int 	dataStageInit	: Initializes data stage info.
*	int		globalsInit	: Initializes mediaQueue's globals.
*	static int 	mediaInit	: Initializes media info.
*	static int 	mediaOptInit	: Gets media option info.
*	static int 	mediaTypeInit	: Initializes media type info.
*	int		mqInit		: Initializes mediaQueue.
*
*   Date		: <Jan 28, 1998>
*
*   SCCS data		: @(#)
*	Module Name	: init.c
*	Version Number	: 1.14
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/02/27 SDJ	: Updated to new am library.
*	98/12/06 SDJ	: Upgraded to new AM library, mediaTypes table.
*	98/12/17 SDJ	: Changes AMT_TYPE to AM_TG_TYPE.
*	99/01/06 SDJ	: Use gen_file instead of stat() and UNIX file
*			  functions.
*	99/01/22 SDJ	: Added variable  globals.gNumMediaFound.
*	99/03/05 NRH	: Added variable globlas.gRollback.
*	99/04/29 NRH	: Added updating the actions table.
*     2000/02/03 SDJ	: Added new global vars, gCheckSize, gCheckDate,
*			  gPack, and gCmdLineArgs.  Added mediaOptInit().
*     2000/03/17 SDJ	: Added globals.gMaxUnitSize and globals.gSplitSize.
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
#include "gen_eptr.h"
#include "gen_msg.h"
#include "gen_file.h"
#include "db.h"
#include "am.h"


/*
 ************************************************************************
 *  Media Queue  include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */

static	int	mediaInit( void );
static 	int	mediaOptInit( void );
static 	int	mediaTypeInit( void );
static 	int	dataStageInit( void );

/*+
************************************************************************
*
*   Function:	dataStageInit
*
*   Purpose:
*	Reads the data stage path from the am library.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal Completion.
*	int	E_AM		: Error in the CADC am library.
*	int	E_DATA_STAGE	: Error with the data stage. 
*
************************************************************************
-*/

static	int	dataStageInit
(
    void
)
{
    char	*archDataStage;	/* Return string from amDataStageGet().	*/
    struct stat stat_struct;    /* Return struct from file_dir().	*/
    char	*userDataStage;	/* Return string from amDataStageGet().	*/


    CHECK_AM( amDataStageGet( &archDataStage, &userDataStage, NULL ) );

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
		"Not in am library" );
	printMessage( msg, MESS_ERROR );
	return( E_DATA_STAGE );
    }


    /*
     *  Use file_dir() to check the validity of the path.
     */

    if ( ! file_dir( globals.gDataStage, &stat_struct ) )
    {
        formatMessage( E_DATA_STAGE, globals.gMedia.ammMediaRequestId,
                "file_dir() failed for path" );
        printMessage( msg, MESS_ERROR );
        return( E_DATA_STAGE );
    }

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
*	int	E_MEMORY	: Memory allocation error.
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

    globals.gDataStage[0]		= '\0';
    globals.gMaxUnitSize		= 0;
    globals.gSplitSize			= 0;
	
    globals.gMediaId			= -1;
    globals.gMediaType.amtTypeName[0]	= '\0';    
    
    globals.gTreeInfo			= NULL;
    
    globals.gNumMedia			= 0;
    globals.gNumMediaFound		= 0;
    
    globals.gForce 			= FALSE;
    globals.gVerbose 			= FALSE;
    globals.gVersion			= FALSE;
    globals.gRollback			= FALSE;

    globals.gCheckSize			= T_MAYBE;
    globals.gCheckDate			= T_MAYBE;
    globals.gPack			= T_MAYBE;


    CHECK_NULL( globals.gCmdLineArgs = char_alloc( PATH_MAX ) );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mediaInit
*
*   Purpose:
*	Reads the media struct from the media table. 
*
*   Values Returned:
*	int	S_SUCCESS	: Normal Completion.
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


    amStatus = amGet( AM_MG_MEDIA_REQUEST_ID, &globals.gMedia,
	    globals.gDbProc1, globals.gMediaId  );
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
*   Function:	mediaOptInit
*
*   Purpose:
*	Reads the media Option information from the database and
*	adjusts globals.gCheckNoSize, globals.gCheckNoDate,  and
*	globals.gPack based on the results 
*
*   Values Returned:
*	int	S_SUCCESS	: Normal Completion.
*	int	E_AM		: Error in am library function.
*	int	E_MEMORY	: Memory allocation error.	
*
************************************************************************
-*/

static	int	mediaOptInit
(
    void
)
{
    int		amStatus;	/* Ret. status from amGet().		*/
    int		argc;		/* Number of arguments.			*/
    char	*argv[MAX_ARGS];/* Argument values.			*/
    AM_MEDIA_OPTIONS
    		amMediaOptions; /* Media options for this request.	*/

    amMediaOptions.amoMediaRequestId = globals.gMediaId;
    (void) sprintf( amMediaOptions.amoProcessName, APPLICATION );
    
    amStatus = amMediaOptionsGet( globals.gDbProc1, &amMediaOptions );
    if ( amStatus == AM_NO_MEDIA_OPTIONS )
    {
	return( S_SUCCESS );
    }
    CHECK_AM( amStatus );
    

    /*
     * Update global variables by parsing the options from the
     * database.
     */

    CHECK_NULL( globals.gDbArgs = strsav( amMediaOptions.amoOptions ) );
    strtokens( amMediaOptions.amoOptions, MAX_ARGS, argv, &argc );
    CHECK( parseArgs( DATABASE_ARGS, argc, argv ) );
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mediaTypeInit
*
*   Purpose:
*	Reads the media type from the am library. Performs calculations
*	w.r.t. block size, etc.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal Completion.
*	int	E_AM		: Error in am library function.
*	int	E_MEDIA_TYPE	: Media type not found. 
*	int	E_DB_USAGE	: Incorrect options in database.
*	int	E_ARG_TYPE	: Invalid arg. parsing type given.
*
************************************************************************
-*/

static	int	mediaTypeInit
(
    void
)
{
    int		typeStatus;	/* Ret. status from amMedTypeGet.	*/



    typeStatus = amMediaTypeGet( AM_TG_TYPE, &globals.gMediaType,
	    globals.gDbProc1, globals.gMedia.ammMediaType );
    if ( typeStatus == AM_NO_TYPE )
    {
	formatMessage( E_MEDIA_TYPE, globals.gMedia.ammMediaType, 
	        globals.gMedia.ammMediaRequestId );
	printMessage( msg, MESS_ERROR );
	return( E_MEDIA_TYPE );
    }
    CHECK_AM( typeStatus );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mqInit
*
*   Purpose:
*	Initialization for mediaQueue.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*	int	E_DATA_STAGE	: Error with the data stage. 
*	int	E_DB_LIB	: Error in the CADC db library.
*	int	E_MEDIA_TYPE	: Media type not found. 
*	int	E_NO_MEDIA	: Media not found in table.
*	int	E_ONLY_ONE	: Only one executable of this type allowed.
*	int	E_SYSTEM	: Error in the operating system.
*
************************************************************************
-*/

int		mqInit
(
    void
)
{
    char	logFile[PATH_MAX];	/* Log file name.		*/
    char	application[17];	/* String for application name.	*/


 
    (void) sprintf( logFile, "%s.%d.log", APPLICATION, globals.gMediaId );
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

    (void) sprintf( application, "%s%d", APP_SHORT, 
	    globals.gMediaId );
    CHECK_DB_LIB( db_init( application ) );
    msg_append( msg, db_msg );
    msg_clear( db_msg );
    printMessage( msg, MESS_INFO );


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
     *  Also cancel any pending queries.
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
     * Fetch the media options information and adjust global
     * variables, if necessary.
     */

    CHECK( mediaOptInit() );


    /*
     *  Initialize media type structure, and file & dir overhead 
     *  block counts. 
     */

    CHECK( mediaTypeInit( ) );


    /*
     *  Initialize the data stage path. 
     */

    CHECK( dataStageInit( ) );


    /*
     * Add the ingest or rollback action to the action table.
     */

    if ( globals.gRollback )
    {
	CHECK_AM( amActionAdd( globals.gDbProc1, globals.gMediaId, NULL, 
		AM_MA_UNQUEUE ) );
    }
    else
    {
	CHECK_AM( amActionAdd( globals.gDbProc1, globals.gMediaId, NULL, 
		AM_MA_QUEUE ) );
    }


    /*
     * Set the Maximum unit size now that we have the info from the
     * database.
     */

    globals.gMaxUnitSize = globals.gMediaType.amtMaxSize;
    globals.gMaxUnitSize = CONV_TO_BLOCKS( globals.gMaxUnitSize * 1024 );

    globals.gSplitSize = globals.gMediaType.amtSplitSize;
    globals.gSplitSize = CONV_TO_BLOCKS( globals.gSplitSize * 1024 );
    
    return( S_SUCCESS );
}
