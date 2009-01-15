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
// dhsStorage/h/stoRequestCmd.H
//
// PURPOSE:
// This file contains code to handle Media Request commands. 
//
// CLASSES:
// cStoRequestCmd - Handles media request commands. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:33  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.7  2001/03/12 20:42:33  nhill
// Added method fileAvailParse to class cStoRequestCmd.
//
// Revision 1.6  2000/02/03 00:52:41  nhill
// Added data member srcOptionList to the cStoRequestCmd class to store
// the list of asto application options to be used for user requests.
// Added the config function to read the asto options from the config
// file.
// Added function userAppOptions to parse lines from the config file.
//
// Revision 1.5  2000/01/05 20:42:26  nhill
// Added method validPath to the cStoRequestCmd class.
//
// Revision 1.4  1999/10/12 23:23:08  nhill
// Removed the exec member function.
// Broke the exec functions into smaller units.
//
// Revision 1.3  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.2  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOREQUESTCMD_H
#define	STOREQUESTCMD_H

#include "string"
#include "map"

#define DHS_NO_BULK
#define DHS_NO_GET
#define DHS_NO_PUT

//
//***********************************************************************
//+
// CLASS NAME:
// cStoRequestCmd
//
// PUBLIC METHODS:
// (void) config	- Read the default am application options from 
//			  the configuration file.
// (void) execStoArchiveFileAvailable
//			- Called to indicate an archive file is available.
// (void) execUserMediaRequest
//			- Called to create a new user media request.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles incoming media request commands. 
//
//-
//***********************************************************************
//
 
class   cStoRequestCmd: public cDhsServerCommand
{
    public:
	static void	config( cStoStatus & );
	void		execStoArchiveFileAvailable( void );
	void		execUserMediaRequest( void );

    protected:
    private:
	void		addMediaFiles( const AM_MEDIA &, char**, char**, 
				char**, char **, int, cStoStatus & );
	void		fileAvailParse( char *, char *, int &, char *, char &, 
				cStoStatus & );
	void		newUserRequest( AM_MEDIA &, cStoStatus & );
	static boolean	userAppOptions( int, char ** );
	void		userReqParseFileInfo( char **&, char **&, char **&, 
				char **&, int &, cStoStatus & );
	void		userReqParseReqInfo( char *, char *, char *, int &,
				char *, bool &, cStoStatus & );
	bool		validPath( const char * );
	
	static std::map< std::string, std::string, std::less< std::string > >
			srcOptionList;	// List of options for each AM	
					// application.  These are inserted
					// into the am database when user
					// requests are created.
};
#endif // STOREQUESTCMD_H
