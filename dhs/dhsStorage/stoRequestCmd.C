static char rcsid[] = "$Id: stoRequestCmd.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoRequestCmd.C
//
// PURPOSE:
// Contains methods of the cStoRequestCmd class. 
//
// METHODS:
// cStoRequestCmd::config	- Read the configuration file.
// cStoRequestCmd::execStoArchiveFileAvailable	- Handle archive requests.
// cStoRequestCmd::execUserMediaRequest	- Handle user requests.
// cStoRequestCmd::addMediaFiles - Inserts the media files listed in the 
//				input parameters into a request.
// cStoRequestCmd::fileAvailParse - Parses the arguments from a file available 
//				command.
// cStoRequestCmd::newUserRequest - Creates a new user media request.
// cStoRequestCmd::userAppOptions - Read the user request AM application options
//				from the configuration file.
// cStoRequestCmd::userReqParseFileInfo - Parses the file information from 
//				a stoUserMediaRequest command.
// cStoRequestCmd::userReqParseReqInfo - Parses the request information 
//				from a stoUserMediaRequest command.
// cStoRequestCmd::validPath 	- Check to ensure that a file path is valid.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:19  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.21  2001/03/12 20:38:00  nhill
// Added type checking on command input parameters. Separated
// file available command parsing into a separate method.
//
// Revision 1.20  2000/12/19 19:28:38  jaeger
// Added some error checking.  Ensuring file IDs were not to big.
//
// Revision 1.19  2000/10/16 22:19:14  nhill
// Updated to be compatable with the workshop version of STL.
//
// Revision 1.18  2000/08/22 19:30:34  nhill
// Fixed a bug which caused file names to be wrong when data is retrieved
// through a get.
//
// Revision 1.17  2000/02/03 00:56:38  nhill
// Added function config to read request command things from the config
// file.
// Added function cStoRequestCmd::userAppOptions to parse lines from
// the config file.
// Changed function cStoRequestCmd::newUserRequest to set the ASTO options
// for the new media request to those read from the configuration file.
//
// Revision 1.16  2000/01/06 18:02:50  nhill
// Changed to permit an empty notify client.
// Added a check to verify that a file path contains a valid path.
//
// Revision 1.15  1999/10/12 23:00:52  nhill
// Changed to allow re-using of previously existing user requests.
//
// Revision 1.14  1999/06/21 19:57:37  nhill
// Fixed a bug when extracting file size from a command.
//
// Revision 1.13  1999/06/16 22:50:19  nhill
// Added an attribute containing the name of the client to notify when
// a user request is complete.
//
// Revision 1.12  1999/02/25 00:14:41  jaeger
// Removed intialization of ammTotalSize since it is no longer a member of
// the amMedia structure.
//
// Revision 1.11  1999/02/24 23:53:40  jaeger
// Made sure pConnect is initalized to NULL before calling STL find.
//
// Revision 1.10  1998/08/05 19:18:59  dunn
// Changed STO_RSP_BAD_ATTR_VALUE ,essage, which had 666 rather
// than a string in there.
//
// Revision 1.9  1998/07/31 21:02:25  cockayne
// Fixed user media request processing. It was previously not returning the
// media request identifier when the command was done.
//
// Revision 1.8  1998/06/24 16:04:51  cockayne
// Modified command AV list parsing to be more robust.
//
// Revision 1.7  1998/05/15 17:13:19  cockayne
// Modification for memory error checking.
//
// Revision 1.6  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.5  1998/04/20 21:49:27  cockayne
// Archive File Available commands now arrive with the media request name
// a.k.a. segregation dir, instead of the integer request id. Also made
// mods to cast retrieval sizes to 'long long'.
//
// Revision 1.4  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 21:04:17  cockayne
// Complete overhaul of user media request handling, due to new method
// of communicating the list of file information.
//
// Revision 1.2  1998/03/26 21:21:15  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoAction.H"
#include "stoMedia.H"
#include "stoConnection.H"
#include "stoRequestCmd.H"


//
// Define configuration strings
//

#define	STO_CFG_APPOPTIONS	"userAppOptions"


//
// Declare static data in cStoRequestCmd class.
//

std::map< std::string, std::string, std::less< std::string > >
		cStoRequestCmd::srcOptionList;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::config
//
// INVOCATION:
// cStoRequestCmd::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the user request configuration information from the configuration
// file
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// userAppOptions
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoRequestCmd::config
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    srcOptionList.clear();
    checkConfig( config_get( STO_CFG_APPOPTIONS, userAppOptions ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::execStoArchiveFileAvailable
//
// INVOCATION:
// execStoArchiveFileAvailable();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Process an archive-file command. 
//
// DESCRIPTION:
// Log receipt of the command. Ensure it is from a known connector. 
// Parse attribute-values from the command. Ensure a valid media ID.
// Ensure this is not a previously-seen file, then add the new file
// to the database. Update media object statistics to reflect the 
// file has been added. 
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

void		cStoRequestCmd::execStoArchiveFileAvailable
(
    void
)
{
    AM_FILE	amFile;			// am library file structure.
    int		amStatus;		// am library function return status.
    char	compFlag;		// Compression to use.
    DBPROCESS 	*dbProcess;		// Database process. 
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char	fileId[AM_FILE_ID_LEN];	// File Identifier string. 
    char	fileName[AM_FILE_NAME_LEN];
    					// File name string.
    int		fileSize; 		// File size.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    char	mediaRequestName[AM_MEDIA_REQUEST_NAME_LEN];
					// Media Request Name.
    cStoMedia	*pMedia;		// Ptr to media object. 
    cStoStatus  status;                 // Status.
    cDhsConnection
    		*pConnect;
    

    //
    // Respond to let client continue. Log receipt of command. 
    //

    checkDhs( response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus ),
	    dhsStatus, status, return );
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
	    dhsStatus, status, return );
    status.S_CMD_RECEIVED( status,  name(), id, ip );


    status.displayStop();


    //
    // Ensure command is coming from a known connector. If it's not,
    // we will not be able to notify of archival completion.
    //

    pConnect = NULL;
    pConnect = cStoConnection::find( id, NULL );
    if ( pConnect == NULL )
    {
	status.E_CMD_CONNECTION( status, name(), id, ip );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );
	status.display();
	return;
    }


    //
    // Parse command - get media ID, fileId, and size, fileName, and 
    // compression flag.
    //

    fileAvailParse( mediaRequestName, fileId, fileSize, fileName, 
	    compFlag, status );
    if ( ! status.ok() )
    {
	status.E_CMD_FORMAT( status, name() );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ), 
		dhsStatus, status, VOID );
	status.display();
	return;
    }


    //
    // Ensure media request ID is valid. 
    //

    pMedia = cStoMedia::findByName( mediaRequestName );
    if ( pMedia == NULL )
    {
	status.E_CMD_BAD_ATTR_VALUE( status, name(), 
		STO_CA_MEDIA_REQUEST_ID, mediaRequestName );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );
	status.display();
	return;
    }


    //
    // Check if this file ID has already been added to database. 
    //

    dbProcess = cStoDbManager::acquire();
    amStatus = amFileGet( AM_FG_MEDIA_FILE, &amFile, dbProcess, 
	    pMedia->getMediaRequestId(), fileId );
    cStoDbManager::release();
    if ( amStatus == AM_SUCCESS )
    {
	status.E_DUPLICATE_FILE( status, fileId );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );
	status.display();
	return;
    }
    else if ( amStatus != AM_NO_FILE )
    {
	checkAm( amStatus, status, VOID );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );
	return;
    }


    //
    // Load up am library file struct. Add new file to database. 
    //

    strcpy( amFile.amfFileId, fileId );
    strcpy( amFile.amfFilename, fileName );
    amFile.amfFileSize = fileSize;
    amFile.amfCompressionFlag = compFlag;
    amFile.amfMediaRequestId = pMedia->getMediaRequestId();

    strcpy( amFile.amfSourceId, id );
    amFile.amfStatus = AMF_UNPROCESSED;
    amFile.amfUncompFileSize = -1;
    amFile.amfDateNotified = time( NULL );
    amFile.amfMediaUnitName[0] = '\0';

    dbProcess = cStoDbManager::acquire();
    amStatus = amFilePut( &amFile, dbProcess );
    cStoDbManager::release();
    if ( amStatus != AM_SUCCESS )
    {
	checkAm( amStatus, status, VOID );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );;
	return;
    }


    //
    // Add size to amount retrievable for this media ID.
    //

    pMedia->lock();
    pMedia->setRetrieveSize( ( pMedia->getRetrieveSize() + 
	    (long long) fileSize ) );
    pMedia->unlock();
    

    //
    // Respond to client with DONE. 
    //

    checkDhs( response( DHS_CS_DONE, dhsStatus ), 
	    dhsStatus, status, return );

    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::execUserMediaRequest
//
// INVOCATION:
// execUserMediaRequest();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Process a user-media-request command. 
//
// DESCRIPTION:
// Log receipt of the command. Ensure command came from a known connector.
// Parse attribute-values from the command. Parse arrays from the command.
// Initialize am library structures, and fetch a new media identifier from
// the database for this new user media request. Add files to the database,
// then create a new media object. 
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

void		cStoRequestCmd::execUserMediaRequest
(
    void
)
{
    AM_MEDIA	amMedia;		// am library media request structure.
    int		amStatus;		// am library function return status.
    cDhsAttrib	attribute;		// attribute object.
    DBPROCESS 	*dbProcess;		// Database process. 
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char	mediaRequestName[AM_MEDIA_REQUEST_NAME_LEN];	
					// Media request name string.
    char	unitNameRoot[AM_UNIT_NAME_ROOT_LEN];		
    					// Unit name root string. 
    int		numCopies;		// Number of copies to make.
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    char	mediaType[AM_MEDIA_TYPE_LEN];		
    					// Type of media to write to.
    int		arraySize;
    char	**pFileIdArray;		// Pointer to attribute data.
    char	**pCompFlagArray;	// Pointer to attribute data.
    char	**pFileSizeArray;	// Pointer to attribute data.
    char	**pFilePathArray;	// Pointer to attribute data.
    cStoMedia	*pMedia;		// Ptr to media object. 
    char	responseString[256];	// String to format response into.
    bool	reuse = false;		// Try to re-use an existing request?
    cStoStatus  status;                 // Status.
    cDhsConnection
    		*pConnect;
    cDhsAvList  responseAvList;		// Response a-v list to build.
    char	notifyClient[50];	// Client to notify about completion.
    bool	usingOldReq( false );	// True if an old request is being
    					// re-used.


    //    
    // Respond to let client continue. Log receipt of command.
    //

    response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus );
    dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
    status.S_CMD_RECEIVED( status, name(), id, ip );


    //
    // Parse the request information from the command.
    //

    status.displayStop();
    userReqParseReqInfo( mediaRequestName, notifyClient, unitNameRoot, 
	    numCopies, mediaType, reuse, status );


    //
    // Parse the file information from the command.
    //

    userReqParseFileInfo( pFileIdArray, pFileSizeArray, pCompFlagArray, 
	    pFilePathArray, arraySize, status );

    if ( ! status.ok() )
    {
	status.E_CMD_FORMAT( status, name() );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ), 
		dhsStatus, status, VOID );
	status.display();
	return;
    }


    if ( strlen( notifyClient ) != 0 )
    {
	//
	// Ensure command is coming from a known connector. If it's not,
	// we will not be able to notify of archival completion.
	//

	pConnect = NULL;
	pConnect = cStoConnection::find( notifyClient, NULL );
	if ( pConnect == NULL )
	{
	    (void) sprintf( responseString, STO_RSP_UNAUTHORIZED, 
		    notifyClient, ip );
	    checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		    dhsStatus, status, return );
	    status.E_CMD_CONNECTION( status, name(), notifyClient, ip );
	    return;
	}
    }


    //
    // Initialize am media request structure. 
    //

    strcpy( amMedia.ammMediaRequestName, mediaRequestName );
    strcpy( amMedia.ammMediaRequestType, AM_RT_USER );
    strcpy( amMedia.ammMediaType, mediaType );
    amMedia.ammNumCopies = numCopies;
    strcpy( amMedia.ammUnitNameRoot, unitNameRoot );
    strcpy( amMedia.ammSourceId, notifyClient );
    amMedia.ammStatus = AMM_CREATING;
    amMedia.ammDateRequested = time( NULL );
    amMedia.ammDateCompleted = 0;
    amMedia.ammUnitNamePrecision = AM_DEFAULT_PRECISION;


    //
    // If the reuse flag is set, attempt to find another request
    // that can be used for this data.
    if ( reuse )
    {
	dbProcess = cStoDbManager::acquire();
	amStatus = amGet( AM_MG_REUSE, &amMedia, dbProcess );
	if ( amStatus == AM_SUCCESS )
	{
	    //
	    // Another requeset was found, make sure there is only
	    // one matching request.
	    //

	    amStatus = amGet( AM_MG_NEXT, &amMedia, dbProcess );
	    if ( amStatus == AM_NO_MEDIA )
	    {
		//
		// There was only one request, so use it.
		//

		usingOldReq = true;
	    }
	    else if ( amStatus == AM_SUCCESS )
	    {
		//
		// There is more than one request, which is an error.
		//

		status.E_MULTIPLE_REQUEST( status, 
			amMedia.ammMediaRequestName );
		checkDhs( response( DHS_CS_ERROR, status.message(), 
			dhsStatus ), dhsStatus, status, VOID );
		dbcanquery( dbProcess );
		cStoDbManager::release();
		status.display();
		return;
	    }
	    else
	    {
		dbcanquery( dbProcess );
		cStoDbManager::release();
		checkAm( amStatus, status, VOID );
		status.E_CMD_PROCESSING( status, name(), id, ip );
		checkDhs( response( DHS_CS_ERROR, status.message(), 
			dhsStatus ), dhsStatus, status, VOID );
		status.display();
		return;
	    }

	    if ( amMedia.ammStatus == AMM_DONE )
	    {
		//
		// If the status of the existing request is AMM_DONE, set the
		// status to AMM_IN_PROG.
		//

		amMedia.ammStatus = AMM_IN_PROG;
		amStatus = amPut( &amMedia, dbProcess );
		if ( amStatus != AM_SUCCESS )
		{
		    cStoDbManager::release();
		    checkAm( amStatus, status, return );
		}
		checkNull( ( new cStoMedia( &amMedia, true ) ), status, VOID );
	    }
	    else if ( amMedia.ammStatus != AMM_IN_PROG )
	    {
		//
		// If the media status is not AMM_IN_PROG, then this is an
		// error.
		//

		status.E_REUSE_REQ_STATUS( status, 
			amMedia.ammMediaRequestName, amMedia.ammStatus );
		checkDhs( response( DHS_CS_ERROR, status.message(), 
			dhsStatus ), dhsStatus, status, VOID );
		cStoDbManager::release();
		status.display();
		return;
	    }
	}
	else if ( amStatus != AM_NO_MEDIA )
	{
	    checkAm( amStatus, status, VOID );
	    dbcanquery( dbProcess );
	    cStoDbManager::release();
	    status.E_CMD_PROCESSING( status, name(), id, ip );
	    checkDhs( response( DHS_CS_ERROR, status.message(), 
		    dhsStatus ), dhsStatus, status, VOID );
	    status.display();
	    return;
	}

	dbcanquery( dbProcess );
	cStoDbManager::release();
    }

    if ( ! usingOldReq )
    {
	newUserRequest( amMedia, status );
	if ( ! status.ok() )
	{
	    status.E_CMD_PROCESSING( status, name(), id, ip );
	    checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ), 
		    dhsStatus, status, VOID );
	    status.display();
	    return;
	}
    }


    addMediaFiles( amMedia, pFileIdArray, pFileSizeArray, pCompFlagArray,
	    pFilePathArray, arraySize, status );
    if ( ! status.ok() )
    {
	status.E_CMD_PROCESSING( status, name(), id, ip );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ), 
		dhsStatus, status, VOID );
	status.display();
	return;
    }
    status.displayStart();


    if ( ! usingOldReq )
    {
	//
	// Now that all files have been parsed out correctly, update db, 
	// create object. Update EPICS. 
	//

	dbProcess = cStoDbManager::acquire();
	amMedia.ammStatus = AMM_IN_PROG;
	amStatus = amPut( &amMedia, dbProcess );
	cStoDbManager::release();
	if ( amStatus != AM_SUCCESS )
	{
	    (void) sprintf( responseString, "%s", STO_RSP_DB_ERROR );
	    checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		    dhsStatus, status, return );
	    status.E_CMD_PROCESSING( status, name(), id, ip );
	    checkAm( amStatus, status, return );
	}


	//
	// Create new media object, adding it to the list and updating EPICS. 
	//

	checkNull( ( pMedia = new cStoMedia( &amMedia ) ),
		status, VOID );
    }
    else
    {
	if ( ( pMedia = cStoMedia::findById( amMedia.ammMediaRequestId ) ) == 
		NULL )
	{
	    (void) sprintf( responseString, STO_RSP_MEDIAID_INVALID, 
		    amMedia.ammMediaRequestId );
	    checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		    dhsStatus, status, return );
	    status.E_CMD_PROCESSING( status, name(), id, ip );
	    return;
	}
    }
    check( pMedia->db2Stats( NULL, status ), status, VOID );


    //
    // Compose AV list with new media request id to return to client. 
    //

    checkDhs( responseAvList.add( STO_CA_MEDIA_REQUEST_ID, DHS_DT_INT32,
	    pMedia->getMediaRequestId(), dhsStatus ),
	    dhsStatus, status, VOID );

    
    //
    // Respond to client with DONE.
    //

    checkDhs( response( DHS_CS_DONE, responseAvList, dhsStatus ),
	    dhsStatus, status, return );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::addMediaFiles
//
// INVOCATION:
// AM_MEDIA	amMedia;
// char		**pfileIdArray, pFileSizeArray, pCompFlagArray, pFilePathArray;
// int		arraySize;
// cStoStatus	status;
// addMediaFiles( amMedia, pFileIdArray, pFileSizeArray, pCompFlagArray,
//	pFilePathArray, arraySize, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) amMedia (AM_MEDIA &) This is the AM media object associated with the
//				request.
// (>) pFileIdArray(char **) This is the array of file ids.
// (>) pFileSizeArray(char **) This is the array of file sizes.
// (>) pCompFlagArray(char **) This is the array of compression flags.
// (>) pFilePathArray(char **) This is the array of file paths.
// (>) arraySize(int) This is the number of elements in the arrays.
// (>) status(cStoStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method inserts the media files listed in the input parameters into
// the request in the amMedia parameter
//
// DESCRIPTION:
// Loop through each element in the arrays, parsing the strings into the
// required values in an AM_FILE structure, and calling amFilePut to 
// insert the AM_FILE structure into the database.
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

void		cStoRequestCmd::addMediaFiles
(
    const AM_MEDIA		// (in)  The media request that data are
		&amMedia,	//       to be written to.
    char	**pFileIdArray,	// (in)  The list of file ids.
    char	**pFileSizeArray,// (in) The list of file sizes.
    char	**pCompFlagArray,// (in) The list of compression flags.
    char	**pFilePathArray,// (in) The list of file paths.
    int		arraySize,	// (in)  The file array size.
    cStoStatus	&status		// (mod) Function return status.
)
{
    AM_FILE	amFile;		// The AM library file structure.
    int		amStatus;	// AM library return status.
    int		count;		// File counter.
    DBPROCESS	*dbProcess;	// The database connection to use.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );	
				// DHS library return status.
    char	*ext;		// Fine name extension pointer.
    char	responseString[256];	// String to format response into.


    checkStat( status, return );


    //
    // Prepare media file fields that are universal. 
    //

    amFile.amfMediaRequestId = amMedia.ammMediaRequestId;
    amFile.amfDateNotified = amMedia.ammDateRequested;
    amFile.amfUncompFileSize = -1;
    amFile.amfMediaUnitName[0] = '\0';
    amFile.amfSourceId[0] = '\0';
    amFile.amfStatus = AMF_UNPROCESSED;
    

    for ( count = 0; count < arraySize; count++ )
    {
	//
	// Load up media file struct, add file to database. 
	//

	if ( streq( pCompFlagArray[count], "GZIP" ) || 
		streq( pCompFlagArray[count], "TRUE" ) )
	{
	    amFile.amfCompressionFlag = AMF_COMP_GZIP;
	}
	else if ( streq( pCompFlagArray[count], "NONE" ) ||
		streq( pCompFlagArray[count], "FALSE" ) )
	{
	    amFile.amfCompressionFlag = AMF_COMP_NONE;
	}
	else if ( streq( pCompFlagArray[count], "ASIS" ) )
	{
	    ext = strext( pFilePathArray[count] );
	    if ( ext != NULL && streq( ext, "gz" ) )
	    {
		amFile.amfCompressionFlag = AMF_COMP_GZIP;
	    }
	    else
	    {
		amFile.amfCompressionFlag = AMF_COMP_NONE;
	    }
	}
	else
	{
	    status.E_CMD_BAD_ATTR_VALUE( status, name(), 
		    STO_CA_COMP_FLAG_ARRAY, pCompFlagArray[count] );
	    return;
	}
	strncpy( amFile.amfFileId, pFileIdArray[count], AM_FILE_ID_LEN );
	amFile.amfFileId[AM_FILE_ID_LEN-1] = '\0';
	amFile.amfFileSize = atoi( pFileSizeArray[count] );
	strncpy( amFile.amfFilename, pFilePathArray[count], AM_FILE_NAME_LEN );
	amFile.amfFilename[AM_FILE_NAME_LEN-1] = '\0';


	//
	// Check if this file ID has already been added to database. 
	//

	dbProcess = cStoDbManager::acquire();
	amStatus = amFileGet( AM_FG_UNIQUE, &amFile, dbProcess, 
		amFile.amfMediaRequestId, amFile.amfFileId,
		amFile.amfFilename );
	cStoDbManager::release();
	if ( amStatus == AM_SUCCESS )
	{
	    (void) sprintf( responseString, STO_RSP_DUPLICATE_FILE, 
		    amFile.amfFileId );
	    checkDhs( response( DHS_CS_BUSY, responseString, dhsStatus ),
		    dhsStatus, status, return );
	    status.S_DUPLICATE_FILE( status, amFile.amfFileId, 
		    amMedia.ammMediaRequestName, amMedia.ammMediaRequestId );
	    status.display();
	    status( status.S_SUCCESS );
	}
	else if ( amStatus != AM_NO_FILE )
	{
	    (void) sprintf( responseString, "%s", STO_RSP_DB_ERR );
	    checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		    dhsStatus, status, return );
	    checkAm( amStatus, status, return );
	}


	dbProcess = cStoDbManager::acquire();
	amStatus = amFilePut( &amFile, dbProcess );
	cStoDbManager::release();
	if ( amStatus != AM_SUCCESS )
	{
	    checkAm( amStatus, status, return );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::fileAvailParse
//
// INVOCATION:
// fileAvailParse( mediaRequestName, fileId, fileSize, fileName, 
//	compFlag, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) mediaRequestName (char *) This is the name of the media request.
// (<) fileId (char *) This is the file id of the file.
// (<) fileSize (int &) This is the size of the file.
// (<) fileName (char *) This is the file name of the file.
// (<) compFlag (char &) This is the compression flag.
// (>) status(cStoStatus &) The function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function parses the parameters from the archive file available
//	command.
//
// DESCRIPTION:
// Parse each of the parameters in turn.
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

void		cStoRequestCmd::fileAvailParse
(
    char	*mediaRequestName,	// (out) The media request name.
    char	*fileId,		// (out) The file id.
    int		&fileSize,		// (out) The size of the file.
    char	*fileName,		// (out) Name of the file.
    char	&compFlag,		// (out) The compression to use.
    cStoStatus	&status			// (mod) Function return status.
)
{
    cDhsAttrib	attribute;		// attribute object.
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    int		ndims;			// The number of dimensions of an 
    					// attribute.
    void	*pData;			// Pointer to attribute data.
    DHS_DATA_TYPE			// The type of data from an attribute.
    		type;

    char* dummy;                        // dummy arg to attribute.info() (XXX allan)
    
    //
    // Find the segregation directory.
    //

    checkDhs( attribute = find( STO_CA_SEGREGATION_DIR, dhsStatus ), 
	    dhsStatus, status, return );
    checkDhs( attribute.info( dummy, type, ndims, NULL, 
	    (void **) &pData, dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	strncpy( mediaRequestName, (char *) pData, 
		AM_MEDIA_REQUEST_NAME_LEN );
	mediaRequestName[ AM_MEDIA_REQUEST_NAME_LEN - 1 ] = '\0';
    }
    else
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_SEGREGATION_DIR );
	return;
    }


    //
    // Find the file id.
    //

    checkDhs( attribute = find( STO_CA_FILE_ID, dhsStatus ), 
	    dhsStatus, status, return );
    checkDhs( attribute.info( dummy, type, ndims, NULL, 
	    (void **) &pData, dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	strncpy( fileId, (char *) pData, AM_FILE_ID_LEN );
	fileId[ AM_FILE_ID_LEN - 1 ] = '\0';
    }
    else 
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_FILE_ID );
	return;
    }


    //
    // Find the file size.
    //

    checkDhs( attribute = find( STO_CA_FILESIZE, dhsStatus ), 
	    dhsStatus, status, return );
    checkDhs( attribute.info( dummy, type, ndims, NULL, 
	    (void **) &pData, dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_INT32 && ndims == 0 )
    {
	fileSize = *(int *) pData;
    }
    else
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_FILESIZE );
	return;
    }


    //
    // Look for the optional File name.
    //

    attribute = find( STO_CA_FILEPATH, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB )
    {
	fileName[0] = '\0';
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	checkDhs( attribute.info( dummy, type, ndims, NULL, 
		(void **) &pData, dhsStatus ), dhsStatus, status, return );
	if ( type == DHS_DT_STRING && ndims == 0 )
	{
	    strncpy( fileName, (char *) pData, AM_FILE_NAME_LEN );
	    fileName[ AM_FILE_NAME_LEN - 1 ] = '\0';
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_FILEPATH );
	    return;
	}
    }
    else
    {
	status.E_DHS( status, 
		"attribute = find( STO_CA_FILEPATH, dhsStatus )" );
    }


    //
    // Look for the optional compression flag..
    //

    attribute = find( STO_CA_COMP_FLAG, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB )
    {
	compFlag = AMF_COMP_GZIP;
	dhsStatus = DHS_S_SUCCESS;
    }
    else if ( dhsStatus == DHS_S_SUCCESS )
    {
	checkDhs( attribute.info( dummy, type, ndims, NULL, 
		(void **) &pData, dhsStatus ), dhsStatus, status, return );
	if ( type == DHS_DT_STRING && ndims == 0 )
	{
	    if ( streq( (char *) pData, "NONE" ) || 
		    streq( (char *) pData, "FALSE" ) )
	    {
		compFlag = AMF_COMP_NONE;
	    }
	    else if ( streq( (char *) pData, "GZIP" ) || 
		    streq( (char *) pData, "TRUE" ) )
	    {
		compFlag = AMF_COMP_GZIP;
	    }
	    else if ( streq( (char *) pData, "ASIS" ))
	    {
		compFlag = AMF_COMP_ASIS;
	    }
	    else
	    {
		status.E_CMD_BAD_ATTR_VALUE( status, name(), 
			STO_CA_COMP_FLAG, (char *) pData );
		return;
	    }
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_COMP_FLAG );
	    return;
	}
    }
    else
    {
	status.E_DHS( status, 
		"attribute = find( STO_CA_COMP_FLAG, dhsStatus )" );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::newUserRequest
//
// INVOCATION:
// AM_MEDIA	amMedia;
// cStoStatus	status;
// req.newUserRequest( amMedia, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) amMedia (AM_MEDIA &) Media structure to fill in.
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Creates a new user media request
//
// DESCRIPTION:
// Use the amMediaIdGet function to get the media request id.
// Use the amPut function to create a new user request.
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

void		cStoRequestCmd::newUserRequest
(
    AM_MEDIA	&amMedia,	// (mod) The media request structure.
    cStoStatus	&status		// (mod) Function return status.
)
{
    int		amStatus;	// AM library function return status.
    DBPROCESS	*dbProcess;	// Database connection.
    AM_MEDIA_OPTIONS		// Options for new request.
    		mediaOptions;
    std::map< std::string, std::string, std::less< std::string > >::iterator
    		i;


    //
    // Get new media ID, create new database row for media request. 
    //
    
    dbProcess = cStoDbManager::acquire();
    amStatus = amMediaIdGet( &(amMedia.ammMediaRequestId), dbProcess );
    if ( amStatus == AM_SUCCESS )
    {
	amStatus = amPut( &amMedia, dbProcess );
	for ( i = srcOptionList.begin(); 
		amStatus == AM_SUCCESS && i != srcOptionList.end(); i++ )
	{
	    mediaOptions.amoMediaRequestId = amMedia.ammMediaRequestId;
	    strncpy( mediaOptions.amoProcessName, (*i).first.data(), 
		    AM_PROC_NAME_LEN );
	    mediaOptions.amoProcessName[ AM_PROC_NAME_LEN - 1 ] = '\0';
	    strncpy( mediaOptions.amoOptions, (*i).second.data(), 
		    AM_MEDIA_OPTION_LEN );
	    mediaOptions.amoOptions[ AM_MEDIA_OPTION_LEN - 1 ] = '\0';

	    amStatus = amMediaOptionsPut( dbProcess, &mediaOptions );
	}
    }
    cStoDbManager::release();
    checkAm( amStatus, status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::userAppOptions
//
// INVOCATION:
// N.A. Called by the config_get function in the gen library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of 
//           tokens is correctly parseable. FALSE otherwise. 
//
// PURPOSE:
// To parse configuration file lines with keyword "userAppOptions" from the
// configuration file.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// cStoRequestCmd::config
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean	cStoRequestCmd::userAppOptions
(
    int		numTokens,	// (in)  Number of tokens from the config file.
    char	**tokens	// (in)  The array of tokens.
)
{
    boolean	ret;		// Function return value.
    cStoStatus	status;

    ret = TRUE;
    if ( numTokens == 3 )
    {
	if ( srcOptionList.find( tokens[1] ) != srcOptionList.end() )
	{
	    status.E_USER_APP_OPTIONS( status, tokens[1] );
	    status.E_CONFIG_KWD( status, tokens[0] );
	    ret = FALSE;
	}
	srcOptionList[tokens[1]] = tokens[2];
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    
    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::userReqParseFileInfo
//
// INVOCATION:
// char 	**pFileId, **pFileSize, **pCompFlag, 
//		**pFilePath;
// cStoStatus	status;
// req.userReqParseFileInfo( pFileId, pFileSize, pCompFlag, pFilePath, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) pFileId (char **&) Returns the array of file ids.
// (!) pFileSize (char **&) Returns the array of file sizes.
// (!) pCompFlag (char **&) Returns the array of compression flags.
// (!) pFilePath (char **&) Returns the array of file paths.
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Parses the file information from a stoUserMediaRequest command
//
// DESCRIPTION:
// Find the attributes, and extract the values for the file ids, files sizes,
// compression flags, and file paths int turn.
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

void		cStoRequestCmd::userReqParseFileInfo
(
    char	**&pFileId,	// (mod) Returns the array of file ids.
    char	**&pFileSize,	// (mod) Returns the array of file sizes.
    char	**&pCompFlag,	// (mod) Returns the array of compression flags.
    char	**&pFilePath,	// (mod) Returns the array of file paths.
    int		&arraySize,	// (mod) Returns the size of the arrays.
    cStoStatus	&status		// (mod) Function return status.
)
{
    cDhsAttrib	attribute;	// An attribute from the command.
    DHS_STATUS	dhsStatus(DHS_S_SUCCESS);	
				// DHS library return status.
    unsigned long
    		dims[7];	// The array dimensions.
    int		i;
    int		nDims;		// The number of array dimensions.
    DHS_DATA_TYPE		// The data type.
    		type;

    char* dummy;                        // dummy arg to attribute.info() (XXX allan)

    checkStat( status, return );


    //
    // Parse the file id's, sizes, compression flags, and file paths.
    // They are all in their own arrays. Ensure the sizes of these 
    // arrays are equivalent. 
    //
    
    attribute = find( STO_CA_FILE_ID_ARRAY, dhsStatus );
    checkDhs( attribute.info( dummy, type, nDims, dims, 
	    (void **) &pFileId, dhsStatus ), dhsStatus, status, return );
    arraySize = dims[0];
    if ( nDims != 1 )
    {
	status.E_ARRAY_SIZES( status, name() );
    }
    if ( type != DHS_DT_STRING )
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_FILE_ID_ARRAY );
    }

    attribute = find( STO_CA_FILE_SIZE_ARRAY, dhsStatus );
    checkDhs( attribute.info( dummy, type, nDims, dims, 
	    (void **) &pFileSize, dhsStatus ), dhsStatus, status, return );
    if ( nDims != 1 || arraySize != dims[0] )
    {
	status.E_ARRAY_SIZES( status, name() );
    }
    if ( type != DHS_DT_STRING )
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_FILE_SIZE_ARRAY );
    }

    attribute = find( STO_CA_COMP_FLAG_ARRAY, dhsStatus );
    checkDhs( attribute.info( dummy, type, nDims, dims, 
	    (void **) &pCompFlag, dhsStatus ), dhsStatus, status, return );
    if ( nDims != 1 || arraySize != dims[0] )
    {
	status.E_ARRAY_SIZES( status, name() );
    }
    if ( type != DHS_DT_STRING )
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_COMP_FLAG_ARRAY );
    }

    attribute = find( STO_CA_FILE_PATH_ARRAY, dhsStatus );
    checkDhs( attribute.info( dummy, type, nDims, dims, 
	    (void **) &pFilePath, dhsStatus ), dhsStatus, status, return );
    if ( nDims != 1 || arraySize != dims[0] )
    {
	status.E_ARRAY_SIZES( status, name() );
    }
    if ( type != DHS_DT_STRING )
    {
	status.E_PARAM_TYPE( status, name(), STO_CA_FILE_PATH_ARRAY );
    }
    if ( ! status.ok() )
    {
	return;
    }

    for ( i = 0; i < arraySize; i++ )
    {
	if ( ! validPath( pFilePath[i] ) )
	{
	    status.E_FILE_PATH( status, pFilePath[i] );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::userReqParseReqInfo
//
// INVOCATION:
// char		name[AM_MEDIA_REQUEST_NAME_LEN];
// char 	client[50];
// char 	root[AM_UNIT_NAME_ROOT_LEN];
// int		copies;
// char 	mediaType[AM_MEDIA_TYPE_LEN];
// bool		reuse;
// cStoStatus	status;
// cmd.userReqParseReqInfo( name, client, root, copies, mediaType, reuse, 
//		status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) name (char *) The media request name.
// (!) client (char *) The notify client name.
// (!) root (char *) The unit name root.
// (!) copies (int &) The number of copies to make.
// (!) mediaType (char *) The media type.
// (!) reuse (bool &) Attempt to reuse an old request?
// (!) status (cStoStatus &) function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Parses the request information from a stoUserMediaRequest command
//
// DESCRIPTION:
// Trivial.
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

void		cStoRequestCmd::userReqParseReqInfo
(
    char	*name,		// (mod) The media request name.
    char	*client,	// (mod) The notify client.
    char	*root,		// (mod) The unit name root.
    int		&copies,	// (mod) The number of copies to make.
    char	*mediaType,	// (mod) The type of media.
    bool	&reuse,		// (mod) Attempt to reuse an existing request?
    cStoStatus	&status		// (mod) Function return status.
)
{
    cDhsAttrib	attribute;	// An attribute from the command.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );	
				// Dhs library return status.
    boolean	flag;		// Generic boolean value from an attribute.
    void	*pData;		// Data in the argument list.
    DHS_DATA_TYPE		// The type of data returned.
		type;
    int		ndims;		// The dimensions of the value.

    char* dummy;                // dummy arg to attribute.info() (XXX allan)

    checkStat( status, return );


    //
    // Find the request name value.
    //

    attribute = find( STO_CA_REQUEST_NAME, dhsStatus );
    checkDhs( attribute.info( dummy, type, ndims, NULL, &pData, 
	    dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	strncpy( name, (char *) pData, AM_MEDIA_REQUEST_NAME_LEN );
	name[AM_MEDIA_REQUEST_NAME_LEN-1] = '\0';
    }
    else
    {
	status.E_PARAM_TYPE( status, this->name(), STO_CA_REQUEST_NAME );
    }


    //
    // Get the notify client name.
    //

    attribute = find( STO_CA_NOTIFY_CLIENT, dhsStatus );
    checkDhs( attribute.info( dummy, type, ndims, NULL, &pData, 
	    dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	strncpy( client, (char *) pData, 50 );
	client[49] = '\0';
    }
    else
    {
	status.E_PARAM_TYPE( status, this->name(), STO_CA_NOTIFY_CLIENT );
    }


    //
    // Get the unit name root.
    //

    attribute = find( STO_CA_UNIT_NAME_ROOT, dhsStatus );
    checkDhs( attribute.info( dummy, type, ndims, NULL, &pData, 
	    dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	strncpy( root, (char *) pData, AM_UNIT_NAME_ROOT_LEN );
	root[AM_UNIT_NAME_ROOT_LEN-1] = '\0';
    }
    else
    {
	status.E_PARAM_TYPE( status, this->name(), STO_CA_UNIT_NAME_ROOT );
    }


    //
    // Get the number of copies.
    //

    attribute = find( STO_CA_NUM_COPIES, dhsStatus );
    checkDhs( attribute.info( dummy, type, ndims, NULL, &pData, 
	    dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	copies = atoi( (char *) pData );
    }
    else
    {
	status.E_PARAM_TYPE( status, this->name(), STO_CA_NUM_COPIES );
    }


    //
    // Get the media type.
    //

    attribute = find( STO_CA_MEDIA_TYPE, dhsStatus );
    checkDhs( attribute.info( dummy, type, ndims, NULL, &pData, 
	    dhsStatus ), dhsStatus, status, return );
    if ( type == DHS_DT_STRING && ndims == 0 )
    {
	strncpy( mediaType, (char *) pData, AM_MEDIA_TYPE_LEN );
	mediaType[AM_MEDIA_TYPE_LEN-1] = '\0';
    }
    else
    {
	status.E_PARAM_TYPE( status, this->name(), STO_CA_MEDIA_TYPE );
    }


    //
    // If present, get the reuse flag.
    //

    reuse = false;
    attribute = find( STO_CA_REUSE, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB )
    {
	reuse = false;
	dhsStatus = DHS_S_SUCCESS;
    }
    else
    {
	checkDhs( attribute.info( dummy, type, ndims, NULL, &pData, 
		dhsStatus ), dhsStatus, status, return );
	if ( type == DHS_DT_STRING && ndims == 0 )
	{
	    if ( str2bool( (char *) pData, &flag ) )
	    {
		reuse = flag;
	    }
	}
	else
	{
	    status.E_PARAM_TYPE( status, this->name(), STO_CA_REUSE );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRequestCmd::validPath
//
// INVOCATION:
// if ( validPath( path ) )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) path (const char *) The path to verify.
//
// FUNCTION VALUE:
// (bool)  True if the path is valid, false otherwise.
//
// PURPOSE:
// Verify that a path is valid.
//
// DESCRIPTION:
// Invalid paths contain the following patterns: "//", "^../", "/..$",
// "^..$", "/../", "^./", "/.$", "^.$", "/./", "^/", "/$"
// where ^ is the beginning of the path, and $ is the end of the path.
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

bool		cStoRequestCmd::validPath
(
    const char	*path		// (in)  Path to verify.
)
{
    int		len;


    len = strlen( path );
    if ( strstr( path, "//" ) != NULL || strncmp( path, "../", 3 ) == 0 || 
	    streq( path, ".." ) || strstr( path, "/../" ) != NULL || 
	    strncmp( path, "./", 2 ) == 0 || streq( path, "." ) || 
	    strstr( path, "/./" ) != NULL || path[0] == '/' || path[0] == '\0' )
    {
	return( false );
    } 

    if ( ( len >= 3 && streq( path + len - 3, "/.." ) ) ||
	    ( len >= 2 && streq( path + len - 2, "/." ) ) ||
	    ( len >= 1 && streq( path + len - 1, "/" ) ) )
    {
	return( false );
    }

    return( true );
}
