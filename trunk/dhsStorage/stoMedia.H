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
// dhsStorage/h/stoMedia.H
//
// PURPOSE:
// Media requests are the reason Storage Server exists. This file
// contains code to provide a list of media requests and support functions.
//
// CLASSES:
// cStoMedia - Store media request information in a list. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.14  2000/12/19 19:36:35  nhill
// Updated for workshop 5.
//
// Revision 1.13  1999/12/22 17:36:57  jaeger
// Changed maximum media from 10 to 20.
//
// Revision 1.12  1999/10/12 23:22:01  nhill
// Added the findById member function.
//
// Revision 1.11  1999/06/18 22:27:06  nhill
// Fixed the calculate on units from bytes.
//
// Revision 1.10  1999/06/16 23:50:24  nhill
// Major re-organization.
//
// Revision 1.9  1999/02/24 23:55:28  jaeger
// Unit size changed from Megs to Kilobytes.
//
// Revision 1.8  1998/05/25 19:04:59  cockayne
// Modified transformation routines to properly round to the nearest 0.01
//
// Revision 1.7  1998/05/15 17:18:50  cockayne
// Added methods to support epics array overflow processing.
//
// Revision 1.6  1998/04/23 16:15:50  cockayne
// Modified to use new status and health.
//
// Revision 1.5  1998/04/20 20:53:58  cockayne
// Added default constructor, necessary to support new method findByName(),
// also modified set and get methods for epics records which are 'long long'ss
// to return and accept the correct type.
//
// Revision 1.4  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOMEDIA_H
#define	STOMEDIA_H

#define NO_FIND_CREATE
#include "genListOf.H"
#include "globals.H"
#include "stoDbm.H"
#include "stoFiveState.H"
#include "stoEpicsRecord.H"


//
// Define EPICS record array size.  Note that this value must match the
// the EPICS array size if the EPICS database is being used, and must
// match the size specified in the console configuration file.
//

#define STO_MAX_MEDIA_EPICS_ARRAY	20


//
// Define EPICS record names - NOTE: some have embedded integer formats.
//

#define	STO_ERM_NUM_REQS		"numReqs"
#define	STO_ERM_REQ_OVERFLOW		"reqOverflow"
#define	STO_ERM_REQ_ARRAY_SIZE		"reqArraySize"

#define STO_ERM_IN_USE			"req%d:inUse"
#define	STO_ERM_REQUEST_ID		"req%d:reqId"
#define	STO_ERM_REQUEST_NAME		"req%d:reqName"
#define	STO_ERM_MEDIA_TYPE		"req%d:mediaType"
#define STO_ERM_REQUEST_TYPE		"req%d:reqType"
#define STO_ERM_NUM_COPIES		"req%d:numCopies"

#define	STO_ERM_AMOUNT_RETRIEVABLE	"req%d:retrieveSize"
#define	STO_ERM_AMOUNT_QUEUEABLE	"req%d:queueSize"
#define	STO_ERM_NUM_PREPARABLE		"req%d:prepNum"
#define	STO_ERM_NUM_WRITEABLE		"req%d:writeNum"
#define	STO_ERM_NUM_CLEANUPABLE		"req%d:cleanupNum"
#define	STO_ERM_NOTIFIABLE		"req%d:notifyValue"

#define STO_ERM_RETRIEVE_STATE		"req%d:retrieveState"
#define STO_ERM_QUEUE_STATE		"req%d:queueState"
#define STO_ERM_PREPARE_STATE		"req%d:prepState"
#define STO_ERM_WRITE_STATE		"req%d:writeState"
#define STO_ERM_CLEANUP_STATE		"req%d:cleanupState"
#define STO_ERM_NOTIFY_STATE		"req%d:notifyState"
#define STO_ERM_REQ_TYPE		"req%d:notifyState"
#define STO_ERM_NOTIFY_STATE		"req%d:notifyState"



//
//***********************************************************************
//+
// CLASS NAME:
// cStoMedia
//
// PUBLIC METHODS:
// (cStoMedia)	cStoMedia	- Constructor (two different types).
// ()		~cStoMedia	- Deconstructor.
// (int)	selectEpicsRow	- Pick a row for an object in the Epics array.
// (void)	init		- Initializes the list of media objects.
// (void)	exit		- Nuke list, free resources held by the class.
// (void)	ref		- Increment reference counter.
// (void)	deref		- Decrement reference counter.
// (AM_MEDIA *)	setPAmMedia	- Set ptr to am library media struct.
// (AM_MEDIA *)	getPAmMedia	- Get ptr to am library media struct.
// (STO_FIVE_STATE_TYPE) 
//		getActionState	- Get current state of an action.
// (char *)	bool2str	- Convert bool variable to string for Epics.
// (char *)	bytes2Megs	- Convert int number of bytes to float 
//				  number of megabytes for Epics.
// (char *)	bytes2Units	- Convert int number of bytes to float 
//				  number of units for Epics.
// (cStoMedia *)findById	- Find a media request object given its media
//				  id.
// (cStoMedia *)findByName	- Find a media request object given the name
//				  of the media request.
// (int)	getEpicsArrayRow- Get the epics row assigned to this media 
//				  request.
// (char *)	getMediaRequestName 
//				- Return object's media request id. 
// (int)	getMediaRequestId - Return object's media request id. 
// (void)	setRetrieveState - Set state of retrieve action.
// (void)	setQueueState	- Set state of queue action.
// (void)	setPrepareState - Set state of prepare action.
// (void)	setWriteState	- Set state of write action.
// (void)	setCleanupState - Set state of cleanup action.
// (void)	setNotifyState	- Set state of notify action.
// (void)	setRetrieveSize	- Set value of retrieve size Epics record. 
// (long long)	getRetrieveSize	- Get value of retrieve size Epics record. 
// (void)	setQueueSize	- Set value of queue size Epics record. 
// (long long)	getQueueSize	- Get value of queue size Epics record. 
// (void)	setPrepNum	- Set value of prepare number Epics record. 
// (int)	getPrepNum	- Get value of prepare number Epics record. 
// (void)	setWriteNum	- Set value of write num Epics record. 
// (int)	getWriteNum	- Get value of write num Epics record. 
// (void)	setCleanupNum	- Set value of cleanup num Epics record. 
// (int)	getCleanupnum	- Get value of cleanup num Epics record. 
// (void)	setNotify	- Set value of notify Epics record. 
// (int)	getNotify	- Get value of notify Epics record. 
// (void)	flushAll	- Send all EPICS data to the specified row
//				  in the EPICS media information arrays.
// (char *)	type		- Return the type of the media request.
// (void	db2Stats	- Initialize all statistics from the database,
//				  intended to be called using forEach().
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles storing information about media requests in a list. 
//
// Objects of this type represent 'active' media requests - that is, 
// media requests which can currently have actions taken towards advancing
// the media request toward completion.
//-
//***********************************************************************
//
 
class cStoMedia: 
	public cMutex,
	public cListOf< cStoMedia, int, cStoStatus >
{
    public:
			cStoMedia( AM_MEDIA *, bool = true );
			~cStoMedia( void );

	static int	selectEpicsRow( void );
	static void	init( cStoStatus & );
	static void	exit( cStoStatus & );
	void		ref( void )
			{
			    smRefCount ++;
			}
	void		deref( void)
			{
			    smRefCount --;
			    if ( smRefCount <= 0 )
			    {
				delete this;
			    }
			}


	AM_MEDIA 	*setPAmMedia( AM_MEDIA * );
	inline AM_MEDIA *getPAmMedia( void )
			{
			    return( smPAmMedia );
			};
	STO_FIVE_STATE_TYPE
			getActionState( STO_ACTION_TYPE );
	
	inline char	*bool2str( bool b )
			{
			    sprintf( smB2SString, "%s",
				    ( b ? "True" : "False" ) );
			    return( smB2SString );
			};
	char		*bytes2Megs( long long );
	inline char	*bytes2Units( long long l )
			{
			    float	value;
			    value = l / ( (float) smUnitSize * 1024 );
			    if ( l > 0 )
			    {
				value = MAX( value, .01 );
			    }
			    sprintf( smB2UString, "%5.2f", value );
			    return( smB2UString );
			};
	static cStoMedia
			*findById( int );
	static cStoMedia	
			*findByName( char * );


	//
	// Set and get methods for media object data.
	//
	
	inline int	getEpicsArrayRow( void )
			{
			    return( smEpicsArrayRow );
			};
	inline char	*getMediaRequestName( void )
			{
			    return( smPAmMedia->ammMediaRequestName );
			};
	inline int	getMediaRequestId( void )
			{
			    return( smPAmMedia->ammMediaRequestId );
			};
	inline void	setRetrieveState( STO_FIVE_STATE_TYPE fs, 
				cStoStatus &s )
			{
			    checkStat( s, return );
			    cStoFiveState fsObject( fs );
			    smRetrieveState = fsObject;
			    if ( !cStoFiveState::isEqual( 
				    smRetrieveState.value(), fsObject ) )
			    {
				s.E_FIVESTATE( s );
			    }
			};
	inline void	setQueueState( STO_FIVE_STATE_TYPE fs, 
				cStoStatus &s )
			{
			    checkStat( s, return );
			    cStoFiveState fsObject( fs );
			    smQueueState = fsObject;
			    if ( !cStoFiveState::isEqual( 
				    smQueueState.value(), fsObject ) )
			    {
				s.E_FIVESTATE( s );
			    }
			};
	inline void	setPrepareState( STO_FIVE_STATE_TYPE fs, 
				cStoStatus &s )
			{
			    checkStat( s, return );
			    cStoFiveState fsObject( fs );
			    smPrepareState = fsObject;
			    if ( !cStoFiveState::isEqual( 
				    smPrepareState.value(), fsObject ) )
			    {
				s.E_FIVESTATE( s );
			    }
			};
	inline void	setWriteState( STO_FIVE_STATE_TYPE fs, 
				cStoStatus &s )
			{
			    checkStat( s, return );
			    cStoFiveState fsObject( fs );
			    smWriteState = fsObject;
			    if ( !cStoFiveState::isEqual( 
				    smWriteState.value(), fsObject ) )
			    {
				s.E_FIVESTATE( s );
			    }
			};
	inline void	setCleanupState( STO_FIVE_STATE_TYPE fs, 
				cStoStatus &s )
			{
			    checkStat( s, return );
			    cStoFiveState fsObject( fs );
			    smCleanupState = fsObject;
			    if ( !cStoFiveState::isEqual( 
				    smCleanupState.value(), fsObject ) )
			    {
				s.E_FIVESTATE( s );
			    }
			};
	inline void	setNotifyState( STO_FIVE_STATE_TYPE fs, 
				cStoStatus &s )
			{
			    checkStat( s, return );
			    cStoFiveState fsObject( fs );
			    smNotifyState = fsObject;
			    if ( !cStoFiveState::isEqual( 
				    smNotifyState.value(), fsObject ) )
			    {
				s.E_FIVESTATE( s );
			    }
			};
	inline void	setRetrieveSize( long long size )
			{
			    smRetrieveSize = size;
			};
	inline long long
			getRetrieveSize( void )
			{
			    return( smRetrieveSize.value() );
			};
	inline void	setQueueSize( long long size )
			{
			    smQueueSize = size;
			};
	inline long long	
			getQueueSize( void )
			{
			    return( smQueueSize.value() );
			};
	inline void	setPrepNum( int num )
			{
			    smPrepNum = num;
			};
	inline int	getPrepNum( void )
			{
			    return( smPrepNum.value() );
			};
	inline void	setWriteNum( int num )
			{
			    smWriteNum = num;
			};
	inline int	getWriteNum( void )
			{
			    return( smWriteNum.value() );
			};
	inline void	setCleanupNum( int num )
			{
			    smCleanupNum = num;
			};
	inline int	getCleanupNum( void )
			{
			    return( smCleanupNum.value() );
			};
	inline void	setNotify( bool notifiable )
			{
			    smNotify = notifiable;
			};
	inline int	getNotify( void )
			{
			    return( smNotify.value() );
			};
	inline void	flushAll( int row )
			{
			    smEpicsArrayRow = row;
			    smEpicsRecInUse.setIndex( row );
			    smMediaReqId.setIndex( row );
			    smMediaReqName.setIndex( row );
			    smMediaType.setIndex( row );
			    smRequestType.setIndex( row );
			    smNumCopies.setIndex( row );
			    smRetrieveSize.setIndex( row );
			    smQueueSize.setIndex( row );
			    smPrepNum.setIndex( row );
			    smWriteNum.setIndex( row );
			    smCleanupNum.setIndex( row );
			    smNotify.setIndex( row );
			    smRetrieveState.setIndex( row );
			    smQueueState.setIndex( row );
			    smPrepareState.setIndex( row );
			    smWriteState.setIndex( row );
			    smCleanupState.setIndex( row );
			    smNotifyState.setIndex( row );
			};
	const char *	type( void )
			{
			    return( smMediaType.value() );
			}


	//
	// Object methods to be called from within forEach().
	//

	void		db2Stats( void *, cStoStatus & );


    protected:
    private:
	//
	// The following are used when converting things to strings for the
	// epics classes.
	//

	char		smB2MString[60];	// Bytes to megs conversion.
	char		smB2SString[60];	// Bool to string conversion.
	char		smB2UString[60];	// Bytes to units conversion.


	//
	// EPICS free array row list. 
	//

	static bool	smEpicsFreeList[STO_MAX_MEDIA_EPICS_ARRAY];
					// The list of free/non-free EPICS 
					// media array rows.

	//
	// Flag that indicates if object is in use, used by init().
	//

	bool		smInUse;
	int		smRefCount;	// Reference counter.
	

	//
	// Number of media requests. 
	// Boolean set if EPICS array is overflowing. 
	//

	static cStoEpicsRecord<int, cStoMedia, cStoStatus>	
			smArraySize;	// Request array size.
	static cStoEpicsRecord<int, cStoMedia, cStoStatus>	
			smNumReqs;	// Number of currently-active requests.
	static cStoEpicsRecord<bool, cStoMedia, cStoStatus>	
			smEpicsOverflow;// true if EPICS is overflowed. 


	//
	// Pointer to am library media request structure. 
	// Index into EPICS Media Request Array.
	//
	
	AM_MEDIA	*smPAmMedia;	// am library media struct. 
	int		smEpicsArrayRow;// Row in EPICS array. 
	int		smUnitSize;	// Unit Size in Kilobytes. 


	//
	// Some am library information which is kept in EPICS. 
	//
	
	cStoEpicsRecord<bool, cStoMedia, cStoStatus>
			smEpicsRecInUse;// Is the epics record in use?
	cStoEpicsRecord<int, cStoMedia, cStoStatus>
			smMediaReqId;	// Media Request Identifier.
	cStoEpicsRecord<char *, cStoMedia, cStoStatus>
			smMediaReqName; // Media Request Name.
	cStoEpicsRecord<char *, cStoMedia, cStoStatus>
			smMediaType;	// Type of Media to write. 
	cStoEpicsRecord<char *, cStoMedia, cStoStatus>
			smRequestType;	// Type of Media media request. 
	cStoEpicsRecord<int, cStoMedia, cStoStatus>
			smNumCopies;	// Number of copies to write.


	//
	// Stats about the media request which are reported through EPICS.
	//

	cStoEpicsRecord<long long, cStoMedia, cStoStatus>
			smRetrieveSize; // Number of retrievable Bytes.
	cStoEpicsRecord<long long, cStoMedia, cStoStatus>
			smQueueSize;	// Number of queueable Bytes.
	cStoEpicsRecord<int, cStoMedia, cStoStatus>
			smPrepNum;	// Number of preparable units. 
	cStoEpicsRecord<int, cStoMedia, cStoStatus>
			smWriteNum;	// Number of writable units.
	cStoEpicsRecord<int, cStoMedia, cStoStatus>
			smCleanupNum;	// Number of cleanupable units. 
	cStoEpicsRecord<bool, cStoMedia, cStoStatus>
			smNotify;	// Can completion-notification be done?


	//
	// State of each process which the media request will require. 
	//
	
	cStoEpicsRecord<cStoFiveState, cStoMedia, cStoStatus>
			smRetrieveState;// State of media request's
					// retrieval process.
	cStoEpicsRecord<cStoFiveState, cStoMedia, cStoStatus>
			smQueueState;	// State of media request's
					// queueing process.
	cStoEpicsRecord<cStoFiveState, cStoMedia, cStoStatus>
			smPrepareState;	// State of media request's
					// preparing process.
	cStoEpicsRecord<cStoFiveState, cStoMedia, cStoStatus>
			smWriteState;	// State of media request's
					// writing process.
	cStoEpicsRecord<cStoFiveState, cStoMedia, cStoStatus>
			smCleanupState;// State of media request's
					// cleanup process.
	cStoEpicsRecord<cStoFiveState, cStoMedia, cStoStatus>
			smNotifyState;	// State of media request's
					// completion-notification process.
};
#endif // STOMEDIA_H
