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
// dhsStorage/h/stoConnection.H
//
// PURPOSE:
// Contains the definitions and declarations for the DHS 
// Storage Server connection class.
//
// CLASSES:
// cStoConnection - Connections to/from the Storage Server. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:21  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.7  2000/12/19 19:37:02  nhill
// Updated for workshop 5.
//
// Revision 1.6  1999/06/16 23:47:25  nhill
// Added the NO_FIND_CREATE flag.
//
// Revision 1.5  1998/06/25 18:09:51  cockayne
// Modified to match data server's config file syntax.
//
// Revision 1.4  1998/05/15 17:17:17  cockayne
// Modification sto send startup commands to list of data servers.
//
// Revision 1.3  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.2  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOCONNECTION_H
#define	STOCONNECTION_H

#define NO_FIND_CREATE
#include <list>
#include <string>
#include "genListOf.H"
#include "globals.H"
#include "stoStartCmd.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cStoConnection
//
// PUBLIC METHODS: 
// (cStoConnection)	cStoConnection	- Constructor (two flavours).
// (void)		exit		- Exit method deconstructs all in list.
// (void)		connectionClose	- Required for closing all connections.
// (cDhsConnection *)	getDataServerConnection- Return Data Server connection.
// (void)		config		- Configuration method for class. 
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Holds the list of authorized connectors.
//
//-
//***********************************************************************
//

class cStoConnection :
	public cDhsConnection,
	public cListOf< cStoConnection, std::string, cStoStatus >
{
    public:
	typedef std::list< cStoConnection * >		notL;
	typedef std::list< cStoConnection * >::iterator	notIt;


	//
	// Constructors, Deconstructors.
	//

	inline		cStoConnection( std::string )
			:   cListOf< cStoConnection, std::string, cStoStatus >
				( "", false )
			{
			};
	inline		cStoConnection( char *identity, char *ipAddress,
				bool = true )
			:   cListOf< cStoConnection, std::string, cStoStatus >
				( std::string( identity ), true ),
			    cDhsConnection( ipAddress, identity )
			{
			};

	inline static void
			exit( cStoStatus &status )
			{
			    notIt		connListIter;

			    checkStat( status, return );
			    forEach( &cStoConnection::connectionClose, NULL, status );
			    deleteAll();
			    for ( connListIter = scNotifyList.begin();
				    scNotifyList.size() != 0;
				    connListIter = scNotifyList.begin() )
			    {
				scNotifyList.erase( connListIter );
			    }
			    return;
			};
	inline void	connectionClose( void *, cStoStatus &status )
			{
			    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
			    checkStat( status, return );

			    if ( this->isOpen() )
			    {
				checkDhs( this->close( dhsStatus ), 
					dhsStatus, status, VOID );
			    }
			};
	inline static cDhsConnection
			*getDataServerConnection( void )
			{
			    return( scPDataServer );
			};

	static void	config( cStoStatus & );
	static void	aliveCommands( cStoStatus & );


    protected:
    private:
	static boolean	localDataServer( int, char *[] );
	static boolean	notifyDataServer( int, char *[] );
	static boolean	authorized( int, char *[] );


	//
	// Private Data.
	//

	static notL	scNotifyList;		// List of servers to notify
						// when Storage Server starts.
	static cDhsConnection	
			*scPDataServer;		// Ptr to connection to 
						// Data Server.
};
#endif // STOCONNECTION_H
