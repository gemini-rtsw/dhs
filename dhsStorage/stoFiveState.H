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
// dhsStorage/h/stoFiveState.H
//
// PURPOSE:
// Storage Server processes may take on any of five states. This file
// contains class definitions to support this principle. 
//
// CLASSES:
// cStoFiveState - Class to store a proccess's state. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
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

#ifndef STOFIVESTATE_H
#define	STOFIVESTATE_H

#include <iostream.h>
#include "globals.H"


//
// Define five-state strings for display through EPICS. 
//

#define	STO_FSS_UNKNOWN			"UNKNOWN"
#define	STO_FSS_IDLE			"IDLE"
#define	STO_FSS_REQUESTED		"REQUESTED"
#define	STO_FSS_IN_PROGRESS		"IN-PROGRESS"
#define	STO_FSS_ROLLBACK_REQUESTED	"ROLLBACK REQUESTED"
#define	STO_FSS_ROLLBACK_IN_PROGRESS	"ROLLBACK IN-PROGRESS"


//
// Define enumerated type - The possible states of a five-state variable.
//

typedef enum
{
    STO_FST_UNKNOWN,
    STO_FST_IDLE,
    STO_FST_REQUESTED,
    STO_FST_IN_PROGRESS,
    STO_FST_ROLLBACK_REQUESTED,
    STO_FST_ROLLBACK_IN_PROGRESS
} STO_FIVE_STATE_TYPE;



//
//***********************************************************************
//+
// CLASS NAME:
// cStoFiveState
//
// PUBLIC METHODS:
// (cStoFiveState)	cStoFiveState	- Constructor (two flavours).
// (void)		state2str	- Convert a state to a string.
// (cStoFiveState &)	idle		- Return an idle-state object.
// (ostream &)		operator<<	- Output operator.
// (bool)		isEqual		- Check equality of two objects.
// (STO_FIVE_STATE_TYPE) getState	- Return the state of an object.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Used to store the 'state' of the action processes Storage Server performs.
//
//-
//***********************************************************************
//
 
class cStoFiveState
{
    public:
			cStoFiveState( void );
	inline		cStoFiveState( STO_FIVE_STATE_TYPE fs )
			{
			    sfsState = fs;
			};

	void		state2str( char * );
	inline static cStoFiveState 
			&idle( void )
			{
			    static cStoFiveState fiveStateUnknown;
			    return( fiveStateUnknown );
			};
	inline friend ostream 
			&operator<<( ostream &os, cStoFiveState &fs )
			{
			    char	buffer[40];
			    fs.state2str( &buffer[0] );
			    os << buffer;
			    return( os );
			};
	static inline bool	
			isEqual( cStoFiveState &fs1, cStoFiveState &fs2 )
			{
			    if ( fs1.sfsState == fs2.sfsState )
			    {
				return( true );
			    }
			    return( false );
			};
	inline STO_FIVE_STATE_TYPE
			getState( void )
			{
			    return( sfsState );
			};

    protected:
    private:
	//
	// Private Data.
	//

	STO_FIVE_STATE_TYPE	sfsState;	// The state. 
};
#endif // STOFIVESTATE_H
