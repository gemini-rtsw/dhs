/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Include File Name:	gen/h/dir.h
*
*   Purpose:
*	Contains defines and declarations for directory and file utilities. 
*
*   Date		: Dec 21, 1998
*
*   SCCS data		: @(#)
*	Module Name	: gen_file.h
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 02/24/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifndef _gen_file_h
#define _gen_file_h

#ifndef S_ISDIR
#define S_ISDIR(mode)   (((mode)&0xF000) == 0x4000)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)   (((mode)&0xF000) == 0x8000)
#endif

#ifndef S_ISLNK
#define S_ISLNK(mode)   (((mode)&0xF000) == 0xa000)
#endif



/*
 *  External function declarations.
 */

#ifndef VxWorks

extern	boolean	file(  char path[PATH_MAX], struct stat* stat_buf );
extern	boolean	file_mk(  char path[PATH_MAX], mode_t, int *file );
extern	boolean	file_rm(  char path[PATH_MAX] );


extern	boolean	file_dir(  char path[PATH_MAX], struct stat* stat_buf );
extern	boolean file_dir_empty(  char path[PATH_MAX] );
extern	boolean	file_dir_rm(  char path[PATH_MAX] );
extern	boolean	file_dir_mk(  char path[PATH_MAX], mode_t );

extern	boolean	file_sym(  char path[PATH_MAX], struct stat* stat_buf );
extern	boolean	file_sym_mk(  char source[PATH_MAX],
		 char dest[PATH_MAX], mode_t );
extern	boolean	file_sym_rm(  char path[PATH_MAX] );

extern	boolean	file_link(  char path[PATH_MAX], struct stat* stat_buf );
extern	boolean	file_link_mk(  char source[PATH_MAX],
		 char dest[PATH_MAX], mode_t );
extern	boolean	file_link_rm(  char path[PATH_MAX] );
#endif /* VxWorks */

#endif /* _gen_file_h */

