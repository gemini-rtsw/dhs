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
// dhsQlServer/h/sequenceCommands.H
//
// PURPOSE:
// This class contains the interface to the routines which perform the
// gemini sequence commands.
//
// CLASSES:
// cQlsSequenceCmd	- An object of this type is create to handle a 
//			  sequence command command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/15 23:15:10  nhill
// Changed to use command handlers with method specific callbacks.
//
// Revision 1.1  1997/10/22 17:49:54  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef SEQUENCECOMMANDS_H
#define	SEQUENCECOMMANDS_H


//
//***********************************************************************
//+
// CLASS NAME:
// cQlsSequenceCmd
//
// PUBLIC METHODS: 
// (void) debugLevel - Called to start the processing of the debugLevel command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing a sequence command.
//
//-
//***********************************************************************
//

class cQlsSequenceCmd: public cDhsServerCommand
{
    public:
			cQlsSequenceCmd();
	void		debugLevel( void );
	void		exit( void );
	void		init( void );
	void		resetHealth( void );
	void		simulateLevel( void );
	void		test( void );
	bool		cmdAbort( void );
    protected:
    private:
	bool		qscAbort;
};
#endif // SEQUENCECOMMANDS_H
