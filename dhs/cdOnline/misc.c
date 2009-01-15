/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
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
*   Module Name:	cdOnline/src/misc.c
*
*   Purpose:
*	Miscelaneous routines for program cdOnline.
*
*   Routines:
*	int	globalsInit	: Initialize the globals structure.
*	int	logDb		: Log to database the startup.
*
*   Date		: Dec 13, 1996
*
*   SCCS data		: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 05/01/98
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/18 JSD :	Added logging to db by added logDb.
*       97/03/18 SEC  : Mod'ed for change to al lib to fetch db
*                       server name in the alDbInfoGet call. 
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_msg.h"
#include "db.h"

#include "globals.h"

/*+
************************************************************************
*
*   Function:	globalsInit
*
*   Purpose:
*	Initialize the globals structure.
*
*   Values Returned:
*	int	CO_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		globalsInit
(
    void
)
{
    globals.gDbproc = NULL;
    globals.gTarget = NULL;
    globals.gUnmount = FALSE;
    globals.gVersion = FALSE;

    return( CO_SUCCESS );
}
