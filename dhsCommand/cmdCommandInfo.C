static char rcsid[] = "$Id: cmdCommandInfo.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsCommand/src/cmdCommandInfo.C
//
// PURPOSE:
// Whatever
//
// MEHTOD NAME(S)
// cCmdCommandInfo::appliesToCmd - Determines if the given subsystem is in
//		the subsystem list for a command.
// cCmdCommandInfo::attribFind - Given an attribute name, find the attribute
//		in the list of attributes for the command.
// cCmdCommandInfo::cCmdCommandInfo - Constructor for the cCmdCommandInfo 
//		class.
// cCmdCommandInfo::~cCmdCommandInfo - Destructor for the cCmdCommandInfo 
//		class.
// cCmdCommandInfo::attribute - Parse an attribute line from the configuration
//		file.
// cCmdCommandInfo::attributeAdd - Add an attribute to the command.
// cCmdCommandInfo::command - Parse a command line from the configuration file.
// cCmdCommandInfo::config - Read the command information from the 
//		configuration file.
// cCmdCommandInfo::csExit - Cause the command server to exit.
// cCmdCommandInfo::debugSet - Set the debug level of the command server.
// cCmdCommandInfo::exec - Executed to start the processing of a command.
// cCmdCommandInfo::exit - Clean up the class for exit.
// cCmdCommandInfo::find - Find the command in the list of commands.
// cCmdCommandInfo::reSend - Send a command to the appropriate subsystems.
// cCmdCommandInfo::riReSend - Send a reset or init command to the appropriate 
//		subsystems.
// cCmdCommandInfo::reset - Cause the command server to reset.
// cCmdCommandInfo::resetHealth - Set the health of the command server to good.
// cCmdCommandInfo::simulateSet - Set the simulate level of the command server.
// cCmdCommandInfo::test - Test the command server.
// cCmdCommandInfo::subsystemAdd - Add a subsystem to the list of subsystems
//		associated with a command.
// cCmdCommandInfo::systemExit - Send an exit command to all subsystems.
// cCmdCommandInfo::verify - Verify the parameters of the command are correct.
// cCmdAttributeInfo::~cCmdAttributeInfo - Destructor for the cCmdAttributeInfo
//		class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:25:24  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.17  1998/08/11 22:14:43  nhill
// Added some delays to prevent deadlocks.
//
// Revision 1.16  1998/08/04 21:52:16  nhill
// *** empty log message ***
//
// Revision 1.15  1998/06/28 23:41:46  nhill
// Changed the error handling for forwarded messages to allow the
// messages to be sent to any remaining systems, even if one of them
// fails.
//
// Revision 1.14  1998/06/28 22:19:02  nhill
// Added some error checking to the functions that parse the configuration
// file.
//
// Revision 1.13  1998/06/26 19:26:32  nhill
// Separated the handling of exit and systemExit commands.
//
// Revision 1.12  1998/06/26 18:55:54  nhill
// Fixed some bugs in teh redirection of commands.
// Improved error checking when reading config files.
//
// Revision 1.11  1998/05/29 21:17:35  jaeger
// Alter the code that sends commands to the various servers, it now
// uses the subsystem order given in the configuration file or in
// the subsystem parameter list, instead of hard-coding the order
// in.
//
// Revision 1.10  1998/05/21 17:01:08  jaeger
// Made better error messages when command forwarding fails and made
// sure the error messages got returned in the command response.
//
// Revision 1.9  1998/04/15 23:58:29  nhill
// Updated the documentation.
// Changed to use the newest version of the cDhsStatus class.
//
// Revision 1.8  1998/03/13 22:06:23  jaeger
// Added code to handle reset and init specially.  Added "appliesToCmd" which
// checks if a particular subsystem is in the commands subsystem list.  Added
// resetHealth command and test command.  Added checking for errors when
// forwarding commands to subsystems.  Added a reset method.  Added disabling
// of commands during resets and inits.
//
// Revision 1.7  1997/12/03 20:00:39  jaeger
// Added checking of attribute dimensions for command values.
//
// Revision 1.6  1997/10/03 23:00:30  nhill
// Changed to work with the object space STL library.
//
// Revision 1.5  1997/06/13 20:46:19  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.4  1997/05/01 17:51:15  nhill
// Added the subsystemExit flag to indicate the subsytems should be told to exit.
//
// Revision 1.3  1997/04/28 22:53:47  nhill
// Added shutdown of client systems on exit.
//
// Revision 1.2  1997/04/24 19:43:31  nhill
// Added some "const" keywords.
//
// Revision 1.1  1997/04/16 21:50:14  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <algorithm>
#include <unistd.h>
    
extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
}
#include "genMutex.H"
#include "globals.H"
#include "cmdSubsystem.H"
#include "cmdCommandInfo.H"
#include "cmdDhs.H"


cCmdCommandInfo::tCommandMap	cCmdCommandInfo::cciCommandMap;
cMutex		cCmdCommandInfo::cciMutex;
bool		cCmdCommandInfo::cciSimulate = false;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::appliesToCmd
//
// INVOCATION:
// command.appliesToCmd( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command 	    (cDhsServerCommand &)   The command received.
// (>) subsystem    (char*)		    Subsystem to search for.
// (!) status 	    (cDhsStatus &) 	    Function return status.
//
// FUNCTION VALUE:
// TRUE  (boolean) : If the command server is in the subsystem list.
// FALSE (boolean) : otherwise.
//
// PURPOSE:
// Determines if the given subsystem is in the subsystem list
// for a particular command.
//
// DESCRIPTION:
// If there is a subsystemlist for a given command search the list,
// which may be a single string or an array of strings, for the
// given subsystem.  If the subsystem is in the list then
// return TRUE, otherwise return FALSE.
// If there is no subsystemlist return TRUE.
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

boolean		cCmdCommandInfo::appliesToCmd
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    const char* subsystem,	// (in)  The Subsystem to search for.
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    cDhsAttrib 	attrib;		// The level attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    ulong	dims[7];
    int		i;
    int		ndims;
    boolean	ret;
    char	**subsystemList;
    bool	noList;
    cDhsSubsystem::iterator
    		j;
    cCmdSubsystem
    		*pSubsystem;


    //
    // If there is a list of subsystems, the command server may be in the
    // list.
    //

    ret = FALSE;
    attrib = command.find( "subsystem", dhsStatus );

    if ( dhsStatus == DHS_S_NO_ATTRIB )
    {
	noList = true;
    }
    else
    {
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    const char *msg;
	    DHS_STATUS	s( DHS_S_SUCCESS );
	    status.E_DHS( status, 
		    "attrib = command.find( \"subsystem\", dhsStatus )" );
	    while ( s == DHS_S_SUCCESS )
	    {	
		msg = cDhs::message( s );
		if ( s == DHS_S_SUCCESS )
		{
		    status += msg;
		    cDhs::messageClear( s );
		}
	    }
	    return( FALSE );
	}

	//
	// Get the list of subsystems.
	//

	attrib.info( ndims, dims, (void **) &subsystemList, dhsStatus );
	if ( ndims > 0 || 
		( ndims == 0 && strne( (char *) subsystemList, "" ) ) )
	{
	    noList = false;
	}
	else
	{
	    noList = true;
	}
    }




    if ( noList )
    {
	//
	// No subsystem list, so the command applies to all
	// subsystems in the commands subsystem list.
	//

	ret = FALSE;
	for ( j = (cDhsSubsystem::iterator&)cciSubsystemList.begin(); j != cciSubsystemList.end(); j++ )
	{
	    pSubsystem = (cCmdSubsystem *) *j;

	    if ( *pSubsystem == subsystem )
	    {
		ret = TRUE;
		break;
	    }
	}
    }
    else
    {
	//
	// Check each subsystem in the list to see if the command server is
	// there. If it is there, the command applies to the command server.
	//

	if ( ndims == 0 )
	{
	    if ( strcmp( (char*) subsystemList, "" ) == 0 ||
		    strcmp( (char*) subsystemList, subsystem ) == 0 )
	    {
		ret = TRUE;
	    }
	}
	else if ( ndims == 1 )
	{
	    for ( i = 0; i < dims[0]; i++ )
	    {
		if ( strcmp( subsystemList[i], subsystem ) == 0 )
		{
		    ret = TRUE;
		    break;
		}
	    }
	}
    	else
	{
	    //
	    // Error, subsystem list should not have more than
	    // two dimentions.
	    //

	    ret = FALSE;
	    status.E_ATTRIB_DIMS( status, "subsystem" );
	}
    }
    return ( ret );
}
    

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::attribFind
//
// INVOCATION:
// cCmdCommandInfo::attribFind( attribName, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) attribName (const char *) Name of the attribute to look for
// (!) status (cCmdStatus &) Fucntion return status.
//
// FUNCTION VALUE:
// (cCmdAttributeInfo *) The attribute located.
//
// PURPOSE:
// Given an attribute name, find the attribute in a commands attributes
//
// DESCRIPTION:
// Use the STL find function on the list of attributes.
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

cCmdAttributeInfo
		*cCmdCommandInfo::attribFind
(
    const char	*attribName,	// (in)  Name of the attribute to look for.
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    cCmdAttributeInfo::constIterator
		i;

    i = cciAttributeList.find( std::string( attribName ) );
    if ( i == cciAttributeList.end() )
    {
	return( NULL );
    }
    else
    {
	return( (*i).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::cCmdCommandInfo
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (const char **) List of tokens on the configuration line.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cCmdCommandInfo class
//
// DESCRIPTION:
// - Parse the configuration information passed in the arguments and set
//   the attributes of the object.
// - Add the object to the STL map containing the list of all objects.
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

		cCmdCommandInfo::cCmdCommandInfo
(
    int		numTokens,	// (in)  Number of tokens on the configuration 
				//	line.
    const char	**tokens	// (in)  List of tokens on the configuration 
				//	line.
)
{
    int		i;
    cCmdSubsystem::iterator
    		j;		// Iterator into the subsystem list.
    cCmdSubsystem *pSubsystem;	// Pointer to a subsystem structure.
    cCmdStatus	status;		// Status value.


    cciName = strsav( tokens[1] );
    if ( strcmp( tokens[2], "yes" ) == 0 )
    {
	cciAbortable = true;
    }
    else
    {
	cciAbortable = false;
    }

    if ( strcmp( tokens[3], "yes" ) == 0 )
    {
	cciSubsystemAttrib = true;
    }
    else
    {
	cciSubsystemAttrib = false;
    }

    for ( i = 4; i < numTokens; i++ )
    {
	if ( strcmp( tokens[i], "*" ) == 0 )
	{
	    //
	    // Command is sent to all subsystems.
	    //

	    for ( j = cCmdSubsystem::begin(); j != cCmdSubsystem::end(); j++ )
	    {
		subsystemAdd( (cCmdSubsystem *) (*j), status );
	    }
	}
	else
	{
	    pSubsystem = cCmdSubsystem::subsystemFind( tokens[i], status );
	    if ( status.ok() )
	    {
		subsystemAdd( pSubsystem, status );
	    }
	    else
	    {
		status.E_SUBSYSTEM_NAME( status, tokens[i] );
	    }
	}
    }

    cciCommandMap[ std::string( cciName ) ] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::~cCmdCommandInfo
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
// Destructor for the cCmdCommandInfo class
//
// DESCRIPTION:
// Delete all resources allocated for a command.
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

		cCmdCommandInfo::~cCmdCommandInfo
(
    void
)
{
    while ( cciAttributeList.size() != 0 )
    {
	delete ((*cciAttributeList.begin()).second);
	cciAttributeList.erase( cciAttributeList.begin() );
    }

    cciCommandMap.erase( cciName );
    free( cciName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::attribute
//
// INVOCATION:
// n.a. Called as a callback from the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (const char **) List of tokens on the configuration line.
//
// FUNCTION VALUE:
// (boolean) True if the line parsed ok, false otherwise.
//
// PURPOSE:
// Add an attribute to a command
//
// DESCRIPTION:
// - Find the command with which the attribute is associated.
// - Create a new attribute object.
// - Add the attribute to the command.
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

boolean		cCmdCommandInfo::attribute
(
    int		numTokens,	// (in)  Number of tokens on the configuration 
				//	line.
    const char	**tokens	// (in)  List of tokens on the configuration 
				//	line.
)
{
    boolean	ret;
    cCmdCommandInfo		// Pointer to the attributes command.
    		*pCmd;
    cCmdAttributeInfo		// Pointer to the new attribute.
    		*pAttribute;
    cCmdStatus	status;		// Error status value.
    boolean	optional;	// Is the attribute optional.
    DHS_DATA_TYPE		// type of the parameter.
    		type;


    if ( numTokens == 5 )
    {
	check( pCmd = cCmdCommandInfo::find( tokens[1], status ), 
		status, return( FALSE ) );
	type = cDhsAttrib::typeMap( tokens[3] );
	if ( pCmd->attribFind( tokens[2], status ) != NULL )
	{
	    status.E_DUP_ATTRIB( status, tokens[2], tokens[1] );
	    ret = FALSE;
	}
	else if ( ( type = cDhsAttrib::typeMap( tokens[3] ) ) == DHS_DT_NONE )
	{
	    status.E_ATTRIB_TYPE( status, tokens[3], tokens[2], tokens[1] );
	    ret = FALSE;
	}
	else if ( ! str2bool( tokens[4], &optional ) )
	{
	    status.E_ATTRIB_OPTIONAL( status, tokens[4], tokens[2], 
		    tokens[1] );
	    ret = FALSE;
	}
	else
	{
	    checkNull( pAttribute = new cCmdAttributeInfo( tokens[2], 
		    (bool) optional, type ), status, return( FALSE ) );
	    check( pCmd->attributeAdd( pAttribute, status ), status, 
		    return( FALSE ) );
	    ret = TRUE;
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::attributeAdd
//
// INVOCATION:
// commandInfo.attributeAdd( pAttrInfo, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAttrInfo (cCmdAttributeInfo *) The attribute to add to the command.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a new attribte to a command
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

void		cCmdCommandInfo::attributeAdd
(
    cCmdAttributeInfo		// (in)  The attribute to add to the command.
		*pAttrInfo,
    cCmdStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );
    cciAttributeList[std::string(pAttrInfo->name())] = pAttrInfo;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::command
//
// INVOCATION:
// n.a. Called as a callback from the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (const char **) List of tokens on the configuration line.
//
// FUNCTION VALUE:
// (boolean) True if the line parsed ok, false otherwise.
//
// PURPOSE:
// Add a new command object for each command in the configuration file
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

boolean		cCmdCommandInfo::command
(
    int		numTokens,	// (in)  Number of tokens on the configuration 
				//	line.
    const char	**tokens	// (in)  List of tokens on the configuration 
				//	line.
)
{
    boolean	ret;
    cCmdStatus	status;		// Error status value.
    cCmdCommandInfo		// Command information pointer.
    		*pCommand;


    if ( numTokens >= 4 )
    {
	if ( cciCommandMap.find( tokens[1] ) == cciCommandMap.end() )
	{
	    checkNull( pCommand = new cCmdCommandInfo( numTokens, tokens ), 
		    status, return( FALSE ) );
	    ret = TRUE;
	}
	else
	{
	    status.E_DUP_CMD( status, tokens[1] );
	    ret = FALSE;
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::config
//
// INVOCATION:
// cCmdCommandInfo::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the command configuration information
//
// DESCRIPTION:
// Use the CADC configuration library to read the configuration.
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

void		cCmdCommandInfo::config
(
    cCmdStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    exit( status );

    checkConfig( config_get( "command", (boolean (*) (int, char **))command ), status, return );
    checkConfig( config_get( "attribute", (boolean (*) (int, char **))attribute ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::csExit
//
// INVOCATION:
// command.csExit( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cDhsServerCommand &) The command received.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Cause the command server to exit.
//
// DESCRIPTION:
// Set the retry flag and stop the event loop.  This will result
// in the command server exiting.
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

void		cCmdCommandInfo::csExit
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    if ( appliesToCmd( command, PREFIX, status ) )
    {
	cCmdDhs::retry( false );
	cDhs::stop( dhsStatus );
    }
    else
    {
	cCmdDhs::enable();
    }
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::debugSet
//
// INVOCATION:
// command.debugSet( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cDhsServerCommand &) The command received.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the debug level from the command attributes
//
// DESCRIPTION:
// - If the command applies to the command server:
// -  Get the new debug level.
// -  Set the debug level of the cCmdStatus class.
// -  Set the debug level of the DHS library.
// -  Update the debug level shown by the status server.
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

void		cCmdCommandInfo::debugSet
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    bool	applies( true );
    cDhsAttrib 	attrib;		// The level attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*level;		// The new debug level.


    checkStat( status, return );


    if ( appliesToCmd( command, PREFIX, status ) )
    {
	//
	// If the command applies to the command server, get the value of
	// the level attribute.
	//

	dhsStatus = DHS_S_SUCCESS;
	attrib = command.find( "level", dhsStatus );
	attrib.info( (void **) &level, dhsStatus );


	//
	// Set the debug level for the application.
	//

	status.debugLevel( level );

	
	//
	// Set the DHS library debug level to the appropriate value.
	//

	if ( strcmp( level, "NONE" ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_OFF, dhsStatus );
	}
	else if ( strcmp( level, "MIN" ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_ON, dhsStatus );
	}
	else if ( strcmp( level, "FULL" ) == 0 )
	{
	    cDhs::debugLevel( DHS_DEBUG_FULL, dhsStatus );
	}
	else
	{
	    status.E_DEBUG_CMD( status, "Invalid level" );
	}

	if ( status.ok() )
	{
	    //
	    // Log the change in the debug level.
	    //

	    status.S_DEBUG_LEVEL( status, level );


	    //
	    // Update the debug status field in the SAD.
	    //

	    status.update( "debug", level, status );
	    status.flush( status );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::exec
//
// INVOCATION:
// command.exec( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cCmdServerCmd &) The DHS server command object.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Process a command
//
// DESCRIPTION:
// Verify the command attributes, update the command status to busy, 
// forward the command to any appropriate subsystems, and then do any
// processes required from the command server.
//
// Note that this function could be active by more than one thread
// for the same cCmdCommandInfo object.  This means this function
// must be re-entrant.  The constness of this function helps to ensure
// this since it ensures that none of the threads will modify the 
// cCmdCommandInfo object.
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

void		cCmdCommandInfo::exec
(
    cCmdServerCmd		// (in)  The server command.
		&command,
    cCmdStatus	&status		// (mod) The function return status.
) const
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cCmdStatus	tStatus( status );


    checkStat( status, return );
    status.displayStop();

    verify( command, status );
    if ( status.ok() )
    {
	//
	// Set the command state to busy.
	//

	command.response( DHS_CS_BUSY, dhsStatus );


	//
	// Send the command to the appropriate subsystems.
	//

	if ( strcmp( command.name(), "reset" ) == 0 ||
		strcmp( command.name(), "init" ) == 0 )
	{
	    cCmdDhs::disable();
	    riReSend( command, status );
	} else if ( !cciSimulate ||
		strcmp( command.name(), "simulateLevel" ) == 0 )
	{
	    reSend( command, status );
	} 


	//
	//  These commmands require specific action from the command server.
	//

	if ( cciSimulate && strcmp( command.name(), "simulateLevel" ) != 0 )
	{
	    // Empty
	}
	else if ( strcmp( command.name(), "debugLevel" ) == 0 )
	{
	    debugSet( command, tStatus );
	}
	else if ( strcmp( command.name(), "resetHealth" ) == 0 )
	{
	    resetHealth( command, tStatus );
	}
	else if ( strcmp( command.name(), "simulateLevel" ) == 0 )
	{
	    simulateSet( command, tStatus ); 
	} else if ( strcmp( command.name(), "test"  ) == 0 )
	{
	    test( command, tStatus );
	}
	

	//
	// Check to see if the command succeeded.
	//

	if ( status.ok() && tStatus.ok() )
	{
	    command.response( DHS_CS_DONE, dhsStatus );
	}
	else
	{
	    if ( ! tStatus.ok() )
	    {
		if ( status.ok() )
		{
		    status = tStatus;
		}
		else
		{
		    status += tStatus;
		}
	    }
	    command.response( DHS_CS_ERROR, status.message(), dhsStatus );
	}


	//
	// These command require action from the command server, but cannot
	// be processed before the last response to the client since they
	// shut down the DHS library event loop.
	//

	if ( strcmp( command.name(), "init" ) == 0 || 
		strcmp( command.name(), "reset" ) == 0 )
	{
	    reset( command, tStatus );
	}
	else if ( strcmp( command.name(), "cmdSystemExit" ) == 0 )
	{
	    systemExit( tStatus );
	}
	else if ( strcmp( command.name(), "exit" ) == 0 )
	{
	    csExit( command, tStatus );
	}
    }
    else
    {
	command.response( DHS_CS_ERROR, status.message(), dhsStatus );
    }
    status.display();
    status.displayStart();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::exit
//
// INVOCATION:
// cCmdCommandInfo::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all commands
//
// DESCRIPTION:
// - Delete each of the cCmdCommandInfo objects in the STL map containing
//   all of the commands.
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

void		cCmdCommandInfo::exit
(
    cCmdStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    while ( cciCommandMap.size() != 0 )
    {
	delete ((*cciCommandMap.begin()).second);
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::find
//
// INVOCATION:
// cCmdCommandInfo::find( commandName, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) commandName (const char *) Name of the command to look for
// (!) status (cCmdStatus &) Fucntion return status.
//
// FUNCTION VALUE:
// (cCmdCommandInfo *) The command located.
//
// PURPOSE:
// Given the command name, find the command in the list of commands
//
// DESCRIPTION:
// - Search for the command name in the STL command map.
// - Return the command or null.
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

cCmdCommandInfo	*cCmdCommandInfo::find
(
    const char	*commandName,	// (in)  The name of the command to locate.
    cCmdStatus	&status		// (mod) Function return status.
)
{
    iterator		i;

    i = cciCommandMap.find( std::string( commandName ) );
    if ( i == cciCommandMap.end() )
    {
	status.E_CMD_NAME( status, commandName );
	return( NULL );
    }
    else
    {
	return( (*i).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::reSend
//
// INVOCATION:
// cmdinfo.reSend( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) serverCommand (cCmdServerCmd &) The DHS server command object.
// (!) status (cCmdStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a command on to the subsystems
//
// DESCRIPTION:
// - Get the list of subsystems to send the command to.
// - Forward the cmmand to each subsystem in the list.
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

void		cCmdCommandInfo::reSend
(
    cCmdServerCmd		// (in)  The server command.
		&serverCommand,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    bool	error = false;
    cDhsSubsystem::iterator
    		i;
    cCmdSubsystem
    		*pSubsystem;
    cDhsAttrib	subsystemList;
    cCmdStatus	tStatus( status );
    int		ndims;
    ulong	dims[7];
    char	**ptr;


    checkStat( status, return );
    tStatus.displayStop();
    ndims = -1;
    

    if ( cciSubsystemAttrib )
    {
	subsystemList = serverCommand.find( "subsystem", dhsStatus );
	dhsStatus = DHS_S_SUCCESS;
    }


    //
    // Send the command to every subsystem except the command server.
    //

    if ( ! ( subsystemList == DHS_AV_ID_NULL ) )
    {
	checkDhs( subsystemList.info( ndims, dims, (void **) &ptr, dhsStatus ),
		dhsStatus, status, return );	
    }


    if ( subsystemList == DHS_AV_ID_NULL ||
	( ndims == 0 && streq( (char*)ptr, "" ) ) )
    {
	//
	// There is no subsystem list with the command, so loop through
	// the complete list of subsystems associated with the command,
	// sending the command to each subsystem.
	//

	for ( i = (cDhsSubsystem::iterator&)cciSubsystemList.begin(); i != cciSubsystemList.end(); i++ )
	{
	    pSubsystem = (cCmdSubsystem *) *i;

	    if (! ( *pSubsystem == PREFIX ) )
	    {
		if ( serverCommand.forward( pSubsystem->connect(), tStatus ) )
		{
		    tStatus.E_FORWARD_CMD( status, name(), pSubsystem->name() );
		}
		if ( ! tStatus.ok() )
		{
		    if ( status.ok() )
		    {
			status = tStatus;
		    }
		    else
		    {
			status += tStatus;
		    }
		}
		tStatus( tStatus.S_SUCCESS );
	    }
	}
    }
    else if ( ndims == 0 )
    {
	//
	// There is a subysytem list, with one entry, in the command
	// attributes, so send the subcommand to that subsystem.
	//

	pSubsystem = cCmdSubsystem::subsystemFind( (char*)ptr, status );
	if ( pSubsystem == NULL )
	{
	    tStatus.E_SUBSYSTEM_NAME( status, (char*)ptr );
	}
	else if ( ! ( *pSubsystem == PREFIX ) )
	{
	    if ( serverCommand.forward( pSubsystem->connect(), tStatus ) )
	    {
		tStatus.E_FORWARD_CMD( status, name(), pSubsystem->name() );
	    }
	    if ( ! tStatus.ok() )
	    {
		if ( status.ok() )
		{
		    status = tStatus;
		}
		else
		{
		    status += tStatus;
		}
	    }
	    tStatus( tStatus.S_SUCCESS );
	}
    }
    else if ( ndims == 1 )
    {
	//
	// There is a subysytem list in the command so loop through
	// the command's subsystem list, sending a subcommand to each 
	// subsystem.
	//
	    
	for ( int j = 0; j < dims[0]; j++, ptr++ )
	{
	    pSubsystem = cCmdSubsystem::subsystemFind( *ptr, status );
	    if ( pSubsystem == NULL )
	    {
		tStatus.E_SUBSYSTEM_NAME( status, (char*)ptr );
	    }
	    else if ( ! ( *pSubsystem == PREFIX ) )
	    {
		if ( serverCommand.forward( pSubsystem->connect(), tStatus ) )
		{
		    tStatus.E_FORWARD_CMD( status, name(), pSubsystem->name() );
		}
	    }
	    if ( ! tStatus.ok() )
	    {
		if ( status.ok() )
		{
		    status = tStatus;
		}
		else
		{
		    status += tStatus;
		}
	    }
	    tStatus( tStatus.S_SUCCESS );
	}
    }
    else
    {
	status.E_ATTRIB_DIMS( status, "subsystem" );
	return;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::riReSend
//
// INVOCATION:
// cmdinfo.riReSend( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) serverCommand (cCmdServerCmd &) The DHS server command object.
// (!) status (cCmdStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a reset or init  command on to the subsystems
//
// DESCRIPTION:
// Either send the init command to all of the subsystems listed in the
// given subsytem list or if no list is given to all of the subsystems,
// with the exception of the command server.
//
// If the status server is in the subsystem list forward the reset command
// to it first.  Try and reconnect to the status server, if a connection
// can be established then the status server is ready.
//    
// Then forward the reset command to the remaining subsystems, attempt to
// connect to each one after forwarding the command.  
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

void	cCmdCommandInfo::riReSend
(
    cCmdServerCmd		// (in)  The server command.
		&serverCommand,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    bool	error = false;
    cDhsSubsystem::iterator
    		i;
    cCmdSubsystem
    		*pSubsystem;
    cDhsAttrib	subsystemList;
    cCmdStatus	tStatus( status );
    int		ndims;
    ulong	dims[7];
    char	**ptr;


    checkStat( status, return );
    tStatus.displayStop();
    

    if ( cciSubsystemAttrib )
    {
	subsystemList = serverCommand.find( "subsystem", dhsStatus );
	dhsStatus = DHS_S_SUCCESS;
    }

    //
    // Send the command to every subsystem except the cmomand server.
    //
    // For some unknown reason things hang if there isn't a 3 sec.
    // delay before attempting to reconnect.
    //

    if ( ! ( subsystemList == DHS_AV_ID_NULL ) )
    {
	checkDhs( subsystemList.info( ndims, dims, (void **) &ptr, dhsStatus ),
		dhsStatus, status, return );	
    }


    if ( subsystemList == DHS_AV_ID_NULL ||
	( ndims == 0 && streq( (char*)ptr, "" ) ) )
    {
	//
	// There is no subsystem list with the command, so loop through
	// the complete list of subsystems associated with the command,
	// sending the command to each subsystem.
	//
	//

	cciMutex.lock();
	for ( i = (cDhsSubsystem::iterator&)cciSubsystemList.begin(); i != cciSubsystemList.end(); i++ )
	{
	    pSubsystem = (cCmdSubsystem *) *i;
	    if ( ! ( *pSubsystem == PREFIX ) )
	    {
		if ( ! serverCommand.riForward( pSubsystem->connect(),
			tStatus ) )
		{
		    sleep( 3 );
		    pSubsystem->connect( status );
		}
		else
		{
		    tStatus.E_FORWARD_CMD( status, name(), pSubsystem->name() );
		}


		//
		// If it is the status server we need to initialize
		// out status class.
		//
		
		if ( *pSubsystem == STA_PREFIX )
		{
		    cCmdStatus::init( pSubsystem->connect(), PREFIX, status );
		}
 	    }
	    if ( ! tStatus.ok() )
	    {
		if ( status.ok() )
		{
		    status = tStatus;
		}
		else
		{
		    status += tStatus;
		}
	    }
	    tStatus( tStatus.S_SUCCESS );
	}
	cciMutex.unlock();
    }
    else if ( ndims == 0 )
    {
	//
	// There is a subysytem list, with one entry, in the command
	// attributes, so send the subcommand to that subsystem.
	//

	cciMutex.lock();
	pSubsystem = cCmdSubsystem::subsystemFind( (char*)ptr, status );
	if ( pSubsystem == NULL )
	{
	    tStatus.E_SUBSYSTEM_NAME( status, (char*)ptr );
	}
	else if( pSubsystem != NULL && ! ( *pSubsystem == PREFIX ) )
	{
	    if ( !serverCommand.riForward( pSubsystem->connect(), tStatus ) )
	    {
		sleep( 3 );
		pSubsystem->connect( status );
	    }
	    else	
	    {	
		tStatus.E_FORWARD_CMD( status, name(), pSubsystem->name() );
	    }

	    
	    //
	    // If it is the status server we need to initialize
	    // out status class.
	    //
		
	    if ( *pSubsystem == STA_PREFIX )
	    {
		cCmdStatus::init( pSubsystem->connect(), PREFIX, status );
	    }
	}
	
	cciMutex.unlock();
    }
    else if ( ndims == 1 )
    {

	//
	// There is a subysytem list in the command attributes, so loop through
	// the command's subsystem list, sending a subcommand to each 
	// subsystem. 
	//

	cciMutex.lock();
	for ( int j = 0; j < dims[0]; j++, ptr++ )
	{
	    pSubsystem = cCmdSubsystem::subsystemFind( *ptr, tStatus );
	    if ( pSubsystem == NULL )
	    {
		tStatus.E_SUBSYSTEM_NAME( status, (char*)ptr );
	    }
	    else if ( ! ( *pSubsystem == PREFIX ) )
	    {
		if ( !serverCommand.riForward( pSubsystem->connect(), 
			tStatus ) )
		{
		    sleep( 3 );
		    pSubsystem->connect( status );
		}
		else
		{
		    tStatus.E_FORWARD_CMD( status, name(), pSubsystem->name() );
		}


		//
		// If it is the status server we need to initialize
		// out status class.
		//
		
		if ( *pSubsystem == STA_PREFIX )
		{
		    cCmdStatus::init( pSubsystem->connect(), PREFIX, status );
		}
	    }
	    if ( ! tStatus.ok() )
	    {
		if ( status.ok() )
		{
		    status = tStatus;
		}
		else
		{
		    status += tStatus;
		}
	    }
	    tStatus( tStatus.S_SUCCESS );
	}
	cciMutex.unlock();
    }
    else
    {
	status.E_ATTRIB_DIMS( status, "subsystem" );
	return;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::reset
//
// INVOCATION:
// command.reset( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cDhsServerCommand &) The command received.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Cause the command server to be reset.
//
// DESCRIPTION:
// Set the retry flag and stop the event loop.  This will result
// in the command server being re-initialized in the loop in the
// main function.  This has to be done after to command responses
// since the IMP connections won't be available after the dhsStop.
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

void		cCmdCommandInfo::reset
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    if ( appliesToCmd( command, PREFIX, status ) )
    {
	cCmdDhs::retry( true );
	cDhs::stop( dhsStatus );
    }
    else
    {
	cCmdDhs::enable();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::resetHealth
//
// INVOCATION:
// command.resetHealth( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cDhsServerCommand &) The command received.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the health of the command server to GOOD.
//
// DESCRIPTION:
// Print a status messsage and send a message to the status server
// which indicates that the command server's health should be set to GOOD.
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

void		cCmdCommandInfo::resetHealth
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    checkStat( status, return );


    if ( appliesToCmd( command, PREFIX, status ) )
    {
	status.S_RESET_HEALTH( status, "GOOD" );
	status.resetHealth( status );
    }

}



//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::simulateSet
//
// INVOCATION:
// command.simulateSet( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cDhsServerCommand &) The command received.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the simulate level from the command attributes
//
// DESCRIPTION:
// - If the command applies to the command server:
// -  Get the new simulate level.
// -  Set the simulate level associated with this class.
// -  Update the simulate level shown by the DHS Status Server.
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

void		cCmdCommandInfo::simulateSet
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    cDhsAttrib 	attrib;		// The level attribute.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*level;		// The new debug level.

    checkStat( status, return );


    if ( appliesToCmd( command, PREFIX, status ) )
    {
	//
	// If the command applies to the command server, get the value of
	// the level attribute.
	//

	dhsStatus = DHS_S_SUCCESS;
	attrib = command.find( "level", dhsStatus );
	attrib.info( (void **) &level, dhsStatus );


	//
	// Set the simulate level for the application.  None and simulate
	// are the same since in full simulate, the applications do the
	// simulation, and so the command server functions normally.
	//

	if ( strcmp( level, "NONE" ) == 0 || strcmp( level, "FULL" ) == 0 )
	{
	    simulate( false );
	}
	else if ( strcmp( level, "VSM" ) == 0 || strcmp( level, "FAST" ) == 0 )
	{
	    simulate( true );
	}
	else
	{
	    status.E_SIMULATE_CMD( status, "Invalid level" );
	}

	if ( status.ok() )
	{
	    //
	    // Log the change in the debug level.
	    //

	    status.S_SIMULATE_LEVEL( status, level );


	    //
	    // Update the simulate status field in the SAD.
	    //

	    status.update( "simulate", level, status );
	    status.flush( status );
	}
    }
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::test
//
// INVOCATION:
// command.test( command, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cDhsServerCommand &) The command received.
// (!) status (cDhsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Test the command server.
//
// DESCRIPTION:
// If the test command issued applies to the command server then
// ping each of the subsystems that the command server is connected
// if any of them fail to respond then the test has failed.  
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

void		cCmdCommandInfo::test
(
    cDhsServerCommand&		// (in)  The command received.
    		command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    boolean	success;	// True if test succeeds.
    

    checkStat( status, return );

    status.update( "state", "TESTING", status );
    status.flush( status );
    status.S_TESTING( status );
    
    if ( appliesToCmd( command, PREFIX, status ) )
    {
	
	//
	// Ping each subsystem the command server is connected to.
	// If none of them time out the test succeeds.
	//

	cCmdSubsystem::pingAll( &success );
	if ( success == TRUE )
	{
	    status.S_TEST_SUCCESS( status );
	}
	else
	{
	    status.S_TEST_FAILURE( status );
	}
    }

    status.update( "state", "RUNNING", status );
    status.flush( status );

    return;   
}



//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::subsystemAdd
//
// INVOCATION:
// cCmdCommandInfo::subsystemAdd( subsystem, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) subsystem (cCmdSubsystem *) Subsystem to add to the list.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a subsystem to the list of subsystems associated with a command
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

void		cCmdCommandInfo::subsystemAdd
(
    cCmdSubsystem		// (in)  The subsystem to add to the list.
		*subsystem,
    cCmdStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );
    cciSubsystemList.push_back( subsystem );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::systemExit
//
// INVOCATION:
// systemExit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send an exit command to all subsystems
//
// DESCRIPTION:
// Send an exit command to all subsystems and wait for it to complete.
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

void		cCmdCommandInfo::systemExit
(
    cCmdStatus	&		// (mod) Function return status.
) const
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    cCmdDhs::retry( false );
    cCmdSubsystem::subsystemExit( true );
    cDhs::stop( dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdCommandInfo::verify
//
// INVOCATION:
// commandInfo.verify( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (cCmdServerCmd &) The DHS server command object.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Verify the parameters of the command are correct.
//
// DESCRIPTION:
// - Verify that all required attributes are in the command
// - Verify that no unknown parameters are in the command unless they
//   are allowed by the command.
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

void		cCmdCommandInfo::verify
(
    const cDhsServerCommand	// (in)  The server command.
		&command,
    cCmdStatus	&status		// (mod) Function return status.
) const
{
    cDhsAttrib	attrib;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cCmdAttributeInfo::constIterator	// Iterator in the attribute list.
		i;
    int		j;			// Iterator in the command attributes.
    char	*name;			// Attribute name;
    DHS_DATA_TYPE			// Attribute data type;
		type;
    bool	wildCard;		// Is there a wildcard attribute?
    cCmdAttributeInfo			// Command server attirbute info.
    		*pAttribInfo;
    cCmdStatus	s;


    checkStat( status, return );


    //
    //  Verify that all required attributes are in the command.
    //

    
    for ( i = cciAttributeList.begin(); i != cciAttributeList.end(); i++ )
    {
	if ( ! (*i).second->optional() )
	{
	    command.find( (*i).second->name(), dhsStatus );
	    if ( dhsStatus == DHS_S_NO_ATTRIB )
	    {
		status.E_ATTRIB_MISSING( status, (*i).second->name(), 
			command.name() );
		dhsStatus = DHS_S_SUCCESS;
	    }
	}
    }


    //
    // Verify that there are no unknown attributes in the command (unless
    // expicitly allowed).
    //

    if ( cciAttributeList.find( "*" ) == cciAttributeList.end() )
    {
	wildCard = false;
    }
    else
    {
	wildCard = true;
    }

    for ( j = 0; status.ok(); j++ )
    {
	attrib = command.find( j, dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    break;
	}

	if ( ! wildCard )
	{
	    attrib.info( name, type, dhsStatus );
	    pAttribInfo = attribFind( name, status );
	    if ( pAttribInfo == NULL )
	    {
		status.E_ATTRIB_NAME( status, name, this->name() );
	    }
	    else if ( status.ok() && type != pAttribInfo->type() )
	    {
		status.E_ATTRIB_TYPE( status, name, command.name() );
	    }
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdAttributeInfo::cCmdAttributeInfo
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the attribute.
// (>) optional (bool) Is the attribute optional?
// (>) type (DHS_DATA_TYPE) Type of the attribute.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cCmdAttributeInfo class
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

		cCmdAttributeInfo::cCmdAttributeInfo
(
    const char	*name,		// (in)  Name of the attribute.
    bool	optional,	// (in)  Is the attribute optional?
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type
)
{
    caiName = strsav( name );

    caiType = type;

    caiOptional = optional;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdAttributeInfo::~cCmdAttributeInfo
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
// Destructor for the cCmdAttributeInfo class
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

		cCmdAttributeInfo::~cCmdAttributeInfo
(
    void
)
{
    free( caiName );
}
