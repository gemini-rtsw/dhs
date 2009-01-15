/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999
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
*   Module Name:	mfsOnline/src/main.c
*
*   Purpose:
*	Program to update the vds archive table to indicate that a mfs
*	is mounted or dismounted.
*
*   Routines:
*	int		main		: Main routine.
*	static int	parseCmdLine	: Parse the command line.
*
*   Date		: Feb 23, 1999
*
*   SCCS data           : @(#)
*       Module Name     : main.c
*       Version Number  : 1.3
*       Release Number  : 1
*       Last Updated    : 04/10/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/02/23 SDJ  	: Copied from cdOnline.
*	99/03/22 SDJ	: Cosmetic changes to match other executables.
*     2000/03/22 SDJ	: Switched from dvdOnline to mfsOnline.
*     2000/04/10 SDJ	: Added signalHandler().  Updated to latest CADC
*			  software standards.  Moved initialization items
*			  into there own function, initMfsOnline().
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
#include <time.h>


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
#include "ad.h"
#include "db.h"


/*
 ************************************************************************
 *  mfsOnline include files.
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
*	Main routine for program mfsOnline.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_AD		: Error in the ad library.
*	int	E_MFS		: Error in the mfs library.
*	int	E_DB_LIB	: Error in the cadc db library.
*	int	E_MEMORY	: Memory allocation failure.
*	int	E_USAGE	: Command line usage error.
*	int 	E_SYSTEM	: System call error.	
*
************************************************************************
-*/

int		main
(
    int		argc,		/* (in)  Number of command line args.	*/
    char	*argv[]		/* (in)  The command line args.		*/
)
{
    /*
     *  Initialize the global variables.
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
     * Initaliaze mfsOnline
     */

    CHECK( initMfsOnline( ) );



    /*
     * Mount/unmount the file syatem.
     */
    
    
    if ( globals.gUnmount )
    {	
	CHECK( mfsUnmount( globals.gTarget ) );
    }
    else
    {
	CHECK( mfsMount( globals.gTarget ) );
    }


    CHECK_DB_LIB( db_close( globals.gDbproc ) );
    CHECK_DB_LIB( db_exit() );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	parseCmdLine
*
*   Purpose:
*	Parse the command line for program mfsOnline.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_MEMORY	: Memory allocation failure.
*	int	E_USAGE	: Command line usage error.
*
************************************************************************
-*/

static int	parseCmdLine
(
    int		argc,		/* (in)  Number of arguments.		*/
    char	*argv[]		/* (in)  Command line args.		*/
)
{
    struct tm	create_tm;	/* Creation time of the volume.		*/
    char	day_str[30];	/* Day volume was created.		*/
    int		i;
    boolean	minus_a;	/* -a option used?			*/
    boolean	minus_d;	/* -d option used?			*/
    boolean	minus_help;	/* -help option used?			*/
    boolean	minus_t;	/* -t option used?			*/
    boolean	minus_u;	/* -u option used?			*/
    boolean	minus_v;	/* -v optoin used?			*/
    boolean	minus_V;	/* -V option used?			*/
    int		status;		/* Return status.			*/
    boolean	target;		/* Target disk/path set?		*/
    char	time_str[30];	/* Time volume was created.		*/

    

    day_str[0]	= '\0';
    time_str[0]	= '\0';
    minus_a	= FALSE;
    minus_d	= FALSE;
    minus_help 	= FALSE;
    minus_t	= FALSE;
    minus_u 	= FALSE;
    minus_v	= FALSE;
    minus_V 	= FALSE;
    target 	= FALSE;
    

    /*
     *  Parse command line.
     */

    for( i = 0,  status = S_SUCCESS; i < argc; i++ )
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
	    else if ( streq( argv[i], "-d" ) && ! minus_d )
	    {
		if ( ++i >= argc || i + 1 > argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    minus_d = TRUE;

		    
		    /*
		     * Parse the day, expected format YYYY/MM/DD
		     */
		    
		    (void) strcpy( day_str, argv[i++] );
		    day_str[4] = '\0';
		    day_str[7] = '\0';
		    if ( ! str2int( day_str, &(create_tm.tm_year) )
			    || ! str2int( day_str + 5, &(create_tm.tm_mon) )
			    || ! str2int( day_str + 8, &(create_tm.tm_mday ) ) )
		    {
			status = E_USAGE;
		    }
		    else
		    {
			create_tm.tm_year -= 1900;
		    }

		    
		    /*
		     * Parse the time of day, expected format HH:MM:SS
		     */
		    
		    (void) strcpy( time_str, argv[i] );
		    time_str[2] = '\0';
		    time_str[5] = '\0';
		    if ( ! str2int( time_str, &( create_tm.tm_hour ) )
			  || ! str2int( time_str + 3, &( create_tm.tm_min ) )
			  || ! str2int( time_str + 6, &( create_tm.tm_sec ) ) )
		    {
			status = E_USAGE;
		    }


		    /*
		     * Convert to a UNIX time.
		     */

		    if ( status != E_USAGE && ( globals.gCreateDate =
			    mktime( &create_tm ) ) < 0 )
		    {
			status = E_USAGE;
		    }
		}
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
                    streq( argv[i], "-?" ) ||
                    streq( argv[i], "-h" ) ) && ! minus_help )
            {
                minus_help = TRUE;
            }
	    else if ( streq( argv[i], "-u" ) && ! minus_u )
	    {
		minus_u = TRUE;
		globals.gUnmount = TRUE;
	    }
	    else if ( streq( argv[i], "-t" ) && ! minus_t )
	    {
		if ( ++i >= argc  || strlen( argv[i] ) > AD_VOLUMETYPE_LEN )
		{
		    status = E_USAGE;
		}
		else
		{
		    minus_t = TRUE;
		    (void) strcpy( globals.gVolumeType, argv[i] );
		}
	    }
	    else if ( streq( argv[i], "-v" ) && ! minus_v )
            {
                globals.gVerbose = TRUE;
		minus_v = TRUE;
	    }
	    else if ( streq( argv[i], "-V" ) && ! minus_V )
	    {
		minus_V = TRUE;
		globals.gVersion = TRUE;
	    }
	    else
	    {
		status = E_USAGE;
	    }
	}
	else
	{
	    if ( target )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		target = TRUE;
		CHECK_NULL( globals.gTarget = strsav( argv[i] ) );
	    }
	}
    }


    /*
     *  If -V option was used, print long version message and exit.
     */

    if ( globals.gVersion == TRUE )
    {
	formatMessage( S_VERSION, version_long );
	printMessage( msg, MESS_OPER );
	status = S_QUIT;
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


    if ( status != S_QUIT && (
	status == E_USAGE || 
	    ( ! minus_V && ! target ) ) )
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
    formatMessage( S_MFSO_SIGNAL );
    printMessage( msg, MESS_OPER );

    if ( globals.gDbproc != NULL );
    {
	dbcanquery( globals.gDbproc );
    }
    closeMfsOnline();
    
    exit( S_MFSO_SIGNAL );
}

