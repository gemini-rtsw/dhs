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
*   Module Name:	mediaWrite/src/main.c
*
*   Purpose:
*   Purpose:
*	mediaWrite writes CDs, DVDS, or tapes from a physical volume
*	on magnetic disk, normally prepared by mediaPrep.
*
*   Routines:
*	int		main		: Main routine.
*	static int 	parseCmdLine	: Parses the command line.
*     	static void     signalHandler   : Handles signals.
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: main.c
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmer		: Shannon jaeger
*
*   Modification History:
*      	98/09/09 SDJ    : Copied from cdWrite
*	98/10/20 SDJ	: Cosmetic changes.
*	98/10/22 SDJ  	: Script info from AM instead of config file.
*       98/11/23 SDJ  	: Made sure all pending queries are cancelled on
*                      	  start-up and shut-down.  Added device support.
*	99/03/08 NRH	: Added support for rollback and writing a specific
*			  number of copies of a unit.
*	99/03/25 SDJ	: Correctly saves the child process IDs now.
*	99/04/08 NRH	: Added the -simulate option.
*	99/04/28 NRH	: Changed to call cleanup even if an error occurs.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


#include <sys/types.h>
#include <sys/wait.h>
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
 *  Media Write include files.
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
 * Internal function prototypes
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
*	Main program for mediaWrite.
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
    int		status;		/* Function return status.		*/


    formatMessage( S_VERSION, version_short );


    /*
     *  Initialize global variables.
     */

    CHECK( globalsInit( ) );
 

    /*
     *  Register signal handlers.
     */
    
    (void) signal( SIGINT, signalHandler );
    (void) signal( SIGTERM, signalHandler );
    
    
    /*
     *  Parse command line.
     */

    CHECK( parseCmdLine( --argc, ++argv ) );
    printMessage( msg, MESS_ERROR );
 

    /*
     *  Initialize.
     */

    CHECK( mwInit( ) );


    /*
     *  Process.
     */

    if ( globals.gRollback )
    {
	status = rollback( );
    }
    else
    {
	status = writeMedia( );
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
    boolean	minusForce;	/* Has user set -force?			*/
    boolean     minusHelp;	/* Has user set -help?			*/
    boolean	minusI;		/* Has user set -i?			*/
    boolean	minusL;		/* Has user set -l?			*/
    boolean     minusLowV;	/* Has user set -v? (lowercase)		*/
    boolean	minusN;		/* Has user set -n?			*/
    boolean	minusNumCopies;	/* Has user set -num_copies?		*/
    boolean	minusRollback;	/* Has user set -rollback?		*/
    boolean	minusSimulate;	/* Has user set -simulate?		*/
    int		status;


    minusCapV = FALSE;
    minusForce = FALSE;
    minusHelp = FALSE;
    minusI = FALSE;
    minusL = FALSE;
    minusLowV = FALSE;
    minusN = FALSE;
    minusRollback = FALSE;
    minusSimulate = FALSE;
    minusNumCopies = FALSE;


    /*
     *  Parse command line.
     */

    for ( i = 0, status = S_SUCCESS; i < argc && status == S_SUCCESS; i++ )
    {
        if ( argv[i][0] == '-' )
	{
	    if ( streq( argv[i], "-force" ) && ! minusForce )
            {
                globals.gForce = TRUE;
                minusForce = TRUE;
            }
	    else if ( streq( argv[i], "-i" ) )
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
	    else if ( streq( argv[i], "-l" ) )
	    {
		if ( ( minusL ) || ( ++i > argc ) )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusL = TRUE;
		    (void) sprintf( globals.gLogicalDeviceName, "%s", argv[i] );
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
	    else if ( streq( argv[i], "-num_copies" ) )
	    {
		if ( ( minusNumCopies ) || ( ++i > argc ) )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusNumCopies = TRUE;
		    if ( ! str2int( argv[i], &globals.gNumCopies ) )
		    {
			status = E_USAGE;
		    }
		}
	    }
	    else if ( streq( argv[i], "-rollback" ) && ! minusRollback )
	    {
		globals.gRollback = TRUE;
		minusRollback = TRUE;
	    }
	    else if ( streq( argv[i], "-simulate" ) && ! minusSimulate )
	    {
		globals.gSimulate = TRUE;
		minusSimulate = TRUE;
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
		    streq( argv[i], "-?" ) ||
		    streq( argv[i], "-h" ) ) && ! minusHelp )
	    {
		minusHelp = TRUE;
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

    if ( ( status != S_QUIT ) && 
	    ( ( status == E_USAGE ) ||
	    ( ! minusI ) ||
	    ( ! minusN  ) ) ) 
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
*     It is possible that the signal could arrive and the condition
*     that the flag is set to TRUE in between checking the flag and
*     forking and execing. This is unsafe. Condition variable should
*     really be added.
*
************************************************************************
-*/

void    signalHandler
(
    int         sig             /* (in)  The signal sent.               */
)
{
    AM_DEVICE	*amDevicePtr;	/* AM library device structure.		*/
    int		execStatus;	/* execute status of the exec.		*/
    int 	index;		/* eptr index.				*/
    pid_t	*pidPtr;	/* Process ID pointer.			*/

    
    formatMessage( S_MW_SIGNAL );
    printMessage( msg, MESS_ERROR );

    globals.gScriptKill = TRUE;

    for( index = eptrlen( globals.gPidList ); index >= 0; index-- )
    {
	pidPtr = eptritem( globals.gPidList, index );
	if ( pidPtr != NULL )
	{
	    (void)kill( *pidPtr, SIGKILL );
	    (void)waitpid( *pidPtr, &execStatus, 0 );
	    if ( errno != 0 )
	    {
		formatMessage( MSG_ERRNO, "waitpid()" );
		printMessage( msg, MESS_ERROR );
	    }
	}
    }
    

    /*
     *  Release all of the devices.
     */

    for( index = eptrlen( globals.gDeviceList ) - 1; index >= 0; index -- )
    {
	if ( ( amDevicePtr = eptritem( globals.gDeviceList, index ) ) != NULL )
	{
	    if( ( amDeviceClear( AM_DC_DEVICE, globals.gDbProc1, 
		amDevicePtr->amdDeviceName )  )!= AM_SUCCESS )
	    {
		formatMessage( E_AM );
		msg_append( msg, amMsg );
		msg_clear( amMsg );
		printMessage( msg, MESS_ERROR );
	    }
	}
	else
	{
	    formatMessage( E_MEMORY );
	    printMessage( msg, MESS_ERROR );
	}
    }
    
    dbcanquery( globals.gDbProc1 );
    exit( S_MW_SIGNAL );
}
