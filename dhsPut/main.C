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
// dhsPut/src/main.C
//
// PURPOSE:
// Manually does put requests of a file(s) to the Data Server
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
#include "putDhs.H"
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

    putDhs	lputDhs;	// Dhs object.
    cPutStatus	status;		// Fucntion return status.
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
	    return (0);
	}

	cPutList::setLast( status );
	cPutList::setAllDSNames( status );


	//
	//  Initialize DHS library.
	//

	lputDhs.init( status );


	//
	//  Start the DHS event loop and make connection to data server.
	//

	lputDhs.start( status );
	    

	//
	//  If the lifetime is set, or new is set, do those command first.
	//

	lputDhs.firstCmds( status );


	//
	//  While we are requesting files, do the following:
	//

	cPutList	*pPutList;		// Points to list.
	DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
	cPutList::iterator
			i;			// Pointer to list of puts.
	putDataset	*pPutDs;		// Put object.
	int		bufSize;		// Buffer size to send.
	void		*buffer;		// Buffer to send.
	bool		firstTime = TRUE;	// First time flag.
	cPutList::iterator
			j;


	while ( status.ok() &&
		( pPutList = cPutList::getNext( i, firstTime ) ) != NULL )
	{
	    firstTime = FALSE;
	    if ( !lputDhs.doNext() )
	    {
		break;
	    }


	    //
	    //  Create a new object and read the data into a buffer.
	    //

	    pPutDs = new putDataset( status, pPutList->getName(), 
		    &buffer, bufSize, lputDhs.getType(), FALSE );
	    checkStat( status, continue );
	    pPutDs->buffer( buffer, bufSize );
	    if ( lputDhs.getType() == DHS_BD_PT_DS || 
		    lputDhs.getType() == DHS_BD_PT_DS_QL )
	    {
		pPutDs->access( dhsStatus );
	    }

	    checkStat( status, continue );

	    
	    //
	    // The last flag is set = true if the putDhs nolast flag isn't
	    // set, and if the pPutList last flag for the file is set and
	    // if the bufZero flag is not set.
	    //

	    checkDhs( pPutDs->put( lputDhs.getConnect(),
		    pPutList->getFileId(), ( !putDhs::buffZero() & 
		    ! putDhs::getNoLast() ) &
		    pPutList->getLast(), dhsStatus ),
		    dhsStatus, status, break );

	    checkStat( status, continue );

	    //
	    // Wait for the put to complete.
	    //

	    checkDhs( pPutDs->wait( dhsStatus ), dhsStatus, status, break );

	    if ( pPutDs->status( dhsStatus ) != DHS_CS_DONE )
	    {
		status.E_PUT_FAILED( status, pPutList->getName() );
	    }


	    //
	    //  Delete the put file object.
	    //

	    j = i;
	    delete pPutDs;
	   
	}


	//
	//  Check to see if the last buffer should be sent with length
	//  zero and the last flag set.
	//

	
	if ( status.ok() && putDhs::buffZero() )
	{
	    cPutList	*lastOne;		// Points to last one in list.
	    int		zeroBuffer = 0;
	    char	*dsName = putDhs::getDSName();
	    char	*fileId = putDhs::getDSName();



	    if ( firstTime )
	    {
		lastOne = cPutList::getNext( j, firstTime );
		
	    }
	    else
	    {
		lastOne = (*j).second;
	    }
	    if ( lastOne != NULL )
	    {
		dsName = lastOne->getName();
		fileId = lastOne->getFileId();
	    }


	    //
	    //  Create a new object, and send it.
	    //

	    pPutDs = new putDataset( status, dsName, 
		    NULL, zeroBuffer, lputDhs.getType(), TRUE );
	    buffer = NULL;
	    pPutDs->buffer( buffer, zeroBuffer );

	    checkDhs( pPutDs->put( lputDhs.getConnect(),
		    fileId, ! putDhs::getNoLast(), dhsStatus ),
		    dhsStatus, status, VOID );
	    checkDhs( pPutDs->wait( dhsStatus ), dhsStatus, status, VOID );
	    if ( pPutDs->status( dhsStatus ) != DHS_CS_DONE )
	    {
		status.E_PUT_FAILED( status, dsName );
	    }

	    delete pPutDs;
	}

       //
       //  Close the dhs library.
       //

	lputDhs.exit( status );

    }
    catch( const cPutStatus &s )
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
	    cDhs::messageClear(s1);
	}
	lputDhs.kill();
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
