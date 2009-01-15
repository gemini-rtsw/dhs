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
// dhsData/h/con.H
//
// PURPOSE:
// Contains the definitions for managing connections.
//
// CLASSES:
// cDtsConManager - Connection management.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.4  2001/03/27 21:09:15  nhill
// Changed to be compatable with workshop 5 syntax.
//
// Revision 1.3  1997/09/29 16:18:38  dunn
// Code cleanup.
//
// Revision 1.2  1997/09/17 22:21:14  dunn
// Code cleanup.
//
// Revision 1.1  1997/08/19 22:32:38  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef CON_H
#define	CON_H

#include "genMutex.H"
#include "dhs++.H"
#include "dhsGenConnect.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsConManager
//
// PUBLIC METHODS: 
// (boolean) authorized() - True if the identity string is authorized.
// (bool) connectTo() - Connect to the passed in connection.
// (void) config() - Read the connection configuration.
// (void) close() - Close off and clear up all connections.
// (void) disconnectTo() - Set the disconnection flag for the passed in string.
// (bool) testAll() - Run a test on all connections.
//
// PURPOSE:
// Handles connections.
//
//-
//***********************************************************************
//

class 	cDtsConManager : public cConnectList
{
    public:
	static boolean		authorized( const char * ) ;
	static bool		connectTo( cDtsStatus &, char *, 
					cDhsConnection **);
	static void		config( clStatus & );
	static void		close( void );
	static void		disconnectTo( cDtsStatus &, char * );
	static bool		testAll( cDtsStatus & );

    protected:
	static boolean
				connectors( int, char *const* );
    private:
	inline			cDtsConManager( const char *const* c )
				    :cConnectList ( c ) 
				    {
					cdcClConnection = FALSE;
					cdcSrConnection = FALSE;
				    }
				~cDtsConManager( void );
	inline void		clientConnected( bool x )
				    {
					cdcClConnection = x;
				    };
	inline void		serverConnected( bool x )
				    {
					cdcSrConnection = x;
				    };
	inline bool		clientConnected( void )
				    {
					return ( cdcClConnection );
				    };
	inline bool		serverConnected( void )
				    {
					return ( cdcSrConnection );
				    };

	cMutex			cdcMutexConServer;
						// Protect the connection.
	cMutex			cdcMutexConClient;
						// Protect the connection.
	cDhsConnection		cdcServerConn;	// Server Connection.
	char			cdcConType;	// Connection type, Client or
						// Server
	bool			cdcClConnection;// Indicates client connection.
	bool			cdcSrConnection;// Indicates server connection.
};
#endif // CON_H
