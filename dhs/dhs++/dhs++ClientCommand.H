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
// dhs++/h/dhs++ClientCommand.H
//
// PURPOSE:
// Whatever
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
// Revision 1.12  1998/04/02 19:36:52  nhill
// Moved method cDhsClientCommand::responseGet to class cDhsTag in
// file dhs++Client.H
//
// Revision 1.11  1997/12/01 21:39:09  cockayne
// Modified declaration of connect() method.
//
// Revision 1.10  1997/08/26 21:11:49  nhill
// Move the cDhsClientCommand::cmdDone, cDhsClientCommand::status,
// cDhsClientCommand::tag, and cDhsClientCommand::wait functions to
// the cDhsTag base class.
//
// Revision 1.9  1997/08/19 19:59:24  nhill
// Added function cDhsClientCommand::status.
//
// Revision 1.8  1997/07/23 19:44:36  nhill
// Added a function to get a specific response from a client command.
//
// Revision 1.7  1997/07/10 19:00:43  nhill
// Removed the mutex and added a pointer to an existance flag.
//
// Revision 1.6  1997/05/01 23:10:18  nhill
// Added a mutex to the cDhsClientCommand class.
// Moved teh apply method to the C file.
//
// Revision 1.5  1997/04/15 20:09:27  nhill
// Added a method to abort a command and some new accessor functions.
//
// Revision 1.4  1997/03/25 00:02:46  nhill
// Checkin for install.
//
// Revision 1.3  1997/03/06 19:16:30  nhill
// Cleaned up the header file dependancies.
//
// Revision 1.2  1997/02/28 00:54:14  nhill
// Checkin for install.
//
// Revision 1.1  1997/02/21 21:46:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSPPCLIENTCOMMAND_H
#define	DHSPPCLIENTCOMMAND_H

class	cDhsServerCommand;

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsClientCommand
//
// PUBLIC METHODS: 
// (None) cDhsClientCommand - Constructor.
// (None) ~cDhsClientCommand - Destructor.
// (void) apply - Send the command to the server.
// (void) cmdAbort - Abort the command.
// (cDhsConnection) connect - Return the connection the command went to.
// (cDhsAvList) 
//
// PURPOSE:
// This is the base class for client command classes. Applications
// should create classes derived from this type which define the
// behaviour of the done and response methods.
//
//-
//***********************************************************************
//

class	cDhsClientCommand: public cDhsAvList, public cDhsTag
{
    friend class cDhs;

    public:
			cDhsClientCommand( char * );
			cDhsClientCommand( const cDhsServerCommand & );
	virtual 	~cDhsClientCommand( void );
	void		apply( const cDhsConnection &, DHS_STATUS & );
	void		cmdAbort( DHS_STATUS & );
	inline const cDhsConnection
			&connect( void );
    protected:
    private:
	virtual void	done( DHS_CMD_STATUS, char *, const cDhsAvList & ){};
	virtual void	response( DHS_CMD_STATUS, char *, 
				const cDhsAvList & ){};

	bool		*dccPExists;	// Pointer to a flag to signal the
					// destruction of this object.
	const cDhsConnection		// Pointer to the connection.
			*dccPConnect;
	char		*dccCommand;	// The command name.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientCommand::connect
//
// INVOCATION:
// connection = command.connect();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cDhsConnection &) Returns the connection a command was sent to.
//
// PURPOSE:
// Accessor function to get a commands connection
//
// DESCRIPTION:
// Trivial.
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

const cDhsConnection	&cDhsClientCommand::connect
(
    void
)
{
    return( *dccPConnect );
}
#endif // DHSPPCLIENTCOMMAND_H
