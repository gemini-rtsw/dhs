static char rcsid[] = "$Id: stoDbm.C,v 1.1.1.1 2002-11-24 20:31:50 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
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
// dhsStorage/src/stoDbm.C
//
// PURPOSE:
// Contains methods of cStoDbManager class. 
//
// METHOD NAME(S)
// cStoDbManager::test	- Test database functionality.
// cStoDbManager::init	- Initialize database connections.
// cStoDbManager::exit	- Exit, freeing resources held by cStoDbManger class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.6  1999/10/12 22:51:55  nhill
// Added the DHS_NO_BULK symbol.
//
// Revision 1.5  1999/06/18 22:30:02  nhill
// Added call to clear the am messages after initialization.
//
// Revision 1.4  1999/06/16 23:56:10  nhill
// Changed the calls to amDbInfoGet. Added a call to cleanup the am library.
//
// Revision 1.3  1998/04/23 16:14:45  cockayne
// Modified to use new status and health.
//
// Revision 1.2  1998/04/14 16:57:28  cockayne
// Cleaned up, added comments.
//
// Revision 1.1  1998/03/10 18:29:00  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
 
extern "C"
{
#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"

#include "db.h"
}

#define DHS_NO_COMMAND
#define DHS_NO_GET
#define DHS_NO_PUT
#define DHS_NO_SERVER

#include "globals.H"
#include "stoEpicsRecord.H"
#include "stoDhs.H"
#include "stoDbm.H"

#include "stoFile.H"


//
// Define static data for the cStoDbManager class. 
//

struct dbprocess *
		cStoDbManager::sdmDbProcess = NULL;
cMutex		cStoDbManager::sdmDbProcMutex;
char		*cStoDbManager::sdmAmDbServer = NULL;
char		*cStoDbManager::sdmAmDatabase = NULL;
char		*cStoDbManager::sdmAmArchiveRetrievalStage = NULL;
char		*cStoDbManager::sdmAmUserRetrievalStage = NULL;
char		*cStoDbManager::sdmAmMediaStage = NULL;




//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDbManager::test
//
// INVOCATION:
// test( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &)	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Tests the database
//
// DESCRIPTION:
// Uses its own db process. Tests the database by opening and 
// rolling back a transaction. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None. 
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStoDbManager::test
(
    cStoStatus	&status		// (mod) Program status.
)
{
    DBPROCESS	*dbProcess;	// Database process to use.
    char	*database;	// Database to use.
    char	*dbServer;	// Database server to use. 


    checkStat( status, return );


    //
    // Fetch am database information for the db_open() call.
    //

    checkAm( amDbInfoGet( &dbServer, &database, NULL, NULL, NULL, NULL ),
	    status, return );


    //
    // Open a dbprocess. 
    //

    checkDbLib( db_open( dbServer, NULL, NULL, database, FALSE, 
	    &dbProcess ), status, return );

 
    //
    //  Test by opening and rolling back a tranasaction.
    //
 
    checkDbLib( db_simple_query( dbProcess,
            "begin transaction \n" ), status, VOID );
 
    checkDbLib( db_simple_query( dbProcess,
            "rollback transaction \n" ), status, VOID );
 

    //
    // Close the dbprocess.
    //

    db_close( dbProcess );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDbManager::init
//
// INVOCATION:
// init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &) Program status;
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Initialise the database 
//
// DESCRIPTION:
// Call db_init() and db_open() to initialize and connect to the 
// database. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None. 
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void 		cStoDbManager::init
(
    cStoStatus	&status		// (mod) Program status.
)
{
    char	*appName;	// Application name to register with db.
 

    checkStat( status, return );
 
 
    //
    // Initialize db library.
    //
 
    appName = NULL;
    checkNull( appName = strsav( cDhs::identity() ), status, return );
    check( db_init( appName ), status, return );
 
 
    //
    // Initialize am library. 
    //

    checkAm( amInit(), status, return );
    msg_clear( amMsg );

    
    //
    // Fetch am library parameters, initialize database connection.
    //
 
    checkAm( amDbInfoGet( &sdmAmDbServer, &sdmAmDatabase, NULL, NULL, NULL, 
	    NULL ), status, return );
    checkDbLib( db_open( sdmAmDbServer, NULL, NULL, sdmAmDatabase, FALSE, 
	    &( sdmDbProcess ) ), status, return );


    //
    // Save the staging directories in program data. 
    //

    amDataStageGet( &sdmAmArchiveRetrievalStage, &sdmAmUserRetrievalStage, 
	    &sdmAmMediaStage );


    //
    // Free memory allocated to save the identity. 
    //

    gen_free( appName );

    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoDbManager::exit
//
// INVOCATION:
// exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoStatus &) 	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Close the database 
//
// DESCRIPTION:
// Calls db_close() and db_exit() to close down the database. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None. 
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void 		cStoDbManager::exit
(
    cStoStatus	&status		// Program status. 
)
{
    checkStat( status, return );


    //
    // Clean up the am library. 
    //

    amClose();


    db_close( cStoDbManager::sdmDbProcess );
    cStoDbManager::sdmDbProcess = NULL;
    db_exit();
}
