static char rcsid[] = "$Id: cmd.C,v 1.1.1.1 2002-11-24 20:26:38 brighton Exp $";
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
// dhsData/src/cmd.C
//
// PURPOSE:
// Handles some control requests sent to the server.  Delete requests,
// storage server or oldp startup, invalid commands, storage server
// archive notifications, 
//
// METHOD NAME(S)
//  cDtsDhsArchive::~cDtsDhsArchive() : Deconstructor
//  cDtsDhsArchive::exec()	      : Execution point for an archive cmd.
//  cDtsDhsArchive::getFileIds()      : Get fileIds to send in archive cmd.
/// cDtsDhsCmdRequest::exec()	      : Execution pt. for a command request.
/// cDtsDhsDsInfo::exec()	      : Execution pt. for a dsInfo cmd request.
//  cDtsDhsDelete::exec()	      : Execution pt. for a delete ds request.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.10  2000/07/27 20:08:39  dunn
// Added standalone mode features.
//
// Revision 1.9  1998/05/07 23:32:47  dunn
// Fixed S_WARN_INV_STATE message, and got rid of opotential locking prob.
//
// Revision 1.8  1998/05/04 22:57:38  dunn
// Added dataset ifno request and fixes due to status calass changes.
//
// Revision 1.7  1997/10/03 23:00:08  dunn
// Fixes to work with new stl lib.  Do not init attrib to null.
//
// Revision 1.6  1997/09/17 22:10:20  dunn
// Code cleanup, fixed some locks.
//
// Revision 1.5  1997/09/05 19:51:54  dunn
// Added the archive receipt from ss
//
// Revision 1.4  1997/08/29 22:59:23  dunn
// Fixed leaks.
//
// Revision 1.3  1997/08/19 22:27:42  dunn
// Added object locking.
//
// Revision 1.2  1997/07/29 22:00:25  dunn
// Added delete command
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

#include <iostream.h>

#include "globals.H"
extern "C"
{
#include <sys/stat.h>
#include "gen_types.h"
#include "gen_str.h"
}

#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "store.H"

#include "cmd.H"

#include "dhsGenConnect.H"

//
//  Local defines.
//

msgText( cDtsDhsDelete::cStatus, S_WARN_INV_DS, P_NULL,
	    "Warning, data server did not have information about dataset %s.\n" );
msgText( cDtsDhsDelete::cStatus, S_WARN_NULL_LABEL, P_NULL,
	    "Error, empty datasetname sent in delete command\n" );
msgText( cDtsDhsDelete::cStatus, S_WARN_PERM, P_NULL,
	    "Error, dataset %s is received & has a lifetime of perm., cannot delete.\n" );


msgText( cDtsDhsControl::rStatus,  E_DATASET_NAME, P_NULL,
    "Error, invalid datasetName `%s', cannot set %s." );
msgText( cDtsDhsControl::rStatus,  E_DATASET_RECEIVED, P_NULL,
    "Error, data already received for dataset `%s', cannot set %s." );
msgText( cDtsDhsControl::rStatus, E_INVALID_CTL,  P_NULL,
	    "Error, invalid control sent." );
msgText( cDtsDhsControl::rStatus, E_NO_LIST,  P_NULL,
	    "No attribute `%s' found in avList for dataset %s, %s not set." );
msgText( cDtsDhsControl::rStatus, E_NO_NAME,  P_NULL,
	    "Error occurred while generated unique name, none generated." );
msgText( cDtsDhsControl::rStatus, S_WARN_INV_DS, P_NULL,
	    "Warning, data server did not have information about dataset %s.\n" );
msgText( cDtsDhsControl::rStatus, E_RMV_RECEIVED,  P_NULL,
	    "Error, have already received all data for dataset %s, cannot %s." );

msgText( cDtsDhsArchive::aStatus, S_WARN_NULL_SEG, P_NULL,
	    "Error, empty segregation directory sent in archive notify command\n" );
msgText( cDtsDhsArchive::aStatus, S_WARN_INV_DS, P_NULL,
	    "Warning, invalid state for dataset %s, ignoring archive cmd.\n" );

//
//***********************************************************************
//+
// FUNCTION NAME:
// n.a. Deconstructor.
//
// INVOCATION:
// ~cDtsDhsArchive()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function frees memory
//
// DESCRIPTION:
// This function frees the datasetname.
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

	cDtsDhsArchive::~cDtsDhsArchive
(
    void
)
{
    if ( ddaDatasetName != NULL )
    {
	free( ddaDatasetName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsArchive::exec
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
// This function is called when storage server sends notification that
// files have been archived on the storage server and now can be 
// deleted on the dataserver
//
// DESCRIPTION:
// This function is called when storage server sends notification that
// files have been archived on the storage server.  It expects an
// attribute called DTS_SS_SEG_DIR and then cycles through attributes
// fileId0, fileId1, ...  For each file id it verifies the state of
// the dataset, and deletes it appropriately from the dataserver.
// This callback is not attached if running standalone.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The fileid sent back has to have been received, and ss notified set.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void cDtsDhsArchive::exec
(
    void
)
{
    char	*address   = NULL;// Ip address of the client.
    cDhsAttrib	attrib;		// An attribute in the list.
    char	*clientName = NULL;
    				// Name of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    cDtsStatus	status;		// Function return status.



    status.S_ARC_HANDLER( status, name() );
    ddaDatasetName = NULL;
    arcStatus.displayStop();


    //
    //  Send a response to the client so it can get on with its life.
    //

    if ( cDtsDhs::simulate() )
    {
	status.S_ARC_REQUEST( status, "N/A", "N/A", "N/A", "DHS_CS_DONE");
	response( DHS_CS_DONE, "done", dhsStatus );
	return;
    }
    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the command.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Assume its from the storage server, get the segregation dir
    //  sent in the avList, then cycle through the fileIds.
    //

    void		*segDir = NULL;		// Pointer to the seg dir.

    attrib = find( DTS_SS_SEG_DIR, dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	attrib.info( &segDir, dhsStatus );

    }
    if ( dhsStatus == DHS_S_NO_ATTRIB || attrib == 0 || 
	    ( strlen((char *)segDir) <= 0) )
    {
	segDir = (char *) strsav( " " );
	status.E_INVALID_COMMAND( status, 
		"Notify archive command with no segregation dir",
		sourceName(), sourceAddr() );
	arcStatus.S_WARN_NULL_SEG( arcStatus );
    }


    //
    //  If everything is fine then cycle through the fileIds.
    //

    getFileIds( status );


    //
    //  Log the event and send a completed response.
    //

    dhsStatus = DHS_S_SUCCESS;
    if ( !status.ok() )
    {
	status.S_ARC_REQUEST( status, sourceName(), sourceAddr(), 
		(char *) segDir, "DHS_CS_ERROR" );
	response( DHS_CS_ERROR, arcStatus.message(), dhsStatus );
    }
    else
    {
	status.S_ARC_REQUEST( status, sourceName(), sourceAddr(), 
		(char *) segDir, "DHS_CS_DONE");
	response( DHS_CS_DONE, arcStatus.message(), dhsStatus );
    }
    free(segDir);

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsArchive::getFileIds
//
// INVOCATION:
// getFileIds()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function cycles through all fileIds sent from the storage server
//
// DESCRIPTION:
// This function cycles through all fileIds sent from the storage server,
// starting at 0 and continuing until if finds no more in sequence.
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

void cDtsDhsArchive::getFileIds
(
    cDtsStatus	&status			//  (mod) Function return status.
)
{
    cDhsAttrib	attrib;		// An attribute in the list.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// Dhs status.
    char	fileId[20];	// Attribute fileId name.
    char	*datasetName;	// Dataset name returned by ss.
    cDtsDatasetList
    		*pListPtr;	// Pointer to item in list.


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  For each file Id.
    //

    for ( int i = 0; status.ok() ; i ++ )
    {
	//
	//  Compose the fileId name and get it.
	//

	sprintf( fileId, "%s%d", DTS_SS_FILE_ID, i );
	attrib = find( fileId, dhsStatus );

	if ( dhsStatus == DHS_S_SUCCESS )
	{
	    datasetName = NULL;
	    attrib.info( (void **)&datasetName, dhsStatus );

	    if ( datasetName == NULL )
	    {
		break;
	    }

	    //
	    //  Find that datasetname in the list or database.
	    //
	    
	    pListPtr =  cDtsDatasetList::datasetNameFind( status, 
		    datasetName );

	    if ( !status.ok() || status == status.S_NOT_IN_DS_LIST )
	    {
		//
		//  Could not find this dataset name in the list or db, 
		//  print warning and ignore it.
		//

		status.S_WARN_INV_STATE( status, "ss notify", datasetName );
		continue;
	    }


	    //
	    //  Lock the dataset object, check the state of the object and
	    //  if it passes, then delete it from the dataserver.
	    //

	    cDtsDatasetTable	*dsPtr = NULL;
	    pListPtr->datasetObjectLock();
	    dsPtr = pListPtr->getDs();

	    if ( dsPtr != NULL )
	    {
		if ( dsPtr->dstReceived != DTS_DS_ALL_RECEIVED )
		{
		    //
		    //  Have not received all data, print warning, and continue.
		    //

		    status.S_WARN_INV_STATE( status, "received", datasetName,
			    "archive" );
		    arcStatus.S_WARN_INV_DS( arcStatus, datasetName );
		    pListPtr->datasetObjectUnlock();
		    continue;
		}
		else if ( dsPtr->dstSsNotify != DTS_SS_NOTIFIED )
		{
		    //
		    //  SS was never notified, print warning, and continue.
		    //

		    status.S_WARN_INV_STATE( status, "ssNotify", datasetName, 
			    "archive" );
		    arcStatus.S_WARN_INV_DS( arcStatus, datasetName );
		    pListPtr->datasetObjectUnlock();
		    continue;
		}
	
	    }


	    //
	    //  Have received notification of archiving, can delete
	    //  the dataset, but first write to the dataset table.
	    //

	    cDtsDatasetTable::dsPut( status, datasetName, dsPtr );


	    //
	    //  Delete from ds list, md, file system, delete from process.
	    //

	    pListPtr->datasetObjectUnlock();
	    pListPtr->datasetListRemove( status, dsPtr->dstReceived,
		    dsPtr->dstFormat );
	    pListPtr->datasetObjectLock();

	    if ( !status.ok() )
	    {
		status.S_WARN_INV_STATE( status, " ", datasetName, 
			    "archive" );
		arcStatus.S_WARN_INV_DS( arcStatus, datasetName );
	    }
	    else
	    {
		status.S_ARC_REQ( status, datasetName );
	    }
    	    pListPtr->datasetObjectUnlock();
	    delete pListPtr;
	}
	else
	{
	    //
	    //  No more fileids in the sds structure, stop.
	    //

	    break;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsCmdRequest::exec
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
// This function is called for commands not handled, and notification
// from the storage or oldp server that it has started up
//
// DESCRIPTION:
// This function is called for commands not handled, and notification
// from the storage or oldp server that it has started up.
// Error out for standalone mode.
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

void cDtsDhsCmdRequest::exec
(
    void
)
{
    char	*address = NULL; // Ip address of the client.
    char	*clientName = NULL;
    				// Name of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    cDtsStatus	status;		// Function return status.


    //
    //  Send a response to the client so it can get on with its life.
    //  Set the response message to not display.
    //

    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the command.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Only do these commands if NOT running in standalone mode.
    //

    if ( !status.standAlone() && streq( name(), DTS_SS_START_CMD ) )
    {
	//
	//  Command is notification from the storage server that it has
	//  started, check the db for notifications to send to it.
	//

	status.S_CMD_RECEIVED( status, "Storage server startup", 
		sourceName(), sourceAddr() );
	cDtsDhs::ssServerNotify( status );
    }
    else if ( !status.standAlone() && streq( name(), DTS_OLDP_START_CMD ) )
    {
	//
	//  Command is notification from the oldp server that it has
	//  started, check the db for notifications to send to it.
	//

	status.S_CMD_RECEIVED( status, "Oldp server startup", 
		sourceName(), sourceAddr() );
	cDtsDhs::olServerNotify( status );
    }
    else
    {
	//
	// Else its either an invalid command, or in standalone mode
	// so all of these commands are invalid.
	//

	status.E_INVALID_COMMAND( status, name(), sourceName(), sourceAddr() );
	response( DHS_CS_ERROR, "Invalid command received", dhsStatus );
    }


    //
    //  Log the event and send a completed response.
    //
   
    dhsStatus = DHS_S_SUCCESS;
    if ( !status.ok() )
    {
	response( DHS_CS_ERROR, dhsStatus );
    }
    else
    {
	response( DHS_CS_DONE, dhsStatus );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsDsInfo::exec
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
// This function is called for commands requesting information about
// a dataset
//
// DESCRIPTION:
// This function is called for commands requesting information about
// a dataset.
// Error out for standalone mode.
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

void cDtsDhsDsInfo::exec
(
    void
)
{
    char	*address = NULL;	 // Ip address of the client.
    char	*clientName = NULL;	// Name of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// Dhs status.
    cDhsAvList	responseAvList;		// Attrib value list for the response.
    cDtsStatus	status;			// Function return status.


    //
    //  Send a response to the client so it can get on with its life.
    //  Set the response message to not display.
    //

    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the command.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );

    if ( status.standAlone() )
    {
	//
	// In standalone mode this command is invalid, return an error.
	//

	status.E_INVALID_COMMAND( status, name(), sourceName(), sourceAddr() );
	response( DHS_CS_ERROR, "In standalone mode, this command is not valid",
	    dhsStatus );
    }
    else if ( streq( name(), DTS_CMD_DS_INFO ) )
    {
	//
	//  Command is notification from the storage server that it has
	//  started, check the db for notifications to send to it.
	//

	cDtsDbManager::getFileList( status, &responseAvList );
    }


    //
    //  Log the event and send a completed response.
    //
   
    dhsStatus = DHS_S_SUCCESS;
    if ( !status.ok() )
    {
	status.S_CTL_REQUEST( status, "dataset info request", sourceName(), 
		sourceAddr(), "N/A", "DHS_CS_ERROR" );
	response( DHS_CS_ERROR, dhsStatus );
    }
    else
    {
	status.S_CTL_REQUEST( status, "dataset info request", sourceName(), 
		sourceAddr(), "N/A", "DHS_CS_DONE" );
	response( DHS_CS_DONE, "done", responseAvList, dhsStatus );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsDelete::exec
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
// This function is called for deleting dataset commands.
//
// DESCRIPTION:
// This function is called for deleting dataset commands.  It
// expects an attribute DTS_CTL_LABEL, and if the dataserver has
// information about it and the lifetime is temporary then it
// will delete the dataset.
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

void cDtsDhsDelete::exec
(
    void
)
{
    char	*address = NULL;// Ip address of the client.
    cDhsAttrib	attrib;				// An attribute in the list.
    char	*clientName = NULL;
    				// Name of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    cDtsDatasetList
    		*pListPtr;	// Pointer to item in list.
    cDtsStatus	status;		// Status.


    status.S_DEL_HANDLER( status, name() );
    dddDatasetName = NULL;
    cmdStatus.displayStop();


    //
    //  Send a response to the client so it can get on with its life.
    //

    if ( cDtsDhs::simulate() )
    {
	status.S_DEL_REQ( status, "N/A", "N/A", "N/A", "DHS_CS_DONE" );
	response( DHS_CS_DONE, "done", dhsStatus );
	return;
    }
    response( DHS_CS_BUSY, "working", dhsStatus );


    //
    //  Get information about who sent the command.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Verify that the avList has the datasetName in it.
    //

    attrib = find( DTS_CTL_LABEL, dhsStatus );
    if ( dhsStatus == DHS_S_SUCCESS )
    {
	void		*dsLabel;		// Pointer to the label.
	attrib.info( &dsLabel, dhsStatus );
	dddDatasetName = strsav( (char *) dsLabel );
    }
    if ( attrib == 0 || ( strlen(dddDatasetName) <= 0) )
    {
	
	status.E_INVALID_COMMAND( status, "Delete request with no datasetname",
		sourceName(), sourceAddr() );
	cmdStatus.S_WARN_NULL_LABEL( cmdStatus );
    }


    //
    //  Find the dataset in the list or database.
    //

    pListPtr =  cDtsDatasetList::datasetNameFind( status, dddDatasetName );

    if ( status.ok() && status != status.S_NOT_IN_DS_LIST )
    {
	cDtsDatasetTable	*dsPtr = NULL;
	pListPtr->datasetObjectLock();
	dsPtr = pListPtr->getDs();

	if ( dsPtr != NULL && dsPtr->dstReceived != DTS_DS_NOT_RECEIVED )
	{
	    //
	    //  Have received data already, check that the lifetime is temp.
	    //
	
	    if ( dsPtr->dstLifetime == DTS_TEMP )
	    {
		//
		//  Delete from ds list, md, file system, delete from process.
		//

		pListPtr->datasetObjectUnlock();
		pListPtr->datasetListRemove( status, dsPtr->dstReceived,
			dsPtr->dstFormat );
		pListPtr->datasetObjectLock();
	    }
	    else
	    {
		//
		//  Can't delete permanent data.
		//

		status.E_DEL_PERM( status, dddDatasetName );
		cmdStatus.S_WARN_PERM( cmdStatus, dddDatasetName );
	    }
	}
	else
	{
	    //
	    //  ELSE Have never received data, delete it from
	    //  list and  process table.
	    //
	    pListPtr->datasetObjectUnlock();
	    pListPtr->datasetListRemove( status, DTS_DS_NOT_RECEIVED,
		    DTS_FM_UNKNOWN );
	    pListPtr->datasetObjectLock();
	    status.S_DEL_NODATA( status, dddDatasetName );
	}

	pListPtr->datasetObjectUnlock();

	if ( status.ok() )
	{
	    delete pListPtr;
	}
    }
    else if ( status == status.S_NOT_IN_DS_LIST )
    {
	//
	//  Never new about this dataset, given a warning returned.
	//

	cmdStatus.S_WARN_INV_DS( cmdStatus, dddDatasetName );
	status.S_WARN_INV_DS( status, dddDatasetName, "delete" );
    }


    //
    //  Log the event and send a completed response.
    //
   
    dhsStatus = DHS_S_SUCCESS;
    if ( !status.ok() )
    {
	status.S_DEL_REQ( status, sourceName(), sourceAddr(), dddDatasetName,
		"DHS_CS_ERROR" );
	response( DHS_CS_ERROR, cmdStatus.message(), dhsStatus );
    }
    else
    {
	status.S_DEL_REQ( status, sourceName(), sourceAddr(), dddDatasetName,
		"DHS_CS_DONE");
	response( DHS_CS_DONE, cmdStatus.message(), dhsStatus );
    }

    if ( dddDatasetName != NULL )
    {
	free( dddDatasetName );
    }
}
