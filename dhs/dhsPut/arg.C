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
// dhsPut/src/arg.C
//
// PURPOSE:
// Read the command line
//
// METHOD NAME(S)
// argInit - Parse the command line.
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <unistd.h>

#include "globals.H"
#include "putDhs.H"
#include "list.H"


extern	char 	*version_long;


//
//  Define type strings in the command line.
//

#define PUT_RAW		"RAW"
#define PUT_FITS	"FITS"
#define PUT_RAW_UNIQUE	"RAW_UK"
#define PUT_FITS_UNIQUE	"FITS_UK"
#define PUT_DS		"DS"
#define PUT_DS_QL	"QL"


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
// (!) status (cPutStatus &) Function return status.
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
    cPutStatus	&status		// (mod) Function return status.
)
{
    int		i;
    char	*ipAddr;
    char	*identity;
    char	*dsIdentity;
    char	*dsName;
    char	identityString[31];


//
// WARNING: should redo this, it is not done nicely at all.  It will
// take the first instance of a command line parameter and ignore the rest.
// Should do all checking of whether a variable is set in here, rather then
// have methods in putDhs to do it.  Should fix - later.
//


    checkStat( status, return );	// from dhsGenStatus


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-V" ) == 0 )
	{
	    status.S_VERSION( status, version_long );
	    return;
	}
	else if ( strcmp( argv[i], "-debug" ) == 0 )
	{
	    //
	    //  Get the debug level.
	    //

	    i++;
	    if ( i >= argc || argv[i] == NULL )
	    {
		status.E_USAGE( status );
		return;
	    }
	    status.debugLevel( argv[i] );
	    if ( ! status.ok() )
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-bufZero" ) == 0 )
	{
	    //
	    //  Send a last thing with buffer length zero.
	    //
	    
	    putDhs::buffZero( TRUE );
	}
	else if ( strcmp( argv[i], "-new" ) == 0 )
	{
	    //
	    //  Generate a new label and use it as the ds name.
	    //
	    
	    //putDhs::setNewLabel();
	    std::cout << "new not implemented at this time" << std::endl; 
	    status.E_USAGE( status );
	}
	else if ( strcmp( argv[i], "-id" ) == 0 )
	{
	    //
	    //  Get the identity string for this program.
	    //

	    i++;
	    if ( argv[i] != NULL && strlen ( argv[i] ) > 0 )
	    {
		checkNull( identity = (char *) strsav( argv[i] ),
			status, return );
		putDhs::setIdentity( identity );
	    }
	    else
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-d" ) == 0 )
	{
	    //
	    //  Get the identityString for the data server.
	    //

	    i++;
	    if ( argv[i] != NULL && strlen ( argv[i] ) > 0 )
	    {
		checkNull( dsIdentity = (char *) strsav( argv[i] ),
			status, return );
		putDhs::setDsIdentity( dsIdentity );
	    }
	    else
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-l" ) == 0 )
	{
	    //
	    //  Get the lifetime of the data.
	    //

	    i++;
	    if ( argv[i] != NULL && 
		    ( argv[i][0] == PUT_PERM || argv[i][0] == PUT_TEMP ) )
	    {
		putDhs::setLife( argv[i][0] );
	    }
	    else
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-ip" ) == 0 )
	{
	    //
	    //  Get the ipAddress for the data server.
	    //

	    i++;
	    if ( argv[i] != NULL && strlen ( argv[i] ) > 0 )
	    {
		checkNull( ipAddr = (char *) strsav( argv[i] ),
			status, return );
		putDhs::setIpAddr( ipAddr );
	    }
	    else
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-n" ) == 0 )
	{
	    //
	    //  Get the dataset name.
	    //

	    i++;
	    if ( argv[i] != NULL && strlen ( argv[i] ) > 0 )
	    {
		checkNull( dsName  = (char *) strsav( argv[i] ),
			status, return );
		putDhs::setDSName( dsName );
	    }
	    else
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-noLast" ) == 0 )
	{
	    putDhs::setNoLast();
	}
	else if ( strcmp( argv[i], "-f" ) == 0 )
	{
	    //
	    //  Get the file names.
	    //

	    i++;
	    if ( argv[i] == NULL || ! cPutList::readIn( argv[i], status ) )
	    {
		status.E_USAGE( status );
		return;
	    }
	}
	else if ( strcmp( argv[i], "-t" ) == 0 )
	{
	    //
	    //  Get the data type.
	    //

	    i++;
	    if ( argv[i] == NULL )
	    {
		status.E_USAGE( status );
		return;
	    }
	    if ( streq( argv[i], PUT_RAW ) )
	    {
		putDhs::setType( DHS_BD_PT_RAW );
	    }
	    else if ( streq( argv[i], PUT_RAW_UNIQUE ) )
	    {
		putDhs::setType( DHS_BD_PT_RAW_UNIQUE );
	    }
	    else if ( streq( argv[i], PUT_FITS ) )
	    {
		putDhs::setType( DHS_BD_PT_FITS );
	    }
	    else if ( streq( argv[i], PUT_FITS_UNIQUE ) )
	    {
		putDhs::setType( DHS_BD_PT_FITS_UNIQUE );
	    }
	    else if ( streq( argv[i], PUT_DS ) )
	    {
		putDhs::setType( DHS_BD_PT_DS );
	    }
	    else if ( streq( argv[i], PUT_DS_QL ) )
	    {
		putDhs::setType( DHS_BD_PT_DS_QL );
	    }
	    else
	    {
		status.E_USAGE( status );
		return;
	    }

	}
	else
	{
	    //
	    //  This is silly, not sure who put this in??
	    //

	    if ( argv[i] == NULL || ! cPutList::readIn( argv[i], status ) )
	    {
		status.E_USAGE( status );
		return;
	    }
	}
    }


    //
    //  Check that the mandatory variables are set.
    //

    if ( ! putDhs::variablesSet() )
    {
	status.E_USAGE( status );
	return;
    }


    //
    //  If identity string is not set then default to application name.
    //

    sprintf( identityString, "%s.%08x.%04x", 
	    APPLICATION, gethostid(), getpid() );
    putDhs::setIdentity( strsav( identityString ) );


    //
    //  If put type is not set, default to raw.
    //

    putDhs::setType( DHS_BD_PT_RAW );
}
