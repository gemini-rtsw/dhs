static char rcsid[] = "$Id: simPutDefault.C,v 1.1.1.1 2002-11-24 20:30:12 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
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
// simPutDefault.C
//
// PURPOSE:
// Handle data put commands (default)
//
// MEHTOD NAME(S)
// cSimPutDefault::exec()
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:37  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:53:46  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.1  1997/10/03 23:08:09  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
// Include Files. 
//

#include "globals.H"
#include "simPutDefault.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cSimPutDefault::exec
//
// INVOCATION:
// n.a. called by put handler
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Handles the default case of the dhs put command.
//
// DESCRIPTION:
// Not much different from purpose.
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

void		cSimPutDefault::exec
(
    void
)
{
    char	*ip;		// Ip address of the connector.
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
                                // DHS library function return status.
    char	*id;		// Name of the connector.
    cSimStatus
		status;		// Status.
 

    //
    //  Send a response to the client so it can get on with its life.
    //
 
    response( DHS_CS_BUSY, "working", dhsStatus );
 
 
    //
    //  Get information about who sent the command.
    //
 
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
            dhsStatus, status, VOID );
 

    //
    //  Display a message.
    //
 
    status.E_INVALID_PUT( status, label(), id, ip );
 
 
    //
    //  Send an error back.
    //
 
    response( DHS_CS_ERROR, "Invalid put request received", dhsStatus );
}
