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
// dhsStatus/h/subsystem.H
//
// PURPOSE:
// Contains the interface to the cDhsSubsystem class.
//
// CLASSES:
// cDhsSubsystem - Keeps a list of DHS subsystems.
//
// MACROS:
// checkSubsystem - Checks the return status from a call to a method
//		in the cDhsSubsystem class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:46  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  2001/03/12 20:58:37  nhill
// Changed to work with the workshop 5 compilters.
//
// Revision 1.5  1998/06/24 23:13:19  nhill
// Changed the operator== method to take a const char *.
//
// Revision 1.4  1998/04/21 17:32:46  nhill
// Removed the E_CONFIG and E_CONFIG_KEYWORD messages since they
// are available in the cDhsStatus class.
// Updated the documentation.
//
// Revision 1.3  1997/06/11 19:50:58  nhill
// Changed to a new status class.
//
// Revision 1.2  1997/04/24 19:33:39  nhill
// Added some "const" keywords.
//
// Revision 1.1  1997/04/15 23:06:07  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef SUBSYSTEM_H
#define	SUBSYSTEM_H

#include <vector>
#include <algorithm>

#ifdef STL_NAMESPACE
using namespace STL_NAMESPACE;
#endif // STL_NAMESPACE

#define	checkSubsystem( fn, ssStatus, status, ret )			\
	fn;								\
	if ( ssStatus != ssStatus.S_SUCCESS )				\
	{								\
	    status.E_SUBSYSTEM( status, #fn );				\
	    status += ssStatus;						\
	    ssStatus.clear();						\
	    ret;							\
	}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsSubsystem
//
// PUBLIC METHODS: 
// (bool) operator== - Compare a subsystem name with a character string.
// (iterator) begin - Return the iterator pointing to the start of the list.
// (void) config - Read a list of subsystems from the configuration file.
// (iterator) end - Return the iterator pointing **past** the end of the list.
// (void) exit - Shut down the class for exit.
// (const char *) name - Return the name of the subsystem.
// (cDhsSubsystem *) subsystemFind - Locate a subsystem matching the 
//		specified name.
//
// PUBLIC TYPES: 
// tSysList - A list of subsystems.
// iterator - An iterator into the list of subsystems.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Stores a list of subsystems
//
// The list of subsystems is based on the STL vector class, so the 
// interface to this class is also similar to the STL vector interface.
//
//-
//***********************************************************************
//

class 	cDhsSubsystem
{
    public:
	class	cStatus: public cDhsStatus
	{
	    public:
		msgPrefix( "dg" );
		msgId( E_NOT_FOUND );
	    protected:
	    private:
	};

	typedef	std::vector<cDhsSubsystem *> tSysList;
	typedef	tSysList::iterator	iterator;

	enum{ SYSTEM_NAME_LENGTH = 4 };

	inline bool	operator==( const char * name ) const
			    { return( strcmp( name, csName ) == 0 ); };
	static inline iterator	
			begin( void )
			    { return( csList.begin() ); };
	static void	config( cStatus & );
	static inline iterator	
			end( void )
			    { return( csList.end() ); };
	static void	exit( cStatus & );
	inline const char *name( void ) 
			    { return( csName ); };
	static inline cDhsSubsystem
			*subsystemFind( const char * name, cStatus &status )
			    { 
				iterator	i;
				for ( i = begin(); i != end(); i++ )
				{
				    if ( strcmp( (*i)->csName, name ) == 0 )
				    {
					return( *i );
				    }
				}
				status.E_NOT_FOUND( status, name );
				return( NULL );
			    }
    protected:
			cDhsSubsystem( const char * );
	virtual		~cDhsSubsystem( void ) {};
    private:
	inline		cDhsSubsystem( const cDhsSubsystem & ) 
			    { throw "a fit"; };
	inline cDhsSubsystem
			&operator = ( const cDhsSubsystem & ) 
			    { throw "a fit"; };
	static boolean	subsystem( int, char ** );

    	static tSysList	csList;		// List of all subsystems.
	char		csName[SYSTEM_NAME_LENGTH+1];	
					// Name of the Subsystem.
};
#endif // SUBSYSTEM_H
