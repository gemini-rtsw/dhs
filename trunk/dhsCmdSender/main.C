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
// dhs++Client/src/main.C
//
// PURPOSE:
// This is a test program to test the client side of the DHS++ library.
//
// METHOD NAME(S)
// main - The main routine only.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:25:12  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:34  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.13  1998/06/19 00:35:01  cockayne
// Added -timeCommand command-line-option, cleaned up command line parsing.
//
// Revision 1.12  1998/06/17 20:39:13  nhill
// Fixed a bug that occured when the first line of a file is blank.
//
// Revision 1.11  1998/06/15 14:42:54  cockayne
// Increased size of arrays.
//
// Revision 1.10  1998/06/04 20:37:49  nhill
// Changed to use C streams instead of cout. This makes the coordination
// of sds messages better.
//
// Revision 1.9  1998/05/05 17:44:28  nhill
// Added DHS_DT_UINT32 to the supported types.
//
// Revision 1.8  1997/12/01 23:18:26  nhill
// Removed the close from the shutdown process.
//
// Revision 1.7  1997/11/20 00:30:35  nhill
// Bug fix.
//
// Revision 1.6  1997/11/20 00:16:33  nhill
// bug fix.
//
// Revision 1.5  1997/11/19 23:56:15  nhill
// Added a command line option to display the command results.
//
// Revision 1.4  1997/11/19 17:30:50  nhill
// Fixed handling of comments.
//
// Revision 1.3  1997/09/23 15:47:23  dunn
// *** empty log message ***
//
// Revision 1.2  1997/05/01 17:56:14  nhill
// Changed the message string pointer to const.
//
// Revision 1.1  1997/04/16 21:57:58  nhill
// Initial revision
//
// Revision 1.1  1997/02/28 18:07:37  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
// This file only uses client stuff in the DHS++ library.  Setting the
// DHS_NO_SERVER symbol will speed up compilation a little.
//

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <ctime>

extern "C"
{
int	usleep( uint );
#include "gen_types.h"
#include "gen_str.h"
}

#define	DHS_NO_SERVER
#include "dhs++.H"
#include "genCond.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cError
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this class is created when the dhs library sends an
// error message using the error callback. Generally this means debugging
// messages or error messages from errors in the event loop.
//
//-
//***********************************************************************
//

class	cError: public cDhsError
{
    private:
	void	error();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cError::error
//
// INVOCATION:
// n.a.  This function is called by the error handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Handle an error from the DHS++ library
//
// DESCRIPTION:
// Print the error message to standard out.
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

void		cError::error()
{
    //
    // The cDhsError provides various accessor functions for getting 
    // information about the error, including "message" to return the
    // text.
    //

    puts( message() );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cmd
//
// PUBLIC METHODS: 
// (None) cmd - constructor for the command.
//
// PURPOSE:
// An object of this class is created to send a command to a server.
//
//-
//***********************************************************************
//

class cmd: public cDhsClientCommand
{
    public:
    	cmd( char *, bool printResponse = false );
    private:
	void	done( DHS_CMD_STATUS, char *, const cDhsAvList & );
	void	response( DHS_CMD_STATUS, char *, const cDhsAvList & );

	bool	cmdPrintResponse;
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cmd::cmd
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the command
//
// DESCRIPTION:
// This function only calls the cDhsClientCommand constructor.
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

		cmd::cmd 
(
    char	*name,
    bool	printResponse
)
: cDhsClientCommand( name )
{
    cmdPrintResponse = printResponse;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cmd::done
//
// INVOCATION:
// n.a.  This function is called by the DHS++ library when a command is
// complete.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cs (DHS_CMD_STATUS) The status the command finished with.
// (>) msg (char *) The message associated with the status.
// (>) avList (cDhsAvList) The response attribute value list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Handle the completion of a command
//
// For now, this is just printing some messages.
//
// DESCRIPTION:
// Print information about the command.
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

void		cmd::done 
(
    DHS_CMD_STATUS	cs,
    char		*msg,
    const cDhsAvList	&avList
)
{
    //puts( "Command Done" );

    response( cs, msg, avList );


    //
    // Some applicatons might delete "this" here, but that would assume
    // the command is on the heap, and that no use is made of the command
    // after the "apply". (i.e. the waits that I have done in the main
    // program wouldn't be allowed since there would be no way to know
    // if the command still existed.)
    //
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cmd::response
//
// INVOCATION:
// n.a.  This function is called by the DHS++ library when a response is
// received.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cs (DHS_CMD_STATUS) The status the command finished with.
// (>) msg (char *) The message associated with the status.
// (>) avList (cDhsAvList) The response attribute value list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Handle the completion of a command
//
// For now, this is just printing some messages.
//
// DESCRIPTION:
// Print information about the command.
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

void		cmd::response 
(
    DHS_CMD_STATUS	cs,
    char		*msg,
    const cDhsAvList	&avList
)
{
    DHS_STATUS		status( DHS_S_SUCCESS );


    if ( cs != DHS_CS_BUSY )
    {
	if ( cs == DHS_CS_DONE )
	{
	    puts ( "Command Response: DONE " );
	}
	else if ( cs == DHS_CS_ERROR)
	{
	    puts( "Command Response: ERROR" );
	}
	else
	{
	    printf(  "Command Response: state %d\n", cs );
	}
	if ( msg != NULL )
	{
	    printf( "\nmessage: %s\n", msg );
	}
    }

    //
    // Print the attribute value list to stdout.
    //

    if ( cmdPrintResponse )
    {
	avList.print( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhs++Client <clientName> <serverAddress> <serverName>
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (in) Number of command line arguments.
// (>) argv (in) List of command line arguments.
//
// FUNCTION VALUE:
// (int) Program return status.
//
// PURPOSE:
// Test program to exercise the client part of the DHS++ library
//
// DESCRIPTION:
// Do lots of stuff.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// More error checking wouldn't hurt.
//-
//***********************************************************************
//

int	main 
( 
    int		argc, 
    char	*argv[]
)
{
    ulong		dims[7];
    char		dataLine[80000];
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    int			numTokens;
    const char		*msg;
    DHS_ERR_LEVEL	errLev;
    DHS_STATUS		errNum;
    cmd			*pCmd;		// A command to send.
    DHS_STATUS		status( DHS_S_SUCCESS );
    					// Function return status.
    const char  	*str;
    uint		timeout;
    char		*tokens[2000];
    bool		printResponse = false;
    bool		timeCommand = false;
    struct timespec	ts1;
    struct timespec	ts2;
    float		aTime;
    char		aTimeStr[10];
    int			argIndex;


    //
    // Parse Command Line.
    //
    
    if ( argc < 4 || argc > 6 )
    {
	std::cout << "usage: dhsCmdSender <clientName> <serverAddress> "
		"<serverName> [ -printResult ] [ -timeCommand ]"
		<< std::endl;
	exit( -1 );
    }

    for ( argIndex = 4; argIndex < argc; argIndex++ )
    {
	if ( strcmp( argv[argIndex], "-printResult" ) == 0 )
	{
	    printResponse = true;
	}
	else if ( strcmp( argv[argIndex], "-timeCommand" ) == 0 )
	{
	    timeCommand = true;
	}
	else
	{
	    std::cout << "usage: dhsCmdSender <clientName> <serverAddress> "
		    "<serverName> [ -printResult ] [ -timeCommand ]"
		    << std::endl;
	    exit( -1 );
	}
    }


    //
    // the try block catches exceptions, mostly in the constructors.
    //

    try
    {
	//
	//  Create the cDhs object.  This initializes the dhs library.
	//

	cDhs	dhsl( argv[1], 102*1024, DHS_DEBUG_OFF );


	//
	//  Create the error handler to handle errors from the library.
	//

	cDhsErrorHandler<cError>
			errHandler;
	

	//
	// Create a new connection object.
	//

	cDhsConnection	connect( argv[2], argv[3] );



	//puts( "after init" );

	
	//
	// Start the dhs event loop in a thread.
	//

	dhsl.startThread( status );


	//
	//  Open the connection to the server.
	//

	connect.open( status );


	for ( std::cin.getline( dataLine, sizeof( dataLine ) );
		! std::cin.eof() && status == DHS_S_SUCCESS; )
	{
	    strtokens( dataLine, 20, tokens, &numTokens );
	    if ( numTokens == 0 || tokens[0][0] == '#' )
	    {
		std::cin.getline( dataLine, sizeof( dataLine ) );
		continue;
	    }

	    if ( numTokens == 2 )
	    {
		timeout = atoi( tokens[1] );
	    }
	    else
	    {
		timeout = 0;
	    }

	    if ( numTokens == 1 || numTokens == 2 )
	    {
		pCmd = new cmd( tokens[0], printResponse );

		for ( std::cin.getline( dataLine, sizeof( dataLine ) );
			! std::cin.eof() && status == DHS_S_SUCCESS && 
			dataLine[0] == '\t';
			std::cin.getline( dataLine, sizeof( dataLine ) ) )
		{
		    strtokens( dataLine, 2000, tokens, &numTokens );
		    if ( numTokens >= 3 )
		    {
			if ( strcmp( tokens[2], "yes" ) == 0 )
			{
			    dims[0] = numTokens - 3;
			    if ( strcmp( tokens[1], "DHS_DT_STRING" ) == 0 )
			    {
				pCmd->add( tokens[0], DHS_DT_STRING, 1, 
					dims, &(tokens[3]), status );
			    }
			    else if ( strcmp( tokens[1], "DHS_DT_UINT32" ) == 0 )
			    {
				uint	*vals = (uint *) malloc( dims[0] * sizeof( uint ) );
				int	i;

				for ( i = 0; i < dims[0]; i++ )
				{
				    vals[i] = atoi( tokens[i+3] );
				}
				pCmd->add( tokens[0], DHS_DT_UINT32, 1, 
					dims, vals, status );
				free( vals );
			    }
			    else if ( strcmp( tokens[1], "DHS_DT_INT32" ) == 0 )
			    {
				int	*vals = (int * ) malloc( dims[0] * sizeof( int ) );
				int	i;

				for ( i = 0; i < dims[0]; i++ )
				{
				    vals[i] = atoi( tokens[i+3] );
				}
				pCmd->add( tokens[0], DHS_DT_INT32, 1, 
					dims, vals, status );
				free( vals );
			    }
			}
			else
			{
			    if ( strcmp( tokens[1], "DHS_DT_INT32" ) == 0 )
			    {
				pCmd->add( tokens[0], DHS_DT_INT32, atoi(tokens[3]), 
					status );
			    }
			    else if ( strcmp( tokens[1], "DHS_DT_UINT32" ) == 0 )
			    {
				pCmd->add( tokens[0], DHS_DT_UINT32, atoi(tokens[3]), 
					status );
			    }
			    else 
			    //if ( strcmp( tokens[1], "DHS_DT_STRING" ) == 0 )
			    {
				pCmd->add( tokens[0], DHS_DT_STRING, tokens[3], 
					status );
			    }
			}
		    }
		}
		pCmd->print( status );

		if ( timeCommand )
		{
		    clock_gettime( CLOCK_REALTIME, &ts1 );
		}

		pCmd->apply( connect, status );

		if ( timeCommand )
		{
		    clock_gettime( CLOCK_REALTIME, &ts2 );
		}

		if ( timeout != 0 )
		{
		    usleep( timeout );
		    pCmd->cmdAbort( dhsStatus );
		}

		pCmd->wait( status );
		delete pCmd;

		if ( timeCommand )
		{
		    aTime = (float) ts2.tv_sec - (float) ts1.tv_sec;
		    if ( ts2.tv_nsec > ts1.tv_nsec )
		    {
			aTime += ( (float) ts2.tv_nsec - ts1.tv_nsec ) / 
				1000000000;
		    }
		    else
		    {
			aTime += 1 + ( (float) ts2.tv_nsec - ts1.tv_nsec ) / 
				1000000000;
		    }
		    sprintf( aTimeStr, "%5.3f", aTime );
		    std::cout << std::endl << "Time until command accepted: " << 
			    aTimeStr << std::endl;
		}
	    }
	    else
	    {
		std::cin.getline( dataLine, sizeof( dataLine ) );
	    }
	}


	//
	// Check to see if an error occured anywhere above, and if one did,
	// print out the messages.
	//

	if ( status != DHS_S_SUCCESS )
	{
	    std::cout << "status: " << status << std::endl;
	    for ( DHS_STATUS s1 = DHS_S_SUCCESS; s1 == DHS_S_SUCCESS; )
	    {
		msg = cDhs::message( errNum, errLev, s1 );
		std::cout << msg << std::endl;
		cDhs::messageClear( s1 );
	    }
	    throw status;
	}

	//std::cout << "exiting" << std::endl;
    }
    catch( const DHS_STATUS s )
    {
        DHS_STATUS      st;
        DHS_STATUS      s1( DHS_S_SUCCESS );
        DHS_ERR_LEVEL   el;
 
 
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
        return( -1 );
    }

    //std::cout << "done" << std::endl;
}
