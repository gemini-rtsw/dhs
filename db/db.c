/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	db/src/db.c
*
*   Purpose:
*	General sybase database access routines.
*
*   Routines:
*	int	    db_binarydata	: Saves column data to a binary array.
*	int	    db_chardata		: Saves column data to a char.
*	int	    db_close		: Closes a db connection.
*	int	    db_count_app	: Counts the number of copies of
*					  an application connnected to the db.
*	int	    db_crc32data	: Save a 32 bit crc value.
*	int	    db_crc64data	: Save a 64 bit crc value.
*   	int	    db_datetime2time	: Convert SYBASE datetime to unix time.
*	int	    db_datetimedata	: Saves column data to DBDATETIME* var.
*	int	    db_doubledata	: Saves column data to a double.
*	static int  db_err_handler	: Error handler.
*	int	    db_exit		: Exists the cadc_db library.
*	int	    db_floatdata	: Saves column data to a float.
*	int	    db_init		: Initializes the cadc db library.
*	int	    db_index_id		: Determines ind_id of col in table.
*	int	    db_intdata		: Saves column data to an int.
*	int	    db_smallintdata	: Saves column data to an int.
*	int	    db_tinyintdata	: Saves column data to an int.
*	static int  db_msg_handler	: Message handler.
*	int	    db_open		: Initializes a db connection.
*	int	    db_simple_query	: Simple query function.
*	long	    db_str2time		: Convert SYBASE string to unix time.
*	void	    db_strcpy		: Copy a string, duplicating single 
*					  quotes.
*	int	    db_stringdata	: Saves column data to a string. 
*	char	    *db_time2str	: Convert unix time to SYBASE string.
*	int	    db_timedata		: Save a time value to a time_t.
*	static int  dp_compare		: Compares a dbprocess with an process
*					  structure.
*	static pfv  dp_free		: Frees the memory allocated to a proc.
*
*   Date		: Sep 09, 1993
*
*   SCCS data		: @(#)
*	Module Name	: db.c
*	Version Number	: 1.39
*	Release Number	: 1
*	Last Updated	: 04/06/00
*
*   Programmer		: Norman Hill
*
*   Modification History:
*	95/03/07 Mark Healey  :	Added db_stringdata, db_floatdata, db_intdata.
*	95/08/03 Paul W.      :	Added hostname parameter to db_count_app.
*	95/08/16 NRH  :	Added function db_strcpy.
*	95/09/26 NRH  :	Added function db_binarydata.
*	96/01/18 SEC  :	Modified db_simple_query - incresaed maximum
*			strlen(format) to 2048 - see man page.
*	96/02/07 SEC  :	Added function db_datetimedata().
*	96/07/30 DHS  :	Corrected the comments in db_strcpy so that 
*			they reflect the functionality of the arguments.
*	97/07/02 SEC  :	Updated to ANSI.
*	97/11/02 pdd  : Added db_doubledata()
*	97/12/17 pdd  : added db_smallintdata() and db_tinyintdata().
*	99/10/14 sdj  : Cleaned up for lint.
*	00/04/06 nrh  : Added db_crc32data() and db_crc64data().
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "db.h"
#include "local_db.h"

EPTR		*db_peroc_list;
LOCAL_DB	local_db;

static int	dp_compare( PROCESS *, PROCESS * );
static void	dp_free( PROCESS * );
static int	db_err_handler( DBPROCESS *, int, int, int, char *, char * );
static int	db_msg_handler( DBPROCESS *, DBINT, int, int, char *, 
			char *, char *, int );

/*+
************************************************************************
*
*   Function:	db_binarydata
*
*   Purpose:
*	To save the column data into a binary array.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value was NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_TRUNCATED	: Array was truncated.
*	int	DB_E_BAD_COLUMN : Invalid column specified.
*
************************************************************************
-*/

int	db_binarydata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  Column to get the string from.	*/
    byte	*array,		/* (out) The binary array to return.	*/
    int		bytes		/* (in)  The size of the array.		*/
)
{
    int		i;
    int		len;
    int		status;

    status = DB_SUCCESS;

    len = dbdatlen( db, column );

    if ( len < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_binarydata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    if ( len > bytes )
    {
	len = bytes;
	status = DB_TRUNCATED;
    }
    (void) memcpy( array, (DBBINARY *) dbdata( db, column ), len );

    for ( i = len; i < bytes; i++ )
    {
	array[i] = 0;
    }

    return( status );
}

/*+
************************************************************************
*
*   Function:	db_chardata
*
*
*   Purpose:
*	To save the column data to a character variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*
************************************************************************
-*/

int	db_chardata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    char	*c		/* (out) The variable to copy to.	*/
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_chardata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *c = *( (DBCHAR *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: db_close
*
*   Purpose:
*	Closes a dbprocess and removes it from the current list of 
*	processes.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_E_NOT_EXISTS	: Specified db process doesn't exist in the 
*				  list of processes.
*
************************************************************************
-*/

int		db_close
( 
    DBPROCESS	*dbprocess	/* (in)  Dbprocess to close.		*/
)
{
    int		index;		/* Position of the dbproc in the list.	*/


    /*
     *  Find the dbprocess in the list.
     */

    if ( eptr_search( local_db.ld_proc_list, (pointer) &dbprocess, &index, 
	    dp_compare ) )
    {
	eptr_del( local_db.ld_proc_list, index, 1, dp_free );
    }
    else
    {
	db_format_message( DB_E_NOT_EXISTS );
	return( DB_E_NOT_EXISTS );
    }

    dbclose( dbprocess );

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_count_app
*
*   Purpose:
*	Counts the number of distinct copies of a specified application
*	attached to the database.  The application argument should be
*	the same as was used in the db_init function.  The application
*	string can contain sybase wild card characters.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*	int	DB_E_DB		: Database error.
*
************************************************************************
-*/

int		db_count_app
(
    DBPROCESS	*dbprocess,	/* (in)  Dbprocess to close.		*/
    char	*application,	/* (in)  The application to query.	*/
    char	*hostname,	/* (in)  The hostname to query.		*/
    int		*num_copies	/* (out) The number of copies running.	*/
)
{
    DB_CHECK_DB( dbfcmd( dbprocess, 
	    "select \n"
	    "    count( distinct hostname + convert( varchar, hostprocess ) )\n"
	    "from \n"
	    "    master..sysprocesses \n"
	    "where \n"
	    "    program_name like '%s' \n",
	    application ) );
    
    if ( hostname != NULL ) 
    {
	DB_CHECK_DB( dbfcmd( dbprocess,
	    " and \n"
	    "    hostname like '%s' \n", hostname ) );
    }
	
    DB_CHECK_DB( dbsqlexec( dbprocess ) );
    DB_CHECK_DB( dbresults( dbprocess ) );

    if ( ( DBROWS( dbprocess ) == FAIL ) || 
	    ( dbnextrow( dbprocess ) == NO_MORE_ROWS ) )
    {
	*num_copies = 0;
    }
    else
    {
	DB_CHECK( db_intdata( dbprocess, 1, num_copies ) );
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_crc32data
*
*   Purpose:
*	To save the column data to a 32 bit variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*
************************************************************************
-*/

int	db_crc32data
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    unsigned long		/* (out) The variable to copy to.	*/
		*i
)
{
    if ( dbdatlen( db, column ) < 4 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_crc32data" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *i = (*( (DBBINARY *) dbdata( db, column ) ) ) << 24 | 
	    (*( (DBBINARY *) dbdata( db, column ) + 1 ) ) << 16 |
	    (*( (DBBINARY *) dbdata( db, column ) + 2 ) ) << 8 |
	    *( (DBBINARY *) dbdata( db, column ) + 3 );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_crc64data
*
*   Purpose:
*	To save the column data to a 64 bit variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*
************************************************************************
-*/

int	db_crc64data
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    unsigned long long		/* (out) The variable to copy to.	*/
		*i
)
{
    int		j;
    byte	value[8];

    if ( dbdatlen( db, column ) < 8 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_crc64data" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    DB_CHECK( db_binarydata( db, column, value, 8 ) );

    for ( *i = 0, j = 0; j < 8; j++ )
    {
	*i = *i << 8;
	*i |= value[j];
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_datetime2time
*
*   Purpose:
*	Convert a sybase datetime to a unix time value.
*
*   Values Returned:
*	long	: Unix time coresponding to the sybase time.
*
************************************************************************
-*/

time_t	db_datetime2time
(
    DBDATETIME	*sybase_time	/* (in) Sybase time to convert.		*/
)
{
    time_t	unix_time;

    unix_time = (sybase_time->dtdays - DB_DAYS1900TO1970)*DB_SECSPERDAY;
    unix_time += (sybase_time->dttime/300);

    return( unix_time );
}

/*+
************************************************************************
*
*   Function:   db_datetimedata
*
*
*   Purpose:
*       To save the column data to a character variable.
*
*   Values Returned:
*       int     DB_NULL_VALUE   : Value is NULL.
*       int     DB_SUCCESS      : Successful execution.
*       int     DB_E_BAD_COLUMN : Invalid column specified.
*
************************************************************************
-*/
 
int     db_datetimedata
(
    DBPROCESS   *db,            /* (in)  The dbprocess to use.          */
    int         column,         /* (in)  The column to get data from.   */
    DBDATETIME  *datetime       /* (out) The variable to copy to.       */
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
        db_format_message( DB_E_BAD_COLUMN, column, "db_datetimedata" );
        return( DB_E_BAD_COLUMN );
    }
 
    if ( dbdatlen( db, column ) == 0 )
    {
        return( DB_NULL_VALUE );
    }
 
    *datetime = *( (DBDATETIME *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_doubledata
*
*
*   Purpose:
*	To save the column data to a double variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value was NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Bad column specified.
*
************************************************************************
-*/

int	db_doubledata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    double	*f		/* (out) The double data.		*/
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_doubledata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *f = *( (DBFLT8 *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: db_err_handler
*
*   Purpose:
*       Handles the printing of the DB-Library error messsages.
*	Statement of purpose.
*
*   Values Returned:
*	int		INT_CANCEL	: Return fail from the dblib function.
*
************************************************************************
-*/

/*ARGSUSED*/
static int	db_err_handler
( 
    DBPROCESS	*dbproc,		/* (in)  Database process.	*/
    int		severity,		/* (in)  Severity of error.	*/
    int		dberr,			/* (in)  Database error code.	*/
    int		oserr,			/* (in)  OS error code.		*/
    char	*dberrstr,		/* (in)  Database error message.*/
    char	*oserrstr		/* (in)  OS error message.	*/
)
{
    if ( dberr != DBNOERR && dberrstr != NULL )
    {
	db_format_message( DB_E_DB_LIBRARY, dberr, dberrstr );
    }
    if ( oserr != DBNOERR && oserrstr != NULL )
    {
	db_format_message( DB_E_DB_LIBRARY, oserr, oserrstr );
    }

    return( INT_CANCEL );
}

/*+
************************************************************************
*
*   Function: db_exit
*
*   Purpose:
*	Does the final exit from the cadc db library.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		db_exit
(
    void
)
{
    int		i;
    PROCESS	*process;


    for ( i = 0; i < eptrlen( local_db.ld_proc_list ); i++ )
    {
	process = (PROCESS *) eptritem( local_db.ld_proc_list, i );
	(void) dbclose( process->pr_dbproc );
    }
    eptr_free( local_db.ld_proc_list, dp_free );
    dbexit();

    if ( local_db.ld_log_fp != NULL )
    {
	(void) fclose( local_db.ld_log_fp );
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_floatdata
*
*
*   Purpose:
*	To save the column data to a float variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value was NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Bad column specified.
*
************************************************************************
-*/

int	db_floatdata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    float	*f		/* (out) The float data.		*/
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_floatdata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *f = *( (DBFLT8 *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: db_init
*
*   Purpose:
*	Initialize this library.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_E_DB		: An error in the db library.
*	int	DB_E_MEMORY	: Memory allocation failure.
*	int	DB_E_SYSTEM	: A unix system error occurred.
*
************************************************************************
-*/

int		db_init
( 
    char	*application	/* (in)  Name of the application.	*/
)
{
    char	*log_path;
    char	*log_file;


    if ( dbinit() == FAIL )
    {
	db_format_message( DB_E_DB );
	return( DB_E_DB );
    }


    /*
     *  if the DB_LOG_DIR environment variable is set, open a db.log file.
     */

    if ( ( log_path = getenv( "DB_LOG_DIR" ) ) != NULL )
    {
	DB_CHECK_NULL( log_file = strsav( log_path ) );
	DB_CHECK_NULL( log_file = strapp( log_file, "/db.log" ) );
	if ( ( local_db.ld_log_fp = fopen( log_file, "w" ) ) == NULL )
	{
	    db_format_message( MSG_ERRNO, log_file );
	    return( DB_E_SYSTEM );
	}
	gen_free( log_file );
    }
    else
    {
	local_db.ld_log_fp = NULL;
    }


    DB_CHECK_NULL( local_db.ld_application = strsav( application ) );


    dberrhandle( db_err_handler );
    dbmsghandle( db_msg_handler );


    /*
     *  Initialize the list of processes.
     */

    DB_CHECK_NULL( local_db.ld_proc_list = eptr_alloc( 10 ) );

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_index_id
*
*   Purpose:
*	To determine the ind_id for a specific column in a 
*	specific table.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_NO_INDEX	: Index not found.
*	int	DB_E_DB		: Database error.
*	int	DB_E_MEMORY	: Out of memory error.
*
************************************************************************
-*/

int		db_index_id
(
    DBPROCESS	*dbproc,	/* (in)  Process pointer. 		*/
    char	*database_name,	/* (in)  Name of database. 		*/
    char	*table_name,	/* (in)  Name of table. 		*/
    char	*col_name,	/* (in)  Name of column. 		*/
    int		*ind_id 	/* (out) Index id. 			*/
)
{
    int		db_status;
    int		status;

    *ind_id = 0;
    status = DB_NO_INDEX;

    db_status = dbfcmd( dbproc, 
	    "select \n"
	    "	indid \n"
	    "from \n"
	    "	%s..sysindexes \n"
	    "where \n"
	    "	index_col( '%s..%s', indid, 1 ) = '%s' \n"
	    "	    and id = object_id( '%s..%s' ) \n",
	    database_name, database_name, table_name, col_name, 
	    database_name, table_name );
    DB_CHECK_DB( db_status );

    DB_CHECK_DB( dbsqlexec( dbproc ) );
    DB_CHECK_DB( dbresults( dbproc ) );

    if ( DBROWS( dbproc ) == SUCCEED )
    {
	DB_CHECK_DB( dbnextrow( dbproc ) );
	if ( dbdata( dbproc, 1 ) != NULL )
	{
	    *ind_id = *( (DBSMALLINT *) dbdata( dbproc, 1 ) );
	    status = DB_SUCCESS;
	}
    }

    DB_CHECK_DB( dbcanquery( dbproc ) );

    return( status );
}

/*+
************************************************************************
*
*   Function:	db_intdata
*
*   Purpose:
*	To save the column data to an integer variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*
************************************************************************
-*/

int	db_intdata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    int		*i		/* (out) The variable to copy to.	*/
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_intdata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *i = *( (DBINT *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_smallintdata
*
*   Purpose:
*	To save the column data to an integer variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*
************************************************************************
-*/

int	db_smallintdata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    int		*i		/* (out) The variable to copy to.	*/
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_smallintdata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *i = *( (DBSMALLINT *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_tinyintdata
*
*   Purpose:
*	To save the column data to an integer variable.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value is NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_E_BAD_COLUMN	: Invalid column specified.
*
************************************************************************
-*/

int	db_tinyintdata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  The column to get data from.	*/
    int		*i		/* (out) The variable to copy to.	*/
)
{
    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_tinyintdata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    *i = *( (DBTINYINT *) dbdata( db, column ) );
    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: db_msg_handler
*
*   Purpose:
*	Handles the printing of the SQL-Server error messsages.  These
*	messages have a severity > 10.  Otherwise these are just
*	informational messages which can be ignored.
*
*   Values Returned:
*	int		0	: Normal return.
*
************************************************************************
-*/

/*ARGSUSED*/
static int	db_msg_handler
( 
    DBPROCESS	*dbproc,		/* (in)  Database process.	*/
    DBINT	msgno,			/* (in)  Server message code.	*/
    int		msgstate,		/* (in)  Error state code.	*/
    int		severity,		/* (in)  Severity of error.	*/
    char	*msgtext, 		/* (in)  Error message.		*/
    char	*srvname,		/* (in)  Server name.		*/
    char	*procname,		/* (in)  Stored procedure name.	*/
    int		line			/* (in)  Line number of error.	*/
)
{
#ifdef DEBUG
    char	*buffer;
    int		len;
#endif


    db_errno = msgno;

    if ( severity > 10 )
    {
	db_format_message( DB_E_SQL_SERVER, msgno, msgtext );

#ifdef DEBUG
	/*
	 *  Print the command that caused the error.
	 */

	len = dbstrlen( dbproc );
	if ( len > 0 )
	{
	    buffer = gen_alloc( len + 1 );
	    (void) dbstrcpy( dbproc, 0, len, buffer );
	    db_format_message( DB_E_SQL_SERVER, msgno, buffer );
	    gen_free( buffer );
	}
#endif
    }

   return( 0 );
}

/*+
************************************************************************
*
*   Function: db_open
*
*   Purpose:
*	Initialize a sybase db connection.  If username and password
*	are null, it will use the dbrc to get them.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_E_DB		: Error accessing the database.
*	int	DB_E_IOCTL	: Error calling ioctl.
*	int	DB_E_MEMORY	: Memory allocation failure.
*	int	DB_E_NO_USER_INF: Could not find username and password.
*	int	DB_E_PERMIT	: Permitions on the .dbrc file are 
*				  incorrect.
*	int	DB_E_SYS_SERV	: Error in the system services calls.
*
************************************************************************
-*/

int		db_open
( 
    char	*server,	/* (in)  Sybase server to use.		*/
    char	*username,	/* (in)  Sybase username to use.	*/
    char	*password,	/* (in)  Sybase password to use.	*/
    char	*db,		/* (in)  Sybase db to use.		*/
    boolean	prompt,		/* (in)  Prompt for username and 	*/
				/*       password if they are null and	*/
				/*	 not in the dbrc file?		*/
    DBPROCESS	**dbproc	/* (out) Opened db process.		*/
)
{
    int		index;
    boolean	user_info_malloced;
    char	*local_username;
    char	*local_password;
    LOGINREC	*login;
    PROCESS	*process;


    if ( server == NULL && ( server = getenv( "DSQUERY" ) ) == NULL )
    {
	server = "SYBASE";
    }


    if ( username == NULL || password == NULL )
    {
	DB_CHECK( dbrc_get( server, db, !prompt, 
		&local_username, &local_password ) );
	username = local_username;
	password = local_password;
	user_info_malloced = TRUE;
    }
    else
    {
	user_info_malloced = FALSE;
    }


    /*
     *  Open the connection to the source database.
     */

    login = dblogin();
    DBSETLAPP( login, local_db.ld_application );
    DBSETLUSER( login, username );
    DBSETLPWD( login, password );
    BCP_SETL( login, TRUE );

    if ( user_info_malloced )
    {
	gen_free( username );
	gen_free( password );
    }


    *dbproc = dbopen( login, server );
    if ( *dbproc == NULL )
    {
	db_format_message( DB_E_DB );
	return( DB_E_DB );
    }

    DB_CHECK_DB( dbuse( *dbproc, db ) );

    
    /*
     *  Save the information about this dbprocess.
     */

    DB_CHECK_NULL( process = process_alloc() );

    process->pr_dbproc = *dbproc;
    process->pr_dblogin = login;
    DB_CHECK_NULL( process->pr_db = strsav( db ) );

    (void) eptr_search( local_db.ld_proc_list, process, &index, dp_compare );
    
    DB_CHECK_NULL( eptr_insert( local_db.ld_proc_list, index, process ) );

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: db_simple_query
*
*   Purpose:
*	Executes a simple query, ie., one that does not retrieve data.
*
*   Values Returned:
*	int	DS_SUCCESS	: Successful completion.
*	int	DB_E_DB		: Database error.
*
************************************************************************
-*/

int		db_simple_query
( 
    DBPROCESS	*dbproc, 		/* (in)  Database process id.	*/
    char	*format, 		/* (in)  Printf style format.	*/
    ... 				/* (in)  Arguments to format.	*/
)
{
    va_list	args;
    char	*buffer;
    int		status;


    DB_CHECK_NULL( buffer = 
	    char_alloc( MAX( 2048, (int) strlen( format ) * 2 ) ) );
    va_start( args, format );
    (void) vsprintf( buffer, format, args );
    va_end( args );

    status = dbcmd( dbproc, buffer );
    gen_free( buffer );
    DB_CHECK_DB( status );
    DB_CHECK_DB( dbsqlexec( dbproc ) );
    if ( dbresults( dbproc ) == FAIL )
    {
	db_format_message( DB_E_DB );
	return( DB_E_DB );
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_str2time
*
*   Purpose:
*	Convert a sybase date/time string (as produced by convert with
*	style 109) to a unix time value.
*
*   Values Returned:
*	long			: Unix time coresponding to the sybase time.
*	long	DB_E_DATE_FORMAT: Invalid date format.
*
************************************************************************
-*/

long	db_str2time
(
    char	*sybase_time,	/* (in)	 Sybase time to convert.	*/
    int		timezone	/* (in)	 Time zone of the sybase string.*/
				/*	 (DB_UTC or DB_LOCAL_TIME).	*/
)
{
    char	am_pm[3];
    char	cmonth[4];
    int		i;
    static char	old_tz[10];
    int		time;
    struct tm	tm;
    static char *month_list[] = { "Jan", "Feb", "Mar", "Apr", "May",
	"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


    gen_zero( &tm, sizeof( tm ) );
    (void) sscanf( sybase_time, "%3s %d %d %d:%d:%d:%*d%2s", 
	    cmonth, &(tm.tm_mday), &(tm.tm_year), &(tm.tm_hour), 
	    &(tm.tm_min), &(tm.tm_sec), am_pm );
    if ( tm.tm_year > 1900 )
    {
	tm.tm_year -= 1900;
    }


    for ( tm.tm_mon = -1, i = 0; i < 12; i++ )
    {
	if ( streq( cmonth, month_list[i] ) )
	{
	    tm.tm_mon = i;
	    break;
	}
    }


    if ( streq( am_pm, "PM" ) )
    {
	if ( tm.tm_hour != 12 )
	{
	    tm.tm_hour += 12;
	}
    }
    else
    {
	if ( tm.tm_hour == 12 )
	{
	    tm.tm_hour = 0;
	}
    }


    if ( tm.tm_mon == -1 ) 
    {
	db_format_message( DB_E_DATE_FORMAT, sybase_time );
	return( DB_E_DATE_FORMAT );
    }


    if ( timezone == DB_UTC )
    {
	if ( getenv( "TZ" ) != NULL )
	{
	    sprintf( old_tz, "TZ=%s", getenv( "TZ" ) );
	}
	else
	{
	    old_tz[0] = '\0';
	}
	(void) putenv( "TZ=UST" );
	tm.tm_isdst = 0;
	time = mktime( &tm );
	(void) putenv( old_tz );
    }
    else
    {
	tm.tm_isdst = -1;
	time = mktime( &tm );
    }

    return( time );
}

/*+
************************************************************************
*
*   Function:	db_strcpy
*
*   Purpose:
*       This routine copies a string, changing single quotes (') to pairs
*       of single quoes so they can be loaded into sybase.  The output
*       string is always null terminated.   And any trailing spaces are
*       removed.  The output string must be large enough to contain the
*	modified string (in the worst case, the output string will be
*	twice the length of the input string).
*
*   Values Returned:
*	none
*
************************************************************************
-*/

void		db_strcpy
(
    char	*out_addr,	/* (out) The destination string address.*/
    char	*in_addr	/* (in)  The source string address.	*/
)
{
    int         i;
    int         ti;
 
 
    for ( i = 0, ti = 0; ; i++, ti++ )
    {
        out_addr[ti] = in_addr[i];
        if ( in_addr[i] == '\'' )
        {
            ti ++;
            out_addr[ti] = '\'';
        }
        if ( in_addr[i] == '\0' )
        {
            break;
        }
    }
    out_addr[ti] = '\0';
    for ( i = strlen( out_addr ) - 1; i > 0; i -- )
    {
 
        if ( out_addr[i] != ' ' )
        {
            break;
        }
    }
    out_addr[i + 1] = '\0';
 
}

/*+
************************************************************************
*
*   Function:	db_stringdata
*
*   Purpose:
*	To save the column data into a string and null terminate the string.
*
*   Values Returned:
*	int	DB_NULL_VALUE	: Value was NULL.
*	int	DB_SUCCESS	: Successful execution.
*	int	DB_TRUNCATED	: String "c" was truncated.
*	int	DB_E_BAD_COLUMN : Invalid column specified.
*
************************************************************************
-*/

int	db_stringdata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  Column to get the string from.	*/
    char	*c,		/* (out) The string to return.		*/
    int		bytes		/* (in)  The size of the char array.	*/
)
{
    int		len;
    int		status;

    status = DB_SUCCESS;

    len = dbdatlen( db, column );

    if ( len < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_stringdata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    if ( len > ( bytes - 1 ) )
    {
	len = bytes - 1;
	status = DB_TRUNCATED;
    }
    (void) strncpy( c, (DBCHAR *) dbdata( db, column ), len );
    c[len] = '\0';
    (void) strfit( c );
    return( status );
}

/*+
************************************************************************
*
*   Function:	db_table_exists
*
*   Purpose:
*	Determines if a specified table exists.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_E_DB		: Error in the db library.
*
************************************************************************
-*/

int		db_table_exists
(
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    char	*db_name,	/* (in)  The sybase db name.		*/
    char	*table_name,	/* (in)  The table name.		*/
    boolean	*exists		/* (out) Set true if table table exists.*/
)
{
    DB_CHECK( db_simple_query( dbproc, 
	    "select object_id( \"%s..%s\" )", db_name, table_name ) );
    
    DB_CHECK_DB( dbnextrow( dbproc ) );

    if ( dbdatlen( dbproc, 1 ) == 0 )
    {
	*exists = FALSE;
    }
    else
    {
	*exists = TRUE;
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	db_time2str
*
*   Purpose:
*	Format a time string usable by sybase as input to a datetime.
*
*   Values Returned:
*	char	*timestr	: The sybase time string.
*
************************************************************************
-*/

char	*db_time2str
(
    long	unix_time,	/* (in)  Unix time to format.		*/
    int		timezone	/* (in)	 Time zone of the sybase string.*/
				/*	 (DB_UTC or DB_LOCAL_TIME).	*/
)
{
    static char	buffer[41];	/* Buffer to return string in.		*/


    if ( timezone == DB_UTC )
    {
	(void) strftime( buffer, 40, "%b %d %Y %H:%M:%S", 
		gmtime( (time_t *) &unix_time ) );
    }
    else
    {
	(void) strftime( buffer, 40, "%b %d %Y %H:%M:%S", 
		localtime( (time_t *) &unix_time ) );
    }

    return( buffer );
}

/*+
************************************************************************
*
*   Function:	db_timedata
*
*   Purpose:
*	Saves a sybase time value into a time_t.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*	int	DB_NULL_VALUE	: The time is null.
*	int	DB_E_BAD_COLUMN	: Bad column number.
*
************************************************************************
-*/

int		db_timedata
(
    DBPROCESS	*db,		/* (in)  The dbprocess to use.		*/
    int		column,		/* (in)  Column to get the string from.	*/
    time_t	*time_t		/* (out) The time returned.		*/
)
{ 
    DBDATETIME	*sybase_time;	/* The sybase version of the time.	*/


    if ( dbdatlen( db, column ) < 0 )
    {
	db_format_message( DB_E_BAD_COLUMN, column, "db_timedata" );
	return( DB_E_BAD_COLUMN );
    }

    if ( dbdata( db, column ) == NULL )
    {
	return( DB_NULL_VALUE );
    }

    sybase_time = (DBDATETIME *) dbdata( db, column );

    *time_t = db_datetime2time( sybase_time );

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis: dp_compare
*
*   Purpose:
*	Compares two process structures. 
*
*   Values Returned:
*	int	-1		: process1 < process2
*	int	0		: process1 == process2
*	intg	1		: process1 > process2
*
************************************************************************
-*/

static int	dp_compare
( 
    PROCESS	*process1,	/* (in)  First process to compare.	*/
    PROCESS	*process2	/* (in)  Second process to compare.	*/
)
{
    if ( process1->pr_dbproc < process2->pr_dbproc )
    {
	return( -1 );
    }
    else if ( process1->pr_dbproc == process2->pr_dbproc )
    {
	return( 0 );
    }
    else
    {
	return( 1 );
    }
}

/*+
************************************************************************
*
*   Function: dp_free
*
*   Purpose:
*	Frees the memory allocated to a process structure.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion.
*
************************************************************************
-*/

static void	dp_free
( 
    PROCESS	*process	/* (in)  The process to free.		*/
)
{
    dbloginfree( process->pr_dblogin );
    if ( process->pr_db != NULL )
    {
	gen_free( process->pr_db );
    }
    gen_free( process );

}
