static char rcsid[] = "$Id: avListUtil.c,v 1.1.1.1 2002-11-24 20:19:43 brighton Exp $";
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
 * dhs/src/avListUtil.c
 *
 * PURPOSE:
 * Miscelaneous utility routines.
 *
 * FUNCTION NAME(S)
 * dhsAvListExport - Export an attribute value list to a memory buffer.
 * dhsAvListPrint - Print an attribute value list to stdout.
 * dhsAvListSize - Return the buffer size required to export a av list.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:57  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.3  1998/08/04 16:12:40  nhill
 * Rearranged to code to be more object-oriented.
 *
 * Revision 1.2  1997/08/06 16:18:33  nhill
 * Added dhsAvListExport and dhsAvListSize functions.
 *
 * Revision 1.1  1997/02/12 22:05:23  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/
#include "localDhs.h"

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListExport
 *
 * INVOCATION:
 * dhsAvListExport( avList, buffer, bufferSize, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) The attribute value list to export.
 * (>) buffer (void *) The buffer to received the av list.
 * (>) bufferSize (unsigned long) The size of the buffer.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Export an attribute value list into a memory buffer
 *
 * The buffer parameter must contain a buffer large enough to contain 
 * the exported structure. The size of the exported structure can be
 * determined before hand using the dhsAvListSize function.
 *
 * DESCRIPTION:
 * Call function SdsExport.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsAvListSize
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsAvListExport
(
    DHS_AV_LIST	avList,		/* (in)  Attribute value list to export.*/
    void	*buffer,	/* (in)  Buffer to receive the av list.	*/
    unsigned long		/* (in)  Size of the buffer.		*/
		bufferSize,
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    StatusType	sdsStatus;


    fnEntry( "dhsAvListExport" );
    statCheck( *pStatus, VOID );
    isInit( *pStatus, VOID );

    sdsStatus = SDS__OK;
    sdsCheck( SdsExport( dhsSdsId( avList ), bufferSize, buffer, &sdsStatus ),
	    sdsStatus, *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListPrint
 *
 * INVOCATION:
 * dhsAvListPrint( avList, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avlist (DHS_AV_LIST) The attribute value list to print.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * none.
 *
 * PURPOSE:
 * Print an attribute value list
 *
 * DESCRIPTION:
 * Use the SdsList function to print an attribute value list.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void		dhsAvListPrint
(
    DHS_AV_LIST	avList,		/* (in)  The av list to print.		*/
    DHS_STATUS	*pStatus	/* (mod) The function return status.	*/
)
{
    StatusType	sdsStatus;	/* SDS function return status.		*/

    fnEntry( "dhsAvListPrint" );
    statCheck( *pStatus, VOID );

    sdsStatus = SDS__OK;
    sdsCheck( SdsList( dhsSdsId( avList ), &sdsStatus ), 
	    sdsStatus, *pStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsAvListSize
 *
 * INVOCATION:
 * size = dhsAvListSize( avList, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) The attribute value list to size.
 * (!) pStatus (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Determine the size an av list will require when it is exported
 *
 * DESCRIPTION:
 * Call function SdsSize.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsAvListExport
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

unsigned long	dhsAvListSize
(
    DHS_AV_LIST	avList,		/* (in)  Attribute value list to size.	*/
    DHS_STATUS	*pStatus	/* (mod) Function return status.	*/
)
{
    unsigned long
    		size;
    StatusType	sdsStatus;


    fnEntry( "dhsAvListSize" );
    statCheck( *pStatus, 0 );
    isInit( *pStatus, 0 );

    sdsStatus = SDS__OK;
    sdsCheck( SdsSize( dhsSdsId( avList ), &size, &sdsStatus ), 
	    sdsStatus, *pStatus, 0 );

    fnReturn( size );
}
