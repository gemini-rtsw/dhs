static char rcsid[] = "$Id: main.C,v 1.1.1.1 2002-11-24 20:29:01 brighton Exp $";
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
// dhsQlServer/src/main.C
//
// PURPOSE:
// Main routine for the DHS Quick Look Server.
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
// Revision 1.12  2001/03/27 20:42:56  nhill
// Changed a bug where DHS errors during startup were not printed.
//
// Revision 1.11  1998/05/11 16:22:28  nhill
// Changed the comparison of status to S_SUCCESS to a call to status.ok.
//
// Revision 1.10  1998/05/05 17:05:30  nhill
// Changed to use resetHealth to set the health to good.
//
// Revision 1.9  1998/04/27 21:06:26  nhill
// Added a check to ensure this is a master qls before connecting to
// the status server.
//
// Revision 1.8  1998/04/15 22:34:50  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.7  1998/03/13 22:24:01  jaeger
// Added dhsGenSignal to handle signals in a separate thread.
//
// Revision 1.6  1998/03/06 17:24:39  nhill
// Added error checking on the connection to the status server.
//
// Revision 1.5  1997/11/18 23:10:32  nhill
// Changed the temporary directory name to be based on the application name.
//
// Revision 1.4  1997/08/27 22:05:40  nhill
// Changed to let IMP determine if another QLS is running.
//
// Revision 1.3  1997/08/26 21:50:40  nhill
// Fixed version number printing.
//
// Revision 1.2  1997/08/20 21:22:34  nhill
// *** empty log message ***
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
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

#include "dhsGenSignal.H"
    
#include "globals.H"
#include "qlsDhs.H"
#include "qlsStream.H"
#include "qlsFitsKeyword.H"
#include "qlsDataset.H"
#include "qlsQlt.H"
#include "qlsSlave.H"
#include "qlsMaster.H"


//
// Set up a temporary directory object.
//

cDhsTmpDir	tmpDir;

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
// This program redistributes data to Quick Look Tools.
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
    struct passwd		// Password information.
    		*pPswd;
    cQlsDhs	qlsDhs;		// DHS object.
    cQlsStatus	status;		// Function return status.
    const char	*str;		// DHS message string.
    char	tmpPath[ PATH_MAX ];
				// Path for the temporary directory.


    //
    // Set up the signal handlers.
    //

    dhsGenSignal( status, SIGINT, sigHandler, SIGTERM, sigHandler, NULL );

    try
    {
	//
	// Parse the command line.
	//

	argInit( argc, argv, status );

	cQlsDhs::retry( true );
	while ( cQlsDhs::retry() && status.ok() )
	{
	    //
	    // Set the retry flag to false. If this flag is still false when
	    // the event loop exits, then the program will exit. If the flag
	    // has been changed to true, then the program will reset and
	    // re-read the configuration files and then re-enter the event 
	    // loop.
	    //

	    cQlsDhs::retry( false );


	    //
	    // Read the configuration file.
	    //

	    configInit( status );


	    //
	    // Initialize the DHS and DHS++ libraries.
	    //

	    qlsDhs.init( status );
	    if ( ! status.ok() )
	    {
		break;
	    }


	    //
	    // Setup the temporary directory.
	    //

	    pPswd = getpwuid( getuid() );
	    sprintf( tmpPath, "/tmp/%s.%s", APPLICATION, pPswd->pw_name );
	    tmpDir.init( tmpPath );


	    //
	    //  Clean out anything in the temporary directory.
	    //

	    checkSystem( tmpDir.clear( NULL ), status, return( -1 ) );


	    //
	    // Connect to the status server.
	    //

	    cQlsStatus::init( PREFIX, status );
	    if ( ! cQlsMaster::isSlave() )
	    {
		check( cQlsStatus::connect( status ), status, return( -1 ) );
	    }


	    //
	    // Update the state of the server.
	    //

	    cQlsStatus::resetHealth( status );
	    cQlsStatus::update( "debug", status.debugString(), status );
	    if ( cQlsDhs::simulate() )
	    {
		cQlsStatus::update( "simulate", "FAST", status );
	    }
	    else
	    {
		cQlsStatus::update( "simulate", "NONE", status );
	    }
	    cQlsStatus::update( "state", "RUNNING", status );
	    cQlsStatus::flush( status );


	    //
	    // Open the connection to the master QLS if appropriate.
	    //

	    if ( cQlsMaster::isSlave() )
	    {
		cQlsMaster::open( status );
	    }


	    //
	    // Start the DHS library event loop.  The event loop will not
	    // complete until the application is shut down or reset.
	    //

	    qlsDhs.start( status );


	    //
	    // If the retry flag is true, then it should re-initialize
	    // everything, otherwise it is time to exit.  Update the
	    // program state appropriately.
	    //

	    if ( cQlsDhs::retry() )
	    {
		cQlsStatus::update( "state", "INITIALIZING", status );
		status.S_RESET( status );
	    }
	    else
	    {
		cQlsStatus::update( "state", "SHUTTINGDOWN", status );
		status.S_EXITING( status );
	    }
	    cQlsStatus::flush( status );


	    //
	    // Delete all cQlsQlt objects.
	    //

	    cQlsQlt::exit( status );


	    //
	    // Delete all cQlsSlave objects.
	    //

	    cQlsSlave::exit( status );


	    //
	    // Delete all cQlsDataset objects.
	    //

	    cQlsDataset::exit( status );


	    //
	    // Delete all cQlsStream objects.
	    //

	    cQlsStream::exit( status );


	    //
	    // Delete all cQlsFitsKeyword objects.
	    //

	    cQlsFitsKeyword::exit( status );


	    //
	    // Close the connection to the master server.
	    //

	    cQlsMaster::close( status );


	    //
	    // Disconnect from the status server.
	    //

	    cQlsStatus::disconnect( status );


	    //
	    // Clean up the DHS and DHS++ libraries.
	    //

	    qlsDhs.exit( status );


	    //
	    //  Clean out the temporary directory.
	    //

	    checkSystem( tmpDir.clear( NULL ), status, return( -1 ) );
	}
    }
    catch( const cQlsStatus &s )
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
	DHS_STATUS	s1( DHS_S_SUCCESS );
	DHS_ERR_LEVEL	el;
	//
	// Error in the dhs library.
	//

	cout << "Error in the dhs library: " << s << endl;
	for ( str = cDhs::message( st, el, s1 ); s1 == DHS_S_SUCCESS; 
		str = cDhs::message( st, el, s1 ) )
	{
	    cout << str << endl;
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
