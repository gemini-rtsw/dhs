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
// dhsHistory/h/hisFits.H
//
// PURPOSE:
// Header file for the initialization class cHisFits.
//
// CLASSES:
// cHisFits - Used to write the fits file. 
//
//
//INDENT-OFF*
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef HISF_H
#define	HISF_H

#include "dhs++.H"
#include "fitsio.h"


#define checkFits( fn, s, ret )						\
    {									\
	int		fStat;						\
	char		fStr[31];					\
	if ( ( fStat = fn ) != 0 )					\
	{								\
	    ffgerr( fStat, &fStr[0] );					\
	    s.E_CFITSIO( s, fStat, #fn, fStr );				\
	    ret;							\
	}								\
    }



//
//***********************************************************************
//+
// CLASS NAME:
// cHisFits
//
// PUBLIC METHODS: 
// (void)	createFits	- Create a fits file. 
// (void) 	fitsAddEvents	- Add events to a fits file. 
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

class cHisFits 
{
    public:
	static void	createFits( char *, char *, char *, cHisStatus & );
	static void	fitsAddEvents( int, fitsfile *, cHisStatus & );
    protected:
    private:
	static int 	hfTFields;
	static char	*hfTType[];
	static char	*hfTForm[];
	static char	*hfTUnit[];
	static char	*hfFilepath;
    
};
#endif // HISF_H
