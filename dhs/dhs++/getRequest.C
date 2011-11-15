static char rcsid[] = "$Id: getRequest.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhs++/src/getRequest.C
//
// PURPOSE:
// Contains the implementation of the cDhsGetRequest class.
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:24:51  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:01  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.8  1998/04/02 19:29:34  nhill
// Moved method cDhsGetRequest::response from dhs++GetRequest.H.
//
// Revision 1.7  1998/03/13 21:39:01  nhill
// Removed some problems that could occur when a handler is removed while
// a get request is being processed.
//
// Revision 1.6  1998/03/05 23:45:40  nhill
// Added checking to ensure the handler is enabled before handing a get
// request.
//
// Revision 1.5  1998/02/24 01:22:34  nhill
// Bug fixes.
//
// Revision 1.4  1997/04/15 21:08:00  nhill
// Removed debugging messages.
//
// Revision 1.3  1997/03/25 00:02:02  nhill
// Checkin for install.
//
// Revision 1.2  1997/03/06 19:16:04  nhill
// Cleaned up the header file dependancies.
//
// Revision 1.1  1997/02/21 21:45:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <cstdlib>
#include <map>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
}

#include "genMutex.H"
#define	DHS_NO_CLIENT
#define	DHS_NO_PUT
#define	DHS_NO_COMMAND
#include "dhs++.H"

typedef std::map< DHS_BD_GET_TYPE, cDhsGetHandlerBase *, std::less<DHS_BD_GET_TYPE> >
		tHandlerList;
typedef tHandlerList::iterator
		iHandlerList;

#define	dghHandlerList (*(tHandlerList *) dghPHandlerList)
#define	dghMutex (*((cMutex *) dghPMutex))
    
bool	cDhsGetHandlerBase::dghCbInit = false;
void	*cDhsGetHandlerBase::dghPHandlerList = NULL;
void	*cDhsGetHandlerBase::dghPMutex = NULL;


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandlerBase::cDhsGetHandlerBase
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Create a new thread for each request?
// (>) type (DHS_BD_GET_TYPE) Type of data the handler applies to.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the cDhsGetHandlerBase class.
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

		cDhsGetHandlerBase::cDhsGetHandlerBase
(
    bool	threaded,	// (in)  Create a new thread for each command?
    DHS_BD_GET_TYPE		// (in)  Type of data handler applies to.
		type
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    dghThreaded = threaded;
    dghGetType = type;

    if ( dghPMutex == NULL )
    {
	dghPMutex = new cMutex;
    }


    //
    //  If necessary set up the command callback function.
    //

    dghMutex.lock();

    if ( ! dghCbInit )
    {
	dhOldCallback = dhsCallbackSet( DHS_CBT_SERVER_GET, 
		(DHS_CB_FN_PTR) getCallback, &status );
	dghCbInit = true;
    }


    //
    // Add the handler to the list of handers.
    //

    if ( dghPHandlerList == NULL )
    {
	dghPHandlerList = new tHandlerList;
    }
    dghHandlerList[ type ] = this;
    dghMutex.unlock();

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandlerBase::~cDhsGetHandlerBase
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
// Destructor for the cDhsGetHandlerBase class
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

		cDhsGetHandlerBase::~cDhsGetHandlerBase
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    dghMutex.lock();
    dghHandlerList.erase( dghGetType );


    //
    // If there are no more get handlers in existance, then reset the 
    // callback function.
    //

    if ( dghHandlerList.size() == 0 )
    {
	dhsCallbackSet( DHS_CBT_GET, dhOldCallback, &status );
	dghCbInit = false;
	delete (tHandlerList*)dghPHandlerList;
	dghPHandlerList = NULL;
    }
    dghMutex.unlock();


    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandlerBase::getCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_COMMAND, getCallback, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection associated with the command.
// (>) tag (DHS_TAG) The tag associated with the command.
// (>) label (char *) The label for the data.
// (>) type (DHS_BD_GET_TYPE) The type of data being received.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This is assigned as the server get callback function for the DHS library
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

void		cDhsGetHandlerBase::getCallback
(
    DHS_CONNECT	connect, 	// (in)  Connection associated with the command.
    DHS_TAG	tag, 		// (in)  The tag associated with the command.
    char	*label,		// (in)  The data label.
    DHS_BD_GET_TYPE		// (in)  The type of the data.
		type
)
{
    iHandlerList i;		// Iterator in the handler list.
    cDhsGetHandlerBase		// The current get handler.
		*pHandler;
    cDhsGetRequest		// An object representing the request.
		*pGetObj;
    DHS_STATUS	status( DHS_S_SUCCESS );		
				// DHS library return status.
    bool	threaded;


    //
    //  Get the pointer to the current handler.
    //

    dghMutex.lock();
    if ( &dghHandlerList == NULL )
    {
	dhsBdResponse( connect, tag, DHS_CS_ERROR, 
		"This server cannot handle get requests.", 
		DHS_AV_LIST_NULL, DHS_BD_GT_RAW, NULL, 0, &status );
	dghMutex.unlock();
	return;
    
    }
    else
    {
	if ( ( i = dghHandlerList.find( type ) ) == dghHandlerList.end() )
	{
	    if ( ( i = dghHandlerList.find( DHS_BD_GT_UNKNOWN ) ) == 
		    dghHandlerList.end() )
	    {
		dhsBdResponse( connect, tag, DHS_CS_ERROR, 
			"This server cannot handle get requests of this type.", 
			DHS_AV_LIST_NULL, DHS_BD_GT_RAW, NULL, 0, &status );
		dghMutex.unlock();
		return;
	    }
	}
    }

    pHandler = (*i).second;


    pGetObj = pHandler->newGet();
    pGetObj->init( connect, tag, label, type );


    if ( ! pHandler->enabled() )
    {
	status = DHS_S_SUCCESS;
	pGetObj->response( DHS_CS_ERROR, "Get handler is disabled", status );
	delete pGetObj;
	dghMutex.unlock();
	return;
    }


    threaded = pHandler->dghThreaded;
    dghMutex.unlock();
    if ( threaded )
    {
	status = DHS_S_SUCCESS;
	pGetObj->dgrFreeData = true;
	pGetObj->dgrLabel = strsav( pGetObj->dgrLabel );

	if ( status != DHS_S_SUCCESS )
	{
	    dhsMsgCallback( pGetObj->dgrConnect, pGetObj->dgrTag );
	    status = DHS_S_SUCCESS;
	    pGetObj->response( DHS_CS_ERROR, "Failed to get data", status );
	}
	else
	{
	    threadCreate( execThread, pGetObj );
	}
    }
    else
    {
	pGetObj->exec();
	delete pGetObj;
    }
    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandlerBase::execThread
//
// INVOCATION:
// s = threadCreate( execThread, pGetObj );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pGetObj (cDhsServerCommand *) Pointer to a server command
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

void		*cDhsGetHandlerBase::execThread
(
    void	*arg		// (in)  Pointer to a server command.
)
{
    cDhsGetRequest		//  Pointer to a server command.
		*pGetObj;


    pGetObj = (cDhsGetRequest *) arg;


    try {
    	pGetObj->exec();
	}
	catch( const char * s )
	{
		//
		// Some memory allocation errors will cause this exception.
		//
		std::cout << "cDhsGetHandlerBase::execThread: Died with string: " << s << std::endl;
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

		std::cout << "cDhsGetHandlerBase::execThread: Error in the dhs library: " << s << std::endl;

		for ( str = cDhs::message( st, el, s1 ); s1 == DHS_S_SUCCESS;
			str = cDhs::message( st, el, s1 ) )
		{
			std::cerr << str << std::endl;
			cDhs::messageClear( s1 );
		}
	}
	catch ( ... )
	{
		std::cout << "cDhsGetHandlerBase::execThread: Terminated with unknown exception." << std::endl;
	}

    threadDestroy();
    delete pGetObj;

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::~cDhsGetRequest
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
// Destructor for a get request
//
// DESCRIPTION:
// Delete any memory allocated to the object.
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

		cDhsGetRequest::~cDhsGetRequest
(
    void
)
{
    if ( dgrFreeData )
    {
	if ( dgrLabel != NULL )
	{
	    free( dgrLabel );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::init
//
// INVOCATION:
// cDhsGetHandlerBase	*pHandler;
// DHS_CONNECT		connect;
// DHS_TAG		tag;
// char			*label;
// DHS_BD_GET_TYPE	getType;
// get::init( pHandler, connect, tag, label, getType );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) The connection the request came from.
// (>) tag (DHS_TAG) The clients command tag.
// (>) label (char *) The data label requested.
// (>) getType (DHS_BD_GET_TYPE) The type of data to retrieve.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the data members of the cDhsGetRequest class
//
// This is not implemented as a constructor so developers creating derived
// classes don't have to create constructors with all those arguments.
//
// DESCRIPTION:
// Set the data members from the parameters.
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

void		cDhsGetRequest::init
(
    DHS_CONNECT	connect,	// (in)  The connection the request came from.
    DHS_TAG	tag,		// (in)  The clients command tag.
    char	*label,		// (in)  The data label requested.
    DHS_BD_GET_TYPE		// (in)  The type of data to retrieve.
		getType
)
{
    dgrConnect = connect;
    dgrLabel = label;
    dgrTag = tag;
    dgrType = getType;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::response
//
// INVOCATION:
// DHS_CMD_STATUS	cmdStatus;
// req.response( cmdStatus, "status string", avList, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) New command status.
// (>) text (char *) Text string associated with the status.
// (>) avList (DHS_AV_LIST) Av list to send in the response.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response to a client
//
// DESCRIPTION:
// Execute the dhsBdResponse function.
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

void		cDhsGetRequest::response
(
    DHS_CMD_STATUS		// (in)  The new status for the command.
    		cmdStatus,
    const char	*text,		// (in)  The text associated with the status.
    const cDhsAvList		// (in)  The avList to send in the response.
		&avList,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_AV_LIST	avId;


    //
    // Ensure that the command state has been changed to DHS_CS_BUSY before
    // it is set to any other state.  This is necessary to avoid deadlock
    // on the client side under certain conditions. (The conditions are:
    // the client allocates the get dataset object on the heap, and deletes 
    // the get dataset in the callback routine.)
    //

    if ( cmdStatus != DHS_CS_BUSY && dgrLastResponse != DHS_CS_BUSY )
    {
	dhsBdResponse( dgrConnect, dgrTag, DHS_CS_BUSY, NULL, DHS_AV_LIST_NULL, 
		DHS_BD_GT_RAW, NULL, 0, &status );
    }

    if ( &avList == NULL )
    {
	avId = DHS_AV_LIST_NULL;
    }
    else
    {
	avId = avList.dalAvList;
    }

    dhsBdResponse( dgrConnect, dgrTag, cmdStatus, text, avId, 
	    DHS_BD_GT_RAW, NULL, 0, &status );

    dgrLastResponse = cmdStatus;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::response
//
// INVOCATION:
// DHS_CMD_STATUS	cmdStatus;
// DHS_BD_GET_TYPE	type;
// req.response( cmdStatus, "status string", avList, type, pData, length, 
//		status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) New command status.
// (>) text (char *) Text string associated with the status.
// (>) avList (const cDhsAvList &) The attribute value list to send in the 
//		response.
// (>) type (DHS_BD_GET_TYPE) Type of data to be sent.
// (>) pData (void *) Pointer to a data buffer.
// (>) length (unsigned long) Length of the data buffer.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response buffer to the client
//
// DESCRIPTION:
// Execute the dhsBdResponse function.
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

void		cDhsGetRequest::response
(
    DHS_CMD_STATUS		// (in)  New command status.
    		cmdStatus,
    const char	*text,		// (in)  Text string associated with the status.
    const cDhsAvList		// (in)  The attribute value list to send in 
		&avList,	//	 the response.
    DHS_BD_GET_TYPE		// (in)  Type of data to be sent.
		type,
    const void	*pData,		// (in)  Pointer to the data buffer.
    unsigned long		// (in)  Length of the data buffer.
    		length,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_AV_LIST	avId;


    if ( cmdStatus != DHS_CS_BUSY && dgrLastResponse != DHS_CS_BUSY )
    {
	dhsBdResponse( dgrConnect, dgrTag, DHS_CS_BUSY, NULL, DHS_AV_LIST_NULL, 
		DHS_BD_GT_RAW, NULL, 0, &status );
    }

    if ( &avList == NULL )
    {
	avId = DHS_AV_LIST_NULL;
    }
    else
    {
	avId = avList.dalAvList;
    }

    dhsBdResponse( dgrConnect, dgrTag, cmdStatus, text, avId, 
	    type, pData, length, &status );

    dgrLastResponse = cmdStatus;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::response
//
// INVOCATION:
// DHS_CMD_STATUS	cmdStatus;
// DHS_BD_GET_TYPE	type;
// req.response( cmdStatus, "status string", avList, type, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) New command status.
// (>) text (char *) Text string associated with the status.
// (>) avList (const cDhsAvList &) The attribute value list to send in the 
//			response.
// (>) type (DHS_BD_GET_TYPE) Type of data to be sent.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response buffer to the client
//
// DESCRIPTION:
// Execute the dhsBdResponse function.
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

void		cDhsGetRequest::response
(
    DHS_CMD_STATUS		// (in)  New command status.
    		cmdStatus,
    const char	*text,		// (in)  Text string associated with the status.
    const cDhsAvList 		// (in)  The attribute value list to send in 
		&avList,	//	 the response.
    DHS_BD_GET_TYPE		// (in)  Type of data to be sent.
		type,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_AV_LIST	avId;


    if ( cmdStatus != DHS_CS_BUSY && dgrLastResponse != DHS_CS_BUSY )
    {
	dhsBdResponse( dgrConnect, dgrTag, DHS_CS_BUSY, NULL, DHS_AV_LIST_NULL, 
		DHS_BD_GT_RAW, NULL, 0, &status );
    }

    if ( &avList == NULL )
    {
	avId = DHS_AV_LIST_NULL;
    }
    else
    {
	avId = avList.dalAvList;
    }

    dhsBdResponse( dgrConnect, dgrTag, cmdStatus, text, avId, 
	    type, ddsObject, &status );

    dgrLastResponse = cmdStatus;
}
