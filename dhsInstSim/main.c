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
*   Module Name:	sim/src/main.c
*
*   Purpose:
*	Contains the main and command line functions for sim.
*
*   Routines:
*	int		main		: Main routine.
*	static int 	parseCmdLine	: Parses the command line.
*
*   Date		: 
*
*   SCCS data		: 
*	Module Name	: 
*	Version Number	: 
*	Release Number	: 
*	Last Updated	:
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/time.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "dhs.h"

#include "globals.h"

GLOBALS	globals;

static int	parseCmdLine( int, char ** );


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
*
************************************************************************
-*/

int	main
( 
    int		argc,
    char	*argv[]
)
{
    int		status;		/* Return statu from process.		*/
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

    CHECK( simInit( ) );


    /*
     *  Process.
     */

    status = process( );


    /*
     *  Clean up, exit. 
     */

    CHECK( cleanup( ) );


    return( status );
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
*	int	S_QUIT		: User set flag which causes quick quit.
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
    int         i;
    boolean	minusID;		
    boolean	minusDS;		
    boolean	minusS;		
    boolean	minusQl;		
    boolean	minusDSIP;		
    boolean	minusDebug;	
    boolean	minusInput;	
    boolean     minusHelp;	
    boolean	minusNow;
    boolean	minusBuffs;
    int		status;
    int		strf_status;
    char	tempStr[256];
    time_t	timeVar;
    char	mode[15];


    minusID = FALSE;
    minusDS = FALSE;
    minusS = FALSE;
    minusQl = FALSE;
    minusDSIP = FALSE;
    minusDebug = FALSE;
    minusInput = FALSE;
    minusHelp = FALSE;
    minusBuffs = FALSE;
    minusNow = FALSE;


    /*
     *  Parse command line.
     */

    for ( i = 0, status = S_SUCCESS; i < argc && status == S_SUCCESS; i++ )
    {
	if ( streq( argv[i], "-?" ) )
	{
	    minusHelp = TRUE;
	}
	else if ( streq( argv[i], "-i" ) && !minusInput )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		globals.g_inputfile = strsav( argv[i] );
		minusInput = TRUE;
	    }
	}
	else if ( streq( argv[i], "-debug" ) && !minusDebug )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE; 
	    }
	    else
	    {
		(void) str2upper( mode, argv[i] );
		if ( streq( mode, "ON" ) )
		{
		    globals.g_debug_mode = DHS_DEBUG_ON;
		}
		else if ( streq( mode, "OFF" ) )
		{
		    globals.g_debug_mode = DHS_DEBUG_OFF;
		}
		else if ( streq( mode, "FULL" ) )
		{
		    globals.g_debug_mode = DHS_DEBUG_FULL;
		}
		else
		{
		    status = E_USAGE;
		}
		minusDebug = TRUE;
	    }
	}
	else if ( streq( argv[i], "-id" ) && !minusID  )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		globals.g_identity = strsav( argv[i] );
		minusID = TRUE;
	    }
	}
	else if ( streq( argv[i], "-ds" ) && !minusDS )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		globals.g_dataServer = strsav( argv[i] );
		minusDS = TRUE;
	    }
	}
	else if ( streq( argv[i], "-ip" ) && !minusDSIP )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		globals.g_dataServIP = strsav( argv[i] );
		minusDSIP = TRUE;
	    }
	}
	else if ( streq( argv[i], "-s" ) && !minusS )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		globals.gSynchTimeStr = strsav( argv[i] );
		minusS = TRUE;
	    }
	}
	else if ( streq( argv[i], "-ql" ) && !minusQl )
	{
		globals.gQlData = TRUE;
		minusQl = TRUE;
	}
	else if ( streq( argv[i], "-buffs" ) && !minusBuffs )
	{
	    if ( ++i > argc )
	    {
		status = E_USAGE;
	    }
	    else
	    {
		globals.gNumBuffers = atoi( argv[i] );
		minusBuffs = TRUE;
		if ( globals.gNumBuffers <= 0 )
		{
		    status = E_USAGE;
		}
	    }
	}
	else if ( streq( argv[i], "-now" ) && !minusNow )
	{
	    globals.gSynchNow = TRUE;
	    minusNow = TRUE;
	}
	else
	{
	    status = E_USAGE;
	}
    }


    /*
     *  If -help option was used, print usage message and exit. 
     */ 
     
    if ( minusHelp )
    {
	formatMessage( S_USAGE );
	printMessage( msg, MESS_ERROR );
	status = S_QUIT;
    }


    /*
     *  Check that the required set of options was set. 
     */

    if ( (status != S_QUIT ) && 
	    ( ( status == E_USAGE ) || ( !minusInput ) || ( !minusID ) || 
	      ( !minusDS ) || ( !minusDSIP ) || ( !minusNow && !minusS ) ||
	      ( minusNow && minusS ) ) )
    {
	formatMessage( E_USAGE );
	formatMessage( S_USAGE );
	printMessage( msg, MESS_ERROR );
	return( E_USAGE );
    }

    return( status );
}
