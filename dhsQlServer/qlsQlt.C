static char rcsid[] = "$Id: qlsQlt.C,v 1.1.1.1 2002-11-24 20:29:21 brighton Exp $";
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
// dhsQlServer/src/qlsQlt.C
//
// PURPOSE:
// Contains the implementation of the cQlsQlt class
//
// CLASSES
// cQlsNotifyCmd - Notify command to be sent to a QLT
//
// METHOD NAME(S)
// cQlsNotifyCmd::done - Executed when a qlsNotify command is complete.
// cQlsQlt::cQlsQlt - Constructor for the cQlsQlt class.
// cQlsQlt::~cQlsQlt - Destructor for the cQlsQlt class.
// cQlsQlt::checkNotify - Check to see if a QLT needs to be notified that a
//		the data has been modified.
// cQlsQlt::commandThread - Sends commands to a Quick Look Tool.
// cQlsQlt::Notify - Notify a QLT that a frame has changed.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.12  2001/03/27 20:51:23  nhill
// Several bug fixes related to locking and timing of notifies to clients.
//
// Revision 1.11  1999/01/20 23:17:12  nhill
// Added a "regions" attribute to the qltNotify command.
//
// Revision 1.10  1998/06/16 22:09:28  nhill
// Fixed deadlocking problems that occured when QLTs were started and stoped
// while the QLS was receiving data.
//
// Revision 1.9  1998/04/27 21:10:02  nhill
// Changed to use separate header and data files.
//
// Revision 1.8  1998/04/15 22:58:45  nhill
// Updated to the newest version of the cDhsStatus class.
// Updated the documentation.
//
// Revision 1.7  1997/12/10 21:57:51  nhill
// Added the queueEmpty option to the qltNotify command.
//
// Revision 1.6  1997/12/05 00:49:49  nhill
// changed to use a single, separate thread to send commands to the
// QLT.
//
// Revision 1.5  1997/11/30 21:30:10  nhill
// Changed to use the cListOf class to take care of the list of all
// cQlsQlt objects.
//
// Revision 1.4  1997/10/22 17:44:42  nhill
// Changed to create a hard link to the frame data file for each quick look
// tool.
//
// Revision 1.3  1997/10/01 17:25:09  nhill
// Second alpha release.
//
// Revision 1.2  1997/08/20 21:24:31  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_BULK
#define	DHS_NO_SERVER

#include <sys/stat.h>

#include "dhs++.H"

#include "globals.H"
#include "qlsQlt.H"
#include "qlsStream.H"
#include "qlsDataset.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cQlsNotifyCmd
//
// PUBLIC METHODS: 
// (None) cQlsNotifyCmd - constructor for the class.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Send a notification command to a QLT
//
//-
//***********************************************************************
//

class 	cQlsNotifyCmd: public cDhsClientCommand
{
    public:
	inline		cQlsNotifyCmd( cQlsFrame & frame )
				: cDhsClientCommand( "qltNotify" )
			{ 
			    qncPFrame = &frame;
			}
	inline cQlsFrame
			&frame( void ) { return *qncPFrame; };
    protected:
    private:
	void            done( DHS_CMD_STATUS, char *, const cDhsAvList & );

	cQlsFrame	*qncPFrame;
};

//
//***********************************************************************
// cQlsNotifyCmd::done
//
// INVOCATION:
// n.a. Called automatically by the dhs++ library
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cmdStatus (DHS_CMD_STATUS) Status of the command.
// (>) message (char *) Message associated with the command.
// (>) avList (cDhsAvList &) Av list associated with the command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when the notify command is complete
//
// DESCRIPTION:
// Check that the command succeeded and then delete the command object.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This assumes the cQlsNotifyCmd object was allocated on the heap.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void          cQlsNotifyCmd::done
(
    DHS_CMD_STATUS            // (in)  State of the command
              cmdStatus,
    char      *message,       // (in)  Message associated with the command.
    const cDhsAvList  &       // (in)  AvList associated with the command.
)
{
    cQlsStatus        status;
    DHS_STATUS        dhsStatus( DHS_S_SUCCESS );


    if ( cmdStatus != DHS_CS_DONE )
    {
	status.E_COMMAND( status, "qlsNotify" );
	if ( message != NULL )
	{
	  status += message;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::cQlsQlt
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connection (DHS_CONNECT) The connection the QLT is attached to.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a Quick look client object
//
// DESCRIPTION:
// Set the connection and add the object to the list of all cQlsQlt objects.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function should be called with the class mutex locked for writing 
// to prevent other threads from using the list while it is being modified.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cQlsQlt::cQlsQlt
(
    DHS_CONNECT	connection	// (in)  The connection the QLT is attached to.
): cQlsSubscriber( connection ), 
	cListOf< cQlsQlt, DHS_CONNECT, cQlsStatus >( connection )
{
    int		s;
    
    qqtExitThread = false;
    qqtThreadRunning = true;
    s = pthread_create( &qqtThreadId, NULL, commandThread, this );
    
    if ( s != 0 )
    {
	throw "Failed to create a thread.";
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::~cQlsQlt
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for the cQlsQlt class
//
// DESCRIPTION:
// - Unsubscribe all quick look streams.
// - Remove the qlt from the list of qlts
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cQlsQlt::~cQlsQlt
(
    void
)
{
    cQlsStatus	status;
    cQlsNotifyCmd
    		*pCommand;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    lock();

    qqtExitThread = true;
    qqtQueueCond.broadcast();
    unlock();

    dhsEventLoop( DHS_ELT_COND, checkRunningFlag, this, &dhsStatus );

    pthread_join( qqtThreadId, NULL );

    while ( ! qqtCommandQueue.empty() )
    {
	pCommand = qqtCommandQueue.front();
	qqtCommandQueue.pop();
	delete pCommand;
    }


    //
    // Unsubscribe all quick look streams.
    //

    unSubscribeAll( status );

    tmpDir.clear( connect().name(), NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::checkRunningFlag
//
// INVOCATION:
// dhsEventLoop( DHS_ELT_COND, checkRunningFlag, this, &dhsStatus );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg (void *) A pointer to the cQlsQlt object.
//
// FUNCTION VALUE:
// (boolean) True when the event loop is no longer running.
//
// PURPOSE:
// Called to wait until the QLT command thread is no longer running
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cQlsQlt::checkRunningFlag
(
    void	*arg		// (in)  Pointer to the cQlsQlt object.
)
{
    cQlsQlt	*pQlt( (cQlsQlt *) arg );

    return( ! pQlt->qqtThreadRunning );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::checkNotify
//
// INVOCATION:
// qlt.checkNotify( pDataset, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pArg (void *) The dataset to notify it about.
// (!) static (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function notifies the specified Quick Look Tool that the specified
// dataset has been modified
//
// The pDataset parameter is passed as an argument instead of making this
// method non-static in order to allow the method to be called from the
// cQlsQlt::forEach method.
//
// DESCRIPTION:
// - loop through each stream subscribed to by the QLT.
// - if the steam is associated with the dataset, call function cQlsQlt::notify
//   for each frame in the dataset.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsQlt::checkNotify
(
    void	*pArg,		// (in)  The dataset to notify it about.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cQlsStream::iStreamList
    		i;
    cQlsStream	*pStream;	// A subscribed stream.
    cQlsDataset	*pDataset( (cQlsDataset *) pArg );


    checkStat( status, return);


    //
    // Do everything with the mutex locked to ensure the list of streams
    // isn't changing as we look at it.
    //

    lock();


    //
    // Loop through all of the streams subscribed to by the QLT.
    //

    for ( i = streamList().begin(); i != streamList().end(); i++ )
    {
	//
	// Check to see if the dataset has the stream associated with it.
	//

	pStream = (*i).second;
	if ( strcmp( pStream->name(), "*" ) == 0 ||
		pDataset->hasStream( *pStream ) )
	{
	    //
	    // The dataset and the QLT have a frame in common, so notify
	    // the QLT of all modified frames.
	    //

	    pDataset->forEachFrame( cQlsQlt::notify, this, status );
	    break;
	}
    }
    unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::commandThread
//
// INVOCATION:
// s = pthread_create( NULL, &attr, commandThread, NULL );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) qlt (void *) The qlt object being processed.
//
// FUNCTION VALUE:
// (void *) Not used.
//
// PURPOSE:
// Sends commands to a Quick Look Tool
//
// This function is run in a separate thread, it removes commands from the
// command queue and sends them on the the QLT.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		*cQlsQlt::commandThread
(
    void	*arg		// (in)  Pointer to the qlt.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    cQlsNotifyCmd
    		*pNotifyCmd;
    cQlsQlt	*pQlt;
    cQlsStatus	status;
    char	frameHeader[PATH_MAX];
    char	frameData[PATH_MAX];
    struct stat	statBuf;


    pQlt = (cQlsQlt *) arg;


    while ( ! pQlt->qqtExitThread )
    {
	dhsStatus = DHS_S_SUCCESS;
	status( status.S_SUCCESS );

	pQlt->qqtQueueCond.lock();
	if ( pQlt->qqtCommandQueue.empty() )
	{
	    pQlt->qqtQueueCond.wait();
	    pQlt->qqtQueueCond.unlock();
	}
	else
	{
	    pNotifyCmd = pQlt->qqtCommandQueue.front();
	    pQlt->qqtCommandQueue.pop();


//
// Create the path name for the frame header.
//

tmpDir.mkPath( frameHeader, pQlt->connect().name(), 
	strtail( pNotifyCmd->frame().headerFName() ), NULL );

if ( stat( frameHeader, &statBuf ) != 0 )
{
    checkSystem( link( pNotifyCmd->frame().headerFName(), frameHeader ), 
	    status, throw status );
}


//
// Create the path name for the frame data.
//

tmpDir.mkPath( frameData, pQlt->connect().name(), 
	strtail( pNotifyCmd->frame().dataFName() ), NULL );

if ( stat( frameData, &statBuf ) != 0 )
{
    checkSystem( link( pNotifyCmd->frame().dataFName(), frameData ), 
	    status, throw status );
}

//
// Add the attributes to the command.
//

pNotifyCmd->add( "headerFileName", DHS_DT_STRING, frameHeader, 
	dhsStatus );
pNotifyCmd->add( "dataFileName", DHS_DT_STRING, frameData, dhsStatus );


	    if ( pQlt->qqtCommandQueue.empty() )
	    {
		pNotifyCmd->add( "queueEmpty", DHS_DT_STRING, "true", 
			dhsStatus );
	    }
	    else
	    {
		pNotifyCmd->add( "queueEmpty", DHS_DT_STRING, "false", 
			dhsStatus );
	    }
	    pQlt->qqtQueueCond.unlock();

	    if ( pQlt->connect().isOpen() )
	    {
		pNotifyCmd->frame().rLock();
		status.displayStop();
		checkDhs( pNotifyCmd->apply( pQlt->connect(), dhsStatus ), 
			dhsStatus, status, VOID );
		checkDhs( pNotifyCmd->wait( dhsStatus ), 
			dhsStatus, status, VOID );

		if ( status.ok() )
		{
		    status.S_QLT_NOTIFY( status, pQlt->connect().name(), 
			    pNotifyCmd->frame().dataset().datasetName(),  
			    pNotifyCmd->frame().frameId() );
		    status.display();
		}

		else
		{
		    if ( pQlt->connect().isOpen() )
		    {
			status.display();
		    }
		    else
		    {
			cout << "message: " << status.message();
			// status.display();
			status.clear();
		    }
		}
		status.displayStart();

		pNotifyCmd->frame().unlock();
	    }

	    pNotifyCmd->frame().dataset().deRef();

	    delete pNotifyCmd;
	}
    }

    pQlt->qqtThreadRunning = false;
    dhsStatus = DHS_S_SUCCESS;
    dhsElBroadcast( &dhsStatus );

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::notify
//
// INVOCATION:
// cQlsQlt::notify( frame, pQlt, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) frame (cQlsFrame &) The frame that has been changed.
// (>) arg (void *) Pointer to the quick look tool to notify.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Notify a QLT that a frame has changed
//
// This function is intended to be called by the cQlsDataset::forEachFrame
// function.
//
// DESCRIPTION:
// Execute function qltNotify.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsQlt::notify
(
    cQlsFrame	&frame,		// (in)  The frame that has changed.
    void	*arg,		// (in)  Pointer to the quick look tool to
    				//	 be notified.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    cQlsQlt	*pQlt( (cQlsQlt *) arg );


    pQlt->qltNotify( frame, frame.frameId(), status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::qltNotify
//
// INVOCATION:
// qltNotify( frame, frameId, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cQlsFrame (cQlsFrame &) The frame that has changed.
// (>) frameId (const char *) The frame id to use for the frame.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Notify the Qlt about the change in the frame data
//
// DESCRIPTION:
// If the frame has been modified, send a qltNotify command to the QLT.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsQlt::qltNotify
(
    cQlsFrame	&frame,		// (in)  The frame that has changed.
    const char	*frameId,	// (in)  The frame id to send with the notify.
    cQlsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );	
				// DHS library function return status.
    cQlsNotifyCmd		// Command to be used to notify a QLT.
    		*notifyCmd;


    checkStat( status, return );


    //
    // Has the frame been modified?
    //

    frame.rLock();
    if ( frame.modified() )
    {
	//
	// Create a new command object.
	//

	notifyCmd = new cQlsNotifyCmd( frame );


	//
	// Add the attributes to the command.
	//

	notifyCmd->add( "datasetName", DHS_DT_STRING, 
		frame.dataset().datasetName(), dhsStatus );
	notifyCmd->add( "frameId", DHS_DT_STRING, frameId, dhsStatus );

	if ( &(frame.dataset().lastFrame()) == &frame )
	{
	    notifyCmd->add( "lastFrame", DHS_DT_STRING, "true", dhsStatus );
	}
	else
	{
	    notifyCmd->add( "lastFrame", DHS_DT_STRING, "false", dhsStatus );
	}

	frame.regionsAdd( *notifyCmd, status );
	frame.unlock();

	//
	// Send the command to the QLT.
	//

	frame.dataset().ref();
	qqtQueueCond.lock();
	qqtCommandQueue.push( notifyCmd );
	qqtQueueCond.unlock();
	qqtQueueCond.broadcast();
    }
    else
    {
	frame.unlock();
    }
}
