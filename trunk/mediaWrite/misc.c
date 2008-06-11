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
*   Module Name:	mediaWrite/src/misc.c
*
*   Purpose:*
*	Contains miscellaneous routines for media write.
*
*   Routines:
*	int	checkDbProc		: Checks if the dbproc has died. 
*	int	checkUnique		: Checks only one mediaQueue is running.
*	int	cleanup			: Cleans up before mediaQueue exits.
*	int 	composeDbAppName	: Composes app name for database.
*	int	deviceUpdate		: Add process info to device struct.
*	char*	strDigEnd		: Finds the digits at the string end.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Shannon jaeger
*
*   Modification History:
*       98/09/09 SDJ    : Copied from cdWrite
*       98/10/21 SDJ    : Added device handling.
*       98/11/23 SDJ    : Added deviceUpdate().
*       99/04/28 NRH    : Changed cleanup to update the actions table.
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

#include "db.h"
#include "am.h"
 
 
/*
 ************************************************************************
 *  Media write include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypes
 ************************************************************************
 */

static char	*strDigEnd( char * );

/*+
************************************************************************
*
*   Function:   checkDbproc
*
*   Purpose:
*       Check that a dbprocess is open and ready to use.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_DB_LIB	: Error in the cadc db library.
*	int	E_REQ		: Error in the req library.
*
************************************************************************
-*/
 
int             checkDbProc
(
    DBPROCESS   **dbProc
)
{
    char        *database;	/* Database to connect to.		*/
    char        *dbServer;	/* Database server name to use.		*/
 
 
    if ( *dbProc != NULL )
    {
        /*
         *  Test the dbprocess by doing a use.
         */
 
        if ( dbuse( *dbProc, "master" ) == SUCCEED )
        {
            return( S_SUCCESS );
        }
 
 
        /* 
         *  Close the dbprocess if it failed the test.
         */
 
        (void) db_close( *dbProc );
	*dbProc = NULL;
    }
 
 
    /*
     *  If the dbprocess was not open, or if it failed the test, open it.
     */
 
    CHECK_AM( amDbInfoGet( &dbServer, &database, NULL, NULL, NULL, NULL ) );
 
    CHECK_DB_LIB( db_open( dbServer, NULL, NULL, database,
            FALSE, dbProc ) );
 
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	checkUnique
*
*   Purpose:
*	Checks that only one mediaQueue process is running on the
*	given media ID.
*
*   Values Returned:
*	int	S_SUCCESS	: It's OK for this one to be running.
*	int	E_DB_LIB	: Error in the cadc db library.
*	int	E_ONLY_ONE	: Only one is allowed. (see purpose)
*	int	E_SYSTEM	: Error in a system call.
*
************************************************************************
-*/

int		checkUnique
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    char	*appString	/* (in)  The application name string.	*/
)
{
    int		appCount;	/* # of mediaQueues running on this ID.	*/


    CHECK_DB_LIB( db_count_app( dbproc, appString, NULL, &appCount ) );


    /*
     *  If appCount > 1, there's another mediaQueue running on this ID, 
     *  so remove the log file close the db process, and exit.
     */

    if ( appCount > 1 )
    {
	/*
	 *  Close the log file.
	 */

	formatMessage( E_ONLY_ONE );
	printMessage( msg, MESS_ERROR );

	(void) fclose( globals.gLogFp );
	globals.gLogFp = NULL;

	return( E_ONLY_ONE );
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	cleanup
*
*   Purpose:
*	Cleans up before mediaWrite exits.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int 	E_AM		: Error in am library.
*
************************************************************************
-*/

int		cleanup
(
    void
)
{
    /*
     * Remove action from the action table.
     */

    CHECK_AM( amActionDelete( globals.gDbProc1, globals.gMediaId, 
	    globals.gMediaUnitName ) );


    /*
     * Close AM
     */
 
    amClose();
 
    
    /*
     * Close database connections.
     */
    
    if ( globals.gDbProc1 != NULL  )
    {
        CHECK_DB_LIB( db_close( globals.gDbProc1 ) );
        globals.gDbProc1 = NULL;
    }
 
    if ( globals.gDbProc2 != NULL  )
    {
        CHECK_DB_LIB( db_close( globals.gDbProc2 ) );
        globals.gDbProc2 = NULL;
    }
    CHECK_DB_LIB( db_exit() ); 



    /*
     * Close logfile.
     */
    
    (void) fclose( globals.gLogFp );
    globals.gLogFp = NULL;


    /*
     * Cleanup memory
     */


    if ( globals.gDeviceList != NULL )
    {
	eptr_free( globals.gDeviceList, free );
	globals.gDeviceList = NULL;
    }

    if ( globals.gPidList != NULL )
    {
	eptr_free( globals.gPidList, free );
	globals.gPidList = NULL;
    }
    

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   composeDbAppName
*
*   Purpose:
*	Composes the application name for the database. This is
*	necessary as the field in SYBASE is only 16 characters long,
*	and to put all information from all parameters in could take
*	more than 16 characters. 
*
*	Suggestion: If a problem ever arises where the string becomes
*	too long, convert both integers to a higher-than 10 base 
*	representation, such as hexadecimal. This should cut down 
*	the number of characters a bit. 
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion. 
*	int	E_UNITNAME_FMT	: Error decoding unit name.
*
************************************************************************
-*/

int		composeDbAppName
( 
    char	*application,	/* (out) Returns the new db app name.	*/
    char	*prefix,	/* (in)  The application prefix.	*/
    int		mediaId,	/* (in)  The media ID.			*/
    char	*mediaUnitName	/* (in)  The media unit name.		*/
)
{
    char	*uNameNumStr;/* String for the unit name number.	*/
    int		uNameNumInt;	/* Integer for the unit name number.	*/


    /*
     *  Strip off 'root_name', and leading zeroes from mediaUnitName. 
     */

    uNameNumStr = NULL;
    uNameNumStr = strDigEnd( mediaUnitName );
    if ( uNameNumStr == NULL )
    {
	formatMessage( E_UNITNAME_FMT, mediaUnitName, mediaId );
	printMessage( msg, MESS_ERROR );
	return( E_UNITNAME_FMT );
    }
    if ( ! str2int( uNameNumStr, &uNameNumInt ) )
    {
	formatMessage( E_UNITNAME_FMT, mediaUnitName, mediaId );
	printMessage( msg, MESS_ERROR );
        return( E_UNITNAME_FMT );
    }
    
    
    /* 
     *  Print the return string - [prefix][media ID][unit # integer].
     */

    (void) sprintf( application, "%s%d%d", prefix, mediaId, 
	    uNameNumInt );
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   deviceUpdate
*
*   Purpose:
*       Set the process specific information in an AM device structure.
*
*   Values Returned:
*       int     S_SUCCESS       : Normal Completion.
*       int     E_SYSTEM        : System error occured during getpid()
*
*   References:
*       *** delete if not applicable ***
*
************************************************************************
-*/

int     deviceUpdate
(
    AM_DEVICE	*amDevicePtr
)
{
    CHECK_SYSTEM( amDevicePtr->amdProcessId = getpid(), "getpid" );
    (void)strcpy( amDevicePtr->amdProcessName, "mediaWrite" );
    amDevicePtr->amdMediaRequestId = globals.gMedia.ammMediaRequestId;
    (void)strcpy( amDevicePtr->amdMediaUnitName,
            globals.gMediaUnit.amuMediaUnitName );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   strDigEnd
*
*   Purpose:
*	Return a pointer to the digits at the end of a string.
*	to them.
*
*   Values Returned:
*	char *	ptr	: Pointer into existing C string.
*	
*	
*
************************************************************************
-*/

static	char	*strDigEnd
( 
    char	*strptr		/* (in) pointer to existing string.	*/
)
{
    char	*ptr;		/* pointer to manipulate.		*/


    ptr = strptr + strlen( strptr ) - 1;

    while ( *ptr >= '0' && *ptr <= '9' )
    {
	ptr--;
    }
    ptr++;


    return( ptr ); 
}
