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
// dhsStorage/h/stoUnit.H
//
// PURPOSE:
// Units of media are generated to satisfy media requests. This file
// contains code to provide a list of media units and support functions.
//
// CLASSES:
// cStoUnit - Store media unit information in a list. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:36  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.7  2000/12/19 19:37:49  nhill
// Updated for workshop 5.
//
// Revision 1.6  1999/06/16 23:05:07  nhill
// Major update and bug fixes.
//
// Revision 1.5  1999/02/24 23:57:23  jaeger
// Added findByName(), findFirstUnit(), findLastUnit(), compare(),
// and getMediaUnitNameRoot().
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

#ifndef STOUNIT_H
#define	STOUNIT_H

#define NO_FIND_CREATE
#include "genListOf.H"
#include "globals.H"
#include "stoDbm.H"
#include "stoEpicsRecord.H"



//
// Define EPICS record array size.  Note that this value must match the
// the EPICS array size if the EPICS database is being used, and must
// match the size specified in the console configuration file.
//

#define STO_MAX_UNIT_EPICS_ARRAY	20


//
// Define EPICS record names - NOTE: some have embedded integer formats.
//

#define	STO_ERU_NUM_UNITS		"numUnits"
#define	STO_ERU_UNIT_ARRAY_SIZE		"unitArraySize"
#define	STO_ERU_UNIT_OVERFLOW		"unitOverflow"

#define STO_ERU_IN_USE			"unit%d:inUse"
#define	STO_ERU_REQUEST_ID		"unit%d:reqId"
#define	STO_ERU_UNIT_NAME		"unit%d:unitName"

#define	STO_ERU_UNIT_SIZE		"unit%d:unitSize"
#define	STO_ERU_PREPARED		"unit%d:prepValue"
#define	STO_ERU_NUM_WRITTEN		"unit%d:writeNum"
#define	STO_ERU_CLEANEDUP		"unit%d:cleanupValue"



//
//***********************************************************************
//+
// CLASS NAME:
// cStoUnit
//
// PUBLIC METHODS:
// (cStoUnit)	cStoUnit	- Constructor (four types).
// ()		~cStoUnit	- Deconstructor.
// (void)	ref		- Increment reference counter.
// (void)	deref		- Decrement reference counter.
// (int)	selectEpicsRow	- Pick a row for an object in the Epics array.
// (void)	init		- Initializes the list of media unit objects.
// (void) 	exit		- Nuke list, free resources held by the class.
// (void)	flushAll	- Re-send all records to epics.
// (int		compare		- Compare the names of two units.
// (cStoUnit *)	findByName	- Find a unit given a media request id and
//				  media unit name.
// (cStoUnit *)	findFirstUnit	- Get the media unit with the smallest unit
//				  name with the given media request id and
//				  status value.
// (cStoUnit *)	findLastUnit	- Get the medi unit with the largest unit name
//				  with the given media request id and status
//				  value.
// (char *)	getMediaUnitNameRoot
//				- Get the media name root from the amUnit
//				  structure.
// (AM_UNIT *)	setPAmUnit	- Set ptr to am library media unit struct. 
// (AM_UNIT *)	getPAmUnit	- Get ptr to am library media unit struct. 
// (int)	getMediaRequestId - Return object's media request id.
// (char *)	getMediaUnitName - Return object's media request name.
// (void)	setUnitSize	- Sets object's unit size.
// (int)	getUnitSize	- Gets object's unit size. 
// (void)	setPrepared	- Sets object's prepared flag.
// (bool)	getPrepared	- Gets object's prepared flag.
// (void)	setNumWritten	- Sets object's number-written counter.
// (int)	getNumWritten	- Gets object's number-written counter.
// (void)	setCleanedUp	- Sets object's cleaned-up flag.
// (bool)	getCleanedUp	- Gets object's cleaned-up flag.
// (void)	db2Stats	- Initialize all statistics from the database,
//				  intended to be called using forEach().
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Stores media unit information in a list. 
//
// Objects of this class represent 'active' media units - that is, media 
// units which can have some action taken toward completing writing the
// unit to physical medium. 
//-
//***********************************************************************
//
 
class cStoUnit: 
	public cListOf< cStoUnit, std::string, cStoStatus >
{
    friend
    			class cListOf< cStoUnit, std::string, cStoStatus >;
    public:
			cStoUnit( AM_UNIT *, bool );


    //
    // The destructor is private to ensure the only way to delete the object
    // is through the dereference function.
    //

    private:
			~cStoUnit( void );
    public:


	void		ref( void )
			{
			    suRefCount++;
			}
	void		deref( void )
			{
			    if ( (--suRefCount) <= 0 )
			    {
				delete this;
			    }
			}
	static int	selectEpicsRow( void );
	static void	init( cStoStatus & );
	static void	exit( cStoStatus & );
	void		flushAll( int epicsArrayRow )
			{
			    suEpicsArrayRow = epicsArrayRow;

			    suEpicsRecInUse.setIndex( epicsArrayRow );
			    suMediaReqId.setIndex( epicsArrayRow );
			    suMediaUnitName.setIndex( epicsArrayRow );
			    suUnitSize.setIndex( epicsArrayRow );
			    suPrepared.setIndex( epicsArrayRow );
			    suNumWritten.setIndex( epicsArrayRow );
			    suCleanedUp.setIndex( epicsArrayRow );
			}


	static int	compare( char[], char[], char[], char[],
				boolean * );
	static cStoUnit *findByName( int, char * );
	static cStoUnit *findFirstUnit( int, const char );
	static cStoUnit *findLastUnit( int, const char );
	static const char
			*getMediaUnitNameRoot( int );
	

	//
	// Set and get methods for media object data.
	//
	
	AM_UNIT 	*setPAmUnit( AM_UNIT * );
	inline AM_UNIT 	*getPAmUnit( void )
			{
			    return( suPAmUnit );
			};
	inline int	getMediaRequestId( void )
			{
			    return( suPAmUnit->amuMediaRequestId );
			};
	inline char	*getMediaUnitName( void )
			{
			    return( suPAmUnit->amuMediaUnitName );
			};
	inline void	setUnitSize( int size )
			{
			    suUnitSize = size;
			};
	inline int	getUnitSize( void )
			{
			    return( suUnitSize.value() );
			};
	inline void	setPrepared( bool prepared )
			{
			    suPrepared = prepared;
			};
	inline bool	getPrepared( void )
			{
			    return( suPrepared.value() );
			};
	inline void	setNumWritten( int num )
			{
			    suNumWritten = num;
			};
	inline int	getNumWritten( void )
			{
			    return( suNumWritten.value() );
			};
	inline void	setCleanedUp( bool cleanedUp )
			{
			    suCleanedUp = cleanedUp;
			};
	inline bool	getCleanedUp( void )
			{
			    return( suCleanedUp.value() );
			};


	//
	// Object methods to be called from within forEach().
	//

	void		db2Stats( void *, cStoStatus & );


    protected:
    private:
	//
	// EPICS free array row list. 
	//

	static bool	suEpicsFreeList[STO_MAX_UNIT_EPICS_ARRAY];
					// The list of free/non-free EPICS 
					// unit array rows. 

	int		suRefCount;	// Reference counter for this object.


	//
	// Number of media units. 
	// Boolean set if EPICS array is overflowing. 
	//

	static cStoEpicsRecord<int, cStoUnit, cStoStatus>	
			suArraySize;	// Unit EPICS array size.
	static cStoEpicsRecord<int, cStoUnit, cStoStatus>	
			suNumUnits;	// Number of currently-active units.
	static cStoEpicsRecord<bool, cStoUnit, cStoStatus>	
			suEpicsOverflow;// true if EPICS is overflowed.

	
	//
	// Flag indicating if object is in use, used during init method.
	//

	boolean		suInUse;
	


	//
	// Pointer to am library media unit structure. 
	// Index into EPICS Media Unit Array.
	//
	
	AM_UNIT		*suPAmUnit;	// am library media unit struct. 
	int		suEpicsArrayRow;// Row in EPICS array. 


	//
	// Some am library information which is kept in EPICS. 
	//
	
	cStoEpicsRecord<bool, cStoUnit, cStoStatus>
			suEpicsRecInUse;// Is the epics record in use?
	cStoEpicsRecord<int, cStoUnit, cStoStatus>
			suMediaReqId;	// Media Request Identifier.
	cStoEpicsRecord<char *, cStoUnit, cStoStatus>
			suMediaUnitName;// Unit Name.


	//
	// Stats about the media unit which are reported through EPICS.
	//

	cStoEpicsRecord<long long, cStoUnit, cStoStatus>
			suUnitSize;	// Number of Bytes on the unit.
	cStoEpicsRecord<bool, cStoUnit, cStoStatus>
			suPrepared;	// Has the Unit been prepared. 
	cStoEpicsRecord<int, cStoUnit, cStoStatus>
			suNumWritten;	// Number copies written of the unit.
	cStoEpicsRecord<bool, cStoUnit, cStoStatus>
			suCleanedUp;	// Is it OK to clean-up the unit. 
};
#endif // STOUNIT_H
