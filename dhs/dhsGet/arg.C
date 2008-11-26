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
// dhsGet/src/arg.C
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
#include "getDhs.H"
#include "list.H"


extern	char 	*version_long;


//
//  Define type strings in the command line.
//

#define GET_RAW	"RAW"
#define GET_FITS "FITS"
#define GET_HEADER "HEADER"
#define GET_FITS_ASIS "FITS_ASIS"
#define GET_RAW_ASIS	"RAW_ASIS"


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
// (!) status (cGetStatus &) Function return status.
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
    cGetStatus	&status		// (mod) Function return status.
)
{
    int		i;
    char	*ipAddr;
    char	*identity;
    char	*dsIdentity;
    char	*destin;
    char	identityString[31];


//
// WARNING: should redo this, it is not done nicely at all.  It will
// take the first instance of a command line parameter and ignore the rest.
// Should do all checking of whether a variable is set in here, rather then
// have methods in getDhs to do it.  Should fix - later.
//


    checkStat( status, return );	// from dhsGenStatus


    for ( i = 1; i < argc; i++ )
    {
	if ( strcmp( argv[i], "-V" ) == 0 )
	{
	    status.S_VERSION( status, version_long );
	    status.display();
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
		status.display();
		return;
	    }
	    status.debugLevel( argv[i] );
	    if ( ! status.ok() )
	    {
		status.E_USAGE( status );
		status.display();
		return;
	    }
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
		getDhs::setIdentity( identity );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
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
		getDhs::setDsIdentity( dsIdentity );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
		return;
	    }
	}
	else if ( strcmp( argv[i], "-dest" ) == 0 )
	{
	    //
	    //  Get the destination directory.
	    //

	    i++;
	    if ( argv[i] != NULL && strlen ( argv[i] ) > 0 )
	    {
		checkNull( destin = (char *) strsav( argv[i] ),
			status, return );
		getDhs::setDestin( destin );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
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
		getDhs::setIpAddr( ipAddr );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
		return;
	    }
	}
	else if ( strcmp( argv[i], "-n" ) == 0 )
	{
	    //
	    //  Get the dataset names.
	    //

	    i++;
	    if ( argv[i] == NULL ||  ! cGetList::readIn( argv[i], status ) )
	    {
		status.E_USAGE( status );
		status.display();
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
		status.display();
		return;
	    }
	    if ( streq( argv[i], GET_FITS ) )
	    {
		getDhs::setType( DHS_BD_GT_FITS );
	    }
	    else if ( streq( argv[i], GET_FITS_ASIS ) )
	    {
		getDhs::setType( DHS_BD_GT_FITS_ASIS );
	    }
	    else if ( streq( argv[i], GET_RAW ) )
	    {
		getDhs::setType( DHS_BD_GT_RAW );
	    }
	    else if ( streq( argv[i], GET_RAW_ASIS ) )
	    {
		getDhs::setType( DHS_BD_GT_RAW_ASIS);
	    }
	    else if ( streq( argv[i], GET_HEADER ) )
	    {
		getDhs::setType( DHS_BD_GT_FITS_HEADER );
	    }
	    else
	    {
		status.E_USAGE( status );
		status.display();
		return;
	    }

	}
	else
	{
	    status.E_USAGE( status );
	    status.display();
	    return;
	}
    }


    //
    //  Check that the mandatory variables are set.
    //

    if ( ! getDhs::variablesSet() )
    {
	status.E_USAGE( status );
	status.display();
	return;
    }


    //
    //  If identity string is not set then default to applicatio name.
    //

    sprintf( identityString, "%s.%08x.%04x", 
	    APPLICATION, gethostid(), getpid() );
    getDhs::setIdentity( strsav( identityString ) );


    //
    //  If get type is not set, default to raw.
    //

    getDhs::setType( DHS_BD_GT_RAW );

}

