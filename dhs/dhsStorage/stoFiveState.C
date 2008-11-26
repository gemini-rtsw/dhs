static char rcsid[] = "$Id: stoFiveState.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsStorage/src/stoFiveState.C
//
// PURPOSE:
// Contains methods of the cStoFiveState class. 
//
// METHOD NAME(S)
// cStoFiveState::cStoFiveState	- Constructor for cStoFivestate class. 
// cStoFiveState::state2str	- Converts a state to a string.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:57  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.2  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_COMMAND
#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER
#define DHS_NO_CLIENT

#include "globals.H"
#include "stoFiveState.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoFiveState::cStoFiveState
//
// INVOCATION:
// cStoFiveState();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
// 
// FUNCTION VALUE:
// (cStoFiveState) A new five-state object.
// 
// PURPOSE:
// Constructor for objects of the cStoFiveState class. 
// 
// DESCRIPTION:
// Set the initial state to IDLE. 
// 
// EXTERNAL VARIABLES:
// None.
// 
// PRIOR REQUIREMENTS:
// None.
// 
// SEE ALSO:
// None.
// 
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cStoFiveState::cStoFiveState
(
    void
)
{
    sfsState = STO_FST_IDLE;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoFiveState::state2str
//
// INVOCATION:
// fiveStateObject.state2str( returnString );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) returnString  (char *)	- String to sprintf() return string into.
// 
// FUNCTION VALUE:
// None.
// 
// PURPOSE:
// Convert a five-state enum value into a string.
// 
// DESCRIPTION:
// Pretty basic.
// 
// EXTERNAL VARIABLES:
// None.
// 
// PRIOR REQUIREMENTS:
// None.
// 
// SEE ALSO:
// None.
// 
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoFiveState::state2str
(
    char	*returnString	// String to sprintf() return into. 
)
{
    switch ( sfsState )
    {
	case STO_FST_IDLE:
	    sprintf( returnString, "%s", STO_FSS_IDLE );
	    break;

	case STO_FST_REQUESTED:
	    sprintf( returnString, "%s", STO_FSS_REQUESTED );
	    break;

	case STO_FST_IN_PROGRESS:
	    sprintf( returnString, "%s", STO_FSS_IN_PROGRESS );
	    break;

	case STO_FST_ROLLBACK_REQUESTED:
	    sprintf( returnString, "%s", STO_FSS_ROLLBACK_REQUESTED );
	    break;

	case STO_FST_ROLLBACK_IN_PROGRESS:
	    sprintf( returnString, "%s", STO_FSS_ROLLBACK_IN_PROGRESS );
	    break;

	case STO_FST_UNKNOWN:
	default:
	    sprintf( returnString, "%s", STO_FSS_UNKNOWN );
	    break;
    }
}
