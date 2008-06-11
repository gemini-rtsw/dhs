/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998				(c) 1998.
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
*   Include File Name:	tpIngest/h/globals.h
*
*   Purpose:
*	Constants, typedefs, macros and external variable declarations.
*
*   Date		: Feb 20, 1998
*
*   Field SCCS data	: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 05/19/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/04/09 SDJ	: Added msgs for fork() and exec().  Remove AL
*			  message.  Added messages and global variables
*			  for signal handling.
*	99/05/19 sjg	: Changed g_upper to tri-state AD_CASE value.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Constants
 */

#define	TI_AUTO_REINGEST	14	/* Auto reingest.		*/
#define	TI_DONE			13	/* Finish ingest.		*/
#define	TI_END_OF_FILE		12	/* Reach the end of a file.	*/
#define	TI_ENTER_TAPE_NAME	11	/* Prompt to enter tape name.	*/
#define	TI_FILE_PUT		10	/* File being ingested.		*/
#define	TI_FILE_UPDATE		9	/* Update the file.		*/
#define	TI_QUIT			8	/* Quit tpIngest. 		*/
#define	TI_READY		7	/* Prompt user to put the tape.	*/
#define	TI_RE_INGEST		6	/* Reingest the tape.		*/
#define TI_SIGNAL		5	/* Processing a signal.		*/
#define	TI_TAPE_PUT		4	/* Tape being ingested.		*/
#define	TI_TAPE_UPDATE		3	/* Tape being updated.		*/
#define TI_USAGE		2	/* Usage message.		*/
#define	TI_VERSION		1	/* Print version number.	*/

#define	TI_SUCCESS		0	/* Successful execution.	*/

#define	TI_E_AD_FATAL		(-1)	/* AD error with no salvage.	*/
#define	TI_E_DB			(-3)	/* Error in database routines.	*/
#define	TI_E_DB_LIB		(-4)	/* Error in database routines.	*/
#define TI_E_EXEC_ABNORMAL_EXIT	(-5)	/* Abnormal exit from script.	*/
#define TI_E_EXEC_NONZERO_EXIT	(-6)	/* Script returned nonzero val.	*/
#define TI_E_FORK_N_EXEC	(-7)	/* Error fork/exec'ing script.	*/
#define	TI_E_GET_FILE		(-8)	/* Error in processing a file.	*/
#define	TI_E_MEMORY		(-9)	/* Memory error.		*/
#define	TI_E_NO_ARCHIVE         (-10)	/* Unrecognized archive.	*/
#define	TI_E_NO_DEFAULT         (-11)	/* No default archive.          */
#define	TI_E_NO_NAME		(-12)	/* No name for the tape.	*/
#define	TI_E_OPEN_FILE		(-13)	/* Error in opening a file.	*/
#define	TI_E_PRESS		(-14)	/* Error in cadc press library.	*/
#define	TI_E_SCRIPT		(-15)	/* Error in executing script.	*/
#define	TI_E_SYSTEM		(-16)	/* Error in a system call.	*/
#define	TI_E_TP			(-17)	/* Error in the tp library.	*/
#define	TI_E_USAGE		(-18)	/* Incorrect usage.		*/

#define MESS_LEN		128
#define TI_EXTENSION_LEN	5
#define TP_TAPE_NAME_LEN	32
#define TP_FILE_NAME_LEN	64


#define APPLICATION		"tpIngest"
#define LOGNAME_FORMAT		"tpIngest.%s.log"


/*
 *  _message types.
 */

#define	MESS_ERROR	0
#define	MESS_INFO	1
#define	MESS_WARNING	2
#define MESS_REPLY	3


/*
 *  Prompt types.
 */

#define	READY		0
#define REINGEST	1


/*
 *  Carriage control.
 */

#define CR		TRUE
#define NO_CR		FALSE


/*
 *  Macros
 */

#define	TI_CHECK(s)							\
	{   int estatus;						\
	    if ( ( estatus = (s) ) != TI_SUCCESS )			\
		return(estatus); }

#define	TI_CHECK_AD(s)							\
	{   if ( (s) != AD_SUCCESS )					\
	    {   format_message( TI_E_AD_FATAL );			\
		msg_append( msg, ad_msg );				\
		msg_clear( ad_msg );					\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_AD_FATAL ); } }

#define	TI_CHECK_TP(s)							\
	{   if( (s) !=TP_SUCCESS ) 					\
	    {   format_message( TI_E_TP );				\
		msg_append( msg, tp_msg );				\
		msg_clear( tp_msg );					\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_TP ); } }

#define TI_CHECK_CONF_NULL(s)						\
	{   if ( (s) == NULL ) 						\
	    {   format_message( TI_E_MEMORY );  			\
		print_message( msg, MESS_ERROR, CR );           	\
		return( FALSE ); } }
 
#define TI_CHECK_DB(s)							\
	{   if ( (s) == FAIL )						\
	    {   format_message( TI_E_DB );				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_DB ); } }
 
#define TI_CHECK_DB_LIB(s)						\
	{   if ( (s) != DB_SUCCESS ) 					\
	    {   format_message( TI_E_DB );      			\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_DB_LIB ); } }

#define	TI_CHECK_NULL(s)						\
	{   if ( (s) == NULL ) 						\
	    {   format_message( TI_E_MEMORY );				\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_MEMORY ); } }

#define	TI_CHECK_PRESS(s)						\
	{   if ( (s) != PR_SUCCESS )					\
	    {   format_message( TI_E_PRESS );				\
		msg_append( msg, pr_msg );				\
		msg_clear( pr_msg );					\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_PRESS ); } }

#define	TI_CHECK_SYSTEM( s, a )						\
	{   if ( (s) != 0 )						\
	    {   format_message( MSG_ERRNO, a );				\
		print_message( msg, MESS_ERROR, CR );			\
		return( TI_E_SYSTEM ); } }


/*
 *  Typedefs
 */

typedef	struct	globals
{
    boolean	g_auto_reingest;
    AD_ARCHNAME g_archive;		/* Name of the tapes archive.	*/
    AD_CASE	g_case;			/* Case conversion?		*/
    struct dbprocess
		*g_dbproc;		/* The dbproc to use.		*/
    char	*g_device;		/* The device of tape.		*/
    boolean	g_kill;			/* TRUE when tpIngest is being	*/
    					/* terminated.			*/
    FILE	*g_log_fp;		/* Log file pointer		*/
    int		g_num_files;		/* Number of files ingested.	*/
    int		g_num_kbytes;		/* Number of kbytes ingested.	*/
    pid_t	g_script_pid;		/* Process ID of script spawned.*/
    boolean 	g_reingest;		/* Allow reingest.		*/
    AD_VOLNAME	g_tape_name;		/* Name to use for the tape.	*/
    char	g_tape_type[8];		/* The physical type of a tape.	*/
    char	*g_type;		/* The data type of the data.	*/
    TP_SCRIPTS 	*g_tp_script;		/* The script used for ingest.	*/
    boolean	g_verbose;		/* Verbose mode set?		*/
    boolean	g_version;		/* Print version and exit?	*/
} GLOBALS;


/*
 *  External variable declarations.
 */

extern	GLOBALS	globals;
extern	char	msg[];

extern	char	*version_long;
extern	char	*version_short;


void	format_message( int, ... );
void	print_message( char *, int, boolean );

int	archive_check( void );
int	execute_script( char *, char * );
int	get_tape_name( void );
int	init_db( void );
int	init_globals( void );
int	ingest( void );
boolean	prompt_user( int );
