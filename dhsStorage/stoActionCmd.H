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
// dhsStorage/h/stoActionCmd.H
//
// PURPOSE:
// Contains the class which will accept Storage Server Action commands.
//
// CLASSES:
// cStoActionCmd - Accepts Stoarge Server action commands. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.5  1999/06/16 23:46:56  nhill
// Major re-organization.
//
// Revision 1.4  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOACTIONCMD_H
#define	STOACTIONCMD_H

#include "genMutex.H"
#include "genCond.H"
#include "globals.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cStoActionCmd
//
// PUBLIC METHODS: 
// (void)	execActionRequest - Execute an action request.
// (void)	execActionCancel - Execute an action cancel request.
// (void)	execRollbackRequest - Execute a rollback request.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Each incoming action command instantiates this class to handle the command.
//-
//***********************************************************************
//

class	cStoActionCmd: public cDhsServerCommand
{
    public:
	void		execActionRequest( void );
	void		execActionCancel( void );
	void		execRollbackRequest( void );

    private:
};
#endif // STOACTIONCMD_H
