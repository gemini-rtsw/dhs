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
// dhsHistory/h/hisDbm.H
//
// PURPOSE:
// Contains the external definitions for the database connections used in 
// the DHS history server.
//
// CLASSES:
//
//INDENT-OFF*
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef HISDBM_H
#define	HISDBM_H

extern "C"
{
#include <sybfront.h>
#include <sybdb.h>
} 

#include "genMutex.H"
#include "dhs++.H"

#define	checkDbLib( fn, status, ret )					\
    {									\
	int		s;						\
	if ( ( s = fn ) != DB_SUCCESS )					\
	{								\
	    status.E_DB_LIB( status, db_msg );				\
	    dbConnRelease();						\
	    ret;							\
	}								\
    }

#define	checkDb( fn, status, ret )					\
    {									\
	int		s;						\
	if ( ( s = fn ) == FAIL )					\
	{								\
	    status.E_DB( status, db_msg, #fn );				\
	    status.display();						\
	    dbConnRelease();						\
	    ret;							\
	}								\
    }

//
// Must declare existence of a class called cHisEvent. 
//

class		cHisEvent;


//
//***********************************************************************
//+
// CLASS NAME:
// cHisDbManager
//
// PUBLIC METHODS: 
// (void)	config		- Initializes config'ed data from config file.
// (void)	dbClose		- Close the db connections.
// (dbprocess *) dbConnGet	- Locks and returns db process. 
// (void)	dbConnRelease	- Unlocks db process. 
// (void)	dbInit 		- Initialize db libraries and open connection.
// (char *)	filesTable	- Returns name of files Table. 
// (char *)	historyDatabase	- Returns name of history logging database. 
// (char *)	historyTable	- Returns name of history logging table. 
// (void)	ingestFile	- Ingests a file into the database. 
// (char *)	serverName	- Returns name of SYBASE server. 
// (void)	put		- Adds put-ing a record to the history table 
//				  into the current db process's query.
// (void)	testDatabase	- Test database connection.
//
// PURPOSE:
// Allows database interaction. 
//
//-
//***********************************************************************
//

class 	cHisDbManager
{
    public:
	static inline struct dbprocess *
			dbConnGet( void )
			{
			    hdmDbProcMutex.lock();
			    dbcanquery( hdmDbProcess );
			    return( hdmDbProcess );
			};
	static inline void
			dbConnRelease( void )
			{
			    hdmDbProcMutex.unlock();
			};
	static inline char *
			historyDatabase( void )
			{
			    return( hdmDatabase ); 
			};
	static inline char *
		    	historyEventTbl( void )
			{
			    return( hdmEventTbl );
			};
	static inline char *
		    	historyLabelTbl( void )
			{
			    return( hdmLabelTbl );
			};
	static inline char *
			serverName( void )
			{
			    return( hdmServer ); 
			};

	static void	config( cHisStatus & );
	static void	checkLabelTbl( char *, bool &, cHisStatus & );
	static void	dbClose( cHisStatus & );
	static void	dbInit( cHisStatus & );
	static void	ingestFile( char *, char *, cHisStatus & );
	static void	putEventTbl( cHisEvent &, dbprocess *, cHisStatus & );
	static void	putLabelTbl( char *, cHisEvent &, cHisEvent &, 
				dbprocess *, cHisStatus & );
	static void	testDatabase( cHisStatus & );

    protected:
    private:
	static boolean	database( int, char ** );	
					// Config database parameters.

	static struct dbprocess
			*hdmDbProcess;	// DbProcess to use.
	static cMutex	hdmDbProcMutex;	// Mutex prevents misuse of dbprocess. 
	static char	*hdmServer;	// Database server name.
	static char	*hdmDatabase;	// Database name.
	static char	*hdmEventTbl;	// History Table name.
	static char	*hdmLabelTbl;	// Files Table name.
};
#endif // HISDBM_H
