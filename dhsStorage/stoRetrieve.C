static char rcsid[] = "$Id: stoRetrieve.C,v 1.1.1.1 2002-11-24 20:32:21 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1999				(c) 1999
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
// dhsStorage/src/stoRetrieve.C
//
// PURPOSE:
// Contains the implementation of the cStoRetrieve class.
//
// METHOD NAME(S)
// cStoRetrieve::coexist - Determine if this action and the specified action
//		can co-exist on the same media request.
// cStoRetrieve::execute - Execute this retrieve action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.4  2000/10/16 22:20:55  nhill
// Updated to be compatable with the workshop version of STL.
//
// Revision 1.3  1999/10/12 23:18:53  nhill
// Fixed the code that allows multiple sources of data.
//
// Revision 1.2  1999/07/08 21:33:37  nhill
// Changed to allow data to be retrieved from any of the know data servers,
// instead of just the default data server.
//
// Revision 1.1  1999/06/17 00:01:30  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "globals.H"
#include "stoAction.H"
#include "stoFile.H"
#include "stoGet.H"
#include "stoConnection.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRetrieve::coexist
//
// INVOCATION:
// if ( coexist( action.getActionType().getAction(), action.rollback() ) )
// ...
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) actionType (STO_ACTION_TYPE) The type of the action being tested.
// (>) isRollback (bool) Is the action being tested a rollback?
//
// FUNCTION VALUE:
// (boolean) True if the actions coexist.
//
// PURPOSE:
// Determine if this action can coexist with another
//
// DESCRIPTION:
// Exclude other Retrieve actions.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cStoRetrieve::coexist
(
    STO_ACTION_TYPE
		actionType,	// (in)  The action type tested.
    bool			// (in)  Is the other action a rollback?
)
{
    if ( actionType == STO_AT_RETRIEVE )
    {
	return( false );
    }
    else
    {
	return( true );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoRetrieve::execute
//
// INVOCATION:
// retrieve();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processing for retrieval. 
//
// DESCRIPTION:
// Find the media object in the list of such objects. Set its retrieval 
// state to IN-PROGRESS. If the retrieve option 'size' is in effect, 
// calculate the maximum size to retrieve. Enter an enclosing loop, and
// stay in the loop until retrieving is complete or the action is 
// stopped. Build the list of file objects to retrieve from the database. 
// Iterate through this list, creating get objects to use for retrieving
// the files. the get() method causes the files to be retrieved. The 
// arriving files are handled by the cStoGet class's done() method. 
// Wait until the gets are complete, then either enter a timed-wait 
// or exit. On the way out, update the media object's retrieve state
// back to IDLE.
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

void		cStoRetrieve::execute
(
    void
)
{
    DBPROCESS	*dbProcess;		// Database process. 
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
					// DHS library status. 
    bool	nothingDone;		// A boolean. Might remove it later. 
    bool	done = false;		// A boolean. Might remove it later. 
    cStoMedia	*pMedia;		// Ptr to media object.
    cStoStatus	status;			// Program status.
    int		amStatus;		// am library status. 
    AM_FILE	amFile;			// Am library file structure. 
    cStoFile	*pFile;			// Ptr. to file object.
    cStoGet	*pGet;
    map < string, cStoFile *, less< string > >
		fileList;		// List of files. 
    map < string, cStoFile *, less< string > >::iterator
		iter;			// File list iterator
    cDhsConnection
    		*pConnect( NULL );
    int		maxRetrievalSize = 0;	// max size. 
    char	topLevelDir[PATH_MAX];
    int		totalSize;
    char	*sourceId;


    totalSize = 0;


    //
    // Locate media request object in the list of media requests. 
    // Update EPICS to say that retrieving is IN_PROGRESS
    //

    pMedia = cStoMedia::find( getMediaRequestId(), NULL );
    check( pMedia->setRetrieveState( STO_FST_IN_PROGRESS, status ), 
	    status, return );


    //
    // Calculate maximum size of retrieval.
    //

    maxRetrievalSize = srSize * 1024 * 1024;


    //
    // If user media request, ensure top-level directory exists.
    //

    if ( streq( pMedia->getPAmMedia()->ammMediaRequestType, 
	    AM_RT_USER ) )
    {
	(void) sprintf( topLevelDir, "%s/%d.%s", 
		cStoDbManager::getAmUserRetrievalStage(),
		pMedia->getPAmMedia()->ammMediaRequestId,
		pMedia->getPAmMedia()->ammMediaRequestName );
	if ( ( mkdir( topLevelDir, 
		S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH ) != 0 )
		&& ( errno != EEXIST ) )
	{
	    status( status.E_PATH );
	}
    }


    //
    // Loop forever. Or, until retrieving is completed. 
    //
    
    for ( ; status.ok() ; )
    {
	//
	// Add stuff that is ' ', 'U', or 'E' to the list. If the thing
	// is already in the list, ignore it. 
	//

	nothingDone = true;
	dbProcess = cStoDbManager::acquire();
	pFile = NULL;
	for ( amStatus = amFileGet( AM_FG_RETRIEVABLE, &amFile, 
		dbProcess, pMedia->getMediaRequestId() ); 
		( amStatus == AM_SUCCESS && getStopped() == false );
		amStatus = amFileGet( AM_FG_NEXT, &amFile, dbProcess ) )
	{
	    iter = fileList.find( string( amFile.amfFileId ) );
	    if ( iter == fileList.end() )
	    {
		nothingDone = false;
		checkNull( ( pFile = new cStoFile( &amFile ) ), 
			status, break );
		fileList[string(amFile.amfFileId)] = pFile;
	    }
	}
	cStoDbManager::release();
	if ( amStatus != AM_NO_FILE )
	{
	    checkAm( amStatus, status, break );
	}


	//
	// If list isn't empty & we aren't stopping, start retrieving.
	//
	
	if ( pFile != NULL && !fileList.empty() && !getStopped() )
	{
	    //
	    // Iterate through list.
	    //

	    for ( iter = fileList.begin(); 
		    getStopped() == false && status.ok() && 
		    iter != fileList.end(); 
		    iter++ )
	    {
		pFile = ((*iter).second);


		//
		// If the file has been processed, skip it.
		//

		if ( pFile->getProcessed() )
		{
		    continue;
		}


		//
		// If we've retrieved more than the upper limit, stop.
		//

		if ( maxRetrievalSize > 0 && totalSize > maxRetrievalSize )
		{
		    status.S_RETRIEVE_LIMIT_REACHED( status,  
			    pMedia->getMediaRequestId(), srSize );
		    done = true;
		    break;
		}
		totalSize += pFile->getPAmFile()->amfFileSize;


	       //
	       // Figure out which data server gets the request.
	       //

		if ( streq( pMedia->getPAmMedia()->ammMediaRequestType, 
			AM_RT_USER ) )
		{
		    pConnect = cStoConnection::getDataServerConnection();
		    sourceId = "local data server";
		}
		else
		{
		    if ( strlen( pFile->getPAmFile()->amfSourceId ) == 0 )
		    {
		      sourceId = pMedia->getPAmMedia()->ammSourceId;
		    }
		    else
		    {
		      sourceId = pFile->getPAmFile()->amfSourceId;
		    }


		    if ( pConnect == NULL )
		    {
		      //
		      // If the connection hasn't been made, connect to the
		      // data server.
		      //

		      pConnect = cStoConnection::find( sourceId );
		    }
		    else if ( strcmp( pConnect->name(), sourceId ) != 0 )
		    {
		      //
		      // If the source data server has changed, close the 
		      // current connection and open a connection to the 
		      // new source.
		      //

		      dhsStatus = DHS_S_SUCCESS;
		      pConnect->close( dhsStatus );
		      pConnect = cStoConnection::find( sourceId );
		    }
		}


	       if ( pConnect == NULL )
	       {
		  status.E_UNABLE_TO_CONN( status, sourceId );
		  goto cleanup;
	       }
	       else
	       {
		  dhsStatus = DHS_S_SUCCESS;
		  pConnect->open( dhsStatus );
		  if ( dhsStatus != DHS_S_SUCCESS )
		  {
		     status.E_UNABLE_TO_CONN( status, sourceId );
		     goto cleanup;
		  }
	       }


		//
		// Get the file. 
		//

		nothingDone = false;
		pFile->setProcessed( true );
		checkNull( ( pGet = new cStoGet( pFile->getPAmFile(), 
			pMedia, this ) ), status, break );
		checkDhs( pGet->get( *pConnect, pFile->fileId(), 
			DHS_BD_GT_RAW_ASIS, dhsStatus ),
		dhsStatus, status, delete pGet );
	    }


	    //
	    // If there're gets still going, wait for 'em to complete. 
	    //

	    cStoGet::waitUntilGetsComplete( this );
    

	}


	//
	// If stop flag is set or status is bad, break loop. 
	//

	if ( getStopped() || !status.ok() || pFile == NULL || done )
	{
	    break;
	}
	break;
    }


cleanup:
   //
   // Close connection to dataserver.
   //

   if ( pConnect != NULL )
   {
      pConnect->close( dhsStatus );
   }


    //
    // Get rid of file list. 
    //

    for ( iter = fileList.begin();
	    fileList.size() != 0;
	    iter = fileList.begin() )
    {
	pFile = ((*iter).second);
	delete( pFile );
	fileList.erase( iter );
    }


    //
    // Update EPICS to say that retrieving is IDLE.
    //

    pMedia->setRetrieveState( STO_FST_IDLE, status );


    return;
}
