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
// dhsStatus/h/dhsStat.H
//
// PURPOSE:
// Contains the external definitions for the DHS status server.
//
// CLASSES:
// cDhsStatus - Status class for the DHS system.
// cDhsStatus::cStatusMessage - A message in the status class.
// cDhsStatus::cStatusProperty - A property of a message.
// cDhsStatus::cStatusPropertyList - A list of properties of a message.
//
// Functions:
// operatior<< - Overloaded function to display a cDhsStatus message to 
//		a stream.
// cDhsStatus::cDhsStatus - Constructor for the status class.
// cDhsStatus::operator() - Assign a status value to a status object.
// cDhsStatus::operator+= - Append a string to a status message.
// cDhsStatus::debugLevel - Accessor functions to set and return the debug
//		level for the class.
// cDhsStatus::status - Assign a status value to a cDhsStatus object.
// cDhsStatus::operator| - Combine two properties into a property list.
//
// MACROS:
// msgPrefix - Define the message prefix for a module.
// msgId - Define a new status message id.
// msgText - Define the text for a status message.
// msgProperty - Define a new message property.
// msgPropertyDef - Define the callback function for a message property
// checkStat - Check a status code.
// checkNull - Check for a NULL return value.
// check - Execute a function and check its return code.
// checkSystem - Execute a system function and check its return code.
// checkConfig - Execute a function in the genConfig library and check its
//		return code.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:45  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.14  2001/03/12 20:58:12  nhill
// Changed to work with the workshop 5.0 compilters.
//
// Revision 1.13  1998/06/22 19:20:42  nhill
// Added a function to allow the callback associated with a property to be
// replaced.
//
// Revision 1.12  1998/04/23 22:29:39  nhill
// Changed the E_MEMORY message to include the command that caused the
// error.
//
// Revision 1.11  1998/04/21 17:06:59  nhill
// Updated the documentation.
// Added properties to the cDhsStatus::cStatusMessage class.
// Moved some checking routines into this file from applications.
// Moved the status server interface to a separate file.
//
// Revision 1.10  1998/04/06 16:47:57  nhill
// Added some new checking functions.
// Added information required to align messges.
//
// Revision 1.9  1997/10/30 22:21:02  nhill
// Fixed some typos.
//
// Revision 1.8  1997/10/01 16:35:25  nhill
// Changed the arguments to method cSta::init.
//
// Revision 1.7  1997/09/18 17:44:05  nhill
// Added function cDhsConnection.
//
// Revision 1.6  1997/08/19 19:36:20  nhill
// Fixed some lint type problems.
//
// Revision 1.5  1997/07/10 19:26:58  nhill
// Added a new error flag to the status class.
//
// Revision 1.4  1997/06/13 21:17:40  nhill
// Added functions to the cSta class to read status server info from a
// configuration file.
//
// Revision 1.3  1997/06/11 19:50:58  nhill
// Changed to a new status class.
//
// Revision 1.2  1997/04/24 19:32:41  nhill
// Minor oragnizational changes.
//
// Revision 1.1  1997/04/15 23:06:07  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSSTAT_H
#define	DHSSTAT_H

#include "genBool.H"

#include <stdarg.h>
#include <iostream.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
}

class	cDhsConnection;
class	cStaCommand;
class	cMutex;

//
// The VOID macro allows an empty argument to be passed to a macro with
// arguments.
//

#define	VOID
#define	P_NULL	NULL

//
//***********************************************************************
//+
// MACRO NAME:
// msgPrefix
//
// INVOCATION:
// class cModuleStatus: public cDhsStatus
// {
//	msgPrefix( "modulePrefix" );
//	...
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) modPrefix (char *) Prefix string to use for the module
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Define the prefix string to use for a module
//
// DESCRIPTION:
// Define a virtual function to return the prefix string. The cDhsStatus
// class executes the function when it needs to get the string.
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

#define	msgPrefix( modPrefix )						\
    virtual char *prefix()						\
    {									\
	return( modPrefix );						\
    }

//
//***********************************************************************
//+
// METHOD NAME:
// msgId
//
// INVOCATION:
// class cModuleStatus: public cDhsStatus
// {
//	msgId( "messageCode" );
//	...
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) code (char *) Code for the new message.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Define a new message code
//
// Message status codes should start with "S_" for information messages and
// "E_" for error messages.  In order to avoid unresolved symbols, this 
// must be matched with an invocation of the msgText macro in one of the
// modules .C files.
//
// DESCRIPTION:
// Define a static cStatusMessage object with the message code as its name.
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

#define	msgId( code )							\
	    static cStatusMessage code


//
//***********************************************************************
//+
// MACRO NAME:
// msgText
//
// INVOCATION:
// msgText( cModStatus, "messageCode", 
//		cModStatus::P_DEBUG | cModStatus::P_LOG, "message text" );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) class (class) The status class in which the corresponding msgId macro 
//		was invoked.
// (>) code (char *) The code of the message being defined.
// (>) properties (cDhsStatus::cStatusProperty) A list of properties for 
//		the code.
// (>) text (char *) The text for the message.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Define the text for a message code
//
// This macro must be executed at the file scope of a C file (normally
// in msg.C). There must be a coresponding invocation of macro msgId in 
// the definition of class "class". The properties argument allows specific
// properties to be associated with the code.
//
// DESCRIPTION:
// Initialize the static object defined by the msgId macro.
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

#define	msgText( class, code, properties, text )			\
	    cDhsStatus::cStatusMessage 	class::code( #code, properties, text )

//
//***********************************************************************
//+
// MACRO NAME:
// msgProperty
//
// INVOCATION:
// msgProperty( propertyName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) propertyName (char *) The name of the new property.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Define a new status message propery
//
// This macro should be used in the public section of the definition of
// a class derived from the cDhsStatus class.
//
// DESCRIPTION:
// Create a new static cStatusProperty object named with the property's name.
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

#define	msgProperty( propertyName )					\
	    static cStatusProperty propertyName

//
//***********************************************************************
//+
// MACRO NAME:
// msgPropertyDef
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) class (cStaStatus derived) The class to which the property belongs.
// (>) propertyName (char *) The name of the property.
// (>) fn (tPropertyFn) Callback function for the property.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Defines a message property
//
// This macro defines a property declared with the msgProperty macro. This
// macro should be used at the file level in one C file, and should be passed
// the function to execute when the property is present.
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

#define	msgPropertyDef( class, property, fn )				\
	    cDhsStatus::cStatusProperty	class::property( fn )

//
//***********************************************************************
//+
// MACRO NAME:
// checkStat
//
// INVOCATION:
// checkStat( status, return );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (cDhsStatus) Status value to check.
// (>) ret (expr) Expression to execute to cause the function to return.
//
// MACRO VALUE:
// None
//
// PURPOSE:
// Check to see if the status is success, and execute the specified 
// expression if it is not
//
// This macro is used at the begining of fuctions to ensure that the 
// inherited status value is success, and to return if it is not.
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

#define	checkStat( s, ret )						\
    if ( ! (s).ok() )							\
    {									\
	ret;								\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// checkNull
//
// INVOCATION:
// checkNull( x = malloc( 10 ), status, expr )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) expr1 (void *) Expression to evaluate and test.
// (>) status (cDhsStatus) Status object to use to report the error.
// (>) expr2 (expr) Expression to execute if expr1 evaluates to NULL.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Evaluate expr1, and if it evaluates to NULL, set the status parameter
// to memory allocation error, and then evaulate expr2
//
// This macro is used to test to see if memory allocation routines have
// returned NULL, and if they have, set an error condition and (usually)
// return.
//
// Note that expr2 can be set to the VOID macro if no action is required
// after displaying the message.
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

#define	checkNull( expr1, status, expr2 )				\
    {									\
	if ( ( expr1 ) == NULL )					\
	{								\
	    (status).E_MEMORY( status, #expr1 );			\
	    (status).display();						\
	    expr2;							\
	}								\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// check
//
// INVOCATION:
// check( fn, status, return )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fn (expr) A function to execute
// (>) status (cDhsStatus) The status object that contains the status of
//		the function in the previous argument.
// (>) expr (expr) Expression to execute if the function returned error.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Execute the function and test its return status
//
// This function is used to test the return status of a function, and
// (usually) return if it failed. The status parameter must contain the 
// return status of the function.
//
// The fn argument may contain the VOID macro if the function has already
// been executed.
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

#define	check( fn, s, expr )						\
    fn;									\
    if ( !( (s).ok() ) )						\
    {									\
	expr;								\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// checkSystem
//
// INVOCATION:
// checkSystem( fn, status, return )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) expr1 (expr) The system function to execute
// (>) status (cDhsStatus) Status object to use to report the error
// (>) expr2 (expr) Expression to execute if the function fails
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Check the return status of a system function, and (usually) return
// if the function fails
//
// The return value from a system function can be passed as the expr1 
// argumentif the function has already been executed. The VOID macro 
// can be passed as the expr2 argument if no actions are required after
// the message is displayed.
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

#define	checkSystem( expr1, status, expr2 )				\
    {									\
	int		sysStat;					\
	if ( ( sysStat = expr1 ) < 0 )					\
	{								\
	    (status).sysErrno();					\
	    (status).display();						\
	    expr2;							\
	}								\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// checkConfig
//
// INVOCATION:
// checkConfig( configFn, status, return );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) configFn (expr) Configuration function to execute
// (>) status (cDhsStatus) Status object to use to report the error
// (>) expr (expr) Expression to execute when a failure occurs
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Execute a function from the genConfig library and check its return status
//
// The configFn argument must be a function or return status from a function
// in the genConfig library. The expr argument may be passed the VOID
// macro if no action is required after the message is displayed.
//
// DESCRIPTION:
// Check the function return status, and if it is not success format the
// config error message and append the message returned from the genConfig
// library.
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

#define	checkConfig( fn, status, ret )					\
    {									\
	int		s;						\
	if ( ( s ) = ( fn ) != GC_SUCCESS )				\
	{								\
	    (status).E_CONFIG( status );				\
	    (status) += gc_msg;						\
	    (status).display();						\
	    ret;							\
	}								\
    }

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsStatus
//
// PUBLIC CLASSES: 
// cStatusMessage - This class contains one status message.
// cStatusProperty - This is a property of a status message.
// cStatusPropertyList - This is a list of status properties.
//
// PUBLIC METHODS: 
// (none) cDhsStatus - Constructor for the class.
// (void) operator() - Assign a status code to this status object.
// (bool) operator!= - Compare this status object to a status code.
// (bool) operator== - Compare this status object to a status code.
// (cDhsStatus &) operator+= - Append a string to this status object's 
//		message.
// (cDhsStatus &) operator+= - Append a status objects's message to this 
//		status object's message.
// (void) clear - Clear this status object's message.
// (tDebugLevel) debugLevel - Set or return the current debug level of the 
//		class.
// (char *) debugString - Return the string form of the current debug level.
// (void) display - Display this status object's message.
// (void) display - Set the display method for this object to be the display
//		method of a specified status object.
// (void) displayStart - Start automatic display of messages.
// (void) displayStop - Stop automatic display of messages.
// (int) maxCodeLen - Return the longest message code curently defined.
// (char *) message - Return the current message string.
// (bool) ok - Test to see if the current code is a success code.
// (void) status - Set this status message to the specified string.
// (void) status - Set this status state to the state of the specified 
//		status object.
// (void) status - Set this status state to the specified message type.
// (char *) sysErrno - Format a message containing the current system error
//		message (from the errno variables).
//
// PUBLIC TYPES: 
// tDebugLevel - The debug levels known to this status object.
// tStatus - A status message type.
//
// PUBLIC DATA: 
// S_SUCCESS - The success status code.
// E_CONFIG - The configuration error status code.
// E_CONFIG_KWD - Error reading keyword status code. It takes one argument
//		which should be the keyword name.
// E_DHS - Error in the DHS library status code. It takes one argument which
//		should be the error message returned by the DHS library.
// E_INIT - Attempt to use class before initialization status code. It takes
//		one argument which should be the class name.
// E_MEMORY - Memory allocation failure status code.
// E_SYSTEM - System function error status code.
//
// PUBLIC PROPERTIES:
// P_DEBUG_MIN - Terminate message processing if debug level is < DEBUG_MIN.
// P_DEBUG_FULL - Terminate message processing if debug level < DEBUG_FULL.
// P_NO_DISP - Don't do any processing for this message.
// P_NULL - Default message handling.
//
// PUBLIC STATUS CODES:
// S_CONFIG_READ - Reading configuration file.
// S_DHS - Message from the DHS library.
// S_SUCCESS - Successful completion.
// E_CONFIG - Error reading the configuration file.
// E_CONFIG_KWD - Error parsing configuration keyword.
// E_DHS - Error from the DHS library.
// E_INIT - Attempt to use a class before it is initialized.
// E_MEMORY - Memory allocation failure.
// E_SYSTEM - Unix system error.
//
// PURPOSE:
// Contains a status value
//
//-
//***********************************************************************
//

class 	cDhsStatus
{
    public:
	class		cStatusMessage;
	class		cStatusProperty;
	class		cStatusPropertyList;
	friend		class cStatusMessage;
	friend		class cStatusPropertyList;

	typedef enum
	{
	    DEBUG_NONE	= 0,
	    DEBUG_MIN	= 1,
	    DEBUG_FULL	= 2
	} tDebugLevel;

	typedef cStatusMessage 
			tStatus;

	msgProperty( P_DEBUG_MIN );
	msgProperty( P_DEBUG_FULL );
	msgProperty( P_NO_DISP );

	msgId( S_CONFIG_READ );
	msgId( S_DHS );
	msgId( S_SUCCESS );
	msgId( E_CONFIG );
	msgId( E_CONFIG_KWD );
	msgId( E_DHS );
	msgId( E_INIT );
	msgId( E_MEMORY );
	msgId( E_SYSTEM );

	inline		cDhsStatus( );
	inline void	operator() ( const cStatusMessage & s );
	bool		operator!=( const tStatus& s ) const
				{ 
				    return( dsStatus != &s ); 
				};
	bool		operator==( const tStatus&  s ) const
				{ 
				    return( dsStatus == &s ); 
				};
	inline cDhsStatus
			&operator+=( const char * string );
	cDhsStatus	&operator+=( const cDhsStatus & s )
				{
				    *this += s.dsBuffer;
				    return( *this );
				}
	void		clear( void )
				{
				    msg_clear( dsBuffer );
				};
	static tDebugLevel
			debugLevel( void ) 
				{
				    return( dsDebugLevel ); 
				};
	inline static tDebugLevel
			debugLevel( const char * );
	inline static tDebugLevel
			debugLevel( tDebugLevel );
	static char *	debugString( void ) 
				{
				    return( dsDebugStrings[ dsDebugLevel ] ); 
				};
	void		display()
				{
				    msgDisplay( dsBuffer );
				    clear();
				};
	void		display( cDhsStatus & s )
				{
				    dsParent = &s;
				};
	void		displayStart( void )
				{
				    dsAutoDisplay = true;
				};
	void		displayStop( void )
				{
				    dsAutoDisplay = false;
				};
	inline int	maxCodeLen( void );
	const char	*message( void ) const
				{
				    return( dsBuffer );
				}
	virtual void	msgDisplay( const char * );
	bool		ok( void ) const
				{
				    return( ! dsError );
				};
	void		status( const char * string )
				{
				    strcpy( dsBuffer, string );
				}
	void		status( const cDhsStatus & s )
				{
				    dsStatus = s.dsStatus;
				    dsError = s.dsError;
				    dsBuffer[0] = '\n';
				    *this += s.dsBuffer;
				}
	inline void	status( const tStatus& s );
	void		sysErrno( void );

    protected:
    private:
	virtual char *	prefix()
			{
			    if ( dsParent != NULL )
			    {
				return( dsParent->prefix() );
			    }
			    else
			    {
				return( "Undefined" );
			    }
			};
	static bool	debugCheck( cDhsStatus &, 
				const cDhsStatus::cStatusProperty &, 
				const char * );
	static bool	retFalse( cDhsStatus &, 
				const cDhsStatus::cStatusProperty &, 
				const char * );

	bool		dsAutoDisplay;	// Automatically display data?
	char		dsBuffer[ MSG_MAX_LEN ];
	static tDebugLevel	
			dsDebugLevel;	// Current debug level.
	static char	*dsDebugStrings[];
	bool		dsError;	// Is the status currently an error.
	static int	dsNumDebugStrings;
	cDhsStatus	*dsParent;
	const tStatus	*dsStatus;	// Current status.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsStatus::cStatusProperty
//
// PUBLIC METHODS: 
// (none) cStatusProperty - Constructor for the class.
// (tPropertyFn) callback - Replace the callback function associated with
//		a property.
// (cDhsStatus::cStatusProperty &) operator| - Join to properties into
//		a property list.
//
// PUBLIC TYPES: 
// tPropertyFn - Prototype of the callback function for a property.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This is a property of a message
//
// It keeps track of some action (in the form of a callback function)
// to be executed when a property is present on a message.
//
//-
//***********************************************************************
//

class cDhsStatus::cStatusProperty
{
    friend class cDhsStatus::cStatusPropertyList;

    public:
	typedef bool (*tPropertyFn) ( cDhsStatus &, 
		const cDhsStatus::cStatusProperty &, const char * );

			cStatusProperty( tPropertyFn fn )
			{
			    spCallbackFn = fn;
			};
	tPropertyFn	callback( tPropertyFn fn )
			{
			    tPropertyFn	oldFn( spCallbackFn );
			    spCallbackFn = fn;
			    return( oldFn );
			};
	inline cDhsStatus::cStatusPropertyList
			operator| ( const cDhsStatus::cStatusProperty & );
    protected:
    private:
	tPropertyFn	spCallbackFn;
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsStatus::cStatusPropertyList
//
// PUBLIC METHODS: 
// (none) cStatusPropertyList - Constructor for the class.
// (none) cStatusPropertyList - Copy constructor for the class.
// (none) ~cStatusPropertyList - Destructor for the class.
// (cDhsStatus::cStatusPropertyList) operator| - operator to add a property
//		onto a property list.
// (cDhsStatus::cStatusPropertyList) operator+= - operator to add a property
//		onto a property list.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class keeps a list properties
//
// The class also provides methods for creating the list of properties.
//
//-
//***********************************************************************
//

class cDhsStatus::cStatusPropertyList
{
    friend class cDhsStatus::cStatusMessage;
    friend class cDhsStatus;

    public:
			cStatusPropertyList()
			{
			    eptr_init( &splEptrList, 10 );
			}
			cStatusPropertyList( 
				const cDhsStatus::cStatusPropertyList &list2 )
			{
			    int		i;

			    eptr_init( &splEptrList, 10 );
			    for ( i = 0; i < eptrlen( &(list2.splEptrList) ); 
				    i++ )
			    {
				*this += *( cDhsStatus::cStatusProperty *) 
					eptritem( &(list2.splEptrList), i );
			    }
			};
			~cStatusPropertyList()
			{
			    eptr_clear( &splEptrList, NULL );
			}
	cDhsStatus::cStatusPropertyList
			&operator|( const cDhsStatus::cStatusProperty &prop )
			{
			    *this += prop;
			    return( *this );
			};
	cDhsStatus::cStatusPropertyList
			&operator+=( const cDhsStatus::cStatusProperty &prop )
			{
			    eptr_insert( &splEptrList, 
				    eptrlen( &splEptrList ), (void *) &prop );
			    return( *this );
			}
    protected:
    private:
	void		properties( cDhsStatus &status, 
				const char *string ) const;
	const cStatusPropertyList
			&operator=( const cDhsStatus::cStatusPropertyList &arg )
			{
			    return( arg );
			};

	EPTR		splEptrList;	// A list of properties.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsStatus::cStatusMessage
//
// PUBLIC METHODS: 
// (none) cStatusMessage - Constructor for the class.
// (void) operator() - various operators to assign this status to a 
//		cDhsStatus object.
// (cDhsStatus::cStatusMessage &) operator+= - Add a property or list of
//		properties to a status message.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Objects of this class store information about status messages used by
// the cDhsStatus class.
//
//-
//***********************************************************************
//

class	cDhsStatus::cStatusMessage
{
    friend class cDhsStatus;

public:
    cStatusMessage( char *, void *, char * );
    cStatusMessage( char *, cDhsStatus::cStatusProperty &, 
		    char * );
    cStatusMessage( char *, const cDhsStatus::cStatusPropertyList &, char * ); // XXX allan: added const
    void operator() ( cDhsStatus &, ... );
    cDhsStatus::cStatusMessage& operator+= ( const cDhsStatus::cStatusProperty & );  // XXX allan: added const
    cDhsStatus::cStatusMessage
    &operator+= (const cDhsStatus::cStatusPropertyList &propList ) // XXX allan: added const
    {
	int		i;

	for( i = 0; i < eptrlen( &propList.splEptrList ); 
	     i++ )
	    {
		*this += *(cStatusProperty *)
		    eptritem( &propList.splEptrList, i );
	    }
	return( *this );
    }
private:
    void		operator() ( va_list, cDhsStatus & );

    char		*smCode;	// Message error string.
    bool		smError;	// True if the message is an error.
    char		*smFormat;	// Message format string.
    static int	smMaxCodeLen;	// The longest message code length.
    cDhsStatus::cStatusPropertyList	// The message's properties.
    smProperties;
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// operator<<
//
// INVOCATION:
// stream << status << endl;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) s (ostream &) The stream to print to.
// (>) status (cDhsStatus &) The status to print.
//
// FUNCTION VALUE:
// (ostream &) the stream being used.
//
// PURPOSE:
// Display a message to a stream
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

inline ostream&	operator<<
( 
    ostream	&s, 		// (in)  The stream to print to.
    cDhsStatus	&status 	// (in)  The status to print.
)
{
    return s << status.message();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cDhsStatus
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
// Constructor for a status value.
//
// DESCRIPTION:
// Set the initial status value to success.
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

		cDhsStatus::cDhsStatus
(
    void
)
{
    dsAutoDisplay = true;
    dsStatus = &S_SUCCESS;
    dsError = false;
    dsBuffer[0] = '\0';
    dsParent = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::operator()
//
// INVOCATION:
// status( S_SUCCESS );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) s (const cStatusMessage &) The new status to assign.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Assign the status in the argument to the status object
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

void		cDhsStatus::operator()
(
    const cStatusMessage	// (in)  The new status to assign.
		&s
)
{
    dsStatus = &s;
    dsError = s.smError;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::operator+=
//
// INVOCATION:
// status += string;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string (char *) String to add to the status object.
//
// FUNCTION VALUE:
// (cDhsStatus &) This status object.
//
// PURPOSE:
// Append a string to the message associated with a status message
//
// DESCRIPTION:
// - Append the string to the status buffer.
// - Execute the properties callbacks for the currently set status.
// - If the autoDisplay flag is set, display the message.
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

cDhsStatus	&cDhsStatus::operator+=
(
    const char	*string		// (in)  The string to append.
)
{
    msg_append( dsBuffer, (char *) string );
    dsStatus->smProperties.properties( *this, string );
    if ( dsAutoDisplay )
    {
	display();
    }
    return( *this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::debugLevel
//
// INVOCATION:
// cDhsStatus::debugLevel( level );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string (char *) The debug level string.
//
// FUNCTION VALUE:
// (tDebugLevel) The existing debug level.
//
// PURPOSE:
// Set the new default debug level.
//
// DESCRIPTION:
// Assign the level to the default debug level and return the old level.
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

cDhsStatus::tDebugLevel	cDhsStatus::debugLevel
(
    const char	*string		// (in)  The new debug level.
)
{
    int		i;
    tDebugLevel	oldLevel;


    oldLevel = dsDebugLevel;

    for ( i = 0; i < dsNumDebugStrings; i++ )
    {
	if ( strcmp( string, dsDebugStrings[i] ) == 0 )
	{
	    dsDebugLevel = (tDebugLevel) i;
	}
    }

    return( oldLevel );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::debugLevel
//
// INVOCATION:
// cDhsStatus::debugLevel( level );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) level (tDebugLevel) The new debug level.
//
// FUNCTION VALUE:
// (tDebugLevel) The existing debug level.
//
// PURPOSE:
// Set the new default debug level.
//
// DESCRIPTION:
// Assign the level to the default debug level and return the old level.
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

cDhsStatus::tDebugLevel	cDhsStatus::debugLevel
(
    tDebugLevel	level		// (in)  The default debug level.
)
{
    tDebugLevel	oldLevel;

    oldLevel = dsDebugLevel;
    dsDebugLevel = level;

    return( oldLevel );
}
inline int	cDhsStatus::maxCodeLen( void )
{
    return( cDhsStatus::cStatusMessage::smMaxCodeLen );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::status
//
// INVOCATION:
// status.status( S_SUCCESS );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) s (tStatus &) Status to assign to the status object.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Assign a status to a cDhsStatus object
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

void		cDhsStatus::status
(
    const tStatus		// (in)  Status to assign to the status object.
		& s
)
{
    dsStatus = &s;
    dsError = s.smError;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusProperty::operator|
//
// INVOCATION:
// propertyList = property1 | property2;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) prop (cDhsStatus::cStatusProperty &) The property to or with this 
//			property.
//
// FUNCTION VALUE:
// (cDhsStatus::cStatusPropertyList &) A list of properties.
//
// PURPOSE:
// Merge two properties into a list of properties
//
// DESCRIPTION:
// - Create a new property list identical to this property list.
// - Append the new property to the property list.
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

cDhsStatus::cStatusPropertyList
		cDhsStatus::cStatusProperty::operator|
(
    const cDhsStatus::cStatusProperty
		&prop		// (in)  The property to or with this property
)
{
    cDhsStatus::cStatusPropertyList
		propertyList;

    propertyList += *this;
    propertyList += prop;
    return( propertyList );
};
#endif // DHSSTAT_H
