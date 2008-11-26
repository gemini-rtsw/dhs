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
*   Include File Name:	mfsIngest/h/globals.h
*
*   Purpose:
*	Constants, typedefs, macros and external variable declarations.
*
*   Date		: Mar 10, 1999
*
*   SCCS data		: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 05/24/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/03/10 SDJ	: Copied from mfsIngest.
*       99/05/19 SDJ    : Changed gUpper to tri-state AD_CASE value.
*     2000/03/27 SDJ    : Updated to new CADC software standards.  Switched
*                         from dvdIngest to mfsIngest.  Added messages,
*                         functions, and global variables to support switch.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************ 
 *  Constants
 ************************************************************************
 */

#define	S_FILE_PUT		12	/* Volume being ingested.	*/
#define S_MFSI_SIGNAL		11	/* mfsIngest handling signal.	*/
#define	S_NOT_REINGEST		10	/* Not re-ingesting a volume.	*/
#define	S_QUIT			9	/* User quit.			*/
#define	S_RE_INGEST		8	/* Volume has been ingested.	*/
#define S_USAGE			7	/* Print usage information.	*/	
#define S_VERSION		6	/* Print version number.	*/
#define	S_VOLUME_NAMED		5	/* Name of the mounted volume.	*/
#define	S_VOLUME_NAMEUSED	4	/* Volume name is already used.	*/
#define	S_VOLUME_NEWNAME	3	/* Print volume will be named msg.*/
#define	S_VOLUME_PUT		2	/* Filename being ingested.	*/
#define	S_VOLUME_UPDATE		1	/* Volume being updated.	*/

#define	S_SUCCESS		0	/* Successful execution.	*/

#define	E_AD_FATAL		(-1)	/* AD error with no salvage.	*/
#define	E_ARCHIVE		(-2)	/* No record with key in AD.	*/
#define	E_DB			(-3)	/* Error in database routines.	*/
#define	E_DB_LIB		(-4)	/* Error in database routines.	*/
#define	E_FILE_ID_LEN		(-5)	/* Maximum path length exceeded.*/
#define	E_LOG			(-6)	/* User error.			*/
#define	E_MEMORY		(-7)	/* Memory error.		*/
#define	E_MFS			(-8)	/* Error in the mfs library.	*/
#define	E_NO_DEFAULT		(-9)	/* No default archive.          */
#define	E_PATH_LEN		(-10)	/* Maximum path length exceeded.*/
#define	E_PRESS			(-11)	/* Error in cadc press library.	*/
#define	E_SYSTEM		(-12)	/* Error in a system call.	*/
#define	E_USAGE			(-13)	/* Incorrect usage.		*/
#define E_VOL_FORMAT		(-14)	/* Unrecognized file system.	*/
#define E_VOL_VERIFY		(-15)	/* Volume verification failed.	*/

#define MESS_LEN		128
#define EXTENSION_LEN	5


#define APPLICATION		"mfsIngest"


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
				/* Prompt user, no carriage return.	*/


/*
 ************************************************************************
 *  Prompt types.
 ************************************************************************
 */

#define	CONTINUE	0
#define	CORRECT		1
#define	NEW_VOLUME	3
#define	QUIT		4
#define	READY		5
#define REINGEST	6
#define	RENAME_VOLUME	7
#define	RETRY		8


/*
 ************************************************************************
 *  Macros
 ************************************************************************
 */

#define	CHECK(s)							\
	{   int estatus;						\
	    if ( ( estatus = (s) ) != S_SUCCESS )			\
		return(estatus); }

#define	CHECK_AD(s)						\
	{   if ( (s) != AD_SUCCESS )					\
	    {   formatMessage( E_AD_FATAL );			\
		msg_append( msg, ad_msg );				\
		msg_clear( ad_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_AD_FATAL ); } }


#define	CHECK_MFS(s)						\
	{   if( (s) != MFS_SUCCESS ) 					\
	    {   formatMessage( E_MFS );				\
		msg_append( msg, mfs_msg );				\
		msg_clear( mfs_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_MFS ); } }

#define CHECK_CONF_NULL(s)						\
	{   if ( (s) == NULL ) 						\
	    {   formatMessage( E_MEMORY );  			\
		printMessage( msg, MESS_ERROR );           		\
		return( FALSE ); } }
 
#define CHECK_DB(s)						\
	{   if ( (s) == FAIL )						\
	    {   formatMessage( E_DB );				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_DB ); } }
 
#define CHECK_DB_LIB(s)						\
	{   if ( (s) != DB_SUCCESS ) 					\
	    {   formatMessage( E_DB );      			\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_DB_LIB ); } }

#define	CHECK_NULL(s)						\
	{   if ( (s) == NULL ) 						\
	    {   formatMessage( E_MEMORY );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MEMORY ); } }

#define	CHECK_SYSTEM( s, str )					\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, str );			\
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

typedef	struct	globals
{
    boolean	gAllowReingest;		/* Allow automatic re-ingest.	*/ 
    AD_ARCHNAME	gArchive;		/* Name of the volumes archive.	*/
    boolean	gBatch;			/* Run in batch mode?		*/
    AD_CASE	gCase;			/* Case converstion?		*/
    AD_VOLCRC	gCrc;			/* Volume CRC to be ingested.	*/
    

    time_t	gCreateDate;		/* Creation date of the mfs.	*/
    struct dbprocess
		*gDbproc;		/* The dbproc to use.		*/
    FILE	*gLogFp;		/* Log file pointer		*/
    MFS_FORMAT	gMfsFormat;		/* File format from mfs lib.	*/
    AD_PATH	gMountPoint;		/* Volume moint point.		*/
    int		gNumFiles;		/* Number of files ingested.	*/
    int		gNumKbytes;		/* Number of kbytes ingested.	*/
    struct eptr	*gPathList;		/* A list of paths to ingest.	*/
    boolean	gReingest;		/* Is the volume being reingested?*/
    boolean	gVerbose;		/* Verbose mode set?		*/
    boolean	gVersion;		/* Print version and exit?	*/
    AD_VOLNAME	gVolumename;		/* Name to use for the volume.	*/
    AD_VOLTYPE	gVolumeType;		/* Type of media being read.	*/
    AD_VOLFORMAT gVolumeFormat;		/* File format on media ad lib.	*/
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

void	closeMfsIngest( void );
void	formatMessage( int, ... );
int	getType( void );
int	initGlobals( void );
int	initMfsIngest( void );
int	ingest( void );
void	printMessage( char *, int );
boolean	promptUser( int );
int	volumeLoad( void );
