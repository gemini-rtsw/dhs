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
*   Module Name:	mediaCleanup/src/process.c
*
*   Purpose:
*	Contains routines for performing mediaCleanup's processing.
*
*   Routines:
*	static 	int	cleanRStage	: Clean the retrieval staging area.
*	static	int	mediaUpdate	: Updates the media table.
*	int		process		: Perform mediaCleanup's processing.
*
*   Date		: Dec 12, 1996
*
*   SCCS data		: @(#)
*	Module Name	: process.c
*	Version Number	: 1.22
*	Release Number	: 1
*	Last Updated	: 03/27/00
*
*   Programmers		: Marc LeBlanc
*			: Steve Cockayne
*
*   Modification History:
*	97/01/28 SEC  :	Mod'ed to set unit status dependent on queue type.
*	97/02/03 SEC  :	Mod'ed to just remove data and gear directories.
*	97/02/04 SEC  :	Fixed bug trying to remove non-existent dirs.
*	97/02/05 SEC  :	Changed cleanDir to NOT remove top-level dir.
*	97/02/21 SEC  :	Fixed up some things. 
*	97/02/27 SEC  :	Added code to delete the data stage directory.
*	97/02/28 SEC  :	Fixed bug in mediaUpdate().
*       97/07/03 RG   : For internal source change status directly to
*                       AMU_COMPLETE
*	97/07/16 SEC  : Slight logic change to status updating.
*	98/10/26 SDJ  : Added functionality to clean-up retrieval area.
*	98/12/14 SDJ  : Updated to new AM.  Changed algorithm to match
*			algorithm used by the Storage Server.
*	99/01/07 SDJ  : Removed cleanDir(), switch from UNIX file functions
*			to ones in gen_file.
*	99/02/24 SDJ  : No longer require a staging directory.  If no
*			staging directory is found then then only the
*			files are removed.
*	99/03/17 NRH  : Added some information to the S_CLEANED_UP message.
*	99/04/12 SDJ  : Fixed a bug in process(), amStatus was not being
*			handled properly.
*	99/04/28 NRH  : Added a dbcancel to mediaUpdate.
*	99/10/18 NRH  : Fixed a bug. Added code to set a unit's status 
*			directly to done if there is not client to notify.
*	99/11/19 SDJ  : No longer a fatal error if a directory can not
*			be removed.
*	00/03/27 NRH  : Changed to use system and rm to remove directory 
*			contents because the call to the file function fails.
*			The gen_file functions fail on files whose size is
*			greater than MAXINT since stat() fails.
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
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
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
#include "gen_msg.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "gen_file.h"
#include "db.h"
#include "am.h"


/*
 ************************************************************************
 *  Media Cleanup include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */

static	int	cleanRStage( char * );
static	int	mediaUpdate( void );

/*+
************************************************************************
*
*   Function:	cleanRStage
*
*   Purpose:
*	Remove all of the files from the retrieval staging area.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_SYSTEM	: Error in system call. 
*
************************************************************************
-*/
int	cleanRStage
(
    char	*stagePath	/* (in)  Path to retrieval staging area.*/
)
{
    AM_FILE	amFile;		/* AM lib. file structure.		*/
    AM_FILE	*amFilePtr;	/* AM lib. file structure ptr.		*/
    int		amStatus;	/* Return status from AM lib. function.	*/
    EPTR	*dirList;	/* List of directories.			*/
    char        *dirPathPtr;	/* String to add to eptr list.          */
    char	fullDirPath[PATH_MAX];
    				/* Full path to a directory.		*/
    EPTR	*fileList;	/* List of files to delete.		*/
    char	filePath[PATH_MAX];
    				/* Full path to the file.		*/
    int		index;		/* index for eptr. list.		*/
    int		systemStatus;	/* Status of system operations.		*/


    CHECK_NULL( fileList = eptr_alloc( 1000 ) );
    CHECK_NULL( dirList = eptr_alloc( 500 ) );
    systemStatus = S_SUCCESS;
    
    
    /*
     * Construct list of files to be removed.
     */

    for( index = 0, amStatus = amFileGet( AM_FG_MEDIA_UNIT, &amFile,
	    globals.gDbProc, globals.gMediaUnit ); amStatus == AM_SUCCESS;
	    index++, amStatus = amFileGet( AM_FG_NEXT, &amFile,
	    globals.gDbProc ) )
    {
	CHECK_NULL( amFilePtr = (AM_FILE*) gen_alloc( sizeof( AM_FILE ) ) );
	CHECK_NULL( gen_cpy( amFilePtr, &amFile, sizeof( AM_FILE) ) );
	CHECK_NULL( eptr_insert( fileList, eptrlen( fileList ), amFilePtr ) );
    }

    if ( amStatus != AM_NO_FILE )
    {
	CHECK_AM( amStatus );
    }
    CHECK_NULL( eptr_sort( fileList, amFileCompare ) );

    
    
    /*
     * Loop thru the file list to create a directory list, remove
     * the files, and update the file status in the database..
     */

    for( index = ( eptrlen( fileList ) - 1 ); index > -1; index -- )
    {
        CHECK_NULL( amFilePtr = (AM_FILE *) eptritem( fileList, index ) );


	(void) sprintf( filePath, "%s/%s", stagePath, amFilePtr->amfFilename );
	if ( ! file_rm( filePath ) )
	{
	    if ( errno != 0 )
	    {
		formatMessage( MSG_ERRNO, filePath );
	    }
	    else
	    {
		formatMessage( E_FILE_RM, filePath );
	    }
	    printMessage( msg, MESS_OPER);
	    systemStatus = E_SYSTEM;
	    continue;
	}
	else
	{
	    formatMessage( S_REMOVED, amFilePtr->amfFilename );
	    printMessage( msg, MESS_INFO );
	    amFilePtr->amfStatus = AMF_DONE;
	    CHECK_AM( amFilePut( amFilePtr, globals.gDbProc ) );
	}


	CHECK( checkNewDir( amFilePtr, dirList ) );
    }



    /*
     * Loop through directory list, removing any empty directories.
     */

    for( index = ( eptrlen( dirList ) - 1 ); index > -1; index -- )
    {
	CHECK_NULL( dirPathPtr = eptritem( dirList, index ) );
	(void) sprintf( fullDirPath, "%s/%s", stagePath, dirPathPtr );
	if ( rmdir( fullDirPath ) != 0 && errno != EEXIST &&
		errno != ENOTEMPTY && errno != ENOENT )
	{
	    if ( errno != 0 )
	    {
		formatMessage( E_DIR_RM, fullDirPath );
		formatMessage( MSG_ERRNO, fullDirPath );
	    }
	    else
	    {
		formatMessage( E_DIR_RM, fullDirPath );
	    }
	    printMessage( msg, MESS_OPER );
	}
    }
    CHECK( systemStatus );


    /*
     * Free used memory.
     */
    
    eptr_free( fileList, free );
    fileList = NULL;
    eptr_free( dirList, free );
    dirList = NULL;
    
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	mediaUpdate
*
*   Purpose:
*	Updates the media table if necessary. 
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in am library.
*	int	E_NO_MEDIA	: No media tuple matches query.
*
************************************************************************
-*/

static	int	mediaUpdate
(
    void
)
{
    AM_FILE	amFile;		/* Media file structure.		*/
    AM_MEDIA	amMedia;	/* Media structure.			*/
    int         amStatus;       /* Ret. status from amGet().            */
    char	dataStageDir[PATH_MAX];
				/* Full path to data stage directory.	*/


    /*
     *  Fetch the latest media table tuple matching the MediaId. 
     */

    amStatus = amGet( AM_MG_MEDIA_REQUEST_ID, &amMedia, globals.gDbProc,
            globals.gMediaId );
    if ( amStatus == AM_NO_MEDIA )
    {
        formatMessage( E_NO_MEDIA, globals.gMediaId );
        printMessage( msg, MESS_ERROR );
        return( E_NO_MEDIA );
    }
    CHECK_AM( amStatus );
    

    /*
     *  If user media has status AMM_DONE, and all files are either
     *  AMF_WRITTEN or AMF_COMPLETE, then this media is AMM_DONE.
     */

    if ( amMedia.ammStatus != AMM_IN_PROG ||
	    streq( amMedia.ammMediaRequestType, AM_RT_ARCHIVE ) )
    {
	return( S_SUCCESS );
    }

    if ( ( amStatus = amFileGet( AM_FG_UNWRITTEN, &amFile,
	    globals.gDbProc, amMedia.ammMediaRequestId ) ) == AM_NO_FILE )
    {
	if ( streq( globals.gMedia.ammSourceId, AM_SRC_INTERNAL ) )
	{
	    CHECK_AM( amFileUpdate( AM_FU_COMPLETE_USER, globals.gDbProc,
		    amMedia.ammMediaRequestId ) );
	    globals.gMedia.ammStatus = AMM_DONE;
	    CHECK_AM( amPut( &globals.gMedia, globals.gDbProc ) );

	    /*
	     *  Remove User/Arch Data Stage dir.
	     */

	    (void) sprintf( dataStageDir, "%s/%d.%s", globals.gDataStage,
		    globals.gMediaId, globals.gMedia.ammMediaRequestName );
	    if ( rmdir( dataStageDir ) != 0 )
	    {
		if ( errno != 0 )
		{
		    formatMessage( MSG_ERRNO, dataStageDir );
		}
		else
		{
		    formatMessage( E_DIR_RM, dataStageDir );
		}
		printMessage( msg, MESS_ERROR );
		return( E_SYSTEM );
	    }
	}
    }
    else
    {
	CHECK_DB( dbcancel( globals.gDbProc ) );
	CHECK_AM( amStatus );
    }


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	process
*
*   Purpose:
*	Process. 
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM 		: Error in am library.
*	int	E_NO_MEDIA	: No media tuple matches query.
*	int	E_SYSTEM 	: Error in system call.
*
************************************************************************
-*/

int		process
(
    void
)
{
    AM_MSTAGE   amMStage;       /* The media stage structure.           */
    int         amStatus;       /* Ret. status from amGet().            */
    char	dirPath[PATH_MAX];
    				/* Path to directory to clean out.	*/
    int         status;		/* Function return status.		*/


    status = S_SUCCESS;
    amStatus = AM_SUCCESS;
        
    
    if ( strne( globals.gMDSDirectory, "" ) )
    {
	/*
	 * Clean out the staging directory containing the hard links,
	 * remove them all.
	 */

	(void) sprintf ( dirPath, "%s/%s",  globals.gDataStage,
		globals.gMDSDirectory  );
	if ( ! file_dir_empty( dirPath ) )
	{
	    formatMessage( E_DIR_EMPTY, dirPath );
	    printMessage( msg, MESS_ERROR );
	    status = E_DIR_EMPTY;
	}
    
    

	/* 
	 * Clean out the Media Data Stage directory's, delete its contents
	 * and free media staging directory.
	 *********************************************************************
	 *                       NOTE
	 *********************************************************************
	 *
	 * Currently a system call is used with the "rm" command to
	 * delete the contents of the media staging directory.  The gen
	 * library file() function should be used but do to failures in
	 * the lstat() function it fails.  lstat() fails because the file
	 * size overflows when removing large UDF images.  The original
	 * code has been retained but ifdef'ed out.
	 */

#ifdef OMIT
	(void) sprintf ( dirPath, "%s/%s",  globals.gMediaDataStage,
		globals.gMDSDirectory  );
	if ( ! file_dir_empty( dirPath ) )
	{
	    formatMessage( E_DIR_EMPTY, dirPath );
	    printMessage( msg, MESS_ERROR );
	    status = E_DIR_EMPTY;
	}
#endif
	(void) sprintf ( dirPath, "rm -rf %s/%s/*",  globals.gMediaDataStage,
		globals.gMDSDirectory  );
	if ( system( dirPath ) != 0 )
	{
	    formatMessage( MSG_ERRNO, dirPath );
	    printMessage( msg, MESS_ERROR );
	    formatMessage( E_DIR_EMPTY, dirPath );
	    printMessage( msg, MESS_ERROR );
	    status = E_DIR_EMPTY;
	}

	amMStage.amsMediaRequestId = globals.gMediaId;
	(void) sprintf( amMStage.amsDirectory, "%s", globals.gMDSDirectory );
	(void) sprintf( amMStage.amsMediaUnitName,"%s",
		globals.gMediaUnitName );
	amStatus = amMSClear( &amMStage, globals.gDbProc );
    }
    
    
    /*
     * Clean out the Retreival staging area.
     */
    
    (void) sprintf ( dirPath, "%s/%d.%s",  globals.gDataStage,
	    globals.gMedia.ammMediaRequestId,
	    globals.gMedia.ammMediaRequestName );
    if ( cleanRStage( dirPath ) != S_SUCCESS )
    {
	formatMessage( E_CLEAN_RSTAGE, dirPath );
	printMessage( msg, MESS_ERROR );
	status = E_CLEAN_RSTAGE;
    }
    CHECK_AM( amStatus );
    


    if ( status == S_SUCCESS )
    {
	/* 
	 *  Update media unit's state. 
	 */
    
	if ( streq( globals.gMedia.ammSourceId, AM_SRC_INTERNAL ) )
	{
	    globals.gMediaUnit.amuStatus = AMU_DONE;
	}
	else
	{
	    globals.gMediaUnit.amuStatus = AMU_CLEANED_UP;
	}
	CHECK_AM( amUnitPut( &globals.gMediaUnit, globals.gDbProc ) );


	/* 
	 *  If possible, update media table to 'done writing',
	 *  remove Data Stage. 
	 */

	CHECK( mediaUpdate( ) );
    }
    else
    {
	return( status );
    }
    


    /*
     *  If got to this point, must have mediaCleanedup OK.
     */

    formatMessage( S_CLEANED_UP, globals.gMediaUnit.amuMediaUnitName,
	    globals.gMediaUnit.amuMediaRequestId );
    printMessage( msg, MESS_OPER );

    return( S_SUCCESS );
}
