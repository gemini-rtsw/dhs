//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 2000				(c) 2000
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
// dhsStoPut/h/list.H
//
// PURPOSE:
// Contains the definitions for the list of files to put/sent to the
// Storage Server.  Also contains the definition for the list of
// mediaTypes and the mapping from retrieval to DHS media types.
//
// CLASSES:
// cStoPutList 	    - List of files to put to the storage server.
// cMediaTypeList   - Lisf of media types allowed.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.3  2001/01/12 22:15:17  jaeger
// Added proper documentation.  Added methods findFile and getFileSize to
// class cStoPutList.  Added status parameter to cMediaTypeList constructor.
//
// Revision 1.2  2000/12/19 19:35:32  jaeger
// Added size method to the put list.  Fixed up findDhsMeiaType to work
// properly. Added getRetMediaType and getDhsMediaType methods to the
// mediaType list.
//
// Revision 1.1  2000/12/04 22:02:36  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOPUTLISTS_H
#define	STOPUTLISTS_H

#include <map>
#include <list>
#include <string>

//
//***********************************************************************
//+
// CLASS NAME:
// cStoPutList
//
// PUBLIC METHODS:
// (cStoPutList*) findFile  - Find a given file on the list.    
// (int) getFileSize	    - Get the size of the file.
// (cStoPutList *) getNext  - Get the next filename off the list.
// (char *) getSrcName 	    - Return the source filename name.
// (char *) getDestName     - Return the source filename name.
// (char *) getFileId	    - Return the filename name.
// (void) readIn 	    - Read from argument list.
// (int) size		    - Find the size of the list.    
//
// PURPOSE:
// Contains and maintains a list of filename to put.
//
//-
//***********************************************************************
//

class 	cStoPutList
{
    public:
	typedef map< string,		// Sort key is of datatype string.
		     cStoPutList *,	// Insert into this containr this type
		     less<string> >	// This is the sorting mechanizm.
		tStoPutList;		// This var. provides access to containr
	typedef tStoPutList::iterator	// This provides an iterator to containr
		iterator;

	static void
		readIn( char *, char *, cStoPutStatus & );
	static cStoPutList * 
		findFile( const char* );
	static cStoPutList * 
		getNext( cStoPutList::iterator &, bool  );
	inline char * getSrcName ( void ) { return ( cptSrcFilename ); };
	inline char * getDestName ( void ) { return ( cptDestFilename ); };
	inline char * getFileId ( void ) { return ( cptFileId ); };
	inline int getFileSize ( void ) { return ( cptFileSize ); };
	static int size ( void ) { return( cptStoPutList.size() ); }
	

    protected:
    private:
		cStoPutList( const char *, const char *, cStoPutStatus & );
					// Constructor. Adds to list.
		~cStoPutList();		// Destructor.  Deletes from list.
	static tStoPutList
		cptStoPutList;		// List of all files to put.
	char	*cptSrcFilename;	// Source file path.
	char	*cptDestFilename;	// Destination file path. 
	char	*cptFileId;		// FileId. 
	int	cptFileSize;		// FileSize
	static void
		exit( cStoPutStatus & );	// Cycles through all objects.
	
};


//
//***********************************************************************
//+
// CLASS NAME:
// cMediaTypeList
//
// PUBLIC METHODS:
// (void) config	       - Read mediaType information from 
//				 the configuration file.	
// (char*) findDhsMedType      - Find the Dhs Media type corresponding
//    				 to a given dhsStoPut mediatype.
// (cMediatypelist *) getNext  - Get the next filename off the list.
// (char *) getRetMediaType    - Return the retrieval media type.
// (char *) getDhsMediaType    - Return the DHS media type.
//
// PURPOSE:
// Contains and maintains a list of media types.
//
//-
//***********************************************************************
//

class 	cMediaTypeList
{
    public:
	typedef map< string,		// Sort key is of datatype string.
		     cMediaTypeList *,	// Insert into this container this type
		     less<string> >	// This is the sorting mechanizm.
		tMediaTypeList;		// This var. provides access to container
	typedef tMediaTypeList::iterator // This provides an iterator to container
		iterator;

	static void	config( cStoPutStatus & );
	inline const char *
		getRetMediaType( void ) { return( cptRetMediaType ); };
	inline const char *
		getDhsMediaType( void ) { return( cptDhsMediaType ); };
	static const char *
		findDhsMediaType ( const char * ); 

    protected:
    private:
	cMediaTypeList( const char *, const char *, cStoPutStatus & );
					// Constructor. Adds to list.
	~cMediaTypeList();		// Destructor.  Deletes from list.
	static tMediaTypeList
		cptMediaTypeList;		// List of all media types.
	static void
		exit( cStoPutStatus & );	// Cycles through all objects.
	char	*cptRetMediaType;		// Retrieve media type.
	char	*cptDhsMediaType;		// Dhs media type.
	static boolean	mediaTypeConfig( int, char *[] );
};

#endif // STOPUTLISTS_H

