static char rcsid[] = "$Id: arg.C,v 1.1.1.1 2002-11-24 20:31:02 brighton Exp $";
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
// dhsStoPut/src/arg.C
//
// Read the command line arguments/options given.
//
// METHOD NAME(S)
// argInit - Parse the command line arguments/options given.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.4  2001/01/12 22:20:50  jaeger
// Added proper handling of the -simulate command-line option.  Added
// a duplicate file check for the -f argument.  No longer displays
// usage message if the status is not okay.
//
// Revision 1.3  2000/12/19 19:31:23  jaeger
// Enabled -user.  Moved the setting of the unitPrefix from here to
// main.  Made sure an error message is displayed if the status is
// not okay but the status is not E_USAGE.
//
// Revision 1.2  2000/12/12 21:08:27  jaeger
// Added short version message to the version message.
//
// Revision 1.1  2000/12/05 06:52:35  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <unistd.h>

#include "globals.H"
#include "stoPutDhs.H"
#include "lists.H"


extern	char 	*version_long;
extern	char 	*version_short;

//
//***********************************************************************
//+
// FUNCTION NAME:
// argInit
//
// INVOCATION:
// argInit( argc, argv, pStoPutDhs, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc 	   (int) 	    Number of command line arguments.
// (>) argv 	   (char *[])  	    List of command line arguments.
// (>) pStoPutDhs  (cStoPutDhs*)    Object handling the data push.
// (!) status 	   (cPutStatus &)   Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Parse the command line arguments.
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
    cStoPutDhs  *pStoPutDhs,    // (in)  
    cStoPutStatus
    		&status		// (mod) Function return status.
)
{
    int		i;		// Counter
    char	*srcFile;	// source file path.
    char	*destFile;	// destination on media file path.
    int		numCopies;	// number of copies of each media.
    
    boolean	minusArchive;	// Has user set -archive?
    boolean	minusComp;	// Has user set -comp?
    boolean	minusCopies;	// Has user set -copies?
    boolean	minusDebug;	// Has user set -debug?
    boolean	minusF;		// Has user set -f?
    boolean	minusHelp;	// Has user set -help, -?, or -h?
    boolean	minusID;	// Has user set -id?
    boolean	minusIP;	// Has user set -ip?
    boolean	minusMedia;	// Has user set -media?
    boolean	minusPrefix;	// Has user set -prefix?
    boolean	minusS;		// Has user set -s?
    boolean	minusSimulate;	// Has user set -simulate?
    boolean	minusUser;	// Has user set -user?
    boolean	minusV;		// Has user set -V?


    //
    // Initialize variables
    //

    minusArchive	= FALSE;
    minusComp		= FALSE;
    minusCopies		= FALSE;
    minusDebug		= FALSE;
    minusF		= FALSE;
    minusHelp		= FALSE;
    minusID		= FALSE;
    minusIP		= FALSE;
    minusMedia		= FALSE;
    minusPrefix		= FALSE;
    minusS		= FALSE;
    minusSimulate	= FALSE;
    minusUser		= FALSE;
    minusV		= FALSE;
    
    
    checkStat( status, return );	// from dhsGenStatus


    //
    // Start the parsing.
    //

    for ( i = 1; i < argc && status.ok(); i++ )
    {
	if ( argv[i][0] == '-' )
        {
	    if ( streq( argv[i], "-archive" ) && ! minusArchive 
    		&& ! minusUser && ++i < argc )
	    {
		pStoPutDhs->setArchiveStream( strsav( argv[i] ), status );
		minusArchive = TRUE;
	    }
	    else if ( streq( argv[i], "-comp" ) && ! minusComp && ++i < argc )
	    {
		pStoPutDhs->setCompression( strsav( argv[i] ), status );
		if ( ! status.ok() )
		{
		    status.S_USAGE( status );
		    status.E_USAGE( status );
		}
		minusComp = TRUE;
	    }
	    else if ( streq( argv[i], "-copies" ) && ! minusCopies
		&& ! minusArchive && ++i < argc )
	    {
		if( !str2int( argv[i], &numCopies ) || numCopies < 1 )
		{
		    status.S_USAGE( status );
		    status.E_USAGE( status );
		}
		else
		{
		    pStoPutDhs->setNumCopies( numCopies );
		}
		minusCopies = TRUE;
	    }
	    else if ( streq( argv[i], "-debug" ) && ! minusDebug &&
		++i < argc )
	    {
		status.debugLevel( argv[i] );
		if ( ! status.ok() )
		{
		    status.S_USAGE( status );
		    status.E_USAGE( status );
		}
		minusDebug = TRUE;
	    }
	    else if ( streq( argv[i], "-f" ) && ++i < argc )
	    {
		checkNull( srcFile = (char *) strsav( argv[i] ),
			status, return );
		
		if ( i+1 < argc && argv[i+1][0] != '-' )
		{
		    //
		    // The user has given a destination file.
		    //

		    checkNull( destFile = ( char *) strsav( argv[++i] ),
	    		status, return );
		}
		else
		{
		    destFile = NULL;
		}

		if ( cStoPutList::findFile( srcFile ) != NULL )
		{
		    status.E_DUP_FILE( status, srcFile );
		}
		else
		{
		    cStoPutList::readIn( srcFile, destFile, status );
		}
		minusF = TRUE;
	    }
	    else if ( ( streq( argv[i], "-?" ) || streq( argv[i], "-h" )
    		    || streq( argv[i], "-help" ) ) && ! minusHelp )
	    {
		status.S_USAGE( status );
		status.S_HELP( status );
		minusHelp = TRUE;
	    }
	    else if ( streq( argv[i], "-id" ) && ! minusID && ++i < argc )
	    {
		//
		//  Get the identity string for this program.
		//

		pStoPutDhs->setIdentity( strsav( argv[i] ), status );
		minusID = TRUE;
	    }
	    else if ( streq( argv[i], "-ip" ) && ! minusIP && ++i < argc )
	    {
		//
		//  Get the ipAddress for the Storage Server.
		//

		pStoPutDhs->setStoIpAddr( strsav( argv[i] ), status );
		minusIP = TRUE;
	    }
	    else if ( streq( argv[i], "-media" ) && ! minusMedia &&
    		! minusArchive && ++i < argc )
	    {
		//
		//  Get the media types.
		//
		
		pStoPutDhs->setMediaType( strsav( argv[i] ), status );
		minusMedia = TRUE;
	    }
	    else if ( streq( argv[i], "-prefix" ) && ! minusPrefix &&
        		! minusArchive && ++i < argc )
	    {
		//
		//  Get the prefix for the media names.
		//

		pStoPutDhs->setUnitPrefix( strsav( argv[i] ), status );
		minusPrefix = TRUE;
	    }
	    else if ( streq( argv[i], "-s" ) && ! minusS && ++i < argc )
	    {
		//
		//  Get the identityString of the Storage server.
		//

		pStoPutDhs->setStoIdentity( strsav( argv[i] ), status );
		minusS = TRUE;
	    }
	    else if ( streq( argv[i], "-simulate" ) && ! minusSimulate )
	    {
		//
		// Were are in simulation mode.
		//

		pStoPutDhs->setSimulate( true );
	    }
	    else if ( streq( argv[i], "-user" ) && ! minusUser
    		&& ! minusArchive && ++i < argc )
	    {
		//
		//  Get the user stream the file is to be sent to.
		//

		pStoPutDhs->setUserStream( strsav( argv[i] ), status );
		minusUser = TRUE;
	    }
	    else if ( streq( argv[i], "-V" ) && ! minusV )
	    {
		status.S_VERSION( status, version_short, version_long );
		minusV = TRUE;
	    }
	    else
	    {
		status.S_USAGE( status );
		status.E_USAGE( status );
	    }
	}
	else
	{
	    status.S_USAGE( status );
	    status.E_USAGE( status );
	}
    }

    
    //
    //  If help or version information was requested just return.
    //

    if (  minusHelp || minusV )
    {
	return;
    }
    
    
    //
    //  Check that the mandatory variables are set.
    //

    if ( status.ok() && ( ! minusF || ( ! minusArchive && ! minusUser ) ) )
    {
	status.S_USAGE( status );
	status.E_USAGE( status );
	return;
    }


    return;
}
