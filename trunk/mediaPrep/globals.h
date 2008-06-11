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
*   Include File Name:	mediaPrep/h/global.h
*
*   Purpose:
*	Contains global definitions for mediaPrep
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/09/09 SDJ	: Copied from cdPrep.
*       98/11/23 SDJ  	: Removed references to GEAR and moved script info
*			  into AM libarary.
*       99/01/06 SDJ 	: Added E_EMPTY_DIR, and E_LINK msgs.
*	99/03/08 NRH	: Added support for rollback.
*	99/03/18 NRH	: Added some new messages.
*	99/04/15 SDJ	: Added S_SCRIPT_END.
*     2000/03/17 SDJ	: Added S_EXEC_COMMAND message and execCommand().
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  Constan Definitions.
 ************************************************************************
 */

/*
 *  Define the return code symbols.
 */

#define S_BEGINNING	17      /* Msg: Beginning staging (first run).	*/
#define S_CANT_PREP	16	/* Msg: Can't prepare media yet.	*/
#define S_COMPLETE      15	/* Msg: Completed writing unit to media.*/
#define S_CONTINUING	14	/* Msg: Continuing staging (not 1st run)*/
#define S_EXEC_COMMAND	13	/* Msg: Command being executed.		*/
#define	S_MP_SIGNAL	12	/* Msg: mediaPrep handling a signal.	*/
#define S_QUIT		11	/* Quit at user's request.		*/
#define	S_ROLLBACK	10	/* Begin rollback operation.		*/
#define	S_ROLLBACK_DONE	9	/* Rollback operation complete.		*/
#define S_SCRIPT_END	8       /* End of script log dump.		*/
#define S_SCRIPT_ERR	7       /* Msg: Script had errors, check log. 	*/
#define	S_SCRIPT_LOG	6	/* about to dump script log.		*/
#define	S_SCRIPT_LOG_DATA 5	/* A line from the script log.		*/
#define S_SCRIPT_PHYSVOLS 4	/* Msg: Num. physical vols script wrote.*/
#define S_STATS		3	/* Msg: statistics summary.		*/
#define S_USAGE		2	/* Command line usage message.		*/
#define	S_VERSION	1	/* Version message.			*/

#define	S_SUCCESS	0	/* Normal completion.			*/

#define	E_AM		(-1)	/* Error in the am library.		*/
#define E_BAD_F_STATUS 	(-2)	/* Error: file status wrong for mediaPrep.*/
#define E_BAD_U_STATUS 	(-3)	/* Error: unit status wrong for mediaPrep.*/
#define	E_CONFIG	(-4)	/* Error reading config file.		*/
#define	E_DATA_STAGE	(-5)	/* Error with the data stage.		*/
#define E_DB            (-6)    /* Error in the sybase library.         */
#define	E_DB_LIB	(-7)	/* Error in the CADC sybase lib.	*/
#define E_DISPLAY	(-8)	/* DISPLAY env. variable not set.	*/
#define E_EMPTY_DIR     (-9)    /* Unable to empty dir. used for hard links*/
#define	E_EXEC_ABNORMAL_EXIT (-10) /* Abnormal exit of exec'ed xterm.	*/
#define	E_EXEC_NONZERO_EXIT (-11) /* Exec'ed executable returned nonzero.*/
#define E_FILESIZE	(-12)	/* Filesize does not match.		*/
#define	E_FORK_N_EXEC	(-13)	/* Error forking/execing script xterm.	*/
#define E_LINK          (-14)	/* Unable to create symbolic link for file.*/
#define E_LOG_FMT  	(-15)	/* Script log format incorrect.		*/
#define	E_MEMORY	(-16)	/* Memory allocation failure.		*/
#define E_NO_DEVICE	(-17)	/* No device for the given media type.	*/
#define E_NO_FILES	(-18)	/* No media files found in db.		*/
#define E_NO_MEDIA	(-19)	/* Media ID missing from media table.	*/
#define E_NO_UNIT	(-20)	/* Media unit missing from units table.	*/
#define E_NOT_COMPLETE  (-21)   /* Script hasn't yet prepared all files.*/
#define	E_ONLY_ONE	(-22)	/* One running mediaPrep allowed.	*/
#define E_OTHER_UNIT	(-23)	/* Other unit 'registered' in MDS dir.	*/
#define E_STAGE_DIR	(-24)	/* Stage dir is not a directory.	*/
#define	E_SYSTEM	(-25)	/* Error in a system call.		*/
#define E_UNITNAME_FMT	(-26)	/* Error decoding unit name.		*/
#define	E_USAGE		(-27)	/* Command line usage error.		*/
#define E_WRONG_DIR	(-28)	/* Unit started in another directory.	*/

/*
 *  This is the subject used for general messages to the operator.
 */

#define	APPLICATION		"mediaPrep"
#define	APP_SHORT		"mp"


/*
 *  Message symbols.
 */

#define	MESS_LOG	0x01	/* To logfile.				*/
#define	MESS_CON	0x02	/* Send to console.			*/
#define	MESS_CON_VERB	0x04	/* Send to console if in verbose mode.	*/
#define	MESS_NOCR	0x08	/* No carriage return on end of line.	*/

#define MESS_ERROR      MESS_LOG | MESS_CON
				/* To console and logfile.		*/
#define MESS_INFO       MESS_LOG | MESS_CON_VERB
				/* Only to logfile unless verbose mode.	*/
#define MESS_OPER	MESS_LOG | MESS_CON
				/* Non-error message that the operator 
				   should see - to console and logfile.	*/


/*
 ************************************************************************
 *  Return status handling macros.
 ************************************************************************
 */


/*+
 ************************************************************************
 *
 *   Macro Name: CHECK
 *
 *   Parameters: (">" input, "!" modified, "<" output)
 *   (>) status  (int)   Current status value.
 *
 *   Purpose:
 *       Verify the Function return status.
 *
 *   Values Returned:
 *       None    : if no error
 *       status  : if an error occurs.
 *
 ************************************************************************
-*/

#define	CHECK(s)							\
	{   int		estatus;					\
	    if ( ( estatus = (s) ) < 0 || estatus == S_QUIT )		\
	 	return( estatus ); }

/*+
 ************************************************************************
 *
 *   Macro Name: CHECK_AM
 *	
 *   Parameters: (">" input, "!" modified, "<" output)
 *   (>) status  (int)   am library function status.
 *	
 *   Purpose:
 *       Check the success of an am library function.
 *
 *   Values Returned:
 *       None    : if no errors occur.
 *       status  : E_AM if an error occurs.
 *
 ************************************************************************
-*/

#define	CHECK_AM( s )							\
	{   if ( (s) != AM_SUCCESS )					\
	    {	formatMessage( E_AM );					\
		msg_append( msg, amMsg );				\
		msg_clear( amMsg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_AM ); } }

/*+
 ************************************************************************
 *
 *   Macro Name: CHECK_DB
 *
 *   Parameters: (">" input, "!" modified, "<" output)
 *   (>) status  (int)   am library function status.
 *
 *   Purpose:
 *       Check the success of a SYBASE db libarary function.	
 *
 *   Values Returned:
 *       None    : if no errors occur.
 *       status  : E_DB if an error occurs.
 *
 ************************************************************************
-*/

#define CHECK_DB( s )                                                   \
        {								\
	    if ( ( s ) == FAIL )                                        \
            {								\
		formatMessage( E_DB );                                  \
                msg_append( msg, db_msg );                              \
                msg_clear( db_msg );                                    \
                printMessage( msg, MESS_ERROR );                        \
                return( E_DB );						\
	    } 								\
	}

/*+
 ************************************************************************
 *
 *   Macro Name: CHECK_DB_LIB
 *	
 *   Parameters: (">" input, "!" modified, "<" output)
 *   (>) status  (int)   am library function status.
 *
 *   Purpose:
 *       Check the success of a SYBASE db libarary function.
 *
 *   Values Returned:
 *       None    : if no errors occur.
 *       status  : E_DB_LIB if an error occurs.
 *
 ************************************************************************
-*/

#define	CHECK_DB_LIB( s )						\
	{   if( (s) != DB_SUCCESS )					\
	    {	formatMessage( E_DB_LIB );				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_DB_LIB ); } }

/*+
 ************************************************************************
 *
 *   Macro Name: CHECK_NULL
 *
 *   Parameters: (">" input, "!" modified, "<" output)
 *   (>) status  (int)   function return status.
 *
 *   Purpose:
 *       Check that the pointer returned by a memory allocation
 *       routine is not NULL.	
 *
 *   Values Returned:
 *       None    : if no errors occur.
 *       status  : E_MEMORY if an error occurs.
 *
************************************************************************
-*/

#define	CHECK_NULL(s)							\
	{   if ( (s) == NULL )						\
	    {	formatMessage( E_MEMORY );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MEMORY ); } }

/*+
************************************************************************
*
*   Macro Name: CHECK_SYSTEM
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status  (int)   	function return status.
*   (>) file	(const char *)	Filename
*
*   Purpose:
*       Check the success of an operating sysptem function.
*
*   Values Returned:
*       None    : if no errors occur.
*       status  : E_MEMORY if an error occurs.
*
************************************************************************
-*/

#define	CHECK_SYSTEM( s, file )						\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, file );			\
		printMessage( msg, MESS_ERROR );			\
		return( E_SYSTEM ); } }

/*
 ************************************************************************
 *  Define the globals structure.
 ************************************************************************
 */

typedef	struct	globals
{
    struct dbprocess
    		*gDbProc1;		/* Dbprocess to use.		*/
    struct dbprocess
    		*gDbProc2;		/* Dbprocess to use.		*/
    FILE        *gLogFp;		/* Log file pointer.            */

    char	gMDSDirectory[PATH_MAX];/* MDS Dir to use from cmd line.*/
    struct amMedia
    		gMedia;			/* The media being processed.	*/
    int		gMediaId;		/* Media ID from cmd line.	*/
    struct amMedStage
    		gMediaStage;		/* The Media stage.		*/
    char	gMediaStagePath[PATH_MAX];	
					/* Path to Media Data Stage.	*/
    struct amUnit
    		gMediaUnit;		/* The unit being processed.	*/
    char	gMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];	
    					/* Unit name from cmd line.	*/
    int		gNumBadStatus;		/* # files that have bad status.*/
    int		gNumFiles;		/* # files (Total).		*/
    int		gNumInRS;		/* # files in Retrieval Stage.	*/
    int		gNumNeedReRetrieving;	/* # files that need retrieval. */
    int		gNumQueued;		/* # files that are Queued.    .*/
    char	gRetrievalStagePath[PATH_MAX];
    					/* Path to Retrieval Stage.	*/
    boolean	gRollback;		/* Are we doing a rollback?.	*/
    char        *gScript;		/* Name of the prep script.	*/
    char	gScriptConfigFPath[PATH_MAX];
					/* Full path to script config file.*/
    char        gScriptLogFileName[PATH_MAX];
    					/* Log file name.		*/
    boolean	gScriptKill;		/* Are we killing the script?	*/
    pid_t	gScriptProcessId;	/* Process Id of script xterm.	*/
    boolean	gVerbose;		/* Run in verbose mode.		*/
    boolean	gVersion;		/* Print the version and exit.	*/
} GLOBALS;
 

/*
 ************************************************************************
 *  Declare the global symbols.
 ************************************************************************
 */

extern  int     errno;
extern	GLOBALS	globals;
extern	char	msg[];
extern	char	*version_long;
extern	char	*version_short;


/*
 ************************************************************************
 *  Declare the global functions.
 ************************************************************************
 */

int	addToFileStats( char, const char * );
int	checkDbProc( struct dbprocess ** );
int	checkUnique( struct dbprocess *, char * );
int	cleanup( void );
int	composeDbAppName( char *, char *, int, char * );
int	execCommand( char[], int, ... );
void	formatMessage( int, ... );
int	globalsInit( void );
int  	mpInit( void );
int	prepMedia( EPTR * );
void	printMessage( char *, int );
void	printFileStats( char * );
int	process( void );
int	rollback( void );

