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
// dhsQlServer/h/qlsSlave.H
//
// PURPOSE:
// Contains the interface to the cQlsSlave class.
//
// CLASSES:
// cQlsSlave - An object of this class is created to describe each slave
//		QLS connected to this QLS.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/27 20:52:19  nhill
// Added a testAll method to the cQlsSlave class.
//
// Revision 1.2  1997/11/30 21:22:23  nhill
// Changed the cQlsSlave class to use the cListOf class to keep the list
// of all slaves.
//
// Revision 1.1  1997/08/20 21:21:32  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSSLAVE_H
#define	QLSSLAVE_H

#include "genListOf.H"

class	cQlsStream;
class	cDhsPutRequest;

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsSlave
//
// PUBLIC METHODS: 
//		cQlsSlave	- Constructor for the class.
//		~cQlsSlave	- Destructor for the class.
// (void) checkForward		- Check if a dataset should be forwarded, and
//				  if necessary, forward it.
// (static void)exit		- Delete all cQlsSlave objects.
//
// PUBLIC TYPES:
// tCfArg - Type to allow a pair of arguments to be passed to the forEach 
//		method.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains information about each slave Quick Look Server
//
// An object of this class is created each time a slave connects, and
// is destroyed when the slave disconnects.
//-
//***********************************************************************
//

class	cQlsSlave: 
		public cQlsSubscriber,
		public cListOf< cQlsSlave, DHS_CONNECT, cQlsStatus >
{
    public:
	typedef	pair< cQlsDataset *, cDhsPutRequest * > tCfArg;
    
			cQlsSlave( DHS_CONNECT );
	virtual		~cQlsSlave( void );
	void		checkForward( void *, cQlsStatus & );
	inline static void
			exit( cQlsStatus & );
	static void	testAll( cQlsStatus &status )
			{
			    forEach( &cQlsSlave::test, NULL, status );
			}
    protected:
    private:
	void		dsStreams( cQlsDataset &, cQlsStatus & );
	void		forward( cDhsPutRequest *, cQlsStatus & );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsSlave::exit
//
// INVOCATION:
// cQlsSlave::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Clean up the static data in the cQlsSlave class.
//
// DESCRIPTION:
// Delete all objects in the list of all cQlsSlave objects.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This function assumes all cQlsSlave objects are allocated on the heap.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cQlsSlave::exit
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    deleteAll();
}
#endif // QLSSLAVE_H
