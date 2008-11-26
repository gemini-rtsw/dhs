/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1999				(c) 1999.
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
*   Module Name:	mfsIngest/src/misc.c
*
*   Purpose:
*	Contains several utility routines for mfsingest.
*
*   Routines:
*	void	closeMfsIngest	: Closes optical volumes.
*	int	getType		: Get the type of media being read.
*	int	volumeLoad	: Prompts for and confirms a volume load.
*
*   Date		: Mar 10, 1999
*
*   SCCS data		: @(#)
*	Module Name	: misc.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 03/27/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	99/03/10 SDJ	: Copied from cdIngest.
*	99/10/14 SDJ	: Changed gUpper to tri-state AD_CASE value.
*     2000/03/27 SDJ	: Updated to new CADC software standards.  Switched
*		 	  from dvdIngest to mfsIngest.  Added getType().
*			  Moved initiliazation routines into init.c.
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
#include "mfs.h"
#include "db.h"

#include "globals.h"

/*+
************************************************************************
*
*   Function:	closeMfsIngest
*
*   Purpose:
*	Clean up before exiting.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void    closeMfsIngest
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

    if ( globals.gPathList != NULL )
    {
	gen_free( globals.gPathList );
	globals.gPathList = NULL;
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
	
	(void) sprintf( buffer, "Enter the type of media to be ingested (CD, DVD, or MD) : " );
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
*   Function:	volumeLoad
*
*   Purpose:
*	Prompt for the user to load a source volume.
*
*   Values Returned:
*	int	S_SUCCESS	: All went well.
*	int	S_QUIT		: User does not wish to continue.
*
************************************************************************
-*/

int	volumeLoad
( 
    void
)
{
    char	buffer[1024];
    boolean	remount;
    int		i;
    
    remount = FALSE;
    do 
    {
	/*
	 *  Prompt the operator for a volume.
	 */
	
	(void) sprintf( buffer, "Enter any path on the volume to ingest : " );
	printMessage( buffer, MESS_PROMPT );

	if ( fgets( globals.gMountPoint, 
		sizeof( globals.gMountPoint ), stdin ) == NULL )
	{
	    if ( promptUser( S_QUIT ) )
	    {
		return( S_QUIT );
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


	/*
	 * Verify the path given.
	 */

	if ( mfs_check_mnt( globals.gMountPoint, globals.gVolumename,
		&globals.gCreateDate, globals.gMfsFormat ) != MFS_SUCCESS )
	{
	    printMessage( mfs_get_message(), MESS_ERROR );
	    remount = TRUE;
	    continue;
	}


	formatMessage( S_VOLUME_NAMED, globals.gVolumename );
	printMessage( msg, MESS_ERROR );

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


    return( S_SUCCESS );

}
