static char rcsid[] = "$Id: stoStage.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoStage.C
//
// PURPOSE:
// Contains methods of the cStoStage class. 
//
// METHOD NAME(S)
// cStoStage::cStoStage		- Constructor
// cStoStage::~cStoStage	- Destructor
// cStoStage::init		- Initialize list of stage directories.
// cStoStage::find		- Find a directory by req. ID. and req. name.
// cStoStage::findByDirectory	- Find a directory by dir. name.
// cStoStage::selectEpicsRow	- Pick a row in the epics array.
// cStoStage::setPAmStage	- Set pointer to am stage struct.
// cStoStage::exit		- Delete list, releasing resources held
//    				  by the class.
// cStoStage::clear		- Don't show stages in EPICS
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:26  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  1999/06/17 00:00:33  nhill
// Major re-organization.
//
// Revision 1.5  1999/02/24 23:59:47  jaeger
// Added find(), findByDirectory(), and flush() methods.
// Also altered init() instead of assuming no stages exists it updates
// existing stages with db info and delete those which were not found in
// the database.
//
// Revision 1.4  1998/05/15 17:13:45  cockayne
// Modifications for program flow and memory error checking.
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

#include <iostream>
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

#define DHS_NO_COMMAND
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
#include "stoStage.H"


//
// Define the static data in the cStoStage class.
//

bool		cStoStage::ssEpicsFreeList[STO_MAX_STAGE_EPICS_ARRAY];
						// The list of free/non-free
						// EPICS stage array rows.
						// Number of requests. 

cStoEpicsRecord<int, cStoStage, cStoStatus>	
		cStoStage::ssArraySize( STO_ERS_ARRAY_SIZE, STO_MAX_STAGE_EPICS_ARRAY, NULL, NULL );
						// Number of requests. 


//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStage::cStoStage
//
// INVOCATION:
// cStoStage( pAmStage, addToList );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmStage (AM_MSTAGE *) - Pointer to am media stage struct.
// (>) addToList (bool)     - Boolean telling whether to add object to list.
// 
// FUNCTION VALUE:
// (cStoStage) new stage object.
// 
// PURPOSE:
// Constructor for cStoStage objects.
// 
// DESCRIPTION:
// Construct the object, adding it to the list of such objects, and 
// initializing all class data, including getting a row in the Epics
// array, saving a copy of the am library stage struct, and initializing
// the Epics records. 
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

		cStoStage::cStoStage
(
    void
)
:   cListOf< cStoStage, int, cStoStatus >( selectEpicsRow() ),

    //
    // Set EPICS array row, set am library stage struct pointer. 
    //

    ssEpicsArrayRow( index() ),

    
    //
    // Set the am library fields which are stored in EPICS. 
    //

    ssEpicsRecInUse( ssEpicsArrayRow, STO_ERS_IN_USE, false, this, NULL ),
    ssMediaUnitName( ssEpicsArrayRow, STO_ERS_UNIT_NAME, (char *) NULL, this, NULL ),
    ssMediaRequestId( ssEpicsArrayRow, STO_ERS_REQUEST_ID, -1, this, NULL ),
    ssDirectory( ssEpicsArrayRow, STO_ERS_DIR_NAME, (char *) NULL, this, NULL ),
    ssPAmStage( NULL )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStage::~cStoStage
//
// INVOCATION:
// ~cStoStage();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor for cStoStage class.
//
// DESCRIPTION:
// Free memory allocated for the object. Set Epics records so the
// stage is no longer displayed in the console, and then check if 
// the array was previously overflowing and handle that too. 
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

		cStoStage::~cStoStage
(
    void
)
{
    cStoStage::iObjectList
    		i;		// Stage list iterator. 
    cStoStatus	status;		// Program status.


    if ( ssPAmStage != NULL )
    {
	gen_free( ssPAmStage );
    }

    
    //
    // Set the epics free list entry to false (not in use). Set inUse 
    // flag so this EPICS record array row is not displayed. return.
    //

    ssEpicsFreeList[ssEpicsArrayRow] = false;
    ssEpicsRecInUse = false;


    //
    // Must free memory allocated into char * epics records. 
    //

    if ( ssMediaUnitName.value() != NULL )
    {
	gen_free( ssMediaUnitName.value() );
    }
    if ( ssDirectory.value() != NULL )
    {
	gen_free( ssDirectory.value() );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStage::init
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
// Initialize the list of stage objects.
//
// DESCRIPTION:
// In a loop, fetch all am library media stage structs from the database. 
// For each one, construct a new cStoUnit object and add it to the list.
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

void		cStoStage::init
(
    cStoStatus	&status		// (mod) Program status.
)
{
    AM_MSTAGE	amStage;	// CADC am library stage structure.
    int		amStatus;	// CADC am library return status. 
    DBPROCESS	*dbProcess;	// Database process. 
    cStoStage::iObjectList
                sIter;         	// Iterator for the node list. 
    cStoStage	*pStage;        // Ptr to node object.
    
    
    checkStat( status, return );


    ssArraySize = ssArraySize.value();


    //
    // Set all inUse flags to false.
    //

    cStoStage::listRLock();
    for ( sIter = cStoStage::begin(); sIter != cStoStage::end(); sIter++ )
    {
	pStage = ((*sIter).second);
        pStage->ssInUse = FALSE;
    }    
    cStoStage::listUnlock();


    //
    // In a loop, fetch all db stage rows worth reporting thru EPICS.
    //

    dbProcess = cStoDbManager::acquire();
    amStatus = AM_SUCCESS; 
    for ( amStatus = amMSGet( AM_SG_ALL, &amStage, dbProcess ); 
	    status.ok() && amStatus == AM_SUCCESS; 
	    amStatus = amMSGet( AM_SG_NEXT, &amStage, dbProcess ) )
    {
	if ( ( pStage = findByDirectory( amStage.amsDirectory ) ) == NULL )
	{
	    //
	    // If we do not already have this one the create a new one.
	    // New stage object. Add to list. Update info in EPICS. 
	    //

	    checkNull( ( pStage = new cStoStage() ), status, VOID );
	    pStage->setDirectory( amStage.amsDirectory );
	    checkNull( pStage->ssPAmStage = setPAmStage( &amStage ), 
		    status, return );
	    if ( amStage.amsMediaRequestId == - 1 )
	    {
		pStage->ssMediaRequestId.blank();
		pStage->ssMediaUnitName.blank();
		pStage->ssInUse	= FALSE;
	    }
	    else
	    {
		pStage->ssMediaRequestId = amStage.amsMediaRequestId;
		pStage->setMediaUnitName( amStage.amsMediaUnitName );
		pStage->ssInUse	= TRUE;
	    }
	}
	else
	{
	    if ( amStage.amsMediaRequestId != -1 )
	    {
		pStage->setMediaRequestId( amStage.amsMediaRequestId );
		pStage->setMediaUnitName( amStage.amsMediaUnitName );
		pStage->ssInUse = TRUE;
	    }
	}
	pStage->ssEpicsRecInUse = true;
    }
    
    cStoDbManager::release();
    if ( amStatus < 0 )
    {
	checkAm( amStatus, status, return );
    }


    //
    // Check all inuse flags, clearing all those that are no longer used.
    //

    cStoStage::listRLock();
    for ( sIter = cStoStage::begin(); sIter != cStoStage::end(); sIter++ )
    {
	pStage = ((*sIter).second);
	if ( pStage->ssInUse == FALSE )
	{
	    pStage->clear();
	}
    }    
    cStoStage::listUnlock();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::find
//
// INVOCATION:
//    find( mediaRequestId, mediaUnitName );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaRequestId	(int)		- Media request identifer.
// (>) mediaUnitName  	(char *)	- Name of the unit to find.
//
// FUNCTION VALUE:
// (cStoStage *) - Ptr to the matching unit.
// NULL         - When no match is found.
//
// PURPOSE:
// To find a staging directory with a media request id and media unit name
// which matches those given.    
//
// DESCRIPTION:
// Iterate through the list of stage directory  and look for the requested
// one, then return the appropriate value.
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

cStoStage	*cStoStage::find
(
    int		mediaRequestId,		// (in)  Media Request ID to find.
    char 	*mediaUnitName		// (in)  Media unit name to find.
)
{
    cStoStage::iObjectList
                sIter;          	// Iterator for the node list. 
    cStoStage	*pStage;               	// Ptr to node object.
    
 
    cStoStage::listRLock();
    for ( sIter = cStoStage::begin(); 
            sIter != cStoStage::end(); 
            sIter++ )
    {
	pStage = ((*sIter).second);
        if ( pStage->getMediaRequestId() == mediaRequestId &&
		streq( pStage->getMediaUnitName(), mediaUnitName ) )
        {
	    cStoStage::listUnlock();
            return( pStage );
        }
    }
    
    cStoStage::listUnlock();
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::findByDirectory
//
// INVOCATION:
//    findByDirectory( directory );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) directory  (char *)	- Name of the directory to find.
//
// FUNCTION VALUE:
// (cStoStage *) - Ptr to the matching unit.
// NULL         - When no match is found.
//
// PURPOSE:
// To find a stage directory which matches the one given.
//
// DESCRIPTION:
// Iterate through the list of units and look for the one given, then return
// the appropriate value.
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

cStoStage	*cStoStage::findByDirectory
(
    char 	*directory
)
{
    cStoStage::iObjectList
                sIter;          	// Iterator for the node list. 
    cStoStage	*pStage;               	// Ptr to node object.
    
 
    cStoStage::listRLock();
    for ( sIter = cStoStage::begin(); 
            sIter != cStoStage::end(); 
            sIter++ )
    {
	pStage = ((*sIter).second);
        if ( streq( pStage->getDirectory(), directory ) )
        {
	    cStoStage::listUnlock();
            return( pStage );
        }
    }
 
    cStoStage::listUnlock();
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStage::selectEpicsRow
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
// found, return its index. If not found, the Storage Server's health is 
// set to warning. 
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

int		cStoStage::selectEpicsRow
(
    void
)
{
    int		num;		// Loop counter. 
    cStoStatus	status;


    //
    // Loop through the list of EPICS array rows, try to find a free one.
    //

    for ( num = 0; num < STO_MAX_STAGE_EPICS_ARRAY; num++ )
    {
	if ( ssEpicsFreeList[num] == false )
	{
	    ssEpicsFreeList[num] = true;
	    return( num );
	}
    }


    //
    // If get to here, must have full EPICS array. Set health to ERROR.
    //

    status.E_STAGES_OVERFLOW( status );


    // Should #define the '-1'.


    return( -1 );
}

//
//***********************************************************************
//+
// Function NAME:
// cStoStage::setPAmStage
//
// INVOCATION:
// setPAmStage( pAmStage );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmStage (AM_MSTAGE *) - Ptr to am library media stage struct.
//
// FUNCTION VALUE:
// (AM_MSTAGE *) Pointer to saved am library stage struct. 
//
// PURPOSE:
// Save a copy of the am library stage struct in the class data. 
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

AM_MSTAGE	*cStoStage::setPAmStage
(
    AM_MSTAGE	*pAmStage	// (in)  Ptr to am library stage structure.
)
{
    AM_MSTAGE	*ptr;		// Ptr to am media stage struct. 


    //
    // Allocate space and save a copy of am library stage structure.
    //

    ptr = (AM_MSTAGE *) gen_alloc( sizeof( AM_MSTAGE ) );
    gen_cpy( ptr, pAmStage, sizeof( AM_MSTAGE ) );


    //
    // Return pointer to allocated stuff. 
    //

    return( ptr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStage::exit
//
// INVOCATION:
// exit( status );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status   (cStoStatus &)  - Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Delete all objects in the list. 
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

void		cStoStage::exit
(
    cStoStatus	&status		// (mod) Program status.
)
{
    checkStat( status, return );


    //
    // For each media stage object in the list, nuke it. 
    //

    deleteAll();

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoStage::clear
//
// INVOCATION:
// clear();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set flag so contents of certain Epics records aren't shown on console. 
//
// DESCRIPTION:
// Pretty Basic.
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

void		cStoStage::clear
(
    void
)
{
    //
    // Set inUse flag to false. 
    //

    ssMediaRequestId.blank();
    ssMediaUnitName.blank();
    setInUse( false );
}
