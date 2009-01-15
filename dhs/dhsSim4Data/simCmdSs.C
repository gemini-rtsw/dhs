
static char rcsid[] = "$Id: simCmdSs.C,v 1.1.1.1 2002-11-24 20:30:11 brighton Exp $";
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
// simCmdSs.C
//
// PURPOSE:
// Handle commands sent to the storageServer simulator.
//
// METHOD NAME(S)
// cSimCmdSs::exec()
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:53:02  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.1  1997/10/03 23:07:54  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  Include files.
//

#include "globals.H"
#include "sim.H"
#include "simCmdSs.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cSimCmdSs::exec
//
// INVOCATION:
// n.a. This function is called by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called by put handler to process storageServer simulator commands
//
// DESCRIPTION:
// Print information about the connection and the a-v list, sent back 
// notification of completion of command. 
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
// Probably loads. 
//-
//***********************************************************************
//

void		cSimCmdSs::exec
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
    void	*pData1;	// Pointer to the data for an attribute.
    void	*pData2;	// Pointer to the data for an attribute.
    void	*pData3;	// Pointer to the data for an attribute.
    cDhsConnection
    		server;		// Connection to remote server.
    cDhsClientCommand	
		avList( STV_SS_NOTIFY );		
				// Return av-list. 
    char	tempIntStr[80];	// String for size of file.
 

    //
    //  Send a response to the client so it can get on with its life.
    //
 
    response( DHS_CS_BUSY, "Working - SS Simulator", dhsStatus );
 
 
    //
    //  Get information about who sent the command.
    //
 
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
            dhsStatus, status, VOID );
 

    //
    //  Unpack the av list.
    //

    attrib = find( STV_SS_FILEID, dhsStatus );
    attrib.info( &pData1, dhsStatus );

    attrib = find( STV_SS_FILESIZE, dhsStatus );
    attrib.info( &pData2, dhsStatus );

    attrib = find( STV_SS_SEGREGATIONDIR, dhsStatus );
    attrib.info( &pData3, dhsStatus );


    //
    //  Print messages.
    //
 
    status.S_CMD_SS( status, name(), id, ip );
    status.S_CMD_SS_AV( status, STV_SS_FILEID, pData1 );
    (void) sprintf( tempIntStr, "%d", *((int *) pData2 ) );
    status.S_CMD_SS_AV( status, STV_SS_FILESIZE, tempIntStr );
    status.S_CMD_SS_AV( status, STV_SS_SEGREGATIONDIR, pData3 );
 
 
    //
    //  Send completion back.
    //
 
    response( DHS_CS_DONE, "Done - SS Simulator", dhsStatus );


    //
    //  Send back notification that file archiving is complete. 
    //

    if ( cSim::notify() )
    {
	avList.add( STV_SS_FILEID, DHS_DT_STRING, (char *) pData1, dhsStatus );
	server.open( ip, id, dhsStatus );
	avList.apply( server, dhsStatus );
	avList.wait( dhsStatus );
    }
}
