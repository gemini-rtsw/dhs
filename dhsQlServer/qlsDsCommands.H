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
// dhsQlServer/h/qlsDsCommands.H
//
// PURPOSE:
// Contains the interface to command object for handling commands related
// to datasets.
//
// CLASSES:
// cQlsDsAbortCmd	- An object of this type is create to handle a 
//			  qlsDsAbort command.
// cQlsDsGet - Takes care of the processing of a qlsDsGet command.
// cQlsPutChunk		- An object of this type is create to handle a chunk 
//			  received by the QLS.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1997/12/10 21:58:21  nhill
// Made the cQlsDsGet::config method non inline.
//
// Revision 1.1  1997/10/22 17:50:04  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSDSCOMMANDS_H
#define	QLSDSCOMMANDS_H

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsDsAbortCmd
//
// PUBLIC METHODS: 
// (void) exec - Called to start the processing of the qlsDsAbort command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing a qlsDsAbort command
//
//-
//***********************************************************************
//

class	cQlsDsAbortCmd: public cDhsServerCommand
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
// cQlsDsGet
//
// PUBLIC METHODS: 
// (void) exec - Called to start the processing of the qlsDsGet command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the processing of a qlsDsGet command
//
//-
//***********************************************************************
//

class 	cQlsDsGet: public cDhsServerCommand
{
    public:
	inline		cQlsDsGet( void );
	static void	config( cQlsStatus & );
	static boolean	dataServerId( int, char ** );
	void		exec( void );
    protected:
    private:
	static char	*qdgDataServerAddr;
					// Address of the data server.
	cDhsConnection	qdgDataServerConnect;
					// Connection to the data server.
	static char	*qdgDataServerName;	
					// Name of the data server.
	cDhsGetDataset	qdgGet;		// Client get request.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDsGet::cQlsDsGet
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cQlsDsGet class
//
// DESCRIPTION:
// Description of the algorithm.
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

		cQlsDsGet::cQlsDsGet
(
    void
) : qdgDataServerConnect( qdgDataServerAddr, qdgDataServerName )
{
}

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsPutChunk
//
// PUBLIC METHODS: 
// (void) exec - This function is executed to start the processig of the 
//		request
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles chunks being received by the dhs Quick Look Server.
//
//-
//***********************************************************************
//

class 	cQlsPutChunk: public cDhsPutRequest
{
    public:
	void		exec( void );
    protected:
    private:
};
#endif // QLSDSCOMMANDS_H
