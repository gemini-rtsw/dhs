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
// dhsData/h/dtsDhs.H
//
// PURPOSE:
// Header file for the initialization class cDtsDhs.
//
// CLASSES:
// cDtsDhsName - Source name class.
// cDtsDhs - Main class
// cDtsDhsConnect - Connection class.
// cDtsDhsError - Error class.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2000/07/27 20:10:51  dunn
// Added standalone features.
//
// Revision 1.7  1998/06/16 16:06:55  dunn
// Changed VOID's to void.
//
// Revision 1.6  1998/05/04 22:52:05  dunn
// Changed cDtsDhsError class to not include cDtsDhsName info,
// Added storing number of incomplete datasets.
// dAdded ready flag to indicate when dataserver is ready.
//
// Revision 1.5  1997/10/01 16:23:34  dunn
// Code cleanup.
//
// Revision 1.4  1997/09/17 22:24:31  dunn
// Added cleanupTemp, ssServerNotify, olServerNotify.
//
// Revision 1.3  1997/08/29 23:07:50  dunn
// Added comments, simulateString, closeLibs, kill
//
// Revision 1.2  1997/08/19 22:32:48  dunn
// Bug fixes.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSDHS_H
#define	DTSDHS_H

#include "dhs++.H"

//
//  Defines
//

#define	checkSf( fn, sfStatus, status, ret )				\
    {									\
	fn;								\
	if ( sfStatus != SF_S_SUCCESS )					\
	{								\
	    status.E_SF( status, sfMsg );				\
	    ret;							\
	}								\
    }


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsName
//
// PUBLIC METHODS: 
// cDtsDhsName - Constructor.
// ~cDtsDhsName - Deconstructor.
// (void) sourceAddr - Saves source address.
// (char *) sourceAddr - Returns source address.
// (void) sourceName - Saves source name.
// (char *) sourceName - Returns source name.
//
// PURPOSE:
// This is meant to be an inherited class for all classes that get callbacks.
// It gets the connection information and is able to pass on both the source
// name and source IP address to the instance of the thread.
//
//-
//***********************************************************************
//
class 	cDtsDhsName
{
    public:
	inline		cDtsDhsName ( void )
			    {
				cdnSourceName = NULL;
				cdnSourceAddr = NULL ;

			    };
	inline		~cDtsDhsName ( void )
			    {
				free( cdnSourceName );
				free( cdnSourceAddr );
			    };
	inline void	sourceAddr( char * address )
			    {
				if( cdnSourceAddr != NULL )
				{
				    free( cdnSourceAddr );
				}
				if ( address == NULL )
				{
				    cdnSourceAddr =  strsav( "unknown" );
				}
				else
				{
				    cdnSourceAddr =  strsav( address );
				}
			    };
	inline char	*sourceAddr( void )
			    {
				return ( cdnSourceAddr );
			    };
	inline void	sourceName( char * clientName )
			    {
				if( cdnSourceName != NULL )
				{
				    free( cdnSourceName );
				}
				if ( clientName == NULL )
				{
				    cdnSourceName =  strsav( "unknown" );
				}
				else
				{
				    cdnSourceName =  strsav( clientName );
				}
			    };
	inline char	*sourceName( void )
			    {
				return ( cdnSourceName );
			    };

	cDhsAvList	cdnResponseAvList;	// Attribute value list for
						// the response.
    protected:
    private:
	char		*cdnSourceName;		// Name of the requestor.
	char		*cdnSourceAddr;		// Address of the requestor.
						// the response.

};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhs
//
// PUBLIC METHODS: 
// cDtsDhs 		- Constructor.
// cDtsDhs 		- Deconstructor.
// compressExtension	- Returns compression extension.
// compressType		- Returns the comrpession type.
// numIncomp		- Returns the number of incomplete ds in a string.
// numIncomp		- Decrements or increments number of incomplete ds.
// oldpServer		- Returns the oldp server identity.
// qlServer		- Returns the quick look server identity.
// ready		- Ready to receive requests.
// retry		- Set the retry flag.
// retry		- Return the retry flag.
// segDir		- Returns the segregation directory.
// simulate		- Returns the simulate state.
// simulate		- Sets the simulate state.
// simulateString	- Sets the simulate string.
// simulateString	- Returns the simulate string.
// statusServer		- Returns the status server identity.
// storageServer	- Returns the storage server identity.
// cleanupTemp		- Cleanup the Temp area.
// closeLibs		- Close libraries.
// config		- Read dhs configuration file parameters.
// exit			- Get rid of dhs handlers and objects.
// init			- Setup handlers and init dhs library.
// initLibs		- Initialize libraries.
// initPress		- Initialize press library.
// kill			- Force and exit.
// makeList		- Make internal list based on files.
// olServerNotify	- Notify oldp server of outstanding notifications.
// recursiveLook	- Recursively go through files.
// sizeGet		- Get size of files.
// ssServerNotify	- Notify storage server of outstanding notifications.
// start		- Start the event loop.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// dtsStSrConnect	- Status server connection.
//
// PURPOSE:
// Takes care of the DHS++ library and any other general startup,shutdown.
//
//-
//***********************************************************************
//

class	cDtsDhs
{
    public:
			cDtsDhs( void );
			~cDtsDhs( void );
	static inline char *
			compressExtension( void ) 
			    {
				return( sdCompExt ); 
			    };
	static inline char *
			compressType( void ) 
			    { 
				return( sdCompType ); 
			    };
	static inline char *
			numIncomp( void ) 
			    { 
				sprintf( sdNumIncompStr, "%d", 
					sdNumIncompleteDs);
				return( sdNumIncompStr ); 
			    };
	static inline void 
			numIncomp( bool x, cDtsStatus &status ) 
			    { 
				if ( x )
				{
				    cDtsDhs::sdNumIncompleteDs++;
				}
				else
				{
				    if ( cDtsDhs::sdNumIncompleteDs > 0 )
				    {
					cDtsDhs::sdNumIncompleteDs--;
				    }
				}
				cStaStatus::update( "numIncomplete", 
					cDtsDhs::numIncomp(), status );
				cStaStatus::flush( status );
			    };
	static inline char *
			oldpServer( void ) 
			    { 
				return( sdOldpServer ); 
			    };
	static inline char *
			qlServer( void ) 
			    { 
				return( sdQlServer ); 
			    };
	static inline bool	
			ready( void ) 
			    { 
				return( sdReady ); 
			    };
	static inline void	
			ready( cDtsStatus &status, bool v ) 
			    { 
				if ( status.ok() )
				{
				    sdReady = v; 
				}
			    };
	static inline bool	
			retry( void ) 
			    { 
				return( sdRetry ); 
			    };
	static inline bool	
			retry( bool v ) 
			    { 
				return( sdRetry = v ); 
			    };
	static inline char *
			segDir( void ) 
			    { 
				return( sdSegDir ); 
			    };
	static inline bool
			simulate( void ) 
			    { 
				return( sdSimulate ); 
			    };
	static inline bool
			simulate( bool );
	static inline void
			simulateString( bool x )
			    { 
				sdSimulate = x;
			    };
	static inline char *
			simulateString( void ) 
			    { 
				if ( sdSimulate )
				    return( "FULL");
				else
				    return( "NONE");
			    };
	static inline char *
			statusServer( void ) 
			    {
				return( sdStatServer ); 
			    };
	static inline char *
			storageServer( void ) 
			    { 
				return( sdStorServer ); 
			    };

	void		cleanupTemp( cDtsStatus & );
	void		closeLibs( cDtsStatus & );
	static void	config( cDtsStatus & );
	void		exit( cDtsStatus & );
	void		init( cDtsStatus & );
	void		initLibs( cDtsStatus & );
	void		initPress( cDtsStatus & );
	void		kill( void );
	void		makeList( cDtsStatus & );
	void		recursiveLook( cDtsStatus &, char *, int );
	void		sizeGet( cDtsStatus &, char *, char *, int );
	static void	olServerNotify( cDtsStatus & );
	static void	ssServerNotify( cDtsStatus & );
	void		start( cDtsStatus & );

	static cDhsConnection
			dtsStSrConnect;	// Status Server connection.

    protected:
    private:
	static boolean	segregation( int, char **);
	static boolean	servers( int, char **);
	static boolean	compress( int, char ** );

	static char	*sdCompExt;	// Name for the compression extension.
	static char	*sdCompType;	// Name for the compression type.
//	static char	*sdIdentity;	// Name for this server.
	static int	sdNumIncompleteDs;
					// Num incomplete datasets.
	static char	sdNumIncompStr[5];
					// Num incomplete string.
	static char	*sdOldpServer;	// Name for the oldp server.
	cDhs		*sdPDhs;	// The dhs library object.
	static char	*sdQlServer;	// Name for the quick look server.
	static bool	sdReady;	// True indicates we are ready to recv.
	static bool	sdRetry;	// True if rerun.
	static char	*sdSegDir;	// Name for archive seg. dir.
	static bool	sdSimulate;	// True if in simulate mode.
	static char	*sdStatServer;	// Name for the status server.
	static char	*sdStorServer;	// Name for the storage server.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsConnect
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Object to deal with DHS library connection/disconnection requests.
//
//-
//***********************************************************************
//

class 	cDtsDhsConnect: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsError
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

class 	cDtsDhsError: public cDhsError
{
    public:
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::simulate
//
// INVOCATION:
// cDtsDhs::simulate( true );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) value (bool) New value for the simulate member.
//
// FUNCTION VALUE:
// (bool) The old value of the simulate member.
//
// PURPOSE:
// Set the new value of the simulate data member and return the old value
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cDtsDhs::simulate
(
    bool	value		// (in)  New value for the simulate member.
)
{
    bool	oldSimulate;

    oldSimulate = sdSimulate;
    sdSimulate = value;
    return( oldSimulate );
}

#endif // DTSDHS_H
