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
// dhsStorage/h/stoAction.H
//
// PURPOSE:
// Contains the definitions and declarations for the DHS 
// Storage Server action class.
//
// CLASSES:
// cStoAction - Actions of the Storage Server. 
// cStoRetrieve - Retrieve action.
// cStoQueue - Queue action.
// cStoPrepare - Prepare action.
// cStoWrite - Write action.
// cStoCleanup - Cleanup action.
// cStoNotify - Notify action.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2000/01/06 17:57:53  nhill
// Moved the media reference increment to the cStoAction base class.
//
// Revision 1.7  1999/12/22 17:37:22  jaeger
// Changed the maximum actions from 5 to 15.
//
// Revision 1.6  1999/10/12 23:20:34  nhill
// Removed some unused data members.
//
// Revision 1.5  1999/06/16 23:45:08  nhill
// Major update. Devided actions into specialized sub-classes of the
// cStoAction class.
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

#ifndef STOACTION_H
#define	STOACTION_H

#include <list>
#include <string>
#include "globals.H"

#include "genMutex.H"
#include "genCond.H"

#define NO_FIND_CREATE
#include "genListOf.H"
#include "stoActionType.H"
#include "stoEpicsRecord.H"
#include "stoFiveState.H"
#include "stoMedia.H"


//
// Define EPICS record array size.  Note that this value must match the
// the EPICS array size if the EPICS database is being used, and must
// match the size specified in the console configuration file.
//

#define STO_MAX_ACTION_EPICS_ARRAY	15


//
// Define EPICS records. 
//

#define	STO_ERA_NUM_ACTIONS		"numActions"
#define	STO_ERA_ACTION_OVERFLOW		"actionOverflow"
#define	STO_ERA_ACTION_EPICS_ARRAY	"actionArraySize"

#define	STO_ERA_IN_USE			"action%d:inUse"
#define	STO_ERA_POSITION		"action%d:position"
#define	STO_ERA_REQUEST_ID		"action%d:reqId"
#define	STO_ERA_ACTION			"action%d:action"
#define	STO_ERA_OPTION			"action%d:option"
#define	STO_ERA_ACTIVE			"action%d:active"


//
// Define check for negative exit status.
//

#define NEGEXITSTATUS(stat) 	( ((int)(((stat)>>8)&0x1)) == 0 )


//
// Forward-reference the cStoMedia class. 
//

class cStoMedia;

//
//***********************************************************************
//+
// CLASS NAME:
// cStoAction
//
// PUBLIC METHODS: 
// (cStoAction)	cStoAction	- Constructor
// ()		~cStoAction	- Deconstructor
// (void)	init		- Initialize the EPICS action array, 
//				  initializing the values.
// (STO_ACTION_TYPE) str2action	- Convert string to action type enum.
// (bool)	findAction 	- Find a specific action.
// (void)	queueLock	- Lock action queue condition variable.
// (void)	queueUnlock	- Unlock action queue condition variable.
// (cStoAction *)
//		getNonActiveAction - Fetch next action to be processed.
// (int)	selectEpicsRow	- Pick row in Epics array for new action.
// (int)	getNextPosition - Get next position for Epics array.
// (bool)	disabled	- Check other active actions to determine if
//				  this action is disabled.
// (void)	execute		- Pure virtual function to execute an action.
// (void)	flush		- Re-send all action EPICS values to the
//				  EPICS database.
// (cStoActionType &) 
//		getActionType	- Get action type.
// (int)	getMediaRequestId
//				- Get action's media request id.
// (void)	setActive	- Set action's active flag.
// (bool)	getActive	- Get action's active flag.
// (bool)	getStopped	- Get the saStopped flag value.
// (bool)	setStopped	- Set the saStopped flag value.
// (bool)	isProcessable	- Check if action is currently processable.
// (bool)	rollback	- Get the saRollback flag value.
// (void)	stopAction	- Stop an action currently in progress.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Each task of the Storage Server constitutes an instance of this class.
// 
// The class catains information about actions and the queue of actions. 
// Methods include set and get methods to modify the private data, and 
// methods to access and process the queue of actions. 
//-
//***********************************************************************
//

class	cStoAction
{
    public:
	//
	// Typedefs - We'll use the list to be a queue. 
	//

	typedef list< cStoAction * >
			tActionQueue;		// List type.
	typedef list< cStoAction * >::iterator
			iActionQueue;		// Iterator type.


	//
	// Constructors, Deconstructors.
	//

			cStoAction( STO_ACTION_TYPE, cStoMedia *, 
				bool rollback );
	virtual		~cStoAction( void );


	//
	// Static Methods.
	//
	
	static void	init( cStoStatus & );

	static STO_ACTION_TYPE
			str2action( char * );
	static cStoAction
			*findAction( STO_ACTION_TYPE, int );
	inline static void	
			queueLock( void )
			{
			    saQueueMutex.lock();
			};
	inline static void	
			queueUnlock( void )
			{
			    saQueueMutex.unlock();
			};
	static cStoAction *
			getNonActiveAction( void );
	static int	selectEpicsRow( void );
	static int	getNextPosition( void );
	



	//
	// Non-static methods.
	//

	bool		disabled( void );
	virtual void	execute( void ) = 0;
	void		flush( void );
	inline cStoActionType
			&getActionType( void ) 
			{ 
			    return( saActionType.value() ); 
			};
	inline int	getMediaRequestId( void ) 
			{ 
			    return( saMediaRequestId.value() ); 
			};
	inline void	setActive( bool b )
			{
			    saActive = b;
			};
	inline bool	getActive( void )
			{
			    return( saActive.value() );
			};
	bool		getStopped( void )
			{
			    return( saStopped );
			}
	void		setStopped( bool value )
			{
			    saStopped = value;
			}
	bool		isProcessable( void );	
	bool		rollback( void )
			{
			    return( saRollback );
			}
	void		stopAction( void );


    protected:
	//
	// Protected Data.
	//
    
	static int	saEpicsFreeList[STO_MAX_ACTION_EPICS_ARRAY];
					// The list of free/non-free EPICS
					// action array rows.
	static int	saPositionCounter;
					// Array position counter.

	static cMutex	saQueueMutex;	// Queue control mutex
	static tActionQueue
			saActionQueue;	// Queue of Storage Actions

	static cStoEpicsRecord<int, cStoAction, cStoStatus>
			saNumActions;	// Number of actions in the queue. 
	static cStoEpicsRecord<bool, cStoAction, cStoStatus>
			saEpicsOverflow;// Set to true if the action array 
					// is overflowing.
	static cStoEpicsRecord<int, cStoAction, cStoStatus>
			saArraySize;	// Size of the action array.


	int		saEpicsArrayRow;// Row in EPICS array.
	bool		saRollback;	// Is this a rollback?
	bool		saStopped;	// Has this action been stopped?
	pid_t		saProcessId;	// Process id of task.

	cStoEpicsRecord<int, cStoAction, cStoStatus>
			saMediaRequestId;	
					// Media ID.
	cStoEpicsRecord<cStoActionType, cStoAction, cStoStatus>
			saActionType;	// Action type.
	cStoEpicsRecord<string, cStoAction, cStoStatus>
			saOption;	// Option.
	cStoEpicsRecord<bool, cStoAction, cStoStatus>
			saInUse;	// is this epics record in use?
	cStoEpicsRecord<bool, cStoAction, cStoStatus>
			saActive;	// Set to true of action is
					// being processed.
	cStoEpicsRecord<int, cStoAction, cStoStatus>
			saPosition;	// Position in the processing queue.
	cStoMedia	*saPMedia;	// The media to which this action 
					// applies.
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool ) = 0;
					// Pure virtual function to determine 
					// if this action can safely coexist 
					// with the specified action type on 
					// the same request/and or media unit.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoRetrieve
//
// PUBLIC METHODS: 
// ()		cStoRetrieve	- Constructor.
// ()		~cStoRetrieve	- Destructor.
// (void)	execute		- Execute a retrieve action.
// (void)	getCount	- Set the get count data item.
// (int)	getCount	- Return the get count data item.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class contains retrieve specific action methods and data.
//
//-
//***********************************************************************
//

class cStoRetrieve: public cStoAction
{
    public:
		cStoRetrieve( cStoMedia *pMedia, int size )
		    : cStoAction( STO_AT_RETRIEVE, pMedia, false )
		{
		    cStoStatus	status;
		    char	string[30];

		    srSize = size;
		    srGetCount = 0;
		    sprintf( string, " size: %d", size );
		    saOption = saOption.value() + string;

		    pMedia->setRetrieveState( STO_FST_REQUESTED, status );
		}
		~cStoRetrieve( void )
		{
		    cStoStatus	status;

		    saPMedia->setRetrieveState( STO_FST_IDLE, status );
		}
	void	execute( void );
	int	getCount( void )
		{
		    return( srGetCount );
		}
	void	getCount( int count )
		{
		    srGetCount = count;
		}
    protected:
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool );
					// Function to determine if this
					// action can coexist with another.

	int		srGetCount;	// Number of active gets for this 
					// action.
	int		srSize;		// Maximum size to retrieve, or 0
					// for no limit.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoQueue
//
// PUBLIC METHODS: 
// ()		cStoQueue	- Constructor.
// ()		~cStoQueue	- Destructor.
// (void)	execute		- Execute a queue action.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class contains queuing specific methods and data.
//
//-
//***********************************************************************
//

class cStoQueue: public cStoAction
{
    public:
		cStoQueue( cStoMedia *pMedia, bool force, int numUnits, 
			bool rollback = false )
		    : cStoAction( STO_AT_QUEUE, pMedia, rollback )
		{
		    cStoStatus	status;
		    char	string[30];

		    sqForce = force;
		    sqNumUnits = numUnits;
		    if ( force )
		    {
			saOption = saOption.value() + " force";
		    }
		    if ( numUnits != 0 )
		    {
			sprintf( string, " Num. units: %d", numUnits );
			saOption = saOption.value() + string;
		    }

		    if ( rollback )
		    {
			pMedia->setQueueState( STO_FST_ROLLBACK_REQUESTED, 
				status );
		    }
		    else
		    {
			pMedia->setQueueState( STO_FST_REQUESTED, status );
		    }
		}
		~cStoQueue( void )
		{
		    cStoStatus	status;

		    saPMedia->setQueueState( STO_FST_IDLE, status );
		}
	void	execute( void );
    protected:
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool );
					// Function to determine if this action
					// can safely coexist with another.

	bool		sqForce;	// Force queuing?
	int		sqNumUnits;	// Number of units to Queue.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoPrepare
//
// PUBLIC METHODS: 
// ()		cStoPrepare	- Constructor.
// ()		~cStoPrepare	- Destructor.
// (void)	execute		- Execute a preparation action.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class contains preparation specific methods and data.
//
//-
//***********************************************************************
//

class cStoPrepare: public cStoAction
{
    public:
		cStoPrepare( cStoMedia *pMedia, int number, 
			bool rollback = false )
		    : cStoAction( STO_AT_PREPARE, pMedia, rollback )
		{
		    char	buffer[30];
		    cStoStatus	status;

		    spNumPrepare = number;
		    if ( number != 0 )
		    {
			sprintf( buffer, " Num. Units: %d", number );
			saOption = saOption.value() += buffer;
		    }

		    if ( rollback )
		    {
			pMedia->setPrepareState( STO_FST_ROLLBACK_REQUESTED, 
				status );
		    }
		    else
		    {
			pMedia->setPrepareState( STO_FST_REQUESTED, status );
		    }
		}
		~cStoPrepare( void )
		{
		    cStoStatus	status;

		    saPMedia->setPrepareState( STO_FST_IDLE, status );
		}    
	void	execute( void );
    protected:
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool );
					// Function to determine if this action
					// can safely coexist with another.

	int		spNumPrepare;	// The number of units to prepare.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoWrite
//
// PUBLIC METHODS: 
// ()		cStoWrite	- Constructor.
// ()		~cStoWrite	- Destructor.
// (void)	execute		- Execute a write action.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains write action specific methods and data.
//
//-
//***********************************************************************
//

class cStoWrite: public cStoAction
{
    public:
		cStoWrite( cStoMedia *, bool, int, char *, bool = false );
		~cStoWrite( void )
		{
		    cStoStatus	status;

		    if ( swDevice != NULL )
		    {
			gen_free( swDevice );
		    }
		    saPMedia->setWriteState( STO_FST_IDLE, status );
		}
	void	execute( void );
    protected:
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool );
					// Determine if this action can safely
					// coexist with the other action.

	char		*swDevice;	// The device to use.
	bool		swForce;	// Force writing to a busy logical 
					// device?
	char		swNumUnits;	// Number of units to write.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoCleanup
//
// PUBLIC METHODS: 
// ()		cStoCleanup	- Constructor.
// ()		~cStoCleanup	- Destructor.
// (void)	execute		- Execute a cleanup action.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains cleanup specific methods and data.
//
//-
//***********************************************************************
//

class cStoCleanup: public cStoAction
{
    public:
		cStoCleanup( cStoMedia *pMedia, int number )
		    : cStoAction( STO_AT_CLEANUP, pMedia, false )
		{
		    char	buffer[30];
		    cStoStatus	status;

		    scNumCleanup = number;
		    if ( number != 0 )
		    {
			sprintf( buffer, " Number: %d", number );
			saOption = saOption.value() + buffer;
		    }

		    pMedia->setCleanupState( STO_FST_REQUESTED, status );
		}
		~cStoCleanup( void )
		{
		    cStoStatus	status;

		    saPMedia->setCleanupState( STO_FST_IDLE, status );
		}		    
	void	execute( void );
    protected:
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool );
					// Determine if this action can safely
					// coexist with the other action.
	int		scNumCleanup;	// The number of units to clean up.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStoNotify
//
// PUBLIC METHODS: 
// ()		cStoNotify	- Contstuctor.
// ()		~cStoNotify	- Destructor.
// (void)	execute		- Execute a notify action.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains notify specific methods and data.
//
//-
//***********************************************************************
//

class 	cStoNotify: public cStoAction
{
    public:
		cStoNotify( cStoMedia *pMedia )
		    : cStoAction( STO_AT_NOTIFY, pMedia, false )
		{
		    cStoStatus	status;

		    pMedia->setNotifyState( STO_FST_REQUESTED, status );
		}
		~cStoNotify( void );
	void	execute( void );
    protected:
    private:
	virtual bool	coexist( STO_ACTION_TYPE, bool );
					// Determine if this action can safely
					// coexist with the other action.
};
#endif // STOACTION_H
