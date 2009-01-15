/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1995				(c) 1995.
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
*   Include File Name:	cd/h/local_cd.h
*
*   Purpose:
*	Local definitions for the cd library.
*
*   Date		: Apr 11, 1995
*
*   SCCS data		: @(#)
*	Module Name	: local_cd.h
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 04/19/95
*
*   Programmer		: Norm Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#define	VSTAB		"/etc/vstab"


#define	CD_CHECK_SYSTEM( s, str )					\
	{   if ( ( s ) < 0 )						\
	    {	cd_format_message( MSG_ERRNO, str );			\
		return( CD_E_SYSTEM ); } }

void	cd_format_message( int, ... );
