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
*   Module Name:	sim/src/init.c
*
*   Purpose:
*	Contains initialization routines for thes sim application.
*
*   Routines:
*	int		globalsInit	: Initializes sim globals.
*	int		simInit		: Initializes sim.
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
#include <pthread.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "gen_msg.h"

#include "dhs.h"

#include "globals.h"



/*+
************************************************************************
*
*   Function:	globalsInit
*
*   Purpose:
*	Initialize sim's global variables.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		globalsInit
(
    void
)
{
    globals.g_log_file		= NULL;
    globals.g_log_fp		= NULL;
    globals.g_inputfile         = NULL;
    globals.g_identity          = NULL;
    globals.g_dataServer        = NULL;
    globals.g_dataServIP        = NULL;
    globals.g_connect           = NULL;
    globals.gSimList		= NULL;
    globals.gSynchTimeStr	= NULL;

    globals.gSynchNow		= FALSE;
    globals.gQlData		= FALSE;

    globals.gSynchTime		= 0;
    globals.gFilesSent		= 0;
    globals.gNumBuffers		= 2;

    CHECK( pthread_mutex_init( &globals.gMutex, NULL ) );
    CHECK( pthread_cond_init( &globals.gCond, NULL ) );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	simInit
*
*   Purpose:
*	Initialization for sim.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		simInit
(
    void
)
{
    DHS_STATUS	dhsStatus;
    DHS_THREAD	threadId;


    dhsStatus = DHS_S_SUCCESS;


    /*
     *  Open log file. 
     */
    
    CHECK_NULL( globals.g_log_file = strsav( "Simulator.log" ) );
    if ( ( globals.g_log_fp = fopen( globals.g_log_file, "a" ) ) == NULL )
    {
	formatMessage( MSG_ERRNO, globals.g_log_file );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }


    /*
     *  Initialize DHS library.
     */

    CHECK_DHS( dhsInit( globals.g_identity, 2,
	    &dhsStatus ), dhsStatus );
    

    /*
     *  Set up callbacks.
     */

    CHECK_DHS( dhsCallbackSet( DHS_CBT_ERROR, errCB, 
	    &dhsStatus ), dhsStatus );
    CHECK_DHS( dhsCallbackSet( DHS_CBT_RESPONSE, respCB,
	    &dhsStatus ), dhsStatus );
    CHECK_DHS( dhsCallbackSet( DHS_CBT_PUT, putCB,
	    &dhsStatus ), dhsStatus );
    

    /*
     *  Set the debug level and start the event loop in a separate thread.
     */

    (void) dhsDebugLevel( globals.g_debug_mode, &dhsStatus );
    CHECK_DHS( dhsEventLoop( DHS_ELT_THREADED, &threadId, 
	    &dhsStatus ), dhsStatus );


    /*
     *  Open connection to Data Server. 
     */

    CHECK_DHS( globals.g_connect = dhsConnect( globals.g_dataServIP,
	    globals.g_dataServer, NULL, 
	    &dhsStatus ), dhsStatus );

 
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	cleanup
*
*   Purpose:
*	Cleanup for sim.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		cleanup
(
    void
)
{
    DHS_STATUS	dhsStatus;
    boolean	done;


    dhsStatus = DHS_S_SUCCESS;
    done = FALSE;


    /*
     *  Wait until all callbax have been called. 
     */

    while ( !done )
    {
	pthread_mutex_lock( &globals.gMutex );
	if ( globals.gFilesSent == 0 )
	{
	    done = TRUE;
	    pthread_mutex_unlock( &globals.gMutex );
	}
	else
	{
	    pthread_cond_wait( &globals.gCond, &globals.gMutex );
	    pthread_mutex_unlock( &globals.gMutex );
	}
    }

    
    /*
     *  Shut down the dhs event loop. 
     */

    CHECK_DHS( dhsExit( &dhsStatus ), dhsStatus );


    return( S_SUCCESS );
}
