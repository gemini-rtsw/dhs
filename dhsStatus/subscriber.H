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
// dhsStatus/h/subscriber.H
//
// PURPOSE:
// Contains the interface to the subscriber class.
//
// CLASSES:
// cStaSubscriber - An object of this class is created for each subscriber
//		attached to the status server.
// cStaSubscribe - An object of this class is created for each subscribe or
//		unsubscribe command received by the status server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:29  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/06/24 23:06:15  nhill
// Added a kill function to the subscriber class.
//
// Revision 1.2  1998/06/19 18:10:40  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.1  1998/05/12 20:20:58  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef SUBSCRIBER_H
#define	SUBSCRIBER_H
#include <list.h>
#include <stack.h>

extern "C" {
#include "gen_types.h"
#include "gen_str.h"
}

#define	DHS_NO_BULK
#include "dhs++.H"
#define	NO_FIND_CREATE
#include "genListOf.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cStaSubscriber
//
// PUBLIC METHODS: 
// (None) cStaSubscriber - Constructor for the class.
// (None) ~cStaSubscriber - Destructor for the class.
// (void) exit - Clean up the class for exit.
// (void) flush - Send all unflushed commands to the subscribers.
// (void) postAll - Post a status channel to all subscribers.
// (void) post - Post a channel value to a subscriber.
// (void) send - Send the current status update command to the subscriber.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// An object of this class is created for each ICD1c subscriber to DHS 
// status data
//
//-
//***********************************************************************
//

class cStaSubscriber:
	    public cListOf< cStaSubscriber, string, cStaStat >
{
    friend class cStaSubscribe;

    public:
	typedef list<cDhsClientCommand *> tUpdateQueue;

			cStaSubscriber( DHS_CONNECT, char *, char * );
			cStaSubscriber( char * );
			~cStaSubscriber( void );
	static void	exit( cStaStat & );
	static void	flush( cStaStat & );
	void		kill( cStaStat & );
	static void	postAll( cStaChannel &channel, cStaStat &status )
			{
			    cStaSubscriber::forEach( &cStaSubscriber::post, &channel, status );
			}
	void		post( void *, cStaStat & );
	void		send( void *, cStaStat & );
    protected:
    private:
	cDhsClientCommand
			*popWait( void );
	void		push( cDhsClientCommand * );
	static void	*updateThread( void * );
	static void	connectCallback( cDhsConnection *, void * );

	cCond		ssCond;
	bool		ssExitNow;
	cDhsConnection	ssConnect;
	static cMutex	ssMutex;
	char		*ssMonitorName;
	tUpdateQueue	ssUpdateQueue;
	pthread_t	ssUpdateThread;
	cDhsClientCommand
			*ssUpdateCmd;
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStaSubscribe
//
// PUBLIC METHODS: 
// (void) subscribe - The connector wants to subscribe to the status items.
// (void) unsubscribe - The connector want to unsubscribe for the status 
//		items.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// An object of this class is created for each subscribe or unsubscribe
// command received
//
//-
//***********************************************************************
//

class	cStaSubscribe: public cDhsServerCommand
{
    public:
	void	subscribe( void );
	void	unsubscribe( void );
    protected:
    private:
};
#endif // SUBSCRIBER_H
