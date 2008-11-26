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
// dhsQlServer/h/qlsDhs.H
//
// PURPOSE:
// Contains the interface to the cQlsDhs class
//
// CLASSES:
// cQlsDhs - Takes care of the dhs++ library setup.
//
// METHODS:
// cQlsDhs:cQlsDhs - Constructor for the cQlsDhs class.
// cQlsDhs:simulate - Set or get the simulation mode for the cQlsDhs class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/15 23:13:58  nhill
// Moved the configuration methods to the dhs++ library.
// Updated the documentation.
//
// Revision 1.2  1997/08/20 21:19:48  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSDHS_H
#define	QLSDHS_H

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsDhs
//
// PUBLIC METHODS: 
//		cQlsDhs		- Constructor for the class.
//		~cQlsDhs	- Destructor for the class.
// (static void)config		- Gets DHS library config info from the 
//				  configuration file.
// (void)	exit		- Stop the event loop.
// (static char *)		- Identity return the IMP identity read
//		identity	  from the configuration file.
// (void)	init		- Initailize the dhs library.
// (static bool)retry		- Set and/or return the sdRetry member.
// (static bool)simulation	- Set and/or return the sdSimulate member.
// (void)	start		- Start the dhs event loop.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the DHS++ library setup.
//
//-
//***********************************************************************
//

class	cQlsDhs
{
    public:
	inline		cQlsDhs( void );
	inline		~cQlsDhs( void )
			{
			    if ( sdPDhs != NULL )
			    {
				delete (cDhs *) sdPDhs;
			    }
			}
	void		exit( cQlsStatus & );
	void		init( cQlsStatus & );
	inline static bool	
			retry( void ) { return( sdRetry ); };
	inline static bool	
			retry( bool v ) { return( sdRetry = v ); };
	inline static bool
			simulate( void ) { return( sdSimulate ); };
	inline static bool
			simulate( bool );
	void		start( cQlsStatus & );
    protected:
    private:
	cDhs		*sdPDhs;	// The dhs library object.
	static bool	sdRetry;	// True if rerun.
	static bool	sdSimulate;	// True if in simulate mode.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDhs::cQlsDhs
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cQlsDhs class
//
// DESCRIPTION:
// Trivial
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

		cQlsDhs::cQlsDhs
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
// cQlsDhs::simulate
//
// INVOCATION:
// cQlsDhs::simulate( true );
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

bool		cQlsDhs::simulate
(
    bool	value		// (in)  New value for the simulate member.
)
{
    bool	oldSimulate;

    oldSimulate = sdSimulate;
    sdSimulate = value;
    return( oldSimulate );
}
#endif // QLSDHS_H
