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
// dhsPut/src/putDhs.C
//
// PURPOSE:
// Class methods to handle the dhs library startup, init, and errors.
//
// METHOD NAME(S)
// putDhsError::error 		- Error handler.
// putDhs::~putDhs 		- Deconstructor.
// putDhs::exit			- Get rid of dhs handlers and objects.
// putDhs::init			- Setup handlers and init dhs library.
// putDhs::kill			- Force an exit.
// putDhs::start		- Start the event loop.
// putDhs::firstCmds		- Execute any preliminary cmds.
// putDhs::lifeCmd		- Send life ctl cmds to server.
// putDataset::putDataset	- Constructor.
// putDataset::done
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
#include "putDhs.H"
#include "list.H"
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

DHS_BD_PUT_TYPE	putDhs::sdDataType = DHS_BD_PT_UNKNOWN;	// Put data type.

bool		putDhs::sdBuffZero = FALSE;	// Last put with buffer zero.
bool		putDhs::sdContinue = TRUE;	
char		*putDhs::sdDataServer = NULL;	// Name for the data server.
char		*putDhs::sdDsIpAddr = NULL;	// Ip address of data server.
char		*putDhs::sdDSName = NULL;
char		*putDhs::sdIdentity = NULL;
char		putDhs::sdLifetime = 0;
bool		putDhs::sdNewLabel = FALSE;	
bool		putDhs::sdNoLast = FALSE;	
bool		putDhs::sdSimulate = false;

cDhsConnection	putDhs::sdDataConnect;			// Data Server connection.


//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhsError::error
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

void		putDhsError::error
(
    void
)
{
    cPutStatus	status;

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

	std::cout << "Error, file not sent successfully : " << message() << std::endl;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhs::~putDhs
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
// Destructor for the putDhs class
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

		putDhs::~putDhs
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
// putDhs::exit
//
// INVOCATION:
// putDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
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
void		putDhs::exit
(
    cPutStatus	&status		// (mod) Function return status.
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
// putDhs::init
//
// INVOCATION:
// putDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
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

void		putDhs::init
(
    cPutStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    checkStat( status, return );


    //
    //  Set the debug level
    //

    switch ( cPutStatus::debugLevel() )
    {
	case	cPutStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cPutStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cPutStatus::DEBUG_FULL:
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

    checkNull( new cDhsErrorHandler<putDhsError>, status, return );


}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhs::firstCmds
//
// INVOCATION:
// cmdDhsObj.firstCmds( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// If the lifetime is set, or new is set, do those command first
//
// DESCRIPTION:
// If the lifetime is set, or new is set, do those command first.
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

void		putDhs::firstCmds
(
    cPutStatus	&status		// (mod) Function return status.
)
{
    char	lifeChar;
    int		lifeInt;

    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  If new is set, then request a new label.
    //

    //
    //  If lifetime is set, then send a lifetime control cmd to the server.
    //

    if ( getDSName == NULL )
    {
	//
	//  Global dataset name not set, error out.
	//

	status.E_DATASETNAME_SET( status );
	return;
    }

    if ( ( lifeChar = getLife() ) != 0 )
    {
	switch ( lifeChar )
	{
	    case PUT_PERM:
		lifeInt = (int) DHS_BD_LT_PERMANENT;
		break;

	    case PUT_TEMP:
		lifeInt = (int) DHS_BD_LT_TEMPORARY;
		break;
	}

	lifeCmd( status, lifeInt );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhs::kill
//
// INVOCATION:
// putDhs::kill( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
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
void		putDhs::kill
(
    VOID
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    //
    //  Close connection to the dataServer.
    //

    sdDataConnect.close( dhsStatus );

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
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

void		putDhs::start
(
    cPutStatus	&status		// (mod) Function return status.
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
// putDataset::putDataset
//
// INVOCATION:
// putDataset = new pPutDs( filename );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Open the file and read it into the buffer.
//
// DESCRIPTION:
// Open the file and read it into the buffer.
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
    cPutStatus		&status,	// (mod) Status passed in.
    char		*filePath,	// (in)  Complete filename and path.
    void		**buffer,	// (out) The data read in.
    int			&dbSize,	// (out) Length of the data buffer.
    DHS_BD_PUT_TYPE	type,		// (in)  Data type.
    bool		zeroBuff	// (in)  True for zero buffer.
)
:cDhsPutDataset( NULL, 0, type )
{
    struct stat stat_buf;		// Output of the stat call.
    FILE	*fp;			// fp of the file.
    int		sizeRead;		// Size read.


    //
    //  Check the status and initialize buffer size to 0.
    //

    checkStat( status, return );
    dbSize = 0;


    //
    //  Do a stat on the file to make sure it exists and get the size.
    //

    if ( !zeroBuff )
    {

	if ( stat( filePath, &stat_buf ) == -1 )
	{
	    //
	    //  File doesn't exit. Error out.
	    //

	    status.E_NO_FILE( status, filePath );
	    return;
	}
	dbSize = stat_buf.st_size;


	//
	//  Open the file for reading.
	//

	if ( ( fp = fopen( filePath,  "r" ) ) == NULL )
	{
	    status.E_NO_READ( status, filePath );
	    status.sysErrno();
	    return;
	}
	status.S_OPEN_FILE( status, filePath );


	//
	//  Alloc room and read in the file.
	//

	*buffer = gen_alloc( (dbSize + 1) );
	sizeRead = fread( *buffer, dbSize, 1, fp );

	if ( sizeRead < 1 )
	{
	    //
	    // Nothing read.  Or read error.
	    //
	    
	    status.E_NO_FILE( status, filePath );
	}

	(void) fclose( fp );

    }
    return;
}
    

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDataset::done
//
// INVOCATION:
// putDataset.done()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Maybe free the data
//
// DESCRIPTION:
// Maybe free the data
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

void		putDataset::done
(
    DHS_CMD_STATUS	cs,
    char		*string,
    cDhsAvList 		&avList
)
{
    cPutStatus	status;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    if ( cs == DHS_CS_DONE )
    {
	//
	//  free the data buffer.
	//

//	( status );

    }

    if ( cs == DHS_CS_ERROR ||  !status.ok() )
    {
	std::cout << "Put Request FAILED for dataset     : "<< label();
    }
    else if ( cs == DHS_CS_DONE )
    {
	std::cout << "Put Request SUCCESSFUL for dataset : "<< label();
    }

    if ( cs != DHS_CS_BUSY )
    {
	cDhsAttrib  attrib;			// An attribute in the list.
	void	    *datasetName;		// datasetName.


	//
	//  Find the datasetName and print it.
	//

	attrib = avList.find( "datasetName", dhsStatus );
	attrib.info( &datasetName, dhsStatus);
	if ( dhsStatus == DHS_S_SUCCESS)
	{
	    std::cout << std::endl << "              Dataset name returned: "<< (char *) datasetName;
	}

	if ( string != NULL )
	{
	    std::cout << std::endl << "message: " << string << std::endl;
	}
	else
	{
	    std::cout << std::endl;
	}
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putDhs::lifeCmd
//
// INVOCATION:
// putDsh.lifeCmd()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Maybe free the data
//
// DESCRIPTION:
// Maybe free the data
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

void		putDhs::lifeCmd
(
    cPutStatus	&status,		// (mod) Function return status.
    int		lifeInt			// (in)  Life command to set.
)
{
    cDhsClientCommand		*command;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    
    if ( getDSName() != NULL )
    {
	command = new cDhsClientCommand( DP_BDCTL );

	checkDhs( command->add( DP_CTL, DHS_DT_INT32, DP_LIFE_CTL, dhsStatus ),
		dhsStatus, status, VOID );
	checkDhs( command->add( DP_LIFE, DHS_DT_INT32, lifeInt, dhsStatus ),
		dhsStatus, status, VOID );
	checkDhs( command->add( DP_DATASETNAME, DHS_DT_STRING, getDSName(),
		dhsStatus ), dhsStatus, status, VOID );

	checkDhs( command->apply( getConnect(), dhsStatus ),
		dhsStatus, status, VOID );
	checkDhs( command->wait( dhsStatus ),
		dhsStatus, status, VOID );

	if ( command->status( dhsStatus ) == DHS_CS_ERROR )
	{
	    status.E_LIFE_FAILED( status, getDSName() );
	}
	else if ( command->status( dhsStatus ) == DHS_CS_DONE )
	{
	    status.S_LIFE_SET( status, getDSName() );
	}
	delete command;
    }
    else
    {
	//
	//  Else cycle through the put list, and for each file listed, set
	//  the lifetime.
	//

	cPutList	*pPutList;
	cPutList::iterator
			i;
	bool		firstTime = TRUE;


	while ( status.ok() &&
		( pPutList = cPutList::getNext( i, firstTime ) ) != NULL )
	{
	    firstTime = FALSE;
	    if ( !doNext() )
	    {
		break;
	    }

	    command = new cDhsClientCommand( DP_BDCTL );

	    checkDhs( command->add( DP_CTL, DHS_DT_INT32, DP_LIFE_CTL, 
		    dhsStatus ), dhsStatus, status, VOID );
	    checkDhs( command->add( DP_LIFE, DHS_DT_INT32, lifeInt, dhsStatus ),
		    dhsStatus, status, VOID );
	    checkDhs( command->add( DP_DATASETNAME, DHS_DT_STRING, 
		    pPutList->getFileId(),
		    dhsStatus ), dhsStatus, status, VOID );

	    checkDhs( command->apply( getConnect(), dhsStatus ),
		    dhsStatus, status, VOID );
	    checkDhs( command->wait( dhsStatus ),
		    dhsStatus, status, VOID );

	    if ( command->status( dhsStatus ) == DHS_CS_ERROR )
	    {
		status.E_LIFE_FAILED( status, pPutList->getFileId() );
	    }
	    else if ( command->status( dhsStatus ) == DHS_CS_DONE )
	    {
		status.S_LIFE_SET( status, pPutList->getFileId() );
	    }
	    delete command;
	}
	firstTime = FALSE;
    }


}
