/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Include File Name:	dvd/h/dvd.h
*
*   Purpose:
*	Global definitions for the dvd library.
*
*   Date		: March 9, 1999
*
*   SCCS data           : @(#)
*       Module Name     : dvd.h
*       Version Number  : 1.1
*       Release Number  : 1
*       Last Updated    : 03/15/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Status and message codes.
 */

#define	DVD_SUCCESS		0	/* Normal completion.		*/

#define	DVD_E_NOT_DVD		(-1)	/* Specified device not a dvd	*/
#define	DVD_E_SYSTEM		(-2)	/* Error in the system routines.*/
#define DVD_E_VSTAB		(-3)    /* vstab format error.          */

extern	int	dvd_check_mnt( char *, char *, time_t * );

extern	char	dvd_msg[];
