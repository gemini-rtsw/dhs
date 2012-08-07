static char rcsid[] = "$Id: subsystem.C,v 1.1.1.1 2002-11-24 20:25:32 brighton Exp $";
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
// - Open the configuration file.
// - Configure the cDhs class.
// - Configure the cCmdSubsystem class.
// - Configure the cCmdCommandInfo class.
// - Close the configuration file.
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsCommand/src/subsystem.C
//
// PURPOSE:
// Contains the implementation of the cCmdSubsystem class.
//
// METHOD NAME(S)
// cCmdSubsystem::cCmdSubsystem - Constructor for the cCmdSubsystem class.
// cCmdSubsystem::~cCmdSubsystem - Destructor for the cCmdSubsystem class.
// cCmdSubsystem::connect - Connect to the subsystem.
// cCmdSubsystem::connectAll - Connect to all known subsystems.
// cCmdSubsystem::config - Read the subsystem information from the 
//		configuration file.
// cCmdSubsystem::exitAll - Shut down all subsystems.
// cCmdSubsystem::pingAll - Ping all subsystems.
// cCmdSubsystem::subsystem - Parse a subsystem line from the configuration
//		file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.9  1998/05/05 16:42:19  cockayne
// Added code to ignore E_INIT errors on subsystem connection.
//
// Revision 1.8  1998/04/16 21:52:37  nhill
// Updated the documentation.
// Changed to use the latest version of the cDhsStatus class.
//
// Revision 1.7  1998/03/13 22:03:12  jaeger
// Added pingAll and exitAll methods.  Changed connectAll so it attempts
// connection if CMD isn't exiting.  Also changed the connect messages
//
// Revision 1.6  1997/10/03 23:01:58  nhill
// Changed to work with the object space stl library.
//
// Revision 1.5  1997/06/13 20:46:19  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.4  1997/05/01 17:50:51  nhill
// Added the subsystemExit flag to indicate the subsytems should be told to exit.
//
// Revision 1.3  1997/04/28 22:53:47  nhill
// Added shutdown of client systems on exit.
//
// Revision 1.2  1997/04/24 19:46:48  nhill
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

#include <unistd.h>
#include <stdlib.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
}

#include "globals.H"
#include "cmdSubsystem.H"

bool	cCmdSubsystem::cssSubsystemExit = false;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::cCmdSubsystem
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) subsystem (const char *) Name of the subsystem.
// (>) address (const char *) Address of the subsystem.
// (>) name (const char *) IMP name of the subsystem.
// (>) commandLine (const char *) Command line to run the subsystem.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cCmdSubsystem class
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

		cCmdSubsystem::cCmdSubsystem
(
    const char	*subsystem,	// (in)  Name of the subsystem.
    const char	*address,	// (in)  Address of the sybsystem.
    const char	*name,		// (in)  IMP name of the subsystem.
    const char	*commandLine	// (in)  Command line to start the subsystem.
) : cDhsSubsystem( subsystem )
{
    cssAddress = strsav( address );
    cssServerName = strsav( name );
    cssCommandLine = strsav( commandLine );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::~cCmdSubsystem
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
// Destructor for the cCmdSubsystem class
//
// DESCRIPTION:
// Free the resources allocated to the object.
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

		cCmdSubsystem::~cCmdSubsystem
(
    void
)
{
    free( cssAddress );
    free( cssServerName );
    free( cssCommandLine );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::connect
//
// INVOCATION:
// subsystem.connect( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Attempt to connect to the server or start the server if necessary
//
// DESCRIPTION:
// - Attempt 10 times to connect to the subsystem, sleeping 2 seconds
//   between each unsuccessful attempt.
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

void		cCmdSubsystem::connect
(
    cCmdStatus	&status,		// (mod) Function return status.
    int retries,				// Maximum number of connection retries
    int retryDelay				// Delay (in seconds) between connection retries
)
{
    DHS_STATUS	dhsStatus;
    int		i;


    checkStat( status, return );


    if ( !connected() )
    {
	//  
	// Attempt to open the connection.
	//

	dhsStatus = DHS_S_SUCCESS;
	for ( i =0; (retries == 0 || i < retries) && !cCmdSubsystem::subsystemExit(); i++ )
	{
	    checkDhs( cssConnect.open( cssAddress, cssServerName, dhsStatus ),
		    dhsStatus, status, VOID );
	    if ( dhsStatus == DHS_S_SUCCESS )
	    {
		break;
	    }
	    else
	    {
		sleep(retryDelay);
		dhsStatus = DHS_S_SUCCESS;
	    }
	}
	
	if ( connected() )
	{
	    status.S_CONNECTED( status, cssServerName, cssAddress );
	}
	else
	{
	    status.E_CONNECTING( status, cssServerName, cssAddress );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::connectAll
//
// INVOCATION:
// cCmdSubsystem::connectAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Connect to all subsystems
//
// DESCRIPTION:
// Description of the algorithm.
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

void		cCmdSubsystem::connectAll
(
    cCmdStatus	&status		// (mod) Function return status.
)
{
    iterator	i;		// Iterator through the list of subsystems.
    checkStat( status, return );

    for ( i = begin(); i != end(); i++ )
    {
	if ( ! ( *(*i) == PREFIX  ) )
	{
	    ((cCmdSubsystem *) (*i))->connect( status, 0, 5);
	    if ( status == status.E_INIT )
	    {
		status.S_SUCCESS( status );
	    }
	}

	if ( *(*i) == STA_PREFIX )
	{
	    cCmdStatus::init( ( (cCmdSubsystem *) (*i))->connect(), 
		    PREFIX, status );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::config
//
// INVOCATION:
// cCmdSubsystem::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the subsystem information from the configuration file
//
// DESCRIPTION:
// Use the configuration library to read the list of subsystems.
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

void		cCmdSubsystem::config
(
    cStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    exit( status );

    checkConfig( config_get( "subsystem", subsystem ), status, return );
}
    

 //
 //***********************************************************************
 //+
 // FUNCTION NAME:
 // cCmdSubsystem::exitAll
 //
 // INVOCATION:
 // cCmdSubsystem::exitAll( status );
 //
 // PARAMETERS: (">" input, "!" modified, "<" output)
 // (!) status (cCmdStatus &) Function return status.
 //
 // FUNCTION VALUE:
 // None.
 //
 // PURPOSE:
 // Shut down all subsystems
 //
 // DESCRIPTION:
 // For each subsystem, except the command server, apply the "exit" command.
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
 
 void          cCmdSubsystem::exitAll
 (
     cCmdStatus        &status         // (mod) Function return status.
 )
 {
     DHS_STATUS            dhsStatus;
     iterator      i;          // Iterator through the list of subsystems.
     cDhsClientCommand         // Ping subcommand.
                   pCmd( "exit" );     
     
 
     for ( i = begin(); i != end(); i++ )
     {
       if ( ! ( *(*i) == PREFIX ) )
       {
           dhsStatus = DHS_S_SUCCESS;
                   
           checkDhs( pCmd.apply( ((cCmdSubsystem *) (*i))->connect(), 
                   dhsStatus ), dhsStatus, status, VOID );
           if ( dhsStatus == DHS_S_SUCCESS )
           {
               checkDhs( pCmd.wait( dhsStatus ), dhsStatus, status, VOID );
           }
       }
     }
     return;
 }

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::pingAll
//
// INVOCATION:
// cCmdSubsystem::pingAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Ping to all subsystems
//
// DESCRIPTION:
// For each subsystem, except the command server, apply the "ping" command.
// If an error occurs for any of the subsystem then set "success" to FALSE.
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

void		cCmdSubsystem::pingAll
(
    boolean	*success	// True if all subsystems return from
    		       		// the ping command.	
)
{
    DHS_STATUS	    dhsStatus;
    iterator	    i;		// Iterator through the list of subsystems.
    cDhsClientCommand		// Ping subcommand.
		    pCmd( "ping" );	
    

    *success = TRUE;
    for ( i = begin(); i != end(); i++ )
    {
	if ( ! ( *(*i) == PREFIX ) )
	{
	    dhsStatus = DHS_S_SUCCESS;
	    	    
	    pCmd.apply( ((cCmdSubsystem *) (*i))->connect(), dhsStatus );
	    if ( dhsStatus == DHS_S_SUCCESS )
	    {
		pCmd.wait( dhsStatus );
	    }
	    
	    if ( dhsStatus != DHS_S_SUCCESS )
	    {
		*success = FALSE;
	    }
	}
    }
    return;
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::subsystem
//
// INVOCATION:
// n.a. Called as a callback from the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the subsystem information from the configuration file
//
// DESCRIPTION:
// Create a new cCmdSubsystem object and add it to the list.
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

boolean		cCmdSubsystem::subsystem
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cCmdStatus	status;		// Status code.
    cCmdSubsystem *pSubsystem;	// Pointer to the new subsystem.



    if ( numTokens == 5 )
    {
	checkNull( pSubsystem = new cCmdSubsystem( tokens[1], tokens[2], 
		tokens[3], tokens[4] ), status, return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}
