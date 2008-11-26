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
// dhsData/h/store.H
//
// PURPOSE:
// Contains the external definitions for the disk storage management.
//
// CLASSES:
// cDtsStoreManager - Handle all storage management.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  1997/10/03 22:59:24  dunn
// Code cleanup.
//
// Revision 1.5  1997/09/17 22:26:59  dunn
// Test all feature added to this class.
//
// Revision 1.4  1997/08/19 22:33:31  dunn
// Bug fixes.
//
// Revision 1.3  1997/07/29 22:06:14  dunn
// Added fileRemove.
//
// Revision 1.2  1997/07/23 19:45:57  dunn
// changed setTempPath to check status, and to check that the path being set
// actually exists.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSSTR_H
#define	DTSSTR_H

#include <map>
#include <list>
#include <string>

#include "dhs++.H"
#include "dhsGenStatus.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsStoreManager
//
// PUBLIC METHODS: 
// tempVol - Returns the temporary volume.
// permVol - Returns the permanent volume.
// tempPath - Returns the temporary path.
// permPath - Returns the permanent path.
// setTempPath - Sets the temporary path.
// setPermPath - Sets the permanent path.
// (void) init - Initilization
// (void) checkPath - Check that the path passed in exists.
// (void) config - Sets up call to read line in config file.
// (void) determineSpace - Determine space used the passed in storage area.
// (void) fileRemove - Remove a file or directory.
// (void) makeDir - Make a directory.
// (void) testAll - Verify that the temporary and perm disk areas exist.
// (void) updateSpace - Update space on the status server.
//
// PURPOSE:
// Maintains the temporary and permanent storage information.
//
//-
//***********************************************************************
//

class 	cDtsStoreManager
{
    public:
	//
	//  The following are defines for locking temporary or perm.
	//

	typedef enum
        {
	    	ST_PERM = 0,
		ST_TEMP = 1
	} tLife;

	static inline char *
			tempVol( void )
			    { 
				return( smTempVol ); 
			    };
	static inline char *
			permVol( void )
			    { 
				return( smPermVol ); 
			    };
	static inline char *
			tempPath( void )
			    { 
				return( smTempPath ); 
			    };
	static inline char *
			permPath( void )
			    { 
				return( smPermPath ); 
			    };
	static inline void
			setTempPath( cDtsStatus &status, char *x )
			    { 
				smTempPath = x; 
				checkPath( status, ST_TEMP );
			    };
	static inline void
			setPermPath( cDtsStatus &status, char *x )
			    { 
				smPermPath = x;
				checkPath( status, ST_PERM );
			    };

	static void	init( cDtsStatus & );
	static void	checkPath( cDtsStatus &, tLife );
	static void	config( cDtsStatus & );
	static int	determineSpace( cDtsStatus &, tLife );
	static void	fileRemove( cDtsStatus &, char *, tLife, int );
	static void	makeDir( cDtsStatus &, char *, char * );
	static bool	testAll( cDtsStatus & );
	static void	updateSpace( cDtsStatus &, tLife, int );
    protected:
    private:
	static boolean	store( int, char ** );	

	static char	*smTempPath;	// Path to the temporary storage area.
	static char	*smPermPath;	// Path to the temporary storage area.

	static char	*smTempVol;	// Volume name in ad for temp.
	static char	*smPermVol;	// Volume name in ad for perm.

	static int	smTempSpTotal;	// Total space in temp storage.
	static int	smPermSpTotal;	// Total space in perm storage.

	static int	smTempSpUsed;	// Space used in temp storage.
	static int	smPermSpUsed;	// Space used in perm storage.

	static cMutex	cdmMutexTempSz;	// Mutex to protect temp size.
	static cMutex	cdmMutexPermSz;	// Mutex to protect temp size.

};

#endif // DTSSTR_H
