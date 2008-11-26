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
*   Include File Name:	mediaQueue/h/globals.h
*
*   Purpose:
*	Contains global definitions for mediaQueue
*
*   Date		: <Jan 28, 1998>
*
*   SCCS data		: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 01/28/98
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/07/31 SEC	: Put return code symbols in proper order.
*	98/09/14 SDJ	: Added new tree structure for sorting files.
*	98/11/23 SDJ	: Removed support for large files.
*	98/12/06 SDJ	: Moved configuration info into AM.
*	98/12/15 SDJ	: Fixed bug in UNIT_FULL macro (add division by 100).
*	99/01/06 SDJ	: Order msgs properly.
*	99/01/22 SDJ	: Added gNumMediaFound to global struct.
*	99/03/05 NRH	: Added globals.gRollback. Added status values used
*			  by rollback.
*	99/03/24 SDJ	: Added macro TREE_SIZE().  Added functions
*			  findDirOverhead(), processTreeInit(), and
*			  processTreeUpdate().  Altered the tree structure
*			  slightly.
*	99/03/29 NRH	: Spelling.
*	99/03/29 SDJ	: Added revSizeCompare().
*	99/03/31 SDJ	: Added UNIT_FILLED() and FILE_SIZE().  UNIT_FULL()
*			  is full only if unit is >= max size.
*     2000/02/03 SDJ	: Added msgs DB_USAGE, E_DB_USAGE, and S_DIFF_ARGS.
*			  Added tri-state truth value types.  Added global
*			  variables gCmdLineArgs, gDbArgs, gCheckDate,
*			  gCheckSize, and gPack.
*     2000/03/17 SDJ	: Added globals.gMaxUnitSize and globals.gSplitSize.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 *  Define the return code symbols.
 ************************************************************************
 */

#define S_COMPLETE	25	/* Message: Completed queuing a unit.	*/
#define	S_DB_USAGE	24	/* Database argument usage message.	*/	
#define S_DIFF_ARGS	23	/* Cmd-line and database args differ.	*/
#define S_FILE_INFO	22	/* Print tree information.		*/
#define S_FULL		21	/* Unit is full!			*/
#define S_FORCING	20	/* Message: Force-queuing a unit.	*/
#define S_MQ_SIGNAL	19	/* Signal has occured.			*/
#define S_NO_FILES	18	/* No files are ready to queue.		*/
#define S_NOT_ENOUGH	17	/* Not enuf data to write unit o'media.	*/
#define	S_NUM_ROLLBACK	16	/* Not enought units to rollback.	*/
#define S_PREPARING	15	/* Message: Preparing to queue a unit.	*/
#define	S_PT_TREE_END 	14	/* Print the directory tree.		*/
#define	S_PT_TREE_START 13	/* Print the directory tree.		*/
#define	S_PT_UNIT_END	12	/* Print the unit information.		*/
#define	S_PT_UNIT_START	11	/* Print the unit information.		*/
#define S_QUEUING	10	/* Message: Trying to queue a unit.	*/
#define S_QUIT		9	/* Quit at user's request.		*/
#define	S_ROLLBACK	8	/* Rolling back a unit.			*/
#define	S_ROLLBACK_DONE	7	/* Rollback complete to rolback a unit.	*/
#define S_TREE_INFO	6	/* Print tree information.		*/
#define S_TREE_UNIT_INFO 5	/* Print tree unit information.		*/
#define S_UNITNAME	4	/* No unit names available.		*/
#define S_UNITNAME_FMT	3	/* Error decoding unit name.		*/
#define S_USAGE		2	/* Command line usage message.		*/
#define	S_VERSION	1	/* Version message.			*/

#define	S_SUCCESS	0	/* Normal completion.			*/

#define	E_AM		(-1)	/* Error in the am library.		*/
#define E_ARG_TYPE 	(-2)	/* Error invalid arg. parsing type.	*/
#define	E_DATA_STAGE	(-3)	/* Error with the data stage.		*/
#define	E_DB		(-4)	/* Error in the sybase library.		*/
#define	E_DB_LIB	(-5)	/* Error in the CADC sybase lib.	*/
#define E_DB_USAGE	(-6)	/* Error in Database arg. usage.	*/
#define E_FILE_TOBIG	(-7)	/* File to big for media.		*/
#define E_FILESIZE	(-8)	/* Filesize does not match.		*/
#define E_MEDIA_TYPE	(-9)	/* Media ID contains bad media type.	*/
#define	E_MEMORY	(-10)	/* Memory allocation failure.		*/
#define E_NO_MEDIA	(-11)	/* Media ID missing from media table.	*/
#define E_NO_PRUNE	(-12)	/* Unable to prune file from tree.	*/
#define	E_NO_UNITS	(-13)	/* No units to roll back.		*/
#define	E_ONLY_ONE	(-14)	/* One running reqmon allowed.		*/
#define E_OPTIONS	(-15)	/* Bad options list from AM.		*/
#define	E_SYSTEM	(-16)	/* Error in a system call.		*/
#define E_TREE_TYPE	(-17)	/* Bad tree type given.			*/
#define	E_USAGE		(-18)	/* Command line usage error.		*/


/*
 ************************************************************************
 *  This is the subject used for general messages to the operator.
 ************************************************************************
 */

#define	APPLICATION		"mediaQueue"

/*
 *  APP_SHORT is the base name concatenated with the media id to form
 *  the application name used when connecting to the database. This
 *  application name should be the same application name used by the
 *  mediaIngest program, which allows mediaQueue and mediaIngest to
 *  detect whether the other program is running on the same media id.
 */

#define	APP_SHORT		"mi/q"



/*
 * Three value truth data structure
 */

typedef int	truth;

#define T_MAYBE	( (truth) -1 )
#define T_FALSE	( (truth) 0 )
#define T_TRUE	( (truth) 1 )


/*
 * Maximum number of command-line (database) arguments allowed.
 */

#define MAX_ARGS 	50


/*
 ************************************************************************
 *  Message symbols.
 ************************************************************************
 */

#define	MESS_LOG	0x01	/* To logfile.				*/
#define	MESS_CON	0x02	/* Send to console.			*/
#define	MESS_CON_VERB	0x04	/* Send to console if in verbose mode.	*/
#define	MESS_NOCR	0x08	/* No carriage return on end of line.	*/

#define MESS_ERROR      MESS_LOG | MESS_CON
				/* To console and logfile.		*/
#define MESS_INFO       MESS_LOG | MESS_CON_VERB
				/* Only to logfile unless verbose mode.	*/
#define MESS_OPER       MESS_LOG | MESS_CON
                                /* Non-error message that the operator 
				   should see - to console and logfile. */


/*+
************************************************************************
*
*   Macro Name:	CHECK
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status	(int)	Current status value.
*
*   Purpose:
*	Verify the Function return status.
*
*   Values Returned:
*	None	: if no error
*       status	: if an error occurs.
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
*   Macro Name:	CHECK_AM
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status	(int)	am library function status.
*
*   Purpose:
*	Check the success of an am library function.
*
*   Values Returned:
*	None	: if no errors occur.
*       status	: E_AM if an error occurs.
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
*   Macro Name:	CHECK_SYBASE
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status	(int)	am library function status.
*
*   Purpose:
*	Check the success of a SYBASE db libarary function.
*
*   Values Returned:
*	None	: if no errors occur.
*       status	: E_DB if an error occurs.
*
************************************************************************
-*/

#define	CHECK_DB( s )							\
	{   if ( ( s ) == FAIL )					\
	    {	formatMessage( E_DB );					\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( E_DB ); } }

/*+
************************************************************************
*
*   Macro Name:	CHECK_DB_LIB
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status	(int)	am library function status.
*
*   Purpose:
*	Check the success of a CADC db library function.
*
*   Values Returned:
*	None	: if no errors occur.
*       status	: E_DB_LIB if an error occurs.
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
*   Macro Name:	CHECK_NULL
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status	(int)	function return status.
*
*   Purpose:
*	Check that the pointer returned by a memory allocation
*  	routine is not NULL.
*
*   Values Returned:
*	None	: if no errors occur.
*       status	: E_MEMORY if an error occurs.
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
*   Macro Name:	CHECK_SYSTEM
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) status	(int)		Operating system function status.
*   (>) file	(const char*)	File name.
*
*   Purpose:
*	Check the success of an operating sysptem function.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

#define	CHECK_SYSTEM( s, file )						\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, file );			\
		printMessage( msg, MESS_ERROR );			\
		return( E_SYSTEM ); } }

/*+
************************************************************************
*
*   Macro Name:	CONV_TO_BLOCKS
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) byteVal  ( int )	Byte value.
*
*   Purpose:
*	Convert a block value into a byte value.
*
*   Values Returned:
*	( int ) : Byte value as a block value..
*
************************************************************************
-*/

#define	CONV_TO_BLOCKS( byteVal )					\
    ( ( (double)byteVal + (double)globals.gMediaType.amtBlockSize - 1 ) \
	   / (double)( globals.gMediaType.amtBlockSize ) )

     

/*+
************************************************************************
*
*   Macro Name:	FILE_SIZE
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) treePtr   ( TREE_INFO* )	Tree structure.
*
*   Purpose:
*	Find the size of a given tree.
*
*   Values Returned:
*	(double) : Size of the tree.
*
************************************************************************
-*/
#define FILE_SIZE( filePtr )	       				 	\
    ( CONV_TO_BLOCKS( filePtr->fiAmFilePtr->amfFileSize )               \
	    + globals.gMediaType.amtFileOverh )

/*+
************************************************************************
*
*   Macro Name:	FIT_ON_UNIT
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) size	  ( int )	Size to be added to unit
*   (>) unitSize  ( int )	Current size of the unit.
*
*   Purpose:
*	Verify that there is room for "size" on the unit.
*
*   Values Returned:
*       (boolean) : TRUE if the "size" value will fit on the unit.
*
************************************************************************
-*/

#define	FIT_ON_UNIT( size, unitSize )					\
    ( (double)size + unitSize <	globals.gMaxUnitSize )

/*+
************************************************************************
*
*   Macro Name:	TREE_SIZE
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) treePtr   ( TREE_INFO* )	Tree structure.
*
*   Purpose:
*	Find the size of a given tree.
*
*   Values Returned:
*	(double) : Size of the tree.
*
************************************************************************
-*/
#define TREE_SIZE( treePtr )						\
    ( treePtr->tiSubTreeSize + treePtr->tiSubTreeOverhead +             \
	    treePtr->tiFileSize + globals.gMediaType.amtDirOverh )

/*+
************************************************************************
*
*   Macro Name:	UNIT_FILLED
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) unitSize  ( int )	Size of the unit.
*
*   Purpose:
*	Convert a block value into a byte value.
*
*   Values Returned:
*	(boolean) : TRUE if unitSize > threshold but less than maxsize
*
************************************************************************
-*/

#define	UNIT_FILLED( unitSize )						\
    ( (unitSize) < globals.gMaxUnitSize					\
	&& (unitSize) > ( ( globals.gMediaType.amtThreshold / 100 ) *	\
		globals.gMaxUnitSize ) )

/*+
************************************************************************
*
*   Macro Name:	UNIT_FULL
*
*   Parameters: (">" input, "!" modified, "<" output)
*   (>) unitSize  ( int )	Size of the unit.
*
*   Purpose:
*	Convert a block value into a byte value.
*
*   Values Returned:
*	(boolean) : TRUE if unitSize > threshold but less than maxsize
*
************************************************************************
-*/

#define	UNIT_FULL( unitSize )						 \
    ( (unitSize) >= globals.gMaxUnitSize )

/*
 ************************************************************************
 *			Data Type definitions.
 ************************************************************************
 */

/*
 * Argument parsing types.
 */

typedef enum
{
    CMD_LINE_ARGS,
    DATABASE_ARGS
} ARG_TYPE;

       
/*
 * Directory tree types.
 */

typedef	enum
{
    TT_FILE,
    TT_TREE
} TREE_TYPE;


/*
 *  Structure to hold directory information.
 */


typedef struct tree_info
{
    char	*tiTreeName;		/* Directory Tree name.		*/
    char        *tiGenPath;		/* Generic path, not including
                                           at this level in the tree.   */
    double	tiSubTreeSize;		/* Size of all the subtrees.	*/
    double	tiSubTreeOverhead;	/* Overhead needed to chose	*/
    					/* the subtrees. 		*/
    double	tiFileSize;		/* Sum of all the file sizes.	*/
    time_t	tiOldestDate;		/* Date of oldest file.	        */
    EPTR	*tiFileList;		/* List of files in the dir.	*/
    EPTR	*tiTreeList;		/* List of subdirectories.	*/
    struct tree_info
    		*tiParent;		/* Parent tree structure.	*/
    boolean	tiSmallSubTrees;	/* TRUE if all sub trees less	*/
    					/*   than the medium's capacity.*/
    boolean	tiSorted;		/* TRUE if subtress are sorted.	*/
    boolean     tiIsSplit;		/* Can split this directory.    */
    char        tiMediaUnitName[AM_MEDIA_UNIT_NAME_LEN];
    					/* Media Unit dir belongs to.	*/
} TREE_INFO;


/*
 *  Structure to hold Filey information.
 */

typedef struct file_info
{
    AM_FILE	*fiAmFilePtr;		/* am File structure.		*/
    TREE_INFO	*fiParent;		/* Parent tree structure.	*/
} FILE_INFO;



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

    char	gDataStage[PATH_MAX];	/* Path to Data Stage.		*/

    double	gMaxUnitSize;		/* Maximum num of data blocks 	*/
    					/* allowed on a media unit. 	*/
    double	gSplitSize;		/* Split size in blocks.	*/

    struct amMedia
    		gMedia;			/* The media being processed.	*/
    int		gMediaId;		/* Media ID to queue.		*/
    AM_MED_TYPE	gMediaType;		/* Media type deft'n.		*/
    
    struct eptr *gTreeInfo;		/* Info about the tree stuct.   */

    char	*gCmdLineArgs;		/* Command-line args given.	*/
    char	*gDbArgs;		/* Command-line args given.	*/

    int		gNumMedia;		/* Number of media to create.	*/
    int		gNumMediaFound;		/* Number of media found in dir.*/
    boolean	gForce;			/* Force queue partial units?	*/
    boolean	gRollback;		/* Rollback a queue operation.	*/
    truth	gCheckDate;		/* Check files dates when queue?.*/
    truth	gCheckSize;		/* Check files sizes when queue?.*/
    truth	gPack;			/* Pack media as full as possible.*/
    boolean	gVerbose;		/* Run in verbose mode.		*/
    boolean	gVersion;		/* Print the version and exit.	*/
} GLOBALS;

 

/*
 ************************************************************************
 *  Declare the global symbols.
 ************************************************************************	 */

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
boolean	checkForSplit( double );
int	checkUnique( struct dbprocess *, char * );
int	cleanup( void );
int	createUnit( struct amUnit * );
int	deleteUnit( struct amUnit *, struct dbprocess * );
int	fileAdd( FILE_INFO *, char[], TREE_INFO*, int * );
int	fileCompare( FILE_INFO *, FILE_INFO * );
void	fileInfoFree( FILE_INFO * );
int	findDirOverhead( FILE_INFO *, AM_UNIT * );
void	formatMessage( int, ... );
int	globalsInit( void );
int	mediaAdd( FILE_INFO *, int, EPTR *, double ** );
int	mediaUnitCompare( AM_UNIT *, AM_UNIT * );
int	mqInit( void );
int	parseArgs( ARG_TYPE, int, char ** );
void	printMessage( char *, int );
int	printTree( TREE_INFO );
int	process( void );
int	printUnit( TREE_INFO, AM_UNIT );
int	processTreeInit( TREE_INFO * );
int	processTreeUpdate( TREE_INFO *, AM_UNIT * );
int	pruneTree( FILE_INFO *, TREE_INFO * );
int	revSizeCompare( FILE_INFO *, FILE_INFO * );
int	rollback( void );
int	treeCompare( TREE_INFO *, TREE_INFO * );
int	treeNameCompare( char *, TREE_INFO * );
void	treeFree( TREE_INFO * );
int	treeInit( TREE_INFO *, char *, TREE_INFO *);
