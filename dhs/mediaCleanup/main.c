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
*   Module Name:	mediaCleanup/src/main.c
*
*   Purpose:
*	Performs cleanup processing for a media unit.
*
*   Routines:
*	int		main		: Main routine.
*	static int      parseCmdLine	: Parses the command line.
*	static 	void	signalHandler	: Handles signals.
*
*   Date		: Dec 12, 1996
*
*   SCCS data		: @(#)
*	Module Name	: main.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmers		: Marc LeBlanc
*			: Steve Cockayne
*			: Shannon Jaeger
*
*   Modification History:
*	98/08/24 SDJ  	: Upgraded to new AM library. Remove AL library.
*	99/03/16 NRH  	: Added the rollback option.
*	99/04/13 NRH  	: Added signal handlers.
*	99/04/28 NRH  	: Changed added call to cleanup.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <signal.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"
#include "am.h"

#include "globals.h"

GLOBALS		globals;

static int	parseCmdLine( int, char ** );
static void	signalHandler( int );

/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main program for mediaCleanup.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		main
(
    int		argc,		/* (in)  Number of command line args.	*/
    char	*argv[]		/* (in)  The command line args.		*/
)
{
    int		status;		/* Function return status.		*/


    formatMessage( S_VERSION, version_short );


    /*
     *  Register signal handlers.
     */
    
    (void) signal( SIGINT, signalHandler );
    (void) signal( SIGTERM, signalHandler );


    /*
     *  Initialize the global variables.
     */

    CHECK( globalsInit( ) );


    /*
     *  Parse the command line.
     */

    CHECK( parseCmdLine( --argc, ++ argv ) );
    printMessage( msg, MESS_ERROR );


    /*
     *  Initialize.
     */

    CHECK( mcInit( ) );


    /*
     *  Perform mediaCleanup's processing.
     */


    if ( globals.gRollback )
    {
	status = rollback( );
    }
    else
    {
	status = process( );
    }

    CHECK( cleanup( ) );
    CHECK( status );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	parseCmdLine
*
*   Purpose:
*	Parses the command line.
*
*   Values Returned:
*	int	S_QUIT		: User set flag which causes quick quit.
*	int	S_SUCCESS	: Normal completion.
*	int	E_USAGE		: Usage error.
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
    boolean	minusHelp;	/* Has user set -help?			*/
    boolean	minusI;		/* Has user set -i?			*/
    boolean	minusN;		/* Has user set -n?			*/
    boolean	minusCapV;	/* Has user set -V? (uppercase)		*/
    boolean	minusLowV;	/* Has user set -v? (lowercase)		*/
    boolean	minusRollback;	/* Has user set -rollback?		*/
    int		status;


    minusHelp = FALSE;
    minusI = FALSE;
    minusN = FALSE;
    minusCapV = FALSE;
    minusLowV = FALSE;
    minusRollback = FALSE;

    for ( i = 0, status = S_SUCCESS; i < argc && status == S_SUCCESS; i++ )
    {
	if ( argv[i][0] == '-' )
	{
	    if ( streq( argv[i], "-i" ) && ! minusI )
	    {
		if ( ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusI = TRUE;
		    if ( !str2int( argv[i], &globals.gMediaId ) )
		    {
			status = E_USAGE;
		    }
		}
	    }
	    else if ( streq( argv[i], "-n" ) && ! minusN )
	    {
		if (  ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusN = TRUE;
		    (void) sprintf( globals.gMediaUnitName, "%s", argv[i] );
		}
	    }
	    else if ( streq( argv[i], "-rollback" ) && ! minusRollback )
	    {
		minusRollback = TRUE;
		globals.gRollback = TRUE;
	    }
	    else if ( streq( argv[i], "-v" ) && ! minusLowV )
	    {
		minusLowV = TRUE;
		globals.gVerbose = TRUE;
	    }
	    else if ( streq( argv[i], "-V" ) && ! minusCapV )
	    {
		minusCapV = TRUE;
		globals.gVersion = TRUE;
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
		    streq( argv[i], "-?" ) ||
		    streq( argv[i], "-h" ) ) && ! minusHelp )
	    {
		minusHelp = TRUE;
	    }
	    else
	    {
		status = E_USAGE;
	    }

	}
	else
	{
	    status = E_USAGE;
	}
    }


    /*
     *  If -V option was used, print long version message and exit.
     */
 
    if ( minusCapV )
    {
        formatMessage( S_VERSION, version_long );
        printMessage( msg, MESS_ERROR );
        status = S_QUIT;
    }
 
 
    /*
     *  If -help option was used, print usage message and exit.
     */
 
    if ( minusHelp )
    {
        formatMessage( S_USAGE, version_long );
        printMessage( msg, MESS_ERROR );
        status = S_QUIT;
    }


    if ( ( status != S_QUIT ) &&
	    ( ( status == E_USAGE ) ||
	    ( ! minusI ) ||
	    ( ! minusN ) ) )
    {
        formatMessage( E_USAGE );
        formatMessage( S_USAGE );
        printMessage( msg, MESS_ERROR );
        return( E_USAGE );
    }

    return( status );
}

/*+
************************************************************************
*
*   Function:	signalHandler
*
*   Purpose:
*	Handles signals.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	signalHandler
(
        int         sig             /* (in)  The signal sent.               */
)
{
    formatMessage( S_MC_SIGNAL );
    printMessage( msg, MESS_ERROR );

    dbcanquery( globals.gDbProc );
    exit( S_MC_SIGNAL );
}
