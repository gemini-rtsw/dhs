static char rcsid[] = "$Id: arg.C,v 1.1.1.1 2002-11-24 20:30:25 brighton Exp $";
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
// dhsStatus/src/arg.C
//
// PURPOSE:
// This file contans routines to parse the command line arguments.
//
// MEHTOD NAME(S)
// argInit - Function to read the command line arguments.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.11  1998/06/19 18:01:16  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.10  1998/06/01 22:27:10  jaeger
// Made sure cStaDhs and cStaMon classes simulate level is based on
// the command-line argument given.
//
// Revision 1.9  1998/05/20 04:56:16  jaeger
// Added a call to the resource monitor simulate command if running in
// simulate mode.
//
// Revision 1.8  1998/05/12 20:21:21  nhill
// Updated the documentation.
//
// Revision 1.7  1998/04/15 20:56:12  nhill
// Updated the documentation.
// Removed the explicit calls the the status display routine.
//
// Revision 1.6  1998/01/08 20:59:02  jaeger
// Changed a comparison with status.S_SUCCESS to a status.ok().
//
// Revision 1.5  1997/09/05 18:54:34  nhill
// Fixed the version display.
//
// Revision 1.4  1997/08/19 20:11:14  nhill
// Fixed a bug in the command line parsing.
//
// Revision 1.3  1997/06/13 20:41:22  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.2  1997/04/16 22:26:10  nhill
// Changed to use the defDebug function to set the debug mode.
//
// Revision 1.1  1997/03/27 22:15:42  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "globals.H"
#if defined(EPICS_DHS)
#include "staChannel.H"
#endif
#include "staDhs.H"
#include "staMonitor.H"

extern	char 	*version_long;
extern	char	*version_short;

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
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Parse the command line
//
// DESCRIPTION:
// Check each word on the command line and process it appropriately.
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
    cStaStat	&status		// (mod) Function return status.
)
{
    int		i;


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-V" ) == 0 )
	{
	    status.S_VERSION( status, version_short, version_long );
	    break;
	}
	else if ( strcmp( argv[i], "-debug" ) == 0 )
	{
	    i++;
	    if ( i >= argc )
	    {
		status.E_USAGE( status );
		break;
	    }
	    status.debugLevel( argv[i] );
	    if ( !status.ok() )
	    {
		status.E_USAGE( status );
		break;
	    }
	}
	else if ( strcmp( argv[i], "-simulate" ) == 0 )
	{
	    i++;
	    if ( i >= argc )
	    {
		status.E_USAGE( status );
		break;
	    }
	    if ( strcmp( argv[i], "NONE" ) == 0 )
	    {
#if defined(EPICS_DHS)
		cStaChannel::simulate( cStaChannel::SIMULATE_NONE );
#endif
		cStaDhs::simulate( false );
		cStaMon::simulate( false );
	    }
	    else if ( strcmp( argv[i], "VSM" ) == 0 )
	    {
#if defined(EPICS_DHS)
		cStaChannel::simulate( cStaChannel::SIMULATE_VSM );
#endif
		cStaDhs::simulate( false );
		cStaMon::simulate( false );
	    }
	    else if ( strcmp( argv[i], "FAST" ) == 0 )
	    {
#if defined(EPICS_DHS)
		cStaChannel::simulate( cStaChannel::SIMULATE_FAST );
#endif
		cStaDhs::simulate( true );
		cStaMon::simulate( true );
	    }
	    else if ( strcmp( argv[i], "FULL" ) == 0 )
	    {
#if defined(EPICS_DHS)
		cStaChannel::simulate( cStaChannel::SIMULATE_FULL );
#endif
		cStaDhs::simulate( true );
		cStaMon::simulate( true );
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
