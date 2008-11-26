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
// dhs++/h/dhs++Client.H
//
// PURPOSE:
// Contains the interfaces to classes required only by dhs++ library
// client applications.
//
// CLASSES:
// cDhsConnection - Manages a connection to a server.
// cDhsTag - Base class for all client commands and bulk data requests.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.16  1998/05/19 18:11:56  nhill
// Added an == operator to the cDhsStatus class.
//
// Revision 1.15  1998/05/12 21:02:15  nhill
// Fixed a bug in the cDhsConnect::open method.
// Updated the documentation.
//
// Revision 1.14  1998/05/11 16:34:27  nhill
// Added a function to execute a callback when the connection state changes.
//
// Revision 1.13  1998/05/04 20:50:44  nhill
// Added an accessor function to return the address read from the config
// file.
//
// Revision 1.12  1998/04/02 19:34:57  nhill
// Moved method cDhsTag::responseGet from class cDhsClientCommand in
// file dhs++ClientCommand.H.
//
// Revision 1.11  1998/02/24 01:11:42  nhill
// Added some error checks.
//
// Revision 1.10  1998/01/20 19:02:49  nhill
// Added the connection callback routine.
//
// Revision 1.9  1997/11/30 21:15:12  nhill
// Added a constructor for the cDhsTag class.
//
// Revision 1.8  1997/08/26 21:11:06  nhill
// Added the cDhsTag class.
//
// Revision 1.7  1997/08/19 19:57:53  nhill
// Added function cDhsConnection::dhsConnect and cDhsConnection::name.
//
// Revision 1.6  1997/06/12 17:00:50  nhill
// Disabled the assignment and copy constructor in the cDhsConnection class.
//
// Revision 1.5  1997/04/30 23:48:26  nhill
// Added a dcOpen flag to the connection class, and methods to access it.
//
// Revision 1.4  1997/04/15 20:07:38  nhill
// Moved the open function to the C file.
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

#ifndef DHSPPCLIENT_H
#define	DHSPPCLIENT_H

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsConnection
//
// PUBLIC METHODS: 
// (None) cDhsConnection - Constructor.
// (None) ~cDhsConnection - Destructor.
// (const char *) address - Return the internet address associated with the
//		connection.
// (void) close - Close a connection.
// (DHS_CONNECT) dhsConnect - Return the DHS_CONNECT associated with the 
//		object.
// (bool) isOpen - Return true if the connection is open.
// (const char *) name - Return the IMP name associated with the connection.
// (void) open - Open a connection.
//
// PURPOSE:
// Contains information about a connection to a server
//
//-
//***********************************************************************
//

class	cDhsConnection
{
    friend class cDhsClientCommand;
    friend class cDhsPutDataset;
    friend class cDhsGetDataset;

    public:
	typedef		void (*tCallbackFn) ( cDhsConnection *, void * );

	inline		cDhsConnection( void );
			cDhsConnection( char *, char * );
			cDhsConnection( DHS_CONNECT );
			~cDhsConnection( void );
	bool		operator==( cDhsConnection & connect )
			{
			    return( connect.dcConnection == dcConnection );
			}
	inline const char
			*address( void ) const;
	inline void	callbackSet( tCallbackFn, void * ); 
	inline void	close( DHS_STATUS & );
	inline DHS_CONNECT
			dhsConnect( void ) const;
	inline bool	isOpen( void ) const;
	inline const char
			*name( void ) const;
	inline void	open( DHS_STATUS & );
	void		open( char *, char *, DHS_STATUS & );

        // XXX allan: changed from private
	static void	connectCallback( DHS_CONNECT, DHS_CON_STATE );
					// state changes.

    protected:
    private:
			cDhsConnection( cDhsConnection & ){};
	inline cDhsConnection	
			&operator=( const cDhsConnection & )
			{ 
			    return( *this ); 
			}
	void		*dcCallbackArg;	// Argument to the callback fn.
	tCallbackFn	dcCallbackFn;	// Function called when connect state
					// changes.
	DHS_CONNECT	dcConnection;	// DHS connectdion id.
	char		*dcIpAddress;	// Ip address of server.
	char		*dcName;	// Name of server.
	bool		dcOpen;		// True if the connection is open.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::cDhsConnection
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
// Parameterless constructor for a connection
//
// DESCRIPTION:
// Initialize the data members.
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

		cDhsConnection::cDhsConnection
(
    void
)
{
    dcConnection = DHS_CONNECT_NULL;
    dcName = NULL;
    dcIpAddress = NULL;
    dcOpen = false;
    dcCallbackFn = NULL;
    dcCallbackArg = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::address
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (const char *) The address associated with the connection.
//
// PURPOSE:
// Accessor function to return the address of an IMP connection
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

const char	*cDhsConnection::address
(
    void
) const
{
    return( dcIpAddress );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::callbackSet
//
// INVOCATION:
// cDhsConnection::callbackSet( fn, arg );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fn (tCallbackFn) Function to execute when connection state changes.
// (>) arg (void *) argument to the callback function.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the callback function to execute when the connection state changes
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

void		cDhsConnection::callbackSet
(
    tCallbackFn	fn,		// (in)  Function to execute when connection 
    				//	 state changes.
    void	*arg		// (in)  Argument to pass to the callback 
    				//	 function.
)
{
    dcCallbackFn = fn;
    dcCallbackArg = arg;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::close
//
// INVOCATION:
// connect.close( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Fucntion return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Close a connection
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
// SEE ALSO:
// - open( DHS_STATUS & );
// - open( char *, char *, DHS_STATUS & );
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsConnection::close
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dcOpen = false;
    dhsDisconnect( dcConnection, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::dhsConnect
//
// INVOCATION:
// connection.dhsConnect
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_CONNECT) Return the DHS connection id.
//
// PURPOSE:
// Return the DHS connection id associated with a connection
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

DHS_CONNECT	cDhsConnection::dhsConnect
(
    void
) const
{
    if ( this != NULL )
    {
	return( dcConnection );
    }
    else
    {
	return( DHS_CONNECT_NULL );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::isOpen
//
// INVOCATION:
// isItOpen = connection.isOpen();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) True if the connection is open.
//
// PURPOSE:
// Accessor function to return true if the connection is open.
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

bool		cDhsConnection::isOpen
(
    void
) const
{
    return( dcOpen );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::name
//
// INVOCATION:
// name = connection.name()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (const char *) The IMP name of the connection.
//
// PURPOSE:
// Accessor function to return the IMP name of the connection
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

const char	*cDhsConnection::name
(
    void
) const
{
    return( dcName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsConnection::open
//
// INVOCATION:
// connect.open( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Open a connection to a server
//
// DESCRIPTION:
// - Check that the name and address are not null.
// - Use dhsConnect to open the connection.
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

void		cDhsConnection::open
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( dcIpAddress == NULL || dcName == NULL )
    {
	status = DHS_E_CONNECT;
    }
    else
    {
	dcConnection = ::dhsConnect( dcIpAddress, dcName, this, &status );
	if ( status == DHS_S_SUCCESS )
	{
	    dcOpen = true;
	}
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsTag
//
// PUBLIC METHODS: 
// cDhsTag - Constructor for the class.
// (bool) isDone - Return true of the request is complete.
// (cDhsAvList) responseGet - Return the av list associated with a response.
// (DHS_CMD_STATUS) status - Return the current status of the command.
// (DHS_TAG) tag - Return the tag associated with the command.
// (void) wait - Wait for the command to complete.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains methods and data describing a command tag
//
//-
//***********************************************************************
//

class 	cDhsTag
{
    friend class cDhsClientDataset;
    friend class cDhsGetDataset;
    friend class cDhsPutDataset;
    friend class cDhsClientCommand;
    friend class cDhs;

    public:
	inline		cDhsTag() { dtTag = DHS_TAG_NULL; };
	inline bool	isDone( DHS_STATUS & );
	inline cDhsAvList
			responseGet( int, DHS_STATUS & );
	inline DHS_CMD_STATUS
			status( DHS_STATUS & );
	inline DHS_CMD_STATUS
			status( char *&, DHS_STATUS & );
	inline DHS_TAG	&tag( void );
	inline void	wait( DHS_STATUS & );
    protected:

    private:
	DHS_TAG		dtTag;		// Tag associated with the command
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTag::isDone
//
// INVOCATION:
// isDone = putDataset->isDone( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Check to see if a request is complete
//
// DESCRIPTION:
// Execute the dhsTagDone function.
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

bool		cDhsTag::isDone
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( dhsTagDone( dtTag, &status ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTag::responseGet
//
// INVOCATION:
// avList = tag.responseGet( 0, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) respNum (int) Response number to return.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// (cDhsAvList) The attribute value list corresponding to the response.
//
// PURPOSE:
// Return the Avlist corresponding the the specified response
//
// If no response can be found, the status is returned set to DHS_S_NO_RESP.
//
// DESCRIPTION:
// Execute the dhsResponseGet function.
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

cDhsAvList	cDhsTag::responseGet
(
    int		respNum,	// (in)  The response number to return.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsAvList( dhsResponseGet( tag(), respNum, &status ) ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTag::status
//
// INVOCATION:
// reqStatus = request.status( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS) Function return status.
//
// FUNCTION VALUE:
// (DHS_CMD_STATUS) The status of the request.
//
// PURPOSE:
// Return the current status of a request
//
// DESCRIPTION:
// Execute the dhsStatus function.
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

DHS_CMD_STATUS	cDhsTag::status
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( dhsStatus( tag(), NULL, &status ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTag::status
//
// INVOCATION:
// char		*respString;
// reqStatus = request.status( respString, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) status (char *&) Response string for the dataset.
// (!) status (DHS_STATUS) Function return status.
//
// FUNCTION VALUE:
// (DHS_CMD_STATUS) The status of the request.
//
// PURPOSE:
// Return the current status and response string for a request
//
// DESCRIPTION:
// Execute the dhsStatus function.
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

DHS_CMD_STATUS	cDhsTag::status
(
    char	*&respString,	// (out) Response string for the command.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( dhsStatus( tag(), &respString, &status ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTag::tag
//
// INVOCATION:
// tag = command.tag()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_TAG) The tag associated with the command.
//
// PURPOSE:
// Accessor function to return the tag
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

DHS_TAG		&cDhsTag::tag
(
    void
)
{
    return( dtTag );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsTag::wait
//
// INVOCATION:
// tag.wait( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Wait until the transfer is complete
//
// DESCRIPTION:
// Use the dhsWait function to wait until the transfer is complete.
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

void		cDhsTag::wait
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( tag() != DHS_TAG_NULL )
    {
	dhsWait( 1, &tag(), &status );
    }
}
#endif // DHSPPCLIENT_H
