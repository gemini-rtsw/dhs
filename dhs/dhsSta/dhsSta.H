//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998
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
// dhsSta/h/dhsSta.H
//
// PURPOSE:
// Contains the local interface to the cStaStatus class.
//
// CLASSES:
// cStaStatus - Interface to the dhs status server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/05/04 20:49:34  nhill
// Added a memory of subsystem health.
//
// Revision 1.3  1998/04/24 21:21:20  nhill
// Moved this file from the dhsGen library.
//
// Revision 1.2  1998/04/23 22:28:57  nhill
// Added a method to return the connection being used by the cStaStatus
// class to talk to the status server.
//
// Revision 1.1  1998/04/21 05:50:05  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSSTA_H
#define	DHSSTA_H

#include "dhsGenStatus.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cStaStatus
//
// PUBLIC METHODS: 
// config - Read the status server configuration information from a
//	configuration file.
// connect - Connect to the status server.
// disconnect - Disconnect from the status server.
// init - Initialize this class.
// flush - Flush any status updates to the status server.
// update - Update a status record in the status server.
// resetHealth - Reset the application health to good.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PUBLIC PROPERTIES
// P_LOG - The message should be sent to the status server as a log message.
// P_HEALTH_BAD - If the status is set, set the health to bad.
// P_HEALTH_WARN - If the status is set, set the health to warning.
//
// PURPOSE:
// This class provides an interface to the DHS status server
//
// The class inherits from the cDhsStatus class, and should be used instead
// of cDhsStatus by applications that will use the status server to report
// their health and state.  The class provides methods to connect to the
// status server, to update status records in the status server, adds
// properties to allow messages to be logged to the status server and to
// update the applications health indicated by the status server.
//
//-
//***********************************************************************
//

class cStaStatus: public cDhsStatus
{
    public:
	msgId( E_SS_OPEN );

	msgProperty( P_LOG );
	msgProperty( P_HEALTH_BAD );
	msgProperty( P_HEALTH_WARN );

	static void	config( cStaStatus & );
	static void	connect( cStaStatus & );
	inline static const cDhsConnection
                        &connection ( void )
                        {
                            return( *ssPConnection );
                        };
	static void	disconnect( cStaStatus & );
	static void	init( cDhsConnection &, const char *, cStaStatus & );
	static void	init( const char *, cStaStatus & );
	static void	flush( cDhsStatus & );
	static void	update( const char *, const char *, 
				cDhsStatus & );
	static void	resetHealth( cDhsStatus &status )
			{
			    update( "health", "GOOD", status );
			    update( "healthDesc", "GOOD", status );
			    ssHealth = NULL;
			    flush( status );
			}
    protected:
    private:
	static boolean	statusServer( int, char *[] );
	static bool	messageLog( cDhsStatus &, 
				const cDhsStatus::cStatusProperty &, 
				const char * );
	static bool	healthSet( cDhsStatus &,
				const cDhsStatus::cStatusProperty &,
				const char * );

	static char	*ssAddress;	// Status server internet address.
	static bool	ssConnectAlloc;	// Is the connection allocated by this 
					// lib?
	static char	*ssId;		// Status server IMP id.
	static bool	ssIsInit;	// Has the server been initialized?
	static cMutex	ssMutex;	// Mutex for status updates.
	static cDhsConnection		// Connection to the status server.
			*ssPConnection;
	static const char		// Prefix for this application.
			*ssPrefix;
	static cStaCommand		// Status update command pointer.
			*ssPStaCommand;
	static const cStatusProperty	// The current health of the subsystem.
			*ssHealth;
};
#endif // DHSSTA_H
