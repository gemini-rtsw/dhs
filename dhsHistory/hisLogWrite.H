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
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "dhs++.H"

#include "globals.H"
#include "genCond.H"



//
//***********************************************************************
//+
// CLASS NAME:
// cHisLogWrite
//
// PUBLIC METHODS: 
// 		cHisLogWrite	- Constructor.
//		~cHisLogWrite	- Deconstructor.
// (void)	config		- Init's config'ed data from config file.
// (void)	HLWlock		- Lock - Only one hisLogWrite at a time. 
// (void)	HLWunlock 	- Unlock.
// (void) 	HLWRunning	- TRUE if hisLogWrite is being processed. 
// (void) 	setHLWRunning	- Set flag to indicate hisLogWrite is running. 
// (void)	exec		- Called to start processing of hisLogWrite.
// (void)	processCmd	- Called to process a hisLogWrite command.
// (void)	composePaths	- Compose paths to fits and raw files. 
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the init command
//
//-
//***********************************************************************
//

class cHisLogWrite: public cDhsServerCommand
{
    public:
	//
	// Constructors, Deconstructors.
	//

	inline		cHisLogWrite( void )
			{
			    hlwLabel = NULL;
			};
	inline		cHisLogWrite( char *l )
			{
			    if ( l != NULL )
			    {
				hlwLabel = strsav( l );
			    }
			};
	inline		~cHisLogWrite( void )
			{
			    if ( hlwLabel != NULL )
			    {
				free( hlwLabel );
				hlwLabel = NULL;
			    }
			};

	//
	// Static Methods.
	//
    
	static void	config( cHisStatus & );
	static inline void
			HLWlock( void )
			{
			    hlwRunCond.lock();
			};
	static inline void
			HLWunlock( void )
			{
			    hlwRunCond.unlock();
			};
	static inline void
			HLWbroadcast( void )
			{
			    hlwRunCond.broadcast();
			};
	static inline void
			HLWwait( void )
			{
			    hlwRunCond.wait();
			};
	static inline bool
			stoppingHLW( void )
			{
			    return( hlwStopping );
			};
	static inline bool
			HLWRunning( void )
			{
			    return( hlwRunning );
			};
	static inline void
			setHLWRunning( bool b )
			{
			    hlwRunning = b;
			};
	static inline void
			setHLWStopping( bool b )
			{
			    hlwStopping = b;
			};
	static void stopHLW( void );

	//
	// Non-static methods.
	//
	
	void		exec( void );
	void		processCmd( int, cHisStatus & );
	void		composePaths( cHisStatus & );

    protected:
    private:
	static boolean	dataServer( int, char ** );
	void		putFile( char *, char *, cHisStatus & );

	static bool	hlwRunning;		// Is a cmd being processed?
	static bool	hlwStopping;		// Is a cmd being stopped?
	static cCond 	hlwRunCond;		// Cond. variable for above.

	static cDhsConnection
	                hlwDataServer;		// Connection to dataServer.
	static char 	*hlwDSAddress;		// Address of dataServer.
	static char	*hlwDSIdentity;		// Identity of dataServer.

	char		*hlwLabel;		// Label from dataServer. 
	char		*hlwRawPath;		// Path to the raw file. 
	char		*hlwFitsPath;		// Path to the fits file. 
};
