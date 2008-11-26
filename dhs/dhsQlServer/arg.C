static char rcsid[] = "$Id: arg.C,v 1.1.1.1 2002-11-24 20:28:59 brighton Exp $";
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
// dhsQlServer/src/arg.C
//
// PURPOSE:
// Contains routines to parse the command line arguments.
//
// METHOD NAME(S)
// argInit - Parse the command line.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/04/15 22:30:42  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.3  1997/08/26 21:50:13  nhill
// Fixed version number printing.
//
// Revision 1.2  1997/08/20 21:22:14  nhill
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

#include "globals.H"
#include "qlsDhs.H"
#include "qlsMaster.H"


extern	char 	*version_long;
extern	char 	*version_short;

//
//***********************************************************************
//+
// FUNCTION NAME:
// argInit
//
// INVOCATION:
// argInit( argc, argv, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) Number of command line arguments.
// (>) argv (char *[]) List of command line arguments.
// (!) status (cStaStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Parse the command line
//
// DESCRIPTION:
// Read each of the command line arguments and perform appropriate actions.
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

void		argInit
(
    int		argc,		// (in)  Number of command line arguments.
    char	*argv[],	// (in)  List of command line arguments.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    int		i;

    checkStat( status, VOID );


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-V" ) == 0 )
	{
	    //
	    // Print the program version.
	    //

	    status.S_VERSION( status, version_short, version_long );
	    break;
	}
	else if ( strcmp( argv[i], "-debug" ) == 0 )
	{
	    //
	    // Set the debug level.
	    //

	    //
	    // Make sure there is a second argument.
	    //

	    i++;
	    if ( i >= argc )
	    {
		status.E_USAGE( status );
		break;
	    }


	    //
	    // Set the debug level based on the second argument.
	    //

	    status.debugLevel( argv[i] );
	    if ( ! status.ok() )
	    {
		status.E_USAGE( status );
		break;
	    }
	}
	else if ( strcmp( argv[i], "-simulate" ) == 0 )
	{
	    //
	    // Turn simulation on.
	    //

	    cQlsDhs::simulate( true );
	}
	else if ( strcmp( argv[i], "-master" ) == 0 )
	{
	    //
	    //  Set a flag indicating this is not a slave QLS.
	    //

	    cQlsMaster::isSlave( false );
	}
	else
	{
	    status.E_USAGE( status );
	    break;
	}
    }
}
