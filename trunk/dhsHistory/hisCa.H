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
// dhsHistory/h/hisCa.H
//
// PURPOSE:
// Contains the interface to the cHisCa class
//
// CLASSES:
// cHisCa - Takes care of the EPICS Channel Access.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:04  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:22  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/07/31 20:54:29  cockayne
// Removed init() method. - Not used.
//
// Revision 1.3  1998/04/17 22:51:32  nhill
// Updated the the newest version of the cDhsStatus class.
//
// Revision 1.2  1997/10/20 21:47:53  cockayne
// Check-in for install.
//
// Revision 1.1  1997/10/06 20:16:22  cockayne
// Initial revision
//
// Revision 1.2  1997/08/20 21:19:48  nhill
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

#ifndef HISCA_H
#define	HISCA_H

#include <string>
#include <list>
#include <map>
#include <stack>
#include "genMutex.H"
#include "genCond.H"

//
// Define status checking macro.
//

#define checkCa( fn, s, ret )						\
    {									\
	int             caStatus;					\
	if ( (caStatus = fn) != ECA_NORMAL )				\
	{								\
	    s.E_CA( s, caStatus, #fn, ca_message( caStatus ) );		\
	    ret;							\
	}								\
    }



//
//***********************************************************************
//+
// CLASS NAME:
// cHisCa
//
// PUBLIC METHODS: 
// 		cHisCa		- Constructor.
// 		~cHisCa		- Destructor.
// (void)	config		- Init's config'ed data from config file.
// (void)	channelLoop	- Channel access's event loop.
// (void)	channelLoopStart- Starts channel access's event loop.
// (void)	channelLoopStop	- Starts channel access's event loop.
// (char *)	tempDir		- Returns path to the working directory. 
// (void)	testChannel 	- Tests a channel. 
// (void)	testChannelAccess
//				- Tests all channels.
// (void)	testDiskArea	- Tests the disk area. 
// (void)	chanListAdd	- Add a channel to the channel list. 
// (void)	callbackAdd	- Adds a callback for a channel.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the EPICS Channel Access. 
//
//-
//***********************************************************************
//

class	cHisCa
{
    public:
	//
	// Constructors, Destructors.
	//
	
	inline		cHisCa( char * name )
			{
			    hcChannelName = strsav( name );
			};
	inline		~cHisCa( void )
			{
			    free( hcChannelName );
			};


	//
	// Static methods.
	//

	static void	config( cHisStatus & );
	static void	channelLoop( cHisStatus & );
	static void	channelLoopStart( cHisStatus & );
	static inline void
			channelLoopStop( void )
			{
			    hcChannelLoopDone = true;
			};
	static inline char *   
			tempDir( void )
			{
			    return( hcDirectory );
			};
	static void	testChannelAccess( cHisStatus & );
	static void	testDiskArea( cHisStatus & );


	//
	// Non-static methods.
	//

	void		chanListAdd( cHisStatus & );
	void		callbackAdd( cHisStatus & );
	void		callbackRemove( cHisStatus & );
	void		testChannel( cHisStatus & );

	
	
	//
	// Typedefs.
	//

	typedef	    std::map< std::string,	// Sort key is of datatype 'string'.
			cHisCa *,	// Insert into container of this type.
			std::less<std::string> >	// This is the sorting mechanism.
		    tChanList;		// Provides access to container.
	typedef tChanList::iterator
		    iChanList;		// Provides an iterator to above.


    protected:
    private:
	//
	// Private methods.
	//

	static boolean	channel( int, char ** );
	static boolean	directory( int, char ** );
	static void	channelCallback( struct event_handler_args );

	
	//
	// Private Data.
	//

	static tChanList
			hcChannelList;		// Lists channels to connect. 
	static bool	hcChannelLoopDone;	// Done with event loop yet?
	static char	*hcDirectory;		// Directory to use. 

	char		*hcChannelName;		// Object's channel name.
	evid		hcEvId;			// Channel Access Lib `evid'.
};
#endif // HISCA_H
