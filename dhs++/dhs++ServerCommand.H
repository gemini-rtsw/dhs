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
// dhs++/h/dhs++ServerCommand.H
//
// PURPOSE:
// Header for the server command classes.
//
// CLASSES:
// cDhsServerCommand - Object created for each individual command received
//		by a server.
// cDhsCmdHandler - An object of this class is created to handle classes of
//		commands received by a server.
// cDhsCmdHandlerBase - This class groups data and methods common to all
//		instances of the cDhsCmdHandler class.
// cDhsAbortCmd - This is a class derived from the cDhsServerCommand class,
//		and it handles abort commands.
//
// METHODS:
// cDhsServerCommand::connect - Accessor function to return the connection
//		to the client.
// cDhsServerCommand::deref - Decrement the reference counter for a command
//		object.
// cDhsServerCommand::name - Return the command name of a command.
// cDhsServerCommand::response - Send a response for a command.
// cDhsCmdHandler::cDhsCmdHandler - Constructor for the class.
// cDhsCmdHandler::newCmd - Create a new cDhsServerCommand derived object 
//		of the appropriate type.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.11  1998/04/23 22:55:22  nhill
// Updated the documentation.
// Added the option to have a specified callback function when setting up
// a command handler.
//
// Revision 1.10  1998/03/25 17:50:16  nhill
// Moved method deref from the serverCommand.C file.
//
// Revision 1.9  1998/03/13 21:38:13  nhill
// Removed the thread count from the server put command handler.
//
// Revision 1.8  1997/07/10 19:02:55  nhill
// Removed the setting of the command state to busy.  The locking this
// was intended to support deadlocked in some cases.
//
// Revision 1.7  1997/05/01 23:22:58  nhill
// Changed to ensure the command state goes to DHS_CS_BUSY before it goes to
// one of the completion states.
//
// Revision 1.6  1997/04/24 16:56:39  nhill
// Added "const" keywords to function prototypes.
//
// Revision 1.5  1997/04/15 21:12:28  nhill
// Added a list of all server commands.
// Added classes and code to allow an abort command.
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

#ifndef DHSPPSERVERCOMMAND_H
#define	DHSPPSERVERCOMMAND_H

//
//  this is to allow forward references.
//

class	cDhsCmdHandlerBase;

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsServerCommand
//
// PUBLIC METHODS: 
// (bool) cmdAbort - Executed to abort the processing of a command.
// (cDhsServerCommand *) find - Find the command object associated with
//		a command tag.
// (DHS_CONNECT) connect - Return the connection associated with the command.
// (void) deref - Decrement the reference counter associated with the command.
// (char *) name - Return the command name.
// (void) response - Send a response to the client.
//
// PURPOSE:
// Base class for user specified server command objects
//
// SEE ALSO:
// cDhsCmdHandler
//
//-
//***********************************************************************
//

class cDhsServerCommand: public cDhsAvList
{
    friend class cDhsCmdHandlerBase;

    public:
	virtual bool	cmdAbort( void );
	cDhsServerCommand
			*commandFind( DHS_TAG );
	inline DHS_CONNECT
			connect( void );
	inline void	deref( void );
	inline const char	
			*name( void ) const;
	inline void	response( DHS_CMD_STATUS, const char *, 
				const cDhsAvList &, DHS_STATUS & );
	inline void	response( DHS_CMD_STATUS, const cDhsAvList &, 
				DHS_STATUS & );
	inline void	response( DHS_CMD_STATUS, const char *, DHS_STATUS & );
	inline void	response( DHS_CMD_STATUS, DHS_STATUS & );
        virtual void	exec( void ); // XXX allan: changed from protected
    protected:
			cDhsServerCommand( void );
	virtual		~cDhsServerCommand( void );

	cDhsCmdHandlerBase		// Handler that created the command.
			*dscPHandler;
    private:
	void		init( DHS_CONNECT, DHS_TAG, char *, DHS_AV_LIST, 
				cDhsCmdHandlerBase& );

	DHS_CONNECT	dscConnect;	// Connection for the command.
	char		*dscName;	// Name of the command.
	static void	*dscPCommandList;// Pointer to a list of all commands.
	int		dscRefCount;	// Reference counter.
	DHS_TAG		dscTag;		// Client command tag for the command.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::connect
//
// INVOCATION:
// connection = command.connect();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_CONNECT) The connection to the client system.
//
// PURPOSE:
// Accessor function to return the client connection
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

DHS_CONNECT	cDhsServerCommand::connect
(
    void
)
{
    return( dscConnect );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::deref
//
// INVOCATION:
// command.deref()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Decrecment the reference count and delete the dataset when it reaches 0
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

void		cDhsServerCommand::deref
(
    void
)
{
    dscRefCount--;
    if ( dscRefCount == 0 )
    {
	delete this;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::name
//
// INVOCATION:
// name = cmd.name();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (char *) The name of the command.
//
// PURPOSE:
// Accessor function to return the name of the command
//
// DESCRIPTION:
// n.a.
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

const char	*cDhsServerCommand::name
(
    void
) const
{
    return( dscName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::response
//
// INVOCATION:
// obj.response( cmdStatus, string, avList, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CMD_STATUS) New status for the command.
// (>) message (char *) Status message for the command.
// (>) avList (const cDhsAvList &) Response attribute value list.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response to a command
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

void		cDhsServerCommand::response
(
    DHS_CMD_STATUS		// (in)  New status for the command.
		cmdStatus,
    const char 	*message, 	// (in)  Status message for the command.
    const cDhsAvList		// (in)  Response attribute value list.
		&avList,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsCmdResponse( dscConnect, dscTag, cmdStatus, message, avList.dalAvList, 
	    &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::response
//
// INVOCATION:
// obj.response( cmdStatus, avList, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CMD_STATUS) New status for the command.
// (>) avList (const cDhsAvlist &) Response attribute value list.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response to a command (without a message)
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

void		cDhsServerCommand::response
(
    DHS_CMD_STATUS		// (in)  New status for the command.
		cmdStatus,
    const cDhsAvList		// (in)  Response attribute value list.
		&avList,
    DHS_STATUS	&status		// (mod) Function return status.
) 
{
    response( cmdStatus, NULL, avList, status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::response
//
// INVOCATION:
// obj.response( cmdStatus, string, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CMD_STATUS) New status for the command.
// (>) message (char *) Status message for the command.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response to a command (without an attribute value list)
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

void		cDhsServerCommand::response
(
    DHS_CMD_STATUS		// (in)  New status for the command.
		cmdStatus,
    const char	*message, 	// (in)  Status message for the command.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    response( cmdStatus, message, DHS_AV_LIST_NULL, status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::response
//
// INVOCATION:
// obj.response( cmdStatus, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CMD_STATUS) New status for the command.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response to a command (without an attribute value list)
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

void		cDhsServerCommand::response
(
    DHS_CMD_STATUS		// (in)  New status for the command.
		cmdStatus,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    response( cmdStatus, NULL, DHS_AV_LIST_NULL, status );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsCmdHandlerBase
//
// PUBLIC METHODS: 
// (none) ~cDhsCmdHandlerBase - Destructor for the class.
//
// PURPOSE:
// This class only exists to allow data and methods common to all instances
// of the cDhsCmdHandler template class to be moved a shared class
//
// This should prevent these shared resources from being created for each
// instance of the template class.
//
// SEE ALSO:
// - class cDhsCmdHandler
// - class cDhsServerCommand
//
//-
//***********************************************************************
//

class cDhsCmdHandlerBase: public cDhsHandler
{
    public:
	virtual		~cDhsCmdHandlerBase( void );
    protected:
	typedef	void	*tBaseExecFn;
	
			cDhsCmdHandlerBase( bool, const char * );
	virtual cDhsServerCommand
			*newCmd( void ) = 0;
    private:
	static void	cmdCallback( DHS_CONNECT, DHS_TAG, char *, 
				DHS_AV_LIST );
	static void	*execThread( void * );
	virtual void	exec( cDhsServerCommand * ) = 0;

	static void	*dchPRwLock;	// Read/write lock to protect the
					// list of handlers.
	static bool	dchInit;	// Is the callback initialized?
	char		*dchCommand;	// Command name.
	static DHS_CB_FN_PTR		// Pointer to the old callback function.
			dchOldCallback;
	bool		dchThreaded;	// Execute each command in a thread?
	static void	*dchPHandlerList;	// List of all handlers.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsCmdHandler
//
// PUBLIC METHODS: 
// (None) cDhsCmdHandler - Constructor.
//
// PURPOSE:
// Created to handle commands received by a server.
//
// SEE ALSO
// - class cDhsCmdHandlerBase
// - class cDhsServerCommand
//
//-
//***********************************************************************
//

template <class userCmd>
class cDhsCmdHandler : public cDhsCmdHandlerBase
{
    public:
	typedef		void (userCmd::*tExecFn) ( void );

	inline	    	cDhsCmdHandler( bool = false, const char * = NULL );
	inline	    	cDhsCmdHandler( bool, const char *, tExecFn  );
    protected:
    private:
	cDhsServerCommand
			*newCmd( void );
	void		exec( cDhsServerCommand *pCmdObj )
			{
			    ( ( (userCmd *) pCmdObj )->*dchExecFn)();
			}


	tExecFn		dchExecFn;	// Function to execute for the command.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsAbortCmd
//
// PUBLIC METHODS: 
// (type) method - description
//
// PURPOSE:
// Handle abort commands.
//
//-
//***********************************************************************
//

class 	cDhsAbortCmd: public cDhsServerCommand
{
    public:
    protected:
    private:
	void		exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandler::cDhsCmdHandler
//
// INVOCATION:
// cDhsCmdHandler	obj( false );
// cDhsCmdHandler	obj( false, "command name" );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Handle each command in a new thread?
// (>) name (char *) Command name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDhsCmdHandler class
//
// DESCRIPTION:
// Set up the DHS library command callback function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// A cDhs object must have been created.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

template <class userCmd>
		cDhsCmdHandler<userCmd>::cDhsCmdHandler
(
    bool	threaded,	// (in)  Handle each command in a new thread?
    const char	*name		// (in)  Command name.
) : cDhsCmdHandlerBase( threaded, name )
{
    dchExecFn = &cDhsServerCommand::exec;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandler::cDhsCmdHandler
//
// INVOCATION:
// cDhsCmdHandler	obj( false );
// cDhsCmdHandler	obj( false, "command name" );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Handle each command in a new thread?
// (>) name (char *) Command name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDhsCmdHandler class
//
// DESCRIPTION:
// Set up the DHS library command callback function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// A cDhs object must have been created.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

template <class userCmd>
		cDhsCmdHandler<userCmd>::cDhsCmdHandler
(
    bool	threaded,	// (in)  Handle each command in a new thread?
    const char	*name,		// (in)  Command name.
    tExecFn	fn		// (in)  Function to handle the command.
) : cDhsCmdHandlerBase( threaded, name )
{
    dchExecFn = fn;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandler::newCmd
//
// INVOCATION:
// pCmdObj = cmdHandler.newCmd();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cDhsServerCommand *) Pointer to the new object.
//
// PURPOSE:
// Create a new command object when a new command is received
//
// DESCRIPTION:
// Allocate space for the object.
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

template <class userCmd>
cDhsServerCommand	*cDhsCmdHandler<userCmd>::newCmd
(
    void
)
{
    userCmd	*pCmd;		// Pointer to the new command object.


    pCmd = new userCmd;
    if ( pCmd == NULL )
    {
	throw "memory allocation error";
    }

    return( pCmd );
}
#endif // DHSPPSERVERCOMMAND_H
