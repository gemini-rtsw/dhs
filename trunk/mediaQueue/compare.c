/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	mediaQueue/src/compare.c
*
*   Purpose:
*	Contains eptr search comparison functions.
*
*   Routines:
*	int	dateCompare	: compare function for two UNIX dates
*	int	fileCompare	: EPTR compare function two am file structures.
*	int	mediaUnitCompare: Compare two media units.
*	int	sizeCompare	: compare function for two file sizes.
*	int	treeCompare	: EPTR compare for two directory structures.
*	int	treeNameCompare	: EPTR sort compare function for dir. names.
*
*   Date		: <Jan 28, 1998>
*
*   SCCS data		: @(#)
*	Module Name	: compare.c
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 03/17/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/01/28 SDJ	: Copied from VLT and remove al library dependencies.
*	98/09/98 SDJ	: Added compares for tree (directory) structure.
*	99/03/05 NRH	: Added mediaUnitCompare.
*	99/03/14 SDJ	: Added use of new TREE_SIZE macro.
*	99/03/29 SDJ	: checkForSplit() was comparing block and bytes.
*     2000/02/03 SDJ	: Changed file/tree compares to compare only
*			  the information desired, based on command-line
*			  args and database options.
*     2000/03/17 SDJ	: Use the new amfDayNotified in the file struct.
*			  when comparing dates when file dates and sizes
*			  are to be checked.  This garauntees a unique
*			  sort which was not the case before.  As a result
*			  dateCompare() has been simplified.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

/*
 ************************************************************************
 *  SYBASE DB libarary include files.
 ************************************************************************
 */

#include <sybfront.h>
#include <sybdb.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "gen_msg.h"
#include "db.h"
#include "am.h"


/*
 ************************************************************************
 *  Media Queue  include files.
 ************************************************************************
 */

#include "globals.h"


static int	dateCompare( time_t, time_t );
static int	sizeCompare( double, double );

/*+
************************************************************************
*
*   Function:   dateCompare
*
*   Purpose:
*	Compares two dates for sorting the eptr list in eptr_sort.
*
*   Values Returned:
*	int	1	: time1 has an earlier date.
*	int	0	: The dates are identical.
*	int	-1	: time2 has an earlier date.
*
************************************************************************
-*/

int		dateCompare
( 
    time_t	time1,	/* (in)  UNIX time value		*/
    time_t	time2	/* (in)  UNIX time value		*/
)
{
    if ( time1 < time2 )
    {
	return( -1 );
    }
    else if ( time1 > time2 )
    {
	return( 1 );	
    }	
    else	
    {
	return( 0 );
    }
}

/*+
************************************************************************
*
*   Function:   fileCompare
*
*   Purpose:
*	Compares filenames first, then date, followed by size.
*
*   Values Returned:
*	int	1	: filePtr1 has an earlier date.
*	int	0	: The dates are identical.
*	int	-1	: filePtr2 has an earlier date.
*
************************************************************************
-*/

int		fileCompare
( 
    FILE_INFO	*filePtr1,	/* (in)  Ptr. to media file structure.	*/
    FILE_INFO	*filePtr2	/* (in)  Ptr. to media file structure.	*/
)
{
    int		ret;		/* Return value.			*/

    ret = 0;
    
    if ( strne( filePtr1->fiAmFilePtr->amfFilename,
	    filePtr2->fiAmFilePtr->amfFilename ) )
    {
	if ( globals.gCheckDate )
	{
	    if ( globals.gCheckSize && globals.gCheckDate ) 
	    {
		/*
		 * If we are checking the size then we should consider
		 * the files written on the "same day" as having the
		 *  same time.
		 */

		ret = dateCompare( filePtr1->fiAmFilePtr->amfDayNotified,
			filePtr2->fiAmFilePtr->amfDayNotified );
	    }
	    else
	    {
		ret = dateCompare( filePtr1->fiAmFilePtr->amfDateNotified,
			filePtr2->fiAmFilePtr->amfDateNotified );
	    }
	}
	
	if ( ret == 0 && globals.gCheckSize )
	{
	    ret = sizeCompare( filePtr1->fiAmFilePtr->amfFileSize,
		    filePtr2->fiAmFilePtr->amfFileSize );
	}

	if ( ret == 0 )
	{
	    ret = strcmp( filePtr1->fiAmFilePtr->amfFilename,
		    filePtr2->fiAmFilePtr->amfFilename );
	}
    }

    return( ret );
}

/*+
************************************************************************
*
*   Function:	mediaUnitCompare
*
*   Purpose:
*	Compare two media units.
*
*   Values Returned:
*	int	-1		: mediaUnit1 < mediaUnit2.
*	int	0		: mediaUnit1 == mediaUnit2.
*	int	1		: mediaUnit1 > mediaUnit2.
*
************************************************************************
-*/

int		mediaUnitCompare
(
    AM_UNIT 	*mediaUnit1,	/* (in)  The first media unit.		*/
    AM_UNIT	*mediaUnit2	/* (in)  The second media unit.		*/
)
{
    if ( mediaUnit1->amuMediaRequestId > mediaUnit2->amuMediaRequestId )
    {
	return( 1 );
    }
    else if ( mediaUnit1->amuMediaRequestId < mediaUnit2->amuMediaRequestId )
    {
	return( -1 );
    }
    else
    {
	return( strcmp( mediaUnit1->amuMediaUnitName, 
		mediaUnit2->amuMediaUnitName ) );
    }
}

/*+
************************************************************************
*
*   Function:   revSizeCompare
*
*   Purpose:
*	Compares two file sizes for sorting the eptr list in eptr_sort
*	so that the files are sorted with the largest one first.
*
*   Values Returned:
*	int	1	: size1 is bigger.
*	int	0	: The dates are identical.
*	int	-1	: size2 is bigger.
*
************************************************************************
-*/

int		revSizeCompare
( 
    FILE_INFO	*filePtr1,	/* (in)  Ptr. to media file structure.	*/
    FILE_INFO	*filePtr2	/* (in)  Ptr. to media file structure.	*/
)
{
    if ( filePtr2->fiAmFilePtr->amfFileSize <
	    filePtr1->fiAmFilePtr->amfFileSize )
    {
	return( 1 );
    }
    else if ( filePtr2->fiAmFilePtr->amfFileSize <
	    filePtr1->fiAmFilePtr->amfFileSize )
    {
	return( -1 );
    }

    return( 0 );
}

/*+
************************************************************************
*
*   Function:   sizeCompare
*
*   Purpose:
*	Compares two file sizes for sorting the eptr list in eptr_sort.
*
*   Values Returned:
*	int	1	: size2 is bigger.
*	int	0	: The es are identical.
*	int	-1	: size1 is bigger.
*
************************************************************************
-*/

int		sizeCompare
( 
    double	size1,	/* (in)  UNIX time value		*/
    double	size2	/* (in)  UNIX time value		*/
)
{
    if ( size1 < size2 )
    {
	return( 1 );
    }
    else if ( size1 > size2 )
    {
	return( -1 );
    }

    return( 0 );
}

/*+
************************************************************************
*
*   Function:   treeCompare
*
*   Purpose:
*	Compares two directories trees, first comaring date then size.
*
*   Values Returned:
*	int	1	: treePtr1 has an earlier date.
*	int	0	: The directory trees are identical.
*	int	-1	: treePtr2 has an earlier date.
*
************************************************************************
-*/

int		treeCompare
( 
    TREE_INFO	*treePtr1,	/* (in)  Ptr. to dir. tree structure.	*/
    TREE_INFO	*treePtr2	/* (in)  Ptr. to dir. tree structure.	*/
)
{
    int		ret;		/* Return value.			*/

    ret = 0;
    if ( strne( treePtr1->tiTreeName, treePtr2->tiTreeName ) )
    {
	if ( globals.gCheckDate )
	{
	    ret = dateCompare( treePtr1->tiOldestDate, treePtr2->tiOldestDate );
	}

	if ( ret == 0 && globals.gCheckSize )
	{
	    ret = sizeCompare ( TREE_SIZE( treePtr1 ), TREE_SIZE( treePtr2 ) );
	}
	
	if ( ret == 0 )
	{
	    ret = strcmp( treePtr1->tiTreeName, treePtr2->tiTreeName );
	}
    }
    

    return( ret );
}

/*+
************************************************************************
*
*   Function:   treeNameCompare
*
*   Purpose:
*	Compares two paths for searching the eptr list in eptr_search.
*
*   Values Returned:
*	int	1	: treeName comes before treePtr->tiTreeName.
*	int	0	: The paths are identical.
*	int	-1	: treePtr->tiTreeName comes before treeName.
*
************************************************************************
-*/

int		treeNameCompare
( 
    char	*treeName,	/* (in)  Directory tree name.		*/
    TREE_INFO	*treePtr	/* (in)  Ptr. to Dir. tree structure.	*/
)
{
    return( strcmp( treeName, treePtr->tiTreeName ) );
}

