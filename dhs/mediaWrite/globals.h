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
*   Include File Name:	mediaWrite/h/globals.h
*
*   Purpose:
*	Contains global definitions for mediaWrite
*
*   Date		: Sep 09, 1998
*
*   SCCS data		: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 04/15/99
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*       98/09/09 SDJ    : Copied from cdPrep
*       98/10/21 SDJ  	: Added support for device booking.  Also added
*                        retrieval of script name from AM.
*       98/11/23 SDJ  	: Finalized device booking, added CHECK_SYSTEM
*                         macro.
*	99/01/24 SDJ	: Added retrieval staging area as a global.
*	99/03/08 NRH	: Added support for rollback and writing a specific
*			  number of copies of a unit.
*	99/03/14 SDJ	: Removed extra deft'n configInit().
*	99/03/18 NRH	: Added S_SCRIPT_LOG, S_SCRIPT_LOG_DATA, and
*			  S_NO_COPIES messages.
*	99/03/25 SDJ	: Added E_DEV_NOT_FOUND and removed gScriptProcessId.
*	99/04/08 NRH	: Added the gSimulate option. Added E_DEVICE_TYPE.
*	99/04/15 SDJ	: Added S_SCRIPT_END.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 ************************************************************************
 *  Constant Definitions.
 ************************************************************************
 */

/*
 *  Define the return code symbols.
 */
#define S_BEGINNING	16	/* Msg: Beginning mediaWrite.		*/
#define S_COMPLETE      15	/* Msg: Completed writing unit to Media.*/
#define	S_COPIES_WRITTEN 14	/* Inform about the number of disks written.*/
#define S_DEV_SELECTED	13	/* Msg: Selected device.		*/
#define S_MW_SIGNAL	12	/* Msg: mediaWrite handling a signal.	*/
#define	S_NO_COPIES	11	/* No copies of the unit are to be written.*/
#define S_QUIT		10	/* Quit at user's request.		*/
#define	S_ROLLBACK	9	/* Starting rollback.			*/
#define	S_ROLLBACK_DONE	8	/* Finished rollback.			*/
#define S_SCRIPT_END	7	/* End of script log dump.              */
#define S_SCRIPT_ERR	6       /* Msg: script had errors - check log.	*/
#define	S_SCRIPT_LOG	5	/* Copy Script log to mediaWrite log.	*/
#define	S_SCRIPT_LOG_DATA 4	/* Line from script log file.		*/
#define S_SCRIPT_NUM_DONE 3	/* Msg: How many copies script wrote.	*/
#define S_USAGE		2	/* Command line usage message.		*/
#define	S_VERSION	1	/* Version message.			*/

#define	S_SUCCESS	0	/* Normal completion.			*/

#define	E_AM		(-1)	/* Error in the am library.		*/
#define E_BAD_U_STATUS 	(-2)	/* Error: unit status wrong for mediaWrite*/
#define	E_CONFIG	(-3)	/* Error reading config file.           */
#define	E_DATA_STAGE	(-4)	/* Error with the data stage.		*/
#define E_DEV_NOT_FOUND	(-5)	/* Logical device not found in AM.	*/
#define E_DEVICE_BUSY	(-6)	/* Device is busy, unable to book.	*/
#define	E_DEVICE_TYPE	(-7)	/* Logical device doesn't match device	*/
				/* type.				*/
#define E_DB            (-8)    /* Error in the sybase library.         */
#define	E_DB_LIB	(-9)	/* Error in the CADC sybase lib.	*/
#define E_DISPLAY	(-10)	/* DISPLAY env. variable not set.	*/
#define	E_EXEC_ABNORMAL_EXIT (-11) /* Abnormal exit of exec'ed xterm.	*/
#define	E_EXEC_NONZERO_EXIT (-12) /* Exec'ed executable returned nonzero.*/
#define	E_FORK_N_EXEC	(-13)	/* Error forking and execing script's xterm.*/
#define E_LOG_FMT  	(-14)	/* Script's log format incorrect.	*/
#define	E_MEMORY	(-15)	/* Memory allocation failure.		*/
#define E_NO_DEVICE	(-16)	/* No device matching media type.	*/
#define E_NO_MEDIA	(-17)	/* Media ID missing from media table.	*/
#define	E_NO_ROLLBACK	(-18)	/* There is nothing to roll back.	*/
#define E_NO_UNIT	(-19)	/* Media unit missing from units table.	*/
#define E_NOT_COMPLETE  (-20)   /* script hasn't yet written all copies.*/
#define E_NUM_COPIES	(-21)	/* Script didn't write all of its copies*/
#define	E_ONLY_ONE	(-22)	/* One running mediaWrite allowed.	*/
#define E_STAGE_DIR	(-23)	/* Stage dir is not a directory.	*/
#define	E_SYSTEM	(-24)	/* Error in a system call.		*/
#define E_UNITNAME_FMT	(-25)	/* Error decoding unit name.		*/
#define E_UNIT_UNREG	(-26)	/* Unit not registered in database.	*/
#define	E_USAGE		(-27)	/* Command line usage error.		*/


/*
 *  This is the subject used for general messages to the operator.
 */

#define	APPLICATION		"mediaWrite"
#define	APP_SHORT		"mw"



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
        {   if ( ( s ) == FAIL )                                        \
            {   formatMessage( E_DB );                                  \
                msg_append( msg, db_msg );                              \
                msg_clear( db_msg );                                    \
                printMessage( msg, MESS_ERROR );                        \
                return( E_DB ); } }

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
*   (>) fn	(const char *)	Function name.
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

#define	CHECK_SYSTEM( s, fn )						\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, fn );			\
		printMessage( msg, MESS_ERROR );			\
		return( E_SYSTEM ); } }

/*
 ************************************************************************
 *  Define the globals structure.
 ************************************************************************
 */

typedef	struct	globals
{
    char	gDataStage[PATH_MAX];	/* Path to Data Stage.		*/
    struct dbprocess
    		*gDbProc1;		/* Dbprocess to use.		*/
    struct dbprocess
    		*gDbProc2;		/* Dbprocess to use.		*/
    
    FILE        *gLogFp;                /* Log file pointer.            */

    char        *gScript;               /* Name of the prep script.     */
    char        gScriptConfigFPath[PATH_MAX];
    					/* Path to script config file.	*/
    char        gScriptLogFileName[PATH_MAX];
                                        /* Log file name.               */
    boolean     gScriptKill;            /* Are we killing the script?   */
    EPTR        *gPidList;		/* List of child process iD's.	*/

    
    int		gNumCopies;		/* The number of copies to write.*/
    char	gMDSDirectory[AM_MSTAGE_DIR_LEN];
    					/* MDS Dir to use from cmd line.*/
    struct amMedia gMedia;		/* The media being processed.	*/
    struct amMedStage
    		gMediaStage;	        /* The media stage that is used.*/
    char	gMediaStagePath[PATH_MAX];	
					/* Path to Media Data Stage.	*/
    char        gRetrievalStagePath[PATH_MAX];
                                        /* Path to Retrieval Stage.     */
    int		gMediaId;		/* Media ID from cmd line.	*/
    struct amUnit gMediaUnit;		/* The unit being processed.	*/
    char	gMediaUnitName[AM_MEDIA_REQUEST_NAME_LEN];	
    					/* Unit name from cmd line.	*/

    char	gLogicalDeviceName[AM_LOGICAL_DEV_NAME_LEN];
    					/* Logical devices to be used.	*/
    EPTR	*gDeviceList;		/* Devices to be used.		*/
    boolean	gForce;			/* TRUE forces writing.		*/
    boolean	gRollback;		/* Rollback write?		*/
    boolean	gSimulate;		/* Simulate write?		*/
    
    
    int         gNumDirs;               /* # of directories necessary.  */
    int         gNumFiles;              /* # files (Total).             */
    int         gNumInMDS;              /* # files in Media Data Stage. */
    int         gNumInDS;               /* # files in Data Stage.       */
    int         gNumNeedReRetrieving;   /* # files that need reretrieve.*/
    int         gNumBadStatus;          /* # files that have bad status.*/
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

int	checkDbProc( struct dbprocess ** );
int	checkUnique( struct dbprocess *, char * );
int	cleanup( void );
int	composeDbAppName( char *, char *, int, char * );
int	deviceUpdate( struct amDevice * );
int	mwInit( void );
void	formatMessage( int, ... );
int	globalsInit( void );
void	printMessage( char *, int );
int	rollback( void );
int	writeMedia( void );
