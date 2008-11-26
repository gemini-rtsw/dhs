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
*   Module Name:	mediaCleanup/src/compare.c
*
*   Purpose:
*       Contains eptr search comparison functions.
*
*   Routines:
*       int     pathCompare     : EPTR compare function for paths.
*
*   Date		: Oct 26, 1998
*
*   SCCS data		: @(#)
*	Module Name	: compare.c
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 12/15/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */



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
 *  Media Cleanup include files.
 ************************************************************************
 */

#include "globals.h"




/*+
************************************************************************
*
*   Function:   amFileCompare
*
*   Purpose:
*       Compares two paths for searching the eptr list in eptr_search.
*
*   Values Returned:
*       int     1       : path1 comes before path2.
*       int     0       : The paths are identical.
*       int     -1      : path2 comes before path1.
*
************************************************************************
-*/

int             amFileCompare
( 
    AM_FILE     *amFilePtr1,	/* (in)  Am file struct. number 1.	*/
    AM_FILE     *amFilePtr2	/* (in)  Am file struct. number 2.	*/
)
{
    return( strcmp( amFilePtr1->amfFilename, amFilePtr2->amfFilename ) );
}

/*+
************************************************************************
*
*   Function:   pathCompare
*
*   Purpose:
*       Compares two paths for searching the eptr list in eptr_search.
*
*   Values Returned:
*       int     1       : path1 comes before path2.
*       int     0       : The paths are identical.
*       int     -1      : path2 comes before path1.
*
************************************************************************
-*/

int             pathCompare
( 
    char        *path1,         /* (in)  Path string number 1.          */
    char        *path2          /* (in)  Path string number 2.          */
)
{
    return( strcmp( path1, path2 ) );
}
