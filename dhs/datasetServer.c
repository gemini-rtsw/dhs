static char rcsid[] = "$Id: datasetServer.c,v 1.1.1.1 2002-11-24 20:20:19 brighton Exp $";
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
 * dhs/src/datasetServer.c
 *
 * PURPOSE:
 * Contains routines only used by an application acting as an ICD 3 server.
 *
 * FUNCTION NAME(S)
 * dhsBdResponse - Send a response to a client which includes bulk data.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.12  1998/08/04 16:14:35  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.11  1998/03/05 23:35:20  nhill
 * Removed unused variable.
 *
 * Revision 1.10  1998/02/16 23:43:37  nhill
 * Changed to use dhsConnectUnlock to unlock the connection mutex.
 *
 * Revision 1.9  1997/12/18 22:09:18  nhill
 * Changed dhsBdResponse to check for missing connections.
 *
 * Revision 1.8  1997/06/10 22:56:45  nhill
 * Added the _ASIS data types to the dhsBdResponse function.
 *
 * Revision 1.7  1997/06/04 18:08:33  nhill
 * Added the DHS_BD_GT_??_ASIS types to dhsBdGet.
 * Dropped the DHS_BD_GT_DS... types from dhsBdGet.
 *
 * Revision 1.6  1997/04/24 16:49:39  nhill
 * Added some "const" keywords.
 *
 * Revision 1.5  1997/03/26 16:59:21  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.4  1997/02/24 19:17:17  nhill
 * Checkin for install.
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
#ifndef	DHS_NO_SERVER

#include <stdarg.h>
#include <stdlib.h>

#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBdResponse
 *
 * INVOCATION:
 * DHS_CONNECT		connect;
 * DHS_TAG		tag;
 * char			*string;
 * DHS_AV_LIST		avList;
 * DHS_BD_DATASET	dataset;
 * DHS_STATUS		status;
 * void			*pData;
 * unsigned long	length;
 * dhsBdResponse( connect, tag, DHS_CS_DONE, string, avList, 
 *		DHS_BD_GT_FITS, pData, length, &status );
 * dhsBdResponse( connect, tag, DHS_CS_DONE, string, avList, 
 *		DHS_BD_GT_FITS_ASIS, pData, length, &status );
 * dhsBdResponse( connect, tag, DHS_CS_DONE, string, avList, 
 *		DHS_BD_GT_FITS_HEADER, pData, length, &status );
 * dhsBdResponse( connect, tag, DHS_CS_DONE, string, avList, 
 *		DHS_BD_GT_RAW, pData, length, &status );
 * dhsBdResponse( connect, tag, DHS_CS_DONE, string, avList, 
 *		DHS_BD_GT_RAW_ASIS, pData, length, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connect (DHS_CONNECT) Connection to send the response to.
 * (>) tag (DHS_TAG) Tag associated with the request on the client.
 * (>) cmdStatus (DHS_CMD_STATUS) New status for the get command.
 * (>) statusStrig (char *) New status string for the get command.
 * (>) avList (DHS_AV_LIST) Attribute value list for the response.
 * (>) getType (DHS_BD_GET_TYPE) The type of data being returned.
 * (>) dataset (DHS_BD_DATASET) Optional dataset to send.
 * (>) pData (void *) Optional pointer to a data buffer.
 * (>) length (unsigned long) Optional length of a data buffer.
 * (!> pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Send reply to a client, including bulk data
 *
 * DESCRIPTION:
 * Create a message containing the bulk data and send it to the client.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * - The DHS library must be initalized with the dhsInit function.
 * - A data get request must have been received by the application calling
 *   this function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsBdResponse
(
    DHS_CONNECT	connect,	/* (in)  Connection to the client.	*/
    DHS_TAG	tag,		/* (in)  Clients command tag.		*/
    DHS_CMD_STATUS		/* (in)  Status new command status.	*/
    		cmdStatus,
    const char	*statusString,	/* (in)  Command status description.	*/
    DHS_AV_LIST	avList,		/* (in)  Attribute value list for the	*/					/*	 command.			*/
    DHS_BD_GET_TYPE		/* (in)  Type of the data.		*/
    		getType,
    ...				/* Optional arguements.			*/
)
{
    /*
     *  Arguments from the vararg list.
     */

    ulong	length;		/* (in) Length of the bulk data array.	*/
    byte	*pData;		/* (in) Pointer to the bulk data array.	*/
    DHS_STATUS	*pStatus;	/* (mod) Function return status.	*/

    tDhsCmd	*pResponse;	/* The response command.		*/
    DHS_STATUS	s;		/* Function return status.		*/
    DHS_STATUS	s2;		/* Function return status.		*/
    va_list	varg;		/* Variable arguments.			*/


    fnEntry( "dhsbdResponse" );


    /*
     *  The check of the library initialization and function return status
     *  usually done here are done later since the function return status
     *  is not yet available.
     */


    /*
     *  Extract the variable arguments from the vararg list.
     */

    va_start( varg, getType );
    switch( getType )
    {
	case	DHS_BD_GT_FITS:
	case	DHS_BD_GT_FITS_ASIS:
	case	DHS_BD_GT_FITS_HEADER:
	case	DHS_BD_GT_RAW:
	case	DHS_BD_GT_RAW_ASIS:
	    /*
	     *  If the bulk data is a memory buffer, the variable arguments
	     *  are a pointer to the buffer and the length of the buffer.
	     */

	    pData = va_arg( varg, void * );
	    length = va_arg( varg, ulong );
	    break;
	default:
	    break;
    }


    /*
     *  The last argument in the vararg list is always the function return
     *  status.
     */

    pStatus = va_arg( varg, DHS_STATUS * );
    va_end( varg );
    

    /*
     *  Check the function return status and that the DHS library has been
     *  initialized.
     */

    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );


    /*
     *  Create a new command for the response.
     */

    check( pResponse = dhsCmdNew( statusString, avList, NULL, DHS_MSG_BD_REPLY, 
	    FALSE, tag, cmdStatus, pStatus ), *pStatus, VOID );


    /*
     *  Set up the bulk data information in the command, depending on the
     *  type of the bulk data.
     */

    switch( getType )
    {
	case	DHS_BD_GT_FITS:
	case	DHS_BD_GT_FITS_ASIS:
	case	DHS_BD_GT_FITS_HEADER:
	case	DHS_BD_GT_RAW:
	case	DHS_BD_GT_RAW_ASIS:
	    pResponse->dcBulkPtr = pData;
	    pResponse->dcBulkLen = length;
	    pResponse->dcBulkType = DHS_BT_BUFFER;
	    break;
	default:
	    break;
    }


    /*
     *  Send the command to the client.
     */

    s = DHS_S_SUCCESS;
    dhsConnectLock( dhsConnectPtr( connect ), &s );
    dhsConnectSend( dhsConnectPtr( connect ), pResponse, &s );
    if ( s != DHS_E_CON_NOT_FOUND )
    {
	s2 = DHS_S_SUCCESS;
	dhsConnectUnlock( dhsConnectPtr( connect ), &s2 );
    }


    /*
     *  Dereferencing the response command should cause it to be deleted.
     *  (deletion could be delayed if the command is required for bulk data
     *  buffers)
     */

    dhsCmdDeref( pResponse, pStatus );


    if ( s != DHS_S_SUCCESS )
    {
	*pStatus = s;
    }

    fnReturn( VOID );
}
#endif /* DHS_NO_SERVER */
