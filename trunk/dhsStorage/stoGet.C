static char rcsid[] = "$Id: stoGet.C,v 1.3 2003-01-15 18:23:35 brighton Exp $";
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
// dhsStorage/src/stoGet.C
//
// PURPOSE:
// Contains methods for cStoGet class. 
//
// METHOD NAME(S)
// cStoGet::cStoGet	- Constructor.
// cStoGet::~cStoGet	- Deconstructor
// cStoGet::waitUntilGetsComplete - Blocks until all outstanding gets return.
// cStoGet::setPAmFile	- Sets pointer to am library file struct.
// cStoGet::done	- Executed by dhs library when file is here. 
// cStoGet::execThread	- pthread_create() exec-ed method to take care of file.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/27 17:15:09  brighton
// ported to gcc-3.2.1/linux
//
// Revision 1.1.1.1  2002/11/24 20:32:02  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.13  2001/03/12 20:35:09  nhill
// Added type checking on command input parameters.
//
// Revision 1.12  2000/08/22 19:28:44  nhill
// Fixed a bug which caused file names to be wrong when data is retrieved
// through a get.
//
// Revision 1.11  1999/10/12 22:59:46  nhill
// Re-organized some of the code.
//
// Revision 1.10  1999/07/08 21:32:44  nhill
// Fixed a bug where the file id wasn't displayed correctly when the get
// returned an error.
//
// Revision 1.9  1999/06/16 23:58:17  nhill
// Major update.
//
// Revision 1.8  1998/06/24 16:02:49  cockayne
// Added code to handle compression/uncompression.
//
// Revision 1.7  1998/05/15 17:09:23  cockayne
// Many modifications, to support compression/uncompression.
//
// Revision 1.6  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.5  1998/04/20 20:39:19  cockayne
// Fixed bug - was not properly casting to 'long long' where applicable.
//
// Revision 1.4  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 21:01:43  cockayne
// Modified definition of done() method to match new dhs++ library.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


extern "C"
{
#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_config.h"
#include "db.h"
}

#define DHS_NO_COMMAND
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoActionType.H"
#include "stoDbm.H"
#include "stoFile.H"
#include "stoFiveState.H"
#include "stoMedia.H"
#include "stoGet.H"


//
// Define static data members of the cStoGet class. 
//

int		cStoGet::sgCount( 0 );
cCond		cStoGet::sgCond;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoGet::cStoGet
//
// INVOCATION:
// cStoGet( pAmFile, pMedia );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmFile	(AM_FILE *)	- Pointer to am library file struct. 
// (>) pMedia	(cStoMedia *)	- Pointer to media object.
// (>) pAction	(cStoMedia *)	- The action being processed.
//
// FUNCTION VALUE:
// (cStoGet) new cStoGet object.
//
// PURPOSE:
// Create a new cStoGet object. 
//
// DESCRIPTION:
// Save pointer to am library file struct. Save pointer to media object.
// lock(), increment outstanding gets counter, and unlock(). If we hit the
// max number of gets, wait until the number drops.
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

		cStoGet::cStoGet
(
    AM_FILE	*pAmFile,	// Ptr to am library file struct.
    cStoMedia	*pMedia,	// Ptr to media object.
    cStoRetrieve	
		*pAction	// Ptr to action object.
)
:   sgPAmFile( setPAmFile( pAmFile ) ),
    sgPMedia( pMedia ),
    sgPAction( pAction ),
    sgString( NULL )
{
    //
    // Lock, increment counter, then wait until counter comes down below max. 
    //
    
    sgCond.lock();
    sgCount++;
    pAction->getCount( pAction->getCount() + 1 );
    sgPAvList = NULL;
    
    //
    // If the total number of active gets, or the number of active gets for
    // this node are exeeded, wait until a get is completed.
    //

    while ( sgCount > STO_GET_MAX || pAction->getCount() > STO_ACTION_GET_MAX )
    {
	sgCond.wait();
    }
    sgCond.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoGet::~cStoGet
//
// INVOCATION:
// ~cStoGet();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstruct cStoGet object.
//
// DESCRIPTION:
// Free memory used by the object, and lock(), decrement counter, unock(),
// then broadcast to let threads waiting on the condition variable continue.
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

		cStoGet::~cStoGet
(
    void
)
{
    //
    // Free memory allocated in constructor. Do not free pointer to media obj.
    //

    gen_free( sgPAmFile );


    //
    // Lock, decrement count, then broadcast that count's been decremented.
    //
    
    sgCond.lock();
    sgCount--;
    sgPAction->getCount( sgPAction->getCount() - 1 );
    sgCond.unlock();
    sgCond.broadcast();
    if ( sgPAvList != NULL )
    {
    	delete sgPAvList;
    }

    if ( sgString != NULL )
    {
	gen_free( sgString );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoGet::waitUntilGetsComplete
//
// INVOCATION:
// waitUntilGetsComplete();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Blocks calling thread until all outstanding gets for this action have
// completed.
//
// DESCRIPTION:
// lock, and wait on condition that number of gets is greater than 0.
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

void		cStoGet::waitUntilGetsComplete
(
    cStoRetrieve		// The action being processed.
    		*pAction
)
{
    //
    // Wait for all gets for this action are destroyed, so count is 0.
    //
    
    sgCond.lock();
    while( pAction->getCount() > 0 )
    {
	sgCond.wait();
    }
    sgCond.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoGet::setPAmFile
//
// INVOCATION:
// setPAmFile( pAmFile );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmFile	(AM_FILE *)  - Ptr to am library file struct.
//
// FUNCTION VALUE:
// (AM_FILE *) Ptr to saved am library file struct. 
//
// PURPOSE:
// Allocate memory and save a copy of the am library file struct.
//
// DESCRIPTION:
// Pretty basic. 
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

AM_FILE		*cStoGet::setPAmFile
(
    AM_FILE	*pAmFile	// Ptr to am library file struct.
)
{
    AM_FILE	*ptr;		// Ptr to am library file struct.


    //
    // Allocate space and save a copy of am library file structure.
    //

    ptr = (AM_FILE *) gen_alloc( sizeof( AM_FILE ) );
    if ( ptr != NULL )
    {
	gen_cpy( ptr, pAmFile, sizeof( AM_FILE ) );
    }


    //
    // Return pointer to allocated stuff.
    //

    return( ptr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoGet::done
//
// INVOCATION:
// n/a method executed directory from dhs library on completion of get method.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS ) - Command status.
// (>) returnedString (char *)	- String returned.
// (>) ---	(cDhsAvList &)	- AvList returned (currently ignored)
// 
// FUNCTION VALUE:
// None.
// 
// PURPOSE:
// Save string and command status, execute thread to handle file.
// 
// DESCRIPTION:
// Use the POSIX threads library to execute the execThread() method. 
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

void		cStoGet::done
(
    DHS_CMD_STATUS	
		cmdStatus, 	// (in)  Command status.
    char 	*returnedString,	
    				// (in)  String returned.
    cDhsAvList	&returnedAvList	// (in)  AvList returned 
)
{
    cStoStatus	status;		// Program status. 
    pthread_attr_t
    		attr;           // POSIX thread attributes.
    pthread_t   thread;         // XXX allan: added
    int		pthread_status;	// Return status from pthread_create().


    //
    // If command is in final state, spawn thread to save file. 
    //

    if ( cmdStatus != DHS_CS_BUSY )
    {
	//
	// Save command's status, string and a-v list in class data.
	//
	
	sgStatus = cmdStatus;
	if ( returnedString != NULL )
	{
	    checkNull( ( sgString = strsav( returnedString ) ),
		    status, VOID );
	}
	if ( !returnedAvList.isNull() )
	{
	    checkNull( ( sgPAvList = new cDhsAvList( returnedAvList ) ),
		    status, VOID );
	}


	//
	// Spawn a thread to save file, decompress, update db & EPICS. 
	//
	
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
	pthread_status = pthread_create( &thread, &attr, execThread, this );
	if ( pthread_status != 0 )
	{
	    status.E_PTHREAD( status );
	}
	pthread_attr_destroy( &attr );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoGet::execThread
//
// INVOCATION:
// n/a invoked by the pthread_create() routine. 
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg   (void *) - Argument. In this case it's a pointer to a get object.
// 
// FUNCTION VALUE:
// (void *) NULL
// 
// PURPOSE:
// Handles the file returned by the get() method. 
// 
// DESCRIPTION:
// If command status is an error, determine whether this is an unavailable
// file or an errored file. If the command status is DONE, save the returned
// filename. If necessary, create the path to the file's location. Write
// the file. Deal with compression (not done at time of documentation). 
// Update the database. Update media object statistics. delete the get 
// object.
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

void		*cStoGet::execThread
(
    void	*arg
)
{
    cDhsAttrib	attribute;
    char	compressionFlag;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// DHS library status. 
    int		i;
    bool	loopDone(false);// Set to true when we want to break loop.
    int		ndims;		// Number of dimensions of an attribute.
    DHS_DATA_TYPE		// Data type of an attribute.
    		type;
    AM_FILE	oldAmFile;	// The am file structure before modification.
    cStoGet	*pGet;		// Ptr to get object. 
    void	*pData;
    cStoStatus	status;		// Program status. 
    
    char*       dummy;          // dummy arg for attribute.info() (XXX allan: added)


    pGet = (cStoGet *) arg;

    
    //
    // Save the existing AM file structure so we can delete the row in
    // am table later.
    //

    memcpy( &oldAmFile, pGet->getPAmFile(), sizeof( AM_FILE ) );

    if ( pGet->getStatus() == DHS_CS_ERROR || 
	    pGet->getStatus() == DHS_CS_ABORTED ||
	    pGet->getStatus() == DHS_CS_LOST )
    {
	status.E_FILE_GET( status, 
		    pGet->getPAmFile()->amfFileId,
		    pGet->getPMedia()->getPAmMedia()->ammMediaRequestId );
	status( status.S_SUCCESS );


	//
	// Failure status is tracked as follows. If the DHS denies knowledge
	// of the file ID, it is an ERROR. Otherwise, the file is 
	// UNAVAILABLE.
	//

	if ( pGet->getString() != NULL && 
		strstr( pGet->getString(), "locally or remotely" ) != NULL )
	{
	    pGet->getPAmFile()->amfStatus = AMF_UNAVAILABLE;
	}
	else
	{
	    pGet->getPAmFile()->amfStatus = AMF_ERROR;
	}
	gen_free( pGet->sgString );
	pGet->sgString = NULL;
    }
    else if ( pGet->getStatus() == DHS_CS_DONE )
    {
	//
	// Loop exists only so we can say 'break' and skip stuff.
	//
	
	for ( i = 0; i < 1; i++ )
	{
	    if ( streq ( pGet->getPAmFile()->amfFilename, "" ) )
	    {
		//
		// Get filename from a-v list. Add to am library struct.
		// Get compression here, too.
		//

		attribute = pGet->getAvList()->find( STO_CA_FILENAME, 
			dhsStatus );
		checkDhs( attribute.info( dummy, type, ndims, NULL, 
			(void **) &pData, dhsStatus ), dhsStatus, status, 
			break );
		if ( type == DHS_DT_STRING && ndims == 0 )
		{
		    strncpy( pGet->getPAmFile()->amfFilename, (char *) pData,
			    AM_FILE_NAME_LEN );
		    pGet->getPAmFile()->amfFilename[ AM_FILE_NAME_LEN - 1 ] = 
			    '\0';
		}
		else
		{
		    status.E_PARAM_TYPE( status, "dhsBdGet", STO_CA_FILENAME );
		    break;
		}
	    }


	    //
	    // Get the file compression state.
	    //

	    attribute = pGet->getAvList()->find( STO_CA_COMPRESSION, 
		    dhsStatus );
	    checkDhs( attribute.info( dummy, type, ndims, NULL, 
		    (void **) &pData, dhsStatus ), dhsStatus, status, break );
	    if ( type == DHS_DT_STRING && ndims == 0 )
	    {
		compressionFlag = *(char *) pData;
	    }
	    else
	    {
		status.E_PARAM_TYPE( status, "dhsBdGet", STO_CA_COMPRESSION );
		break;
	    }

	    checkDhs( VOID, dhsStatus, status, break );


	    cStoFile	stoFile( pGet->getPAmFile() );
	    stoFile.save( *pGet, pGet->label(), compressionFlag, oldAmFile, 
		    status );
	}
    }

    delete pGet;


    return( NULL );
}
