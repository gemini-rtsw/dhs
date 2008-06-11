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
// dhsStorage/h/stoDhs.H
//
// PURPOSE:
// Contains the interface to the cStoDhs class
//
// CLASSES:
// cStoDhs - Takes care of the dhs++ library setup.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  1999/10/12 23:21:13  nhill
// Removed the unused getHealth data member.
//
// Revision 1.7  1999/06/16 23:48:08  nhill
// Fixed a typto in the STO_ERD_* strings.
//
// Revision 1.6  1998/06/24 16:05:56  cockayne
// Added readyToReceiveCommands().
//
// Revision 1.5  1998/04/23 16:15:50  cockayne
// Modified to use new status and health.
//
// Revision 1.4  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STODHS_H
#define	STODHS_H

#include "globals.H"
#include "stoEpicsRecord.H"


//
// Define Epics Records for Storage Server. 
//

#define STO_ERD_STATE		"state"
#define STO_ERD_SIMULATE	"simulate"
#define STO_ERD_DEBUG		"debug"


//
// Define state strings.
//

#define STO_STATE_RUNNING       "RUNNING"
#define STO_STATE_TESTING       "TESTING"
#define STO_STATE_SHUTDOWN      "SHUTTINGDOWN"
#define STO_STATE_INITIALIZE    "INITIALIZING"


//
// Define health strings. 
//

#define STO_HEALTH_GOOD         "GOOD"
#define STO_HEALTH_WARNING      "WARNING"
#define STO_HEALTH_BAD          "BAD"


//
// Define health levels.
//

#define	STO_HL_GOOD		2
#define	STO_HL_WARNING		1
#define	STO_HL_BAD		0


//
// Define debug strings.
//
 
#define STO_DEBUG_LEVEL_NONE    "NONE"
#define STO_DEBUG_LEVEL_MIN     "MIN"
#define STO_DEBUG_LEVEL_FULL    "FULL"


//
// Define simulate strings.
//

#define STO_SIM_LEVEL_NONE      "NONE"
#define STO_SIM_LEVEL_VSM       "VSM"
#define STO_SIM_LEVEL_FAST      "FAST"
#define STO_SIM_LEVEL_FULL      "FULL"



//
//***********************************************************************
//+
// CLASS NAME:
// cStoDhs
//
// PUBLIC METHODS: 
// (cStoDhs)	cStoDhs		- Constructor.
// ()		~cStoDhs	- Destructor.
// (void)	config		- Init's configurable data from config file.
// (void)	exit		- Stop the event loop.
// (void)	init		- Initailize the dhs library.
// (void)	start		- Start dhs event loop processing.
// (bool)	retry		- Set and/or return the hdRetry member.
// (bool)	simulate	- Set and/or return the hdSimulate member.
// (void)	start		- Start the dhs event loop.
// (bool)	readyToReceiveCommands- Check ready to receive commands.
// (void)	setState	- Sets the run-state of Storage Server.
// (void)	setDebug	- Sets the debug level of Storage Server.
// (void)	setSimulate	- Sets the simulation level. 
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the DHS++ library interactions.
//
//-
//***********************************************************************
//

class	cStoDhs
{
    public:
	//
	// Constructors, destructors.
	//
	
			cStoDhs( void );
	inline		~cStoDhs( void )
			{
			    if ( sdPDhs != NULL )
			    {
				delete (cDhs *) sdPDhs;
			    }
			}

	
	//
	// Static member functions. 
	//
	
	inline static bool
			readyToReceiveCommands( bool b ) 
			{ 
			    return( sdReadyToReceiveCommands = b ); 
			};
	inline static bool
			readyToReceiveCommands( void ) 
			{ 
			    return( sdReadyToReceiveCommands ); 
			};
	inline static bool	
			retry( void ) 
			{ 
			    return( sdRetry ); 
			};
	inline static bool	
			retry( bool v ) 
			{ 
			    return( sdRetry = v ); 
			};
	inline static bool
			simulate( void ) 
			{ 
			    return( sdSimulateFlag ); 
			};
	inline static bool
			simulate( bool value )
			{
			    bool	oldSimulate;

			    oldSimulate = sdSimulateFlag;
			    sdSimulateFlag = value;
			    return( oldSimulate );
			};
	inline static void
			setState( char * stateStr )
			{
			    if ( sdState.value() != NULL )
			    {
				gen_free( sdState.value() );
			    }
			    sdState = strsav( stateStr );
			};
	inline static void
			setDebug( char * debugStr )
			{
			    if ( sdDebug.value() != NULL )
			    {
				gen_free( sdDebug.value() );
			    }
			    sdDebug = strsav( debugStr );
			};
	inline static void
			setSimulate( char * simulateStr )
			{
			    if ( sdSimulate.value() != NULL )
			    {
				gen_free( sdSimulate.value() );
			    }
			    sdSimulate = strsav( simulateStr );
			};


	
	//
	// Non-static member functions. 
	//
	
	void		exit( cStoStatus & );
	void		init( cStoStatus & );
	void		start( cStoStatus & );


    protected:
    private:

	static cStoEpicsRecord< char *, cStoDhs, cStoStatus >
			sdState;	// Status Server's state.
	static cStoEpicsRecord< char *, cStoDhs, cStoStatus >
			sdSimulate;	// Status Server's simulate level.
	static cStoEpicsRecord< char *, cStoDhs, cStoStatus >
			sdDebug;	// Status Server's debug level.

	static char	*sdDSIdentity;	// Data Server identity.
	static char	*sdDSAddress;	// Data Server address.
	static bool	sdReadyToReceiveCommands;
					// True when ready to process commands. 
	static bool	sdRetry;	// True if rerun.
	static bool	sdSimulateFlag;	// True if in simulate mode.

	cDhs		*sdPDhs;	// The dhs library object.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoDhsError
//
// PUBLIC METHODS:
// (void) error - Called to start the processing of an error message.
//
// PURPOSE:
// An object of this type is created and executed each time an error
// occurs in the DHS library. This class defines the error methods
// originally declared as pure virtual in the cDhsError base class.
//
//-
//***********************************************************************
//
 
class		cStoDhsError : public cDhsError
{
     private:
	 void            error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoConnect
//
// PUBLIC METHODS:
// (void) error - Called to start the processing of a connection message.
//
// PURPOSE:
// Object to deal with DHS library connection requests.
//
//-
//***********************************************************************
//
 
class   cStoConnect: public cDhsError
{
    friend class cStoDhs;

     private:
	 void            error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoDisconnect
//
// PUBLIC METHODS:
// (void) error - Called to start processing an disconnect message.
//
// PURPOSE:
// Object to deal with DHS library disconnect requests.
//
//-
//***********************************************************************
//
 
class   cStoDisconnect: public cDhsError
{
    private:
        void            error( void );
};
#endif // STODHS_H
