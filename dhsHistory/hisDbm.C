static char rcsid[] = "$Id: hisDbm.C,v 1.1.1.1 2002-11-24 20:28:15 brighton Exp $";
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
// <filename>
//
// PURPOSE:
// Whatever
//
// METHOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:21  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/05/05 23:46:07  cockayne
// Added call to free memory allocated during initialization.
//
// Revision 1.3  1998/04/17 22:44:49  nhill
// Updated to use the newest version of the cDhsStatus class.
//
// Revision 1.2  1997/10/20 21:46:41  cockayne
// Check-in for install.
//
// Revision 1.1  1997/10/06 20:15:12  cockayne
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
#include "gen_config.h"

#include "db.h"
}

#include "dhs++.H"

#include "globals.H"
#include "hisDhs.H"
#include "hisDbm.H"
#include "hisEvent.H"
#include "hisLogWrite.H"


//
// Define static data for the cHisDbManager class. 
//

struct dbprocess *
		cHisDbManager::hdmDbProcess = NULL;
cMutex		cHisDbManager::hdmDbProcMutex;
char *		cHisDbManager::hdmServer = NULL;
char *		cHisDbManager::hdmDatabase = NULL;
char *		cHisDbManager::hdmEventTbl = NULL;
char *		cHisDbManager::hdmLabelTbl = NULL;


//
//***********************************************************************
//+
// FUNCTION NAME:
// checkLabelTbl
//
// INVOCATION:
// checkLabelTbl( label, inDb, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) label 	(char *) 	Label to check in db for. 
// (!) inDb	(bool &) 	Flag will be set to true if label in db. 
// (!) status	(cHisStatus &) 	Program status. 
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Checks if label is in db 
//
// DESCRIPTION:
// Gets ( thus locks ) the db process. execute a db_simple_query to check
// if the db contains the label. Releases ( thus unlocking ) the db process. 
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

void		cHisDbManager::checkLabelTbl
(
    char	*label,		// (in)  Label to search for in DB.
    bool	&inDb,		// (mod) Flag to set if label is in db.
    cHisStatus	&status		// (mod) Function return status.
)
{
    DBPROCESS	*dbProc;


    checkStat( status, return );


    dbProc = dbConnGet();
 
 
    //
    //  See if label is in DB.
    //

    checkDbLib( db_simple_query( dbProc,
	    "select \n"
	    "    * \n"
	    "from \n"
	    "    %s..%s \n"
	    "where \n"
	    "    dhsLabel = '%s' \n",
	    cHisDbManager::historyDatabase(),
	    cHisDbManager::historyLabelTbl(),
	    label ), status, return );

    if ( DBROWS( dbProc ) == SUCCEED )
    {
	inDb = true;
    }


    dbConnRelease();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// testDatabase
//
// INVOCATION:
// testDatabase( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &)	Program status.
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

void		cHisDbManager::testDatabase
(
    cHisStatus	&status		// (mod) Program status.
)
{
    DBPROCESS	*dbProc;


    checkStat( status, return );


    //
    // Open a dbprocess. 
    //

    checkDbLib( db_open( cHisDbManager::serverName(), NULL, NULL, 
	    cHisDbManager::historyDatabase(), FALSE, &dbProc ),
	    status, return );

 
    //
    //  Test by opening and rolling back a tranasaction.
    //
 
    checkDbLib( db_simple_query( dbProc,
            "begin transaction \n" ), status, VOID );
 
    checkDbLib( db_simple_query( dbProc,
            "rollback transaction \n" ), status, VOID );
 

    //
    // Close the dbprocess.
    //

    db_close( dbProc );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// config
//
// INVOCATION:
// config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Performs runtime configuration for cHisDbm class
//
// DESCRIPTION:
// Performs runtime configuration for cHisDbm class
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

void		cHisDbManager::config
(
    cHisStatus	&status		// (mod) Program status.
)
{
    checkStat( status, return );


    checkConfig( config_get( HIS_CFG_DATABASE, database ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// database
//
// INVOCATION:
// booleanVariable = directory( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)		Number of tokens in tokens parameter.
// (>) tokens	 (char **)	Pointer to packed array of token strings. 
//
// FUNCTION VALUE:
// (boolean) TRUE	- Tokens were successfully parsed. 
// (boolean) FALSE	- Tokens were not successfully parsed. 
//
// PURPOSE:
// Parse "database" configuration file lines
//
// DESCRIPTION:
// Checks the number of tokens on the line is correct. Frees any previously
// configgured strings, save the new strings. 
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

boolean         cHisDbManager::database
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  List of tokens.
)
{
    boolean	ret;		// Function return value.
    cHisStatus	status;		// Status code.


    //
    // Check the number of tokens on the line is correct.
    //

    if ( numTokens == 5 )
    {
	//
	// Free any existing configged strings.
	//

	if ( hdmServer != NULL )
	{
	    free( hdmServer );
	}
	if ( hdmDatabase != NULL )
	{
	    free( hdmDatabase );
	}
	if ( hdmEventTbl != NULL )
	{
	    free( hdmEventTbl );
	}
	if ( hdmLabelTbl != NULL )
	{
	    free( hdmLabelTbl );
	}


	//
	// Save the new strings.
	//

	checkNull( hdmServer = (char *) strsav( tokens[1] ), status, 
		return( FALSE ) );
	checkNull( hdmDatabase = (char *) strsav( tokens[2] ), status, 
		return( FALSE ) );
	checkNull( hdmEventTbl = (char *) strsav( tokens[3] ), status, 
		return( FALSE ) );
	checkNull( hdmLabelTbl = (char *) strsav( tokens[4] ), status, 
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	//
	// Number of tokens on the line was wrong.
	//

	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// dbInit
//
// INVOCATION:
// dbInit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &) Program status;
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

void 		cHisDbManager::dbInit
(
    cHisStatus	&status		// (mod) Program status.
)
{
    char	*appName = NULL;
    				// Application name to register with db.
 
 
    checkStat( status, return );
 
 
    //
    //	Initialize db library.
    //
 
    checkNull( appName = strsav( cHisDhs::identity() ), status, return );
    check( db_init( appName ), status, return );
 
 
    //
    //	Initialize database connection.
    //
 
    checkDbLib( db_open( cHisDbManager::serverName(), NULL, NULL, 
	    cHisDbManager::historyDatabase(), 
	    FALSE, & ( cHisDbManager::hdmDbProcess ) ), 
	    status, return );


    gen_free( appName );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// dbClose
//
// INVOCATION:
// dbClose( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &) 	Program status.
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

void 		cHisDbManager::dbClose
(
    cHisStatus	&status
)
{
    checkStat( status, return );


    db_close( cHisDbManager::hdmDbProcess );
    cHisDbManager::hdmDbProcess = NULL;
    db_exit();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putLabelTbl
//
// INVOCATION:
// putLabelTbl( label, startEvent, endEvent, dbproc, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) label		(char *)	Label to insert into DB.
// (>) startEvent	(cHisEvent &)	Event to insert into DB.
// (>) endEvent		(cHisEvent &)	Event to insert into DB.
// (>) dbProc		(DBPROCESS *)	Dbprocess to use.
// (!) status		(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Insert a tuple into label table
//
// DESCRIPTION:
// Uses dbfcmd() to insert a tuple into the database. 
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

void		cHisDbManager::putLabelTbl
(
    char	*label,		// (in)  Label to insert into DB.
    cHisEvent	&startEvent,	// (in)  Event to insert into DB.
    cHisEvent	&endEvent,	// (in)  Event to insert into DB.
    DBPROCESS   *dbProc,        // (in)  Dbprocess to use.
    cHisStatus	&status		// (mod) Program status.
)
{
    int		dbStatus;


    //
    // Insert tuple into label table. 
    //

    checkDb( dbfcmd( dbProc,
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    dhsLabel, \n"
	    "    startDate, \n"
	    "    startNsec, \n"
	    "    endDate, \n"
	    "    endNsec \n"
	    ") \n"
	    "values ( \n"
	    "   '%s', \n"
	    "   dateadd( second, %d, 'jan 1 1970' ), \n"
	    "   %d, \n"
	    "   dateadd( second, %d, 'jan 1 1970' ), \n"
	    "   %d \n"
	    ") \n",
	    cHisDbManager::historyDatabase(),
	    cHisDbManager::historyLabelTbl(),
	    label,
	    startEvent.date(),
	    startEvent.nsec(),
	    endEvent.date(),
	    endEvent.nsec() ), status, return );


    checkDb( dbsqlexec( dbProc ), status, return );
    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
    {
    }
    checkDb( dbStatus, status, return );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putEventTbl
//
// INVOCATION:
// putEventTbl( event, dbproc, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) event	(cHisEvent &)	Event to insert into DB.
// (>) dbProc	(DBPROCESS *)	Dbprocess to use.
// (!) status	(cHisStatus &)	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Insert a tuple into event table
//
// DESCRIPTION:
// Uses dbfcmd() to add insertion of a tuple into the event table to the
// current query associated with the dbProc. Note that the query is
// not executed until later. 
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

void		cHisDbManager::putEventTbl
(
    cHisEvent	&event,		// (in)  Event to insert into DB.
    DBPROCESS 	*dbProc,	// (in)  Dbprocess to use. 
    cHisStatus	&status		// (mod) Program status.
)
{
    char	tempstr[80];	// String of 'db-fixed' data. 


    //
    // Fix string for db - db_strcpy() takes care of single quotes. 
    //

    db_strcpy( tempstr, event.value() );


    //
    // Add insertion of the data to current dbprocess. Do not exec yet.
    //

    checkDb( dbfcmd( dbProc,
	    "insert into \n"
	    "    %s..%s \n"
	    "( \n"
	    "    sir_label, \n"
	    "    sir_value, \n"
	    "    date, \n"
	    "    nsec \n"
	    ") \n"
	    "values ( \n"
	    "   '%s', \n"
	    "   '%s', \n"
	    "   dateadd( second, %d, 'jan 1 1970' ), \n"
	    "   %d \n"
	    ") \n",
	    cHisDbManager::historyDatabase(),
	    cHisDbManager::historyEventTbl(),
	    event.SIR(),
	    tempstr,
	    event.date(),
	    event.nsec() ), status, return );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// ingestFile
//
// INVOCATION:
// ingestFile( label, filePath, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) label	(char *)	DHS label to save to DB.
// (>) filePath	(char *)	Path to file to insert.
// (!) status	(cHisStatus &) 	Program status. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Ingests the file at filePath into the database
//
// DESCRIPTION:
// Get db process. Open the file at filePath. Save first event in the 
// file for later insert to label table. Open a transaction with the 
// database. Loop until done - add up to 1000 events to the sql query 
// in dbProc. Execute the insert query when 1000 events have been added, 
// or at EOF. Save last event, insert first event, last event and label 
// to label table, commit transaction, close file.
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

void		cHisDbManager::ingestFile
(
    char	*label,		// (in)  DHS label to save to DB.
    char	*filePath,	// (in)  Path to file to insert. 
    cHisStatus	&status		// (mod) Program status.
)
{
    int		fileDesc = -1;	// File pointer to use. 
    int		dbStatus;	// Return status from db calls.
    bool	loopDone;	// Have we finished looping? 
    int		x;		// Inner loop counter. 
    int		close_status;	// return status from fclose;
    int		lseek_offset;	// Actual offset returned from lseek(). 
    DBPROCESS   *dbProc;	// DB Process to use. 
    cHisEvent	event;		// Event read from file. 
    cHisEvent	startEvent;	// First event in file. 
    cHisEvent	endEvent;	// Last event in file. 


    checkStat( status, return );
    dbProc = dbConnGet();


    //
    //  Open File.
    //

    fileDesc = open( filePath, O_RDONLY );
    if ( fileDesc == -1 )
    {
	status.E_FOPEN( status, filePath );
	status.sysErrno();
	dbConnRelease();
	return;
    }


    //
    // Save first event for label table. reset file to start. 
    //

    startEvent.readRecord( fileDesc, loopDone );
    lseek_offset = lseek( fileDesc, 0, SEEK_SET );
    if ( lseek_offset == -1 )
    {
	status.E_FSEEK( status, filePath );
	status.sysErrno();
	dbConnRelease();
	return;
    }


    //
    //  Open Transaction.
    //

    checkDbLib( db_simple_query( dbProc,
	    "begin transaction \n" ), status, return );


    //
    //  Loop until done inserting file. 
    //

    loopDone = false;
    while ( !loopDone )
    {
	//
	// If hisLogWrite cmd has been stopped by init or exit command, break.
	//

	if ( cHisLogWrite::stoppingHLW() )
	{
	    break;
	}


	//
	//  Add a thousand events to the sql insert statement. 
	//

	for ( x = 0; x < 1000; x++ )
	{
	    event.readRecord( fileDesc, loopDone );
	    if ( loopDone )
	    {
		break;
	    }

	    check( putEventTbl( event, dbProc, status ), status, return ); 
	}


	//
	// Submit sql insert statement. 
	//

	if ( x != 0 )
	{
	    checkDb( dbsqlexec( dbProc ), status, return );
	    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
	    {
	    }
	    checkDb( dbStatus, status, return );
	}
    }
    

    //
    // If hisLogWrite cmd has been stopped by init or exit command, rollback.
    //
    
    if ( cHisLogWrite::stoppingHLW() )
    {
	//
	//  Roll-back Transaction. 
	//

	checkDbLib( db_simple_query( dbProc,
		"rollback transaction \n" ), status, return );
    }
    else
    {
	//
	// Save last event, insert into other table.
	//

	endEvent = event;
	check( putLabelTbl( label, startEvent, endEvent, dbProc, status ), 
		status, return );


	//
	//  Close Transaction. 
	//

	checkDbLib( db_simple_query( dbProc,
		"commit transaction \n" ), status, return );
    }


    //
    // Close file. 
    //

    close_status = close( fileDesc );
    if ( close_status == -1 )
    {
	status.E_FCLOSE( status, filePath );
	status.sysErrno();
    }


    dbConnRelease();

    
    return;
}
