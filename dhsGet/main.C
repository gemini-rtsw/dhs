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
// dhsGet/src/main.C
//
// PURPOSE:
// Manually request a file from the Data Server
//
// METHOD NAME(S)
// main 	- the main routine.
// sigHandler 	- Interrupt signal handler.
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  Includes
//

#include <iostream.h>
#include <signal.h>

#include "globals.H"
#include "getDhs.H"
#include "list.H"

//
//  Define locals
//

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
// Main program to run the dataServer.
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

    getDhs	lgetDhs;	// Dhs object.
    cGetStatus	status;		// Fucntion return status.
    const char  *str;		// ?


    //
    //  Start up the interrupt handler.
    //

    (void) signal( SIGINT, sigHandler );
    (void) signal( SIGTERM, sigHandler );

    try
    {
	//
	//  Parse the command line.
	//
	
	argInit( argc, argv, status );
	if ( status == status.S_VERSION )
	{
	    return( 0 );
	}


	//
	//  Initialize DHS library.
	//

	lgetDhs.init( status );


	//
	//  Start the DHS event loop and make connection to data server.
	//

	lgetDhs.start( status );
	    

	//
	//  While we are requesting files, do the following:
	//

	cGetList	*dataSet;
	DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
	cGetList::iterator
			i;
	getDataset	*pGetDs;

	while ( status.ok() &&
		( dataSet = cGetList::getNext( i ) ) != NULL )
	{
	    if ( !lgetDhs.doNext() )
	    {
		break;
	    }


	    //
	    //  Create a new dataset object and request the data.
	    //

	   pGetDs = new getDataset;

	   pGetDs->sdMutex.lock();
	   checkDhs( pGetDs->get( lgetDhs.getConnect(),
		    dataSet->getName(), lgetDhs.getType(), dhsStatus ),
		    dhsStatus, status, break );


	    //
	    // Wait for the get to complete and save the data if possible.
	    //

	    checkDhs( pGetDs->wait( dhsStatus ), dhsStatus, status, break );

	    if ( pGetDs->status( dhsStatus ) != DHS_CS_DONE )
	    {
		status.E_GET_FAILED( status, dataSet->getName() );
	    }


	    //
	    //  Verify the done method has completed by waiting for the
	    //  done method of unlock the mutex because you can lock it.
	    //  Then delete the get dataset.
	    //

	    pGetDs->sdMutex.lock();
	    pGetDs->sdMutex.unlock();

	    delete pGetDs;
	   
	}

       //
       //  Close the dhs library.
       //

	lgetDhs.exit( status );

    }
    catch( const cGetStatus &s )
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
	str = cDhs::message( st, el, s1 );
	for ( ; s1 == DHS_S_SUCCESS; 
		str = cDhs::message( st, el, s1 ) )
	{
	    cout << str << endl;
	    cDhs::messageClear( s1 );
	}

	lgetDhs.kill();
	return( -1 );
    }

    if ( status.ok() )
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
    DHS_STATUS	status( DHS_S_SUCCESS );


    cDhs::stop( status );
    (void) signal( sig, sigHandler );
}
