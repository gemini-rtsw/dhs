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
*   Module Name:	mediaPrep/src/stats.c
*
*   Purpose:
*	Contains routines for accessing and updating mediaPrep's stats.
*
*   Routines:
*	int	addToFileStats		: For creating file statistics. 
*	void	updateStagedFileStats	: Updates file statistics.
*	void	printFileStats		: Prints file stats message. 
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: stats.c
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 12/07/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
#*	98/09/09 SDJ	: Copied from cdPrep
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */
 
#include <stdio.h>
#include <unistd.h>
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
#include "gen_msg.h"
#include "gen_eptr.h"
#include "gen_config.h"
#include "db.h"
#include "am.h"
 
 
/*
 ************************************************************************
 *  Media Prep  include files.
 ************************************************************************
 */

#include "globals.h"



/*+
************************************************************************
*
*   Function:   addToFileStats
*
*   Purpose:
*	Given the parameter status, adds to the correct total. 
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion. 
*
************************************************************************
-*/
 
int		addToFileStats
(
    char	status,		/* (in)  Status to add to statistics.	*/
    const char	unitName[AM_MEDIA_UNIT_NAME_LEN ]
    				/* (in)  Media unit file is assigned to.*/
)
{
    globals.gNumFiles++;

    switch( status )
    {
	case AMF_UNPROCESSED:
	case AMF_UNAVAILABLE:
	    globals.gNumNeedReRetrieving++;
	    break;

	case AMF_RETRIEVED:
	    globals.gNumInRS++;
	    break;
	    
 	case AMF_DONE:
	    break;

	case AMF_ERROR:
	default:
	    globals.gNumBadStatus++;
	    break;
    }

    if ( unitName != NULL && strne( unitName, "" ) )
    {
	globals.gNumQueued++;
    }
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   printFileStats
*
*   Purpose:
*	Prints the file statistics message. 
*
*   Values Returned:
*	none
*
************************************************************************
-*/
 
void		printFileStats
(
    char	*message	/* (in)  Message to add to stats.	*/
)
{
    formatMessage( S_STATS, message, globals.gNumFiles, globals.gNumInRS,
	    globals.gNumQueued, globals.gNumNeedReRetrieving, 
	    globals.gNumBadStatus );
    printMessage( msg, MESS_INFO );
}

