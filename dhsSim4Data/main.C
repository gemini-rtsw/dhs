static char rcsid[] = "$Id: main.C,v 1.1.1.1 2002-11-24 20:30:06 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
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
// dhsSim4Data/src/main.C
//
// PURPOSE:
// Main program for the dhs Simulator for the dataServer
//
// MEHTOD NAME(S)
// main()
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.1  1997/10/03 23:07:04  cockayne
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
 
#include <iostream.h>
#include <signal.h>

#include "globals.H"
#include "sim.H"

//
//  Static function prototypes.
//

static void     sigHandler( int );



//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// n.a. - Called automatically when the program is run.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc	(int) 	The number of cmd line args passed to dhsSim4Data
// (>) argv[]	(char*) The cmd line args passed to dhsSim4Data
//
// FUNCTION VALUE:
// (int) Return value from dhsSim4Data
//
// PURPOSE:
// Serves as the main program for dhsSim4Data
//
// DESCRIPTION:
// Pretty basic. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

int     main 
( 
    int         argc, 
    char        *argv[]
)
{
    cSim	theSim;		// Sim object.
    cSimStatus	status;         // Function return status.


    //
    //  Set up signal handlers.
    //
    
    (void) signal( SIGINT, sigHandler );
    (void) signal( SIGTERM, sigHandler );


    //
    //  Parse command line.
    //

    argInit( argc, argv, status );


    //
    //  Main loop bit. 
    //
    
    while ( status.ok() )
    {
	//
	//  Fetch runtime configuration. 
	//
	
	configInit( status ) ;


	//
	//  Initialize DHS library.
	//

	theSim.init( status );


	//
	//  Start DHS event loop.
	//

	theSim.start( status );
	

	//
	//  Exit from DHS library.
	//

	theSim.exit( status );


	break;
    } 

    
    //
    //  Check status, exit. 
    //

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
 
static void     sigHandler
(
    int		sig		// (in)  The signal number sent.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Return status from the DHS library.
    cSimStatus	status;		// Function return status.



    status.E_CONTROL_C( status );
    status.display();

    cDhs::stop( dhsStatus );

    (void) signal( sig, sigHandler );
}
