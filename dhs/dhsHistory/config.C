static char rcsid[] = "$Id: config.C,v 1.1.1.1 2002-11-24 20:28:09 brighton Exp $";
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
// dhsHistory/src/config.C
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
// Revision 1.1.1.1  1999/08/23 19:58:21  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/04/17 22:43:20  nhill
// Updated to use the newest version of the cDhsStatus class.
//
// Revision 1.3  1997/10/20 21:46:30  cockayne
// Check-in for install.
//
// Revision 1.2  1997/10/06 20:14:48  cockayne
// Check-in for code review
//
// Revision 1.1  1997/09/02 21:13:35  cockayne
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

#include "dhs++.H"
#include "cadef.h"

#include "globals.H"
#include "hisDhs.H"
#include "hisEvent.H"
#include "hisDbm.H"
#include "hisCa.H"
#include "hisLogWrite.H"



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
// (!) status 	(cHisStatus &) 	Descripton of the parameter
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
    cHisStatus	&status		// (mod) Function return status.
)
{
    char	*fileName;
    

    checkStat( status, return );


    //
    // Open configuration file.
    //

    checkConfig( config_open( APPLICATION, &fileName ), status, return );
    status.S_CONFIG_READ( status, fileName );


    //
    // Read information describing DHS history server.
    // Read DHS library connection information.
    // Read History-Log-Write command-specific connection information.
    // Read EPICS Channel Access information. 
    // Read database configuration information.
    //

    cHisStatus::config( status );
    cDhs::config( status );
    cHisLogWrite::config( status );
    cHisCa::config( status );
    cHisDbManager::config( status );

    
    //
    // Close the configuration file.
    //

    config_close();


    return;
}
