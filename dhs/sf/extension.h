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
 * sf/h/extnsion.h
 *
 * PURPOSE:
 * Internal SF definitions of the extension list.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1998/04/30 06:16:20  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef SF_EXTENSION_H
#define	SF_EXTENSION_H


#define SF_NULL_EXTVER  -1


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern  SF_EXT_INFO 	*sfAddExt( SF_DS_INFO*, char[], SF_STATUS* );
extern  SF_EXT_INFO 	*sfAddExtVer( SF_DS_INFO*, SF_EXT_INFO*, SF_STATUS* );
extern  void        	sfDelExt( EPTR*, EPTR*, SF_EXT_INFO*, SF_STATUS* );
extern	int 		sfExtCompare( SF_EXT_INFO*, SF_EXT_INFO* );
extern 	void		sfExtFree( SF_EXT_INFO* );
extern	int 		sfExtVerCompare( SF_EXT_INFO*, SF_EXT_INFO* );
extern  SF_EXT_INFO 	*sfFindExt( EPTR*, char[], SF_STATUS* );
extern	SF_EXT_INFO	*sfFindExtVer( EPTR*, char[], long, SF_STATUS* );
extern	int		sfFindLastExtVer( EPTR*, char[], SF_STATUS* );



#endif /* SF_EXTENSION_H */
