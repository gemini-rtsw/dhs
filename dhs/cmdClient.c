static char rcsid[] = "$Id: cmdClient.c,v 1.1.1.1 2002-11-24 20:19:57 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1996				(c) 1996.
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
 * All rights reserved			Tous droits reserves
 * 					
 * NRC disclaims any warranties,	Le CNRC denie toute garantie
 * expressed, implied, or statu-	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.	fortuit, resultant de l'utili-
 * 					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * dhs/src/cmdClient.c
 *
 * PURPOSE:
 * Contains DHS library command interface routines used primarily by DHS clients.
 *
 * FUNCTION NAME(S)
 * dhsApply - Send a command to a server.
 * dhsResponseGet - Get a specific response from a tag.
 * dhsStatus - Return the state of a command.
 * dhsTagDone - Return TRUE if the specified tag is complete.
 * dhsTagFree - Free the resources allocated to a tag.
 * dhsUserDataGet - Get the user data pointer assigned to a tag.
 * dhsUserDataSet - Set the user data pointer assigned to a tag.
 * dhsWait - Wait for a list of tags to reach an end state.
 * tagsDone - Return true when all tags in a list reach an end state.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.26  2001/03/27 20:25:16  nhill
 * Changed dhsUserDataGet to always return a value.
 * Removed unused variables.
 *
 * Revision 1.25  1999/02/01 21:04:32  nhill
 * Added checking that the tag passed in is valid.
 *
 * Revision 1.24  1998/08/04 16:13:13  nhill
 * Rearranged to code to be more object-oriented.
 *
 * Revision 1.23  1998/06/28 22:17:37  nhill
 * Fixed a bug that could result in a connection being left locked after
 * a call to dhsDisconnect.
 *
 * Revision 1.22  1998/06/26 17:38:43  nhill
 * Bug fix.
 *
 * Revision 1.21  1998/06/16 21:53:33  nhill
 * *** empty log message ***
 *
 * Revision 1.20  1998/05/13 22:30:38  nhill
 * Changed to ensure a dhsWait will not complete until after the callback
 * has been executed.
 *
 * Revision 1.19  1998/03/05 23:31:50  nhill
 * Changed to prevent usage of freed connect structures while
 * connections are being closed.
 *
 * Revision 1.18  1998/02/24 01:05:06  nhill
 * Bug fixes.
 *
 * Revision 1.17  1998/02/16 23:41:36  nhill
 * Added function dhsConUserDataSet and dhsUserDataSet to set user data
 * pointers.
 * Changed to use dhsConnectUnlock to unlock the connection mutex.
 *
 * Revision 1.16  1998/01/20 18:22:01  nhill
 * Added function dhsIsConnected.
 *
 * Revision 1.15  1997/12/18 21:50:20  nhill
 * Made changes required to allow automatic reconnection attempts after
 * a server crash.
 *
 * Revision 1.14  1997/09/26 18:00:43  nhill
 * Added a wait for a connection to close after a disconnnect.
 *
 * Revision 1.13  1997/09/17 20:34:28  nhill
 * Added mutex locks around some IMP functions.
 * Changed the wait function to wait for all callbacks for the command
 * to complete.
 *
 * Revision 1.12  1997/04/15 19:43:42  nhill
 * Changed to allow duplicate connections to a server, with the first
 * connection returned for all subsequence connections.
 *
 * Revision 1.11  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.10  1997/02/04 20:22:55  nhill
 * Lint fixes.
 *
 * Revision 1.9  1997/02/04 19:41:34  nhill
 * Checkpoint for install.
 *
 * Revision 1.8  1997/01/28 18:59:50  nhill
 * Checking before install.
 *
 * Revision 1.7  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.5  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.4  1997/01/20 17:15:11  nhill
 * Checkpoint.
 *
 * Revision 1.3  1997/01/14 20:25:07  nhill
 * Stable Check in.
 *
 * Revision 1.2  1997/01/13 17:58:34  nhill
 * Reasonably stable checkpoint.
 *
 * Revision 1.1  1997/01/06 17:24:10  nhill
 * Initial revision
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdarg.h>
#include <stdlib.h>


/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"


/*
 ************************************************************************
 *  Local structure declarations.
 ************************************************************************
 */


/*
 *  Create a structure to contain a list of tags. This structure only 
 *  exists so a list can be passed to the tagsDone function as a single
 *  argument.  Only a single argument is possible since the tagsDone
 *  function is used is used as a condition testing function passed to
 *  the function dhsEventLoop.
 */

typedef struct tagStruct
{
    int		tsNumTags;	/* Number of tags in the list.		*/
    const DHS_TAG	
		*tsTags;	/* The list of tags.			*/
} tTagStruct;


/*
 ************************************************************************
 *  Local function declarations.
 ************************************************************************
 */

static boolean	tagsDone( tTagStruct * );

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsApply
 *
 * INVOCATION:
 * tag = dhsApply( connection, command, avList, pUserData, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) The connection to send the command to.
 * (>) command (const char *) The command string.
 * (>) avList (DHS_AV_LIST) Attribute value list for the command.
 * (>) pUserData (void *) User data pointer for the command.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_TAG) Tag created to identify the command.
 *
 * PURPOSE:
 * Send a command to a server
 *
 * DESCRIPTION:
 * Create a new command structure and then send the command. This function
 * waits until the command is no longer in the DHS_CS_PENDING state before
 * it returns. The command structure tag is returned.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The connection must be opened with the dhsConnect function.
 * The atribute value list must be created with the dhsAvListNew function
 * (unless it is the empty list, DHS_AV_LIST_NULL).
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_TAG		dhsApply
(
    DHS_CONNECT	connection,	/* (in)  Server Connection.		*/
    const char	*command,	/* (in)  The command name.		*/
    DHS_AV_LIST	avList,		/* (in)  AV list for the command.	*/
    void	*pUserData,	/* (in)  User data pointer.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* Tag for the new command.		*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    DHS_STATUS	s2;		/* Temporary function return status.	*/


    fnEntry( "dhsApply" );
    statCheck( *pStatus, DHS_TAG_NULL );
    isInit( *pStatus, DHS_TAG_NULL );


    /*
     *  Create a new command structure.
     */

    check( pCmd = dhsCmdNew( command, avList, pUserData, DHS_MSG_APPLY, 
	    DHS_CF_CALLBACK, DHS_TAG_NULL, DHS_CS_PENDING, pStatus ), 
	    *pStatus, DHS_TAG_NULL );


    s = DHS_S_SUCCESS;


    /*
     *  Increase the reference counter for the command so it doesn't get
     *  deleted by a callback function before the status is checked.
     */

    dhsCmdRef( pCmd, &s );


    /*
     *  Send the command to the server.
     */

    s2 = DHS_S_SUCCESS;
    dhsConnectLock( dhsConnectPtr( connection ), &s );
    dhsConnectSend( dhsConnectPtr( connection ), pCmd, &s );
    if ( s != DHS_E_CON_NOT_FOUND )
    {
	dhsConnectUnlock( dhsConnectPtr( connection ), &s2 );
    }
    if ( s != DHS_S_SUCCESS )
    {
	/*
	 *  The command failed so delete it.
	 */

	check( dhsCmdDelete( pCmd, pStatus ), *pStatus, DHS_TAG_NULL );
	*pStatus = s;
	fnReturn( DHS_TAG_NULL );
    }

    /*
     *  Wait until the command leaves the DHS_CS_PENDING state.
     */

    check( dhsEventLoop( DHS_ELT_COND, dhsCmdStart, pCmd, pStatus ), 
	    *pStatus, DHS_TAG_NULL );
    

    /*
     *  Decrement the reference counter.  It is possible the command
     *  structure will be deleted here if the command was completed and
     *  deleted by a callback function between the call to dhsConnectSend
     *  and the call the dhsCmdDeref.
     */

    dhsCmdDeref( pCmd, &s );

    fnReturn( dhsCmdTag( pCmd ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsResponseGet
 *
 * INVOCATION:
 * avList = dhsResponseGet( tag, i, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) The command tag to query.
 * (>) respNum (int) The response number to return.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_LIST) Attribute value list send for the specified response.
 *
 * PURPOSE:
 * Get the attribute value list send with a specific response to a command
 *
 * DESCRIPTION:
 * Find the appropriate response in the list of responses and return it.
 *
 * If the response number requested is less than zero, the latest response
 * is returned.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tag must be returned by the dhsApply function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_AV_LIST	dhsResponseGet
(
    DHS_TAG	tag,		/* (in)  Tag to search.			*/
    int		respNum,	/* (in)  Index of the response.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* Response av list.			*/
    tDhsCmd	*pCmd;		/* Command pointer.			*/
    DHS_STATUS	s;		/* Temporary return status.		*/


    fnEntry( "dhsResponseGet" );
    statCheck( *pStatus, DHS_AV_LIST_NULL );
    isInit( *pStatus, DHS_AV_LIST_NULL );


    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tag ) );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	fnReturn( DHS_AV_LIST_NULL );
    }


    /*
     *  Lock the mutex so another thread can't insert new responses
     *  while the list is being examined.
     */

    dhsMutexLock( &( pCmd->dcMutex ), pStatus );
    if ( respNum < 0 )
    {
	respNum = pCmd->dcNumResp - 1;
    }


    if ( respNum < 0 || respNum >= pCmd->dcNumResp )
    {
	avList = DHS_AV_LIST_NULL;
	*pStatus = DHS_S_NO_RESP;
    }
    else
    {
	avList = pCmd->dcResponse[respNum];
    }


    /*
     *  Unlock the mutex.
     */

    s = DHS_S_SUCCESS;
    dhsMutexUnlock( &( pCmd->dcMutex ), &s );
    if ( s != DHS_S_SUCCESS )
    {
	*pStatus = s;
    }

    fnReturn( avList );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsStatus
 *
 * INVOCATION:
 * char *string;
 * cmdState = dhsStatus( tag, &string, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) The tag to query.
 * (!) pString (char **) Status string associated with the tag.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_CMD_STATUS) Current state of the command.
 *
 * PURPOSE:
 * Return the current state of a command
 *
 * DESCRIPTION:
 * Get the state and state string from the command structure and return
 * them.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The dhsLibrary must be initialized with the dhsInit function.
 * The tag must be supplied by one of the other DHS library function
 * calls.
 *
 * DEFICIENCIES:
 * It doesn't check to make sure the tag hasn't been freed.
 *-
 ************************************************************************
 */

DHS_CMD_STATUS	dhsStatus
(
    DHS_TAG	tag,		/* (in)  The tag to search.		*/
    char	**string,	/* (out) The string associated with the */
    				/*	 tag.				*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;		/* The command structure.		*/

    fnEntry( "dhsStatus" );

    statCheck( *pStatus, DHS_CS_PENDING );
    isInit( *pStatus, DHS_CS_PENDING );


    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tag ) );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	fnReturn( DHS_CS_ERROR );
    }
    
    if ( string != NULL )
    {
	*string = pCmd->dcString;
    }

    fnReturn( pCmd->dcState );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTagDone
 *
 * INVOCATION:
 * dhsTagDone( tag, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) Tag to check.
 * (!) pStatus (DHS_STATUS *) Functon return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Check to see if a tag is in one of the end states
 *
 * DESCRIPTION:
 * Execute the dhsCmdDone function
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tag must be returned by one of the other DHS library functions.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_BOOLEAN	dhsTagDone
(
    DHS_TAG	tag,		/* (in)  The tag to free.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;


    fnEntry( "dhsTagDone" );

    statCheck( *pStatus, DHS_FALSE );
    isInit( *pStatus, DHS_FALSE );

    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tag ) );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	fnReturn( DHS_FALSE );
    }

    fnReturn( dhsCmdDone( pCmd, pStatus ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsTagFree
 *
 * INVOCATION:
 * dhsTagFree( tag, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) Tag to free.
 * (!) pStatus (DHS_STATUS *) Functon return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Free a tag and all resources allocated to it
 *
 * DESCRIPTION:
 * Free the command structure associated with the tag.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tag must be returned by one of the other DHS library functions.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsTagFree
(
    DHS_TAG	tag,		/* (in)  The tag to free.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;


    fnEntry( "dhsTagFree" );

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );

    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tag ) );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	fnReturn( VOID );
    }

    dhsCmdDeref( pCmd, pStatus );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsUserDataGet
 *
 * INVOCATION:
 * pUserData = dhsUserDataGet( tag, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) Tag to query.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (void *) User data pointer of the tag.
 *
 * PURPOSE:
 * Get the user data pointer associated with a tag
 *
 * DESCRIPTION:
 * Return the user data pointer stored in the command structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tag must be returned by on of the other DHS library functions.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		*dhsUserDataGet
(
    DHS_TAG	tag,		/* (in)  The tag to search.		*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    void	*ptr;
    tDhsCmd	*pCmd;


    fnEntry( "dhsUserDataGet" );

    statCheck( *pStatus, NULL );
    isInit( *pStatus, NULL );

    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tag ) );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	fnReturn( NULL );
    }
    
    dhsCmdRef( pCmd, pStatus );
    dhsMutexLock( &( pCmd->dcMutex ), pStatus );
    ptr = pCmd->dcUserData;
    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );
    dhsCmdDeref( pCmd, pStatus );
    fnReturn( ptr );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsUserDataSet
 *
 * INVOCATION:
 * dhsUserDataSet( tag, pUserData, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) Tag to query.
 * (>) pUserData (void *) The new user data pointer.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Set the user data pointer associated with a tag
 *
 * This function should be used with caution. The dhsApply/dhsPut/dhsGet
 * functions should be used to initialize the user data pointer, since
 * it is possible for there to be a command response between the apply/get/put
 * and the call to this function.
 *
 * DESCRIPTION:
 * Set the user data pointer stored in the command structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tag must be returned by on of the other DHS library functions.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsUserDataSet
(
    DHS_TAG	tag,		/* (in)  The tag to search.		*/
    void	*pUserData,	/* (in)  The new user data pointer.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    tDhsCmd	*pCmd;


    fnEntry( "dhsUserDataGet" );

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );

    pCmd = dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tag ) );
    if ( *pStatus != DHS_S_SUCCESS )
    {
	dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tag );
	fnReturn( VOID );
    }

    dhsCmdRef( pCmd, pStatus );
    dhsMutexLock( &( pCmd->dcMutex ), pStatus );
    pCmd->dcUserData = pUserData;
    dhsMutexUnlock( &( pCmd->dcMutex ), pStatus );
    dhsCmdDeref( pCmd, pStatus );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsWait
 *
 * INVOCATION:
 * int		numTags;
 * DHS_TAG	tags[5];
 * dhsWait( numTags, tags, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) numTags (int) Number of tags in the list.
 * (>) tagList (DHS_TAG[]) List of tags to check.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Wait until all commands listed in the tag list have completed or failed
 *
 * DESCRIPTION:
 * Use the conditional form of the event loop to either run the event loop
 * until the condition is true, or let an existing event loop run and 
 * wait for the condition be become true.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tags must be returned by other DHS library functions.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsWait
(
    int		numTags,	/* (in)  Number of tags in the list.	*/
    const DHS_TAG		/* (in)  List of tags to wait for.	*/
		*tagList,
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    int		i;
    tTagStruct	tagStruct;	/* List of tags.			*/


    fnEntry( "dhsWait" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Set up a tTagStruct so all information about the tag struct
     *  can be passed to function tagsDone in its single argument.
     */

    tagStruct.tsTags = tagList;
    tagStruct.tsNumTags = numTags;


    /*
     *  Check that all of the tags exist.
     */

    for ( i = 0; i < numTags; i++ )
    {
	dhsCmdFind( DHS_CF_PTR, pStatus, dhsCmdPtr( tagList[i] ) );
	if ( *pStatus != DHS_S_SUCCESS )
	{
	    dhsMsgFmt( DHS_E_CMD_NOT_FOUND, DHS_DEBUG_OFF, tagList[i] );
	    fnReturn( VOID );
	}
    }


    /*
     *  Wait until function tagsDone returns true.
     */
    
    for ( i = 0; i < numTags; i++ )
    {
	dhsCmdRef( dhsCmdPtr( tagList[i] ), pStatus );
    }

    dhsEventLoop( DHS_ELT_COND, tagsDone, &tagStruct, pStatus );

    for ( i = 0; i < numTags; i++ )
    {
	dhsCmdDeref( dhsCmdPtr( tagList[i] ), pStatus );
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * tagsDone
 *
 * INVOCATION:
 * tTagStruct *tagStruct;
 * areDone = tagsDone( &tagStruct );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pTagStruct (tTagStruct *) Tag list structure.
 *
 * FUNCTION VALUE:
 * (boolean) True if all tags in the list are done.
 *
 * PURPOSE:
 * Return true when all tags in the list reach an end state
 *
 * DESCRIPTION:
 * Check each tag in the tag list and return FALSE when one is not
 * complete, otherwise return true.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 * The tags must be returned by other DHS library functions.
 *
 * DEFICIENCIES:
 * No check is done to make sure the tags haven't been freed.
 *-
 ************************************************************************
 */

static boolean	tagsDone
(
    tTagStruct	*pTagStruct	/* (in)  Pointer to the tTagStruct.	*/
)
{
    int		i;
    DHS_TAG	tag;		/* Command tag.				*/


    fnEntry( "tagsDone" );


    /*
     *  Loop through each tag in the list and test it.
     */

    for ( i = 0; i < pTagStruct->tsNumTags; i++ )
    {
	/* 
	 *  Get the current tag.
	 */

	tag = pTagStruct->tsTags[i];


	/*
	 *  Check to see if the command is complete.
	 */

	if ( ! dhsCmdPtr( tag )->dcCmdDone )
	{
	    fnReturn( FALSE );
	}
    }

    fnReturn( TRUE );
}
