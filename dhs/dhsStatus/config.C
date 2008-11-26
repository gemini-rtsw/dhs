static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:30:26 brighton Exp $";
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
// dhsStatus/src/config.C
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
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1998/06/19 18:02:07  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.6  1998/05/12 20:21:34  nhill
// Updated the documentation.
//
// Revision 1.5  1998/04/15 20:57:00  nhill
// Updated to the newest version of the status class.
//
// Revision 1.4  1998/02/05 21:18:18  jaeger
// Added configuration details for monitoring.
//
// Revision 1.3  1997/06/13 20:41:22  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.2  1997/04/16 22:27:14  nhill
// Removed the dhsSubsystem header file.
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
#include "staDhs.H"
#if defined(EPICS_DHS)
#include "staChannel.H"
#endif
#include "staMonitor.H"    

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
// (!) status (cStaStat &) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the configuration file for the dhsStatus program
//
// DESCRIPTION:
// - Open the configuration file.
// - Read the dhs++ library configuration data.
// - Read the subsystem configuration data.
// - Read the channel configuration data.
// - Read the resource monitoring configuration data.
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
    cStaStat	&status		// (mod) Function return status.
)
{
    char	*fileName;
    cDhsSubsystem::cStatus	// Subsystem function return status.
    		ssStatus;

    ssStatus.display( status );
    checkStat( status, return );

    checkConfig( config_open( APPLICATION, &fileName ), status, return );

    status.S_CONFIG_READ( status, fileName );

    cDhs::config( status );
    checkSubsystem( cDhsSubsystem::config( ssStatus ), ssStatus, status, 
	    return );
#if defined(EPICS_DHS)
    cStaChannel::config( status );
#endif


    //	
    // Read monitoring configuration values.
    //

    cStaMon::configAll( status );
    
    config_close();
}
