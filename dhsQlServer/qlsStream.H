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
// dhsQlServer/h/qlsStream.H
//
// PURPOSE:
// Contains the interface to the cQlsStream class.
//
// CLASSES:
// cQlsStream - Contains a list of streams for the DHS Quick Look Server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2001/03/27 20:41:37  nhill
// Added the qsDeleting data member to the cQlsSubscriber structure.
// Made the disconnect member of the cQlsSubscriber structure non-static.
//
// Revision 1.7  1998/06/16 22:15:30  nhill
// Added the connection callback routine.
//
// Revision 1.6  1998/04/27 21:05:26  nhill
// Added the diconnect and test methods to class cQlsSubscriber.
//
// Revision 1.5  1997/11/30 21:23:16  nhill
// Changed the prototype of function addStreams.
//
// Revision 1.4  1997/10/30 18:09:10  nhill
// Removed a redundant definition of method qlStream.
// Removed an unused definition of method cQlsSubscriber::unsubscribe.
//
// Revision 1.3  1997/10/22 17:48:39  nhill
// Added a new argument to the cQlsStreams ref and deRef methods to allow
// public datasets to be treated differently from datasets requested for
// a specific qlt.
//
// Revision 1.2  1997/08/20 21:21:02  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSSTREAM_H
#define	QLSSTREAM_H

#include <string>
#include <map>

#include "genRwLock.H"

class	cDhsAvList;
class	cQlsSubscriber;

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsStream
//
// PUBLIC METHODS: 
// (None) cQlsStream - Constructor for the class.
// (None) ~cQlsStream - Destructor for the class.
// (static void) addStreams - Add a streams attribute to an DHS++ library 
//		av list.
// (static void) config - Read a list of streams from the configuration file.
// (int) deRef - Decrement the reference counter for the stream.
// (static void) exit - Delete all existing cQlsStream objects.
// (cQlsStream *) find - Find a cQlsStream object by stream name.
// (static void) forEach - Execute a function on every existing cQlsStream 
//		object.
// (static void) listRLock - Lock the lock protecting the class.
// (static void) listUnlock - Unlock the lock protecting the class.
// (static void) listWLock - Lock the lock protecting the class.
// (char *) name - Return the name of the stream.
// (static boolean) qlStream - Process a qlStream line from the configuration
//		file.
// (int) ref - Increment the reference counter for the stream.
//
// PUBLIC TYPES:
// tStreamList - STL map of quick look streams indexed by stream name.
// iStreamList - STL iterator into the tStreamList type.
// tSubscriberList - STL map of quick look stream subscribers indexed by
//		DHS library connection id.
// iSubscriberList - STL iterator into the tSubscriberList type.
// tForEachFn - Function to be passed to the cQlsStream::foreach method.
//
// PUBLIC DATA:
// None.
//
// PURPOSE:
// Stores information about the quick look streams known to the QLS.
//
//-
//***********************************************************************
//

class cQlsStream 
{
    public:
	typedef map < string, cQlsStream *, less<string> >
			tStreamList;
	typedef tStreamList::iterator 
			iStreamList;
	typedef map < DHS_CONNECT, cQlsSubscriber *, less<DHS_CONNECT> >
			tSubscriberList;
	typedef tSubscriberList::iterator
			iSubscriberList;
	typedef 	void (*tForEachFn)( cQlsStream &, void * );

			cQlsStream( const char * );
	virtual		~cQlsStream( void );
	static void	addStreams( cDhsAvList &, bool, cQlsStatus & );
	static void	config( cQlsStatus & );
	inline int	deRef( bool isPublic = true )
			{
			    if ( isPublic )
			    {
				qlsPubRefCount --;
			    }
			    qlsRefCount --;
			    return( qlsRefCount );
			}
	static void	exit( cQlsStatus & );
	static cQlsStream
			*find( const char *, cQlsStatus & );
	static void	forEach( tForEachFn, void *, cQlsStatus & );
	inline static void
			listRLock( void )
			{
			    qlsRwLock.rLock();
			}
	inline static void
			listUnlock( void )
			{
			    qlsRwLock.unlock();
			}
	inline static void
			listWLock( void )
			{
			    qlsRwLock.wLock();
			}
	inline const char	
			*name( void ) const
			{
			    return( qlsName );
			}
	inline int	ref( boolean isPublic = true  )
			{
			    if ( isPublic )
			    {
				qlsPubRefCount ++;
			    }
			    qlsRefCount ++;
			    return( qlsRefCount );
			}
	void		subscribe( cQlsSubscriber &, cQlsStatus & );
	void		unSubscribe( cQlsSubscriber &, cQlsStatus & );
    protected:
    private:
	static boolean	qlStream( int, char ** );

	char		*qlsName;	// Name of the quick look stream.
	int		qlsRefCount;	// Reference counter.
	int		qlsPubRefCount;	// Public reference counter.
	static cRwLock	qlsRwLock;	// Lock to protect the list of streams.
	static tStreamList		// A list of all quick look streams.
			qlsStreamList;
	tSubscriberList qlsSubscriberList;
					// A list of all subscribers.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsSubscriber
//
// PUBLIC METHODS: 
// (None) cQlsSubscriber - Constructor for the class.
// (None) ~cQlsSubscriber - Destructor for the class.
// (cDhsConnection &) connect - Return the dhs++ connection associated with
//		the subscriber.
// (void) disconnect - Called when a subscriber disconnects.
// (void) subscribe - Subscribe the subscriber to a stream.
// (cQlsSubscriber::tStreamList &) streamList - Return the list of all
//		streams to which the subscriber is subscribed.
// (void) test - Test the subscriber and its connection.
// (void) unSubscribe - Unsubscribe the subscriber from a stream.
// (void) unSubscribeAll - Unsubscribe the subscriber from all streams.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Common base class for all subscribers to quick look streams
//
//-
//***********************************************************************
//

class	cQlsSubscriber: public cMutex
{
    public:
	inline		cQlsSubscriber( DHS_CONNECT connection ): 
				qsConnection( connection )
			{
			    qsConnection.callbackSet( connectCallback, this );
			    qsDeleting = false;
			}
	virtual		~cQlsSubscriber( void );
	inline cDhsConnection
                        &connect( void )
                        {
                            return( qsConnection );
                        }
	void		disconnect( DHS_CONNECT, cQlsStatus & );
	inline cQlsStream::tStreamList	
			&streamList( void )
			{
			    return( qsStreamList );
			}
	void		subscribe( char *, cQlsStatus & );
	virtual void	test( void *, cQlsStatus & );
	void		unSubscribeAll( cQlsStatus & );
    protected:
    private:
	static void	connectCallback( cDhsConnection *, void * );

	cDhsConnection	qsConnection;	// Connection to the QLT.
	cQlsStream::tStreamList		// List of all subscribed streams.
			qsStreamList;
	bool		qsDeleting;	// This flag is set when the 
					// subscriber is in the process 
					// of being deleted. This prevents 
					// multiple deletions of the same 
					// subscriber structure.
};
#endif // QLSSTREAM_H
