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
// dhsQlServer/h/qlsFitsKeyword.H
//
// PURPOSE:
// Contains the definition of the cQlsFitsKeyword class.
//
// CLASSES:
// cQlsFitsKeyword - Class to keep track of a list of FITS keywords.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1997/08/20 21:20:06  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSFITSKEYWORD_H
#define	QLSFITSKEYWORD_H

#include <string>
#include <map>

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsFitsKeyword
//
// PUBLIC METHODS: 
//		cQlsFitsKeyword	- Constructor for the cQlsFitsKeyword class.
//		~cQlsFitsKeyword- Destructor for the cQlsFitsKeyword class.
// (static void)config		- Read the keywords from the configuration file.
// (static void)exit		- Called to delete all objects of this class.
// (static cQlsFitsKeyword)	- Locate a cQlsFitsKeyword object based on
//		find		  the keyword name.
// (static bool)required	- Returns true of the keyword should be copied.
//
// PUBLIC TYPES:
// tKeywordList - STL map of cQlsFitsKeyword objects indexed by keyword name
//		and whether it is in the main header or not.
// iKeywordList - Iterator into the tKeywordList type.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Class to keep track of a list of the FITS keywords
//
// An object of this class is created to store the information read from the
// configuration file about each FITS keyword.
//
//-
//***********************************************************************
//

class 	cQlsFitsKeyword
{
    private:
	//
	// This class is declared first, since the typedef of type tKeywordList
	// must have it available.
	//

	class		cFKWKey
	{
	    public:
		inline		cFKWKey( const char * name, bool main )
				{
				    fkwkName = name;
				    fkwkMainHeader = main;
				}
		bool		operator<( const cFKWKey & ) const;
	    protected:
	    private:
		const char	*fkwkName;	// Keyword name.
		bool		fkwkMainHeader;	// True if the keyword is from 
						// the main header.
	};
    public:
	typedef map < cFKWKey, cQlsFitsKeyword *, less<cFKWKey> >
			tKeywordList;
	typedef tKeywordList::iterator 
			iKeywordList;

			cQlsFitsKeyword( char *, bool, bool, bool );
			~cQlsFitsKeyword( void );
	static void	config( cQlsStatus & );
	static void	exit( cQlsStatus & );
	static cQlsFitsKeyword
			*find( const char *, bool, cQlsStatus & );
	inline bool	isRequired( bool isFirst )
			{
			    return( ( isFirst && qfkFirstChunk ) || 
				    ( ( ! isFirst ) && qfkNextChunk ) );
			}
    protected:
    private:
	static boolean	fitsKeyword( int, char ** );
	static bool	strtobool( char * );

	static cQlsFitsKeyword		// Default keyword information.
			*qfkPDefaultKwd;
	bool		qfkFirstChunk;	// True if keyword should be copied from
					// the first chunk
	static tKeywordList
			qfkKeywordList;
	bool		qfkMainHeader;	// True of the keyword applies to one
					// in the main header.
	char		*qfkName;	// Name of the keyword.
	bool		qfkNextChunk;	// True if the keyword should be copied
					// from chunks other than the first.
};
#endif // QLSFITSKEYWORD_H
