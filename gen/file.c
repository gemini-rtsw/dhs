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
*   Module Name:	gen/src/dir.c
*
*   Purpose:
*	Contains routines for creation and removal of files and
*	directories.
*
*   Routines:
*	boolean	file		: returns TRUE if path is a a regular file
*				  and has no links to it..
*	boolean file_mk		: Creates a file and any parent directories
*				  that don't already exist
*	boolean file_rm		: Removes a file which has no links to it.
*	boolean	file_dir	: returns TRUE if path is a directory.
*	boolaan	file_dir_empty	: Remove all files and subdirectories in
*				  a given directory.
*	boolean	file_dir_mk	: Creates a path and all necessary
*				  parent directories if they don't already
*				  exist.
*	boolean file_dir_rm	: Removes a path and any subdirectories
*				  and/or files below it.
*	boolean	file_link	: return TRUE if the file has more than
*				  one link.
*	boolean file_link_mk	: Creates a hard link and any parent 
*				  directories that don't already exist.
*	boolean file_link_rm	: Removes a file only if there is more
*				  than one link to it.
*	boolean	file_sym	: return TRUE if path is a symbolic link.
*	boolean file_sym_mk	: Creates a symbolic link and any parent 
*				  directories that don't already exist
*	boolean file_sym_rm	: Removes a symbolic link.
*
*   Date		: Dec 21, 1998
*
*   SCCS data		: @(#)
*	Module Name	: file.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 03/20/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*     2000/02/29 SDJ	: Allow "NULL" pointer to stat structure.  This
*			  way application doesn't need to worry about
*			  creating a stat structure.
*     2000/03/02 SDJ	: Put fail "EOVERFLOW" when doing lstat() back in.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Standard include file.
 */

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>


/*
 *  CADC include file.
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_file.h"

/*
 *  Internal function prototypes
 */

#ifndef VxWorks

/*+
************************************************************************
*
*   Function:	file
*
*   Purpose:
*	Verify that a path is a file
*
*   Values Returned:
*	boolean	TRUE		: 'path' is a file.
*	boolean	FALSE		: 'path' is a not file.
*
************************************************************************
-*/

boolean file
(
   char		path[PATH_MAX],	/* (in)  Path to be checked.. 		*/
   struct stat *stat_buf	/* (out) File statistic buffer.		*/
)
{
    boolean 	ret;		/* Function's return value.		*/
    struct stat	local_stat;	/* Local stat structure.		*/

    
    /*
     * Figure out if the path is a directory or file.
     */

    ret = TRUE;
    errno = 0;
    if ( lstat( path, &local_stat ) )
    {
        ret = FALSE;
    }
    
    if ( ret && ( ! S_ISREG( local_stat.st_mode ) ||
	    S_ISLNK( local_stat.st_mode ) || local_stat.st_nlink > 1 ) )
    {
	ret = FALSE;
    }


    if ( stat_buf != NULL )
    {
	(void) memcpy( stat_buf, &local_stat, sizeof( struct stat ) );
    }
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_mk
*
*   Purpose:
*	Create a file at a given path.  Any parent directories
*	that do not exist are automatically created.
*
*	The file that is open is opened for reading and writing.
*
*   Values Returned:
*	boolean	TRUE		: If given path exists after execution.
*	boolean FALSE		: If directory is *not* created.
*
************************************************************************
-*/

boolean	file_mk
(
   char		path[PATH_MAX],	/* (in)  Path that is to be created. 	*/
   mode_t	mode,		/* (in)	 File permission mode.		*/
   int		*fd		/* (out) File descriptor created. 	*/
)
{
    char	parent[PATH_MAX];   /* Parent directory.		*/
    boolean	result;		    /* Result for file().		*/
    boolean     ret;		    /* Function return value.		*/
    struct stat	local_stat;	    /* Statistical file info.		*/

    
    ret = TRUE;
    *fd = -1;
    (void) sprintf( parent, "%s", path );
    strhead( parent );

    if ( strne( path, parent ) )
    {
	ret = file_dir_mk( parent, mode );
    }

    result =  file( path, &local_stat );
    if ( ret && ( ( !result && errno == ENOENT  ) || result ) )
    {
	if( ( *fd = creat( path, mode ) ) < 0  )
	{
	    ret = FALSE;
	}
    }
    else 
    {
	ret = result;
    }
    
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_rm
*
*   Purpose:
*	Removes a particular file.
*
*   Values Returned:
*	boolean	TRUE		: If file successfully removed.
*	boolean FALSE		: If file not removed.
*
************************************************************************
-*/

boolean	file_rm
(
   char		path[PATH_MAX]	/* (in)  Path that is to be cleared. */
)
{
    boolean	ret;
    struct stat	local_stat;	    /* Statistical file info.		*/
    
    ret = FALSE;
    if( file( path, &local_stat ) )
    {
	if ( unlink( path ) == 0 || errno == ENOENT )
	{
	    errno = 0;
	    ret = TRUE;
	}
    }
    else if ( errno == ENOENT )
    {
	errno = 0;
	ret = TRUE;
    }
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_dir
*
*   Purpose:
*	Verify that a path is a directory
*
*   Values Returned:
*	boolean	TRUE		: 'path' is a directory.
*	boolean	FALSE		: 'path' is a not directory.
*
************************************************************************
-*/

boolean file_dir
(
   char		path[PATH_MAX],	/* (in)  Path to be checked.. 		*/
   struct stat 	*stat_buf	/* (out) File statistic buffer.		*/
)
{
    boolean 	ret;		/* Function's return value.		*/
    struct stat	local_stat;	/* Local stat structure.		*/
        

    /*
     * Figure out if the path is a directory or file.
     */

    ret = TRUE;
    errno = 0;
    
    if ( lstat( path, &local_stat ) )
    {
        ret = FALSE;
    }
    
    if ( ret && ( ! S_ISDIR( local_stat.st_mode ) ||
	    S_ISLNK( local_stat.st_mode ) ) )
    {
	ret =  FALSE;
    }

    if ( stat_buf != NULL )
    {
	(void) memcpy( stat_buf, &local_stat, sizeof( struct stat ) );
    }
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_dir_empty
*
*   Purpose:
*	Remove all files and directories below a given directory.
*	equivalent to the UNIX rm -rf <path>/ *  <path>/.*
*	command.
*
*   Values Returned:
*	boolean	TRUE		: if directory is cleared
*	boolean	FALSE		: if direcotyr is not cleared.
*
************************************************************************
-*/

boolean	file_dir_empty
(
    char	path[PATH_MAX]	/* (in)  Path to empty. 	*/
)
{
    DIR         *dir;			/* File descriptor to dir.	*/	
    struct dirent	
		*dir_ent;		/* Directory entry.		*/
    char        local_path[PATH_MAX];	/* Local path variable.		*/
    boolean	ret;			/* Function return value.	*/
    struct stat local_stat;		/* File statistic buffer.	*/
    
	
    /*
     * Make sure path is a directory and it is readable.
     */
    
    ret = TRUE;
    dir = NULL;
    if ( ( dir_ent = (struct dirent *) malloc( sizeof( struct dirent ) + 
	    pathconf( path, _PC_NAME_MAX ) + 1 ) ) == NULL )
    {
	ret = FALSE;
    }
    

    if ( ! file_dir( path, &local_stat ) )
    {
	ret = FALSE;
    }
    else
    {
        if ( ( dir = opendir( path ) ) == NULL )
        {
            ret = FALSE;
        }
    }


    /*
     * Start removing the contents of the directory.
     */

#if defined( solaris2 )
    while ( ret && ( dir != NULL) && ( readdir_r( dir, dir_ent ) != NULL ) )
#elif defined( hp9000 )
    while ( ret && ( dir != NULL) && ( readdir_r( dir, dir_ent ) == 0 ) )
#else
    while ( ret && ( dir != NULL) && ( ( dir_ent = readdir( dir ) ) != NULL ) )
#endif 
    {
	if ( streq( dir_ent->d_name, "." ) || 
		streq( dir_ent->d_name, ".." ) != 0 )
	{	
	    continue;
	}

	strcpy( local_path, path );
	strcat( local_path, "/" );
	strcat( local_path, dir_ent->d_name );
	
	if ( file( local_path, &local_stat ) )
	{
	    ret = file_rm( local_path );
	}
	else if ( file_dir( local_path, &local_stat ) )
	{
	    ret = file_dir_rm( local_path );
	}
	else if ( file_link( local_path, &local_stat ) )
	{
	    ret = file_link_rm( local_path );
	}
	else if ( file_sym( local_path, &local_stat ) )
	{
	    ret = file_sym_rm( local_path );
	}
	else
	{
	    ret = FALSE;
	}
    }


    /*
     * Cleanup
     */
    
    if ( dir_ent != NULL )
    {
	gen_free( dir_ent );
	dir_ent = NULL;
    }
    

    if ( dir != NULL )
    {
        (void) closedir( dir );
	dir = NULL;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_dir_mk
*
*   Purpose:
*	Create a directory from a given path.  Any parent directories
*	that do not exist are automatically created.
*
*   Values Returned:
*	boolean	TRUE		: If directory is created.
*	boolean FALSE		: If directory is *not* created.
*
************************************************************************
-*/

boolean	file_dir_mk
(
   char		path[PATH_MAX],	/* (in)  Path that is to be created. */
   mode_t	mode		/* (in)	 File permission mode.		*/
)
{
    char	parent[PATH_MAX];	/* Path to parent dir.		*/
    boolean	result;			/* Function return value.	*/
    boolean     ret;			/* This funt. return value.	*/
    struct stat local_stat;		/* File statistic buffer.	*/	


    ret = TRUE;

    result = file_dir( path, &local_stat );
    if ( ! result && errno == ENOENT )
    {
	/*
	 * First create any parenet directories.
	 */
	
	errno = 0;
	(void) sprintf( parent, "%s", path );
	strhead( parent );
	if ( strne( parent, path ) )
	{
	    ret = file_dir_mk( parent, mode );
	}

	/*
	 * Now make the desired directory.
	 */
	if ( ret && mkdir( path, mode ) != 0 )
	{
	    ret = FALSE;
	}
    }
    else
    {
	ret = result;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_dir_rm
*
*   Purpose:
*	Remove a directory and all of its contents, which inclues
*	all files and directories below the path.
*	Equivalent ot he unix rm -fr <path> command.
*
*	 On failure, errno should be set.
*
*   Values Returned:
*	boolean	TRUE		: If path successfully removed.
*	boolean FALSE		: If path not removed.

*
************************************************************************
-*/

boolean	file_dir_rm
(
   char		path[PATH_MAX]	/* (in)  Path that is to be cleared. */
)
{
    boolean	ret;

    ret = file_dir_empty(  path );
    if ( ret && (  rmdir( path ) != 0 ) )
    {
	ret =  FALSE;
    }
    else if (  errno == ENOENT )
    {
	errno = 0;
	ret = TRUE;
    }

    
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_link
*
*   Purpose:
*	Verify that a path is a file with more than one hard link to it.
*
*   Values Returned:
*	boolean	TRUE		: 'path' is a file.
*	boolean	FALSE		: 'path' is a not file.
*
************************************************************************
-*/

boolean file_link
(
   char		path[PATH_MAX],	/* (in)  Path to be checked.. 		*/
   struct stat 	*stat_buf	/* (out) File statistic buffer.		*/
)
{
    boolean 	ret;		/* Function's return value.		*/
    struct stat	local_stat;	/* Local stat structure.		*/

    
    /*
     * Figure out if the path is a directory or file.
     */

    ret = TRUE;
    errno = 0;

    if ( lstat( path, &local_stat ) )
    {
        ret = FALSE;
    }
    
    if ( ret && ( ! S_ISREG( local_stat.st_mode ) || local_stat.st_nlink < 2 ) )
    {
	ret = FALSE;
    }

    if ( stat_buf != NULL )
    {
	(void) memcpy( stat_buf, &local_stat, sizeof( struct stat ) );
    }
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_link_mk
*
*   Purpose:
*	Creates a hard link between an existing file and a given
*	path.  If any parent directories do not exist in the given
*	path they are created.
*
*   Values Returned:
*	boolean	TRUE		: If symbolic hard exists.
*	boolean FALSE		: If directory is *not* created.
*
************************************************************************
-*/

boolean	file_link_mk
(
   char		source[PATH_MAX],	/* (in)  source file.		*/
   char		dest[PATH_MAX],		/* (in)  hard link destination.	*/
   mode_t	mode			/* (in)	 dir. permission mode.	*/
)
{
    char	parent[PATH_MAX];	/* Parent directory.		*/
    boolean     ret;			/* Function return value.	*/
    boolean	result;			/* Result from link().		*/
    struct stat local_stat;		/* File statistic buffer.	*/

    
    ret = TRUE;
    (void) sprintf( parent, "%s", dest );
    strhead( parent );
    if ( strne( dest, parent ) )
    {
	ret = file_dir_mk( parent, mode );
    }

    
    result =  file_link( dest, &local_stat );
    if ( ret && ( !result && errno == ENOENT ) )
    {
	errno = 0;
	if( link( source, dest ) != 0 )
	{
	    ret = FALSE;
	}
    }
    else
    {
	ret = result;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_link_rm
*
*   Purpose:
*	Removes a particular file, as a consequence decrementing the
*	number of hard links to a file.
*
*   Values Returned:
*	boolean	TRUE		: If file successfully removed.
*	boolean FALSE		: If file not removed.
*
************************************************************************
-*/

boolean	file_link_rm
(
   char		path[PATH_MAX]	/* (in)  Path that is to be cleared. */
)
{
    boolean	ret;			/* Function return value.	*/	
    struct stat local_stat;		/* File statistic buffer.	*/
	
    ret = FALSE;
    if( file_link( path, &local_stat ) )
    {
	if ( unlink( path ) == 0 || errno == ENOENT )
	{
	    errno = 0;
	    ret = TRUE;
	}
    }
    else if ( errno == ENOENT )
    {
	errno = 0;
	ret = TRUE;
    }
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_sym
*
*   Purpose:
*	Determine if a given path is a symbolic link or not.
*
*   Values Returned:
*	boolean	TRUE		: If path is a symbolic linke
*	boolean FALSE		: If path is *not* a symbolic linke
*
************************************************************************
-*/

boolean	file_sym
(
   char		path[PATH_MAX],	/* (in)  Path that is to be checked.	*/
   struct stat 	*stat_buf	/* (out) File statistic buffer.		*/
)
{
    boolean 	ret;		/* Function's return value.		*/
    struct stat	local_stat;	/* Local stat structure.		*/

    
    /*
     * Figure out if the path is a directory or file.
     */

    ret = TRUE;
    errno = 0;

    if ( lstat( path, &local_stat ) )
    {
        ret = FALSE;
    }
    
    if ( ret && ! S_ISLNK( local_stat.st_mode ) )
    {
	ret =  FALSE;
    }

    if ( stat_buf != NULL )
    {
	(void) memcpy( stat_buf, &local_stat, sizeof( struct stat ) );
    }
    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_sym_mk
*
*   Purpose:
*	Creates a hard link between a exisging file and a given
*	path.  If any parent directories do not exist in the given
*	path they are created.
*
*   Values Returned:
*	boolean	TRUE		: If symbolic hard exists.
*	boolean FALSE		: If directory is *not* created.
*
************************************************************************
-*/

boolean	file_sym_mk
(
   char		source[PATH_MAX],	/* (in)  source file.		*/
   char		dest[PATH_MAX],		/* (in)  hard link destination.	*/
   mode_t	mode			/* (in)	 dir. permission mode.	*/
)
{
    char	parent[PATH_MAX];	/* Parent directory.		*/
    boolean     ret;			/* Function return value.	*/
    boolean	result;			/* Result from link().		*/
    struct stat local_stat;		/* File statistic buffer.	*/
	
    
    /*
     * Make sure the source path exists.
     */

    ret = TRUE;
    if ( lstat( source, &local_stat ) )
    {
	ret = FALSE;
    }
		
	    
    /*
     * Create all parent directories if necessary
     */
    
    (void) sprintf( parent, "%s", dest );
    strhead( parent );
    if ( ret && strne( dest, parent ) )
    {
	ret = file_dir_mk( parent, mode );
    }


    /*
     * Create the link if everything is ok so far.
     */

    result = file_sym( dest, &local_stat );
    if ( ret && ( !result  && errno == ENOENT ) )
    {
	errno = 0;
	if( symlink( source, dest ) != 0 )
	{
	    ret = FALSE;
	}
    }
    else
    {
	ret = result;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Function:	file_sym_rm
*
*   Purpose:
*	Removes a particular symbolic link.
*
*   Values Returned:
*	boolean	TRUE		: If symbolic link successfully removed.
*	boolean FALSE		: If symbolic link not removed.
*
************************************************************************
-*/

boolean	file_sym_rm
(
   char		path[PATH_MAX]	/* (in)  Path that is to be cleared. */
)
{
    boolean	ret;			/* Function return value.	*/	
    struct stat local_stat;		/* File statistic buffer.	*/
	

    ret = FALSE;
    if( file_sym( path, &local_stat ) )
    {
	if ( unlink( path ) == 0 || errno == ENOENT )
	{
	    errno = 0;
	    ret = TRUE;
	}
    }
    else if ( errno == ENOENT )
    {
	errno = 0;
	ret = TRUE;
    }
    
    return( ret );
}
#endif /* VxWorks */

