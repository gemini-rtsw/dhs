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
// dhsHistory/src/main.C
//
// PURPOSE:
// Main routine for the DHS History Server.
//
// METHOD NAME(S)
// main - Program main routine.
// checkUnique - Ensure only one History Server is running on this system.
// sigHandler - Handler for SIGINT and SIGTERM signals.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:26  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:21  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1998/05/11 16:21:49  nhill
// Changed comparison of status to S_SUCCESS to status.ok.
//
// Revision 1.6  1998/04/17 22:48:19  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.5  1998/03/13 21:45:44  jaeger
// Add new dhgGenSignal which handles signals in a separate thread.
//
// Revision 1.4  1998/03/06 19:47:10  nhill
// Added code to clear the dhs message after it is used.
//
// Revision 1.3  1997/10/20 21:47:36  cockayne
// Check-in for install.
//
// Revision 1.2  1997/10/06 20:15:53  cockayne
// Check-in for code review
//
// Revision 1.1  1997/09/02 21:13:35  cockayne
// Initial revision
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

#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <csignal>
#include <cerrno>
#include <pwd.h>

#include "cadef.h"
#include "dhs++.H"
#include "dhsGenSignal.H"
    
#include "globals.H"
#include "hisDhs.H"
#include "hisEvent.H"
#include "hisCa.H"
#include "hisDbm.H"
#include "hisRecovery.H"


//
// Set up a temporary directory object.
//

cDhsTmpDir	tmpDir;

static void	checkUnique( cHisStatus & );
static void	sigHandler( int );

//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhsHistory
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) Number of parameters.
// (>) argv (char *[]) List of parameters.
//
// FUNCTION VALUE:
// (int) Exit error status.
//
// PURPOSE:
// Main program section for dhs History Server. 
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
    cHisDhs	hisDhs;		// DHS object.
    cHisStatus	status;		// Function return status.
    const char	*str;		// DHS message string.


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

	cHisDhs::retry( true );
	while ( cHisDhs::retry() && status.ok() )
	{
	    //
	    // Set retry flag to false. If flag is still false when
	    // event loop exits, then historyServer exits. If flag
	    // has been changed to true, then historyServer will reset 
	    // & re-read configuration files & re-enter event loop.
	    //

	    cHisDhs::retry( false );


	    //
	    // Read configuration file.
	    //

	    configInit( status );


	    //
	    // Check that this is only HS running on system.
	    //

	    checkUnique( status );


	    //
	    //  Initialize database. 
	    //
	    
	    cHisDbManager::dbInit( status );


	    //
	    // Initialize DHS & DHS++ libraries.
	    //

	    hisDhs.init( status );


	    //
	    // Start DHS library event loop in separate thread.
	    //

	    hisDhs.start( status );


	    //
	    // Check if can detect previous crash, attempt recovery. 
	    //

	    cHisRecovery::checkRecovery( status );

	    
	    //
	    // Monitor EPICS stuff in a loop. 
	    //

	    cHisCa::channelLoopStart( status );
	    

	    //
	    // Clean up the DHS and DHS++ libraries.
	    //

	    hisDhs.exit( status );


	    //
	    //  Close the datbase. 
	    //

	    cHisDbManager::dbClose( status );
	}
    }
    catch( const cHisStatus &s )
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
	DHS_STATUS s1( DHS_S_SUCCESS ); 


	//
	// Error in the dhs library.
	//

	str = cDhs::message( st, el, s1 );

	std::cout << "Error in the dhs library: " << s << std::endl;
	for ( ; s1 == DHS_S_SUCCESS; str = cDhs::message( st, el, s1 ) )
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
// checkUnique
//
// INVOCATION:
// checkUnique( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cHisStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Ensure only one copy of the history server is running on this system
//
// DESCRIPTION:
// - Open a file in the temporary directory that will contain the task name
// and process id of a running HS.
// - Use kill to check to see if an existing process is still running.
// - If no process is running, write this tasks name and process id to the
// file.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// There is a slight chance that two HS's could examine the file at the
// same time, and both decide that they are the only HS running.
//-
//***********************************************************************
//

void		checkUnique
(
    cHisStatus	&status		// (mod) Function return status.
)
{
    int		fd;		// File descriptor for the file.
    char	fileName[PATH_MAX];
    				// File name for the file.
    FILE	*fp;		// File pointer for the file.
    pid_t	pid;		// Process id of a candiate History Server.
    struct passwd		// Password information.
    		*pPswd;


    checkStat( status, return );


    //
    // Open the file in /tmp. This is done with open instead of fopen to
    // allow more control over how the file is opened/created.
    //

    pPswd = getpwuid( getuid() );
    sprintf( fileName, "/tmp/dhsHistoryId.%s", pPswd->pw_name );
    fd = open( fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | 
	    S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
    fp = fdopen( fd, "r+" );


    //
    // Try to read the process id of the running HS from the file. If the
    // pid is in the file, and the pid is not this process, and the process is
    // still running, then another HS must be running.
    //

    if ( fscanf( fp, "%ld\n", &pid ) == 1 && pid != getpid() && 
	    ( kill( pid, 0 ) == 0 || errno != ESRCH ) )
    {
	status.E_RUNNING( status, pid );
    }
    else
    {
	//
	// No other HS is running, so write this tasks information to the
	// file.
	//

	(void) fseek( fp, 0, SEEK_SET );
	(void) fprintf( fp, "%ld\n", getpid() );
	(void) fprintf( fp, "%s.%ld\n", cHisDhs::identity(), getpid() );
    }


    //
    // close the file.
    //

    fclose( fp );
    close( fd );
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
    //
    //  Stop the EPICS Channel Access event loop.
    //
    
    cHisCa::channelLoopStop();
    cHisDhs::retry( false );
}
