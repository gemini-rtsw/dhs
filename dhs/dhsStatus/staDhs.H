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
// dhsStatus/h/staDhs.H
//
// PURPOSE:
// Contains the interface to the cStaDhs class.
//
// CLASSES:
// cStaDhs - Interface to the DHS++ library.
//
// METHODS:
// (cStaDhs::cStaDhs) Constructor for the cStaDhs class.
// (cStaDhs::simulate) Get or set the simulate mode for the cStaDhs library.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/06/19 18:10:12  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.3  1998/04/15 22:23:44  nhill
// Updated the documentation.
// Moved the configuration methods to the dhs++ library.
//
// Revision 1.2  1998/02/17 00:23:57  jaeger
// Add methods and a bool value which is set to TRUE if EPICS is initialized.
//
// Revision 1.1  1997/03/27 22:16:14  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STADHS_H
#define	STADHS_H

#include "dhs++.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cStaDhs
//
// PUBLIC METHODS: 
// (type) method - description
// cStaDhs - Constructor for the class.
// ~cStaDhs - Destructor for the class.
// (bool) isInit - Get or set the sdIsInit flag.
// (bool) retry - Get or set the sdRetry flag.
// (bool) simulate - Get or set the sdSimulate flag.
// (void) start - Start the DHS event loop.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the DHS++ library.
//
//-
//***********************************************************************
//

class	cStaDhs
{
    public:
	inline		cStaDhs( void );
			~cStaDhs( void );
	static inline bool
			isInit( void ) { return( sdIsInit ); };
	static inline bool
			isInit( bool v ) { return( sdIsInit = v ); };
	static inline bool	
			retry( void ) { return( sdRetry ); };
	static inline bool	
			retry( bool v ) { return( sdRetry = v ); };
	static inline bool
			simulate( void ) { return( sdSimulate ); };
	static inline bool
			simulate( bool );
	void		start( cStaStat & );
    protected:
    private:
	static bool	sdIsInit;	// True if EPICS is initialized.
	cDhs		*sdPDhs;	// The dhs library object.
	static bool	sdRetry;	// True if rerun.
	static bool	sdSimulate;	// True if in simulate mode.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaDhs::cStaDhs
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
// Constructor for the cStaDhs class
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

		cStaDhs::cStaDhs
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
// cStaDhs::simulate
//
// INVOCATION:
// cStaDhs::simulate( true );
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

bool		cStaDhs::simulate
(
    bool	value		// (in)  New value for the simulate member.
)
{
    bool	oldSimulate;

    oldSimulate = sdSimulate;
    sdSimulate = value;
    return( oldSimulate );
}
#endif // STADHS_H
