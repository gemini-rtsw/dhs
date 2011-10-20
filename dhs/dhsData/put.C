static char rcsid[] = "$Id: put.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/put.C
//
// PURPOSE:
// Handles functions required in bulk data put requests.
//
// METHOD NAME(S)
// cDtsDhsPut::~cDtsDhsPut()	: Deconstructor.
// cDtsDhsPut::destinationExtract() : Determine where to write a put buffer.
// cDtsDhsPut::fileProcess()	: Process the file - write it somewhere.
// cDtsDhsPut::lastFlagCheck()	: Check if last flag is set.
// cDtsDhsPut::oldpServerNotify() : Notify the OLDP server.
// cDtsDhsPut::qlSend()		: Send to Quick Look.
// cDtsDhsPut::serverNotify()	: Determine which server to send it to.
// cDtsDhsPut::setSeg()		: Determine the segration info.
// cDtsDhsPut::storageServerNotify()	: Notify the Storage Server.
// cDtsPutRaw::exec()		: Execution point of a put of RAW data.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:23  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.18  2000/08/10 22:12:04  dunn
//  Added check in for bad data type.
//
// Revision 1.17  2000/07/27 20:10:28  dunn
// Added standalone features.
//
// Revision 1.16  1998/06/22 20:48:03  dunn
// Fixed some messages to user.
//
// Revision 1.15  1998/06/11 16:40:21  dunn
// No longer writes to /tmp if write to temp or perm fails.
//
// Revision 1.14  1998/05/26 18:48:21  dunn
// Rounded the buf size guess to be multiple of 2880 + 1.
//
// Revision 1.13  1998/05/25 20:04:49  dunn
// Fixed it so that bufSize was not changed as cfitsio uses the address.
//
// Revision 1.12  1998/05/13 19:45:17  dunn
// Added warning messages if the sds2fits failed - returned to user.
//
// Revision 1.11  1998/05/04 23:01:19  dunn
// Changes for status, messages, and putting numm ll
// buffer.
//
// Revision 1.10  1997/10/17 20:46:01  dunn
// Changed logic to return an error if there were any warnings.
//
// Revision 1.9  1997/10/03 23:02:12  dunn
// Got rid of cDtsDhsQlPut class, and use cDhsPutDataset instead.
//
// Revision 1.8  1997/10/01 16:23:54  dunn
// Code cleanup.
//
// Revision 1.7  1997/09/29 21:22:14  dunn
// Fixed bug in calculation of space used.
//
// Revision 1.6  1997/09/17 22:13:43  dunn
// Code cleanup and insuring locks are enabled.
//
// Revision 1.5  1997/08/29 23:03:47  dunn
// Fixed leaks, cleaned it up, added the close routines.
//
// Revision 1.4  1997/08/19 22:31:03  dunn
// Implemented qlSend and other fixes.
//
// Revision 1.3  1997/07/29 22:02:54  dunn
// Fixed space update and various little things.
//
// Revision 1.2  1997/07/23 19:41:55  dunn
// Changed storageServer command string and attribute names.
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

#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "store.H"
#include "con.H"
#include "dhsGenConnect.H"
#include "cmd.H"
#include "put.H"


//
//  Local defines.
//

msgText( cDtsDhsPut::pStatus, S_BAD_TYPE, P_NULL,
	    "Some or all data received with different data type `%c', not `%c'." );
msgText( cDtsDhsPut::pStatus, S_CONV_WARNING, P_NULL,
	    "Warnings generated converting to FITS:\n%s." );
msgText( cDtsDhsPut::pStatus, S_DATA_RECEIVED, P_NULL,
	    "Data already received for this dataset." );
msgText( cDtsDhsPut::pStatus, S_DATA_RECOVERY, P_NULL,
	    "Error composing the dataset, recovering dataset %s, tarring dataset.\n" );
msgText( cDtsDhsPut::pStatus, S_E_CONV_WARNING, P_NULL,
	    "Error converting to FITS:\n%s." );
msgText( cDtsDhsPut::pStatus, S_E_TAR_FILE, P_NULL,
	    "Error tarring dataset:\n%s." );
msgText( cDtsDhsPut::pStatus, S_FILE_OPEN, P_NULL,
	    "Error, file open failed on the data server for dataset : %s\n" );
msgText( cDtsDhsPut::pStatus, S_FILE_WRITE, P_NULL,
	    "Error, file write failed on the data server for dataset : %s\n" );
msgText( cDtsDhsPut::pStatus, S_HEADER_ERROR, P_NULL,
	    "Error parsing the header, saving but setting health to bad, error : %s." );
msgText( cDtsDhsPut::pStatus, S_HEADER_WARN, P_NULL,
	    "Warnings parsing the header : %s." );
msgText( cDtsDhsPut::pStatus, S_NEW_UNIQUE_NAME, P_NULL,
	    "New datasetName is : %s\n" );
msgText( cDtsDhsPut::pStatus, S_NEW_DS_NAME, P_NULL,
	    "The new dataset name generated for %s is : %s." );
msgText( cDtsDhsPut::pStatus, S_NO_DATA, P_NULL,
	    "Error, composing the dataset, %s, no data received.\n" );
msgText( cDtsDhsPut::pStatus, S_NO_SPACE, P_NULL,
	    "Not enough space available : %d to write datasetName : %s\n" );
msgText( cDtsDhsPut::pStatus, S_NOT_CONTRIB, P_NULL,
	    "You are not a registered contributor for this dataset." );
msgText( cDtsDhsPut::pStatus, S_TMP_FILE, P_NULL,
	    "An unrecoverable error occurred, NOT writing the data!!!!\n" );
msgText( cDtsDhsPut::pStatus, S_WARN_INV_LABEL, P_NULL,
	    "Invalid datasetname sent, too many spaces, new datasetName is : %s\n" );
msgText( cDtsDhsPut::pStatus, S_WARN_LAST_FLAG, P_NULL,
	    "Last Flag set incorrectly for datatype %d for dataset : %s\n" );
msgText( cDtsDhsPut::pStatus, S_WARN_NULL_LABEL, P_NULL,
	    "Empty datasetname sent, new datasetName is : %s\n" );


#define DTS_FITS_EXT	".fits"
#define DTS_RAW_EXT	".raw"
#define DTS_UNKNOWN_EXT	".unk"


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::~cDtsDhsPut
//
// INVOCATION:
// n.a.  Deconstructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor
//
// DESCRIPTION:
// Deconstructor.  Free segregation directory name.
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

	cDtsDhsPut::~cDtsDhsPut
(
    void
)
{
    if ( cdpSegDir != NULL )
    {
	free ( cdpSegDir );
	cdpSegDir = NULL;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::destinationExtract
//
// INVOCATION:
// destinationExtract ()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (<) fname (char **) File name to write to.
// (<) filePath (char **) Complete path with file name to write to.
// (<) vol (char **) Volume name to write to in ad.
// (>) size (int) Size to write.
// (>) dsPtr (cDtsDatasetTable *) Dataset table information.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To determine where to write the buffer
//
// DESCRIPTION:
// To determine where to write the buffer, either temporary or permanent
// store.  Also determine if there is enough room to write the file.
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

void cDtsDhsPut::destinationExtract
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	**fname,		// (out) Filename.
    char	**filePath,		// (out) Complete filename.
    char	**vol,			// (out) Volume name.
    int		size,			// (in)  Size to write.
    cDtsDatasetTable
    		*dsPtr			// (in)  Dataset table information.
)
{
    int		spaceAvail;		// Current space avail.
    cDtsStoreManager::tLife		// Lifetime flag.
    		storeFlag;


    //
    //  Check the status.
    //

    checkStat( status, return );



    //
    //  If the lifetime is permanent, then the path is the perm. store
    //  else temporary store.
    //

    checkNull( *filePath = (char *) gen_alloc ( PATH_MAX ), status,
    	    return );

    if ( dsPtr->dstLifetime == DTS_PERM )
    {
	*vol =  cDtsStoreManager::permVol();
	(void) strcpy( *filePath, cDtsStoreManager::permPath() );
	storeFlag =  cDtsStoreManager::ST_PERM;
    }
    else
    {
	*vol =  cDtsStoreManager::tempVol();
	(void) strcpy( *filePath, cDtsStoreManager::tempPath() );
	storeFlag =  cDtsStoreManager::ST_TEMP;
    }


    //
    //  Now append the datasetname + "." + datatype.
    //

    (void) strcat( *filePath, "/" );
    *fname = *filePath + strlen (*filePath );
    (void) strcat( *filePath, dsPtr->dstDatasetName );

    if ( type() == DHS_BD_PT_RAW || type() == DHS_BD_PT_RAW_UNIQUE )
    {
	(void) strcat( *filePath, DTS_RAW_EXT );
    }
    else if ( type() == DHS_BD_PT_FITS || type() == DHS_BD_PT_FITS_UNIQUE ||
	    type() == DHS_BD_PT_DS )
    {
	(void) strcat( *filePath, DTS_FITS_EXT );
    }
    else
    {
	(void) strcat( *filePath, DTS_UNKNOWN_EXT );
    }


    //
    //  See if we have enough disk space to write the file.
    //

    spaceAvail = cDtsStoreManager::determineSpace( status, storeFlag );
    if ( status.ok() && (spaceAvail - ( (size+1023)/1024) )  <  1 ) 
    {
	cDtsStoreManager::updateSpace( status, storeFlag, 0 );
	status.E_NO_SPACE( status, spaceAvail, *fname, ((size+1023)/1024) );
	putStatus.S_NO_SPACE( putStatus, spaceAvail, dsPtr->dstDatasetName  );
	return;
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::fileProcess
//
// INVOCATION:
// fileProcess( status, filePath, bufSize )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) filePath (char *) Complete path where to write the file.
// (>) bufSize (int) Size of the buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To write the data received
//
// DESCRIPTION:
// To write the data received to the filePath passed in.  If the status is
// not ok, then at least write the data to /tmp with a temporary filename.
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

void cDtsDhsPut::fileProcess
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	*filePath,		// (in)  Complete path.
    int		bufSize			// (in)  Size of the buffer.
)
{
    FILE        *fp;                    // File pointer.
    int		actWritten;		// Num bytes actually written.
    char	*path;
    


    //
    //  Check the status.  If the status is not ok at this point, then
    //  generate a new name and write it there so that at least the data
    //  is somewhere.
    //

    if ( !status.ok() || filePath == NULL )
    {
	putStatus.S_TMP_FILE( putStatus );
	status.E_NOT_WRITTEN( status, label() );
	return;
    }
    else
    {
	path = filePath;
    }
    

    //
    //  Open the file for writing.
    //

    if ( ( fp = fopen( path,  "w" ) ) == NULL )
     {
	status.E_FILE_OPEN( status, path );
	status.sysErrno();
	status.display();
	putStatus.S_FILE_OPEN( putStatus, label() );
	return;
     }


    //
    //  Write the file.
    //

    if ( ( actWritten = fwrite(buffer(), 1, bufSize, fp ) ) != bufSize )
    {
	status.E_FILE_WRITE( status, path );
	status.sysErrno();
	putStatus.S_FILE_WRITE( putStatus, label() );
	(void) fclose ( fp );
	return;
    }
    (void) fclose ( fp );
    status.S_WROTE_FILE( status, path );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::lastFlagCheck
//
// INVOCATION:
// lastFlagCheck( status, datasetName )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) datasetName (char * ) - DatasetName
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To check that the last flag is set properly for the data type
//
// DESCRIPTION:
// To check that the last flag is set properly for the data type.
// For raw, fits or DS_QL the last flag should be set.  A warning
// is sent back if it is not set.
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

void cDtsDhsPut::lastFlagCheck
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	*datasetName	 	// (in)  Datasetname.
)
{
    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Check that the first word is simple if type if fits.
    //

    if ( ( type() == DHS_BD_PT_FITS || type() == DHS_BD_PT_RAW ||
	    type() == DHS_BD_PT_FITS_UNIQUE ||
	    type() == DHS_BD_PT_RAW_UNIQUE ||
	    type() == DHS_BD_PT_DS_QL ) && !last() )
    {
	putStatus.S_WARN_LAST_FLAG( putStatus, type(), datasetName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::oldpServerNotify
//
// INVOCATION:
// oldpServerNotify( status, dsPtr, clientName, ipAddr )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (!) dsPtr (cDtsDatasetTable *) Dataset table information.
// (>) clientName (char * ) Name of the requestor.
// (>) ipAddr (char * ) IP Address of the requestor.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is notifies the oldp server that the dataset is available
//
// DESCRIPTION:
// This function is notifies the oldp server that the dataset is available
// for retrieval.
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

void cDtsDhsPut::oldpServerNotify
(
    cDtsStatus	&status,		// (mod) Status passed in.
    cDtsDatasetTable
    		*dsPtr,			// (mod) Dataset table information.
    char	*clientName,		// (in)  Source of request.
    char	*ipAddr			// (in)  Ip source.
)
{
    DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;
    					// Return command status.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    					// Dhs status.
    cDhsClientCommand
    		pCmd( DTS_OLDP_NOTIFY_CMD );
					// Pointer to archive command.
    cDhsConnection	*pConn;		// Connection information.


    //
    //  Check the status.
    //

    checkStat( status, return );


    if ( cDtsConManager::connectTo( status, cDtsDhs::oldpServer(), &pConn ) )
    {
	//
	//  Connected to the oldp server, compose the command.
	//

	checkDhs( pCmd.add( DTS_SS_FILE_ID, DHS_DT_STRING, 
		dsPtr->dstDatasetName, dhsStatus ), dhsStatus, 
		status, VOID );

	//
	//  Apply the command, wait for the reply, and check that status.
	//

	checkDhs( pCmd.apply( *pConn, dhsStatus ), 
	    dhsStatus, status, VOID );
	pCmd.wait( dhsStatus );
	checkDhs( (cmdStat = pCmd.status( dhsStatus )), dhsStatus, status, 
		VOID );


	//
	//  Update the oldp notify flag if status was successful.
	//
	if ( cmdStat != DHS_CS_DONE || !status.ok() || 
		dhsStatus != DHS_S_SUCCESS )
	{
	    status.S_OLDP_NOTIFY( status, clientName, ipAddr, 
		    dsPtr->dstDatasetName, "DHS_CS_ERROR" );
	}
	else
	{
	    dsPtr->dstOldpNotify = DTS_OL_NOTIFIED;
	    status.S_OLDP_NOTIFY( status, clientName, ipAddr,
		    dsPtr->dstDatasetName, "DHS_CS_DONE" );
	}
    }
    else
    {
	//
	//  Could not connect to the oldp server.
	//

	status.S_SERV_CONNECT( status, "oldp", "file notify command", 
		clientName, ipAddr, dsPtr->dstDatasetName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::qlSend
//
// INVOCATION:
// qlSend()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (<) datasetName (char *) Dataset name to send.
// (<) lastChunk (bool) True indicates this is the last chunk for this dataset.
// (!) pDsList (cDtsDatasetList *) Dataset list information.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function converts the buffer, if necessary and sends to quick look
//
// DESCRIPTION:
// This function converts the buffer from sds to fits if the datatype is 
// DS.  Then the data is sent in fits format to the quick look server.
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

void cDtsDhsPut::qlSend
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	*datasetName,		// (in)  Datasetname.
    bool	lastChunk,		// (in)  Last of the data.
    cDtsDatasetList
    		*pDsList		// (mod) Dataset object.
)
{
    long 		bufSize = length();
    					// Length of the buffer.
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    					// Dhs status.
    void		*fitsBuffer = NULL;
    					// Buffer pointer.
    fitsfile		*ffPtr;		// Fits file pointer.
    cDhsPutDataset	*pPut;		// Put pointer.
    SF_STATUS		sfStatus = SF_S_SUCCESS;
    					// Sf status.
    int			tmpSize;	// Temp size.


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  If type is ds, then convert to fits buffer.
    //  Doesn't matter if quickLook is available, need dsInfo from convert
    //  anyway.
    //

    if ( type() == DHS_BD_PT_DS || type() == DHS_BD_PT_DS_QL )
    {
	SF_DS_INFO	*dsInfo = NULL;

	if ( length() == 0 )
	{
	    fitsBuffer = NULL;
	}
	else
	{
	    //
	    //  This is a DS chunk, alloc room for the fits buffer, and access
	    //  the sds strubutre.
	    //

	    bufSize = (long int)(bufSize + 0.85 * bufSize);
	    bufSize = (bufSize / 2880) + 1;
	    bufSize = bufSize * 2880;

	    checkNull( ( fitsBuffer = gen_calloc( bufSize, 1 )), status, return );
	    checkDhs( access( dhsStatus ), dhsStatus, status, VOID );

	    cDtsPutDs::lockSf();
	    if ( type() == DHS_BD_PT_DS )
	    {
		//
		//  This is DS, so use the dsInfo from the datasetlist, 
		//  if it exists.
		//

		if ( pDsList->cdlDsInfo == NULL )
		{
		    //
		    //  This is the first chunk, init the dsInfo structure.
		    //

		    checkSf( ( pDsList->cdlDsInfo = sfDsInfoInit( datasetName,
			    &sfStatus ) ), sfStatus, status, VOID ); 
		}
		dsInfo = pDsList->cdlDsInfo;
	    }
	    else if ( type() == DHS_BD_PT_DS_QL )
	    {
		//
		//  Else the type is DS_QL, create a new dsInfo structure.
		//

		checkSf( ( dsInfo = sfDsInfoInit( datasetName, &sfStatus ) ), 
			sfStatus, status, VOID ); 
	    }
	    if ( !status.ok() )
	    {
		//
		//  Not successful creating dsInfo structure, free it and return
		//

		if ( type() == DHS_BD_PT_DS_QL && dsInfo != NULL )
		{
		    sfDsInfoFree( dsInfo );
		    dsInfo = NULL;
		}
		cDtsPutDs::unlockSf();
		if ( fitsBuffer != NULL )
		{
		    free( fitsBuffer );
		    fitsBuffer = NULL;
		}
		return;
	    }


	    //
	    //  Set the file pointer to point at the buffer and convert.
	    //

	    checkSfLib( ( ffPtr = sfSetFitsMem( &fitsBuffer, &bufSize, dsInfo, 
		    &sfStatus )), sfStatus, datasetName, dsInfo, status, VOID );
       if ( status.ok() )
	    {
		sfSds2Fits( objectId(), ffPtr , dsInfo, TRUE, &sfStatus );
		if ( sfStatus <  SF_S_SUCCESS )
		{
		    putStatus.S_E_CONV_WARNING( putStatus, sfMsg );
		    status.E_SDS2FITS( status, datasetName, sfMsg );
		}
	    }


	    //
	    //  Free dsInfo, if necessary.
	    //

	    if ( type() == DHS_BD_PT_DS_QL && dsInfo != NULL )
	    {
		sfDsInfoFree( dsInfo );
		dsInfo = NULL;
	    }


	    //
	    //  Put warnings from conversion into return message.
	    //

	    if ( sfStatus > SF_S_SUCCESS  )
	    {
		putStatus.S_CONV_WARNING( putStatus, sfMsg );
		status.S_CONV_WARNINGS( status, datasetName, sfMsg );
	    }


	    //
	    //  Close the fits file.
	    //

	    if ( sfStatus < SF_S_SUCCESS )
	    {
		SF_STATUS		sf2Status = SF_S_SUCCESS;

		sfCloseFitsFile( ffPtr, &sf2Status );
		cDtsPutDs::unlockSf();
		if (  fitsBuffer != NULL )
		{
		    free( fitsBuffer );
		}

		return;
	    }
	    else
	    {
		tmpSize = sfFitsFileSize( ffPtr, &sfStatus );
		sfCloseFitsFile( ffPtr, &sfStatus );
		cDtsPutDs::unlockSf();
		bufSize = tmpSize;
	    }
	}
    }
    else
    {
	//
	//  Else this is a Fits file already.  No conversion required.
	//

	fitsBuffer = buffer();
	bufSize = length();
    }


    //
    //  If the conversion worked, and the serverOn flag is set from cmd line.
    //

    if ( status.ok() && status.serversOn() )
    {
	cDhsConnection	*pConn;			//  Connection pointer.
	DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;	//  Cmd return status.

	if ( cDtsConManager::connectTo( status, cDtsDhs::qlServer(), &pConn ) )
	{
	    //
	    //  Put the data, wait for the completion, check the status.
	    //

	    pPut = new cDhsPutDataset(fitsBuffer, (unsigned long )bufSize, 
		    DHS_BD_PT_FITS );
	    checkDhs( pPut->put( *pConn, datasetName, 
		    lastChunk, dhsStatus ), dhsStatus, status, VOID );
	    pPut->wait( dhsStatus );
	    checkDhs( (cmdStat = pPut->status( dhsStatus )), dhsStatus, status, 
		VOID );
	    status.S_PUT_QL( status, datasetName, cmdStat );
	    
	    delete pPut;


	    //
	    //  Rest the status to success regardless.  We don't really care
	    //  if it fails to get to quick look.
	    //

	    status.status( status.S_SUCCESS );
	}
	else
	{
	    //
	    //  Else were not able to connect to quick loook server.
	    //

	    status.S_QL_CONNECT( status, "QL put data", sourceName(), 
		    sourceAddr(), datasetName );
	}
    }

    //
    //  Free the buffer, if necessary.
    //

    if ( ( type() == DHS_BD_PT_DS || type() == DHS_BD_PT_DS_QL )
	    && fitsBuffer != NULL )
    {
	free( fitsBuffer );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::serverNotify
//
// INVOCATION:
// serverNotify( status, dsPtr );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (!) dsPtr (cDtsDatasetTable *) Dataset table information.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is to notify either/or the oldp or storage server
//
// DESCRIPTION:
// This function is to notify either/or the oldp or storage server.
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

void cDtsDhsPut::serverNotify
(
    cDtsStatus	&status,		// (mod) Status passed in.
    cDtsDatasetTable
    		*dsPtr			// (mod) Dataset table information.

)
{
    bool	updateRequired = FALSE;	//  Indicates updating required.


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Just return if the data server has been started with no servers,
    //  or in standalone mode.
    //

    if ( !status.serversOn() || status.standAlone() )
    {
	return;
    }


    //
    //  Only notify the oldp if all data has been received and the 
    //  health is not sick.
    //

    if ( dsPtr->dstReceived == DTS_DS_ALL_RECEIVED &&
	    dsPtr->dstHealth != DTS_HL_SICK )
    {
	oldpServerNotify( status, dsPtr, sourceName(), sourceAddr() );
	updateRequired = TRUE;
    }


    //
    //  If all has been received, life is permanent and storage
    //  server has not been notified, then notify it.
    //

    if ( dsPtr->dstReceived == DTS_DS_ALL_RECEIVED &&
	    dsPtr->dstLifetime == DTS_PERM &&
	    dsPtr->dstSsNotify == DTS_SS_NOT_NOTIFIED )
    {
	storageServerNotify( status, dsPtr, sourceName(), sourceAddr(), 
		cDtsDhsPut::segDir() );
	updateRequired = TRUE;
    }


    //
    //  Update the database now.
    //

    if ( updateRequired )
    {
	cDtsDatasetList::updateStatus( status, dsPtr->dstDatasetName, dsPtr );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::setSeg
//
// INVOCATION:
// setSeg( status, &datasetName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (!) datasetName (char **) Dataset name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function determines if the segregation dir was sent in the dataset
// label
//
// DESCRIPTION:
// This function determines if the segregation dir was sent in the dataset
// label.  If it was then it will strip the segregation dir off the dataset
// name and save it and reset the dataset name.
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

void cDtsDhsPut::setSeg
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	**datasetName		// (mod) Dataset name.
)
{


    //
    //  Check the status.
    //

    checkStat( status, return );

    char *tmp;
    int                 numTokens;
    char                *tokens[DTS_DATASET_NAME_LEN];

    tmp = strsav( *datasetName );
    strtokens( tmp, DTS_MAX_CONTRIBUTORS, tokens, &numTokens );
    if ( numTokens == 1 )
    {
	//
	//  Then only sent the dataset name, set the segregation dir
	//  to be the default one in the configuration file.
	//

	cDtsDhsPut::segDir( cDtsDhs::segDir() );
    }
    else if ( numTokens == 2 )
    {
	//
	// Assume were sent the segregation dir in first name, and the
	// dataset name in the second.  Strip it off and reset datasetname.
	//

	cDtsDhsPut::segDir( tokens[0] );
	free( *datasetName );
	*datasetName = strsav( tokens[1] );
    }
    else
    {
	//
	//  Error, too many words in the label passed in.  Create a new name.
	//

	status.S_LABEL_INVALID( status, sourceName(), sourceAddr() );
	cDtsUniqueList::createUniqueName( status, datasetName );
	putStatus.S_WARN_INV_LABEL( putStatus, *datasetName );
    }
    free( tmp );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsPut::storageServerNotify
//
// INVOCATION:
// storageServerNotify( status, dsPtr, clientName, ipAddr );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (!) dsPtr (cDtsDatasetTable *) Dataset table information.
// (>) clientName (char *) Name of the requestor.
// (>) ipAddr (char *) IP Address of the requestor.
// (>) segDir (char *) Segregation directory.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is notifies the ss server that a file is available for
// archiving
//
// DESCRIPTION:
// This function is notifies the ss server that a file is available for
// archiving.
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

void cDtsDhsPut::storageServerNotify
(
    cDtsStatus	&status,		// (mod) Status passed in.
    cDtsDatasetTable
    		*dsPtr,			// (mod) Dataset table information.
    char	*clientName,		// (in)  Source of request.
    char	*ipAddr,		// (in)  Ip source.
    char	*segDir			// (in)  Segregation dir.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cDhsClientCommand
    		pCmd( DTS_SS_NOTIFY_CMD );
					// Pointer to archive command.



    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Connect to the storage server.
    //

    cDhsConnection	*pConn;
    DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;

    if ( cDtsConManager::connectTo( status, cDtsDhs::storageServer(), &pConn ) )
    {
	//
	//  Compose the command.  Add fileId, fileSize, segregationDir.
	//

	checkDhs( pCmd.add( DTS_SS_FILE_ID, DHS_DT_STRING, 
		dsPtr->dstDatasetName, dhsStatus ), dhsStatus, status, VOID );
	checkDhs( pCmd.add( DTS_SS_FILE_SZ, DHS_DT_INT32, dsPtr->dstFileSize, 
		dhsStatus ), dhsStatus, status, VOID );
	checkDhs( pCmd.add( DTS_SS_SEG_DIR, DHS_DT_STRING, segDir,
		dhsStatus ), dhsStatus, status, VOID );


	//
	//  Apply the command, wait for the reply, check the return status.
	//

	checkDhs( pCmd.apply( *pConn, dhsStatus ), 
	    dhsStatus, status, VOID );
	pCmd.wait( dhsStatus );
	checkDhs( (cmdStat = pCmd.status( dhsStatus )), dhsStatus, status, 
		VOID );


	//
	//  Update the ss notify flag if status was successful.
	//

	if ( cmdStat != DHS_CS_DONE || !status.ok() || 
		dhsStatus != DHS_S_SUCCESS )
	{
	    status.S_SS_ARCHIVE( status, clientName, ipAddr, 
		    dsPtr->dstDatasetName, "DHS_CS_ERROR" );
	}
	else
	{
	    dsPtr->dstSsNotify = DTS_SS_NOTIFIED;
	    status.S_SS_ARCHIVE( status, clientName, ipAddr,
		    dsPtr->dstDatasetName, "DHS_CS_DONE" );
	}
    }
    else
    {
	//
	//  Were not able to connect to storage server.
	//

	status.S_SERV_CONNECT( status, "storage", "archive file command", 
		clientName, ipAddr, dsPtr->dstDatasetName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsPutRaw::exec
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
// request
//
// DESCRIPTION:
// This function is called by the put handler to start the processing of a
// request.
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

void cDtsPutRaw::exec
(
)
{
    char	*address = NULL;// Ip address of the client.
    bool	allReceived;	// All data received.
    bool	badType;	// Bad type match.
    char	*clientName = NULL;
    				// Name of the client.
    char	*datasetName;	// Datasetname.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// Dhs status.
    cDtsDatasetTable
    		*dsPtr;		// Pointer to database info.
    char	*filePath;	// Path to where the file is.
    char	*fname;		// Filename.
    bool	newName;	// New name generated.
    bool	notContrib;	// Not a contributor.
    cDtsDatasetList
    		*pList;		// Pointer to List information.
    cDtsStatus	status;		// Function return status.
    char	*uniqueName;	// Unique name in the datasetName.
    char	*volName;	// Volume name.


    status.S_PUT_HANDLER( status, type(), label() );
    putStatus.displayStop();


    //
    //  If data type is not handled, return an error. 
    //  If in simulate mode, return a done.
    //  Otherwise send a response to the client so it can get on with its life.
    //

    if ( type() == DHS_BD_PT_UNKNOWN || type() == DHS_BD_PT_SDS )
    {
	cdnResponseAvList.add( "datasetName", DHS_DT_STRING, label(), 
		dhsStatus );
	status.S_PUT_REQ( status, "N/A", "N/A", label(), "DHS_CS_ERROR" );
	response( DHS_CS_ERROR, "Data type not handled by data server", 
		cdnResponseAvList, dhsStatus );
	return;
    }

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
    //  Determine if the segregation dir has been sent in the label.
    //

    setSeg( status, &datasetName );


    //
    //  Verify the unique name, and datasetname.
    //

    cDtsUniqueList::uniqueNameVerify( status, datasetName, &uniqueName );
    pList = cDtsDatasetList::datasetNameVerify( status, &datasetName, 
	    uniqueName, sourceName(), type(), &dsPtr, length(), allReceived,
	    notContrib, badType, newName );
    free( uniqueName );

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
    if ( status.standAlone() && pList != NULL )
    {
	pList->updateObject( status, filePath, type() );
    }

    free( filePath );


    //
    // Update status in the database.
    //

    cDtsDatasetList::updateStatus( status, datasetName, dsPtr );


    //
    //  Log the event and send a completed response.
    //
 
    dhsStatus = DHS_S_SUCCESS;
    char *p = ( char *) putStatus.message();
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
	    status.display();
	}

	status.S_PUT_REQ( status, sourceName(), sourceAddr(), datasetName,
		"DHS_CS_DONE");
	response( DHS_CS_DONE, putStatus.message(), cdnResponseAvList, 
		dhsStatus );
    }


    //
    //  Notify the oldp and storage servers.
    //

    serverNotify( status, dsPtr );
    pList->datasetObjectUnlock();
    free( datasetName );
}
