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
*   Module Name:	am/src/amDevice.c
*
*   Purpose:
*	The book table is a 'registry' of the devices. The device name
*	must be unique in this table in order to ensure only one
*	process is using the device at any given time.
*
*   Routines:
*	int 	amDeviceBook		: Book a row in book tbl.
*	int 	amDeviceClear		: Clear a row(s) in book tbl.
*	int 	amDeviceGet		: Get a row from the book tbl.
*	int	amDeviceLookup	      	: Look up device name(s), given a 
*				  	  logical name.
*	int	amLogicalDeviceGet	: Get logical device from the database.
*	int 	amLogicalLookup      	: Look up logical name(s), given
*					  a device name
*
*   Date		: November 26, 1998
*
*   SCCS data           : @(#)
*       Module Name     : amDevice.c
*       Version Number  : 1.6
*       Release Number  : 1
*       Last Updated    : 01/13/00
*
*   Programmer		: Shannon Jaeger
*
*   Modification History:
*	98/11/26 SDJ  :	Moved above functions from am.c to make the
*			AM library more managable.
*	98/11/27 SDJ  : Reworked the booking queries to use the
*			"book" table.
*	98/12/01 SDJ  : Added amLogicalDeviceGet()
*	98/20/01 SDJ  : amDeviceBook() added device table to from clause.
*			Added "get types" to amDeviceClear() and
*			amDeviceGet().
*	99/04/09 SDJ  : Fixed amDevcieLookup() to use the media type when
*			it calls amDeviceGet().
*	99/05/26 NRH  : Added amLogicalDeviceGet. Fixed a bug in amDeviceClear
*			when clearing based on logical device.
*	99/12/01 SDJ  : Added AM_LDG_DNAME "get type" to amLogicalDeviceGet().
*     2000/01/13 SDJ  : Fixed parameters for formatMessage for AM_E_NOT_BOOKED.
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
*   Function:	amDeviceBook
*
*   Purpose:
*	Attempts to book a device(s).
*
*
*   Values Returned:
*	int	AM_SUCCESS	: Normal completion.
*	int	AM_E_NOT_BOOKED	: Device can not be booked.
*	int 	AM_E_DB		: Error in SYBASE db library.
*	int	AM_E_DB_LIB	: Error in CADC db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int		amDeviceBook
(
    const char	*amLogicalDeviceName,
    				/* (in) Logical device to be booked.	*/
    boolean	force,		/* (in)  Force the booking.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    AM_DEVICE	*amDevicePtr	/* (mod) The device to book.		*/
)
{
    int		numRows;	/* Num. rows affected by update.	*/
    int		status;		/* Return status from the db            */
        

    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );

    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	    "begin transaction" ) );
    

    /*
     * Make sure there is something to update.
     */

     AM_CHECK_DB( dbfcmd( dbproc,
	    "select book.deviceName \n"
	    "from %s..%s book,\n"
	    "	%s..%s dev \n"
	    "where \n"
	    "	book.deviceName = '%s' \n"
	    "   and dev.deviceName = book.deviceName \n"
	    "	and dev.available = 'Y' ",
	    localAm.laDatabase, localAm.laBookingTable,
	    localAm.laDatabase, localAm.laDeviceTable,
	    amDevicePtr->amdDeviceName ) );
    
     if ( ! force )
     {
	AM_CHECK_DB( dbfcmd( dbproc,"\n	and book.inUse != 'Y' "	) );
     }

     AM_CHECK_DB( dbsqlexec( dbproc ) );
     AM_CHECK_DB( dbresults( dbproc ) );
     if ( ( numRows =  DBROWS( dbproc ) ) != 1 )
     {	 
	 AM_CHECK_DB_LIB( db_simple_query( dbproc,
		 "rollback transaction" ) );
	 amFormatMessage( AM_E_NOT_BOOKED, amDevicePtr->amdDeviceName,
		 numRows );
	 return( AM_E_NOT_BOOKED );
     }
     while( ( status = dbnextrow( dbproc ) ) != NO_MORE_ROWS )
     {
     }


    /*
     *	Set the "inUse" flag and other fields in the "book" table
     *  if and only if we are forcing the booking or the device is
     *	not currently booked.  This query locks the tables during the
     *	update to ensure correctness.
     */

     AM_CHECK_DB( dbfcmd( dbproc,
	    "update %s..%s \n"
	    "set inUse = 'Y', \n"
	    "	processId = %d, \n"
	    "	processName = '%s', \n"
	    "	media_request_id = %d, \n"
	    "	media_unit_name = '%s', \n"
	    "	timeStamp = getDate(), \n"
	    "	logicalName = '%s' \n"
	    "from %s..%s book,\n"
	    "	%s..%s dev \n"
	    "where \n"
	    "	book.deviceName = '%s' \n"
	    "	and dev.available = 'Y' ",
	    localAm.laDatabase, localAm.laBookingTable,
	    amDevicePtr->amdProcessId, amDevicePtr->amdProcessName, 
	    amDevicePtr->amdMediaRequestId, amDevicePtr->amdMediaUnitName,
	    amLogicalDeviceName,
	    localAm.laDatabase, localAm.laBookingTable,
	    localAm.laDatabase, localAm.laDeviceTable,
	    amDevicePtr->amdDeviceName ) );

    if ( ! force )
    {
	AM_CHECK_DB( dbfcmd( dbproc,"\n	and book.inUse != 'Y' " ) );
    }
      
    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );
    while( ( status = dbnextrow( dbproc ) ) == MORE_ROWS )
    {
    }
    AM_CHECK_DB( status );

     
    /*
     * Update the device information.
     */
    
    amDevicePtr->amdInUse = TRUE;
    AM_CHECK_DB( dbfcmd( dbproc,
	    "select datediff( second, 'jan 1 1970', timeStamp ) \n"
	    "from %s..%s \n"
	    "where deviceName = '%s' \n",
	    localAm.laDatabase, localAm.laBookingTable,
	    amDevicePtr->amdDeviceName ) );
    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	amDevicePtr->amdTimeStamp = -1;
    }
    else if ( ( status = db_intdata( dbproc, 1,
	    (int *) &( amDevicePtr->amdTimeStamp ) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdTimeStamp = -1;
    }
    else
    {
	AM_CHECK_DB_LIB( status );
    }
    

    AM_CHECK_DB_LIB( db_simple_query( dbproc, "commit transaction" ) );
      
    
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amDeviceClear
*
*   Purpose:
*	Clears a booked device.
*
*	amDeviceClear( AM_DC_ALL, DBPROCESS * );
*
*	amDeviceClear( AM_DC_DEVICE, DBPROCESS *, AM_DEVICE );
*
*	char amLiceName[AM_LOGICAL_DEVICE_NAME_LEN]
*	amDeviceClear( AM_DC_LOGICAL, DBPROCESS *, amLiceName );
*
*
*   Values Returned:
*	int	AM_SUCCESS		: Normal completion.
*	int 	AM_E_DB			: Error in SYBASE db library.
*	int	AM_E_DB_LIB		: Error in CADC db library.
*	int	AM_E_INIT		: Library is not initialized.
*	int	AM_E_BOOKCLEAR_ENUM	: Bad type given.
*
*   WARNING: This function does not do any checks to ensure the
*	     given porcess is indeed the one that booked it for
*	     types AM_DC_DEVICE and AM_DC_ALL 
*
************************************************************************
-*/

int		amDeviceClear
(
    AM_DEVICE_CLEAR
    		selectType,	/* (in)	 Type of selection.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...
)
{
    va_list	args;	    	/* The variable-length argument list.	*/
    AM_DEVICE	*amDevicePtr;   /* AM device structure.			*/
    char	amLogicalDeviceName[AM_LOGICAL_DEV_NAME_LEN];
    				/* Name of a logical device. 		*/

    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );
    
    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	    "begin transaction" ) );


    /*
     * Set all fields to NULL except the deviceName and the inUse
     * value which is set to "N".
     */

    AM_CHECK_DB( dbfcmd( dbproc, 
	    "update \n"
	    "   %s..%s \n"
	    "set \n"
	    "    inUse='N', \n"
	    "    processName=NULL, \n"
	    "    processId=NULL, \n"
	    "    media_request_id=NULL, \n"
	    "    media_unit_name=NULL, \n"
	    "    timeStamp=NULL, \n"
	    "    logicalName=NULL ",
	localAm.laDatabase, localAm.laBookingTable ) ); 

    switch( selectType )
    {
	case AM_DC_ALL:
	    break;

	case AM_DC_DEVICE:
	    va_start( args, dbproc );
	    amDevicePtr = (AM_DEVICE *) va_arg( args, AM_DEVICE * );
	    va_end( args );

	    AM_CHECK_DB( dbfcmd( dbproc,
    		    "\nwhere \n"
		    "   deviceName='%s' \n"
		    "   and processId=%d \n"
		    "   and processName='%s' \n"
		    "   and media_request_id=%d \n"
		    "   and media_unit_name='%s' ",
		    amDevicePtr->amdDeviceName,
		    amDevicePtr->amdProcessId,
		    amDevicePtr->amdProcessName,
		    amDevicePtr->amdMediaRequestId,
		    amDevicePtr->amdMediaUnitName ) );
	    break;

	case AM_DC_LOGICAL:
	    va_start( args, dbproc );
	    (void)strcpy( amLogicalDeviceName, 
		    (char *) va_arg( args, char * ) );
	    va_end( args );

	    AM_CHECK_DB( dbfcmd( dbproc,
		    "\nwhere \n"
		    "	 logicalName='%s' ",
		    amLogicalDeviceName ) );
	    break;

	default:
	    amFormatMessage( AM_E_BOOKCLEAR_ENUM, selectType );
	    AM_CHECK_DB_LIB( db_simple_query( dbproc,
		    "rollback transaction" ) );
	    return( AM_E_BOOKCLEAR_ENUM );
    }
    
    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    
    
    AM_CHECK_DB_LIB( db_simple_query( dbproc,
	"commit transaction" ) );


    if ( selectType == AM_DC_DEVICE )
    {
	/*
	 * Update the device information.
	 */

	amDevicePtr->amdProcessId		= -1;
	amDevicePtr->amdProcessName[0]	='\0';
	amDevicePtr->amdMediaRequestId	= -1;
	amDevicePtr->amdMediaUnitName[0]	= '\0';
	amDevicePtr->amdTimeStamp 		= -1;
	amDevicePtr->amdInUse		= 'N';
    }
    
    
    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amDeviceGet
*
*   Purpose:
*       Gets a specified device or list of devices from the device table.
*       If more than one file matches the specifiecation, the first device is 
*       returned with the first call to this function, and any others should
*       be retrieved by calling this function with a select_type of
*       AM_DG_NEXT.
*
*       char    *deviceName;
*       amDeviceGet( AM_DG_NAME, AM_DEVICE *, DBPROCESS *, deviceName );
*
*       amDeviceGet( AM_DG_ALL, AM_DEVICE *, DBPROCESS * );
*
*       char    *mediaType;
*       amDeviceGet( AM_DG_TYPE, AM_DEVICE *, DBPROCESS *, mediaType );
*
*       char    *deviceName, *mediaType;
*       amDeviceGet( AM_DG_NAME_TYPE, AM_DEVICE *, DBPROCESS *, deviceName, 
*			mediaType );
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_NO_DEVICE	: No devices to return.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_DEVICEGET_ENUM
*				: Device get type is not valid.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amDeviceGet
(
    AM_DEVICE_GET
    		selectType,	/* (in)  Type of selection.		*/
    AM_DEVICE	*amDevicePtr,	/* (out) Device's record.		*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...				/* (in)  The optional arguments.	*/
)
{
    va_list	args;		/* The variable-length argument list	*/
    int		dbStatus;	/* Return status from db lib.		*/
    char	inUseFlag;	/* Device online/offline?		*/
    char	onlineFlag;	/* Device online/offline?		*/
    int		status;		/* Return status from the db.		*/

    AM_CHECK_INIT;


    /*
     * Join the device and mediaTypes table to gather all the information
     * which describes a particular device.
     */

    if ( selectType != AM_DG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select \n"
		"    dev.deviceName, \n"
		"    driverName, \n"
		"    deviceInfo, \n"
		"    media_type, \n"
		"    rSpeed, \n"
		"    wSpeed, \n"
		"    available, \n"
		"    inUse, \n"
		"    processId, \n"
		"    processName, \n"
		"    media_request_id, \n"
		"    media_unit_name, \n"
		"    datediff( second, 'jan 1 1970', timeStamp )"
		"from \n"
		"    %s..%s dev, \n"
		"    %s..%s book \n"
		"where \n"
		"   dev.deviceName = book.deviceName \n",
		localAm.laDatabase, localAm.laDeviceTable, 
		localAm.laDatabase, localAm.laBookingTable ) );
	

	switch ( selectType )
	{
	    case AM_DG_ALL:
		break;

	    case AM_DG_NAME:
		va_start( args, dbproc );
		(void) strcpy( amDevicePtr->amdDeviceName,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    and dev.deviceName = '%s' \n",
			amDevicePtr->amdDeviceName ) );
		break;

	    case AM_DG_NAME_TYPE:
		va_start( args, dbproc );
		(void) strcpy( amDevicePtr->amdDeviceName,
		        (char *) va_arg( args, char * ) );
		(void) strcpy( amDevicePtr->amdType,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    and dev.deviceName = '%s' \n"
			"    and media_type = '%s' \n",
			amDevicePtr->amdDeviceName, amDevicePtr->amdType ) );
		break;
	    case AM_DG_TYPE:
		va_start( args, dbproc );
		(void) strcpy( amDevicePtr->amdType,
		        (char *) va_arg( args, char * ) );
		va_end( args );

		AM_CHECK_DB( dbfcmd( dbproc,
			"    and media_type = '%s' \n",
			amDevicePtr->amdType ) );
		break;

	    default:
		amFormatMessage( AM_E_DEVICEGET_ENUM, selectType );
		return( AM_E_DEVICEGET_ENUM );
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }

    
    /*
     * Process the results.
     */
     
    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_DEVICE );
    }
    AM_CHECK_DB( status );

    
    AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, amDevicePtr->amdDeviceName,
	    sizeof( amDevicePtr->amdDeviceName ) ) );
    
    if( ( dbStatus = db_stringdata( dbproc, 2,  amDevicePtr->amdDeviceDriver,
	    sizeof( amDevicePtr->amdDeviceDriver ) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdDeviceDriver[0] = '\0';
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );
	

   if ( ( dbStatus =  db_stringdata( dbproc, 3, amDevicePtr->amdDeviceInfo,
	   sizeof( amDevicePtr->amdDeviceInfo ) ) ) == DB_NULL_VALUE )
   {
       amDevicePtr->amdDeviceInfo[0] = '\0';
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );       


    AM_CHECK_DB_LIB( db_stringdata( dbproc, 4, amDevicePtr->amdType,
	    sizeof( amDevicePtr->amdType ) ) );

   if ( ( dbStatus = db_intdata( dbproc, 5,
	   (int *) &(amDevicePtr->amdRSpeed ) ) ) == DB_NULL_VALUE )
   {
       amDevicePtr->amdRSpeed = -1;
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );

    if ( ( dbStatus = db_intdata( dbproc, 6,
	    (int *) &(amDevicePtr->amdWSpeed ) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdWSpeed = -1;
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );
   

    AM_CHECK_DB_LIB( db_chardata( dbproc, 7, &onlineFlag ) );
    if ( onlineFlag == 'Y' )
    {
	amDevicePtr->amdOnline = TRUE;
    }
    else
    {
	amDevicePtr->amdOnline = FALSE;
    }

    AM_CHECK_DB_LIB( db_chardata( dbproc, 8, &inUseFlag ) );
    if ( inUseFlag == 'Y' )
    {
	amDevicePtr->amdInUse = TRUE;
    }
    else
    {
	amDevicePtr->amdInUse = FALSE;
    }

    if ( ( dbStatus = db_intdata( dbproc,9,
	    (int*) &(amDevicePtr->amdProcessId) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdProcessId = -1;;
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );

    if ( ( dbStatus = db_stringdata( dbproc, 10, amDevicePtr->amdProcessName,
	    sizeof( amDevicePtr->amdProcessName ) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdProcessName[0] = '\0';
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );
	
    if ( ( dbStatus = db_intdata( dbproc, 11,
	    &(amDevicePtr->amdMediaRequestId) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdMediaRequestId = -1;
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );
       
    if ( ( dbStatus = db_stringdata( dbproc, 12, amDevicePtr->amdMediaUnitName,
	    sizeof( amDevicePtr->amdMediaUnitName ) ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdMediaUnitName[0] = '\0';
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );	

    if ( ( dbStatus = db_intdata( dbproc, 13,
	    (int*)&amDevicePtr->amdTimeStamp ) ) == DB_NULL_VALUE )
    {
	amDevicePtr->amdTimeStamp = -1;
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amDeviceLookup
*
*   Purpose:
*       Gets a list of device names that correspond to a logical name.
*
*  WARNING, if this encounters an error, it just returns and does not free
*  up the memory allocated to the eptr structure.
*
*   Values Returned:
*	int	AM_SUCCESS	: Successful execution.
*	int	AM_NO_DEVICE	: No devices to return.
*	int	AM_E_DB		: Error in the sybase db library.
*	int	AM_E_DB_LIB	: Error in the cadc db library.
*	int	AM_E_INIT	: Library is not initialized.
*
************************************************************************
-*/

int	amDeviceLookup
(
    const char	*logicalName,	/* (in)  Name of logical device.	*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    const char	*mediaType,	/* (in)  Media type to check for.	*/
    struct eptr **deviceList	/* (out) Returned eptr list of names.	*/
)
{
    AM_DEVICE	*amDevPtr;	/* Pointer to a device.			*/
    
    char	*deviceNamePtr; /* Name of a device.                    */
    int         index;		/* Index into eptr list.		*/
    struct eptr *nameList;      /* List of device names found.	*/
    int		status;		/* Return status from the db		*/


    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );


    /*
     * Get the list of devices associated with the logical name.
     */

    AM_CHECK_NULL( nameList = eptr_alloc( 5 ) );
    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "   distinct deviceName \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    logicalName = '%s' \n",
	    localAm.laDatabase, localAm.laLogicalTable,
	    logicalName ) );

    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );

    
    while ( (status = dbnextrow( dbproc )) != NO_MORE_ROWS )
    {
	AM_CHECK_DB( status );


	AM_CHECK_NULL( deviceNamePtr = char_alloc( AM_DEVICE_NAME_LEN  ) );
	AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, deviceNamePtr,
		AM_DEVICE_NAME_LEN ) );
	AM_CHECK_NULL( nameList = eptr_insert( nameList, eptrlen( nameList ),
		deviceNamePtr ) );
    }


    /*
     * Loop through the device names and get their complete description.
     */

    AM_CHECK_NULL( *deviceList );
    for( index = eptrlen( nameList ) - 1; index >= 0; index-- )
    {
	AM_CHECK_NULL( deviceNamePtr = eptritem( nameList, index ) );
	AM_CHECK_NULL( amDevPtr = gen_alloc( sizeof( AM_DEVICE ) ) );
	AM_CHECK( amDeviceGet( AM_DG_NAME_TYPE, amDevPtr, dbproc,
		deviceNamePtr, mediaType ) );
	AM_CHECK_NULL( *deviceList = eptr_insert( *deviceList,
		eptrlen( *deviceList ), amDevPtr ) );
    }
    AM_CHECK_NULL( *deviceList = eptr_sort( *deviceList, devCmp ) );
    

    if ( nameList != NULL )
    {
	eptr_free( nameList, free );
	nameList = NULL;
    }
    

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amLogicalDeviceGet
*
*   Purpose:
*	Get a list of logical devices.
*
*   Values Returned:
*	int	AM_E_LOG_DEVGET_ENUM
*				: Invalid logical device get type.
*	int	AM_E_DB		: Database error.
*	int	AM_E_DB_LIB	: CADC database library error.
*
************************************************************************
-*/

int		amLogicalDeviceGet
(
    AM_LOGICAL_DEVICE_GET	/* (in)  The type of get being done.	*/
		selectType,		 
    AM_LOGICAL_DEVICE		/* (mod) The logical device structure	*/
		*pLogicalDevice,/*	 to populate.			*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    ...
)
{
    va_list	args;		/* The variable-length argument list.	*/
    char	amDeviceName[AM_DEVICE_NAME_LEN];
       				/* Name of a device.			*/
    int		dbStatus;	/* database fucntion return status.	*/
    boolean	flag;		/* Flag from the database.		*/
    char	logicalBookDevice[ AM_LOGICAL_DEV_NAME_LEN ];
    				/* logical device name.			*/
    int		status;		/* function return status.		*/


    /*
     * Issue a query to get a list of logical devices from the database.
     * The unique key on logical device would be "logical device name,
     * media type".
     */

    if ( selectType != AM_LDG_NEXT )
    {
	AM_CHECK_DB( dbcanquery( dbproc ) );

	AM_CHECK_DB( dbfcmd( dbproc,
		"select distinct\n"
		"    l.logicalName, \n"
		"    d.media_type, \n"
		"    max( m.capacity ), \n"
		"    max( m.description ), \n"
		"    min( d.available ), \n"
		"    max( b1.inUse ), \n"
		"    max( b1.media_request_id ), \n"
		"    max( b1.media_unit_name ), \n"
		"    max( b1.logicalName ), \n"
		"    count(*) \n"
		"from \n"
		"    %s..%s d, \n"
		"    %s..%s b1, \n"
		"    %s..%s l, \n"
		"    %s..%s m \n"
		"where \n"
		"    d.deviceName = b1.deviceName\n"
		"    and m.media_type = d.media_type\n"
		"    and l.deviceName = d.deviceName\n",
		localAm.laDatabase, localAm.laDeviceTable, 
		localAm.laDatabase, localAm.laBookingTable,
		localAm.laDatabase, localAm.laLogicalTable,
		localAm.laDatabase, localAm.laMedTypesTable ) );
	

	switch ( selectType )
	{
	    case AM_LDG_DNAME:
		va_start( args, dbproc );
		(void) strcpy( amDeviceName, (char*)va_arg( args, char * ) );
		va_end( args );
		
		AM_CHECK_DB( dbfcmd( dbproc,
			"and l.deviceName = '%s'\n"
			"group by \n"
			"    l.logicalName, d.media_type \n"
			"having \n"
			"    d.deviceName = b1.deviceName and \n"
			"    m.media_type = d.media_type and \n"
			"    l.deviceName = d.deviceName \n",
			amDeviceName ) );
		break;

	    case AM_LDG_ALL:
		AM_CHECK_DB( dbfcmd( dbproc,
			"group by \n"
			"    l.logicalName, d.media_type \n"
			"having \n"
			"    d.deviceName = b1.deviceName and \n"
			"    m.media_type = d.media_type and \n"
			"    l.deviceName = d.deviceName \n" ) );
		break;

	    default:
		amFormatMessage( AM_E_LOG_DEVGET_ENUM, selectType );
		return( AM_E_LOG_DEVGET_ENUM );
	}

	AM_CHECK_DB( dbsqlexec( dbproc ) );
	AM_CHECK_DB( dbresults( dbproc ) );
    }

    
    /*
     * Process the results.
     */
     
    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_DEVICE );
    }
    AM_CHECK_DB( status );

    
    AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, pLogicalDevice->amldName,
	    sizeof( pLogicalDevice->amldName ) ) );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 2, pLogicalDevice->amldMediaType,
	    sizeof( pLogicalDevice->amldMediaType ) ) );
    
    AM_CHECK_DB_LIB( db_intdata( dbproc, 3,
	    (int *) &(pLogicalDevice->amldCapacity ) ) );

    AM_CHECK_DB_LIB( db_stringdata( dbproc, 4, 
	    pLogicalDevice->amldMediaTypeDesc,
	    sizeof( pLogicalDevice->amldMediaTypeDesc ) ) );

    AM_CHECK_DB_LIB( db_chardata( dbproc, 5, &flag ) );
    if ( flag == 'Y' )
    {
	pLogicalDevice->amldAvailable = TRUE;
    }
    else
    {
	pLogicalDevice->amldAvailable = FALSE;
    }

    AM_CHECK_DB_LIB( db_chardata( dbproc, 6, &flag ) );
    if ( flag == 'Y' )
    {
	pLogicalDevice->amldInUse = TRUE;
    }
    else
    {
	pLogicalDevice->amldInUse = FALSE;
    }

    if ( ( dbStatus = db_intdata( dbproc, 7,  
	    &( pLogicalDevice->amldMediaRequestId ) ) ) == DB_NULL_VALUE )
    {
	pLogicalDevice->amldMediaRequestId = -1;
	dbStatus = DB_SUCCESS;
    }
    AM_CHECK_DB_LIB( dbStatus );
	

    if ( ( dbStatus =  db_stringdata( dbproc, 8, 
	    pLogicalDevice->amldMediaUnitName,
	    sizeof( pLogicalDevice->amldMediaUnitName ) ) ) == DB_NULL_VALUE )
    {
	pLogicalDevice->amldMediaUnitName[0] = '\0';
	dbStatus = DB_SUCCESS;
    }

    AM_CHECK_DB_LIB( dbStatus );       
    if ( ( dbStatus =  db_stringdata( dbproc, 9, logicalBookDevice, 
	    sizeof( logicalBookDevice ) ) ) == DB_NULL_VALUE )
    {
	dbStatus = DB_SUCCESS;
    }
    else
	if ( strcmp( logicalBookDevice, "" ) != 0 && 
	    strcmp( logicalBookDevice, pLogicalDevice->amldName ) != 0 )
    {
	pLogicalDevice->amldMediaUnitName[0] = '\0';
	pLogicalDevice->amldMediaRequestId = -1;
    }
    AM_CHECK_DB_LIB( dbStatus );       


    AM_CHECK_DB_LIB( db_intdata( dbproc, 10,
	    (int *) &(pLogicalDevice->amldNumDevices ) ) );

    return( AM_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	amLogicalLookup
*
*   Purpose:
*	To retrieve logical device(s) that is associated with a particular
*	device.  The logical device with the fewest devicess associated
*	with it is given first.
*
*   Values Returned:
*	int	AM_SUCCESS		: Successful execution.
*	int	AM_NO_LOGICAL_DEV	: No devices to return.
*	int	AM_E_DB			: Error in the sybase db library.
*	int	AM_E_DB_LIB		: Error in the cadc db library.
*	int	AM_E_INIT		: Library is not initialized.
*
************************************************************************
-*/

int	amLogicalLookup
(
    char	*deviceName,	/* (in)  Device name to look for.	*/
    DBPROCESS	*dbproc,	/* (in)  The dbprocess to use.		*/
    struct eptr	**logicalDevList 
    				/* (out) Logical device name(s) found.	*/
)
{
    char		*logDevPtr;	/* Logical Device Pointer.	*/
    int			status;		/* Return status from the db.	*/
	
    AM_CHECK_INIT;

    AM_CHECK_DB( dbcanquery( dbproc ) );

    AM_CHECK_DB( dbfcmd( dbproc,
	    "select \n"
	    "	distinct deviceName, \n"
	    "from \n"
	    "	%s..%s, \n"
	    "where \n"
	    "   deviceName = '%s' \n",
	    localAm.laDatabase, localAm.laLogicalTable,
	    deviceName ) );
    
    AM_CHECK_DB( dbsqlexec( dbproc ) );
    AM_CHECK_DB( dbresults( dbproc ) );
    status = dbnextrow( dbproc );
    if ( status == NO_MORE_ROWS )
    {
	return( AM_NO_LOGICAL_DEV );
    }

    while ( (status = dbnextrow( dbproc )) != NO_MORE_ROWS )
    {
	AM_CHECK_NULL( logDevPtr = char_alloc( AM_LOGICAL_DEV_NAME_LEN ) );
	
	AM_CHECK_DB_LIB( db_stringdata( dbproc, 1, logDevPtr,
		AM_LOGICAL_DEV_NAME_LEN ) );

	AM_CHECK_NULL( *logicalDevList = eptr_insert( *logicalDevList,
		eptrlen( *logicalDevList ), logDevPtr ) );
    }
    
    AM_CHECK_NULL( *logicalDevList = eptr_sort( *logicalDevList, strcmp ) );

    return( AM_SUCCESS );
}
