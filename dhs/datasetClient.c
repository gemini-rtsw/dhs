static char rcsid[] = "$Id: datasetClient.c,v 1.1.1.1 2002-11-24 20:20:18 brighton Exp $";
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
 * dhs/src/icd3Client.c
 *
 * PURPOSE:
 * Contains routines that should only be used by applications behaving like
 * an ICD 3 client.
 *
 * FUNCTION NAME(S)
 * dhsBdCtl - Send a control function to an ICD 3 server.
 * dhsBdDelete - Delete a dataset from the data server.
 * dhsBdGet - Get a dataset associated with a data label.
 * dhsBdName - Request a new data label from a server.
 * dhsBdPut - Send bulk data to the data server.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.18  2001/03/27 20:28:51  nhill
 * Added vxWorks.h when compiling for vxworks.
 *
 * Revision 1.17  1998/08/04 16:14:16  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.16  1998/05/25 19:52:11  nhill
 * Removed function dhsBdPtrGet.
 *
 * Revision 1.15  1998/02/16 23:43:20  nhill
 * Changed to use dhsConnectUnlock to unlock the connection mutex.
 *
 * Revision 1.14  1997/12/18 21:53:44  nhill
 * Changed to check for missing connections in dhsBdCtl, dhsBdDelete,
 * dhsBdGet, and dhsBdPut.
 *
 * Revision 1.13  1997/09/18 20:54:30  nhill
 * Improved error checking and response processing in the dhsBdCtl function.
 *
 * Revision 1.12  1997/08/19 19:46:08  nhill
 * Added some coments.
 *
 * Revision 1.11  1997/06/04 18:04:43  nhill
 * Changed some dataLabel terminology to datasetName.
 * Removed the lifetime, qlstream and numContrib parameters to dhsBdName.
 * added the DHS_BD_PT_??_UNIQUE put data types.
 *
 * Revision 1.10  1997/04/24 16:49:01  nhill
 * Added some "const" keywords.
 * Fixed an incorrect name in a fnEntry macro call.
 *
 * Revision 1.9  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.8  1997/02/24 19:17:17  nhill
 * Checkin for install.
 *
 * Revision 1.7  1997/02/04 19:41:34  nhill
 * Checkpoint for install.
 *
 * Revision 1.6  1997/01/30 17:18:54  nhill
 * Checkin for install.
 *
 * Revision 1.5  1997/01/29 18:18:23  nhill
 * Checkin for install.
 *
 * Revision 1.4  1997/01/22 21:34:52  nhill
 * stable checkin.
 *
 * Revision 1.3  1997/01/20 17:15:11  nhill
 * Checkpoint.
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

#ifdef vxWorks
#include <vxWorks.h> /* must come before stdarg.h */
#endif
#include <stdarg.h>
#include <stdlib.h>

#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdClt
 *
 * INVOCATION:
 * DHS_CONNECT		connect;
 * char			datasetName[] = "data label";
 * DHS_BD_LIFETIME	lifetime;
 * char			*contribs[] = { "contrib1", "contrib2", "contrib3" };
 * char			*streams[] = { "stream1", "stream2" };
 * char			*newlabel;
 * dhsBdCtl( connect, DHS_BD_CTL_ABORT, datasetName, &status );
 * dhsBdCtl( connect, DHS_BD_CTL_RESET, datasetName, &status );
 * dhsBdCtl( connect, DHS_BD_CTL_LIFETIME, datasetName, lifetime, &status );
 * dhsBdCtl( connect, DHS_BD_CTL_CONTRIB, datasetName, 3, contribs, &status );
 * dhsBdCtl( connect, DHS_BD_CTL_QLSTREAM, datasetName, 2, streams, &status );
 * dhsBdCtl( connect, DHS_BD_CTL_GETNAME, &newLabel, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to get the control.
 * (>) ctl (DHS_BD_CTL) The ctl to send.
 * (>) datasetName (char *) Optional data label for the ctl.
 * (>) lifetime (DHS_BD_LIFETIME) Optional data lifetime.
 * (>) arraySize (int) Optional contributor or stream array size.
 * (>) nameArray (char **) Optional array of contributors or streams.
 * (<) pDataLabel (char **) Optional output data label
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send a control function to a bulk data server
 *
 * DESCRIPTION:
 * Extract the optional arguments from the vararg list and send a message
 * to the server based on the optional arguments.
 *
 * The type of ctl being sent determines the optional argument list.
 *
 * EXTERNAL VARIABLES:
 * - The thread specific data structure is used to hold the new data label
 *   while it is returned to the caller.
 *
 * PRIOR REQUIREMENTS:
 * - The DHS library must be initialized with the dhsInit function.
 * - A connection must have been opened with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsBdCtl
(
    DHS_CONNECT	connect,	/* (in)  Connection to get the control.	*/
    DHS_BD_CTL	ctl,		/* (in)  The ctl to send.		*/
    ...				/* (mod) Optional arguments and return 	*/
				/*	 status.			*/
)
{
    /*
     *  Arguments from the vararg list.
     */

    int		arraySize;	/* (in)  Contributor or stream array size.*/
    char	*datasetName;	/* (in)  Data label for the ctl.	*/
    int		i;
    DHS_BD_LIFETIME		/* (in)  New lifetime for the data label.*/
    		lifeTime;
    char	**nameArray;	/* (in)  Array of contributors or streams.*/
    char	**pDataLabel;	/* (out) Output data label.		*/

    DHS_AV_ID	av;		/* An attribute in the av list.		*/
    DHS_AV_LIST	avList;		/* The commands av list.		*/
    char	*cmdString;	/* Command response string.		*/
    DHS_CMD_STATUS		/* Completion status of a command.	*/
		cmdStatus;
    tDhsCmd	*pCmd;		/* Pointer to the command for the ctl.	*/
    DHS_AV_LIST	response;	/* Command response av list.		*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    DHS_STATUS	s2;		/* Temporary function return status.	*/
    DHS_STATUS	*pStatus;	/* Function return status.		*/
    DHS_TAG	tag;		/* Tag associated with the ctl command.	*/
    tDhsTsd	*pTsd;		/* Pointer to the thread specific data.	*/
    char	*pChar;		/* Generic character pointer.		*/
    va_list	varg;		/* Variable argument list.		*/


    fnEntry( "dhsBdCtl" );


    /*
     *  The initialization an status checks that usually occur here are
     *  delayed, since the status parameter has be be extracted from the
     *  vararg list.
     */


    /*
     *  Create a new av list for the ctl command.
     */

    s = DHS_S_SUCCESS;
    avList = dhsAvListNew( &s );


    /*
     *  Extract the optional arguments from the variable argument list.
     *  The arguments required depend on the ctl type.
     */

    va_start( varg, ctl );
    switch( ctl )
    {
	case	DHS_BD_CTL_ABORT:
	case	DHS_BD_CTL_RESET:
	    /*
	     *  Extract the data label to be aborted or reset from the
	     *  vararg list and add it to the av list.
	     */

	    datasetName = va_arg( varg, char * );
	    dhsAvAdd( avList, "datasetName", DHS_DT_STRING, 0, NULL, 
		    datasetName, &s );
	    break;
	case	DHS_BD_CTL_LIFETIME:
	    /*
	     *  Extract the data label to be changed from the vararg list and 
	     *  add it to the av list.
	     */

	    datasetName = va_arg( varg, char * );
	    dhsAvAdd( avList, "datasetName", DHS_DT_STRING, 0, NULL, 
		    datasetName, &s );


	    /*
	     *  Extract the new lifetime from the vararg list and add it to
	     *  the av list.
	     */

	    lifeTime = va_arg( varg, DHS_BD_LIFETIME );
	    dhsAvAdd( avList, "lifeTime", DHS_DT_INT32, 0, NULL, 
		    lifeTime, &s );
	    break;
	case	DHS_BD_CTL_CONTRIB:
	case	DHS_BD_CTL_QLSTREAM:
	    /*
	     *  Extract the data label to be changed from the vararg list and 
	     *  add it to the av list.
	     */

	    datasetName = va_arg( varg, char * );
	    dhsAvAdd( avList, "datasetName", DHS_DT_STRING, 0, NULL, 
		    datasetName, &s );
	    
	    
	    /*
	     *  Extract the number of names in the name array from the 
	     *  vararg list.
	     */

	    arraySize = va_arg( varg, int );


	    /*
	     *  Extract the name array from the vararg list and add it to
	     *  the av list.
	     */

	    nameArray = va_arg( varg, char ** );
	    dhsAvAdd( avList, "nameArray", DHS_DT_STRING, 1, 
		    (ulong *) &arraySize, nameArray, &s );
	    break;
	case	DHS_BD_CTL_GETNAME:
	    /*
	     *  Extract the pointer to the new data label from the vararg
	     *  list.
	     */

	    pDataLabel = va_arg( varg, char ** );
	    break;
	default:
	    break;
    }


    /*
     *  Get the function return status pointer from the vararg list.
     */

    pStatus = va_arg( varg, DHS_STATUS * );
    va_end( varg );


    /*
     *  Do a belated check that the status value is ok, and the DHS library
     *  has been initialized.
     */

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  If any of the operations done before the function return status 
     *  was extracted failed, then return the failure status.
     */

    if ( s != DHS_S_SUCCESS )
    {
	*pStatus = s;
	fnReturn( VOID );
    }
    

    /*
     *  Add the ctl function type to the av list.
     */

    dhsAvAdd( avList, "ctl", DHS_DT_INT32, 0, NULL, ctl, pStatus );


    /*
     *  Create a new command to sent the ctl.
     */

    check( pCmd = dhsCmdNew( "bdCtl", avList, NULL, DHS_MSG_APPLY, 
	    DHS_CF_NONE, DHS_TAG_NULL, DHS_CS_PENDING, pStatus ), 
	    *pStatus, VOID );


    /*
     *  Send the ctl command to the server.
     */

    dhsConnectLock( dhsConnectPtr( connect ), &s );
    dhsConnectSend( dhsConnectPtr( connect ), pCmd, &s );
    if ( s != DHS_E_CON_NOT_FOUND )
    {
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( dhsConnectPtr( connect ), &s2 );
    }


    /*
     *  Wait for the ctl command to complete.
     */

    tag = dhsCmdTag( pCmd );
    dhsWait( 1, &tag, &s );

    cmdStatus = dhsStatus( tag, &cmdString, pStatus );
    if ( cmdStatus != DHS_CS_DONE )
    {
	if ( cmdString != NULL )
	{
	    dhsMsgFmt( DHS_E_CTL_CMD, DHS_DEBUG_OFF, cmdString );
	}
	else
	{
	    dhsMsgFmt( DHS_E_CTL_CMD, DHS_DEBUG_OFF, "" );
	}
	*pStatus = DHS_E_CTL_CMD;
	fnReturn( VOID );
    }

    /*
     *  If the ctl was a DHS_BD_CTL_GETNAME, the new name has be be extracted
     *  from the response attribute value list.
     */

    if ( ctl == DHS_BD_CTL_GETNAME )
    {
	for ( i = 0; s == DHS_S_SUCCESS; i++ )
	{
	    /*
	     *  Get the response attribute value list.
	     */

	    response = dhsResponseGet( tag, i, &s );
	    if ( response == DHS_AV_LIST_NULL )
	    {
		continue;
	    }


	    /*
	     *  Find the new data label in the av list.
	     */

	    av = dhsAvFind( response, "dataLabel", &s );
	    if ( s == DHS_S_SUCCESS )
	    {
		break;
	    }
	    else if ( s == DHS_S_NO_ATTRIB )
	    {
		s = DHS_S_SUCCESS;
	    }
	}
	if ( *pStatus == DHS_S_NO_RESP )
	{
	    dhsMsgFmt( DHS_E_NO_ATTRIB, DHS_DEBUG_OFF, "dataLabel" );
	    *pStatus = DHS_S_NO_RESP;
	    fnReturn( VOID );
	}


	/*
	 *  Get the value of the new data label.
	 */

	dhsAvInfo( av, NULL, NULL, NULL, NULL, &pChar, &s );


	/*
	 *  Copy the new data label into a buffer in the thread specific
	 *  data structure so a pointer to it can be returned without
	 *  fear that the memory will be re-used.
	 */

	pTsd = dhsTsdGet( &s );
	if ( s == DHS_S_SUCCESS )
	{
	    (void) strcpy( pTsd->dtDataLabel, pChar );
	    *pDataLabel = pTsd->dtDataLabel;
	}
    }


    /*
     *  Free the command tag and av list.
     */

    dhsTagFree( tag, pStatus );
    dhsAvListFree( avList, pStatus );


    /*
     *  If any of the operations failed, return the failure code.
     */

    if ( *pStatus == DHS_S_SUCCESS && s != DHS_S_SUCCESS )
    {
	*pStatus = s;
    }


    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdDelete
 *
 * INVOCATION:
 * DHS_CONNECT	connect;
 * char		*label;
 * DHS_STATU	status;
 * dhsBdDelete( connect, label, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to the data server.
 * (>) label (char *) The data label to delete.
 * (!) pStatus (DHS_STATUS *) Funtion return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Delete a dataset from the data server
 *
 * DESCRIPTION:
 * - Create an attribute value list with the dataset name as an attribute.
 * - Send the command to the data server.
 * - Wait for the command to complete.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The DHS library must be initialized with the dhsInit function.
 * - A connection must have been opened with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsBdDelete
(
    DHS_CONNECT	connect,	/* (in)  Connection to the data server.	*/
    char	*label,		/* (in)  The data label to delete.	*/
    DHS_STATUS	*pStatus	/* (in)  Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* The commands av list.		*/
    tDhsCmd	*pCmd;		/* Pointer to the command for the ctl.	*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    DHS_STATUS	s2;		/* Temporary function return status.	*/
    DHS_TAG	tag;		/* Tag associated with the ctl command.	*/


    fnEntry( "dhsBdDelete" );

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Create a new av list for the command.
     */

    s = DHS_S_SUCCESS;
    avList = dhsAvListNew( &s );

    dhsAvAdd( avList, "datasetName", DHS_DT_STRING, 0, NULL, label, &s );


    /*
     *  Create a new command to sent the delete command.
     */

    check( pCmd = dhsCmdNew( "bdDelete", avList, NULL, DHS_MSG_APPLY, 
	    DHS_CF_NONE, DHS_TAG_NULL, DHS_CS_PENDING, pStatus ), 
	    *pStatus, VOID );


    /*
     *  Send the command to the server.
     */

    dhsConnectLock( dhsConnectPtr( connect ), &s );
    dhsConnectSend( dhsConnectPtr( connect ), pCmd, &s );
    if ( s != DHS_E_CON_NOT_FOUND )
    {
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( dhsConnectPtr( connect ), &s2 );
    }


    /*
     *  Wait for the command to complete.
     */

    tag = dhsCmdTag( pCmd );
    dhsWait( 1, &tag, &s );


    /*
     *  Free the command tag and av list.
     */

    dhsTagFree( tag, pStatus );
    dhsAvListFree( avList, pStatus );


    /*
     *  If any of the operations failed, return the failure code.
     */

    if ( *pStatus == DHS_S_SUCCESS && s != DHS_S_SUCCESS )
    {
	*pStatus = s;
    }

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdGet
 *
 * INVOCATION:
 * dhsBdGet( connect, label, type, pUserData, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) The connection to the server.
 * (>) datasetName (char *) The dataset name to retrieve.
 * (>) getType (DHS_BD_GET_TYPE) Type format for the retrieved data.
 * (>) pUserData (void *) User data to be associated with the request.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_TAG) The command tag associated with the request.
 *
 * PURPOSE:
 * Get a dataset associated with a data label
 *
 * This function returns before the data is retrieved. The data can be
 * retrieved by waiting for the command tag to complete with the dhsWait
 * and/or dhsStatus functions, or a get callback function can be set up
 * by the application to process the completion of the request.
 *
 * Note that this function waits until the server changes the status of
 * the command to something other than DHS_CS_PENDING before it returns.
 *
 * DESCRIPTION:
 * Send a DHS_MSG_BD_GET message to the server with the type of data
 * requested in the message av list.  The data label requested is sent
 * as the message label.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The DHS library must be initialized with the dhsInit function.
 * - A connection must have been opened with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_TAG		dhsBdGet
(
    DHS_CONNECT	connect,	/* (in)  The connection to the server.	*/
    const char	*datasetName,	/* (in)  The label to retrieve.		*/
    DHS_BD_GET_TYPE		/* (in)  The type of data to retrieve.	*/
    		getType,
    void	*pUserData,	/* (in)  Pointer to the user data.	*/
    DHS_STATUS	*pStatus	/* (out) Function return status.	*/
)
{
    DHS_AV_LIST	avList;		/* The request attribute value list.	*/
    tDhsCmd	*pCmd;		/* The command to send.			*/
    DHS_STATUS	s;		/* Function return status.		*/
    DHS_STATUS	s2;		/* Function return status.		*/


    fnEntry( "dhsBdGet" );

    statCheck( *pStatus, DHS_TAG_NULL );
    isInit( *pStatus, DHS_TAG_NULL );


    /*
     *  Create a new av list containing the data type requested.
     */

    check( avList = dhsAvListNew( pStatus ), *pStatus, DHS_TAG_NULL );
    dhsAvAdd( avList, "getType", DHS_DT_INT32, 0, NULL, getType, pStatus );


    /*
     *  Create a command object for the message.
     */

    check( pCmd = dhsCmdNew( datasetName, avList, pUserData, DHS_MSG_BD_GET, 
	    DHS_CF_CALLBACK, DHS_TAG_NULL, DHS_CS_PENDING, pStatus ), *pStatus, 
	    DHS_TAG_NULL );
    pCmd->dcGetType = getType;


    s = DHS_S_SUCCESS;


    /*
     *  Increment the command reference counter. This prevents the command
     *  from completing and being deleted before we are done with it.
     */

    dhsCmdRef( pCmd, pStatus );


    /*
     *  Send the message to the server.
     */

    dhsConnectLock( dhsConnectPtr( connect ), &s );
    dhsConnectSend( dhsConnectPtr( connect ), pCmd, &s );
    if ( s != DHS_E_CON_NOT_FOUND )
    {
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( dhsConnectPtr( connect ), &s2 );
    }


    /*
     *  Check for errors sending the command.
     */

    if ( s == DHS_S_SUCCESS )
    {
	/*
	 *  The command was sent so wait until the server accepts it.
	 */

	check( dhsEventLoop( DHS_ELT_COND, dhsCmdStart, pCmd, pStatus ), 
		*pStatus, DHS_CONNECT_NULL );
	

	/*
	 *  We are done with the command now, so decrement the reference
	 *  count. If the command has already completed, this may delete
	 *  the command structure.
	 */

	dhsCmdDeref( pCmd, pStatus );
    }
    else
    {
	check( dhsCmdDelete( pCmd, pStatus ), *pStatus, DHS_TAG_NULL );
	*pStatus = s;
    }


    /*
     *  Free the attribute value list.
     */

    dhsAvListFree( avList, pStatus );

    fnReturn( dhsCmdTag( pCmd ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdName
 *
 * INVOCATION:
 * DHS_CONNECT		connect;
 * DHS_STATUS		status;
 * name = dhsBdName( connect, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to request the name from.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (char *) New data label.
 *
 * PURPOSE:
 * Request a new data label from a server
 *
 * DESCRIPTION:
 * Execute a series of dhsBdClt function calls to get and intialize the label.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The DHS library must be initialized with the dhsInit function.
 * - A connection must have been opened with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

char		*dhsBdName
(
    DHS_CONNECT	connect,	/* (in)  The connection to use.		*/
    DHS_STATUS	*pStatus	/* (out) Function return status.	*/
)
{
    char	*datasetName;	/* Pointer to the new data label.	*/


    fnEntry( "dhsBdName" );

    statCheck( *pStatus, NULL );
    isInit( *pStatus, NULL );


    /*
     *  Get a new data label.
     */

    check( dhsBdCtl( connect, DHS_BD_CTL_GETNAME, &datasetName, pStatus ), 
	    *pStatus, NULL );


    fnReturn( datasetName );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdPut
 *
 * INVOCATION:
 * DHS_CONNECT		connect;
 * char			*datasetName;
 * DHS_BD_DATASET	dataset;
 * DHS_STATUS		status;
 * void			*pData;
 * unsigned long	length;
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_DS, true, dataset, &status );
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_DS_QL, true, dataset, 
 *		&status );
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_RAW, true, pData, length, 
 *		&status );
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_RAW_UNIQUE, true, pData, 
 *		length, &status );
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_FITS, true, pData, length, 
 *		&status );
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_FITS_UNIQUE, true, pData, 
 *		length, &status );
 * tag = dhsBdPut( connect, datasetName, DHS_BD_PT_SDS, true, pData, length, 
 *		&status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to send the data over.
 * (>) datasetName (char *) Data label for the data.
 * (>) putType (DHS_BD_PUT_TYPE) Format of the data to send.
 * (>) last (DHS_BOOLEAN) Is this the last data for the datasetName?
 * (>) dataset (DHS_BD_DATASET) Optional dataset to send.
 * (>) pData (void *) Optional pointer to a data buffer.
 * (>) length (unsigned long) Optional length of the data buffer.
 * (>) pUserData (void *) User data pointer.
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_TAG) Tag associated with the data put.
 *
 * PURPOSE:
 * Send bulk data to a data server
 *
 * The user data pointer is passed to the callback routine in order to
 * allow application specific data to be passed to the callback routine.
 * If the user data pointer is not required, the value should be set to
 * NULL.
 *
 * DESCRIPTION:
 * Extract the optional parameters from the vararg list, and then send
 * a message to the server containing the bulk data.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The DHS library must be initialized with the dhsInit function.
 * - A connection must have been opened with the dhsConnect function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_TAG		dhsBdPut
(
    DHS_CONNECT	connect,	/* (in)  Connection to receive data.	*/
    const char	*datasetName,	/* (in)  datasetName for the data.	*/
    DHS_BD_PUT_TYPE		/* (in)  Type form of the data.		*/
    		putType,
    DHS_BOOLEAN	last,		/* (in)  Is this the last part of the	*/
    				/*	 dataset.			*/
    ...
)
{
    /*
     *  Arguments from the vararg list.
     */

    void	*bulkPtr;	/* (in)  Pointer to a bulk data buffer.	*/
    ulong	bulkLen;	/* (in)  Length of the bulk data buffer.*/
    DHS_BD_DATASET		/* (in)  Dataset to send.		*/
    		dataset;
    DHS_STATUS	*pStatus;	/* (mod) Function return status.	*/
    void	*pUserData;	/* (in)  User data pointer.		*/

    DHS_AV_LIST	avList;		/* Attribute value list for the message.*/
    tDhsCmd	*pCmd;		/* Command structure for the message.	*/
    DHS_STATUS	s;		/* Temporary function return status.	*/
    DHS_STATUS	s2;		/* Temporary function return status.	*/
    va_list	varg;		/* Variable argument list.		*/

    fnEntry( "dhsBdPut" );


    /*
     *  The checking of the function return status and library initialization
     *  usually done here are put off until later since the status parameter
     *  isn't available yet.
     */

    /*
     *  Extract the variable members of the variable argument list.
     */

    va_start( varg, last );
    switch( putType )
    {
	case	DHS_BD_PT_DS:
	case	DHS_BD_PT_DS_QL:
	    /*
	     *  For bulk data in the form of a dataset, the only variable 
	     *  argument is the dataset.
	     */

	    dataset = va_arg( varg, DHS_BD_DATASET );
	    break;
	case	DHS_BD_PT_FITS:
	case	DHS_BD_PT_FITS_UNIQUE:
	case	DHS_BD_PT_RAW:
	case	DHS_BD_PT_RAW_UNIQUE:
	case	DHS_BD_PT_SDS:
	    /*
	     *  For bulk data in the form of a memory buffer, the variable
	     *  arguments are a pointer to the buffer and the length of the
	     *  buffer.
	     */

	    bulkPtr = va_arg( varg, void * );
	    bulkLen = va_arg( varg, ulong );
	    break;
	default:
	    break;
    }


    /*
     *  The last two arguments in the variable argument list are always the
     *  user data pointer and the function return status.
     */
    
    pUserData = va_arg( varg, void * );
    pStatus = va_arg( varg, DHS_STATUS * );
    va_end( varg );


    /*
     * Check that the status is ok and the DHS library is initialized.
     */

    statCheck( *pStatus, DHS_TAG_NULL );
    isInit( *pStatus, DHS_TAG_NULL );


    /*
     *  Create an attribute value list containing the information
     *  about the data.
     */

    avList = dhsAvListNew( pStatus );
    dhsAvAdd( avList, "datasetName", DHS_DT_STRING, 0, NULL, datasetName, pStatus );
    dhsAvAdd( avList, "putType", DHS_DT_INT32, 0, NULL, putType, pStatus );
    dhsAvAdd( avList, "last", DHS_DT_BOOLEAN, 0, NULL, last, pStatus );


    /*
     *  Create a new command structure to send the data.
     */

    pCmd = dhsCmdNew( datasetName, avList, pUserData, DHS_MSG_BD_PUT, 
	    DHS_CF_CALLBACK, DHS_TAG_NULL, DHS_CS_BUSY, pStatus );
    

    /*
     *  Set up the bulk data information in the command structure.
     */

    switch( putType )
    {
	case	DHS_BD_PT_DS:
	case	DHS_BD_PT_DS_QL:
	    pCmd->dcDataset = dataset;
	    pCmd->dcBulkType = DHS_BT_SDS;
	    break;
	case	DHS_BD_PT_FITS:
	case	DHS_BD_PT_FITS_UNIQUE:
	case	DHS_BD_PT_RAW:
	case	DHS_BD_PT_RAW_UNIQUE:
	case	DHS_BD_PT_SDS:
	    pCmd->dcBulkPtr = bulkPtr;
	    pCmd->dcBulkLen = bulkLen;
	    pCmd->dcBulkType = DHS_BT_BUFFER;
	    break;
	default:
	    break;
    }


    /*
     *  Send the command to the server.
     */

    s = DHS_S_SUCCESS;
    dhsConnectLock( dhsConnectPtr( connect ), &s );
    dhsConnectSend( dhsConnectPtr( connect ), pCmd, &s );
    if ( s != DHS_E_CON_NOT_FOUND )
    {
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( dhsConnectPtr( connect ), &s2 );
    }


    /*
     *  Free the attribute value list.
     */

    dhsAvListFree( avList, pStatus );


    /*
     * Check the return status of the send.
     */

    if ( s != DHS_S_SUCCESS )
    {
	check( dhsCmdDelete( pCmd, pStatus ), *pStatus, DHS_TAG_NULL );
	*pStatus = s;
	pCmd = NULL;
    }


    /*
     *  Try to run the event loop to process any pending events.
     */

    dhsEventLoop( DHS_ELT_NO_WAIT, pStatus );


    /*
     *  Return the tag associated with the put.
     */

    fnReturn( dhsCmdTag( pCmd ) );
}
