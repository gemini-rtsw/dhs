static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:28:59 brighton Exp $";
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
// dhsQlServer/src/config.C
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
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/15 22:33:07  nhill
// Changed to the latest version of the cDhsStatus class.
//
// Revision 1.2  1997/08/20 21:22:24  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
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

#include "dhs++.H"

#include "globals.H"
#include "qlsDhs.H"
#include "qlsStream.H"
#include "qlsFitsKeyword.H"
#include "qlsMaster.H"
#include "qlsDsCommands.H"

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
// ([C type]) description of the return value.
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
    cQlsStatus	&status		// (mod) Function return status.
)
{
    char	*fileName;
    

    checkStat( status, return );


    //
    // Open the configuration file.
    //

    checkConfig( config_open( APPLICATION, &fileName ), status, return );


    //
    // Read the information describing the DHS status server.
    //

    cQlsStatus::config( status );


    //
    // Read the DHS library connection information.
    //

    cDhs::config( status );
    cQlsDsGet::config( status );


    //
    // If this is a master QLS, read the list of default streams from
    // the configuration file.
    //

    if ( ! cQlsMaster::isSlave() )
    {
	cQlsStream::config( status );
    }


    //
    // If this is a slave QLS, read the information required to locate a 
    // master QLS.
    //

    if ( cQlsMaster::isSlave() )
    {
	cQlsMaster::config( status );
    }


    //
    // Read the FITS keyword information.
    //

    cQlsFitsKeyword::config( status );



    //
    // Log a message.
    //

    status.S_CONFIG_READ( status, fileName );


    //
    // Close the configuration file.
    //

    config_close();
}
