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
*   Module Name:	mediaQueue/src/main.c
*
*   Purpose:
*	Contains the main and command line functions for mediaQueue.
*
*   Routines:
*
*	static void	checkTruthArg	: Verify a "truth" argument.
*	int		main		: Main routine.
*	int 		parseArgs	: Parses the command line.
*       static void     signalHandler   : Handles signals.
*
*   Date		: Jan 28, 1998
*
*   SCCS data		: @(#)
*	Module Name	: main.c
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 02/03/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/12/06 SDJ	: Made sure all queries are cancelled when exiting.
*	99/03/05 NRH	: Added rollback option.
*	99/03/29 NRH	: Changed signal handling to exit on a signal.
*	99/04/29 NRH	: Changed to execute cleanup even if an error occurs.
*     2000/02/03 SDJ	: Added checkTruthArgs(), Made parseArgs() global.
*			  Updated parseArgs() so it could parse both
*			  command-line arguments and options from the
*			  database.  Added support for -size, -date, and
*			  -pack options/cmd-line args.
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
#include "am.h"


/*
 ************************************************************************
 *  Media Queue include files.
 ************************************************************************
 */

#include "globals.h"


/*
 ************************************************************************
 *  Globals
 ************************************************************************
 */

GLOBALS	globals;



/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */

static void	checkTruthArg( int, char *[], truth *, boolean *, int *);
static void     signalHandler( int );

/*+
************************************************************************
*
*   Function:	checkTruthArg
*
*   Purpose:
*	Check a "truth" command-line argument for correctness.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
************************************************************************
-*/

void	checkTruthArg
(
    int		index,		/* (in)  Index into the argument array.	*/
    char	*args[],	/* (in)	 Argument list.			*/
    truth	*var,		/* (mod) Truth var. where value is stored.*/
    boolean	*warning,	/* (mod) Display warning?		*/
    int		*status		/* (out) Function return status.	*/
)
{
    boolean	bool;		/* A boolean value.			*/
    
    status = S_SUCCESS;

    if ( ! str2bool( args[index], &bool ) )
    {
	*status = E_USAGE;
    }
    else if ( bool )
    {
	if ( *var == T_FALSE )
	{
	    *warning = TRUE;
	}
	else
	{
	    *var = T_TRUE;
	}
    }
    else if ( ! bool )
    {
	if ( *var == T_TRUE )
	{
	    *warning = TRUE;
	}
	else
	{
	    *var = T_FALSE;
	}
    }
}

/*+
************************************************************************
*
*   Function:	main
*
*   Purpose:
*	Main routine.
*
*   Values Returned:
*	int	S_NO_FILES	: Success, but no files ready to queue.
*	int	S_QUIT		: User set flag which causes quick quit.
*       int 	S_SUCCESS	: All went well.
*	int	E_AM		: Error in the CADC am library.
*	int	E_ARG_TYPE	: Invalid arg. parsing type given.
*	int	E_DATA_STAGE	: Error with the data stage. 
*	int	E_DB_LIB	: Error in the CADC db library.
*	int	E_DB_USAGE	: Incorrect options in database.
*	int	E_FILESIZE	: Actual file size doesn't match database.
*	int	E_FILE_TOBIG	: File is to big for media.
*	int	E_MEDIA_TYPE	: Media type not found. 
*	int	E_NO_MEDIA	: Media not found in table.
*	int	E_NO_UNITS	: There are no units to rollback.
*	int	E_ONLY_ONE	: Only one executable of this type allowed.
*	int	E_SELECT_ITEM	: Unable to select item.
*	int	E_SYSTEM	: Error in the operating system.
*	int	E_TREE_TYPE	: Bad tree type given to select items.
*	int	E_UNITNAME_FMT	: Error formatting new media unit name.
*       int 	E_USAGE		: Incorrect command-line arg usage.
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

    CHECK( parseArgs( CMD_LINE_ARGS, --argc, ++argv ) );
    printMessage( msg, MESS_INFO );
 

    /*
     *  Initialize.
     */

    status = mqInit( );


    /*
     *  Process.
     */

    if ( status == S_SUCCESS && globals.gRollback )
    {
	status = rollback( );
    }
    else if ( status == S_SUCCESS )
    {
	status = process( );
    }


    /*
     *  Clean up, exit. 
     */

    (void) cleanup( );
    CHECK( status );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	parseArgs
*
*   Purpose:
*       Reads in the values entered on the command line or the list of
*	"options" from the media_options table.  If any of
*       the values are invalid, the command line is rejected.  If the
*	database options and command-line options differ from the database
*	options.
*
*   Values Returned:
*	int	S_QUIT		: User set flag which causes quick quit.
*       int 	S_SUCCESS	: All went well.
*       int 	E_USAGE		: Incorrect command-line arg usage.
*	int	E_DB_USAGE	: Incorrect options in database.
*	int	E_ARG_TYPE	: Invalid arg. parsing type given.
*
************************************************************************
-*/

int 	parseArgs
(
    ARG_TYPE	argType,	/* (in)  Command-line or database args?	*/
    int		argc,		/* (in)  Argument count.		*/
    char	*argv[]		/* (in)  Argument values.		*/
)
{
    int         i;
    int		mediaId;	/* Tmp media ID.			*/
    boolean	minusForce;	/* Has user set -force?			*/
    boolean     minusHelp;	/* Has user set -help?			*/
    boolean	minusI;		/* Has user set -i?			*/
    boolean     minusCapV;	/* Has user set -V? (uppercase)		*/
    boolean	minusDate;	/* Has user set -nodate?                */
    boolean     minusLowV;	/* Has user set -v? (lowercase)		*/
    boolean	minusPack;	/* Has user set -nopack?		*/
    boolean	minusSize;	/* Has user set -nosize?                */
    boolean	minusNumUnits;	/* Has user set -c?			*/
    boolean	minusRollback;	/* Has the user set -rollback?		*/
    int		numUnits;	/* Number of units to queue.		*/
    int		status;
    boolean	warn;		/* Warn user about cmd-line and database*/
    				/* differences.				*/
    

    minusCapV 		= FALSE;
    minusDate		= FALSE;
    minusForce 		= FALSE;
    minusHelp 		= FALSE;
    minusI 		= FALSE;
    minusLowV 		= FALSE;
    minusPack		= FALSE;
    minusSize     	= FALSE;
    minusNumUnits 	= FALSE;
    minusRollback 	= FALSE;


    /*
     *  Parse command line.
     */
    
    for ( i = 0, status = S_SUCCESS, warn=FALSE;
	  i < argc && status == S_SUCCESS; i++ )
    {
	/*
	 * Keep track of the command line arguments given.
	 */
	
	if ( argType == CMD_LINE_ARGS )
	{
	    (void) sprintf( globals.gCmdLineArgs, "%s %s", globals.gCmdLineArgs,
		    argv[i] );
	}


	/*
	 * Determine which argument was given.
	 */
	
	if ( argv[i][0] == '-' )
	{
	    if ( streq( argv[i], "-date" ) && ( ! minusDate  ) )
	    {
		if ( ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    checkTruthArg( i, argv, &globals.gCheckDate, &warn,
			    &status );
		    if ( status == S_SUCCESS )
		    {
			if ( argType == CMD_LINE_ARGS )
			{
			    (void) sprintf( globals.gCmdLineArgs, "%s %s",
				    globals.gCmdLineArgs,
				    argv[i] );
			}
			minusDate = TRUE;
		    }
		}
	    }
	    else if ( ( streq( argv[i], "-force" ) ) && ( ! minusForce )
			    && argType != DATABASE_ARGS )
	    {
		minusForce = TRUE;
		globals.gForce = TRUE;
	    }
	    else if ( ( streq( argv[i], "-help" ) ||
		    streq( argv[i], "-?" ) ||
		    streq( argv[i], "-h" ) ) && ! minusHelp
		    && argType != DATABASE_ARGS ) 
	    {
		minusHelp = TRUE;
	    }
	    else if ( streq( argv[i], "-i" ) && ! minusI )
	    {
		if ( ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusI = TRUE;
		    if ( ! str2int( argv[i], &mediaId ) )
		    {
			status = E_USAGE;
		    }

		    if ( globals.gMediaId != -1 && globals.gMediaId != mediaId )
		    {
			warn = TRUE;
		    }

		    if ( argType == CMD_LINE_ARGS )
		    {
			(void) sprintf( globals.gCmdLineArgs, "%s %s",
				globals.gCmdLineArgs,
				argv[i] );
		    }
		    globals.gMediaId = mediaId;
		}
	    }
	    else if ( streq( argv[i], "-num_units" ) && ( ! minusNumUnits  ) )
	    {
		if ( ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    minusNumUnits = TRUE;
		    if ( ! str2int( argv[i], &numUnits ) || numUnits <= 0 )
		    {
			status = E_USAGE;
		    }

		    if ( globals.gNumMedia > 0 && globals.gNumMedia != numUnits )
		    {
			warn = TRUE;
		    }
		    if ( argType == CMD_LINE_ARGS )
		    {
			(void) sprintf( globals.gCmdLineArgs, "%s %s",
				globals.gCmdLineArgs,
				argv[i] );
		    }
		    globals.gNumMedia = numUnits;
		}
	    }
	    else if ( streq( argv[i], "-pack" ) && ( ! minusPack  ) )
	    {
		if ( ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    checkTruthArg( i, argv, &globals.gPack, &warn, &status );
		    if ( status == S_SUCCESS )
		    {
			if ( argType == CMD_LINE_ARGS )
			{
			    (void) sprintf( globals.gCmdLineArgs, "%s %s",
				    globals.gCmdLineArgs,
				    argv[i] );
			}
			minusPack = TRUE;
		    }
		}
	    }
	    else if ( streq( argv[i], "-rollback" ) && ! minusRollback
		&& argType != DATABASE_ARGS )
	    {
		globals.gRollback = TRUE;
		minusRollback = TRUE;
	    }
	    else if ( streq( argv[i], "-size" ) && ( ! minusSize  ) )
	    {
		if ( ++i >= argc )
		{
		    status = E_USAGE;
		}
		else
		{
		    checkTruthArg( i, argv, &globals.gCheckSize, &warn,
			    &status );
		    if ( status == S_SUCCESS )
		    {
			if ( argType == CMD_LINE_ARGS )
			{
			    (void) sprintf( globals.gCmdLineArgs, "%s %s",
				    globals.gCmdLineArgs,
				    argv[i] );
			}
			minusSize = TRUE;
		    }
		}
	    }
	    else if ( streq( argv[i], "-v" ) && ! minusLowV )
	    {
		globals.gVerbose = TRUE;
		minusLowV = TRUE;
	    }
	    else if ( streq( argv[i], "-V" ) && ! minusCapV
		    && argType != DATABASE_ARGS )
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


    if ( argType == CMD_LINE_ARGS )
    {
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
			( ! minusI ) ) )
	{
	    formatMessage( E_USAGE );
	    formatMessage( S_USAGE );
	    printMessage( msg, MESS_ERROR );
	    return( E_USAGE );
	}
    }
    else if ( argType == DATABASE_ARGS )
    {
	/*
	 * Assign default values to "truth" args.
	 */

	if ( globals.gCheckDate == T_MAYBE )
	{
	    globals.gCheckDate = T_TRUE;
	}

	if ( globals.gCheckSize == T_MAYBE )
	{
	    globals.gCheckSize = T_TRUE;
	}

	if ( globals.gPack == T_MAYBE )
	{
	    globals.gPack = T_TRUE;
	}


	/*
	 * Print warning if there were differences in the database
	 * and the command-line arguments.
	 */

	if ( warn )
	{
	    formatMessage( S_DIFF_ARGS, globals.gCmdLineArgs, globals.gDbArgs );
	    printMessage( msg, MESS_CON );
	}
	

	/*
	 * If there were errors parsing the command-line args then
	 * report them.
	 */

	if ( status != S_SUCCESS )
	{
	    formatMessage( E_DB_USAGE, globals.gDbArgs );
	    formatMessage( S_DB_USAGE );
	    printMessage( msg, MESS_ERROR );
	    return E_DB_USAGE;
	}
    }
    else
    {
	/*
	 * Error invalid argument parsing type.
	 */

	formatMessage( E_ARG_TYPE, argType );
	printMessage( msg, MESS_ERROR );
	return E_ARG_TYPE;
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
    formatMessage( S_MQ_SIGNAL );
    printMessage( msg, MESS_ERROR );

    dbcanquery( globals.gDbProc1 );
    
    (void) signal( sig, signalHandler );

    exit( 0 );
}
