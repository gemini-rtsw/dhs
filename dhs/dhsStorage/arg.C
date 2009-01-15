static char rcsid[] = "$Id: arg.C,v 1.1.1.1 2002-11-24 20:31:36 brighton Exp $";
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
// dhsStorage/src/arg.C
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
// Revision 1.4  1999/06/16 23:51:39  nhill
// Added cStoNode to the objects with a simulation mode.
//
// Revision 1.3  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.2  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_COMMAND
#define	DHS_NO_BULK
#define	DHS_NO_GET
#define	DHS_NO_PUT
#define	DHS_NO_SERVER

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoNode.H"


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
// (>) argc 	(int) 		- Number of command line arguments.
// (>) argv 	(char *[]) 	- List of command line arguments.
// (!) status 	(cStoStatus &) 	- Program status.
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
    cStoStatus	&status		// (mod) Function return status.
)
{
    int		i;


    checkStat( status, VOID );


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-V" ) == 0 )
	{
	    //
	    // Print version message.
	    //

	    status.S_VERSION( status, version_short, version_long );
	    break;
	}
	else if ( strcmp( argv[i], "-debug" ) == 0 )
	{
	    //
	    // Set debug level.
	    //

	    i++;
	    if ( i >= argc )
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
	else if ( strcmp( argv[i], "-simulate" ) == 0 )
	{
	    //
	    // Turn simulation on.
	    //

	    i++;
	    if ( i >= argc )
	    {
		status.E_USAGE( status );
		break;
	    }

	    if ( strcmp( argv[i], STO_SIM_LEVEL_VSM ) == 0 || 
		    strcmp( argv[i], STO_SIM_LEVEL_NONE ) )
	    {
		cStoDhs::simulate( false );
	    }
	    else if ( strcmp( argv[i], STO_SIM_LEVEL_FAST ) == 0 || 
		    strcmp( argv[i], STO_SIM_LEVEL_FULL ) )
	    {
		cStoDhs::simulate( true );
		cStoNode::simulate( true );
	    }
	    else
	    {
		status.E_USAGE( status );
		break;
	    }

	}
	else
	{
	    status.E_USAGE( status );
	    break;
	}
    }
}
