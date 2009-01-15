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
// dhsQlServer/h/qlsMaster.H
//
// PURPOSE:
// Whatever
//
// CLASSES:
// cQlsMaster - Describes the master Quick Look Server sending data to
//		the current Quick Look Server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1997/10/22 17:48:05  nhill
// Changed the order of the master name and address in the constructor.
//
// Revision 1.2  1997/08/20 21:20:20  nhill
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

#ifndef QLSMASTER_H
#define	QLSMASTER_H

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsMaster
//
// PUBLIC METHODS: 
// (static void)config		- Read the master info from the config file.
// (static void)close		- Close the connection to the master QLS.
// (static bool)isSlave		- Set or get the qlmIsSlave Member.
// (static void)open		- Open the connection to the master QLS.
// (static void)queryStreams	- Send a query to the master QLS requesting
//				  a list of active streams.
// (static void)updateStreams	- Send a new list of subscribed streams to
//				  the master QLS.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None
//
// PURPOSE:
// Describes the master QLS attached to this server
//
//-
//***********************************************************************
//

class cQlsMaster: public cDhsConnection
{
    public:
	inline static void
			close( cQlsStatus & );
	static void	config( cQlsStatus & );
	inline static bool	
			isSlave( void )
			{
			    return( qlmIsSlave );
			}
	inline static bool	
			isSlave( bool newVal )
			{
			    bool oldVal( qlmIsSlave );
			    qlmIsSlave = newVal;
			    return( oldVal );
			}
	static void	open( cQlsStatus &status );
	static void	queryStreams( cDhsAvList &, cQlsStatus & );
	static void	updateStreams( cQlsStatus & );
    protected:
    private:
	inline		cQlsMaster( char * name, char * address ): 
				cDhsConnection( address, name ){};
	static boolean	masterId( int, char ** );

	static bool	qlmIsSlave;
	static class cQlsMaster
			*qlmMaster;
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsMaster::close
//
// INVOCATION:
// cQlsMaster::close( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Close the connection to the master QLS
//
// DESCRIPTION:
// If the connection is open, close it.
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

void		cQlsMaster::close
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    if ( qlmMaster != NULL && qlmMaster->cDhsConnection::isOpen() )
    {
	checkDhs( qlmMaster->cDhsConnection::close( dhsStatus ), 
		dhsStatus, status, return );
    }
}
#endif // QLSMASTER_H
