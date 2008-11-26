/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999.
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
*   Module Name:	mfsIngest/src/main.c
*
*   Purpose:
*	Contains the main, command line and usage functions for mfsingest.
*
*   Routines:
*	int		main		: Main routine.
*	static int 	parseCmdLine  	: Parses the command line.
*
*   Date		: Dec 09, 1996
*
*   SCCS data		: @(#)
*	Module Name	: main.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 05/24/00
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	99/03/10 SDJ	: Copied from cdIngest.
*     2000/03/27 SDJ    : Updated to new CADC software standards.  Switched
*                         from dvdIngest to mfsIngest.  Added signalHandler().
*                         Removed printUsage(). Moved initiliazation routines 
*			  into init.c.  Added -t cmd-line option.
*    2000/04/05 SDJ	: Added a check in parseCmdLine so that the
*			  archive, and volume type don't exceed the max.
*			  length.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

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
#include "db.h"
#include "ad.h"
#include "mfs.h"


/*
 ************************************************************************
 *  MFS Ingest include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 *  Global definitions
 ************************************************************************
 */ 

GLOBALS	globals;

/*
 ************************************************************************
 *  Static function prototypes.
 ************************************************************************
 */ 


static int	parseCmdLine( int, char ** );
static void	signalHandler( int );

/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main routine.
*
*   Values Returned:
*	int	S_SUCCESS	: Successful completion.	
*	int	S_QUIT		: Quit at users request.
*	int	E_AD		: Error in the ad library.
*	int	E_DB_LIB	: Error in the cadc db library.
*       int 	E_MEMORY	: Memory error.
*	int	E_PATH_LEN	: File path len exceeds archive table size.
*	int	E_SYSTEM	: Error in a system call.
*       int 	E_USAGE		: Incorrect usage.
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

    status = S_SUCCESS;
    formatMessage( S_VERSION, version_short );
    printMessage( msg, MESS_CON );
    msg_clear( msg );
    

    /*
     *  Initialize global variables.
     */

    CHECK( initGlobals() );
 


    /*
     *  Register signal handlers.
     */
    
    (void) signal( SIGINT, signalHandler );
    (void) signal( SIGTERM, signalHandler );

    
    /*
     *  Parse the command line.
     */

    CHECK( parseCmdLine( --argc, ++argv ) );


    /*
     * Initialize database, read config files, make connection to
     * database, and other initializations.
     */
    
    status = initMfsIngest();

    if ( status == S_SUCCESS )
    {
	/*
	 *  Ingest!
	 */

	status = ingest();
    }
    

    /*
     * Cleanup resources properly.
     */
    
    closeMfsIngest();

    return( status );
}

/*+
************************************************************************
*
*   Function:	parseCmdLine
*
*   Purpose:
*       To read in the values entered in the command line.  If any of
*       the values are invalid, the command line is rejected.
*
*   Values Returned:
*       int 	S_SUCCESS	: All went well.
*       int 	E_MEMORY	: Memory error.
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
    int         i;
    boolean	minus_a;		/* Has the user set -a?		*/
    boolean	minus_b;		/* Has the user set -b?		*/
    boolean	minus_help;		/* Has the user used -help?	*/
    boolean	minus_r;		/* Has the user set -r?		*/
    boolean	minus_t;		/* Has hte user set -t?		*/	
    boolean     minus_v;                /* Has the user set -v?         */
    boolean     minus_V;                /* Has the user set -V?         */
    char	*path;			/* Path on command line.	*/
    int		status;


    minus_a 	= FALSE;
    minus_b 	= FALSE;
    minus_help	= FALSE;
    minus_r 	= FALSE;
    minus_t	= FALSE;
    minus_v 	= FALSE;
    minus_V 	= FALSE;
    

    /*
     *  Parse command line.
     */

    for( i = 0, status = S_SUCCESS; status == S_SUCCESS && i < argc; i++ )
    {
        if ( argv[i][0] == '-' )
        {
            /*
             *  Parameter flag.
             */

	    if ( streq( argv[i], "-a" ) && ! minus_a )
	    {
		if ( ++i >= argc || strlen( argv[i] ) > AD_ARCHNAME_LEN )
		{
		    status = E_USAGE;
		}
		else
		{
		    (void) strcpy( globals.gArchive, argv[i] );
		    minus_a = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-b" ) && ! minus_b )
	    {
		globals.gBatch = TRUE;
		minus_b = TRUE;
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
                    streq( argv[i], "-?" ) ||
                    streq( argv[i], "-h" ) ) && ! minus_help )
            {
                minus_help = TRUE;
            }
	    else if ( streq( argv[i], "-r" ) && ! minus_r )
	    {
		globals.gAllowReingest = TRUE;
		minus_r = TRUE;
	    }
	    else if ( streq( argv[i], "-t" ) && ! minus_t )
	    {
		if ( ++i >= argc || strlen( argv[i] ) > AD_VOLUMETYPE_LEN )
		{
		    status = E_USAGE;
		}
		else
		{
		    (void) strcpy( globals.gVolumeType, argv[i] );
		    minus_t = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-v" ) && ! minus_v )
            {
                globals.gVerbose = TRUE;
		minus_v = TRUE;
	    }
            else if ( streq( argv[i], "-V" ) && ! minus_V )
            {
		globals.gVersion = TRUE;
		minus_V = TRUE;
	    }
            else
            {
		status = E_USAGE;
            }
        }
        else
        {
	    if ( globals.gPathList == NULL )
	    {
		CHECK_NULL( globals.gPathList = eptr_alloc( 5 ) );
	    }

	    CHECK_NULL( path = strsav( argv[i] ) );
	    CHECK_NULL( eptr_insert( globals.gPathList, 
		    eptrlen( globals.gPathList ), path ) );
        }
    }


    /*
     *  If -V option was used, print long version message and exit.
     */

    if ( minus_V )
    {
	formatMessage( S_VERSION, version_long );
	printMessage( msg, MESS_OPER );
	return( S_SUCCESS );
    }


    /*
     *  If -help option was used, print usage message and exit. 
     */ 
     
    if ( minus_help )
    {
        formatMessage( S_USAGE, version_long );
        printMessage( msg, MESS_OPER );
        status = S_QUIT;
    }


    if ( status != S_QUIT &&
            ( status == E_USAGE && ! minus_V  ) )
    {
        formatMessage( E_USAGE );
        formatMessage( S_USAGE );
        printMessage( msg, MESS_ERROR );
        status = E_USAGE;
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
*       It is possible that the signal could arrive and the condition
*       that the flag is set to TRUE in between checking the flag and
*       forking and execing. This is unsafe. Condition variable should
*       really be added.
*
************************************************************************
-*/

void    signalHandler
(
    int         sig             /* (in)  The signal sent.               */
)
{
    formatMessage( S_MFSI_SIGNAL );
    printMessage( msg, MESS_ERROR );

    if ( globals.gDbproc != NULL );
    {
	dbcanquery( globals.gDbproc );
    }
    
    closeMfsIngest();
    
    exit( S_MFSI_SIGNAL );
}
