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
*   Module Name:	mediaCleanup/src/rollback.c
*
*   Purpose:
*	Rollback a mediaCleanup operation.
*
*   Routines:
*	int	rollback	: Rollback a mediaQueue operation.
*
*   Date		: Mar 16, 1999
*
*   SCCS data		: @(#)
*	Module Name	: rollback.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Norm Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_msg.h"
#include "db.h"
#include "am.h"

#include "globals.h"


static int	getFileList( EPTR ** );
static int	setUnitQueued( EPTR * );

/*+
************************************************************************
*
*   Function:	getFileList
*
*   Purpose:
*	Get a list of files in teh media unit.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory allocation failure.
*	int	E_AM		: Error in the AM library.
*
************************************************************************
-*/

static int	getFileList
(
    EPTR	**fileList	/* (out) The list of files.		*/
)
{
    AM_FILE	amFile;		/* AM lib. file structure.		*/
    AM_FILE	*pAmFile;	/* AM lib. file structure ptr.		*/
    int		amStatus;	/* Status returned by am functions.	*/
    int		index;		/* index for eptr. list.		*/


    CHECK_NULL( *fileList = eptr_alloc( 1000 ) );

    
    /*
     * Construct list of files to be removed.
     */

    for( index = 0, amStatus = amFileGet( AM_FG_MEDIA_UNIT, &amFile,
	    globals.gDbProc, globals.gMediaUnit ); amStatus == AM_SUCCESS;
	    index++, amStatus = amFileGet( AM_FG_NEXT, &amFile,
	    globals.gDbProc ) )
    {
	CHECK_NULL( pAmFile = (AM_FILE*) gen_alloc( sizeof( AM_FILE ) ) );
	CHECK_NULL( gen_cpy( pAmFile, &amFile, sizeof( AM_FILE) ) );
	CHECK_NULL( eptr_insert( *fileList, eptrlen( *fileList ), pAmFile ) );
    }

    if ( amStatus != AM_NO_FILE )
    {
	CHECK_AM( amStatus );
    }
    CHECK_NULL( eptr_sort( *fileList, amFileCompare ) );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	rollback
*
*   Purpose:
*	Rollback a mediaCleanup operation.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		rollback
(
    void
)
{
    EPTR	*fileList;	/* A list of files in the unit.		*/


    formatMessage( S_ROLLBACK, globals.gMediaUnit.amuMediaUnitName,
	    globals.gMediaUnit.amuMediaRequestId );
    printMessage( msg, MESS_OPER );


    /*
     * Get a list of files in the unit.
     */

    CHECK( getFileList( &fileList ) );


    /*
     * Change the media unit state to "Q"ueued, and set the state of all 
     * of the files in the media unit to "R"etrieved.
     */

    CHECK( setUnitQueued( fileList ) );


    eptr_free( fileList, NULL );

    formatMessage( S_ROLLBACK_DONE, globals.gMediaUnit.amuMediaUnitName,
	    globals.gMediaUnit.amuMediaRequestId );
    printMessage( msg, MESS_OPER );

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	setUnitQueued
*
*   Purpose:
*	This function sets the state of a media unit to queued.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AM		: Error in the AM library.
*
************************************************************************
-*/

static int	setUnitQueued
(
    EPTR	*fileList	/* (in) The list of files in the unit.	*/
)
{
    int		i;
    AM_FILE	*pAmFile;
    int		status;


    /*
     * Begin a database transaction to ensure that all or none of the
     * transactions are completed.
     */

    CHECK_DB_LIB( db_simple_query( globals.gDbProc, "begin transaction" ) );
    status = S_SUCCESS;


    /*
     * Change the file states to retrieved.
     */

    for ( i = 0; i < eptrlen( fileList ) && status == S_SUCCESS; i++ )
    {
	pAmFile = eptritem( fileList, i );
	pAmFile->amfStatus = AMF_UNAVAILABLE;
	if ( amFilePut( pAmFile, globals.gDbProc ) != AM_SUCCESS )
	{
	    formatMessage( E_AM );
	    msg_append( msg, amMsg );
	    msg_clear( amMsg );
	    printMessage( msg, MESS_ERROR );
	    status = E_AM;
	}
    }


    /*
     * If all files statues where set to retrieved, set the unit state
     * to queued.
     */

    if ( status == S_SUCCESS )
    {
	globals.gMediaUnit.amuStatus = AMU_QUEUED;
	globals.gMediaUnit.amuCopiesWritten = 
		MAX( 0, globals.gMediaUnit.amuCopiesWritten - 1 );
	if ( amUnitPut( &globals.gMediaUnit, globals.gDbProc ) != AM_SUCCESS )
	{
	    formatMessage( E_AM );
	    msg_append( msg, amMsg );
	    msg_clear( amMsg );
	    printMessage( msg, MESS_ERROR );
	    status = E_AM;
	}
    }

    
    /* 
     * If everything is ok so far, and the media request state is done, set
     * it to AMM_IN_PROG.
     */

    if ( status == S_SUCCESS && globals.gMedia.ammStatus == AMM_DONE )
    {
	globals.gMedia.ammStatus = AMM_IN_PROG;
	if ( amUnitPut( &globals.gMediaUnit, globals.gDbProc ) != AM_SUCCESS )
	{
	    formatMessage( E_AM );
	    msg_append( msg, amMsg );
	    msg_clear( amMsg );
	    printMessage( msg, MESS_ERROR );
	    status = E_AM;
	}
    }


    if ( status == S_SUCCESS )
    {
	CHECK_DB_LIB( db_simple_query( globals.gDbProc, 
		"commit transaction" ) );
    }
    else
    {
	CHECK_DB_LIB( db_simple_query( globals.gDbProc, 
		"rollback transaction" ) );
    }

    return( S_SUCCESS );
}
