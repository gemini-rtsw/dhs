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
*   Module Name:	mediaQueue/src/rollback.c
*
*   Purpose:
*	This file contains routines to undo a queuing operation.
*
*   Routines:
*	int	routine		: Rollback a queue operation.
*
*   Date		: Mar 1, 1999
*
*   SCCS data		: @(#)
*	Module Name	: rollback.c
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 02/03/00
*
*   Programmer		: Norm Hill
*
*   Modification History:
*	99/03/24 SDJ	: Rollback now works if "num_units" is not
*			  specified.
*	99/04/26 SDJ	: Removed memory leaks in eptr lists.
*     2000/02/03 SDJ	: Fixed bug in rollback().  It didn't check
*			  the AM lib. return statusm if amUnitGet
*			  returned somthing other than AM_NO_UNITS.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/types.h>
#include <sys/stat.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_msg.h"
#include "am.h"

#include "globals.h"


/*+
************************************************************************
*
*   Function:	rollback
*
*   Purpose:
*	Rollback previously queued media units.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the AM library.
*	int	E_MEMORY	: Memory allocation failure.
*	int	E_NO_UNITS	: There are no units to rollback.
*
************************************************************************
-*/

int		rollback
(
    void
)
{
    AM_FILE	amFile;		/* A returned by the am library.	*/
    int		amStatus;	/* Status from am library.		*/
    AM_UNIT	amUnit;		/* Unit read from am library.		*/
    EPTR	*fileList;	/* Files in the unit to be deleted.	*/
    int		i;
    int		index;		/* Place to put this unit in unitList	*/
    int		j;
    AM_UNIT	*lastUnit;	/* The lowest numbered unit curretly in	*/
    				/* the list of units to be removed.	*/
    AM_FILE	*pAmFile;	/* The current file.			*/
    AM_UNIT	*pUnit;		/* The current unit.			*/
    int		status;		/* Function return status.		*/
    EPTR	*unitList;	/* An eptr list of units to be deleted.	*/


    /*
     * Initialize the eptr lists.
     */

    CHECK_NULL( unitList = eptr_alloc( globals.gNumMedia + 1 ) );
    CHECK_NULL( fileList = eptr_alloc( 5000 ) );
    status = S_SUCCESS;


    /*
     * Get a list of units to be deleted. These are the highest numbered
     * units, up to the number of units specified on the command line.
     * The amUnitGet call gets all queued units.
     */
    
    lastUnit = NULL;
    for ( ( amStatus = amUnitGet( AM_UG_STATUS_AFTER_NAME, &amUnit, 
	    globals.gDbProc1, globals.gMediaId, AMU_QUEUED, "", "" ) ); 
	    amStatus == AM_SUCCESS && status == S_SUCCESS; 
	    amStatus = amUnitGet( AM_UG_NEXT, &amUnit, globals.gDbProc1 ) )
    {
	/*
	 * Insert the unit into the list if the list doesn't contain 
	 * the number of units specified on the command line, or if this
	 * unit is numbered higher than the lowest numbered unit in the list.
	 */

	if ( eptrlen( unitList ) < globals.gNumMedia ||
		globals.gNumMedia == 0 ||
		( lastUnit != NULL && 
		mediaUnitCompare( &amUnit, lastUnit ) > 0 ) )
	{
	    pUnit = gen_alloc( sizeof( struct amUnit ) );
	    (void) memcpy( pUnit, &amUnit, sizeof( amUnit ) );


	    /*
	     * The list is kept sorted so it is easy to find the lowest
	     * numbered unit.
	     */

	    eptr_search( unitList, &amUnit, &index, mediaUnitCompare );
	    CHECK_NULL( unitList = eptr_insert( unitList, index, pUnit ) );


	    /*
	     * If the newly inserted unit makes the list overfull, delete
	     * the lowest numbered unit from the list.
	     */


	    if ( eptrlen( unitList ) > globals.gNumMedia &&
		    globals.gNumMedia != 0 )
		
	    {
		eptr_del( unitList, 0, 1, free );
	    }


	    /* 
	     * Set the lastUnit pointer to point to the lowest numbered unit.
	     */

	    lastUnit = eptritem( unitList, 0);
	}
    }

    if ( amStatus != AM_NO_UNITS )
    {
	CHECK_AM( amStatus );
    }
    
    
    if ( eptrlen( unitList ) == 0 )
    {
	formatMessage( E_NO_UNITS );
	printMessage( msg, MESS_ERROR );
	status = E_NO_UNITS;
    }
    else if ( eptrlen( unitList ) < globals.gNumMedia )
    {
	formatMessage( S_NUM_ROLLBACK, eptrlen( unitList ) );
	printMessage( msg, MESS_ERROR );
    }


    /* 
     * Loop through each unit in unitList, deleting the units and changing
     * the files associated with the unit to be not associated with a unit.
     */

    for ( i = eptrlen( unitList ) - 1; i >= 0 && status == S_SUCCESS; i-- )
    {
	pUnit = eptritem( unitList, i );
	formatMessage( S_ROLLBACK, globals.gMediaType.amtTypeName,
		pUnit->amuMediaUnitName );
	printMessage( msg, MESS_OPER );


	/*
	 * Build a list of media files which belong to the unit.
	 */

	for ( amStatus = amFileGet( AM_FG_MEDIA_UNIT, &amFile, 
		globals.gDbProc1, pUnit ); 
		amStatus == AM_SUCCESS;
		amStatus = amFileGet( AM_FG_NEXT, &amFile, globals.gDbProc1 ) )
	{
	    CHECK_NULL( pAmFile = (AM_FILE *) gen_alloc( sizeof( AM_FILE ) ) );
	    (void) memcpy( pAmFile, &amFile, sizeof( AM_FILE ) );
	    CHECK_NULL( fileList = eptr_insert( fileList,
		    eptrlen( fileList ), pAmFile ) );
	}

	if ( amStatus != AM_NO_FILE )
	{
	    CHECK_AM( amStatus );
	}


	/*
	 * Loop through the list of files, updating the media unit name to
	 * null in the db.
	 */

	for ( j = 0; j < eptrlen( fileList ); j++ )
	{
	    pAmFile = eptritem( fileList, j );
	    pAmFile->amfMediaUnitName[0] = '\0';
	    CHECK_AM( amFilePut( pAmFile, globals.gDbProc1 ) );
	}


	eptr_del( fileList, 0, eptrlen( fileList ), free );
	    


	/*
	 * Remove the unit from the database.
	 */

	CHECK_AM( amUnitDelete( pUnit, globals.gDbProc1 ) );

	formatMessage( S_ROLLBACK_DONE, globals.gMediaType.amtTypeName, 
		pUnit->amuMediaUnitName );
	printMessage( msg, MESS_OPER );
    }


    /*
     * Free memory
     */
    
    if ( fileList != NULL )
    {
	eptr_free( fileList, free );
	fileList = NULL;
    }


    if ( unitList != NULL )
    {
	eptr_free( unitList, free );
	unitList  = NULL;
    }
    
	
    return( status );
}
