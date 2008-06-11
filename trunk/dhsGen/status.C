static char rcsid[] = "$Id: status.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsGen/src/status.C
//
// PURPOSE:
// Contains the implementation of the cDhsStatus class.
//
// METHOD NAME(S)
// cDhsStatus::debugCheck - This method is called when the P_DEBUG_MIN
//		or P_DEBUG_FULL properties are present on a message.
// cDhsStatus::sysErrno - This method formats a message describing a system
//		errno value.
// cDhsStatus::msgDisplay - Default message display method.
// cDhsStatus::cStatusMessage::cStatusMessage - constructor for the
//		cDhsStatus::cStatusMessage class.
// cDhsStatus::cStatusMessage::operator() - Format a message into the message
//		buffer.
// cDhsStatus::cStatusMessage::operator+= - Add a new property to the message.
// cDhsStatus::cStatusMessage::properties - Execute the callback function
//		for each of the properties in this property list.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:54  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.16  2000/01/05 00:16:06  nhill
// Changed to use snprintf instead of the IMP printf functions.
//
// Revision 1.15  1998/05/04 20:42:07  nhill
// Removed clearing the status code when a debug message is not displayed.
//
// Revision 1.14  1998/04/23 22:21:46  nhill
// Changed the memory allocation failure message.
//
// Revision 1.13  1998/04/21 17:09:26  nhill
// Added property P_NO_DISP and function retFalse to support it.
//
// Revision 1.12  1998/04/20 21:52:35  nhill
// Added properties to the status message objects.
// Removed unused functions.
// Updated the documentation.
//
// Revision 1.11  1998/04/06 16:47:03  nhill
// Added some new standard messages.
// Changed to align message text.
//
// Revision 1.10  1997/10/30 22:20:30  nhill
// Changed to use safe version of the print routines.
//
// Revision 1.9  1997/09/18 17:38:15  nhill
// Changed the dhs library error message.
//
// Revision 1.8  1997/08/19 19:24:58  nhill
// Fixed bugs when handling system errors.
//
// Revision 1.7  1997/07/10 19:28:39  nhill
// Added an assumption that message starting with E_ are errors.
//
// Revision 1.6  1997/06/13 21:16:55  nhill
// Added the E_CONFIG status.
//
// Revision 1.5  1997/06/11 19:48:25  nhill
// Changed to a new status class.
//
// Revision 1.4  1997/05/01 17:11:23  nhill
// Changed the message string output to const.
//
// Revision 1.3  1997/04/24 19:35:21  nhill
// Removed the cStaCommand::response method.
//
// Revision 1.2  1997/04/16 16:35:06  nhill
// Changed the name of the include files.
//
// Revision 1.1  1997/04/15 23:06:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <cstdarg>
#include <cerrno>
#include <cstdio>

#include "genMutex.H"

#include "dhsGenStatus.H"

cDhsStatus::tDebugLevel	cDhsStatus::dsDebugLevel = cDhsStatus::DEBUG_NONE;
char *			cDhsStatus::dsDebugStrings[] = 
				{ "NONE", "MIN", "FULL" };
int			cDhsStatus::dsNumDebugStrings = 
				sizeof( cDhsStatus::dsDebugStrings ) / 
				sizeof( char * );
int			cDhsStatus::cStatusMessage::smMaxCodeLen = 0;


//
// Set up the default property objects.
//

msgPropertyDef( cDhsStatus, P_DEBUG_MIN, cDhsStatus::debugCheck );
msgPropertyDef( cDhsStatus, P_DEBUG_FULL, cDhsStatus::debugCheck );
msgPropertyDef( cDhsStatus, P_NO_DISP, cDhsStatus::retFalse );


//
// Set up the default messages.
//

msgText( cDhsStatus, S_CONFIG_READ, P_DEBUG_MIN,
	"Reading configuration file `%s'." );
msgText( cDhsStatus, S_DHS, P_NULL,
	"%s" );
msgText( cDhsStatus, S_SUCCESS, P_NO_DISP,
	"Successful completion." );
msgText( cDhsStatus, E_CONFIG, P_NULL,
	"Error reading configuration file." );
msgText( cDhsStatus, E_CONFIG_KWD, P_NULL,
	"Error reading `%s' configuration keyword" );
msgText( cDhsStatus, E_DHS, P_NULL,
	"Error in DHS library function:\n%s" );
msgText( cDhsStatus, E_INIT, P_NULL,
	"Attempt to use classs %s before it is initialized" );
msgText( cDhsStatus, E_MEMORY, P_NULL,
	"Memory allocation failure in expression:\n%s" );
msgText( cDhsStatus, E_SYSTEM, P_NULL,
	"Sytem error message." );

#define	MSG_INDENT_LENGTH	10

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::debugCheck
//
// INVOCATION:
// N.A. This is a property callback function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (const cDhsStatus &) The status that caused the callback.
// (>) prop (const cDhsStatus::cStatusProperty &) The property that caused the 
//		callback.
// (>) string (const char *) The string to display.
//
// FUNCTION VALUE:
// (bool) True if other callbacks should be executed.
//
// PURPOSE:
// Return false of the current debug level <= the debug level of the property
//
// DESCRIPTION:
// Check the current debug level against the property debug level.
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

bool		cDhsStatus::debugCheck
(
    cDhsStatus	&status,	// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
		&prop,		// (in)  The property that caused the callback.
    const char	*		// (in)  The string to display.
)
{
    if ( debugLevel() == DEBUG_NONE || 
	    ( &prop == &( status.P_DEBUG_FULL ) && 
		    debugLevel() == DEBUG_MIN ) ) 
    {
	//
	// The debug level is isn't high enough so c;lear the message and
	// return false.
	//

	status.clear();
	return( false );
    }
    else 
    {
	//
	// The debug level is high enough so return true.
	//

	return( true );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::retFalse
//
// INVOCATION:
// N.A. This is a property callback function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (const cDhsStatus &) The status that caused the callback.
// (>) prop (const cDhsStatus::cStatusProperty &) The property that caused the 
//		callback.
// (>) string (const char *) The string to display.
//
// FUNCTION VALUE:
// (bool) Always returns false.
//
// PURPOSE:
// Return false to stop all further processing of a status message
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

bool		cDhsStatus::retFalse
(
    cDhsStatus	&status,	// (in)  The status that caused the callback.
    const cDhsStatus::cStatusProperty
		&,		// (in)  The property that caused the callback.
    const char	*		// (in)  The string to display.
)
{
    status.clear();
    return( false );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::sysErrno
//
// INVOCATION:
// status.sysErrno()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Format an error message describing the current errno value
//
// DESCRIPTION:
// Format a message which includes the errno value and strerror string.
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

void		cDhsStatus::sysErrno
(
    void
)
{
    int		len;
    char	temp_buffer1[ MSG_MAX_LEN ];

    (void) sprintf( temp_buffer1, "(%s %d) ", prefix(), errno );
    len = strlen( dsBuffer );
    if ( dsBuffer[0] != '\0' && len < MSG_MAX_LEN - 1 )
    {
	strcat( dsBuffer, "\n" );
    }
    strncat( dsBuffer, temp_buffer1, MSG_MAX_LEN - len - 1 );
    len = strlen( dsBuffer );
    strncat( dsBuffer, strerror( errno ), MSG_MAX_LEN - len - 1 );

    dsBuffer[MSG_MAX_LEN - 1] = '\0';

    dsStatus = &(this->E_SYSTEM);
    dsError = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::msgDisplay
//
// INVOCATION:
// status.msgDisplay( string );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string (const char *) The string to msgDisplay.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Display a message
//
// DESCRIPTION:
// Default message handling is to display it to stdout, or if a parent
// object was specified, use the parents display routine.
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

void		cDhsStatus::msgDisplay
(
    const char	*string		// (in)  The message to display.
)
{
    if ( *string != '\0' )
    {
	if ( dsParent == NULL )
	{
	    std::cout << string << std::endl;
	}
	else
	{
	    dsParent->msgDisplay( string );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusMessage::cStatusMessage
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) code (char *) The message code string.
// (>) (void *) A dummy argument to ensure all constructors have the same
//		number of arguments.
// (>) format (char *) The message format string.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the status messages
//
// DESCRIPTION:
// Assign the status string to the message.
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

		cDhsStatus::cStatusMessage::cStatusMessage
(
    char	*code,		// (in)  The code string.
    void	*,		// (in)  Dummy argument.
    char	*format		// (in)  The format string.
)
{
    smError = ( *code == 'E' ) && ( *(code+1) == '_' );
    smFormat = format;
    smCode = code;
    smMaxCodeLen = MAX( smMaxCodeLen, strlen( code ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusMessage::cStatusMessage
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) code (char *) The message code string.
// (>) prop (cDhsStatus::cStatusProperty &) A property for the message.
// (>) format (char *) The message format string.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the status messages
//
// DESCRIPTION:
// Assign the status string to the message.
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

		cDhsStatus::cStatusMessage::cStatusMessage
(
    char	*code,		// (in)  The code string.
    cDhsStatus::cStatusProperty	// (in)  A property for the message.
		&prop,
    char	*format		// (in)  The format string.
)
{
    smError = ( *code == 'E' ) && ( *(code+1) == '_' );
    smFormat = format;
    smCode = code;
    smMaxCodeLen = MAX( smMaxCodeLen, strlen( code ) );
    smProperties | prop;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusMessage::cStatusMessage
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) code (char *) The message code string.
// (>) propList (cDhsStatus::cStatusPropertyList &) A property list for the 
//		message.
// (>) format (char *) The message format string.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the status messages
//
// DESCRIPTION:
// Assign the status string to the message.
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

cDhsStatus::cStatusMessage::cStatusMessage
(
    char	*code,		// (in)  The code string.
    const cDhsStatus::cStatusPropertyList& propList, // (in)  A property list for the message. (XXX allan: added const)
    char	*format		// (in)  The format string.
): smProperties( propList )
{
    smError = ( *code == 'E' ) && ( *(code+1) == '_' );
    smFormat = format;
    smCode = code;
    smMaxCodeLen = MAX( smMaxCodeLen, strlen( code ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusMessage::operator()
//
// INVOCATION:
// status.E_EOF( status, args );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (cDhsStatus &) The status object to use.
// (>) ... (vararg) Optional arguments for the format.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Format a string into the message buffer.
//
// DESCRIPTION:
// Assign the status message to the status object and excute the
// cDhsStatus::cStatusMessage::operator( varg, status ) method.
// If the autodisplay flag is true, automatically display the message.
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

void		cDhsStatus::cStatusMessage::operator()

(
    cDhsStatus	&status,	// (in)  The status object to use.
    ...
)
{
    va_list	varg;


    va_start( varg, &status );

    status.dsStatus = this;
    status.dsError = smError;
    (*this)( varg, status );

    if ( status.dsAutoDisplay )
    {
	status.display();
    }

    va_end( varg );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusMessage::operator()
//
// INVOCATION:
// statusItem( varg, prefix, buffer );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) varg (va_list) Optional arguments for the format.
// (!) status (cDhsStatus &) The status object being updated.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Format a string into the buffer.
//
// DESCRIPTION:
// - Create a format string for the message.
// - Format the message prefix into a temporary buffer.
// - Append the message and its arguments to the prefix.
// - Indent any continuation lines in the message.
// - Copy the message to the status object's buffer.
// - Execute the property callback for the status message.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsStatus::cStatusMessage::operator()
(
    va_list	varg,		// (in)  Optional arguments for the format.
    cDhsStatus	&status		// (in)  The status object being updated.
)
{
    char	temp_buffer1[MSG_MAX_LEN];
    char	temp_buffer2[MSG_MAX_LEN];
    char	*t1, *t2;
    int		i;
    int		len;
    char	format[41];


    //
    // Format the format string.
    //

    (void) sprintf( format, "(%%s %%s) " );


    //
    // Pad the format string with spaces to align the message text.
    //

    if ( smMaxCodeLen - strlen( smCode ) > 0 )
    {
	strncat( format, "                                        ", 
		MIN( 41 - strlen( format ), smMaxCodeLen - strlen( smCode ) ) );
    }


    //
    // Format the message prefix into the buffer.
    //

    (void) snprintf( temp_buffer1, MSG_MAX_LEN, format, status.prefix(), 
	    smCode );
    len = strlen( temp_buffer1 );


    //
    // Append the message and its arguments to the message.
    //

    (void) vsnprintf( temp_buffer1 + len, MSG_MAX_LEN - len, smFormat, varg );


    //
    // Indent any continuation lines.
    //

    for ( t1 = temp_buffer1, t2 = temp_buffer2; 
	    *t1 && t2 < temp_buffer2 + MSG_MAX_LEN - 1; t1++ )
    {
	*t2++ = *t1;
	if ( *t1 == '\n' )
	{
	    for ( i = 0; i < MSG_INDENT_LENGTH; i++ )
	    {
		*t2++ = ' ';
	    }
	}
    }
    *t2 = '\0';


    //
    // Copy the message to the buffer.
    //

    len = strlen( status.dsBuffer );
    if ( status.dsBuffer[0] != '\0' && len < MSG_MAX_LEN - 1 )
    {
	strcat( status.dsBuffer, "\n" );
    }

    strncat( status.dsBuffer, temp_buffer2, MSG_MAX_LEN - len - 1 );
    status.dsBuffer[MSG_MAX_LEN - 1] = '\0';


    //
    // Execute the property callbacks for the status message.
    //

    smProperties.properties( status, temp_buffer2 );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusMessage::operator+=
//
// INVOCATION:
// message+= property;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) prop (cDhsStatus::cStatusProperty &) Property to add to the message.
//
// FUNCTION VALUE:
// (cDhsStatus::cStatusMessage &) This message.
//
// PURPOSE:
// Add a property to this message
//
// DESCRIPTION:
// Add the properties to this message's property list.
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

cDhsStatus::cStatusMessage	
		&cDhsStatus::cStatusMessage::operator+=
(
    const cDhsStatus::cStatusProperty	// (in)  Property to add to the message. (XXX allan: added const)
		&prop
)
{
    smProperties += prop;
    return( *this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsStatus::cStatusPropertyList::properties
//
// INVOCATION:
// propertyList.properties( status, string );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (const cDhsStatus &) The status object associated with the string.
// (>) string (const char *) The string to display.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute the callback function for each of the properties in the property
// list
//
// DESCRIPTION:
// Loop through the list of properties, executing their callback function.
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

void		cDhsStatus::cStatusPropertyList::properties
(
    cDhsStatus	&status,	// (in)  The status object associated with 
    				//	 the string.
    const char	*string		// (in)  The string being added to the status.
) const
{
    int		i;
    cDhsStatus::cStatusProperty
		*pProp;
    bool	result( true );

    for ( i = 0; i < eptrlen( &splEptrList ); i++ )
    {
	pProp = (cDhsStatus::cStatusProperty *) eptritem( &splEptrList, i );
	if ( pProp->spCallbackFn != NULL )
	{
	    if ( ! pProp->spCallbackFn( status, *pProp, string ) )
	    {
		break;
	    }
	}
    }
}
