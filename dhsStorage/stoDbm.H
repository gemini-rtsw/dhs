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
// dhsStorage/h/stoDbm.H
//
// PURPOSE:
// Contains definitions for the database connections used in the 
// DHS Storage Server
//
// CLASSES:
// cStoDbManager - Manages database connections.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.5  1998/04/23 16:15:50  cockayne
// Modified to use new status and health.
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

#ifndef STODBM_H
#define	STODBM_H

extern "C"
{
#include <sybfront.h>
#include <sybdb.h>
#include "db.h"
#include "am.h"
} 

#include "genMutex.H"
#include "globals.H"


//
//
//


//
// Define error checking macros for the am & db libraries.
//
 
#define checkAm( fn, status, ret )					\
    {									\
	int             s;						\
	if ( ( s = fn ) != AM_SUCCESS )					\
	{								\
	    status.E_AM( status );					\
	    status += amMsg;						\
	    cStoDbManager::release();					\
	    ret;							\
	}								\
    }

#define	checkDbLib( fn, status, ret )					\
    {									\
	int		s;						\
	if ( ( s = fn ) != DB_SUCCESS )					\
	{								\
	    status.E_DB_LIB( status, db_msg );				\
	    cStoDbManager::release();					\
	    ret;							\
	}								\
    }



//
//***********************************************************************
//+
// CLASS NAME:
// cStoDbManager
//
// PUBLIC METHODS: 
// (void)	close	- Close db connections.
// (dbprocess *) acquire- Acquires a db process locking to prevent 
//				use by multiple threads. 
// (void)	release	- Release a db process. 
// (void)	exit 	- Releases resources used by cStoDbManager class.
// (void)	init 	- Initialize db libraries and open connection.
// (void)	test	- Test database connection.
// (char *)	getAmArchiveRetrievalStage- Return path of archive stage dir.
// (char *)	getAmUserRetrievalStage- Return path of user stage dir.
// (char *)	getAmMediaStage- Return path of media stage dir.
//
// PURPOSE:
// Facilitates SYBASE database interaction. 
//
// The class makes for thread-safe database interaction using a cMutex
// object, and also stores some of the am library configurable data for
// quick retrieval.
//-
//***********************************************************************
//

class 	cStoDbManager
{
    public:
	static inline struct dbprocess *
			acquire( void )
			{
			    sdmDbProcMutex.lock();
			    dbcanquery( sdmDbProcess );
			    return( sdmDbProcess );
			};
	static inline void
			release( void )
			{
			    sdmDbProcMutex.unlock();
			};

	static void	exit( cStoStatus & );
	static void	init( cStoStatus & );
	static void	test( cStoStatus & );

	inline static char	
			*getAmArchiveRetrievalStage( void )
			{
			    return( sdmAmArchiveRetrievalStage );
			};
	inline static char	
			*getAmUserRetrievalStage( void )
			{
			    return( sdmAmUserRetrievalStage );
			};
	inline static char	
			*getAmMediaStage( void )
			{
			    return( sdmAmMediaStage );
			};

    protected:
    private:
	static struct dbprocess
			*sdmDbProcess;	// DbProcess to use.
	static cMutex	sdmDbProcMutex;	// Mutex prevents misuse of dbprocess. 

	static char	*sdmAmDatabase;	// am library database.
	static char	*sdmAmDbServer;	// am library database server.
	static char	*sdmAmArchiveRetrievalStage;	
					// am library Archive staging path.
	static char	*sdmAmUserRetrievalStage;	
					// am library User staging path.
	static char	*sdmAmMediaStage;	
					// am library Media staging path.
};
#endif // STODBM_H
