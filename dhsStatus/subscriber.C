static char rcsid[] = "$Id: subscriber.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998				(c) 1998
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
// dhsStatus/src/subscriber.C
//
// PURPOSE:
// Contains the implementation of the cStaSubscriber and cStaSubscribe 
// classes.
//
// METHOD NAME(S)
// cStaSubscriber::cStaSubscriber - constructor for the class.
// cStaSubscriber::~cStaSubscriber - constructor for the class.
// cStaSubscriber::connectCallback - Executed when the connection associated
//		with a subscriber changes state.
// cStaSubscriber::exit - Clean up the cStaSubscriber class before exit.
// cStaSubscriber::flush - Send all update commands for all subscribers.
// cStaSubscriber::popWait - Pop the next update command from this 
//		subscriber from the command queue, or wait for a new update 
//		if the command queue is empty.
// cStaSubscriber::post - Add a new status item to the list of items to be
//		sent to a subscriber.
// cStaSubscriber::push - Add a new update command to the command queue.
// cStaSubscriber::send - Add the current command to the command queue
//		and zero the current command.
// cStaSubscriber::updateThread - This function is run in a separate thread,
//		and pops commands off the subscriber's command queue and 
//		executes them.
// cStaSubscribe::subscribe - This method is executed to start the processing
//		of a subscribe command.
// cStaSubscribe::unsubscribe - This method is executed to start the processing
//		of an unsubscribe command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:30:55  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1999/06/16 22:30:20  nhill
// Removed code to send status values to subscribers on startup. This
// should be done, but OCSWish doesn't respond well to the early status
// reports.
//
// Revision 1.6  1998/06/24 23:19:16  nhill
// Added the cStaSubscriber::kill method.
// Changed the way subscriber objects and subscriber threads are killed.
//
// Revision 1.5  1998/06/19 18:04:24  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.4  1998/05/19 18:13:59  nhill
// Changed connection callback to allow for more than one status acceptor
// on each connection.
//
// Revision 1.3  1998/05/15 21:29:28  nhill
// Changed the position of the sleep.
//
// Revision 1.2  1998/05/15 21:14:13  nhill
// Added a sleep to allow the subscriber to get ready for the updates after
// it initially subscribes.
//
// Revision 1.1  1998/05/12 20:26:50  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <unistd.h>
#include <pthread.h>

#include "globals.H"
#if defined(EPICS_DHS)
#include "staChannel.H"
#endif
#include "subscriber.H"

cMutex	cStaSubscriber::ssMutex;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::cStaSubscriber
//
// INVOCATION:
// N.A.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (DHS_CONNECT) Connection to the subscriber.
// (>) monitorName (char *) Name of the status acceptor.
// (>) monitorId (char *) Id of the subscriber.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initializes a new subscriber object
//
// DESCRIPTION:
// - Intialize the data members.
// - Create a new thread to handle sending commands to the subscribers.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The connection must be an open connection to the subscriber.
// There must not be another monitor with the same monitorId.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cStaSubscriber::cStaSubscriber
(
    DHS_CONNECT	connect,	// (in)  The connection to the subscriber.
    char	*monitorName,	// (in)  The name of the status monitor.
    char	*monitorId	// (in)  Id of the status monitor.
): cListOf< cStaSubscriber, std::string, cStaStat > ( monitorId ),
	ssConnect( connect )
{
    cStaStat	status;

    
    ssConnect.callbackSet( connectCallback, this );
    ssMonitorName = strsav( monitorName );
    ssExitNow = false;
    ssUpdateCmd = NULL;
    ssUpdateThread = 0;

    checkSystem( pthread_create( &ssUpdateThread, NULL, updateThread, this ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::~cStaSubscriber
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
// Destructor for the class
//
// DESCRIPTION:
// - Set a flag and do a broadcast to cause the subscribers command thread
//   to exit.
// - Delete any unsent command on the command queue.
// - Delete the current command.
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

		cStaSubscriber::~cStaSubscriber
(
    void
)
{
    cDhsClientCommand
    		*pCmd;


    //
    // Delete any unsent command on the queue.
    //

    while ( ssUpdateQueue.size() != 0 )
    {
	pCmd = ssUpdateQueue.front();
	delete pCmd;
	ssUpdateQueue.pop_front();
    }


    //
    // Delete the current command.
    //

    if ( ssUpdateCmd != NULL )
    {
	delete ssUpdateCmd;
    }


    gen_free( ssMonitorName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// name
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) paramName (int) Descripton of the parameter
// (!) paramName (float) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
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
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStaSubscriber::kill
(
    cStaStat	&status		// (mod) Function return status.
)
{
    //
    // Cause the command sender thread to exit.
    //

    ssExitNow = true;
    ssCond.broadcast();
    pthread_join( ssUpdateThread, NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::connectCallback
//
// INVOCATION:
// N.A. Called automatically by the dhs++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (cDhsConnection &) Connection that changed state.
// (>) subscriber (void *) User data pointer. In this case, the subscriber
//		associated with the connection.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is executed when the connection associated with a subscriber
// changes state
//
// DESCRIPTION:
// Delete the cStaSubscriber object associated with the connection.
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

void		cStaSubscriber::connectCallback
(
    cDhsConnection		// (in)  Connection that changed state.
    		*connect,
    void	*		// (in)  Not used.
)
{
    cStaSubscriber::iObjectList
		i;
    cStaSubscriber
    		*pSubscriber;
    cStaStat	status;


    if ( ! connect->isOpen() )
    {
	for ( i = cStaSubscriber::begin(); i != cStaSubscriber::end(); 
		i = cStaSubscriber::begin() )
	{
	    pSubscriber = (*i).second;
	    if ( pSubscriber->ssConnect == *connect )
	    {
		pSubscriber->kill( status );
	    }
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::exit
//
// INVOCATION:
// cStaSubscriber::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all subscribers
//
// DESCRIPTION:
// - Delete each object in the list of cStaSubscriber objects.
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

void		cStaSubscriber::exit
(
    cStaStat	&status		// (mod) Function return status.
)
{
    cStaSubscriber::iObjectList
		i;


    checkStat( status, return );


    for ( i = cStaSubscriber::begin(); i != cStaSubscriber::end(); 
	    i = cStaSubscriber::begin() )
    {
	((*i).second)->kill( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::flush
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// For each subscriber, send any unsent update commands
//
// DESCRIPTION:
// Trivial
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

void		cStaSubscriber::flush
(
    cStaStat	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    forEach( &cStaSubscriber::send, NULL, status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::popWait
//
// INVOCATION:
// pUpdate = subscriber.popWait();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (cStaPost *) The next update for this subscriber.
//
// PURPOSE:
// Pop the next update for this subscriber, or wait if the queue is empty
//
// To allow the calling routine to stop waiting without having a command
// to send, this routine returns NULL if the queue is empty and the queue
// is still empty. It is the calling routines responsibility to check for
// NULL and re-execute this routine if the broadcast was spurious.
//
// DESCRIPTION:
// If there is something on the queue pop it and return a pointer to it.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The ssCond cond condition variable must be locked before executing this
// function and unlocked after it returns.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cDhsClientCommand	
		*cStaSubscriber::popWait
(
    void
)
{
    cDhsClientCommand	
		*pCmd;


    //
    // If the queue is empty, wait for the condition variable.
    //

    if ( ssUpdateQueue.size() == 0 )
    {
	ssCond.wait();
    }
    if ( ssUpdateQueue.size() == 0 )
    {
	pCmd = NULL;
    }
    else
    {
	pCmd = ssUpdateQueue.front();
	ssUpdateQueue.pop_front();
    }

    return( pCmd );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::post
//
// INVOCATION:
// subscriber.post( channel, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) channel (void *) Pointer to the channel.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Update a channel value for a subscriber
//
// This method doesn't actually send a command to the subsciber, but only
// adds a new attribute to the update command. The processes of sending the
// command is initiated with the send method.
//
// DESCRIPTION:
// - Ensure the subscriber isn't in the process of shutting down.
// - If there is no current update command, create a new current update command.
// - Add an attribute to the command containing the channel value.
// - Add an attribute to the command containing the information about the value.
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

void		cStaSubscriber::post
(
    void	*channel,	// (in)  The channel to update.
    cStaStat	&status		// (mod) Function return status.
)
{
#if defined(EPICS_DHS)
    cStaChannel	*pChannel( (cStaChannel *) channel );
#endif
    ulong	data[3];
    int		nDims;
    ulong	dims[1];
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	buffer[128];

    checkStat( status, return );


    //
    // Ensure the subscriber isn't in the process of shutting down.
    //

    if ( ssExitNow )
    {
	return;
    }


    //
    // Create a new update command if necessary.
    //

    ssMutex.lock();
    if ( ssUpdateCmd == NULL )
    {
	ssUpdateCmd = new cDhsClientCommand( "_postStatus" );
	checkDhs( ssUpdateCmd->add( "acceptorName", DHS_DT_STRING, 
		ssMonitorName, dhsStatus ), dhsStatus, status, 
		ssMutex.unlock(); return );
    }


    //
    // Add the channel value attribute to the update command.
    //

#if defined(EPICS_DHS)
    checkDhs( ssUpdateCmd->add( pChannel->name(), DHS_DT_STRING, 
	    pChannel->data(), dhsStatus ), dhsStatus, status, 
	    ssMutex.unlock(); return );

    //
    // Add the channel info attribute to the update command.
    //

    sprintf( buffer, "%s.info", pChannel->name() );
    dims[0] = 3;
    nDims = 1;
    data[0] = 0;
    data[1] = 0;
    data[2] = pChannel->time();
    checkDhs( ssUpdateCmd->add( buffer, DHS_DT_UINT32, nDims, dims, data,
	    dhsStatus ), dhsStatus, status, ssMutex.unlock(); return );
#endif

    ssMutex.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::push
//
// INVOCATION:
// subscriber.push( pCommand );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pCommand (cDhsClientCommand *) Command to add to the queue.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a command to the subscriber's update command queue
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

void		cStaSubscriber::push
(
    cDhsClientCommand		// (in)  The command to add to the queue.
		*pCommand
)
{
    ssCond.lock();
    ssUpdateQueue.push_back( pCommand );
    ssCond.unlock();
    ssCond.broadcast();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::send
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) (void *) Ignored parameter.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This method queues the current update command for execution
//
// The command will be executed when the subscribers execution thread pops
// it off the queue.
//
// DESCRIPTION:
// - Check to see if the subscriber is in the process of exiting.
// - Copy the pointer to the update command.
// - Change the pointer to the current update command to null.
// - Push the update command onto the command queue.
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

void		cStaSubscriber::send
(
    void 	*,		// (mod) Ignored argument.
    cStaStat	&status		// (mod) Function return status.
)
{
    cDhsClientCommand
    		*pCmd;

    checkStat( status, return );


    if ( ! ssExitNow )
    {
	ssMutex.lock();
	pCmd = ssUpdateCmd;
	ssUpdateCmd = NULL;

	if ( pCmd != NULL )
	{
	    push( pCmd );
	}
	ssMutex.unlock();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscriber::updateThread
//
// INVOCATION:
// pthread_create( &ssUpdateThread, NULL, updateThread, this );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg (void *) A pointer to the cStaSubscriber for this thread. 
//
// FUNCTION VALUE:
// (void *) Ignored
//
// PURPOSE:
// This method is executed in a new thread, and takes care of sending
// queued update commands to the subscriber
//
// The commands are added to the queue by the send method
//
// DESCRIPTION:
// - While the subscriber is not exiting:
//   - pop a command off the queue
//   - If the command is not null:
//     - Execute the command.
//     - Wait for the command to complete.
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

void		*cStaSubscriber::updateThread
(
    void	*arg		// (in)  A pointer to the cStaSubscriber for
    				//	 this thread.
)
{
    cDhsClientCommand		// The command to execute.
		*pCmd;
    cStaSubscriber		// This subscriber.
    		*pSubscriber;
    cStaStat	status;		// Function return status.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    				// DHS library function return status.


    pSubscriber = (cStaSubscriber *) arg;

    sleep( 1 );

    pSubscriber->ssCond.lock();


    //
    // Loop until the subscriber unsubscribes.
    //

    while ( ! pSubscriber->ssExitNow )
    {
	//
	// Try to get a command from the top of the queue.
	//

	pCmd = pSubscriber->popWait();


	if ( pCmd != NULL )
	{
	    //
	    // If a command was retrieved, execute it.
	    //

	    pSubscriber->ssCond.unlock();

	    checkDhs( pCmd->apply( pSubscriber->ssConnect, dhsStatus ),
		    dhsStatus, status, VOID );


	    //
	    // Wait for the command to complete.
	    //

	    checkDhs( pCmd->wait( dhsStatus ), dhsStatus, status, VOID );

	    pSubscriber->ssCond.lock();


	    //
	    // Delete the command.
	    //

	    delete pCmd;
	}
	status.status( status.S_SUCCESS );
	dhsStatus = DHS_S_SUCCESS;
    }
    pSubscriber->ssCond.unlock();

    delete pSubscriber;

    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscribe::subscribe
//
// INVOCATION:
// N.A. Called by a dhs command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called to start the processing of a subscribe command
//
// DESCRIPTION:
// - Get the monitor name from the av list.
// - Create a new cStaSubscriber object for the subscriber.
// - Post all channel data to the subscriber.
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

void		cStaSubscribe::subscribe
(
    void
)
{
    char	id[50];
    char	*monitorName;
    cDhsAttrib	av;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    DHS_DATA_TYPE
    		type;
    int		nDims;
    ulong	dims[7];
    cStaStat	status;
    char	*pName;
    char	*name;
    char	*address;
#if defined(EPICS_DHS)
    cStaChannel::tChMap
		*pChMap;
    cStaChannel	*pChan;
    cStaChannel::iChMap
    		i;
#endif
	cStaSubscriber
    		*pSubscriber;


    status.displayStop();


    //
    // Get the monitor name from the av list.
    //

    checkDhs( av = find( "subscribeTo", dhsStatus ), dhsStatus, status, VOID );
    checkDhs( av.info( pName, type, nDims, dims, (void **) &monitorName, 
	    dhsStatus ), dhsStatus, status, VOID );
    if ( type != DHS_DT_STRING || nDims != 0 || dhsStatus != DHS_S_SUCCESS )
    {
	status.E_SUBSCRIBE_CMD( status );
	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }

    response( DHS_CS_BUSY, dhsStatus );
    status.S_CMD_RECEIVED( status, "_dhsSubscribe" );


    //
    // Create a unique id for the monitor.
    //

    dhsConnectInfo( connect(), &address, &name, &dhsStatus );

    sprintf( id, "%s:%s", name, monitorName );

    cStaSubscriber::ssMutex.lock();


    //
    // If the monitor doesn't already exist, create a new one.
    //

    if ( ( pSubscriber = cStaSubscriber::find( id ) ) == NULL )
    {
	pSubscriber = new cStaSubscriber( connect(), monitorName, id );
    }
    cStaSubscriber::ssMutex.unlock();


#ifdef OMIT
    //
    // Post all status values to the subscriber.
    //

#if defined(EPICS_DHS)
    pChMap = (cStaChannel::tChMap *) cStaChannel::scList;
    for ( i = pChMap->begin(); i != pChMap->end(); i++ )
    {
	pChan = (*i).second;
	if ( ! pChan->scIndexed )
	{
	    pSubscriber->post( (void *) pChan, status );
	}
    }
#endif


    //
    // Send the update command to the subscriber.
    //

    pSubscriber->send( NULL, status );
#endif /* OMIT */

    response( DHS_CS_DONE, dhsStatus );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaSubscribe::unsubscribe
//
// INVOCATION:
// N.A. Called by a dhs command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called to start the processing of an unsubscribe command
//
// DESCRIPTION:
// - Get the monitor name from the av list.
// - find the cStaSubscriber object associated with the monitor.
// - Delete the cStaSubscriber object.
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

void		cStaSubscribe::unsubscribe
(
    void
)
{
    char	id[50];
    char	*monitorName;
    cDhsAttrib	av;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    DHS_DATA_TYPE
    		type;
    int		nDims;
    ulong	dims[7];
    cStaStat	status;
    char	*pName;
    cStaSubscriber
    		*pSubscriber;
    char	*name;
    char	*address;


    status.displayStop();


    //
    // Get the monitor name from the av list.
    //

    checkDhs( av = find( "subscribeTo", dhsStatus ), dhsStatus, status, VOID );
    checkDhs( av.info( pName, type, nDims, dims, (void **) &monitorName, 
	    dhsStatus ), dhsStatus, status, VOID );
    if ( type != DHS_DT_STRING || dims != 0 || dhsStatus != DHS_S_SUCCESS )
    {
	status.E_UNSUBSCRIBE_CMD( status );
	dhsStatus = DHS_S_SUCCESS;
	response( DHS_CS_ERROR, status.message(), dhsStatus );
	status.display();
	return;
    }

    response( DHS_CS_BUSY, dhsStatus );
    status.S_CMD_RECEIVED( status, "_dhsUnsubscribe" );

    dhsConnectInfo( connect(), &address, &name, &dhsStatus );

    sprintf( id, "%s:%s", name, monitorName );


    //
    // If the subscriber object was found, delete it.
    //

    if ( ( pSubscriber = cStaSubscriber::find( id ) ) != NULL )
    {
	pSubscriber->kill( status );
    }

    response( DHS_CS_DONE, dhsStatus );
}
