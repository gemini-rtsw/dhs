/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	db/h/local_db.h
*
*   Purpose:
*	Contains the local definitions for the db library.
*
*   Date		: Sept 10, 1993
*
*   SCCS data		: @(#)
*	Module Name	: local_db.h
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 5/25/94
*
*   Programmer		: Norman Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#define	DEFAULT_DB_FILE	".dbrc"	/*  dbrc file name.			*/


typedef struct process
{
    struct dbprocess	*pr_dbproc;	/* The dbprocess.		*/
    struct loginrec	*pr_dblogin;	/* The db login record.		*/
    char		*pr_db;		/* The current database.	*/
} PROCESS;

typedef struct local_db
{
    FILE	*ld_log_fp;		/* File pointer to the log file.*/
    boolean	ld_timeout;		/* A timeout has occured.	*/
    char	*ld_application;	/* Name of the application.	*/
    struct eptr	*ld_proc_list;		/* An eptr list of PROCESS'.	*/
} LOCAL_DB;

extern	LOCAL_DB	local_db;	/* Global variables.		*/

#define	process_alloc()	((PROCESS *) gen_alloc( sizeof( PROCESS ) ) )

#define	DB_CHECK(s)		{ int estatus;				\
				  if((estatus=(s))!=DB_SUCCESS)		\
					return(estatus); }

#define DB_CHECK_DB(s)          { if ((s)==FAIL){			\
				    db_format_message( DB_E_DB );	\
				    return(DB_E_DB); } }

#define DB_CHECK_NULL(ptr)      { if ((ptr)==NULL) {			\
				    db_format_message( DB_E_MEMORY );	\
				    return( DB_E_MEMORY ); } }


#define DB_CHECK_SIGVEC(s)      { if ((s) != 0 ) {                    \
				    db_format_message( MSG_ERRNO, "sigvec" );\
				    return( DB_E_SIGVEC ); } }


void	db_format_message( int, ... );
