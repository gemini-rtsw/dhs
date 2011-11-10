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
// dhs++/h/dhs++Dhs.H
//
// PURPOSE:
// Header file defining the classes used by all applications using the
// dhs++ library.
//
// CLASSES:
// cDhs - This class represents the whole DHS library.
// cDhsError - This class contains information about an error that occured
//		in the DHS library.
// cDhsHandler - This class contains methods and data common to all handlers.
// cDhsErrorHandlerBase - This class is the base class for the cDhsHandler
//		class.
// cDhsErrorHandler - This class is created to handle asyncronous errors from 
//		the dhs	library.
// cDhsAttrib - This class represents an attribute from an attribute 
//		value list.
// cDhsAvList - This class represents an attribute value list.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.22  1998/06/22 19:17:02  nhill
// Added method cDhsAvList::exportDataset. 
// Moved cDhs::stop to dhs.C
//
// Revision 1.21  1998/05/12 21:29:46  nhill
// Updated the documentation.
//
// Revision 1.20  1998/05/04 20:52:31  nhill
// Added a timeout value.
//
// Revision 1.19  1998/04/23 22:33:04  nhill
// Updated teh documentation.
// Added functions to read information from a configuration file.
//
// Revision 1.18  1998/04/02 19:42:16  nhill
// Moved class cDhsAvlist and its methods from dhs++Command.H.
//
// Revision 1.17  1998/03/05 23:47:44  nhill
// Added method enable to class cDhsHandler. Made function disable
// public.
//
// Revision 1.16  1998/02/24 01:12:02  nhill
// Added functions threadJoin, threadCreate and threadDestroy.
// Added a constructor for the cDhsHandler class.
//
// Revision 1.15  1997/12/01 21:40:56  cockayne
// Modified to suit Rational Rose analyzer.
//
// Revision 1.14  1997/09/22 23:53:49  nhill
// Added a destructor for the cDhsErrorHandler class.
//
// Revision 1.13  1997/09/18 17:45:44  nhill
// Improved the error display in the checkDhs macro.
//
// Revision 1.12  1997/08/29 22:34:03  nhill
// Added a null virtual destructor for the cDhsError class.
//
// Revision 1.11  1997/07/14 23:22:18  nhill
// Backed out of the r1.10 changes.
//
// Revision 1.9  1997/07/10 19:04:15  nhill
// Changed the debugLevel function to return the debug level.
//
// Revision 1.8  1997/06/12 16:56:05  nhill
// Changed the error checking macros to take a complete statement as the last
// argument instead of just a return value.
//
// Revision 1.7  1997/05/13 17:19:57  nhill
// Added the broadcast method to the cDhs class.
//
// Revision 1.6  1997/04/24 16:56:39  nhill
// Added "const" keywords to function prototypes.
//
// Revision 1.5  1997/04/15 20:14:35  nhill
// Added cDhsAttrib::operator==.
// Added a function to get messages from the dhs library.
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

#ifndef DHSPPDHS_H
#define	DHSPPDHS_H

#define	checkDhs( fn, dhsStatus, status, ret )				\
	fn;								\
	if ( dhsStatus != DHS_S_SUCCESS )				\
	{								\
	    const char *msg;						\
	    DHS_STATUS	s( DHS_S_SUCCESS );				\
	    status.E_DHS( status, #fn );				\
	    while ( s == DHS_S_SUCCESS )				\
	    {								\
		msg = cDhs::message( s );				\
		if ( s == DHS_S_SUCCESS )				\
		{							\
		    status += msg;					\
		    cDhs::messageClear( s );				\
		}							\
	    }								\
	    status.display();						\
	    ret;							\
	}


#include <dhs.h>

//
//***********************************************************************
//+
// CLASS NAME:
// cDhs
//
// PUBLIC METHODS: 
// (None) cDhs		- Constructor.
// (None) ~cDhs		- Destructor.
// (void) broadcast	- Broadcast to the dhs event loop condition variable.
// (static void)config	- Gets DHS library config info from the configuration 
//			  file.
// (void) debugLevel	- Set the debug level of the DHS library.
// (char *) identity	- Return the imp identity associated with this
//			  application.
// (char *) message	- Return the oldest message in the message queue.
// (void) messageClear	- Delete the oldest message from the message queue.
// (int) numConnect	- Return the number of connections allowed by this
//			  application.
// (void) start		- Start the dhs library event loop.
// (void) startThread	- Start the DHS library event loop in a thread.
// (void) stop		- Stop the DHS library event loop.
//
// PURPOSE:
// This class is instantiated to represent the DHS library.  Only one
// object of this class should be created, and the DHS library is 
// intialized in the constructor, and shutdown in the destructor.
//
//-
//***********************************************************************
//

class cDhs
{
    public:
			cDhs( const char *, int, 
				DHS_DEBUG_LEVEL = DHS_DEBUG_OFF );
			cDhs( DHS_DEBUG_LEVEL debugLevel = DHS_DEBUG_OFF )
			{
			    commonConst( debugLevel );
			}
			~cDhs( void );
	static void	 broadcast( void );
	static void	config( cDhsStatus & );
	static inline DHS_DEBUG_LEVEL
			debugLevel( DHS_DEBUG_LEVEL, DHS_STATUS & );
	inline static char
			*identity( void ) { return( dIdentity ); };
	static inline const char *
			message( DHS_STATUS &, DHS_ERR_LEVEL &, DHS_STATUS & );
	static inline const char *
			message( DHS_STATUS & );
	static inline void
			messageClear( DHS_STATUS & );
	inline static int
			numConnect( void ) { return( dNumConnect ); };
	static inline void	
			start( DHS_STATUS & );
	static inline DHS_THREAD	
			startThread( DHS_STATUS & );
	static void	stop( DHS_STATUS & );
    protected:
    private:
	static void	getCallback( DHS_CONNECT, DHS_TAG, char *, 
				DHS_BD_GET_TYPE, DHS_CMD_STATUS, char *, 
				DHS_AV_LIST, void *, int, void * );
	static boolean	identity( int, char ** );
	static boolean	imp( int, char ** );
	void		commonConst( DHS_DEBUG_LEVEL );
	static void	putCallback( DHS_CONNECT, DHS_TAG, DHS_CMD_STATUS, 
				char *, char *, void * );
	static void	responseCallback( DHS_CONNECT, DHS_TAG, char *, 
				DHS_CMD_STATUS, char *, DHS_AV_LIST, void * );

	static char	*dIdentity;	// Name for this server.
	static int	dNumConnect;	// Maximum number of connections.
	static int	dTimeout;	// Timeout for this server.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// debugLevel
//
// INVOCATION:
// cDhs::debugLevel( debugLevel, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) debugLevel (DHS_DEBUG_LEVEL) The new debug level.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the DHS library debug level
//
// DESCRIPTION:
// Call the dhsDebugLevel function.
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

DHS_DEBUG_LEVEL	cDhs::debugLevel
(
    DHS_DEBUG_LEVEL		// (in)  New debug level.
		debugLevel,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( dhsDebugLevel( debugLevel, &status ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::message
//
// INVOCATION:
// cDhs::message( errStat, errLevel, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) errStat (DHS_STATUS &) Status of the error message.
// (<) errLevel (DHS_ERR_LEVEL &) Error level of the error message.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// (char *) The error message.
//
// PURPOSE:
// Return the oldest error reported by the DHS library.
//
// A status of DHS_S_NO_MESSAGE will be returned if there are no messages.
//
// DESCRIPTION:
// Execute the dhsMessage function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs::messageClear
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

const char	*cDhs::message
(
    DHS_STATUS	&errStat,	// (out) Status of the error message.
    DHS_ERR_LEVEL		// (out) Error level of the error message.
    		&errLevel,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( dhsMessage( &errStat, &errLevel, &status ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::message
//
// INVOCATION:
// cDhs::message( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// (char *) The error message.
//
// PURPOSE:
// Return the oldest error reported by the DHS library.
//
// A status of DHS_S_NO_MESSAGE will be returned if there are no messages.
//
// DESCRIPTION:
// Execute the dhsMessage function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs::messageClear
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

const char	*cDhs::message
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( dhsMessage( NULL, NULL, &status ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhs::messageClear
//
// INVOCATION:
// cDhs::messageClear( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Remove the oldest message from the DHS library message queue
//
// DESCRIPTION:
// Execute the dhsMessageClear function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs::message
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhs::messageClear
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsMessageClear( &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// start
//
// INVOCATION:
// cDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Run the DHS library event loop.
//
// DESCRIPTION:
// Execute the dhsEventLoop function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs::startThread
// - cDHs::stop
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhs::start
(
    DHS_STATUS	&status			// (mod)  Function return status.
)
{
    dhsEventLoop( DHS_ELT_WAIT, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// startThread
//
// INVOCATION:
// cDhsObj.startThread( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Run the DHS library event loop in a separate thread.
//
// DESCRIPTION:
// Execute the dhsEventLoop function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhs::startThreaded
// - cDHs::stop
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

DHS_THREAD	cDhs::startThread
(
    DHS_STATUS	&status			// (mod)  Function return status.
)
{
    DHS_THREAD	threadId;		// The event loop thread id.


    dhsEventLoop( DHS_ELT_THREADED, &threadId, &status );
    return( threadId );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsError
//
// PUBLIC METHODS: 
// ~cDhsError - Destructor for the class.
// (DHS_CONNECT) connect - Return the connection associated with the error.
// (DHS_ERR_LEVEL) level - Return the error level associated with the error.
// (char *) message - Return the message string associated with the error.
// (DHS_STATUS) status - Return the error number associated with the error.
// (DHS_TAG) tag - Return the command tag associated with the error.
// (pthread_t) thread - Return the thread id associated with the error.
// (void *) userData - Return the user data pointer associated with the error.
//
// PURPOSE:
// This class contains information about an error that occured in the DHS 
// library
//
// An application using this class must define an "error" method to process
// the error.
//
//-
//***********************************************************************
//

class cDhsError
{
    friend class cDhsErrorHandlerBase;

    public:
	inline virtual		~cDhsError( void ){};
	inline DHS_CONNECT	connect( void );
	inline DHS_ERR_LEVEL	level( void );
	inline char		*message( void );
	inline DHS_STATUS	status( void );
	inline DHS_TAG		tag( void );
	inline pthread_t	thread( void );
	inline void		*userData( void );
    protected:
    private:
	virtual void		error( void );
	void			init( DHS_CONNECT, DHS_STATUS, DHS_ERR_LEVEL, 
					char *, DHS_TAG, pthread_t, void * );

	DHS_CONNECT	deConnect;	// The connection id.
	DHS_ERR_LEVEL	deLevel;	// The Error level.
	char		*deMessage;	// The error message.
	DHS_STATUS	deStatus;	// Status of the error.
	DHS_TAG		deTag;		// Command tag.
	pthread_t	deThread;	// Thread message occured in.
	void		*dePUserData;	// User data pointer.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::connect
//
// INVOCATION:
// connect = err.connect();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_CONNECT) The connection associated with the error.
//
// PURPOSE:
// Accessor function to return the connection number.
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

DHS_CONNECT	cDhsError::connect
(
    void
)
{
    return( deConnect );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::level
//
// INVOCATION:
// level = err.level();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_ERR_LEVEL) The error level associated with the error.
//
// PURPOSE:
// Accessor function to return the error level.
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

DHS_ERR_LEVEL	cDhsError::level
(
    void
)
{
    return( deLevel );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::message
//
// INVOCATION:
// message = err.message();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (char *) The message associated with the error.
//
// PURPOSE:
// Accessor function to return the message string.
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

char		*cDhsError::message
(
    void
)
{
    return( deMessage );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::status
//
// INVOCATION:
// status = err.status();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_STATUS) The error status associated with the error.
//
// PURPOSE:
// Accessor function to return the error number.
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

DHS_STATUS	cDhsError::status
(
    void
)
{
    return( deStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::tag
//
// INVOCATION:
// tag = err.tag();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_TAG) The tag associated with the error.
//
// PURPOSE:
// Accessor function to return the tag.
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

DHS_TAG	cDhsError::tag
(
    void
)
{
    return( deTag );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::thread
//
// INVOCATION:
// thread = err.thread();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (pthread_t) The thread the error occured in.
//
// PURPOSE:
// Accessor function to return the thread number.
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

pthread_t	cDhsError::thread
(
    void
)
{
    return( deThread );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsError::userData
//
// INVOCATION:
// userData = err.userData();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (void *) The user data pointer  associated with the error.
//
// PURPOSE:
// Accessor function to return the user data pointer.
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

void		*cDhsError::userData
(
    void
)
{
    return( dePUserData );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsHandler
//
// PUBLIC METHODS: 
// cDhsHandler		- Constructor for the class.
// ~cDhsHandler		- Destructor.  Here only to make sure the correct
//			  destrutor is actually called.
// (void) disable	- Disable the handler.
// (void) enable	- Enable the handler.
// (void) forEach	- Execute a function for each handler object.
// (int)  threadJoin	- Wait for all handler threads to complete.
//
// PURPOSE:
// This is the parent of all handlers.
//
// It contains common data, but is mainly here to allow generic pointers
// to handlers.
//
//-
//***********************************************************************
//

class 	cDhsHandler
{
    public:
	typedef		void (*tForEachFn)( cDhsHandler * );
	typedef		void * (*tThreadFn)( void * );

			cDhsHandler( void );
	virtual		~cDhsHandler( void );
	void		disable( void );
	void		enable( void );
	static void	forEach( tForEachFn );
	static void	threadJoin( void );
    protected:
	inline bool	enabled( void )
			{
			    return( dhEnabled );
			}
	static void	threadCreate( tThreadFn, void * );
	static void	threadDestroy( void );

	DHS_CB_FN_PTR	dhOldCallback;
	static void	*dhPRwLock;	// Read/write lock to protect the
					// list of handlers.
    private:
	static void	*dhPHandlerList;// List of all handlers.
	bool		dhEnabled;	// True if the handle is enabled.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsErrorHandlerBase
//
// PUBLIC METHODS: 
// cDhsErrorHandlerBase - Constructor for the class.
// ~cDhsErrorHandlerBase - Destructor for the class.
// (cDhsError *) newError - Create a new error object.
//
// PURPOSE:
// This class only exists to move methods and data from the cDhsErrorHandler
// class into a none template class that can be shared by all instances of
// the cDhsErrorHandler class.
//
// SEE ALSO;
// - class cDhsErrorHandler
//
//-
//***********************************************************************
//

class cDhsErrorHandlerBase: public cDhsHandler
{
    public:
    protected:
		    	cDhsErrorHandlerBase( DHS_STATUS, DHS_ERR_LEVEL );
	virtual		~cDhsErrorHandlerBase( void );
	virtual cDhsError
			*newError( void ) = 0;
    private:
	static void 	errorCallback( DHS_CONNECT, DHS_STATUS, DHS_ERR_LEVEL, 
				char *, DHS_TAG, void * );
	
	DHS_STATUS	dehErrStatus;	// Status the handler is for.
	DHS_ERR_LEVEL	dehErrLevel;	// Error level the hander is for.
	static bool	dehCbInit;	// Is the callback initialized?
	static DHS_CB_FN_PTR		// The old callback function.
			dehOldCallback;
	cDhsErrorHandlerBase		// Previous error handler.
			*dehOldHandler;
	static void	*dehPHandlerList;// List of error handlers.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsErrorHandler
//
// PUBLIC METHODS: 
// cDhsErrorHandler - Constructor for the class.
// ~cDhsErrorHandler - Destructor for the class.
//
// PURPOSE:
// Created to handle error messages from the dhs library
//
// SEE ALSO:
// - class cDhsErrorHandlerBase
//-
//***********************************************************************
//

template <class userError>
class cDhsErrorHandler: public cDhsErrorHandlerBase
{
    public:
			cDhsErrorHandler( void ): 
				cDhsErrorHandlerBase( DHS_S_NULL, DHS_EL_NULL )
				{};
			cDhsErrorHandler( DHS_STATUS errStatus ):
				cDhsErrorHandlerBase( errStatus, DHS_EL_NULL )
				{};
			cDhsErrorHandler( DHS_ERR_LEVEL errLevel ):
				cDhsErrorHandlerBase( DHS_S_NULL, errLevel )
				{};
	virtual		~cDhsErrorHandler( void );
    protected:
    private:
	cDhsError	*newError( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsErrorHandler::~cDhsErrorHandler
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
// Destructor for an error handler
//
// DESCRIPTION:
// Lock the error handler mutexes to ensure it is not in use
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

template <class userError>
		cDhsErrorHandler<userError>::~cDhsErrorHandler
(
    void
)
{
    disable();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsErrorHandler::newError
//
// INVOCATION:
// obj = newError();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cDhsError *) Pointer to the error object to use.
//
// PURPOSE:
// Return a pointer to an error object to use to handle an error.
//
// DESCRIPTION:
// Return a pointer to the errorObj member.
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

template <class userError>
cDhsError	*cDhsErrorHandler<userError>::newError
(
    void
)
{
    return( new userError );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsAttrib
//
// PUBLIC METHODS: 
// (None) cDhsAttrib - Constructor.
// (bool) operator== - Compare two attributes.
// (void) info - Return information about the attribute.
// (const char *) typeMap - Turn a DHS type into a string.
// (DHS_DATA_TYPE) typeMap - Turn a string to a DHS type.
//
// PURPOSE:
// Contains information about an attribute in a av list or dataset.
//
// Note the "info" functions should be template member functions, with the
// "void **" parameter replaced by the template type.
//
//-
//***********************************************************************
//

class	cDhsAttrib
{
    friend class cDhsAvList;

    public:
	inline		cDhsAttrib( void );
	inline		cDhsAttrib( DHS_AV_ID );
	inline bool	operator==( DHS_AV_ID ) const;
	inline void	info( char *&, DHS_DATA_TYPE &, DHS_STATUS & ) const;
	inline void	info( int &, unsigned long[], void **, 
				DHS_STATUS & ) const;
	inline void	info( void **, DHS_STATUS & ) const;
	inline void	info( char *&, DHS_DATA_TYPE &, int &, unsigned long[], 
				void **, DHS_STATUS & ) const;
	static const char	
			*typeMap( DHS_DATA_TYPE );
	static DHS_DATA_TYPE	
			typeMap( const char* );
    protected:
    private:
	DHS_AV_ID	daId;		// DHS attribute id.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::cDhsAttrib
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
// Constructor for an attirubte
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

		cDhsAttrib::cDhsAttrib
(
    void
)
{
    daId = DHS_AV_LIST_NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::cDhsAttrib
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) avId (DHS_AV_ID) The initial attribute id.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for an attirubte
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

		cDhsAttrib::cDhsAttrib
(
    DHS_AV_ID	avId		// (in)  The initial attribute id.
)
{
    daId = avId;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::operator==
//
// INVOCATION:
// isEqual = ( avlist == avId );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) avId (DHS_AV_ID) The attribute id to compare.
//
// FUNCTION VALUE:
// (bool) True if the attribute id == the objects attribute id.
//
// PURPOSE:
// Compare an attribute id to the attribute id stored by the class
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

bool		cDhsAttrib::operator==
(
    DHS_AV_ID	avId		// (in)  The attribute to compare.
) const
{
    return( avId == daId );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::info
//
// INVOCATION:
// char			*name;
// DHS_DATA_TYPE	type;
// int			ndims;
// unsigned long	dims[7];
// void			*pData;
// DHS_STATUS		status;
// atrib.info( name, type, ndims, dims, pData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) name (char *&) The attribute name.
// (<) type (DHS_DATA_TYPE &) The data type.
// (<) ndims (int &) The number of dimensions of the data.
// (<) dims (unsigned long []) The dimensions of the array.
// (<) pData (void **) Pointer to the data.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get all available information about an attribute
//
// DESCRIPTION:
// Execute the dhsAvInfo function.
//
// Note that this should be a template member function, with the pData
// argument replaced with a reference to a pointer to the template type.
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

void		cDhsAttrib::info
(
    char	*&name,		// (out) The attribute name.
    DHS_DATA_TYPE		// (out) The data type.
		&type,
    int		&ndims,		// (out) The number of dimensions of the data.
    unsigned long		// (out) The dimensions of the data.
		*dims,
    void	**pData,	// (out) Pointer to the data.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsAvInfo( daId, &name, &type, &ndims, dims, pData, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::info
//
// INVOCATION:
// char			*name;
// DHS_DATA_TYPE	type;
// DHS_STATUS		status;
// atrib.info( name, type, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) name (char *&) The attribute name.
// (<) type (DHS_DATA_TYPE &) The data type.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get the name and type of an attribute
//
// DESCRIPTION:
// Execute the dhsAvInfo function.
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

void		cDhsAttrib::info
(
    char	*&name,		// (out) The attribute name.
    DHS_DATA_TYPE		// (out) The data type.
		&type,
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsAvInfo( daId, &name, &type, NULL, NULL, NULL, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::info
//
// INVOCATION:
// int			ndims;
// unsigned long	dims[7];
// void			*pData;
// DHS_STATUS		status;
// atrib.info( ndims, dims, pData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) ndims (int &) The number of dimensions of the data.
// (<) dims (unsigned long []) The dimensions of the array.
// (<) pData (void **) Pointer to the data.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get information about the data in an attribute
//
// DESCRIPTION:
// Execute the dhsAvInfo function.
//
// Note that this should be a template member function, with the pData
// argument replaced with a reference to a pointer to the template type.
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

void		cDhsAttrib::info
(
    int		&ndims,		// (out) The number of dimensions of the data.
    unsigned long		// (out) The dimensions of the data.
		*dims,
    void	**pData,	// (out) Pointer to the data.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsAvInfo( daId, NULL, NULL, &ndims, dims, pData, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAttrib::info
//
// INVOCATION:
// void			*pData;
// DHS_STATUS		status;
// atrib.info( pData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) pData (void **) Pointer to the data.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get a pointer to the data
//
// DESCRIPTION:
// Execute the dhsAvInfo function.
//
// Note that this should be a template member function, with the pData
// argument replaced with a reference to a pointer to the template type.
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

void		cDhsAttrib::info
(
    void	**pData,	// (out) Pointer to the data.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsAvInfo( daId, NULL, NULL, NULL, NULL, pData, &status );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsAvlist
//
// PUBLIC METHODS: 
// (void) cDhsAvList - Constructor.
// (void) ~cDhsAvList - Destructor.
// (cDhsAvList &) operator= - Assign one avlist to another.
// (void) add - Add an attribute to the av list.
// (cDhsAttrib) find - Find an attribute in the av list.
// (bool) isNull - Test an avlist to see if it is null.
// (void) print - Print the av list to standard out.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
//-
//***********************************************************************
//

class	cDhsAvList
{
    friend class cDhsServerCommand;
    friend class cDhsGetRequest;
    friend class cDhsPutRequest;

    public:
	inline			cDhsAvList( void );
	inline			cDhsAvList( DHS_AV_LIST );
	inline			cDhsAvList( const cDhsAvList & );
	inline			~cDhsAvList( void );
	cDhsAvList		&operator=( const cDhsAvList &avlist );
	inline void		add( const char *, DHS_DATA_TYPE, int, 
					const unsigned long *, const void *, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, char, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, 
					    unsigned char, DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, short, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, 
					unsigned short, DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, int, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, unsigned int, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, long, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, unsigned long, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, double, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, const char *, 
					DHS_STATUS & );
	inline void		add( const char *, DHS_DATA_TYPE, 
					const cDhsAvList &, DHS_STATUS & );
        // XXX allan: 2002/11/18: export is a reserved word: renamed to exportDataset
	void			exportDataset( char *, DHS_STATUS & ) const;
	inline cDhsAttrib	find( const char *, DHS_STATUS & ) const;
	inline cDhsAttrib	find( int, DHS_STATUS & ) const;
	inline bool		isNull( void ) const;
	inline void		print( DHS_STATUS & ) const;
    protected:
	DHS_AV_LIST	dalAvList;	// The av list id.
	bool		dalFreeIt;	// Free the av list when done?
    private:
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::cDhsAvList
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
// Constructor for an attribute value list
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

		cDhsAvList::cDhsAvList
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    dalAvList = dhsAvListNew( &status );

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
    dalFreeIt = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::cDhsAvList
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) avList (DHS_AV_LIST) The initial av list id.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for an attribute value list
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

		cDhsAvList::cDhsAvList
(
    DHS_AV_LIST	avList		// (in)  The initial av list.
)
{
    dalAvList = avList;
    dalFreeIt = false;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::cDhsAvList
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) avlist (const cDhsAvList &) The source av list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy constructor for an attribute value list
//
// DESCRIPTION:
// Make a copy of the av list with the dhsAvListCopy function.
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

		cDhsAvList::cDhsAvList
(
    const cDhsAvList		// (in)  The source av list.
		&avList
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    dalAvList = dhsAvListCopy( avList.dalAvList, &status );
    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }

    dalFreeIt = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::~cDhsAvList
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
// Destructor for an attribute value list.
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

		cDhsAvList::~cDhsAvList
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    if ( dalFreeIt )
    {
	dhsAvListFree( dalAvList, &status );

	if ( status != DHS_S_SUCCESS )
	{
	    throw status;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::add
//
// INVOCATION:
// char			*name;
// DHS_DATA_TYPE	type;
// int			ndims;
// int			dims[7];
// <type>		array[dims[0][ ...;
// DHS_STATUS		status;
// avList.add( name, type, ndims, dims, array, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (const char *) Attribute name.
// (>) type (DHS_DATA_TYPE) Type to use for storing the data.
// (>) ndims (int) Number of dimensions of the data array.
// (>) dims (int[]) Dimensions of the array.
// (>) array (const void *) Pointer to the data array.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add an attribute whose value is an array to an attribute value list
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

void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    int		ndims,		// (in)  Number of dimensios of the array.
    const ulong	dims[],		// (in)  Dimensions of the array.
    const void	*array,		// (in)  pointer to the data array.
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, ndims, dims, array, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::add
//
// INVOCATION:
// char		*name;
// DHS_DATA_TYPE type;
// t		value;
// DHS_STATUS	status;
// avList.add( name, type, value, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Attribute name.
// (>) type (DHS_DATA_TYPE) Type to use for storing the data.
// (>) value (T) The data value.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add an attribute whose value is a scalar to an attribute value list
//
// DESCRIPTION:
// This function should be implemented as a template member function,
// but since the compiler currently does not support this, each of the
// possible incarnations has been implemented.
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

void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    char	value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}

void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    unsigned char		// (in)  The data value
		value,
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    short	value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    ushort	value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    int		value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    unsigned int		// (in)  The data value
		value,
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    long	value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    ulong	value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    double	value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    const char	*value,		// (in)  The data value
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value, &status );
}
void		cDhsAvList::add
(
    const char	*name,		// (in)  Attribute name.
    DHS_DATA_TYPE		// (in)  Storage type for the data.
    		type,
    const cDhsAvList		// (in)  The data value
		&value,
    DHS_STATUS	&status		// (mod) function return status.
)
{
    dhsAvAdd( dalAvList, name, type, 0, NULL, value.dalAvList, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::find
//
// INVOCATION:
// attrib = avList.find( "attrib name", status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (const char *) Name of the attribute.
// (!) status (DHS_STATUS &) function return status.
//
// FUNCTION VALUE:
// (cDhsAttrib) The attribute located.
//
// PURPOSE:
// Locate an attribute in an attribute value list given its name
//
// This function returns DHS_S_NO_ATRRIB if the attribute could not be found.
//
// DESCRIPTION:
// Execute the dhsAvFind function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhsAvList::find
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cDhsAttrib	cDhsAvList::find
(
    const char	*name,		// (in)  Name of the attribute to find.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    DHS_AV_ID	avId;		// DHS library attribute id.


    avId = dhsAvFind( dalAvList, name, &status );

    return( cDhsAttrib( avId ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::find
//
// INVOCATION:
// attrib = avList.find( i, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) index (int) Index of the attribute.
// (!) status (DHS_STATUS &) function return status.
//
// FUNCTION VALUE:
// (cDhsAttrib) The attribute located.
//
// PURPOSE:
// Locate an attribute in an attribute value list given its index
//
// This function returns DHS_S_NO_ATRRIB if the attribute could not be found.
//
// DESCRIPTION:
// Execute the dhsAvIndex function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - cDhsAvList::find
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cDhsAttrib	cDhsAvList::find
(
    int		index,		// (in)  Index of the attribute to find.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    DHS_AV_ID	avId;		// DHS library attribute id.


    avId = dhsAvIndex( dalAvList, index, &status );

    return( cDhsAttrib( avId ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::isNull
//
// INVOCATION:
// isItNull = avList.isNull();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (bool) True if the avList is NULL, false otherwise.
//
// PURPOSE:
// Test to see if the av list is null
//
// DESCRIPTION:
// Compare the av list id DHS_AV_LIST_NULL.
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

bool		cDhsAvList::isNull
(
    void
) const
{
    if ( dalAvList == DHS_AV_LIST_NULL )
    {
	return( true );
    }
    else
    {
	return( false );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsAvList::print
//
// INVOCATION:
// avList.print();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Print an attribute value list to stdout
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

void		cDhsAvList::print
(
    DHS_STATUS	&status		// (mod)  Function return status.
) const
{
    if ( dalAvList != DHS_AV_LIST_NULL )
    {
	dhsAvListPrint( dalAvList, &status );
    }
}
#endif // DHSPPDHS_H
