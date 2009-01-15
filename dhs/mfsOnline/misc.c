/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999
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
*   Module Name:	mfsOnline/src/misc.c
*
*   Purpose:
*	Miscelaneous routines for program mfsOnline.
*
*   Routines:
*static int	archiveCheck	: Checks the validity of the archive.
*	void	closeMfsOnline	: Cleans-up resources used by mfsOnline
*	int	getType		: Get the volume type from the user.
*static int	initDb		: Initialize resources for mfsingest.
*	int	initGlobals	: Initialize the globals structure.
*	int	initMfsOnline	: Initialize mfsOnline.
*
*   Date		: Feb 23, 1999
*
*   SCCS data           : @(#)
*       Module Name     : misc.c
*       Version Number  : 1.3
*       Release Number  : 1
*       Last Updated    : 04/10/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/02/23 SDJ  	: Copied from cdOnline.
*	99/03/22 SDJ	: Cosmetic changes to match other executables.
*     2000/03/21 SDJ	: Changed from dvdOnline to mfsOnline.
*     2000/04/10 SDJ	: Added initMfsOnline(), getType(), archiveCheck(),
*			  and closeMfsOnline().
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 * SYBASE include files
 ************************************************************************
 */

#include <sybfront.h>
#include <sybdb.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_str.h"
#include "db.h"
#include "ad.h"
#include "mfs.h"

/*
 ************************************************************************
 *  mfsOnline include files.
 ************************************************************************
 */

#include "globals.h"
/*
 ************************************************************************
 *  Static function prototypes.
 ************************************************************************
 */ 

static int	archiveCheck( void );
static int	initDb( void );

/*+
************************************************************************
*
*   Function:   archiveCheck
*
*   Purpose:
*	Checks that the given or the default archive is valid.
*
*   Values Returned:
*	int	S_SUCCESS	: All went well
*	int	E_NO_DEFAULT	: No default archive.
*	int	E_ARCHIVE	: Archive name not valid.
*
************************************************************************
-*/

int    archiveCheck
(
    void
)
{
    AD_ARCHIVE	*archive;

    /*
     *  If archive not specified on cmd line, set to ad's default. 
     *  Otherwise, verify that it is a valid archive.
     */ 

    if ( strlen( globals.gArchive ) == 0 )
    {
	archive = ad_archive_info( NULL );
	if ( archive == NULL )
	{
	    formatMessage( E_NO_DEFAULT );
	    printMessage( msg, MESS_ERROR );
	    return( E_NO_DEFAULT );
	}
	(void) strcpy( globals.gArchive, archive->ada_name );
    }
    else
    {
    	archive = ad_archive_info( globals.gArchive );
	if ( archive == NULL )
	{
	    formatMessage( E_ARCHIVE, globals.gArchive );
	    printMessage( msg, MESS_ERROR );
	    return( E_ARCHIVE );
	}
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   closeMfsOnline
*
*   Purpose:
*	Statement of purpose.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

void    closeMfsOnline
(
    void
)
{
    if ( globals.gDbproc != NULL )
    {
	(void)db_exit();
	globals.gDbproc = NULL;
    }

    if ( globals.gLogFp != NULL )
    {
	(void) fclose( globals.gLogFp );
	globals.gLogFp = NULL;
    }

}

/*+
************************************************************************
*
*   Function:	getType
*
*   Purpose:
*	Prompt for the user to specify the type of media being read.
*
*   Values Returned:
*	int	S_SUCCESS	: All went well.
*	int	S_QUIT		: User does not wish to continue.
*
************************************************************************
-*/

int	getType
( 
    void
)
{
    char	buffer[1024];
    boolean	reprompt;
    int		i;
    
    reprompt = FALSE;
    do 
    {
	/*
	 *  Prompt the operator for a volume.
	 */
	
	(void) sprintf( buffer, "Enter the type of volume to be"
		" mounted/unmounted (CD, DVD, or MD) : " );
	printMessage( buffer, MESS_PROMPT );

	if ( fgets( globals.gVolumeType, 
		sizeof( globals.gVolumeType ), stdin ) == NULL )
	{
	    if ( promptUser( S_QUIT ) )
	    {
		return( S_QUIT );
	    }
	    reprompt = TRUE;
	    rewind( stdin );
	    continue;
	}


	/*
	 *  Remove any newlines in the string.
	 */

	for ( i = 0; i < (int) strlen( globals.gVolumeType ); i++ )
	{
	    if ( globals.gVolumeType[i] == '\n' )
	    {
		globals.gVolumeType[i] = '\0';
	    }
	}

	(void) strfit( globals.gVolumeType );
    }
    while ( reprompt );


    return( S_SUCCESS );

} 

/*+
************************************************************************
*
*   Synopsis:
*	int	initDb( )
*
*   Purpose:
*	Initialize the resources needed for mfsingest's operation.
*
*   Parameters:
*
*   Values Returned:
*	int	S_SUCCESS	: All went well.
*	int	E_AD		: Error in the ad library.
*	int	E_AL		: Error in the al library.
*	int	E_DB_LIB	: Database library error occurred.
*
************************************************************************
-*/

int	initDb
(
    void
)
{
    char	*dbname;	/* Name of database to use.		*/
    char	*dbserver;	/* Name of the server to use.		*/
    
    /* 
     *  Initialize the CADC db library, print library init message.
     */

    CHECK_DB_LIB( db_init( APPLICATION ) );
    msg_append( msg, db_msg );
    msg_clear( db_msg );
    printMessage( msg, MESS_OPER );
    

    /*
     *  Get the database name, and open a connection.
     */

    CHECK_AD( ad_dbinfo_get( AD_MED_MFS, &dbserver, &dbname, NULL, NULL ) );
    CHECK_DB_LIB( db_open( dbserver, NULL, NULL, dbname,
	    FALSE, &globals.gDbproc ) );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	initGlobals
*
*   Purpose:
*	Initialize the globals structure.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		initGlobals
(
    void
)
{
    globals.gArchive[0]		= '\0';
    globals.gCreateDate		= -1;
    globals.gDbproc 		= NULL;
    globals.gLogFp		= NULL;
    globals.gTarget 		= NULL;
    globals.gUnmount 		= FALSE;
    globals.gVersion 		= FALSE;
    globals.gVerbose  		= FALSE;
    globals.gVolumeType[0]	= '\0';
    
    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	initMfsOnline
*
*   Purpose:
*	Initialization for mfsIngest.
*
*   Values Returned:
*	int	S_SUCCESS	: Normal Completion
*	int	E_DB		: Error in the SYBASE library.
*	int	E_DB_LIB	: Error in the CADC databas library.
*	int	E_LOG		: Error opening the log file.
*
************************************************************************
-*/

int	initMfsOnline
(
    void
)
{
    char	logname[PATH_MAX];		/* Log file name.	*/
    
    
    /*
     * Open the log file.
     */

    (void) sprintf( logname, "%s.log", APPLICATION );
    if ( ( globals.gLogFp = fopen( logname, "a" ) ) == NULL )
    {
	formatMessage( MSG_ERRNO, logname );
	printMessage( msg, MESS_ERROR );
	return( E_LOG );
    }


    /*
     *  Write the version information to the log file.
     */
 
    formatMessage( S_VERSION, version_short );
    printMessage( msg, MESS_LOG );


    /*
     *  Initialize the ad library. 
     */
    
    CHECK_AD( ad_init() );
    msg_append( msg, ad_msg );
    msg_clear( ad_msg );
    printMessage( msg, MESS_INFO );


    /*
     * Determine the archive that mfs belongs too if we are marking
     * as on-line.
     */

    if ( globals.gUnmount == FALSE )
    {
	CHECK( archiveCheck() );
    }
    
    
    
    /*
     * Initialize the database.
     */

    CHECK( initDb() );


    return( S_SUCCESS );
}

