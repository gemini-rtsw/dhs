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
// dhsHistory/h/hisPutFile.H
//
// PURPOSE:
// Header file for the initialization class cHisPutFile.
//
// CLASSES:
// cHisPutFile - Empty class, used to do the put request.
//
//
//INDENT-OFF*
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef HISREC_H
#define	HISREC_H

#include "dhs++.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cHisPutFile
//
// PUBLIC METHODS: 
// 		cHisPutFile	 - Constructor
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// none.
//
// PURPOSE:
// Handles the response from a put request.
//
//-
//***********************************************************************
//

class cHisRecovery
{
    public:
	static void	checkWorkingDir( char **, char **, bool &, bool &, 
				cHisStatus & );
	static void	checkRecovery( cHisStatus & );

    protected:
    private:
};
#endif // HISREC_H
