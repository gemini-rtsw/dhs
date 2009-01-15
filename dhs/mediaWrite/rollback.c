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
*   Module Name:	mediaWrite/src/rollback.c
*
*   Purpose:
*	Contains functions to rollback a write operation.
*
*   Routines:
*	int	rollback	: Rollback a write operation.
*
*   Date		: Mar 1, 1999
*
*   SCCS data		: @(#)
*	Module Name	: rollback.c
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 03/08/99
*
*   Programmer		: Norm Hill
*
*   Modification History:
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
*	Rollback a previous write operation.
*
*   Values Returned:
*	S_SUCCESS	: Normal completion.
*	E_AM		: Error in the am library.
*
************************************************************************
-*/

int		rollback
(
    void
)
{
    formatMessage( S_ROLLBACK, globals.gMediaUnit.amuMediaUnitName, 
	    globals.gMediaId );
    printMessage( msg, MESS_OPER );


    /*
     * Update the the db. Decrement the number of copies written by and,
     * and set status to 'P' for the media unit.
     */

    if ( globals.gNumCopies == 0 )
    {
	globals.gMediaUnit.amuCopiesWritten = 0;
    }
    else
    {
	globals.gMediaUnit.amuCopiesWritten = 
		MAX( globals.gMediaUnit.amuCopiesWritten - 
		globals.gNumCopies, 0 );
    }
    globals.gMediaUnit.amuStatus = AMU_PREPARED;
    CHECK_AM( amUnitPut( &(globals.gMediaUnit), globals.gDbProc1 ) );

    formatMessage( S_ROLLBACK_DONE, globals.gMediaUnit.amuMediaUnitName, 
	    globals.gMediaId );
    formatMessage( S_COPIES_WRITTEN, globals.gMediaUnit.amuMediaUnitName,
	    globals.gMediaUnit.amuCopiesWritten );
    printMessage( msg, MESS_OPER );


    return( S_SUCCESS );
}
