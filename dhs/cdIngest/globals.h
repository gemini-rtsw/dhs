/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
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
*   Include File Name:	cdIngest/h/globals.h
*
*   Purpose:
*	Constants, typedefs, macros and external variable declarations.
*
*   Date		: Dec 09, 1996
*
*   Field SCCS data	: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 05/19/99
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/18 JSD  :	Added CI_E_AL and macro CI_CHECK_AL.
*	97/06/20 SEC  :	Archive needed to become a (char *) to be 
*			compatible with ad_default_archive() call. 
*	97/07/02 SEC  :	Cleaned up, and added CI_CHECK_PRESS and 
*			CI_E_PRESS macros. 
*	98/01/21 gz   : Added CI_E_NO_DEFAULT.
*	99/05/19 sjg	: Changed gUpper to tri-state AD_CASE value.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Constants
 */

#define	CI_DISK_DONE		13	/* Current disk is done.	*/
#define	CI_DISK_NAMED		12	/* Name of the mounted disk.	*/
#define	CI_DISK_NAMEUSED	11	/* Disk name is already used.	*/
#define	CI_DISK_NEWNAME		10	/* Print disk will be named msg.*/
#define	CI_DISK_PUT		8	/* Filename being ingested.	*/
#define	CI_DISK_UPDATE		7	/* Disk being updated.		*/
#define	CI_FILE_PUT		6	/* Volume being ingested.	*/
#define	CI_MUST_RENAME		5	/* Volume must be renamed.	*/
#define	CI_NOT_REINGEST		4	/* Not re-ingesting a disk.	*/
#define	CI_QUIT			3	/* User quit.			*/
#define	CI_RE_INGEST		2	/* Volume has been ingested.	*/
#define CI_VERSION		1	/* Print version number.	*/

#define	CI_SUCCESS		0	/* Successful execution.	*/

#define	CI_E_AD_FATAL		(-1)	/* AD error with no salvage.	*/
#define	CI_E_AD_NOREC		(-2)	/* No record with key in AD.	*/
#define	CI_E_AL			(-3)	/* Error in the al library.	*/
#define	CI_E_ARCHIVE		(-4)	/* No record with key in AD.	*/
#define	CI_E_CD			(-5)	/* Error in the cd library.	*/
#define	CI_E_DB			(-6)	/* Error in database routines.	*/
#define	CI_E_DB_LIB		(-7)	/* Error in database routines.	*/
#define	CI_E_DIR_READ		(-8)	/* Error reading directory.	*/
#define	CI_E_LOG		(-9)	/* User error.			*/
#define	CI_E_MEMORY		(-10)	/* Memory error.		*/
#define	CI_E_NO_DEFAULT         (-11)   /* No default archive.          */
#define	CI_E_PATH_LEN		(-12)	/* Maximum path length exceeded.*/
#define	CI_E_PRESS		(-13)	/* Error in cadc press library.	*/
#define	CI_E_SYSTEM		(-14)	/* Error in a system call.	*/
#define	CI_E_USAGE		(-15)	/* Incorrect usage.		*/

#define MESS_LEN		128
#define CI_EXTENSION_LEN	5


#define APPLICATION		"cdIngest"
#define LOGNAME_FORMAT		"cdIngest.%s.log"


/*
 *  Message types.
 */

#define	MESS_ERROR	0
#define	MESS_INFO	1
#define	MESS_PROMPT	2
#define	MESS_WARNING	3
#define MESS_REPLY	4


/*
 *  Prompt types.
 */

#define	CONTINUE	0
#define	CORRECT		1
#define	NEW_DISK	3
#define	QUIT		4
#define	READY		5
#define REINGEST	6
#define	RENAME_DISK	7
#define	RETRY		8


/*
 *  Carriage control.
 */

#define CR		TRUE
#define NO_CR		FALSE


/*
 *  Macros
 */

#define	CI_CHECK(s)							\
	{   int estatus;						\
	    if ( ( estatus = (s) ) != CI_SUCCESS )			\
		return(estatus); }

#define	CI_CHECK_AD(s)							\
	{   if ( (s) != AD_SUCCESS )					\
	    {   formatMessage( CI_E_AD_FATAL );				\
		msg_append( msg, ad_msg );				\
		msg_clear( ad_msg );					\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_AD_FATAL ); } }

#define CI_CHECK_AL(s)							\
	{   if ( (s) != AL_SUCCESS )					\
	    {   formatMessage( CI_E_AL );				\
		msg_append( msg, alMsg );				\
		msg_clear( alMsg );					\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_AL ); } }

#define	CI_CHECK_CD(s)							\
	{   if( (s) !=CD_SUCCESS ) 					\
	    {   formatMessage( CI_E_CD );				\
		msg_append( msg, cd_msg );				\
		msg_clear( cd_msg );					\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_CD ); } }

#define CI_CHECK_CONF_NULL(s)						\
	{   if ( (s) == NULL ) 						\
	    {   formatMessage( CI_E_MEMORY );  				\
		printMessage( msg, MESS_ERROR, CR );           		\
		return( FALSE ); } }
 
#define CI_CHECK_DB(s)							\
	{   if ( (s) == FAIL )						\
	    {   formatMessage( CI_E_DB );				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_DB ); } }
 
#define CI_CHECK_DB_LIB(s)						\
	{   if ( (s) != DB_SUCCESS ) 					\
	    {   formatMessage( CI_E_DB );      				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_DB_LIB ); } }

#define	CI_CHECK_NULL(s)						\
	{   if ( (s) == NULL ) 						\
	    {   formatMessage( CI_E_MEMORY );				\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_MEMORY ); } }

#define	CI_CHECK_PRESS(s)						\
	{   if ( (s) != PR_SUCCESS )					\
	    {   formatMessage( CI_E_PRESS );				\
		msg_append( msg, pr_msg );				\
		msg_clear( pr_msg );					\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_PRESS ); } }

#define	CI_CHECK_SYSTEM( s, str )					\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, str );			\
		printMessage( msg, MESS_ERROR, CR );			\
		return( CI_E_SYSTEM ); } }


/*
 *  Typedefs
 */

typedef	struct	globals
{
    boolean	gAllowReingest;		/* Allow automatic re-ingest.	*/ 
    AD_ARCHNAME	gArchive;		/* Name of the disks archive.	*/
    boolean	gBatch;			/* Run in batch mode?		*/
    AD_CASE	gCase;			/* Case conversion?		*/
    time_t	gCreateDate;		/* Creation date of the cd.	*/
    struct dbprocess
		*gDbproc;		/* The dbproc to use.		*/
    AD_VOLNAME	gDiskname;		/* Name to use for the disk.	*/
    FILE	*gLogFp;		/* Log file pointer		*/
    AD_PATH	gMountPoint;		/* Disk moint point.		*/
    int		gNumFiles;		/* Number of files ingested.	*/
    int		gNumKbytes;		/* Number of kbytes ingested.	*/
    struct eptr	*gPathList;		/* A list of paths to ingest.	*/
    boolean	gReingest;		/* Is the disk being reingested?*/
    boolean	gVerbose;		/* Verbose mode set?		*/
    boolean	gVersion;		/* Print version and exit?	*/
} GLOBALS;


/*
 *  External variable declarations.
 */

extern	GLOBALS	globals;
extern	char	msg[];

extern	char	*version_long;
extern	char	*version_short;


int	archiveCheck( void );
void	formatMessage( int, ... );
void	printMessage( char *, int, boolean );
void	closeCdIngest( void );
int	diskLoad( void );
int	initDb( void );
int	initGlobals( void );
int	ingest( void );
boolean	promptUser( int );
