//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998
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
// qldp/h/localQldp.H
//
// PURPOSE:
// This file contains the general interface to the Quick Look data processing
// classes.
//
// CLASSES:
// cQldpStatus - Status class for the Quick Look data processing classes.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1999/01/20 19:33:28  nhill
// Added message E_OPERAND_SIZE.
//
// Revision 1.1  1998/11/13 23:37:27  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef LOCALQLDP_H
#define	LOCALQLDP_H

#include "dhsGenStatus.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cQldpStatus
//
// PUBLIC METHODS: 
// None.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This is the status class for the Quick Look data processing routines
//
//-
//***********************************************************************
//

class cQldpStatus: public cDhsStatus
{
    msgPrefix( "qldp" );

    public:
	msgId( E_ARRAY_OP );
	msgId( E_CONST_OP );
	msgId( E_DPAPPLY_USAGE );
	msgId( E_DPCLEAR_USAGE );
	msgId( E_DPSET_USAGE );
	msgId( E_FOPEN );
	msgId( E_MMAP );
	msgId( E_OPERAND_SIZE );
    protected:
    private:
};
#endif // LOCALQLDP_H
