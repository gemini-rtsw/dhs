//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997.				(c) 1997
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
// dhs++/h/localDhs++.H
//
// PURPOSE:
// Contains the interface to the cErrHandlerKey class
//
// CLASSES:
// cErrHandlerKey - A key into a list of error handlers.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1997/12/01 21:41:38  cockayne
// Mofdified to suit Rational Rose analyzer.
//
// Revision 1.1  1997/09/22 23:54:16  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef LOCALDHSPP_H
#define	LOCALDHSPP_H


//
//***********************************************************************
//+
// CLASS NAME:
// cErrHandlerKey
//
// PUBLIC METHODS: 
// (type) method - description
//
// PURPOSE:
// Contains a key into the list of error handlers.
//
//-
//***********************************************************************
//

class	cErrHandlerKey
{
    public:
	inline		cErrHandlerKey( DHS_STATUS, DHS_ERR_LEVEL );
	inline		cErrHandlerKey( void ) {};
	bool 		operator<( const cErrHandlerKey & ) const;
    protected:
    private:
	DHS_STATUS	ehkErrStatus;
	DHS_ERR_LEVEL	ehkErrLevel;
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cErrHandlerKey::cErrHandlerKey
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) errStatus (DHS_STATUS) Error status for the key.
// (>) errLevel (DHS_ERR_LEVEL) Error level for the key.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the key used to locate error handlers in the error
// handler list
//
// DESCRIPTION:
// Set the error status and error level members.
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

		cErrHandlerKey::cErrHandlerKey
(
    DHS_STATUS	errStatus,	// (in)  Error status for the key.
    DHS_ERR_LEVEL		// (in)  Error level for the key.
		errLevel
)
{
    ehkErrStatus = errStatus;
    ehkErrLevel = errLevel;
}
#endif // LOCALDHSPP_H


