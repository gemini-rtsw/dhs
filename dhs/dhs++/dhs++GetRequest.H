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
// dhs++/h/dhs++GetRequest.H
//
// PURPOSE:
// Header for the server command class.
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
// Revision 1.10  1998/04/02 19:43:58  nhill
// Added new response methods to allow avLists to be send with
// responses.
// Moved larger response methods to getRequest.C.
//
// Revision 1.9  1998/03/13 21:36:54  nhill
// Removed the thread count from the server get handler.
//
// Revision 1.8  1997/06/12 16:57:24  nhill
// Added a function to return a requests connection id.
// Changed to not use the DHS_BD_GT_DS request types for unkown request types.
//
// Revision 1.7  1997/05/01 23:15:50  nhill
// Changed to ensure the request state goes to DHS_CS_BUSY before it goes
// to DHS_CS_DONE.
//
// Revision 1.6  1997/04/24 16:56:39  nhill
// Added "const" keywords to function prototypes.
//
// Revision 1.5  1997/03/25 00:02:46  nhill
// Checkin for install.
//
// Revision 1.4  1997/03/06 19:16:30  nhill
// Cleaned up the header file dependancies.
//
// Revision 1.3  1997/02/28 00:54:14  nhill
// Checkin for install.
//
// Revision 1.2  1997/02/24 19:23:10  nhill
// checkin for install.
//
// Revision 1.1  1997/02/21 21:46:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSPPGETREQUEST_H
#define	DHSPPGETREQUEST_H


//
//  this is to allow forward references.
//

class	cDhsGetHandlerBase;

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsGetRequest
//
// PUBLIC METHODS: 
// (void) connect - Return the connection that received the request.
// (void) response - Send a response to the client.
// (char *) label - Return the data label requested.
// (DHS_BD_GET_TYPE) type - Return the format requested for the data.
//
// PURPOSE:
// Base class for user specified server get request objects
//
// SEE ALSO:
// - class cDhsGetHandler
//
//-
//***********************************************************************
//

class cDhsGetRequest : public cDhsDataset
{
    friend class cDhsGetHandlerBase;

    public:
	inline DHS_CONNECT
			connect( void );
	void		response( DHS_CMD_STATUS, const char *, 
				const cDhsAvList &, DHS_STATUS & );
	inline void	response( DHS_CMD_STATUS cmdStatus, const char * text, 
				DHS_STATUS & status )
			{
			    response( cmdStatus, text, 
				    *(cDhsAvList *) NULL, status );
			}
	void		response( DHS_CMD_STATUS, const char *, 
				const cDhsAvList &, DHS_BD_GET_TYPE, 
				DHS_STATUS & );
	inline void	response( DHS_CMD_STATUS cmdStatus, const char * text, 
				DHS_BD_GET_TYPE getType, DHS_STATUS & status )
			{
			    response( cmdStatus, text, *(cDhsAvList *) NULL,
				    getType, status );
			}
	void		response( DHS_CMD_STATUS, const char *, 
				const cDhsAvList &, DHS_BD_GET_TYPE, 
				const void *, unsigned long, DHS_STATUS & );
	inline void	response( DHS_CMD_STATUS cmdStatus, const char * text, 
				DHS_BD_GET_TYPE getType, const void * ptr, 
				unsigned long length, DHS_STATUS & status )
			{
			    response( cmdStatus, text, 
				    *(cDhsAvList *) NULL, getType, ptr, 
				    length, status );
			}
	inline const char	
			*label( void );
	inline DHS_BD_GET_TYPE	
			type( void );
    protected:
	inline		cDhsGetRequest( void );
	virtual		~cDhsGetRequest( void );
	virtual void	exec( void ) = 0;
    private:
	void		init( DHS_CONNECT, DHS_TAG, char *, DHS_BD_GET_TYPE );

	bool		dgrFreeData;	// Free the data when done?
	DHS_CONNECT	dgrConnect;	// Connection data came over.
	char		*dgrLabel;	// Data label for the data.
	DHS_CMD_STATUS	dgrLastResponse;// The last response sent to the client.
	DHS_TAG		dgrTag;		// Client tag assigned to the data.
	DHS_BD_GET_TYPE	dgrType;	// Type of data in the buffer.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::cDhsGetRequest
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
// Constructor for a get request
//
// DESCRIPTION:
// Initialize some of the data members.
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

		cDhsGetRequest::cDhsGetRequest
(
    void
) : cDhsDataset( DHS_BD_DATASET_NULL )
{
    dgrFreeData = false;
    dgrLastResponse = DHS_CS_PENDING;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::connect
//
// INVOCATION:
// connect = request.connect();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_CONNECT) the connection associated with the request.
//
// PURPOSE:
// Accessor function to return the connection associated with the request
//
// DESCRIPTION:
// Trivial
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

DHS_CONNECT	cDhsGetRequest::connect
(
    void
)
{
    return( dgrConnect );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::label
//
// INVOCATION:
// label = req.label();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (const char *) Data label for the request.
//
// PURPOSE:
// Accessor function to return the data label for the request
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

const char	*cDhsGetRequest::label
(
    void
)
{
    return( dgrLabel );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetRequest::type
//
// INVOCATION:
// type = req.type();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_BD_GET_TYPE) The type of data requested.
//
// PURPOSE:
// Accessor function to return the type of the data requested
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

DHS_BD_GET_TYPE	cDhsGetRequest::type
(
    void
)
{
    return( dgrType );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsGetHandlerBase
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// This class only exists to allow methods and data common to all 
// instantiations of the cDhsGetHandler template class to be moved into
// a common, shared class.
//
// SEE ALSO:
// - class cDhsGetHandler
// - class cDhsGetRequest
//
//-
//***********************************************************************
//

class cDhsGetHandlerBase: public cDhsHandler
{
    public:
    protected:
		       cDhsGetHandlerBase( bool, DHS_BD_GET_TYPE );
	virtual	       ~cDhsGetHandlerBase( void );
	virtual cDhsGetRequest
			*newGet( void ) = 0;
    private:
	static void	*execThread( void * );
	static void	getCallback( DHS_CONNECT, DHS_TAG, char *, 
				DHS_BD_GET_TYPE );

	static bool	dghCbInit;	// Is the callback initialized?
	DHS_BD_GET_TYPE	dghGetType;	// Get type the handler applies to.
	static void	*dghPHandlerList;// List of get handlers.
	static void	*dghPMutex;	// Mutex to protect the handler list.
	bool		dghThreaded;	// Execute each request in a thread?
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsGetHandler
//
// PUBLIC METHODS: 
// (None) cDhsGetHandler - Constructor.
//
// PURPOSE:
// Created to handle get requests received by a server.
//
// SEE ALSO:
// - class cDhsGetHandlerBase
// - class cDhsGetRequest
//-
//***********************************************************************
//

template <class userGet>
class cDhsGetHandler : public cDhsGetHandlerBase
{
    public:
	inline	    	cDhsGetHandler( bool = false );
	inline	    	cDhsGetHandler( bool, DHS_BD_GET_TYPE );
    protected:
    private:
	cDhsGetRequest	*newGet( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandler::cDhsGetHandler
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Handle each request in a new thread?
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDhsGetHandler class
//
// DESCRIPTION:
// Execute the cDhsGetHandlerBase callback function.
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

template <class userGet>
		cDhsGetHandler<userGet>::cDhsGetHandler
(
    bool	threaded	// (in)  Handle each request in a new thread?
) : cDhsGetHandlerBase( threaded, DHS_BD_GT_UNKNOWN )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandler::cDhsGetHandler
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Handle each request in a new thread?
// (>) type (DHS_BD_GET_TYPE) Type of data the handler applies to.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDhsGetHandler class
//
// DESCRIPTION:
// Execute the cDhsGetHandlerBase callback function.
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

template <class userGet>
		cDhsGetHandler<userGet>::cDhsGetHandler
(
    bool	threaded,	// (in)  Handle each request in a new thread?
    DHS_BD_GET_TYPE		// (in)  Handle each request in a new thread?
		type
) : cDhsGetHandlerBase( threaded, type )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsGetHandler::newGet
//
// INVOCATION:
// pGetRequestObj = getHandler.newGet();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cDhsGetRequest *) Pointer to the new object.
//
// PURPOSE:
// Create a new command object when a new get request is received
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

template <class userGet>
cDhsGetRequest	*cDhsGetHandler<userGet>::newGet
(
    void
)
{
    userGet	*pGet;		// Pointer to the new get request object.


    pGet = new userGet;
    if ( pGet == NULL )
    {
	throw "memory allocation error";
    }

    return( pGet );
}
#endif // DHSPPGETREQUEST_H
