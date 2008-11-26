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
*   Module Name:	cdIngest/src/main.c
*
*   Purpose:
*	Contains the main, command line and usage functions for cdingest.
*
*   Routines:
*	int		main		: Main routine.
*	static int 	parseCmdLine  	: Parses the command line.
*	static void	printUsage	: Prints a usage statement.
*
*   Date		: Dec 09, 1996
*
*   Field SCCS data	: @(#)
*	Module Name	: main.c
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 06/23/98
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/11 JSD  :	Fixed usage message.
*	97/02/19 JSD  :	Added archive to cmd line.
*	97/06/20 SEC  :	Fixed archive so uses default ad lib archive if 
*			not on cmd line, and fixed version flag. 
*	98/01/21 gz   : When ad_default_archive() returns NULL, changed 
*		        the msg from "Out of memory" to "No default 
*		        archive found.
*		        Fixed parseCmdLine function to handle when -a 
*		        option NO archive followed and -b option NO
*			path specified.
*		        Removed print vesion_long from paseCmdLine.
*	98/06/23 sjg  : Added use of archiveCheck function.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "ad.h"
#include "globals.h"

GLOBALS	globals;

static int	parseCmdLine( int, char ** );
static void	printUsage();


/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main routine.
*
*   Values Returned:
*	int	CI_SUCCESS	: Successful completion.	
*	int	CI_QUIT		: Quit at users request.
*	int	CI_E_AD		: Error in the ad library.
*	int	CI_E_DB_LIB	: Error in the cadc db library.
*	int	CI_E_LOG	: Error opening the log file.
*       int 	CI_E_MEMORY	: Memory error.
*	int	CI_E_PATH_LEN	: File path len exceeds archive table size.
*	int	CI_E_SYSTEM	: Error in a system call.
*       int 	CI_E_USAGE	: Incorrect usage.
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


    formatMessage( CI_VERSION, version_short );


    /*
     *  Initialize global variables.
     */

    CI_CHECK( initGlobals() );
 

    /*
     *  Parse the command line.
     */

    CI_CHECK( parseCmdLine( --argc, ++argv ) );


    if ( globals.gVersion )
    {
	formatMessage( CI_VERSION, version_long );
	printMessage( msg, MESS_ERROR, CR );
	return( CI_SUCCESS );
    }

    printMessage( msg, MESS_ERROR, CR );
 

    /*
     *  Initialize the ad library. 
     */

    CI_CHECK_AD( ad_init() );
    msg_append( msg, ad_msg );
    msg_clear( ad_msg );
    printMessage( msg, MESS_INFO, CR );


    /*
     *  Check the archive.
     */

    CI_CHECK( archiveCheck() );


    /*
     *  Log to the database and initialize the resources necessary 
     *  to ingest the disk(s).
     *  Messages handled by initDb.
     */

    if ( ( status = initDb() ) != CI_SUCCESS )
    {
	closeCdIngest();
	return( status );
    }


    /*
     *  Ingest!
     */

    status = ingest();
    closeCdIngest();

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
*       int 	CI_SUCCESS	: All went well.
*       int 	CI_E_MEMORY	: Memory error.
*       int 	CI_E_USAGE	: Incorrect usage.
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
    boolean	minus_r;		/* Has the user set -r?		*/
    boolean     minus_v;                /* Has the user set -v?         */
    boolean     minus_V;                /* Has the user set -V?         */
    char	*path;			/* Path on command line.	*/
    int		status;


    minus_a = FALSE;
    minus_b = FALSE;
    minus_r = FALSE;
    minus_v = FALSE;
    minus_V = FALSE;
    status = CI_SUCCESS;


    /*
     *  Parse command line.
     */

    i = 0;
    while ( status == CI_SUCCESS && i < argc )
    {
        if ( argv[i][0] == '-' )
        {
            /*
             *  Parameter flag.
             */

	    if ( streq( argv[i], "-a" ) )
	    {
		if ( minus_a || ( i + 1 == argc ) || 
			strlen( argv[i+1] ) > AD_ARCHNAME_LEN ||
			argv[i+1][0] == '-' )
		{
		    status = CI_E_USAGE;
		}
		else
		{
		    (void) strcpy( globals.gArchive, argv[++i] );
		    minus_a = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-b" ) )
	    {
		if ( minus_b )
		{
		    status = CI_E_USAGE;
		}
		else
		{
		    globals.gBatch = TRUE;
		    minus_b = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-r" ) )
	    {
		if ( minus_r )
		{
		    status = CI_E_USAGE;
		}
		else
		{
		    globals.gAllowReingest = TRUE;
		    minus_r = TRUE;
		}
	    }
	    else if ( streq( argv[i], "-v" ) )
            {
                if ( minus_v )
                {
		    status = CI_E_USAGE;
                }
                else
                {
		    globals.gVerbose = TRUE;
		    minus_v = TRUE;
		}
	    }
            else if ( streq( argv[i], "-V" ) )
            {
                if ( minus_V )
                {
		    status = CI_E_USAGE;
                }
                else
                {
		    globals.gVersion = TRUE;
		    minus_V = TRUE;
		}
	    }
            else
            {
		status = CI_E_USAGE;
            }
        }
        else
        {
	    if ( globals.gPathList == NULL )
	    {
		CI_CHECK_NULL( globals.gPathList = eptr_alloc( 5 ) );
	    }

	    CI_CHECK_NULL( path = strsav( argv[i] ) );
	    CI_CHECK_NULL( eptr_insert( globals.gPathList, 
		    eptrlen( globals.gPathList ), path ) );
        }

	i++;
    }

    if ( status == CI_E_USAGE ||
         globals.gBatch && ( globals.gPathList == NULL ) )
    {
	printUsage();
	return( CI_E_USAGE );
    }

    return( CI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	printUsage
*
*   Purpose:
*	Prints the usage message.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

static void	printUsage
(
    void
)
{
    (void) fprintf( stderr,
	"Usage: cdIngest [-a archive] [options] [path ...]\n"
	"                [-a]            - Archive to add cd to.\n"
	"                [-b]            - Run in batch mode.\n"
	"                                  paths must also be on cmd line.\n"
	"                [-r]            - Don't prompt on re-ingest.\n"
	"                [-v]            - Verbose mode.\n"
	"                [-V]            - Print version and exit.\n" );
}
