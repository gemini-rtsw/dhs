static char rcsid[] = "$Id: stoMedia.C,v 1.1.1.1 2002-11-24 20:32:05 brighton Exp $";
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
// dhsStorage/src/stoMedia.C
//
// PURPOSE:
// Contains methods for cStoMedia class. 
//
// METHOD NAME(S)
// cStoMedia::cStoMedia		- Constructor.
// cStoMedia::~cStoMedia	- Deconstructor.
// cStoMedia::init		- Initializes list of media objects.
// cStoMedia::findById		- Find a media request object given the request
//				  id.
// cStoMedia::findByName	- Find a media request object given the request
//				  name.
// cStoMedia::selectEpicsRow	- Pick a row in the Epics Array.
// cStoMedia::setPAmMedia	- Set ptr to am libraray media struct.
// cStoMedia::db2Stats		- Query database for unit object stat values.
// cStoMedia::exit		- Nuke list, release resources held by class.
// cStoMedia::getActionState	- Get state of a media object action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.12  2000/12/19 19:34:42  nhill
// Updated for Workshop verison 5.
//
// Revision 1.11  1999/10/12 23:00:28  nhill
// Added method cStoMedia::findById.
//
// Revision 1.10  1999/06/21 19:57:37  nhill
// Removed a debugging sleep.
//
// Revision 1.9  1999/06/18 22:30:31  nhill
// Improved error handling when media type is not found in the database.
//
// Revision 1.8  1999/06/16 23:58:40  nhill
// Major update.
//
// Revision 1.7  1999/02/25 00:05:14  jaeger
// Altered init() instead of assuming no media request exit it updates
// existing media request with db info and deletes thos which were not
// found in the database.  Updated to use new AM library.
//
// Revision 1.6  1998/07/31 21:05:24  cockayne
// In an attempt to fix a 'glitch' in communications between the Storage Server,
// Status Server, and Console, Moved the contructor of the "in-use" flag to
// occur later (time-wise) than previously. The 'glitch' still occurs.
// The 'glitch' occurs when a new user media request command arrives.
// The Console complains about an invalid value of a status record, but if the
// Status Server is flushed, all values appear OK. Not sure how to fix this.
//
// Revision 1.5  1998/05/15 17:11:03  cockayne
// Modifications to allow Epics overflow processing, and memory error checking.
//
// Revision 1.4  1998/04/20 20:55:55  cockayne
// Modified deconstructor to handle default-constructed objects correctly.
// Added findByName() method. Added casting to take care or integer
// overflow problem.
//
// Revision 1.3  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoAction.H"
#include "stoDhs.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoMedia.H"


//
// Define the static data in the cStoMedia class.
//

bool		cStoMedia::smEpicsFreeList[STO_MAX_MEDIA_EPICS_ARRAY];
						// The list of free/non-free
						// EPICS media array rows.
cStoEpicsRecord<bool, cStoMedia, cStoStatus>	
		cStoMedia::smEpicsOverflow( STO_ERM_REQ_OVERFLOW, false, NULL, NULL );
						// true if EPICS is overflowed.
cStoEpicsRecord<int, cStoMedia, cStoStatus>	
		cStoMedia::smNumReqs( STO_ERM_NUM_REQS, 0, NULL, NULL );
						// Number of requests. 
cStoEpicsRecord<int, cStoMedia, cStoStatus>	
		cStoMedia::smArraySize( STO_ERM_REQ_ARRAY_SIZE, STO_MAX_MEDIA_EPICS_ARRAY, NULL, NULL );
						// Number of requests. 

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::cStoMedia
//
// INVOCATION:
// cStoMedia( pAmMedia, addToList );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmMedia  (AM_MEDIA *) - Pointer to am media struct.
// (>) addToList (bool)     - Boolean telling whether to add object to list.
// 
// FUNCTION VALUE:
// (cStoMedia) new media object.
// 
// PURPOSE:
// Constructor for cStoMedia objects.
// 
// DESCRIPTION:
// Construct the object, adding it to the list of such objects, and 
// initializing all class data, including getting a row in the Epics
// array, saving a copy of the am library media struct, and initializing
// the Epics records.
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

		cStoMedia::cStoMedia
(
    AM_MEDIA	*pAmMedia,
    bool	addToList	// Set to true if we're to add to the list.
)
:   cListOf< cStoMedia, int, cStoStatus >( pAmMedia->ammMediaRequestId, 
	    addToList ),

    //
    // Set EPICS array row, set am library media struct pointer. 
    //

    smEpicsArrayRow( selectEpicsRow() ),
    smPAmMedia( NULL ),

    
    //
    // Set the am library fields which are stored in EPICS. 
    //

    smMediaReqId( smEpicsArrayRow, STO_ERM_REQUEST_ID, this, NULL ),
    smMediaReqName( smEpicsArrayRow, STO_ERM_REQUEST_NAME, this, NULL ),
    smMediaType( smEpicsArrayRow, STO_ERM_MEDIA_TYPE, this, NULL ),
    smRequestType( smEpicsArrayRow, STO_ERM_REQUEST_TYPE, this, NULL ),
    smNumCopies( smEpicsArrayRow, STO_ERM_NUM_COPIES, this, 0 ),


    //
    // Set the statistics which are stored in EPICS to UNKNOWN. 
    //
    
    smRetrieveSize( smEpicsArrayRow, STO_ERM_AMOUNT_RETRIEVABLE, 
	    this, &cStoMedia::bytes2Megs ),
    smQueueSize( smEpicsArrayRow, STO_ERM_AMOUNT_QUEUEABLE, 
	    this, &cStoMedia::bytes2Units ),
    smPrepNum( smEpicsArrayRow, STO_ERM_NUM_PREPARABLE, 
	    this, NULL ),
    smWriteNum( smEpicsArrayRow, STO_ERM_NUM_WRITEABLE, 
	    this, NULL ),
    smCleanupNum( smEpicsArrayRow, STO_ERM_NUM_CLEANUPABLE, 
	    this, NULL ),
    smNotify( smEpicsArrayRow, STO_ERM_NOTIFIABLE, 
	    this, &cStoMedia::bool2str ),


    //
    // Set the fivestates which are stored in EPICS.
    //

    smRetrieveState( smEpicsArrayRow, STO_ERM_RETRIEVE_STATE, 
	    cStoFiveState::idle(), this, NULL ),
    smQueueState( smEpicsArrayRow, STO_ERM_QUEUE_STATE, 
	    cStoFiveState::idle(), this, NULL ),
    smPrepareState( smEpicsArrayRow, STO_ERM_PREPARE_STATE, 
	    cStoFiveState::idle(), this, NULL ),
    smWriteState( smEpicsArrayRow, STO_ERM_WRITE_STATE, 
	    cStoFiveState::idle(), this, NULL ),
    smCleanupState( smEpicsArrayRow, STO_ERM_CLEANUP_STATE, 
	    cStoFiveState::idle(), this, NULL ),
    smNotifyState( smEpicsArrayRow, STO_ERM_NOTIFY_STATE, 
	    cStoFiveState::idle(), this, NULL ),
    smEpicsRecInUse( smEpicsArrayRow, STO_ERM_IN_USE, 
	    true, this, NULL ),
    smRefCount( 1 )
{
    setPAmMedia( pAmMedia );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::~cStoMedia
//
// INVOCATION:
// ~cStoMedia();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor for cStoMedia class.
//
// DESCRIPTION:
// Free memory allocated for the object. Set Epics records so the
// media request is no longer displayed in the console, and then check if
// the array was previously overflowing and handle that too.
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

		cStoMedia::~cStoMedia
(
    void
)
{
    cStoMedia::iObjectList
    		i;		// Media list iterator. 
    cStoStatus	status;		// Program status.
    cStoMedia	*pMedia;
    cStoMedia::iObjectList
                mIter;          // Iterator for the node list. 
    bool	overflow;


    if ( smPAmMedia->ammMediaRequestId == -1 )
    {
	gen_free( smPAmMedia );
	smPAmMedia= NULL;
	return;
    }

    gen_free( smPAmMedia );
    smPAmMedia= NULL;
    

    //
    // Set the epics free list entry to false (not in use). Set inUse 
    // flag so this EPICS record array row is not displayed. return.
    //

    smEpicsFreeList[smEpicsArrayRow] = false;
    smNumReqs = smNumReqs.value() - 1;
    smEpicsRecInUse = false;


    //
    // Must free memory allocated into char * epics records. 
    //

    gen_free( smMediaReqName.value() );
    gen_free( smMediaType.value() );
    gen_free( smRequestType.value() );



    
    //
    // Check if Epics Array was overflowing before this object was deleted.
    //

    for ( mIter = cStoMedia::begin(); 
            mIter != cStoMedia::end(); 
            mIter++ )
    {
	pMedia = (*mIter).second;
	if ( pMedia->getEpicsArrayRow() == -1 )
	{
	    //
	    // Set EPICS array row, cause EPICS update. decrement NumReqs,
	    // See if we can un-set Overflow. 
	    //
	
	    smEpicsFreeList[smEpicsArrayRow] = true;
	    pMedia->flushAll( smEpicsArrayRow );
	    pMedia->smEpicsRecInUse = true;
	    break;
	}
    }

    overflow = false;
    for ( mIter = cStoMedia::begin(); 
            mIter != cStoMedia::end(); 
            mIter++ )
    {
	pMedia = (*mIter).second;
	if ( pMedia->getEpicsArrayRow() == -1 )
	{
	    overflow = true;
	    break;
	}
    }

    smEpicsOverflow = overflow;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::init
//
// INVOCATION:
// init( status );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status   (cStoStatus &)  - Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the list of unit objects.
//
// DESCRIPTION:
// In a loop, fetch all am library media structs from the database. 
// For each one, construct a new cStoMedia object and add it to the list.
// When done creating new objects, update the statistics using forEach()
// and the db2Stats() method. 
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

void		cStoMedia::init
(
    cStoStatus	&status		// (mod) Program status.
)
{
    AM_MEDIA	amMedia;	// CADC am library media structure. 
    int		amStatus;	// CADC am library return status. 
    DBPROCESS	*dbProcess;	// Database process. 
    cStoMedia::iObjectList
                mIter;          // Iterator for the node list. 
    cStoMedia::tObjectList	// A list of media to be deleted.
    		mediaList;
    cStoMedia	*pMedia;	// Ptr to media object.
    
    checkStat( status, return );

    
    smNumReqs = smNumReqs.value();
    smEpicsOverflow = smEpicsOverflow.value();
    smArraySize = smArraySize.value();


    //
    // Initialize all inuse flags to false.
    //

    cStoMedia::listRLock();
    for ( mIter = cStoMedia::begin(); mIter != cStoMedia::end(); mIter++ )
    {
	pMedia = ((*mIter).second);
	mediaList[ pMedia->getMediaRequestId()] = pMedia;
	pMedia->smInUse = false;
    }
    cStoMedia::listUnlock();
    
    
    //
    // In a loop, fetch all db media rows worth reporting thru EPICS.
    //

    dbProcess = cStoDbManager::acquire();
    amStatus = AM_SUCCESS; 
    for ( amStatus = amGet( AM_MG_STATUS, &amMedia, dbProcess, 'I' ); 
	    status.ok() && amStatus == AM_SUCCESS; 
	    amStatus = amGet( AM_MG_NEXT, &amMedia, dbProcess ) )
    {
	if ( ( pMedia = findById( amMedia.ammMediaRequestId ) ) == NULL )
	{
	    //
	    // New media object. Add to list. Update info in EPICS. 
	    //
		
	    checkNull( ( pMedia = new cStoMedia( &amMedia ) ), status, VOID );
        }
	else
	{
	    checkNull( pMedia->setPAmMedia( &amMedia ), status, VOID );
	}
	pMedia->smInUse = true;
    }
    cStoDbManager::release();
    if ( amStatus < 0 )
    {
	checkAm( amStatus, status, return );
    }


    //
    // Delete all unused media.
    //

    for ( mIter = mediaList.begin(); mIter != mediaList.end(); mIter++ )
    {
	pMedia = ((*mIter).second);
	if ( pMedia->smInUse == false )
	{
	    pMedia->deref();
	}
    }


    forEach( &cStoMedia::db2Stats, NULL, status );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::findById
//
// INVOCATION:
// pMedia = cStoMedia::findById( mediaId );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaId (int) the media id to find.
//
// FUNCTION VALUE:
// (cStoMedia *) The media matching the request id.
//
// PURPOSE:
// Find a cStoMedia request matching the media request id
//
// DESCRIPTION:
// Loop through the media requests looking for one matching the ID.
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

cStoMedia	*cStoMedia::findById
(
    int		mediaId		// (in)  The media request id.
)
{
    cStoMedia::iObjectList
                mIter;			// Iterator for the node list. 
    cStoMedia	*pMedia;		// Ptr to node object.
    
 
    cStoMedia::listRLock();
    for ( mIter = cStoMedia::begin(); mIter != cStoMedia::end(); mIter++ )
    {
	pMedia = ((*mIter).second);
        if ( pMedia->getMediaRequestId() == mediaId )
        {
	    cStoMedia::listUnlock();
            return( pMedia );
        }
    }
    cStoMedia::listUnlock();
 
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::findByName
//
// INVOCATION:
// pMedia = cStoMedia::findByName( name );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (char *) name	The name to search for. Null if none found.
//
// FUNCTION VALUE:
// (cStoMedia *) The media matching the name.
//
// PURPOSE:
// Find a cStoMedia object given a media request name.
//
// DESCRIPTION:
// Loop through the media requests looking for one matching the name.
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

cStoMedia	*cStoMedia::findByName
(
    char 	*name
)
{
    cStoMedia::iObjectList
                mIter;			// Iterator for the node list. 
    cStoMedia	*pMedia;		// Ptr to node object.
    
 
    cStoMedia::listRLock();
    for ( mIter = cStoMedia::begin(); mIter != cStoMedia::end(); mIter++ )
    {
	pMedia = ((*mIter).second);
        if ( streq( pMedia->getMediaRequestName(), name ) )
        {
	    cStoMedia::listUnlock();
            return( pMedia );
        }
    }
    cStoMedia::listUnlock();
 
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::selectEpicsRow
//
// INVOCATION:
// selectEpicsRow();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (int) The row in Epics to assign to a new unit object. Returns -1 if
//       there are no free rows in the Epics array.
//
// PURPOSE:
// Picks a row in the Epics array.
//
// DESCRIPTION:
// Loop through the Epics array of free rows, looking for a free one. If
// found, return its index. If not found, the overflow flag is set, 
// and the Storage Server's health is set to warning. 
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

int		cStoMedia::selectEpicsRow
(
    void
)
{
    int		num;		// Loop counter. 
    cStoStatus	status;


    //
    // Loop through the list of EPICS array rows, try to find a free one.
    //

    smNumReqs = smNumReqs.value() + 1;
    for ( num = 0; num < STO_MAX_MEDIA_EPICS_ARRAY; num++ )
    {
	if ( smEpicsFreeList[num] == false )
	{
	    smEpicsFreeList[num] = true;
	    return( num );
	}
    }


    //
    // If get to here, must have full EPICS array. Set health to WARNING.
    //

    status.E_REQUEST_OVERFLOW( status );


    smEpicsOverflow = true;

    return( -1 );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::setPAmMedia
//
// INVOCATION:
// media.setPAmMedia( pAmMedia );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmMedia  (AM_MEDIA *) - Ptr to am library media struct.
//
// FUNCTION VALUE:
// (AM_MEDIA *) - Ptr to saved am library media struct.
//
// PURPOSE:
// Save a copy of the am library media struct in the class data. 
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

AM_MEDIA	*cStoMedia::setPAmMedia
(
    AM_MEDIA	*pAmMedia	// (in)  Ptr to am library media structure.
)
{
    AM_MEDIA	*ptr;		// Ptr to am media struct. 


    //
    // Lock object, allocate space & save copy of am library media structure.
    //

    lock();
    ptr = (AM_MEDIA *) gen_alloc( sizeof( AM_MEDIA ) );
    if ( ptr != NULL )
    {
	gen_cpy( ptr, pAmMedia, sizeof( AM_MEDIA ) );
    }


    if ( smPAmMedia != NULL )
    {
	gen_free( smPAmMedia );
    }

    smPAmMedia = ptr;


    //
    // Must free memory allocated into char * epics records. 
    //

    if ( smMediaReqName.valueKnown() )
    {
	gen_free( smMediaReqName.value() );
    }
    if ( smMediaType.valueKnown() )
    {
	gen_free( smMediaType.value() );
    }
    if ( smRequestType.valueKnown() )
    {
	gen_free( smRequestType.value() );
    }


    //
    // Set the am library fields which are stored in EPICS. 
    //

    smMediaReqId = pAmMedia->ammMediaRequestId;
    smMediaReqName = strsav( pAmMedia->ammMediaRequestName );
    smMediaType = strsav( pAmMedia->ammMediaType );
    smRequestType = strsav( pAmMedia->ammMediaRequestType );
    smNumCopies = pAmMedia->ammNumCopies;


    //
    // Unlock media object, return pointer to allocated stuff. 
    //

    unlock();


    return( ptr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::db2Stats
//
// INVOCATION:
// n/a invoked only by the forEach() method.
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) ---      (void *)        - Could be ptr to anything. Ignored here. 
// (!) status   (cStoStatus &)  - Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Update the statistics for the cStoMedia class.
//
// DESCRIPTION:
// This method is called for each object in the media object list. For each
// object, query the database for the media request statistics, and update 
// the class data.
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

void		cStoMedia::db2Stats
(
    void	*,		// ()    Dummy variable. 
    cStoStatus	&status		// (mod) Program status.
)
{
    DBPROCESS	*dbProcess;	// Database process. 
    boolean	dbBooleanValue;	// Boolean return value from the database.
    float	dbFloatValue;	// Float return value from the database.
    int		dbIntValue;	// Integer return value from the database.
    AM_MED_TYPE	amMedType;	// 
    int		amStatus;	// Return value from the am library.
    		


    checkStat( status, return );


    //
    // Size of the units this media request will produce. 
    //

    

    dbProcess = cStoDbManager::acquire();
    checkAm( amStatus = amMediaTypeGet( AM_TG_TYPE, &amMedType, dbProcess,
	   smPAmMedia->ammMediaType ), status, VOID );
    if ( amStatus == AM_NO_TYPE )
    {
	status.E_NO_MEDIA_TYPE( status, smPAmMedia->ammMediaType ); 
	status( status.S_SUCCESS );
	smUnitSize = -1;
    }
    else
    {
	smUnitSize = amMedType.amtMaxSize;
    }
    cStoDbManager::release();

    //
    // Amount Retrievable
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amAmountRetrievable( dbProcess, smPAmMedia->ammMediaRequestId, 
	    &dbFloatValue ), status, VOID );
    smRetrieveSize = (long long) dbFloatValue;
    cStoDbManager::release();
    


    //
    // Amount Queueable
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amAmountQueueable( dbProcess, smPAmMedia->ammMediaRequestId, 
	    &dbFloatValue ), status, VOID );
    smQueueSize = (long long) dbFloatValue;
    cStoDbManager::release();


    //
    // Num Preparable
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amNumPreparable( dbProcess, smPAmMedia->ammMediaRequestId, 
	    &dbIntValue ), status, VOID );
    smPrepNum = dbIntValue;
    cStoDbManager::release();


    //
    // Num Writable
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amNumWriteable( dbProcess, smPAmMedia->ammMediaRequestId, 
	    &dbIntValue ), status, VOID );
    smWriteNum = dbIntValue;
    cStoDbManager::release();


    //
    // Num Cleanupable
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amNumCleanupable( dbProcess, smPAmMedia->ammMediaRequestId, 
	    &dbIntValue ), status, VOID );
    smCleanupNum = dbIntValue;
    cStoDbManager::release();


    //
    // Notifiable
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amNotifiable( dbProcess, smPAmMedia->ammMediaRequestId, 
	    smPAmMedia->ammMediaRequestType, &dbBooleanValue ), 
	    status, VOID );
    smNotify = ( ( dbBooleanValue == TRUE ) ? true : false );
    cStoDbManager::release();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::exit
//
// INVOCATION:
// exit( status );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &)	- Program Status. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Exit method for the cStoMdia class. 
//
// DESCRIPTION:
// Delete all media objects from the list. 
//
// EXTERNAL VARIABLES:
//
// PRIOR REQUIREMENTS:
//
// DEFICIENCIES:
//-
//***********************************************************************
//

void		cStoMedia::exit
(
    cStoStatus	&status		// (mod) Program status.
)
{
    checkStat( status, return );


    //
    // For each media object in the list, nuke it. 
    //

    deleteAll();

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::getActionState
//
// INVOCATION:
// getActionState( actionType );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) actionType   (STO_ACTION_TYPE) - Action type of which to check status.
//
// FUNCTION VALUE:
// (STO_FIVE_STATE_TYPE) Action state.
//
// PURPOSE:
// Check the state of the action.
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

STO_FIVE_STATE_TYPE	cStoMedia::getActionState
(
    STO_ACTION_TYPE	actionType	// (in)  Action type to check status.
)
{
    switch( actionType )
    {
	case STO_AT_RETRIEVE:
	    return( smRetrieveState.value().getState() );
	    break;

	case STO_AT_QUEUE:
	    return( smQueueState.value().getState() );
	    break;

	case STO_AT_PREPARE:
	    return( smPrepareState.value().getState() );
	    break;

	case STO_AT_WRITE:
	    return( smWriteState.value().getState() );
	    break;

	case STO_AT_CLEANUP:
	    return( smCleanupState.value().getState() );
	    break;

	case STO_AT_NOTIFY:
	    return( smNotifyState.value().getState() );
	    break;

	case STO_AT_NONE:
	default:
	    return( STO_FST_IDLE );
	    break;
    }
}

	char	*cStoMedia::bytes2Megs( long long l )
			{
			    sprintf( smB2MString, "%5.2f",
				    (float) ( l + ( 0.005 * 1024 * 1024 - 1 ) )
				    / ( 1024 * 1024 ) );
			    return( smB2MString );
			};

