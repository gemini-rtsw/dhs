static char rcsid[] = "$Id: stoDevice.C,v 1.1.1.1 2002-11-24 20:31:52 brighton Exp $";
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
// dhsStorage/src/stoDevice.C
//
// PURPOSE:
// Contains the implementation of the cStoDevice class.
//
// METHOD NAME(S)
// cStoDevice::cStoDevice	- Constructor for the cStoDevice class.
// cStoDevice::~cStoDevice	- Destructor for the cStoDevice class.
// cStoDevice::book		- Book a logical device.
// cStoDevice::deviceSelect	- Select the most appropriate logical device.
// cStoDevice::deviceSet	- Set the information associated with a device.
// cStoDevice::find		- Find a logical device given its name and
//				  type.
// cStoDevice::init		- Initialize the objects in the cStoDevice 
//				  class.
// cStoDevice::inUseUpdate	- Query the database to get the current value
//				  of the inUse flag, and update the inUse value.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.3  2000/12/19 19:34:21  nhill
// Updated for Workshop verison 5.
//
// Revision 1.2  1999/10/12 22:54:53  nhill
// Fixed some comments.
//
// Revision 1.1  1999/06/17 00:01:30  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <sybfront.h>
#include <sybdb.h>
extern "C"
{
#include "gen_types.h"
#include "gen_eptr.h"
}

#include "globals.H"
#include "stoDbm.H"
#include "stoDevice.H"


cStoEpicsRecord<int, cStoDevice, cStoStatus>
	cStoDevice::sdArraySize( STO_ERD_DEVICE_ARRAY_SIZE, STO_MAX_DEVICES_EPICS_ARRAY, NULL, NULL );
						// Device array size.

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::cStoDevice
//
// INVOCATION:
// new cStoDevice( i );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) row (int) The row in the EPICS record array for this device.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize an empty device structure
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cStoDevice::cStoDevice
(
    int		row		// (in)  Row in the EPICS record array.
):	cListOf<cStoDevice, int, cStoStatus > ( row, true ),
	sdEpicsArrayRow( row ),
	sdEpicsRecInUse( row, STO_ERD_IN_USE, false, this, NULL ),
	sdAvailable( row, STO_ERD_AVAILABLE, false, this, NULL ),
	sdCapacity( row, STO_ERD_CAPACITY, false, 0, NULL ),
	sdDeviceInUse( row, STO_ERD_DEVICE_IN_USE, false, this, NULL ),
	sdLogicalName( row, STO_ERD_LOGICAL_NAME, "", this, NULL ),
	sdNumDevices( row, STO_ERD_NUM_DEVICES, 0, this, NULL ),
	sdRequestId( row, STO_ERD_REQUEST_ID, 0, this, NULL ),
	sdType( row, STO_ERD_TYPE, "", this, NULL ),
	sdTypeDesc( row, STO_ERD_TYPE_DESC, "", this, NULL ),
	sdUnitName( row, STO_ERD_UNIT_NAME, "", this, NULL )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::~cStoDevice
//
// INVOCATION:
// N.a Called automatically when the object is destroyed.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the EPICS row to unused.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cStoDevice::~cStoDevice
(
    void
)
{
    sdEpicsRecInUse = false;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::book
//
// INVOCATION:
// device.book( false, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) force (bool) Force booking even if the device is already booked?
// (!) status (cStoStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Book a device in the database
//
// DESCRIPTION:
// - Get a list of physical devices associated with the logical device.
// - Update the database to indicate each of the physical devices is busy.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDevice::book
(
    const bool	force,		// (in)  Force booking even if the device is
    				//	 busy?
    cStoStatus	&status		// (mod) Function return status.
)
{
    int		amStatus;
    DBPROCESS	*dbProcess;	// DB process to use.
    int		i;
    AM_DEVICE	*pAmDevice;	// Device from the am library.
    EPTR	*pDeviceList;	// List of AM devices that are associated
				// with a logical device.


    checkStat( status, return );


    //
    // Get a list of AM_DEVICEs associated with this logical device.
    //

    checkNull( pDeviceList = eptr_alloc( 10 ), status, return );
    dbProcess = cStoDbManager::acquire();
    checkAm( amDeviceLookup( sdLogicalName.value().data(), dbProcess, 
	    sdType.value().data(), &pDeviceList ), status,  VOID );


    if ( status.ok() )
    {
	//
	// Mark each of the physical devices associated with this logical
	// device as being in use.
	//

	for ( i = 0; i < eptrlen( pDeviceList ); i++ )
	{
	    pAmDevice = (AM_DEVICE *) eptritem( pDeviceList, i );
	    amStatus = amDeviceBook( sdLogicalName.value().data(), force, 
		    dbProcess, pAmDevice );
	}
    }


    cStoDbManager::release();


    //
    // Free the eptr list.
    //

    eptr_free( pDeviceList, (pfv) ::free );


    if ( amStatus == AM_SUCCESS )
    {
	return;
    }
    else if ( amStatus == AM_E_NOT_BOOKED )
    {
	status.E_DEV_NOT_BOOKED( status, sdLogicalName.value().data() );
    }
    else
    {
	checkAm( amStatus, status, VOID );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::deviceSelect
//
// INVOCATION:
// pDevice = cStoDevice::deviceSelect( mediaType, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaType (const char *) The type of the media to be written.
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// (cStoDevice *) The best logical device to use to write the media.
//
// PURPOSE:
// Select the best logical device to use to write a media type
//
// DESCRIPTION:
// Loop through the list of devices, attemping to find the best one that
// is able to write the required media type.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cStoDevice	*cStoDevice::deviceSelect
(
    const char	*mediaType,	// (in)  The media type to be written.
    cStoStatus	&status		// (mod) Function return status.
)
{
    cStoDevice::iObjectList	// Iterator on the list of logical devices.
    		iDevice;
    cStoDevice	*pDevice;	// Pointer to a logical device.
    cStoDevice	*retDevice;	// The best logical device seen yet.


    //
    // Loop over each of the logical devices in the list of logical devices.
    //

    cStoDevice::listRLock();
    for ( retDevice = NULL, iDevice = cStoDevice::begin(); 
	    iDevice != cStoDevice::end(); iDevice++ )
    {
	pDevice = (*iDevice).second;


	//
	// Check to see that:
	// - the device is a valid device.
	// - the device is available.
	// - the device is not in use.
	// - the device can write the specified type.
	// 

	if ( pDevice->sdEpicsRecInUse.value() && 
		pDevice->sdAvailable.value() &&
		! pDevice->sdDeviceInUse.value () &&
		pDevice->sdType.value() == mediaType )
	{
	    //
	    // If pDevice is a beter device than retDevice, use pDevice
	    // instead. "Better" is defined as (in order of priority):
	    //   - retDevice hasn't been assigned yet.
	    //   - pDevice uses a smaller number of physical devices than
	    //     retDevice.
	    //   - pDevice is alphabetically smaller that ret-device.
	    //

	    if ( retDevice == NULL ||
		    retDevice->sdNumDevices.value() > 
		    pDevice->sdNumDevices.value() ||
		    ( retDevice->sdNumDevices.value() == 
		    pDevice->sdNumDevices.value() && 
		    retDevice->sdLogicalName.value() > 
		    pDevice->sdLogicalName.value() ) )
	    {
		retDevice = pDevice;
	    }
	}
    }
    cStoDevice::listUnlock();


    //
    // Check that a device was selected.
    //

    if ( retDevice == NULL )
    {
	status.E_NO_DEVICE( status, mediaType );
    }

    return( retDevice );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::deviceSet
//
// INVOCATION:
// cStoDevice::deviceSet( logicalDevice, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) logicalDevice (AM_LOGICAL_DEVICE) The am logical device information.
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the data members of a free cStoStatus object based on the contents
// of an AM_LOGICAL_DEVICE object
//
// DESCRIPTION:
// - Find an unused device object.
// - Assign the data from the AM_LOGICAL_DEVICE structure to the class data
//   members.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDevice::deviceSet
(
    AM_LOGICAL_DEVICE		// (in)  The logical device information.
		&logicalDevice,
    cStoStatus	&status		// (mod) Function return status.
)
{
    cStoDevice	*pDevice;	// The cStoDevice object being initialized.
    cStoDevice::iObjectList	// Iterator in the list of devices.
    		iDevice;


    //
    // Find an unused device object.
    //

    for ( pDevice = NULL, iDevice = cStoDevice::begin(); 
	    iDevice != cStoDevice::end(); iDevice++ )
    {
	if ( ! (*iDevice).second->sdEpicsRecInUse.value() )
	{
	    pDevice = (*iDevice).second;
	    break;
	}
    }
    if ( pDevice == NULL )
    {
	status.E_DEVICE_OVERFLOW( status );
	return;
    }


    //
    // Assign the data from the structure to the class data members.
    //

    pDevice->sdAvailable = logicalDevice.amldAvailable;
    pDevice->sdCapacity = logicalDevice.amldCapacity;
    pDevice->sdDeviceInUse = logicalDevice.amldInUse;
    pDevice->sdEpicsRecInUse = true;
    pDevice->sdLogicalName = logicalDevice.amldName;
    pDevice->sdNumDevices = logicalDevice.amldNumDevices;
    pDevice->sdType = logicalDevice.amldMediaType;
    pDevice->sdTypeDesc = logicalDevice.amldMediaTypeDesc;


    //
    // If the mediaRequest id is -1, the logical device isn't booked by
    // a request, so blank the request id and unit name values.
    //

    if ( logicalDevice.amldMediaRequestId == -1 )
    {
	pDevice->sdRequestId.blank();
	pDevice->sdUnitName.blank();
    }
    else
    {
	pDevice->sdRequestId = logicalDevice.amldMediaRequestId;
	pDevice->sdUnitName = logicalDevice.amldMediaUnitName;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::find
//
// INVOCATION:
// pDevice = find( logicalName, type, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) logicalName (char *)	The logical name of the device.
// (>) type (char *)		The type of the logical device.
// (!) status (cStoStatus &)	The function return status.
//
// FUNCTION VALUE:
// (cStoDevice *) The device matching the logical name and type.
//
// PURPOSE:
// Find a device matching the given logical name and device type.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cStoDevice	*cStoDevice::find
(
    const char	*logicalName,	// (in)  The logical name to look for.
    const char	*type,		// (in)  The device type to look for.
    cStoStatus	&status		// (mod) The function return status.
)
{
    cStoDevice::iObjectList
		iDevice;
    cStoDevice	*pDevice;


    checkStat( status, return( NULL ) );

    for ( iDevice = cStoDevice::begin(); iDevice != cStoDevice::end(); 
	    iDevice ++ )
    {
	pDevice = (*iDevice).second;

	// XXX allan added (const string&) cast
	if ( (const string&)pDevice->sdLogicalName == logicalName && 
		(const string&)pDevice->sdType == type )
	{
	    return( pDevice );
	}
    }

    status.E_DEVICE_FIND( status, logicalName, type );
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::init
//
// INVOCATION:
// cStoDevice::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the list of logical devices.
//
// DESCRIPTION:
// Create a cStoDevice object for each device.
// Set every cStoDevice object to indicate the EPICS row isn't in use.
// Get every logical device from the database, and assign it to a 
// 	cStoDevice object.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDevice::init
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    int		amStatus( AM_SUCCESS );
    DBPROCESS	*dbProcess;
    int		i;
    AM_LOGICAL_DEVICE
    		logicalDevice;
    cStoDevice::iObjectList
		iDevice;


    checkStat( status, return );


    sdArraySize = sdArraySize.value();


    //
    // If the list is empty, create one object for each row in the EPTR 
    // array.
    //

    if ( begin() == end() )
    {
	for ( i = 0; i < STO_MAX_DEVICES_EPICS_ARRAY; i++ )
	{
	    new cStoDevice( i );
	}
    }


    //
    // Set all epics record to not in use.
    //

    for ( iDevice = cStoDevice::begin(); iDevice != cStoDevice::end(); 
	    iDevice ++ )
    {
	(*iDevice).second->sdEpicsRecInUse = false; 
    }


    //
    // In a loop, fetch all logical device/type combinations and assign
    // them to a cStoDevice object.
    //

    dbProcess = cStoDbManager::acquire();
    for ( amStatus = amLogicalDeviceGet( AM_LDG_ALL, &logicalDevice, 
	    dbProcess );
	    status.ok() && amStatus == AM_SUCCESS;
	    amStatus = amLogicalDeviceGet( AM_LDG_NEXT, &logicalDevice, 
	    dbProcess ) )
    {
	deviceSet( logicalDevice, status );
    }
    cStoDbManager::release();

    if ( amStatus != AM_NO_DEVICE )
    {
	checkAm( amStatus, status, return );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDevice::inUseUpdate
//
// INVOCATION:
// cStoDevice::inUseUpdate( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Query the database and update all cStoDevice objects to match
// the in-use status read from the database
//
// DESCRIPTION:
// - Fetch each logical device from the am database.
// - Find the coresponding cStoDevice object.
// - If the in-use flag has changed, update the value.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDevice::inUseUpdate
(
    cStoStatus	&status		// (mod) Function return status.
)
{
    int		amStatus;	// Am library function return status.
    DBPROCESS	*dbProcess;	// The db process to use.
    cStoDevice::iObjectList	// Iterator over the cStoDevice objects.
    		iDevice;
    cStoDevice	*pDevice;	// The cStoDevice object being processed.
    AM_LOGICAL_DEVICE		// The AM logical device being processed.
    		logicalDevice;


    checkStat( status, return );


    //
    // In a loop, fetch all logical device/type combinations.
    //

    dbProcess = cStoDbManager::acquire();
    for ( amStatus = amLogicalDeviceGet( AM_LDG_ALL, &logicalDevice, 
	    dbProcess );
	    status.ok() && amStatus == AM_SUCCESS;
	    amStatus = amLogicalDeviceGet( AM_LDG_NEXT, &logicalDevice, 
	    dbProcess ) )
    {
	//
	// find the cStoObject matching the logical device from the database.
	//

	check( pDevice = cStoDevice::find( logicalDevice.amldName, 
		logicalDevice.amldMediaType, status ), status, break );


	//
	// If the database in use flag doesn't match the in use flag assigned
	// to this object, update the object in use flag.
	//

	if ( pDevice->sdDeviceInUse != logicalDevice.amldInUse )
	{
	    pDevice->sdDeviceInUse = logicalDevice.amldInUse;
	    if ( logicalDevice.amldMediaRequestId == -1 )
	    {
		pDevice->sdRequestId.blank();
		pDevice->sdUnitName.blank();
	    }
	    else
	    {
		pDevice->sdRequestId = logicalDevice.amldMediaRequestId;
		pDevice->sdUnitName = logicalDevice.amldMediaUnitName;
	    }
	}
    }
    cStoDbManager::release();
}
