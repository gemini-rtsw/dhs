//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997.				(c) 1997
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
// dhsGen/h/dhsGenTmp.H
//
// PURPOSE:
// Contains a class to create and maintain a temporary working directory
//
// CLASSES:
// cDhsTmpDir - Handle a temporary working directory.
//
// METHODS:
// cDhsTmpDir::cDhsTmpDir - Constructor for the class.
// cDhsTmpDir::~cDhsTmpDir - Destructor for the class.
// cDhsTmpDir::init - Initialize a temporary directory object.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:01  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/05/27 17:55:26  nhill
// Updated the documentation.
//
// Revision 1.1  1997/08/19 20:27:11  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSGENTMP_H
#define	DHSGENTMP_H

#include "gen_types.h"
#include "gen_str.h"

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsTmpDir
//
// PUBLIC METHODS: 
// cDhsTmpDir - Constructor for the class.
// ~cDhsTmpDir - Destructor for the class.
// (void) clear - Remove files and subdirectories in the temporary directory.
// (char *) mkPath - Make a path name in the directory.
// (int) rmPath - Remove a path from the temporary directory.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class is used to maintain a temporary directory structure
//
//-
//***********************************************************************
//

class	cDhsTmpDir
{
    public:
	inline		cDhsTmpDir( char * );
	inline		cDhsTmpDir( void );
	inline		~cDhsTmpDir( void );
	int		clear( ... );
	inline void	init( char * );
	int		mkPath( char *, ... );
	static int	rmPath( char * );
    protected:
    private:
	char		*dtdRoot;	// The temporary directory.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::cDhsTmpDir
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize a new cDhsTmpDir object
//
// DESCRIPTION:
// Trivial.
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

		cDhsTmpDir::cDhsTmpDir
(
    void
)
{
    dtdRoot = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::cDhsTmpDir
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) directory (char *) The temporary directory path.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize a new cDhsTmpDir object
//
// DESCRIPTION:
// Trivial.
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

		cDhsTmpDir::cDhsTmpDir
(
    char	*directory	// (in)  The temporary directory path.
)
{
    dtdRoot = strsav( directory );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::~cDhsTmpDir
//
// INVOCATION:
// n.a. 
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for the cDhsTmpDir class
//
// The destructor removes any files in the temporary directory.
//
// DESCRIPTION:
// Trivial.
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

		cDhsTmpDir::~cDhsTmpDir
(
    void
)
{
    //
    // remove any files in the directory.
    //

    clear( NULL );

    if ( dtdRoot != NULL )
    {
	gen_free( dtdRoot );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTmpDir::init
//
// INVOCATION:
// tmpDir.init( path );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) path (char *) New path for the directory.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set a new path for a temporary directory
//
// DESCRIPTION:
// Set the dtdRoot data member.
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

void		cDhsTmpDir::init
(
    char	*path		// (in)  New path for the directory.
)
{
    if ( dtdRoot != NULL )
    {
	gen_free( dtdRoot );
    }

    dtdRoot = strsav( path );
}
#endif // DHSGENTMP_H
