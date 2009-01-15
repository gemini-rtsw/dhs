
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
// dhsStorage/h/stoRefreshCmd.H
//
// PURPOSE:
// Contains code to handle a Storage server Refresh command.
//
// CLASSES:
// cStoRefreshCmd - Takes care of the "stoRefresh" command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1  1999/02/24 23:58:33  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOREFRESHCMD_H
#define	STOREFRESHCMD_H

#include "globals.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cStoRefreshCmd
//
// PUBLIC METHODS:
// (void)	exec	- Called to start processing of sequence command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles the "stoRefresh" command received by the Storage Server.
//
//-
//***********************************************************************
//
 
class   cStoRefreshCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
	void		execDefault( void );
	void		execRefresh( void );
};
#endif // STOREFRESHCMD_H
