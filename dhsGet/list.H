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
// dhsGet/h/list.H
//
// PURPOSE:
// Contains the definitions for the list of datasets to get.
//
// CLASSES:
// cGetList - List of datasets.
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GETLIST_H
#define	GETLIST_H

#include <map>
#include <list>
#include <string>


//
//***********************************************************************
//+
// CLASS NAME:
// cGetList
//
// PUBLIC METHODS: 
// (char *) getNext - Get the next dataset off the list.
// (char *) getName - Return the dataset name.
// (void) readIn - Read from argument list.
//
// PURPOSE:
// Contains and maintains a list of datasets to get.
//
//-
//***********************************************************************
//

class 	cGetList
{
    public:
	typedef map< string,		// Sort key is of datatype string.
		     cGetList *,	// Insert into this containr this type
		     less<string> >	// This is the sorting mechanizm.
		tGetList;		// This var. provides access to containr
	typedef tGetList::iterator	// This provides an iterator to containr
		iterator;

	static boolean
		readIn( char *, cGetStatus & );
	static cGetList * 
		getNext( cGetList::iterator & );
	inline char *
			getName ( void ) { return ( cgtDatasetName ); };

    protected:
    private:
		cGetList( const char * );
					// Constructor. Adds to list.
		~cGetList();		// Destructor.  Deletes from list.
	static tGetList
		cgtGetList;		// List of all datasets to get.
	char	*cgtDatasetName;	// Dataset name. 
	static void
		exit( cGetStatus & );	// Cycles through all objects.
	
};

#endif // GETLIST_H
