static char rcsid[] = "$Id: cmdServer.c,v 1.1.1.1 2002-11-24 20:19:58 brighton Exp $";
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
 * dhs/src/cmdServer.c
 *
 * PURPOSE:
 * Contains interface routines in the DHS library used mainly by the
 * icd1c interface by applications acting as servers.
 *
 * FUNCTION NAME(S)
 * dhsCmdResponse - Send a response to a command.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:57  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.8  1998/08/04 16:13:20  nhill
 * Rearranged to code to be more object-oriented.
 *
 * Revision 1.7  1998/02/16 23:42:48  nhill
 * Changed to use dhsConnectUnlock to unlock the connection mutex.
 *
 * Revision 1.6  1998/02/05 18:59:08  nhill
 * Removed the conditional compilation flags.
 *
 * Revision 1.5  1997/12/18 21:53:17  nhill
 * Added checking for closed connections to dhsCmdResponse.
 *
 * Revision 1.4  1997/02/24 19:17:17  nhill
 * Checkin for install.
 *
 * Revision 1.3  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.2  1997/01/20 17:15:11  nhill
 * Checkpoint.
 *
 * Revision 1.1  1997/01/06 17:24:10  nhill
 * Initial revision
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
-*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdarg.h>
#include <stdlib.h>


/*
 ************************************************************************
 *  DHS library specific include files.
 ************************************************************************
 */

#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdResponse
 *
 * INVOCATION:
 * dhsCmdResponse( connect, tag, cmdStatus, statusString, avList, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to send the response to.
 * (>) tag (DHS_TAG) Command tag send by the client.
 * (>) cmdStatus (DHS_CMD_STATUS) New status for the command.
 * (>) statusString (char *) String describing the command status.
 * (>) avList (DHS_AV_LIST) Attribute value list containing the response.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send a command response to a client
 *
 * DESCRIPTION:
 * Create a new command object and send it to the client.  The command
 * object is deleted immediately since it is never referenced again by
 * the server.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized the the dhsInit function.
 * The tag must be supplied by the server command callback function.
 * The attribute value list must be created with the dhsAvListNew function
 * (unless it is the null av list, DHS_AV_LIST_NULL).
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsCmdResponse
(
    DHS_CONNECT	connect,	/* (in)  Connection to the client.	*/
    DHS_TAG	tag,		/* (in)  Client command tag.		*/
    DHS_CMD_STATUS		/* (in)  New command state.		*/
		cmdStatus,
    const char	*statusString,	/* (in)  State description.		*/
    DHS_AV_LIST	avList,		/* (in)  Response av list.		*/
    DHS_STATUS	*pStatus	/* (mod) function return status.	*/
)
{
    tDhsCmd	*response;	/* The response command.		*/
    DHS_STATUS	s2;		/* Temporary function status.		*/


    fnEntry( "dhsCmdResponse" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Create a new command structure.
     */

    check( response = dhsCmdNew( statusString, avList, NULL, DHS_MSG_CMD_REPLY, 
	    FALSE, tag, cmdStatus, pStatus ), *pStatus, VOID );


    /*
     *  Send the command to the client.
     */

    dhsConnectLock( dhsConnectPtr( connect ), pStatus );
    dhsConnectSend( dhsConnectPtr( connect ), response, pStatus );
    if ( *pStatus != DHS_E_CON_NOT_FOUND )
    {
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( dhsConnectPtr( connect ), &s2 );
    }


    /*
     *  Delete the command structure.
     */

    dhsCmdDeref( response, pStatus );

    fnReturn( VOID );
}
