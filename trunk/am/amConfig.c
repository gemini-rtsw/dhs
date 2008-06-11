/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	am/src/amConfig.c
*
*   Purpose:
*	Read the configuration file for the am library.
*
*   Routines:
*	static int	addDataStage	: Add datastage areas.
*	static int	addDefaultPrec	: Add the default precision.
*	static int	addDeviceTables	: Add device tables.
*	static int	addIngestScript	: Add ingest script name.
*	static int	addMediaDatabase
*					: Add media db info.
*	static boolean  addMediaScripts	: Parses media scripts config info.
*	int		amConfigInit	: Read the configuration file.
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: amConfig.c
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 02/03/00
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/09/05 JSD  :	Added addDeviceTables and addMediaScripts.
*	98/10/27 SDJ  :	Moved mediaTypeCmp to compare.c and fixed bugs
*			in the eptr list handling.
*	98/11/23 JSD  :	Added reading in mediaTypes tbl name.
*			Removed addMediaType - all in db tbl now.
*			Added addIngestScript and optionally get it.
*	98/12/06 SDJ  : Added addDefualtPrec().
*	99/04/28 NRH  : Added media_actions to the list of database tables
*			in the configuration file.
*     2000/02/03 SDJ  : Added media_options table to the list of database
*			tables in the config. file.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/
 
#include <sybfront.h>
#include <sybdb.h>

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "db.h"

#include "am.h"
#include "localAm.h"

#define	KEY_ASTO_MED_DB		"amDb"
#define	KEY_DATASTAGE		"dataStage"
#define KEY_DEFAULT_PREC	"defaultPrecision" 
#define	KEY_DEVICE		"deviceTables"
#define	KEY_INGEST		"ingestScript"
#define	KEY_SCRIPTS		"scripts"


static boolean	addDataStage( int, char ** );
static boolean	addDefaultPrec( int, char ** );
static boolean	addDeviceTables( int, char ** );
static boolean	addIngestScript( int, char ** );
static boolean	addMediaDatabase( int, char ** );
static boolean	addMediaScripts( int, char ** );

/*+
************************************************************************
*
*   Function:	addDataStage
*
*   Purpose:
*	Reads the Data Stage configuration information from the 
*	configuration file. 
*
*   Values Returned:
*	boolean	TRUE		: Normal completion
*	boolean	FALSE		: Something went wrong.
*
************************************************************************
-*/

static boolean	addDataStage
(
    int		numTokens,	/* (in)  The number of arguments.	*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{

    if ( numTokens != 4 )
    {
	return( FALSE );
    }

    /*
     *  Parse the User Data Stage path.
     */

    if ( (int) strlen( tokens[1] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laUserDataStage = strsav( tokens[1] ) );
  

    /*
     *  Parse the Archive Data Stage path.
     */

    if ( (int) strlen( tokens[2] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laArchDataStage = strsav( tokens[2] ) );


    /*
     *  Parse the Media Data Stage path.
     */

    if ( (int) strlen( tokens[1] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laMediaDataStage = strsav( tokens[3] ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	addDeviceTables
*
*   Purpose:
*	Reads the Device Table configuration information from the 
*	configuration file. 
*
*   Values Returned:
*	boolean	TRUE		: Normal completion
*	boolean	FALSE		: Something went wrong.
*
************************************************************************
-*/

static boolean	addDefaultPrec
(
    int		numTokens,	/* (in)  The number of arguments.	*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{

    if ( numTokens != 2 )
    {
	return( FALSE );
    }

    /*
     *  Parse the device table name.
     */

    if ( ! str2int( tokens[1], &localAm.laDefaultPrec ) )
    {
	return( FALSE );
    }

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	addDeviceTables
*
*   Purpose:
*	Reads the Device Table configuration information from the 
*	configuration file. 
*
*   Values Returned:
*	boolean	TRUE		: Normal completion
*	boolean	FALSE		: Something went wrong.
*
************************************************************************
-*/

static boolean	addDeviceTables
(
    int		numTokens,	/* (in)  The number of arguments.	*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{

    if ( numTokens != 5 )
    {
	return( FALSE );
    }

    /*
     *  Parse the device table name.
     */

    if ( (int) strlen( tokens[1] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laDeviceTable = strsav( tokens[1] ) );
  

    /*
     *  Parse the booking table name.
     */

    if ( (int) strlen( tokens[2] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laBookingTable = strsav( tokens[2] ) );


    /*
     *  Parse the logicalDevice table name.
     */

    if ( (int) strlen( tokens[3] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laLogicalTable = strsav( tokens[3] ) );



    /*
     *  Parse the mediaTypes table name.
     */

    if ( (int) strlen( tokens[4] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laMedTypesTable = strsav( tokens[4] ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	addIngestScript
*
*   Purpose:
*	Reads the mediaIngest script name from the configuration file. 
*
*   Values Returned:
*	boolean	TRUE		: Normal completion
*	boolean	FALSE		: Something went wrong.
*
************************************************************************
-*/

static boolean	addIngestScript
(
    int		numTokens,	/* (in)  The number of arguments.	*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{

    if ( numTokens != 2 )
    {
	return( FALSE );
    }

    /*
     *  Parse the ingest script name.
     */

    if ( (int) strlen( tokens[1] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( localAm.laIngestScript = strsav( tokens[1] ) );
  

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	addMediaDatabase
*
*   Purpose:
*	Adds the database and table information to the am  
*	library configuration.
*
*   Values Returned:
*	boolean	TRUE		: Data parsed and insered correctly.
*	boolean	FALSE		: Data couldn't be parsed or inserted.
*
************************************************************************
-*/

static boolean	addMediaDatabase
(
    int		numTokens,	/* (in)  Number of tokens on the line.	*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    if ( numTokens != 10 )
    {
	return( FALSE );
    }

    localAm.laServer = strsav( tokens[1] );
    localAm.laDatabase = strsav( tokens[2] );
    localAm.laMediaTable = strsav( tokens[3] );
    localAm.laMedFilesTable = strsav( tokens[4] );
    localAm.laMedUnitsTable = strsav( tokens[5] );
    localAm.laMedStagesTable = strsav( tokens[6] );
    localAm.laStatusTable = strsav( tokens[7] );
    localAm.laMedActionsTable = strsav( tokens[8] );
    localAm.laMedOptionsTable = strsav( tokens[9] );

    if ( localAm.laServer== NULL ||
	    localAm.laDatabase == NULL || 
	    localAm.laMediaTable == NULL ||
	    localAm.laMedFilesTable == NULL ||
	    localAm.laMedUnitsTable == NULL ||
	    localAm.laStatusTable == NULL ||
	    localAm.laMedActionsTable == NULL ||
	    localAm.laMedOptionsTable == NULL )
    {
	amFormatMessage( AM_E_MEMORY );
	return( FALSE );
    }

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	addMediaScripts
*
*   Purpose:
*	Reads the Media Scripts information from the configuration file. 
*	Adds the information to the gMediaScripts EPTR list. 
*
*   Values Returned:
*	boolean	TRUE		: Normal completion
*	boolean	FALSE		: Something went wrong.
*
************************************************************************
-*/

static boolean	addMediaScripts
(
    int		numTokens,	/* (in)  The number of arguments.	*/
    char	**tokens	/* (in)  The list of tokens.		*/
)
{
    char	mediaType[AM_MEDIA_TYPE_LEN];
				/* Name of the type.			*/
    AM_MED_SCRIPT
    		*mediaScriptPtr;	/* Contains info about a media scripts.	*/
    int		index;


    if ( numTokens != 6 )
    {
	return( FALSE );
    }


    /*
     *  Parse the Media type.
     */

    if ( (int) strlen( tokens[1] ) <= 0  ||
	 (int) strlen( tokens[1] ) > AM_MEDIA_TYPE_LEN )
    {
	return( FALSE );
    }
    (void) strcpy( mediaType, tokens[1] );


    /*
     *  Check that media type is valid by checking with everything in the
     *  validMedia list.
     */
    


    /*
     *  Make room for the media type structure and save the type name.
     */

    mediaScriptPtr  = (AM_MED_SCRIPT *) gen_alloc ( sizeof( AM_MED_SCRIPT  ) );
    (void) strcpy( mediaScriptPtr->amsTypeName, mediaType );


    /*
     *  Parse the Preparation Script Name.
     */

    if ( tokens[2] == NULL || (int) strlen( tokens[2] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( mediaScriptPtr->amsPrepScript = strsav( tokens[2] ) );


    /*
     *  Parse the Preparation File Name.
     */

    if ( tokens[3] == NULL || (int) strlen( tokens[3] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( mediaScriptPtr->amsPrepFile = strsav( tokens[3] ) );



    /*
     *  Parse the Write Script Name.
     */

    if ( tokens[4] == NULL || (int) strlen( tokens[4] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( mediaScriptPtr->amsWriteScript =
	    strsav( tokens[4] ) );


    /*
     *  Parse the Write File Name.
     */

    if ( tokens[5] == NULL || (int) strlen( tokens[5] ) <= 0 )
    {
	return( FALSE );
    }
    AM_CHECK_NULL_CONFIG( mediaScriptPtr->amsWriteFile = strsav( tokens[5] ) );


    /*
     *  Find the insertion point for the media type and insert it.
     */

    if ( eptr_search( localAm.laMediaScripts, mediaScriptPtr->amsTypeName,
	    &index, scriptCmp ) )
    {
	return( FALSE );
    }

    AM_CHECK_NULL_CONFIG( eptr_insert( localAm.laMediaScripts, index,
	    mediaScriptPtr ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	amConfigInit
*
*   Purpose:
*	Read configuration file for the am library.
*
*   Values Returned:
*	int	AM_SUCCESS	: normal completion.
*	int	AM_E_CONFIG	: Error reading config file.
*
************************************************************************
-*/

int		amConfigInit
(
    void
)
{
    char	*configFile;
    int		get_status;


    AM_CHECK_CONFIG( config_open( AM_LIBRARY, &configFile ) );

    amFormatMessage( AM_CFILE_OPEN, configFile );


    AM_CHECK_CONFIG( config_get( KEY_DATASTAGE, addDataStage ) );
    AM_CHECK_CONFIG( config_get( KEY_ASTO_MED_DB, addMediaDatabase ) );
    AM_CHECK_CONFIG( config_get( KEY_DEVICE, addDeviceTables ) );

 
    AM_CHECK_NULL( localAm.laMediaScripts = eptr_alloc( 5 ) );
    AM_CHECK_CONFIG( config_get( KEY_SCRIPTS, addMediaScripts ) );
    AM_CHECK_CONFIG( config_get( KEY_DEFAULT_PREC, addDefaultPrec ) );


    /*
     *  Optionally get the mediaIngest script name.
     */

    get_status = config_get( KEY_INGEST, addIngestScript );
    if ( get_status != GC_E_KEY_NOT_FOUND )
    {
	AM_CHECK_CONFIG( get_status );
    }
    msg_clear( gc_msg );



    config_close();

    return( AM_SUCCESS );
}

