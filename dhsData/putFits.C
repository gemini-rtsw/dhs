static char rcsid[] = "$Id: putFits.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/putFits.C
//
// PURPOSE:
// Handles functions required in bulk data put fits requests.
//
// METHOD NAME(S)
// cDtsPutFits::exec() : Execution point of PUT a FITS file.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:26  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.12  2000/08/10 22:11:27  dunn
//  Added check in for bad data type.
//
// Revision 1.11  2000/07/27 20:10:28  dunn
// Added standalone features.
//
// Revision 1.10  1998/05/04 23:01:48  dunn
// Changed to return avList with datasetname and status changes.
//
// Revision 1.9  1997/10/17 20:46:18  dunn
//  Changed logic to return an error if there were any warnings.
//
// Revision 1.8  1997/10/01 16:24:03  dunn
// Got rid of fileProcess and used parent call.
//
// Revision 1.7  1997/09/17 22:15:03  dunn
// Code cleanup.
//
// Revision 1.6  1997/09/05 19:52:45  dunn
// Small fix.
//
// Revision 1.5  1997/08/29 23:04:08  dunn
// Added the source name and addr stuff.
//
// Revision 1.4  1997/08/19 22:31:24  dunn
// Various fixes.
//
// Revision 1.3  1997/07/29 22:03:27  dunn
// Various fixes.
//
// Revision 1.2  1997/07/23 19:42:34  dunn
// Have added commented out parseHeader stuff, and added updating of status
// before responding.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
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

#include "globals.H"
extern "C"
{
#include <sys/stat.h>
#include "gen_types.h"
#include "gen_str.h"
#include "gen_eptr.h"

#include "sf.h"
}

#include "fh.H"


#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "store.H"
#include "put.H"
#include "dhsGenConnect.H"

//
//  Local defines.
//


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutFits::exec
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
// This function is called by the put handler to start the processing of a
// put of fits data request
//
// DESCRIPTION:
// This function is called by the put handler to start the processing of a
// put of fits data request.
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

void cDtsPutFits::exec
(
    VOID
)
{
    char	*address = NULL;// Ip address of the client.
    bool	allReceived;	// All data received.
    bool	allSet;		// All data chunks sent.
    bool	badType;	// Bad type match.
    char	*clientName = NULL;
    				// Name of the client.
    char	*datasetName = NULL;
    				// Datasetname.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    cDtsDatasetTable
    		*dsPtr;		// Pointer to database info.
    char	*filePath;	// Path to where the file is.
    char	*fname;		// Filename.
    bool	newName;	// New name generated.
    bool	notContrib;	// Not a contributor.
    cDtsDatasetList	
    		*pDsList;	// Ptr to new object.
    cDtsStatus	status;		// Function return status.
    char	*uniqueName;	// Unique name in the datasetName.
    char	*volName;	// Volume name.


    status.S_PUT_HANDLER( status, type(), label() );
    putStatus.displayStop();


    //
    //  Send a response to the client so it can get on with its life.
    //

    if ( cDtsDhs::simulate() )
    {
	cdnResponseAvList.add( "datasetName", DHS_DT_STRING, label(), 
		dhsStatus );
	status.S_PUT_REQ( status, "N/A", "N/A", label(), "DHS_CS_DONE" );
	response( DHS_CS_DONE, "done", cdnResponseAvList, dhsStatus );
	return;
    }
    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the request.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Check that the label is not empty.
    //

    if ( strlen( label() ) == 0 )
    {
	//
	//  Create a new datasetname and unique name if its empty.
	//

	status.S_LABEL_INVALID( status, sourceName(), sourceAddr() );
	cDtsUniqueList::createUniqueName( status, &datasetName );
	putStatus.S_WARN_NULL_LABEL( putStatus, datasetName );
    }
    else
    {
	datasetName = strsav( label() );
    }


    //
    //  Determine if the segregation dir has been sent.
    //

    setSeg( status, &datasetName );



    //
    //  Verify the unique name, and datasetname.
    //

    cDtsUniqueList::uniqueNameVerify( status, datasetName, &uniqueName );
    pDsList = cDtsDatasetList::datasetNameVerify( status, &datasetName, 
	    uniqueName, sourceName(), type(), length(), allSet, allReceived, 
	    notContrib, badType, newName, TRUE );
    free( uniqueName );
    dsPtr = pDsList->getDs();
    if ( allReceived )
    {
	//
	//  All data has been received for this dataset name.
	//

	putStatus.S_DATA_RECEIVED( putStatus );
	status.S_DATA_RECEIVED( status, datasetName, sourceName() );
    }
    else if ( notContrib )
    {
	//
	//  This source is not a listed contributor to the dataset.
	//

	putStatus.S_NOT_CONTRIB ( putStatus );
	status.S_NOT_CONTRIB( status, sourceName(), label() );
    }
    else if ( badType )
    {
	//
	//  Data type set for this dataset does not match.
	//
	char	oldDataType;

	oldDataType = ( dsPtr != NULL )? dsPtr->dstFormat : '?';
	putStatus.S_BAD_TYPE( putStatus, oldDataType, type() );
	status.S_BAD_TYPE( status, datasetName, oldDataType, type() );
    }


    //
    //  Check the last flag, determine where to write the file and write it.
    //

    lastFlagCheck( status, datasetName );
    destinationExtract( status, &fname, &filePath, &volName, length(), dsPtr );
    fileProcess( status, filePath, length() );
    if ( dsPtr != NULL )
    {
	//
	// Update the space listed at the status server.
	//

	if ( dsPtr->dstLifetime == DTS_PERM )
	{
	    cDtsStoreManager::updateSpace( status, 
		    cDtsStoreManager::ST_PERM, length() );
	}
	else
	{
	    cDtsStoreManager::updateSpace( status, 
		    cDtsStoreManager::ST_TEMP, length() );
	}
    }


    //
    //  Write to the archive table to make this file retrievable.
    //  If running standalone update the list structure with the path
    //  that the file was written to.
    //
    
    cDtsDbManager::makeRetrievable( status, datasetName, fname, volName, 
	    length() );
    if ( status.standAlone() && pDsList != NULL )
    {
	pDsList->updateObject( status, filePath, type() );
    }

    free( filePath );



    //
    //  Send to Quick Look.
    //

    qlSend( status, datasetName, TRUE, pDsList );


    //
    //  Parse the fits header.
    //

    if ( status.ok() && status.parseOn() )
    {
	cHdrParser	fitsParse;
	cHdrParser::hStatus		hstatus;


	hstatus.displayStop();

	//
	//  Access the buffer, parse the header.
	//

	cDtsPutDs::lockSf();
	fitsParse.parseHeader( hstatus, buffer(), length(), datasetName );
	cDtsPutDs::unlockSf();
	if ( !hstatus.ok() )
	{
	    //
	    //  Parse  had errors, give a warning, set health to bad.
	    //

	    dsPtr->dstHealth = DTS_HL_SICK;
	    putStatus.S_HEADER_ERROR( putStatus, hstatus.message() );
	    status.S_HEADER_ERROR( status, datasetName, hstatus.message() );
	}
	else if ( hstatus != hstatus.S_SUCCESS )
	{
	    //
	    //  Warnings parsing header, send back.
	    //

	    putStatus.S_HEADER_WARN( putStatus, hstatus.message() );
	    status.S_HEADER_WARN( status, datasetName, hstatus.message() );
	}
   }


    //
    // Update status in the database.
    //

    cDtsDatasetList::updateStatus( status, datasetName, dsPtr );



    //
    //  Log the event and send a completed response.
    //
   
    dhsStatus = DHS_S_SUCCESS;
    char *p = (char *) putStatus.message();
    cdnResponseAvList.add( "datasetName", DHS_DT_STRING, datasetName,
	    dhsStatus );

    if ( !status.ok() || p[0] != '\0' )
    {
	if ( newName )
	{
	    //
	    //  A new datasetname was generated because either all the data was 
	    //  received or they were not a contributor.
	    //

	    putStatus.S_NEW_DS_NAME ( putStatus, label(), datasetName );
	    status.S_NEW_DS_NAME ( status, label(), datasetName );
	}
	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_ERROR" );
	response( DHS_CS_ERROR, putStatus.message(), cdnResponseAvList,
		dhsStatus );
    }
    else
    {
	if ( newName )
	{
	    //
	    //  A new datasetname was generated because either all the data was 
	    //  received or they were not a contributor.
	    //

	    putStatus.S_NEW_DS_NAME ( putStatus, label(), datasetName );
	    status.S_NEW_DS_NAME ( status, label(), datasetName );
	}
	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_DONE");
	response( DHS_CS_DONE, putStatus.message(), cdnResponseAvList,
		dhsStatus );
    }


    //
    //  Notify the servers.
    //

    serverNotify( status, dsPtr );
    pDsList->datasetObjectUnlock();
    free( datasetName );
}
