static char rcsid[] = "$Id: dhsLocal.c,v 1.1.1.1 2002-11-24 20:20:25 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997				(c) 1997
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
 * dhsLibrary/src/dhsLocal.c
 *
 * PURPOSE:
 * Whatever
 *
 * FUNCTION NAME(S)
 * dhsCallbackSet - Set a callback function.
 * dhsLocalExit - Clean up the dhsLocal structure.
 * dhsLocalInit - Initalize the dhsLocal structure.
 * initSet - Set the initialized variable.
 * initTest - Check to see if the library is initialized.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.11  1998/08/04 16:14:58  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.10  1998/02/24 01:05:19  nhill
 * Added dlExitCount to the dhsLocal structure.
 *
 * Revision 1.9  1998/01/20 18:22:25  nhill
 * Added the connection callback.
 *
 * Revision 1.8  1997/12/24 19:30:13  nhill
 * Added initialization of the dlLockout variable.
 *
 * Revision 1.7  1997/12/18 21:51:03  nhill
 * Added some new initializations.
 *
 * Revision 1.6  1997/08/27 22:02:54  nhill
 * Added a separate dhsLocalIsInit to indicate if the dhsLocal structure
 * is initialized.
 *
 * Revision 1.5  1997/01/27 22:35:03  nhill
 * Checkin before install.
 *
 * Revision 1.3  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.2  1997/01/13 17:58:34  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */

#include "localDhs.h"


/*
 *  allocate storage for the dhsLocal storage structure.
 */

tDhsLocal	dhsLocal;
boolean		dhsLocalIsInit = FALSE;
static boolean	initialized = FALSE;

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCallbackSet
 *
 * INVOCATION:
 * oldCallback = dhsCallbackSet( cbtype, cbFn, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) cbType (DHS_CB_TYPE) Type of callback function to set.
 * (>) cbFn (DHS_CB_FN_PTR) Pointer to the new callback function.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_CB_FN_PTR) Pointer to the old callback function.
 *
 * PURPOSE:
 * Set a pointer to a new callback function
 *
 * Setting the pointer to NULL disables the callback. The previously
 * set callback function is returned.
 *
 * DESCRIPTION:
 * Set the appropriate function pointer.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal: Used to store the callback function pointers.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must be initialized with the dhsInit function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_CB_FN_PTR	dhsCallbackSet
(
    DHS_CB_TYPE	cbType,		/* (in)  Type of callback to set.	*/
    DHS_CB_FN_PTR
    		cbFn,		/* (in)  New callback function pointer.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    DHS_CB_FN_PTR	oldFn;	/* The old callback function.		*/

    
    fnEntry( "dhsCallbackSet" );
    statCheck( *pStatus, NULL );
    isInit( *pStatus, NULL );


    /*
     *  Set the appropriate callback function.
     */

    switch ( cbType )
    {
	case	DHS_CBT_COMMAND:
	    oldFn = dhsLocal.dlCBFCommand;
	    /*
	     *  Function called when commands are received by a server.
	     */

	    dhsLocal.dlCBFCommand = cbFn;
	    break;

	case	DHS_CBT_CONNECT:
	    oldFn = dhsLocal.dlCBFConnect;
	    /*
	     *  Function called when commands are received by a server.
	     */

	    dhsLocal.dlCBFConnect = cbFn;
	    break;

	case	DHS_CBT_ERROR:
	    /*
	     *  Error callback function.
	     */

	    oldFn = dhsLocal.dlCBFError;
	    dhsLocal.dlCBFError = cbFn;
	    break;

	case	DHS_CBT_GET:
	    /*
	     *  Function called when the response to a data get request
	     *  is received.
	     */

	    oldFn = dhsLocal.dlCBFGet;
	    dhsLocal.dlCBFGet = cbFn;
	    break;
	case	DHS_CBT_PUT:
	    /*
	     *  Function called when a response to a bulk data put is 
	     *  received.
	     */

	    oldFn = dhsLocal.dlCBFPut;
	    dhsLocal.dlCBFPut = cbFn;
	    break;
	case	DHS_CBT_RESPONSE:
	    /*
	     *  Function called when the response to a command is received.
	     */

	    oldFn = dhsLocal.dlCBFResponse;
	    dhsLocal.dlCBFResponse = cbFn;
	    break;
	case	DHS_CBT_SERVER_GET:
	    /*
	     *  Function called on a server when a bulk data get request
	     *  is received.
	     */

	    oldFn = dhsLocal.dlCBFServerGet;
	    dhsLocal.dlCBFServerGet = cbFn;
	    break;
	case	DHS_CBT_SERVER_PUT:
	    /*
	     *  Function called on a server when a bulk data put request
	     *  is received.
	     */

	    oldFn = dhsLocal.dlCBFServerPut;
	    dhsLocal.dlCBFServerPut = cbFn;
	    break;
	default:
	    /*
	     *  If none of the above, it is an error.
	     */

	    dhsMsgFmt( DHS_E_CB_TYPE, DHS_DEBUG_OFF, cbType );
	    *pStatus = DHS_E_CB_TYPE;
	    oldFn = NULL;
    }

    fnReturn( oldFn );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsLocalExit
 *
 * INVOCATION:
 * dhsLocalExit( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Clean up the dhsLocal structure.
 *
 * DESCRIPTION:
 * Free anything allocated to the dhsLocal structure.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal: The structure to initialize.
 *
 * PRIOR REQUIREMENTS:
 * The structure should have been initialized with the dhsLocalInit function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	dhsLocalExit
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    statCheck( *pStatus, VOID );

    dhsLocalIsInit = FALSE;

    dhsMutexDestroy( &( dhsLocal.dlImpMutex ), pStatus );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsLocalInit
 *
 * INVOCATION:
 * dhsLocalInit( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Initialize the dhs local structure.
 *
 * DESCRIPTION:
 * Set all values to their null values.
 *
 * EXTERNAL VARIABLES:
 * - dhsLocal: The structure to initialize.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	dhsLocalInit
(
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    statCheck( *pStatus, VOID );

    dhsLocal.dlCBFCommand = NULL;
    dhsLocal.dlCBFConnect = NULL;
    dhsLocal.dlCBFError = NULL;
    dhsLocal.dlCBFGet = NULL;
    dhsLocal.dlCBFPut = NULL;
    dhsLocal.dlCBFResponse = NULL;
    dhsLocal.dlCBFServerGet = NULL;
    dhsLocal.dlCBFServerPut = NULL;
    dhsLocal.dlExitCount = 0;
    dhsLocal.dlImpId = NULL;
    dhsMutexInit( &( dhsLocal.dlImpMutex ), pStatus );
    dhsLocal.dlLockout = TRUE;
    dhsMsgInit( &( dhsLocal.dlMsg ) );
    dhsLocal.dlMsg.dmMsg = dhsLocal.dlMsgString;
    dhsLocal.dlMsg.dmMsg[0] = '\0';
    dhsLocal.dlMsg.dmFree = TRUE;
    dhsLocal.dlTimeout = 10;

    dhsLocalIsInit = TRUE;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 *	initSet
 *
 * PURPOSE:
 *	Set the initialized state of the DHS library
 *
 * DESCRIPTION:
 *	none
 *
 * INVOCATION:
 *	initSet( boolean );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *	(>) value (boolean) New value for the initialized state
 *	(<) fnName (long) Descripton of the parameter
 *
 * FUNCTION VALUE:
 *	none
 *
 * EXTERNAL VARIABLES:
 *	initialized - Keeps track of the current state.
 *
 * PRIOR REQUIREMENTS:
 *	none
 *
 * DEFICIENCIES:
 *	none
 *-
 ************************************************************************
 */

void		dhsInitSet
(
    boolean	value		/* (in)  New value for the variable.	*/
)
{

    initialized = value;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 *	initTest
 *
 * PURPOSE:
 *	Return the current initialization state of the DHS library
 *
 * DESCRIPTION:
 *	none
 *
 * INVOCATION:
 *	state = initTest()
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 *	none
 *
 * FUNCTION VALUE:
 *	(boolean) TRUE if the library is initialized, false otherwise
 *
 * EXTERNAL VARIABLES:
 *	initialized - Keeps track of the current state.
 *
 * PRIOR REQUIREMENTS:
 *	none
 *
 * DEFICIENCIES:
 *	none
 *-
 ************************************************************************
 */

boolean		dhsInitTest
(
    void
)
{
    return( initialized );
}
