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
 * sf/h/frameInfo.h
 *
 * PURPOSE:
 * Defines the frame information sturcture for sds2fits conversion.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.2  1998/05/14 18:52:32  jaeger
 * Added function definitions for sfFrameCompare and sfFrameFind.
 *
 * Revision 1.1  1998/04/30 06:17:11  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef SF_FRAME_INFO_H
#define	SF_FRAME_INFO_H

typedef struct
{
    DHS_BD_FRAME	sdsId;
    char		frmId[FLEN_VALUE];
    char		frmName[FLEN_VALUE];
    char		extName[FLEN_VALUE];
    long		extVer;
    boolean		hasData;
} SF_FRAME_INFO;

#define SF_NULL_EXTVER 	-1

extern int		sfFrameCompare( SF_FRAME_INFO*, SF_FRAME_INFO* );
extern SF_FRAME_INFO    *sfFrameFind( EPTR*, char[], SF_STATUS* );
extern EPTR		*sfFrameListCreate( int, SF_STATUS* );
extern void		sfFrameListDelete( EPTR*, SF_STATUS* );
extern void		sfFrameListInsert( EPTR**, SF_FRAME_INFO*, SF_STATUS* );
extern SF_FRAME_INFO	*sfNewFrameInfo( char[FLEN_VALUE], const char *,
				DHS_BD_FRAME, SF_STATUS* );

    
#endif /* <SF_FRAME_INFO_H> */
