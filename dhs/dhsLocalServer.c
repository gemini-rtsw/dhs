static char rcsid[] = "$Id: dhsLocalServer.c,v 1.1.1.1 2002-11-24 20:20:26 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1996				(c) 1996.
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
 * All rights reserved			Tous droits reserves
 * 					
 * NRC disclaims any warranties,	Le CNRC denie toute garantie
 * expressed, implied, or statu-	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.	fortuit, resultant de l'utili-
 * 					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * dhs/src/dhsLocalServer.c
 *
 * PURPOSE:
 * Contains routines only used by a DHS server.
 *
 * FUNCTION NAME(S)
 * dhsSetTimeout - Set the timeout to be used by clients attempting to
 *		   reconnect to this server.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.7  1998/08/04 16:15:11  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.6  1997/12/18 21:51:23  nhill
 * Added function dhsSetTimeout.
 *
 * Revision 1.5  1997/02/24 19:17:17  nhill
 * Checkin for install.
 *
 * Revision 1.4  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.3  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef DHS_NO_SERVER

/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdarg.h>
#include <stdlib.h>


/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsSetTimeout
 *
 * INVOCATION:
 * dhsSetTimeout( 20 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) timeout (int) The new timeout value in seconds.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Set the timeout to be used by clients attempting to reconnect to 
 * this server
 * 
 * This value is passed to clients in the connection response message.
 * The timout only effects connections made after the timeout is set.
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsSetTimeout
(
    int		timeout		/* (in)  The new timeout in seconds.	*/
)
{
    dhsLocal.dlTimeout = timeout;
}
#endif /* DHS_NO_SERVER */
