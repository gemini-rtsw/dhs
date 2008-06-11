/*
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
*   Module Name:	mediaQueue/src/process.c
*
*   Purpose:
*	Contains routines for processing the Media ID.
*
*   Routines:
*	static int 	buildUnits	: Builds units of media for a media ID.
*	static int 	checkRecovery	: If recovery is necessary, recovers.
*	static int 	fetchMediaFiles	: Get media_files, check validity.
*	int		process		: Process the media ID.
*	static int 	queueUnit	: Queues a unit of media.
*	static int	selectItems	: Selects items for a media unit.
*	static int 	selectTree	: Select a dir. tree for unit.
*
*   Date		: <Jan 28, 1998>
*
*   SCCS data		: @(#)
*	Module Name	: process.c
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 01/28/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/02/27 SDJ	: Updated to new am library.
*	98/10/10 SDJ	: Added new algorithm for directory splitting.
*	98/11/23 SDJ	: Fixed dir. split algorithm by first moving builUnit
*			  to selectTree(), simplified the data gathering,
*			  and fixed mucho bugs.
*	98/12/06 SDJ	: Upgraded to new AM, introduction of mediaTypes
*			  table.
*	99/01/06 SDJ	: Use gen_file instead of stat() and other UNIX 
*			  file functions.  Altered logic which determines
*			  whether or not the writing was forced.
*	99/02/24 SDJ	: Changed logic where units are "forced" or not.
*			  Only the last unit is forced if it is not big
*			  enough, otherwise they are not forced.
*	99/03/14 SDJ	: Altered selection algorithm to correctly
*			  calculate file and dir overhead for a given file.
*	99/03/29 NRH	: Spelling fixes.
*	99/03/29 SDJ	: Added sorting of files, largest to smallest, when
*			  determining number of media needed.  File status
*			  is changed to unavailable if file does not exit
*			  on disk, otherwise set it to Error.  Changing
*			  splitting algorithm to evenly distribute directories
*			  if parent tree has no files or at least one unit
*			  worth of files in it.
*       99/04/26	: Fixed msg display bug when file is not found.
*     2000/02/03 SDJ	: Use UNIT_FILLED, rather than UNIT_FULL macro to 
*			  allow the selection process to stop when the
*			  first file does not fit, rather than trying
*			  to completely fill the units.
*    2000/03/17 SDJ	: Made better use of MACROS.  Fixed the tiSubTreeSize
*			  so that it is decremented properly when files
*			  are selected for a media unit. No longer tries
*			  to split directories evenly across media, since
*			  this is effective only if all of the data to be
*			  written has been placed into the database, which
*			  is rare.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

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
#include "gen_msg.h"
#include "gen_eptr.h"
#include "gen_file.h"
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

static	int     buildUnits( EPTR *, TREE_INFO * );
static	int     checkRecovery( void );
static	int     fetchMediaFiles( EPTR *, TREE_INFO * );
static	int     queueUnit( EPTR *, AM_UNIT * );
static 	int	selectDir( double, int, TREE_INFO *, double *, AM_UNIT * );
static 	int	selectItems( TREE_TYPE, boolean, int, TREE_INFO *, double *,
			AM_UNIT * );
static	int     selectTree( boolean, TREE_INFO *, double *, AM_UNIT * );

/*+
************************************************************************
*
*   Function:	buildUnits
*
*   Purpose:
*	Build units of media using the parameter list of media files. 
*
*   Values Returned:
*	int	S_SUCCESS	: Successful completion.
*	int	E_MEMORY	: Memory Error.
*	int	E_SELECT_ITEM	: Unable to select item.
*	int	E_TREE_TYPE	: Bad tree type given to select items.
*	int	E_AM		: Error in the CADC am library. 
*	int	E_MEMORY	: Memory Error.
*	int	E_UNITNAME_FMT	: Error formatting new media unit name.
*
************************************************************************
-*/

static	int	buildUnits
(
    EPTR 	*fileList,	/* (mod) eptr list of chosen files. 	*/
    TREE_INFO	*treePtr	/* (mod) top of dir. tree sturcture.	*/
)
{
    AM_UNIT	amUnit;		/* Media Unit that is created.		*/
    int		buildStatus;	/* Ret. status from buildUnit().	*/
    double	unitSize;	/* Size of media unit (in blocks).	*/
    double	percentage;	/* How full the media is.		*/

    for( ;globals.gNumMedia > 0 && eptrlen( fileList ) > 0;
	 globals.gNumMedia--, globals.gNumMediaFound-- )
    {
	/*
	 *  Create the next possible unit of media.
	 */

	CHECK( createUnit( &amUnit ) );


	/*
	 *  If we successfully build a unit, queue it. Else we're done. 
	 */

	 
	formatMessage( S_PREPARING, globals.gMediaType.amtTypeName,
		amUnit.amuMediaUnitName );
	printMessage( msg, MESS_OPER );

	unitSize = 0;
	buildStatus = selectTree( TRUE, treePtr, &unitSize, &amUnit );
	

	/* Calculate the percentage size and the unit size in
	 * kilobytes. The local unit size is in blocks but it is stored
	 * in the database as kilobytes.
	 */
	percentage = ( unitSize / globals.gMaxUnitSize ) * 100.0;
	amUnit.amuTotalSize = ( unitSize *
		(double) globals.gMediaType.amtBlockSize ) / 1024;

	/*
	 * We create the unit if:
	 *	1. the unit is not empty, 
	 *	   no errors occured while selecting the data, and
	 *	   the number of media Found is > 1.
	 *	2. The unit is full, and
 	 *	   no errors occured while selecting the data, and
	 * We force the creation of the unit if:
	 *	1. The unit is not empty,
	 *	   no errors occured while selecting the data,
	 *	   the unit is not full, and
	 *	   forcing was specified.
	 */

	
	if ( ( buildStatus == S_SUCCESS || buildStatus == S_FULL )
		&& unitSize > 0
		&& ( UNIT_FILLED( unitSize ) || globals.gForce ) )
	{
	    CHECK( printUnit( *treePtr, amUnit ) );
	    
	    if ( ! UNIT_FILLED( unitSize ) && globals.gForce )
	    {

		formatMessage( S_FORCING, globals.gMediaType.amtTypeName, 
			amUnit.amuMediaUnitName, percentage );
		printMessage( msg, MESS_OPER );
	    }
	    else
	    {
		formatMessage( S_QUEUING, globals.gMediaType.amtTypeName, 
			amUnit.amuMediaUnitName );
		printMessage( msg, MESS_OPER );
	    }
	    
	    CHECK( queueUnit( fileList, &amUnit ) );
	    
	    formatMessage( S_COMPLETE, globals.gMediaType.amtTypeName, 
		    amUnit.amuMediaUnitName, percentage );
	    printMessage( msg, MESS_OPER );
	}
	else
	{
	    formatMessage( S_NOT_ENOUGH, globals.gMediaType.amtTypeName,
		    amUnit.amuMediaUnitName, percentage,
		    globals.gNumMedia );
	    printMessage( msg, MESS_OPER );
	    buildStatus = S_QUIT;
	}

	
	CHECK( buildStatus );
    }
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   checkRecovery
*
*   Purpose:
*       Checks whether or not a media ID has had queuing partially 
*	completed. If so, resets media file statusses and deletes 
*	the offending media unit so queuing can be re-done. 
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in am library routine.
*
************************************************************************
-*/

static	int		checkRecovery
(
    void
)
{
    AM_UNIT	amUnit;		/* Media unit struct.			*/
    int		unitStatus;	/* Ret. status from amUnitGet().	*/

	
    for ( unitStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID, &amUnit, 
		    globals.gDbProc1, globals.gMedia.ammMediaRequestId );
	    unitStatus == AM_SUCCESS;
	    unitStatus = amUnitGet( AM_UG_NEXT, &amUnit,
		    globals.gDbProc1, globals.gMedia.ammMediaRequestId ) )
    {
	/*
	 *  If a unit exists with status AMU_QUEUEING, mediaQueue must
	 *  have exited without setting status to AMU_QUEUED, thus the
	 *  unit needs removing, and its files need resetting. 
	 *  Therefore, if the status is NOT AMU_QUEUEING, the unit is OK.
	 */

	if ( amUnit.amuStatus != AMU_QUEUEING )
	{
	    continue;
	}


	/*
	 *  Status is AMU_QUEUEING. Reset media_files. Delete media_unit.
	 */

	CHECK_AM( amFileUpdate( AM_FU_MEDIA_Q_ROLLBACK, 
		globals.gDbProc2, amUnit.amuMediaRequestId, 
		amUnit.amuMediaUnitName ) );

	CHECK_AM( deleteUnit( &amUnit, globals.gDbProc2 ) );

    }

    if ( unitStatus == AM_NO_UNITS )
    {
	return( S_SUCCESS );
    }
    CHECK_AM( unitStatus );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   fetchMediaFiles
*
*   Purpose:
*	Builds EPTR list of media_files table entries for a media ID. 
*
*   Values Returned:
*	int	S_NO_FILES	: Success, but no files ready to queue.
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the am library.
*	int	E_FILESIZE	: Actual file size does not match database size.
*	int	E_FILE_TOBIG	: File is to big for media.
*	int	E_MEMORY	: A memory error occurred.
*
************************************************************************
-*/

static	int		fetchMediaFiles
(
    EPTR 	*fileList,	/* (mod) eptr list of chosen files.     */
    TREE_INFO	*treeTop	/* (mod) directory tree structure.	*/
)
{
    AM_FILE	amFile;		/* Media file struct.			*/
    AM_FILE	*amFilePtr;	/* Ptr. to media file struct.		*/
    int		fileAddStatus;	/* Ret. status from fileListAdd.	*/
    boolean	fileError;	/* Are there any file errors?		*/
    FILE_INFO	*filePtr;	/* File information structure.		*/
    char	fpath[PATH_MAX];/* complete path to file for stat.	*/
    int		fileStatus;	/* Ret. status from amFileGet().	*/
    double	*media;         /* Media the file is added to.		*/
    int		mediaAddStatus;	/* Ret. status from mediaListAdd.	*/
    EPTR	*mediaList;	/* List of media needed.		*/
    boolean	memoryError;	/* TRUE if a memory error occurs.	*/
    int		numNewDirs;	/* # of new dirs. added to the tree.	*/
    struct stat	stat_struct;    /* Return struct from stat().           */
    int		i;
    

    memoryError = FALSE;
    fileAddStatus = S_SUCCESS;
    mediaAddStatus = S_SUCCESS;
    
    
    CHECK_NULL( mediaList = eptr_alloc( 4 ) );
    for ( fileStatus = amFileGet( AM_FG_QUEUEABLE, &amFile, 
		    globals.gDbProc1, globals.gMedia.ammMediaRequestId );
	    fileStatus == AM_SUCCESS;
	    fileStatus = amFileGet( AM_FG_NEXT, &amFile,
		    globals.gDbProc1, globals.gMedia.ammMediaRequestId ) )
    {
	fileError = FALSE;

	/*
	 *  Ensure (with stat) file exists. Check filesize is correct. 
	 */

	(void) sprintf( fpath, "%s/%d.%s/%s", globals.gDataStage, 
		globals.gMedia.ammMediaRequestId, 			
		globals.gMedia.ammMediaRequestName, amFile.amfFilename );
	
	stat_struct.st_size = -1;
	if ( ! file( fpath, &stat_struct ) &&
		! file_link( fpath, &stat_struct ) )
	{
	    fileError = TRUE;
	    formatMessage( MSG_ERRNO, fpath );
	    printMessage( msg, MESS_ERROR );
	    amFile.amfStatus = AMF_UNAVAILABLE;
	    CHECK_AM( amFilePut( &amFile, globals.gDbProc2 ) );
	}
	else 
	{
	    if ( amFile.amfFileSize != stat_struct.st_size )
	    {
		fileError = TRUE;
		amFile.amfStatus = AMF_ERROR;
		CHECK_AM( amFilePut( &amFile, globals.gDbProc2 ) );
		formatMessage( E_FILESIZE, fpath );
		printMessage( msg, MESS_ERROR );
	    }
	    else if ( CONV_TO_BLOCKS( amFile.amfFileSize ) >
		    globals.gMaxUnitSize )
	    {
		formatMessage( E_FILE_TOBIG, globals.gMedia.ammMediaType,
			fpath );
		printMessage( msg, MESS_ERROR );
	    }

	    amFile.amfDateNotified = stat_struct.st_mtime;
	}
	

	if ( ! fileError )
	{
	    /*
	     * Add file to file list.
	     */
	    
	    amFilePtr = (AM_FILE*)gen_alloc( sizeof( AM_FILE ) );
	    filePtr = (FILE_INFO*)gen_alloc( sizeof( FILE_INFO ) );
	    if ( amFilePtr == NULL || filePtr == NULL )
	    {
		memoryError = TRUE;
	    }
	    else
	    {
		(void) memcpy( amFilePtr, &amFile, sizeof( AM_FILE ) );
		filePtr->fiAmFilePtr = amFilePtr;
		filePtr->fiParent = NULL;
		if ( ( eptr_insert( fileList, eptrlen( fileList ), 
		    filePtr ) ) == NULL )
		{
		    memoryError = TRUE;
		}
	    }


	    /*
	     * Add file to directory tree and media List.
	     */

	    numNewDirs = 0;
	    fileAddStatus = fileAdd( filePtr, "/", treeTop, &numNewDirs );
	}
    }


    /*
     * Sort the file list so bigger files appear first.  Tends to
     * improve the greedy bin packing algorithm used to decide how 
     * many media are needed.
     */

    CHECK_NULL( fileList = eptr_sort( fileList, revSizeCompare ) );
    for( i = eptrlen( fileList ) - 1; i >= 0; i -- )
    {
	CHECK_NULL( filePtr = eptritem( fileList, i ) );
	media = NULL;
	mediaAddStatus=mediaAdd( filePtr, numNewDirs, mediaList,  &media );
    }


    if ( mediaList != NULL )
    {
	globals.gNumMediaFound = eptrlen( mediaList );
	eptr_free( mediaList, free );
	mediaList = NULL;
    }
    
    if ( memoryError )
    {
	formatMessage( E_MEMORY );
	printMessage( msg, MESS_ERROR );
    }
    CHECK( fileAddStatus );
    CHECK( mediaAddStatus );
    
    if ( fileStatus != AM_NO_FILE )
    {
	CHECK_AM( fileStatus );
    }
    else if ( eptrlen( fileList ) == 0 )
    {
	formatMessage( S_NO_FILES, globals.gMedia.ammMediaRequestId );
	printMessage( msg, MESS_ERROR );
	return( S_NO_FILES );
    }

    CHECK( printTree( *treeTop ) );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	process
*
*   Purpose:
*	Process a media.
*
*   Values Returned:
*	int	S_SUCCESS	: Successful completion.
*	int	E_MEMORY	: Memory Error.
*	int	E_SELECT_ITEM	: Unable to select item.
*	int	E_TREE_TYPE	: Bad tree type given to select items.
*	int	E_AM		: Error in the CADC am library. 
*	int	E_MEMORY	: Memory Error.
*	int	E_UNITNAME_FMT	: Error formatting new media unit name.
*	int	S_NO_FILES	: Success, but no files ready to queue.
*	int	E_FILESIZE	: Actual file size doesn't match database.
*	int	E_FILE_TOBIG	: File is to big for media.
*
************************************************************************
-*/

int		process
(
    void
)
{
    int		buildStatus;	/* Ret. status from buildUnits().	*/
    EPTR 	*fileList;	/* Files chosen for one unit of media.	*/
    int		fetchStatus;	/* Ret. status from fetchMediaFiles().	*/
    TREE_INFO	*treeTop;	/* Top of directory tree.		*/
    int		treeInitStatus;	/* Ret. status from treeInit().		*/
    
    
    /*
     *  If recovery is necessary, recover. 
     */

    CHECK( checkRecovery( ) );


    /*
     *  Allocate eptr list for files. Populate the eptr list and dir. tree. 
     */

    CHECK_NULL( fileList = eptr_alloc( 1000 ) );
    CHECK_NULL( treeTop = (TREE_INFO*)gen_alloc( sizeof( TREE_INFO ) ) );
    treeInitStatus = treeInit( NULL, "/", treeTop );
    fetchStatus = fetchMediaFiles( fileList, treeTop );
    if ( fetchStatus != S_SUCCESS || treeInitStatus != S_SUCCESS )
    {
	treeFree( treeTop );
	eptr_free( fileList, fileInfoFree );
	fileList = NULL;
	if ( fetchStatus == S_NO_FILES )
	{
	    return( S_SUCCESS );
	}
    }
   CHECK( fetchStatus );
   CHECK( treeInitStatus );

   if ( globals.gNumMedia < 1 )
   {
       globals.gNumMedia = globals.gNumMediaFound;
   }
   
   

   /*
    * Create the media unit(s).
    */
    
   buildStatus = buildUnits( fileList, treeTop );


   /*
    *  Free the file eptr list and directory tree.
    */

   treeFree( treeTop );
   eptr_free( fileList, fileInfoFree );
   fileList = NULL;

   CHECK( buildStatus );

   return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   queueUnit
*
*   Purpose:
*	Queues a pre-built unit of media from the List. 
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*
************************************************************************
-*/

static	int	queueUnit
(
    EPTR	*fileList,      /* (mod) eptr list of chosen files.     */
    AM_UNIT	*amUnitPtr	/* (in)  Ptr to media unit structure.	*/
)
{
    FILE_INFO	*filePtr;	/* Ptr. to media file structure.	*/
    int		index;		/* Eptr index and loop counter.		*/
    

    /* 
     *  Create new media_units entry with AMU_QUEUEING status.
     */

    amUnitPtr->amuStatus = AMU_QUEUEING;
    CHECK_AM( amUnitPut( amUnitPtr, globals.gDbProc1 ) );

    
    /*
     * Loop through fileList, update media_unit_name field in media_files.
     */

    for ( index = ( eptrlen( fileList ) - 1 ); index > -1; index-- )
    {
        filePtr = (FILE_INFO *) eptritem( fileList, index );
	if ( filePtr->fiAmFilePtr->amfStatus != AMF_RETRIEVED || strne(
	    filePtr->fiAmFilePtr->amfMediaUnitName,
	    amUnitPtr->amuMediaUnitName  ) )
	{
	    continue;
	}
	CHECK_AM( amFilePut( filePtr->fiAmFilePtr, globals.gDbProc1 ) );
    }


    /*
     *  Update new media_units entry status to AMU_QUEUED.
     */

    amUnitPtr->amuStatus = AMU_QUEUED;
    CHECK_AM( amUnitPut( amUnitPtr, globals.gDbProc1 ) );


    /*
     *  Loop through fileList, remove entries just added to table. 
     */

    for ( index = ( eptrlen( fileList ) - 1 ); index > -1; index-- )
    {
	filePtr = (FILE_INFO *) eptritem( fileList, index );

	if ( strne( filePtr->fiAmFilePtr->amfMediaUnitName,
		amUnitPtr->amuMediaUnitName  ) ||
    		filePtr->fiAmFilePtr->amfStatus != AMF_RETRIEVED  )
	{
	    continue;
	}

	
	CHECK( pruneTree( filePtr, filePtr->fiParent ) );
	(void) eptr_del( fileList, index, 1, fileInfoFree );
    }

    CHECK_NULL( fileList = eptr_fit( fileList ) );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	selectDir
*
*   Purpose:
*	To mark a directory and all of its parents as selected.
*	Also 
*
*   Values Returned:
*	int	S_SUCCESS	: Successful completion.
*
************************************************************************
-*/

int	selectDir
(
    double	fileSize,	/* (in)  Size of the file selected.	*/
    int		level,		/* (in)	 Distance from the leaf (file.)	*/
    TREE_INFO	*treePtr,	/* (mod) Dir. tree info.		*/
    double	*unitSize,	/* (mod) Number of bytes selected.	*/
    AM_UNIT	*amUnitPtr	/* (mod) Media unit to be filled.	*/
)
{
    if ( treePtr == NULL )
    {
	return( S_SUCCESS );
    }
    
    if ( strne( treePtr->tiMediaUnitName, amUnitPtr->amuMediaUnitName ) )
    {
	(void) sprintf( treePtr->tiMediaUnitName, "%s",
		amUnitPtr->amuMediaUnitName );
	*unitSize += globals.gMediaType.amtDirOverh;   
    }

    if ( level > 1 )
    {
	treePtr->tiSubTreeSize -= fileSize;
    }
    

    CHECK( selectDir( fileSize, level+1, treePtr->tiParent, unitSize, amUnitPtr ) );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	selectItems
*
*   Purpose:
*	To select an item -- assign it to a particular media unit.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory Error.
*	int	E_SELECT_ITEM	: Unable to select item.
*	int	E_TREE_TYPE	: Bad tree type given.
*
************************************************************************
-*/

int	selectItems
(
    TREE_TYPE	type,		/* (in)  Type of item to be selected.	*/
    boolean	split,		/* (in)  TRUE if item can be split.	*/
    int		index,		/* (in)  Where the item is in the list.	*/
    TREE_INFO	*treePtr,	/* (in)  Tree that contains the item.	*/
    double	*unitSize,	/* (in)	 Size of the unit in blocks.	*/
    AM_UNIT	*amUnitPtr	/* (in)  Media unit item is assigned to.*/
)
{
    FILE_INFO	*filePtr;	/* File information structure.		*/
    double	fileSize;	/* Size of the file include all dir.	*/
    				/* needed to include it.		*/
    int		last;		/* Last item eptr list.			*/
    int		localIndex;	/* Local eptr list counter.		*/
    int		ret;		/* Function return value.		*/
    TREE_INFO	*subTreePtr;	/* Ptr. to a tree structure.		*/
    double     	subTreeSize;	/* Sum of all the sub tree  sizes.	*/
    int		status;		/* Function return status.		*/

    subTreeSize = treePtr->tiSubTreeSize + treePtr->tiSubTreeOverhead;

    switch ( type )
    {
	/*
	 * Select one or more subtrees
	 */
	
	case TT_TREE:
	    if ( treePtr->tiTreeList == NULL  ||
		    ( index == -1 && ! FIT_ON_UNIT( subTreeSize, *unitSize )
		    && ! split	) )
	    {
		return( S_SUCCESS );
	    }
	    else if ( index == -1 )
	    {
		last = ( eptrlen( treePtr->tiTreeList ) );
		for( localIndex =0; localIndex < last; localIndex ++ )
		{
		    CHECK( selectItems( type, split, localIndex, treePtr,
			    unitSize, amUnitPtr ) );
 		}
	    }
	    else if ( index >= 0 && index < eptrlen( treePtr->tiTreeList ) ) 
	    {
		CHECK_NULL( subTreePtr = eptritem( treePtr->tiTreeList,
			index ) );
		status = S_SUCCESS;
		if ( ( status =  selectTree( split, subTreePtr,	unitSize,
			amUnitPtr ) ) == S_FULL )
		{
		    return( S_FULL );
		}
		CHECK( status );
	    }
	    break; 
	    
	case TT_FILE:
	    /*
	     * Select one or more files.
	     */
	    
	    if ( treePtr->tiFileList == NULL  || ( index == -1 && ! split
		    && ! FIT_ON_UNIT( treePtr->tiFileSize, *unitSize ) ) ) 
	    {
		return( S_SUCCESS );
	    }
	    else if ( index == -1 )
	    {
		last = ( eptrlen( treePtr->tiFileList ) );
		for( localIndex = 0; localIndex < last; localIndex ++ )
		{
		    if ( ( ret = selectItems( type, FALSE, localIndex, treePtr,
			    unitSize, amUnitPtr ) ) == S_FULL )
		    {
			break;
		    }
		    else
		    {
			CHECK( ret );
		    }
		}
	    }
	    else if ( index >= 0 && index < eptrlen( treePtr->tiFileList ) ) 
	    {
		CHECK_NULL( filePtr = eptritem( treePtr->tiFileList, index ) );
		fileSize = FILE_SIZE(filePtr) +
		    findDirOverhead( filePtr, amUnitPtr );
		
		if ( streq( filePtr->fiAmFilePtr->amfMediaUnitName, "" ) &&
			FIT_ON_UNIT( fileSize, *unitSize ) )
		{
		    (void) sprintf( filePtr->fiAmFilePtr->amfMediaUnitName,
			    "%s", amUnitPtr->amuMediaUnitName );
		    *unitSize += fileSize;
		    treePtr->tiFileSize -= FILE_SIZE( filePtr );
		    CHECK( selectDir( FILE_SIZE( filePtr ), 1,
			    filePtr->fiParent, unitSize, amUnitPtr ) );
		}
		else if ( streq( filePtr->fiAmFilePtr->amfMediaUnitName, "" )
			&& ! globals.gPack && *unitSize > 0 )
		{
		    return ( S_FULL );
		}
	    }
	    break;
	    
	default:
	    formatMessage( E_TREE_TYPE, type );
	    printMessage( msg, MESS_ERROR );
	    return( E_TREE_TYPE );
    }
    
	    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	selectTree
*
*   Purpose:
*	Build a single unit of media selecting files from the directory
*	tree given.
*
*   Values Returnd:
*	int	S_SUCCESS	: Successful completion.
*	int	E_MEMORY	: Memory Error.
*	int	E_SELECT_ITEM	: Unable to select item.
*	int	E_TREE_TYPE	: Bad tree type given to select items.
*
************************************************************************
-*/

static	int	selectTree
(
    boolean	checkSplit,	/* (in)  TRUE if need to check for split*/
    TREE_INFO	*treePtr,	/* (mod) Dir. tree info.		*/
    double	*unitSize,	/* (mod) Number of bytes selected.	*/
    AM_UNIT	*amUnitPtr	/* (mod) Media unit to be filled.	*/
)
{
    int		index;		/* Index into eptr lists.		*/
    int		last;		/* Last item in an eptr list.		*/
    int		status;		/* Function return status.		*/

    /*
     * Make sure subtrees and files are sorted before starting, and
     * do some variable initialization.
     */

    CHECK( processTreeInit( treePtr ) );

    
    /*
     * Start selecting those files
     */

    if ( FIT_ON_UNIT( TREE_SIZE( treePtr ), *unitSize ) )
    {
	/*
	 * The entire tree fits so put it all on.
	 */
	CHECK( selectItems( TT_TREE, FALSE, -1, treePtr, unitSize, amUnitPtr ) );
	CHECK( selectItems( TT_FILE, FALSE, -1, treePtr, unitSize, amUnitPtr ) );
    }
    else if( checkSplit && checkForSplit( TREE_SIZE( treePtr ) ) )
    {
	/*
	 * We can't select the whole tree so try to split things up nicely
	 *    1. Try and put as many complete subdirectories on the
	 *	 media as possible.
	 *    2. Attempt to put all files on the media (if possible).
	 *    3. If files can be split up then try and put some of the
	 *	 files on the media.
	 *    4. Finally, attempt to add parts of the subdirectories
	 *	to the media, where splitting is allowed.
	 */
	    
	status = S_SUCCESS;
	if ( treePtr->tiTreeList != NULL && ! UNIT_FILLED( *unitSize ) )
	{
	    last = eptrlen( treePtr->tiTreeList );
	    for( index = 0; index < last && ! UNIT_FILLED( *unitSize )
		     && status == S_SUCCESS;  index++ )
	    {	
		status = selectItems( TT_TREE, FALSE, index, treePtr,
			unitSize, amUnitPtr );
		if ( status == S_FULL && !globals.gPack )
		{
		    break;
		}
	    }
	}
	CHECK( status );
	    
	if ( status != S_FULL && treePtr->tiFileList != NULL &&
		! UNIT_FILLED( *unitSize ) )
	{
	    CHECK( selectItems( TT_FILE, FALSE, -1, treePtr, unitSize,
		    amUnitPtr ) );
	}

	if ( treePtr->tiTreeList != NULL && ! UNIT_FILLED( *unitSize ) )
	{
	    last = eptrlen( treePtr->tiTreeList );
	    for( index = 0; index < last &&  ! UNIT_FILLED( *unitSize );
		 index++ )
	    {	
		status = selectItems( TT_TREE, TRUE, index, treePtr,
			unitSize, amUnitPtr );
		if ( status == S_FULL && !globals.gPack )
		{
		    break;
		}
	    }
	    CHECK( status );
	}

	if ( globals.gPack || *unitSize <= 0 ||
		treePtr->tiFileSize > globals.gMaxUnitSize )
	{
	    if ( treePtr->tiFileList != NULL && ! UNIT_FILLED( *unitSize )
		    && checkForSplit( treePtr->tiFileSize ) )
	    {
		CHECK( selectItems( TT_FILE, TRUE, -1, treePtr, unitSize,
			amUnitPtr ) );
	    }
	}
    }
    else if ( ! FIT_ON_UNIT( TREE_SIZE( treePtr ), *unitSize )  &&
	    ! globals.gPack && *unitSize > 0 )
    {
	return( S_FULL );
    }
    else
    {
	return( S_SUCCESS );
    }


    /*
     * Update Tree info.
     */

    CHECK( processTreeUpdate( treePtr, amUnitPtr ) );
    return( S_SUCCESS );
}
