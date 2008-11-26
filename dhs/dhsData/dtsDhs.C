static char rcsid[] = "$Id: dtsDhs.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/dtsDhs.C
//
// PURPOSE:
// Class methods to handle the dhs library startup, init, and errors.
//
// METHOD NAME(S)
// cDtsDhs::cDtsDhs 		- Constructor.
// cDtsDhs::~cDtsDhs 		- Deconstructor.
// cDtsDhs::cleanupTemp		- Cleans up temp.
// cDtsDhs::closeLibs		- Close libraries.
// cDtsDhs::compress		- Read compress keyword from config file.
// cDtsDhs::config		- Read dhs configuration file parameters.
// cDtsDhs::exit		- Get rid of dhs handlers and objects.
// cDtsDhs::init		- Setup handlers and init dhs library.
// cDtsDhs::initLibs		- Initialize libraries.
// cDtsDhs::initPress		- Initialize press library.
// cDtsDhs::kill		- Force and exit.
// cDtsDhs::makeList		- Make a list of files for standalone.
// cDtsDhs::olServerNotify	- Oldp notify.
// cDtsDhs::recursiveLook	- Recursively go through directories.
// cDtsDhs::segregation		- Read segregation information.
// cDtsDhs::servers		- Read servers keyword from config file.
// cDtsDhs::sizeGet		- Get size of files in directories.
// cDtsDhs::ssServerNotify	- Notify the storage server.
// cDtsDhs::start		- Start the event loop.
// cDtsDhsConnect::error		- Connection handler.
// cDtsDhsError::error 		- Error handler.
// 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:00  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.20  2001/02/26 21:59:34  dunn
// Added the determination of unique name after making a list in
// standalone mode.
//
// Revision 1.19  2000/07/27 20:09:52  dunn
// Added standalone features.
//
// Revision 1.18  1999/06/16 22:24:22  nhill
// Added code to get the correct file size from the ad tables when
// notifying storage server of new files.
//
// Revision 1.17  1998/06/11 18:08:29  dunn
// Fixed duplicate typedef.
//
// Revision 1.16  1998/06/11 16:38:43  dunn
// Removed duplicate typedef line.
//
// Revision 1.15  1998/05/07 23:34:03  dunn
// Fixed message to storageServerNotify and updateing of oldp notify flag.
//
// Revision 1.14  1998/05/04 22:59:44  dunn
// Status class changes and error class changes.
//
// Revision 1.13  1997/12/04 21:14:09  cockayne
// Added resetting of status after clearing out staging space.
//
// Revision 1.12  1997/12/03 17:49:58  cockayne
// Modifications made to template instantiations to make Rational
// Rose analyzer happy.
//
// Revision 1.11  1997/10/17 20:46:38  dunn
// Added handling of all put data types.
//
// Revision 1.10  1997/10/03 23:01:29  dunn
// Fixes to handle new stl lib.  tableInfo iterator defined at the top.
//
// Revision 1.9  1997/09/29 21:19:21  dunn
// Removed status server close and replaced the cSta disconnect.
//
// Revision 1.8  1997/09/17 22:12:56  dunn
// Code cleanup.
//
// Revision 1.7  1997/09/05 19:53:08  dunn
// Added the archive notify from ss command.
//
// Revision 1.6  1997/08/29 23:01:21  dunn
// Added force kill, and added name functions.
//
// Revision 1.5  1997/08/19 22:30:20  dunn
// Various fixes.
//
// Revision 1.4  1997/07/29 22:01:18  dunn
// Fixed it if dhsConnectInfo did not return name and address.
//
// Revision 1.3  1997/07/24 16:25:18  dunn
// Made disconnections more robust.
//
// Revision 1.2  1997/07/23 19:41:06  dunn
// Commented out the fh library initialization.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//



#include <map>
#include <list>
#include <string>
extern "C"
{
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "gen_eptr.h"
#include "press.h"
#include "sf.h"

#include "sys/types.h"
#include "unistd.h"
}

#include "globals.H"

#if defined(SYBASE_DHS)
#include "fh.H"
#endif

#include "dtsDhs.H"
#include "dbm.H"
#include "list.H"
#include "get.H"
#include "put.H"
#include "cmd.H"
#include "store.H"
#include "dhsGenConnect.H"
#include "con.H"
#include "seq.H"


cDhsConnection	cDtsDhs::dtsStSrConnect;	// Status Server connection.

bool		cDtsDhs::sdReady = false;	// DTS is not initialized
bool		cDtsDhs::sdRetry = false;
bool		cDtsDhs::sdSimulate = false;

char		*cDtsDhs::sdCompExt = NULL;	// Name for compress. ext.
char		*cDtsDhs::sdCompType = NULL;	// Name for compress. type.
int		cDtsDhs::sdNumIncompleteDs = 0;	// Num of incomp. ds.
char		cDtsDhs::sdNumIncompStr[5];	// Name for num incomp. ds.
char		*cDtsDhs::sdOldpServer = NULL;	// Name for the oldp server.
char		*cDtsDhs::sdQlServer = NULL;	// Name for quick look server.
char		*cDtsDhs::sdSegDir = NULL;	// Name for archive seg dir.
char		*cDtsDhs::sdStatServer = NULL;	// Name for status server.
char		*cDtsDhs::sdStorServer = NULL;	// Name for storage server.


static void	delHandler( cDhsHandler *handler )
{
    delete handler;
}

extern	char 	*version_long;


//
// Typedefs. To allow Rational Rose analyzer to 'see' instantiations. 
//

typedef		std::map< std::string,
	    	cDtsDatasetTable *,
	    	std::less<std::string> >		tTableList;

typedef	cDhsCmdHandler<cDhsAbortCmd>		cDhsAbortCmdHandler;
typedef	cDhsCmdHandler<cDtsDhsArchive>		cDtsDhsArchiveHandler;
typedef	cDhsCmdHandler<cDtsDhsCmdRequest>	cDtsDhsCmdRequestHandler;
typedef	cDhsCmdHandler<cDtsDhsControl>		cDtsDhsControlHandler;
typedef	cDhsCmdHandler<cDtsDhsDelete>		cDtsDhsDeleteHandler;
typedef	cDhsCmdHandler<cDtsDhsSequence>		cDtsDhsSequenceHandler;
typedef	cDhsCmdHandler<cDtsDhsDsInfo>	 	cDtsDhsDsInfoHandler;
typedef	cDhsErrorHandler<cDtsDhsConnect>	cDtsDhsConnectHandler;
typedef	cDhsErrorHandler<cDtsDhsError>		cDtsDhsErrorHandler;
typedef	cDhsGetHandler<cDtsDhsGet>		cDtsDhsGetHandler;
typedef	cDhsPutHandler<cDtsPutDs>		cDtsPutDsHandler;
typedef	cDhsPutHandler<cDtsPutDsQl>		cDtsPutDsQlHandler;
typedef	cDhsPutHandler<cDtsPutFits>		cDtsPutFitsHandler;
typedef	cDhsPutHandler<cDtsPutRaw>		cDtsPutRawHandler;


//
// Configuration keyword defines.
//

#define DTS_KEYWORD_COMPRESS	"compress"
#define DTS_KEYWORD_SERVERS	"servers"
#define DTS_KEYWORD_SEGREGATION	"segregation"
#define DTS_FILE_OVERHEAD	2
#define DTS_BLOCK_SIZE		2048

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::cDtsDhs
//
// INVOCATION:
// n.a.  Constructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cDtsDhs class
//
// DESCRIPTION:
// Initialize the data members.
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

		cDtsDhs::cDtsDhs
(
    void
)
{
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::~cDtsDhs
//
// INVOCATION:
// n.a.  Deconstructor
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for the cDtsDhs class
//
// DESCRIPTION:
// Delete the dhs library object.
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

		cDtsDhs::~cDtsDhs
(
    void
)
{
    if ( sdPDhs != NULL )
    {
	delete (cDhs *) sdPDhs;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::cleanupTemp
//
// INVOCATION:
// cleanupTemp
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Functions return value.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// Cleanup the temp area before continuing
//
// DESCRIPTION:
// Cleanup the temp staging area before continuing.
// Do nothing for standalone mode.
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

void		cDtsDhs::cleanupTemp
(
    cDtsStatus	&status			// (mod) Function return status.
)
{
    char		filePath[PATH_MAX];
    cDtsDatasetTable	*pTbl;			// Pointer to tbl info.
    tTableList		 tableInfo;		// Provides access to containr
    tTableList::iterator i;			// Provides iterator to containr




    checkStat( status, return );


    //
    //  Just return if cleanup is turned off.
    //  OR in standalone mode.  Standalone mode is 
    //  cleanuped up in sizeGet function.
    //

    if ( !status.cleanupOn() || status.standAlone() )
    {
	return;
    }


    //
    //  Cycle thru the process table and get a list of datasets
    //  that require recovering.
    //

    cDtsDatasetTable::fileListGet( status, 'R', tableInfo );


    //
    //  For each one in the list, recover.
    //

    for ( i = tableInfo.begin(); i != tableInfo.end(); i = tableInfo.begin() )
    {
	
	pTbl = (*i).second;


	//
	//  Compose dir name, recover that dataset, and delete from list.
	//

	sprintf( filePath, "%s/%s/", cDtsStoreManager::tempPath(), 
		pTbl->dstDatasetName );

	pTbl->dstReceived = DTS_DS_ALL_RECEIVED;
	cDtsPutDs::datasetRecovery( status, (char *) filePath, pTbl );

	delete (*i).second;
	tableInfo.erase(i);

    }

    
    //
    //  For all files with lifetime TEMPORARY, delete from temp and tbls.
    //

    cDtsDatasetTable::fileListGet( status, 'T', tableInfo );

    for ( i = tableInfo.begin(); i != tableInfo.end();
	    i = tableInfo.begin() )
    {
	
	pTbl = (*i).second;


	//
	//  Print a warning that we are about to delete this dataset.
	//

	status.S_DEL_TEMP( status, pTbl->dstDatasetName );


	//
	//  Make Unretrievable. Remove from the process table.
	//  Will get a warning for non-retrievable files.
	//

	cDtsDbManager::makeUnretrievable( status, pTbl->dstDatasetName, 
	    pTbl->dstLifetime, TRUE );
	cDtsDatasetTable::fileDel( status, pTbl->dstDatasetName );

	delete (*i).second;
	tableInfo.erase(i);


	//
	// Reset status to Success. 
	//
	
	status.status( status.S_SUCCESS );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::closeLibs
//
// INVOCATION:
// cDtsDhs::closeLibs( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Close the all libraries
//
// DESCRIPTION:
// Close the all libraries.
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

void		cDtsDhs::closeLibs
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
#if defined(SYBASE_DHS)
    cHdrParser::hStatus	hstatus;		// Status for the fits header lib.
#endif



    //
    //  Close the fits header library.
    //

    if ( status.parseOn() )
    {
#if defined(SYBASE_DHS)
	cHdrParser::close( hstatus );
	if ( !hstatus.ok() )
	{
	    status.E_HEADER_INIT( status, hstatus.message() );
	}
#else
   status.E_DB( status, "cDtsDhs::closeLibs - no SYBASE support: try -noParse?" );
   return;
#endif
    }


    //
    //  Close the db and ad library and connections.
    //

    if ( !status.standAlone() )
    {
	cDtsDbManager::close(); 
    }


    //
    // Disconnect to the status server.
    //

    cStaStatus::disconnect( status );
    cDtsConManager::disconnectTo( status, sdStatServer );


    //
    //  Delete both the unique and dataset list.
    //

    cDtsUniqueList::exit( status );
    cDtsDatasetList::exit( status );


    //
    // Remove the Connection library.
    //

    cDtsConManager::close();

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::compress
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
// Read the compression information from the config file and store it
//
// DESCRIPTION:
// Save the compression extension and string.
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

boolean		cDtsDhs::compress
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cDtsStatus	status;		// Status code.

    if ( numTokens == 3 )
    {
	if ( sdCompExt != NULL )
	{
	    free( sdCompExt );
	}
	checkNull( sdCompExt = (char *) strsav( tokens[1] ), status, 
		return( FALSE ) );
	if ( sdCompType != NULL )
	{
	    free( sdCompType );
	}
	checkNull( sdCompType = (char *) strsav( tokens[2] ), status, 
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	status.display();
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::config
//
// INVOCATION:
// cDtsDhs::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the dhs library configuration parameters
//
// DESCRIPTION:
// Read in the identity keyword info and then the imp keyword info.
// And finally the server identity keyword information.
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

void		cDtsDhs::config
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
#if defined(SYBASE_DHS)
    cHdrParser::hStatus	hstatus;
#endif


    checkStat( status, return );


    checkConfig( config_get( DTS_KEYWORD_COMPRESS, compress ), status, return );
    checkConfig( config_get( DTS_KEYWORD_SERVERS, servers ), status, return );
    checkConfig( config_get( DTS_KEYWORD_SEGREGATION, segregation ), 
	    status, return );

    //
    //  Read the fits table name.
    //

    if ( status.parseOn() )
    {
#if defined(SYBASE_DHS)
	cHdrParser::config( hstatus );
	if ( !hstatus.ok() )
	{
	    status.E_HEADER_CONFIG( status, hstatus.message() );
	}
#else
   status.E_DB( status, "cDtsDhs::config - no SYBASE support: try -noParse?" );
#endif
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::exit
//
// INVOCATION:
// cDtsDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of all DHS handlers and the cDhs object
//
// DESCRIPTION:
// For each handler attached, delete them.  Then delete the dhs library
// object and set the pointer to null.
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
void		cDtsDhs::exit
(
    cDtsStatus	&status		// (mod) Function return status.
)
{

    checkStat( status, return );

    if ( sdPDhs != NULL )
    {
	cDhsHandler::forEach( delHandler );

	delete sdPDhs;
	sdPDhs = NULL;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::init
//
// INVOCATION:
// cDtsDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the dhs library
//
// DESCRIPTION:
// Set debug level, set up the dhs object, setup the handlers.
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

void		cDtsDhs::init
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    sdRetry = false;
    checkStat( status, return );


    //
    //  Set the debug level
    //

    switch ( cDtsStatus::defDhsDebug() )
    {
	case	cDtsStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cDtsStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cDtsStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }


    //
    //  Set up the dhs object.
    //

    checkNull( sdPDhs = new cDhs( level ), status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDtsDhsErrorHandler, status, return );
    checkNull( new cDtsDhsConnectHandler( DHS_S_CONNECT ), 
	    status, return );
    checkNull( new cDtsDhsConnectHandler( DHS_S_DISCONNECT ), 
	    status, return );


    //
    // Set up the command handler.
    //

    checkNull( new cDtsDhsCmdRequestHandler( true ), 
	    status, return );
    checkNull( new cDtsDhsControlHandler ( true, DTS_CTL_DEFINITION ), 
	    status, return );
    checkNull( new cDtsDhsDeleteHandler( true, DTS_CMD_DELETE ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_TEST_CMD ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_INIT_CMD ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_EXIT_CMD ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_RESET_CMD ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_RESET_HEALTH_CMD ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_DEBUG_CMD ), 
	    status, return );
    checkNull( new cDtsDhsSequenceHandler( false, DTS_SIM_CMD ),
	    status, return );

    //
    //  Don't bother adding these calls backs if running in standalone.
    //  These cmds are invalid in standalone: Archive notify, Storage
    //  server start cmd, oldp start command.
    //

    if ( !status.standAlone() )
    {
	checkNull( new cDtsDhsArchiveHandler( true, DTS_CMD_ARC_NOTIFY ), 
		status, return );
	checkNull( new cDtsDhsCmdRequestHandler( true, DTS_SS_START_CMD ), 
		status, return );
	checkNull( new cDtsDhsCmdRequestHandler( true, DTS_OLDP_START_CMD ), 
		status, return );
    }

    checkNull( new cDtsDhsSequenceHandler( false, DTS_PING_CMD ),
	    status, return );
    checkNull( new cDtsDhsDsInfoHandler( true, DTS_CMD_DS_INFO ), 
	    status, return );


    //
    //  Implement the abort command for tests only.
    //

    checkNull( new cDhsAbortCmdHandler( false, DTS_ABORT_TEST_CMD ),
	    status, return );


    //
    // Set up the put handlers.
    //

    checkNull( new cDtsPutRawHandler( true, DHS_BD_PT_RAW ), 
	    status, return ); 
    checkNull( new cDtsPutFitsHandler( true, DHS_BD_PT_FITS ), 
	    status, return ); 
    checkNull( new cDtsPutRawHandler( true, DHS_BD_PT_RAW_UNIQUE ), 
	    status, return ); 
    checkNull( new cDtsPutFitsHandler( true, DHS_BD_PT_FITS_UNIQUE ), 
	    status, return ); 
    checkNull( new cDtsPutDsHandler( true, DHS_BD_PT_DS ), 
	    status, return ); 
    checkNull( new cDtsPutDsQlHandler( true, DHS_BD_PT_DS_QL ), 
	    status, return ); 
    checkNull( new cDtsPutRawHandler( true, DHS_BD_PT_UNKNOWN ),
	    status, return ); 
    checkNull( new cDtsPutRawHandler( true, DHS_BD_PT_SDS),
	    status, return ); 


    //
    // Set up the get handlers.
    //

   checkNull( new cDtsDhsGetHandler( true ), status, return ); 

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::initLibs
//
// INVOCATION:
// cDtsDhs::initLibs( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the all libraries
//
// DESCRIPTION:
// Initialize the all libraries.
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

void		cDtsDhs::initLibs
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char	*statIpAddr;		// Status server ip address.
    cDtsConManager::clStatus
    		cStatus;		// Connection class status.
#if defined(SYBASE_DHS)
    cHdrParser::hStatus	hstatus;		// Status for the fits header lib.
#endif
    SF_STATUS	sfStatus;		// Status for sf library.


    checkStat( status, return );


    //
    // Initialize the Connection library.
    //

    cDtsConManager::init( status.debugLevel(), cStatus );
    if ( !cStatus.ok() )
    {
	status.E_CONN_LIST( status,  "cDtsConManager::init", 
		cStatus.message() );
	return;
    }


    //
    // Lookup the status server ip address and connect to the status server.
    // Then init. the library.
    //

    if ( (statIpAddr = cDtsConManager::lookup( sdStatServer ) ) == NULL )
    {
	status.E_SS_CONNECT_INFO( status );
	return;
    } 
    
    dtsStSrConnect.open( statIpAddr, sdStatServer, dhsStatus);
    if ( dhsStatus != DHS_S_SUCCESS )
    {
	//
	// Open to Status Server failed.
	//

	const char *msg;
	DHS_STATUS  s( DHS_S_SUCCESS ); 
	status.E_SS_CONNECT_ERROR( status );
	while ( s == DHS_S_SUCCESS )
	{
	    msg = cDhs::message( s );
	    if ( s == DHS_S_SUCCESS )
	    {
		status += msg;
		cDhs::messageClear( s );
	    }
	}
	status.display();
	return;
    }


    checkDhs( dtsStSrConnect.open( statIpAddr, sdStatServer, dhsStatus), 
	    dhsStatus, status, return );
	
    check( cStaStatus::init( dtsStSrConnect, PREFIX, status ), status, return );


    //
    //  Log the startup and connecting to status server.
    //

    status.S_STARTUP( status, APPLICATION, version_long );
    status.S_CONNECTING( status, sdStatServer, statIpAddr );


    //
    //  Update the status server with the values.
    //

    cStaStatus::update( "health", "GOOD", status );
    cStaStatus::update( "healthDesc", "GOOD", status );
    cStaStatus::update( "state", "INITIALIZING", status );
    cStaStatus::update( "debug", status.debugString(), status );
    cStaStatus::update( "simulate", cDtsDhs::simulateString(), status );
    cStaStatus::update( "numIncomplete", cDtsDhs::numIncomp(), status );
    cStaStatus::flush( status );


    //
    //  Initialize the db and ad library and connections.
    //

    cDtsDbManager::init( status );


    //
    //  Initialize the press library.
    //

    initPress( status );
    checkStat( status, return );


    //
    //  Initialize the fits header library.
    //

    if ( status.parseOn() )
    {
#if defined(SYBASE_DHS)
	cHdrParser::init( hstatus, cDtsDbManager::storeDb(),
		cDtsDbManager::serverName() );
	if ( !hstatus.ok() )
	{
	    status.E_HEADER_INIT( status, hstatus.message() );
	    return;
	}
#else
   status.E_DB( status, "cDtsDhs::initLibs - no SYBASE support: try -noParse?" );
   return;
#endif
    }


    //
    //  Initialize the sf library.
    //

    sfStatus = SF_S_SUCCESS;
    checkSf( sfInit( &sfStatus ), sfStatus, status, return );



    //
    //  Initialize the disk space management.
    //

    cDtsStoreManager::init( status );


    //
    //  Compose the unique name prefix.
    //

    cDtsUniqueList::composeUName( status );


    //
    //  Start the label purge event loop.
    //

    cDtsUniqueList::init( status );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::initPress
//
// INVOCATION:
// cDtsDhs::initPress( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize the compression library
//
// DESCRIPTION:
// Initialize the compression library.
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

void		cDtsDhs::initPress
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Check that it is a known compress type string.
    //

    if ( streq( compressType(), PR_GZIP ) )
    {
	//
	//  Gzip
	//

	if ( ( press_setopt( PR_GZIP, FALSE, 1 ) ) != PR_SUCCESS )
	{							
	    status.E_PRESS_LIB( status, pr_msg );	
	}				
    }
    else if ( streq( compressType(), PR_HCOMP ) )
    {
	//
	// H compress
	//

	if ( (press_setopt( PR_HCOMP,  FALSE, FALSE, 0, "fits", 0, 0 ) )
		!= PR_SUCCESS )
	{							
	    status.E_PRESS_LIB( status, pr_msg );	
	}				
    }
    else if ( streq( compressType(), PR_UNIX ) )
    {
	//
	// Unix compress
	//

	if ( ( press_setopt( PR_UNIX, FALSE ) ) != PR_SUCCESS )
	{							
	    status.E_PRESS_LIB( status, pr_msg );	
	}				
    }
    else
    {
	//
	// Error in compress type.
	//
	
	status.E_COMP_TYPE( status, compressType() );

    }
    status.S_COMP_TYPE( status, compressType() );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::kill
//
// INVOCATION:
// cDtsDhs::kill( status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of the cDhs object
//
// DESCRIPTION:
// Delete the dhs library object and set the pointer to null.
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
void		cDtsDhs::kill
(
    void
)
{
    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::makeList
//
// INVOCATION:
// makeList
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Functions return value.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// Make internal list based on files in temp and perm.
//
// DESCRIPTION:
// Make internal list based on files in temp and perm.
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

void		cDtsDhs::makeList
(
    cDtsStatus	&status			// (mod) Function return status.
)
{
    tTableList		 tableInfo;		// Provides access to containr
    tTableList::iterator i;			// Provides iterator to containr




    checkStat( status, return );



    //
    //  Just return if not running standalone.
    //

    if ( !status.standAlone() )
    {
	return;
    }

    
    //
    //  For all files in temp & perm, add them to the list, or delete them.
    //

    recursiveLook( status, cDtsStoreManager::tempPath(), 1 );
    recursiveLook( status, cDtsStoreManager::permPath(), 0 );


    //
    //  Now, reset the unique name count now that we have all the
    //  files in the temp and perm path in our hands.
    //

    cDtsUniqueList::composeUName( status );

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::recursiveLook
//
// INVOCATION:
// recursiveLook
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Functions return value.
// (>) path (char *) Path to look in.
// (>) type (int) Storage type, PERM=2, or TEMP=1
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// Make internal list based on files in temp and perm.
//
// DESCRIPTION:
// Make internal list based on files in temp and perm.
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

void		cDtsDhs::recursiveLook
(
    cDtsStatus	&status,	// (mod) Function return status.
    char	*path,		// (in)  The path to check.
    int		type		// (in)  TEMP or PERM.
)
{
    struct dirent *dirent;
    DIR         *dirp;
    char        new_path[PATH_MAX];


    checkStat( status, return );


    //
    // Open the directory
    //

    if ( ( dirp = opendir( path ) ) == NULL )
    {
	//
	//  Errror, directory doesn't exist.
	// 
	
	status.E_DIR_GONE( status, path );
	return;
    }


    //
    //  Cycle thru all files below that directory.
    //

    for ( dirent = readdir( dirp ); dirent != NULL;
            dirent = readdir( dirp ) )
    {
        if ( streq( dirent->d_name, "." ) ||
                streq( dirent->d_name, ".." ) )
        {
            continue;
        }


	//
	//  Create a path to this dir or file that we have found.
	//  Then call sizeGet to determine what it it.
	//

        (void) sprintf( new_path, "%s/%s", path, dirent->d_name );

	sizeGet( status, new_path, dirent->d_name, type );

    }

    if ( closedir( dirp ) != 0 )
    {
	status.E_DIR_CLOSE( status, path );
	return;
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::sizeGet
//
// INVOCATION:
// sizeGet
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Functions return value.
// (>) path (char *) Path to look in.
// (>) name (char *) Name of the file.
// (>) type (int) Storage type, PERM=2, or TEMP=1
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// Make internal list based on files in temp and perm
//
// DESCRIPTION:
// Make internal list based on files in temp and perm.
// Recursively called.
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

void		cDtsDhs::sizeGet
(
    cDtsStatus	&status,	// (mod) Function return status.
    char        *path,          // (in)  The path to check.
    char        *name,          // (in)  The name of the file or dir.
    int		type		// (in)  Type of size get.
)   
{
    struct stat stat_buf;	// Stat structure for stat of file.
    char	*ukName;	// Unique name.



    //
    //  Do a stat on the path passed in to determine type.
    //

    if ( stat( path, &stat_buf )  < 0 )
    {
	status.E_STAT_FILE( status, path );
	return;
    }


    //
    // If its a directory, then recursively go down to get the
    // files that are in it.
    //

    if ( S_ISDIR( stat_buf.st_mode ) )
    {
        //
        //  This is a directory so use recursiveLook to
	// go down further.
        //

        recursiveLook( status, path, type );
    }
    else if ( S_ISREG( stat_buf.st_mode ) )
    {
	//
	//  If this is temp && cleanup is On, then delete the file, 
	//  otherwise we will add the file to the dataset list.
	//

	if ( type == 1 && status.cleanupOn() )
	{
	    //
	    //  Print a warning that we are about to delete this dataset.
	    //

	    status.S_DEL_TEMP( status, path );
	    cDtsStoreManager::fileRemove( status, path, 
	    		cDtsStoreManager::ST_TEMP, stat_buf.st_size );
	}
	else
	{
	    //
	    //  Otherwise we are going to add this file to the dataset list.
	    //

	    cDtsDatasetTable	*pTableInfo = NULL;
						// Local table dataset info.
	    cDtsDatasetList	*pDsList;	// Ptr to new object.
	    char	 	format;		// Format of the file.
	    char		rootName[128];	// Root of the datasetname.
	    char		comp;		// Compression type.


	    //
	    //  First get the root of the dataset name, compression type and 
	    //  format.  The root will be the datasetName (without any
	    //  extensions added).
	    //

	    (void)cDtsDbManager::nameInfo( status, name, rootName, 
		    &format, &comp );
		

	    //
	    //  Look for the dataset in the list.
	    //  

	    pDsList =  cDtsDatasetList::datasetNameFind( status, 
		    rootName );

	    if ( status == status.S_NOT_IN_DS_LIST )
	    {
		//
		//  Added it to unique list, create a datasettable entry,
		//  then add it to the datasetList.
		//

		cDtsUniqueList::uniqueNameVerify( status, rootName, &ukName );
		pTableInfo = new cDtsDatasetTable( rootName, ukName,
			time(NULL), DTS_DS_ALL_RECEIVED, 
			(type)?DTS_TEMP:DTS_PERM, DTS_HL_HEALTHY,
			DTS_OL_NOTIFIED, DTS_SS_NOTIFIED, format, 
			cDhs::identity() );
		pDsList = new cDtsDatasetList( rootName, path, pTableInfo, 
			format, comp, stat_buf.st_size );
		pDsList->add( status );
		status.S_DS_LIST_ADD( status, rootName );
	    }
	    else
	    {
		//
		//
		// WARNING about neat little data server feature:
		// If you have 2 files with the same dataset name, 
		// but in different directories in the same permanent 
		// tree IT WILL IGNORE THE SECOND FILE WITH THE SAME NAME!!!!!!
		//

		status.S_DS_LIST_NOADD( status, rootName );
	    }
	}

    }
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::olServerNotify
//
// INVOCATION:
// olServerNotify
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is to notify the storage server of any notifications
//
// DESCRIPTION:
// This function is to notify the storage server of any notifications
// that haven't been sent.
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

void cDtsDhs::olServerNotify
(
    cDtsStatus	&status 		// (mod) Status passed in.
)
{
    cDtsDatasetTable	*pTbl;			// Ptr to table struct.
    tTableList		 tableInfo;		// Provides access to containr
    tTableList::iterator i;			// Provides iterator to containr


    checkStat( status, return );


    //
    //  Just return if the data server has been started with no servers,
    //  or in simulate mode.
    //

    if ( !status.serversOn() || status.standAlone() || cDtsDhs::simulate()  )
    {
	return;
    }


    //
    //  Cycle thru the process table and get a list of datasets
    //  that require OLDP notification.
    //

    cDtsDatasetTable::fileListGet( status, 'O', tableInfo );


    //
    //  For each one in the list, notify ss.
    //

    for ( i = tableInfo.begin(); i != tableInfo.end(); i = tableInfo.begin() )
    {
	
	pTbl = (*i).second;

	pTbl->getAdFileInfo( status );
	cDtsDhsPut::oldpServerNotify( status, pTbl, "n/a", "n/a" );
	cDtsDatasetList::updateStatus( status, pTbl->dstDatasetName, pTbl );

	delete (*i).second;
	tableInfo.erase(i);
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::segregation
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
// Read the segregation line from a configuration file and store it
//
// DESCRIPTION:
// Read in the segregation directory name.
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

boolean		cDtsDhs::segregation
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cDtsStatus	status;		// Status code.

    if ( numTokens == 2 )
    {
	//
	//  Read in segregation directory.
	//

	if ( sdSegDir != NULL )
	{
	    free( sdSegDir  );
	}
	checkNull( sdSegDir  = (char *) strsav( tokens[1] ), status, 
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
// cDtsDhs::servers
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
// Read the servers line from a configuration file and store it
//
// DESCRIPTION:
// Read in the identity strings of all servers needed.
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

boolean		cDtsDhs::servers
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cDtsStatus	status;		// Status code.

    if ( numTokens == 5 )
    {
	//
	//  Read in identity for the storage server.
	//

	if ( sdStorServer!= NULL )
	{
	    free( sdStorServer );
	}
	checkNull( sdStorServer = (char *) strsav( tokens[1] ), status, 
		return( FALSE ) );


	//
	//  Read in identity for the oldp server.
	//

	if ( sdOldpServer != NULL )
	{
	    free( sdOldpServer );
	}
	checkNull( sdOldpServer = (char *) strsav( tokens[2] ), status, 
		return( FALSE ) );


	//
	//  Read in identity for the status server.
	//

	if ( sdStatServer!= NULL )
	{
	    free( sdStatServer );
	}
	checkNull( sdStatServer = (char *) strsav( tokens[3] ), status, 
		return( FALSE ) );


	//
	//  Read in identity for the quick look server.
	//

	if ( sdQlServer!= NULL )
	{
	    free( sdQlServer );
	}
	checkNull( sdQlServer = (char *) strsav( tokens[4] ), status, 
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
// cDtsDhs::ssServerNotify
//
// INVOCATION:
// ssServerNotify
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is to notify the oldp server of any notifications
//
// DESCRIPTION:
// This function is to notify the oldp server of any notifications
// that haven't been sent.
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

void cDtsDhs::ssServerNotify
(
    cDtsStatus	&status 		// (mod) Status passed in.
)
{
    cDtsDatasetTable	*pTbl;			// Ptr to table struct.
    tTableList		 tableInfo;		// Provides access to containr
    tTableList::iterator i;			// Provides iterator to containr


    checkStat( status, return );


    //
    //  Just return if the data server has been started with no servers,
    //  or in simulate mode.
    //

    if ( !status.serversOn() || status.standAlone() || cDtsDhs::simulate()  )
    {
	return;
    }


    //
    //  Cycle thru the process table and get a list of datasets
    //  that require notification at the storage server.
    //

    cDtsDatasetTable::fileListGet( status, 'S', tableInfo );


    //
    //  For each one in the list, notify ss.
    //


    for ( i = tableInfo.begin(); i != tableInfo.end(); i = tableInfo.begin() )
    {
	
	pTbl = (*i).second;
	pTbl->getAdFileInfo( status );

	cDtsDhsPut::storageServerNotify( status, pTbl, cDhs::identity(), 
		"n/a", cDtsDhs::segDir() );

	cDtsDatasetList::updateStatus( status, pTbl->dstDatasetName, pTbl );

	delete (*i).second;
	tableInfo.erase(i);
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the DHS library event loop
//
// DESCRIPTION:
// Check the status, set the state and then start the event loop.
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

void		cDtsDhs::start
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );

    //
    //  Update state on status server to running.
    //

    cStaStatus::update( "state", "RUNNING", status );
    cStaStatus::flush( status );


    //
    //  Start the event loop.
    //

    status.S_RUNNING( status );
    checkDhs( sdPDhs->start( dhsStatus ), dhsStatus, status, return );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsConnect::error
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when a connection/disconnection request is received by the DHS library
//
// DESCRIPTION:
// Log the connection or disconnection.
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

void		cDtsDhsConnect::error
(
    void
)
{
    char	*address = NULL;
    					// Ip address of the client.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
					// DHS function return status.
    char	*clientName = NULL;
    					// Name of the client.
    cDtsStatus	lstatus;



    //
    //  Get information about connector/disconnector.
    //

    dhsConnectInfo( connect(), &address, &clientName, &dhsStatus );

    if ( status() == DHS_S_CONNECT )
    {
	//
	//  This is a connection.  Make sure they are an authorized connector.
	//

	//
	//  Check that the dataserver is ready to receive connects.
	//
	
	if ( !cDtsDhs::ready() )
	{
	    dhsStatus = DHS_S_SUCCESS;
	    dhsDisconnect( connect(), &dhsStatus );
	    lstatus.S_NOT_READY( lstatus, clientName, address );
	}
	else if ( dhsStatus != DHS_S_SUCCESS || 
		!cDtsConManager::authorized( clientName ) )
	{
	    //
	    //  Not an autorized connector, disconnect them.
	    //

	    dhsStatus = DHS_S_SUCCESS;
	    dhsDisconnect( connect(), &dhsStatus );

	    if ( connect() != cDtsDhs::dtsStSrConnect.dhsConnect() )
	    {
		//
		// Not the status server disconnecting, log it.
		//

		lstatus.S_BAD_CONNECTION( lstatus, clientName, address );
	    }
	    else
	    {
		//
		//  Log to stdout, cause its the status server.
		//

		lstatus.S_BAD_CONNECTION( lstatus, clientName, address );
	    }
	}
	else
	{
	    //
	    //  Authorized user, print it out.
	    //

	    lstatus.S_CONNECT_REQ( lstatus, clientName, address );
	}
    }
    else
    {
	//
	//  Else its a disconnect, set flags in connect list if possible.
	//

	if ( connect() != 0 )
	{
	    cDtsConManager::disconnectTo( lstatus, clientName );
	    if ( connect() != cDtsDhs::dtsStSrConnect.dhsConnect() )
	    {
		lstatus.S_DISCONNECT_REQ( lstatus, clientName, address );
	    }
	    else
	    {
		lstatus.S_DISCONNECT_REQ( lstatus,  clientName, address );
	    }
	}
    } 
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDhsError::error
//
// INVOCATION:
// n.a. Called automatically by the DHS++ library.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Called when an error occurs in the DHS library
//
// DESCRIPTION:
// Print the error messages.
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

void		cDtsDhsError::error
(
    void
)
{
    cDtsStatus		dtsStatus;
    cDtsStatus		lstatus;	//  Connection status.


    //
    //  If its a debug message, print to stdout.
    //

    if ( level() == DHS_EL_DEBUG || level() == DHS_EL_INFO )
    {
	dtsStatus.S_DEBUG_MSG( dtsStatus, message() );
    }
    else
    {
	//
	//  Check to see who its from.
	//

	char		*address = NULL;	// Ip address of the client.
	DHS_STATUS	dhsStatus( DHS_S_SUCCESS );		
						// DHS function return status.
	char		*clientName = NULL;	// Name of the client.

	dhsConnectInfo( connect(), &address, &clientName, &dhsStatus );

	//
	//  If its a lost connection (ignore DHS_S_SHUTDOWN), check
	//  to see if its the status server and if so exit, otherwise
	//  make sure you remove them from your connection list.
	//

	if ( status() == DHS_E_CON_LOST )
	{
	    //
	    //  This is a lost connection, check to see if its the 
	    //  status server.
	    //

	    if ( connect() == cDtsDhs::dtsStSrConnect.dhsConnect() )
	    {
		//
		//  Exit gracefully.  Set retry to false, log and stop.
		//

		dtsStatus.S_LOST_CONN( dtsStatus, clientName, address, ", EXITTING!!");
		cStaStatus::disconnect( lstatus );
		cDtsDhs::retry( false );
		cDtsDhs::ready( lstatus, false );
		cDhs::stop( dhsStatus );
	    }
	    else
	    {
		//
		// Remove this user from your connection list.
		//

		dtsStatus.S_LOST_CONN( dtsStatus, clientName, address, " ");
		cDtsConManager::disconnectTo( lstatus, clientName );
	    }
	}
	else
	{
	    //
	    //  Else is it just some type of error, print it out.
	    //

	    dtsStatus.E_MSG( dtsStatus, message() );
	}
    }
}
