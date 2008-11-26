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
// dhs++/h/dhs++PutRequest.H
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
// Revision 1.10  1998/04/02 19:45:30  nhill
// Added a new method to allow avLists to be sent with responses.
// Moved the larger response function to file putRequest.C.
//
// Revision 1.9  1998/03/13 21:37:37  nhill
// Removed the thread count from the server put handler.
//
// Revision 1.8  1997/06/12 16:58:29  nhill
// Added a function to return the requests connection.
//
// Revision 1.7  1997/05/01 23:21:16  nhill
// Changed to ensure a response setting the request state to DHS_CS_BUSY is
// sent before the command reaches and end state.
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

#ifndef DHSPPPUTREQUEST_H
#define	DHSPPPUTREQUEST_H


//
//  this is to allow forward references.
//

class	cDhsPutHandlerBase;

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsPutRequest
//
// PUBLIC METHODS: 
// (char *) label - Return the data label associated with the data.
// (bool) last - Return true if this is the last chunk for the data label.
// (void) response - Send a response to the client.
// (DHS_BD_PUT_TYPE) type - Return the format of the data.
//
// PURPOSE:
// Base class for application specific put request objects
//
// SEE ALSO:
// - class cDhsPutHandler
//
//-
//***********************************************************************
//

class cDhsPutRequest: public cDhsDataset
{
    friend class cDhsPutHandlerBase;

    public:
	inline DHS_CONNECT
			connect( void );
	inline const char	
			*label( void );
	inline bool	last( void );
	inline void	response( DHS_CMD_STATUS cmdStatus, const char * text, 
				DHS_STATUS & status )
			{
			    response( cmdStatus, text, *(cDhsAvList *) NULL,
				    status );
			}
	void		response( DHS_CMD_STATUS, const char *, 
				const cDhsAvList &, DHS_STATUS & );
	inline DHS_BD_PUT_TYPE
			type( void );
    protected:
	inline		cDhsPutRequest( void );
	virtual		~cDhsPutRequest( void );

	virtual void	exec( void ) = 0;
    private:
	void		init( DHS_CONNECT, DHS_TAG, char *, DHS_BD_PUT_TYPE, 
				DHS_BOOLEAN, void *, int );

	DHS_CONNECT	dprConnect;	// Connection data came over.
	bool		dprFreeData;	// Free the data when done?
	char		*dprLabel;	// Data label for the data.
	bool		dprLast;	// Is this the last of the data.
	DHS_CMD_STATUS	dprLastResponse;// The last status sent to the client.
	DHS_TAG		dprTag;		// Client tag assigned to the data.
	DHS_BD_PUT_TYPE	dprType;	// Type of data in the buffer.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::cDhsPutRequest
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
// constructor for a cDhsPutRequest object
//
// DESCRIPTION:
// Initialize some of the member data.
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

		cDhsPutRequest::cDhsPutRequest
(
    void
) : cDhsDataset( DHS_BD_DATASET_NULL )
{
    dprFreeData = false;
    dprLabel = NULL;
    dprLastResponse = DHS_CS_PENDING;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::connect
//
// INVOCATION:
// connect = request.connection();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_CONNECT) The connection associated with the request.
//
// PURPOSE:
// Accessor function to return the connection associated with a request.
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

DHS_CONNECT	cDhsPutRequest::connect
(
    void
)
{
    return( dprConnect );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::label
//
// INVOCATION:
// label = req.label();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (char *) Pointer to the data label for the request.
//
// PURPOSE:
// Accessor function to return the data label in the request.
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

const char	*cDhsPutRequest::label
(
    void
)
{
    return dprLabel;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::last
//
// INVOCATION:
// last = req.last();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) Is this the last chunk for the datalabel?
//
// PURPOSE:
// Accessor function to return the dprLast value.
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

bool		cDhsPutRequest::last
(
    void
)
{
    return( dprLast );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutRequest::type
//
// INVOCATION:
// type = req.type();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_BD_PUT_TYPE) Type of data sent.
//
// PURPOSE:
// Accessor function to return the type of data in the request
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

DHS_BD_PUT_TYPE	cDhsPutRequest::type
(
    void
)
{
    return( dprType );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsPutHandlerBase
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// This class only exists to allow methods and data common to all 
// instances of the cDhsPutHandler template class to be placed in a 
// common, shared class.
//
// SEE ALSO:
// - class cDhsPutHandler
// - class cDhsPutRequest
//-
//***********************************************************************
//

class cDhsPutHandlerBase: public cDhsHandler
{
    public:
    protected:
		       cDhsPutHandlerBase( bool, DHS_BD_PUT_TYPE );
	virtual	       ~cDhsPutHandlerBase( void );
	virtual cDhsPutRequest
			*newPut( void ) = 0;
    private:
	static void	putCallback( DHS_CONNECT, DHS_TAG, char *, 
				DHS_BD_PUT_TYPE, DHS_BOOLEAN, void *, 
				unsigned long );
	static void	*execThread( void * );

	static bool	dphCbInit;	// Is the callback initialized.
	DHS_BD_PUT_TYPE	dphPutType;	// Type of data being put.
	static void	*dphPHandlerList;// List of put handlers.
	static void	*dphPMutex;	// Mutex to protect the handler list.
	bool		dphThreaded;	// Execute each request in a thread?
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsPutHandler
//
// PUBLIC METHODS: 
// (None) cDhsPutHandler - Constructor.
//
// PURPOSE:
// Created to handle data received by a server.
//
// SEE ALSO:
// - class cDhsPutHandlerBase
//-
//***********************************************************************
//

template <class userPut>
class cDhsPutHandler : public cDhsPutHandlerBase
{
    public:
	inline	    	cDhsPutHandler( bool = false, 
				DHS_BD_PUT_TYPE = DHS_BD_PT_UNKNOWN );
    protected:
    private:
	cDhsPutRequest	*newPut( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutHandler::cDhsPutHandler
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) threaded (bool) Handle each request in a new thread?
// (>) type (DHS_BD_PUT_TYPE) Type of puts the handler handles.
// (>) dataLabel (char *) Data label pattern for the handler.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute the cDhsPutHandlerBase constructor.
//
// DESCRIPTION:
// Set up the DHS library put request callback function.
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

template <class userPut>
		cDhsPutHandler<userPut>::cDhsPutHandler
(
    bool	threaded,	// (in)  Handle each request in a new thread?
    DHS_BD_PUT_TYPE		// (in)  Type of put the handler handles.
    		type
) : cDhsPutHandlerBase( threaded, type )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutHandler::newPut
//
// INVOCATION:
// pPutRequestObj = putHandler.newPut();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cDhsPutRequest *) Pointer to the new object.
//
// PURPOSE:
// Create a new command object when a new put request is received
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

template <class userPut>
cDhsPutRequest	*cDhsPutHandler<userPut>::newPut
(
    void
)
{
    userPut	*pPut;		// Pointer to the new put request object.


    pPut = new userPut;
    if ( pPut == NULL )
    {
	throw "memory allocation error";
    }

    return( pPut );
}
#endif // DHSPPPUTREQUEST_H
