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
// dhsData/src/main.C
//
// PURPOSE:
// Main program for the Data Server
//
// METHOD NAME(S)
// main 	- the main routine.
// sigHandler 	- Interrupt signal handler.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:16  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2000/07/27 20:10:14  dunn
// Added standalone features.
//
// Revision 1.7  1998/05/07 23:35:08  dunn
// Uncommented call to ssServerNotify
//
// Revision 1.6  1998/05/04 23:00:54  dunn
// Added ready flag setting and unsetting.
//
// Revision 1.5  1998/03/13 21:43:21  jaeger
// Added dhsGen thread for signal handling.
//
// Revision 1.4  1997/09/29 21:21:22  dunn
// Moved cSta::disconnect to closeLibs
//
// Revision 1.3  1997/09/17 22:14:30  dunn
// Added temp cleanup and commented out ss and ol notify upon startup.
//
// Revision 1.2  1997/08/29 23:02:46  dunn
// Added exiting from config errors and added all the close libraries stuff.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  Includes
//

#include <iostream>
#include <csignal>

#include "globals.H"

#include "dtsDhs.H"
#include "dhsGenConnect.H"
#include "dhsGenSignal.H"

static void     sigHandler( int );


//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhsData
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) The number of command line arguments.
// (>) argv (char *[]) This list of command line arguments.
//
// FUNCTION VALUE:
// (int) Program return status.
//
// PURPOSE:
// Main program to run the dataServer
//
// DESCRIPTION:
// Set up the handlers and then run the event loop.
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

int	main 
( 
    int		argc, 
    char	*argv[]
)
{

    cDtsDhs	dtsDhs;		// Dhs object.
    cDtsStatus	status;		// Fucntion return status.
    const char  *str;		// ?


    //
    //  Start up the interrupt handler.
    //

    dhsGenSignal( status, SIGINT, sigHandler, SIGTERM, sigHandler, NULL );

    try
    {
	//
	//  Parse the command line.
	//
	
	check( argInit( argc, argv, status ), status, return(-1) );
	if ( status == status.S_VERSION )
	{
	    return (0);
	}
	

	//
	//  While we are starting or re-starting, do the following.
	//

	do
	{
	    //
	    //  Set the retry flag to flase.
	    //

	    cDtsDhs::retry( false );
	    cDtsDhs::ready( status, false );


	    //
	    // Read the configuration.
	    //

	    configInit( status ) ;


	    //
	    //  Initialize DHS and DHS++ libraries.
	    //

	    dtsDhs.init( status );
	    if ( !status.ok() )
	    {
		break;
	    }


	    //
	    //  Initialize any libraries.
	    //  Make a connection to the status server, then initialize it.
	    //

	    dtsDhs.initLibs( status );


	    //
	    //  Cleanup temp, and make list if running standalone.
	    //

	    dtsDhs.cleanupTemp( status );
	    dtsDhs.makeList( status );


	    //
	    //  Notify the Server of any missing connections.
	    //

	    dtsDhs.ssServerNotify( status );
	    //// Don't need to inform oldp dtsDhs.olServerNotify( status );
   

	    //
	    //  Start the DHS event loop.
	    //

	    cDtsDhs::ready( status, true );
	    dtsDhs.start( status );
	    

	    //
	    //  Event loop has ended, check for retry and set state accordingly.
	    //

	    if ( cDtsDhs::retry() )
	    {
		cStaStatus::update( "state", "INITIALIZING", status );
	    }
	    else
	    {
		cStaStatus::update( "state", "SHUTTINGDOWN", status );
	    }
	    cStaStatus::flush( status );

	   
	   //
	   //  Close any libraries open and exit.
	   //
	    
	   dtsDhs.closeLibs( status );
	   dtsDhs.exit( status );

	} while ( cDtsDhs::retry() && status.ok() );

    }
    catch( const cDtsStatus &s )
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
	DHS_STATUS	s1( DHS_S_SUCCESS );
	DHS_ERR_LEVEL	el;


	//
	// Error in the dhs library.
	//

	std::cout << "Error in the dhs library: " << s << std::endl;
	str = cDhs::message( st, el, s1 );
	for ( ; s1 == DHS_S_SUCCESS; 
		str = cDhs::message( st, el, s1 ) )
	{
	    std::cout << str << std::endl;
	    cDhs::messageClear( s1 );
	}
	dtsDhs.kill();
	return( -1 );
    }

    if ( status.ok() || status == status.E_ONLY_ONE )
    {
	return ( 0 );
    }
    else
    {
	return ( -1 );
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
    int		sig		// (in)  The signal number sent.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cDtsStatus	status;



    status.E_CONTROL_C( status );

    cDhs::stop( dhsStatus );
}
