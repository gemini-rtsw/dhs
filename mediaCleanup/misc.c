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
*   Module Name:	mediaCleanup/src/misc.c
*
*   Purpose:
*	Miscelaneous routines.
*
*   Routines:
*	int	checkDbproc		: Checks or opens a dbprocess.
*       int     checkNewDir		: Checks if this is a 'new' dir.
*	int	cleanup			: Cleanup before exit.
*	int	composeDbAppName	: Composes the db application name.
*       int     pathCompare     	: EPTR compare function for paths.
*
*   Date		: Dec 12, 1996
*
*   SCCS data		: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmers		: Marc LeBlanc
*			: Steve Cockayne
*			: Shannon Jaeger
*
*   Modification History:
*	97/02/21 SEC  : Added some message printing.
*	97/02/28 SEC  : Added strDigEnd().
*       97/03/18 SEC  : Mod'ed for change to al, am libs to fetch db
*                       server name in the a*DbInfoGet call.
* 	98/08/24 SDJ  : Added checkNewDir().
* 	99/04/29 NRH  : Added cleanup().
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "db.h"
#include "am.h"

#include "globals.h"

static	char	*strDigEnd ( char * );

/*+
************************************************************************
*
*   Function:   checkDbproc
*
*   Purpose:
*       Check that a dbprocess is open and ready to use.
*
*   Values Returned:
*       int     S_SUCCESS       : Normal completion.
*       int     E_AM            : Error in the CADC am library.
*       int     E_DB_LIB        : Error in the CADC db library.
*
************************************************************************
-*/
 
int             checkDbProc
(
    DBPROCESS   **dbProc
)
{
    char        *database;	/* Am database name.			*/
    char        *dbServer;	/* Database server name for am datbase.	*/
 
 
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
*   Function:   checkNewDir
*
*   Purpose:
*       Checks if the file passed in adds new directories to the 
*       list of directories to be created on the media. 
*
*   Values Returned:
*       int     S_SUCCESS       : Function completed normally.
*       int     E_MEMORY        : Error - out of memory.
*
************************************************************************
-*/

int             checkNewDir
( 
    AM_FILE     *afPtr,         /* (in)  Ptr. to media file structure.  */
    EPTR        *dirList	/* (mod) Eptr list of directories.      */ 
)
{
    boolean     done;           /* Loop controller.                     */
    int         index;          /* Index into eptr list.                */
    char        path[PATH_MAX]; /* Path string to manipulate.           */
    char        *pathPtr;       /* String to add to eptr list.          */


    done = FALSE;


    /* 
     *  Pull off the 'directory' part of the filename.
     */

    (void) sprintf( path, "%s", afPtr->amfFilename );
    strhead( path );
    if ( streq( path, afPtr->amfFilename ) )
    {
        /*
         *  No new directories. (path had no '/' in it).
         */
         
        return( S_SUCCESS );
    }

    while ( ! done )
    {
        if ( eptr_search( dirList, path, &index, pathCompare ) )
        {
            /*
             *  This dir and all parent dirs must already be in the list. 
             */

            done = TRUE;
            continue;
        }
        

        /*
         *  It isn't in the list. Add it to the list in sorted order. 
         */

        CHECK_NULL( pathPtr = gen_alloc( PATH_MAX ) );
        (void) memcpy( pathPtr, &path, PATH_MAX );
        CHECK_NULL( eptr_insert( dirList, index, pathPtr ) );


        /*
         *  Get the next parent dir to check.
         */

        strhead( path );
    }


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   checkUnique
*
*   Purpose:
*       Checks that only one mediaQueue process is running on the
*       given media ID.
*
*   Values Returned:
*       int     S_SUCCESS       : It's OK for this one to be running.
*       int     E_DB_LIB        : Error in the cadc db library.
*       int     E_ONLY_ONE      : Only one is allowed. (see purpose)
*
************************************************************************
-*/
 
int             checkUnique
(
    DBPROCESS   *dbproc,        /* (in)  The dbprocess to use.          */
    char        *appString      /* (in)  The application name string.   */
)
{
    int         appCount;       /* # of mediaQueues running on this ID. */
 
 
    CHECK_DB_LIB( db_count_app( dbproc, appString, NULL, &appCount ) );
 
 
    /*
     *  If appCount > 1, there's another mediaQueue running on this ID, 
     *  so remove the log file, close the db process, and exit.
     */
 
    if ( appCount > 1 )
    {
        /*
         *  close the log file.
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
*	Cleans up before mediaCleanup exits.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the AM library.
*       int     E_DB_LIB        : Error in the cadc db library.
*       int     E_DB		: Error in the SYBASE db library.
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

    CHECK_AM( amActionDelete( globals.gDbProc, globals.gMediaId, 
	    globals.gMediaUnitName ) );


    /*
     * Close AM
     */
 
    amClose();
 
    
    /*
     * Close database connection.
     */
    
    if ( globals.gDbProc != NULL  )
    {
        CHECK_DB_LIB( db_close( globals.gDbProc ) );
        globals.gDbProc = NULL;
    }
 
    CHECK_DB_LIB( db_exit() ); 
    
    
    /*
     * Close log file.
     */

    (void) fclose( globals.gLogFp );
    globals.gLogFp = NULL;


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   composeDbAppName
*
*   Purpose:
*       Composes the application name for the database. This is
*       necessary as the field in SYBASE is only 16 characters long,
*       and to put all information from all parameters in could take
*       more than 16 characters. 
*
*       Suggestion: If a problem ever arises where the string becomes
*       too long, convert both integers to a higher-than 10 base 
*       representation, such as hexadecimal. This should cut down 
*       the number of characters a bit. 
*
*   Values Returned:
*       int     S_SUCCESS       : Normal completion. 
*       int     E_UNITNAME_FMT  : Error decoding unit name.
*
************************************************************************
-*/
 
int             composeDbAppName
( 
    char        *application,   /* (out) Returns the new db app name.   */
    char        *prefix,        /* (in)  The application prefix.        */
    int         mediaId,        /* (in)  The media ID.                  */
    char        *mediaUnitName  /* (in)  The media unit name.           */
)
{
    char        *uNameNumStr;/* String for the unit name number.        */
    int         uNameNumInt;    /* Integer for the unit name number.    */
 
 
    /*
     *  Strip off 'root name', and leading zeroes from mediaUnitName. 
     */
 
    uNameNumStr = NULL;
    uNameNumStr = strDigEnd( mediaUnitName );
    if ( uNameNumStr == NULL )
    {
	formatMessage( E_UNITNAME_FMT, mediaUnitName, globals.gMediaId );
	printMessage( msg, MESS_ERROR );
        return( E_UNITNAME_FMT );
    }
    if ( !str2int( uNameNumStr, &uNameNumInt ) )
    {
	formatMessage( E_UNITNAME_FMT, mediaUnitName, globals.gMediaId );
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
*   Function:   strDigEnd
*
*   Purpose:
*       Return a pointer to the digits at the end of a string.
*       to them.
*
*   Values Returned:
*       char *  ptr     : Pointer into existing C string.
*
*
*
************************************************************************
-*/
 
static  char    *strDigEnd
( 
    char        *strptr         /* (in) pointer to existing string.     */
)
{
    char        *ptr;           /* pointer to manipulate.               */


    ptr = strptr + strlen( strptr ) - 1;

    while ( *ptr >= '0' && *ptr <= '9' )
    {
	ptr--;
    }
    ptr++;


    return( ptr ); 
}
