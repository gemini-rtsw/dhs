static char rcsid[] = "$Id: main.C,v 1.1.1.1 2002-11-24 20:31:38 brighton Exp $";
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
// dhsStorage/src/main.C
//
// PURPOSE:
// Main routine for the DHS Storage Server.
//
// METHOD NAME(S)
// main - Program main routine.
// sigHandler - Handler for SIGINT and SIGTERM signals.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2001/03/09 22:49:32  nhill
// Changed to exit immediately when the -V option is used.
// Moved initialization of the status class to prevent crashes when
// errors are detected during startup.
//
// Revision 1.7  1999/06/18 22:29:10  nhill
// Fixed a bug that occured when a DHS status error was thrown.
//
// Revision 1.6  1999/06/16 23:52:23  nhill
// major re-organization.
//
// Revision 1.5  1998/05/15 17:00:29  cockayne
// Removed "#define DHS_NO_COMMAND", Mod'ed status check from old
// "status == status.S_SUCCESS" to "status.ok()".
//
// Revision 1.4  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.3  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.2  1998/03/31 16:58:53  cockayne
// Modified the way signals are handled.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "dhsGenSignal.H"
#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoDbm.H"
#include "stoAction.H"
#include "stoFiveState.H"
#include "stoUnit.H"
#include "stoMedia.H"
#include "stoStage.H"
#include "stoNode.H"
#include "stoConnection.H"
#include "stoDevice.H"


//
// Declare static funciton. 
//

static void	sigHandler( int );

//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhsStorage
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc	(int)	- Number of parameters.
// (>) argv	(char *[]) - List of parameters.
//
// FUNCTION VALUE:
// (int) Exit error status.
//
// PURPOSE:
// Main program section for DHS Storage Server. 
//
// DESCRIPTION:
// - Initialize the program
// - Run the DHS event loop
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
    cStoDhs	stoDhs;		// DHS object.
    cStoStatus	status;		// Program Status.
    const char	*str;		// DHS message string.


    //
    // Set up signal handlers.
    //

    dhsGenSignal( status, SIGINT, sigHandler, SIGTERM, sigHandler, NULL );

    try
    {
	//
	// Parse the command line.
	//

	argInit( argc, argv, status );
	if ( status == status.S_VERSION )
	{
	    return( 0 );
	}

	stoDhs.retry( true );
	while ( stoDhs.retry() && status.ok() )
	{
	    //
	    // Set retry flag to false. If flag is still false when
	    // event loop exits, then historyServer exits. If flag
	    // has been changed to true, then historyServer will reset 
	    // & re-read configuration files & re-enter event loop.
	    //

	    stoDhs.retry( false );


	    //
	    // Initialize the status class.
	    //

	    cStaStatus::init( PREFIX, status );


	    //
	    // Read configuration file.
	    //

	    configInit( status );


	    //
	    // Initialize DHS & DHS++ libraries.
	    //

	    stoDhs.init( status );


	    //
	    // Initialize database. 
	    //
	    
	    cStoDbManager::init( status );


	    //
	    // Initialize list of media objects.
	    //

	    cStoMedia::init( status );
	    
	    
	    //
	    // Initialize list of media unit objects.
	    //

	    cStoUnit::init( status );
	    
	    
	    //
	    // Initialize list of media stage directory objects.
	    //

	    cStoStage::init( status );
	    
	    
	    //
	    // Initialize the device list.
	    //

	    cStoDevice::init( status );


	    //
	    // Initialize processing nodes.
	    //

	    cStoNode::init( status );

	    
	    cStoAction::init( status );


	    //
	    // Start DHS library event loop. Wait until completed. 
	    //

	    stoDhs.start( status );


	    //
	    // Close down processing nodes
	    //

	    cStoNode::exit( status );
	    

	    //
	    // Close down the device list.
	    //

	    cStoDevice::exit( status );


	    //
	    // Exit method gets rid of media stage directory object list.
	    //

	    cStoStage::exit( status );
	    
	    
	    //
	    // Exit method gets rid of media unit object list.
	    //

	    cStoUnit::exit( status );
	    
	    
	    //
	    // Exit method gets rid of media object list.
	    //

	    cStoMedia::exit( status );
	    
	    
	    //
	    // Exit method gets rid of connection list. 
	    //

	    cStoConnection::exit( status );


	    //
	    // Clean up the DHS and DHS++ libraries.
	    //

	    stoDhs.exit( status );


	    //
	    // Exit the database. 
	    //

	    cStoDbManager::exit( status );
	}
    }
    catch( const cStoStatus &s )
    {
	//
	//  Handle error from the DHS library.
	//

	cout << s.message() << endl;
	return( -1 );
    }
    catch( const char * s )
    {
	//
	// Some memory allocation errors will cause this exception.
	//

	cout << "died with string: " << s << endl;
	return( -1 );
    }
    catch( const DHS_STATUS s )
    {
	DHS_STATUS	st;
	DHS_ERR_LEVEL	el;
	DHS_STATUS s1( DHS_S_SUCCESS ); 


	//
	// Error in the dhs library.
	//

	str = cDhs::message( st, el, s1 );

	cout << "Error in the dhs library: " << s << endl;
	for ( ; s1 == DHS_S_SUCCESS; str = cDhs::message( st, el, s1 ), 
		cDhs::messageClear( s1 ) )
	{
	    cout << str << endl;
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
// (>) sig	(int)	- Signal number that caused function to be executed.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when a SIGINT or SIGTERM signal is received
//
// DESCRIPTION:
// Use the stop() method to shut down the DHS library event loop.
// This function should never be called directly, only be called 
// in response to a signal.
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
    int				// (in)  The signal number sent. Ignored.
)
{
    DHS_STATUS  status( DHS_S_SUCCESS );


    //
    // Set retry flag for exit. Stop DHS event loop.
    //
    
    cStoDhs::retry( false );
    cDhs::stop( status );
}
