static char rcsid[] = "$Id: main.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 2000				(c) 2000
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
// dhsStoPut/src/main.C
//
// PURPOSE:
// Manually does put requests of a file(s) to the Storage Server
//
// METHOD NAME(S)
// main 	- the main routine.
// sigHandler 	- Interrupt signal handler.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:06  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.8  2001/01/12 22:23:46  jaeger
// Added media type check.  Added proper handling of the -simulate
// command-line option.
//
// Revision 1.7  2000/12/19 19:30:41  jaeger
// Added verification of mediaType and setting of the unitPrefix.
//
// Revision 1.6  2000/12/12 23:07:46  jaeger
// Show short version message only if there are errors.  Otherwise
// the error goes undetected.
//
// Revision 1.5  2000/12/12 20:16:28  jaeger
// Switched the order of config file reading and command-line argument
// reading; command-line args are now read first.  Added a short
// version message which will be displayed always.
//
// Revision 1.4  2000/12/12 00:22:08  jaeger
// Moved call to cDhsDataest::buffer to cStoPut::cStoPut method.
//
// Revision 1.3  2000/12/11 20:03:41  jaeger
// Updated call to cPutFile constructor.  Added call bo cPutFile->buffer(),
// without this the buffer is empty and no data would be sent.
//
// Revision 1.2  2000/12/07 23:49:27  jaeger
// Fixed bug:  the file ID was being passed into the cPutFile constructor
// instead of the full fill name.
//
// Revision 1.1  2000/12/05 06:54:14  jaeger
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
#include "stoPutDhs.H"
#include "lists.H"

extern	char 	*version_short;

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
// dhsStoPut
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) The number of command line arguments.
// (>) argv (char *[]) This list of command line arguments.
//
// FUNCTION VALUE:
// (int) Program return status.
//
// PURPOSE:
// Main program to run the dhsStoPut client.
//
// DESCRIPTION:
// Set up the handlers, parse the command line arguments, read the
// configuration file and then run the event loop.
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

    cStoPutDhs		lstoPutDhs;	// Dhs object.
    cStoPutStatus	status;		// Function return status.
    const char  	*str;		// Output message.
    char 		unitPrefix[PATH_MAX];
    					// Prefix for media unit names.


    //
    //  Start up the interrupt handler.
    //

    (void) signal( SIGINT, sigHandler );
    (void) signal( SIGTERM, sigHandler );

    try
    {
	// 
	//
	//  Parse the command line.
	//
	
	argInit( argc, argv, &lstoPutDhs, status );
	if ( status == status.S_VERSION || status == status.S_HELP )
	{
	    return( 0 );
	}

	//
	// Display the version information.
	//

	if ( status.ok() )
	{
	    status.S_VERSION_SHORT( status, version_short );
	}
	

	//
	// Read config file.
	//

	configInit( &lstoPutDhs, status );
	if ( ! status.ok() )
	{
	    return( -1 );
	}

	
	//
	// If this is a user request then check the mediaType
	// to be used.
	//

	if ( lstoPutDhs.getUserStream() != NULL )
	{
	   if (  cMediaTypeList::findDhsMediaType( lstoPutDhs.getMediaType() )
   		== NULL )
	   {
	       status.E_MT_NOT_FOUND( status, lstoPutDhs.getMediaType() );
	       return( -1 );
	   }
	}

	
	//
	//  Initialize DHS library.
	//

	lstoPutDhs.init( status );


	//
	//  Start the DHS event loop and make connection to Storage server.
	//

	lstoPutDhs.start( status );
	    

	//
	// Send all of the file information to the Storage Server.
	// Using different commands depending on the type of
	// stream.
	//

	if ( lstoPutDhs.getArchiveStream() != NULL )
	{
	    lstoPutDhs.archiveFileAvail( status );
	}
	else if ( lstoPutDhs.getUserStream() != NULL )
	{
	    if ( lstoPutDhs.getUnitPrefix() == NULL )
	    {
		sprintf( unitPrefix, "%s_%s_", lstoPutDhs.getOrganization(),
	              lstoPutDhs.getUserStream() );
		lstoPutDhs.setUnitPrefix( strsav( unitPrefix ), status );
	    }
	    lstoPutDhs.userMediaRequest( status );
	}
	else
	{
	    status.E_USAGE( status );
	    return( -1 );
	}
	checkStat( status, return( -1 ) );
	

	//
	//  While we are requesting files, do the following:
	//

	cStoPutList	*pStoPutList;		// Points to list.
	DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
	cStoPutList::iterator
			i;			// Pointer to list of puts.
	cPutFile	*pPutFile;		// Put object.
	bool		firstTime = TRUE;	// First time flag.


	while ( status.ok() &&
		( pStoPutList = cStoPutList::getNext( i, firstTime ) ) != NULL )
	{
	    firstTime = FALSE;

	    
	    //
	    //  Create a new object and read the data into a buffer.
	    //

	    pPutFile = new cPutFile( status, pStoPutList->getSrcName(),
    		     pStoPutList->getFileSize() );
	    checkStat( status, continue );

	    
	    //
	    // Send the file to the Storage Server, if not in
	    // simulate mode and if the Storage Server was
	    // notfifed of the file.
	    //

	    if ( ! cStoPutDhs::simulate() )
	    {
		checkDhs( pPutFile->put( lstoPutDhs.getConnect(),
			pStoPutList->getFileId(), true, dhsStatus ), dhsStatus,
			status, break );

		//
		// Wait for the put to complete.
		//

		checkDhs( pPutFile->wait( dhsStatus ), dhsStatus, status, break );
		if ( pPutFile->status( dhsStatus ) != DHS_CS_DONE )
		{
		    status.E_PUT_FAILED( status, pStoPutList->getFileId() );
		}
	    }
	    


	    //
	    //  Delete the put file object.
	    //

	    delete pPutFile;
	}


       //
       //  Close the dhs library.
       //

	lstoPutDhs.exit( status );

    }
    catch( const cStoPutStatus &s )
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
	    cDhs::messageClear(s1);
	}
	lstoPutDhs.kill();
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
