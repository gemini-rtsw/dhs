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
*   Include File Name:	mfs/h/mfs.h
*
*   Purpose:
*	Global definitions for the mfs library.
*
*   Date		: March 9, 1999
*
*   SCCS data           : @(#)
*       Module Name     : mfs.h
*       Version Number  : 1.3
*       Release Number  : 1
*       Last Updated    : 04/07/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*     2000/03/06 SDJ	: Changed to "mfs" library from "dvd" library.
*			  Added MFS_ISO9660, MFS_UDF, MFS_UFS, MFS_UNKNOWN,
*			  and MFS_FORMAT.  Updated to new CADC standards.
*     2000/04/07 SDJ	: Added MFS_ISO9660_JB.  CD's in the juke box
*			  need to be treated differently than those
*			  in a CD-ROM drive.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Status and message codes.
 */

#define	MFS_SUCCESS	0	/* Normal completion.			*/

#define	MFS_E_NOT_MFS	(-1)	/* Specified device not a recognized	*/
				/* mountable file sys.			*/
#define	MFS_E_SYSTEM	(-2)	/* Error in the system routines.	*/
#define MFS_E_VSTAB	(-3)    /* vstab format error.          	*/


/*
 * Global defines
 */

#define MFS_ISO9660_JB	"hrfs"
#define MFS_ISO9660	"hsfs"
#define MFS_UDF_JB	"udf"
#define MFS_UFS		"ufs"
#define MFS_UNKNOWN	""


#define MFS_LEN		10

/*
 * Type definietions.
 */

typedef	char	MFS_FORMAT[ MFS_LEN ];


/*
 * External function prototypes.
 */

extern	int	mfs_check_mnt( char *, char *, time_t *, char * );
extern	char	*mfs_get_message( void );


