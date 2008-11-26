static char rcsid[] = "$Id: putRequest.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhs++/src/putRequest.C
//
// PURPOSE:
// Contains the implementation of the cDhsPutRequest class.
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:25:03  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.9  1998/04/02 19:31:41  nhill
// Moved method cDhsPutRequest::response from dhs++PutRquest.H.
//
// Revision 1.8  1998/03/13 21:40:04  nhill
// Fixed some problems that could occur when a handler was removed
// while a put request was being processed.
//
// Revision 1.7  1998/03/05 23:46:06  nhill
// Added checking to ensure the handler is enabled before handing a put
// request.
//
// Revision 1.6  1998/02/24 01:22:57  nhill
// Bug fixes.
//
// Revision 1.5  1997/07/11 21:45:57  nhill
// Fixed a bug where the memory of an accessed object could be freed
// before the sds object was freed.
//
// Revision 1.4  1997/04/15 21:08:12  nhill
// Removed debugging message.
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
#define	DHS_NO_GET
#define	DHS_NO_COMMAND
#include "dhs++.H"

typedef std::map< DHS_BD_PUT_TYPE, cDhsPutHandlerBase *, std::less<DHS_BD_PUT_TYPE> >
		tHandlerList;
typedef tHandlerList::iterator
		iHandlerList;

#define	dphHandlerList (*(tHandlerList *) dphPHandlerList)
#define	dphMutex (*((cMutex *) dphPMutex))

bool	cDhsPutHandlerBase::dphCbInit = false;
void	*cDhsPutHandlerBase::dphPHandlerList = NULL;
void	*cDhsPutHandlerBase::dphPMutex = NULL;


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutHandlerBase::cDhsPutHandlerBase
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Create a new thread for each request?
// (>) type (DHS_BD_PUT_TYPE) Type of puts the handler handles.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDhsPutHandlerBase class
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

		cDhsPutHandlerBase::cDhsPutHandlerBase
(
    bool	threaded,	// (in)  Create a new thread for each command?
    DHS_BD_PUT_TYPE		// (in)  The type of puts the handler handles.
    		type
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    dphThreaded = threaded;
    dphPutType = type;


    if ( dphPMutex == NULL )
    {
	dphPMutex = new cMutex;
    }


    //
    //  If necessary, set up the command callback function.
    //

    dphMutex.lock();

    if ( ! dphCbInit )
    {
	dhOldCallback = dhsCallbackSet( DHS_CBT_SERVER_PUT, 
		(DHS_CB_FN_PTR) putCallback, &status );
	dphCbInit = true;
    }


    //
    // Add the handler to the list of handlers.
    //

    if ( dphPHandlerList == NULL )
    {
	dphPHandlerList = new tHandlerList;
    }

    dphHandlerList[ dphPutType ] = this;
    dphMutex.unlock();
    

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutHandlerBase::~cDhsPutHandlerBase
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
// Destructor for the cDhsPutHandlerBase class
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

		cDhsPutHandlerBase::~cDhsPutHandlerBase
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );

    dphMutex.lock();
    dphHandlerList.erase( dphPutType );


    //
    // If there are no more put handlers in existance, then reset the 
    // callback function.
    //

    if ( dphHandlerList.size() == 0 )
    {
	dhsCallbackSet( DHS_CBT_PUT, dhOldCallback, &status );
	dphCbInit = false;
	delete (tHandlerList*)dphPHandlerList;
	dphPHandlerList = NULL;
    }
    dphMutex.unlock();

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
} 
//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutHandlerBase::PutCallback
//
// INVOCATION:
// dhsCallbackSet( DHS_CBT_COMMAND, PutCallback, &status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection associated with the command.
// (>) tag (DHS_TAG) The tag associated with the command.
// (>) label (char *) The label for the data.
// (>) type (DHS_BD_PUT_TYPE) The type of data being received.
// (>) last (DHS_BOOLEAN) Is this the last data for the label?
// (>) pData (void *) Pointer to the data buffer.
// (>) length (int) length of the data buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This is assigned as the server put callback function for the DHS library
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

void		cDhsPutHandlerBase::putCallback
(
    DHS_CONNECT	connect, 	// (in)  Connection associated with the command.
    DHS_TAG	tag, 		// (in)  The tag associated with the command.
    char	*label,		// (in)  The data label.
    DHS_BD_PUT_TYPE		// (in)  The type of the data.
		type,
    DHS_BOOLEAN	last,		// (in)  Is this the last data for the label?
    void	*pData,		// (in)  Pointer to the data buffer.
    unsigned long		// (in)  Length of the data buffer.
		length
)
{
    iHandlerList i;		// Iterator in the handler list.
    cDhsPutHandlerBase		// The current put handler.
		*pHandler;
    cDhsPutRequest		// An object representing the request.
		*pPutObj;
    DHS_STATUS	status( DHS_S_SUCCESS );		
				// DHS library return status.
    bool	threaded;

   
    //
    //  Get the pointer to the handler.
    //

    dphMutex.lock();
    if ( ( & dphHandlerList ) == NULL )
    {
	dhsCmdResponse( connect, tag, DHS_CS_ERROR, 
		"This server cannot handle put requests.", 
		DHS_AV_LIST_NULL, &status );
	dphMutex.unlock();
	return;
    }
    else
    {
	if ( ( i = dphHandlerList.find( type ) )
		== dphHandlerList.end() )
	{
	    if ( ( i = dphHandlerList.find( DHS_BD_PT_UNKNOWN ) ) == 
		    dphHandlerList.end() )
	    {
		dhsCmdResponse( connect, tag, DHS_CS_ERROR, 
			"Server cannot handle this data type.", 
			DHS_AV_LIST_NULL, &status );
		dphMutex.unlock();
		return;
	    }
	}
    }

    pHandler = (*i).second;

    pPutObj = pHandler->newPut();
    pPutObj->init( connect, tag, label, type, last, pData, length );


    if ( ! pHandler->enabled() )
    {
	status = DHS_S_SUCCESS;
	pPutObj->response( DHS_CS_ERROR, "Put handler is disabled", status );
	delete pPutObj;
	dphMutex.unlock();
	return;
    }


    threaded = pHandler->dphThreaded;
    dphMutex.unlock();
    if ( threaded )
    {
	status = DHS_S_SUCCESS;
	pPutObj->dprFreeData = true;
	pPutObj->ddsPBuffer = new char[length];
	memcpy( pPutObj->ddsPBuffer, pData, length );
	pPutObj->dprLabel = strsav( pPutObj->dprLabel );

	if ( status != DHS_S_SUCCESS )
	{
	    dhsMsgCallback( pPutObj->dprConnect, pPutObj->dprTag );
	    status = DHS_S_SUCCESS;
	    pPutObj->response( DHS_CS_ERROR, "Failed to put data", status );
	}
	else
	{
	    threadCreate( execThread, pPutObj );
	}
    }
    else
    {
	pPutObj->exec();
	delete pPutObj;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutHandlerBase::execThread
//
// INVOCATION:
// s = threadCreate( execThread, pPutObj );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pPutObj (cDhsServerCommand *) Pointer to a server command
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

void		*cDhsPutHandlerBase::execThread
(
    void	*arg		// (in)  Pointer to a server command.
)
{
    cDhsPutRequest		//  Pointer to a server command.
		*pPutObj;


    pPutObj = (cDhsPutRequest *) arg;


    pPutObj->exec();

    threadDestroy();
    delete pPutObj;

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::~cDhsPutRequest
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
// Destructor for a put request
//
// DESCRIPTION:
// If necessary, frees the memory allocated to the request.
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

		cDhsPutRequest::~cDhsPutRequest
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    if ( dprFreeData )
    {
	if ( dprLabel != NULL )
	{
	    free( dprLabel );
	}


	//
	// Free the object now. This is necessary since if the dataset
	// is accessed from the ddsPBuffer, Freeing the sds object
	// after the buffer is free (as it would be in the dataset
	// destructor) can lead to problems if the memory is reused.
	//

	if ( ddsObjectFree )
	{
	    dhsBdDsFree( ddsObject, &status );
	    if ( status != DHS_S_SUCCESS )
	    {
		throw status;
	    }
	    ddsObjectFree = false;
	}
	    
	delete [] (char*)ddsPBuffer;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::init
//
// INVOCATION:
// req.init( connect, tag, label, type, last, pData, length );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) The connection the command came from.
// (>) tag (DHS_TAG) The command tag assigned to the client.
// (>) label (char *) The data label for the data.
// (>) type (DHS_BD_PUT_TYPE) The type of data received.
// (>) last (DHS_BOOLEAN) Is this the last data for the dataset?
// (>) pData (void *) Pointer to the data buffer.
// (>) length (int) The length of the data buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialze the members of the cDhsPutRequest structure
//
// DESCRIPTION:
// This isn't a constructor so derived classes don't have to worry about
// creating a constructor with all those arguments.
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

void		cDhsPutRequest::init
(
    DHS_CONNECT	connect,	// (in)  The connection the command came from.
    DHS_TAG	tag,		// (in)  The command tag assigned to the client.
    char	*label,		// (in)  The data label for the data.
    DHS_BD_PUT_TYPE		// (in)  The type of data received.
		type,
    DHS_BOOLEAN	last,		// (in)  Is this the last data for the dataset?
    void	*pData,		// (in)  Pointer to the data buffer.
    int		length		// (in)  The length of the data buffer.
)
{
    dprConnect = connect;
    dprLabel = label;
    dprLast = last;
    ddsLength = length;
    ddsPBuffer = pData;
    dprTag = tag;
    dprType = type;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::response
//
// INVOCATION:
// req.resposne( cmdStatus, text, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) The new status for the request.
// (>) text (char *) The text for the response.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a response for a put request
//
// DESCRIPTION:
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

void		cDhsPutRequest::response
(
    DHS_CMD_STATUS		// (in)  The new status for the request.
		cmdStatus,
    const char	*text,		// (in)  The text describing the request.
    const cDhsAvList		// (in)  The av list to send with the response.
		&avList,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_AV_LIST	avId;


    //
    // Ensure that the request state has been changed to DHS_CS_BUSY before
    // it is set to any other state.  This is necessary to avoid deadlock
    // on the client side under certain conditions. (The conditions are:
    // the client allocates the get dataset object on the heap, and deletes
    // the put dataset in the callback routine.)
    //

    if ( cmdStatus != DHS_CS_BUSY && dprLastResponse != DHS_CS_BUSY )
    {
	dhsCmdResponse( dprConnect, dprTag, DHS_CS_BUSY, NULL, 
		DHS_AV_LIST_NULL, &status );
    }

    if ( &avList == NULL )
    {
	avId = DHS_AV_LIST_NULL;
    }
    else
    {
	avId = avList.dalAvList;
    }

    dhsCmdResponse( dprConnect, dprTag, cmdStatus, text, avId, &status );

    dprLastResponse = cmdStatus;
}
