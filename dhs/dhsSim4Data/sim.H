//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
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
// <filename>
//
// PURPOSE:
// Whatever
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:37  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:56:20  nhill
// Removed methods cSimDhs::identity and cSimDhs::imp.
//
// Revision 1.1  1997/10/03 23:08:38  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef SIM_H
#define SIM_H

#include "dhs++.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cSim
//
// PUBLIC METHODS: 
// (type) method - description
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// (type) data - description
//
// PURPOSE:
// Checking out this C++ thang
// 
//-
//***********************************************************************
//

class 		cSim
{
    public:
	//
	//  Methods.
	//

	inline		cSim( void );
    	static void	config( cSimStatus & );

	void		exit( cSimStatus & );
	void		init( cSimStatus & );
	void		start( cSimStatus & );

	static inline char *
			myDataServer( void ) { return( sdDataServer ); }
	static inline char *
			myDataServIP( void ) { return( sdDataServIP ); }
	static inline bool
			notify( void ) { return( sdNotify ); };
	static inline bool
			notify( bool );

	//
	//  Data.
	//

    protected:
	//
	//  Methods.
	//

	//
	//  Data.
	//
	
    private:
	//
	//  Methods.
	//

	static boolean	dataServer( int, char ** );

	//
	//  Data.
	//
	
	static char	*sdDataServer;	// Name of the dataServer.
	static char	*sdDataServIP;	// IP address of the dataServer.
	cDhs		*sdPDhs;	// The dhs library object.
	static bool	sdNotify;	// True if we're notifying. 
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// name
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) paramName (int) Descripton of the parameter
// (!) paramName (float) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cSim::cSim
(
    void
)
{
    sdPDhs = NULL;
}

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
 
bool            cSim::notify
(
    bool        value           // (in)  New value for the notify member.
)
{
    bool        oldNotify;

    oldNotify = sdNotify;
    sdNotify = value;
    return( oldNotify );
}


#endif // SIM_H

