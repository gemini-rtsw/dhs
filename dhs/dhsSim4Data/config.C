static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:30:06 brighton Exp $";
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
// dhsSim4Data/src/config.C
//
// PURPOSE:
// Contains functions to read the configuration file.
//
// METHOD NAME(S)
// configInit - Read the configuration file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:48:31  nhill
// Changed to use the latest version of the cDhsStatus class.
//
// Revision 1.1  1997/10/03 23:06:58  cockayne
// Initial revision
//
// Revision 1.1  1997/07/10 18:27:55  dunn
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
#include "sim.H"
#include "dhsGenConnect.H"


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
//
// FUNCTION VALUE:
// (void) None.
//
// PURPOSE:
// Knows how to read a configuration file
//
// DESCRIPTION:
// Check the status, open the configuration file, read in the dhs library
// stats, then read in all other variables.
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

void		configInit
(
    cSimStatus
		&status		// (mod) Function return status.
)
{
    char	*fileName;	// Name of the config file.
    cConnectList::clStatus	// Connection list status.
    		cStatus;
    

    checkStat( status, return );


    //
    //  Open the configuration file.
    //

    checkConfig( config_open( APPLICATION, &fileName ), status, return );
    status.S_CONFIG_OPEN( status, fileName );


    //
    //  Read the DHS library parameters in (imp and identity)
    //  Read in the authorized user list.
    //  Read the storage paths.
    //  Read in the names of the servers.
    //

    check( cSim::config( status ), status, return );


    //
    // Log the reading of the config file.  Then close the config file.
    //


    config_close();
}
