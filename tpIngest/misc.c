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
*   Module Name:	tpIngest/src/misc.c
*
*   Purpose:
*	Contains several utility routines for tpingest.
*
*   Routines:
*	int	archive_check	: Checks that the archive is valid.
*	int 	execute_script	: execute the script.
*	int	get_tape_name	: Get the name of the tape.
*	int	init_db		: Initialize resources for tpingest.
*	boolean init_globals	: Initializes the global data structure.
*	int	load_tape	: Prompts for and confirms a tape load.
*
*   Date		: Feb 20, 1998
*
*   Field SCCS data	: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 05/19/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/04/09 SDJ	: Changed system() to a fork() and exec() so
*			  it can be killed by a signal.  Fixed tmpnam()
*			  usage.
*	99/05/19 sjg	: Changed g_upper to tri-state AD_CASE value.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "ad.h"
#include "tp.h"
#include "db.h"

#include "globals.h"


/*+
************************************************************************
*
*   Function:	archive_check
*
*   Purpose:
*	Checks that the given or the default archive is valid.
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well
*	int	TI_E_NO_ARCHIVE	: Archive name not valid.
*	int	TI_E_NO_DEFAULT	: No default archive.
*
************************************************************************
-*/

int    archive_check
(
    void
)
{
    AD_ARCHIVE	*archive;

    /*
     *  If archive not specified on cmd line, set to ad's default. 
     *  Otherwise, verify that it is a valid archive.
     */ 

    if ( strlen( globals.g_archive ) == 0 )
    {
	archive = ad_archive_info( NULL );
	if ( archive == NULL )
	{
	    format_message( TI_E_NO_DEFAULT );
	    print_message( msg, MESS_ERROR, CR );
	    return( TI_E_NO_DEFAULT );
	}
	(void) strcpy( globals.g_archive, archive->ada_name );
    }
    else
    {
    	archive = ad_archive_info( globals.g_archive );
	if ( archive == NULL )
	{
	    format_message(TI_E_NO_ARCHIVE, globals.g_archive );
	    print_message( msg, MESS_ERROR, CR );
	    return( TI_E_NO_ARCHIVE );
	}
    }

    globals.g_case = archive->ada_case;
    return( TI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	execute_script	
*
*   Purpose:
*	Execution the script
*
*   Parameters:
*	char 	*script		: (in) The script to execute.
*	char 	*file_name	: (in) The name of output file.
*	FILE	*out_file	: (out)The result after execute the script.
*
*   Values Returned:
*	int 	TI_SUCCESS	: All went well.
*	int	TI_E_SCRIPT	: Error in opening a file.
*	int	TI_E_SYSTEM	: Error in calling a system function.
*
************************************************************************
-*/

int 	execute_script
(
    char	*script,	/* (in) The script to execute. 		*/
    char	*file_name	/* (in) The name of output file.	*/
)
{
    int		exec_status;	/* Status of exec()			*/
    FILE	*fp;		/* File pointer to the output file.	*/
    char	line[PATH_MAX];	/* The line of status_log file.		*/
    int		line_len;	/* The lenth of the line.		*/
    char	status_log[L_tmpnam];
    				/* Log the status while execute script.	*/
    int		status;		/* Function return value.		*/

    
    status = TI_SUCCESS;
    (void) tmpnam( status_log );

    if ( ! globals.g_kill )
    {
	if ( ( globals.g_script_pid = fork() ) == 0 )
	{
	    if ( execlp( script, script, globals.g_device, file_name,
			    status_log, (char *) NULL ) == -1 )
	    {
		format_message( TI_E_FORK_N_EXEC, script, globals.g_device,
			file_name, status_log );
		print_message( msg, MESS_ERROR, CR );
		_exit( TI_E_FORK_N_EXEC );
	    }
	}
    }
    if (  waitpid( globals.g_script_pid, &exec_status, 0 ) == -1
	    &&  status == TI_SUCCESS )
    {
	format_message( MSG_ERRNO, script );
	print_message( msg, MESS_ERROR, CR );
	status = TI_E_SYSTEM;
    }


    /*
     *  Check out the return value of the exec'ed process. 
     */
         
    if ( !WIFEXITED( exec_status ) )
    {
	format_message( TI_E_EXEC_ABNORMAL_EXIT, script );
	print_message( msg, MESS_ERROR, CR );
	status = TI_E_SYSTEM;
    }
    else if ( WEXITSTATUS( exec_status ) != 0 )
    {
	format_message( TI_E_EXEC_NONZERO_EXIT, script,
		WEXITSTATUS( exec_status ) - 256 );
	print_message( msg, MESS_ERROR, CR );
	status = TI_E_SYSTEM;
    }


    /*
     * Read the status file - check for errors
     */
    
    if ( status == TI_SUCCESS &&
	    ( fp = fopen( status_log, "r" ) ) != NULL )
    {
	while( fgets( line, sizeof( line ), fp ) != NULL )
	{
	    line_len = strlen( line );
	    line[ line_len - 1 ] = '\0';
	    format_message( TI_E_SCRIPT, script, status_log, line );
	    print_message( msg, MESS_ERROR, CR );
	}
	(void) fclose( fp );
	TI_CHECK_SYSTEM(  unlink( status_log ), "unlink" );
    }
    else
    {
	/*
	 * If the file was deleted then no errors occured.
	 */
    }
    

    return( status );
}

/*+
************************************************************************
*
*   Function:	get_tape_name	
*
*   Purpose:
*	Get the name of the tepe.
*
*   Values Returned:
*	int 	TI_SUCCESS	: All went well.
*	int	TI_QUIT		: Quit tpIngest.
*	int	TI_E_GET_FILE	: Error in get file info.
*	int	TI_E_OPEN_FILE	: Error in opening a file.
*	int	TI_E_SCRIPT	: Error in executing script.
*	int	TI_E_SYSTEM	: Error in calling a system function.
*
************************************************************************
-*/

int	get_tape_name
(
   void 
)
{
    char	buffer[1024];
    FILE	*fp;
    int		status = TI_SUCCESS;
    char 	tmp_file[L_tmpnam];


    if ( globals.g_tp_script->tp_name == NULL ) 
    {
        format_message( TI_ENTER_TAPE_NAME );
	print_message( msg, MESS_ERROR, NO_CR );
	
	if ( gets( buffer ) != NULL )
	{
	    (void) strfit( buffer );
	    (void) strncpy( globals.g_tape_name, buffer, 
		    AD_VOLUMENAME_LEN );
	    globals.g_tape_name[AD_VOLUMENAME_LEN] = '\0';
	}

	if ( strlen( globals.g_tape_name ) == 0 )
	{
	    format_message( TI_E_NO_NAME );
	    print_message( msg, MESS_ERROR, CR );
	    status = TI_E_NO_NAME;
	}
    }
    else
    {
	(void) tmpnam( tmp_file );


	/*
	 * Execute a script to get the volume name.
	 */

	status = execute_script( 
	    globals.g_tp_script->tp_name, tmp_file );
	if ( status != TI_SUCCESS )
	{
	    TI_CHECK_SYSTEM( unlink( tmp_file ), "unlink" );
	    TI_CHECK( status );
	}


	/*
	 * Read the contents of the file generated.
	 */

	if ( ( fp = fopen( tmp_file, "r" ) ) == NULL )
	{
	    format_message( MSG_ERRNO, tmp_file );
	    print_message( msg, MESS_ERROR, CR );
	    return( TI_E_SYSTEM );
	}
	

    	if ( fscanf( fp, "%s", buffer ) != 1 )
	{
	    format_message( TI_E_NO_NAME );
	    print_message( msg, MESS_ERROR, CR );
            status = TI_E_NO_NAME;
	}

	(void) fclose( fp );
    
        TI_CHECK_SYSTEM( unlink( tmp_file ), "unlink" );
	(void) strncpy( globals.g_tape_name, buffer,
	     AD_VOLUMENAME_LEN );

    }
    
    return( status );
}

/*+
************************************************************************
*
*   Synopsis:
*	int	init_db( )
*
*   Purpose:
*	Initialize the resources needed for tpingest's operation.
*
*   Parameters:
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well.
*	int	TI_E_AD		: Error in the ad library.
*	int	TI_E_AL		: Error in the al library.
*	int	TI_E_DB_LIB	: Database library error occurred.
*
************************************************************************
-*/

int	init_db
(
    void
)
{
    char	*dbname;
    char	*dbserver;

    TI_CHECK_DB_LIB( db_init( APPLICATION ) );

    /*
     *  Get the database name, and open a connection.
     */

    TI_CHECK_AD( ad_dbinfo_get( AD_MED_TP, &dbserver, &dbname, NULL, NULL ) );
    TI_CHECK_DB_LIB( db_open( dbserver, NULL, NULL, dbname,
	    FALSE, &globals.g_dbproc ) );


    return( TI_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*       int     init_globals()
*
*   Purpose:
*       Initialize the globals variables.
*
*   Parameters:
*       None.
*
*   Values Returned:
*       int	TI_SUCCESS		: All went well
*
************************************************************************
-*/

int	 init_globals
(
    void
)
{
    globals.g_auto_reingest = FALSE;
    globals.g_reingest = FALSE;
    globals.g_verbose = FALSE;
    globals.g_version = FALSE;

    globals.g_num_files = 0;
    globals.g_num_kbytes = 0;

    globals.g_archive[0] = '\0';
    globals.g_dbproc = NULL;
    globals.g_device = NULL;
    globals.g_log_fp = NULL;
    globals.g_tape_name[0] = '\0';
    globals.g_tp_script	= NULL;
    globals.g_tape_type[0] = '\0';
    globals.g_type = NULL;

    return( TI_SUCCESS );
}
