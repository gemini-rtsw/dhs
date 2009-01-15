static char rcsid[] = "$Id: sf.c,v 1.1.1.1 2002-11-24 20:35:47 brighton Exp $";
/*
************************************************************************
****  C A N A D I A N	A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) <year>				(c) <year>
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
*					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.	 NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
*					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * sf/src/sf.c
 *
 * PURPOSE:
 * Library to convert between FITS and SDS file formats.

 *
 * FUNCTION NAME(S)
 * in	    sfExit	: Cleanup the SF library.
 * int	    sfInit	: Initialize SF library.
 * void	    sfInitSet   : Set the SF library initialization status.
 * boolean  sfInitTest	: Check SF initialization status.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.9  1997/08/22 22:10:35  jaeger
 * Added support for multi-dimensional arrays.
 *
 * Revision 1.8  1997/08/15 20:43:37  jaeger
 * Checked in for install
 *
 * Revision 1.7  1997/08/12 21:07:38  jaeger
 * Checked in for install
 *
 * Revision 1.6  1997/08/06 18:30:48  jaeger
 *  Checked in for install and testing with dhs
 *
 * Revision 1.5	 1997/07/09 16:32:45  jaeger
 *  Converted library from bdt to dhs
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

/*
 ************************************************************************
 *  CADC Library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_msg.h"

/*
 ************************************************************************
 *  cfitsio library include files.
 ************************************************************************
 */

#include "fitsio.h"


/*
 ************************************************************************
 *  sf library include files.
 ************************************************************************
 */

#include "sf.h"
#include "localSf.h"


static 	boolean	initialized	= FALSE;

void	sfInitSet( boolean );

void	sfFormatMessage();



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfExit
 *
 * INVOCATION:
 * sfExit( sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) sfStatus 	(SF_STATUS*) 	SF library function status.	
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Cleanup the library before exiting.
 *
 * DESCRIPTION:
 * Description of the algorithm.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - sfInit.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */


void sfExit
( 
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)
{
    DD_STATUS	ddStatus;
    
    fnEntry ( "sfExit", *sfStatus );
    statCheck( *sfStatus,  VOID );

    ddStatus = DD_S_SUCCESS;
    ddCheck( ddExit( &ddStatus ), ddStatus, *sfStatus, VOID );
    fnReturn( VOID );
}



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfInit
 *
 * INVOCATION:
 * sfInit ( sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) sfStatus 	(SF_STATUS*) 	SF library function status.	
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Initialize the data dictionary.
 *
 * DESCRIPTION:
 * Call ddInit to intialize the data dictionary.
 * Append any messages from dd, that occured during intialization
 * to the SF messages.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - sfExit
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfInit
( 
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)
{
    DD_STATUS	ddStatus;
    
    fnEntry ( "sfInit", *sfStatus );
    statCheck( *sfStatus,  VOID );

    ddStatus = DD_S_SUCCESS;
    ddCheck( ddInit( &ddStatus ), ddStatus, *sfStatus, VOID );
    msg_append( sfMsg, ddMsg );
    msg_clear( ddMsg );
    

    sfInitSet( TRUE );
        
    fnReturn( VOID );
}



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfInitSet
 *
 * INVOCATION:
 * sfInitSet( booleanValue )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) value	(boolean)  Initialization value (TRUE/FALSE).
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Set the initialized state of the SF library
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * "initialized" - Keeps track of the current state.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None..
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfInitSet
(
    boolean	value		/* (in)  Initialization value.		*/
)
{
    initialized = value;
}



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfInitTest
 *
 * INVOCATION:
 * booleanValue = sfInitTest( )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * None.
 *
 * FUNCTION VALUE:
 * (boolean) TRUE  : If "sfInit" has been executed.
 * (boolean) FALSE : If "sfInit" has not been executed.
 *
 * PURPOSE:
 * Return the current initialization state of the SF library.
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * "initialized" - Keeps track of the current state.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean	sfInitTest
(
    void
)
{
    return( initialized );
}
