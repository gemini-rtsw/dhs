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
// dhs++/h/dhs++GetDataset.H
//
// PURPOSE:
// Contains the the cDhsGetDataset class.
//
// CLASSES:
// <class> - Description
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.6  1997/12/01 21:40:23  cockayne
// Modified to suit Rational Rose analyzer.
//
// Revision 1.5  1997/05/01 23:12:25  nhill
// Moved the get method to a C file.
//
// Revision 1.4  1997/04/24 16:56:39  nhill
// Added "const" keywords to function prototypes.
//
// Revision 1.3  1997/02/28 00:54:14  nhill
// Checkin for install.
//
// Revision 1.2  1997/02/24 19:23:10  nhill
// checkin for install.
//
// Revision 1.1  1997/02/21 21:46:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSPPGETDATASET_H
#define	DHSPPGETDATASET_H

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsGetDataset
//
// PUBLIC METHODS: 
// (void) get - Get the dataset from the server.
// (DHS_BD_GET_TYPE) type - Return the format of the data.
//
// PURPOSE:
// This is the base class used by an application to create a class
// representing a request for data.
//
//-
//***********************************************************************
//

class	cDhsGetDataset: public cDhsClientDataset
{
    friend class cDhs;

    public:
	void		get( const cDhsConnection &, const char *, 
				DHS_BD_GET_TYPE, DHS_STATUS & );
	inline DHS_BD_GET_TYPE
			type( void );
    protected:
    private:
	DHS_BD_GET_TYPE	dgdType;	// Type of data received.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetDatast::type
//
// INVOCATION:
// type = gds.type();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_BD_GET_TYPE) The format of the data to be sent.
//
// PURPOSE:
// Accessor function to return the format of the data to be sent
//
// DESCRIPTION:
// n.a.
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

DHS_BD_GET_TYPE	cDhsGetDataset::type
(
    void
)
{
    return dgdType;
}
#endif // DHSPPGETDATASET_H
