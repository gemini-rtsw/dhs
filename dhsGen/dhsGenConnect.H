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
// dhsGen/h/dhsGenConnect.H
//
// PURPOSE:
// Contains the external definitions for authorized users.
//
// CLASSES:
// cConnectList - List of authorized connectors
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:41  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2001/03/27 23:54:22  nhill
// Increased the constness of the argument to the cConnectList constructor.
//
// Revision 1.7  2000/08/17 23:51:41  nhill
// Fixed a C++ compatability problem.
//
// Revision 1.6  1998/04/27 20:52:14  dunn
// Got rid of E_NOT_FOUND msgId
//
// Revision 1.5  1998/04/21 05:35:44  nhill
// Removed the E_CONFIG and E_CONNECT status codes. They are available
// by default in the cDhsStatus class.
//
// Revision 1.4  1997/09/10 19:12:54  dunn
// Added returning of the identity string.
//
// Revision 1.3  1997/08/19 20:00:29  dunn
// Cleaned up the file.
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSCONN_H
#define	DHSCONN_H

#include <map>
#include <list>
#include <string>


//
//  Define macro to check status.  If including this file, then must have
//  an error status of E_CONN_LIST defined also.
//

#define	checkConnection( fn, cltStatus, status, ret )			\
	fn;								\
	if ( cltStatus != cltStatus.S_SUCCESS )				\
	{								\
	    status.E_CONN_LIST( status, #fn, cltStatus.message() );	\
	    status.display();						\
	    cltStatus.clear();						\
	    ret;							\
	}

//
//***********************************************************************
//+
// CLASS NAME:
// cConnectList
//
// PUBLIC METHODS: 
// (void) connected  	- Set the connected flag.
// (bool) connected	- Return the connected flag.
// (char *) identity	- Return the identity string.
// (char *) ipAddr 	- Return the ip address.
// (boolean) authorized - Return TRUE if the identity string is an auth. user
// (void) config 	- Read from a configuration for the list of autho. users
// (void) init		- Initialize the class.
// (char *) lookup 	- Lookup the ip address of an identity string passed in.
// (cConnectList *) lookup
//			- Lookup and return a pointer in list given identity.
//
// PURPOSE:
// Contains and maintains a list of valid connectors.
//
//-
//***********************************************************************
//

class 	cConnectList
{

    public:
	class	clStatus: public cDhsStatus	// Status return values
	{
	    public:
		msgPrefix( "dg" );
		msgId( S_CONN_DEL );
		msgId( S_CONN_ADD );
		msgId( E_MULTI_KEYWORD );
	    protected:
	    private:
	};

	inline void	connected( bool flag )
			    {
				cctConnected = flag;
			    };
	inline bool	connected( void )
			    {
				return( cctConnected );
			    };
	inline char	*ipAddr( void )
			    {
				return( cctIpAddress );
			    };
	inline char	*identity( void )
			    {
				return( cctIdentityString );
			    };

	typedef 	std::map< std::string,	// Sort key is of datatype string.
		     	cConnectList *,	// Container has this type
		     	std::less<std::string> >	// This is the sorting mechanizm.
			tConnectList;	// Provides access to container.
	typedef tConnectList::iterator
			iterator;	// Provides an iterator into containr

	static boolean	authorized( const char * ) ;
					// Return TRUE if found identity string.
	static void
			config( clStatus & );
					// Read from config file.
	static void
			init( cDhsStatus::tDebugLevel, clStatus & );
					// Pass on debug level.
	static char	*lookup( const char * );
					// Lookup an IP address.
	static cConnectList
			*lookup( const char *, bool );
					// Lookup and return ptr in list.

    protected:
	static tConnectList
			cctConnectList;	// List of all auth. connectors.
			cConnectList( const char *const* );
					// Constructor. Adds to list.
			~cConnectList();
					// Destructor.  Deletes from list.
	static boolean
			connectors( int, char ** );
	static void
			exit( clStatus & );
					// Cycles through all objects.
    private:

	char		*cctIdentityString;
					// Identity String
	char		*cctIpAddress;
					// Ip Address
	bool		cctConnected;
					// Are connected.
					// Parses the tokens in config.
	
};

#endif // DHSCONN_H
