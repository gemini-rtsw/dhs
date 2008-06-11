static char rcsid[] = "$Id: arg.C,v 1.1.1.1 2002-11-24 20:26:35 brighton Exp $";
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
// dhsData/src/arg.C
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
// Revision 1.5  2000/07/27 20:08:12  dunn
// Changed to make standalone arguments.
//
// Revision 1.4  1998/05/04 22:57:17  dunn
// removed status.display items.
//
// Revision 1.3  1997/09/17 22:09:37  dunn
// Added simulate in the command line.
//
// Revision 1.2  1997/08/19 22:27:22  dunn
// Added noServer and noParse command line args
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
#include "dtsDhs.H"
#include "store.H"

extern	char 	*version_long;
cDhsStatus::tDebugLevel	cDtsStatus::dhsLevel;
bool		cDtsStatus::dtsCleanup = TRUE;
bool		cDtsStatus::dtsHeader = TRUE;
bool		cDtsStatus::dtsServer = TRUE;
bool		cDtsStatus::dtsStandalone = FALSE;

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
// Check the status.  For each argument, read it and if invalid then
// exit.
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
    cDtsStatus	&status		// (mod) Function return status.
)
{
    int		i;
    char	*tmp;			//  Temp. ptr. to a string.

    //
    //  Check the status.
    //

    checkStat( status, return );	// from dhsGenStatus


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-V" ) == 0 )
	{
	    //
	    //  Print out the version information.
	    //

	    status.S_VERSION( status, version_long );
	    status.display();
	    break;
	}
	else if ( strcmp( argv[i], "-debug" ) == 0 )
	{
	    //
	    //  Set the Debug Level.
	    //

	    i++;
	    if ( i >= argc || argv[i] == NULL )
	    {
		status.E_USAGE( status );
		status.display();
		break;
	    }
	    status.debugLevel( argv[i] );
	    if ( ! status.ok() )
	    {
		status.E_USAGE( status );
		break;
	    }

	    //
	    //  Check for NO_DHS flag.  Means set DHS debug level to NONE.
	    //

	    if ( ++i < argc && argv[i] != NULL &&
		    strcmp( argv[i], "NO_DHS" ) == 0 )
	    {
		status.defDhsDebug( ( cDhsStatus::tDebugLevel ) 0 );
	    }
	    else
	    {
		status.defDhsDebug( cDtsStatus::debugLevel() );
		i--;
	    }
	}
	else if ( strcmp( argv[i], "-noDb" ) == 0 )
	{
	    //
	    //  Set standalone flag.
	    //


	    status.standAlone( TRUE );

	}
	else if ( strcmp( argv[i], "-temp" ) == 0 )
	{
	    //
	    //  Set the temporary directory, and set the
	    //  standalone flag.
	    //

	    i++;
	    if ( i >= argc || argv[i] == NULL )
	    {
		status.E_USAGE( status );
		status.display();
		break;
	    }


	    //
	    //  Check for valid path, and save it.
	    //

	    if ( i < argc && argv[i] != NULL )
	    {
		tmp = (char *) strsav( argv[i] );
		cDtsStoreManager::setTempPath( status, tmp );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
		break;
	    }
	}
	else if ( strcmp( argv[i], "-perm" ) == 0 )
	{
	    //
	    //  Set the permanent directory.
	    //

	    i++;
	    if ( i >= argc || argv[i] == NULL )
	    {
		status.E_USAGE( status );
		status.display();
		break;
	    }


	    //
	    //  Check for valid path, and save it.
	    //

	    if ( i < argc && argv[i] != NULL )
	    {
		tmp = (char *) strsav( argv[i] );
		cDtsStoreManager::setPermPath( status, tmp );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
		break;
	    }
	}
	else if ( strcmp( argv[i], "-simulate" ) == 0 )
	{
	    //
	    //  Set simulate on.  Means do nothing in puts.
	    //

	    cDtsDhs::simulate( true );
	}
	else if ( strcmp( argv[i], "-noServers" ) == 0 )
	{
	    //
	    //  Set servers off.  Means don't notify any servers, oldp, ss, ql.
	    //

	    status.serversOn( FALSE );
	}
	else if ( strcmp( argv[i], "-noParse" ) == 0 )
	{
	    //
	    //  Set parsing off.  Means don't parse fits header informatin.
	    //

	    status.parseOn( FALSE );
	}
	else if ( strcmp( argv[i], "-noCleanup" ) == 0 )
	{
	    //
	    //  Set cleanup off.  Means don't delete from temp upon startup.
	    //

	    status.cleanupOn( FALSE );
	}
	else
	{
	    status.E_USAGE( status );
	    status.display();
	    break;
	}
    }


    //
    //  Check that, if standalone is set, then both the temp
    //  and perm path are set.

    if ( status.ok() )
    {

	if ( status.standAlone() && 
		( cDtsStoreManager::permPath() == NULL ||
		  cDtsStoreManager::tempPath() == NULL ) )
	{
	    status.E_USAGE( status );
	    status.display();
	}
	else if ( status.standAlone() )
	{
	    //
	    //  Turn parsing off for standalone.
	    //  

	    status.parseOn( FALSE );
	}
	else
	{
	    //
	    // Otherwise, not doing standalone, so check that the 
	    // temp and perm path are not set.
	    //
	    if ( cDtsStoreManager::permPath() != NULL ||
		  cDtsStoreManager::tempPath() != NULL ) 
	    {
		status.E_USAGE( status );
		status.display();
	    }
	}
    }
}
