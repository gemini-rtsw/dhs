static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:31:37 brighton Exp $";
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
// dhsStorage/src/config.C
//
// PURPOSE:
// Contains configuration initialization method.
//
// METHOD NAME(S)
// configInit - Read the configuration file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.5  2000/02/03 00:55:08  nhill
// Added a call to cStoRequestCmd::config to read the ASTO
// appliation options from the configuration file.
//
// Revision 1.4  1998/05/15 16:59:48  cockayne
// Removed "#define DHS_NO_COMMAND".
//
// Revision 1.3  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
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

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoAction.H"
#include "stoNode.H"
#include "stoConnection.H"
#include "stoRequestCmd.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// configInit
//
// INVOCATION:
// configInit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cStoStatus &)	- Descripton of the parameter
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Read the configuration file for the program
//
// DESCRIPTION:
// Read the configuration file for the various components of the program.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		configInit
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    char	*fileName;	// Ptr to returned config file name. 
    

    checkStat( status, return );


    //
    // Open configuration file.
    //

    checkConfig( config_open( APPLICATION, &fileName ), status, return );
    status.S_CONFIG_READ( status, fileName );


    //
    // Read information describing DHS Stoarge Server:
    // 		DHS Status Server information. 
    // 		DHS library connection information.
    //		Number of processing nodes to use. 
    // Close the configuration file.
    //

    cStaStatus::config( status );
    cDhs::config( status );
    cStoNode::config( status );
    cStoConnection::config( status );
    cStoRequestCmd::config( status );

    config_close();


    return;
}
