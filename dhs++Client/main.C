static char rcsid[] = "$Id: main.C,v 1.1.1.1 2002-11-24 20:25:10 brighton Exp $";
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
// MEHTOD NAME(S)
// main - The main routine only.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:32  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1998/05/05 19:27:40  nhill
// *** empty log message ***
//
// Revision 1.6  1997/07/14 23:38:51  nhill
// Updated for a new version of dhs/icd3.
//
// Revision 1.5  1997/05/13 17:37:06  nhill
// Fixed up the cleanup after puts.
//
// Revision 1.4  1997/05/01 23:32:39  nhill
// Removed the wait for put completion. It is unsafe when the put request
// is being deleted in the done method.
//
// Revision 1.3  1997/04/24 20:28:13  nhill
// Added some Const keywords.
//
// Revision 1.2  1997/04/16 21:06:11  nhill
// *** empty log message ***
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

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>

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

    cout << message() << endl;
}

//
//***********************************************************************
//+
// CLASS NAME:
// cError2
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

class	cError2: public cDhsError
{
    private:
	void	error();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cError2::error
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

void		cError2::error()
{
    //
    // The cDhsError provides various accessor functions for getting 
    // information about the error, including "message" to return the
    // text.
    //

    cout << "debugMessage" << endl;
    cout << message() << endl;
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
    	cmd( char * );
    private:
	void done( DHS_CMD_STATUS, char *, const cDhsAvList & );
	void response( DHS_CMD_STATUS, char *, const cDhsAvList & );
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
    char	*name
)
: cDhsClientCommand( name )
{
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
    cout << "command done" << endl;

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
// cmd::Response
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


    cout << "command response" << endl;
    cout << "state: " << cs << endl;
    if ( msg != NULL )
    {
	cout << "message: " << msg << endl;
    }


    //
    // Print the attribute value list to stdout.
    //

    avList.print( status );
}

//
//***********************************************************************
//+
// CLASS NAME:
// putDataset
//
// PUBLIC METHODS: 
// (None) putDataset - Constructor for the put datasets.
// (None) ~putDataset - Destructor for the put datasets.
// (void) waitAll - Wait for all outstanding puts to complete.
//
// PURPOSE:
// An object of this type should be created for each dataset to be sent
// to a server.
//
// The counter & condition variable stuff in this class don't have much
// to do with the DHS or DHS++ libraries, but is just an attempt at
// implementing the waitAll function.
//
//-
//***********************************************************************
//

class putDataset: public cDhsPutDataset
{
    public:
			putDataset( void );
			putDataset( putDataset & );
			putDataset( void *, unsigned long, DHS_BD_PUT_TYPE);
			~putDataset( void );
	static void	waitAll( int );

    private:
	void		done( DHS_CMD_STATUS, char * string, cDhsAvList & );

	static int	counter;	// Count number of existing datasets.
	static cCond	cond;		// Condition variable for this class.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::putDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) ds (putDataset &) Source dataset.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy constructor for a putDataset
//
// This is here to allow datasets to be copied and sent to the server again.
// A useful feature for a test program, but probably of no use at all
// for a real application.
//
// DESCRIPTION:
// call the cDhsPutDataset copy constructor and increment the object counter.
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

		putDataset::putDataset
(
    putDataset	&ds
): cDhsPutDataset( ds )
{
    cond.lock();
    counter ++;
    cond.unlock();
    cout << "counter: " << counter << endl;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::putDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Default constructor for a putDataset
//
// This constructor would be used when a dataset structure is to be created
// using the cDhsDataset methods.
//
// DESCRIPTION:
// Increment the object counter.
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

		putDataset::putDataset
(
    void
)
{
    cond.lock();
    counter ++;
    cond.unlock();
    cout << "counter: " << counter << endl;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::putDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pBuffer (void *) Buffer to be sent.
// (>) length (unsigned long) Length of the buffer to be sent.
// (>) type (DHS_BD_PUT_TYPE) The type of data in the buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a putDataset
//
// This constructor would be used when the data to be sent is in
// a memory buffer.
//
// DESCRIPTION:
// Call the cDhsPutDataset constructor and increment the object counter.
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

		putDataset::putDataset
(
    void	*pBuffer,
    unsigned long length,
    DHS_BD_PUT_TYPE type
) : cDhsPutDataset( pBuffer, length, type )
{
    cond.lock();
    counter ++;
    cond.unlock();
    cout << "counter: " << counter << endl;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::~putDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for a put dataset
//
// DESCRIPTION:
// Decrement the object counter and broadcast to the condition variable
// so the waitAll function will check to see if the counter has reached 0.
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

		putDataset::~putDataset
(
    void
)
{
    cond.lock();
    counter --;
    cond.unlock();
    cout << "counter: " << counter << endl;


    //
    // This broadcast isn't needed by the DHS libraries, but is only a way
    // for the waitAll function to wait until all existing putDatasets
    // have been destroyed.
    //

    cond.broadcast();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::done
//
// INVOCATION:
// n.a.  This function is called by the dhs++ library when a server indicates
// a dataset is complete.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cs (DHS_CMD_STATUS) The command status.
// (>) string (char *) The string associated with the command status.
// (>) avList (avList &) The string associated with the command status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This fuction is called when a put operation completes
//
// DESCRIPTION:
// Print a mesage and delete "this".  Note that by deleting "this" I have
// made it imposible to use the cDhsPutDataset::wait function to wait
// for the dataset to complete, since there is no way to know how long
// the putDataset will continue to exist after teh cDhsPutDataset::put
// method is executed.  That is why the putDataset::waitAll function exists.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function assumes that "this" was allocated on the heap.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	putDataset::done
(
    DHS_CMD_STATUS	,
    char		*string,
    cDhsAvList		&avList
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    if ( string != NULL )
    {
	cout << "string: " << string << endl;
    }


    avList.print( status );


    //
    //  Destroy the putDataset object.
    //

    if ( isDone( status ) )
    {
	cout << "put is complete" << endl;
	delete this;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::waitAll
//
// INVOCATION:
// putDataset::waitAll();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Wait until all putDatasets have been deleted
//
// The datasets will be deleted by the "done" method as the dhs library
// executes it to indicate completion of the put.
//
// DESCRIPTION:
// The function waits on the condition variable until the counter becomese
// zero. The counter is decremented in the destructor for the putDataset.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// This would work better if the "put" function incremented the counter
// and the and the done function decremented it, so "active" puts would
// be counted, not just datasets in existance, but this is good enough
// for a test program.
//-
//***********************************************************************
//

void	putDataset::waitAll( int count )
{
    cond.lock();

    //
    //  Loop until no putDatasets exist.
    //

    while( counter > count )
    {
	//
	// Wait for the destructor to do a broadcast.
	//

	cond.wait();
    }
    cond.unlock();
}

//
//***********************************************************************
//+
// CLASS NAME:
// getDataset
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this type is created to request a dataset from a server
//
//-
//***********************************************************************
//

class getDataset: public cDhsGetDataset
{
    private:
	void done( DHS_CMD_STATUS, char * string, cDhsAvList & );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDataset::done
//
// INVOCATION:
// n.a. This function is executed by the dhs library when a get is complete.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cs (DHS_CMD_STATUS) The command status.
// (>) avList (cDhsAvList &) The avlist from the response.
// (>) string (char *) The string associated with the command status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is executed when the get request is complete
//
// This function should take care of doing whatever is necessary
// to complete the processing of the request.
//
// DESCRIPTION:
// Print some messages, and if the data was sent as a dataset stucture,
// print the structure.
//
// Accessor functions in the cDhsGetDataset class provide information 
// about the data.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// The implementation of this is inconsistant with the equivalanet for
// commands in that both responses and completion messages come to the
// same function.   The inconsistancy should be removed.
//-
//***********************************************************************
//

void	getDataset::done
(
    DHS_CMD_STATUS	cs,
    char		*string,
    cDhsAvList		&avList
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    cout << "type: " << type() << endl;


    //
    // This check is nessasary since the function is also called when
    // the get command changes from DHS_CS_PENDING to DHS_CS_BUSY.
    // There should also be checks for errors.
    //

    avList.print( status );

    if ( cs == DHS_CS_DONE )
    {
	cout << "get is complete" << endl;
    }

    if ( string != NULL )
    {
	cout << "get done string: " << string << endl;
    }
    print( status );
}



//
// Initialize some static data members.
//

int 	putDataset::counter = 0;
cCond	putDataset::cond;

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
    char		buffer[1024];	// Data to send as a raw buffer.
    cDhsFrame		frame;		// A frame in a dataset.
    cDhsFrame		frame2;		// Another frame in a dataset.
    char		label[40];	// A data label.
    cmd			*pCmd;		// A command to send.
    cmd			*pCmd2;		// Another command to send.
    putDataset		*pDs;		// A dataset to send.
    putDataset		*pDs1;		// Another dataset to send.
    getDataset		*pGetDs;	// A dataset to retrieve.
    long		*pData;		// Pointer to a data buffer.
    DHS_STATUS		status( DHS_S_SUCCESS );
    					// Function return status.
    ulong		ndims[7];	// An attribute dimensions.
    int			intArray[5][10];// An array to send as an attribute.
    const char		*msg;		// A message string.
    DHS_STATUS		errNum;		// An error number.
    DHS_ERR_LEVEL	errLev;		// An error level.
    int			i,j;
    ulong		uli, ulj;
    const char		*str;


    cout << "starting" << endl;

    if ( argc != 4 )
    {
	cout << "usage: dhs++Client <clientName> <serverAddress> <serverName>"
		<< endl;
	exit( -1 );
    }


    //
    // the try block catches exceptions, mostly in the constructors.
    //

    try
    {
	//
	//  Create the cDhs object.  This initializes the dhs library.
	//

	cDhs	dhsl( argv[1], 102*1024, DHS_DEBUG_ON );


	//
	//  Create the error handler to handle errors from the library.
	//

	cDhsErrorHandler<cError>
			eh1;
	cDhsErrorHandler<cError>
			eh2( DHS_S_CONNECT );
	cDhsErrorHandler<cError2>
			eh3( DHS_EL_DEBUG );
	

	//
	// Create a new connection object.
	//

	cDhsConnection	connect( argv[2], argv[3] );


	cout << "after init" << endl;

	
	//
	// Start the dhs event loop in a thread.
	//

	dhsl.startThread( status );


	//
	//  Open the connection to the server.
	//

	connect.open( status );


	//
	// Create a new command to send to the server.
	//

	pCmd = new cmd( "testCommand" );


	//
	// Add a variety of attributes to the command.
	//

	pCmd->add( "char", DHS_DT_CHAR, 'a', status );
	pCmd->add( "byte", DHS_DT_INT8, 10, status );
	pCmd->add( "short", DHS_DT_INT16, (short) 20, status );
	pCmd->add( "long", DHS_DT_INT32, (long) 30, status );
	pCmd->add( "float", DHS_DT_FLOAT, 1.5, status );
	pCmd->add( "double", DHS_DT_DOUBLE, 1.5, status );
	pCmd->add( "string", DHS_DT_STRING, "hi there", status );
	pCmd->add( "boolean", DHS_DT_BOOLEAN, true, status );


	//
	// Add an array attirbute to the command.
	//

	ndims[0] = 5;
	ndims[1] = 10;
	for ( uli = 0; uli < ndims[0]; uli++ )
	{
	    for ( ulj = 0; ulj < ndims[1]; ulj++ )
	    {
		intArray[uli][ulj] = ulj + uli * ndims[1];
	    }
	}
	pCmd->add( "intarray", DHS_DT_INT32, 2, ndims, intArray, 
		status );


	//
	//  Print the command to stdout.
	//

	pCmd->print( status );


	//
	// Send the command to the server.
	//

	pCmd->apply( connect, status );


	//
	//  Create another command, pretty much like the last one.
	//

	pCmd2 = new cmd( "cmd2" );

	pCmd2->add( "char", DHS_DT_CHAR, 'a', status );
	pCmd2->add( "byte", DHS_DT_INT8, 10, status );
	pCmd2->add( "short", DHS_DT_INT16, (short) 20, status );
	pCmd2->add( "long", DHS_DT_INT32, (long) 30, status );
	pCmd2->add( "float", DHS_DT_FLOAT, 1.5, status );
	pCmd2->add( "double", DHS_DT_DOUBLE, 1.5, status );
	pCmd2->add( "string", DHS_DT_STRING, "hi there", status );
	pCmd2->add( "boolean", DHS_DT_BOOLEAN, true, status );

	pCmd2->print( status );


	//
	//  Send the second command to the server.
	//

	pCmd2->apply( connect, status );


	//
	//  Wait for the commands to complete.
	//

	pCmd->wait( status );
	pCmd2->wait( status );


	//
	//  Send the second command again just to see if a command 
	//  can be reused.
	//

	pCmd2->apply( connect, status );
	pCmd2->wait( status );


	//
	//  Print out the status value to see if any of the above functions
	//  failed.
	//

	cout << "status: " << status << endl;


	//
	// Delete both commands.
	//

	delete pCmd;
	delete pCmd2;



	//
	// Now for some bulk data stuff.
	//

	//
	// Create a new dataset and add a bunch of attributes.
	//

	pDs = new putDataset;
	pDs->attribAdd( "char", DHS_DT_CHAR, 'a', status );
	pDs->attribAdd( "byte", DHS_DT_INT8, 10, status );
	pDs->attribAdd( "short", DHS_DT_INT16, (short) 20, status );
	pDs->attribAdd( "long", DHS_DT_INT32, (long) 30, status );
	pDs->attribAdd( "float", DHS_DT_FLOAT, 1.5, status );
	pDs->attribAdd( "double", DHS_DT_DOUBLE, 1.5, status );
	pDs->attribAdd( "string", DHS_DT_STRING, "hi there", status );
	pDs->attribAdd( "boolean", DHS_DT_BOOLEAN, true, status );
	pDs->attribAdd( "intarray", DHS_DT_INT32, 2, ndims, intArray, 
		status );


	//
	// Add a frame to the dataset containing a 256x512 int array.
	//

	ndims[0] = 256;
	ndims[1] = 512;
	frame = pDs->frameAdd( "test frame", 12, 
		DHS_DT_INT32, 2, ndims, pData, status );
	

	//
	//  Fill in the frame data.
	//

	for ( i = 0; i < 512*256; i++ )
	{
	    pData[i] = i;
	}


	//
	//  Add some frame specific attributes.
	//

	frame.attribAdd( "char", DHS_DT_CHAR, 'a', status );
	frame.attribAdd( "byte", DHS_DT_INT8, 10, status );
	frame.attribAdd( "short", DHS_DT_INT16, (short) 20, status );
	frame.attribAdd( "long", DHS_DT_INT32, (long) 30, status );
	frame.attribAdd( "float", DHS_DT_FLOAT, 1.5, status );
	frame.attribAdd( "double", DHS_DT_DOUBLE, 1.5, status );
	frame.attribAdd( "string", DHS_DT_STRING, "hi there", status );
	frame.attribAdd( "boolean", DHS_DT_BOOLEAN, true, status );


	//
	//  Add a second frame to the dataset, more or less the same as 
	//  the first one.
	//

	frame = pDs->frameAdd( "test frame2", 13, DHS_DT_INT32, 2, 
		ndims, pData, status );
	for ( i = 0; i < 512*256; i++ )
	{
	    pData[i] = i;
	}

	frame.attribAdd( "char", DHS_DT_CHAR, 'b', status );
	frame.attribAdd( "byte", DHS_DT_INT8, 100, status );
	frame.attribAdd( "short", DHS_DT_INT16, (short) 200, status );
	frame.attribAdd( "long", DHS_DT_INT32, (long) 300, status );
	frame.attribAdd( "float", DHS_DT_FLOAT, 10.5, status );
	frame.attribAdd( "double", DHS_DT_DOUBLE, 20.5, status );
	frame.attribAdd( "string", DHS_DT_STRING, "hi there again", status );
	frame.attribAdd( "boolean", DHS_DT_BOOLEAN, false, status );



	//
	//  Add a subframe to the last frame.
	//

	frame2 = frame.frameAdd( "test frame2", 13, DHS_DT_INT32, 2, 
		ndims, pData, status );
	

	//
	//  fill in the subframe data.
	//

	for ( i = 0; i < 512*256; i++ )
	{
	    pData[i] = i;
	}


	//
	// Add attributes to the subframe.
	//

	frame2.attribAdd( "char", DHS_DT_CHAR, 'b', status );
	frame2.attribAdd( "byte", DHS_DT_INT8, 100, status );
	frame2.attribAdd( "short", DHS_DT_INT16, (short) 200, status );
	frame2.attribAdd( "long", DHS_DT_INT32, (long) 300, status );
	frame2.attribAdd( "float", DHS_DT_FLOAT, 10.5, status );
	frame2.attribAdd( "double", DHS_DT_DOUBLE, 20.5, status );
	frame2.attribAdd( "string", DHS_DT_STRING, "hi there again", status );
	frame2.attribAdd( "boolean", DHS_DT_BOOLEAN, false, status );


	//
	//  Print the newly constructed dataset to stdout.
	//

	pDs->print( status );
	pDs->exportDataset( status );


	//
	//  These loops send the dataset to the server, over and over again.
	//  The wait after the inner loop is because IMP isn't too happy
	//  about large numbers of bulk data transfers being active at
	//  the same time.
	//

	for ( i = 0; i < 10 && status == DHS_S_SUCCESS; i++ )
	{
	    for ( j = 0; j < 25 && status == DHS_S_SUCCESS; j++ )
	    {
		//
		//  Create a new dataset based on the old dataset.  This
		//  isn't a useful thing to do in real life, but its great 
		//  for testing.  The dataset is deleted in the "done"
		//  method of the putDataset class.
		//

		pDs1 = new putDataset( *pDs);
		sprintf( label, "dataLabel%s%d", argv[1], i * 10 + j );
		pDs1->put( connect, label, true, status );
	    }
	    putDataset::waitAll( 1 );
	}


	
	//
	//  Delete the original dataset.
	//

	delete pDs;


	if ( status != DHS_S_SUCCESS )
	{
	    cout << "status: " << status << endl;
	    for ( DHS_STATUS s1 = DHS_S_SUCCESS; s1 == DHS_S_SUCCESS; )
	    {
		msg = cDhs::message( errNum, errLev, s1 );
		cout << msg << endl;
		cDhs::messageClear( s1 );
	    }
	    throw status;
	}


	//
	// Now try sending a data buffer (i.e. something not structured
	// as a dataset.
	//

	for ( i = 0; i < 1024; i++ )
	{
	    buffer[i] = i;
	}


	//
	//  Create a new dataset for sending the raw data.
	//

	pDs1 = new putDataset( buffer, 1024, DHS_BD_PT_FITS );
	pDs1->put( connect, "fitsBuffer", true, status );


	//
	//  Wait for all of the datasets to finish.
	//

	putDataset::waitAll( 0 );


	//
	//  Now retrieve a dataset from a server.
	//

	pGetDs = new getDataset;

	pGetDs->get( connect, "data label", DHS_BD_GT_FITS, status );


	//
	//  Wait for the get to complete.
	//

	pGetDs->wait( status );


	//
	// Delete the get dataset.
	//

	delete	pGetDs;


	//
	//  Get another datset, this time structured as a dataset.
	//

	pGetDs = new getDataset;

	pGetDs->get( connect, "data label", DHS_BD_GT_RAW, status );
	pGetDs->wait( status );

	delete	pGetDs;


	//
	// This causes the test server to exit.  Sometimes useful for 
	// checking for memory leaks in the server and things like that.
	//

#ifdef OMIT
	pCmd = new cmd( "quit" );
	pCmd->apply( connect, status );
	pCmd->wait( status );
	delete pCmd;
#endif // OMIT


	//
	// Close the connection to the server.
	//

	connect.close( status );


	//
	// Check to see if an error occured anywhere above, and if one did,
	// print out the messages.
	//

	if ( status != DHS_S_SUCCESS )
	{
	    cout << "status: " << status << endl;
	    for ( DHS_STATUS s1 = DHS_S_SUCCESS; s1 == DHS_S_SUCCESS; )
	    {
		msg = cDhs::message( errNum, errLev, s1 );
		cout << msg << endl;
		cDhs::messageClear( s1 );
	    }
	    throw status;
	}

	cout << "exiting" << endl;
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
	return( -1 );
    }

    cout << "done" << endl;
}
