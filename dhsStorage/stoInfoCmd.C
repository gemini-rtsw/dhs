static char rcsid[] = "$Id: stoInfoCmd.C,v 1.1.1.1 2002-11-24 20:32:02 brighton Exp $";
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
// dhsStorage/src/stoInfoCmd.C
//
// PURPOSE:
// Contains methods for the cStoInfoCmd command
//
// METHODS
// cStoInfoCmd::exec	- Method called by dhs lib to process info commands.
// cStoInfoCmd::files	- Method to fill an a-v list with file information.
// cStoInfoCmd::units	- Method to fill an a-v list with unit information.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.9  2001/03/12 20:36:31  nhill
// Added type checking on command parameters.
//
// Revision 1.8  1999/06/16 23:58:32  nhill
// Major update.
//
// Revision 1.7  1998/06/24 16:03:38  cockayne
// Modified command AV list parsing to be more robust.
//
// Revision 1.6  1998/05/15 17:10:07  cockayne
// Modifications to memory and library error checking.
//
// Revision 1.5  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.4  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 21:02:13  cockayne
// Fixed bug in info command type attribute-value parsing.
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

#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <list>

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
#include "stoFiveState.H"
#include "stoActionType.H"
#include "stoDhs.H"
#include "stoDbm.H"
#include "stoMedia.H"
#include "stoInfoCmd.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoInfoCmd::exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called when an info command is received
//
// DESCRIPTION:
// Log receipt of command. Parse command's a-v list, and execute 
// either the files() or units() methods to fill a return a-v list.
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

void		cStoInfoCmd::exec
(
    void
)
{
    DHS_DATA_TYPE			// The data type of an attribute.
    		attribType;
    cDhsAttrib	attribute;		// attribute object.
    void	*pData;			// Pointer to attribute data.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    int		mediaRequestId;		// 
    int		nDims;			// The dimensions of an attribute.
    cDhsAvList	responseAvList;		// response a-v list. 
    cStoStatus	status;			// Program status. 
    					// Status for the dhs library. 
    char	type[20];		// Type of information requested. 

    char*       dummy;                  // dummy arg for attribute.info() (XXX allan: added)

    //
    // Respond to let client continue. Log receipt of command. 
    //

    checkDhs( response( DHS_CS_BUSY, STO_RSP_WORKING_MSG, dhsStatus ),
	    dhsStatus, status, return );
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
	    dhsStatus, status, return );
    status.S_CMD_RECEIVED( status, name(), id, ip );
    status.displayStop();


    //
    // Parse command - get info command type. 
    //

    do
    {
	attribute = find( STO_CA_INFO_CMD_TYPE, dhsStatus );
	attribute.info( dummy, attribType, nDims, NULL, 
		(void **) &pData, dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    break;
	}


	if ( attribType == DHS_DT_STRING && nDims == 0 )
	{
	    strncpy( type, (char *) pData, sizeof( type ) );
	    type[ sizeof( type ) - 1 ] = '\0';
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_INFO_CMD_TYPE );
	    break;
	}


	attribute = find( STO_CA_MEDIA_REQUEST_ID, dhsStatus );
	attribute.info( dummy, attribType, nDims, NULL, 
		(void **) &pData, dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    break;
	}

	if ( attribType == DHS_DT_STRING && nDims == 0 && 
		strne( "", (char *) pData ) )
	{
	    mediaRequestId = atoi( (char *) pData );
	}
	else
	{
	    status.E_PARAM_TYPE( status, name(), STO_CA_MEDIA_REQUEST_ID );
	    break;
	}
    }
    while ( false );


    if ( dhsStatus != DHS_S_SUCCESS || ! status.ok() )
    {
	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_FORMAT( status, name() );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );
	status.display();
	return;
    }


    //
    // Execute query, compose return a-v list. 
    //

    if ( streq( type, STO_CAV_RETRIEVABLE ) ||
	    streq( type, STO_CAV_QUEUEABLE ) )
    {
	files( type, responseAvList, mediaRequestId, status );
    }
    else if ( streq( type, STO_CAV_PREPARABLE ) ||
	    streq( type, STO_CAV_WRITEABLE )  ||
	    streq( type, STO_CAV_NOTIFYABLE ) ||
	    streq( type, STO_CAV_CLEANUPABLE ) )
    {
	units( type, responseAvList, mediaRequestId, status );
    }
    else
    {
	dhsStatus = DHS_S_SUCCESS;
	status.E_CMD_FORMAT( status, name() );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ),
		dhsStatus, status, return );
	status.display();
	return;
    }


    //
    // If there was an error in building the return a-v list, response has
    // already been sent, so just exit this routine. 
    //

    if ( !status.ok() )
    {
	return;
    }


    //
    // Respond to client with DONE and the av-list. 
    //

    checkDhs( response( DHS_CS_DONE, responseAvList, dhsStatus ), 
	    dhsStatus, status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoInfoCmd::files
//
// INVOCATION:
// files( type, responseAvList, mediaRequestId, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type	(char *)	- Type of query to issue to the database.
// (<) responseAvList (cDhsAvList &) - Response a-v list to build.
// (>) mediaRequestId (int)	- Media Request ID.
// (!) status	(cStoStatus &)	- Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Compose a-v list of files to return to command sender.
//
// DESCRIPTION:
// Determine query select type, query database for list of files. 
// Compose return a-v list. 
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

void		cStoInfoCmd::files
(
    char	*type,
    cDhsAvList	&responseAvList,	// Response a-v list to build.
    int		mediaRequestId,		// Media Request ID. 
    cStoStatus  &status			// Program status.
)
{
    AM_FILE	amFile;			// am library file structure.
    AM_FILE	*pAmFile;		// Ptr to am library file structure.
    int		amStatus;		// am library function return status.
    DBPROCESS 	*dbProcess;		// Database process. 
    char	responseString[256];	// String to format response into.
    int		count;			// File counter;
    list < AM_FILE >
		fileList;		// List of files.
    list < AM_FILE >::iterator
		fileIter;		// File list iterator
    char	*fileInfoBuffer;	// Return file information buffer. 
    char	fileInfoHeader[90];	// Return file information buffer. 
    char	*fileInfoPtr;		// Ptr into file info buffer. 
    char	amFileStatus[29];	//
    char	*fileTableName;		//
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    AM_FILE_GET	amSelectType;


    //
    // Assemble list of files from the database. 
    //

    if ( streq( type, STO_CAV_RETRIEVABLE ) )
    {
	amSelectType = AM_FG_RETRIEVABLE;
    }
    else // streq( type, STO_CAV_QUEUEABLE )
    {
	amSelectType = AM_FG_QUEUEABLE;
    }

    dbProcess = cStoDbManager::acquire();
    for ( amStatus = amFileGet( amSelectType, &amFile, dbProcess, 
	    mediaRequestId );
	    status.ok() && amStatus == AM_SUCCESS;
	    amStatus = amFileGet( AM_FG_NEXT, &amFile, dbProcess ) )
    {
	fileList.push_back( amFile );
    }
    cStoDbManager::release();
    if ( amStatus != AM_NO_FILE )
    {
	(void) sprintf( responseString, "%s", STO_RSP_DB_ERR );
	checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		dhsStatus, status, VOID );
	checkAm( amStatus, status, VOID );
    }


    //
    // Get table name for files table. 
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amDbInfoGet( NULL, NULL, NULL, NULL, &fileTableName, NULL ),
	    status, VOID );
    cStoDbManager::release();


    //
    // Allocate space for return buffer.
    //

    checkNull( ( fileInfoBuffer = (char *) gen_alloc( 
	    ( fileList.size() * 90 * sizeof(char *) )  + 1) ),
	    status, VOID );


    //
    // Prepare buffer to return in a-v list. 
    //

    fileInfoPtr = fileInfoBuffer;
    for ( count = 0, fileIter = fileList.begin();
	    fileIter != fileList.end();
	    fileIter++, count++ )
    {
	pAmFile = &(*fileIter);
	dbProcess = cStoDbManager::acquire();
	checkAm( amStatusStr( dbProcess, &amFileStatus[0], fileTableName, 
		pAmFile->amfStatus ), status, VOID );
	cStoDbManager::release();

	sprintf( fileInfoPtr, "%40s%20d%29s\n", 
		pAmFile->amfFileId, pAmFile->amfFileSize, amFileStatus );
	fileInfoPtr += 90;
    }

    sprintf( fileInfoHeader, "%40s%20s%29s", 
	    "File Identifier", "File Size", "Status" );


    //
    // Add stuff to a-v list. 
    //

    checkDhs( responseAvList.add( STO_CA_DATA_HEADER, DHS_DT_STRING, 
	    fileInfoHeader, dhsStatus ), 
	    dhsStatus, status, VOID );

    checkDhs( responseAvList.add( STO_CA_DATA, DHS_DT_STRING, 
	    fileInfoBuffer, dhsStatus ), 
	    dhsStatus, status, VOID );

    

    //
    // Free buffer.
    //

    gen_free( fileInfoBuffer );


    if ( !status.ok() )
    {
	(void) sprintf( responseString, "%s", STO_RSP_INFO_ERR );
	checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		dhsStatus, status, return );
    }

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoInfoCmd::units
//
// INVOCATION:
// units( type, responseAvList, mediaRequestId, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type	(char *)	- Type of query to issue to the database.
// (<) responseAvList (cDhsAvList &) - Response a-v list to build.
// (>) mediaRequestId (int)	- Media Request ID.
// (!) status	(cStoStatus &)	- Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Compose a-v list of units to return to command sender.
//
// DESCRIPTION:
// Determine query select type, query database for list of units. 
// Compose return a-v list. 
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

void		cStoInfoCmd::units
(
    char	*type,
    cDhsAvList	&responseAvList,	// Response a-v list to build.
    int         mediaRequestId,         // Media Request ID.
    cStoStatus  &status			// Program status.
)
{
    AM_UNIT	amUnit;			// am library file structure.
    AM_UNIT	*pAmUnit;		// Ptr to am library file structure.
    int		amStatus;		// am library function return status.
    DBPROCESS 	*dbProcess;		// Database process. 
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
					// DHS Library status.
    char	responseString[256];	// String to format response into.
    int		count;			// File counter;
    list < AM_UNIT >
		unitList;		// List of files.
    list < AM_UNIT >::iterator
		unitIter;		// File list iterator
    char	unitInfoHeader[110];		// fileId array.
    char	*unitInfoBuffer;		// fileId array.
    char	*unitInfoPtr;		// fileId array.
    char	amStatusString[29];
    char	*unitTableName;
    char	amuStatus;


    //
    // Assemble list of units from the database. 
    //

    if ( streq( type, STO_CAV_PREPARABLE ) )
    {
	amuStatus = AMU_QUEUED;
    }
    else if ( streq( type, STO_CAV_WRITEABLE ) )
    {
	amuStatus = AMU_PREPARED;
    }
    else if ( streq( type, STO_CAV_CLEANUPABLE ) )
    {
	amuStatus = AMU_WRITTEN;
    }
    else if ( streq( type, STO_CAV_NOTIFYABLE ) )
    {
	amuStatus = AMU_CLEANED_UP;
    }

    dbProcess = cStoDbManager::acquire();
    for ( amStatus = amUnitGet( AM_UG_STATUS, &amUnit, dbProcess, 
	    amuStatus, NULL );
	    amStatus == AM_SUCCESS;
	    amStatus = amUnitGet( AM_UG_NEXT, &amUnit, dbProcess ) )
    {
	if ( amUnit.amuMediaRequestId != mediaRequestId )
	{
	    continue;
	}
	unitList.push_back( amUnit );
    }
    cStoDbManager::release();
    if ( amStatus != AM_NO_UNITS )
    {
	(void) sprintf( responseString, "%s", STO_RSP_DB_ERR );
	checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		dhsStatus, status, VOID );
	checkAm( amStatus, status, VOID );
    }


    //
    // Allocate space for return buffer.
    //

    checkNull( ( unitInfoBuffer = (char *) gen_alloc( 
	    ( unitList.size() * 110 * sizeof(char *) ) + 1 ) ),
	    status, VOID );


    //
    // Get table name for units table. 
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amDbInfoGet( NULL, NULL, NULL, &unitTableName, NULL, NULL ),
	    status, VOID );
    cStoDbManager::release();


    //
    // Prepare buffer to return in a-v list. 
    //

    unitInfoPtr = unitInfoBuffer;
    for ( count = 0, unitIter = unitList.begin();
	    unitIter != unitList.end();
	    unitIter++, count++ )
    {
	pAmUnit = &(*unitIter);
	dbProcess = cStoDbManager::acquire();
	checkAm( amStatusStr( dbProcess, &amStatusString[0], unitTableName,
		pAmUnit->amuStatus ), status, VOID );
	cStoDbManager::release();

	sprintf( unitInfoPtr, "%20d%20s%20d%29s%20d\n",
		pAmUnit->amuMediaRequestId, pAmUnit->amuMediaUnitName, 
		pAmUnit->amuTotalSize, amStatusString, 
		pAmUnit->amuCopiesWritten );

	unitInfoPtr += 110;
    }

    sprintf( unitInfoHeader, "%20s%20s%20s%29s%20s",
	    "Media Request", "Media Unit Name", "Total Size", 
	    "Status", "Copies Written" );


    //
    // Add stuff to a-v list. 
    //

    checkDhs( responseAvList.add( STO_CA_DATA_HEADER, DHS_DT_STRING, 
	    unitInfoHeader, dhsStatus ), 
	    dhsStatus, status, VOID );

    checkDhs( responseAvList.add( STO_CA_DATA, DHS_DT_STRING, 
	    unitInfoBuffer, dhsStatus ), 
	    dhsStatus, status, VOID );

    

    //
    // Free buffer.
    //

    gen_free( unitInfoBuffer );


    if ( !status.ok() )
    {
	(void) sprintf( responseString, "%s", STO_RSP_INFO_ERR );
	checkDhs( response( DHS_CS_ERROR, responseString, dhsStatus ),
		dhsStatus, status, return );
    }


    return;
}
