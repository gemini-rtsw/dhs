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
// dhsQlServer/h/streamCommands.H
//
// PURPOSE:
// Contains the interface to the classes that handle things to do with 
// streams
//
// CLASSES:
// cQlsClientStreamCmd	- An object of this type is create to handle a 
//			  qlsClientStream command.
// cQlsDsStreamCmd	- An object of this type is create to handle a 
//			  qlsDsStream command.
// cQlsQueryStreamsCmd	- An object of this type is created to handle a 
//			  qlsQueryStreams command.
// cQlsServerStreamCmd	- An object of this type is created to handle a 
//			  qlsServerStream command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.1  1997/10/22 17:50:00  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STREAMCOMMANDS_H
#define	STREAMCOMMANDS_H

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsClientStreamCmd
//
// PUBLIC METHODS: 
// (void) exec - Called to start the processing of the clientStream command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the clientStream command
//
//-
//***********************************************************************
//

class cQlsClientStreamCmd: public cDhsServerCommand
{
    public:
	void		exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsDsStreamCmd
//
// PUBLIC METHODS: 
// (void) exec - Called to start the processing of the dsStream command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the dsStream command
//
//-
//***********************************************************************
//

class cQlsDsStreamCmd: public cDhsServerCommand
{
    public:
	void		exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsQueryStreamsCmd
//
// PUBLIC METHODS: 
// (void) exec - Start executing a qlsQueryStreams command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handle a query streams command
//
//-
//***********************************************************************
//

class cQlsQueryStreamsCmd: public cDhsServerCommand
{
    public:
	void		exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsServerStreamCmd
//
// PUBLIC METHODS: 
// (void) exec - Called to start the processing of the serverStream command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the serverStream command
//
//-
//***********************************************************************
//

class cQlsServerStreamCmd: public cDhsServerCommand
{
    public:
	void		exec( void );
    protected:
    private:
};
#endif // STREAMCOMMANDS_H
