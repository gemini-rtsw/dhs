static char rcsid[] = "$Id: main.C,v 1.1.1.1 2002-11-24 20:30:02 brighton Exp $";
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
// dhs++Server/src/main.C
//
// PURPOSE:
// Test program to check out the behavour of a DHS++ library server.
//
// MEHTOD NAME(S)
// main - the main routine.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:33  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.5  1998/06/02 20:11:40  nhill
// Added the -noStrict option.
//
// Revision 1.4  1998/05/05 19:29:42  nhill
// *** empty log message ***
//
// Revision 1.3  1997/07/14 23:44:13  nhill
// Removed features no longer supported in ICD3.
//
// Revision 1.2  1997/04/16 21:10:48  nhill
// *** empty log message ***
//
// Revision 1.1  1997/02/28 18:25:40  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  This file contains no DHS++ client stuff, so compile time can be
//  improved by eliminating the client header files.
//

#include <iostream.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "dhsGenSignal.H"
#include "dhsGenStatus.H"

#define	DHS_NO_CLIENT
#include "dhs++.H"

extern "C"
{
    int	usleep( unsigned int );
}
static void sigHandler( int );

class 	cClntStatus: public cDhsStatus
{
    public:
	msgPrefix( "qls" );
};
bool	strict = true;




//
//***********************************************************************
//+
// CLASS NAME:
// cCmd
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// A dhs library command object
//
// A command handler is created latter that sets this class up as the
// default command class.  Any command not matching one of the specific
// commands will cause an object of this type to be created.
//
//-
//***********************************************************************
//

class	cCmd: public cDhsServerCommand
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command server.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This is a class to be used by the default command handler to handle
// commands not handled by specific command handlers.
//
// DESCRIPTION:
// Print some information and send some reponses.
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

void 		cCmd::exec()
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    //
    //  Print the command name.
    //

    cout << "default handler" << endl;
    cout << name() << endl;

    print( status );
    usleep( rand()/2000 );


    //
    // Send some respoonses to the client.  One would be enought, but this
    // is a test program.
    //

    response( DHS_CS_BUSY, "going", status );
    usleep( rand()/2000 );
    response( DHS_CS_BUSY, "going", status );
    response( DHS_CS_BUSY, "going", status );
    response( DHS_CS_BUSY, "going", status );
    if ( strict )
    {
	response( DHS_CS_ERROR, "Command not recognized", status );
    }
    else
    {
	response( DHS_CS_DONE, "ok.", status );
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cCmd2
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Class instantiated to handle a "cmd2" command.
//
// A command handler is created in main which takes care of creating
// objects of this class.
//
//-
//***********************************************************************
//

class	cCmd2: public cDhsServerCommand
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmd2::exec
//
// INVOCATION:
// n.a.  This function is executed by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called to begin processing a cmd2 command
//
// This function tests the ability to access the data in an attiribute
// value list/command.
//
// DESCRIPTION:
// Print out lots of information from the command.
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

void cCmd2::exec
(
    void
)
{
    cDhsAttrib	attrib;			// An attribute in the list.
    DHS_STATUS	status( DHS_S_SUCCESS );// Function return status.
    char	*attribName;		// Name of an attribute.
    DHS_DATA_TYPE			// Type of data in an attribute.
    		type;
    int		ndims;			// Number of dimensions of an attrib.
    unsigned long	dims[7];	// Dimensions of an attribute.
    void	*pData;			// Pointer to the data for an attrib.
    cDhsAvList	responseAvList;		// Atribute value list for the response.
    char	*msg;			// Error message from the DHS library.
    DHS_STATUS	errNum;			// Error number from the DHS library.
    DHS_ERR_LEVEL	errLev;		// Error level from the DHS library.



    //
    //  Print the command name
    //

    cout << "cmd2 handler" << endl;
    cout << name() << endl;


    //
    //  Print the attribute value list to stdout.
    //

    print( status );


    //
    // Find an attribute in the list.
    //

    attrib = find( "double", status );


    //
    // Get and display some information about the attribute.
    // Each of the info commands is tried in turn.
    //

    attrib.info( attribName, type, ndims, dims, &pData, status );
    cout << "name: " << attribName << " type: " << type << " ndims: " 
	    << ndims << " data: " << *(double *) pData << endl;
    
    attrib.info( attribName, type, status );
    cout << "name: " << attribName << " type: " << type << endl;

    attrib.info( ndims, dims, &pData, status );
    cout << " ndims: " << ndims << " data: " << *(double *) pData << endl;

    attrib.info( &pData, status );
    cout << " data: " << *(double *) pData << endl;


    //
    // Send a response to the client indicating the command processing has 
    // started.
    //

    response( DHS_CS_BUSY, "going", status );


    //
    // Create add an attribute to the response attribute value list.
    //

    responseAvList.add( "respAtt", DHS_DT_STRING, "value of string",
	    status );


    //
    // Send the attribute value list to the client.
    //

    response( DHS_CS_BUSY, responseAvList, status );


    if ( status == DHS_S_SUCCESS )
    {
	//
	// Indicate the command is complete.  
	//

	response( DHS_CS_DONE, status );
    }
    else
    {
	//
	// Indicate n error occured.
	//

	response( DHS_CS_ERROR, status );
    }


    //
    // Check for any errors that occured in the processing of the command.
    //

    for ( DHS_STATUS s1 = DHS_S_SUCCESS; s1 != DHS_S_NO_MESSAGE; )
    {
	msg = (char*)cDhs::message( errNum, errLev, s1 );
	cout << msg << endl;
	cDhs::messageClear( s1 );
    }
    
}

//
//***********************************************************************
//+
// CLASS NAME:
// cQuit
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this class is created to handle quit commands.
//
// Note that this is the only real command accepted by the
// server, everything else is for demonstration purposes.
//
//-
//***********************************************************************
//

class	cQuit: public cDhsServerCommand
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQuit::exec
//
// INVOCATION:
// n.a.  This function is automatically executed by the quit command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is executed to start the processing of a quit command
//
// DESCRIPTION:
// Stop the event loop which will caused the "start" method to return
// so the main program can clean up and exit.
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

void cQuit::exec()
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    cout << "quitting" << endl;


    //
    //  Indicate the command is complete.
    //

    response( DHS_CS_DONE, status );


    //
    // Stop the event loop.
    //

    cDhs::stop( status );
}

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
// An object of this type is created for each error message sent by the
// DHS library.
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
// error
//
// INVOCATION:
// n.a. This function is called by the error handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the processing for an error message
//
// DESCRIPTION:
// Print the message string to standard out.
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

void cError::error()
{
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
// An object of this type is created for each error message sent by the
// DHS library.
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
// error
//
// INVOCATION:
// n.a. This function is called by the error handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the processing for an error message
//
// DESCRIPTION:
// Print the message string to standard out.
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

void cError2::error()
{
    cout << "connection request" << endl;
    cout << message() << endl;
}

//
//***********************************************************************
//+
// CLASS NAME:
// cPutRequest
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this type is created to handle each put request.
//
//-
//***********************************************************************
//

class	cPutRequest: public cDhsPutRequest
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutRequest::exec
//
// INVOCATION:
// n.a. This function is called by the put handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the processing for a put request on a server
//
// Since the put handler is set up to start a new thread for each request,
// several cPutRequest object may exist at the same time, each running
// the exec function in a separate thread.
//
// DESCRIPTION:
// Write the data to a file, extract some frame and attribute data just
// to prove it can be done.
//
// Note that since this is a test program, there is much redundancy. In
// particular the same information is retrieved several times for some
// of the attributes and frames in slightly differnet ways.
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

void		cPutRequest::exec
(
    void
)
{
    cDhsAttrib	av;		// An attribute from a frame or dataset.
    DHS_STATUS	status = DHS_S_SUCCESS;
				// Function return status.
    cDhsFrame	frame;		// A frame from the dataset
    cDhsFrame	frame2;		// Another frame from the dataset
    char	*name;		// Name of a frame or attribute.
    DHS_DATA_TYPE dataType;	// Type of data in a frame or attribute.
    int		nDims;		// Number of dimensions of a frame or attribute.
    ulong	dims[7];	// Dimensions of a frame or attribute.
    int		*pData;		// Pointer to the data for a frame.
    int		fd;		// File descriptor to write to.
    cDhsAvList	respAvList;	// Response avList


    cout << "put handler" << endl;
    cout << length() << endl;
    cout << label() << endl;


    //
    // Write the data to a file.
    //

    response( DHS_CS_BUSY, "working", status );
    sleep( 1 );
#ifdef OMIT
    fd = open( label(), O_WRONLY | O_CREAT );
    write( fd, buffer(), length() );
    close( fd );
#endif // OMIT


#ifdef OMIT
    if ( type() == DHS_BD_PT_DS || type() == DHS_BD_PT_DS_QL )
    {
	//
	// Make the dataset accessable as a dataset structure.
	//

	access( status );


	//
	// Print the dataset to standard out.
	//

	print( status );


	//
	//  Get one of the frames from the dataset and print some information
	//  about it.
	//

	frame = frameFind( "test frame2", status );
	frame.info( name, dataType, nDims, dims, (void **) &pData, status );
	frame.info( name, dataType, status );
	frame.info( nDims, dims, (void **) &pData, status );

	if ( status == DHS_S_SUCCESS )
	{
	    cout << "name: " << name << " type: " << dataType << " nDims: " << 
		    nDims << " dims: " << dims[0] << " " << dims[1] << endl;
	    //
	    //  Get the subframe from the frame and print some information
	    //  about it.
	    //

	    frame2 = frame.frameFind( "test frame2", status );
	    frame2.info( name, dataType, nDims, dims, (void **) &pData, 
		    status );

	    if ( status == DHS_S_SUCCESS )
	    {
		cout << "name: " << name << " type: " << dataType << 
			" nDims: " << nDims << " dims: " << dims[0] << 
			" " << dims[1] << endl;
	    }
	    else
	    {
		status = DHS_S_SUCCESS;
		cout << "error finding subframe test frame2" << endl << 
			cDhs::message( status ) << endl;
	    }
	}
	else
	{
	    status = DHS_S_SUCCESS;
	    cout << "error finding test frame2" << endl <<
		    cDhs::message( status ) << endl;
	}



	//
	// Find a frame by index id.
	//

	frame = frameFind( 12, status );
	frame.info( name, dataType, nDims, dims, (void **) &pData, status );

	if ( status == DHS_S_SUCCESS )
	{
	    cout << "name: " << name << " type: " << dataType << " nDims: " << 
		    nDims << " dims: " << dims[0] << " " << dims[1] << endl;
	}
	else
	{
	    status = DHS_S_SUCCESS;
	    cout << "error finding frame 12" << endl <<
		    cDhs::message( status ) << endl;
	}


	//
	// find an attribute in the dataset and print some info about it.
	//

	av = attribFind( "long", status );
	av.info( name, dataType, nDims, dims, (void **) &pData, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " value: " << *pData << endl;


	//
	// Find an attribute in a frame and print some info about it.
	//

	av = frame2.attribFind( "long", status );
	av.info( name, dataType, nDims, dims, (void **) &pData, status );
	av.info( nDims, dims, (void **) &pData, status );
	av.info( (void **) &pData, status );
	av.info( name, dataType, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " value: " << *pData << endl;
    }
#endif
    //
    // Create add an attribute to the response attribute value list.
    //

    respAvList.add( "respAtt", DHS_DT_STRING, "put request: value of string",
	    status );

    response( DHS_CS_DONE, "put request: done now", respAvList, status );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cPutRequest2
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this type is created to handle each put request.
//
//-
//***********************************************************************
//

class	cPutRequest2: public cDhsPutRequest
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutRequest2::exec
//
// INVOCATION:
// n.a. This function is called by the put handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the processing for a put request on a server
//
// Since the put handler is set up to start a new thread for each request,
// several cPutRequest2 object may exist at the same time, each running
// the exec function in a separate thread.
//
// DESCRIPTION:
// Write the data to a file, extract some frame and attribute data just
// to prove it can be done.
//
// Note that since this is a test program, there is much redundancy. In
// particular the same information is retrieved several times for some
// of the attributes and frames in slightly differnet ways.
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

void		cPutRequest2::exec
(
    void
)
{
    cDhsAttrib	av;		// An attribute from a frame or dataset.
    DHS_STATUS	status = DHS_S_SUCCESS;
				// Function return status.
    cDhsFrame	frame;		// A frame from the dataset
    cDhsFrame	frame2;		// Another frame from the dataset
    char	*name;		// Name of a frame or attribute.
    DHS_DATA_TYPE dataType;	// Type of data in a frame or attribute.
    int		nDims;		// Number of dimensions of a frame or attribute.
    ulong	dims[7];	// Dimensions of a frame or attribute.
    int		*pData;		// Pointer to the data for a frame.
    int		fd;		// File descriptor to write to.
    cDhsAvList	respAvList;	// Response avList


    cout << "put handler 2" << endl;
    cout << length() << endl;


    //
    // Write the data to a file.
    //

#ifdef OMIT
    fd = open( label(), O_WRONLY | O_CREAT );
    write( fd, buffer(), length() );
    close( fd );
#endif // OMIT


    if ( type() == DHS_BD_PT_DS || type() == DHS_BD_PT_DS_QL )
    {
	//
	// Make the dataset accessable as a dataset structure.
	//

	access( status );


	//
	// Print the dataset to standard out.
	//

	print( status );


	//
	//  Get one of the frames from the dataset and print some information
	//  about it.
	//

	frame = frameFind( "test frame2", status );
	frame.info( name, dataType, nDims, dims, (void **) &pData, status );
	frame.info( name, dataType, status );
	frame.info( nDims, dims, (void **) &pData, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " dims: " << dims[0] << " " << dims[1] << endl;


	//
	//  Get the subframe from the frame and print some information
	//  about it.
	//

	frame2 = frame.frameFind( "test frame2", status );
	frame2.info( name, dataType, nDims, dims, (void **) &pData, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " dims: " << dims[0] << " " << dims[1] << endl;


	//
	// Find a frame by index id.
	//

	frame = frameFind( 12, status );
	frame.info( name, dataType, nDims, dims, (void **) &pData, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " dims: " << dims[0] << " " << dims[1] << endl;


	//
	// find an attribute in the dataset and print some info about it.
	//

	av = attribFind( "long", status );
	av.info( name, dataType, nDims, dims, (void **) &pData, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " value: " << *pData << endl;


	//
	// Find an attribute in a frame and print some info about it.
	//

	av = frame2.attribFind( "long", status );
	av.info( name, dataType, nDims, dims, (void **) &pData, status );
	av.info( nDims, dims, (void **) &pData, status );
	av.info( (void **) &pData, status );
	av.info( name, dataType, status );

	cout << "name: " << name << " type: " << dataType << " nDims: " << 
		nDims << " value: " << *pData << endl;
    }
    respAvList.add( "respAtt", DHS_DT_STRING, "put request2: value of string",
	    status );

    response( DHS_CS_DONE, "put request2: done now", respAvList, status );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cGetRequest
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this type is created for each get request received by
// the server.
//
//-
//***********************************************************************
//

class	cGetRequest: public cDhsGetRequest
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cGetRequest::exec
//
// INVOCATION:
// n.a. This function is called by the get handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called by the get handler to start the processing of a
// request
//
// DESCRIPTION:
// Invent some data to send back to the client.
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

void cGetRequest::exec
(
    void
)
{
    DHS_STATUS	status = DHS_S_SUCCESS;
				// Function return status.
    char	pBuffer[1024];	// Data buffer to send to the client
    cDhsAvList	respAvList;	// Response avList


    cout << "Get handler" << " type: " << type() << endl;
    

    //
    //  Send a response to the client so it can get on with its life.
    //

    response( DHS_CS_BUSY, "working", status );


    switch( type() )
    {
	case	DHS_BD_GT_FITS:
	case	DHS_BD_GT_FITS_ASIS:
	case	DHS_BD_GT_FITS_HEADER:
	case	DHS_BD_GT_RAW:
	case	DHS_BD_GT_RAW_ASIS:
	    //
	    //  Invent a data buffer to send to the client.
	    //

	    for ( int i = 0; i < 1024; i++ )
	    {
		pBuffer[i] = i;
	    }

	    respAvList.add( "respAtt", DHS_DT_STRING, 
		    "get request: value of string", status );
	    response( DHS_CS_DONE, "getRequest: done", respAvList, type(), 
		    pBuffer, 1024, status );
	    break;
    }
}

//
//***********************************************************************
//+
// CLASS NAME:
// cGetRequest2
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// An object of this type is created for each get request received by
// the server.
//
//-
//***********************************************************************
//

class	cGetRequest2: public cDhsGetRequest
{
    private:
	void	exec();
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cGetRequest2::exec
//
// INVOCATION:
// n.a. This function is called by the get handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called by the get handler to start the processing of a
// request
//
// DESCRIPTION:
// Invent some data to send back to the client.
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

void cGetRequest2::exec
(
    void
)
{
    DHS_STATUS	status = DHS_S_SUCCESS;
				// Function return status.
    char	pBuffer[1024];	// Data buffer to send to the client
    cDhsAvList	respAvList;	// Response avList


    cout << "Get handler 2" << " type: " << type() << endl;
    

    //
    //  Send a response to the client so it can get on with its life.
    //

    response( DHS_CS_BUSY, "working", status );


    switch( type() )
    {
	case	DHS_BD_GT_FITS:
	case	DHS_BD_GT_FITS_ASIS:
	case	DHS_BD_GT_FITS_HEADER:
	case	DHS_BD_GT_RAW:
	case	DHS_BD_GT_RAW_ASIS:
	    //
	    //  Invent a data buffer to send to the client.
	    //

	    for ( int i = 0; i < 1024; i++ )
	    {
		pBuffer[i] = i;
	    }

	    respAvList.add( "respAtt", DHS_DT_STRING, 
		    "get request2: value of string", status );
	    response( DHS_CS_DONE, "get request2: done", respAvList, type(), 
		    pBuffer, 1024, status );
	    break;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// main
//
// INVOCATION:
// dhs++Server <name>
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) argc (int) The number of command line arguments.
// (>) argv (char *[]) This list of command line arguments.
//
// FUNCTION VALUE:
// (int) Program return status.
//
// PURPOSE:
// Main program to run a dhs++ test server
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
    cClntStatus		dhsStatus;
    char		*name;
    
    if ( argc < 2 || argc > 3 )
    {
	cout << "usage: dhs++Server [ -noStrict ] <serverName>" << endl;
	exit (-1);
    }

    if ( argc == 3 )
    {
	if ( strcmp( argv[1], "-noStrict" ) == 0 )
	{
	    name = argv[2];
	    strict = false;
	} else if ( strcmp( argv[2], "-noStrict" ) == 0 )
	{
	    name = argv[1];
	    strict = false;
	}
    }
    else
    {
	name = argv[1];
    }

    dhsGenSignal( dhsStatus, SIGINT, sigHandler, NULL );

    cout << "starting" << endl;
    try
    {
	DHS_STATUS	status = DHS_S_SUCCESS;

	//
	// Initialize the dhs library.
	//
	cDhs	dhsl( argv[1], 5, DHS_DEBUG_ON );


	//
	// Create an error handler that will create and execute a cError object
	// for each message.
	//
	cDhsErrorHandler<cError>
			eh;
	cDhsErrorHandler<cError2>
			eh2( DHS_S_CONNECT );
	cDhsErrorHandler<cError2>
			eh3( DHS_S_DISCONNECT );

	//
	// Create a default command handler that will create and execute a 
	// cCmd object for each command not handled by another command handler.
	//

	cDhsCmdHandler<cCmd>
			cmdHandler( false );


	//
	// Create a command handler that will create and execute a cCmd2 object
	// for each "cmd2" command received.
	//

	cDhsCmdHandler<cCmd2>
			cmdHandler2( false, "cmd2" );


	//
	// Create a command handler that will create and execute a cQuit object
	// for each "quit" command received.
	//

	cDhsCmdHandler<cQuit>
			cmdHandler3( false, "quit" );
	

	//
	// Create a put handler that will create and execute a cPutRequest 
	// object for each bulk data put request received by the server.
	//

	cDhsPutHandler<cPutRequest> putHandler( true );
	cDhsPutHandler<cPutRequest2> putHandler2( true, DHS_BD_PT_FITS );


	//
	// Create a get handler that will create and execute a cGetRequest
	// object for each bulk data get request received by the server.
	//

	cDhsGetHandler<cGetRequest> getHandler( true );
	cDhsGetHandler<cGetRequest2> getHandler2( true, DHS_BD_GT_FITS );


	cout << "after init" << endl;

	//  dhsl.debugLevel( DHS_DEBUG_OFF, status );
	

	//
	//  Start the event loop.
	//

	dhsl.start( status );

	cout << "after event Loop" << endl;
    }
    catch( const DHS_STATUS s )
    {
	//
	//  Handle error from the DHS library.
	//

	cout << "died with status: " << s << endl;
	return( s );
    }
    catch( const char * s )
    {
	//
	// Some memory allocation errors will cause this exception.
	//

	cout << "died with string: " << s << endl;
	return( -1 );
    }
    cout << "done" << endl;
}

static void	sigHandler
(
    int		sig		/* (in)  The signal sent.		*/
)
{
    DHS_STATUS	status;


    fprintf( stderr, "handling signal\n" );
    status = DHS_S_SUCCESS;
    (void) dhsEventLoopEnd( &status );
}

