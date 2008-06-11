//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>                           (c) <year>
// National Research Council            Conseil national de recherches
// Ottawa, Canada, K1A 0R6              Ottawa, Canada, K1A 0R6
// All rights reserved                  Tous droits reserves
//
// NRC disclaims any warranties,        Le CNRC denie toute garantie
// expressed, implied, or statu-        enoncee, implicite ou legale,
// tory, of any kind with respect       de quelque nature que se soit,
// to the software, including           concernant le logiciel, y com-
// without limitation any war-          pris sans restriction toute
// ranty of merchantability or          garantie de valeur marchande
// fitness for a particular pur-        ou de pertinence pour un usage
// pose.  NRC shall not be liable       particulier.  Le CNRC ne
// in any event for any damages,        pourra en aucun cas etre tenu
// whether direct or indirect,          responsable de tout dommage,
// special or general, consequen-       direct ou indirect, particul-
// tial or incidental, arising          ier ou general, accessoire ou
// from the use of the software.        fortuit, resultant de l'utili-
//                                      sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// simPutFits.H
//
// PURPOSE:
// Contains class definition for fits put.
//
// MEHTOD NAME(S)
// None.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:37  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.1  1997/10/03 23:09:23  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
 
#ifndef SIMPUTFITS_H
#define SIMPUTFITS_H
 
#include "dhs++.H"
 
 

//
//***********************************************************************
//+
// CLASS NAME:
// cSimPutFits
//
// PUBLIC METHODS:
// None.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles file puts of type fits.
// 
//
//-
//***********************************************************************
//
 
class		cSimPutFits: public cDhsPutRequest
{
    private:
	//
	//  Methods.
	//
 
	void            exec( void );
};
 
#endif // SIMPUTFITS_H
