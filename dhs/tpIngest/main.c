/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998				(c) 1998.
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
*   Module Name:	tpIngest/src/main.c
*
*   Purpose:
*	Contains the main, command line and usage functions for tpingest.
*
*   Routines:
*	int		main		: Main routine.
*	static int 	parse_cmd_line  : Parses the command line.
*
*   Date		: Feb 20, 1998
*
*   Field SCCS data	: @(#)
*	Module Name	: main.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 04/15/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/04/09 SDJ	: Removed print_usage(), added signal_handler().
*			  Added -help command-line arg(). 
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/types.h>
#include <sybfront.h>
#include <sybdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "ad.h"
#include "tp.h"
#include "globals.h"

GLOBALS	globals;

static int	parse_cmd_line( int, char ** );
static void	signal_handler( int );

/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main routine.
*
*   Values Returned:
*	int	TI_SUCCESS	: Successful completion.	
*	int	TI_QUIT		: Quit at users request.
*	int	TI_E_AD		: Error in the ad library.
*	int	TI_E_DB_LIB	: Error in the cadc db library.
*	int	TI_E_LOG	: Error opening the log file.
*       int 	TI_E_MEMORY	: Memory error.
*	int	TI_E_SYSTEM	: Error in a system call.
*       int 	TI_E_USAGE	: Incorrect usage.
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


    format_message( TI_VERSION, version_short );


    /*
     * Register signal handler.
     */

    (void) signal( SIGINT, signal_handler );
    (void) signal( SIGTERM, signal_handler );
 

    /*
     *  Initialize global variables.
     */

    TI_CHECK( init_globals() );
 

    /*
     *  Parse the command line.
     */

    TI_CHECK( parse_cmd_line( --argc, ++argv ) );


    if ( globals.g_version )
    {
	format_message( TI_VERSION, version_long );
	print_message( msg, MESS_ERROR, CR );
	return( TI_SUCCESS );
    }

    print_message( msg, MESS_ERROR, CR );
 

    /*
     *  Initialize the ad library. 
     */

    TI_CHECK_AD( ad_init() );
    msg_append( msg, ad_msg );
    msg_clear( ad_msg );
    print_message( msg, MESS_INFO, CR );

    TI_CHECK( archive_check() );

    
    /*
     *  Initialize the tp library. 
     */

    TI_CHECK_TP( tp_init() );
    msg_append( msg, tp_msg );
    msg_clear( tp_msg );
    print_message( msg, MESS_INFO, CR );
    
    TI_CHECK_TP( tp_script_get( globals.g_type, &globals.g_tp_script ) );


    /*
     *  Log to the database and initialize the resources necessary 
     *  to ingest the tape(s).
     *  messages handled by initDb.
     */

    if ( ( status = init_db() ) != TI_SUCCESS )
    {
	return( status );
    }

    /*
     *  Ingest!
     */

    status = ingest();
    
    return( status );
}

/*+
************************************************************************
*
*   Function:	parse_cmd_line
*
*   Purpose:
*       To read in the values entered in the command line.  If any of
*       the values are invalid, the command line is rejected.
*
*   Values Returned:
*       int 	TI_SUCCESS	: All went well.
*       int 	TI_E_MEMORY	: Memory error.
*       int 	TI_E_USAGE	: Incorrect usage.
*
************************************************************************
-*/

static int 	parse_cmd_line
(
    int		argc,		/* (in)  Argument count.		*/
    char	*argv[]		/* (in)  Argument values.		*/
)
{
    int         i;
    boolean	minus_a;		/* Has the user set -a?		*/
    boolean	minus_d;		/* Has the user set -d?		*/
    boolean	minus_help;		/* Has the user set -help?	*/
    boolean	minus_r;		/* Has the user set -r?		*/
    boolean	minus_t;		/* Has the user set -t?		*/
    boolean     minus_v;                /* Has the user set -v?         */
    boolean     minus_V;                /* Has the user set -V?         */
    int		status;


    minus_a = FALSE;
    minus_d = FALSE;
    minus_help = FALSE;
    minus_r = FALSE;
    minus_t = FALSE;
    minus_v = FALSE;
    minus_V = FALSE;
    status = TI_SUCCESS;


    /*
     *  Parse command line.
     */

    for( i = 0; status == TI_SUCCESS && i < argc ; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            /*
             *  Parameter flag.
             */

	    if ( streq( argv[i], "-a" ) )
	    {
		if ( minus_a || ( ++i > argc ) || argv[i][0] == '-' ||
			strlen( argv[i] ) > AD_ARCHNAME_LEN )
		{
		    status = TI_E_USAGE;
		}
		else
		{
		    (void) strcpy( globals.g_archive, argv[i] );
		    minus_a = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-d" ) )
	    {
		if ( minus_d || ( ++i > argc ) || argv[i][0] == '-' )
		{
		    status = TI_E_USAGE;
		}
		else
		{
		    TI_CHECK_NULL( globals.g_device = strsav( argv[i] ) );
		    minus_d = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-r" ) )
	    {
		if ( minus_r )
		{
		    status = TI_E_USAGE;
		}
		else
		{
		    globals.g_auto_reingest = TRUE;
		    minus_r = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-t" ) )
	    {
	    	if ( minus_t || ( ++i > argc ) || argv[i][0] == '-' )
		{
		    status = TI_E_USAGE;
		}
		else
		{
		    TI_CHECK_NULL( globals.g_type = strsav( argv[i] ) );
		    minus_t = TRUE;
	        }
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
                    streq( argv[i], "-?" ) ||
                    streq( argv[i], "-h" ) ) && ! minus_help )
	    {
		minus_help = TRUE;
	    }
	    else if ( streq( argv[i], "-v" ) )
            {
                if ( minus_v )
                {
		    status = TI_E_USAGE;
                }
                else
                {
		    globals.g_verbose = TRUE;
		    minus_v = TRUE;
		}
	    }
            else if ( streq( argv[i], "-V" ) )
            {
                if ( minus_V )
                {
		    status = TI_E_USAGE;
                }
                else
                {
		    globals.g_version = TRUE;
		    minus_V = TRUE;
		}
	    }
            else
            {
		status = TI_E_USAGE;
            }
        }
	else 
	{
	    status = TI_E_USAGE;
	}
    }

    
    /*
     * If -V was used then print the long vesrion message and exit.
     */

    if ( minus_V )
    {
	format_message( TI_VERSION, version_long );
	print_message( msg, MESS_ERROR, CR );
	status = TI_QUIT;
    }


    /*
     * If -help option was used, pring usage message and exit.
     */

    if ( minus_help )
    {
	format_message( TI_USAGE );
	print_message( msg, MESS_ERROR, CR );
	status = TI_QUIT;
    }


    /*
     * Check that the required set of options was set.
     */
    
    if ( status != TI_QUIT &&
	( status == TI_E_USAGE || ! minus_d || ! minus_t ) )
    {
	format_message( TI_E_USAGE );
	format_message( TI_USAGE );
	print_message( msg, MESS_ERROR, CR );
	status = TI_E_USAGE;
    }

    return( status );
}

/*+
************************************************************************
*
*   Function:	signal_handler
*
*   Purpose:
*	Handles signals
*
*   Values Returned:
*	none.
*
*   Deficiencies:
*       It is possible that the signal could arrive and the condition
*       that the flag is set to TRUE in between checking the flag and
*       forking and execing. This is unsafe. Condition variable should
*       really be added.
*
************************************************************************
-*/

void 	signal_handler
(
    int         sig             /* (in)  The signal sent.               */
)
{
    globals.g_kill = TRUE;
    
    format_message( TI_SIGNAL );
    print_message( msg, MESS_ERROR, CR );

    if ( globals.g_script_pid != (pid_t)-1 )
    {
	(void) kill( globals.g_script_pid, SIGKILL );
	(void) waitpid( globals.g_script_pid, NULL, 0 );
    }

    dbcanquery( globals.g_dbproc );

#ifdef OMIT    
    rollback();
#endif    
    
    exit( TI_SIGNAL );
}

