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
*   Module Name:	mediaPrep/src/process.c
*
*   Purpose:
*	Contains routines for processing the Media Name. 	
*
*   Routines:
*       static int      createLink      : Create a file link in link directory
*	static 	int 	fetchMediaFiles	: Fetch list of media_files.
*	int		process		: Process the media ID.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: process.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 03/18/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/09/09 SDJ	: Copied from cdPrep
*	98/10/20 SDJ	: Added code for DVD file & unit sizes.
*       98/11/06 SDJ    : Made sure struct stat is initialized and
*			  removed large file size support.
*	99/01/06 SDJ	: Added createLink()
*       99/01/06 SDJ    : Use gen_file instead of stat() and other UNIX 
*                         file functions.  Added functionality to create
*                         a directory of hard links to the data.  Altered
*                         logic of media files, goes through entire list
*                         of files reguardless of errors.
*	99/02/24 SDJ	: Fixed E_FILESIZE msg.  Was calling with
*			  an insufficient number of parameters.
*	99/03/18 NRH	: Changed the status returned when preparation fails.
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
 *  Media Prep  include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */

static	int	createLink( AM_FILE );
static	int     fetchMediaFiles( EPTR * );


/*+
************************************************************************
*
*   Function: createLink
*
*   Purpose:
*	Creates a file link to the real dats in the directory tree
*	that is used in the preparation.  This directory contains
*	the entire contents of the media unit.
*
*   Values Returned:
*	int	S_SUCCESS 	: Normal completion.
*	int	E_LINK		: Error in system call.
*
************************************************************************
-*/

int	createLink
( 
    AM_FILE	amFile		/* (in)  Ptr. to media file structure.	*/
)
{
    char        dest[PATH_MAX];    /* Source file.			*/
    char        source[PATH_MAX];  /* Destination file.			*/


    /* 
     *  Create the paths
     */

    (void) sprintf( source, "%s/%d.%s/%s", globals.gRetrievalStagePath, 
	    globals.gMediaId, globals.gMedia.ammMediaRequestName,
	    amFile.amfFilename );
    (void) sprintf( dest, "%s/%s/%s", globals.gRetrievalStagePath,
	    globals.gMDSDirectory, amFile.amfFilename );

    if ( ! file_link_mk( source, dest, S_IRWXU | S_IRWXG | S_IRWXO ) )
    {
	formatMessage( MSG_ERRNO, amFile.amfFilename );
	formatMessage( E_LINK, amFile.amfFilename );
	printMessage( msg, MESS_ERROR );
	return( E_LINK );
    }
    

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   fetchMediaFiles
*
*   Purpose:
*	Builds EPTR list of media_files table entries for a media unit. 
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the am library.
*	int	E_LINK		: Error in system call.
*	int	E_MEMORY	: A memory error occurred.
*
************************************************************************
-*/

static	int	fetchMediaFiles
(
    EPTR 	*fileList	/* (mod) eptr list of chosen files.     */
)
{
    AM_FILE	amFile;		/* Media file struct.			*/
    AM_FILE	*amFilePtr;	/* Ptr. to media file struct.		*/
    char        amString[AM_STATUS_SHORT_LEN];  
				/* Return string from amStatusStr().    */
    boolean	fileError;	/* Is the current file errored?		*/
    char	fpath[PATH_MAX];/* complete path to file for stat.*/
    int		fileStatus;	/* Ret. status from amFileGet().	*/
    int		linkStatus;	/* Ret. status from createLink().	*/
    struct stat	stat_struct;    /* Return struct from file().		*/


    for ( fileStatus = amFileGet( AM_FG_MEDIA_UNIT, &amFile, 
		    globals.gDbProc1, &globals.gMediaUnit );
	    fileStatus == AM_SUCCESS;
	    fileStatus = amFileGet( AM_FG_NEXT, &amFile,
		    globals.gDbProc1, &globals.gMediaUnit ) )
    {
	/*
	 *  Compose full path of file, so it can be verified with stat.
	 */
	 
	fileError = FALSE;
	switch ( amFile.amfStatus )
	{
	    case AMF_RETRIEVED:
		(void) sprintf( fpath, "%s/%d.%s/%s",
			globals.gRetrievalStagePath,
			globals.gMediaId, globals.gMedia.ammMediaRequestName, 
			amFile.amfFilename );
		break;
	    case AMF_UNAVAILABLE:
	    case AMF_UNPROCESSED:
	    case AMF_DONE:
	    case AMF_ERROR:
		CHECK_AM( amStatusStr( globals.gDbProc1, amString,
			"media_files", amFile.amfStatus ) );
		formatMessage( E_BAD_F_STATUS, amFile.amfFileId, amString );
		printMessage( msg, MESS_ERROR );
		break;
	}
	

	/*
	 *  If the file has been retrived stat it. 
	 */
	 
	if ( amFile.amfStatus == AMF_RETRIEVED )
	{
	    if ( ! file( fpath, &stat_struct ) &&
		    ! file_link( fpath, &stat_struct ) )
	    {
		fileError = TRUE;
		formatMessage( MSG_ERRNO, fpath );
		printMessage( msg, MESS_ERROR );
	    }
	    else if ( amFile.amfFileSize != stat_struct.st_size )
	    {
		fileError = TRUE;
		formatMessage( E_FILESIZE, amFile.amfFileSize,
			stat_struct.st_size, amFile.amfFileId );
		printMessage( msg, MESS_ERROR );
	    }
	}


	/*
	 *  If file has existence problem or size problem, update db.
	 */
	 
	if ( fileError )
	{
	    amFile.amfStatus = AMF_UNAVAILABLE;
	    CHECK_AM( amFilePut( &amFile, globals.gDbProc2 ) );
	}


	/*
	 *  Create the hard link for this file.
	 */

	linkStatus = createLink( amFile );
        

	/*
	 *  Add the file to the file list. Update stats. 
	 */

	CHECK_NULL( amFilePtr = gen_alloc( sizeof( AM_FILE ) ) );
	(void) memcpy( amFilePtr, &amFile, sizeof( AM_FILE ) );
	CHECK_NULL( eptr_insert( fileList, eptrlen( fileList ), 
		amFilePtr ) );
	CHECK( addToFileStats( amFile.amfStatus, amFile.amfMediaUnitName ) );
    }

    if ( eptrlen( fileList ) == 0 )
    {
	formatMessage( E_NO_FILES, globals.gMedia.ammMediaRequestId,
		globals.gMediaUnit.amuMediaUnitName );
	printMessage( msg, MESS_ERROR );
	return( E_NO_FILES );
    }
    else if ( fileStatus != AM_NO_FILE )
    {
	CHECK_AM( fileStatus );
    }
    else
    {
	CHECK( linkStatus );
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	process
*
*   Purpose:
*	Process a unit of media.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		process
(
    void
)
{
    int		fetchStatus;	/* Ret. status from fetchMediaFiles().	*/
    EPTR	*fileList;	/* EPTR list for files.			*/
    char	linkDir[PATH_MAX];
				/* Dir. of hard links to data.          */
    int		prepStatus;	/* Return status from prepMedia.	*/
    

    /*
     *  If phys. vol. not previously prepared by mediaPrep and it has
     *  been queued, prepare.
     */

    fileList = NULL;
    if ( globals.gMediaUnit.amuStatus != AMU_PREPARED &&
	    globals.gMediaUnit.amuStatus == AMU_QUEUED )
    {
	/*
	 * Empty the directory that will contain the links.
	 */

	(void) sprintf( linkDir, "%s/%s", globals.gRetrievalStagePath,
		globals.gMDSDirectory );
	if ( ! file_dir_empty( linkDir ) )
	{
	    formatMessage( MSG_ERRNO, linkDir );
	    formatMessage( E_EMPTY_DIR, linkDir );
	    printMessage( msg, MESS_ERROR );
	    return( E_EMPTY_DIR );
	}
        
        
	/*
	 *  Allocate eptr list for files. Populate the eptr list. 
	 */
        
	CHECK_NULL( fileList = eptr_alloc( 1000 ) );
	fetchStatus = fetchMediaFiles( fileList );
	if ( fetchStatus == S_SUCCESS )
	{
	    (void) printFileStats( "Start" );
	}
	else
	{
	    eptr_free( fileList, free );
	}
	CHECK( fetchStatus );


	/*
	 *  If we're not yet ready to prepare the media, exit. 
	 */
	  
	if ( ( globals.gNumQueued != globals.gNumFiles )
    		|| ( globals.gNumInRS != globals.gNumFiles ) )
	{
	    formatMessage( S_CANT_PREP );
	    printMessage( msg, MESS_OPER );
	    return( S_CANT_PREP );
	}


	/*
	 * Prepare the physical image on magnetic disk
	 */

	prepStatus = prepMedia( fileList );
	if ( fileList != NULL )
	{
	    eptr_free( fileList, free );
	}
	CHECK( prepStatus );
    }
    else
    {
	formatMessage( E_BAD_U_STATUS );
	printMessage( msg, globals.gMediaUnit.amuStatus);
	return( S_SUCCESS );
    }
     
    return( S_SUCCESS );
}

