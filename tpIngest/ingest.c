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
*   Module Name:	tpIngest/src/ingest.c
*
*   Purpose:
*	Contains the routines to ingest an tape.
*
*   Routines:
*	static int	add_file_rec	: Adds a file to the odfile table.
*	static int	add_tape_rec	: Adds a tape to the ods table.
*	static int	check_tape_table: Checks if a specific tape is
*					  in the tps table.
*	int		ingest		: The ingest procedure.
*	static int	ingest_tape	: Ingests a tape.
*
*   Date		: Feb 17, 1998 
*
*   SCCS data		: @(#)
*	Module Name	: ingest.c
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 05/19/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/04/09 SDJ	: removed on_control_c().  Fixe tmpnam() usage.
*			  Changed args to execute_script
*	99/04/19 NRH	: Added extraction of uncompressed file size from
*			  the tape ingest script.
*	99/05/19 sjg	: Changed g_upper to tri-state AD_CASE value.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "ad.h"
#include "db.h"
#include "tp.h"
#include "press.h"
#include "globals.h"

static int 	ingest_tape();
static	int    	get_file ( FILE *, char *, char *, int *, int * );

/*+
************************************************************************
*
*   Function:	static int	add_file_rec
*
*   Purpose:
*	Add a file to the file table of the archive directory.
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well.
*	int	TI_E_MEMORY	: Memory error.
*	int	an AD error	: An AD error.
*
************************************************************************
-*/

static int	add_file_rec
(
    char	*file_name,	/* The file name.			*/
    char	*location,	/* The file position on the tape.	*/
    int		file_size,	/* The size of the file.		*/
    int		uncomp_size	/* The uncompressed size of the file.	*/
)
{
    AD_FILE	ad_file;
    char	compression;
    AD_FILE_ID	file_id;
    char	format;
    boolean	new;		/* Is this a new file?			*/	
    boolean	modified;	/* Has the current file changed?	*/
    int		status;

    TI_CHECK_AD( ad_file_init( &ad_file, AD_MED_TP ) );
    

    modified = FALSE;
    new = FALSE;
    if ( globals.g_reingest )
    {
	/*
	 *  Get the existing entry.  
	 */
	
	status = ad_file_get( AD_FG_LOCATION, &ad_file, globals.g_dbproc,
		AD_MED_TP, globals.g_tape_name, file_name, location );
	if ( status == AD_NO_FILE )
	{
	    new = TRUE;
	}
	else
	{
	    TI_CHECK_AD( status );
	}
    }
    else
    {
	modified = TRUE;
    }


    /*
     *  Get the file information.
     */

    TI_CHECK_AD( ad_file_info( strtail( file_name ), file_id,
            &compression, &format ) );

    switch( globals.g_case )
    {
	case AD_CASE_LOWER:
	    str2lower( file_id, file_id );
	    break;

	case AD_CASE_UPPER:
	    str2upper( file_id, file_id );
	    break;

	default:
	    break;
    }


    /*
     *  Populate the new file structure.
     */

    if ( new || strne( ad_file.adf_file_id, file_id ) )
    {
	(void) strcpy( ad_file.adf_file_id, file_id );
	(void) strcpy( ad_file.adf_location.tp.tpl_filename,
		file_name );
	(void) strcpy( ad_file.adf_location.tp.tpl_location,
		location );
	ad_file.adf_compression = compression;
	ad_file.adf_format = format;
	modified = TRUE;
    }

    if ( ad_file.adf_filesize != file_size || 
	    ad_file.adf_filesize_uncomp != uncomp_size )
    {
	ad_file.adf_filesize = file_size;
	ad_file.adf_filesize_uncomp = uncomp_size;
	modified = TRUE;
    }

    (void) strcpy( ad_file.adf_volume_name, globals.g_tape_name );


    /*
     *  If the file is not in the files table, or if has been modified,
     *  Insert it into the files table.
     */

    if ( new || modified )
    {
	TI_CHECK_AD( ad_file_put( &ad_file, globals.g_dbproc ) );
    }

    if ( new )
    {
	format_message( TI_FILE_PUT, ad_file.adf_file_id,
	    ad_file.adf_location.tp.tpl_location, 
	    ad_file.adf_filesize );
    }
    else if ( modified )
    {
	format_message( TI_FILE_UPDATE, ad_file.adf_file_id,
		ad_file.adf_location.tp.tpl_location, 
		ad_file.adf_filesize );
    }
    print_message( msg, MESS_INFO, CR );

    return( TI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	static int	add_tape_rec
*
*   Purpose:
*	Add a tape name to the tape table of the archive directory.
*
*   Parameters:
*	int	num_files	: (in) Number of files on tape.
*	int	num_kbytes	: (in) Total kilobytes in files.
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well.
*	int	TI_E_DB		: An AD error.
*	int	TI_E_MEMORY	: Memory error.
*
************************************************************************
-*/

static int	add_tape_rec
(
    int		num_files,
    int		num_kbytes
)
{
    AD_VOLUME	curr_vol;
    int		status;
    boolean	update;


    /*
     *  Get the contents of the reverse record if it exists.
     */

    TI_CHECK_AD( ad_volume_init( &curr_vol, AD_MED_TP ) );
    status = ad_volume_get( AD_VG_NAME, &curr_vol, globals.g_dbproc,
	    AD_MED_TP, globals.g_tape_name );
    if ( status == AD_SUCCESS )
    {
	curr_vol.adv_vol_info.tp.tpv_verified = AD_NOT_VERIFIED;
	update = TRUE;
    }
    else
    {
	update = FALSE;
	if ( status != AD_NO_VOLUME )
	{
	    TI_CHECK_AD( status );
	}
    }

    (void) strcpy( curr_vol.adv_volume_name, globals.g_tape_name );

    if ( globals.g_archive != NULL )
    {
	(void) strcpy( curr_vol.adv_archive, globals.g_archive );
    }
    
    curr_vol.adv_vol_info.tp.tpv_ingest_date = time( NULL );
    curr_vol.adv_vol_info.tp.tpv_nfiles = num_files;
    curr_vol.adv_vol_info.tp.tpv_nkbytes = num_kbytes;
    (void) strcpy( curr_vol.adv_vol_info.tp.tpv_tp_script, globals.g_type );
    (void) strcpy( curr_vol.adv_vol_info.tp.tpv_type, globals.g_tape_type );


    /*
     *  Now add the current disk to the archive directory.
     */

    TI_CHECK_AD( ad_volume_put( &curr_vol, globals.g_dbproc ) );

    if ( update )
    {
	format_message( TI_TAPE_UPDATE, curr_vol.adv_volume_name,
		curr_vol.adv_vol_info.tp.tpv_nfiles, 
		curr_vol.adv_vol_info.tp.tpv_nkbytes, 
		curr_vol.adv_vol_info.tp.tpv_verified );
	print_message( msg, MESS_INFO, CR );
    }
    else
    {
	format_message( TI_TAPE_PUT, curr_vol.adv_volume_name,
		curr_vol.adv_vol_info.tp.tpv_nfiles, 
		curr_vol.adv_vol_info.tp.tpv_nkbytes, 
		curr_vol.adv_vol_info.tp.tpv_verified );
	print_message( msg, MESS_INFO, CR );
    }

    return( TI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	static int	check_tape_table
*
*   Purpose:
*	Check if "tape_name" is in the tps table.
*
*   Parameters:
*	char	*tape_name	: (in)  The tape name to check.
*	boolean	*found		: (out) Was the tape name found
*					in the tps table?
*	int	*num_files	: (out) Number of files ingested.
*	int	*num_kbytes	: (out) Number of kbytes ingested.
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well.
*	int	TI_E_AD_FATAL	: Fatal ad error.
*
************************************************************************
-*/

static int	check_tape_table
(
    char	*tape_name,	/* (in)  The tape name to check. 	*/
    boolean	*found,		/* (out) Was the tape name found	*/
    				/*       in the tps table?		*/
    int		*num_files,	/* (out) Number of files ingested.	*/
    int		*num_kbytes	/* (out) Number of kbytes ingested.	*/
)
{
    int         status;
    AD_VOLUME   volume;

    status = ad_volume_get( AD_VG_NAME, &volume, globals.g_dbproc,
            AD_MED_TP, tape_name );

    if ( status == AD_NO_VOLUME )
    {
        *found = FALSE;
    }
    else 
    {
        TI_CHECK_AD( status );
        *found = TRUE;
	*num_files = volume.adv_vol_info.tp.tpv_nfiles;
	*num_kbytes = volume.adv_vol_info.tp.tpv_nkbytes;
    }

    return( TI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:  	get_file 
*
*   Purpose:
*	Get file info from the list_file created by read_tape
*	to ingest.
*
*   Values Returned:
*	int	TI_SUCCESS	: Normal completion.
*	int	TP_E_GET_FILE	: Error in open file to read.
*	int 	TI_END_OF_FILE	: Reach the end of the file.
*
************************************************************************
-*/
 
static	int    	get_file 
(
    FILE	*fp,		/* (in)  The OPEND file to read.	*/
    char	*file_name,	/* (out) The file name.			*/
    char	*location,	/* (out) The location show the file 	*/
    				/*       location on the tape.		*/
    int		*file_size,	/* (out) The file size.			*/
    int		*uncomp_size	/* (out) The uncompressed file size.	*/
)
{
    char        line[128];
    int         num_tokens;
    int		size;
    int		sizeUncomp;
    char        *tokens[5];

    file_name[0] = '\0';
    location[0] = '\0';
    *file_size = 0;
    *uncomp_size = 0;
    
    if ( fgets( line, sizeof( line ), fp ) != NULL )
    {
	(void) strtokens( line, 5, tokens, &num_tokens );
	    
	if ( ( num_tokens != 5 ) ||
	     ( !str2int( tokens[3], &size ) ) || 
	     ( !str2int( tokens[4], &sizeUncomp ) ) ) 
	{
	    format_message( TI_E_GET_FILE, line );
	    print_message( msg, MESS_ERROR, CR );
	    return( TI_E_GET_FILE );
	}
	(void) strcpy( location, tokens[0] );
	*file_size = size;
	*uncomp_size = sizeUncomp;
    	(void) strcpy( file_name, tokens[2] );
    }
    else
    {
        return( TI_END_OF_FILE );
    }
 
    return( TI_SUCCESS );
}


/*+
************************************************************************
*
*   Function:	int	ingest
*
*   Purpose:
*	The actual ingest procedure.
*
*   Parameters:
*	None.
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well.
*	int	TI_QUIT		: Quit ingest. 
*	int	TI_E_LOG	: Log file error.
*
************************************************************************
-*/

int	ingest
(
    void
)
{
    boolean	found;
    char	logname[64];
    int		num_files;
    int		num_kbytes;


    if ( prompt_user( READY ) )
    {
	TI_CHECK_TP( tp_device_type( globals.g_device, globals.g_tape_type ) );
    }
    else
    {
        return( TI_QUIT );
    }

    TI_CHECK( get_tape_name() );

    TI_CHECK( check_tape_table( globals.g_tape_name, 
	    &found, &num_files, &num_kbytes ) );

    if ( found )
    {
	if ( num_files != 0 )
	{
	    if ( globals.g_auto_reingest )
	    {
	        format_message( TI_AUTO_REINGEST, globals.g_tape_name );
		print_message( msg, MESS_ERROR, CR );
		globals.g_reingest = TRUE;
	    }
	    else
	    {
	        if ( prompt_user( REINGEST ) )
	        {
		    globals.g_reingest = TRUE;
		}
		else
		{
	       	    return( TI_QUIT );
		}
	    }
	}
    }
	 

    /*
     *  Open the log file.
     */

    (void) sprintf( logname, LOGNAME_FORMAT, globals.g_tape_name );
    if ( ( globals.g_log_fp = fopen( logname, "w" ) ) == NULL )
    {
    	format_message( MSG_ERRNO, logname );
	print_message( msg, MESS_ERROR, CR );
	return( TI_E_OPEN_FILE ); 
    }


    /*
     *  Process the tape.
     */


    TI_CHECK( ingest_tape() );
    
    format_message( TI_DONE, globals.g_tape_name );
    print_message( msg, MESS_ERROR, CR );


    /*
     *  Close the log file.
     */

    (void) fclose( globals.g_log_fp );
    globals.g_log_fp = NULL;

    return( TI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	static int	ingest_tape
*
*   Purpose:
*	Ingest the given tape.
*
*   Parameters:
*	None.
*
*   Values Returned:
*	int	TI_SUCCESS	: All went well.
*	int	TI_E_AD_FATAL	: Fatal ad error.
*	int	TI_E_DB		: Database error.
*	int	TI_E_MEMORY	: Memory error.
*
************************************************************************
-*/

static int	ingest_tape
(
    void
)
{
    char	file_name[AD_FILE_ID_LEN];
    int		file_size;
    int		file_uncomp;
    AD_TP_LOCATION
    		location;
    int 	status;
    char	tmp_list[L_tmpnam];	/* The file info from the tape 	*/
    FILE	*tmpfp;

    (void) tmpnam( tmp_list );


    /*
     * Execute the script to read the tape.
     */
    
    status = execute_script( globals.g_tp_script->tp_ingest, tmp_list );
    if ( status != TI_SUCCESS )
    {
	TI_CHECK_SYSTEM(  unlink( tmp_list ), "unlink" );
	return( status );
    }

    
    /*
     * Read the file created by the script.
     */
    
    if ( ( tmpfp = fopen( tmp_list, "r" ) ) == NULL )
    {
        format_message( MSG_ERRNO, file_name );
	print_message( msg, MESS_ERROR, CR );
	return( TI_E_SYSTEM );
    }

   
    while( ( status = get_file( tmpfp, file_name, location, &file_size, 
	    &file_uncomp ) ) == TI_SUCCESS )
    {
	if ( file_size > 0 && status == TI_SUCCESS )
	{
            globals.g_num_files ++;
            globals.g_num_kbytes += ( file_size + 1023 ) / 1024;
    	    TI_CHECK( add_file_rec( file_name, location, file_size, 
		    file_uncomp ) );
	}
    }
    (void) fclose( tmpfp );
    TI_CHECK_SYSTEM( unlink( tmp_list ), "unlink" );

    
    /*
     *  If end of directory was reached, add the tape to the tape table.
     */

    if ( status == TI_END_OF_FILE )
    {
	TI_CHECK( add_tape_rec( globals.g_num_files, globals.g_num_kbytes ) );
    }
    else 
    {
    	return( status );
    }

    return( TI_SUCCESS );
}
