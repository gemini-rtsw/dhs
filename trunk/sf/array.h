/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1999.				(c) 1999
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
 * sf/h/array.h
 *
 * PURPOSE:
 * Defines the array information structure used is the sfSds2Fits function.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1999/02/09 20:42:25  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef SF_ARRAY_INFO_H
#define	SF_ARRAY_INFO_H

typedef struct
{
    DHS_BD_ATTRIB_ID    sdsId;
    char                extName[NAME_SIZE + 9];
    long                extVer;
} SF_ARRAY_INFO;


#define SF_EXTVERSION_NULL 	0
#define SF_EXTNAME_SIZE		39

extern void	sfArrayAdd( EPTR **, DHS_BD_ATTRIB_ID, char[SF_EXTNAME_SIZE],
			SF_STATUS* );
extern SF_ARRAY_INFO *
		sfArrayFind( EPTR *, char[SF_EXTNAME_SIZE], long, SF_STATUS * );
extern void	sfArrayListFree( EPTR ** );
extern EPTR *	sfArrayListNew( void );
extern void	sfArrayDel( EPTR **, char[SF_EXTNAME_SIZE], long, SF_STATUS * );


#endif /* SF_ARRAY_INFO_H */
