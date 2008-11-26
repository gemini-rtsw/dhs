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
// dhsStoPut/h/stoPutDhs.H
//
// PURPOSE:
// Header file for the initialization class stoPutDhs.
//
// CLASSES:
// cStoPutDhs		- Class to initialize dhsPut
// cPutFile		- Empty class, used to do the sent (put) the
// 		  	  file to the Storage Server.
// cStoPutDhsError	- DHS error handler.
// cStoPutDhsCmd	- DHS command handler.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.5  2001/01/12 22:17:44  jaeger
// Added proper documentation.  Moved methods that were to large to be
// inline into stoPutDhs.C which includes: setArchiveStream, setCompression
// setIdentity, setStoIdentity, setStoIpAddr, setUnitPrefix, and setUserStream.
// Removed cStoPutDhs constructor as it was commented out anyways and
// defined in the class definition.
//
// Revision 1.4  2000/12/19 19:34:38  jaeger
// Changed how the status was being passed in the "set" methods.
// The address is now being passed.
//
// Revision 1.3  2000/12/12 00:28:29  jaeger
// Changed the arguments to the cPutFile constructor.  The buffer of data
// no longer needs to be passed back out.
//
// Revision 1.2  2000/12/11 20:05:17  jaeger
// Changed arguments to the constructor of the cPutFile class.
//
// Revision 1.1  2000/12/04 22:02:36  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOPUTDHS_H
#define	STOPUTDHS_H

#include "dhs++.H"


//
//***********************************************************************
//+
// CLASS NAME:
// stoPutDhs
//
// PUBLIC METHODS: 
// (void) stoPutDhs 		- constructor.
// (void) ~stoPutDhs 		- deconstructor.
// (void) exit 			- Shutdown nicely.
// (void) init 			- Initialize the dhs stuff.
// (void) kill 			- Force an exit.
// (void) start 		- Start the event loop.
// (void) config		- Read configuration information.   
// (cDhsConnection) getConnect 	- Returns connection to the Storage Server.
// (void) setArchiveStream	- Set the archive stream.
// (void) getArchiveStream	- Set the archive stream.    
// (void) setCompression	- Set the compression applied to the files.
// (void) getCompression	- Set the compression applied to the files.
// (void) setFileNotify		- Set the file notification: True = okay.
// (void) setIdentity 		- Set the ID of this Storage Server client.
// (void) setMediaType		- Set the type of media to be written.
// (void) getMediaType		- Set the type of media to be written.
// (void) setNumCopies		- Set the number of media to create.
// (void) setNumCopies		- Set the number of media to create.    
// (char*) getOrganization	- Returns the organization name.
// (char*) setOrganization	- Sets the organization name.
// (void) setUnitPrefix		- Set the media name prefix.
// (void) setUnitPrefix		- Set the media name prefix.    
// (void) setStoIdentity 	- Set the Storage Server identity.
// (void) setStoIdentity 	- Set the Storage Server identity.
// (void) setStoIpAddr 		- Set the IP address of the Storage Server.
// (void) setUserStream		- Set the user stream to put to.
// (void) setUserStream		- Set the user stream to put to.
// (void) archiveFileAvail	- Sends an stoArchiveFileAvailable command
//   				  to the Storage Server.
// (void) userMediaRequest	- Sends an stoUserMediaRequests command
//   				  to the Storage Server.    
//
// PURPOSE:
// Takes care of the DHS++ library setup and variables setup.
// Also is responsible for Sending the stoUserMediaRequest
// and stoArchiveFileAvailable commands to the Storage Server.
//
//-
//***********************************************************************
//

class	cStoPutDhs
{
    public:
			cStoPutDhs( void ) { sdPDhs = NULL; };	// Constructor
			~cStoPutDhs( void ); 	// Destructor
	void		exit( cStoPutStatus & );
	void		init( cStoPutStatus & );
	void		kill( void );
	void		start( cStoPutStatus & );
	static void	config( cStoPutStatus & );
	inline static bool
                        simulate( void ) { return( sdSimulate ); }
	inline static void
			setSimulate( bool sim ) { sdSimulate = sim; };
	static inline cDhsConnection
			&  getConnect ( void ) { return( sdStoConnect ); };

	static inline const char *
			getArchiveStream( void ) { return( sdArchiveStream ); };
	static void	setArchiveStream( char *, cStoPutStatus & );
	
	static inline const char *
			getCompression( void ) { return ( sdCompression ); };
	static void	setCompression( char *, cStoPutStatus & );
	static inline void
			setFileNotify( bool x ) { sdFileNotifyOk = x; };
	static void	setIdentity( char *, cStoPutStatus & );
	static inline const char *
			getMediaType( void ) { return ( sdMediaType ); };
	static void	setMediaType ( char *, cStoPutStatus & );
	static inline int
			getNumCopies( void ) { return( sdNumCopies ); };
	static inline void
			setNumCopies( int x ) { sdNumCopies = x; };
	static inline const char *
			getOrganization( void ) { return ( sdOrganization ); };
	static const char *
			getUnitPrefix ( void ) { return sdUnitPrefix; };
	static void	setStoIdentity( char *, cStoPutStatus & );
	static void	setStoIpAddr ( char *, cStoPutStatus & );
	static void	setUnitPrefix( char *, cStoPutStatus & );
	static const char *
			getUserStream ( void ) { return sdUserStream; };
	static void	setUserStream ( char *, cStoPutStatus & );
	static void	archiveFileAvail( cStoPutStatus & );
	static void	userMediaRequest( cStoPutStatus & );
	

    protected:

    private:
	static boolean		defaultConfig( int, char *[] );
	static boolean		identityConfig( int, char *[] );
	static boolean		organizationConfig( int, char *[] );
	static boolean		storageConfig( int, char *[] );

	static char	*sdArchiveStream;
					// Archive stream to put to.
	static char 	*sdCompression; // Compression to applied to files.
	static char	*sdIdentity;	// Name for this server.
	static char	*sdMediaType;	// Media type files are written to.
	static int	sdNumCopies;	// Number of copies to write.
	static char	*sdOrganization; // Organization creating the media.
	static bool	sdFileNotifyOk;	// File notification was okay.
	static char	*sdUnitPrefix;	// Media unit name prefix.
	static char	*sdStoServer;	// Name for the Storage server.
	static char	*sdStoIpAddr;	// Ip Address of the Storage server.
	static cDhsConnection
			sdStoConnect;	// Storage Server connection.
	static bool	sdSimulate;	// True if in simulate mode.
	static char	*sdUserStream;	// User data stream to put to.
	cDhs		*sdPDhs;	// The dhs library object.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cPutFile
//
// PUBLIC METHODS: 
// (void) stoPutFile - Constructor
// none.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// none.
//
// PURPOSE:
// Handles the response from a storage server put file request.
//
//-
//***********************************************************************
//

class cPutFile: public cDhsPutDataset
{
    public:
	int j;
	    cPutFile( cStoPutStatus & , char *, int );
    protected:
    private:
	void done( DHS_CMD_STATUS, char * string, cDhsAvList & );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoPutDhsError
//
// PUBLIC METHODS:
// (void) error - Handle errors.
//
// PURPOSE:
// An object of this type is created and executed each time an error
// occurs in the DHS library. This class defines the error methods
// originally declared as pure virtual in the cDhsError base class.
//
//-
//***********************************************************************
//
 
class   cStoPutDhsError: public cDhsError
{
    private:
	void            error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoPutDhsCmd
//
// PUBLIC METHODS:
// (none) stoPutDhsCmd 	- Constructor for the command.
//
// PURPOSE:
// An object of this type is created and executed each time an
// stoArchiveFileAvailable or stoUserMediaRequest command is sent.
//
//-
//***********************************************************************
//
 
class   cStoPutDhsCmd: public cDhsClientCommand
{
    public:
	cStoPutDhsCmd( char * name ) : cDhsClientCommand( name ) { };
    private:
	void	done( DHS_CMD_STATUS, char *, const cDhsAvList & );
};
#endif // STOPUTDHS_H
