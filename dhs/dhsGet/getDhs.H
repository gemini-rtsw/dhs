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
// dhsGet/h/getDhs.H
//
// PURPOSE:
// Header file for the initialization class getDhs.
//
// CLASSES:
// getDhs	- Class to initialize dhsGet
// getDataset	- Empty class, used to do the get request.
//
//
//INDENT-OFF*
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GETDHS_H
#define	GETDHS_H

#include "genMutex.H"
#include "dhs++.H"


//
//***********************************************************************
//+
// CLASS NAME:
// getDhs
//
// PUBLIC METHODS: 
// (void) getDhs - constructor.
// (void) ~getDhs - deconstructor.
// (void) exit - Shutdown nicely.
// (void) init - Initialize the dhs stuff.
// (void) kill - Force an exit.
// (void) start	- Start the event loop.
// (void) setType - Set datatype.
// (cDhsConnection) getConnect - Return the connection to the dataServer.
// (void) setIdentity - Set the id of this program
// (void) setDestin - Set the destination of the files.
// (void) setDsIdentity - set the dataServer identity.
// (void) setIpAddr - Set the ipaddress of the dataServer.
// (boolean) variablesSet - Return true if mandatory variables are set.
// (boolean) doNext - A continue flag is returned.

//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// (type) data - description
//
// PURPOSE:
// Takes care of the DHS++ library setup and variables setup.
//
//-
//***********************************************************************
//

class	getDhs
{
    public:
	inline		getDhs( void );
			~getDhs( void );
	static inline void
			setType ( DHS_BD_GET_TYPE x )		\
			{					\
				if ( sdDataType == DHS_BD_GT_UNKNOWN )       \
					sdDataType = x;		\
			};
	static inline DHS_BD_GET_TYPE
			getType ( void )			\
			{					\
				return( sdDataType ); 		\
			};
	static inline cDhsConnection
			&  getConnect ( void ) 			\
			{ 					\
				return( sdDataConnect ); 	\
			};
	static inline void
			setIdentity ( char *x ) 		\
			{					\
				if ( sdIdentity == NULL )	\
					sdIdentity = x;		\
			};
	static inline void
			setDestin ( char *x )
			{					\
				sdDestinDir = x;		\
			};
	static inline char *
			getDestin ( void )
			{					\
				return( sdDestinDir ); 		\
			};
	static inline void
			setDsIdentity( char *x )
			{					\
				sdDataServer= x; 		\
			};
	static inline void
			setIpAddr ( char *x )
			{ 					\
				sdDsIpAddr = x; 		\
			};
	static inline boolean 
			variablesSet( void )
			{		 			\
			if ( sdDataServer == NULL || sdDsIpAddr == NULL ) \
				return( FALSE );		\
			else					\
				return( TRUE );			\
			};
	static inline bool 
			doNext ( void )
			{					\
				return sdContinue; 		\
			};

	void		exit( cGetStatus & );
	void		init( cGetStatus & );
	void		kill( void );
	void		start( cGetStatus & );
    protected:
    private:
	static char	*sdIdentity;	// Name for this server.
	static char	*sdDataServer;	// Name for the data server.
	static char	*sdDsIpAddr;	// Ip Address of the data server.
	static char	*sdDestinDir;	// Destination directory.
	static cDhsConnection
			sdDataConnect;	// Data Server connection.
	static DHS_BD_GET_TYPE
			sdDataType;	// Get data type.
	cDhs		*sdPDhs;	// The dhs library object.
	static bool	sdContinue;	// True if you are to continue.
	static bool	sdSimulate;	// True if in simulate mode.

};

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhs::getDhs
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// Constructor for the getDhs class
//
// DESCRIPTION:
// Initialize the data members.
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

		getDhs::getDhs
(
    void
)
{
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// CLASS NAME:
// getDataset
//
// PUBLIC METHODS: 
// none.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// none.
//
// PURPOSE:
// Handles the response from a get request and write the data if possible
//
//-
//***********************************************************************
//

class getDataset: public cDhsGetDataset
{
    public:
	cMutex		sdMutex;	// Makes gets sync.
    protected:
    private:
	void done( DHS_CMD_STATUS, char * string, cDhsAvList & );
	void datasetWrite( cGetStatus & );

};

#endif // GETDHS_H
