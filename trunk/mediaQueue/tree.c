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
*   Module Name:	mediaQueue/src/tree.c
*
*   Purpose:
*	Contains routines for building a dir. tree structure for a
*	list of filenames.
*
*   Routines:
*	int		fileAdd		: Adds an amFile to a dir. tree.
*	static int	fileListAdd	: Add a amFile to an EPTR file list
*	int		processTreeInit	: Init. a tree for processing.
*	int		processTreeUpdate
*					: Update a tree after processing.	
*	int		pruneTree	: Prunes any unecessary leaves
*					  from the tree.
*	int		treeFree	: Frees a tree struct. from memory.
*	int		treeInit	: Initialize a directory structure.
*	static int	treeListAdd	: Add a tree to an EPTR tree list.
*	static void	updateParent	: update parent values.
*
*   Date		: Sep 29, 1998
*
*   SCCS data		: @(#)
*	Module Name	: tree.c
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/03/14 SDJ	: Added tiSubTreeSize and tiSubTreeOverhead to
*			  tree structure and removed tiTreeSize. Added
*			  functions processTreeInit() and processTreeUpdate().
*	99/03/29 SDJ	: Fixed bug in updateParent().  No longer adds filesize
*			  to the subtree size of the parent tree.
*	99/03/31 SDJ	: Added FILE_SIZE macro. Changed a while loop
*			  to a for loop in processTreeUpdate.
*	99/12/22 SDJ	: Fixed bug, fileadd() seg faulted when filenames
*			  started with a '/'.
*     2000/03/17 SDJ	: Use the new amfDayNotified to determine the file
*			  date when size and data are checked. The old
*			  method did not gaurantee a unique sort when
*			  sorting dates.  Made better use of macros.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

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
#include "gen_eptr.h"
#include "gen_msg.h"
#include "db.h"
#include "am.h"


/*
 ************************************************************************
 *  Media Queue  include files.
 ************************************************************************
 */

#include "globals.h"

static int	treeListAdd( TREE_INFO *, TREE_INFO * );
static int	fileListAdd( TREE_INFO *, FILE_INFO * );
static void	updateParentTree( FILE_INFO *, int, TREE_INFO * );

/*+
************************************************************************
*
*   Function:   fileAdd
*
*   Purpose:
*	Adds a file to a directory tree.  Creating any necessary sub-
*	directories and adds the file to the appropriate directory.
*	This routine is called recursively to find the bottom of the
*	tree.
*
*	It assumes the the top of the tree is "/" and that the filenames
*	are generic paths without the first "/", such as "dir1/myFile".
*
*   Values Returned:
*	int	S_SUCCESS	: Function completed normally.
*	int	E_MEMORY	: Error - out of memory.
*
************************************************************************
-*/

int		fileAdd
( 
    FILE_INFO	*filePtr, 	/* (in)  Ptr. to media file structure.	*/
    char	parentPath[PATH_MAX],
    				/* (in)  Directory path to check.	*/
    TREE_INFO	*parentPtr,	/* (mod) Directory tree Eptr list.	*/
    int		*numNewTrees	/* (mod) Number of new trees added.	*/
)
{
    int		adjustment;	/* Number of '/' added to parent path.	*/
    char	childPath[PATH_MAX];
				/* Generic path to child tree.		*/
    TREE_INFO	*childPtr;	/* Child directory information.		*/
    char	*childName;	/* Name of the child tree.		*/
    int		end;		/* end of the parent path.		*/
    char	*front;		/* Front of the tmp string.		*/
    int		index;		/* Index into eptr list. 		*/
    boolean	isNew;		/* TRUE if child directory tree is new.	*/
    char	*tmpName1;	/* Temporary directory tree name.	*/
    char	*tmpName2;	/* Temporary directory tree name.	*/
    char 	tmpParent[PATH_MAX];
    				/* Parent path used internally.		*/
        

    /*
     * Find the directory tree name to be checked
     */

    childPtr = NULL;
    CHECK_NULL( tmpName1 = strsav( filePtr->fiAmFilePtr->amfFilename ) );
    front = tmpName1;
    adjustment = 0;
    tmpParent[0] = '\0';
    while ( tmpName1[0] == '/' && tmpName1[0] != '\0' )
    {
	tmpName1++;
	adjustment++;
	end = strlen( tmpParent );
	if ( end < PATH_MAX -1 )
	{
	    tmpParent[ end ] = '/';
	    tmpParent[ end + 1 ] = '\0';
	}
    }
	
    if ( streq( parentPath, "/" ) )
    {
	childName = tmpName1;
    }
    else
    {
	childName = tmpName1 + strlen( parentPath );
    }
    tmpName2 = childName;
    if( ( tmpName2 = strstr( tmpName2, "/" ) ) != NULL )
    {
	tmpName2[0] = '\0';
    }
    (void) sprintf( childPath, "%s/%s", tmpParent, tmpName1 );

    
    /*
     * Check to see if we need to add a file to the tree or
     * a subtree ( the file will be added somewhere
     * below the subtree.
     */

    isNew = FALSE;
    if ( streq( childPath+1, filePtr->fiAmFilePtr->amfFilename ) )
    {
	/*
	 * The child is a file so add it to the file list.
	 */
	
	CHECK( fileListAdd( parentPtr, filePtr ) );
    }
    else
    {
	/*
	 * The child is a subtree (directory).
	 */
	
	if ( parentPtr->tiTreeList != NULL &&
	    eptr_search( parentPtr->tiTreeList, childName, &index,
		treeNameCompare ) )
	{
	    /*
	     * Already in list.
	     */

	    CHECK_NULL( childPtr =  eptritem( parentPtr->tiTreeList, index ) );
	}
	else
	{
	    /*
	     * New directory tree.
	     */
	    
	    isNew = TRUE;
	    CHECK_NULL( childPtr = gen_alloc( sizeof( TREE_INFO ) ) );
	    CHECK( treeInit( parentPtr, childName, childPtr ) );
	    CHECK( treeListAdd( parentPtr, childPtr ) );
	}
	CHECK( fileAdd( filePtr, childPath+adjustment, childPtr, numNewTrees ) );
    }
    

    /*
     * Update stats on the way out of the recursion.
     */

    if ( isNew ) {
	(*numNewTrees) ++;
    }

    updateParentTree( filePtr, *numNewTrees, parentPtr );
    if ( childPtr != NULL && UNIT_FULL( TREE_SIZE( childPtr ) ) )
    {
	parentPtr->tiSmallSubTrees = FALSE;
    }
    

    if ( front != NULL )
    {
	gen_free( front );
    }
    
    return( S_SUCCESS ); 
}

/*+
************************************************************************
*
*   Function:	fileListAdd
*
*   Purpose:
*	Add a new file to an EPTR list of am file structures.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory error.
*
*
************************************************************************
-*/

int	fileListAdd
(
    TREE_INFO 	*parent,	/* (in)  Ptr. to parent tree info.	*/
    FILE_INFO	*filePtr	/* (in)  Ptr. to media file struct.	*/
)
{
    /*
     * Add the new file structure to the parent's list of directory trees.
     */

    if ( parent->tiFileList == NULL )
    {
	CHECK_NULL( parent->tiFileList = eptr_alloc( 10 ) );
    }

    CHECK_NULL( parent->tiFileList = eptr_insert( parent->tiFileList,
		eptrlen(parent->tiFileList), filePtr ) );    
    filePtr->fiParent = parent;
    parent->tiFileSize += FILE_SIZE( filePtr );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	printTree  (NOT USED: GOOD FOR DEBUGGING)
*
*   Purpose:
*	Print out the directory tree in a fashion similar to "ls -FC .*".
*
*	Recursively calls itself to print the sub tress.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory error.
*
************************************************************************
-*/

int	printTree
(
    TREE_INFO	treeTop		/* (in)  Top of the directory tree.	*/
)
{	
    FILE_INFO	*filePtr;	/* Ptr. to media file struct.	        */
    int		index;		/* Index into eptr list. 		*/
    int		last;		/* last item in the eptr list.		*/
    TREE_INFO	*subTree;	/* sub tree (directory).		*/


    if ( ! globals.gVerbose )
    {
	return( S_SUCCESS );
    }


    if ( streq( treeTop.tiTreeName, "/" ) )
    {
	formatMessage( S_PT_TREE_START );
	printMessage( msg, MESS_CON_VERB );
    }
    
    
    /*
     * Print the tree's generic path.
     */

    formatMessage( S_TREE_INFO, treeTop.tiGenPath,
	    treeTop.tiSubTreeSize + treeTop.tiSubTreeOverhead +
	    treeTop.tiFileSize + globals.gMediaType.amtDirOverh,
	    treeTop.tiFileSize, treeTop.tiOldestDate );
    printMessage( msg, MESS_CON_VERB );
    

    /*
     * Print out the list of files contained in this tree.
     */
    
    if ( treeTop.tiFileList != NULL )
    {
	last = eptrlen( treeTop.tiFileList );
	for( index=0; index < last; index++ )
	{
	    CHECK_NULL( filePtr = eptritem( treeTop.tiFileList, index ) );

	    formatMessage( S_FILE_INFO, filePtr->fiAmFilePtr->amfFilename,
		    CONV_TO_BLOCKS( filePtr->fiAmFilePtr->amfFileSize ),
		    filePtr->fiAmFilePtr->amfDateNotified );
	    printMessage( msg, MESS_CON_VERB );
	}
    }

    /*
     * Display the sub trees (directories).
     */
    
    if ( treeTop.tiTreeList != NULL )
    {
	last = eptrlen( treeTop.tiTreeList );
	for( index=0; index < last; index++ )
	{
	    CHECK_NULL( subTree = eptritem( treeTop.tiTreeList, index ) );
	    CHECK( printTree( *subTree ) );
	}
    }


    if ( streq( treeTop.tiTreeName, "/" ) )
    {
	formatMessage( S_PT_TREE_END );
	printMessage( msg, MESS_CON_VERB );
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	processTreeInit
*
*   Purpose:
*	Initialize a directory tree structure before it is processed.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Error - out of memory.
*	
************************************************************************
-*/

int	processTreeInit
(
    TREE_INFO	*treePtr	/* (mod) Dir. tree info.		*/    
)
{
    if ( ! treePtr->tiSorted )
    {
	/*
	 * Sub trees are not sorted, so sort them.
	 */

	if ( treePtr->tiTreeList != NULL )
	{
	    CHECK_NULL( treePtr->tiTreeList = eptr_sort(treePtr->tiTreeList,
		    treeCompare ) );
	}

	if ( treePtr->tiFileList != NULL )
	{
	    CHECK_NULL( treePtr->tiFileList = eptr_sort(treePtr->tiFileList,
		    fileCompare ) );
	}
	    
	treePtr->tiSorted = TRUE;
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	processTreeUpdate
*
*   Purpose:
*	Update tree information after it has been processed
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*
v************************************************************************
-*/

int	processTreeUpdate
(
    TREE_INFO	*treePtr,	/* (mod) Dir. tree info.		*/
    AM_UNIT	*amUnitPtr	/* (in) Media unit to be filled.	*/
)
{
    TREE_INFO	*currentTree;	/* Current Directory tree info.		*/
    TREE_INFO	*parentTree;	/* Parent Directory tree info.		*/

    if ( treePtr->tiSubTreeSize > 0 || treePtr->tiFileSize > 0 )
    {
	/*
	 *  Only part of the tree has been selected.
	 */

	treePtr->tiIsSplit = TRUE;
    }
    else if ( streq( treePtr->tiMediaUnitName, amUnitPtr->amuMediaUnitName ) )
    {
	/*
	 * Update parent tree's dir overhead
	 */

	for( currentTree = treePtr; currentTree->tiParent != NULL;
	     currentTree = parentTree )
	{
	    parentTree = currentTree->tiParent;
	    if ( currentTree->tiSubTreeSize <= 0 &&
		    currentTree->tiFileSize <= 0 &&
		    parentTree != NULL )
	    {
		parentTree->tiSubTreeOverhead -= globals.gMediaType.amtDirOverh;
	    }
	}
    }
    

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	pruneTree
*
*   Purpose:
*	To remove any unecessary leaves from a tree.
*
*	Recursively called to prune from the bottom of the tree upwards.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_NO_PRUNE	: Unable to remove filePtr from tree.
*
************************************************************************
-*/

int	pruneTree
(
    FILE_INFO	*filePtr,	/* (in) File structure at the leaf. 	*/
    TREE_INFO	*treePtr	/* (in) Tree to be pruned.		*/
)    
{	
    int		index;		/* Index into eptr list. 		*/
    TREE_INFO	*parent;	/* Parent of the current leaf.		*/

    if ( filePtr != NULL )
    {
	/*
	 * Remove the file from the tree's list of files.
	 */

	
	index = -1;
	if ( treePtr->tiFileList == NULL ||
		! eptr_search( treePtr->tiFileList, filePtr, &index,
		fileCompare ) )
	{
	    /*
	     * Unable to find file so report error.
	     */
	    
	    formatMessage( E_NO_PRUNE, filePtr->fiAmFilePtr->amfFilename );
	    printMessage( msg, MESS_ERROR );
	    return( E_NO_PRUNE );
	}

	
	if ( index != -1 )
	{
	    eptr_del( treePtr->tiFileList, index, 1, NULL );
	    if ( eptrlen( treePtr->tiFileList ) == 0 )
	    {
		(void) eptr_free( treePtr->tiFileList, NULL );
		treePtr->tiFileList = NULL;
	    }
	}
    }

    

    if ( ( treePtr->tiFileList == NULL ||
	    eptrlen( treePtr->tiFileList ) == 0 ) &&
	    ( treePtr->tiTreeList == NULL ||
		    eptrlen( treePtr->tiTreeList ) == 0 ) )
    {
	parent = treePtr->tiParent;
	if ( parent != NULL  &&  eptr_search( parent->tiTreeList,
		treePtr, &index, treeCompare ) )
	{
	    /*
	     * There is nothing left in this directory so remove it from
	     * it's parent and try and prune the parent.
	     */

	    (void) eptr_del( parent->tiTreeList, index, 1, treeFree );
	    CHECK( pruneTree( NULL, parent ) );
	}
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	treeDelete
*
*   Purpose:
*	Delete a tree structure from memory.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

void	treeFree
(
    TREE_INFO	*treePtr	/* (in)  Tree structure to be freed.	*/
)
{
    if ( treePtr == NULL  )
    {
	return;
    }
    
		
    if ( treePtr->tiTreeName != NULL )
    {
	gen_free( treePtr->tiTreeName );
	treePtr->tiTreeName = NULL;
    }

    if ( treePtr->tiGenPath != NULL )
    {
	gen_free( treePtr->tiGenPath );
	treePtr->tiGenPath = NULL;
    }

    if ( treePtr->tiFileList != NULL )
    {
	eptr_free( treePtr->tiFileList, NULL );
	treePtr->tiFileList = NULL;
    }


    if ( treePtr->tiTreeList != NULL )
    {
	eptr_free( treePtr->tiTreeList, treeFree );
	treePtr->tiTreeList = NULL;
    }
    
    
    treePtr->tiParent = NULL;
    
    gen_free( treePtr );
    treePtr = NULL;
}

/*+
************************************************************************
*
*   Function:	treeInit
*
*   Purpose:
*	Initializes a directory tree structure.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory error.
*
************************************************************************
-*/

int	treeInit
(
    TREE_INFO	*parentPtr,	/* (in)  Parent tree pointer.		*/
    char	*treeName,	/* (in)  Name of the new tree.		*/
    TREE_INFO	*newTreePtr	/* (mod) Dir. tree to be initialized.	*/
)
{
    CHECK_NULL( newTreePtr->tiTreeName = strsav( treeName ) );

    if ( parentPtr == NULL )
    {
	CHECK_NULL( newTreePtr->tiGenPath = strsav( treeName ) );
    }
    else
    {
	CHECK_NULL( newTreePtr->tiGenPath = strsav( parentPtr->tiGenPath ) );
	if ( strne( parentPtr->tiGenPath, "/" ) )
	{
	    CHECK_NULL( newTreePtr->tiGenPath = strnapp( newTreePtr->tiGenPath,
		    "/", 1 ) );
	}
	CHECK_NULL( newTreePtr->tiGenPath = strnapp( newTreePtr->tiGenPath,
		treeName, strlen( treeName ) ) );
    }


    newTreePtr->tiFileList = NULL;
    newTreePtr->tiTreeList = NULL;
    newTreePtr->tiParent = parentPtr;
    newTreePtr->tiMediaUnitName[0] = '\0';
    newTreePtr->tiSubTreeSize = 0;
    newTreePtr->tiSubTreeOverhead = 0;
    newTreePtr->tiFileSize = 0;
    newTreePtr->tiOldestDate = time( &(newTreePtr->tiOldestDate) );
    newTreePtr->tiIsSplit = FALSE;
    newTreePtr->tiSmallSubTrees = TRUE;
    newTreePtr->tiSorted = FALSE;
    
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	treeListAdd
*
*   Purpose:
*	Add a new directory tree to an EPTR list of directory trees.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory error.
*
*
************************************************************************
-*/

int	treeListAdd
(
    TREE_INFO 	*parent,
    TREE_INFO 	*newTreePtr
)
{
    int		index;		/* Index into eptr list. 		*/


    /*
     * Add the new directory tree to the parent's list of directory trees.
     */
    
    if ( parent->tiTreeList == NULL )
    {
	CHECK_NULL( parent->tiTreeList = eptr_alloc( 10 ) );
    }

    if ( ! eptr_search( parent->tiTreeList, newTreePtr->tiTreeName, &index,
	    treeNameCompare ) )
    {
	CHECK_NULL( parent->tiTreeList = eptr_insert( parent->tiTreeList,
		index, newTreePtr ) );
    }
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	updateParentTree
*
*   Purpose:
*       Have just added to a file to a tree (either directly or to one
*	of its siblings), now need to update the stats for the parent
*	tree - ie. increment number of files, subtrees, sizes, etc.
*
*   Values Returned:
*       None.
*	
*
************************************************************************
-*/

void 	updateParentTree
(
    FILE_INFO	*filePtr,	/* (in)  Am file structure added.	*/
    int		numNewTrees,	/* (in)	 Number of new directory trees.	*/
    TREE_INFO	*treePtr	/* (mod) Parent tree structure.		*/
)
{
    /*
     * Update the date, it will be the date of the oldest file below
     * this directory tree.
     */
    
    if ( globals.gCheckSize && globals.gCheckDate ) 
    {
	/*
	 * If we are checking the size then we should consider
	 * the files written on the "same day" as having the
	 *  same time.
	 */

	if ( treePtr->tiOldestDate > filePtr->fiAmFilePtr->amfDayNotified )
	{
	    treePtr->tiOldestDate = filePtr->fiAmFilePtr->amfDayNotified;
	}
    }
    else
    {
	if ( treePtr->tiOldestDate > filePtr->fiAmFilePtr->amfDateNotified )
	{
	    treePtr->tiOldestDate = filePtr->fiAmFilePtr->amfDateNotified;
	}
    }	

    
    /*
     * Return if this is the files parent.
     */

    if ( filePtr->fiParent == treePtr )
    {
	return;
    }
    
    
    /*
     * Add size of of the file and its overhead, as well as the overhead
     * for all new trees created.
     */

    treePtr->tiSubTreeSize += FILE_SIZE( filePtr );
    treePtr->tiSubTreeOverhead += numNewTrees * globals.gMediaType.amtDirOverh;
}
