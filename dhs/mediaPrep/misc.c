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
*   Module Name:	mediaPrep/src/misc.c
*
*   Purpose:
*	Contains miscellaneous routines for Media Prep.
*
*   Routines:
*	int	checkDbProc		: Checks if the dbproc has died. 
*	int	checkUnique		: Checks only one mediaQueue is running.
*	int	cleanup			: Cleans up before mediaQueue exits.
*	int 	composeDbAppName	: Composes app name for database.
*	int	execCommand		: Execute a script.
*	int	pathCompare		: Compares two paths.
*	char*	strDigEnd		: Finds the digits at the string end.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/09/09 SDJ	: Copied from cdPrep.
*	98/12/01 SDJ	: Altered cleanup() to cleanup all globals,
*			  as needed.
*	99/03/25 SDJ	: Moved config_close() to init() from cleanup().
*	99/04/28 NRH	: Changed cleanup to update the actions table.
*     2000/03/17 SDJ	: Added execCommand().
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
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/param.h>

 
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
 *  Media Prep  include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypes.
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
     * Close log file.
     */

    (void) fclose( globals.gLogFp );
    globals.gLogFp = NULL;


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	execCommand
*
*   Purpose:
*	Execute a command by doing a fork/exec.  A fork/exec sequence
*	gives us more control.  In particular we can attempt to kill
*	the command that was initiated.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory allocation failure.
*	int	E_SYSTEM	: A system error has occured.
*
************************************************************************
-*/

int	execCommand
(
    char	command[MAXPATHLEN],
    				/* (in)  Command to execute.		*/
    int		numArgs,	/* (in)	 Arguments sent to the cmd.	*/
    ...				/* (in)  Command arguments.		*/
)
{
    char	*argString;		/* String for msg display.	*/
    char	*argVector[50+1];	/* argument vector for exec().	*/
    va_list	args;			/* Command arguments.		*/
    int		execStatus;		/* Status of exec().		*/
    int		i;		
    
    /*
     * Extract the args given and place them in an argument vector.
     * Also create a string to print in the log file.
     */
    
    if ( numArgs > 50 )
    {
	numArgs = 50;
    }
    CHECK_NULL( argString = char_alloc( MAXPATHLEN * numArgs ) );
    argString[0]='\0';

    
    argVector[0] =  command;
    va_start( args, numArgs );
    for( i = 1; i <= numArgs; i++ )
    {
	argVector[i] = (char *)va_arg( args, char* );
	CHECK_NULL( argString = strnapp( argString, argVector[i],
		strlen( argVector[i] ) + 1 ) );
	CHECK_NULL( argString = strnapp( argString, " ", 1 ) );
    }
    va_end( args );
    argVector[numArgs+1] = NULL;

    
    formatMessage( S_EXEC_COMMAND, command, argString );
    printMessage( msg, MESS_LOG );

    
    /*
     * Don't continue if we are quitting, otherwise log what we are
     * executing..
     */
    
    if ( globals.gScriptKill == TRUE )
    {
	return( S_SUCCESS );
    }

    if ( ( globals.gScriptProcessId = fork() ) == 0 )
    {
	/*
	 * Child attemps to execute the command.
	 */
	
	if ( execvp( command, argVector ) == -1 )
	{
	    formatMessage( E_FORK_N_EXEC, command );
	    printMessage( msg, MESS_ERROR );
	    exit(0);
	}
    }
    else if ( globals.gScriptProcessId < 0 )
    {
	/*
	 * fork() failed.
	 */
	    
	formatMessage( E_FORK_N_EXEC, command );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }
    else
    {
	/*
	 * Parent waits until the child exits.
	 */
	
	if ( waitpid( globals.gScriptProcessId, &execStatus, 0 ) == -1 )
	{
	    formatMessage( MSG_ERRNO, command );
	    printMessage( msg, MESS_ERROR );
	    return( E_SYSTEM );	
	}
    

	/*
	 *  Check out the return value of the exec'ed process. 
	 */
	
	if ( !WIFEXITED( execStatus ) )
	{
	    formatMessage( E_EXEC_ABNORMAL_EXIT, command );
	    printMessage( msg, MESS_ERROR );
	    return( E_SYSTEM );
	}
	else if ( WEXITSTATUS( execStatus ) != 0 )
	{
	    formatMessage( E_EXEC_NONZERO_EXIT, command, 
		    WEXITSTATUS( execStatus ) - 256 );
	    printMessage( msg, MESS_ERROR );
	    return( E_SYSTEM );
	}
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
