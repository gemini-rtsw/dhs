static char rcsid[] = "$Id: arg.C,v 1.1.1.1 2002-11-24 20:30:05 brighton Exp $";
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
// dhsSim4Data/src/arg.C
//
// PURPOSE:
// Read the command line
//
// METHOD NAME(S)
// argInit - Parse the command line.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:46:42  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.1  1997/10/03 23:06:31  cockayne
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

#include "globals.H"
#include "sim.H"


extern	char 	*version_long;

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
// For each argument, read it and if invalid then exit
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
    cSimStatus
    		&status		// (mod) Function return status.
)
{
    int		i;		// Loop counter, index into argv.	


    checkStat( status, return );


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-debug" ) == 0 )
	{
	    if ( ++i >= argc )
	    {
		status.E_USAGE( status );
		break;
	    }
	    status.debugLevel( argv[i] );
	    if ( ! status.ok() )
	    {
		status.E_USAGE( status );
		break;
	    }
	}
	else if ( strcmp( argv[i], "-notify" ) == 0 )
	{
	    cSim::notify( TRUE );
	}
	else
	{
	    status.E_USAGE( status );
	    break;
	}
    }
}
