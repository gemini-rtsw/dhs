static char rcsid[] = "$Id: serverCommand.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhs++/src/serverCommand.C
//
// PURPOSE:
// Contains the implementation for the cDhsAbortCmd, cDhsCmdHandlerBase,
// and cDhsServerCommand classes.
//
// METHOD NAME(S)
// cDhsAbortCmd::exec - This method is executed when an abort command
//		is received.
// cDhsCmdHandlerBase::cDhsCmdHandlerBase - Constructor for the class.
// cDhsCmdHandlerBase::~cDhsCmdHandlerBase - Destructor for the class.
// cDhsCmdHandlerBase::cmdCallback - Executed when a command is received.
// cDhsCmdHandlerBase::execThread - Start the processing of a command
//		in its own thread.
// cDhsServerCommand::cDhsServerCommand - constructor for the class.
// cDhsServerCommand::~cDhsServerCommand - Destructor for the class.
// cDhsServerCommand::cmdAbort - Executed when a command is aborted.
// cDhsServerCommand::commandFind - Find a command given a command tag.
// cDhsServerCommand::exec - Default exec function for a command.
// cDhsServerCommand::init - Initialize a command before it is processed.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:25:05  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.13  1998/04/23 23:27:35  nhill
// Updated the documentation.
// Changed the way commands are executed in separate threads.
//
// Revision 1.12  1998/03/25 18:40:00  nhill
// Added some documentation and a call to deref.
//
// Revision 1.11  1998/03/25 17:49:34  nhill
// Moved function deref to the header.
//
// Revision 1.10  1998/03/13 21:40:29  nhill
// Fixed some problems that could occur when a handler was removed while
// a command was being processed.
//
// Revision 1.9  1998/03/05 23:46:22  nhill
// Added checking to ensure the handler is enabled before handing a
// command.
//
// Revision 1.8  1998/02/24 01:23:19  nhill
// Bug fixes.
//
// Revision 1.7  1997/10/02 20:52:25  nhill
// Added locks to protect the command list.
//
// Revision 1.6  1997/08/19 19:56:40  nhill
// Fixed some locking bugs.
//
// Revision 1.5  1997/07/10 19:06:09  nhill
// *** empty log message ***
//
// Revision 1.4  1997/05/01 23:25:23  nhill
// Added initialization of teh dscLastResponse data member.
//
// Revision 1.3  1997/04/15 21:10:04  nhill
// Added code and classes to handle abort commands.
// Added a reference counter to the server command.
// Added a list of all server commands.
//
// Revision 1.2  1997/03/25 00:02:02  nhill
// Checkin for install.
//
// Revision 1.1  1997/02/21 21:45:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <string>
#include <map>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
}

#define	DHS_NO_CLIENT
#define	DHS_NO_BULK

#include "dhs++.H"
#include "genRwLock.H"


typedef std::map< DHS_TAG, cDhsServerCommand *, std::less<DHS_TAG> >
		tCommandList;
typedef tCommandList::iterator
		iCommandList;

#define	dscCommandList (*(tCommandList *) cDhsServerCommand::dscPCommandList)


typedef std::map< std::string, cDhsCmdHandlerBase *, std::less<std::string> >
		tHandlerList;
typedef tHandlerList::iterator
		iHandlerList;

#define	dchHandlerList (*(tHandlerList *) dchPHandlerList)

bool		cDhsCmdHandlerBase::dchInit = false;
DHS_CB_FN_PTR	cDhsCmdHandlerBase::dchOldCallback = NULL;
void		*cDhsCmdHandlerBase::dchPHandlerList = NULL;
void		*cDhsServerCommand::dscPCommandList = NULL;

static cRwLock	rwLock;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAbortCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the abort command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute the abort command
//
// DESCRIPTION:
// - Extract the tag from the attribute value list.
// - Find the command associated with the tag.
// - Execute the command'ss abort function.
// - Send a response back to the aborting function.
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

void		cDhsAbortCmd::exec
(
    void
)
{
    cDhsAttrib 	attrib;		// The level attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cDhsServerCommand		// The command to abort.
		*pServerCommand;
    DHS_TAG	*pTag;		// Pointer to the command tag.


    //
    // Get the tag from the attribute value list.
    //

    attrib = find( "tag", dhsStatus );
    attrib.info( (void **) &pTag, dhsStatus );
    

    //
    // Find the command associated with the tag.
    //

    pServerCommand = commandFind( *pTag );

    if ( pServerCommand != NULL )
    {
	//
	// Execute the abort function
	//

	if ( pServerCommand->cmdAbort() )
	{
	    response( DHS_CS_DONE, dhsStatus );
	}
	else
	{
	    response( DHS_CS_ERROR, "Failed to abort command", dhsStatus );
	}
	pServerCommand->deref();
    }
    else
    {
	response( DHS_CS_ERROR, "Could not find command.", dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandlerBase::cDhsCmdHandlerBase
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Create a new thread for each command?
// (>) name (char *) Name of the command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDhsCmdHandlerBase class
//
// DESCRIPTION:
// Initialize the data members, and if necessary set up the callback function.
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

		cDhsCmdHandlerBase::cDhsCmdHandlerBase
(
    bool	threaded,	// (in)  Create a new thread for each command?
    const char	*name		// (in)  Name of the command.
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    dchThreaded = threaded;


    //
    //  Save the command name.
    //
    
    if ( name == NULL )
    {
	dchCommand = strsav( "" );
    }
    else
    {
	dchCommand = strsav( name );
    }


    //
    //  If necessary, set up the command callback function.
    //

    if ( ! dchInit )
    {
	dchOldCallback = dhsCallbackSet( DHS_CBT_COMMAND, 
		(DHS_CB_FN_PTR) cmdCallback, &status );
	dchInit = true;
    }


    //
    //  Add the handler to the list of handlers.
    //


    rwLock.wLock();
    if ( dchPHandlerList == NULL )
    {
	dchPHandlerList = new tHandlerList;
    }
    dchHandlerList[ std::string( dchCommand ) ] = this;
    rwLock.unlock();


    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandlerBase::~cDhsCmdHandlerBase
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
// Destructor for the cDhsCmdHandlerBase class
//
// DESCRIPTION:
// Frees memory allocated to the structure and removes it from the list of
// handlers.
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

		cDhsCmdHandlerBase::~cDhsCmdHandlerBase
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );
    iCommandList	
		i;


    rwLock.wLock();
    dchHandlerList.erase( std::string( dchCommand ) );


    //
    //  If there are no more command handlers in existance, reset the callback
    //  function.
    //

    if ( dchHandlerList.size() == 0 )
    {
	dhsCallbackSet( DHS_CBT_COMMAND, dchOldCallback, &status );
	dchInit = false;
    }


    rwLock.unlock();

    free( dchCommand );

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandlerBase::cmdCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_COMMAND, cmdCallback, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection associated with the command.
// (>) tag (DHS_TAG) The tag associated with the command.
// (>) command (char *) The command string.
// (>) avList (DHS_AV_LIST) The attribute value list for the command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This is assigned as the command callback function for the dhs library
//
// DESCRIPTION:
// Execute the "exec" function in a thread or otherwise.
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

void		cDhsCmdHandlerBase::cmdCallback
(
    DHS_CONNECT	connect, 	// (in)  Connection associated with the command.
    DHS_TAG	tag, 		// (in)  The tag associated with the command.
    char	*command,	// (in)  The command string.
    DHS_AV_LIST	avList		// (in)  The attribute value list.
)
{
    DHS_AV_LIST	avListCopy;	// A copy of the command attribute value list.
    iHandlerList i;		// Iterator for the handler list.
    cDhsServerCommand
		*pCmdObj;
    cDhsCmdHandlerBase		// Pointer to the appropriate handler.
		*pHandler;
    DHS_STATUS	status;		// DHS library return status.
    bool	threaded;	// Is the hander threaded?

    
    rwLock.wLock();
    if ( ( & dchHandlerList ) == NULL )
    {
	dhsCmdResponse( connect, tag, DHS_CS_ERROR, 
		"This server cannot handle commands.", 
		DHS_AV_LIST_NULL, &status );
	rwLock.unlock();
	return;
    }
    else
    {
	if ( ( i = dchHandlerList.find( std::string( command ) ) ) == 
		dchHandlerList.end() )
	{
	    if ( ( i = dchHandlerList.find( std::string( "" ) ) ) == 
		    dchHandlerList.end() )
	    {
		dhsCmdResponse( connect, tag, DHS_CS_ERROR, 
			"This server cannot handle commands.", 
			DHS_AV_LIST_NULL, &status );
		rwLock.unlock();
		return;
	    }
	}
    }

    pHandler = (*i).second;

    pCmdObj = pHandler->newCmd();


    pCmdObj->init( connect, tag, command, avList, *pHandler );


    if ( ! pHandler->enabled() )
    {
	status = DHS_S_SUCCESS;
	pCmdObj->response( DHS_CS_ERROR, "Command handler is disabled", 
		status );
	rwLock.unlock();
	pCmdObj->deref();
	return;
    }


    threaded = pHandler->dchThreaded;
    rwLock.unlock();


    if ( threaded )
    {
	status = DHS_S_SUCCESS;
	avListCopy = dhsAvListCopy( pCmdObj->dalAvList, &status );
	pCmdObj->dalAvList = avListCopy;
	pCmdObj->dalFreeIt = true;
	pCmdObj->dscName = strsav( pCmdObj->dscName );

	if ( status != DHS_S_SUCCESS )
	{
	    dhsMsgCallback( pCmdObj->dscConnect, pCmdObj->dscTag );
	    status = DHS_S_SUCCESS;
	    pCmdObj->response( DHS_CS_ERROR, "Failed to copy av list", 
		    status );
	}
	else
	{
	    threadCreate( execThread, pCmdObj );
	}
    }
    else
    {
	pHandler->exec( pCmdObj );

	pCmdObj->deref();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsCmdHandlerBase::execThread
//
// INVOCATION:
// threadCreate( execThread, pCmdObj );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pCmdObj (cDhsServerCommand *) Pointer to a server command
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Used to start command processing in a thread
//
// DESCRIPTION:
// Execute the commands "exec" function.
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

void		*cDhsCmdHandlerBase::execThread
(
    void	*arg		// (in)  Pointer to a server command.
)
{
    cDhsServerCommand		//  Pointer to a server command.
		*pCmdObj;


    pCmdObj = (cDhsServerCommand *) arg;

    try {
    	pCmdObj->dscPHandler->exec( pCmdObj );
	}
	catch( const char * s )
	{
		//
		// Some memory allocation errors will cause this exception.
		//
		std::cout << "cDhsCmdHandlerBase::execThread: Execution of command "
				<< pCmdObj->dscName << " died with string: " << s << std::endl;
	}
	catch( const DHS_STATUS s )
	{
		DHS_STATUS	st;
		DHS_STATUS	s1( DHS_S_SUCCESS );
		DHS_ERR_LEVEL	el;
		const char *str;
		//
		// Error in the dhs library.
		//

		std::cout << "cDhsCmdHandlerBase::execThread: Error in the dhs library: "
				<< s << " while executing command " << pCmdObj->dscName << std::endl;

		for ( str = cDhs::message( st, el, s1 ); s1 == DHS_S_SUCCESS;
			str = cDhs::message( st, el, s1 ) )
		{
			std::cerr << str << std::endl;
			cDhs::messageClear( s1 );
		}
	}
	catch ( ... )
	{
		std::cout << "cDhsCmdHandlerBase::execThread: Execution of command "
				<< pCmdObj->dscName << " terminated with unknown exception." << std::endl;
	}
    free( pCmdObj->dscName );

    threadDestroy();
    pCmdObj->deref();

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::cDhsServerCommand
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
// Constructor for the server command object
//
// This constructor is only here to execute the appropriate constructor
// for the cDhsAvList parent class.
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

		cDhsServerCommand::cDhsServerCommand
(
    void
) : cDhsAvList( DHS_AV_LIST_NULL )
{
    dscConnect = DHS_CONNECT_NULL;
    dscName = NULL;
    dscTag = DHS_TAG_NULL;
    dscRefCount = 1;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::~cDhsServerCommand
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
// Destructor for the cDhsServerCommand class
//
// DESCRIPTION:
// If necessary remove the command from the list of commands.
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

		cDhsServerCommand::~cDhsServerCommand
(
    void
)
{
    if ( dscTag != DHS_TAG_NULL )
    {
	rwLock.wLock();
	dscCommandList.erase( dscTag );
	rwLock.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::cmdAbort
//
// INVOCATION:
// command.cmdAbort();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) True if the command was aborted, false otherwise.
//
// PURPOSE:
// Called when a command is aborted
//
// If not overloaded by a derived class, this function just returns false.
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

bool		cDhsServerCommand::cmdAbort
(
    void
)
{
    return( false );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::commandFind
//
// INVOCATION:
// pCommand = cDhsServerCommand::commandFind( clientTag );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) clientTag (DHS_TAG) The client tag.
//
// FUNCTION VALUE:
// (cDhsServerCommand *) Pointer to the server command.
//
// PURPOSE:
// Find a server command object that coresponds to the client tag supplied
//
// Note that this method automatically increments the reference counter
// for the cDhsServerCommand object. In order to allow the object to be
// freed after the returned object is no longer referenced (which prevents
// memory leaks), the deref method must be called.
//
// DESCRIPTION:
// Look the command up in the map.
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

cDhsServerCommand
		*cDhsServerCommand::commandFind
(
    DHS_TAG	clientTag	// (in)  The client tag.
)
{
    iCommandList	i;
    cDhsServerCommand	*pCommand;


    rwLock.rLock();
    if ( ( i = dscCommandList.find( clientTag ) ) == dscCommandList.end() )
    {
	pCommand = NULL;
    }
    else
    {
	pCommand = (*i).second;
	pCommand->dscRefCount++;
    }
    rwLock.unlock();

    return( pCommand );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::exec
//
// INVOCATION:
// N.a. Called by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Default exec funtion for a command
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

void		cDhsServerCommand::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsServerCommand::init
//
// INVOCATION:
// obj.init( connect, tag, name, avlist );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) The connection the command came from.
// (>) tag (DHS_TAG) The command tag assigned to the client.
// (>) name (char *) The name of the command.
// (>) avList (DHS_AV_LIST) The attribute value list for the command.
// (>) pHandler (cDhsCmdHandlerBase &) The handler for the command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the data members of the object
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

void		cDhsServerCommand::init
(
    DHS_CONNECT	connect,	// (in)  The connection the command came from.
    DHS_TAG	tag, 		// (in)  The command tag assigned to the client.
    char	*name,		// (in)  The name of the command.
    DHS_AV_LIST	avList,		// (in)  The attribute value list for the 
				//	 command.
    cDhsCmdHandlerBase		// (in)  The handler for the command.
		&pHandler
)
{
    dalAvList = avList;
    dalFreeIt = false;
    dscConnect = connect;
    dscName = name;
    dscTag = tag;
    dscPHandler = &pHandler;

    if ( dscPCommandList == NULL )
    {
	dscPCommandList = new tCommandList;
    }
    dscCommandList[ dscTag ] = this;
}
