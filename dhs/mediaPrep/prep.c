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
*   Module Name:	mediaPrep/src/prep.c
*
*   Purpose:
*	Contains routines for preparing a  media for
*	writing, creates an image of the media on magnetic disk.
*
*   Routines:
*	int		prepMedia	: Prepares the Media.
*	static	int	parseLog	: Parses the log file.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: prep.c
*	Version Number	: 1.11
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/09/09 SDJ	: Copied from cdPrep
*	98/09/15 SDJ	: Made changes to get script info. from AM.
*       98/10/21 SDJ  	: Added script info from AM.
*       98/12/06 SDJ  	: Altered to support device tables in AM.
*	98/12/17 SDJ	: Added intialization of vars in parseLog.
*       99/01/06 SDJ	: Moved dir. of hard link creation into code, so
*			  script does not need to do it.
*	99/03/18 NRH	: Now appends prep script to mediaPrep log file
*			  when the prep script fails.
*	99/03/26 SDJ	: Fixed parseLog to search for the name of the
*			  script only - not the entire path if one is given.
*	99/04/15 SDJ	: Added logging of xterm information.  Log files
*			  are now removed before the script is called.
*	99/04/20 SDJ	: Changed the xterm log file handling to fix a
*			  problem that occured when the script log file
*			  didn't exist.
*	99/04/28 NRH	: dito.
*     2000/01/31 SDJ	: Fixed parsLog() to error if < 1 physical
*			  images are created.
*     2000/03/17 SDJ	: Switched to fork()/exec() and UNIX rm command since
*			  the gen_file functions are unable to handle
*			  large files (bigger than 2 147 482 647 bytes).
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <unistd.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/wait.h>
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
#include "am.h"
 
 
/*
 ************************************************************************
 *  Media Prep include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal Function Prototypes.
 ************************************************************************
 */

static	int	parseLog( int *, int * );


/*+
************************************************************************
*
*   Function:   parseLog
*
*   Purpose:
*       Parse the log file created by the script, it is searced for
*	some required keywords.  In particular errors, images,
*       prepared, and found.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	S_SCRIPT_ERR	: An error was reported by the script.
*	int	E_LOG_FMT	: Error in the log file.
*	int	E_SYSTEM	: Error in system call. 
*
************************************************************************
-*/
 
static int      parseLog
(
    int		*numPhysvols,	/* (mod) Num Phys. Vols written by.	*/
    int		*numPreped	/* (mod) Num of files prepared.		*/    
)
{
    boolean	errorsSeen;	/* Have we seen 'errors'?		*/
    boolean	foundSeen;	/* Have we seen 'found'?		*/
    FILE        *fp;		/* File pointer.			*/
    boolean	imagesSeen;	/* Have we seen 'images'?		*/
    char        line[128];	/* Line from file.			*/
    int		numErrors;	/* Num of errors in the log file.	*/
    int		numFiles;	/* Num of Files to prepared.		*/
    boolean	prepSeen;	/* Have we seen 'prepared'?		*/
    int		retStat;	/* The value to return.			*/
    char	*scriptName;	/* The name of the script used.		*/
    char	xtermLog[PATH_MAX];
    				/* Log file generated by the xterm.	*/

    /*
     * Initilization of variables.
     */

    CHECK_NULL( scriptName = strtail( globals.gScript ) );
    if ( streq( scriptName, "" ) )
    {
	CHECK_NULL( scriptName = strsav( globals.gScript ) );
    }
    errorsSeen = FALSE;
    foundSeen = FALSE;
    imagesSeen = FALSE;
    prepSeen = FALSE;
    retStat = S_SUCCESS;

    numFiles = 0;
    numErrors = 0;
    *numPreped = 0;
    *numPhysvols = 0;
    

    /*
     *  Parse the scriptslog file looking for images created & errors.
     */
 
    if ( ( fp = fopen( globals.gScriptLogFileName, "r" ) ) == NULL )
    {
	formatMessage( MSG_ERRNO, globals.gScriptLogFileName );
	printMessage( msg, MESS_ERROR );
	retStat = E_SYSTEM;
    }


    while ( retStat == S_SUCCESS &&
	    fgets( line, sizeof( line ), fp ) != NULL )
    {
        /*
         *  Look for script messages in the script prepare log file.
         */
 
        if ( strncmp( line, scriptName, strlen( scriptName ) ) == 0 )
        {
	    if ( strstr( line, "errors" ) != NULL &&
		    sscanf( line, "%*s %*s %*s %*s %*s %d", 
		    &numErrors ) != 0 )
	    {
		errorsSeen = TRUE;
	    }
	    else if ( strstr( line, "images" ) != NULL &&
		    sscanf( line, "%*s %*s %*s %*s %*s %d", 
		    numPhysvols ) != 0 )
	    {
		imagesSeen = TRUE;
	    }
	    else if ( strstr( line, "prepared" ) != NULL &&
		    sscanf( line, "%*s %*s %*s %*s %*s %*s %d",
		    numPreped ) != 0 )
	    {
		prepSeen = TRUE;
	    }
	    else if ( strstr( line, "found" ) != NULL &&
		    sscanf( line, "%*s %*s %*s %*s %*s %*s %d",
		    &numFiles ) != 0 )
	    {
		foundSeen = TRUE;
	    }
		
        }
    }
 

    /*
     *  Make sure 'copies', 'images' and 'errors' seen in logfile. 
     */ 

    if (  retStat == S_SUCCESS &&
	    ( !errorsSeen || !imagesSeen || ! foundSeen || !prepSeen ) )
    {
	formatMessage( E_LOG_FMT );
	printMessage( msg, MESS_ERROR );
	retStat = E_LOG_FMT;
    }
    else if ( retStat == S_SUCCESS )
    {
	if ( numErrors != 0 )
	{
	    formatMessage( S_SCRIPT_ERR );
	    printMessage( msg, MESS_OPER );
	    retStat = S_SCRIPT_ERR;
	}
	else if ( *numPreped != numFiles )
	{
	    formatMessage( S_SCRIPT_ERR );
	    printMessage( msg, MESS_OPER );
	    retStat = S_SCRIPT_ERR;
	}
	else if ( *numPhysvols < 1 )
	{
	    formatMessage( S_SCRIPT_ERR );
	    printMessage( msg, MESS_OPER );
	    retStat = S_SCRIPT_ERR;
	}
	else
	{
	    formatMessage( S_SCRIPT_PHYSVOLS, *numPhysvols );
	    printMessage( msg, MESS_OPER );
	}
    }


    /*
     *  If there was a problem with in the script log file, copy it 
     *  to the mediaPrep log file.
     */

    if ( retStat != S_SUCCESS )
    {
	if ( fp != NULL )
	{
	    /*
	     * Copy the contents of the script's log file to
	     * mediaPrep's log file.
	     */

	    formatMessage( S_SCRIPT_LOG, scriptName );
	    printMessage( msg, MESS_LOG );
	    if ( fseek( fp, 0, SEEK_SET ) == 0 )
	    {
		while ( fgets( line, sizeof( line ), fp ) != NULL )
		{
		    formatMessage( S_SCRIPT_LOG_DATA, line );
		    printMessage( msg, MESS_LOG );
		}
	    }
	    fclose( fp );
	    formatMessage( S_SCRIPT_END, scriptName );
	    printMessage( msg, MESS_LOG );
	}


	/*
	 * Copy the contents of the xterm's log file to mediaPrep's
	 * log file.
	 */

	(void) sprintf( xtermLog, "%s/%s/MP.XTERM.%d.%s.log",
		globals.gMediaStagePath, globals.gMDSDirectory, 
		globals.gMediaId, globals.gMediaUnitName );

	if ( ( fp = fopen( xtermLog, "r" ) ) == NULL )
	{
	    formatMessage( MSG_ERRNO, xtermLog );
	    printMessage( msg, MESS_ERROR );
	    return( E_SYSTEM );
	}
	else
	{
	    formatMessage( S_SCRIPT_LOG, "xterm" );
	    printMessage( msg, MESS_LOG );
	    CHECK_SYSTEM( fseek( fp, 0, SEEK_SET ), xtermLog );
	    while ( fgets( line, sizeof( line ), fp ) != NULL )
	    {
		formatMessage( S_SCRIPT_LOG_DATA, line );
		printMessage( msg, MESS_LOG );
	    }
	    formatMessage( S_SCRIPT_END, scriptName );
	    printMessage( msg, MESS_LOG );
	}
    }

    fclose( fp );
 
 
    return( retStat );
}

/*+
************************************************************************
*
*   Function:   prepMedia
*
*   Purpose:
*	Invokes a script which does further preparations on the media.
*	For example, when writing CDROMs the script creates an image
*	of the CDROM on magnetic disk.
*
*	If the script name is "none" no further processing is done,
*	as in the case of tapes.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		prepMedia
(
    EPTR 	*fileList	/* (in)  List of chosen files.		*/
)
{ 
    int		logStatus;	/* Return value from parseLog.		*/
    int		numPhysvols;	/* Number of Physical volume's written.	*/
    int		numPrepared;	/* Number of files prepared.		*/
    int		status;		/* Function return status.		*/
    char	workDirPath[PATH_MAX];
    				/* Full path to the work directory.	*/
    
    char	arg0[PATH_MAX] = "xterm";
    char	arg1[3] = "-T";
    char	arg2[80];
    char	arg3[3] = "-n";
    char	arg4[13] = "MEDIA PREPARE";
    char	arg5[3] = "-l";
    char	arg6[4] = "-lf";
    char	arg7[PATH_MAX];
    char	arg8[3] = "-e";
    char	arg9[PATH_MAX];
    char	arg10[AM_MEDIA_UNIT_NAME_LEN];
    char	arg11[PATH_MAX];
    char	arg12[PATH_MAX];
    char	arg13[PATH_MAX];
    char	arg14[PATH_MAX];

    int		execStatus;


    status = S_SUCCESS;
    if ( streq( globals.gScript, "none" ) )
    {
	/*
	 * No script to call so prep is finished.
	 */
	
	globals.gMediaUnit.amuStatus = AMU_PREPARED;
	CHECK_AM( amUnitPut( &(globals.gMediaUnit), globals.gDbProc1 ) );
	formatMessage( S_COMPLETE, globals.gMediaUnitName );
        printMessage( msg, MESS_OPER );
    	return( status );
    }
    


    /* 
     *  Compose strings and stuff to pass into the script. 
     */ 
   
    (void) sprintf( workDirPath, "%s/%s", globals.gMediaStagePath, 
	    globals.gMDSDirectory );


    /*
     *  Set up arguments to pass to exec().
     */


    (void) sprintf( arg2, "MEDIA PREP    Media ID :  %d    "
	    "Media Unit Name :  %s",
	    globals.gMediaId, globals.gMediaUnitName );
    (void) sprintf( arg7, "%s/%s/MP.XTERM.%d.%s.log",
	    globals.gMediaStagePath, globals.gMDSDirectory, 
            globals.gMediaId, globals.gMediaUnitName );
    (void) sprintf( arg9, "%s", globals.gScript );
    (void) sprintf( arg10, "%s", globals.gMediaUnitName );
    (void) sprintf( arg11, "%s", globals.gScriptConfigFPath );
    (void) sprintf( arg12, "%s", globals.gScriptLogFileName );
    (void) sprintf( arg13, "%s", workDirPath );
    (void) sprintf( arg14, "%s/%s", globals.gRetrievalStagePath, 
	    globals.gMDSDirectory );


    /*
     * Make sure env. variable DISPLAY is set.
     */
    
    if ( getenv( "DISPLAY" ) == NULL )
    {
	formatMessage( E_DISPLAY );
	printMessage( msg, MESS_ERROR );
	return( E_DISPLAY );
    }

    /*
     * Remove any exiting log files.
     */

    (void) file_rm( arg7 );
    (void) file_rm( arg12 );
	    

    /*
     * Execute the script.
     */

    if ( status == S_SUCCESS )
    {
	status = execCommand( arg0, 14, arg1, arg2, arg3, arg4, arg5, arg6,
		arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14 );
    }


#ifdef OMIT
    if ( globals.gScriptKill == FALSE && status == S_SUCCESS )
    {
	if ( ( globals.gScriptProcessId = fork() ) == 0 )
	{
	    if ( execlp( arg0, arg0, arg1, arg2, arg3, arg4, arg5, arg6,
		    arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14,
		    (char *) NULL ) == -1 )
	    {
		formatMessage( E_FORK_N_EXEC, arg0, arg1, arg2, arg3, arg4,
			arg8, arg9, arg10, arg11, arg12, arg13, arg14 );
		printMessage( msg, MESS_ERROR );
		_exit( 0 );
	    }
	}
	if (  waitpid( globals.gScriptProcessId, &execStatus, 0 ) == -1
		&&  status == S_SUCCESS )
	{
	    formatMessage( MSG_ERRNO, globals.gScript );
	    printMessage( msg, MESS_ERROR );			
	    status = E_SYSTEM;
	}


	/*
	 *  Check out the return value of the exec'ed process. 
	 */
	 
	if ( !WIFEXITED( execStatus ) )
	{
	    formatMessage( E_EXEC_ABNORMAL_EXIT, arg0 );
	    printMessage( msg, MESS_ERROR );
	}
	else if ( WEXITSTATUS( execStatus ) != 0 )
	{
	    formatMessage( E_EXEC_NONZERO_EXIT, arg0, 
		    WEXITSTATUS( execStatus ) - 256 );
	    printMessage( msg, MESS_ERROR );
	}
    }
#endif


    /*
     *  Open and parse the media prep log file. 
     */

    logStatus = parseLog( &numPhysvols, &numPrepared );

    if ( numPrepared != eptrlen( fileList ) && status == S_SUCCESS &&
            logStatus == S_SUCCESS )
    {
        formatMessage( E_NOT_COMPLETE, numPrepared, eptrlen( fileList ) );
        printMessage( msg, MESS_ERROR );
	status = E_NOT_COMPLETE;
    }
    else if ( logStatus != S_SUCCESS  && status == S_SUCCESS )
    {
	formatMessage( E_NOT_COMPLETE, numPrepared, eptrlen( fileList ) );
        printMessage( msg, MESS_ERROR );
	status = E_NOT_COMPLETE;
    }
    else if ( status == S_SUCCESS && logStatus == S_SUCCESS ) 
    {
	/*
	 * Update database.
	 */

	globals.gMediaUnit.amuStatus = AMU_PREPARED;
	CHECK_AM( amUnitPut( &(globals.gMediaUnit), globals.gDbProc1 ) );
	formatMessage( S_COMPLETE, globals.gMediaUnitName );
        printMessage( msg, MESS_OPER );
    }
    

    return ( status ); 
}

