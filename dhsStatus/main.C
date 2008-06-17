static char rcsid[] = "$Id: main.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStatus/src/main.C
//
// PURPOSE:
// Main routine for the DHS status server.
//
// MEHTOD NAME(S)
// main - Program main routine.
// sigHandler - function to execute when SIGINT and SIGTERM signals occur.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:30:27  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.16  1998/06/24 23:16:44  nhill
// Moved the call to cStaSubscriber::exit to cStaDhs::start.
//
// Revision 1.15  1998/06/23 18:01:51  nhill
// Moved the enabling of logging occur before the status server state is initialized.
//
// Revision 1.14  1998/06/19 18:02:41  nhill
// Changed the cStaStatus class name to cStaStat.
// Disabled status logging before init and after shutdown.
//
// Revision 1.13  1998/05/12 20:21:51  nhill
// Updated the documentation.
//
// Revision 1.12  1998/04/15 20:57:25  nhill
// Updated the documentation.
//
// Revision 1.11  1998/03/13 22:06:36  nhill
// Changed the signal handler to one that works with threads.
//
// Revision 1.10  1998/02/17 00:19:04  jaeger
// Put cStaMon::stop() into main - the main loop.
//
// Revision 1.9  1998/02/05 21:18:48  jaeger
// Added database and magnetic disk resource monitoring.
//
// Revision 1.8  1998/01/08 21:00:43  jaeger
// Changed a comparison with status.S_SUCCESS to a status.ok().
//
// Revision 1.7  1997/09/10 00:08:09  nhill
// Fixed a typo bug.
//
// Revision 1.6  1997/09/05 18:54:50  nhill
// Fixed the version display.
//
// Revision 1.5  1997/08/27 19:09:57  nhill
// Added a message clear while displaying dhs messages.
//
// Revision 1.4  1997/06/13 20:41:22  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.3  1997/04/24 20:04:32  nhill
// Added a const keyword.
//
// Revision 1.2  1997/04/16 22:27:44  nhill
// Removed the dhsSubsystem header file.
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
#include "staDhs.H"
#if defined(EPICS_DHS)
#include "staChannel.H"
#endif
#include "staMonitor.H"    
#include "subscriber.H"

#include "dhsGenSignal.H"
static void	sigHandler( int );


//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhsStatus
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) Number of parameters.
// (>) argv (char *[]) List of parameters.
//
// FUNCTION VALUE:
// (int) Exit error status.
//
// PURPOSE:
// This program updates the epics database with the DHS status and history
// data.
//
// DESCRIPTION:
// - Parse the command line.
// - Read the configuration file.
// - Initialize various classes.
// - Run the dhsEvent loop.
// - Clean up various classes.
// - Restart if a reset was done, or exit if an exit was done.
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
    cStaMonDb	staMonDb;	// Database resource monitoring object.
    cStaDhs	staDhs;		// Dhs object.
    cStaMonMd	staMonMd;	// Magnetic disk resource monitoring object.
    cStaStat	status;		// Function return status.
    cDhsSubsystem::cStatus	// Subsystem status value
    		ssStatus;
    const char	*str;


    ssStatus.display( status );
    cStaStat::init( status );

    dhsGenSignal( status, SIGINT, sigHandler, SIGTERM, sigHandler, NULL );
    try
    {
	//
	// Parse the command line.
	//

	check( argInit( argc, argv, status ), status, return( -1 ) );

	cStaDhs::retry( true );
	
	while ( cStaDhs::retry() && status.ok() )
	{
	    //
	    // Set the retry flag to false. If this flag is still false when
	    // the event loop exits, then the program will exit. If the flag
	    // has been changed to true, then the program will reset and
	    // re-read the configuration files and then re-enter the event 
	    // loop.
	    //
	    
	    cStaDhs::retry( false );


	    //
	    // Read the configuration
	    //

	    configInit( status );


	    //
	    // Initialize the channel access library and changed init
	    // Status to allow connections to the status server.
	    //

#if defined(EPICS_DHS)
	    cStaChannel::init( status );
#endif
	    cStaDhs::isInit( true );


	    //
	    // Start DHS event loop.
	    //

	    staDhs.start( status );


	    //
	    //	Stop the monitoring of resources.
	    //

	    cStaMon::stop();
	    
	    
	    //
	    // Close the channel access library.
	    //

#if defined(EPICS_DHS)
	    cStaChannel::exit( status );
#endif


	    checkSubsystem( cDhsSubsystem::exit( ssStatus ), 
		    ssStatus, status, return( -1 ) );
	}
    }
    catch( const cStaStat &s )
    {
	//
	//  Handle error from the DHS library.
	//

	std::cout << "died " << std::endl;
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
	DHS_STATUS	s1( DHS_S_SUCCESS );
	//
	// Error in the dhs library.
	//

	std::cout << "Error in the dhs library: " << s << std::endl;
	str = cDhs::message( st, el, s1 );
	for ( ; s1 == DHS_S_SUCCESS; str = cDhs::message( st, el, s1 ) )
	{
	    std::cout << str << std::endl;
	    cDhs::messageClear( s1 );
	}
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

