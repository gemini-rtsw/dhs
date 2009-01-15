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
*   Include File Name:	mediaCleanup/h/globals.h
*
*   Purpose:
*	Contains the global definitions from mediaCleanup.
*
*   Date		: Dec 12, 1996
*
*   SCCS data		: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.12
*	Release Number	: 1
*	Last Updated	: 04/29/99
*
*   Programmers		: Marc LeBlanc
*			: Steve Cockayne
*			: Shannon Jaeger
*
*   Modification History:
*	97/02/21 SEC  	: Added CHECK_AL, E_AL.
*	98/08/24 SDJ  	: Upgraded to new AM library and remove AL library.
*	99/01/07 SDJ  	: Added a few new messages.
*	99/02/24 SDJ	: Switched E_NO_MSTAGE to S_NO_MSTAGE.
*	99/03/16 NRH	: Added the gRollback flag to globals and the rollback
*			  function.
*	99/04/13 NRH	: Added a message for signal handling.
*	99/04/28 NRH	: Added E_DB added macro CHECK_DB.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


#define	S_CLEANED_UP	10	/* Msg: mediaCleanup exit-with-success.	*/
#define	S_FILE_STATE	9	/* Changing file state to blank.	*/
#define S_NO_MSTAGE     8	/* Media stage table tuple non-existent.*/
#define	S_QUIT		7	/* Quit at users request.		*/
#define S_REMOVED	6	/* File removed from retrieval area.	*/
#define	S_ROLLBACK	5	/* Rollback of unit starting.		*/
#define	S_ROLLBACK_DONE	4	/* Rollback of unit is complete.	*/
#define	S_MC_SIGNAL	3	/* Handling signal.			*/
#define S_USAGE		2	/* Command line usage message.		*/
#define	S_VERSION	1	/* Print version number.		*/

#define	S_SUCCESS	0	/* Normal completion.			*/

#define E_AM		(-1)	/* Error in the am library.		*/
#define	E_CLEAN_RSTAGE	(-2)	/* Error removing files from retr. stage*/
#define E_DATA_STAGE    (-3)	/* Error with the data stage.		*/
#define	E_DB		(-4)	/* Error in the SYBASE db library.	*/
#define E_DB_LIB	(-5)	/* Error in the CADC sybase lib.	*/
#define	E_DIR_EMPTY	(-6)	/* Error occured while emptying a dir.	*/
#define	E_DIR_RM	(-7)	/* Error removing a file.		*/
#define	E_FILE_RM	(-8)	/* Error removing a directory.		*/
#define	E_MEMORY	(-9)	/* Memory allocation failure.		*/
#define E_NO_MEDIA      (-10)	/* Media ID missing from media table.	*/
#define E_NO_SUCH_UNIT  (-11)	/* Media unit not in media_units table.	*/
#define	E_ONLY_ONE	(-12)	/* Only one running mediaCLeanup.	*/
#define	E_SYSTEM	(-13)	/* Error in sytem call.			*/
#define E_UNIT_STATUS   (-14)	/* Bad status. Can't run mediaCleanup.	*/
#define	E_UNITNAME_FMT	(-15)	/* Error decoding unit name.		*/
#define	E_USAGE		(-16)	/* Command line error.			*/


/*
 *  Application names. 
 */
 
#define	APPLICATION	"mediaCleanup"
#define	APP_SHORT	"mc"


/* 
 *  Define the message severity symbols.
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


/*
 *  Return status handling macros.
 */

#define	CHECK(s)							\
	{   int		estatus;					\
	    if ( ( estatus = (s) ) != S_SUCCESS )			\
		return( estatus ); }

#define CHECK_AM( s )                                                   \
        {   if ( (s) != AM_SUCCESS )                                    \
            {   formatMessage( E_AM );                                  \
                msg_append( msg, amMsg );                               \
                msg_clear( amMsg );                                     \
                printMessage( msg, MESS_ERROR );                        \
                return( E_AM ); } }

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

#define CHECK_DB_LIB( s )                                               \
        {   if ( (s) != DB_SUCCESS )                                    \
            {   formatMessage( E_DB_LIB );                              \
                msg_append( msg, db_msg );                              \
                msg_clear( db_msg );                                    \
                printMessage( msg, MESS_ERROR );                        \
                return( E_DB_LIB ); } }

#define	CHECK_NULL(s)							\
	{   if ( (s) == NULL )						\
	    {	formatMessage( E_MEMORY );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MEMORY ); } }

#define	CHECK_SYSTEM( s, file )						\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, file );			\
		printMessage( msg, MESS_ERROR );			\
		return( E_SYSTEM ); } }


typedef struct globals
{
    char	gDataStage[PATH_MAX];	/* The path to the Data Stage.	*/
    struct dbprocess 
		*gDbProc;		/* dbprocess to use. 		*/
    char        gLogFile[PATH_MAX];     /* Log file name.               */
    FILE	*gLogFp;		/* Log file pointer.		*/
    char	gMDSDirectory[AM_MSTAGE_DIR_LEN];
    					/* The MDS dir to clean up.	*/
    char	gMediaDataStage[PATH_MAX];
					/* The path to the MDS.		*/
    struct amMedia 
		gMedia;			/* The media structure.		*/
    int		gMediaId;		/* Media Id.			*/
    struct amUnit
		gMediaUnit;		/* The media unit structure.	*/
    char	gMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];	
					/* Media Name.			*/
    boolean	gRollback;		/* Rollback the operation?	*/
    boolean	gVerbose;		/* Run in verbose mode.		*/
    boolean	gVersion;		/* Print version and exit?	*/
} GLOBALS;


/*
 *  Declare the global symbols.
 */

extern	int	errno;
extern	GLOBALS	globals;
extern	char	msg[];
extern	char	*version_long;
extern	char	*version_short;


/*
 *  Declare the global functions.
 */

int	amFileCompare( AM_FILE *, AM_FILE * );
int	checkDbProc( struct dbprocess ** );
int     checkNewDir( AM_FILE*, EPTR* );
int	checkUnique( struct dbprocess *, char * );
int	cleanup( void );
int	composeDbAppName( );
void	formatMessage( int, ... );
int	globalsInit( void );
int	mcInit( void );
int	pathCompare( char *, char * );
void	printMessage( char *, int );
int	process( void );
int	rollback( void );
