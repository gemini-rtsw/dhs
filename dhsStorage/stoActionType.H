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
// dhsStorage/h/stoActionType.H
//
// PURPOSE:
// Contains the definitions and declarations for the DHS 
// Storage Server action-type class.
//
// CLASSES:
// cStoActionType- Types of actions of the Storage Server. 
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

#ifndef STOACTIONTYPE_H
#define	STOACTIONTYPE_H

#include <iostream.h>


//
// Define action strings.
//

#define STO_AS_RETRIEVE "RETRIEVE"
#define STO_AS_QUEUE    "QUEUE"
#define STO_AS_PREPARE  "PREPARE"
#define STO_AS_WRITE    "WRITE"
#define STO_AS_CLEANUP  "CLEANUP"
#define STO_AS_NOTIFY   "NOTIFY"

#define ACTION_STRING_MAX_LEN	10

//
// Define enumerated types.
//

typedef enum sto_action
{
    STO_AT_NONE,
    STO_AT_RETRIEVE,
    STO_AT_QUEUE,
    STO_AT_PREPARE,
    STO_AT_WRITE,
    STO_AT_CLEANUP,
    STO_AT_NOTIFY
} STO_ACTION_TYPE;



//
//***********************************************************************
//+
// CLASS NAME:
// cStoActionType
//
// PUBLIC METHODS: 
// (cStoActionType)	cStoActionType	- Constructor (2 different types).
// (cStoActionType &)	initializer	- Initialize a cStoActionType object.
// (ostream &)		operator<<	- Output method.
// (void)		setAction	- Set the action type.
// (STO_ACTION_TYPE)	getAction	- Get the action type
// (char *)		getActionTypeString- Get the action type string.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Action types are stored using this class. 
//
// The action type is slightly complicated, containing both an action type 
// enum and the string the type represents. This class serves the purpose.
//-
//***********************************************************************
//

class	cStoActionType
{
    public:
	//
	// Constructors, Deconstructors.
	//

	inline		cStoActionType( void )
			{
			    stAction = STO_AT_NONE;
			};
	inline		cStoActionType( STO_ACTION_TYPE action )
			{
			    stAction = action;
			    switch( stAction )
			    {
				case STO_AT_NONE:
				    (void) sprintf( stActionString, 
					    "%s", "" );
				    break;
				case STO_AT_RETRIEVE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_RETRIEVE );
				    break;
				case STO_AT_QUEUE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_QUEUE );
				    break;
				case STO_AT_PREPARE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_PREPARE );
				    break;
				case STO_AT_WRITE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_WRITE );
				    break;
				case STO_AT_CLEANUP:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_CLEANUP );
				    break;
				case STO_AT_NOTIFY:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_NOTIFY );
				    break;
			    }
			};
	inline static cStoActionType
			&initializer( STO_ACTION_TYPE action )
			{
			    static cStoActionType a;
			    a.setAction( action );
			    return( a );
			};


	//
	// Non-static methods.
	//

	inline friend ostream
			&operator<<( ostream &os, cStoActionType &at )
			{
			    os << at.getActionTypeString();
			    return( os );
			};
	inline void	setAction( STO_ACTION_TYPE action )
			{
			    stAction = action;
			    switch( stAction )
			    {
				case STO_AT_NONE:
				    (void) sprintf( stActionString, 
					    "%s", "" );
				    break;
				case STO_AT_RETRIEVE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_RETRIEVE );
				    break;
				case STO_AT_QUEUE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_QUEUE );
				    break;
				case STO_AT_PREPARE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_PREPARE );
				    break;
				case STO_AT_WRITE:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_WRITE );
				    break;
				case STO_AT_CLEANUP:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_CLEANUP );
				    break;
				case STO_AT_NOTIFY:
				    (void) sprintf( stActionString, 
					    "%s", STO_AS_NOTIFY );
				    break;
			    }
			};
	inline STO_ACTION_TYPE
			getAction( void )
			{
			    return( stAction );
			};
	inline char	*getActionTypeString( void )
			{
			    return( stActionString );
			};


    protected:
    private:
	//
	// Private Data.
	//
    

	STO_ACTION_TYPE	stAction;	// Which action is this? 
	char		stActionString[ACTION_STRING_MAX_LEN];
					// The string of the action.
};
#endif // STOACTIONTYPE_H
