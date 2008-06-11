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
*   Module Name:	cdIngest/src/misc.c
*
*   Purpose:
*	Contains several utility routines for cdingest.
*
*   Routines:
*	int	archiveCheck	: Checks that the archive is valid.
*	void	closeCdIngest	: Closes optical disks.
*	int	diskLoad	: Prompts for and confirms a disk load.
*	int	initDb		: Initialize resources for cdingest.
*	boolean initGlobals	: Initializes the global data structure.
*
*   Date		: Dec 09, 1996
*
*   Field SCCS data	: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 05/19/99
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/05 JSD  :	Initialize with the database name of the CD.
*	97/02/19 JSD  :	Added logging at startup, logDb.
*	97/03/18 SEC  : Mod'ed for change to al lib to fetch db
*			server name in the alDbInfoGet call. 
*	97/06/20 SEC  :	Mod'ed initGlobals() to initialize everything,
*			and mod'ed initilization of globals.gArchive.
*	98/01/21 gz   : Removed logDb function.
*	98/06/23 sjg  : Added archiveCheck() function.
*	99/05/19 sjg	: Changed gUpper to tri-state AD_CASE value.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sybfront.h>
#include <sybdb.h>
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "ad.h"
#include "cd.h"
#include "db.h"

#include "globals.h"



/*+
************************************************************************
*
*   Function:	archiveCheck
*
*   Purpose:
*	Checks that the given or the default archive is valid.
*
*   Values Returned:
*	int	CI_SUCCESS	: All went well
*	int	CI_E_NO_DEFAULT	: No default archive.
*	int	CI_E_ARCHIVE	: Archive name not valid.
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
	    formatMessage( CI_E_NO_DEFAULT );
	    printMessage( msg, MESS_ERROR, CR );
	    return( CI_E_NO_DEFAULT );
	}
	(void) strcpy( globals.gArchive, archive->ada_name );
    }
    else
    {
    	archive = ad_archive_info( globals.gArchive );
	if ( archive == NULL )
	{
	    formatMessage( CI_E_ARCHIVE, globals.gArchive );
	    printMessage( msg, MESS_ERROR, CR );
	    return( CI_E_ARCHIVE );
	}
    }

    globals.gCase = archive->ada_case;
    return( CI_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	closeCdIngest
*
*   Purpose:
*	Clean up before exiting.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void    closeCdIngest
(
    void
)
{
    if ( globals.gDbproc != NULL )
    {
	(void)db_exit();
    }
}

/*+
************************************************************************
*
*   Function:	diskLoad
*
*   Purpose:
*	Prompt for the user to load a source disk.
*
*   Values Returned:
*	int	CI_SUCCESS	: All went well.
*	int	CI_QUIT		: User does not wish to continue.
*
************************************************************************
-*/

int	diskLoad
( 
    void
)
{
    char	buffer[1024];
    boolean	remount;
    int		i;


    do 
    {
	remount = FALSE;


	/*
	 *  Prompt the operator for an optical disk.
	 */

	(void) sprintf( buffer, "Enter any path on the cd to ingest : " );
	printMessage( buffer, MESS_ERROR, NO_CR );

	if ( fgets( globals.gMountPoint, 
		sizeof( globals.gMountPoint ), stdin ) == NULL )
	{
	    if ( promptUser( QUIT ) )
	    {
		return( CI_QUIT );
	    }
	    remount = TRUE;
	    rewind( stdin );
	    continue;
	}


	/*
	 *  Remove any newlines in the string.
	 */

	for ( i = 0; i < (int) strlen( globals.gMountPoint ); i++ )
	{
	    if ( globals.gMountPoint[i] == '\n' )
	    {
		globals.gMountPoint[i] = '\0';
	    }
	}

	(void) strfit( globals.gMountPoint );


	if ( cd_check_mnt( globals.gMountPoint, globals.gDiskname,
		&globals.gCreateDate ) != CD_SUCCESS )
	{
	    printMessage( cd_msg, MESS_ERROR, CR );
	    remount = TRUE;
	    continue;
	}


	formatMessage( CI_DISK_NAMED, globals.gDiskname );
	printMessage( msg, MESS_ERROR, CR );

	if ( promptUser( CONTINUE ) )
	{
	    remount = FALSE;
	}
	else
	{
	    remount = TRUE;
	}
    }
    while ( remount );


    return( CI_SUCCESS );

} /* end of disk load */

/*+
************************************************************************
*
*   Synopsis:
*	int	initDb( )
*
*   Purpose:
*	Initialize the resources needed for cdingest's operation.
*
*   Parameters:
*
*   Values Returned:
*	int	CI_SUCCESS	: All went well.
*	int	CI_E_AD		: Error in the ad library.
*	int	CI_E_AL		: Error in the al library.
*	int	CI_E_DB_LIB	: Database library error occurred.
*
************************************************************************
-*/

int	initDb()
{
    char	*dbname;
    char	*dbserver;

    CI_CHECK_DB_LIB( db_init( APPLICATION ) );

    /*
     *  Get the database name, and open a connection.
     */

    CI_CHECK_AD( ad_dbinfo_get( AD_MED_CD, &dbserver, &dbname, NULL, NULL ) );
    CI_CHECK_DB_LIB( db_open( dbserver, NULL, NULL, dbname,
	    FALSE, &globals.gDbproc ) );


    return( CI_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*       int     initGlobals()
*
*   Purpose:
*       Initialize the globals variables.
*
*   Parameters:
*       None.
*
*   Values Returned:
*       int	CI_SUCCESS		: All went well
*
************************************************************************
-*/

int	 initGlobals()
{
    globals.gAllowReingest 	= FALSE;
    globals.gBatch 		= FALSE;
    globals.gReingest		= FALSE;
    globals.gVerbose 		= FALSE;
    globals.gVersion 		= FALSE;

    globals.gDiskname[0] 	= '\0';
    globals.gMountPoint[0] 	= '\0';

    globals.gCreateDate 	= 0;
    globals.gNumFiles 		= 0;
    globals.gNumKbytes 		= 0;

    globals.gArchive[0] 	= '\0';
    globals.gDbproc 		= NULL;
    globals.gLogFp 		= NULL;
    globals.gPathList 		= NULL;

    return( CI_SUCCESS );
}
