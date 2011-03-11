static char rcsid[] = "$Id: dbm.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/dbm.C
//
// PURPOSE:
// Handles all database connections
//
// METHOD NAME(S)
// cDtsDbManager::close - Close the database connections.
// cDtsDbManager::config - Read dhs configuration file parameters.
// cDtsDbManager::dbParams - Read parameters from config file.
// cDtsDbManager::formatDetermine - Determine format of data.
// cDtsDbManager::formatDetermine - Determine format of the name.
// cDtsDbManager::getDbConn - Lock and return a connection.
// cDtsDbManager::getFileList - Return a list of files.
// cDtsDbManager::init - Initialize db connections.
// cDtsDbManager::makeRetrievable - Write it to the ad tables.
// cDtsDbManager::makeUnretrievable - Write it to the ad tables.
// cDtsDbManager::mountPoints - Determine mount points of the stages.
// cDtsDbManager::mountPoints - Determine mount points of the stages.
// cDtsDbManager::nameInfo - Determine root name information.
// cDtsDbManager::releaseDbConn - Release lock on connection
// cDtsDbManager::testAll - Check all database connections.
// cDtsDbManager::uniqueConnect - Check uniqueness in the database.
// cDtsDatasetTable::cDtsDatasetTable
// cDtsDatasetTable::cDtsDatasetTable
// cDtsDatasetTable::~cDtsDatasetTable
// cDtsDatasetTable::datasetNameMax - Get the maximum dataset name number.
// cDtsDatasetTable::dsGet - Get a datasetname to the ds table.
// cDtsDatasetTable::dsPut - Put a datasetname to the ds table.
// cDtsDatasetTable::fileDel - Delete a datasetname from the database.
// cDtsDatasetTable::fileGet - Get a datasetname from the database.
// cDtsDatasetTable::fileListGet - Get list of datasetnames from the db.
// cDtsDatasetTable::filePut - Put a datasetname to the database.
// cDtsDatasetTable::fileUpdate - Update a datasetname to the database.
// cDtsDatasetTable::getAdFileInfo - Get information about a file from ad.
// cDtsUniqueTable::cDtsUniqueTable - Constructor.
// cDtsUniqueTable::~cDtsUniqueTable - Deconstructor.
// cDtsUniqueTable::uniqueNameGet - Get a unique name from the database.
// cDtsUniqueTable::uniqueNameMax - Get the maximum unique name number.
// cDtsUniqueTable::uniqueNamePut - Put a new unique name in the database.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:26:54  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.17  2000/08/15 17:41:18  dunn
// Got rid of un-used variables.
//
// Revision 1.16  2000/07/27 20:09:41  dunn
// Added standalone features.
//
// Revision 1.15  1999/06/16 22:23:15  nhill
// Added function cDtsDatasetTable::getAdFileInfo.
//
// Revision 1.14  1998/06/24 15:55:18  dunn
// Changed ad_default_archive to ad_archive_info.
//
// Revision 1.13  1998/06/22 20:46:10  dunn
// Fixed isql message to given exeact amount of spaces.
//
// Revision 1.12  1998/05/25 20:03:31  dunn
// Changed macro in db_init call .
//
// Revision 1.11  1998/05/13 19:44:11  dunn
// Fixed isql message to be sent to console.
//
// Revision 1.10  1998/05/07 23:33:34  dunn
// Got rid of health check when getting files to be sent to ss.
//
// Revision 1.9  1998/05/04 22:59:20  dunn
// Status class changes and default archive checks.
//
// Revision 1.8  1997/09/29 21:18:14  dunn
// Changed temp cleanup to attempt to remove from temp, regardless
// of whether it is in ad or not.
//
// Revision 1.7  1997/09/17 22:12:42  dunn
// Code cleanup.
//
// Revision 1.6  1997/09/05 19:52:19  dunn
// Added filePutDs.
//
// Revision 1.5  1997/08/29 23:00:50  dunn
// Fixed leaks and initialized dstFileSize properly in both constructors.
//
// Revision 1.4  1997/08/19 22:29:36  dunn
// Changed logging to db to be the identity name, fixed bugs.
//
// Revision 1.3  1997/07/29 22:01:00  dunn
// Various fixes.
//
// Revision 1.2  1997/07/23 19:30:16  dunn
// Fixed ad_dbInfo_get call to use MD, rather then CD, and set*Path to check status.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
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
#include "genMutex.H"

extern "C"
{
#if defined(SYBASE_DHS)
#include <sybfront.h>
#include <sybdb.h>
#endif

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"

#if defined(SYBASE_DHS)
#include "db.h"
#endif
#include "ad.h"
}

#include "globals.H"
#include "dbm.H"
#include "list.H"
#include "store.H"
#include "dtsDhs.H"

//
//  Local defines.
//

#define DTS_KEYWORD_DB	"database"
cMutex	cDtsDbManager::cdmMutexAd;
cMutex	cDtsDbManager::cdmMutexDs;
cMutex	cDtsDbManager::cdmMutexPr;
cMutex	cDtsDbManager::cdmMutexUn;

struct dbprocess	*cDtsDbManager::dbprocAd = NULL;
struct dbprocess	*cDtsDbManager::dbprocDs = NULL;
struct dbprocess	*cDtsDbManager::dbprocPr = NULL;
struct dbprocess	*cDtsDbManager::dbprocUn = NULL;

char	*cDtsDbManager::cdmAdDatabase = NULL;
char	*cDtsDbManager::cdmAdServer = NULL;
char	*cDtsDbManager::cdmDatabase = NULL;
char	*cDtsDbManager::cdmDsTblName = NULL;
char	*cDtsDbManager::cdmFileTbl = NULL;
char	*cDtsDbManager::cdmPasswd = NULL;
char	*cDtsDbManager::cdmProcTblName = NULL;
char	*cDtsDbManager::cdmServer = NULL;
char	*cDtsDbManager::cdmUniqueTblName = NULL;
char	*cDtsDbManager::cdmUserName = NULL;
char	*cDtsDbManager::cdmVolTbl = NULL;


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::close
//
// INVOCATION:
// cDtsDbManager::close()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) - Returned status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To close the db connections
//
// DESCRIPTION:
// To close the db connections.
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

void cDtsDbManager::close
(
    VOID
)
{
#if defined(SYBASE_DHS)
    bool	somethingOpen = FALSE;		// At least 1 db was open.


    //
    //  Close the db library.
    //

    if ( cDtsDbManager::dbprocAd != NULL )
    {
	db_close( cDtsDbManager::dbprocAd );
	cDtsDbManager::dbprocAd = NULL;
	somethingOpen = TRUE;
    }
    if ( cDtsDbManager::dbprocDs != NULL )
    {
	db_close( cDtsDbManager::dbprocDs );
	cDtsDbManager::dbprocDs = NULL;
	somethingOpen = TRUE;
    }
    if ( cDtsDbManager::dbprocPr != NULL )
    {
	db_close( cDtsDbManager::dbprocPr );
	cDtsDbManager::dbprocPr = NULL;
	somethingOpen = TRUE;
    }
    if ( cDtsDbManager::dbprocUn != NULL )
    {
	db_close( cDtsDbManager::dbprocUn );
	cDtsDbManager::dbprocUn = NULL;
	somethingOpen = TRUE;
    }

    if ( somethingOpen )
    {
	db_exit();
    }
#else
   // no SYBASE support: irrelevant
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::config
//
// INVOCATION:
// cDtsDbManager::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the db library configuration parameters
//
// DESCRIPTION:
// Read in the database name and server.
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

void		cDtsDbManager::config
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    checkConfig( config_get( DTS_KEYWORD_DB, dbParams ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::dbParams
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
// Read the database line from a configuration file and store it
//
// DESCRIPTION:
// Save the sybase server name, database name, dataset, process and unique
// table names.
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

boolean		cDtsDbManager::dbParams
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cDtsStatus	status;		// Status code.

    if ( numTokens == 6 )
    {
	if ( cdmServer != NULL )
	{
	    free( cdmServer );
	}
	checkNull( cdmServer = (char *) strsav( tokens[1] ), status, 
		return( FALSE ) );
	if ( cdmDatabase != NULL )
	{
	    free( cdmDatabase );
	}
	checkNull( cdmDatabase = (char *) strsav( tokens[2] ), status,
		return( FALSE ) );
	if ( cdmUniqueTblName != NULL )
	{
	    free( cdmUniqueTblName );
	}
	checkNull( cdmUniqueTblName = (char *) strsav( tokens[3] ), status,
		return( FALSE ) );
	if ( cdmDsTblName != NULL )
	{
	    free( cdmDsTblName );
	}
	checkNull( cdmDsTblName = (char *) strsav( tokens[4] ), status,
		return( FALSE ) );
	if ( cdmProcTblName != NULL )
	{
	    free( cdmProcTblName );
	}
	checkNull( cdmProcTblName = (char *) strsav( tokens[5] ), status,
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

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::formatDetermine
//
// INVOCATION:
// cDtsbManager::formatDetermine()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type (DHS_BD_PUT_TYPE ) put type.
// 
// FUNCTION VALUE:
// Single format charater to be used in the database.
//
// PURPOSE:
// Returns a single character based on the dhs bulk data type passed in
//
// DESCRIPTION:
// Returns a single character based on the bulk data type passed in.
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

char		cDtsDbManager::formatDetermine
(
    DHS_BD_PUT_TYPE
    		type			// (in)  Type of data sent.
)
{
    if ( type == DHS_BD_PT_RAW || type == DHS_BD_PT_RAW_UNIQUE )
    {
	return( DTS_FM_RAW );
	
    }
    else if ( type == DHS_BD_PT_FITS || type == DHS_BD_PT_FITS_UNIQUE )
    {
	return( DTS_FM_FITS );
    }
    else if ( type == DHS_BD_PT_DS  )
    {
	return( DTS_FM_DS );
    }
    else
    {
	//
	// Type is  DHS_BD_PT_DS_QL,or  DHS_BD_PT_UNKNOWN.
	//

	return( DTS_FM_UNKNOWN );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::formatDetermine
//
// INVOCATION:
// cDtsbManager::formatDetermine()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name ( char *) a name to determine type.
// 
// FUNCTION VALUE:
// Returns format type
//
// PURPOSE:
// Search for fits extension on end of name
//
// DESCRIPTION:
// Search for fits extension on end of name, and if found
// then type is fits, otherwise, raw.
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

char cDtsDbManager::formatDetermine
(
    char	*name		// (in)  Name of the dataset.
)
{
    char	*tail;

    //
    //  Find the last ., and if found, check to see if you are left 
    //  with a "fit" or FIT or FITS or "fits".
    //

    tail = strrchr( name, '.' );
    if ( tail == NULL )
    {
	return( DTS_FM_RAW ); 
    }
    else
    {
	++tail;
	if ( strcasecmp( tail, "fit") == 0 ||
		strcasecmp( tail, "fits" ) == 0 )
	{
	    //
	    //  This is a fits file.
	    //

	   return( DTS_FM_FITS );
	}
	else
	{
	    //
	    // Not a fits file.
	    //
	    
	   return( DTS_FM_RAW );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::getDbConn
//
// INVOCATION:
// cDtsDbManager::getDbConn()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type (tProcessCon) Type of connection to get.
//
// FUNCTION VALUE:
// Pass out the database connection.
//
// PURPOSE:
// To lock and return a database connection
//
// DESCRIPTION:
// To lock and return a database connection.
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

struct dbprocess * cDtsDbManager::getDbConn
(
    tProcessCon		type	// (in)  Type of connection to get.
)
{


   cDtsStatus	status;

#if defined(SYBASE_DHS)

    status.S_DB_CONN( status, type );

    if ( type == DB_AD_LIB )
    {
	//
	//  Ad library connection.
	//

	cdmMutexAd.lock();
	return ( dbprocAd );
    }
    else if ( type == DB_DS_LIB )
    {
	//
	//  Datatset table  library connection.
	//

	cdmMutexDs.lock();
	return ( dbprocDs );
    }
    else if ( type == DB_PR_LIB )
    {
	//
	//  Process table library connection.
	//

	cdmMutexPr.lock();
	return ( dbprocPr );
    }
    else if ( type == DB_UN_LIB )
    {
	//
	//  UniqueName table library connection.
	//

	cdmMutexUn.lock();
	return ( dbprocUn );
    }
#else
   status.E_DB(status, "cDtsDbManager::getDbConn - no SYBASE support: try -noDb?" );
   return ( NULL );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::getFileList
//
// INVOCATION:
// cDtsDbManager::getFileList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (<) returnList (cDhsAvList) Returned av list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To return a line from the ad tables, given the volume
//
// DESCRIPTION:
// To return a line from the ad tables, given the volume.
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

void	cDtsDbManager::getFileList
(
    cDtsStatus		&status,	// (in)  Return status. 
    cDhsAvList		*returnList 	// (out) Return av list.
)
{

#if defined(SYBASE_DHS)
    char	isqlOutput[512];// Isql sent.
    AD_ARCHIVE	*archive;

    checkStat( status, return );


    //
    //  Compose the isql statement.
    //


    archive = ad_archive_info( NULL );

    sprintf(isqlOutput, "use %s\ngo \nselect substring( mdf.file_id, 1, 45), substring( mdf.fname, 1, 45 ), substring(mdf.format, 1, 1), substring( mdf.diskname, 1, 20 ), mdf.uncomp_fsize from %s..%s mdf, %s..%s mds where  mdf.diskname = mds.diskname and archive = '%s' and mdf.status = '%c'  \ngo\n", cdmDatabase, 
    cdmDatabase, cdmFileTbl, cdmDatabase, cdmVolTbl, 
    archive->ada_name, AD_FILE_CURRENT );


    //
    //  Add the server name, database name, username, passwd, and
    //  isql statement to the av list.
    //

    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );

    returnList->add(DTS_AD_SERV, DHS_DT_STRING, cdmAdServer , dhsStatus );
    returnList->add(DTS_AD_DB, DHS_DT_STRING, cdmAdDatabase , dhsStatus );
    returnList->add(DTS_AD_PSSWD, DHS_DT_STRING, cdmPasswd , dhsStatus );
    returnList->add(DTS_AD_USER, DHS_DT_STRING, cdmUserName , dhsStatus );
    returnList->add(DTS_AD_ISQL, DHS_DT_STRING, isqlOutput, dhsStatus );

#else
   status.E_DB(status, "cDtsDbManager::getFileList - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::init
//
// INVOCATION:
// cDtsDbManager::init()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) - Returned status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To initialize the db and ad libraries and open database connections
//
// DESCRIPTION:
// To initialize the db and ad libraries and open database connections.
// Just return if running standalone.
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

void cDtsDbManager::init
(
    cDtsStatus		&status		// (in) Return status. 
)
{
    checkStat( status, return );

    char	*adIdentity;	// Ad identity string.
    char	*appName=NULL;	// Database application register name.

    //
    //  Initialize the ad library.  Do this so we have an ad_file_info call 
    //  we can use in standalone mode.
    //

    checkAd( ad_init(), status, return );
    status += ad_msg;
    msg_clear( ad_msg );
    status.display();

#if defined(SYBASE_DHS)

    //
    //  If running standalone, check the mount points and return.
    //
   
    if ( status.standAlone() )
    {
	mountPoints( status, cDtsStoreManager::tempPath(),
		cDtsStoreManager::permPath() );
	return;
    }


    //
    //  Initialize the db library.
    //

    checkNull( appName = (char *) gen_alloc ( strlen( cDhs::identity() ) 
	    + strlen(cdmDatabase) + 1 ), status, return );
    (void) strcpy( appName, cDhs::identity() );
    (void) strcat( appName, cdmDatabase );
    checkDbLib( db_init( appName ), status, return );


    //
    //  Initialize all database connections.
    //

    checkAd( ad_dbinfo_get( AD_MED_MD, &cdmAdServer, &cdmAdDatabase, 
	    &cdmFileTbl, &cdmVolTbl), status, return );
    checkDbLib( db_open(  cdmAdServer, NULL, NULL, cdmAdDatabase, FALSE, 
	     & ( cDtsDbManager::dbprocAd ) ), status, return );
    checkDbLib( db_open(  cdmServer, NULL, NULL, cdmDatabase, FALSE, 
	    & ( cDtsDbManager::dbprocDs ) ), status, return );
    checkDbLib( db_open(  cdmServer, NULL, NULL, cdmDatabase, FALSE, 
	    & ( cDtsDbManager::dbprocPr ) ), status, return );
    checkDbLib( db_open(  cdmServer, NULL, NULL, cdmDatabase, FALSE, 
	    & ( cDtsDbManager::dbprocUn ) ), status, return );
    checkDbLib( dbrc_get( cdmAdServer, cdmAdDatabase, TRUE, &cdmUserName, 
	    &cdmPasswd ), status, return );

    //
    //  Make sure we are the only one connected to the database.
    //

    uniqueConnect( status,  cDtsDbManager::dbprocUn, appName );


    //
    //  Check that the identity string for the local dataserver in
    //  the ad database matches our identity string.
    //

    checkStat( status, return );
    checkAd( ad_server_id_get( cDtsDbManager::dbprocAd, &adIdentity ),
	    status, return );
    if ( !streq( adIdentity, cDhs::identity() ) )
    {
	status.E_AD_IDENTITY( status, adIdentity, cDhs::identity() );
    }


    //
    //  Get the temporary and permanent mount points from the ad tables.
    //

    mountPoints( status );
    free( appName );
#else
    if ( status.standAlone() )
    {
	mountPoints( status, cDtsStoreManager::tempPath(),
		cDtsStoreManager::permPath() );
	} else {
      status.E_DB(status, "cDtsDbManager::init - no SYBASE support: try -noDb?" );
   }
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::makeRetrievable
//
// INVOCATION:
// cDtsDbManager::makeRetrievable()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) fileId (char *) File id in the ad table.
// (>) fname (char *) Complete filename and path.
// (>) volName (char *) Ptr to the volume name in ad.
// (>) fsize (int) File size.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To add to the ad tables so that the fiile is then retrievable
//
// DESCRIPTION:
// To add to the ad tables so that the fiile is then retrievable.
// Do nothing if running standalone.
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

void	cDtsDbManager::makeRetrievable
(
    cDtsStatus		&status,	// (in)  Return status. 
    char		*fileId,	// (in)  File id.
    char		*fname,		// (in)  Complete filename & path.
    char		*volName,	// (in)  Ptr to the volume name.
    int			fileSize	// (in)  File size.
)
{

    checkStat( status, return );
    if ( status.standAlone() )
    {
	return;
    }


#if defined(SYBASE_DHS)

    struct ad_file	adFile;		// Ad file structure.
    cDtsDbManager	adDbInfo;	// Ad and db information.
    int			estatus;	// Status returned from ad.

    //
    //  Initialize the adFile structure that gets put in the MD tables.
    //

    adFile.adf_filesize = fileSize;
    adFile.adf_archive[0] = '\0';
    adFile.adf_filesize_uncomp = fileSize;
    (void) strcpy( adFile.adf_file_id, fileId );
    adFile.adf_mounted = TRUE;
    adFile.adf_status = AD_FILE_CURRENT;
    (void) strcpy( adFile.adf_volume_name, volName ); 
    adFile.adf_volume_medium = AD_MED_MD;
    (void) strcpy( adFile.adf_location.md.mdl_filename, fname );
    (void) ad_file_info( fname, NULL, &(adFile.adf_compression), 
	    &(adFile.adf_format) );
    estatus = ad_file_put( &adFile, adDbInfo.getDbConn( adDbInfo.DB_AD_LIB ) );
    if ( estatus != AD_SUCCESS )
    {
	status.E_AD( status, ad_msg );
    }
    adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );
#else
   status.E_DB(status, "cDtsDbManager::makeRetrievable - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::makeUnretrievable
//
// INVOCATION:
// cDtsDbManager::makeUnretrievable()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) fileId (char *) FileId in the ad tables.
// (>) lifeTime (char) Lifetime of the dataset.
// (>) fsRemove (bool) If set, indicates remove from file system.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To remove from the ad tables and potential the file system
//
// DESCRIPTION:
// To remove from the ad tables and potential the file system.
// Do nothing if in standalone mode.
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

void	cDtsDbManager::makeUnretrievable
(
    cDtsStatus		&status,	// (in)  Return status. 
    char		*fileId,	// (in)  File id.
    char		lifeTime,	// (in)  Lifetime.
    bool		fsRemove	// (in)  Remove from filesystem.
)
{

    checkStat( status, return );
    if ( status.standAlone() )
    {
	return;
    }

#if defined(SYBASE_DHS)
    struct ad_file	adFile;		// Ad file structure.
    cDtsDbManager	adDbInfo;	// Ad and db information.
    DBPROCESS		*dbProc;	// Dbprocess.
    int			estatus;	// Status returned from ... 
    AD_FILEGET		selectType;	// ad select type.
    char		*vol;		// Volume pointer.


    //
    //  Set the volume name based on the lifetime passed in.
    //

    if ( lifeTime == DTS_PERM )
    {
	vol = cDtsStoreManager::permVol();
    }
    else
    {
	vol = cDtsStoreManager::tempVol();
    }


    //
    //  Get from ad, until you get a file that matches the volume.
    //

    dbProc = adDbInfo.getDbConn( adDbInfo.DB_AD_LIB );
    selectType = AD_FG_NAME;
    while (1)
    {
	estatus = ad_file_get( selectType, &adFile, dbProc,
		AD_MED_MD, NULL, fileId );
	if ( estatus == AD_SUCCESS )
	{
	    //
	    //  Check that the volume name matches the lifetime passed in.
	    //

	    if ( streq( adFile.adf_volume_name, vol ) )
	    {
		dbcanquery( dbProc );
		break;
	    }
	    else
	    {
		selectType = AD_FG_NEXT;
	    }
	    
	}
	else
	{
	    break;
	}
    }


    //
    //  Found the dataset in ad.
    //

    if ( estatus == AD_SUCCESS )
    {
	//
	//  Delete from the ad.
	//

	estatus = ad_file_del( dbProc, AD_MED_MD, vol, 
		adFile.adf_location.md.mdl_filename);
	adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );


	if ( estatus != AD_SUCCESS )
	{
	    //
	    //  Deleting from ad failed.  Print an error.
	    //

	    status.E_AD( status, ad_msg );
	}
	else
	{
	    //
	    //  If fsRemove is set then remove it from the filesystem.
	    //

	    if ( fsRemove )
	    {
		AD_PATH			filename;
						// Actual path and filename.
		cDtsStoreManager::tLife	 storeFlag;


		//
		//  Determine storage flag to pass to remove command.
		//

		if ( lifeTime == DTS_PERM )
		{
		    storeFlag =  cDtsStoreManager::ST_PERM;
		}
		else
		{
		    storeFlag =  cDtsStoreManager::ST_TEMP;
		}


		//
		//  Determine path to the file.
		//

		(void) strcpy( filename, 
			adFile.adf_location.md.mdl_mount_point );
		(void) strcat( filename, "/" );
		(void) strcat( filename, adFile.adf_location.md.mdl_filename );

		cDtsStoreManager::fileRemove( status, filename,
			storeFlag, adFile.adf_filesize );
	    }

	}
    }
    else if ( estatus == AD_NO_FILE )
    {
	//
	//  Dataset not found in ad, print an error.
	//
	
	status.S_NOT_IN_AD( status, fileId );
	adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );


	//
	//  Attempt to remove from fs anyway.
	//

	if ( fsRemove )
	{
	    cDtsStoreManager::tLife	 storeFlag;
	    char  		      tmp[PATH_MAX];
	    
	    if ( lifeTime == DTS_PERM )
	    {
		storeFlag =  cDtsStoreManager::ST_PERM;
		(void) sprintf( tmp, "%s/%s", cDtsStoreManager::permPath(), 
			fileId );
	    }
	    else
	    {
		storeFlag =  cDtsStoreManager::ST_TEMP;
		(void) sprintf( tmp, "%s/%s", cDtsStoreManager::tempPath(), 
			fileId );
	    }

	    cDtsStoreManager::fileRemove( status, tmp,
		    storeFlag, 0 );
	}
    }
#else
   status.E_DB(status, "cDtsDbManager::makeUnretrievable - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::mountPoints
//
// INVOCATION:
// cDtsDbManager::mountPoints( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To get the mount points for the temporary and permanent directories from
// the ad tables
//
// DESCRIPTION:
// To get the mount points for the temporary and permanent directories from
// the ad tables.  Also to check that the archive's match.
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

void		cDtsDbManager::mountPoints
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
#if defined(SYBASE_DHS)
    int		estatus;
    AD_VOLUME	volume;
    char	*tmp;
    DBPROCESS	*dbProc;
    AD_ARCHIVE	*archive;


    checkStat( status, return );


    dbProc = getDbConn( DB_AD_LIB );


    //
    //  Get the Temporary volume name from ad, set it and check it.
    //

    estatus = ad_volume_get( AD_VG_NAME, &volume, dbProc, 
	    AD_MED_MD, cDtsStoreManager::tempVol() );
    if ( estatus == AD_NO_VOLUME )
    {
	status.E_VOL( status, "Temp volume not in ad tables", 
		cDtsStoreManager::tempVol() );
	releaseDbConn( DB_AD_LIB );
	return;
    }
    else if ( estatus != AD_SUCCESS ||
	    !volume.adv_mounted )
    {
	status.E_VOL( status, "Temp volume not mounted in ad tables", 
		cDtsStoreManager::tempVol() );
	releaseDbConn( DB_AD_LIB );
	return;
    }
    tmp = (char *) strsav( volume.adv_vol_info.md.mdv_mount_point );
    cDtsStoreManager::setTempPath( status, tmp );
    

    //
    //  Check that the archive in the db for the temp volume 
    //  matches the ad config file.
    //

    archive = ad_archive_info( NULL );
    if ( !streq( volume.adv_archive, archive->ada_name ) )
    {
	status.E_AD_ARCHIVE( status, "Temp", volume.adv_archive,  
		archive->ada_name );
	releaseDbConn( DB_AD_LIB );
	return;
    }


    //
    //  Get the Permanent volume name from ad, set it and check it.
    //

    estatus = ad_volume_get( AD_VG_NAME, &volume, dbProc, 
	    AD_MED_MD, cDtsStoreManager::permVol() );
    releaseDbConn( DB_AD_LIB );

    if ( estatus == AD_NO_VOLUME )
    {
	status.E_VOL( status, "Perm volume not in ad tables", 
		cDtsStoreManager::permVol() );
	return;
    }
    else if ( estatus != AD_SUCCESS ||
	    !volume.adv_mounted )
    {
	status.E_VOL( status, "Perm volume not mounted in ad tables", 
		cDtsStoreManager::permVol() );
	return;
    }
    tmp = (char *) strsav( volume.adv_vol_info.md.mdv_mount_point );
    cDtsStoreManager::setPermPath( status, tmp );


    //
    //  Check that the archive in the db for the perm volume 
    //  matches the ad config file.
    //

    if ( !streq( volume.adv_archive, archive->ada_name ) )
    {
	status.E_AD_ARCHIVE( status, "Perm", volume.adv_archive,  
		archive->ada_name );
	releaseDbConn( DB_AD_LIB );
	return;
    }
#else
      status.E_DB(status, "cDtsDbManager::mountPoints - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::mountPoints
//
// INVOCATION:
// cDtsDbManager::mountPoints( status, tempPath, permPath );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) tempPath (char *) Temporary path.
// (>) permPath (char *) Permanent path.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To save the mount points for the temporary and permanent 
// directories that are passed in.
//
// DESCRIPTION:
// To save the mount points for the temporary and permanent 
// directories that are passed in.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Must be in standalone mode.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDtsDbManager::mountPoints
(
    cDtsStatus	&status,	// (mod) Function return status.
    char	*tempPath,	// (in)  Temporary path.
    char	*permPath	// (in)  Permanent path.
)
{

    checkStat( status, return );


    //
    //  Check that the paths aren't null.
    //

    if ( tempPath == NULL || permPath == NULL )
    {
	status.E_VOL( status, "Invalid temp/perm path", 
		cDtsStoreManager::tempVol() );
	return;
    }
    cDtsStoreManager::setTempPath( status, tempPath );
    cDtsStoreManager::setPermPath( status, permPath );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::nameInfo
//
// INVOCATION:
// cDtsDbManager::nameInfo()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) fileName (char *) Passed in filename.
// (<) rootName (char *) Datasetname passed out.
// (<) format (char *) Ptr. to format character.
// (<) compression (char *) Ptr. to compression character.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To determine information from a filename
//
// DESCRIPTION:
// To determine the datasetname, format and compression type.
// Format, if not fits, is considered RAW.  Call ad_file_info.
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

void	cDtsDbManager::nameInfo
(
    cDtsStatus		&status,	// (mod) Return status. 
    char		*fileName,	// (in)  Filename.
    char		*rootName,	// (out) Stripped filename. 
    char		*format,	// (out) Ptr to format.
    char		*compression	// (out) Ptr. to compression.
)
{

    checkStat( status, return );

    //
    //  Initialize values of format & compression and call ad_file_info.
    //

    *format = AD_FMT_UNKNOWN;
    *compression = AD_COMP_UNKNOWN;
    (void) ad_file_info( fileName, rootName, compression, format );
    if ( *format == AD_FMT_FITS )
    {
	*format = DTS_FM_FITS ;
    }
    else
    {
    	*format = DTS_FM_RAW;
    }
    status.S_DEBUG_MSG( status, rootName );
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::releaseDbConn
//
// INVOCATION:
// cDtsDbManager::releaseDbConn()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) type (tProcessCon) Type of connection to release.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To release a lock
//
// DESCRIPTION:
// Release a database connection lock dependent on the type passed in.
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

void	cDtsDbManager::releaseDbConn
(
    tProcessCon		type	// (in)  Type of connection to get.
)
{
    cDtsStatus		status;
#if defined(SYBASE_DHS)

    status.S_DB_CONN( status, type );

    switch ( type ) {
	case DB_AD_LIB:
	    cdmMutexAd.unlock();
	    break;
	case DB_DS_LIB:
	    cdmMutexDs.unlock();
	    break;
	case DB_PR_LIB:
	    cdmMutexPr.unlock();
	    break;
	case DB_UN_LIB:
	    cdmMutexUn.unlock();
	    break;
    }
#else
   status.E_DB(status, "cDtsDbManager::releaseDbConn - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::testAll
//
// INVOCATION:
// cDtsDbManager::testAll()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) - Returned status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To test all database connections
//
// DESCRIPTION:
// To test all database connections.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// That there are no dataset names or unique names with the value "X".
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool cDtsDbManager::testAll
(
    cDtsStatus		&status		// (in) Return status. 
)
{
#if defined(SYBASE_DHS)
    struct ad_file	adFile;		// Ad file structure.
    long		dateNotified;
    DBPROCESS		*dbProc;	// Dbprocess.
    int			estatus;	// Status returned from ... 
    cDtsDatasetTable	*pTableInfo = NULL;
    cDtsDbManager	adDbInfo;	// Ad and db information.


    checkStat( status, return( FALSE ) );


    //
    //  Select "X" from all 3 tables.
    //

    cDtsDatasetTable::fileGet( status, "X", &pTableInfo );
    if ( status != status.S_NO_ROWS )
    {
	return( FALSE );
    }

    cDtsUniqueTable::uniqueNameGet( status, "X", &dateNotified );
    if ( status != status.S_NO_ROWS )
    {
	return( FALSE );
    }

    cDtsDatasetTable::dsGet( status, "X", &pTableInfo );
    if ( status != status.S_NO_ROWS )
    {
	return( FALSE );
    }


    //
    //  Check that the ad table exists, both mds and mdfiles
    //

    dbProc = adDbInfo.getDbConn( adDbInfo.DB_AD_LIB );
    estatus = ad_file_get( AD_FG_NAME, &adFile, dbProc,
	    AD_MED_MD, NULL, "X" );
    if ( estatus != AD_NO_FILE && estatus != AD_SUCCESS )
    {
	adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );
	return( FALSE );
    }
    adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );

    return( TRUE );
#else
   status.E_DB(status, "cDtsDbManager::testAll - no SYBASE support: try -noDb?" );
   return( FALSE );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDbManager::uniqueConnect
//
// INVOCATION:
// cDtsDbManager::uniqueConnect()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) dbProc (struct dbprocess *) The db process.
// (>) appString (char *) Application name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To check to see that this is the only dataServer connected to db
//
// DESCRIPTION:
// Check to see that this is the only dataServer with this identity string
// connected to db.
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

void	cDtsDbManager::uniqueConnect
(
    cDtsStatus	&status,		// (mod) Function return status.
    struct dbprocess
		*dbProc,		// (in)  The dbprocess to use.
    char	*appString		// (in)  The application name string.
)
{
#if defined(SYBASE_DHS)
    int         appCount;       // # of mediaQueues running on this ID. 

 
    checkStat( status, return );


    //
    //  Count the number of applications running with that name.
    //

    checkDbLib( db_count_app( dbProc, appString, NULL, &appCount ), 
       status, return );
 
 
    //
    //  If appCount > 1, there's another dataServer running on this ID, 
    //  so log it, close the db process, and exit.
    //
 
    if ( appCount > 1 )
    {
	//
	//  Error more than 1 is running.
	//

	status.E_ONLY_ONE( status, appString );
    }
#else
   status.E_DB(status, "cDtsDbManager::uniqueConnect - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::cDtsDatasetTable
//
// INVOCATION:
// n.a. Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) datasetName (char * ) DatasetName.
// (>) lifetime (char) Lifetime.
// (>) uniqueName (char *) Unique name.
// (>) source (char *) Source of the request.
// (>) dataRec (long) Date received.
// (>) format (DHS_BD_PUT_TYPE) Data type.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To initialize a dataset class structure with the variables passed in
//
// DESCRIPTION:
// To initialize a dataset class structure that may be put in the database
// and will be stored in the dataset list.
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

	 cDtsDatasetTable::cDtsDatasetTable
(
    char	*datasetName, 	// (in)  Datasetname.
    char	lifetime,	// (in)  Lifetime.
    char	*uniqueName,	// (in)  UniqueName.
    char	*source,	// (in)  Source of the data.
    long	dateRec,	// (in)  Date received.
    DHS_BD_PUT_TYPE
    		format		// (in)  Data type.
)
{
    
    dstDatasetName  = (char *) strsav( datasetName );
    if ( lifetime != '\0' )
    {
	dstLifetime = lifetime;
    }
    else
    {
	dstLifetime = cDtsLists::defaultLife();
    }

    if ( dstLifetime == DTS_PERM )
    {
	dstSsNotify 	= DTS_SS_NOT_NOTIFIED;
    }
    else
    {
	dstSsNotify 	= DTS_SS_UNECESSARY;
    }

    dstDateReceived	= dateRec;
    dstUniqueName 	= (char *) strsav( uniqueName );
    dstHealth 		= DTS_HL_HEALTHY;
    dstOldpNotify 	= DTS_OL_NOT_NOTIFIED;
    dstReceived 	= DTS_DS_NOT_RECEIVED;
    dstFormat		= formatDetermine( format );
    dstContributors  	= strsav( source );
    dstFileSize		= 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::cDtsDatasetTable
//
// INVOCATION:
// cDtsDatasetTable::cDtsDatasetTable()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) datasetName (char * ) DatasetName.
// (>) uniqueName (char *) Unique name.
// (>) dateNotified (long) Date received.
// (>) lifetime (char) Lifetime.
// (>) health (char) Health of the dataset.
// (>) oldpNotify (char) Has oldp server been notified.
// (>) ssNotify (char) Has storage server been notified.
// (>) format (char) Data format.
// (>) contributors (char *) List of contributors.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To initialize a dataset class structure with the variables passed in
//
// DESCRIPTION:
// To initialize a dataset class structure that may be put in the database
// and will be stored in the dataset list.
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

	 cDtsDatasetTable::cDtsDatasetTable
(
    char	*datasetName, 	// (in)  Datasetname.
    char	*uniqueName,	// (in)  UniqueName.
    long	dateNotified,	// (in)  Date notified.
    char	received,	// (in)  Received state.
    char	lifetime,	// (in)  Lifetime.
    char	health,		// (in)  Health.
    char	oldpNotify,	// (in)  Oldp notification.
    char	ssNotify,	// (in)  Storage server notification.
    char	format,		// (in)  Data format.
    char	*contributors	// (in)  List of contributors.
)
{
    
    dstDatasetName	= (char *) strsav( datasetName );
    dstLifetime		= lifetime;
    dstSsNotify		= ssNotify;
    dstDateReceived	= dateNotified;
    dstUniqueName 	= (char *) strsav( uniqueName );
    dstHealth 		= health;
    dstOldpNotify 	= oldpNotify;
    dstReceived		= received;
    dstFormat		= format;
    dstContributors  	= strsav( contributors );
    dstFileSize		= 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::~cDtsDatasetTable
//
// INVOCATION:
// cDtsDatasetTable::~cDtsDatasetTable()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To free object variables
//
// DESCRIPTION:
// To free datasetname, uniquename and contributors.
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

	 cDtsDatasetTable::~cDtsDatasetTable
(
    void
)
{
    
    if ( dstDatasetName != NULL )
    {
	free( dstDatasetName );
    }

    if( dstUniqueName != NULL )
    {
	free( dstUniqueName );
    }
    if ( dstContributors != NULL )
    {
	free( dstContributors );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::datasetNameMax
//
// INVOCATION:
// cDtsDatasetTable::datasetNameMax
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) uniquePart (char * ) Unique part of the dataset name. 
// (<) num (int *) Maximum number in the database.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To get the maximum number, given the dataset name
//
// DESCRIPTION:
// To get the maximum number from the database, given the dataset name.
// It searches for the datasetname + - + #.  It looks for the largest
// number.
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

void	cDtsDatasetTable::datasetNameMax
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*uniquePart,		// (in)  Dataset Name.
    int		*num			// (out) Maximum number.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		i;			// Integer counter to return;
    char	tmp[DTS_DATASET_NAME_LEN];
    					// Temporary string.
    char	*tmp2;			// Temporary string.
    int		itmp;			// Temporary number.
    int		numPosition;		// 


    //
    //  Check status, set variables, get connection.
    //

    checkStat( status, return );
    i = 0;
    numPosition = strlen(uniquePart)+1;
   
    dbProc = getDbConn( DB_DS_LIB );
    checkDb( dbcanquery( dbProc ), DB_DS_LIB, status, return );


    //
    // Select from the database where the dataset name if like the uniquePart.
    //

    checkDb( dbfcmd( dbProc,
	"select \n"
	"       datasetName \n"
	"from \n"
	"       %s..%s \n"
	"where \n"
	"       datasetName like '%s%c' \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::processTbl(), uniquePart, '%' ), 
	DB_DS_LIB, status, return );

    checkDb( dbsqlexec( dbProc ), DB_DS_LIB, status, return );
    checkDb( dbresults( dbProc ), DB_DS_LIB, status, return );

    while ( DBROWS( dbProc ) == SUCCEED )
    {
	if ( dbnextrow( dbProc ) == NO_MORE_ROWS )
	{
	    //
	    //  Nothing in the database.
	    //

	    status.S_NO_ROWS( status, uniquePart );
	    break;
	}
	else
	{
	    //
	    //  Get the dataset name, get the integer off the end and try to
	    //  convert to an integer.
	    //

	   checkDbCall( db_stringdata( dbProc, 1, tmp, sizeof( tmp ) ),
		    DB_DS_LIB, status, VOID);
	   if ( (int) strlen(tmp) > numPosition )
	   {
		tmp2 = &(tmp[numPosition]);

		if ( strint( tmp2 ) && (itmp = atoi( tmp2 )) > i  )
	        {
		    i = itmp;
	         }
	    }
	}
    }

    releaseDbConn( DB_DS_LIB );
    *num = ++i;
#else
   status.E_DB(status, "cDtsDbManager::datasetNameMax - no SYBASE support: try -noDb?" );
   *num = -1;
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::dsGet
//
// INVOCATION:
// dsGet(status, datasetName, &tableInfo );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) datasetName (char * ) Dataset Name.
// (<) tableInfo (cDtsDatasetTable **) Dataset table information returned.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To do a get from the database from the dataset table
//
// DESCRIPTION:
// To do a get from the database from the dataset table.
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

void	cDtsDatasetTable::dsGet
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		**tableInfo		// (out) Dataset table information.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    DTS_CONTRIBUTORS
    		contrib;		// List of contributors of the ds.
    char	format;			// Data format.
    char	health;			// Data health;

    checkStat( status, return );
   
    if ( strlen( datasetName ) <= 0 )
    {
	status.E_STR_LEN( status, "datasetName" );
	return;
    }

    dbProc = getDbConn( DB_DS_LIB );


    checkDb( dbcanquery( dbProc ), DB_DS_LIB, status, return );


    //
    // Select from the database and enter into the structure passed in.
    //

    checkDb( dbfcmd( dbProc,
	"select \n"
	"       health, \n"
	"       format, \n"
	"       contributors \n"
	"from \n"
	"       %s..%s \n"
	"where \n"
	"       datasetName = '%s' \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::datasetTbl(), datasetName ), 
	DB_DS_LIB, status, return );

    checkDb( dbsqlexec( dbProc ), DB_DS_LIB, status, return );
    checkDb( dbresults( dbProc ), DB_DS_LIB, status, return );

    if ( DBROWS( dbProc ) == SUCCEED )
    {
	if ( dbnextrow( dbProc ) == NO_MORE_ROWS )
	{
	    //
	    //  Not found in the database.
	    //

	    status.S_NO_ROWS( status, datasetName );
	}
	else
	{
	    checkDbCall( db_chardata( dbProc, 1, &( health ) ), 
		    DB_DS_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 2, &( format ) ), 
		    DB_DS_LIB, status, return );
	    checkDbCall( db_stringdata( dbProc, 3, contrib, sizeof( contrib ) ),
		    DB_DS_LIB, status, return );

	    //
	    //  Save the data to a new dataset table object.
	    //

	    *tableInfo = new cDtsDatasetTable( datasetName, datasetName,
		    0, 0, 0, health, 0, 0, format, contrib );
	}
    }
    else
    {
	//
	//  Select failed to produce rows.
	//

	status.S_NO_ROWS( status, datasetName );
    }

    releaseDbConn( DB_DS_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::dsGet - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::dsPut
//
// INVOCATION:
// dsPut( status, datasetName, tableInfo );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) datasetName (char * ) Dataset Name.
// (>) tableInfo (cDtsDatasetTable *) Dataset table information to put.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To do a get from the database from the dataset table
//
// DESCRIPTION:
// To do a get from the database from the dataset table.
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

void	cDtsDatasetTable::dsPut
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		*tableInfo		// (in)  Dataset table information.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		dbStatus;


    checkStat( status, return );
   
    if ( strlen( datasetName ) <= 0 || tableInfo == NULL )
    {
	status.E_STR_LEN( status, "datasetName or datasetTableInfo" );
	return;
    }

    dbProc = getDbConn( DB_DS_LIB );
    checkDb( dbcanquery( dbProc ), DB_DS_LIB, status, return );


    //
    // Insert into db.
    //

    int		estatus;
    estatus = db_simple_query( dbProc,
	"insert into \n"
	"       %s..%s \n"
	"( \n"
	"       datasetName, \n"
	"       health, \n"
	"       format, \n"
	"       contributors \n"
	") \n"
	"values ( \n"
	"    '%s', \n"
	"    '%c', \n"
	"    '%c', \n"
	"    '%s' \n"
	") \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::datasetTbl(),
	datasetName,
	tableInfo->dstHealth,
	tableInfo->dstFormat,
	tableInfo->dstContributors );

    if ( estatus != DB_SUCCESS )
    {
	status.E_DB( status, db_msg );
	releaseDbConn( DB_DS_LIB );
	return;
    }

    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
    {
    }
    checkDb( dbStatus, DB_DS_LIB, status, return );

    releaseDbConn( DB_DS_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::dsPut - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::fileDel
//
// INVOCATION:
// cDtsDatasetTable::fileDel
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) datasetName (char * ) Dataset Name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To delete from the database from the process table
//
// DESCRIPTION:
// To delete from the database from the process table.
// If running standalone, just return.
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

void	cDtsDatasetTable::fileDel
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*datasetName		// (in)  Dataset name.
)
{
    checkStat( status, return );
  

    //
    //  If running standalone, return.
    //

    if ( status.standAlone() )
    {
	return;
    }

#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		dbStatus;

    if ( strlen( datasetName ) <= 0 )
    {
	status.E_STR_LEN( status, "datasetName" );
	return;
    }

    dbProc = getDbConn( DB_PR_LIB );
    checkDb( dbcanquery( dbProc ), DB_PR_LIB, status, return );


    //
    // Delete into db.
    //

    int		estatus;
    estatus = db_simple_query( dbProc,
	"delete from \n"
	"       %s..%s \n"
	"where "
	"       datasetName = '%s' \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::processTbl(),
	datasetName );

    if ( estatus != DB_SUCCESS )
    {
	status.E_DB( status, db_msg );
	releaseDbConn( DB_PR_LIB );
	return;
    }

    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
    {
    }
    checkDb( dbStatus, DB_PR_LIB, status, return );

    releaseDbConn( DB_PR_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::fileDel - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::fileGet
//
// INVOCATION:
// cDtsDatasetTable::fileGet
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) datasetName (char * ) Dataset Name.
// (<) tableInfo (cDtsDatasetTable **) Dataset table information returned.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To do a get from the database from the process table
//
// DESCRIPTION:
// To do a get from the database from the process table.
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

void	cDtsDatasetTable::fileGet
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		**tableInfo		// (out) Dataset table information.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    DTS_UNIQUE_NAME
    		ukName;			// Unique name of the ds.
    DTS_CONTRIBUTORS
    		contrib;		// List of contributors of the ds.
    char	lifetime;		// Lifetime of the dataset.
    char	oldpFlag;		// Oldp flag.
    char	ssFlag;			// Storage flag.
    char	format;			// Data format.
    char	health;			// Data health;
    char	received;		// Data received state;
    long	dateNotified;		// Date notified.

    checkStat( status, return );
   
    if ( strlen( datasetName ) <= 0 )
    {
	status.E_STR_LEN( status, "datasetName" );
	return;
    }

    dbProc = getDbConn( DB_PR_LIB );


    checkDb( dbcanquery( dbProc ), DB_PR_LIB, status, return );


    //
    // Select from the database and enter into the structure passed in.
    //

    checkDb( dbfcmd( dbProc,
	"select \n"
	"       uniqueName, \n"
	"       datediff( second, 'jan 1 1970', dateReceived ), \n"
	"       received, \n"
	"       lifetime, \n"
	"       health, \n"
	"       oldpNotify, \n"
	"       ssNotify, \n"
	"       format, \n"
	"       contributors \n"
	"from \n"
	"       %s..%s \n"
	"where \n"
	"       datasetName = '%s' \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::processTbl(), datasetName ), 
	DB_PR_LIB, status, return );

    checkDb( dbsqlexec( dbProc ), DB_PR_LIB, status, return );
    checkDb( dbresults( dbProc ), DB_PR_LIB, status, return );

    if ( DBROWS( dbProc ) == SUCCEED )
    {
	if ( dbnextrow( dbProc ) == NO_MORE_ROWS )
	{
	    status.S_NO_ROWS( status, datasetName );
	}
	else
	{
	    checkDbCall( db_stringdata( dbProc, 1, ukName, sizeof( ukName ) ),
		    DB_PR_LIB, status, return );
	    checkDbCall( db_intdata( dbProc, 2, (int *) &(dateNotified) ),
		    DB_PR_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 3, &( received ) ), 
		    DB_PR_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 4, &( lifetime ) ), 
		    DB_PR_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 5, &( health ) ), 
		    DB_PR_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 6, &( oldpFlag ) ), 
		    DB_PR_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 7, &( ssFlag ) ), 
		    DB_PR_LIB, status, return );
	    checkDbCall( db_chardata( dbProc, 8, &( format ) ), 
		    DB_PR_LIB, status, return );
	    checkDbCall( db_stringdata( dbProc, 9, contrib, sizeof( contrib ) ),
		    DB_PR_LIB, status, return );

	    //
	    //  Save the data to a new dataset table object.
	    //

	    *tableInfo = new cDtsDatasetTable( datasetName, ukName,
		    dateNotified, received, lifetime, health, oldpFlag, ssFlag,
		    format, contrib );
	}
    }
    else
    {
	//
	// Select failed to produce any rows.
	//

	status.S_NO_ROWS( status, datasetName );
    }

    releaseDbConn( DB_PR_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::fileGet - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::fileListGet
//
// INVOCATION:
// cDtsDatasetTable::fileListGet
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) selectType (char) Types T, S, O or R.
// (<) tableInfo (map & ) List of dataset objects returned.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To get a list of datasets from the database
//
// DESCRIPTION:
// To get a list of datasets from the database.
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

void	cDtsDatasetTable::fileListGet
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	selectType,		// (in)  Select type.
    std::map< std::string,				// Sort key of datatype string.
    cDtsDatasetTable *,				// Into this containr this type
    std::less<std::string> >				// Sorting mechanizm.
    		&tableInfo		// Provides access to containr
					//        information.
)
{
#if defined(SYBASE_DHS)
    DTS_DATASET_NAME
    		datasetName;		// Unique name of the ds.
    long	dateNotified;		// Date notified.
    DBPROCESS	*dbProc;		// Dbprocess.
    DTS_CONTRIBUTORS
    		contrib;		// List of contributors of the ds.
    char	format;			// Data format.
    char	health;			// Data health;
    char	lifetime;		// Lifetime of the dataset.
    char	oldpFlag;		// Oldp flag.
    cDtsDatasetTable
    		*pTable;		// Pointer to table information.
    char	received;		// Data received state;
    char	ssFlag;			// Storage flag.
    DTS_UNIQUE_NAME
    		ukName;			// Unique name of the ds.


    checkStat( status, return );


    //
    //  Get the database connection and lock, then cancel any prior query.
    //

    dbProc = getDbConn( DB_PR_LIB );
    checkDb( dbcanquery( dbProc ), DB_PR_LIB, status, return );


    //
    // Select from the database and enter into the array passed in.
    // Select where received = Partial and lifetime is permanent.
    //

    checkDb( dbfcmd( dbProc,
	"select \n"
	"       datasetName, \n"
	"       uniqueName, \n"
	"       datediff( second, 'jan 1 1970', dateReceived ), \n"
	"       received, \n"
	"       lifetime, \n"
	"       health, \n"
	"       oldpNotify, \n"
	"       ssNotify, \n"
	"       format, \n"
	"       contributors \n"
	"from \n"
	"       %s..%s \n"
	"where \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::processTbl() ),
	DB_PR_LIB, status, return );

    if ( selectType == 'T' )
    {
	//
	//  Select where lifetime is temporary.
	//

	checkDb( dbfcmd( dbProc,
	    "       lifetime = '%c' \n",
	    DTS_TEMP ), 
	    DB_PR_LIB, status, return );
    }
    else if ( selectType == 'S' )
    {
	//
	//  Where all has been received, lifetime is perm, and 
	//  ssNotify not notified
	//

	checkDb( dbfcmd( dbProc,
	    "       received = '%c' \n"
	    "       and ssNotify = '%c' \n"
	    "       and lifetime = '%c' \n",
	    DTS_DS_ALL_RECEIVED,
	    DTS_SS_NOT_NOTIFIED,
	    DTS_PERM ), 
	    DB_PR_LIB, status, return );
    }
    else if ( selectType == 'O' )
    {
	//
	//  Where lifetime is perm(cause temp's are delete),
	//  health ok, oldpNotify not notified
	//

	checkDb( dbfcmd( dbProc,
	    "       received = '%c' \n"
	    "       and health = '%c' \n"
	    "       and oldpNotify = '%c' \n"
	    "       and lifetime = '%c' \n",
	    DTS_DS_ALL_RECEIVED,
	    DTS_HL_HEALTHY,
	    DTS_OL_NOT_NOTIFIED,
	    DTS_PERM ), 
	    DB_PR_LIB, status, return );
    }
    else if ( selectType == 'R' )
    {
	//
	//  Were data is partially received and lifetime is permanent.
	//

	checkDb( dbfcmd( dbProc,
	    "       received = '%c' \n"
	    "       and lifetime = '%c' \n",
	    DTS_DS_PART_RECEIVED,
	    DTS_PERM ), 
	    DB_PR_LIB, status, return );
    }
    checkDb( dbsqlexec( dbProc ), DB_PR_LIB, status, return );
    checkDb( dbresults( dbProc ), DB_PR_LIB, status, return );

    if ( DBROWS( dbProc ) == SUCCEED )
    {
	while ( dbnextrow( dbProc ) != NO_MORE_ROWS )
	{
	    checkDbCall( db_stringdata( dbProc, 1, datasetName, 
		    sizeof( datasetName ) ), DB_PR_LIB, status, break );
	    checkDbCall( db_stringdata( dbProc, 2, ukName, sizeof( ukName ) ),
		    DB_PR_LIB, status, break );
	    checkDbCall( db_intdata( dbProc, 3, (int *) &(dateNotified) ),
		    DB_PR_LIB, status, break );
	    checkDbCall( db_chardata( dbProc, 4, &( received ) ), 
		    DB_PR_LIB, status, break );
	    checkDbCall( db_chardata( dbProc, 5, &( lifetime ) ), 
		    DB_PR_LIB, status, break );
	    checkDbCall( db_chardata( dbProc, 6, &( health ) ), 
		    DB_PR_LIB, status, break );
	    checkDbCall( db_chardata( dbProc, 7, &( oldpFlag ) ), 
		    DB_PR_LIB, status, break );
	    checkDbCall( db_chardata( dbProc, 8, &( ssFlag ) ), 
		    DB_PR_LIB, status, break );
	    checkDbCall( db_chardata( dbProc, 9, &( format ) ), 
		    DB_PR_LIB, status, break );
	    checkDbCall( db_stringdata( dbProc, 10, contrib, sizeof( contrib ) ),
		    DB_PR_LIB, status, break );

	    //
	    //  Insert into new dataset table object and add to the local list.
	    //

	    pTable = new cDtsDatasetTable( datasetName, ukName,
		    dateNotified, received, lifetime, health, oldpFlag, ssFlag,
		    format, contrib );

	    tableInfo[ std::string( datasetName ) ] = pTable;

	}
    }

    releaseDbConn( DB_PR_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::fileListGet - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::filePut
//
// INVOCATION:
// cDtsDatasetTable::filePut
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) datasetName (char * ) Dataset Name.
// (>) tableInfo (cDtsDatasetTable *) Dataset table information to put.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To put data to the process table
//
// DESCRIPTION:
// Does a put to the process table in the database.
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

void	cDtsDatasetTable::filePut
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		*tableInfo		// (out) Dataset table information.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		dbStatus;


    checkStat( status, return );
   
    if ( strlen( datasetName ) <= 0 || tableInfo == NULL )
    {
	status.E_STR_LEN( status, "datasetName or datasetTableInfo" );
	return;
    }

    dbProc = getDbConn( DB_PR_LIB );
    checkDb( dbcanquery( dbProc ), DB_PR_LIB, status, return );


    //
    // Insert into db.
    //

    int		estatus;
    estatus = db_simple_query( dbProc,
	"insert into \n"
	"       %s..%s \n"
	"( \n"
	"       datasetName, \n"
	"       uniqueName, \n"
	"       dateReceived, \n"
	"       received, \n"
	"       lifetime, \n"
	"       health, \n"
	"       oldpNotify, \n"
	"       ssNotify, \n"
	"       format, \n"
	"       contributors \n"
	") \n"
	"values ( \n"
	"    '%s', \n"
	"    '%s', \n"
	"    dateadd( second, %d, 'jan 1 1970' ), \n"
	"    '%c', \n"
	"    '%c', \n"
	"    '%c', \n"
	"    '%c', \n"
	"    '%c', \n"
	"    '%c', \n"
	"    '%s' \n"
	") \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::processTbl(),
	datasetName,
	tableInfo->dstUniqueName,
	tableInfo->dstDateReceived,
	tableInfo->dstReceived,
	tableInfo->dstLifetime,
	tableInfo->dstHealth,
	tableInfo->dstOldpNotify,
	tableInfo->dstSsNotify,
	tableInfo->dstFormat,
	tableInfo->dstContributors );

    if ( estatus != DB_SUCCESS )
    {
	status.E_DB( status, db_msg );
	releaseDbConn( DB_PR_LIB );
	return;
    }

    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
    {
    }
    checkDb( dbStatus, DB_PR_LIB, status, return );

    releaseDbConn( DB_PR_LIB );

#else
   status.E_DB(status, "cDtsDatasetTable::filePut - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::fileUpdate
//
// INVOCATION:
// cDtsDatasetTable::fileUpdate
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) datasetName (char * ) Dataset Name.
// (>) tableInfo (cDtsDatasetTable *) Dataset table information to update.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To update the process table
//
// DESCRIPTION:
// To update the process table.
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

void	cDtsDatasetTable::fileUpdate
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		*tableInfo		// (in)  Dataset table information.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		dbStatus;


    if ( strlen( datasetName ) <= 0 || tableInfo == NULL )
    {
	status.E_STR_LEN( status, "datasetName or datasetTableInfo" );
	return;
    }

    dbProc = getDbConn( DB_PR_LIB );
    checkDb( dbcanquery( dbProc ), DB_PR_LIB, status, return );


    //
    // Update the db.
    //

    int		estatus;
    estatus = db_simple_query( dbProc,
	"update \n"
	"       %s..%s \n"
	"set  \n"
	"       received = '%c', \n"
	"       lifetime = '%c', \n"
	"       health = '%c', \n"
	"       oldpNotify = '%c', \n"
	"       ssNotify = '%c', \n"
	"       format = '%c', \n"
	"       contributors = '%s' \n"
	"where \n"
	"       datasetName = '%s'\n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::processTbl(),
	tableInfo->dstReceived,
	tableInfo->dstLifetime,
	tableInfo->dstHealth,
	tableInfo->dstOldpNotify,
	tableInfo->dstSsNotify,
	tableInfo->dstFormat,
	tableInfo->dstContributors,
	datasetName );

    if ( estatus != DB_SUCCESS )
    {
	status.E_DB( status, db_msg );
	releaseDbConn( DB_PR_LIB );
	return;
    }

    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
    {
    }
    checkDb( dbStatus, DB_PR_LIB, status, return );

    releaseDbConn( DB_PR_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::fileUpdat - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetTable::getAdFileInfo
//
// INVOCATION:
// dataset.getAdFileInfo( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Fill in the information in the cDtsDatasetTable structure only available
// from the ad library.
//
// DESCRIPTION:
// Description of the algorithm.
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

void		cDtsDatasetTable::getAdFileInfo
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
#if defined(SYBASE_DHS)
    AD_FILE	adFile;
    int		adStatus;	// Ad library return status.
    cDtsDbManager	
		adDbInfo;	// Ad and db information.


    adStatus = ad_file_get( AD_FG_NAME, &adFile, 
	    adDbInfo.getDbConn( adDbInfo.DB_AD_LIB ),
	    AD_MED_MD, NULL, dstDatasetName );
    adDbInfo.releaseDbConn( adDbInfo.DB_AD_LIB );

    dstFileSize = adFile.adf_filesize;

    checkAd( adStatus, status, VOID );
#else
   status.E_DB(status, "cDtsDatasetTable::getAdFileInfo - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueTable::cDtsUniqueTable
//
// INVOCATION:
// n.a.  Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) uniqueName (char * ) - uniqueName.
// (>) dateNotified (long ) = date notified about this unique name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor
//
// DESCRIPTION:
// To initialize a unique table object with variables passed in.
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

	 cDtsUniqueTable::cDtsUniqueTable
(
    char	*uniqueName,	// (in)  UniqueName.
    long	dateNotified	// (in)  Date notified.
)
{
    uqnUniqueName 	= (char *) strsav( uniqueName );
    uqnDateReceived 	= dateNotified;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueTable::~cDtsUniqueTable
//
// INVOCATION:
// n.a. Deconstructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor
//
// DESCRIPTION:
// To free memory.
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

	 cDtsUniqueTable::~cDtsUniqueTable
(
    void
)
{
    if ( uqnUniqueName != NULL )
    {
	free( uqnUniqueName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueTable::uniqueNameGet
//
// INVOCATION:
// cDtsUniqueTable::uniqueNameGet
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) uniqueName (char * ) Unique Name
// (<) dateNotified (long) Returned date.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To get a unique name from the database
//
// DESCRIPTION:
// Attempts to do a get from the database given the unique name.  It returns
// the dateNotified.
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

void	cDtsUniqueTable::uniqueNameGet
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*uniqueName,		// (in)  UniqueName.
    long	*dateNotified		// (out) dataNotified.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.

    checkStat( status, return );
   
    if ( uniqueName == NULL || strlen( uniqueName ) <= 0 )
    {
	//
	//  Unique name is missing.
	//

	status.E_STR_LEN( status, "uniqueName" );
	return;
    }

    dbProc = getDbConn( DB_UN_LIB );
    checkDb( dbcanquery( dbProc ), DB_UN_LIB, status, return );


    //
    // Select from the database and enter into the structure passed in.
    //

    checkDb( dbfcmd( dbProc,
	"select \n"
	"       datediff( second, 'jan 1 1970', dateReceived ) \n"
	"from \n"
	"       %s..%s \n"
	"where \n"
	"       uniqueName = '%s' \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::uniqueTbl(), uniqueName ), 
	DB_UN_LIB, status, return );

    checkDb( dbsqlexec( dbProc ), DB_UN_LIB, status, return );
    checkDb( dbresults( dbProc ), DB_UN_LIB, status, return );

    if ( DBROWS( dbProc ) == SUCCEED )
    {
	if ( dbnextrow( dbProc ) == NO_MORE_ROWS )
	{
	    status.S_NO_ROWS( status, uniqueName );
	}
	else
	{
	    checkDbCall( db_intdata( dbProc, 1, (int *) (dateNotified) ),
		    DB_UN_LIB, status, return );
	}
    }
    else
    {
	//
	// Not found in the database.
	//

	status.S_NO_ROWS( status, uniqueName );
    }

    releaseDbConn( DB_UN_LIB );
#else
   status.E_DB(status, "cDtsDatasetTable::uniqueNameGet - no SYBASE support: try -noDb?" );
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueTable::uniqueNameMax
//
// INVOCATION:
// cDtsUniqueTable::uniqueNameMax( status, uniquePart, num )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) uniquePart (char * ) Unique part of the unique name.
// (<) num (int *) Maximum number in the database.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To get the maximum number, given the unique name
//
// DESCRIPTION:
// To get the maximum number from the database, given the unique name.
// It searches for the uniqueName + - + #.  It looks for the largest
// number and returns it.
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

void	cDtsUniqueTable::uniqueNameMax
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*uniquePart,		// (in)  UniqueName.
    int		*num			// (out) Number.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		i;			// Integer counter to return;
    char	tmp[DTS_UNIQUE_NAME_LEN];
    int		itmp;

    checkStat( status, return );
    i = 0;
   
    dbProc = getDbConn( DB_UN_LIB );
    checkDb( dbcanquery( dbProc ), DB_UN_LIB, status, return );


    //
    // Select from the database and enter into the structure passed in.
    //

    checkDb( dbfcmd( dbProc,
	"select \n"
	"       substring( uniqueName, 11,45 )  \n"
	"from \n"
	"       %s..%s \n"
	"where \n"
	"       uniqueName like '%s' \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::uniqueTbl(), uniquePart ), 
	DB_UN_LIB, status, return );

    checkDb( dbsqlexec( dbProc ), DB_UN_LIB, status, return );
    checkDb( dbresults( dbProc ), DB_UN_LIB, status, return );

    while ( DBROWS( dbProc ) == SUCCEED )
    {
	if ( dbnextrow( dbProc ) == NO_MORE_ROWS )
	{
	    //
	    //  No more rows.
	    //

	    status.S_NO_ROWS( status, uniquePart );
	    break;
	}
	else
	{
	    //
	    //  Get the string, and try to convert to a integer.
	    //

	   checkDbCall( db_stringdata( dbProc, 1, tmp, sizeof( tmp ) ),
		    DB_UN_LIB, status, VOID);
	   if ( strint( tmp ) && (itmp = atoi( tmp)) > i  )
	   {
		i = itmp;
	   }
	}
    }

    releaseDbConn( DB_UN_LIB );
    *num = i;
#else
   status.E_DB(status, "cDtsUniqueTable::uniqueNameMax - no SYBASE support: try -noDb?" );
    *num = -1;
#endif
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueTable::uniqueNamePut
//
// INVOCATION:
// cDtsUniqueTable::uniqueNamePut
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Returned status.
// (>) uniqueName (char * ) Unique Name
// (>) dateNotified (long) Returned date.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To do a Put to the uniqueName table
//
// DESCRIPTION:
// To do a Put to the uniqueName table
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

void	cDtsUniqueTable::uniqueNamePut
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*uniqueName,		// (in)  Unique name to put.
    long	dateNotified		// (in)  Date notified.
)
{
#if defined(SYBASE_DHS)
    DBPROCESS	*dbProc;		// Dbprocess.
    int		dbStatus;


    checkStat( status, return );
   
    if ( uniqueName == NULL || strlen( uniqueName ) <= 0 )
    {
	//
	//  Unique name is missing.
	//

	status.E_STR_LEN( status, "uniqueName" );
	return;
    }

    dbProc = getDbConn( DB_UN_LIB );
    checkDb( dbcanquery( dbProc ), DB_UN_LIB, status, return );


    //
    // Insert into db.
    //

    int		estatus;
    estatus = db_simple_query( dbProc,
	"insert into \n"
	"       %s..%s \n"
	"( \n"
	"    uniqueName, \n"
	"    dateReceived \n"
	") \n"
	"values ( \n"
	"    '%s', \n"
	"    dateadd( second, %d, 'jan 1 1970' ) \n"
	") \n",
	cDtsDbManager::storeDb(),
	cDtsDbManager::uniqueTbl(),
	uniqueName, dateNotified );

    if ( estatus != DB_SUCCESS )
    {
	status.E_DB( status, db_msg );
	releaseDbConn( DB_UN_LIB );
	return;
    }

    while ( ( dbStatus = dbresults( dbProc ) ) == SUCCEED )
    {
    }
    checkDb( dbStatus, DB_UN_LIB, status, return );

    releaseDbConn( DB_UN_LIB );
#else
   status.E_DB(status, "cDtsUniqueTable::uniqueNamePut - no SYBASE support: try -noDb?" );
#endif
}
