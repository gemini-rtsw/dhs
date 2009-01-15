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
*   Module Name:	am/src/compare.c
*
*   Purpose:
*	Various comparison functions that are used with eptr lists in
*	the am library.
*
*   Routines:
*	int		devCmp		" Compares two devices.
*	int		scriptCmp	: Compare function for eptr list.
*
*
*   Date		: Nov 9, 1998
*
*   SCCS data		: @(#)
*	Module Name	: compare.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 11/24/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/11/23 JSD :	Removed mediaTypeCmp.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "db.h"

#include "am.h"
#include "localAm.h"

/*+
************************************************************************
*
*   Function:	deviceCmp
*
*   Purpose:
*	Compare to am device structures.
*
*   Values Returned:
*	int	-1		: if devicePtr1 < devicePtr2
*	int	0		: if devicePtr1 = devicePtr2
*	int	1		: if devicePtr2 > devicePtr1
*
************************************************************************
-*/

int	devCmp
(
    AM_DEVICE	*devicePtr1,	/* (in) A media type.			*/
    AM_DEVICE	*devicePtr2	/* (in) A media type.			*/
)
{
    return( strcmp( devicePtr1->amdDeviceName, devicePtr2->amdDeviceName ) );
}

/*+
************************************************************************
*
*   Function:	scriptCmp
*
*   Purpose:
*	Compare to media scripts for equality.
*
*   Values Returned:
*	int	-1		: if script1 < script2
*	int	0		: if script1 = script2
*	int	1		: if script2 > script1
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

int	scriptCmp
(
    char		*typeName,	/* (in)  A media script type.	*/
    AM_MED_SCRIPT	*script		/* (in)  A media script.	*/
)
{
    return( strcmp( typeName, script->amsTypeName ) );
}


/*+
************************************************************************
*
*   Function:	amScript
*
*   Purpose:
*	Return a script name, given a data type.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_SCRIPT_NF	: Script name not found.
*
************************************************************************
-*/

int		amScript
(
    char	type[ AM_MEDIA_TYPE_LEN],
    				/* (in)  The media type.		*/
    AM_SCRIPT_TYPE
    		scriptType,	/* (in)  The script type, media or prep.*/
    char	**scriptName,
    				/* (out) The script name.	*/
    char	**fileName
    				/* (out) The file name.	*/
)
{
    int		index;
    AM_MED_SCRIPT
    		*scriptPtr;	/* Contains info about a media scripts.	*/


    AM_CHECK_INIT;

    

    /*
     *  Find the type in the eptr list.
     */

    if ( ! eptr_search( localAm.laMediaScripts, type, &index, scriptCmp ) )
    {
	return( AM_E_SCRIPT_NF );
    }
    else 
    {
	if ( ( scriptPtr = eptritem( localAm.laMediaScripts, index ) ) == NULL )
	{
	    return ( AM_E_SCRIPT_NF );
	}

	switch ( scriptType )
	{
	    case AM_ST_PREP:
		    if ( scriptPtr->amsPrepScript != NULL )
		    {
			*scriptName = scriptPtr->amsPrepScript;
		    }
		    if ( scriptPtr->amsPrepFile != NULL )
		    {
			*fileName = scriptPtr->amsPrepFile;
		    }
		    break;
	    case AM_ST_WRITE:
		    if ( scriptPtr->amsWriteScript != NULL )
		    {
			*scriptName = scriptPtr->amsWriteScript;
		    }
		    if ( scriptPtr->amsWriteFile != NULL )
		    {
			*fileName = scriptPtr->amsWriteFile;
		    }
		    break;
	    default:
		    return ( AM_E_SCRIPT_NF );
	}
    }

    return( AM_SUCCESS );
}
