static char rcsid[] = "$Id: dhsGenTmp.C,v 1.1.1.1 2002-11-24 20:27:51 brighton Exp $";
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
// fitness for a particular pur-	ou de pertinence pour un usage
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
// dhsGen/src/dhsGenTmp.C
//
// PURPOSE:
// Contains the implementation of the cDhsTmpDir class.
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1997/11/30 21:17:32  nhill
// Bug fix.
//
// Revision 1.3  1997/08/25 23:06:50  nhill
// Added checks for an un initialized object.
//
// Revision 1.2  1997/08/20 21:26:43  nhill
// Fixed a bug in cDhsTmpDir::mkPath.
//
// Revision 1.1  1997/08/19 20:27:05  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "dhsGenTmp.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::clear
//
// INVOCATION:
// tmpDir.clear( pathPart1, pathPart2, NULL );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) ... (char *) A list of path segments to assemble into a path.
//
// FUNCTION VALUE:
// (int) -1 for success, 0 for failure.
//
// PURPOSE:
// Clear a directory and all files and directories within it
//
// On failure, errno should be set.
//
// DESCRIPTION:
// Description of the algorithm.
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

int		cDhsTmpDir::clear
(
    ...				// (in)  A list of path segements to assemble
    				//	 into a path.
)
{
    va_list	varg;
    char	*segment;	// Each individual path segment.
    char	path[PATH_MAX];	// total path to delete.


    if ( dtdRoot == NULL )
    {
	return( 0 );
    }


    //
    // Assemble a complete path to clear.
    //

    va_start( varg, this ); // XXX allan: changed *this to this

    strcpy( path, dtdRoot );
    while ( ( segment = va_arg( varg, char * ) ) != NULL )
    {
	strcat( path, "/" );
	strcat( path, segment );
    }

    va_end( varg );

    return( rmPath( path ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::rmPath
//
// INVOCATION:
// cDhsTmpDir::rmPath( path );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) path (char *) The path to remove.
//
// FUNCTION VALUE:
// (int) - 0 on success, -1 on failure.
//
// PURPOSE:
// Equivalent to the unix rm -rf <path> command.
//
// DESCRIPTION:
// Description of the algorithm.
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

int		cDhsTmpDir::rmPath
(
    char	*path		// (in)  The path to remove.
)
{
    int		i;
    struct dirent
    		*pDirEnt;
    DIR		*pDir;
    struct stat	statBuf;
    char	localPath[PATH_MAX];


    //
    // Figure out if the path is a directory or file.
    //

    i = stat( path, &statBuf );
    if ( i != 0 && errno == ENOENT )
    {
	return( 0 );
    }
    else if ( i != 0 )
    {
	return( i );
    }


    if ( S_ISDIR( statBuf.st_mode ) )
    {
	pDir = opendir( path );
	if ( pDir == NULL )
	{
	    return( -1 );
	}
	
	pDirEnt = (struct dirent *) malloc( sizeof( struct dirent ) + 
		pathconf( path, _PC_NAME_MAX ) + 1 );
	struct dirent* pResult;
	while ( readdir_r( pDir, pDirEnt, &pResult) != 0 )
	{
	    if ( strcmp( pDirEnt->d_name, "." ) != 0 && 
		    strcmp( pDirEnt->d_name, ".." ) != 0 )
	    {
		strcpy( localPath, path );
		strcat( localPath, "/" );
		strcat( localPath, pDirEnt->d_name );
		i = rmPath( localPath );
		if ( i != 0 )
		{
		    return( i );
		}
	    }
	}

	free( pDirEnt );

	(void) closedir( pDir );
	return( rmdir( path ) );
    }
    else
    {
	return( unlink( path ) );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::mkPath
//
// INVOCATION:
// tmpDir.mkPath( path, segment1, segment2, NULL );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) path (char[PATH_MAX]) The new path name.
// (>) ... (char *) The segments to join to create the path.
//
// FUNCTION VALUE:
// (int) - -1 for failure, 0 for success
//
// PURPOSE:
// Create a new pathname for a temporary file
//
// Returns -1 on failure, and errno will be set.
//
// DESCRIPTION:
// Description of the algorithm.
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

int		cDhsTmpDir::mkPath
(
    char	*path,		// (out) The output path name.
    ...				// (in)  The segments to join to make the path.
)
{
    va_list	varg;
    char	*segment;	// Each individual path segment.
    struct stat	statBuf;
    char	*start;
    char	*end;
    int		i;


    if ( dtdRoot == NULL )
    {
	return( -1 );
    }


    //
    // Assemble a complete path to clear.
    //

    va_start( varg, path );  // XXX allan: changed *this to path

    strcpy( path, dtdRoot );
    while ( ( segment = va_arg( varg, char * ) ) != NULL )
    {
	strcat( path, "/" );
	strcat( path, segment );
    }
    va_end( varg );


    for ( start = path; ( end = strchr( start, '/' ) ) != 0;
	    start = end + 1 )
    {
	if ( end != path )
	{
	    *end = '\0';

	    i = stat( path, &statBuf );
	    if ( i != 0 && errno == ENOENT )
	    {
		i = mkdir( path, S_IRWXU | S_IRWXG | S_IRWXO );
		if ( i != 0 )
		{
		    return( i );
		}
	    }
	    else if ( i != 0 )
	    {
		return( i );
	    }

	    *end = '/';
	}
    }

    return( 0 );
}
