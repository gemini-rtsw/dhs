static char rcsid[] = "$Id: main.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsCommand/src/main.C
//
// PURPOSE:
// Main routine for the DHS command server.
//
// MEHTOD NAME(S)
// main - Program main routine.
// sigHandler - Called when SIGINT or SIGTERM signals are received.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:25:29  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:07  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.8  1998/06/26 18:59:25  nhill
// Added some error checking.
//
// Revision 1.7  1998/04/16 19:09:45  nhill
// Updated the documentation.
// Changed to the latest version of the cDhsStatus class.
//
// Revision 1.6  1998/03/13 21:49:16  jaeger
// Added disabling of commands, change completion sequence so objects
// are not deleted if commands are being processed, add new dhgGenSignal
// for handling signals in a separate thread.
//
// Revision 1.5  1997/09/05 18:55:57  nhill
// Fixed the version display.
//
// Revision 1.4  1997/08/27 19:09:37  nhill
// Added a message clear while displaying dhs messages.
//
// Revision 1.3  1997/06/13 20:46:19  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.2  1997/04/24 19:45:52  nhill
// Added a const keyword.
//
// Revision 1.1  1997/04/16 21:50:14  nhill
// Initial revision
//
// Revision 1.1  1997/03/27 22:15:42  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <csignal>

#include "globals.H"
#include "cmdDhs.H"
#include "cmdSubsystem.H"
#include "cmdCommandInfo.H"
#include "dhsGenSignal.H"

static void	sigHandler( int );

//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhsCmd
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) Number of parameters.
// (>) argv (char *[]) List of parameters.
//
// FUNCTION VALUE:
// (int) Exit error status.
//
// PURPOSE:
// This program re-distributes commands to the DHS subsystems.
//
// DESCRIPTION:
// - Set up the signal handler.
// - Parse the command line arguments.
// - Read the configuration file.
// - Initialize the dhs++ library.
// - Disable ICD1c command processing.
// - Connect to all subsystems.
// - Update the status of the dhsCommand server.
// - Start the DHS event loop. The event loop will continue to run until
//   a reset, init, or exit command is received.
// - After the event loop exits:
// - Update the command server status.
// - Disconnect all subsystems.
// - Clean up the DHS library.
// - If this is a reset or init, re-initialize the program and run the
//   event loop again.
// - If the program did not receive a reset or init command, exit.
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

int		main
(
    int		argc,		// (in)  Number of command line arguments.
    char	*argv[]		// (in)  List of command line arguments.
)
{
    cCmdDhs	*pCmdDhs;	// Dhs object.
    cCmdStatus	status;		// Function return status.
    cCmdSubsystem::cStatus	// Subsysteem class status.
    		ssStatus;
    const char	*str;


    dhsGenSignal( status, SIGINT, sigHandler, SIGTERM, sigHandler, NULL );
    try
    {
	//
	// Parse the command line.
	//

	argInit( argc, argv, status );

	cCmdDhs::retry( true );

	while ( cCmdDhs::retry() && status.ok() )
	{
	    pCmdDhs = new cCmdDhs;

	    
	    //
	    // Set the retry flag to false. If this flag is still false when
	    // the event loop exits, then the program will exit. If the flag
	    // has been changed to true, then the program will reset and
	    // re-read the configuration files and then re-enter the event 
	    // loop.
	    //
	    
	    cCmdDhs::retry( false );


	    //
	    // Read the configuration
	    //

	    check( configInit( status ), status, return( -1 ) );


	    //
	    // Start DHS event loop.
	    //

	    pCmdDhs->init( status );
	    pCmdDhs->disable();


	    //
	    // Connect to all of the subsystems;
	    //

	    cCmdSubsystem::connectAll( status );

	    
	    status.update( "health", "GOOD", status );
	    status.update( "state", "INITIALIZING", status );
	    status.update( "debug", status.debugString(), status );
	    if ( cCmdCommandInfo::simulate() )
	    {
		status.update( "simulate", "FAST", status );
	    }
	    else
	    {
		status.update( "simulate", "NONE", status );
	    }

	    status.update( "state", "RUNNING", status );
	    status.flush( status );

	    pCmdDhs->start( status );

	    if ( cCmdDhs::retry() )
	    {
		status.update( "state", "INITIALIZING", status );
	    }
	    else
	    {
		status.update( "state", "SHUTTINGDOWN", status );
	    }
	    status.flush( status );

	    
	    //
	    // Completion of execution.
	    //
	    
	    cCmdCommandInfo::exit( status );

	    if (  cCmdSubsystem::subsystemExit() )
	    {
		cCmdSubsystem::exitAll( status );
	    }
	    
	    status.disconnect( status );

	    pCmdDhs->exit( status );
	    delete pCmdDhs;

	    checkSubsystem( cCmdSubsystem::exit( ssStatus ), ssStatus, 
       		    status, return( -1 ) );
	}
    }
    catch( const cCmdStatus &s )
    {
	//
	//  Handle error from the DHS library.
	//

	std::cout << s.message() << std::endl;
	return( -1 );
    }
    catch( const char * s )
    {
	//
	// Some memory allocation errors will cause this exception.
	//

	std::cout << "died with string: " << s << std::endl;
	return( -1 );
    }
    catch( const DHS_STATUS s )
    {
	DHS_STATUS	st;
	DHS_ERR_LEVEL	el;
	//
	// Error in the dhs library.
	//

	std::cout << "Error in the dhs library: " << s << std::endl;
	for ( DHS_STATUS s1( DHS_S_SUCCESS ); s1 == DHS_S_SUCCESS; 
		str = cDhs::message( st, el, s1 ) )
	{
	    std::cout << str << std::endl;
	    cDhs::messageClear( s1 );
	}
	return( -1 );
    }
    if ( status.ok() )
    {
	return( 0 );
    }
    else
    {
	return( -1 );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// sigHandler
//
// INVOCATION:
// n.a. Set up as a signal handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) sig (int) The signal number that caused the function to be executed.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when a SIGINT or SIGTERM signal is received
//
// DESCRIPTION:
// Use the dhsEventLoopEnd function to shut down the DHS library event loop.
// This function should only be called in response to a signal.
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

static void	sigHandler
(
    int				// (in)  The signal number sent.
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    cDhs::stop( status );
}
