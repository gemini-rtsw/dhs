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
*   Module Name:	am/src/amInfo.c
*
*   Purpose:
*	Routines that return information about the media, mediaUnits 
*	and mediaFiles tables in the database.
*
*   Routines:
*	int	amAmountRetrievable
*	int	amAmountQueueable
*	int	amNumPreparable
*	int	amPrepared
*	int	amNumWriteable
*	int	amNumCopiesWritten
*	int	amNumCleanupable
*	int	amCleanedUp
*	int	amNotifiable
*	int	amAmountOnUnit
*	int	amNumOnUnit
*
*   Date		: February 10, 1998
*
*   SCCS data		: @(#)
*	Module Name	: amInfo.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 09/11/98
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/10 SEC  : Added module to provide functions which give
*			information about contents of media database tables. 
*			This module added to support gemini Storage Server.
*	98/04/20 SEC  :	Modifications made to return float from 
*			amAmountRetrievable() and amAmountQueueable().
*	98/09/09 JSD  :	Added routine names at top.
*	98/10/27 SDJ  : Added comments and header info.
*	99/03/18 NRH  : Added funciton amNumOnUnit.
*	99/06/16 NRH  : Changed amAmountQueueable to get the amount on
*			incompletely queued units.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
#include <time.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"

#include "am.h"
#include "localAm.h"

LOCAL_AM	localAm;
boolean		amInitFlag;


/*+
************************************************************************
*
*   Function:	amAmountRetrievable
*
*   Purpose:
*	Determine the amount of data, in kilobytes, that is ready
*	for retrieval for a particular request.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amAmountRetrievable
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    float	*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );

    
    /*
     * Query the database.
     */

    /* Return size in bytes */
    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    sum( convert( float, filesize ) ) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and status in ( '%c', '%c', '%c' ) \n",
	    localAm.laDatabase,
	    localAm.laMedFilesTable,
	    mediaRequestId,
	    AMF_UNPROCESSED,
	    AMF_UNAVAILABLE,
	    AMF_ERROR ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );

    
    
    /*
     * Return the amount of data that can retrieved, in kilobytes.
     */

    dbStatus = db_floatdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amAmountQueueable
*
*   Purpose:
*	Determine the amount of data (in Kilobytes) that can be queued.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amAmountQueueable
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    float 	*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/
    float	queuingValue;	/* Value being queued.			*/


    AM_CHECK_INIT;


    *returnValue = 0;
    AM_CHECK_DB( dbcanquery( dbproc ) );


    /*
     * Query the database.
     */

    /* Return size in bytes */
    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    sum( convert( float, filesize ) ) \n"
	    "from \n"
	    "    %s..%s mf \n"
	    "where \n"
	    "    mf.media_request_id = %d \n"
	    "    and mf.status = '%c' \n"
	    "    and mf.media_unit_name is NULL \n",
	    localAm.laDatabase,
	    localAm.laMedFilesTable,
	    mediaRequestId,
	    AMF_RETRIEVED ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );

    

    /*
     * Return the amount of data that can be Queued, in kilobytes.
     */

    dbStatus = db_floatdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }


    /*
     * Add the data that is on incompletely queued units.
     */
    
    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    sum( convert( float, filesize ) ) \n"
	    "from \n"
	    "    %s..%s mf, %s..%s mu \n"
	    "where \n"
	    "    mu.media_request_id = %d \n"
	    "    and mf.media_unit_name = mu.media_unit_name \n"
	    "    and mf.media_request_id = %d \n"
	    "    and mf.status = '%c' \n"
	    "    and mu.status = '%c' \n",
	    localAm.laDatabase,
	    localAm.laMedFilesTable,
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    mediaRequestId,
	    AMF_RETRIEVED,
	    AMU_QUEUEING ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );

    

    /*
     * Return the amount of data that can be Queued, in kilobytes.
     */

    dbStatus = db_floatdata( dbproc, 1, &queuingValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	queuingValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    
    *returnValue += queuingValue;

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amNumPreparable
*
*   Purpose:
*	Determin the number of units that can be prepared for a particular
*	media request.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amNumPreparable
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    int		*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );

    
    /*
     * Query the database.
     */

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    count(*) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and status = '%c' \n",
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    AMU_QUEUED ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * Return the number of units that can be prepared.
     */

    dbStatus = db_intdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amPrepared
*
*   Purpose:
*	Determine whether a particular unit has been prepared.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amPrepared
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    char	*mediaUnitName,	/* (in)  Media Unit Name to match.	*/
    boolean	*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/
    int		dbValue;	/* Value returned from query. 		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );



    /*
     * Query the database.
     */
    
     AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    count(*) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and media_unit_name = '%s' \n"
	    "    and status in ( '%c', '%c' ) \n",
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    mediaUnitName,
	    AMU_PREPARED, AMU_WRITTEN ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * Return TRUE if the unit is prepared.
     */

    dbStatus = db_intdata( dbproc, 1, &dbValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	dbValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    
    *returnValue = ( ( dbValue == 1 ) ? TRUE : FALSE );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amNumWriteable
*
*   Purpose:
*	Determine the number of units that are ready for writing.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amNumWriteable
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    int		*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );


    /*
     * Query the database.
     */

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    count(*) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and status = '%c' \n",
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    AMU_PREPARED ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * Return the number of units ready for writing.
     */
    
    dbStatus = db_intdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amNumCopiesWritten
*
*   Purpose:
*	Determine the number of copies of a particular media unit was
*	written.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amNumCopiesWritten
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    char	*mediaUnitName,	/* (in)  Media Unit Name to match.	*/
    int		*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );



    /*
     * Query the database.
     */

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    copies_written \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and media_unit_name = '%s' \n",
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    mediaUnitName ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	*returnValue = 0;
	return( AM_SUCCESS );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * return the number of copies written.
     */

    dbStatus = db_intdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amNumCleanupable
*
*   Purpose:
*	Determine if there are any media units, and the number of media
*	units that can be cleaned up.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amNumCleanupable
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    int		*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );

    
    /*
     * Query that database.
     */
    
    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    count(*) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and status = '%c' \n",
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    AMU_WRITTEN ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * Return the number of units that can be cleaned up.
     */
    
    dbStatus = db_intdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amCleanedUp
*
*   Purpose:
*	Determine if a unit has been cleaned up.
**
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amCleanedUp
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    char	*mediaUnitName,	/* (in)  Media Unit Name to match.	*/
    boolean	*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/
    int		dbValue;	/* Value returned from query. 		*/


    AM_CHECK_INIT;


    AM_CHECK_DB( dbcanquery( dbproc ) );

    
    /*
     * Query the database.
     */

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    count(*) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and media_unit_name = '%s' \n"
	    "    and status = '%c' \n",
	    localAm.laDatabase,
	    localAm.laMedUnitsTable,
	    mediaRequestId,
	    mediaUnitName,
	    AMU_CLEANED_UP ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * return TRUE if the unit can be cleaned up.
     */

    dbStatus = db_intdata( dbproc, 1, &dbValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	dbValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }

    *returnValue = ( ( dbValue == 1 ) ? TRUE : FALSE );
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amNotifiable
*
*   Purpose:
*	Determine if a media is ready for notification.  For archive
*	requests when there is a media unit that has been cleaned up,
*	but not notified.  For user requests when all files have status
*	DONE.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amNotifiable
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    char	*mediaRequestType,	
    				/* (in)  Media Request type (ARCH or USER)*/
    boolean	*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from db.		*/
    int		dbValue;	/* Return value from db.		*/


    AM_CHECK_INIT;


    /*
     * Query the database.
     */

    AM_CHECK_DB( dbcanquery( dbproc ) );

    if ( streq( mediaRequestType, AM_RT_ARCHIVE ) )
    {
	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    count(*) \n"
		"from \n"
		"    %s..%s \n"
		"where \n"
		"    media_request_id = %d \n"
		"    and status = '%c' \n",
		localAm.laDatabase,
		localAm.laMedUnitsTable,
		mediaRequestId,
		AMU_CLEANED_UP ) );
    }
    else if ( streq( mediaRequestType, AM_RT_USER ) )
    {
	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    count(*) \n"
		"from \n"
		"    %s..%s \n"
		"where \n"
		"    media_request_id = %d \n"
		"    and status != '%c' \n",
		localAm.laDatabase,
		localAm.laMedFilesTable,
		mediaRequestId,
		AMF_DONE ) );
    }
    

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );

    dbStatus = db_intdata( dbproc, 1, &dbValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	dbValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }


    /*
     *  Notifications can be performed for ARCH if there is a cleaned-up
     *  unit, and for USER if there are no files which are not DONE. 
     */

    if ( streq( mediaRequestType, AM_RT_ARCHIVE ) )
    {
	*returnValue = ( ( dbValue > 0 ) ? TRUE : FALSE );
    }
    else if ( streq( mediaRequestType, AM_RT_USER ) )
    {
	*returnValue = ( ( dbValue == 0 ) ? TRUE : FALSE );
    }

    
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amAmountOnUnit
*
*   Purpose:
*	Determin the size of a media unit, specified by the media
*	request name and media request id. The value returned is in
*	kilobytes.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amAmountOnUnit
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  Media Request ID to match.	*/
    char	*mediaUnitName,	/* (in)  Media Unit Name to match.	*/
    int  	*returnValue	/* (out) The return value to set.	*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    /*
     * Query the database for the size, by summing the size of each file.
     */
    
    AM_CHECK_DB( dbcanquery( dbproc ) );


    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    sum( ( filesize ) ) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and media_unit_name = '%s' \n",
	    localAm.laDatabase,
	    localAm.laMedFilesTable,
	    mediaRequestId,
	    mediaUnitName ) );
    

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * Return the value found.
     */

    dbStatus = db_intdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amNumOnUnit
*
*   Purpose:
*	Determine the number of files on a media unit.
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int		amNumOnUnit
(
    DBPROCESS	*dbproc,	/* (in)  dbprocess to use.		*/
    int		mediaRequestId,	/* (in)  The media request to query.	*/
    char	*mediaUnitName,	/* (in)  The media unit name to query.	*/
    int		*returnValue	/* (out) The number of files on the unit.*/
)
{
    int		dbStatus;	/* Return status from the db		*/


    AM_CHECK_INIT;


    /*
     * Query the database for the number of files.
     */
    
    AM_CHECK_DB( dbcanquery( dbproc ) );


    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    count(* ) \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    media_request_id = %d \n"
	    "    and media_unit_name = '%s' \n",
	    localAm.laDatabase,
	    localAm.laMedFilesTable,
	    mediaRequestId,
	    mediaUnitName ) );
    

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );


    dbStatus = dbnextrow( dbproc );
    if ( dbStatus == NO_MORE_ROWS )
    {
	return( AM_E_DB );
    }

    AM_CHECK_DB( dbStatus );


    /*
     * Return the value found.
     */

    dbStatus = db_intdata( dbproc, 1, returnValue );
    if ( dbStatus == DB_NULL_VALUE )
    {
	*returnValue = 0;
    }
    else
    {
	AM_CHECK_DB_LIB( dbStatus );
    }
    

    return( AM_SUCCESS );
}
