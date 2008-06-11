/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	cdOnline/src/main.c
*
*   Purpose:
*	Program to update the cds archive table to indicate that a cd
*	is mounted or dismounted.
*
*   Routines:
*	int		main		: Main routine.
*	static int	parseCmdLine	: Parse the command line.
*	static void	usage		: Print a usage message.
*
*   Date		: Dec 13, 1996
*
*   SCCS data		: @(#)
*	Module Name	: main.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 05/01/98
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/26 JSD  :	Changed capitals in usage message.
*	97/02/18 JSD  :	Added logging to db.
*	97/03/18 SEC  : Mod'ed for change to ad lib to fetch db
*			server name in the adDbInfoGet call. 
*	97/06/20 SEC  :	Fixed version flag. 
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "ad.h"
#include "db.h"

#include "globals.h"


/*
 *  Status function prototypes.
 */

static int	parseCmdLine( int, char ** );
static void	printUsage( void );
 
GLOBALS		globals;


/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main routine for program cdOnline.
*
*   Values Returned:
*	int	CO_SUCCESS	: Normal completion.
*	int	CO_E_AD		: Error in the ad library.
*	int	CO_E_CD		: Error in the cd library.
*	int	CO_E_DB_LIB	: Error in the cadc db library.
*	int	CO_E_MEMORY	: Memory allocation failure.
*	int	CO_E_USAGE	: Command line usage error.
*
************************************************************************
-*/

int		main
(
    int		argc,		/* (in)  Number of command line args.	*/
    char	*argv[]		/* (in)  The command line args.		*/
)
{
    char	*database;	/* archive database name.		*/
    char	*dbserver;	/* archive database server name.	*/
    

    formatMessage( CO_VERSION, version_short );
    printMessage( msg, MESS_ERROR );


    /*
     *  Initialize the global variables.
     */

    CO_CHECK( globalsInit() );


    /*
     *  Parse the command line.
     */

    CO_CHECK( parseCmdLine( --argc, ++argv ) );


    /*
     *  Initialize the ad library.
     */

    CO_CHECK_AD( ad_init() );
    msg_append( msg, ad_msg );
    msg_clear( ad_msg );
    printMessage( ad_msg, MESS_INFO );


    /*
     *  Open the database connection, log to the db, then initialize.
     */

    CO_CHECK_DB_LIB( db_init( APPLICATION ) );

    CO_CHECK_AD( ad_dbinfo_get( AD_MED_CD, &dbserver, &database, 
	    NULL, NULL ) );
    CO_CHECK_DB_LIB( db_open( dbserver, NULL, NULL, database, TRUE, 
	    &globals.gDbproc ) );

    
    if ( globals.gUnmount )
    {
	CO_CHECK( cdUnmount( globals.gTarget ) );
    }
    else
    {
	CO_CHECK( cdMount( globals.gTarget ) );
    }


    CO_CHECK_DB_LIB( db_close( globals.gDbproc ) );
    CO_CHECK_DB_LIB( db_exit() );


    return( CO_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	parseCmdLine
*
*   Purpose:
*	Parse the command line for program cdOnline.
*
*   Values Returned:
*	int	CO_SUCCESS	: Normal completion.
*	int	CO_E_MEMORY	: Memory allocation failure.
*	int	CO_E_USAGE	: Command line usage error.
*
************************************************************************
-*/

static int	parseCmdLine
(
    int		argc,		/* (in)  Number of arguments.		*/
    char	*argv[]		/* (in)  Command line args.		*/
)
{
    int		i;
    boolean	minus_u;	/* -u option used?			*/
    boolean	minus_V;	/* -V option used?			*/
    int		status;		/* Return status.			*/
    boolean	target;		/* Target disk/path set?		*/


    minus_u = FALSE;
    minus_V = FALSE;
    target = FALSE;

    status = CO_SUCCESS;
    i = 0;
    while ( status == CO_SUCCESS && i < argc )
    {
	if ( argv[i][0] == '-' )
	{
	    if ( streq( argv[i], "-u" ) )
	    {
		if ( minus_u )
		{
		    status = CO_E_USAGE;
		}
		else
		{
		    minus_u = TRUE;
		    globals.gUnmount = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-V" ) )
	    {
		if ( minus_V )
		{
		    status = CO_E_USAGE;
		}
		else
		{
		    minus_V = TRUE;
		    globals.gVersion = TRUE;
		}
	    }
	    else
	    {
		status = CO_E_USAGE;
	    }
	}
	else
	{
	    if ( target )
	    {
		status = CO_E_USAGE;
	    }
	    else
	    {
		target = TRUE;
		CO_CHECK_NULL( globals.gTarget = strsav( argv[i] ) );
	    }
	}
	i++;
    }


    if ( globals.gVersion == TRUE )
    {
	formatMessage( CO_VERSION, version_long );
	printMessage( msg, MESS_ERROR );
	status = CO_QUIT;
    }
    
    
    if ( status != CO_QUIT && (
	    ( status != CO_SUCCESS ) || 
	    ( ! minus_V && ! target ) ) )
    {
	printUsage();
	return( CO_E_USAGE );
    }

    return( status );
}

/*+
************************************************************************
*
*   Function:	printUsage
*
*   Purpose:
*	Print a usage message for the 
*
*   Values Returned:
*	none
*
************************************************************************
-*/

static void	printUsage
(
    void
)
{
    (void) fputs(
	"usage: cdOnline -u <diskname>  - Un-mount the specified disk.\n"
	"       cdOnline <path>         - Mount the cd containing the path.\n", 
	stderr );
}
