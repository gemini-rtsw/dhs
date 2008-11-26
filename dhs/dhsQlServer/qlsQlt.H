//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997.				(c) 1997
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
// dhsQlServer/h/qlsQlt.H
//
// PURPOSE:
// Contains the interface to the cQlsQlt class.
//
// CLASSES:
// cQlsQlt - Contains information about each quick look tool attached to
//		a server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:56  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.8  1999/01/20 23:23:21  nhill
// Removed the unused and undefined cQlsQlt::connectCallback method.
//
// Revision 1.7  1998/06/16 22:15:07  nhill
// Added a flag to clean up the way exiting the command thread is done.
//
// Revision 1.6  1998/04/27 20:51:57  nhill
// Added a testAll method to the cQlsQlt class.
//
// Revision 1.5  1997/12/05 00:15:41  nhill
// Changed to use a single, separate thread to send commands to the QLT.
//
// Revision 1.4  1997/11/30 21:21:16  nhill
// Changed the cQlsQlt class to use the cListOf class to keep a list of
// all Qlts.
//
// Revision 1.3  1997/10/01 17:26:46  nhill
// Second alpha release.
//
// Revision 1.2  1997/08/20 21:20:47  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSQLT_H
#define	QLSQLT_H

#include <list>

#include <queue>

#include "genListOf.H"
#include "qlsStream.H"

class	cQlsFrame;
class	cQlsNotifyCmd;

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsQlt
//
// PUBLIC METHODS: 
//		cQlsQlt		- The constructor for the class.
//		~cQlsQlt	- The destructor for the class.
// (void)	checkNotify	- If necessary, notify a QLT that a frame 
//				  has changed.
// (static void)exit		- Delete all existing cQlsQlt objects.
//
// PURPOSE:
// Contains information about each QTL attached to the server
//
//-
//***********************************************************************
//

class	cQlsQlt: 
	    public cQlsSubscriber, 
	    public cListOf< cQlsQlt, DHS_CONNECT, cQlsStatus >
{
    public:
			cQlsQlt( DHS_CONNECT );
	virtual		~cQlsQlt( void );
	void		checkNotify( void *, cQlsStatus & );
	static boolean	checkRunningFlag( void * );
	inline static void
			exit( cQlsStatus & );
        static void	testAll( cQlsStatus &status ) 
			{
			    forEach( &cQlsQlt::test, NULL, status );
			}
        static void	notify( cQlsFrame &, void *, cQlsStatus & ); // XXX allan: was private
    protected:
    private:
	static void	*commandThread( void * );
	void		qltNotify( cQlsFrame &, const char *, cQlsStatus & );

	std::queue< cQlsNotifyCmd *, std::list< cQlsNotifyCmd * > >
			qqtCommandQueue;
	bool		qqtExitThread;
	cCond		qqtQueueCond;
	pthread_t	qqtThreadId;
	bool		qqtThreadRunning;
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsQlt::exit
//
// INVOCATION:
// cQlsQlt::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deletes all cQlsQlt objects
//
// DESCRIPTION:
// Loop through the list of objects, deleting them all.
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

void		cQlsQlt::exit
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    deleteAll();
}
#endif // QLSQLT_H
