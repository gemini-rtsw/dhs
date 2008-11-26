static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:26:40 brighton Exp $";
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
// dhsData/src/config.C
//
// PURPOSE:
// Contains functions to read the configuration file.
//
// METHOD NAME(S)
// configInit : Read the configuration file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.5  2000/07/27 20:09:05  dunn
// Added standalone features.
//
// Revision 1.4  1998/05/04 22:58:38  dunn
// Changes due to status class and dhs lib changes.
//
// Revision 1.3  1997/08/29 22:59:43  dunn
// Fixed call to ConManager to print errors off nicer.
//
// Revision 1.2  1997/08/19 22:28:35  dunn
// Changed cConnectList to use local class cDtsConManager
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
#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "store.H"
#include "dhsGenConnect.H"
#include "con.H"


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
// Knows how to read a configuration file.
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
    cDtsStatus	&status		// (mod) Function return status.
)
{
    char	*fileName = NULL;
    				// Name of the config file.
    cDtsConManager::clStatus	// Connection list status.
    		cStatus;
    

    checkStat( status, return );


    //
    //  Open the configuration file.
    //

    checkConfig( config_open( APPLICATION, &fileName ), status, return );
    status.S_CONFIG_OPEN( status, fileName );


    //
    //  Read the DHS library parameters in (imp and identity)
    //  Read the compression, server and segregation information.
    //  Read in the authorized user list.
    //  Read the storage paths.
    //  Read in the names of the servers.
    //


    check( cDhs::config( status ), status, return );
    check( cDtsDhs::config( status ), status, return );
    check( cDtsDbManager::config( status ), status, return );
    check( cDtsStoreManager::config( status ), status, return );
    check( cDtsLists::config( status ), status, return );
    cDtsConManager::config( cStatus );
    if ( !cStatus.ok() )
    {
	status.E_CONN_LIST( status, "cDtsConManager::config", 
		cStatus.message() );
	return;
    }

    //
    // Log the reading of the config file.  Then close the config file.
    //


    config_close();
}
