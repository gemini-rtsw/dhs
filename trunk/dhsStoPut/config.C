static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:31:02 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 2000				(c) 2000
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
// dhsStoPut/src/config.C
//
// PURPOSE:
// Contains configuration initialization methods.
//
// METHOD NAME(S)
// configInit - read the configuration file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.2  2001/01/12 22:22:10  jaeger
// Added documentation.
//
// Revision 1.1  2000/12/05 06:52:59  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include "globals.H"
#include "stoPutDhs.H"
#include "lists.H"

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// configInit
//
// INVOCATION:
// configInit( pStoPutDhs, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pStoPutDhs  (cStoPutDhs*)    	Object handling the data push.
// (!) status	   (cStoPutStatus &)	Function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the configuration file for the program.
//
// DESCRIPTION:
// Read the configuration file for the various components of the
// program.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// cStoPutDhs::config()
// cMediaTypeList::config()
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	configInit
(
    cStoPutDhs		*pStoPutDhs,	
    cStoPutStatus	&status		// (mod) Function return status.
)
{
    char	*fileName;		// Ptr to returned config. file.

    checkStat( status, return );

    
    //
    // Open the configuration file.
    //

    checkConfig( config_open( APPLICATION, &fileName ), status, return );
    status.S_CONFIG_READ( status, fileName );


    //
    // Read the necessary infromation :
    //		dhsStoPut client identity
    //		Storage Server IP Addr. and name
    //		organization name.
    //		command-line argument default values.
    //		mapping of media types from retrieval to dhs (storage server).
    //

    pStoPutDhs->config( status );
    cMediaTypeList::config( status );
    

    //
    // Close the configuration file.
    //

    config_close();

    return;
}
    
