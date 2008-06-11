static char rcsid[] = "$Id: simCmdOldps.C,v 1.1.1.1 2002-11-24 20:30:09 brighton Exp $";
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
// <filename>
//
// PURPOSE:
// Whatever
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:52:37  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.1  1997/10/03 23:07:27  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "globals.H"
#include "simCmdOldps.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// name
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) paramName (int) Descripton of the parameter
// (!) paramName (float) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cSimCmdOldps::exec
(
    void
)
{
    cDhsAttrib	attrib;		// An attribute of the list. 
    char	*ip;		// Ip address of the connector.
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
                                // DHS library function return status.
    char	*id;		// Name of the connector.
    cSimStatus
		status;		// Status.
    void	*pData;		// Pointer to the data for an attribute.
 

    //
    //  Send a response to the client so it can get on with its life.
    //
 
    response( DHS_CS_BUSY, "Working - OLDPS Simulator", dhsStatus );
 
 
    //
    //  Get information about who sent the command.
    //
 
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
            dhsStatus, status, VOID );
 

    //
    //  Unpack the av list.
    //

    attrib = find( STV_OLDPS_FILEID, dhsStatus );
    attrib.info( &pData, dhsStatus );


    //
    //  Print message.
    //
 
    status.S_CMD_OLDPS( status, name(), id, ip );
    status.S_CMD_OLDPS_AV( status, STV_OLDPS_FILEID, pData );
 
 
    //
    //  Send an error back.
    //
 
    response( DHS_CS_DONE, "Done - OLDPS Simulator", dhsStatus );
}
