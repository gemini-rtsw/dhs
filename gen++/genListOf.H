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
// gen++/h/genListOf.H
//
// PURPOSE:
// This file contains the interface and implementation of the cListof
// class
//
// CLASSES:
// cListOf - Template class to maintain a list of all objects in a base 
// class
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.8  1999/06/16 22:21:25  nhill
// Added function listRLock.
//
// Revision 1.7  1998/06/16 21:56:09  nhill
// Added methods listWLock and listUnlock.
//
// Revision 1.6  1998/05/12 20:54:07  nhill
// Bug fix.
//
// Revision 1.5  1998/05/04 23:07:42  nhill
// Added a compiler flag to disable the findCreate function and all of the
// problems it causes.
//
// Revision 1.4  1998/02/24 01:25:05  nhill
// Added begin and end methods to the list.
//
// Revision 1.3  1997/12/01 23:13:44  nhill
// Bug fix.
//
// Revision 1.2  1997/12/01 19:30:22  nhill
// Updated the documentation.
//
// Revision 1.1  1997/11/30 21:07:26  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GENLISTOF_H
#define	GENLISTOF_H

#include <map>

#include "genRwLock.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cListOf
//
// PUBLIC METHODS: 
// cListOf - Constructor for the class.
// ~cListOf - Destructor for the class.
// (void) deleteAll - Delete all objects of type cUserClass.
// (cUserClass *) find - locate an object in the list of all objects.
// (cUserClass *) findCreate - locate an object in the list of all objects.
//		Create a new object on the heap if an existing one cannot
//		be found.
// (void) forEach - Execute a function for each object in the class.
// (cIndex) index - Return the value of the index for an object.
// (void) listRLock - Get a reader lock on the list.
// (void) listWLock - Get a writer lock on the list.
// (void) listUnlock - Get a writer lock on the list.
//
// PARAMETERS:
// cUserClass - The derived class type.
// cIndex - The type of the index for the list of all objects.
// cStatus - The function return status type used by the foreach function.
// 
//
// PURPOSE:
// This class keeps a list of all objects in a derived class
//
//
// USAGE:
// class derivedClass: public cListOf< derivedClass, indexClass, statusClass >
// { ... }
//
// - The cIndex class must have an assignment operator and copy constructor.
// - The cindex class must have an < operator acceptable to the STL map
//   class.
// - If the findCreate function is used, the cUserClass must have a constructor
//   that takes the index class as an argument.
// - All constructors for the cUserClass must call the constructor for
//   the cListOf class.
//
// DEFICIENCIES:
// The cStatus template parameter would be better implemented using default
// template arguments so that it doesn't have to be specified for applications
// that dont' require the foreach method. It would be event better if the
// foreach function could be implemented as a template member function, 
// in which case the cStatus template argument could be removed from the
// class definition.
//
//-
//***********************************************************************
//

template < class cUserClass, class cIndex, class cStatus >
class 	cListOf
{
    public:
	typedef	map < cIndex, cUserClass *, less< cIndex > >
			tObjectList;
	typedef tObjectList::iterator
			iObjectList;
	typedef	void (cUserClass::*tForEachFn) ( void *, cStatus & );
	typedef void (cUserClass::*tLockFn) ( void );

			cListOf( const cIndex &, bool = true );
	virtual		~cListOf( void );
	inline static iObjectList	
			begin()
			{
			    return( loObjectList.begin() );
			}
	static void	deleteAll( void );
	inline static iObjectList	
			end()
			{
			    return( loObjectList.end() );
			}
	inline static cUserClass	
			*find( cIndex index, tLockFn fn = NULL )
			{
			    return( find( index, fn, false ) );
			}
#ifndef NO_FIND_CREATE
	inline static cUserClass	
			*findCreate( cIndex index, tLockFn fn = NULL )
			{
			    return( find( index, fn, true ) );
			}
#endif	// NO_FIND_CREATE
	static void	forEach( tForEachFn, void *, cStatus & );
	inline cIndex	&index( void )
			{
			    return( loObjectIndex );
			}
	static void	listRLock( void )
			{
			    loRwLock.rLock();
			}
	static void	listWLock( void )
			{
			    loRwLock.wLock();
			}
	static void	listUnlock( void )
			{
			    loRwLock.unlock();
			}
    protected:
    private:
			cListOf( cListOf & ) { return *this; };
	cListOf		&operator=( cListOf & ) { return *this; };
	static void	*deleteThread( void * );
	static cUserClass	
			*find( cIndex, tLockFn, bool );
	static void	*insertThread( void * );

	bool		loInList;
	cIndex		loObjectIndex;
	static 	tObjectList
			loObjectList;
	static cRwLock	loRwLock;
};
template< class cUserClass, class cIndex, class cStatus >
	map< cIndex, cUserClass *, less< cIndex > >
	cListOf< cUserClass, cIndex, cStatus >::loObjectList;
template < class cUserClass, class cIndex, class cStatus >
	cRwLock cListOf< cUserClass, cIndex, cStatus >::loRwLock;

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cIndex, class cStatus > 
//		cListOf< cUserClass, cIndex, cStatus >::cListOf
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) index (cIndex) Index for the new item.
// (>) addToList (bool) True if the object should be added to the list of
//		objects.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the class
//
// This function adds the object to the list of objects.
//
// DESCRIPTION:
// - copy the index value
// - Try to acquire a write lock on the object list.
// - If the lock is acquired, add the object to the list.
// - If the lock is not acquired, reserve a lock key and spawn a thread to
//   add the object to the list.
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

template< class cUserClass, class cIndex, class cStatus > 
		cListOf< cUserClass, cIndex, cStatus >::cListOf
(
    const cIndex	
		&index,		// (in)  The index for the object.
    bool	addToList	// (in)  True of the object is to be added 
    				//	 to the list.
)
{
    pair<int, cListOf *>	
		*arg;
    pthread_attr_t		// Thread attributes.
		attr;
    int		key;
    int		s;


    loObjectIndex = index;
    loInList = false;
    if ( addToList )
    {
	//
	// Try to get a write lock.
	//

	if ( loRwLock.tryWLock() )
	{
	    //
	    // If the write lock was acquired, add the object to the list.
	    //

	    loInList = true;
	    loObjectList[ index ] = (cUserClass *) this;
	    loRwLock.unlock();
	}
	else
	{
	    //
	    // If the lock could not be acquired, reserve a lock key and
	    // spawn a thread to add the object to the list.
	    //

	    key = loRwLock.preLock();
	    loInList = true;
	    arg = new pair<int, cListOf *> ( key, this );

	    pthread_attr_init( &attr );
	    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	    s = pthread_create( NULL, &attr, insertThread, arg );
	    pthread_attr_destroy( &attr );
	    
	    if ( s != 0 )
	    {
		throw "Failed to create a thread.";
	    }
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cIndex, class cStatus > 
//		cListOf< cUserClass, cIndex, cStatus >::~cListOf
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
// Destructor for the class
//
// Removes the object from the list.
//
// DESCRIPTION:
// - Try to acquire a lock on the rw lock.
// - if the lock was acquired, remove the object from the list.
// - If the lock was not acquired, spawn a new thread to remove the object
//   from the list.
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

template< class cUserClass, class cIndex, class cStatus > 
		cListOf< cUserClass, cIndex, cStatus >::~cListOf
(
    void
)
{
    pair<int, cIndex>	
		*arg;
    pthread_attr_t		// Thread attributes.
		attr;
    iObjectList	i;
    int		key;
    int		s;


    if ( loInList )
    {
	//
	// Try to get a write lock.
	//

	if ( loRwLock.tryWLock() )
	{
	    //
	    // If the lock was acquired, remove the object from the list.
	    //

	    i = loObjectList.find( loObjectIndex );
	    if ( i != loObjectList.end() )
	    {
		loObjectList.erase( i );
	    }
	    loRwLock.unlock();
	}
	else
	{
	    //
	    // If the lock was not acquired, reserve a lock key and 
	    // spawn a new thread to remove the object from the list.
	    //

	    key = loRwLock.preLock();
	    arg = new pair<int, cIndex> ( key, loObjectIndex );

	    pthread_attr_init( &attr );
	    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	    s = pthread_create( NULL, &attr, deleteThread, arg );
	    pthread_attr_destroy( &attr );
	    
	    if ( s != 0 )
	    {
		throw "Failed to create a thread.";
	    }
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cIndex, class cStatus >
//		cListOf< cUserClass, cIndex, cStatus >::deleteAll
//
// INVOCATION:
// derivedClass::deleteAll();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all objects for this class
//
// This assumes all objects are on the heap.
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

template< class cUserClass, class cIndex, class cStatus >
void		cListOf< cUserClass, cIndex, cStatus >::deleteAll
(
    void
)
{
    cUserClass	*pObject;
    iObjectList	i;


    loRwLock.wLock();

    for ( i = loObjectList.begin(); i!= loObjectList.end(); 
	    i = loObjectList.begin() )
    {
	pObject = (*i).second;
	loObjectList.erase( i );
	pObject->loInList = false;
	delete pObject;
    }
    
    loRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cIndex, class cStatus >
//		cListOf< cUserClass, cIndex, cStatus >::deleteThread
//
// INVOCATION:
// n.a. Called by pthread_create.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pArg (void *) The argment passed to pthread_create
//
// FUNCTION VALUE:
// (void *) not used. Only there to be compatable with pthread_create.
//
// PURPOSE:
// Remove an object from the list of objects in a separate thread.
//
// A pointer to the object and the object index are passed in a pair object
// passed through the argument.
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

template< class cUserClass, class cIndex, class cStatus >
void		*cListOf< cUserClass, cIndex, cStatus >::deleteThread
(
    void	*pArg		// (in)  The argment passed to 
    				//	 pthread_create
)
{
    pair<int, cIndex>	
		*arg;
    iObjectList	i;
    int		key;
    cIndex	index;

    arg = (pair<int, cIndex> *) pArg;
    key = arg->first;
    index = arg->second;
    delete arg;

    loRwLock.wLock( key );
    i = loObjectList.find( index );
    if ( i != loObjectList.end() )
    {
	loObjectList.erase( i );
    }
    loRwLock.unlock();

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cIndex, class cStatus >
//		cListOf< cUserClass, cIndex, cStatus >::find
//
// INVOCATION:
// pObject = derivedClass::findCreate( index );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) index (cIndex) The index value to find.
// (>) lockFn (tLockFn) A function to call while the list is locked.
// (>) create (bool) True if an object should be created if it can't be found.
//
// FUNCTION VALUE:
// (cUserClass *) The object found, or NULL of none is found
//
// PURPOSE:
// Locate an object in the list given a value of the key
//
// The lockFn is executed on the object after it is located. lockFn may
// not actually be a lock function, but it must be a member of the cUserClass
// class, and it must take no arguments.
//
// DESCRIPTION:
// - Attempt to find an existing object.
// - If an existing object cannot be found, create a new object if the
//   create new flag is set.
// - If a lockFn has been specified, execute it before unlocking the list.
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

template< class cUserClass, class cIndex, class cStatus >
cUserClass	*cListOf< cUserClass, cIndex, cStatus >::find
(
    cIndex	index,		// (in)  The index value to look for.
    tLockFn	lockFn,		// (in)  Function to execute after found.
#ifdef NO_FIND_CREATE
    bool			// (in)  True if an object should be created
    				//	 if it can't be found.
#else // NO_FIND_CREATE
    bool	create		// (in)  True if an object should be created
    				//	 if it can't be found.
#endif // NO_FIND_CREATE
)
{
    iObjectList	i;
    cUserClass	*ret;


    loRwLock.rLock();
    i = loObjectList.find( index );
    if ( i == loObjectList.end() )
    {
#ifndef NO_FIND_CREATE
	if ( create )
	{
	    ret = new cUserClass( index );
	}
	else
#endif // NO_FIND_CREATE
	{
	    ret = FALSE;
	}
    }
    else
    {
	ret = (*i).second;
    }

    if ( ret != 0 && lockFn != 0 )
    {
	(ret->*lockFn)();
    }

    loRwLock.unlock();
    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template < class cUserClass, class cIndex, class cStatus >
//		cListOf< cUserClass, cIndex, cStatus >::forEach
//
// INVOCATION:
// class::forEach( fn, arg, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fn (tForEachFn) The function to call for each object.
// (>) arg (void *) Argument to pass to the function.
// (!) status (cStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute the specified function for each object in the class.
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
// This would be better implemented as a template member function, with
// template parameters for the "arg" parameter and for the "status"
// parameter. The former would improved type checking, the latter would
// allow the cStatus type to be removed from the class template arguments.
//-
//***********************************************************************
//

template< class cUserClass, class cIndex, class cStatus >
void		cListOf< cUserClass, cIndex, cStatus >::forEach
(
    tForEachFn	fn,		// (in)  The function to call.
    void	*arg,		// (in)  Argument to pass to the function.
    cStatus	&status		// (mod) Function return status.
)
{
    iObjectList	i;

    loRwLock.rLock();
    for ( i = loObjectList.begin(); i != loObjectList.end(); i++ )
    {
	(((*i).second)->*fn)( arg, status );
    }
    loRwLock.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cIndex, class cStatus >
//		cListOf< cUserClass, cIndex, cStatus >::insertThread
//
// INVOCATION:
// n.a. Called by pthread_create.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pArg (void *) The argment passed to pthread_create
//
// FUNCTION VALUE:
// (void *) not used. Only there to be compatable with pthread_create.
//
// PURPOSE:
// Add an object to the list of objects in a separate thread.
//
// A pointer to the object and the object index are passed in a pair object
// passed through the argument.
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

template< class cUserClass, class cIndex, class cStatus >
void		*cListOf< cUserClass, cIndex, cStatus >::insertThread
(
    void	*pArg		// (in)  The argment passed to 
    				//	 pthread_create
)
{
    pair<int, cListOf *>	
		*arg;
    int		key;
    cListOf	*pObj;

    arg = (pair<int, cListOf *> *) pArg;
    key = arg->first;
    pObj = arg->second;
    delete arg;

    loRwLock.wLock( key );
    loObjectList[ pObj->loObjectIndex ] = (cUserClass *) pObj;
    loRwLock.unlock();
    return( NULL );
}
#endif // GENLISTOF_H
