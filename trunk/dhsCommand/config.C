static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:25:28 brighton Exp $";
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
// dhsCmd/src/config.C
//
// PURPOSE:
// Contains functions to read the configuration file.
//
// MEHTOD NAME(S)
// configInit - Read the configuration file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/04/16 19:01:18  nhill
// Updated the documentation.
// Changed to use the latest version of the cDhsStatus class.
//
// Revision 1.3  1998/03/13 21:52:18  jaeger
// Check is status is "ok" rather than for success after calling
// config methods.
//
// Revision 1.2  1997/06/13 20:46:19  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.1  1997/04/16 21:50:14  nhill
// Initial revision
//
// Revision 1.1  1997/03/27 22:15:42  nhill
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

#include "globals.H"
#include "cmdDhs.H"
#include "cmdSubsystem.H"
#include "cmdCommandInfo.H"

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
// (!) status (cStaStatus) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the command server configuration file
//
// DESCRIPTION:
// - Open the configuration file.
// - Configure the cDhs class.
// - Configure the cCmdSubsystem class.
// - Configure the cCmdCommandInfo class.
// - Close the configuration file.
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
    cCmdStatus	&status		// (mod) Function return status.
)
{
    char	*fileName;
    cCmdSubsystem::cStatus	// Subsystem function return status.
    		ssStatus;
    

    checkStat( status, return );

    checkConfig( config_open( APPLICATION, &fileName ), status, return );

    check( cDhs::config( status ), status, return );
    
    checkSubsystem( cCmdSubsystem::config( ssStatus ), ssStatus, status, 
	    return );
    check( cCmdCommandInfo::config( status ), status, return );
    
    status+=ssStatus;
    status.S_CONFIG_READ( status, fileName );

    config_close();
}
