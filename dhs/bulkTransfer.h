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
 * dhs/h/bulkTransfer.h
 *
 * PURPOSE:
 * This file contains the interface to the bulk transfer structures and
 * functions.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:39  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:58  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1998/08/04 16:17:09  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef H_BULKTRANSFER_H
#define	H_BULKTRANSFER_H

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBtTag
 *
 * INVOCATION:
 * btTag = dhsBtTag( pDhsBt );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pDhsBt (tDhsBt *) Pointer to a bulk transfer structure.
 *
 * MACRO VALUE:
 * (tDhsBtTag) The tag for the bulk transfer structure.
 *
 * PURPOSE:
 * Given a pointer to a tDhsBt structure, return its tag
 *
 * This is made to look like a function in case the mapping gets more
 * complicated in the future.
 *
 * DESCRIPTION:
 * Cast the pointer into a tDhsBtTag type (unsigned long).
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsBtPtr
 *
 * DEFICIENCIES:
 * Won't work if a pointer can't be cast into a tDhsBtTag and back without
 * loss.
 *-
 ************************************************************************
 */

#define	dhsBtTag( pDhsBt )					\
		    ( (tDhsBtTag) pDhsBt )

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsBtPtr
 *
 * INVOCATION:
 * pDhsBt = dhsBtPtr( dhsBtTag );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) dhsBtTag (tDhsBtTag) Bulk transfer tag.
 *
 * MACRO VALUE:
 * (tDhsBt *) Pointer to the bulk transfer structure.
 *
 * PURPOSE:
 * Given a bulk transfer tag, return a pointer to the bulk transfer structure
 *
 * This is made to look like a function in case the mapping gets more
 * complicated in the future.
 *
 * DESCRIPTION:
 * Cast the tag into a pointer to a bulk transfer structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsBtTag
 *
 * DEFICIENCIES:
 * Won't work if a pointer can't be cast into a tDhsBtTag and back without
 * loss.
 *-
 ************************************************************************
 */

#define	dhsBtPtr( dhsBtTag )					\
		    ( (tDhsBt *) dhsBtTag )


typedef ulong	tDhsBtTag;	/* Bulk transfer tag id.		*/


/*
 *  Create an enumerated type to be used as the type parameter to the
 *  dhsBtFind function.
 */

typedef enum
{
    DHS_BF_BT_TAG		/* find by bulk data transfer id.	*/
} tDhsBtFind;


/*
 *  Create a type to contain information about a bulk transfer.
 */

typedef struct dhsBt
{
    byte		*dbAddress;	/* Shared memory area address.	*/
    tDhsConnect		*dbConnect;	/* Connection for the transfer.	*/
    IMP_SharedMemInfo	dbSharedMem;	/* IMP shared memory info.	*/
    boolean		dbSharedMemUsed;/* Shared memory in use?	*/
    int			dbSize;		/* Size of the buffer.		*/
} tDhsBt;


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern void		dhsBtDelete( tDhsBt *, DHS_STATUS * );
extern tDhsBt		*dhsBtFind( tDhsBtFind, DHS_STATUS *, ... );
extern tDhsBt		*dhsBtNew( tDhsConnect *, DHS_STATUS * );

#endif /* H_BULKTRANSFER_H */

