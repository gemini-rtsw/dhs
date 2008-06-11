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
// dhsSta/h/localCSta.H
//
// PURPOSE:
// Contains the interface to the cStaCommand class.
// Whatever
//
// CLASSES:
// cStaCommand - This is the class for commands sent to the status server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/04/24 21:21:35  nhill
// Moved this file from the dhsGen library.
//
// Revision 1.3  1998/04/21 17:39:59  nhill
// Updated the documentation.
//
// Revision 1.2  1998/04/21 17:36:48  nhill
// Updated the documentation.
//
// Revision 1.1  1997/09/26 18:09:08  nhill
// Initial revision
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef LOCALCSTA_H
#define	LOCALCSTA_H
//
//***********************************************************************
//+
// CLASS NAME:
// cStaCommand
//
// PUBLIC METHODS: 
// (none) cStaCommand - Constructor for the class.
// (none) ~cStaCommand - Destructor for the class.
// (void) waitDone) - Wait until all commands of this class are complete.
//
// PURPOSE:
// This is the class used to send commands to the status server
//
// The commands could be either status update or message log commands.
// The main difference between this class and the cDhsClientCommand class
// is that this class keeps track of how many commands of this class 
// exist.
//
//-
//***********************************************************************
//

class 	cStaCommand: public cDhsClientCommand
{
    public:
	inline		cStaCommand( char * command ) : 
				cDhsClientCommand( command )
			{ 
			    scCounter ++;
			}
	inline		~cStaCommand( void )
			{
			    scCounter --;
			}
	inline static void
			waitDone()
			{
			    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
			    dhsEventLoop( DHS_ELT_COND, isDone, NULL, 
				    &dhsStatus );
			}
    protected:
    private:
	static bool	isDone( void  * );
	void		done( DHS_CMD_STATUS, char *, const cDhsAvList & );

	static int	scCounter;
};
#endif /* LOCALCSTA_H */
