static char rcsid[] = "$Id: store.C,v 1.1.1.1 2002-11-24 20:27:34 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un fs_usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsData/src/store.C
//
// PURPOSE:
// Handles all management of the permanent and temporary storage areas.
//
// METHOD NAME(S)
// cDtsStoreManager::checkPath  -  Check existance of temp or perm store.
// cDtsStoreManager::config - Read dhs configuration file parameters.
// cDtsStoreManager::determineSpace  -  Determine space used.
// cDtsStoreManager::fileRemove  - Remove a file from temp or perm store.
// cDtsStoreManager::init - Init the store class.
// cDtsStoreManager::makeDir  -  Determine space used.
// cDtsStoreManager::store  - Read dhs configuration file parameters.
// cDtsStoreManager::testAll  -  Run test on perm and temporary storage.
// cDtsStoreManager::updateSpace  - Update space in use on the status server.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.10  1998/05/04 23:03:29  dunn
// Added status class changes.
//
// Revision 1.9  1997/10/03 23:03:07  dunn
// Replaced all popen calls to run in workshop.
//
// Revision 1.8  1997/10/01 17:06:00  dunn
// Fixed it so that if df fails, it gives a reasonable answer.
//
// Revision 1.7  1997/09/29 21:23:57  dunn
// Added code in case df fails ignore returned results.
//
// Revision 1.6  1997/09/17 22:15:34  dunn
// code cleanup.
//
// Revision 1.5  1997/08/29 23:05:43  dunn
// Added a slash that was missing.
//
// Revision 1.4  1997/08/19 22:31:59  dunn
// Commented out removeDir for now.
//
// Revision 1.3  1997/07/29 22:02:37  dunn
// Various fixes.
//
// Revision 1.2  1997/07/23 19:43:17  dunn
// Fixed messages and added a check to make sure the permanent and temporary
// paths listed in the ad/md database exist.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  Includes
//

#include <iostream.h>
#include "genMutex.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"

#include "dirent.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/*XXX allan: replaced with statvfs
#ifndef __GNUC__		// Hack to get around the defines in libfu
#define __GNUC__		// libfu does not know about c++ (silly)
#include "fsusage.h"
#undef __GNUC__
#else
#include "fsusage.h"
#endif
XXX*/

#include <sys/statvfs.h>
int statvfs(const char *, struct statvfs *);
}

#include "globals.H"
#include "store.H"
#include "dhsGenTmp.H"


//
//  Local defines.
//

#define DTS_KEYWORD_STORE	"storage"

cMutex	cDtsStoreManager::cdmMutexTempSz;
cMutex	cDtsStoreManager::cdmMutexPermSz;

char	*cDtsStoreManager::smTempPath = NULL;
char	*cDtsStoreManager::smPermPath = NULL;

char	*cDtsStoreManager::smTempVol = NULL;
char	*cDtsStoreManager::smPermVol = NULL;

int	cDtsStoreManager::smTempSpTotal = 0;
int	cDtsStoreManager::smPermSpTotal = 0;

int	cDtsStoreManager::smTempSpUsed = 0;
int	cDtsStoreManager::smPermSpUsed = 0;


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::checkPath()
//
// INVOCATION:
// cDtsStoreManager::checkPath( status, storage );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) storage (tLife) Storage flag, temp or permanent.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Determine if the storage path in exists
//
// DESCRIPTION:
// Determine if the storage path in exists by attempting to open it.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsStoreManager::checkPath
(
    cDtsStatus	&status,		// (mod) Function return status.
    tLife	storage			// (in)  Temp or perm storage.
)
{
    char	*path;			// Path to do a df on.
    DIR		*dirp;			// Directory pointer.



    checkStat( status, return );

    if ( storage == ST_PERM )
    {
	path = permPath();
    }
    else
    {
	path = tempPath();
    }

    //
    //  Open the directory to see if it exists.
    //

    if ( (dirp = opendir( path )) == NULL )
    {
	//
	//  Were not able to open the path, error.
	//

	if ( storage == ST_PERM )
	{
	    status.E_STORAGE_PATH( status, "permanent", path );
	}
	else
	{
	    status.E_STORAGE_PATH( status,"temporary",  path );
	}
       return;
    }
    closedir( dirp );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::config
//
// INVOCATION:
// cDtsStoreManager::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the storage configuration parameters
//
// DESCRIPTION:
// Read in the paths to the temporary and permanent storage areas.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsStoreManager::config
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    checkConfig( config_get( DTS_KEYWORD_STORE, store ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::determineSpace
//
// INVOCATION:
// cDtsStoreManager::determineSpace( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) storage (tLife) Temporary or permanent storage.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Determine space available in temp or permanent storage
//
// DESCRIPTION:
// Attain a lock and do a df on the storage area passed in.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

int		cDtsStoreManager::determineSpace
(
    cDtsStatus	&status,		// (mod) Function return status.
    tLife	storage			// (in)  Temp or perm storage.
)
{
    char	*path;			// Path to do a df on.
    int		available;		// Space available.

    checkStat( status, return(0) );


    //
    //  Lock out the variable before determining space.
    //

    if ( storage == ST_PERM )
    {
	cdmMutexPermSz.lock();
	path = permPath();
    }
    else
    {
	cdmMutexTempSz.lock();
	path = tempPath();
    }


    //
    //  Determine about of space.
    //

    struct statvfs vfs;
    int i = statvfs(path, &vfs);

     if ( i != 0 )
     {
	//
	// The call failed, print a warning and use the last variables.
	//

	if ( storage == ST_PERM )
	{
		available = smPermSpTotal - smPermSpUsed;
	}
	else
	{
		available = smTempSpTotal - smTempSpUsed;
	}
	status.S_DF_CMD( status, "statvfs  failed" );
     }
     else
     {
	//
	// Save the variables.
	//

	if ( storage == ST_PERM )
	{
	    smPermSpTotal = vfs.f_blocks / 2;
	    smPermSpUsed = smPermSpTotal - ( vfs.f_bavail / 2 );
	}
	else
	{
	    smTempSpTotal = vfs.f_blocks / 2;
	    smTempSpUsed = smTempSpTotal - (vfs.f_bfree / 2 );
	}
	available = vfs.f_bavail / 2;
     }

    return( available );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::fileRemove
//
// INVOCATION:
// fileRemove(status, filename, storage, sizeRemoved);
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) filename (char *) filename to remove.
// (>) storage (tLife) Temp or perm storage.
// (>) sizeRemoved (int) Amount removed.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Remove a file from the file system and update the space
//
// DESCRIPTION:
// Remove a file from the file system and update the space on the status server.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsStoreManager::fileRemove
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*filename,		// (in)  Filename with path to remove.
    tLife	storage,		// (in)  Temp or perm storage.
    int		sizeRemoved		// (in)  Amount removed.
)
{

    checkStat( status, return );


    //
    //  Remove the file/dir.
    //

    cDhsTmpDir::rmPath( filename );


    //
    //  Update the space available on the status server.
    //

    updateSpace( status, storage, -(sizeRemoved) );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::init
//
// INVOCATION:
// cDtsStoreManager::init()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) - Returned status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To initialize the temporary storage list and determine about of space
// available in both storage areas
//
// DESCRIPTION:
// To initialize the temporary storage list and determine about of space
// available in both storage areas.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void cDtsStoreManager::init
(
    cDtsStatus		&status	// (in) Return status. 
)
{
    int		tempSpaceAvail;
    int		permSpaceAvail;


    checkStat( status, return );


    //
    //  Determine disk space in temporary and permanent.
    //

    permSpaceAvail = determineSpace( status, ST_PERM );
    tempSpaceAvail = determineSpace( status, ST_TEMP );
    
    status.S_PATHS( status, tempSpaceAvail, tempPath(), permSpaceAvail, 
    		permPath() );

    //
    //  Update the space in both.
    //

    updateSpace( status, ST_PERM, 0 );
    updateSpace( status, ST_TEMP, 0 );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::makeDir()
//
// INVOCATION:
// cDtsStoreManager::makeDir( status, path, dirName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) path (char *) Path to directory to be made.
// (>) dirName (char *) Directory name to create.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Make a directory passed in
//
// DESCRIPTION:
// Make a directory passed in.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsStoreManager::makeDir
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*path,			// (in)  Path that already exists.
    char	*dirName		// (in)  Dir. name to make.
)
{
    checkStat( status, return );
    char	tmp[PATH_MAX];


    tmp[0]='\0';
    (void) strcpy( tmp, path );
    (void) strcat( tmp, "/");
    (void) strcat( tmp, dirName );

    if ( mkdir( tmp, S_IRWXU ) != 0  && errno != EEXIST )
    {
	//
	//  Make dir did not work, error out.
	//

	status.E_MK_DIR( status, tmp );
	status.sysErrno();
	status.display();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::store
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the temporary and permanent storage paths
//
// DESCRIPTION:
// Read the temporary and permanent storage paths and store them.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cDtsStoreManager::store
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cDtsStatus	status;		// Status code.

    if ( numTokens == 3 )
    {
	if ( smTempVol != NULL )
	{
	    free( smTempVol );
	}
	checkNull( smTempVol = (char *) strsav( tokens[1] ), status, 
		return( FALSE ) );
	if ( smPermVol != NULL )
	{
	    free( smPermVol );
	}
	checkNull( smPermVol = (char *) strsav( tokens[2] ), status,
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::testAll()
//
// INVOCATION:
// cDtsStoreManager::testAll( status,  );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Test that temp and perm storage areas exist
//
// DESCRIPTION:
// Test that temp and perm storage areas exist.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cDtsStoreManager::testAll
(
    cDtsStatus	&status 		// (mod) Function return status.
)
{

    checkStat( status, return( FALSE ) );


    //
    //  Determine disk space in temporary and permanent.
    //

    checkPath( status, ST_PERM );
    checkPath( status, ST_TEMP );

    if ( status.ok() )
    {
	return ( TRUE );
    }
    else
    {
	return ( FALSE );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsStoreManager::updateSpace
//
// INVOCATION:
// cDtsStoreManager::updateSpace( status, storage, sizeAdded );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) storage (tLife) Temp or perm storage.
// (>) sizeAdded (int) Amount written.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Update space available in temp or permanent on the status server
//
// DESCRIPTION:
// Update space available in temp or permanent on the status server.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsStoreManager::updateSpace
(
    cDtsStatus	&status,		// (mod) Function return status.
    tLife	storage,		// (in)  Temp or perm storage.
    int		sizeAdded		// (in)  Amount written in bytes.
)
{
//    char	tmp[25];		// temporary size in string.


    sizeAdded = (sizeAdded + 1023) / 1024;


    //
    //  Update the space available. and unlock the variable.
    //  Update the status server.
    //

    if ( storage == ST_PERM )
    {
	if ( status.ok() )
	{
	    smPermSpUsed += sizeAdded;
	}
	cdmMutexPermSz.unlock();
    }
    else
    {
	if ( status.ok() )
	{
	    smTempSpUsed += sizeAdded;
	}
	cdmMutexTempSz.unlock();
    }
}
