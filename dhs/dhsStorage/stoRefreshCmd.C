static char rcsid[] = "$Id: stoRefreshCmd.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1999				(c) 1999
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
// dhsStorage/src/stoRefreshCmd.C
//
// PURPOSE:
// Contains code to handle the storage server refresh command.
//
// CLASSES:
// n/a.
//
// METHOD NAME(S)
// cStoSeqCmd::exec		- Process a refresh command.
// cStoSeqCmd::execDefault	- Process an unrecognized command.
// cStoRefreshCmd::execRefresh	- Process an stoRefresh command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:13  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.2  1999/06/17 00:00:09  nhill
// Added refreshing of the device class.
//
// Revision 1.1  1999/02/24 23:58:54  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoAction.H"
#include "stoRefreshCmd.H"
#include "stoMedia.H"
#include "stoUnit.H"
#include "stoStage.H"
#include "stoDevice.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRefreshCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called when a refresh command is received
//
// DESCRIPTION:
// Call the appropriate command handling method. 
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

void		cStoRefreshCmd::exec
(
    void
)
{
    //
    // If we're not ready to receive commands, respond with ERROR. 
    //
    
    if ( !cStoDhs::readyToReceiveCommands() )
    {
	DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

	response( DHS_CS_ERROR, STO_RSP_NOT_READY, dhsStatus );
    }


    //
    // Depending on the command, pick a handling routine. 
    //

    if ( streq( name(), STO_CMD_REFRESH ) )
    {
	execRefresh();
    }
    else
    {
	execDefault();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRefreshCmd::execDefault
//
// INVOCATION:
// execDefault();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method is called when an unknown command is received
//
// DESCRIPTION:
// Log receipt of the command.
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

void		cStoRefreshCmd::execDefault
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
				// DHS Library status.
    char	*id;		// Identity of client.
    char	*ip;		// Ip address of client.
    char	responseString[256];
    				// String to format response into.
    cStoStatus	status;		// Status. 


    //
    // Get connection info. Log error. Respond to client with ERROR. 
    //
    
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.E_CMD_UNKNOWN( status, name(), id, ip );
    (void) sprintf( responseString, STO_RSP_CMD_UNRECOGNIZED, name() );

    response( DHS_CS_ERROR, responseString, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRefreshCmd::execRefresh
//
// INVOCATION:
// execRefresh();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Begin the processing of the refresh command
//
// DESCRIPTION:
// - Ensure the retry flag is set to false.
// - Execute the cDhs::stop function.
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

void		cStoRefreshCmd::execRefresh
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cStoStatus  status;                 // Status.


    //
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // If in simulation mode, just return DONE. 
    //

    if ( cStoDhs::simulate() )
    {
	response( DHS_CS_DONE, dhsStatus );
	return;
    }


    //
    // Initialize list of media objects.
    //


    cStoMedia::init( status );
	    
	    
    //
    // Initialize list of media unit objects.
    //


    cStoUnit::init( status );
	    
	    
    //
    // Initialize list of media stage directory objects.
    //


    cStoStage::init( status );

    
    //
    // Initialize the list of device objects.
    //

    cStoDevice::init( status );


    response( DHS_CS_DONE, dhsStatus );
}
