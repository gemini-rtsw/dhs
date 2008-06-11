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
// dhsPut/h/list.H
//
// PURPOSE:
// Contains the definitions for the list of files to put.
//
// CLASSES:
// cPutList - List of files to put to the dataserver.
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef PUTLIST_H
#define	PUTLIST_H

#include <map>
#include <list>
#include <string>



//
//***********************************************************************
//+
// CLASS NAME:
// cPutList
//
// PUBLIC METHODS: 
// (char *) getNext - Get the next filename off the list.
// (char *) getName - Return the filename name.
// (char *) getFileId- Return the filename name.
// (void) readIn - Read from argument list.
//
// PURPOSE:
// Contains and maintains a list of filename to put.
//
//-
//***********************************************************************
//

class 	cPutList
{
    public:
	typedef std::map< std::string,		// Sort key is of datatype string.
		     cPutList *,	// Insert into this containr this type
		     std::less<std::string> >	// This is the sorting mechanizm.
		tPutList;		// This var. provides access to containr
	typedef tPutList::iterator	// This provides an iterator to containr
		iterator;

	static boolean
		readIn( char *, cPutStatus & );
	static cPutList * 
		getNext( cPutList::iterator &, bool  );
	inline char * getName ( void ) { return ( cptFilename ); };
	inline char * getFileId ( void ) { return ( cptFileId ); };
	inline bool   getLast ( void ) { return ( cptLastFlag ); };
	static void   setLast ( cPutStatus &  );
	static void   setAllDSNames ( cPutStatus &  );

    protected:
    private:
		cPutList( const char * );
					// Constructor. Adds to list.
		~cPutList();		// Destructor.  Deletes from list.
	static tPutList
		cptPutList;		// List of all files to put.
	char	*cptFilename;		// Filename. 
	char	*cptFileId;		// FileId. 
	bool	cptLastFlag;		// Lastflag. 
	static void
		exit( cPutStatus & );	// Cycles through all objects.
	
};

#endif // PUTLIST_H
