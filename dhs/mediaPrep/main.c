/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Module Name:	mediaPrep/src/main.c
*
*   Purpose:
*	mediaPrep performs the necessary processing inorder to prepare
*	a media for writing; creates a physical media.
*
*   Routines:
*	int		main		: Main routine.
*	static int 	parseCmdLine	: Parses the command line.
*	static void	signalHandler	: Handles signals.
*
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: main.c
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/09/09 SDJ	: Copied from cdPrep
*       98/12/06 SDJ  	: Added code in sig. handler to cancel db queries.
*	99/03/08 NRH	: Added support for rollback.
*	99/03/18 NRH	: Added automatic rollback if media prep fails.
*	99/03/25 SDJ	: Made sure mediPrep exits on a ^C.
*	99/04/01 NRH	: Moved the signalHandler call to dbcanquery to 
*			  before the call to rollback.
*	99/04/28 NRH	: Changed to call cleanup even if an error has
*			  occured.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


#include <sys/types.h>
#include <signal.h>
/*
 ************************************************************************
 *  SYBASE DB libarary include files.
 ************************************************************************
 */
 
#include <sybfront.h>
#include <sybdb.h>
 

/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */
 
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "am.h"
#include "db.h"

 
/*
 ************************************************************************
 *  Media prep include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 *  Globals
 ************************************************************************
 */
 
GLOBALS globals;
 
 
/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */
 
static int      parseCmdLine( int, char ** );
static void	signalHandler( int );
 


/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main program for mediaPrep.
*
*   Values Returned:
*	int	S_SUCCESS	: Successful completion.	
*
************************************************************************
-*/

int	main
( 
    int		argc,
    char	*argv[]
)
{
    int		status;


    formatMessage( S_VERSION, version_short );

    /*
     *  Register signal handler.
     */

    (void) signal( SIGINT, signalHandler );
    (void) signal( SIGTERM, signalHandler );


    /*
     *  Initialize global variables.
     */

    CHECK( globalsInit( ) );
 

    /*
     *  Parse command line.
     */

    CHECK( parseCmdLine( --argc, ++argv ) );
    printMessage( msg, MESS_ERROR );
 

    /*
     *  Initialize.
     */

    CHECK( mpInit( ) );


    if ( globals.gRollback )
    {
	/*
	 * Rollback.
	 */

	status = rollback( );
    }
    else
    {
	/*
	 *  Process.
	 */

	status = process( );
	if ( status != S_SUCCESS )
	{
	    rollback();
	}
    }


    /*
     *  Clean up, exit. 
     */

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
*       Reads in the values entered in the command line.  If any of
*       the values are invalid, the command line is rejected.
*
*   Values Returned:
*	int	S_QUIT		: Command-line parameter causes quick quit.
*       int 	S_SUCCESS	: All went well.
*       int 	E_USAGE		: Incorrect usage.
*
************************************************************************
-*/

static int 	parseCmdLine
(
    int		argc,		/* (in)  Argument count.		*/
    char	*argv[]		/* (in)  Argument values.		*/
)
{
    int         i;		/* loop counter.			*/
    boolean     minusCapV;	/* Has user set -V? (uppercase)		*/
    boolean	minusD;		/* Has user set -d?			*/
    boolean     minusHelp;	/* Has user set -help?			*/
    boolean	minusI;		/* Has user set -i?			*/
    boolean     minusLowV;	/* Has user set -v? (lowercase)		*/
    boolean	minusN;		/* Has user set -n?			*/
    boolean	minusRollback;	/* Has user set -rollback?		*/
    int		status;


    minusCapV = FALSE;
    minusD = FALSE;
    minusHelp = FALSE;
    minusI = FALSE;
    minusLowV = FALSE;
    minusN = FALSE;
    minusRollback = FALSE;


    /*
     *  Parse command line.
     */

    for ( i = 0, status = S_SUCCESS; i < argc && status == S_SUCCESS; i++ )
    {
        if ( argv[i][0] == '-' )
	{
	    if ( streq( argv[i], "-i" ) )
	    {
		if ( ( minusI ) || ( ++i > argc ) )
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
	    else if ( streq( argv[i], "-n" ) )
	    {
		if ( ( minusN ) || ( ++i > argc ) )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusN = TRUE;
		    (void) sprintf( globals.gMediaUnitName, "%s", argv[i] );
		}
	    }
	    else if ( streq( argv[i], "-d" ) )
	    {
		if ( ( minusD ) || ( ++i > argc ) )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusD = TRUE;
		    (void) sprintf( globals.gMDSDirectory, "%s", argv[i] );
		}
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
		    streq( argv[i], "-?" ) ||
		    streq( argv[i], "-h" ) ) && ! minusHelp )
	    {
		minusHelp = TRUE;
	    }
	    else if ( streq( argv[i], "-rollback" ) )
	    {
		minusRollback = TRUE;
		globals.gRollback = TRUE;
	    }
	    else if ( streq( argv[i], "-v" ) && ! minusLowV )
	    {
		globals.gVerbose = TRUE;
		minusLowV = TRUE;
	    }
	    else if ( streq( argv[i], "-V" ) && ! minusCapV )
	    {
		minusCapV = TRUE;
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


    /*
     *  Check that the required set of options was set. 
     */

    if ( (status != S_QUIT ) && 
	    ( ( status == E_USAGE ) ||
	    ( ! minusD && ! minusRollback ) ||
	    ( minusD && minusRollback ) ||
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
*   Function:   signalHandler
*
*   Purpose:
*       Handles signals.
*
*   Values Returned:
*       none
*
*   Deficiencies:
*	It is possible that the signal could arrive and the condition
*	that the flag is set to TRUE in between checking the flag and
*	forking and execing. This is unsafe. Condition variable should
*	really be added.
*
************************************************************************
-*/

void    signalHandler
(
    int         sig             /* (in)  The signal sent.               */
)
{
    formatMessage( S_MP_SIGNAL );
    printMessage( msg, MESS_ERROR );

    globals.gScriptKill = TRUE;

    if ( globals.gScriptProcessId != (pid_t)-1 )
    {
	(void) kill( globals.gScriptProcessId, SIGKILL );
    }

    dbcanquery( globals.gDbProc1 );

    rollback();
    
    exit( S_MP_SIGNAL );
}
