/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1998.				(c) 1998
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
 * dhs/h/av.h
 *
 * PURPOSE:
 * Contains the private interface to the attribute functions.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:39  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.2  2001/03/27 20:02:34  nhill
 * Added the avEditInfo structure.
 *
 * Revision 1.1  1998/08/04 16:17:03  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef AV_H
#define	AV_H

#include <sys/types.h>

/*
 *  The avEditInfo structure is used to store inforation in the SDS
 *  extra data area. This information is used when editing attributes.
 */

typedef struct avEditInfo
{
     EPTR              *aeiNullValue;  /* So it finds a null value when*/
                                       /* Looking for an eptr list in  */
                                       /* The extra information.       */
     DHS_DATA_TYPE     aeiDataType;    /* The type of data in the structure.*/
     SdsIdType         aeiDataID;      /* The pointer to the data.     */
 } tAvEditInfo;

boolean		dhsIncrIndex( ulong[], const ulong[], long );
void		dhsSdsIdAdd( DHS_AV_LIST, SdsIdType, DHS_STATUS * );

#endif /* AV_H */

