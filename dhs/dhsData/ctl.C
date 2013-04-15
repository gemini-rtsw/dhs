static char rcsid[] = "$Id: ctl.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/ctl.C
//
// PURPOSE:
// Handles all control command requests send to the server.
//
// METHOD NAME(S)
//  cDtsDhsControl::cDtsDhsControl() : Constructor.
//  cDtsDhsControl::exec()           : Execution point for a control request.
//  cDtsDhsControl::getLabel()       : Get the label from supplied avlist.
//  cDtsDhsControl::generateUniqueName() : Generate a unique name.
//  cDtsDhsControl::rmDataset()	     : Remove a dataset.
//  cDtsDhsControl::setContribList() : Set the contributors list for a ds.
//  cDtsDhsControl::setLifetime()    : Set the lifetime of a ds.
//  cDtsDhsControl::setQlStream()    : Set the quick look stream for a ds.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2013/03/19 02:56:52  pjy
// Added support for qlsDsAbort command to allow QL server to cleanup memory after abort. New abortQlStream method
//
// Revision 1.1.1.1  2002/11/24 20:26:44  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.11  2000/07/27 20:09:22  dunn
// Added standalone features.
//
// Revision 1.10  1998/05/07 23:33:11  dunn
// Got rid of potential locking prob.
//
// Revision 1.9  1998/05/04 22:58:56  dunn
// Made status class changes and message changes and number of incomplete
// databset changes.
//
// Revision 1.8  1997/10/03 23:01:02  dunn
// Changes to handle new stl lib.
//
// Revision 1.7  1997/09/17 22:12:19  dunn
// Fixed messages, code cleanup, returned label in avlist.
//
// Revision 1.6  1997/09/05 19:52:06  dunn
// Fixed some of the exit/reset
//
// Revision 1.5  1997/08/29 23:00:24  dunn
// Fixed leaks, added reset and abort dataset commands.
//
// Revision 1.4  1997/08/19 22:28:56  dunn
// Added ql streams, fixed contrib streams.
//
// Revision 1.3  1997/07/29 22:00:47  dunn
// Added contrib list settting.
//
// Revision 1.2  1997/07/24 16:25:04  dunn
// Fixed get label stuff
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
}

#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "store.H"
#include "con.H"

#include "cmd.H"

#include "dhsGenConnect.H"

//
//  Local defines.
//



//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::cDtsDhsControl
//
// INVOCATION:
// n.a.  This is the constructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To initilize variables
//
// DESCRIPTION:
// To initilize variables.
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

	 cDtsDhsControl::cDtsDhsControl
(
    void
)
{
    dcrLabel = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::exec
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
// This function is called to begin processing a dbCtl command
//
// DESCRIPTION:
// This function is called to begin processing a dbCtl command.
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

void cDtsDhsControl::exec
(
    void
)
{
    char	*address = NULL;// Ip address of the client.
    cDhsAttrib	attrib;		// An attribute in the list.
    char	*clientName = NULL;
    				// Name of the client.
    void	*control;	// Pointer to the data for an attrib.
    char	*ctrlName = NULL;
    				// Control request type.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    unsigned long
    		dims[7];
    int		icontrol;	// The control sent.
    void	*life;		// Lifetime.
    char	**nameList;	// Life of contribs or ql stream.
    int		nDims;		// Number of dimensions sent.
    cDtsStatus	status;		// Function return status.
    char	*uniqueName;	// Unique name generated.


    status.S_CTL_HANDLER( status, name() );


    //
    //  Send a response to the client so it can get on with its life.
    //

    if ( cDtsDhs::simulate() )
    {
	status.S_CTL_REQUEST( status, "N/A", "N/A", "N/A", "N/A",
		    "DHS_CS_DONE" );
	response( DHS_CS_DONE, "done", dhsStatus );
	return;
    }
    response( DHS_CS_BUSY, "working", dhsStatus );
    cmdStatus.displayStop();


    //
    //  Get information about who sent the command.
    //

    checkDhs( dhsConnectInfo( connect(), &address, &clientName, &dhsStatus ),
	    dhsStatus, status, VOID );
    sourceName( clientName );
    sourceAddr( address );


    //
    //  Find out which control has been issued.
    //

    attrib = find( DTS_CTL_COMMAND, dhsStatus );
    attrib.info( &control, dhsStatus );
    if ( dhsStatus == DHS_S_NO_ATTRIB )
    {
	//
	//  No control command send, error.
	//

	cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_COMMAND, "UNKNOWN", "UNKNOWN" );
	status.E_NO_LIST( status, sourceName(), sourceAddr(), DTS_CTL_COMMAND,
		"unknown", "unknown" );
    }
    else
    {
	//
	//  Get the dataset name.
	//

	icontrol = *( (int *) control );
	getLabel( status, (DHS_BD_CTL) icontrol );


	//
	//  Now, depending on the control requested, do different things.
	//

	if ( status.ok() )
	{

	    switch( (DHS_BD_CTL) icontrol ) 
	    {
		case DHS_BD_CTL_ABORT:
		    //
		    //  Abort dataset was sent.
		    //

		    ctrlName = strsav( "abort dataset" );
		    abortQlStream( status, returnLabel() );
		    rmDataset( status, returnLabel(), TRUE );
		    break;

		case DHS_BD_CTL_RESET:
		    //
		    //  Reset dataset was sent.
		    //

		    ctrlName = strsav( "reset dataset" );
		    rmDataset( status, returnLabel(), FALSE );
		    break;

		case DHS_BD_CTL_LIFETIME:
		    //
		    //  Set the lifetime of a dataset was sent.
		    //

		    int		itmp;

		    ctrlName = strsav( "set lifetime" );
		    attrib = find( DTS_CTL_LIFETIME, dhsStatus );
		    attrib.info( &life, dhsStatus );

		    if ( dhsStatus == DHS_S_SUCCESS )
		    {
			itmp = * (int *) life;
			if ( itmp  >= 0  )
			{
			    setLifetime( status, (DHS_BD_LIFETIME ) itmp,
				    returnLabel() );
			}
			else
			{
			    //
			    //  Invalid lifetime sent.
			    //  Should think about changing this message.
			    //

			    cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_LIFETIME, 
				    returnLabel(), ctrlName );
			    status.E_NO_LIST( status, sourceName(),sourceAddr(),
				    DTS_CTL_LIFETIME, ctrlName, returnLabel() );
			}
		    }
		    else
		    {
			//
			//  Lifetime attribute not found.
			//

			cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_LIFETIME, 
				returnLabel(), ctrlName );
			status.E_NO_LIST( status, sourceName(), sourceAddr(), 
				DTS_CTL_LIFETIME, ctrlName, returnLabel() );
		    }
		    break;

		case DHS_BD_CTL_CONTRIB:
		    //
		    //  Set the contributor list of a dataset was sent.
		    //

		    ctrlName = strsav( "set contributors" );
		    attrib = find( DTS_CTL_NAMES, dhsStatus );
		    attrib.info( nDims, dims, (void **) &nameList, dhsStatus );
		    if ( dhsStatus != DHS_S_NO_ATTRIB && nDims == 1 &&
			    dims[0] > 0 )
		    {
			if ( *nameList != NULL )
			{
			    setContribList( status, nameList, dims[0], 
				    returnLabel() );
			}
			else
			{
			    //
			    //  Null contributor list sent.
			    //

			    cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_NAMES, 
				    returnLabel(), ctrlName );
			    status.E_NO_LIST( status, sourceName(),sourceAddr(),
				    DTS_CTL_NAMES, ctrlName, returnLabel() );
			}
		    }
		    else
		    {
			//
			//  No contributor attribute sent.
			//

			cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_NAMES, 
				returnLabel(), ctrlName );
			status.E_NO_LIST( status, sourceName(), sourceAddr(), 
				DTS_CTL_NAMES, ctrlName, returnLabel() );
		    }
		    break;

		case DHS_BD_CTL_QLSTREAM:
		    //
		    //  Send the ql stream to ql server.
		    //

		    ctrlName = strsav( "set quick look stream" );
		    attrib = find( DTS_CTL_NAMES, dhsStatus );
		    attrib.info( nDims, dims, (void **) &nameList, dhsStatus );
		    if ( dhsStatus == DHS_S_SUCCESS && nDims == 1 &&
			    dims[0] > 0 )
		    {
			if ( *nameList != NULL )
			{
			    setQlStream( status, nameList, returnLabel(), 
				    dims );
			}

			//
			// Ignore if no stream was sent.
			//
		    }
		    else
		    {
			//
			//  No ql stream attribute sent.
			//

			cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_NAMES, 
				returnLabel(), ctrlName );
			status.E_NO_LIST( status, sourceName(), sourceAddr(), 
				DTS_CTL_NAMES, ctrlName, returnLabel() );
		    }

		    break;

		case DHS_BD_CTL_GETNAME:
		    //
		    //  A new unique name was requested.
		    //

		    ctrlName = strsav( "request uniqueName" );
		    generateUniqueName( status, &uniqueName );
		    break;

		default:
		    //
		    //  Unhandedled control request.
		    //

		    ctrlName = strsav( "invalid control" );
		    cmdStatus.E_INVALID_CTL( cmdStatus );
		    status.E_INVALID_COMMAND( status, (char *)control, 
			    sourceName(), sourceAddr() );
		    break;
	    }
	}
    }


    dhsStatus = DHS_S_SUCCESS;
    if ( status.ok() )
    {
	if ( (DHS_BD_CTL) icontrol == DHS_BD_CTL_GETNAME ) 
	{
	    //
	    // Indicate the command is complete.  Return the unique name
	    // in both the avList and string.
	    //

	    cDhsAvList	dataLabelName;
	    checkDhs( dataLabelName.add( DTS_CTL_DATA_LABEL, DHS_DT_STRING, 
		uniqueName, dhsStatus ), dhsStatus, 
		status, VOID );

	    status.S_CTL_REQUEST( status, ctrlName, sourceName(), sourceAddr(),
	    	uniqueName, "DHS_CS_DONE" );
	    response( DHS_CS_DONE, uniqueName, dataLabelName,  dhsStatus );
	}
	else
	{
	    //
	    // Indicate the command is complete.  
	    //

	    status.S_CTL_REQUEST( status, ctrlName, sourceName(), sourceAddr(),
		    returnLabel(), "DHS_CS_DONE" );
	    response( DHS_CS_DONE, cmdStatus.message(), dhsStatus );
	}
    }
    else
    {
	//
	// Indicate an error occured.
	//

	if ( ctrlName == NULL )
	{
	    ctrlName = strsav( "unknown control" );
	}
	if ( returnLabel() == NULL )
	{
	    status.S_CTL_REQUEST( status, ctrlName, sourceName(), sourceAddr(),
	    	"unknown", "DHS_CS_ERROR" );
	}
	else
	{
	    status.S_CTL_REQUEST( status, ctrlName, sourceName(), sourceAddr(),
	    	returnLabel(), "DHS_CS_ERROR" );
	}
	response( DHS_CS_ERROR, cmdStatus.message(), dhsStatus );
    }

    if ( ctrlName != NULL )
    {
	free( ctrlName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::getLabel
//
// INVOCATION:
// getLabel( status, control )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &)  Function return status.
// (>) control (DHS_BD_CTL)  Control request type.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called to extract the dataLabel attribute from the avList
//
// DESCRIPTION:
// This function is called to extract the dataLabel attribute from the avList
// unless the control type is get label.
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

void 	cDtsDhsControl::getLabel
(
    cDtsStatus	&status,		// (mod) Return function status.
    DHS_BD_CTL 	control			// (in)  The control command.
)
{

    cDhsAttrib	attrib;				// An attribute in the list.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );	// Function return status.
    void	*label;				// Pointer to the label.


    if ( control == DHS_BD_CTL_GETNAME )
    {
	//
	//  Control type is get new unique name, so just return.
	//

	return;
    }


    //
    // Find the datasetName in the attribute list.
    //

    attrib = find( DTS_CTL_LABEL, dhsStatus );

    if ( dhsStatus == DHS_S_SUCCESS )
    {
	attrib.info( &label, dhsStatus );
	dcrLabel = (char *) label;
    }
    else
    {
	//
	//  Didn't find the dataset name, error.
	//

	cmdStatus.E_NO_LIST( cmdStatus, DTS_CTL_LABEL, "unknown", "unkown" );
	status.E_NO_LIST( status, sourceName(), sourceAddr(), 
		DTS_CTL_LABEL, "unknown", "unknown" );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::generateUniqueName
//
// INVOCATION:
// generateUniqueName()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &)  Function return status.
// (<) uniqueName (char **)  Return unique name that was generated.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function creates a new unique name
//
// DESCRIPTION:
// This function creates a new unique name based on variables set in
// the configuration file.
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

void cDtsDhsControl::generateUniqueName
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	**uniqueName		// (out) Unique name generated.
)
{
    long		dateNotified;	// Today's date.
    cDtsUniqueList	*pTemp;		// Ptr to new object.


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Generate a new unique name.
    //

    cDtsUniqueList::createUniqueName( status, uniqueName );


    //
    //  Add the unique name to the database, if NOT in standalone.
    //

    if ( !status.standAlone() )
    {
	dateNotified = time(NULL);
	cDtsUniqueTable::uniqueNamePut( status, *uniqueName, dateNotified );
    }


    //
    //  Check the status and add to the list if successful.
    //

    if ( !status.ok() )
    {
	cmdStatus.E_NO_NAME( cmdStatus );
    }
    checkStat( status, return );
    pTemp = new cDtsUniqueList( *uniqueName, dateNotified );
    pTemp->add( status );
    status.S_DS_LIST_ADD( status, *uniqueName );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::rmDataset
//
// INVOCATION:
// rmDataset()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) return function status.
// (>) datasetName (char *) Dataset name to remove.
// (>) abortDs (bool)  If set indicates abort dataset, otherwise reset.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function deletes all information on a dataset so long as
// all of it has not been received yet
//
// DESCRIPTION:
// This function deletes all information on a dataset so long as
// all of it has not been received yet.
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

void 	 cDtsDhsControl::rmDataset
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName,		// (in)  Dataset name.
    bool	abortDs			// (in)  Abort ds, or reset ds.
)
{
    cDtsDatasetList
    		*pListPtr;	// Pointer to item in list.
    char	*cntrl = NULL;	// Control type string for printing.


    //
    //  Check the status.
    //

    checkStat( status, return );

    //
    //  Save the string type.
    //

    if ( abortDs )
    {
	cntrl = strsav ( "abort");
    }
    else
    {
	cntrl = strsav ( "reset");
    }


    //
    //  Find the dataset list.
    //

    pListPtr =  cDtsDatasetList::datasetNameFind( status, datasetName );

    if ( status.ok() && status != status.S_NOT_IN_DS_LIST )
    {
	cDtsDatasetTable	*dsPtr = NULL;
	pListPtr->datasetObjectLock();
	dsPtr = pListPtr->getDs();

	if ( dsPtr != NULL && dsPtr->dstReceived == DTS_DS_ALL_RECEIVED )
	{
	    //
	    //  Have already received all data, function not possible.
	    //

	    cmdStatus.E_RMV_RECEIVED( cmdStatus, datasetName, cntrl );
	    status.E_RMV_RECEIVED( status, datasetName );
	}
	else if ( dsPtr != NULL && dsPtr->dstReceived == DTS_DS_PART_RECEIVED )
	{
	    //
	    //  Delete from ds list, md, file system, delete from process.
	    //  Add back to ds if reset was used.
	    //

	    if ( abortDs )
	    {
		//
		//  Remove all information about this dataset.
		//

		pListPtr->datasetObjectUnlock();
		pListPtr->datasetListRemove( status, dsPtr->dstReceived,
			dsPtr->dstFormat );
		pListPtr->datasetObjectLock();
	    }
	    else
	    {
		//
		//  Reset all varaibles about this dataset and remove
		//  all files associated with this dataset.
		//

		pListPtr->datasetListReset( status, dsPtr );
	    }
	    cDtsLists::lstMutexNumIncomp.lock();
	    cDtsDhs::numIncomp( FALSE, status );
	    cDtsLists::lstMutexNumIncomp.unlock();
	}
	else
	{		
	    //
	    //  Have never received data, if aborting, delete it from
	    //  list and  process table.
	    //

	    if ( abortDs )
	    {
		pListPtr->datasetObjectUnlock();
		pListPtr->datasetListRemove( status, DTS_DS_NOT_RECEIVED,
			DTS_FM_UNKNOWN );
		pListPtr->datasetObjectLock();
	    }
	    status.S_DEL_NODATA( status, datasetName );
	}
	pListPtr->datasetObjectUnlock();
	if ( abortDs && status.ok() )
	{
	    delete pListPtr;
	}

    }
    else if ( status == status.S_NOT_IN_DS_LIST )
    {
	//
	//  Never new about this dataset, return a warning.
	//

	cmdStatus.S_WARN_INV_DS( cmdStatus, datasetName );
	status.S_WARN_INV_DS( status, datasetName, cntrl );
    }

    if ( cntrl != NULL )
    {
	free( cntrl );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::setContribList
//
// INVOCATION:
// setContribList
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) return function status.
// (>) contribs (char **)  Passed in array of contributors.
// (>) numContribs (int) Number of contributors in the list.
// (>) datasetName (char *) Dataset name to remove.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function sets the contributor list
//
// DESCRIPTION:
// This function sets the contributor list.
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

void 	 cDtsDhsControl::setContribList
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	**contribs,		// (in)  Contributors.
    int		numContribs,		// (in)  Number of contributors.
    char	*datasetName		// (in)  Dataset name.
)
{
    bool	allReceived;	// All data received.
    bool	badType;	// Bad type match.
    char	*contribList;	// Ptr to contributor list.
    cDtsDatasetTable
    		*dsPtr;		// Pointer to database info.
    bool	newName;	// New name generated.
    bool	notContrib;	// Not a contributor.
    cDtsDatasetList
    		*pList;		// Pointer to List information.
    char	*uniqueName;	// Unique name in the datasetName.


    //
    //  Check the status.
    //

    checkStat( status, return );
    dsPtr = NULL;


    //
    //  Verify the unique name, and datasetname.
    //

    cDtsUniqueList::uniqueNameVerify( status, datasetName, &uniqueName );
    pList = cDtsDatasetList::datasetNameVerify( status, &datasetName, 
	    uniqueName, sourceName(), DHS_BD_PT_UNKNOWN, &dsPtr, 0, 
	    allReceived, notContrib, badType, newName );
    free( uniqueName );
    if ( status.ok() && allReceived )
    {
	//
	//  Data has already been received, error.
	//

	cmdStatus.E_DATASET_RECEIVED( cmdStatus, datasetName, 
		"contributors list" );
	status.E_DATASET_RECEIVED( status, datasetName, "contrib list" );
    }
    else if ( status.ok() && dsPtr != NULL )
    {
	//
	//  Set the contributor list and update the database.
	//  Put all strings in a space separated list, so
	//  can put in the database.
	//

	contribList = (char *) gen_alloc( 256 );

	if ( dsPtr->dstContributors != NULL )
	{
	    free ( dsPtr->dstContributors );
	}


	(void) strcpy( contribList, contribs[0] );
	for( int  i=1; i < numContribs; i++ )
	{
	    (void) strcat( contribList, " " );
	    (void) strcat( contribList, contribs[i] );
	    
	}
	dsPtr->dstContributors = contribList;
	cDtsDatasetList::resetChunkList( status, datasetName );

	cDtsDatasetList::updateStatus( status, datasetName, dsPtr );

	//
	//  Unlock the object.
	//

	if ( pList != NULL )
	{
	    pList->datasetObjectUnlock();
	}
    }
    else
    {
	cmdStatus.E_DATASET_NAME( cmdStatus, datasetName, "contributors list" );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::setLifetime
//
// INVOCATION:
// setLifetime()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) return function status.
// (>) life (DHS_BD_LIFETIME) Lifetime to set.
// (>) datasetName (char *) Dataset name to remove.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function sets the lifetime for a dataset
//
// DESCRIPTION:
// This function sets the lifetime for a dataset.  It will also 
// reset the Storage server notify variable.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// Does not handle transient lifetime, just sets it to temporary.
//-
//***********************************************************************
//

void 	 cDtsDhsControl::setLifetime
(
    cDtsStatus	&status,		// (mod) Returned function status.
    DHS_BD_LIFETIME
    		life,			// (in)  Lifetime to set.
    char	*datasetName		// (in)  Dataset name.
)
{
    bool	allReceived;	// All data received.
    bool	badType;	// Bad type match.
    cDtsDatasetTable
    		*dsPtr;		// Pointer to database info.
    bool	newName;	// New name generated.
    bool	notContrib;	// Not a contributor.
    cDtsDatasetList
    		*pList;		// Pointer to List information.
    char	*uniqueName;	// Unique name in the datasetName.


    //
    //  Check the status.
    //

    checkStat( status, return );
    dsPtr = NULL;


    //
    //  Verify the unique name, and datasetname.
    //

    cDtsUniqueList::uniqueNameVerify( status, datasetName, &uniqueName );
    pList = cDtsDatasetList::datasetNameVerify( status, &datasetName, 
	    uniqueName, sourceName(), DHS_BD_PT_UNKNOWN, &dsPtr, 0, 
	    allReceived, notContrib, badType, newName );
    free( uniqueName );
    if ( status.ok() && allReceived )
    {
	//
	//  Error, data has already been received for this dataset.
	//
	cmdStatus.E_DATASET_RECEIVED( cmdStatus, datasetName, "lifetime" );
	status.E_DATASET_RECEIVED( status, datasetName, "lifetime" );
    }
    else if ( status.ok() && dsPtr != NULL )
    {
	//
	//  Set the lifetime & the Storage Server notify. Update the database.
	//

	if ( life == DHS_BD_LT_PERMANENT ) 
	{
	    dsPtr->dstLifetime = DTS_PERM;
	    dsPtr->dstSsNotify = DTS_SS_NOT_NOTIFIED;
	}
	else
	{
	    dsPtr->dstLifetime = DTS_TEMP;
	    dsPtr->dstSsNotify = DTS_SS_UNECESSARY;
	}

	cDtsDatasetList::updateStatus( status, datasetName, dsPtr );


	//
	//  Unlock the object.
	//

	if ( pList != NULL )
	{
	    pList->datasetObjectUnlock();
	}
    }
    else
    {
	cmdStatus.E_DATASET_NAME( cmdStatus, datasetName, "lifetime" );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::setQlStream
//
// INVOCATION:
// n.a.  This function is executed by the exec function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) return function status.
// (>) qlStreams  (DHS_BD_LIFETIME) Lifetime to set.
// (>) datasetName (char *) Dataset name to remove.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called to send the quick look stream to the QL server.
//
// DESCRIPTION:
// This function is called to send the quick look stream to the QL server.
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

void 	 cDtsDhsControl::setQlStream
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	**qlStreams,		// (in)  Contributors.
    char	*datasetName,		// (in)  Dataset name.
    unsigned long
    		dims[7]			// (in)  Dimensions.
)
{
    DHS_STATUS			dhsStatus( DHS_S_SUCCESS );
    cDhsClientCommand		pCmd( DTS_QL_NOTIFY_CMD );


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    // Lookup the ql server ip address and connect to the ql server.
    //

    cDhsConnection	*pConn;
    DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;

    if ( status.serversOn() )
    {
	if ( cDtsConManager::connectTo( status, cDtsDhs::qlServer(), &pConn ) )
	{
	    //
	    //  Are connected, send control to quick look.
	    //

	    checkDhs( pCmd.add( DTS_QL_DS_ATTRIB, DHS_DT_STRING, 
		    datasetName, dhsStatus ), dhsStatus, status, VOID );
	    checkDhs( pCmd.add( DTS_QL_STREAM_ATTRIB, DHS_DT_STRING, 1, dims, 
		    qlStreams, dhsStatus ), dhsStatus, status, VOID );

	    //
	    //  Apply the command and wait for the reply.
	    //

	    checkDhs( pCmd.apply( *pConn, dhsStatus ), 
		dhsStatus, status, VOID );
	    pCmd.wait( dhsStatus );
	    checkDhs( (cmdStat = pCmd.status( dhsStatus )), dhsStatus, status, 
		    VOID );

	    if ( cmdStat != DHS_CS_DONE || !status.ok() || 
		    dhsStatus != DHS_S_SUCCESS )
	    {
		//
		//  Put failed, log the error.
		//

		status.S_QL_STREAM( status, sourceName(), sourceAddr(), 
			datasetName, "DHS_CS_ERROR" );
	    }
	    else
	    {
		//
		//  Put succeeded.
		//

		status.S_QL_STREAM( status, sourceName(), sourceAddr(), 
			datasetName, "DHS_CS_DONE" );
	    }
	}
	else
	{
	    //
	    //  Could not connect to QL server.
	    //

	    status.S_QL_CONNECT( status, "QL stream control", sourceName(), 
		    sourceAddr(), datasetName );
	}
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsControl::abortQlStream
//
// INVOCATION:
// n.a.  This function is executed by the exec function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) return function status.
// (>) datasetName (char *) Dataset name to remove.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is called to abort the quick look stream on the QL server.
//
// DESCRIPTION:
// This function is called to abort the quick look stream on the QL server.
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

void 	 cDtsDhsControl::abortQlStream
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName		// (in)  Dataset name.
)
{
    DHS_STATUS			dhsStatus( DHS_S_SUCCESS );
    cDhsClientCommand		pCmd( DTS_QL_ABORT_CMD );


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    // Lookup the ql server ip address and connect to the ql server.
    //

    cDhsConnection	*pConn;
    DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;

    if ( status.serversOn() )
    {
	if ( cDtsConManager::connectTo( status, cDtsDhs::qlServer(), &pConn ) )
	{
	    //
	    //  Are connected, send control to quick look.
	    //

	    checkDhs( pCmd.add( DTS_QL_DS_ATTRIB, DHS_DT_STRING, 
		    datasetName, dhsStatus ), dhsStatus, status, VOID );

	    //
	    //  Apply the command and wait for the reply.
	    //

	    checkDhs( pCmd.apply( *pConn, dhsStatus ), 
		dhsStatus, status, VOID );
	    pCmd.wait( dhsStatus );
	    checkDhs( (cmdStat = pCmd.status( dhsStatus )), dhsStatus, status, 
		    VOID );

	    if ( cmdStat != DHS_CS_DONE || !status.ok() || 
		    dhsStatus != DHS_S_SUCCESS )
	    {
		//
		//  Put failed, log the error.
		//

		status.S_QL_ABORT( status, sourceName(), sourceAddr(), 
			datasetName, "DHS_CS_ERROR" );
	    }
	    else
	    {
		//
		//  Put succeeded.
		//

		status.S_QL_ABORT( status, sourceName(), sourceAddr(), 
			datasetName, "DHS_CS_DONE" );
	    }
	}
	else
	{
	    //
	    //  Could not connect to QL server.
	    //

	    status.S_QL_CONNECT( status, "QL abort dataset", sourceName(), 
		    sourceAddr(), datasetName );
	}
    }

}
