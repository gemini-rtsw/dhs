static char rcsid[] = "$Id: subsystem.C,v 1.1.1.1 2002-11-24 20:27:55 brighton Exp $";
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
// dhsStatus/src/subsystem.C
//
// PURPOSE:
// Contains the implementation of the cDhsSubsystem class.
//
// MEHTOD NAME(S)
// cDhsSubsystem::cDhsSubsystem - Destructor for the class.
// cDhsSubsystem::config - Read information from a configuration file.
// cDhsSubsystem::exit - Clean up the class befoer exit.
// cDhsSubsystem::subsystem - Read a subsystem line from the configuration file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.5  1998/04/21 05:03:56  nhill
// Updated the documentation.
//
// Revision 1.4  1998/04/20 21:20:44  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.3  1997/06/11 19:48:25  nhill
// Changed to a new status class.
//
// Revision 1.2  1997/04/24 19:35:50  nhill
// Added a "const" keyword.
//
// Revision 1.1  1997/04/15 23:06:56  nhill
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
#include "gen_str.h"
}

#include "localDhsSubsystem.H"

cDhsSubsystem::tSysList	cDhsSubsystem::csList;

msgText( cDhsSubsystem::cStatus, E_NOT_FOUND, NULL,
	    "Could not find subsystem named `%s'." );


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsSubsystem::cDhsSubsystem
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string (char *) String to assign to the subsystem name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor taking a subsystem name as a parameter
//
// DESCRIPTION:
// Assign the name to the csName data member.
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

		cDhsSubsystem::cDhsSubsystem
(
    const char	*string		// (in)  The subsystem name.
)
{
    (void) strcpy( csName, string );

    csList.push_back( this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsSubsystem::config
//
// INVOCATION:
// cDhsSubsystem::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the list of subsystems in the configuration file.
//
// DESCRIPTION:
// Use the configuration library to read the list of subsystems.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsSubsystem::config
(
    cStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    exit( status );

    checkConfig( config_get( "subsystem", subsystem ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsSubsystem::exit
//
// INVOCATION:
// cDhsSubsystem::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Clean up the subsystem list
//
// DESCRIPTION:
// Free all of the subsystem objects and delete the subsystem list.
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

void		cDhsSubsystem::exit
(
    cStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    while( csList.size() > 0 )
    {
	delete csList.back();
	csList.pop_back();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsSubsystem:subsystem
//
// INVOCATION:
// n.a. Called automatically from within the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the subsystem information from the configuration file
//
// DESCRIPTION:
// Create a new cDhsSubsystem object and add it to the list.
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

boolean		cDhsSubsystem::subsystem
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cStatus	status;		// Status code.
    cDhsSubsystem *pSubsystem;	// Pointer to the new subsystem.



    if ( numTokens == 2 )
    {
	checkNull( pSubsystem = new cDhsSubsystem( tokens[1] ),
		status, return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}
