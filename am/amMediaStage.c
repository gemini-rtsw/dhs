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
*   Module Name:	am/src/amMediaStage.c
*
*   Purpose:
*	The media_stages table is a 'registry' of the Media Data Stage
*	directories. There should be a one-to-one relationship between
*	(directory) and (media id & unit name). In this way, we ensure
*	that only one media unit is using a Media Data Stage directory
*	at one time - similar to a hotel registry with one booking per 
*	room. 
*
*   Routines:
*	int 	amMSBook	: Book a row in media_stages tbl.
*	int 	amMSClear	: Clear a row in media_stages tbl.
*	int 	amMSGet		: Get a row from media_stages tbl.
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: amMediaStage.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 03/05/99
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/02/23 SEC  :	Extensive makeover.
*	98/10/07 SDJ  : Inclueded gen_eptr.h due to changes in am.h.
*	99/03/05 NRH  : Added an error message to amMSBook.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>

#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"

#include "am.h"
#include "localAm.h"


/*+
************************************************************************
*
*   Function:	amMSGet
*
*   Purpose:
*	Fills in the struct pointed to by amMStage, provided the tuple 
*	exists in the db table that fills the criteria. 
*
*	Current functionality is for the select statement to match 
*	media ID and media name. 
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_NO_MSTAGE	: No tuple to return.
*
************************************************************************
-*/

int	amMSGet
(
    AM_STAGE_GET
    		selectType,	/* (in)  Type of query to execute.	*/
    AM_MSTAGE	*amMStage,	/* (out) Media Stage record to return.	*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...				/* (in)  The optional arguments.	*/
)
{
    va_list	args;		/* The variable-length argument list.	*/
    int		status;		/* Return status from the db.		*/


    AM_CHECK_INIT;


    if ( selectType != AM_SG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    directory, \n"
		"    media_request_id, \n"
		"    media_unit_name \n"
		"from \n"
		"    %s..%s \n",
		localAm.laDatabase,
		localAm.laMedStagesTable ) );
	
	switch ( selectType )
	{
	    case AM_SG_REQUEST_ID_NAME:
		va_start( args, dbproc );
		amMStage->amsMediaRequestId = (int) va_arg( args, int );
		(void) strcpy( amMStage->amsMediaUnitName,
			(char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			    "where \n"
			    "        media_request_id = %d \n"
			    "    and media_unit_name = '%s' \n",
			    amMStage->amsMediaRequestId,
			    amMStage->amsMediaUnitName ) );
		break;

	    case AM_SG_DIRECTORY:
		va_start( args, dbproc );
		(void) strcpy( amMStage->amsDirectory,
			(char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			    "where \n"
			    "    directory = '%s' \n",
			    amMStage->amsDirectory ) );
		break;

	    case AM_SG_ALL:
	    default:
		break;
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_MSTAGE );
    }
    
    AM_CHECK_DB( status );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 1,
	    amMStage->amsDirectory, sizeof( amMStage->amsDirectory ) ) );

    if ( ( status = db_intdata( dbproc, 2,
	    (int *) &( amMStage->amsMediaRequestId ) ) ) == DB_NULL_VALUE )
    {
	amMStage->amsMediaRequestId = -1;
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }

    if ( ( status = db_stringdata( dbproc, 3,
	    amMStage->amsMediaUnitName, 
	    sizeof( amMStage->amsMediaUnitName ) ) ) == DB_NULL_VALUE )
    {
	(void) sprintf( amMStage->amsMediaUnitName, "%s", "" );
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }


    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amMSBook
*
*   Purpose:
*	'Books' a staging directory by registering the media request id
*	and unit name in the media_stages directory. 
*
*   Values Returned:
*       int     AM_SUCCESS      : Normal completion.
*	int	AM_CANT_MSBOOK	: Error - directory already 'booked'. 
*       int     AM_E_DB         : Error in the sybase db library.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*	int	AM_E_MSTAGE_DIR	: The directory name is invalid.
*
************************************************************************
-*/

int		amMSBook
(
    AM_MSTAGE   *amMStage,	/* (in)  Media Stage record to insert.	*/
    DBPROCESS   *dbproc		/* (in)  The dbprocess to use.		*/
)
{
    int		status;		/* Return status from the db.		*/

    AM_CHECK_INIT;

    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	    "begin transaction" ) );

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    directory, \n"
	    "    media_request_id, \n"
	    "    media_unit_name \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    directory = '%s' \n",
	    localAm.laDatabase,
	    localAm.laMedStagesTable,
	    amMStage->amsDirectory ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	AM_CHECK_DB_LIB( db_simple_query( dbproc,
		"rollback transaction" ) );
	amFormatMessage( AM_E_MSTAGE_DIR, amMStage->amsDirectory, 
		localAm.laDatabase, localAm.laMedStagesTable );
	return( AM_E_MSTAGE_DIR );
    }

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "    directory, \n"
	    "    media_request_id, \n"
	    "    media_unit_name \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    directory = '%s' \n"
	    "    and media_request_id = NULL \n"
	    "    and media_unit_name = NULL \n",
	    localAm.laDatabase,
	    localAm.laMedStagesTable,
	    amMStage->amsDirectory ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	AM_CHECK_DB_LIB( db_simple_query( dbproc,
		"rollback transaction" ) );
	return( AM_CANT_MSBOOK );
    }

    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	    "update \n"
	    "    %s..%s \n"
	    "set \n"
	    "    media_request_id = %d, \n"
	    "    media_unit_name = '%s' \n"
	    "where \n"
	    "    directory = '%s' \n",
	    localAm.laDatabase,
	    localAm.laMedStagesTable,
	    amMStage->amsMediaRequestId,
	    amMStage->amsMediaUnitName,
	    amMStage->amsDirectory ) );

    while ( ( status = dbresults( dbproc ) ) == SUCCEED )
    {
    }
    AM_CHECK_DB( status );

    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	    "commit transaction" ) );
		
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amMSClear
*
*   Purpose:
*	Clears the row in the media stages table, effectively
*	'un-locking' that media staging directory for other media 
*	make processes to use. 
*
*   Values Returned:
*       int     AM_SUCCESS      : Normal completion.
*       int     AM_E_DB         : Error in the sybase db library.
*       int     AM_E_DB_LIB     : Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amMSClear
(
    AM_MSTAGE	*amMStage,	/* (in)  Media Stage record to delete.	*/
    DBPROCESS	*dbproc		/* (in)  The dbprocess to use.		*/
)
{
    AM_CHECK_INIT;

    AM_CHECK_DB( dbfcmd( dbproc,
	    "update\n"
	    "    %s..%s \n"
	    "set \n"
	    "    media_request_id = NULL, \n"
	    "    media_unit_name = NULL \n"
	    "where \n"
	    "    directory = '%s' \n"
	    "    and media_request_id = %d \n"
	    "    and media_unit_name = '%s' \n",
	    localAm.laDatabase,
	    localAm.laMedStagesTable,
	    amMStage->amsDirectory,
	    amMStage->amsMediaRequestId,
	    amMStage->amsMediaUnitName ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    return( AM_SUCCESS );
}
