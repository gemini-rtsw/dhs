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
*   Module Name:	mediaPrep/src/rollback.c
*
*   Purpose:
*	rollback 
*
*   Routines:
*	int	rollback	: Rollback a previously perfomed prep operation.
*
*   Date		: Feb 26, 1999
*
*   SCCS data		: @(#)
*	Module Name	: rollback.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Norm Hill
*
*   Modification History:
*	99/04/01 NRH  : Changed to continue with the rollback event if the
*			hard link directory cannot be deleted.
*     2000/03/17 SDJ  : Switched to fork()/exec() and UNIX rm command since
*                       the gen_file functions are unable to handle
*                       large files (bigger than 2 147 482 647 bytes).
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/types.h>
#include <sys/stat.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_file.h"
#include "gen_eptr.h"
#include "am.h"

#include "globals.h"

/*+
************************************************************************
*
*   Function:	rollback
*
*   Purpose:
*	Rollback a previously preparted unit.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_EMPTY_DIR	: Failed to delete file in directory.
*	int	E_AM		: AM library function failed.
*
************************************************************************
-*/

int		rollback
(
    void
)
{
    char	stageDirPath[PATH_MAX];	
				/* The full path to the media staging 	*/
				/* directory.				*/
    char	linkDir[PATH_MAX];	
				/* The full path to the retrieval 	*/
				/* staging directory.			*/

    /*
     * Delete the contents of the staging directory. Build the path to dir,
     * delete files or directories.
     *
     *********************************************************************
     *                       NOTE
     *********************************************************************
     *
     * Currently a fork()/exec() is used with the "rm" command to
     * delete the contents of the media staging directory.  The gen
     * library file() function should be used but do to failures in
     * the lstat() function it fails.  lstat() fails because the file
     * size overflows when removing large UDF images.  The original
     * code has been retained but ifdef'ed out.
     */
    
#ifdef OMIT
    (void) sprintf( stageDirPath, "%s/%s", globals.gMediaStagePath, 
	    globals.gMDSDirectory );
    if ( ! file_dir_empty( stageDirPath ) )
    {
	formatMessage( MSG_ERRNO, "file_dir_empty()" );
	formatMessage( E_EMPTY_DIR, stageDirPath );
	printMessage( msg, MESS_ERROR );
	return( E_EMPTY_DIR );
    }
#endif

    (void) sprintf( stageDirPath, "%s/%s/*.*", globals.gMediaStagePath, 
	    globals.gMDSDirectory );
    if (  execCommand( "rm", 2, "-rf", stageDirPath ) != S_SUCCESS )
    {
	formatMessage( E_EMPTY_DIR, stageDirPath );
	printMessage( msg, MESS_ERROR );
	return( E_EMPTY_DIR );
    }	

    formatMessage( S_ROLLBACK, globals.gMediaUnit.amuMediaUnitName, 
	    globals.gMediaId );
    printMessage( msg, MESS_OPER );


    /*
     * Empty the directory containing the links.
     *
     *********************************************************************
     *                       NOTE
     *********************************************************************
     *
     * Currently a fork()/exec() is used with the "rm" command to
     * delete the contents of the media staging directory.  The gen
     * library file() function should be used but do to failures in
     * the lstat() function it fails.  lstat() fails because the file
     * size overflows when removing large UDF images.  The original
     * code has been retained but ifdef'ed out.
     */

#ifdef OMIT
    (void) sprintf( linkDir, "%s/%s", globals.gRetrievalStagePath,
	    globals.gMDSDirectory );
    if ( ! file_dir_empty( linkDir ) )
    {
	formatMessage( MSG_ERRNO, "file_dir_empty()" );
	formatMessage( E_EMPTY_DIR, linkDir );
	printMessage( msg, MESS_ERROR );
    }
#endif

    (void) sprintf( linkDir, "%s/%s/*.*", globals.gRetrievalStagePath,
	    globals.gMDSDirectory );
    if (  execCommand( "rm", 2, "-rf", linkDir ) != S_SUCCESS )
    {
	formatMessage( E_EMPTY_DIR, stageDirPath );
	printMessage( msg, MESS_ERROR );
	return( E_EMPTY_DIR );
    }	
    

    /*
     * Unbook the media stage directory in the database.
     */

    CHECK_AM( amMSClear( &(globals.gMediaStage), globals.gDbProc1 ) );

    globals.gMediaUnit.amuStatus = AMU_QUEUED;
    CHECK_AM( amUnitPut( &(globals.gMediaUnit), globals.gDbProc1 ) );


    formatMessage( S_ROLLBACK_DONE, globals.gMediaUnit.amuMediaUnitName, 
	    globals.gMediaId );
    printMessage( msg, MESS_OPER );


    return( S_SUCCESS );
}
