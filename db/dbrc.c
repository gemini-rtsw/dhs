/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	db/src/dbrc.c
*
*   Purpose:
*	Contains several utility routines to get the usernames
*	and passwords for database connections.  If the information
*	is not found in the $HOME/.dbrc of the user then the information
*	is prompt for.
*
*	N.B.  The format of the .dbrc file is:
*		servername  databasename  username  password
*
*   Routines:
*	int		dbrc_get	: Gets username and password for db.
*	static void	int_routine	: Interupt handler for when echo is 
*					  disabled.
*	static int	prompt_info	: Prompts for username and password.
*	static int	read_info	: Reads info from ~/.dbrc
*	static int	set_echo	: Sets input echoing on (unix).
*	static int	set_noecho	: Sets input echoing off (unix).
*
*   Date		: Sep 15, 1992
*
*   SCCS data		: @(#)
*	Module Name	: dbrc.c
*	Version Number	: 1.17
*	Release Number	: 1
*	Last Updated	: 08/16/00
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	30/09/92 - nrh	: Moved to a separate library and generalized.
*	30/11/92 - nrh	: Changed to use ioctl instead of curses.
*	16/02/94 - nrh	: Fixed some solaris lint errors.
*	25/05/94 - nrh	: Move into the db library from the dbrc library.
*	16/08/00 - sjg  : Ported echo and noecho to POSIX.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "db.h"
#include "local_db.h"

static void	int_routine( int );
static int	prompt_info( char *, char *, char **, char ** );
static int	read_info( FILE	*, char	*, char	*, char	**, char ** );
static int	set_echo( void );
static int	set_noecho( void );

/*+
************************************************************************
*
*   Function: dbrc_get
*
*   Purpose:
*	If a ~/.dbrc file exists, search for the server/database username
*	and password there. If it doesn't exist or the information is
*	not found, prompt for the info.  Username and password are
*	returned as pointers to dynamically allocated strings.
*
*   Values Returned:
*	int	DB_SUCCESS	: All went well.
*	int	DB_E_IOCTL	: Error calling ioctl.
*	int	DB_E_MEMORY	: Memory allocation failure.
*	int	DB_E_NO_USER_INF: Could not find username and password.
*	int	DB_E_PERMIT	: Permitions on the .dbrc file are 
*				  incorrect.
*	int	DB_E_SYS_SERV	: Error in the system services calls.
*
************************************************************************
-*/

int		dbrc_get
( 
    char	*server,	/* (in)  Server name.			*/
    char	*database,	/* (in)  Database name.			*/
    boolean	batch,		/* (in)  If true prompting is not done.	*/
    char	**username,	/* (out) Username to use.		*/
    char	**password	/* (out) Password to use.		*/
)
{
    FILE	*fp;
    char	*path;
    struct stat	status;


    *username = NULL;
    *password = NULL;

    
    if ( server == NULL )
    {
	server = "SYBASE";
    }

    if ( database == NULL )
    {
	database = "-";
    }


    /*
     *  Get the dbrc file name.
     */

    if ( ( path = getenv( "HOME" ) ) == NULL )
    {
	DB_CHECK_NULL( path = strsav( "./" ) );
    }
    else
    {
	DB_CHECK_NULL( path = strsav( path ) );
	DB_CHECK_NULL( path = strapp( path, "/" ) );
    }
    DB_CHECK_NULL( path = strapp( path, DEFAULT_DB_FILE ) );


    /*
     *  Check the existance, ownership, and protections of the dbrc file.
     */

    if ( stat( path, &status ) != -1 )
    {
	if ( status.st_uid != getuid() || 
		( status.st_mode & ( S_IRWXO | S_IRWXG) ) != 0 )
	{
	    db_format_message( DB_E_PERMIT );
	    return( DB_E_PERMIT );
	}


	/* 
	 *  If there is a readable ~/.dbrc file, open it and find the
	 *  username and passwords there.
	 */

	if ( ( fp = fopen( path, "r" ) ) != NULL )
	{
	    DB_CHECK( read_info( fp, server, database, username,
		    password ) );

	    (void) fclose( fp );
	    gen_free( path );
	}
    }


    /*
     *   If the username is empty, prompt for it and the password.
     */

    if ( *username == NULL )
    {
	/* 
	 *  If in batch mode, return an error status if the username
	 *  and password were not found.
	 */

	if ( batch )
	{
	    db_format_message( DB_E_NO_USER_INF, server, database );
	    return( DB_E_NO_USER_INF );
	}
	else
	{
	    DB_CHECK( prompt_info( server, database, username, password) );
	}
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: int_routine
*
*   Purpose:
*	Interupt handler for when echoing is disabled.
*
*   Values Returned:
*	void
*
************************************************************************
-*/

/* ARGSUSED */
static void	int_routine
( 
    int		sig		/* (in)  The signal number.		*/
)
{
    (void) set_echo();
    exit(0);
}


/*+
************************************************************************
*
*   Function: prompt_info
*
*   Purpose:
*	Prompt for and store the username and password for database specified.
*
*   Values Returned:
*	int	DB_SUCCESS	: All went well.
*	int	DB_E_IOCTL	: Error calling ioctl.
*	int	DB_E_MEMORY	: Memory error.
*	int	DB_E_SYS_SERV	: Error in the system services calls.
*
************************************************************************
-*/

static int	prompt_info
( 
    char	*server,	/* (in)  Server name.			*/
    char	*database,	/* (in)  Database name.			*/
    char	**username,	/* (out) User name.			*/
    char	**password 	/* (out) Password.			*/
)
{
    pfv		old_sigint;
    pfv		old_sigquit;
    char	input_str[12];


    /*
     *  Get db username.
     */

    (void) fprintf( stderr,
	    "Enter information for database %s on server %s\n",
	    database, server );
    (void) fprintf( stderr, "Username: " );
    (void) gets( input_str );
    DB_CHECK_NULL( (*username) = strsav( input_str ) );


    /*
     *  Get db password, ignoring control-c until echo is
     *  turned back on.
     */

    old_sigint = signal( SIGINT, int_routine );
    old_sigquit = signal( SIGQUIT, int_routine );

    (void) fprintf( stderr, "Password: " );
    DB_CHECK( set_noecho() );
    (void) gets( input_str );
    DB_CHECK( set_echo() );
    (void) putc( '\n', stderr );
    DB_CHECK_NULL( (*password) = strsav( input_str ) );

    (void) signal( SIGINT, old_sigint );
    (void) signal( SIGQUIT, old_sigquit );

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: read_info
*
*   Purpose:
*	Check the file for the presence of the server/database 
*	and extract the username and password.  If both username and
*	password are not present, nothing is returned.
*
*   Values Returned:
*	int	DB_SUCCESS	: All went well.
*	int	DB_E_MEMORY	: Memory error.
*
************************************************************************
-*/

static int	read_info
( 
    FILE	*fp,		/* (in)  File pointer to .dbrc file.	*/
    char	*server,	/* (in)  Server name.			*/
    char	*database,	/* (in)  Database name.			*/
    char	**username,	/* (out) User name.			*/
    char	**password	/* (out) Password.			*/
)
{
    char	buffer[STRSIZ];
    int		num_tokens;
    char	*tokens[20];

    /*
     *  Look at each line of .dbrc file.
     */

    while ( fgets( buffer, STRSIZ, fp ) != NULL )
    {
	strtokens( buffer, 20, tokens, &num_tokens );
	if ( num_tokens >= 4 )
	{
	    if ( streq( server, tokens[0] ) && streq( database, tokens[1] ) )
	    {
		DB_CHECK_NULL( (*username) = strsav( tokens[2] ) );
		DB_CHECK_NULL( (*password) = strsav( tokens[3] ) );
		return( DB_SUCCESS );
	    }
	}
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Function: set_echo
*
*   Purpose:
*	Turns on echoing on standard input under unix.
*
*   Values Returned:
*	int	DB_SUCCESS	: Normal completion
*	int	DB_E_IOCTL	: Error calling ioctl.
*
************************************************************************
-*/

static int	set_echo
(
    void
)
{
    struct termios	term_status;


    /*
     *  Get the existing termios structure.
     */

    if ( tcgetattr( STDIN_FILENO, &term_status ) != 0 )
    {
	db_format_message( MSG_ERRNO, "stdin" );
	return( DB_E_IOCTL );
    }


    /*
     *  Set the echo bit.
     */

    term_status.c_lflag = term_status.c_lflag | ECHO;


    /*
     *  Set the status.
     */

    if ( tcsetattr( STDIN_FILENO, TCSAFLUSH, &term_status ) != 0 )
    {
	db_format_message( MSG_ERRNO, "stdin" );
	return( DB_E_IOCTL );
    }

    return( DB_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis: set_noecho
*
*   Purpose:
*	Turns off echoing on standard input under unix.
*
*   Values Returned:
*	int	DB_E_SUCCESS	: Normal completion
*	int	DB_E_IOCTL	: Error calling ioctl.
*
************************************************************************
-*/

static int	set_noecho
(
    void
)
{
    struct termios	term_status;


    /*
     *  Get the existing termios structure.
     */

    if ( tcgetattr( STDIN_FILENO, &term_status ) != 0 )
    {
	db_format_message( MSG_ERRNO, "stdin" );
	return( DB_E_IOCTL );
    }


    /*
     *  Clear the echo bit.
     */

    term_status.c_lflag = term_status.c_lflag & ~ECHO;


    /*
     *  Set the status.
     */

    if ( tcsetattr( STDIN_FILENO, TCSAFLUSH, &term_status ) != 0 )
    {
	db_format_message( MSG_ERRNO, "stdin" );
	return( DB_E_IOCTL );
    }

    return( DB_SUCCESS );
}
