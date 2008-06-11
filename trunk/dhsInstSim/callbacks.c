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
*   Module Name:	sim/src/callbacks.c
*
*   Purpose:
*	Contains callback routines for sim.
*
*   Routines:
*	void	errCB	: Error callback to register with dhs lib.
*	void	respCB	: Command response callback to register with dhs.
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

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "dhs.h"

#include "globals.h"


/*+
************************************************************************
*
*   Function:	errCB
*
*   Purpose:
*	Handles dhs error callbacks. Prints the dhs message, and
*	increments the count of times this function has been called. 
*
*   Values Returned:
*	void
*
************************************************************************
-*/

void		errCB
(
    DHS_CONNECT	connect,	/* (in)  Connection causing the error.	*/
    DHS_STATUS	eNum,		/* (in)  Error number.			*/
    DHS_ERR_LEVEL
		eLev,		/* (in)  Error level.			*/
    char	*dhsMsg,	/* (in)  Error message string.		*/
    DHS_TAG	tag,		/* (in)  Command tag of server.		*/
    void	*userData	/* (in)  User data assoc'ed with error.	*/
)
{
    DHS_STATUS	dhsStatus;	/* Ret. status from dhs lib routine.	*/


    dhsStatus = DHS_S_SUCCESS;


    /*
     *  Print the message.
     */

    formatMessage( E_ERR_CALLBACK, dhsMsg );
    printMessage( msg, MESS_ERROR );
    dhsMessageClear( &dhsStatus );


    return;
}

/*+
************************************************************************
*
*   Function:	respCB
*
*   Purpose:
*	Handles command-response callbacks. 
*
*   Values Returned:
*	void
*
************************************************************************
-*/

void		respCB
(
    DHS_CONNECT	connect,	/* (in)  Connection causing the error.	*/
    DHS_TAG	cmdTag,		/* (in)  Command tag of server.		*/
    char	*cmdStr,	/* (in)  Command string.		*/
    DHS_CMD_STATUS	
		cmdStatus,	/* (in)  Current command status.	*/
    char	*cmdStatStr,	/* (in)  Description of the cmdStatus.	*/
    DHS_AV_LIST	avList,		/* (in)  Command response av list.	*/
    void	*userData	/* (in)  User data assoc'ed with error.	*/
)
{
    DHS_STATUS	dhsStatus;	/* Ret. status from dhs lib routine.	*/


    dhsStatus = DHS_S_SUCCESS;


    formatMessage( S_RESP_CALLBACK, cmdStr );
    printMessage( msg, MESS_ERROR );

    if ( dhsTagDone( cmdTag, &dhsStatus ) )
    {
	dhsStatus = DHS_S_SUCCESS;
	(void) dhsTagFree( cmdTag, &dhsStatus );
    }

    return;
}

/*+
************************************************************************
*
*   Function:	putCB
*
*   Purpose:
*	Handles put callbacks. 
*
*   Values Returned:
*	void
*
************************************************************************
-*/

void		putCB
(
    DHS_CONNECT	connect,	/* (in)  Connection causing the error.	*/
    DHS_TAG	cmdTag,		/* (in)  Command tag of server.		*/
    DHS_CMD_STATUS	
		cmdStatus,	/* (in)  Current command status.	*/
    char	*cmdStatStr,	/* (in)  Description of the cmdStatus.	*/
    char	*label,		/* (in)  Data label. 			*/
    void	*userData	/* (in)  User data assoc'ed with error.	*/
)
{
    DHS_STATUS	dhsStatus;	/* Ret. status from dhs lib routine.	*/
    EVENT	*event;


    dhsStatus = DHS_S_SUCCESS;
    event = (EVENT *) userData;


    formatMessage( S_RESP_CALLBACK, "put" );
    printMessage( msg, MESS_ERROR );

    if ( dhsTagDone( cmdTag, &dhsStatus ) )
    {
	dhsStatus = DHS_S_SUCCESS;
	(void) dhsTagFree( cmdTag, &dhsStatus );
	pthread_mutex_lock( &globals.gMutex );
	globals.gFilesSent--;
	pthread_cond_broadcast( &globals.gCond );
	pthread_mutex_unlock( &globals.gMutex );

	gen_free( event->fname );
	gen_free( event->dsname );
	gen_free( event->identity );
	gen_free( event );
    }

    return;
}
