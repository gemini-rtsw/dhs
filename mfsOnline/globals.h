/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999
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
*   Include File Name:	mfsOnline/h/globals.h
*
*   Purpose:
*	Global definitions for program mfsOnline.
*
*   Date		: Feb 23, 1999
*
*   SCCS data           : @(#)
*       Module Name     : globals.h
*       Version Number  : 1.3
*       Release Number  : 1
*       Last Updated    : 04/10/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/02/23 SDJ  	: Copied from cdOnline.
*	99/03/22 SDJ	: Cosmetic changes to match other executables.
*     2000/04/21 SDJ	: Switched from dvdOnline to mfsOnline.
*			  Added msgs/return codes.  Updated to latest
*			  CADC standards.  Added items to global structure.
*			  Added and removed some global function prototypes.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  Define the message and return codes.
 ************************************************************************
 */

#define S_MFSO_SIGNAL		9	/* mfsOnline handling signal.	*/
#define	S_QUIT			8	/* Quit at user's request.	*/
#define S_USAGE			7	/* Usage message.		*/
#define	S_VERSION		6	/* Version message.		*/
#define S_VOLUME_LIST_ENTRY	5	/* Item in the volume list.	*/
#define S_VOLUME_LIST_FTR	4	/* Footer for volume list.	*/
#define S_VOLUME_LIST_HDR	3	/* Header for volume list.	*/
#define	S_VOLUME_MOUNTED	2	/* The volume has been mounted.	*/
#define	S_VOLUME_UNMOUNTED	1	/* The voluem has been unmounted*/

#define	S_SUCCESS		0	/* Normal completion.		*/

#define	E_AD			(-1)	/* Error in the ad library.	*/
#define	E_ARCHIVE		(-2)	/* Error in archive value.	*/
#define	E_DB_LIB		(-3)	/* Error in the CADC db lib.	*/
#define E_LOG			(-4)	/* Error opening log file.	*/
#define	E_MEMORY		(-5)	/* Memory allocation failure.	*/
#define	E_MFS			(-6)	/* Error in the mfs library.	*/
#define E_MULTI_VOLUMES		(-7)	/* More than one vol. found.	*/
#define E_NO_DEFAULT		(-8)	/* Error no default archive.	*/
#define	E_NO_VOLNAME		(-9)	/* Voluem name not in cds table.*/
#define	E_NO_VOLUME		(-10)	/* Volume not in cds table.	*/
#define E_SYSTEM		(-11)	/* Error in a system call.      */
#define	E_USAGE			(-12)	/* Command line usage error.	*/


#define	APPLICATION		"mfsOnline"
#define APP_SHORT		"mfso"


/*
 ************************************************************************
 *  Message symbols.
 ************************************************************************
 */

#define MESS_LOG        0x01    /* To logfile.                          */
#define MESS_CON        0x02    /* Send to console.                     */
#define MESS_CON_VERB   0x04    /* Send to console if in verbose mode.  */
#define MESS_NOCR       0x08    /* No carriage return on end of line.   */

#define MESS_ERROR      MESS_LOG | MESS_CON
                                /* To console and logfile.              */
#define MESS_INFO       MESS_LOG | MESS_CON_VERB
                                /* Only to logfile unless verbose mode. */
#define MESS_OPER       MESS_LOG | MESS_CON
                                /* Non-error message that the operator
                                   should see - to console and logfile. */
#define MESS_PROMPT	MESS_CON | MESS_NOCR
				/* Prompt the user for info.		*/

/*
 ************************************************************************
 *  Prompt types.
 ************************************************************************
 */

#define	CONTINUE	0
#define	CORRECT		1
#define	QUIT		2
#define	READY		3
#define	RETRY		4


/*
 ************************************************************************
 *  Return status handling macros.
 ************************************************************************
 */

#define	CHECK(s)							\
	{   int		estatus;					\
	    if ( ( estatus = (s) ) != S_SUCCESS )			\
	 	return( estatus ); }

#define	CHECK_AD( s )							\
	{   if ( (s) < 0 )						\
	    {	formatMessage( E_AD );					\
		msg_append( msg, ad_msg );				\
		msg_clear( ad_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_AD ); } }

#define	CHECK_DB_LIB( s )						\
	{   if( (s) != DB_SUCCESS )					\
	    {	formatMessage( E_DB_LIB );				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_DB_LIB ); } }

#define	CHECK_MFS( s )							\
	{   if ( (s) != MFS_SUCCESS )					\
	    {	formatMessage( E_MFS );					\
		msg_append( msg, mfs_get_message() );			\
		msg_clear( mfs_get_message() );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MFS ); } }

#define	CHECK_NULL(s)							\
	{   if ( (s) == NULL )						\
	    {	formatMessage( E_MEMORY );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MEMORY ); } }

#define CHECK_SYSTEM( s, str )                                  	\
	{   if ( (s) < 0 )                                             	\
	    {   formatMessage( MSG_ERRNO, str );			\
	        printMessage( msg, MESS_ERROR );			\
	        return( E_SYSTEM ); } }



/*
 ************************************************************************
 *  Typedefs
 ************************************************************************
 */

/*
 *  Define the globals strucutre.
 */

typedef struct globals
{
    AD_ARCHNAME	gArchive;	/* Archive mfs belongs to.		*/
    time_t	gCreateDate;	/* Creation date of the mfs.		*/
    struct dbprocess 
		*gDbproc;	/* Dbprocess to use.			*/
    FILE        *gLogFp;	/* Log file pointer.            	*/
    char	*gTarget;	/* target disk or path.			*/
    boolean	gUnmount;	/* Are we unmounting a disk?		*/
    boolean	gVersion;	/* Print version and exit?		*/
    boolean     gVerbose;	/* Run in verbose mode.         	*/
    AD_VOLTYPE	gVolumeType;	/* Type of volume mounted/unmounting	*/
} GLOBALS;


/*
 ************************************************************************
 *  External variable declarations.
 ************************************************************************
 */


extern	GLOBALS	globals;
extern	char	msg[];
extern	char	*version_long;
extern	char	*version_short;


/*
 ************************************************************************
 * Global functions prototypes.
 ************************************************************************
 */

void	closeMfsOnline( void );
void	formatMessage( int, ... );
int	getType( void );
int	initGlobals( void );
int	initMfsOnline( void );
int	mfsMount( char * );
int	mfsUnmount( char * );
void	printMessage( char *, int );
boolean	promptUser( int );

