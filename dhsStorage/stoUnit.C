static char rcsid[] = "$Id: stoUnit.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStorage/src/stoUnit.C
//
// PURPOSE:
// Contains methods of the cStoUnit class.
//
// METHOD NAME(S)
// cStoUnit::cStoUnit	    - Constructor.
// cStoUnit::~cStoUnit	    - Deconstructor.
// cStoUnit::compare	    - Compares two units.
// cStoUnit::db2Stats	    - Query database for unit object statistic values.
// cStoUnit::exit	    - Nuke list, releasing resources held by the class.
// cStoUnit::findByName	    - Find a particular unit it the list.
// cStoUnit::findFirstUnit  - Find the first unit with a given status.    
// cStoUnit::findLastUnit   - Find the last unit with a given status.
// cStoUnit::getMediaUnitNameRoot
//			    - Get the root name of the media units from the
//			      am unit structure.
// cStoUnit::init	    - Initialize list of unit objects.
// cStoUnit::selectEpicsRow - Pick a row in Epics array.
// cStoUnit::setPAmUnit	    - Set pointer to am library unit struct.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:29  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  1999/06/17 00:00:43  nhill
// Major re-organization.
//
// Revision 1.5  1999/02/25 00:03:19  jaeger
// Added findByName(), findFirstUnit(), findLastUnit(), and compare().
// Altered init() instead of assuming no units exist it updates existing
// units with db info and deletes those which were not found in the database.
// Added getMediaUnitNameRoot().
//
// Revision 1.4  1998/05/15 17:14:20  cockayne
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
#include "stoMedia.H"
#include "stoDhs.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoUnit.H"


//
// Define the static data in the cStoUnit class.
//

bool		cStoUnit::suEpicsFreeList[STO_MAX_UNIT_EPICS_ARRAY];
						// The list of free/non-free
						// EPICS unit array rows.
cStoEpicsRecord<bool, cStoUnit, cStoStatus>	
		cStoUnit::suEpicsOverflow( STO_ERU_UNIT_OVERFLOW, false, NULL, NULL );
						// true if EPICS is overflowed.
cStoEpicsRecord<int, cStoUnit, cStoStatus>	
		cStoUnit::suNumUnits( STO_ERU_NUM_UNITS, 0, NULL, NULL );
						// Number of units. 
cStoEpicsRecord<int, cStoUnit, cStoStatus>	
		cStoUnit::suArraySize( STO_ERU_UNIT_ARRAY_SIZE, STO_MAX_UNIT_EPICS_ARRAY, NULL, NULL );
						// Unit array size.

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::cStoUnit
//
// INVOCATION:
// cStoUnit( pAmUnit, addToList );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmUnit	(AM_UNIT *) - Pointer to am media unit struct.
// (>) addToList (bool)	    - Boolean telling whether to add object to list.
// 
// FUNCTION VALUE:
// (cStoUnit) new unit object.
// 
// PURPOSE:
// Constructor for cStoUnit objects.
// 
// DESCRIPTION:
// Construct the object, adding it to the list of such objects, and 
// initializing all class data, including getting a row in the Epics
// array, saving a copy of the am library unit struct, and initializing
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

		cStoUnit::cStoUnit
(
    AM_UNIT	*pAmUnit,	// Pointer to am media unit struct.
    bool	addToList	// Set to true if we're to add to the list.
)
:   cListOf< cStoUnit, std::string, cStoStatus >
	( std::string( pAmUnit->amuMediaUnitName ), addToList ),
    suRefCount( 1 ),

    //
    // Set EPICS array row, set am library media unit struct pointer. 
    //

    suEpicsArrayRow( selectEpicsRow() ),
    suPAmUnit( setPAmUnit( pAmUnit) ),

    
    //
    // Set the am library fields which are stored in EPICS. 
    //

    suEpicsRecInUse( suEpicsArrayRow, STO_ERU_IN_USE, 
	    true, this, NULL ),
    suMediaReqId( suEpicsArrayRow, STO_ERU_REQUEST_ID, 
	    pAmUnit->amuMediaRequestId, this, NULL ),
    suMediaUnitName( suEpicsArrayRow, STO_ERU_UNIT_NAME, 
	    strsav( pAmUnit->amuMediaUnitName ), this, NULL ),
    suUnitSize( suEpicsArrayRow, STO_ERU_UNIT_SIZE, 
	    pAmUnit->amuTotalSize, this, NULL ),


    //
    // Set the statistics which are stored in EPICS to UNKNOWN. 
    //
    
    suPrepared( suEpicsArrayRow, STO_ERU_PREPARED, this, NULL ),
    suNumWritten( suEpicsArrayRow, STO_ERU_NUM_WRITTEN, this, NULL ),
    suCleanedUp( suEpicsArrayRow, STO_ERU_CLEANEDUP, this, NULL )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::~cStoUnit
//
// INVOCATION:
// ~cStoUnit();
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor for cStoUnit class.
//
// DESCRIPTION:
// Free memory allocated for the object. Set Epics records so the
// unit is no longer displayed in the console, and then check if 
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

		cStoUnit::~cStoUnit
(
    void
)
{
    cStoUnit::iObjectList
    		i;		// Media unit list iterator. 
    cStoStatus	status;		// Program status.
    cStoUnit	*pUnit;
    bool	overflow;


    if ( suPAmUnit != NULL )
    {
	gen_free( suPAmUnit );
	suPAmUnit = NULL;

    
	//
	// Set the epics free list entry to false (not in use). Set inUse 
	// flag so this EPICS record array row is not displayed. return.
	//

	suEpicsFreeList[suEpicsArrayRow] = false;
	suNumUnits = suNumUnits.value() - 1;
	suEpicsRecInUse = false;


	//
	// Must free memory allocated into char * epics records. 
	//

	gen_free( suMediaUnitName.value() );
    }



    
    //
    // Check if Epics Array was overflowing before this object was deleted.
    //

    //
    // If get to here, must have previously been overflowing.  Therefore, 
    // must have a list entry with array row set to -1, to put in newly
    // free EPICS row.
    //

    for ( i = cStoUnit::begin(); i != cStoUnit::end(); i++ )
    {
	pUnit = (*i).second;
	if ( pUnit->suEpicsArrayRow == -1 )
	{
	    //
	    // Set EPICS array row, cause EPICS update.
	    // See if we can un-set Overflow. 
	    //
	
	    pUnit->flushAll( suEpicsArrayRow );
	    suEpicsFreeList[suEpicsArrayRow] = true;
	    pUnit->suEpicsRecInUse = true;

	    break;
	}
    }


    for ( overflow = false, i = cStoUnit::begin(); i != cStoUnit::end(); i++ )
    {
	pUnit = (*i).second;
	if ( pUnit->suEpicsArrayRow == -1 )
	{
	    overflow = true;
	    break;
	}
    }
    suEpicsOverflow = overflow;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cSto::compare
//
// INVOCATION:
// cSto::compare( unitNameRoot1, mediaUnitName1, unitNameRoot2,
//    mediaUnitName2, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) unitNameRoot1 	(char[AM_UNIT_NAME_ROOT_LEN])
//    				Root name of the two
// (>) mediUnitName1 	(char[AM_MEDIA_UNIT_NAME_LEN])
//    				First unit name to compare with.
// (>) unitNameRoot2 	(char[AM_UNIT_NAME_ROOT_LEN])
//    				Root name of the two
// (>) mediUnitName2 	(char[AM_MEDIA_UNIT_NAME_LEN])
//    				Second unit name to compare with.
// (<) stauts		(boolean) Status of the comparision.    
//
// FUNCTION VALUE:
// (int) < 0 : If unit1 < unit2
// (int)  0  : If unit1 = unit2    
// (int) > 0 : If unit1 > unit2
//
// PURPOSE:
// Compares two media units.
// 
// DESCRIPTION:
// IF the unit name roots differ then return the string comparison between
// these two names.  Otherwise compare the numerical value appended to both
// root names.  
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

int	cStoUnit::compare
(
    char	unitNameRoot1[AM_UNIT_NAME_ROOT_LEN],
    				// (in)  Unit root name of the first unit.
    char	mediaUnitName1[AM_MEDIA_UNIT_NAME_LEN],
    				// (in)  Media unit name of the first unit.
    char	unitNameRoot2[AM_UNIT_NAME_ROOT_LEN],
    				// (in)  Unit root name of the first unit.
    char	mediaUnitName2[AM_MEDIA_UNIT_NAME_LEN],
    				// (in)  Medi1 unit name of the first unit.
    boolean	*status		// (out) Status of the comparison.
)
{
    int		ret;		// Return value.
    int		rootNameLen;	// Length of the unit root name
    int		unitNum1;	// Unit number of the first unit.
    int		unitNum2;	// Unit number of the second unit.
    
    *status = TRUE;
    if ( ( ret = strcmp( unitNameRoot1, unitNameRoot2 ) ) == 0 )
    {
	rootNameLen = strlen( unitNameRoot1 );
	if ( ! str2int( mediaUnitName1 + rootNameLen, &unitNum1 ) )
	{
	    *status = FALSE;
	    ret = 0;
	}
	if ( ! str2int( mediaUnitName2 + rootNameLen, &unitNum2 ) )
	{
	    *status = FALSE;
	    ret = 0;
	}
	ret = unitNum1 - unitNum2;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::db2Stats
//
// INVOCATION:
// n/a invoked only by the forEach() method.
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) ---	(void *)	- Could be ptr to anything. Ignored here. 
// (!) status	(cStoStatus &)	- Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Update the statistics for the cStoUnit class.
//
// DESCRIPTION:
// This method is called for each object in the unit object list. For each
// object, query the database for the unit statistics, and update the class
// data 
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

void		cStoUnit::db2Stats
(
    void	*,		// ()    Dummy variable. 
    cStoStatus	&status		// (mod) Program status.
)
{
    DBPROCESS	*dbProcess;	// Database process. 
    int		dbIntValue;	// Integer return value from the database.
    boolean	dbBooleanValue;	// Boolean return value from the database.


    checkStat( status, return );


    //
    // Prepared?
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amPrepared( dbProcess, suPAmUnit->amuMediaRequestId, 
	    suPAmUnit->amuMediaUnitName, &dbBooleanValue ), status, VOID );
    suPrepared = ( ( dbBooleanValue == TRUE ) ? true : false );
    cStoDbManager::release();


    //
    // Number Written.
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amNumCopiesWritten( dbProcess, suPAmUnit->amuMediaRequestId, 
	    suPAmUnit->amuMediaUnitName, &dbIntValue ), status, VOID );
    suNumWritten = dbIntValue;
    cStoDbManager::release();


    //
    // Cleaned up?
    //

    dbProcess = cStoDbManager::acquire();
    checkAm( amCleanedUp( dbProcess, suPAmUnit->amuMediaRequestId, 
	    suPAmUnit->amuMediaUnitName, &dbBooleanValue ), status, VOID );
    suCleanedUp = ( ( dbBooleanValue == TRUE ) ? true : false );
    cStoDbManager::release();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::exit
//
// INVOCATION:
// exit( status );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &)	- Program status.
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

void		cStoUnit::exit
(
    cStoStatus	&status		// (mod) Program status.
)
{
    checkStat( status, return );


    //
    // For each media unit object in the list, nuke it. 
    //

    deleteAll();

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::findByName
//
// INVOCATION:
//    findByName( mediaRequestId, unitName );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaRequestId (int)	- The media request id.
// (>) unitName  (char *)	- Name of the unit to find.
//
// FUNCTION VALUE:
// (cStoUnit *) - Ptr to the matching unit.
// NULL         - When no match is found.
//
// PURPOSE:
// To find a unit which matches the one given.
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

cStoUnit	*cStoUnit::findByName
(
    int		mediaRequestId,
    char 	*name
)
{
    cStoUnit::iObjectList
                uIter;          	// Iterator for the node list. 
    cStoUnit	*pUnit;               	// Ptr to node object.
    
 
    cStoUnit::listRLock();
    for ( uIter = cStoUnit::begin(); 
            uIter != cStoUnit::end(); 
            uIter++ )
    {
	pUnit = ((*uIter).second);
        if ( streq( pUnit->getMediaUnitName(), name ) && 
		pUnit->getMediaRequestId() == mediaRequestId )
        {
	    cStoUnit::listUnlock();
            return( pUnit );
        }
    }
 
    cStoUnit::listUnlock();
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::findFirstUnit
//
// INVOCATION:
//    findFirstUnit( mediaRequestId, unitStatus );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaRequestId	(int)		- Media Request ID to match.
// (>) unitStatus	(const char)	- Media unist status to match.
//    
// FUNCTION VALUE:
// (cStoUnit *) - Ptr to the last matching unit.
// NULL         - When no unit is found.
//
// PURPOSE:
// To find the media unit with the "smallest" unit name with the given
// media request ID and status value.
//
// DESCRIPTION:
// Iterate through the list of units and look for the one that is the
// greatest.  The greatest unit name is the one if the smallest number
// appended to the root media unit name.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// none.
//
// SEE ALSO:
// cStoMedia::compare
//
// DEFICIENCIES:
// If not all media unit names for the given media request ID have the 
// same root media name root this routine will fail.
//-
//***********************************************************************
//

cStoUnit	*cStoUnit::findFirstUnit
(
    int		mediaRequestId,
    const char	unitStatus
)
{
    boolean	compareStatus;		// Result of the compare.
    cStoMedia	*pMedia;		// Ptr to media object.
    char	prevUnitName[AM_MEDIA_UNIT_NAME_LEN];
					// Previous unit name;
    char	unitNameRoot[AM_UNIT_NAME_ROOT_LEN];
					// The media unit name root.
    boolean	statusMatch;		// TRUE if status value matches.
    cStoUnit::iObjectList
                unitListIter;          	// Iterator for the node list. 
    cStoUnit	*pUnit;               	// Ptr to node object.


    //
    // Initialize the variables.
    //
    
    pUnit = NULL;
    pMedia = cStoMedia::find( mediaRequestId, NULL );    
    (void) sprintf( unitNameRoot, pMedia->getPAmMedia()->ammUnitNameRoot );
    prevUnitName[0] = '\0';


    //
    // Iterate through the list attempting to find the biggest one.
    //

    cStoUnit::listRLock();
    for ( unitListIter = cStoUnit::begin();  unitListIter != cStoUnit::end(); 
	  unitListIter++ )
    {
	if ( ((*unitListIter).second)->getMediaRequestId() != mediaRequestId )
	{
	    //
	    // Current unit does not have the desired media request ID.
	    //

	    continue;
	}


	//
	// Compare the current one with the last unit found.
	//

	compareStatus = TRUE;
	if ( streq( prevUnitName, "" ) || 
		( compare( unitNameRoot, prevUnitName, unitNameRoot,
		((*unitListIter).second)->getMediaUnitName(),
		&compareStatus ) > 0 && &compareStatus ) )
	{
	    //
	    //  Make sure the status values match.
	    //

	    statusMatch = FALSE;
	    switch( unitStatus )
	    {
		case AMU_QUEUEING:
		case AMU_QUEUED:
		case AMU_PREPARED:
		case AMU_WRITTEN:
	    	case AMU_CLEANED_UP:
		case AMU_DONE:
		    if ( ((*unitListIter).second)->suPAmUnit->amuStatus ==
	    		unitStatus )
		    {
			statusMatch = TRUE;
		    }
		    break;

		default:
		    statusMatch = TRUE;
		    break;
	    }
	    
	    
	    if  ( statusMatch )
	    {
		
		//
		// We have a match and the current one is smaller.
		//
		
		pUnit = ((*unitListIter).second);
		(void) sprintf( prevUnitName, "%s",
			((*unitListIter).second)->getMediaUnitName() );
	    }
	}
    }
    cStoUnit::listUnlock();
    
    return( pUnit );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::findLastUnit
//
// INVOCATION:
//    findLastUnit( mediaRequestId, unitStatus );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaRequestId	(int)		- Media Request ID to match.
// (>) unitStatus	(const char)	- Media unist status to match.
//    
// FUNCTION VALUE:
// (cStoUnit *) - Ptr to the last matching unit.
// NULL         - When no unit is found.
//
// PURPOSE:
// To find the media unit with the greatest unit name with the given
// media request ID and status value.
//
// DESCRIPTION:
// Iterate through the list of units and look for the one that is the
// greatest.  The greatest unit name is the one if the largest number
// appended to the root media unit name..
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// none.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// If not all media unit names for the given media request ID have the 
// same root media name root this routine will fail.
//-
//***********************************************************************
//

cStoUnit	*cStoUnit::findLastUnit
(
    int		mediaRequestId,
    const char	unitStatus
)
{
    boolean	compareStatus;		// Result of the compare.
    int		copiesWritten;		// Number of unit copies written.
    char	curUnitStatus;		// Status of the current unit.
    cStoMedia	*pMedia;		// Ptr to media object.
    char	prevUnitName[AM_MEDIA_UNIT_NAME_LEN];
					// Previous unit name;
    char	unitNameRoot[AM_UNIT_NAME_ROOT_LEN];
					// The media unit name root.
    boolean	statusMatch;		// TRUE if status value matches.
    cStoUnit::iObjectList
                unitListIter;          	// Iterator for the node list. 
    cStoUnit	*pUnit;               	// Ptr to node object.


    //
    // Initialize the variables.
    //
    
    pUnit = NULL;
    pMedia = cStoMedia::find( mediaRequestId, NULL );    
    (void) sprintf( unitNameRoot, pMedia->getPAmMedia()->ammUnitNameRoot );
    prevUnitName[0] = '\0';
    

    //
    // Iterate through the list attempting to find the biggest one.
    //

    cStoUnit::listRLock();
    for ( unitListIter = cStoUnit::begin();  unitListIter != cStoUnit::end(); 
	  unitListIter++ )
    {
	if ( ((*unitListIter).second)->getMediaRequestId() != mediaRequestId )
	{
	    //
	    // Current unit does not have the desired media request ID.
	    //

	    continue;
	}


	//
	// Compare the current one with the last unit found.
	//

	compareStatus = TRUE;
	if ( streq( prevUnitName, "" ) || 
		( compare( unitNameRoot, prevUnitName, unitNameRoot,
		((*unitListIter).second)->getMediaUnitName(),
		&compareStatus ) < 0 && compareStatus ) )
	{
	    //
	    //  Make sure the status values match.
	    //

	    statusMatch = FALSE;
	    switch( unitStatus )
	    {
		case AMU_QUEUEING:
		case AMU_QUEUED:
		case AMU_PREPARED:
	    	case AMU_CLEANED_UP:
		case AMU_DONE:
		    if ( ((*unitListIter).second)->suPAmUnit->amuStatus ==
	    		unitStatus )
		    {
			statusMatch = TRUE;
		    }
		    break;
		case AMU_WRITTEN:
		    curUnitStatus =
			((*unitListIter).second)->suPAmUnit->amuStatus;
		    copiesWritten =
			((*unitListIter).second)->getPAmUnit()->amuCopiesWritten;
		    if ( curUnitStatus == unitStatus  ||
	    		    ( curUnitStatus == AMU_PREPARED &&
		    	      copiesWritten != 0 ) )
		    {
			statusMatch = TRUE;
		    }
		    break;

		default:
		    statusMatch = TRUE;
		    break;
	    }
	    
	    
	    if  ( statusMatch )
	    {
		//
		// We have a match and it is bigger.
		//
		
		pUnit = ((*unitListIter).second);
		(void) sprintf( prevUnitName, "%s",
			((*unitListIter).second)->getMediaUnitName() );
	    }
	}
    }
    cStoUnit::listUnlock();
    
    return( pUnit );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoMedia::getMediaUnitNameRoot
//
// INVOCATION:
//    findLastUnit( mediaRequestId);
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaRequestId	(int)		- Media Request ID to match.
//    
// FUNCTION VALUE:
// (cStoUnit *) - Ptr to the unit name root matching the request id given.
// NULL         - When no unit is found.
//
// PURPOSE:
// To find the media unit root name for the given media request id.
// media request ID and status value.
//
// DESCRIPTION:
// Iterate through the list of units and look for the matching media.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// none.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

const char	*cStoUnit::getMediaUnitNameRoot
(
    int		mediaRequestId
)
{
    cStoMedia	*pMedia;		// Ptr to media object.
    cStoUnit	*pUnit;               	// Ptr to node object.


    //
    // Initialize the variables.
    //
    
    pUnit = NULL;
    pMedia = cStoMedia::find( mediaRequestId, NULL );
    return( pMedia->getPAmMedia()->ammUnitNameRoot );
}    

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::init
//
// INVOCATION:
// init( status );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &)	- Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the list of unit objects.
//
// DESCRIPTION:
// In a loop, fetch all am library media unit structs from the database. 
// For each one, construct a new cStoUnit object and add it to the list.
// When done creating new objects, update the statistics using forEach()
// and the db2Stats() method. 
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

void		cStoUnit::init
(
    cStoStatus	&status		// (mod) Program status.
)
{
    AM_UNIT	amUnit;		// CADC am library media unit structure. 
    int		amStatus;	// CADC am library return status. 
    DBPROCESS	*dbProcess;	// Database process. 
    int		i;
    char	name[41];	// Name of the in use array.
    cStoUnit::iObjectList
                uIter;         	// Iterator for the node list. 
    cStoUnit	*pUnit;		// Ptr to media unit object.
    cStoUnit::tObjectList
    		unitList;	// List of units to delete.
    static cMutex		// Mutex to prevent this routine from being
    		mutex;		// run twice at the same time.

    
    checkStat( status, return );

    mutex.lock();

    suEpicsOverflow = suEpicsOverflow.value();
    suNumUnits = suNumUnits.value();
    suArraySize = suArraySize.value();


    //
    // Set all inUse flags to false.
    //

    cStoUnit::listRLock();
    for ( uIter = cStoUnit::begin();  uIter != cStoUnit::end();  uIter++ )
    {
	pUnit = ((*uIter).second);
        pUnit->suInUse = FALSE;
	pUnit->ref();
	unitList[ (char *) pUnit->suMediaUnitName ] = pUnit;
    }    
    cStoUnit::listUnlock();

    for ( i = 0; i < STO_MAX_UNIT_EPICS_ARRAY; i ++ )
    {
	sprintf( name, STO_ERU_IN_USE, i );
	cStaStatus::update( name, "FALSE", status );
    }
    cStaStatus::flush( status );


    //
    // In a loop, fetch all db media_units rows worth reporting thru EPICS.
    //

    dbProcess = cStoDbManager::acquire();
    amStatus = AM_SUCCESS; 
    for ( amStatus = amUnitGet( AM_UG_STATUS, &amUnit, dbProcess, 
	    'P', 'Q', 'W', 'C', NULL ); 
	    status.ok() && amStatus == AM_SUCCESS; 
	    amStatus = amUnitGet( AM_UG_NEXT, &amUnit, dbProcess ) )
    {
	if ( ( pUnit = findByName( amUnit.amuMediaRequestId, 
		amUnit.amuMediaUnitName ) ) == NULL )
	{
	    //
	    // If we do not already have this unit then create it.
	    // New unit object. Add to list. Update information in EPICS. 
	    //
	    
	    checkNull( ( pUnit = new cStoUnit( &amUnit, true ) ), status,
    		VOID );
	}
	else
	{
	    pUnit->setUnitSize( amUnit.amuTotalSize );
	}
	pUnit->suEpicsRecInUse = TRUE;
	pUnit->suInUse = TRUE;
    }
    cStoDbManager::release();
    if ( amStatus < 0 )
    {
	checkAm( amStatus, status, mutex.unlock(); return );
    }


    //
    // Delete all those that are not used.
    //

    for ( uIter = unitList.begin();  uIter != unitList.end();  uIter++ )
    {
	pUnit = ((*uIter).second);
	pUnit->deref();
	if ( pUnit->suInUse == FALSE )
	{
	    pUnit->deref();
	}
    }    


    //
    // Update each object's statistics from the database.
    //
    
    forEach( &cStoUnit::db2Stats, NULL, status );

    mutex.unlock();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::selectEpicsRow
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
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

int		cStoUnit::selectEpicsRow
(
    void
)
{
    int		num;		// Loop counter. 
    cStoStatus	status;


    //
    // Loop through the list of EPICS array rows, try to find a free one.
    //

    suNumUnits = suNumUnits.value() + 1;
    for ( num = 0; num < STO_MAX_UNIT_EPICS_ARRAY; num++ )
    {
	if ( suEpicsFreeList[num] == false )
	{
	    suEpicsFreeList[num] = true;
	    return( num );
	}
    }


    //
    // If get to here, must have full EPICS array. Set health to WARNING.
    //

    status.E_UNIT_OVERFLOW( status );
    suEpicsOverflow = true;

    return( -1 );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoUnit::setPAmUnit
//
// INVOCATION:
// setPAmUnit( pAmUnit );
// 
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pAmUnit	(AM_UNIT *) - Ptr to am library media unit struct.
//
// FUNCTION VALUE:
// (AM_UNIT *) Pointer to saved am library unit struct. 
//
// PURPOSE:
// Save a copy of the am library unit struct in the class data. 
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

AM_UNIT	*cStoUnit::setPAmUnit
(
    AM_UNIT	*pAmUnit	// (in)  Ptr to am library media unit struct.
)
{
    AM_UNIT	*ptr;		// Ptr to am media unit struct. 


    //
    // Allocate space and save a copy of am library media unit struct.
    //

    ptr = (AM_UNIT *) gen_alloc( sizeof( AM_UNIT ) );
    gen_cpy( ptr, pAmUnit, sizeof( AM_UNIT ) );


    //
    // Return pointer to allocated stuff. 
    //

    return( ptr );
}
