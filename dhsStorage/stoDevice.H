//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998
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
// dhsStorage/h/stoDevice.H
//
// PURPOSE:
// Contains the definition of the storage server devices class.
//
// CLASSES:
// cStoDevice	- Records information about logical devices available to
//		  the storage server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1  1999/06/16 23:50:54  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STODEVICE_H
#define	STODEVICE_H

//
// Define EPICS record array size.  Note that this value must match the
// the EPICS array size if the EPICS database is being used, and must
// match the size specified in the console configuration file.
//

#define	STO_MAX_DEVICES_EPICS_ARRAY	20

#define	STO_ERD_DEVICE_ARRAY_SIZE	"deviceArraySize"
#define	STO_ERD_IN_USE			"device%d:inUse"
#define	STO_ERD_AVAILABLE		"device%d:available"
#define	STO_ERD_CAPACITY		"device%d:capacity"
#define	STO_ERD_DEVICE_IN_USE		"device%d:deviceInUse"
#define	STO_ERD_LOGICAL_NAME		"device%d:logicalName"
#define	STO_ERD_NUM_DEVICES		"device%d:numDevices"
#define	STO_ERD_REQUEST_ID		"device%d:reqId"
#define	STO_ERD_TYPE			"device%d:type"
#define	STO_ERD_TYPE_DESC		"device%d:typeDesc"
#define	STO_ERD_UNIT_NAME		"device%d:unitName"

#include <string>

#define	NO_FIND_CREATE
#include "genListOf.H"

#include "globals.H"
#include "stoEpicsRecord.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cStoDevice
//
// PUBLIC METHODS: 
// () cStoDevice 		- Contructor.
// () ~cStoDevice 		- Destructor.
// (void) book 			- Book this logical device.
// (void) clear			- Unbook this logical device.
// (cStoDevice *) deviceSelect	- Select the most appropriate logical device.
// (void) deviceSet		- Set the information for a logical device.
// (void) exit			- Clean up the stoDevice list.
// (cStoDevice *) find		- Find a logical device given a name and
//				  media type.
// (void) init			- Initialize the device list.
// (void) inUseUpdate		- Update the in use flag of every logical 
//				  device.
// (void) logicalName		- Return the logical name of this logical 
//				  device.
// (void) setReqUnit		- Set the request id and unit associated
//				  with a booked device.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class keeps track of a the devices available to the storage server.
//
//-
//***********************************************************************
//

class 	cStoDevice:
	public cListOf< cStoDevice, int, cStoStatus >
{
    public:
			cStoDevice( int );
			~cStoDevice( void );
	void		book( const bool, cStoStatus & );
	void		clear( cStoStatus & status )
			{
			    DBPROCESS	*dbProcess;

			    dbProcess = cStoDbManager::acquire();
			    checkAm( amDeviceClear( AM_DC_LOGICAL, dbProcess, 
				    sdLogicalName.value().data() ), 
				    status, VOID );
			    cStoDbManager::release();
			    sdRequestId.blank();
			    sdUnitName.blank();
			}

			    
	static cStoDevice	
			*deviceSelect( const char *, cStoStatus & );
	static void	deviceSet( AM_LOGICAL_DEVICE &, cStoStatus & );
	static void	exit( cStoStatus & status )
			{
			    checkStat( status, return );

			    deleteAll();
			}
	static cStoDevice
			*find( const char *, const char *, cStoStatus & );
	static void	init( cStoStatus & );
	static void	inUseUpdate( cStoStatus & );
	const char	*logicalName( void )
			{
			    return( sdLogicalName.value().data() );
			}
	void		setReqUnit( int requestId, const char * unitName ) 
			{
			    sdRequestId = requestId;
			    sdUnitName = unitName;
			}
    protected:
    private:
	static cStoEpicsRecord<int, cStoDevice, cStoStatus>
			sdArraySize;	// Device array size.
	cStoEpicsRecord<bool, cStoDevice, cStoStatus>
			sdAvailable;	// Is the n'th device available for use?
	cStoEpicsRecord<int, cStoDevice, cStoStatus>
			sdCapacity;	// Capacity of the device.
	cStoEpicsRecord<bool, cStoDevice, cStoStatus>
			sdDeviceInUse;	// Is the n'th device in use already?
	cStoEpicsRecord<bool, cStoDevice, cStoStatus>
			sdEpicsRecInUse;// Is the n'th EPICS record in use?
	int		sdEpicsArrayRow;// Row in the epics array.
	cStoEpicsRecord<string, cStoDevice, cStoStatus>
			sdLogicalName;	// Logical name of the device.
	cStoEpicsRecord<int, cStoDevice, cStoStatus>
			sdNumDevices;	// Number of physical devices 
					// associated with the logical device.
	cStoEpicsRecord<int, cStoDevice, cStoStatus>
			sdRequestId;	// Request id that booked the device.
	cStoEpicsRecord<string, cStoDevice, cStoStatus>
			sdType;		// Media Type associated with the 
					// device.
	cStoEpicsRecord<string, cStoDevice, cStoStatus>
			sdTypeDesc;	// Description associated with the
					// type.
	cStoEpicsRecord<string, cStoDevice, cStoStatus>
			sdUnitName;	// Unit name that booked the device.
};
#endif // STODEVICE_H


