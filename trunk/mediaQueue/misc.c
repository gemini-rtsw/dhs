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
*   Module Name:	mediaQueue/src/misc.c
*
*   Purpose:
*	Contains miscellaneous routines for mediaQueue.
*
*   Routines:
*	int		checkDbProc	: Checks if the dbproc has died.
*	int		checkSplit	: Determines if some item is splitable.
*	int		checkUnique	: Checks only one mediaQueue runs.
*	int		cleanup		: Cleans up before mediaQueue exits.
*	int		createUnit	: Initializes new unit structure.
*	int		deleteUnit	: Deletes a unit from the table.
*	int		findDirOverhead	: Finds the directory overhead for
*					  a given file.
*	void		fileInfoFree	: Frees file information from mem.
*	int		mediaAdd	: Add a file to the list of media.
*	void		printUnit	: Displays the contents of a media unit 
*
*   Date		: <Jan 18, 1998>
*
*   SCCS data		: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 12/07/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/02/27 SDJ	: Updated to new am library.
*	98/09/25 SDJ	: Added support for variable precision in the
*			  media unit names.
*	98/10/27 SDJ	: Added utility functions for new directory
*			  splitting algorithm.
*	98/11/19 SDJ	: Altered amUnitGet to handle varyied precision
*			  when finding the last one.
*	98/12/06 SDJ	: Altered due to changes in am, mediaTypes table
*			  stores media info instead of the config file.
*	98/12/15 SDJ	: Altered since regmatch_t different on HP.
*	99/01/24 SDJ	: Now use default precision if the media's precision
*			  is less than or equal to 0.
*	99/03/14 SDJ	: Added findDirOverhead().
*	99/03/29 SDJ	: checkForSplit() now compares blocks to blocks
*			  as it should.
*	99/04/29 NRH	: Added updating the action table.
*     2000/02/03 SDJ	: Fixed bug, would perform an amActionDelete even
*			  if no connection had been made to the database.
*     2000/02/07 SDJ	: Fixed bug in createUnit().  If the regcomp() failed
*			  mediaQueue seg faulted since it was printing
*			  a message with an uninitialized string.  The
*			  regcomp() was failing since the wrong regex.h was
*			  being used with the released version, so
*			  a switch was made to strncmp().
*    2000/03/17 SDJ	: Use globals.gSplitSize instead of doing
*			  calculations when needed. Used MACROS where they
*			  should be.
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
#include <math.h>

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
#include "gen_eptr.h"
#include "gen_str.h"
#include "db.h"
#include "am.h"


/*
 ************************************************************************
 *  Media Queue  include files.
 ************************************************************************
 */

#include "globals.h"

/*+
************************************************************************
*
*   Function:   checkDbproc
*
*   Purpose:
*       Check that a dbprocess is open and ready to use.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in am library.
*       int     E_DB_LIB	: Error in db library.
*
************************************************************************
-*/
 
int             checkDbProc
(
    DBPROCESS   **dbProc
)
{
    char        *database;	/* Am database name.			*/
    char        *dbServer;	/* Database server name for am library.	*/
 
 
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
 
    CHECK_DB_LIB( db_open( dbServer, NULL, NULL, database, FALSE, dbProc ) );
 
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	checkForSplit
*
*   Purpose:
*	Check if some item, whose size is given, can be split.
*
*   Values Returned:
*       boolean : TRUE if it can be split
*       boolane : FALSE if it can not be split
*
************************************************************************
-*/

boolean	checkForSplit
(
    double	size		/* (in)  Size to check in blocks.	*/
)
{
    if ( size <= globals.gSplitSize )
    {
	return( FALSE );
    }

    return( TRUE );
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
*	Cleans up before mediaQueue exits.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		cleanup
(
    void
)
{
    /*
     * Remove the action from the action table.
     */

    if ( globals.gDbProc1 != NULL )
    {
	CHECK_AM( amActionDelete( globals.gDbProc1, globals.gMediaId, NULL ) );
    }
    

 
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
    db_exit(); 
    
    
    
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
*   Function:   createUnit
*
*   Purpose:
*	Creates a new media unit structure, and assigns it a name. 
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*	int	E_UNITNAME_FMT	: Error formatting the unit name.
*
************************************************************************
-*/

int             createUnit
(
    AM_UNIT	*unitPtr	/* (mod) Ptr. to media unit structure.	*/
)
{
    AM_UNIT	existingUnit;	/* Unit returned from amUnitGet().	*/
    int         intResult;      /* regexp result converted to an int.   */
    int		num;		/* Number of the Unit.			*/
    char	newUnitName[AM_MEDIA_UNIT_NAME_LEN];
				/* For creating the name.		*/
    char	patt[AM_MEDIA_UNIT_NAME_LEN];
				/* For sscanf pattern, name composing.	*/
    int		precision;	/* Unit name precision.			*/
    int		unitStatus;	/* Return status from amUnitGet().	*/


    /*
     * Use default precision, if precision value is less than 0.
     */

    if ( globals.gMedia.ammUnitNamePrecision >  0 )
    {
	precision = globals.gMedia.ammUnitNamePrecision;
    }
    else
    {
	CHECK_AM( amPrecisionGet( &precision ) );
    }


    /*
     *  Get the *last* unit in the database.
     */

    unitStatus = amUnitGet( AM_UG_MEDIA_REQUEST_ID_LAST, &existingUnit,
	    globals.gDbProc1, globals.gMedia.ammMediaRequestId,
	    globals.gMedia.ammUnitNameRoot );
    if ( unitStatus == AM_NO_UNITS )
    {
	/*
	 *  No pattern match - must be the first one. 
	 */

	num = 1;


	/*
	 * Create the name of the new media unit.
	 */
	
	(void) sprintf( newUnitName, "%s%.*d", globals.gMedia.ammUnitNameRoot,
		precision, num );
    }
    else 
    {
	CHECK_AM( unitStatus );


        /* 
         *  Make scanf pattern, decode & verify decoding of unit name.
         */
 
        (void) sprintf( patt, "%s%%d", globals.gMedia.ammUnitNameRoot );
	(void) sscanf( existingUnit.amuMediaUnitName, patt, &num );


	/*
	 * Make sure the old media unit matches the expected value.
	 * This probably is not necessary.
	 */
	
	if ( ( strncmp( globals.gMedia.ammUnitNameRoot,
		existingUnit.amuMediaUnitName,
		strlen( globals.gMedia.ammUnitNameRoot ) ) ) != 0 )
	{
	    formatMessage( S_UNITNAME_FMT, existingUnit.amuMediaUnitName,
			    globals.gMedia.ammMediaRequestId );
	    printMessage( msg, MESS_CON );
	}
	else if ( !str2int( existingUnit.amuMediaUnitName +
		strlen( globals.gMedia.ammUnitNameRoot ), &intResult )
		|| intResult != num )
	{
	    formatMessage( S_UNITNAME_FMT, existingUnit.amuMediaUnitName,
			    globals.gMedia.ammMediaRequestId );
	    printMessage( msg, MESS_CON );
	}	    
	

	/*
	 * Create the name of the new media unit.
	 */
	
	(void) sprintf( newUnitName, "%s%.*d", globals.gMedia.ammUnitNameRoot,
		precision, ++num );
    }


    /*
     *  format the unit name, making sure the number does not exceed the
     * 	allowed precision.
     */

    if ( num / (int)pow( 10, precision ) > 0 )
    {
	formatMessage( S_UNITNAME, globals.gMedia.ammMediaRequestId );
	printMessage( msg, MESS_OPER );
	return( S_UNITNAME );
    }
    

    /*
     *  Assign the media unit struture.
     */

    (void) sprintf( unitPtr->amuMediaUnitName, "%s", newUnitName ); 

    unitPtr->amuCopiesWritten	= 0;
    unitPtr->amuDateCompleted	= 0;
    unitPtr->amuMediaRequestId 	= globals.gMedia.ammMediaRequestId;
    unitPtr->amuStatus 		= AMU_QUEUEING;
    unitPtr->amuTotalSize 	= 0;
    unitPtr->amuUncompSize 	= 0;
 

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   deleteUnit
*
*   Purpose:
*	Deletes a unit of media from the media_units table. 
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the CADC am library.
*	int	E_DB		: Error in the sybase db library.
*	int	E_DB_LIB	: Error in the CADC db library.
*
************************************************************************
-*/
 
int             deleteUnit
(
    AM_UNIT     *unitPtr,	/* (in)  Ptr. to media unit structure.	*/
    DBPROCESS   *dbProc		/* The dbprocess to use.		*/
)
{
    char        *database;	/* Name of the database.		*/
    int         dbstatus;	/* Ret. status from the db.		*/
    char        *unitsTable;	/* Name of the media units table.	*/
 
 
    CHECK_AM( amDbInfoGet( NULL, &database, NULL, &unitsTable, NULL, NULL ) );
 
    CHECK_DB_LIB( db_simple_query( dbProc,
            "delete \n"
            "from \n"
            "    %s..%s \n"
            "where \n"
            "    media_unit_name = '%s' \n"
            "    and media_request_id = %d \n",
            database, unitsTable,
            unitPtr->amuMediaUnitName, unitPtr->amuMediaRequestId ) );
 
    while ( ( dbstatus = dbresults( dbProc ) ) == SUCCEED )
    {
	/*
	 *  The body of this while statement is intentionally empty.
	 */
    }
    CHECK_DB( dbstatus );
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	findDirOverhead
*
*   Purpose:
*	Finds the directory overhead required to include a file on the
*	unit.  
*
*   Values Returned:
*	int	0: 	If the file has no parent directories not already
*		   	chosen
*	int   	> 0:	If there are parent directories not already chosen.
*	int	< 0:	If an error occurs.
*
************************************************************************
-*/

int	findDirOverhead
(
    FILE_INFO	*filePtr,	/* (in) File information structure.	*/
    AM_UNIT	*amUnitPtr	/* (in) Media unit to be filled.	*/
)
{
    int		overhead;	/* Overhead required.			*/
    TREE_INFO	*treePtr;	/* Directory tree structure.		*/

    if ( filePtr == NULL || amUnitPtr == NULL )
    {
	return( -1 );
    }


    overhead = 0;
    treePtr = filePtr->fiParent;
    while( treePtr != NULL && strne( treePtr->tiMediaUnitName,
	    amUnitPtr->amuMediaUnitName ) )
    {
	overhead += globals.gMediaType.amtDirOverh;
	treePtr = treePtr->tiParent;
    }
    

    return( overhead );
}

/*+
************************************************************************
*
*   Function:	fileInfoFree
*
*   Purpose:
*	Free a file information structure from memory.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	fileInfoFree
(
    FILE_INFO	*filePtr	/* (mod) File info structure to free.	*/
)
{
    if ( filePtr->fiAmFilePtr != NULL )
    {
	gen_free( filePtr->fiAmFilePtr );
	filePtr->fiAmFilePtr = NULL;
    }

    gen_free( filePtr );
    filePtr = NULL;
}

/*+
************************************************************************
*
*   Function:	mediaAdd
*
*   Purpose:
*	To add a file to the list of media.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory error.
*
************************************************************************
-*/

int	mediaAdd
(
    FILE_INFO	*filePtr, 	/* (in)  Ptr. to media file structure.	*/
    int		numDirs,	/* (in)  Number of directories needed.	*/
    EPTR	*mediaList,	/* (mod) List of needed.		*/
    double	**media		/* (out) Media the file is added to.	*/
)
{
    double 	fileSize;	/* File size in blocks.			*/
    boolean	found;		/* TRUE if a media has been found.	*/
    int		index;		/* Index into eptr list. 		*/
    int		last;		/* Last item in eptr list.		*/

    
    /*
     *  Add the file onto one of the media.
     */
	
    last = eptrlen( mediaList );
    found = FALSE;
    fileSize = CONV_TO_BLOCKS( filePtr->fiAmFilePtr ->amfFileSize )
	+ globals.gMediaType.amtFileOverh;
    
    for ( index=0; index < last && ! found; index++ )
    {
	CHECK_NULL( *media = eptritem( mediaList, index ) );
	if ( ! UNIT_FULL( (**media) + fileSize ) )
	{
	    /*
	     * Found a media that this file will fit on.
	     */


	    **media += fileSize;
	    found = TRUE;
	}
    }
	
       
    if ( ! found )
    {
	/*
	 * No room on any of the existing media so create a new one.
	 */
	    
	CHECK_NULL( *media = gen_alloc( sizeof( double ) ) );
	**media = fileSize + globals.gMediaType.amtDirOverh;
	CHECK_NULL( mediaList = eptr_insert( mediaList, last, *media ) );
    }
    CHECK_NULL( media );

    
    /*
     * Add the directory overhead for this file to the media.
     */
		
    **media += numDirs * globals.gMediaType.amtDirOverh;


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	printUnit  (NOT USED: GOOD FOR DEBUGGING)
*
*   Purpose:
*	Prints out the directory tree in a fashion similar to "ls -FC .*".
*	Only prints the parts of the tree which have been assigned to the
*	given mediaUnit
*
*	Recursively calls itself to print the sub tress.
*
*   Values Returned:
*       int     S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory error.
*
************************************************************************
-*/

int	printUnit
(
    TREE_INFO	treeTop,	/* (in)  Top of the directory tree.	*/
    AM_UNIT	amUnit		/* (in)	 Am media unit structure.	*/
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
	formatMessage( S_PT_UNIT_START, amUnit.amuMediaUnitName );
	printMessage( msg, MESS_CON_VERB );
    }

    	
    if ( streq( treeTop.tiMediaUnitName, amUnit.amuMediaUnitName ) )
    {
	/*
	 * Print the tree's generic path.
	 */

	if ( treeTop.tiIsSplit )
	{
	    formatMessage( S_TREE_UNIT_INFO, treeTop.tiGenPath,
		    treeTop.tiOldestDate, "TRUE" );
	    printMessage( msg, MESS_CON_VERB );
	}
	else
	{
	    formatMessage( S_TREE_UNIT_INFO, treeTop.tiGenPath,
		    treeTop.tiOldestDate, "FALSE" );
	    printMessage( msg, MESS_CON_VERB );
	}
    }
    

    
    /*
     * Print out the list of files contained in this tree.
     */
    
    if ( treeTop.tiFileList != NULL )
    {
	
	last = eptrlen( treeTop.tiFileList );
	for( index=0; index < last; index++ )
	{
	    CHECK_NULL( filePtr = eptritem( treeTop.tiFileList, index ) );
	    if ( streq( amUnit.amuMediaUnitName,
		    filePtr->fiAmFilePtr->amfMediaUnitName ) )
	    {

		formatMessage( S_FILE_INFO, filePtr->fiAmFilePtr->amfFilename,
			CONV_TO_BLOCKS( filePtr->fiAmFilePtr->amfFileSize ),
			filePtr->fiAmFilePtr->amfDateNotified );
		printMessage( msg, MESS_CON_VERB );
	    }
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
	    CHECK( printUnit( *subTree, amUnit ) );
	}
    }


    if ( streq( treeTop.tiTreeName, "/" ) )
    {
	formatMessage( S_PT_UNIT_END ); 
	printMessage( msg, MESS_CON_VERB );
    }

    return( S_SUCCESS );
}
