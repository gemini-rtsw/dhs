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
// dhsPut/h/putDhs.H
//
// PURPOSE:
// Header file for the initialization class putDhs.
//
// CLASSES:
// putDhs	- Class to initialize dhsPut
// putDataset	- Empty class, used to do the put request.
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

#ifndef PUTDHS_H
#define	PUTDHS_H

#include "dhs++.H"


//
//***********************************************************************
//+
// CLASS NAME:
// putDhs
//
// PUBLIC METHODS: 
// (void) putDhs - constructor.
// (void) ~putDhs - deconstructor.
// (void) buffZero - Set the sdBuffZero flag.
// (void) buffZero - Return the sdBuffZero flag.
// (void) exit - Shutdown nicely.
// (void) init - Initialize the dhs stuff.
// (void) kill - Force an exit.
// (void) start - Start the event loop.
// (void) setType - Set datatype.
// (cDhsConnection) putConnect - Return the connection to the dataServer.
// (void) setIdentity - Set the id of this program
// (void) setDestin - Set the destination of the files.
// (void) setDsIdentity - set the dataServer identity.
// (void) setIpAddr - Set the ipaddress of the dataServer.
// (void) setDSName - Set the datasetName.
// (void) setNoLast - Set the sdNoLast flag to true.
// (char *) getDSName - Return datasetName.
// (boolean) variablesSet - Return true if mandatory variables are set.
// (boolean) doNext - A continue flag is returned.
//
// PURPOSE:
// Takes care of the DHS++ library setup and variables setup.
//
//-
//***********************************************************************
//

class	putDhs
{
    public:
	inline		putDhs( void );
			~putDhs( void );
	void		exit( cPutStatus & );
	void		init( cPutStatus & );
	void		kill( void );
	void		start( cPutStatus & );
	void		firstCmds( cPutStatus & );
	void		lifeCmd( cPutStatus &, int );
	static inline bool
			buffZero ( void )
			{				\
			    return( sdBuffZero );	\
			};
	static inline void
			buffZero ( bool x )
			{				\
			    sdBuffZero = x;		\
			};
	static inline void
			setType ( DHS_BD_PUT_TYPE x ) {		\
				if ( sdDataType == DHS_BD_PT_UNKNOWN )       \
					sdDataType = x;		\
				};
	static inline DHS_BD_PUT_TYPE
			getType ( void ) { return( sdDataType ); };
	static inline cDhsConnection
			&  getConnect ( void ) { return( sdDataConnect ); };
	static inline void
			setIdentity ( char *x ) {		\
				if ( sdIdentity == NULL )	\
					sdIdentity = x;		\
				};
	static inline void
			setDsIdentity( char *x ) { sdDataServer= x; };
	static inline void
			setDSName ( char *x ) { sdDSName = x; };
	static inline char
			*getDSName ( void ) { return ( sdDSName ); };
	static inline void
			setNewLabel( void ) { sdNewLabel = TRUE; };
	static inline void
			setNoLast( void ) { sdNoLast = TRUE; };
	static inline bool
			getNoLast( void ) { return( sdNoLast ); };
	static inline void
			setLife ( char x ) { sdLifetime = x; };
	static inline char
			getLife ( void ) { return ( sdLifetime ); };
	static inline void
			setIpAddr ( char *x ) { sdDsIpAddr = x; };
	static inline boolean 
			variablesSet( void ) { 			\
			if ( sdDataServer == NULL || sdDsIpAddr == NULL || \
				( sdNewLabel == TRUE && sdDSName != NULL ) ) \
				return( FALSE );		\
			else					\
				return( TRUE );			\
			}
	static inline bool 
			doNext ( void ) { return sdContinue; };
    protected:
    private:
	static char	*sdIdentity;	// Name for this server.
	static char	*sdDSName;	// Dataset name to be used for all.
	static char	*sdDataServer;	// Name for the data server.
	static char	*sdDsIpAddr;	// Ip Address of the data server.
	static char	sdLifetime;	// Lifetime of the dataset.
	static cDhsConnection
			sdDataConnect;	// Data Server connection.
	static DHS_BD_PUT_TYPE
			sdDataType;	// Put data type.
	cDhs		*sdPDhs;	// The dhs library object.
	static bool	sdBuffZero;	// True if last put is buff zero.
	static bool	sdNewLabel;	// True if new label is required.
	static bool	sdNoLast;	// True if never to send a last flag.
	static bool	sdRetry;	// True if rerun.
	static bool	sdSimulate;	// True if in simulate mode.
	static bool	sdContinue;	// True if continue to next ds.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhs::putDhs
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
// Constructor for the putDhs class
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

		putDhs::putDhs
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
// putDataset
//
// PUBLIC METHODS: 
// (void) putDataset - Constructor
// none.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// none.
//
// PURPOSE:
// Handles the response from a put request.
//
//-
//***********************************************************************
//

class putDataset: public cDhsPutDataset
{
    public:
	int j;
	    putDataset( cPutStatus & , 
			char *, void **,
			int &,
			DHS_BD_PUT_TYPE, bool );
    protected:
    private:
	void done( DHS_CMD_STATUS, char * string, cDhsAvList & );
};

//
//***********************************************************************
//+
// CLASS NAME:
// putDhsError
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
 
class   putDhsError: public cDhsError
{
    private:
	void            error( void );
};

#endif // PUTDHS_H
