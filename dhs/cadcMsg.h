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
 * dhsLibrary/h/cadcMsg.h
 *
 * PURPOSE:
 * Slightly modified version of the standard CADC messaging software header
 * file.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:39  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:59  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.3  1997/01/15 20:05:53  nhill
 * Stable develop version.
 *
 * Revision 1.2  1997/01/13 17:59:20  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef CADCMSG_H
#define	CADCMSG_H

#include <stdarg.h>


/*
 *  A huge negative number is required for the MSG_ERRNO value.
 */

#define	MSG_ERRNO	(-9999)


/*
 *  Define the maximum message size.
 */

#define	MSG_MAX_LEN	2048


typedef struct	msg
{
    int		m_status;
    int		m_severity;
    char	*m_format;
} MSG;

extern	void	dhs_msg_append( char *, char * );
extern	void	dhs_msg_clear( char * );
extern	void	dhs_msg_format( char *, char *, int, MSG *, int, va_list );
extern	MSG	*dhs_msg_find( int, MSG *, int );

#endif /* CADCMSG_H */
