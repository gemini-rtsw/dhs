static char rcsid[] = "$Id: fh.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// fh/src/fh.C
//
// PURPOSE:
// Handles the parsing of a fits buffer to be written to the database.
//
// METHOD NAME(S)
// cHdrParseList::cHdrParseList - List constructor.
// cHdrParseList::~cHdrParseList - List deconstructor.
// cHdrParser::checkFitsTable - Check that the fits table exists.
// cHdrParser::close - Close db connections.
// cHdrParser::config - Read dhs configuration file parameters.
// cHdrParser::createSql - Create an sql statement.
// cHdrParser::executeSql - Create an sql statement.
// cHdrParser::init - Initialize db connections.
// cHdrParser::parseHeader - parse fits header buffer.
// cHdrParser::remainingCols - Create an sql statement.
// cHdrParser::stripSpaces - Strip spaces off a value.
// cHdrParser::tblParams - Read parameters from config file.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:52  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.9  1998/06/17 20:22:41  dunn
// Made it check that the instr matches or at least a * before checking
// to see if the keyword is mandatory.
//
// Revision 1.8  1998/04/23 17:04:07  nhill
// Updated to use the new version of the cDhsStatus library.
//
// Revision 1.7  1997/12/22 21:36:36  jaeger
// fixed args to ffopen, pass in an empty filename instead of the
// dataset name.  This tells cfitsio that the mem buffer is a FITS file.
//
// Revision 1.6  1997/10/03 20:59:46  dunn
// Fixes to use new stl library and change to cfitsio lib.
//
// Revision 1.5  1997/09/05 18:41:00  dunn
// Fixed freeing
//
// Revision 1.4  1997/08/29 22:01:13  dunn
// Freed a list and close the cfitsio buffer.
//
// Revision 1.3  1997/08/19 22:41:55  dunn
// Fixed some useless lines.
//
// Revision 1.2  1997/08/19 20:31:22  dunn
// New cfitsio required change to fits_set_membuff.
//
// Revision 1.1  1997/07/29 20:01:44  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
//  Includes
//

#include <iostream>

extern "C"
{
#include <sybfront.h>
#include <sybdb.h>
 
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
#include "gen_eptr.h"
}

#include "genMutex.H"
#include "dhsGenStatus.H"
#include "fh.H"


//
//  Local defines.
//

#define HDR_KEYWORD_DB	"fitsTable"

cMutex	cHdrParser::cdmMutexHdr;
DBPROCESS
	*cHdrParser::hdrDbProc = NULL;
char	*cHdrParser::hdrDatabaseName = NULL;
char	*cHdrParser::hdrFitsTblName = NULL;
bool	cHdrParser::hdrReadConfig = FALSE;
bool	cHdrParser::hdrDDInit = FALSE;


msgText( cHdrParser::hStatus,  S_WARNING_COL, P_NULL,
    "Warning, header information for the database missing : %s." );
msgText( cHdrParser::hStatus,  S_WARNING_MAN, P_NULL,
    "Warning, gemini mandatory keyword missing : %s." );
msgText( cHdrParser::hStatus,  E_CFITS, P_NULL,
    "Error in the cfitsio function, no: %d, string: `%s'." );
msgText( cHdrParser::hStatus,  E_COL_TYPE, P_NULL,
    "Error in data dicitonary, multiple column names %s with different data types." );
msgText( cHdrParser::hStatus, E_DATA_TYPE, P_NULL,
	    "Error, invalid data type for keyword %s, type %d." );
msgText( cHdrParser::hStatus, E_DB, P_NULL,
	    "Error in the db library: `%s'." );
msgText( cHdrParser::hStatus,  E_DB2, P_NULL,
    "Error in the db library: `%s' \nfor function `%s'." );
msgText( cHdrParser::hStatus, E_DD, P_NULL,
	    "Error in the dd library\n%s" );
msgText( cHdrParser::hStatus, E_FITS_TABLE, P_NULL,
	    "Error, fits table and data dictionary column names/types do not correspond" );
msgText( cHdrParser::hStatus, E_INSERT, P_NULL,
	    "Error inserting into fits table : \n%s" );
msgText( cHdrParser::hStatus,  E_READ_CONFIG, P_NULL,
    "Error reading the configuration file" );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParseList::cHdrParseList
//
// INVOCATION:
// Constructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) rowInfo (DD_ROW *) Row information.
// (>) keyword (char *) Keyword.
// (>) value (char *) Value for that keyword.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor.  Save values passed in to the class.
//
// DESCRIPTION:
// Constructor.  Save values passed in to the class.
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

		cHdrParseList::cHdrParseList
(
    DD_ROW	*rowInfo,		// (in)  Row information.
    char	*keyword,		// (in)  Keyword.
    char	*value			// (in)  Value.
)
{
    hdrRow = rowInfo;
    hdrValue = strsav( (char *) value );
    (void) strcpy( hdrKeyword, keyword );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParseList::~cHdrParseList
//
// INVOCATION:
// Constructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor.
//
// DESCRIPTION:
// Deconstructor.
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

		cHdrParseList::~cHdrParseList
(
    VOID
)
{
    free( hdrValue );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::checkFitsTable
//
// INVOCATION:
// cHdrParser::checkFitsTable( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To check that all columns listed in the dd, exist in the fits table.
//
// DESCRIPTION:
// To check that all columns listed in the dd, exist in the fits table.
// It goes through the data dictionary, creates a list of all unique columns
// and passes that to have an sql statement created.  This checks that 
// the columns exist and that they are potentially of the correct data type.
//
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Database must be open and a dbproc created.  Data dictionary must have
// done a uniqueness check that instrument and column are unique.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHdrParser::checkFitsTable
(
    hStatus	&status			// (mod) Function return status.
)
{
    DD_STATUS	ddStatus;		// Data dict. status.
    cHdrParseList::iterator
    		i;			// Interator into list ptr.
    int 	j;			// Increment into data dict.
    cHdrParseList::tParseList
		listPtr;		// List of columns expected in fits tbl.
    cHdrParseList
    		*pColList;		// Temp. ptr to column.
    DD_ROW	*row;			// Row structure in dd.
    char	*statement;		// Sql statement.
    char	value[20];		// Temporary value.


    checkStat( status, return );


    //
    //  Now cycle through the data dictionary, adding to a list all
    //  things that have a column name.  That is a unique column name.
    //

    j = 0;
    ddStatus = DD_S_SUCCESS;
    while( 1 )
    {
	row = ddGetNext( j, &ddStatus );
	if ( row == NULL )
	{
	    break;
	}
	j++;

	if ( !streq( row->ddrColName, "NULL" ) )
	{
	    //
	    //  First see if we already have this column, and if we
	    //  do, check that the data type matches. Otherwise, add it.
	    //

	    i = listPtr.find( std::string( row->ddrColName ) );
	    if ( i != listPtr.end() )
	    {
		if ( (*i).second->hdrRow->ddrType !=  row->ddrType )
		{
		    status.E_COL_TYPE( status, row->ddrColName );
		    break;
		}
	    }
	    else
	    {
		//
		//  This is a new column name, create the value to
		//  insert into the table.
		//
		
		switch( row->ddrType )
		{
		    case DHS_DT_CHAR:
		    case DHS_DT_STRING:
			(void) strcpy( value, "X");
			break;
		    case DHS_DT_BOOLEAN:
			(void) strcpy( value, "T" );
			break;
		    case DHS_DT_INT8 :
		    case DHS_DT_UINT8:
		    case DHS_DT_INT16:
		    case DHS_DT_UINT16:
		    case DHS_DT_INT32:
		    case DHS_DT_UINT32:
		    case DHS_DT_FLOAT:
		    case DHS_DT_DOUBLE:
			(void) strcpy( value, "99" );
			break;
		    default:
			(void) strcpy( value, "X");
		}


		//
		//  Add this to the column list, using the col name as
		//  the key for this list.
		//
		
		pColList = new cHdrParseList( row, row->ddrKeyword, 
			value );
		listPtr[ std::string( row->ddrColName ) ] = pColList;
	    }
	}

    }


    //
    //  Create an sql statement with this list.
    //

    createSql( status, &statement, "StartTest", listPtr );


    //
    //  Now execute the insert sql statement.
    //

    executeSql( status, statement );
    if ( statement != NULL )
    {
	free(statement);
    }


    //
    //  Free the list.
    //

    cHdrParseList	*tmp;
    cHdrParseList::iterator k;
    for ( k = listPtr.begin(); k != listPtr.end(); k=listPtr.begin() )
    {
	tmp =  (*k).second;
	listPtr.erase( k );
	delete tmp;
    }


    //
    //  If successful, delete the dummy line we just put in the database.
    //

    if ( status.ok() )
    {
	statement = (char *) gen_alloc ( 256 );
	(void) sprintf( statement, 
	"delete from %s..%s where datasetName = 'StartTest'", 
	hdrDatabaseName,
	hdrFitsTblName );

	executeSql( status, statement );
	if ( statement != NULL )
	{
	    free(statement);
	}
    }	    
    else
    {
	status.E_FITS_TABLE( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::close
//
// INVOCATION:
// cHdrParser::close()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) - Returned status.
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To close the db connections and free anything.
//
// DESCRIPTION:
// To close the db connections and free anything.
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

void cHdrParser::close
(
    hStatus		&status		// (in) Return status. 
)
{
    DD_STATUS		ddStatus;	// Data dictionary status.


    //
    //  Reset the configuration read flag.  Free the db name.
    //

    hdrReadConfig = FALSE;
    if ( hdrDatabaseName != NULL )
    {
	free( hdrDatabaseName );
	hdrDatabaseName = NULL;
    }


    //
    //  Close the database connection.
    //

    if ( cHdrParser::hdrDbProc != NULL )
    {
	checkDbLibHdr( db_close( cHdrParser::hdrDbProc ), status, return );
	cHdrParser::hdrDbProc = NULL;
    }


    //
    //  Close the data dictionary.
    //

    if ( hdrDDInit )
    {
	hdrDDInit = FALSE;
	ddStatus = DD_S_SUCCESS;
	checkDd( ddExit( &ddStatus ), ddStatus, status, return );
    } 

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::config
//
// INVOCATION:
// cHdrParser::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the db library configuration parameters
//
// DESCRIPTION:
// Read in the fits table name.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHdrParser::config
(
    hStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    checkConfig( config_get( HDR_KEYWORD_DB, tblParams ), status, return );
    hdrReadConfig = TRUE;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::createSql
//
// INVOCATION:
// cHdrParser::createSql
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) Return function status.
// (<) sqlState (char **)  Sql statement created.
// (>) datasetName (char *) Dataset name to create.
// (>) listPtr (tParseList ) List of columns to create sql statement with.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add the column and value to the sql statement.
//
// DESCRIPTION:
// Cycle through the listPtr, and add to an sql statement each column
// and value.
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

void	cHdrParser::createSql
(
    hStatus	&status, 		// (mod) Function return status.
    char	**sqlState,		// (out) Sql statement.
    char	*datasetName,		// (in)  Dataset name.
    cHdrParseList::tParseList	
		listPtr			// (in)  List of columns.
)
{
    cHdrParseList::iterator
    		i;
    char	values[256];
    char	columns[256];
    cHdrParseList
    		*pCol;


    checkStat( status, return );


    //
    //  Return null if the col list is empty.
    //

    if ( ( i = listPtr.begin() ) == listPtr.end() )
    {
	*sqlState = NULL;
	return;
    }


    //
    //  Start the sql statement.
    //

    (void) sprintf( columns, 
	"insert into \n"
	"       %s..%s \n"
	"( datasetName \n",
	hdrDatabaseName,
	hdrFitsTblName );

    (void) sprintf(values, " )\nvalues (\n '%s'", datasetName );


    //
    //  Cycle through the col list, and add to the sql statement.
    //

    for ( i = listPtr.begin(); i != listPtr.end(); i++ )
    {
	pCol = (*i).second;


	//
	//  If the column name is not null and the value is not null, then
	//  add to the insert statement.
	//

	if ( !streq( pCol->hdrRow->ddrColName, "NULL" )  &&
	    !streq( pCol->hdrValue, pCol->hdrRow->ddrNullVal ) )
	{
	    (void) strcat( columns, ", ");
	    (void) strcat( columns, pCol->hdrRow->ddrColName );
	    (void) strcat( values, ", ");
	    switch( pCol->hdrRow->ddrType )
	    {
	    case DHS_DT_CHAR:
	    case DHS_DT_STRING:
		(void) strcat( values, "'");
		(void) strcat( values, pCol->hdrValue );
		(void) strcat( values, "'");
		break;
	    case DHS_DT_BOOLEAN:
		if ( streq( "T", pCol->hdrValue ) )
		{
		    (void) strcat( values, "'TRUE'");
		}
		else
		{
		    (void) strcat( values, "'FALSE'");
		}
		break;
	    case DHS_DT_INT8 :
	    case DHS_DT_UINT8:
	    case DHS_DT_INT16:
	    case DHS_DT_UINT16:
	    case DHS_DT_INT32:
	    case DHS_DT_UINT32:
	    case DHS_DT_FLOAT:
	    case DHS_DT_DOUBLE:
		(void) strcat( values, pCol->hdrValue );
		break;
	    default:
		// Error, invalid data type
		status.E_DATA_TYPE( status, (char *) pCol->hdrRow->ddrKeyword, 
			pCol->hdrRow->ddrType );
		return;
	    }

	}
    }


   //
   //  Complete the composed sql statement.
   // 

    *sqlState = (char *) gen_alloc( strlen( columns) + strlen( values ) + 2 );
   (void) sprintf( *sqlState, "%s%s)", columns, values );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::executeSql
//
// INVOCATION:
// cHdrParser::executeSql
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) Return function status.
// (<) sqlState (char *)  Sql statement to execute.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Execute the sql statement passed in.
//
// DESCRIPTION:
// Execute the sql statement passed in.
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

void	cHdrParser::executeSql
(
    hStatus	&status,		// (mod) Function return status.
    char	*sqlState		// (in)  Sql statement.
)
{
    int		dbstatus;		// Database status.


    //
    //  Return if sqlStatemnt is null or status is bad.
    //

    checkStat( status, return );
    if ( sqlState == NULL )
    {
	return;
    }

    checkDbx( dbfcmd( cHdrParser::hdrDbProc,
	    "%s",
	    sqlState ), status, return );


    checkDbx( dbsqlexec( cHdrParser::hdrDbProc ), status, return );
    while ( ( dbstatus = dbresults( cHdrParser::hdrDbProc ) ) == SUCCEED )
    {}
    if ( dbstatus == FAIL )
    {
	//
	//  Insert failed.
	//

	status.E_INSERT( status, db_msg );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::init
//
// INVOCATION:
// cHdrParser::init()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) - Returned status.
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To initialize the fh library
//
// DESCRIPTION:
// To initialize the database connection, init the dd library 
// and set variables.  It then checks that the columns in the data 
// dictionary exist in the database.
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

void cHdrParser::init
(
    hStatus		&status,	// (in) Return status. 
    char		*databaseName,	// (in) Database name.
    char		*serverName	// (in) Server name.
)
{
    DD_STATUS		ddStatus;	// Data dictionary status.


    //
    //  Check that the configuration file has already been read.
    //

    checkReadConfig( status, return );


    //
    //  Initialize the data dictionary.
    //
    ddStatus = DD_S_SUCCESS;
    checkDd( ddInit( &ddStatus ), ddStatus, status, return );
    hdrDDInit = TRUE;
   

    //
    //  Save the database name.
    //

    if ( hdrDatabaseName != NULL )
    {
	free( hdrDatabaseName );
	hdrDatabaseName = NULL;
    }
    checkNull( hdrDatabaseName = (char *) strsav( databaseName ), status,
	    return );


    //
    //  Initialize all database connections.
    //

    checkDbLibHdr( db_open(  serverName, NULL, NULL, databaseName, FALSE, 
	     & ( cHdrParser::hdrDbProc ) ), status, return );



    //
    //  Get the column names and database and make sure they
    //  are in the database.
    //

    checkFitsTable( status );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::parseHeader
//
// INVOCATION:
// cHdrParser::parseHeader( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) Function return status.
// (>) buffPtr (void *) Data buffer.
// (>) buffSize (long) Data buffer size.
// (>) datasetName (char *) Dataset name to create.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To parse the fits file and save stuff indicated in the data dictionary
// to the database.
//
// DESCRIPTION:
// To parse the fits file and save stuff indicated in the data dictionary
// to the database.  It will also check that the gemini mandatory 
// keywords are in the primary header.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Database must be open and a dbproc created.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHdrParser::parseHeader
(
    hStatus	&status,		// (mod) Function return status.
    void	*buffPtr,		// (in)  Data buffer.
    long	buffSize,		// (in)  Size of the buffer in bytes.
    char	*datasetName		// (in)  Dataset name.
)
{
    DD_STATUS	ddStatus;		// Data dictionary status.
    char        comment[FLEN_COMMENT]; 	// Something.
    int		fitsStatus;		// Cfitsio status.
    fitsfile	*fp;			// Fits file pointer.
    cHdrParseList::tParseList
		hdrListPtr;		// List of cols and values.
    char        instrument[FLEN_VALUE]; 
    					// Instrument.
    int		keypos;			// Position in key. 
    char	keyword[FLEN_KEYWORD];		// Keyword name.
    int		nkeys;			// # of Fits keywords in main header.
    DD_ROW	*row;			// Row structure in dd.
    char	*statement;		// Sql statement.



    //
    //  Check status, set status, and set lock.
    //

    checkStat( status, return );
    fitsStatus = 0;
    cdmMutexHdr.lock();


    //
    //  Do the memory buffer thing, so that now we can pretend its a file.  
    //

    /* XXX allan: 11/19/02: outdated code for cfitsio2430
    fits_set_mem_buff( &fp, &buffPtr,(unsigned int*) &buffSize, 
	    2880, realloc, &fitsStatus );
    if ( fitsStatus != USE_MEM_BUFF )
    {
	status.E_CFITS( status, fitsStatus, "fits_set_mem_buff" );
	cdmMutexHdr.unlock();
	return;
    }


    // The file name must be an empty string, if it isn't cfitsio 
    // assumes that file exists on disk and tries to read but this 
    // file is in memory.

    ffopen( &fp, "", READONLY, &fitsStatus );
    if ( fitsStatus != 0 )
    {
	status.E_CFITS( status, fitsStatus,  "ffopen" );
	cdmMutexHdr.unlock();
	return;
    }

    XXX */

    // XXX allan: updated version for for cfitsio2430
    ffomem( &fp, "", READONLY, &buffPtr,(size_t*) &buffSize,
	    2880, realloc, &fitsStatus);
    if ( fitsStatus != 0 )
    {
	status.E_CFITS( status, fitsStatus,  "ffomem" );
	cdmMutexHdr.unlock();
	return;
    }


    //
    //  Get the instrument from the buffer.
    //

    if ( ffgkys( fp, "INSTRUME", instrument, comment, &fitsStatus ) > 0  ||
	    fitsStatus != 0 )
    {
	//
	//  Set the instrument to wildcard if not found in the fits file.
	//

        strcpy( instrument, "*" );
        fitsStatus = 0;
    }


    // 
    //  Get number of keywords in the main header.
    //
   
    checkFits ( ffghps( fp, &nkeys, &keypos, &fitsStatus ), fitsStatus,
            status, VOID );


    //
    //  For each keyword in the fits buffer search for the keyword in the dd
    //
    
    cHdrParseList	*pColList;
    char		value[FLEN_VALUE];

    for ( int j = 1; status.ok() && j <= nkeys; j++ )  
    {
	//
	//  Get the name, value, and comment from the CHU.
	//

        checkFits( ffgkyn( fp, j, keyword, value, comment, &fitsStatus ),
		fitsStatus, status, break );
	
 
	//
	//  Get the keyword from the data dictionary.
	//
	
	ddStatus = DD_S_SUCCESS;
	ddAttribGet( instrument, keyword, &row, &ddStatus ) ;
	if ( ddStatus == DD_E_NOT_FOUND )
	{
	    //
	    // Keyword not found in the dd, so ignore it.
	    //

	    continue;
	}


	//
	//  Check if this is a mandatory or goes into table.
	//

	if ( row->ddrGemMand || !streq( row->ddrColName, "NULL" ) )
	{

	    //
	    //   Strip spaces off value if type is a string.
	    //

	    if ( row->ddrType == DHS_DT_STRING )
	    {
		stripSpaces( value );
	    }


	    //
	    //  Check if the value in the fits file is null.
	    //

	    if ( streq( value, row->ddrNullVal ) && row->ddrGemMand )
	    {
		//
		//  Give a warning, a mandatory gemini keyword is null.
		//

		status.S_WARNING_MAN( status, row->ddrKeyword );
	    }


	    //
	    //  Add to the list of keywords found in the fits file.
	    //
	    
	    pColList = new cHdrParseList( row, keyword, value );
	    hdrListPtr[ std::string( keyword ) ] = pColList;
	}
    }


    //
    //  Now cycle through the data dictionary, checking for mandatory keywords
    //  and ones that are suppose to go in the database.
    //

    remainingCols( status, instrument, hdrListPtr );


    //
    // Write the sql statement.
    //

    createSql( status, &statement, datasetName, hdrListPtr );


    //
    //  Now execute the sql statement.
    //

    executeSql( status, statement );
    if ( statement != NULL )
    {
	free(statement);
    }
 

    //
    //  Free the list.
    //

    cHdrParseList	*tmp;
    cHdrParseList::iterator i;
    for ( i = hdrListPtr.begin(); 
	    i != hdrListPtr.end(); i=hdrListPtr.begin() )
    {
	tmp =  (*i).second;
	hdrListPtr.erase( i );
	delete tmp;
    }


    //
    //  Close the fits buffer.
    //

    fitsStatus = 0;
    ffclos( fp, &fitsStatus );

    cdmMutexHdr.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::remainingCols
//
// INVOCATION:
// cHdrParser::()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (hStatus &) Return function status.
// (>) instrument (char *) Instrument.
// (>) listPtr (tParseList ) List of keywords in the fits file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To check that all mandatory keywords are in the fits file.
//
// DESCRIPTION:
// To check that all mandatory keywords are in the fits file.  This must
// cycle through the data dictionary and compare again the list of keywords
// found in the file.
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

void	cHdrParser::remainingCols
(
    hStatus	&status,		// (mod) Function return status.
    char	*instrument,		// (in)  Instrument.
    cHdrParseList::tParseList	
		listPtr			// (in)  List of columns.
)
{
    DD_ROW	*row;			// Row structure in dd.
    cHdrParseList::iterator
    		i;
//    cHdrParseList
//    		*pColList;
    int 	j;
    DD_STATUS	ddStatus;


    checkStat( status, return );


    //
    //  Now cycle through the data dictionary, checking for mandatory keywords.
    //

    j = 0;
    ddStatus = DD_S_SUCCESS;
    while( 1 )
    {
	row = ddGetNext( j, &ddStatus );
	if ( row == NULL )
	{
	    break;
	}
	j++;

	i = listPtr.find( std::string( row->ddrKeyword ) );


	//
	//  Only do a comparison if the instrument is * or matches
	//  the one in the passed in buffer.
	//

	if ( row->ddrInstr[0] == '*' || strcmp( instrument, row->ddrInstr ) == 0 )
	{

	    if ( row->ddrGemMand && i==listPtr.end() )
	    {
		//
		// Keyword is gemini mandatory and not in fits buffer, warning.
		//

		status.S_WARNING_MAN( status, row->ddrKeyword );
	    }
	    if ( !streq( row->ddrColName, "NULL" ) && i==listPtr.end() )
	    {
		//
		//  Not in fits buffer, but needed in db, give a warning.
		//

		status.S_WARNING_COL( status, row->ddrKeyword );
		//Do not add to the db. Decided against that.
		// pColList = new cHdrParseList( row, row->ddrKeyword, 
		//	    row->ddrNullVal);
		//listPtr[ std::string( row->ddrKeyword ) ] = pColList;
	    }
	}

    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::stripSpaces
//
// INVOCATION:
// cHdrParser::stripSpaces()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) value (char *) Value to strip.
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Strip spaces off the value.
//
// DESCRIPTION:
// Strip spaces off the value.  It expects the last item to be a hyphen,
// and then strips off that hyphen and any spaces before it.  It expects the
// thing passed in to be a null terminated string.  It will then strip off the
// first hyphen.  It re-writes to the current string.
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

void	cHdrParser::stripSpaces
(
    char	*value			// (mod)  Value to strip.
)
{
    int		x;

    //
    //  Start before the last hypen.
    //

    x = strlen( value ) - 2;

    while ( x > 0 )
    {
	if( value[x] != ' ' )
	{
	    value[x+1] = '\0';
	    break;
	}
	x--;
    }


    //
    // Get rid of the first quote.
    //

    if ( value[0] == '\'' )
    {
	int j;
	for ( x=1, j=0; j < (int) strlen(value); j++, x++ )
	{
	    value[j] = value[x];
	}
    }
	    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHdrParser::tblParams
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the database line from a configuration file and store it.
//
// DESCRIPTION:
// Save the fits table name.
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

boolean		cHdrParser::tblParams
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    hStatus	status;		// Status code.

    if ( numTokens == 2 )
    {
	if ( hdrFitsTblName != NULL )
	{
	    free( hdrFitsTblName );
	}
	checkNull( hdrFitsTblName = (char *) strsav( tokens[1] ), status,
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }

    return( ret );
}
