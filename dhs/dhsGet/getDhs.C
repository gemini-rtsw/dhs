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
// dhsGet/src/getDhs.C
//
// PURPOSE:
// Class methods to handle the dhs library startup, init, and errors.
//
// METHOD NAME(S)
// getDhsError 			- Error handler class.
// getDhsError::error 		- Error handler.
// getDhs::~getDhs 		- Deconstructor.
// getDhs::exit			- Get rid of dhs handlers and objects.
// getDhs::init			- Setup handlers and init dhs library.
// getDhs::kill			- Force exit.
// getDhs::start		- Start the event loop.
// getDataset::datasetWrite	- Write the dataset, if possible.
// getDataset::done
// 
//
//INDENT-OFF*
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


#include <iostream>


#include "globals.H"
#include "getDhs.H"
extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
}

static void	delHandler( cDhsHandler *handler )
{
    delete handler;
}

DHS_BD_GET_TYPE	getDhs::sdDataType = DHS_BD_GT_UNKNOWN;	// Get data type.

char		*getDhs::sdDataServer = NULL;	// Name for the data server.
char		*getDhs::sdDsIpAddr = NULL;	// Ip address of data server.
char		*getDhs::sdDestinDir = NULL;	// Ip address of data server.
cDhsConnection	getDhs::sdDataConnect;			// Data Server connection.
char		*getDhs::sdIdentity = NULL;
bool		getDhs::sdSimulate = false;
bool		getDhs::sdContinue = true;


//
//***********************************************************************
//+
// CLASS NAME:
// getDhsError
//
// PUBLIC METHODS: 
// (void) error - Handle errors.
//
// PURPOSE:
// An object of this type is created and executed each time an error 
// occurs in the DHS library. This class defines the error methods 
// originally declared as pure virtual in the cDhsError base class.
//
//-
//***********************************************************************
//

class 	getDhsError: public cDhsError
{
    private:
	void		error( void );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhsError::error
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when an error occurs in the DHS library
//
// DESCRIPTION:
// Print the error messages.
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

void		getDhsError::error
(
    void
)
{
    cGetStatus	status;

    //
    //  If its a debug message, print to stdout.
    //

    if ( level() == DHS_EL_DEBUG )
    {
	status.S_DEBUG_MSG( status, message() );
    }
    else
    {

	//
	//  Log the error, then print the message.
	//

	std::cout << "Error, dataset not retrievable : " << message() << std::endl;
	status.display();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhs::~getDhs
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
// Destructor for the getDhs class
//
// DESCRIPTION:
// Delete the dhs library object.
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

		getDhs::~getDhs
(
    void
)
{
    if ( sdPDhs != NULL )
    {
	delete (cDhs *) sdPDhs;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhs::exit
//
// INVOCATION:
// getDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cGetStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of all DHS handlers and the cDhs object.
//
// DESCRIPTION:
// For each handler attached, delete them.  Then delete the dhs library
// object and set the pointer to null.
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
void		getDhs::exit
(
    cGetStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Close connection to the dataServer.
    //

    checkDhs( sdDataConnect.close( dhsStatus), dhsStatus, status, return );


    //
    //  Remove each handler.
    //

    cDhsHandler::forEach( delHandler );

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhs::init
//
// INVOCATION:
// getDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cGetStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the dhs library
//
// DESCRIPTION:
// Set debug level, set up the dhs object, setup the handlers.
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

void		getDhs::init
(
    cGetStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    checkStat( status, return );


    //
    //  Set the debug level
    //

    switch ( cGetStatus::debugLevel() )
    {
	case	cGetStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cGetStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cGetStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }



    //
    //  Set up the dhs object.
    //

    checkNull( sdPDhs = new cDhs( sdIdentity, 1 * 51 * 1024, 
	    level ), status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDhsErrorHandler<getDhsError>, status, return );


}

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhs::kill
//
// INVOCATION:
// getDhs::kill( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cGetStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of the cDhs object.
//
// DESCRIPTION:
// Delete the dhs library object and set the pointer to null.
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
void		getDhs::kill
(
    VOID
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    //
    //  Close connection to the dataServer.
    //

    sdDataConnect.close( dhsStatus);


    //
    //  Remove each handler.
    //

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cGetStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the DHS library event loop
//
// DESCRIPTION:
// Check the status, then start the event loop.
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

void		getDhs::start
(
    cGetStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Start the event loop.
    //

    checkDhs( sdPDhs->startThread( dhsStatus ), dhsStatus, status, return );


    //
    // Connect to the Data Server.
    //

    checkDhs( sdDataConnect.open( sdDsIpAddr, sdDataServer, dhsStatus), 
	    dhsStatus, status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDataset::datasetWrite
//
// INVOCATION:
// getDataset.datasetWrite()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cGetStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Save the dataset to a file
//
// DESCRIPTION:
// Save the dataset to a file
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

void		getDataset::datasetWrite
(
    cGetStatus	&status		// (mod) Function return status.
)
{
    int         actWritten;             /* Num of bytes actully written.*/
    void	*data;			// Data buffer.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char        *fileName;		/* Stripped filename.           */
    FILE        *fp;                    /* File pointer.                */
    struct stat stat_buf;               /* Output of the stat call.     */


    checkStat( status, return );

    //
    //  Compose the destination filename.
    //

    if ( getDhs::getDestin() == NULL )
    {
	checkNull( ( fileName = (char *) gen_alloc( strlen( label() ) + 1 ) ),
		status, return );
	(void) strcpy( fileName, label() );
    }
    else
    {
	checkNull( ( fileName = (char *) gen_alloc ( 
		strlen( getDhs::getDestin() ) + strlen( label() ) + 2 ) ),
		status, return );
	(void) strcpy( fileName, getDhs::getDestin() );
	(void) strcat( fileName, "/" );
	(void) strcat( fileName, label() );
    }


    //
    //  Open the file for writing and Write the file to the destination.
    //

    if ( ( fp = fopen( fileName,  "w" ) ) == NULL )
    {
	status.E_NO_WRITE( status, fileName );
	status.sysErrno();
	status.display();
        return;
    }
    status.S_OPEN_FILE( status, fileName );


    //
    //  Write the file.
    //

    data = buffer();
    if ( data != NULL && length() != 0 )
    {
	actWritten = fwrite(data, length(), 1, fp );
    }
    else
    {
	status.S_NULL_BUFFER( status, fileName );
    }
    (void) fclose ( fp );


    //
    //  Check that the file is there and is the size expected.
    //
 
    if ( actWritten != 1 ||  ( stat( fileName,  &stat_buf ) == -1 ) )
    {
	status.E_BAD_WRITE( status, fileName, errno );
	status.display();
        return;
     }
    status.S_WRITTEN_FILE( status, fileName,
	    stat_buf.st_size );
 
 
    return;
}
    

//
//***********************************************************************
//+
// FUNCTION NAME:
// getDataset::done
//
// INVOCATION:
// getDataset.done()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cGetStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Maybe save the data
//
// DESCRIPTION:
// Maybe save the data
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

void		getDataset::done
(
    DHS_CMD_STATUS	cs,
    char		*string,
    cDhsAvList		&avList
)
{
    cGetStatus	status;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    if ( cs == DHS_CS_DONE )
    {
	//
	//  Save the data to the destination directory.
	//

	datasetWrite( status );

    }

    if ( cs == DHS_CS_ERROR ||  !status.ok() )
    {
	std::cout << "Get Request FAILED for dataset : "<< label();
    }
    else if ( cs == DHS_CS_DONE )
    {
	std::cout << "Get Request SUCCESSFUL for dataset : "<< label();
    }

    if ( cs != DHS_CS_BUSY )
    {
	//
	//  Print avlist.
	//

	avList.print( dhsStatus );

	if ( string != NULL )
	{
	    std::cout << std::endl << "message: " << string << std::endl;
	}
	else
	{
	    std::cout << std::endl;
	}

	//
	//  Unlock the mutex if in a complete state, so that is can be 
	//  release later on.

	sdMutex.unlock();

    }

}
