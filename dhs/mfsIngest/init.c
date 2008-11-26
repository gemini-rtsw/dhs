/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 2000.				(c) 2000.
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
*   Module Name:	mfs/src/init.
*
*   Purpose:
*	Routines for initilizing the mfsIngest appliction.
*
*   Routines:
*	static int	archiveCheck	: Checks that the archive is valid.
*	static int	initDb		: Initialize resources for mfsingest.
*	static boolean	initGlobals	: Init. the global data structure.
*	int		initMfsIngest	: Initialize mfsIngest
*
*   Date		: Mar 07, 2000
*
*   SCCS data		: @(#)
*	Module Name	: init.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 05/24/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

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
#include "db.h"
#include "mfs.h"
#include "ad.h"


/*
 ************************************************************************
 *  mfsIngest include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 *  Static function prototypes.
 ************************************************************************
 */ 

static int	archiveCheck( void );
static int	initDb( void );

/*+
************************************************************************
*
*   Function:	archiveCheck
*
*   Purpose:
*	Checks that the given or the default archive is valid.
*
*   Values Returned:
*	int	S_SUCCESS	: All went well
*	int	E_NO_DEFAULT	: No default archive.
*	int	E_ARCHIVE	: Archive name not valid.
*
************************************************************************
-*/

int    archiveCheck
(
    void
)
{
    AD_ARCHIVE	*archive;

    /*
     *  If archive not specified on cmd line, set to ad's default. 
     *  Otherwise, verify that it is a valid archive.
     */ 

    if ( strlen( globals.gArchive ) == 0 )
    {
	archive = ad_archive_info( NULL );
	if ( archive == NULL )
	{
	    formatMessage( E_NO_DEFAULT );
	    printMessage( msg, MESS_ERROR );
	    return( E_NO_DEFAULT );
	}
	(void) strcpy( globals.gArchive, archive->ada_name );
    }
    else
    {
    	archive = ad_archive_info( globals.gArchive );
	if ( archive == NULL )
	{
	    formatMessage( E_ARCHIVE, globals.gArchive );
	    printMessage( msg, MESS_ERROR );
	    return( E_ARCHIVE );
	}
    }

    globals.gCase = archive->ada_case;
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	int	initDb( )
*
*   Purpose:
*	Initialize the resources needed for mfsingest's operation.
*
*   Parameters:
*
*   Values Returned:
*	int	S_SUCCESS	: All went well.
*	int	E_AD		: Error in the ad library.
*	int	E_AL		: Error in the al library.
*	int	E_DB_LIB	: Database library error occurred.
*
************************************************************************
-*/

int	initDb
(
    void
)
{
    char	application[ AD_ARCHNAME_LEN + 10 ];
    				/* String representing this app.	*/
    char	*dbname;	/* Name of the mfs database.		*/
    char	*dbserver;	/* Sybase Db server for the mfs.	*/
    
    
    /* 
     *  Initialize the CADC db library, print library init message.
     */

    (void) sprintf( application, "%s.%s", APPLICATION, 
            globals.gArchive );
    CHECK_DB_LIB( db_init( application ) );
    msg_append( msg, db_msg );
    msg_clear( db_msg );
    printMessage( msg, MESS_OPER );
    

    /*
     *  Get the database name, and open a connection.
     */


    CHECK_AD( ad_dbinfo_get( AD_MED_MFS, &dbserver, &dbname, NULL, NULL ) );
    CHECK_DB_LIB( db_open( dbserver, NULL, NULL, dbname,
	    FALSE, &globals.gDbproc ) );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*       int     initGlobals()
*
*   Purpose:
*       Initialize the globals variables.
*
*   Parameters:
*       None.
*
*   Values Returned:
*       int	S_SUCCESS		: All went well
*
************************************************************************
-*/

int	 initGlobals()
{
    globals.gAllowReingest 	= FALSE;
    globals.gArchive[0] 	= '\0';
    globals.gBatch 		= FALSE;
    globals.gCase		= AD_CASE_NONE;
    globals.gCrc		= AD_CRC_UNKNOWN;
    globals.gCreateDate 	= 0;
    globals.gDbproc 		= NULL;
    globals.gLogFp 		= NULL;
    globals.gMountPoint[0] 	= '\0';
    globals.gNumFiles 		= 0;
    globals.gNumKbytes 		= 0;
    globals.gPathList 		= NULL;
    globals.gReingest		= FALSE;
    globals.gVerbose 		= FALSE;
    globals.gVersion 		= FALSE;
    globals.gVolumename[0] 	= '\0';
    globals.gVolumeType[0] 	= '\0';
    globals.gVolumeFormat[0] 	= '\0';

    ( void ) strcpy( globals.gMfsFormat, MFS_UNKNOWN );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	initMfsIngest
*
*   Purpose:
*	Initialization for mfsIngest.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal Completion
*	int	E_DB		: Error in the SYBASE library.
*	int	E_DB_LIB	: Error in the CADC databas library.
*
************************************************************************
-*/

int	initMfsIngest
(
    void
)
{
    /*
     *  Initialize the ad library. 
     */
    
    CHECK_AD( ad_init() );
    msg_append( msg, ad_msg );
    msg_clear( ad_msg );
    printMessage( msg, MESS_INFO );

    
    /*
     *  Check the archive.
     */

    CHECK( archiveCheck() );


    /*
     *  Write the version information.
     */
 
    formatMessage( S_VERSION, version_short );
    printMessage( msg, MESS_CON );


    /*
     * Initialize the database.
     */

    CHECK( initDb() );


    return( S_SUCCESS );
}
