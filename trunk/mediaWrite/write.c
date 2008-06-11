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
*   Module Name:	mediaWrite/src/write.c
*
*   Purpose:
*	Contains routines for processing the writing of DVDs, CDs and
*	tapes.  It could easily expanded for other medium as well.
*
*   Routines:
*	int		bookDevices	: Book the device(s) for writing.
*	static int	execScript	: Calls exec with write a script.
*	static int	execScripts	: Controls the exec'ing of scripts.
*	int		getNumCopies	: Finds number of copies a particular
*					  script is to make.
*	int		writeMedia	: Writes the CDs.
*	static	int	parseLogs	: Parse write script's log.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: write.c
*	Version Number	: 1.16
*	Release Number	: 1
*	Last Updated	: 03/22/01
*
*   Programmer		: Shannon jaeger
*
*   Modification History:
*      	98/09/09 SDJ    : Copied from cdWrite
*	98/10/20 SDJ	: Added code to deal with booking devices.
*	98/11/16 SDJ	: Added support for writing to multiple devices
*			  concurrently.
*       98/10/21 SDJ  	: Use Am's script name rather than cdWrite's config
*                         file. Also added device booking.
*       98/11/23 SDJ 	: Added device booking support, also removed all
*                         references to GEAR.
*       98/12/17 SDJ	: Changed AM_DV_??? to AM_DG_??? and AMD_DEVICE
*                         to AM_DC_DEVICE.
*	99/01/24 SDJ	: Added execScripts() to make it easy to handle
*			  write scripte "none" value.  Added extra arg
*			  to fork/exec the path to the data links.
*	99/03/08 NRH	: Added support to write a specific number of copies
*			  of a media.
*	99/03/14 SDJ	: Added device name to the device info passed
*			  to the "write script."
*	99/03/18 NRH	: Added the number of files to write to the device
*			  info passed to the "write" script. Changed to copy
*			  the script log into the mediaWrite log if the
*			  script fails.
*	99/03/25 SDJ	: Properly stores child process ID's now.  Also
*			   use only the scripts name, not its full path
*			   when parsing the log file
*	99/04/08 NRH	: Added the the simulate option to the write script
*			  arguments. Added a chedck to ensure that the
*			  device specified matches the media type required.
*	99/04/15 SDJ    : Added logging of xterm information.  Log files
*       		  are now removed before the script is called.
*	99/11/29 SDJ	: Added the logical Device name to the title of
*			  each xterm invoked.
*     2000/01/13 SDJ	: Fixed fork()/exec() behavior.  No return if
*			  exec fails.
*     2000/02/08 SDJ	: Fixed bug in getNumCopies(), and in parseLogs().
*			  getNumCopies returned values > 0 for devices
*			  that were not used.  parseLogs() tried to parse
*			  that were never created.  Both of these problems
*			  occured when a logicalDevice was used that had
*			  more devices than there were copies to write.
*    2000/03/17 SDJ	: Change log file parsing since output from
*			  scripts changed slightly.  It wasn't properly
*			  detecting the number of copies verified.
*    2000/11/25 SDJ	: Fork/exec wasn't detecting all errors.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/



/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
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
 *  Media Write include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal Function Prototypes.
 ************************************************************************
 */

static	int	bookDevices( void );
static	void	execScript( int, int, int, char[] );
static	int	execScripts( int );
static	int	getNumCopies( int, int, int );
static	int	parseLogs( int, int * );

/*+
************************************************************************
*
*   Function:	bookDevices
*
*   Purpose:
*	Books the devices to be used for writing.
*
*   Values Returned:
*	int	S_SUCCESS		: Normal completion.
*	int	E_AM			: Error in AM library routine.
*	int	E_DEVICE_TYPE		: Logical device does not match
*					  the media type required.
*
************************************************************************
-*/

int	bookDevices
(
    void
)
{
    AM_DEVICE	*amDevicePtr;	/* AM device structure.			*/
    AM_LOGICAL_DEVICE           /* A logical device structure.   	*/
                logicalDevice;
    char	logicalDeviceName[AM_LOGICAL_DEV_NAME_LEN];
    				/* Name of the logical Device.		*/
    int		numDevices;	/* Number of device assigned to a	*/
				/* logical name.			*/
    int         index;          /* EPTR index.                          */
    int		status;		/* status from amDeviceGet.		*/


    CHECK_NULL( globals.gDeviceList = eptr_alloc( 10 ) );
    logicalDeviceName[0] = '\0';
    

    /*
     * If a device was specified use it, otherwise pick the first one
     * that is available.
     */

    if ( streq( globals.gLogicalDeviceName, "" ) )
    {
	CHECK_NULL( amDevicePtr = gen_alloc( sizeof( AM_DEVICE ) ) );

	for( status = amDeviceGet( AM_DG_TYPE, amDevicePtr, globals.gDbProc1,
		globals.gMedia.ammMediaType );
	        status != AM_NO_DEVICE;
	     status = amDeviceGet( AM_DG_NEXT, amDevicePtr, globals.gDbProc1,
		    globals.gMedia.ammMediaType ) )
	{
	    if (  amDevicePtr->amdOnline && ( ! amDevicePtr->amdInUse
		    || globals.gForce ) )
	    {
		CHECK_NULL( globals.gDeviceList = eptr_insert(
		    globals.gDeviceList, 0, amDevicePtr ) );
		formatMessage( S_DEV_SELECTED, amDevicePtr->amdDeviceName );
		printMessage( msg, MESS_OPER );
		break;
	    }
	}
    }
    else
    {
	(void) sprintf( logicalDeviceName, globals.gLogicalDeviceName );
	status = amDeviceLookup( logicalDeviceName, globals.gDbProc1,
		globals.gMedia.ammMediaType, &globals.gDeviceList );
	if ( status == AM_NO_DEVICE )
	{
	    formatMessage( E_DEVICE_TYPE, logicalDeviceName,
		    globals.gMedia.ammMediaType );
	    printMessage( msg, MESS_ERROR );
	    return( E_DEVICE_TYPE );
	}
    }

    
    /*
     * Perform some checks on the devices.
     */

    status = S_SUCCESS;
    for( index = eptrlen( globals.gDeviceList ) - 1; index >= 0; index -- )
    {
	CHECK_NULL( amDevicePtr = eptritem( globals.gDeviceList, index ) );
	if ( ! amDevicePtr->amdOnline || ( amDevicePtr->amdInUse
		&& ! globals.gForce ) )
	{
	    formatMessage( E_DEVICE_BUSY, amDevicePtr->amdType,
		    globals.gMedia.ammMediaType );
	    status = E_DEVICE_BUSY;
	}
	else
	{
	    CHECK( deviceUpdate( amDevicePtr ) );
	}
    }

    if ( eptrlen( globals.gDeviceList ) <= 0 )
    {
	if ( streq( logicalDeviceName, "" ) )
	{
	    formatMessage( E_NO_DEVICE, globals.gMedia.ammMediaType );
	    status = E_NO_DEVICE;
	}
	else
	{
	    formatMessage( E_DEV_NOT_FOUND, logicalDeviceName );
	    status = E_DEV_NOT_FOUND;
	}
    }
    
    if ( status != S_SUCCESS )
    {
	printMessage( msg, MESS_ERROR );
	return( status );
    }


    /*
     * Now book the devices.
     */

    for( index = eptrlen( globals.gDeviceList ) - 1; index >= 0; index -- )
    {
	CHECK_NULL( amDevicePtr = eptritem( globals.gDeviceList, index ) );

	
	/*
	 * If no logical device was given on the command line then
	 * determine the logical device name of the device selected.
	 */

	if ( streq( logicalDeviceName, "" ) )
	{
	    status = AM_SUCCESS;
	    numDevices = 100;
	    for( status = amLogicalDeviceGet( AM_LDG_DNAME, &logicalDevice,
		    globals.gDbProc1, amDevicePtr->amdDeviceName );
		 status == AM_SUCCESS;
		 status = amLogicalDeviceGet( AM_LDG_NEXT, &logicalDevice,
			 globals.gDbProc1 ) )
	    {
		if ( logicalDevice.amldNumDevices < numDevices )
		{
		    sprintf( logicalDeviceName, logicalDevice.amldName );
		    numDevices = logicalDevice.amldNumDevices;
		}
	    }
	}
    

	/*
	 * Now book the device.
	 */

	CHECK_AM( amDeviceBook( logicalDeviceName, globals.gForce,
		globals.gDbProc1, amDevicePtr ) );
    }

    
    (void) strcpy( globals.gLogicalDeviceName, logicalDeviceName );
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	execScripts.
*
*   Purpose:
*	Calls execlp() with the write script.  Only child processed
*	should be calling this function.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*       int     E_SYSTEM	: System error occured during execlp()
*	int	E_DISPLAY	: DISPLAY env. not set.
*	int	E_AM		: Error in AM Library routine.
*
************************************************************************
-*/

int	execScripts
(
    int		numCopies	/* (in)  Number of copies to write.	*/
)
{
    AM_DEVICE	*amDevicePtr;	/* AM library device structure.		*/
    boolean	child;		/* TRUE  if this process is a child.	*/
    pid_t	childPid;	/* Child process ID.			*/
    int		execStatus;	/* execute status of the exec.		*/
    int         index;		/* process counter.			*/
    pid_t	*pidPtr;	/* Pointer to a process ID.		*/
    char	workDirPath[PATH_MAX];
    				/* Full path to work directory.		*/
    int		numFiles;	/* The number of files on the unit.	*/
    int		status;


    status = S_SUCCESS;


    /*
     *  Get the number of files on the unit.
     */

    CHECK_AM( amNumOnUnit( globals.gDbProc1, globals.gMediaId, 
	    globals.gMediaUnitName, &numFiles ) );

    
    /*
     *  Compose strings and stuff to pass into write script.
     */

    (void) sprintf( workDirPath, "%s/%s", globals.gMediaStagePath, 
	    globals.gMDSDirectory );

    
    /*
     * Make sure DISPLAY env. variable is set.
     */
    
    if ( getenv( "DISPLAY" ) == NULL )
    {
	formatMessage( E_DISPLAY );
	printMessage( msg, MESS_ERROR );
	return( E_DISPLAY );
    }


    /*
     * Book the device(s) for writting and intialize process list.
     */

    CHECK_NULL( globals.gPidList = eptr_alloc( 10 ) );
    CHECK( bookDevices( ) );


    /*
     * Fork off a number of child processes, storing their process
     * ID's.
     */

    child = FALSE;
    for( index = 0; index < eptrlen( globals.gDeviceList ) &&
	     globals.gScriptKill == FALSE && index < numCopies;
	     index ++ )
    {
	childPid = (pid_t)-1;
	if ( ( childPid = fork() ) == 0 )
	{
	    child = TRUE;

	    break;
	}
	else if ( childPid < 0  )
	{
	    formatMessage( E_FORK_N_EXEC, "", "", "", "", "", "", "",
		    "", "", "", "", "", "", "", "", "", "", "", "" );
	    printMessage( msg, MESS_ERROR );
	    return( E_FORK_N_EXEC );
	}
	
	
	CHECK_NULL( pidPtr = gen_alloc( sizeof( pid_t ) ) );
	(void) memcpy( pidPtr, &childPid, sizeof( pid_t ) );
	CHECK_NULL( globals.gPidList = eptr_insert( globals.gPidList,
		eptrlen( globals.gPidList ), pidPtr ) );
    }


    
    if ( child )
    {
	/*
	 * Exec the script after intializing the process specific
	 * arguments for all children. 
	 */

	execScript( index, numCopies, numFiles, workDirPath );
	exit(0);
    }


    /*
     * Wait for the children to complete their thing and Check out the
     * return value of the exec'ed process. 
     */
    
    while( index > 0 )
    {
	(void)waitpid( (pid_t)-1, &execStatus, 0 );


	if ( ! WIFEXITED( execStatus ) )
	{
	    formatMessage( E_EXEC_ABNORMAL_EXIT, "xterm" );
	    printMessage( msg, MESS_ERROR );
	    status = E_EXEC_ABNORMAL_EXIT;
	}
	else if ( WEXITSTATUS( execStatus ) != 0 )
	{
	    formatMessage( E_EXEC_NONZERO_EXIT, "xterm", 
	    WEXITSTATUS( execStatus ) - 256 );
	    printMessage( msg, MESS_ERROR );
	    status = E_EXEC_NONZERO_EXIT;
	}
	
	index --;
    }


    /*
     *  Release all of the devices.
     */

    for( index = eptrlen( globals.gDeviceList ) - 1; index >= 0; index -- )
    {
	CHECK_NULL( amDevicePtr = eptritem( globals.gDeviceList, index ) );
	CHECK_AM( amDeviceClear( AM_DC_DEVICE, globals.gDbProc1, 
		amDevicePtr->amdDeviceName ) );
    }
    

    return( status );
}

/*+
************************************************************************
*
*   Function:	execScript.
*
*   Purpose:
*	Calls execlp() with the write script.  Only child processed
*	should be calling this function.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*       int     E_SYSTEM	: System error occured during execlp()
*	int	E_DISPLAY	: DISPLAY env. variable not set.
*
************************************************************************
-*/

void	execScript
(
    int		scriptNum,	/* (in)  Which script?			*/
    int		total,		/* (in)  Total number of copies to make.*/
    int		numFiles,	/* (in)  Number of files on the unit.	*/
    char	workDir[PATH_MAX]
    				/* (in)	 Path to the work directory.	*/
)
{
    AM_DEVICE	*amDevicePtr;		/* AM device pointer.		*/

    char	arg0[PATH_MAX] = "xterm";
    char	arg1[3] = "-T";
    char	arg2[1600];
    char	arg3[3] = "-n";
    char	arg4[11] = "MEDIA WRITE";
    char	arg5[] = "-l";
    char	arg6[] = "-lf";
    char	arg7[PATH_MAX];
    char	arg8[3] = "-e";
    char	arg9[PATH_MAX];
    char	arg10[AM_MEDIA_UNIT_NAME_LEN];
    char	arg11[PATH_MAX];
    char	arg12[PATH_MAX];
    char	arg13[PATH_MAX];
    char	arg14[PATH_MAX];
    char	arg15[PATH_MAX];
    char	arg16[PATH_MAX];
    char	arg17[PATH_MAX];
    char	*arg18;


    /*
     *  Set up arguments to pass to exec().
     */

    if( ( amDevicePtr = eptritem( globals.gDeviceList, scriptNum ) ) == NULL )
    {
	return;
    }
    
		
    (void) sprintf( arg2, "MEDIA WRITE SCRIPT: %d   Media ID :  %d    "
	    "Media Unit Name :  %s  DEVICE: %s",
	    scriptNum, globals.gMediaId, globals.gMediaUnitName,
	    globals.gLogicalDeviceName );
    (void) sprintf( arg7, "%s/%s/MW.XTERM.%d.%s.log.%d",
	    globals.gMediaStagePath, globals.gMDSDirectory,
	    globals.gMediaId, globals.gMediaUnitName, scriptNum );
    (void) sprintf( arg9, "%s", globals.gScript );
    (void) sprintf( arg10, "%s", globals.gMediaUnitName );
    (void) sprintf( arg11, "%s", globals.gScriptConfigFPath );
    (void) sprintf( arg12, "%s.%d", globals.gScriptLogFileName, scriptNum );
    (void) sprintf( arg13, "%s", workDir );
    (void) sprintf( arg14, "%s,%s,%d,%d,%s",
	    amDevicePtr->amdDeviceName,
	    amDevicePtr->amdDeviceInfo,
	    amDevicePtr->amdRSpeed, amDevicePtr->amdWSpeed,
	    amDevicePtr->amdDeviceDriver );
    (void) sprintf( arg15, "%d", getNumCopies( total,
	    eptrlen( globals.gDeviceList ), scriptNum ) );
    (void) sprintf( arg16, "%d", numFiles );
    (void) sprintf( arg17, "%s/%s", globals.gRetrievalStagePath, 
	    globals.gMDSDirectory );    
    if ( globals.gSimulate )
    {
	arg18 = "-simulate";
    }
    else
    {
	arg18 = NULL;
    }


    /*
     * Remove any exiting log files.
     */

    (void) file_rm( arg7 );
    (void) file_rm( arg12 );
    

    /*
     * If we aren't exiting then exec the script.
     */

    if ( globals.gScriptKill == FALSE )
    {
	/*
	 * Exec the script.
	 */

	if ( execlp( arg0, arg0, arg1, arg2, arg3, arg4, arg5, arg6,
		arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15,
		arg16, arg17, arg18, (char *) NULL )
		== -1 )
	{
	    if ( arg18 == NULL )
	    {
		arg18 = "(null)";
	    }
	    formatMessage( E_FORK_N_EXEC, arg0, arg1, arg2, arg3, arg4,
		    arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12,
		    arg13, arg14, arg15, arg16, arg17, arg18 );
	    printMessage( msg, MESS_ERROR );
	    return;
	}
    }

    return;
}

/*+
************************************************************************
*
*   Function:  	getNumCopies
*
*   Purpose:
*	Determine the number of copies a particular device is to create.
*
*   Values Returned:
*	int	ret		: Number of copies to create.
*
*
************************************************************************
-*/

int	getNumCopies
(
    int		total,		/* (in)  Total num. of copies.		*/
    int		numDevices,	/* (in)  Number of devices used.	*/	
    int		device		/* (in)	 Which device to calc. for.	*/
)
{
    int		scale;		/* Scaling factor to use.		*/

    /*
     * Normalize the number of devices.
     */
	
    if ( numDevices > total )
    {
	numDevices = total;
    }


    /*
     * Find how many copies to write for this device.
     */
    
    if ( device >= total )
    {
	return 0;
    }

    scale = numDevices - total;
    if ( scale < 0 )
    {
	scale = 0;
    }
    
    return( ( total + device + scale ) / numDevices );
}

/*+
************************************************************************
*
*   Function:   parseLogs
*
*   Purpose:
*       Parse the write scripts' output for required keywords.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_LOG_FMT	: Error in the script's log file.
*	int	E_SYSTEM	: Error in system call. 
*
************************************************************************
-*/
 
static int      parseLogs
(
    int		numCopies,	/* (in)  Total Number of copies expected*/
    int		*totalWritten	/* (mod) Num copies written.		*/
)
{
    boolean	copiesSeen;	/* Have we seen 'copies'?		*/
    boolean	dumpLog;	/* Dump the script log to the mediaWrite*/
    				/* log file?				*/
    boolean	errorsSeen;	/* Have we seen 'errors'?		*/
    FILE        *fp;		/* File pointer.			*/
    int		index;		/* Log file index.			*/
    char        line[128];	/* Line from file.			*/
    char	logFileName[PATH_MAX];
    				/* The log file name.			*/
    int		numDevices;	/* Number of devices that were used.	*/
    int		numErrors;	/* Num of errors in a script's log.	*/
    int		numVerifyErrors;/* Num of verify error in a scipt log.	*/
    int		numWritten;	/* Num of copies written in log.	*/
    int		retStatus;	/* return status of parseLogs.		*/
    char	*scriptName;	/* The name of the script. 		*/
    int		status;		/* Log parsing  status.			*/
    int		totalErrors;	/* Num of errors in all script logs.	*/
    int		totalVerifyErrors;
    				/* Num of verify errors in all script logs.*/
    boolean	verifySeen;	/* Have we seen 'images'?		*/
    char      	xtermLog[PATH_MAX];
    				/* Log file generated by the xterm.     */
    
    *totalWritten = 0;
    totalErrors = 0;
    totalVerifyErrors = 0;
    numDevices = eptrlen( globals.gDeviceList );
    retStatus = S_SUCCESS;


    /*
     * Determine the script name.
     */

    CHECK_NULL( scriptName = strtail( globals.gScript ) );
    if ( streq( scriptName, "" ) )
    {
	CHECK_NULL( scriptName = strsav( globals.gScript ) );
    }
    

    for( index = 0; index < numDevices; index++ )
    {
	if ( getNumCopies( numCopies, numDevices, index ) < 1 )
	{
	    /*
	     * If there was nothing to write on this device, then
	     * it won't have a log file to parse.
	     */

	 continue;
	}
	
	copiesSeen = FALSE;
	errorsSeen = FALSE;
	verifySeen = FALSE;
	dumpLog = FALSE;
	status = S_SUCCESS;
	

	/*
	 *  Parse script log file looking for number of copies written,
	 *  images & errors.
	 */

	(void) sprintf( logFileName, "%s.%d", globals.gScriptLogFileName,
		index );
	if ( ( fp = fopen( logFileName, "r" ) ) == NULL )
	{
	    formatMessage( MSG_ERRNO, logFileName );
	    printMessage( msg, MESS_ERROR );
	    status = E_SYSTEM;
	}

 
	while ( status == S_SUCCESS && scriptName &&
		fgets( line, sizeof( line ), fp ) != NULL )
	{
	    /*
	     *  Look for write script messages in the log file.
	     */

	    if ( strncmp( line, scriptName, strlen( scriptName ) ) == 0 )
	    {
		if ( strstr( line, "copies" ) != NULL &&
			sscanf( line, "%*s %*s %*s %*s %*s %d", 
				&numWritten ) != 0 )
		{
		    copiesSeen = TRUE;
		    *totalWritten += numWritten;
		}
		else if ( strstr( line, "errors" ) != NULL &&	
			sscanf( line, "%*s %*s %*s %*s %*s %d", 
				&numErrors ) != 0 )
		{
		    errorsSeen = TRUE;
		    totalErrors += numErrors;
		}
		else if ( strstr( line, "verify" ) != NULL &&
			sscanf( line, "%*s %*s %*s %*s %*s %*s %d", 
				&numVerifyErrors ) != 0 )
		{
		    verifySeen = TRUE;
		    totalVerifyErrors += numVerifyErrors;
		}
	    }
	}


	/*
	 *  Make sure 'copies', 'verify' and 'errors' seen in logfile. 
	 */ 

	if ( status == S_SUCCESS &&
		( ! copiesSeen || ! errorsSeen || ! verifySeen ) )
	{
	    dumpLog = TRUE;
	    formatMessage( E_LOG_FMT,  logFileName );
	    printMessage( msg, MESS_ERROR );
	    status = E_LOG_FMT;
	}
	else if ( status == S_SUCCESS &&
		( numErrors != 0 || numVerifyErrors != 0 ) )
	{
	    dumpLog = TRUE;
	    formatMessage( S_SCRIPT_ERR, logFileName );
	    printMessage( msg, MESS_OPER );
	    status = S_SCRIPT_ERR;
	}
	else if ( status == S_SUCCESS &&
	   (  numWritten != getNumCopies( numCopies, numDevices, index ) ) )
	{
	    dumpLog = TRUE;
	    formatMessage( E_NUM_COPIES, scriptName, numWritten,
		    getNumCopies( numCopies, numDevices, index ) );
	    printMessage( msg, MESS_OPER );
	    status = E_NUM_COPIES;
	}


	if ( dumpLog )
	{
	    /*
	     * If an error occured, dump the script log file into the 
	     * mediaWrite log file.
	     */

	    formatMessage( S_SCRIPT_LOG, index, scriptName );
	    printMessage( msg, MESS_LOG );
	    CHECK_SYSTEM( fseek( fp, 0, SEEK_SET ), logFileName );
	    while ( fgets( line, sizeof( line ), fp ) != NULL )
	    {
		formatMessage( S_SCRIPT_LOG_DATA, line );
		printMessage( msg, MESS_LOG );
	    }
	    
	    formatMessage( S_SCRIPT_END, index, scriptName );
	    printMessage( msg, MESS_LOG );
	}
	(void) fclose( fp );


	if ( dumpLog )
	{
	    /*
	     * Copy the contents of the xterm's log file to mediaPrep's
	     * log file.
	     */

	    (void) sprintf( xtermLog, "%s/%s/MW.XTERM.%d.%s.log.%d",
		    globals.gMediaStagePath, globals.gMDSDirectory, 
		    globals.gMediaId, globals.gMediaUnitName, index );
	 
	    if ( ( fp = fopen( xtermLog, "r" ) ) == NULL )
	    {
		formatMessage( MSG_ERRNO, xtermLog );
		printMessage( msg, MESS_ERROR );
		status = E_SYSTEM;
	    }
	    else
	    {
		formatMessage( S_SCRIPT_LOG, index, "xterm" );
		printMessage( msg, MESS_LOG );
		CHECK_SYSTEM( fseek( fp, 0, SEEK_SET ), xtermLog );
		while ( fgets( line, sizeof( line ), fp ) != NULL )
		{
		    formatMessage( S_SCRIPT_LOG_DATA, line );
		    printMessage( msg, MESS_LOG );
		}
		formatMessage( S_SCRIPT_END, index, scriptName );
		printMessage( msg, MESS_LOG );
	    }
	}

	if ( status != S_SUCCESS && retStatus == S_SUCCESS )
	{
	    retStatus = status;
	}
	else if ( status < S_SUCCESS && retStatus >= S_SUCCESS )
	{
	    retStatus = status;
	}
    }

    if ( retStatus >= S_SUCCESS && totalErrors == 0 &&
	    totalVerifyErrors == 0 )
    {
	formatMessage( S_SCRIPT_NUM_DONE, *totalWritten );
	printMessage( msg, MESS_OPER );
    }
	
    
    return( retStatus );
}

/*+
************************************************************************
*
*   Function:   writeMedia
*
*   Purpose:
*	To execute CD authoring software in an xterm which burns an
*	image onto a CD-ROM.  The image to be burned must exist on 
*	magentic disk in the staging area. 
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*       int     E_SYSTEM	: System error occured during execlp()
*	int	E_DISPLAY	: DISPLAY env. not set.
*	int	E_AM		: Error in AM Library routine.
*
************************************************************************
-*/

int		writeMedia
( 
)
{

    int		numCopies;	/* Number of copies to write.		*/
    int		numWritten;	/* Number of copiess written.		*/


    /*
     * Initialize variables.
     */

    numWritten	= 0;
    if ( globals.gNumCopies == 0 )
    {
	numCopies = globals.gMedia.ammNumCopies - 
		globals.gMediaUnit.amuCopiesWritten;
    }
    else
    {
	numCopies = MIN( globals.gMedia.ammNumCopies - 
		globals.gMediaUnit.amuCopiesWritten, globals.gNumCopies );
    }


    /*
     * If there is something to be written and a script to call
     * then call the script.
     */

    if ( numCopies > 0 )
    {
	if ( strne( globals.gScript, "none" ) )
	{
	    CHECK( execScripts( numCopies ) );
	    CHECK( parseLogs( numCopies, &numWritten ) );
	}
    }
    else
    {
	formatMessage( S_NO_COPIES, globals.gMediaUnitName );
	printMessage( msg, MESS_OPER );
    }
    

    /*
     *  Update database tables. 
     */

    globals.gMediaUnit.amuCopiesWritten += numWritten;
    if ( globals.gMedia.ammNumCopies <= globals.gMediaUnit.amuCopiesWritten )
    {
	globals.gMediaUnit.amuStatus = AMU_WRITTEN;
	globals.gMediaUnit.amuDateCompleted = time( NULL );
	CHECK_AM( amUnitPut( &globals.gMediaUnit, globals.gDbProc1 ) );

	formatMessage( S_COMPLETE, globals.gMediaUnitName );
	printMessage( msg, MESS_OPER );
    }
    else
    {
	CHECK_AM( amUnitPut( &globals.gMediaUnit, globals.gDbProc1 ) );

	if ( globals.gNumCopies == 0 )
	{
	    formatMessage( E_NOT_COMPLETE, globals.gMediaUnit.amuCopiesWritten, 
		    globals.gMedia.ammNumCopies );
	    printMessage( msg, MESS_ERROR );
	}
    }

    return ( S_SUCCESS ); 
}
