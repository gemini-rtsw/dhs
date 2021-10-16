static char rcsid[] = "$Id: list.C,v 1.3 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/list.C
//
// PURPOSE:
// Handles all management of the dataset list.  Current datasets.
//
// METHOD NAME(S)
// cDtsLists::config			Configuration reading.
// cDtsLists::nameInfo			Read in config info about the name.
// cDtsChunkInfo::findChunk		Find info about a file chunk in list.
// cDtsChunkInfo::~cDtsChunkInfo	Deconstructor
// cDtsChunkInfo::cDtsChunkInfo		Constructor
// cDtsDatasetList::cDtsDatasetList	Constructor
// cDtsDatasetList::cDtsDatasetList	Constructor
// cDtsDatasetList::cDtsDatasetList	Constructor
// cDtsDatasetList::~cDtsDatasetList	Deconstructor
// cDtsDatasetList::add 		Add object to the list
// cDtsDatasetList::addDatabase 	Add object info to the database
// cDtsDatasetList::addList		Add object to the object list.
// cDtsDatasetList::addList		Add object to the object list.
// cDtsDatasetList::chunkExits		Verify a chunk of file exists.
// cDtsDatasetList::chunkLastCheck	Check if this is the last chunk expected
// cDtsDatasetList::datasetListReset	Reset info about a dataset
// cDtsDatasetList::datasetListRemove	Remove object from dataset list
// cDtsDatasetList::datasetNameFind	Find a datasetname
// cDtsDatasetList::datasetNameVerify	Verify info about a datasetname
// cDtsDatasetList::datasetNameVerify	Verify info about a datasetname
// cDtsDatasetList::exit		Exit 
// cDtsDatasetList::findNameMax		Find all common name, & get max. number
// cDtsDatasetList::newDatasetName	Create a new dataset name.
// cDtsDatasetList::resetChunkList	Reset the chunk list.
// cDtsDatasetList::setChunkList	Set the chunk list.
// cDtsDatasetList::updateStatus	Update the status of a dataset.
// cDtsDatasetList::updateObject	Update info in list object about a ds.
// cDtsUniqueList::cDtsUniqueList	Constructor
// cDtsUniqueList::~cDtsUniqueList	Deconstructor
// cDtsUniqueList::add			Add to the list
// cDtsUniqueList::composeUName		Compose a unique name.
// cDtsUniqueList::createUniqueName	Create a unique name.
// cDtsUniqueList::exit			Exit
// cDtsUniqueList::eventLoop		Star the unique name list event loop.
// cDtsUniqueList::findNameMax		Find the maximum unique name.
// cDtsUniqueList::init			Initialize the unique name list.
// cDtsUniqueList::uniqueNameVerify	Verify info about a unique name.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/25 10:32:32  brighton
// added genBool.H
//
// Revision 1.1.1.1  2002/11/24 20:27:15  brighton
// Imported sources
//
// Revision 1.4  2002/02/25 19:04:10  tpaz
// *** empty log message ***
//
// Revision 1.2  2002/02/22 19:29:27  tpaz
// *** empty log message ***
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.19  2001/02/26 21:59:57  dunn
// Chnages to findNameMax for a unique name in standalone mode.
// Got rid of %, and - when looking for a name.
//
// Revision 1.18  2001/02/24 00:03:08  nhill
// Changed to support workshop 5.0.
//
// Revision 1.17  2000/08/10 22:03:05  dunn
// Added check and print of bad type.
//
// Revision 1.16  2000/07/27 20:10:10  dunn
// Added standalone features.
//
// Revision 1.15  1999/06/04 21:43:54  nhill
// Fixed a bug that occured on the first day of the month when the
// date put into the unique names could be set to the 0'th day.
// Fixed the first index assigned to unique names to always be 1.
//
// Revision 1.14  1998/06/16 16:06:38  dunn
// Changed VOID statement
//
// Revision 1.13  1998/05/07 23:34:45  dunn
// Fixed purse of unique list to do it at noon .
//
// Revision 1.12  1998/05/04 23:00:43  dunn
// Lots of changes.
//
// Revision 1.11  1997/10/17 20:45:37  dunn
// Added unique check in datasetNameVerify
//
// Revision 1.10  1997/10/06 20:01:45  dunn
// Fixed comment.
//
// Revision 1.9  1997/10/03 23:20:58  dunn
// List changes.
//
// Revision 1.8  1997/09/29 21:20:18  dunn
// Fixed uniquename generation to include defaultSource and fixed but
// bug when datasetNameVerify is looking at setting a lifetime.
//
// Revision 1.7  1997/09/17 22:13:18  dunn
// Code cleanup
//
// Revision 1.6  1997/09/05 19:52:37  dunn
// Lots of leaks fixed.
//
// Revision 1.5  1997/08/29 23:02:28  dunn
// Added the exit routines.
//
// Revision 1.4  1997/08/19 22:30:37  dunn
// Added chunkLastCheck and others.
//
// Revision 1.3  1997/07/29 22:02:15  dunn
// Various fixes.
//
// Revision 1.2  1997/07/24 16:25:36  dunn
// Fixed get label request, setting of the uniquePrefix
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

#include "genBool.H"

#include <iostream>
//#include <pthread.h>
#include <cstdio>
#include <fstream>
#include <string>
#include <iomanip>

#include "genCond.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "gen_eptr.h"

#include <ctime>
#include "sf.h"
}

#include "globals.H"
#include "dbm.H"
#include "store.H"
#include "list.H"
#include "dtsDhs.H"//  Had to include the next 2 to get lockSf
#include "put.H"

//
//  Local defines.
//

cMutex	cDtsDatasetList::cdlMutexDs;
cMutex	cDtsUniqueList::culMutexUk;

char		cDtsLists::lstLifetime = '\0';
char		cDtsLists::lstLocation    = 'X';
char		cDtsLists::lstSource      = 'X';
char		*cDtsLists::lstUkPrefix;
int		cDtsLists::lstUkPrefixNum = 0;

const std::string cDtsLists::prefixFilename = ".lastlabelprefix";


cDtsDatasetList::tDataList
		cDtsDatasetList::cdlDataList;// List of current datasets.
cDtsUniqueList::tUniqueList
		cDtsUniqueList::culUniqueList;// List of current uniques.

bool		cDtsUniqueList::culElExit = false;
pthread_t	cDtsUniqueList::culElThread = 0;
//static cCond	cDtsUniqueList::culElCond;

cMutex	cDtsLists::lstMutexNumIncomp;
static cCond	culElCond;

#define 	DTS_KEYWORD_NAMES	"names"

//
//***********************************************************************
//+
// FUNCTION NAME:
//	static bool     fileWriteLabelPrefix(char *inPrefixStr, int inPrefixInt);
//
// INVOCATION:
//
// PARAMETERS: 
//	(char *inPrefixStr, int inPrefixInt);
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Write the current label prefix to file
//
// DESCRIPTION:
// Writes the current label prefix to file for persistent storage
// used to continue the label prefix after a reboot
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool cDtsLists::fileWriteLabelPrefix(std::string inPrefixStr, int inPrefixInt)
{

    std::string filePath = std::string(cDtsStoreManager::permPath()) + std::string("/") + cDtsLists::prefixFilename;

    printf("Attempting to write %s\n", filePath.c_str());
    std::ofstream outStream(filePath.c_str(), std::ios::trunc);     // open and erase the old file 

    if (outStream.is_open()) {
        printf("Writing %s%04i\n", inPrefixStr.c_str(), inPrefixInt);
        outStream << inPrefixStr << std::setfill('0') << std::setw(4) << inPrefixInt;
        outStream.close();
	return true;
    }

    printf("Count not open %s\n", filePath.c_str());
    return false;
}



//
//***********************************************************************
//+
// FUNCTION NAME:
//        static bool     fileReadLabelPrefix(char** outPrefixStr, int& outPrefixInt);
//
// INVOCATION:
//
// PARAMETERS:
//       (char** outPrefixStr, int& outPrefixInt);
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the current label prefix file
//
// DESCRIPTION:
// Reads the last written label prefix file and returns string and number parts of the prefix
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// the label prefix file should exist, if not valuse will be set to empty string and 0 
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool cDtsLists::fileReadLabelPrefix(std::string& outPrefixStr, int& outPrefixInt)
{

    std::string filePath = std::string(cDtsStoreManager::permPath()) + std::string("/") + cDtsLists::prefixFilename;

    printf("Attempting to read %s\n", filePath.c_str());
    std::ifstream inStream (filePath.c_str());

    if (inStream.is_open()) 
    { 
        inStream >> outPrefixStr;

        // if the uniqueName matches continue the count 
        std::string num = outPrefixStr.substr(outPrefixStr.length() - 4, 4);

        if (num.length() > 0) {                       // if we found a valid prefix number continue the count
            printf("Found last used prefix: %s\n", outPrefixStr.c_str());
            outPrefixInt = std::atoi(num.c_str());
            inStream.close();		
            return true;
        }
        else {                                       // if we didn't find a valid prefix return false
            printf("Invalid or old prefix in %s\n", filePath.c_str());
            inStream.close();		
            return false;
        }
    }
    else {
        printf("Could not open %s\n", filePath.c_str());
        return false;
    }
}




//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsLists::config
//
// INVOCATION:
// cDtsLists::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the dataset and unique name parameters
//
// DESCRIPTION:
// Read in the default lifetime, qlstream, location and source.
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

void		cDtsLists::config
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    checkConfig( config_get( DTS_KEYWORD_NAMES, nameInfo ), status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsLists::nameInfo
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
// Read the names line from a configuration file and store it
//
// DESCRIPTION:
// Save the default lifetime, location and source.
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

boolean		cDtsLists::nameInfo
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    cDtsStatus	status;		// Status code.

    if ( numTokens == 4 )
    {
	if( tokens[1][0] != DTS_PERM && tokens[1][0] != DTS_TEMP ) 
	{
	    return( FALSE );
	}
	lstLifetime = tokens[1][0];
	
	if( tokens[2][0] != DTS_NORTH &&  tokens[2][0] != DTS_SOUTH ) 
	{
	    return( FALSE );
	}
	else
	{
	    lstLocation = tokens[2][0];
	}

	if( tokens[3][0] != DTS_OCS &&  tokens[3][0] != DTS_BASE && 
		tokens[3][0] != DTS_SUMMIT ) 
	{
	    return( FALSE );
	}
	else
	{
	    lstSource = tokens[3][0];
	}
    }
    else
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	return( FALSE );
    }

    return( TRUE );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::findChunk
//
// INVOCATION:
// cDtsDatasetList::findChunk()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) source (char *) Source of this chunk.
// 
// FUNCTION VALUE:
// Returns true if the passed in source name was found in a chunk,
// otherwise false.
//
// PURPOSE:
// Searchs the chunkList for the source name passed in
//
// DESCRIPTION:
// Searchs the chunkList for the source name passed in.  Returns true, if
// found.
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

cDtsChunkInfo		*cDtsDatasetList::findChunk
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*source			// (in)  Source of the data.
)
{
    cDtsChunkInfo::chunkIterator
			i;		// Ptr to object in list.


    checkStat( status, return(NULL) );


    i = cdlChunkList.find( std::string( source ) );

    if ( i == cdlChunkList.end() )
    {
	//
	//  Not in the list.
	//

	return( NULL ); 
    }

    return( (*i).second );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsChunkInfo::~cDtsChunkInfo
//
// INVOCATION:
// n.a. Deconstructor.
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
// Deconstructor.  Delete and free.
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

		cDtsChunkInfo::~cDtsChunkInfo
(
    void
)
{
    if ( chiContributor != NULL )
    {
	free( chiContributor );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsChunkInfo::cDtsChunkInfo
//
// INVOCATION:
// n.a. Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) contributor (char *) Contributor.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor
//
// DESCRIPTION:
// Constructor.  Initialize the chunk information.
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

		cDtsChunkInfo::cDtsChunkInfo
(
    char	*contributor		// (in) Contributor of the dataset.
)
{
    chiLast = FALSE;
    chiNumChunks = 0;
    chiContributor = strsav( contributor );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::cDtsDatasetList
//
// INVOCATION:
// n.a. Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) datasetName (char *) Dataset name.
// (>) datasetInfo (cDtsDatasetTable *) Dataset table information.
// (>) type (DHS_BD_PUT_TYPE) Data type.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor
//
// DESCRIPTION:
// Save the passed in variables to the object created.  Then add the object 
// to the dataset list.
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

		cDtsDatasetList::cDtsDatasetList
(
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		*datasetInfo, 		// (in)  Dataset table information.
    DHS_BD_PUT_TYPE
    		type			// (in)  Data format sent.
)
{
    cDtsStatus		status;


    cdlDatasetName = strsav( datasetName );
    cdlUniqueName  = strsav( datasetInfo->uniqueName() );
    cdlFormat = (int) type;
    cdlFsize  = 0;
    cdlDs     = datasetInfo;
    cdlDsInfo = NULL;
    cdlPath   = NULL;


    //
    // For each string found in contrib list - create a chunk object
    // then add to this object.
    //

    setChunkList( status );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::cDtsDatasetList
//
// INVOCATION:
// n.a. Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) datasetName (char *) Dataset name.
// (>) datasetInfo (cDtsDatasetTable *) Dataset table information.
// (>) format (char) Data type.
// (>) size (kbytes?) Size of the file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor
//
// DESCRIPTION:
// Save the passed in variables to the object created.  Then add the object 
// to the dataset list.
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

		cDtsDatasetList::cDtsDatasetList
(
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		*datasetInfo, 		// (in)  Dataset table information.
    char	format,			// (in)  Data format sent.
    int		size			// (in)  Size of data.
    
)
{
    cDtsStatus		status;		// Internal function status.


    cdlDatasetName = strsav( datasetName );
    cdlUniqueName = strsav( datasetInfo->uniqueName() );
    cdlFormat = (int) (format == DTS_FM_RAW ) ? 
			DHS_BD_PT_RAW : DHS_BD_PT_FITS;
    cdlFsize  = size;
    cdlDs     = datasetInfo;
    cdlDsInfo = NULL;
    cdlPath   = NULL;


    //
    // Don't add any chunks.
    //

    setChunkList( status );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::cDtsDatasetList
//
// INVOCATION:
// n.a. Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) datasetName (char *) Dataset name.
// (>) path (char *) Path to the file.
// (>) datasetInfo (cDtsDatasetTable *) Dataset table information.
// (>) format (char) Data type.
// (>) compression (char) Compression type.
// (>) size (bytes) Size of the file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor
//
// DESCRIPTION:
// Save the passed in variables to the object created.  Then add the object 
// to the dataset list.
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

		cDtsDatasetList::cDtsDatasetList
(
    char	*datasetName,		// (in)  Dataset name.
    char	*path,			// (in)  Path to dataset.
    cDtsDatasetTable
    		*datasetInfo, 		// (in)  Dataset table information.
    char	format,			// (in)  Data format sent.
    char	comp,			// (in)  Compression type.
    int		size			// (in)  Size of data.
    
)
{
    cDtsStatus		status;		// Interal function status.


    cdlDatasetName = strsav( datasetName );
    cdlPath = strsav( path );
    cdlUniqueName = strsav( datasetInfo->uniqueName() );
    cdlFormat = (int) (format == DTS_FM_RAW ) ? 
			DHS_BD_PT_RAW : DHS_BD_PT_FITS;
    cdlFsize  = size;
    cdlComp   = comp;
    cdlDs     = datasetInfo;
    cdlDsInfo = NULL;


    //
    // Don't add any chunks.
    //

    setChunkList( status );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::~cDtsDatasetList
//
// INVOCATION:
// n.a. Deconstructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deconstructor
//
// DESCRIPTION:
// Deconstructor.  Free variables.
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

		cDtsDatasetList::~cDtsDatasetList
(
    void
)
{
    cDtsStatus		status;


    //
    //  Don't erase object, do it outside the constructor.
    //  Free all malloc things.
    //

    if ( cdlDatasetName != NULL )
    {
	free( cdlDatasetName );
    }
    if ( cdlUniqueName != NULL )
    {
	free( cdlUniqueName );
    }

    if ( cdlDsInfo != NULL )
    {
	cDtsPutDs::lockSf();
	sfDsInfoFree( cdlDsInfo );
	cDtsPutDs::unlockSf();
    }


    //
    //  Cycle through the chunk list associated with this dataset object,
    //  and delete and erase it.
    //

    cDtsChunkInfo	*pChunk;
    for ( cDtsChunkInfo::chunkIterator i = cdlChunkList.begin(); 
	    i != cdlChunkList.end(); i = cdlChunkList.begin() )
    {
	pChunk = (*i).second;
	cdlChunkList.erase(i);
	if ( pChunk != NULL )
	{
	    delete pChunk;
	}
    }
    delete ( cdlDs );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::add
//
// INVOCATION:
// cDtsDatasetList::add()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add to the dataset list
//
// DESCRIPTION:
// Save the passed in variable to the object created.  Then add the object 
// to the dataset list.
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

void	cDtsDatasetList::add
(
    cDtsStatus	&status			// (mod) Status passed in.
)
{
    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Now add this to the dataset list.
    //

    cdlDataList[ std::string( cdlDatasetName ) ] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::addDatabase()
//
// INVOCATION:
// cDtsDatasetList::addDatabase()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) datasetName (char *) Dataset name.
// (>) uniqueName (char *) unique name.
// (<) pDataInfo (cDtsDatasetTable **) Pointer to the dataset tbl information.
// (>) source (char *) Source of the request.
// (>) type (DHS_BD_PUT_TYPE) DHS bulk data put type.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Add a line to the database
//
// DESCRIPTION:
// Create a new dataset table object and put that information to the database,
// if not in standalone mode.
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

void		cDtsDatasetList::addDatabase
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName,		// (in)  Dataset name.
    char	*uniqueName,		// (in)  Unique name.
    cDtsDatasetTable
    		**pDataInfo,		// (mod)  Pointer to dataset table info.
    char	*source,		// (in)   Source of the request.
    DHS_BD_PUT_TYPE
    		type			// (in)  Type of data sent.
)
{

    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Create a new dataset table object, and put that information
    //  to the database.
    //

    *pDataInfo = new cDtsDatasetTable( datasetName, 
	    cDtsLists::defaultLife(), uniqueName, source, time(NULL),
	    type );


    //
    //  If NOT running standalone, then add to the database.
    //

    if ( !status.standAlone() )
    {
	cDtsDatasetTable::filePut( status, datasetName, *pDataInfo );
    }

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::addList()
//
// INVOCATION:
// cDtsDatasetList::addList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) datasetName (char *) Dataset name.
// (>) pDataInfo (cDtsDatasetTable **) Pointer to the dataset tbl information.
// (<) pTemp (cDtsDatasetList **) Pointer to the list item.
// (>) type (DHS_BD_PUT_TYPE) DHS bulk data put type.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To add the cDtsDatasetList passed in to the dataset list
//
// DESCRIPTION:
// To add the cDtsDatasetList passed in to the dataset list.
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

void		cDtsDatasetList::addList
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
    		*pDataInfo,		// (in)  Pointer to dataset table info.
    cDtsDatasetList
		**pTemp,		// (out) Ptr to list item.
    DHS_BD_PUT_TYPE
    		type			// (in)  Type of data sent.
)
{

    //
    //  Check status.
    //

    checkStat( status, return );


    //
    //  Create a new list object and add it to the dataset list.
    //

    *pTemp = new cDtsDatasetList( datasetName, pDataInfo, type );
    (*pTemp)->add( status );
    status.S_DS_LIST_ADD( status, datasetName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::addList()
//
// INVOCATION:
// cDtsDatasetList::addList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) datasetName (char *) Dataset name.
// (>) pDataInfo (cDtsDatasetTable **) Pointer to the dataset tbl information.
// (<) pTemp (cDtsDatasetList **) Pointer to the list item.
// (>) type (DHS_BD_PUT_TYPE) DHS bulk data put type.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Free the passed in dataset name, create a new one and return it.
//
// DESCRIPTION:
// Free the passed in dataset name, create a new one and return it.
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

void		cDtsDatasetList::addList
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName,		// (mod)  Dataset name.
    cDtsDatasetTable
    		*pDataInfo,		// (in)  Pointer to dataset table info.
    cDtsDatasetList
		**pTemp,		// (out) Ptr to list item.
    char	type			// (in)  Type of data sent.
)
{

    DHS_BD_PUT_TYPE	dhsType;	// Type conversion.


    checkStat( status, return );


    //
    //  Convert the type to dhs types.
    //

    if ( type ==  DTS_FM_RAW )
    {
	dhsType = DHS_BD_PT_RAW;
    }
    else if ( type == DTS_FM_FITS )
    {
	dhsType = DHS_BD_PT_FITS;
    }
    else
    {
	dhsType = DHS_BD_PT_UNKNOWN;
    }


    //
    //  Create a new dataset list object and add it to the list.
    //

    *pTemp = new cDtsDatasetList( datasetName, pDataInfo, dhsType );
    (*pTemp)->add( status );
    status.S_DS_LIST_ADD( status, datasetName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::chunkExists
//
// INVOCATION:
// cDtsDatasetList::chunkExists()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) source (char *) Source of this chunk.
// (>) type (DHS_BD_PUT_TYPE) Type of data sent in.
// (>) zeroBuf (bool) Buffer is of length zero, no data.
// 
// FUNCTION VALUE:
// Returns true if the source was found, otherwise false.
//
// PURPOSE:
// Searchs the chunkList for the source name passed in
//
// DESCRIPTION:
// Searchs the chunkList for the source name passed in.  Returns true, if
// found and increments number of chunks.
//
// EXTERNAL VARIABLES:
// Returns true if found in the list, otherwise false.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cDtsDatasetList::chunkExists
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*source,		// (in)  Source of the data.
    DHS_BD_PUT_TYPE
    		type,			// (in)  Type of data sent.
    bool	zeroBuf			// (in)  True if buffer is empty.
)
{
    cDtsChunkInfo::chunkIterator	
			i;		// Ptr to object in list.
    cDtsChunkInfo	*pChunk;	// Ptr to chunk information.


    checkStat( status, return(FALSE) );


    pChunk = findChunk( status, source );
    if ( pChunk == NULL )
    {
	//
	//  Not in the list.
	//

	return( FALSE );
    }


    //
    //  Increment number of chunks for that contributor.
    //  and set the last flag to true if the type is not DS, or the
    //  last flag was set.
    //

    if ( !zeroBuf )
    {
	pChunk->chiNumChunks++;
    }
    if ( type != DHS_BD_PT_DS )
    {
	pChunk->chiLast = TRUE;
    }

    return(TRUE);
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::chunkLastCheck()
//
// INVOCATION:
// cDtsDatasetList::chunkLastCheck()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) lastFlag (bool) Last flag.
// (>) source (char *) Source of the data.
// (>) zeroBuf (bool) Buffer is of length zero, no data.
// (<) allSet (bool) All data received flag.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Check to see if the last chunk has been sent for this dataset/source
// and if its the last chunk to be sent for this dataset
//
// DESCRIPTION:
// Finds the chunk associated with this dataset/source and sets the last 
// flag if necessary and finds out if it is the last one for this dataset.
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

void	cDtsDatasetList::chunkLastCheck
(
    cDtsStatus	&status,		// (mod) Returned function status.
    bool	lastFlag,		// (in)  Last flag.
    char	*source,		// (in)  Source of the data.
    bool	zeroBuf,		// (in)  True if buffer is empty.
    bool	&allSet 		// (out) All data received flag.
)
{
    cDtsChunkInfo	*pChunk;

    checkStat( status, return );

    pChunk = findChunk( status, source );
    if ( pChunk != NULL && lastFlag && !pChunk->chiLast )
    {
	//
	//  Last flag is set for this contributor, set the last flag 
	//  in the chunk, then check to see if all other contrib. last
	//  flags are set.
	//

	pChunk->chiLast = TRUE;
	if ( !zeroBuf )
	{
	    pChunk->chiNumChunks++;
	}	


	cDtsChunkInfo::chunkIterator
			i;		// Ptr to object in list.

	allSet = TRUE;
	for( i=cdlChunkList.begin(); i!=cdlChunkList.end(); i ++ )
	{
	    cDtsChunkInfo       *pTmp;
	    pTmp = (*i).second;
	    if ( !pTmp->chiLast )
	    {
		//
		//  Found a chunk where the last flag was not set.
		//

		allSet = FALSE;
		break;
	    }
	}
    }
    else if ( pChunk != NULL )
    {
	//
	//  Else this is a chunk without the last flag set, so just increment
	//  the number of chunks for this source.
	//

	if ( !zeroBuf )
	{
	    pChunk->chiNumChunks++;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::datasetListReset()
//
// INVOCATION:
// cDtsDatasetList::datasetListReset(status, dsPtr)
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDstStatus & ) Function return status.
// (!) dsPtr (cDtsDatasetTable *) Ptr to dataset table information.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Removes from db, the ds listed, removes data and then adds with received
// flags set to null
//
// DESCRIPTION:
// Removes from db, the ds listed, removes data and then adds with received
// flags set to null.
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

void		cDtsDatasetList::datasetListReset
(
    cDtsStatus	&status, 		// (mod) Returned function status.
    cDtsDatasetTable
		*dsPtr			// (mod) Ptr to the dataset.
)
{
    //
    //  Check status.
    //

    checkStat( status, return );


    //
    //  Check for datatype of DS.
    //

    if ( dsPtr->dstFormat == DTS_FM_DS )
    {
	//
	//  Part of the data is received, remove dir.
	//

	char        tmp[PATH_MAX];
	
	(void) sprintf( tmp, "%s/%s", cDtsStoreManager::tempPath(), 
		cdlDatasetName );
	cDtsStoreManager::fileRemove( status, tmp,
		cDtsStoreManager::ST_TEMP, cdlFsize );
    }


    //
    //  Reset the table information and update the process table, if
    //  not in standalone mode.
    //

    dsPtr->dstReceived = DTS_DS_NOT_RECEIVED;
    dsPtr->dstHealth   = DTS_HL_HEALTHY;
    dsPtr->dstFormat   = DTS_FM_UNKNOWN;

    if ( !status.standAlone() )
    {
	cDtsDatasetTable::fileUpdate( status, dsPtr->dstDatasetName, dsPtr );
    }


    //
    //  Delete the ds info and reset the chunk list.
    //

    if ( cdlDsInfo != NULL )
    {
	cDtsPutDs::lockSf();
	sfDsInfoFree( cdlDsInfo );
	cDtsPutDs::unlockSf();
	cdlDsInfo = NULL;
    }

    cDtsDatasetList::resetChunkList( status, dsPtr->dstDatasetName );
	
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::datasetListRemove()
//
// INVOCATION:
// cDtsDatasetList::datasetListRemove(status, received, format)
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDstStatus & ) Function return status.
// (>) received (char) Received state of this dataset.
// (>) format (char) Data format for this dataset.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To remove information from the database and list about this dataset
//
// DESCRIPTION:
// If all data has been received, then update ad tables to make it 
// unretrievable.  If its of database DS, then may only partially been
// received, so remove the directory.  Remove from the process table
// and then from the dataset list.
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

void		cDtsDatasetList::datasetListRemove
(
    cDtsStatus	&status, 		// (mod) Returned function status.
    char	received,		// (in)  Received status.
    char	format			// (in)  Format of data.
)
{

    //
    //  Check the status and lock the dataset list.
    //

    checkStat( status, return );
    datasetLock();


    if ( received == DTS_DS_ALL_RECEIVED )
    {
	//
	//  All data has been received, then make unretrieveable:
	//  removed from ad, and from the file system.
	//

	cDtsDbManager::makeUnretrievable( status, cdlDatasetName, 
	    cdlDs->dstLifetime, TRUE );
    }
    else if ( format == DTS_FM_DS )
    {
	//
	//  Else only part of the data is received, remove dir.
	//
	char        tmp[PATH_MAX];
	
	(void) sprintf( tmp, "%s/%s", cDtsStoreManager::tempPath(), 
		cdlDatasetName );
	cDtsStoreManager::fileRemove( status, tmp,
		cDtsStoreManager::ST_TEMP, cdlFsize );
	//
	// WARNING!! cdlFsize is not updated so don't know the size 
	//
    }
    else
    {
	//
	//  Else never in the tables.
	//
    }


    //
    //  Remove from the process table.
    //

    cDtsDatasetTable::fileDel( status, cdlDatasetName );

	
    //
    //  Don't need to remove from the list, because you delete outside of
    //  this function and the deconstructor does an erase.  Just removed that,
    //  leave the erase here so that it is inside the lock.
    //

    cdlDataList.erase ( cdlDatasetName );
    datasetUnlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::datasetNameFind()
//
// INVOCATION:
// datasetNameFind( status, datasetName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDstStatus & ) Function return status.
// (>) datasetName (char *) Dataset name.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Searchs the datasetList for the dataset name  passed in and
// returns a pointer to that item
//
// DESCRIPTION:
// Searchs the datasetList for the dataset name passed in and
// returns a pointer to that item.
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

cDtsDatasetList		*cDtsDatasetList::datasetNameFind
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName		// (in)  Dataset name.
)
{
    iterator		i;		// Ptr to object in list.
    cDtsDatasetList	*pDsList;	// Ptr to new object.
    cDtsDatasetTable	*pTableInfo;	// Local table dataset info.
    bool		checkStates = FALSE;
    					// Flag to check states.


    //
    //  Check the status, set nulls, and lock the dataset list.
    //

    checkStat( status, return(NULL) );
    pTableInfo = NULL;
    pDsList = NULL;
    datasetLock();


    //
    //  Search the dataset list.
    //

    i = cdlDataList.find( std::string( datasetName ) );

    if ( i == cdlDataList.end() )
    {
	//
	//  Not found in list, see if its in the database.
	//
	
	if ( !status.standAlone() )
	{
	    cDtsDatasetTable::fileGet( status, datasetName, &pTableInfo );
	}
	if ( status.standAlone() || status == status.S_NO_ROWS )
	{
	    //
	    //  Not in the list or the database, return.
	    //

	    status.S_NOT_IN_DS_LIST( status, datasetName );
	    datasetUnlock();
	    return( NULL );
	}
	else if ( status.ok() )
	{
	    //
	    //  In the database, Add it to the list.
	    //

	    addList( status, datasetName, pTableInfo, &pDsList, 
		    pTableInfo->dstFormat );
	}
    }
    else
    {
	//
	//  Found in the list, return a pointer to it.
	//

	pDsList = (*i).second;
    }

    datasetUnlock();
    return ( pDsList );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::datasetNameVerify
//
// INVOCATION:
// datasetNameVerify(status, &datasetName, uniqueName, source, type, fsize
//     allSet, allReceived, notContrib, badType, newName, lastFlag );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDstStatus & ) Function return status.
// (!) datasetName (char **) Dataset name.
// (>) uniqueName (char *) Unique name.
// (>) source (char *) Source of the request.
// (>) type (DHS_BD_PUT_TYPE) Type of the data.
// (>) fsize (int) File size.
// (<) allSet (bool &) All last flags are set for all contribs.
// (<) allReceived (bool &) All data already received.
// (<) notContrib (bool &) This source not a contrib to this dataset.
// (<) badType (bool &) Passed in datatype does not match saved type.
// (<) newName (bool &) New datasetName created.
// (>) lastFlag (bool) Last flag.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Searchs the datasetList for the dataset/unique combo passed in and
// returns a pointer to that item
//
// DESCRIPTION:
// Searchs the datasetList for the dataset/unique combo passed in and
// returns a pointer to that item.  It determines if a new datasetName
// has to be created and adds it to the database and list.
// This one is used by putSds.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// Long and complicated.  Be really careful if you want to change this.
//-
//***********************************************************************
//


cDtsDatasetList		*cDtsDatasetList::datasetNameVerify
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	**datasetName,		// (mod)  Dataset name.
    char	*uniqueName,		// (in)  Unique name.
    char	*source,		// (in)  Source of the data.
    DHS_BD_PUT_TYPE
    		type,			// (in)  Type of data sent.
    int		fsize,			// (in)  File size.
    bool	&allSet,		// (out) All last flags set.
    bool	&allReceived,		// (out) Already received.
    bool	&notContrib,		// (out) Not a contributor.
    bool	&badType,		// (out) Types don't match.
    bool	&newName,		// (out) New name generated.
    bool	lastFlag		// (in)  Last flag.
)
{
    iterator		i;		// Ptr to object in list.
    bool		inList;		// In the list already.
    cDtsDatasetList	*pDsList = NULL;// Ptr to new object.
    cDtsDatasetTable	*pTableInfo = NULL;
    					// Local table dataset info.
    bool		checkStates;	// Flag to check states.
    cDtsChunkInfo	*pChunk;
    char		initState;	// Initial received state.
    bool		alreadyInList;	// Already in list flag.


    checkStat( status, return(NULL) );
    pTableInfo	=	NULL;
    checkStates	=	FALSE;
    newName	=	FALSE;
    allReceived =	FALSE;
    notContrib	=	FALSE;
    badType	=	FALSE;
    newName	=	FALSE;
    inList	=	FALSE;
    allSet	= 	FALSE;
    initState	= 	'X';
    alreadyInList = 	FALSE;


    datasetLock();

    //
    //  Search the dataset list.
    //

    if ( type != DHS_BD_PT_UNKNOWN && (
	    type == DHS_BD_PT_FITS_UNIQUE || type == DHS_BD_PT_RAW_UNIQUE ) )
    {
	//
	//  Generate a new dataset name. Add to the database and list.
	//

	newDatasetName( status, datasetName );
	addDatabase( status, *datasetName, uniqueName, &pTableInfo, source, 
		type );
	addList( status, *datasetName, pTableInfo, &pDsList, type );
	newName = TRUE;
    }
    else
    {
    
	i = cdlDataList.find( std::string( *datasetName ) );

	if ( i == cdlDataList.end() )
	{

	    //
	    //  Not found in list, first see if its in the database.
	    //
	    
	    if ( !status.standAlone() )
	    {
		cDtsDatasetTable::fileGet( status, *datasetName, &pTableInfo );
	    }
	    if ( status.standAlone() || status == status.S_NO_ROWS )
	    {
		//
		//  Its not in the database, add to the database and list.
		//
	
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		addList( status, *datasetName, pTableInfo, &pDsList, type );
		initState = DTS_DS_NOT_RECEIVED;
	    }
	    else
	    {
		//
		//  Found it in the db., set the flag to check the states.
		//

		checkStates = TRUE;
		initState = pTableInfo->dstReceived;
	    }
	}
	else
	{
	    pDsList = (*i).second;
	    pTableInfo = pDsList->getDs();
	    initState = pTableInfo->dstReceived;
	    checkStates = TRUE;
	    inList = TRUE;
	    alreadyInList = TRUE;
	}


	if ( checkStates )
	{
	    if ( pTableInfo->dstFormat == DTS_FM_UNKNOWN &&
		    pTableInfo->dstReceived == DTS_DS_NOT_RECEIVED )
	    {
		//
		//  Data not received, just life or contrib were set, so set it.
		//

		pTableInfo->dstFormat = cDtsDbManager::formatDetermine( type );
	    }

	    if ( ( type == DHS_BD_PT_DS && 
		     pTableInfo->dstReceived == DTS_DS_ALL_RECEIVED ) ||
		    ( type != DHS_BD_PT_DS &&
		    pTableInfo->dstReceived != DTS_DS_NOT_RECEIVED ) )
	    {
		//
		//  Data already received.
		//

		initState = DTS_DS_NOT_RECEIVED;
		newDatasetName( status, datasetName );
		if ( !inList && pTableInfo != NULL )
		{
		    delete pTableInfo;
		    pTableInfo = NULL;
		}
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		allReceived = TRUE;
		newName = TRUE;
		inList = FALSE;
	    }
	    else if ( pTableInfo->dstFormat != DTS_FM_UNKNOWN &&
		    cDtsDbManager::formatDetermine( type ) != 
			    pTableInfo->dstFormat )
	    {
		//
		//  Type passed in and saved type don't match.
		//

		newDatasetName( status, datasetName );
		if ( !inList && pTableInfo != NULL )
		{
		    delete pTableInfo;
		    pTableInfo = NULL;
		}
		initState = DTS_DS_NOT_RECEIVED;
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		badType = TRUE;
		newName = TRUE;
		inList = FALSE;
	    }
	    else
	    {
		if ( !inList )
		{
		    pDsList = new cDtsDatasetList( *datasetName, pTableInfo, 
			    type );
		}
		

		if ( ( pChunk = pDsList->findChunk( status, source ) ) == NULL )
		{
		    if ( !inList )
		    {
			//
			//  Only delete from the list if we just put it in.

			delete ( pDsList );
		    }
		    initState = DTS_DS_NOT_RECEIVED;
		    newDatasetName( status, datasetName );
		    addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			    source, type );
		    inList = FALSE;
		    notContrib = TRUE;
		    newName = TRUE;
		}
	    }
	    if ( !inList )
	    {
		addList( status, *datasetName, pTableInfo, &pDsList, type );
	    }
	}


	//
	//  If this is a DS type, check to see if this is the last chunk,
	//  or that it has not already received the last chunk.
	//

	if ( type == DHS_BD_PT_DS && status.ok() && pDsList != NULL  
		&& pTableInfo != NULL )
	{
	    pChunk = pDsList->findChunk( status, source );
	    if ( pChunk != NULL && lastFlag &&  pChunk->chiLast )
	    {
		//
		//  Last flag already set, error,
		//  Generate a new dataset name. Add to the database and list.
		//

		newDatasetName( status, datasetName );
		if ( pTableInfo != NULL )
		{
		    delete pTableInfo;
		    pTableInfo = NULL;
		}
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		addList( status, *datasetName, pTableInfo, &pDsList, 
			type );
		newName = TRUE;
		allSet = TRUE;
	    }
	}
    }

    //
    //  Lock this dataset object.  Unlock the list.
    //

    pDsList->datasetObjectLock();
    status.S_CHUNK( status, pDsList->cdlDatasetName, pDsList->cdlNumChunks);
    datasetUnlock();


    //
    //  Set the data received state and increment number of chunks.
    //

    if ( type != DHS_BD_PT_UNKNOWN && status.ok() && pDsList != NULL  
	    && pTableInfo != NULL )
    {
	if ( type == DHS_BD_PT_DS )
	{
	    pDsList->chunkLastCheck( status, lastFlag, 
		    source, ((fsize > 0)? FALSE : TRUE),
		    allSet );
	    if ( status.ok() && !allSet )
	    {
		pTableInfo->dstReceived = DTS_DS_PART_RECEIVED;
	    }
	    else if ( status.ok() && allSet )
	    {
		pTableInfo->dstReceived = DTS_DS_ALL_RECEIVED;
	    }
	}
	else
	{
	    pTableInfo->dstReceived = DTS_DS_ALL_RECEIVED;
	}

	if ( fsize > 0 )
	{
	    pDsList->cdlNumChunks = pDsList->cdlNumChunks + 1;
	}
    }

    pTableInfo->dstFileSize =  pTableInfo->dstFileSize + fsize;
    pDsList->cdlFsize = pDsList->cdlFsize + fsize;


    return( pDsList );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::datasetNameVerify
//
// INVOCATION:
// datasetNameVerify(status, &datasetName, uniqueName, source, type, &dsPtr,
//     fsize, allReceived, notContrib, badType, newName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDstStatus & ) Function return status.
// (!) datasetName (char **) Dataset name.
// (>) uniqueName (char *) Unique name.
// (>) source (char *) Source of the request.
// (>) type (DHS_BD_PUT_TYPE) Type of the data.
// (<) dsPtr (cDtsDatasetTable **) Ptr to dataset table info.
// (>) fsize (int) File size.
// (<) allReceived (bool &) All data already received.
// (<) notContrib (bool &) This source not a contrib to this dataset.
// (<) badType (bool &) Passed in datatype does not match saved type.
// (<) newName (bool &) New datasetName created.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Searchs the datasetList for the dataset/unique combo passed in and
// returns a pointer to that item
//
// DESCRIPTION:
// Searchs the datasetList for the dataset/unique combo passed in and
// returns a pointer to that item.  It determines if a new datasetName
// has to be created and adds it to the database and list.
// This one is used by put, and control settting stuff.  It does worry
// about chunk stuff as there is only 1 contributor and all data is
// sent in 1 chunk.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// Long and complicated.  Be really careful if you want to change this.
//-
//***********************************************************************
//

cDtsDatasetList		*cDtsDatasetList::datasetNameVerify
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	**datasetName,		// (mod)  Dataset name.
    char	*uniqueName,		// (in)  Unique name.
    char	*source,		// (in)  Source of the data.
    DHS_BD_PUT_TYPE
    		type,			// (in)  Type of data sent.
    cDtsDatasetTable
		**dsPtr,		// (out) Ptr to the dataset tbl.
    int		fsize,			// (in)  File size.
    bool	&allReceived,		// (out) Already received.
    bool	&notContrib,		// (out) Not a contributor.
    bool	&badType,		// (out) Types don't match.
    bool	&newName		// (out) New name generated.
)
{
    iterator		i;		// Ptr to object in list.
    bool		inList;		// In the list already.
    cDtsDatasetList	*pDsList = NULL;
    					// Ptr to new object.
    cDtsDatasetTable	*pTableInfo = NULL;
    					// Local table dataset info.
    bool		checkStates;	// Flag to check states.
    bool		alreadyInList;	// Already in list flag.


    checkStat( status, return(NULL) );

    checkStates	=	FALSE;
    pTableInfo	=	NULL;
    newName	=	FALSE;
    allReceived	=	FALSE;
    notContrib	=	FALSE;
    badType	=	FALSE;
    newName	=	FALSE;
    inList	= 	FALSE;
    alreadyInList = 	FALSE;

    datasetLock();

    if ( type != DHS_BD_PT_UNKNOWN && (
	    type == DHS_BD_PT_FITS_UNIQUE || type == DHS_BD_PT_RAW_UNIQUE ) )
    {
	//
	//  Generate a new dataset name. Add to the database and list.
	//

	newDatasetName( status, datasetName );
	addDatabase( status, *datasetName, uniqueName, &pTableInfo, source, 
		type );
	addList( status, *datasetName, pTableInfo, &pDsList, type );
	newName = TRUE;
    }
    else
    {

	//
	//  Search the dataset list.
	//

	i = cdlDataList.find( std::string( *datasetName ) );

	if ( i == cdlDataList.end() )
	{
	    //
	    //  Not found in list, first see if its in the database.
	    //
	    
	    if ( !status.standAlone() )
	    {
		cDtsDatasetTable::fileGet( status, *datasetName, &pTableInfo );
	    }
	    if ( status.standAlone() || status == status.S_NO_ROWS )
	    {
		//
		//  Its not in the database, add to the database and list.
		//
	
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		addList( status, *datasetName, pTableInfo, &pDsList, type );
	    }
	    else
	    {
		checkStates = TRUE;
	    }
	}
	else
	{
	    pDsList = (*i).second;
	    pTableInfo = pDsList->getDs();
	    checkStates = TRUE;
	    inList = TRUE;
	    alreadyInList = TRUE;
	}

	if ( type == DHS_BD_PT_UNKNOWN && checkStates )
	{
	    //
	    //  Setting the lifetime, check that nothing is received.
	    //


	    if ( pTableInfo->dstReceived != DTS_DS_NOT_RECEIVED )
	    {
		//
		//  Data already received, cannot set lifetime
		//

		allReceived = TRUE;
		*dsPtr = NULL;
		datasetUnlock();
		if ( pDsList != NULL )
		{
		    status.S_CHUNK( status, pDsList->cdlDatasetName, 
			    pDsList->cdlNumChunks);
		}
		else
		{
		    status.S_CHUNK( status, pTableInfo->dstDatasetName, 1 );
		}
		return( NULL );
	    }
	    ///// WARNING, add check of chunks here.
	    else if ( !inList )
	    {
		//
		//  Add to the list.
		//

		addList( status, *datasetName, pTableInfo, &pDsList, type );
	    }
	}
	else if ( checkStates )
	{
	    if ( pTableInfo->dstFormat == DTS_FM_UNKNOWN &&
		    pTableInfo->dstReceived == DTS_DS_NOT_RECEIVED )
	    {
		//
		//  Data not received yet, just life or contrib were set, 
		//  so set it.
		//

		pTableInfo->dstFormat = cDtsDbManager::formatDetermine( type );
	    }

	    if ( ( type == DHS_BD_PT_DS && pTableInfo->dstReceived != 
		    DTS_DS_PART_RECEIVED ) || 
		    ( pTableInfo->dstReceived != DTS_DS_NOT_RECEIVED ) )
	    {
		//
		//  Data already received.
		//

		newDatasetName( status, datasetName );
		if ( !inList && pTableInfo != NULL )
		{
		    delete pTableInfo;
		    pTableInfo = NULL;
		}
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		allReceived = TRUE;
		newName = TRUE;
		inList = FALSE;
	    }
	    else if ( pTableInfo->dstFormat != DTS_FM_UNKNOWN &&
		    cDtsDbManager::formatDetermine( type ) != 
			    pTableInfo->dstFormat )
	    {
		//
		//  Type passed in and saved type don't match.  If type is
		//  unkown, them did not get any data yet.
		//

		newDatasetName( status, datasetName );
		if ( !inList && pTableInfo != NULL )
		{
		    delete pTableInfo;
		    pTableInfo = NULL;
		}
		addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			source, type );
		badType = TRUE;
		newName = TRUE;
		inList = FALSE;
	    }
	    else
	    {
		if ( !inList )
		{
		    pDsList = new cDtsDatasetList( *datasetName, pTableInfo,
			    type );
		}
		
		if ( ! pDsList->chunkExists( status, source, type, 
			(fsize > 0)? FALSE : TRUE ) )
		{
		    if ( !inList )
		    {
			//
			//  Only delete from the list if we just put it in.

			delete ( pDsList );
		    }
		    newDatasetName( status, datasetName );
		    addDatabase( status, *datasetName, uniqueName, &pTableInfo, 
			    source, type );
		    notContrib = TRUE;
		    newName = TRUE;
		    inList = FALSE;
		}
	    }
	    if ( !inList )
	    {
		addList( status, *datasetName, pTableInfo, &pDsList, type );
	    }
	}
    }

    pDsList->datasetObjectLock();
    status.S_CHUNK( status, pDsList->cdlDatasetName, pDsList->cdlNumChunks);
    datasetUnlock();

    *dsPtr = pDsList->getDs();
    pTableInfo->dstFileSize =  pTableInfo->dstFileSize + fsize;
    pDsList->cdlFsize = pDsList->cdlFsize + fsize;


    //
    //  Set the data to recevied and increment number of chunks.
    //

    if ( type != DHS_BD_PT_UNKNOWN && status.ok() && *dsPtr != NULL  )
    {
	if ( type == DHS_BD_PT_DS )
	{
	    (*dsPtr)->dstReceived = DTS_DS_PART_RECEIVED;
	}
	else
	{
	    (*dsPtr)->dstReceived = DTS_DS_ALL_RECEIVED;
	}

	if ( fsize > 0 )
	{
	    pDsList->cdlNumChunks = pDsList->cdlNumChunks + 1;
	}
    }
    return( pDsList );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::exit()
//
// INVOCATION:
// exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Deletes all datasets in the dataset list
//
// DESCRIPTION:
// Deletes all datasets in the dataset list.
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

void	cDtsDatasetList::exit
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    //
    //  Cycle through the dataset list and delete and then erase
    //  each one.
    //

    for( iterator i = cdlDataList.begin(); i != cdlDataList.end();
		i = cdlDataList.begin() )
    {
	delete (*i).second;
	cdlDataList.erase( i );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::eventLoop()
//
// INVOCATION:
// eventLoop( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Checks, purges the unique label list, and sleeps for a time
//
// DESCRIPTION:
// Checks, purges the unique label list, and sleeps for a time.
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

void	*cDtsUniqueList::eventLoop
(
    void 	*		// (in)  Dummy argument.
)
{
    timespec	timeOut;	// Time for the next condition variable 
    				// timeout.


    culElCond.lock();
    while ( ! culElExit )
    {
	//
	//  Check and purge the unique name list.
	//

	cDtsUniqueList	*pTemp;		// Ptr to new object.


	//
	//  Get the current time in sec.'s subtract 3 days.
	//  Lock the unique name list.
	//

	clock_gettime( CLOCK_REALTIME, &timeOut );
	timeOut.tv_sec -= 259200;
	cDtsUniqueList::ukNameLock();

	for( iterator i = culUniqueList.begin(); i != culUniqueList.end(); i++ )
	{
	    //
	    //  Delete any labels not accessed in the last 3 days.
	    //

	    pTemp = (*i).second;
	    if ( pTemp->culLastAccess  < timeOut.tv_sec )
	    {
		delete (*i).second;
		culUniqueList.erase( i );
	    }
	}
	
	cDtsUniqueList::ukNameUnlock();


	//
	//  Get the time and set it to sleep for 
	//  3 days from noon of today.
	//

	long	t;			// Time.
	long	timeDiff;		// Time different to noon.
	struct tm	*tm, tm1;	// Time structure.

	t = time(NULL);
	tm = localtime_r( &t, &tm1 );
	timeDiff = tm->tm_hour - 12;	// Time to 12 noon.

	timeOut.tv_sec = timeOut.tv_sec + 777600 + timeDiff*3600;
	culElCond.timedWait( timeOut );
    }
    culElCond.unlock();
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::findNameMax
//
// INVOCATION:
// cDtsDatasetList::findNameMax
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
// To get the maximum number, given the name
//
// DESCRIPTION:
// To get the maximum number from the list, given the name.
// It searches for the internal list for name + - + #.  
// It looks for the largest number.
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

void	cDtsDatasetList::findNameMax
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*uniquePart,		// (in)  Name.
    int		*num			// (out) Maximum number.
)
{
    char	*tmp;			// Temporary string.
    int		itmp;			// Temporary number.
    int		largestNum;		// Largest number found.
    cDtsDatasetList	*pDsList;	// Ptr. to dataset list.


    //
    //  Check status, set variables.
    //

    checkStat( status, return );
    pDsList = NULL;
    itmp = 0;
    largestNum = 0;

    std::string strPrefix;
    int numPrefix;

    if (fileReadLabelPrefix(strPrefix, numPrefix)) {                    // got the previous prefix 
	if (strstr(strPrefix.c_str(), uniquePart)) {                    // if same day continue the count where we left off
            *num = numPrefix;                                               
        }
        else {
            *num = 0;                                                   // no match (aka new day) - start from 0
        }
    }
    else {                                                              // no file - start from 0
        *num = 0;                                                       // count will be incremented 
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::newDatasetName()
//
// INVOCATION:
// newDatasetName( status, &datasetName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// (<) datasetName (char **) New dataset name.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To create a new dataset name
//
// DESCRIPTION:
// Free the passed in dataset name, create a new one and return it.
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

void		cDtsDatasetList::newDatasetName
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	**datasetName		// (out)  Dataset name.
)
{
    int			num;		// number for unique name.
    char		*tmp;	


    //
    //  Must generate a new dataset name, just use the existing one
    //  and add on a -# to it.  First go to the database and find
    //  the maximum # that exists now.  If running standalone, then
    //  must look in the list rather than the database.
    //

    if ( status.standAlone() )
    {
	findNameMax( status, *datasetName, &num );
	num++;
    }
    else
    {
	cDtsDatasetTable::datasetNameMax( status, *datasetName, &num );
    }


    //
    //  Alloc room, free the old dataset name, then point it to the new one.
    //

    if ( status.ok() )
    {
	tmp = (char *) gen_alloc( DTS_DATASET_NAME_LEN );
	sprintf( tmp, "%s-%d", *datasetName, num );
	free( *datasetName );
	*datasetName = tmp;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::resetChunkList()
//
// INVOCATION:
// resetChunkList( status, datasetName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// (>) datasetName (char *) Dataset name.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Reset the chunk list in the dataset list
//
// DESCRIPTION:
// Find the dataset object, and then set the chunk list.
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

void		cDtsDatasetList::resetChunkList
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName		// (in)  Dataset name.
)
{
    iterator		i;		// Ptr to object in list.
    cDtsDatasetList	*pDsList;	// Ptr to new object.


    //
    //  Find the dataset object in the dataset list.
    //

    i = cdlDataList.find( std::string( datasetName ) );

    if ( i != cdlDataList.end() )
    {
	//
	//  Found the object, now set the chunk list.
	//

	pDsList = (*i).second;
	pDsList->setChunkList( status );
    }
    else
    {
	//
	//  Else didn't find the dataset object in the dataset list.
	//

	status.E_DS_NOT_FOUND( status, datasetName );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::setChunkList()
//
// INVOCATION:
// setChunkList( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To remove any existing chunk objects associated with the dataset and
// then add new chunk objects for each contributor for this dataset
//
// DESCRIPTION:
// Check to see if the chunk list has anything in it and if it does delete
// and erase it.  Then for each contributor found in the contrib list
// create a chunk object and add it to that dataset object.
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

void		cDtsDatasetList::setChunkList
(
    cDtsStatus	&status 		// (mod) Returned function status.
)
{
    int                 numTokens;
    cDtsChunkInfo	*pChunk;
    char                *tokens[DTS_DATASET_NAME_LEN];


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Check to see if the chunk list has anything in it, and if
    //  it does delete and earse it from the list.
    //

    for ( cDtsChunkInfo::chunkIterator i = cdlChunkList.begin(); 
	    i != cdlChunkList.end(); i = cdlChunkList.begin() )
    {
	delete (*i).second;
	cdlChunkList.erase(i);
    }
	

    //
    //  For each string found in contrib list - create a chunk object
    //  then add it to the chunk list associatd with the dataset object.
    //

    char *tmp;
    tmp = strsav( cdlDs->contributors() );
    strtokens( tmp, DTS_MAX_CONTRIBUTORS, tokens, &numTokens );
    for ( int j = 0; j < numTokens; j++ )
    {
    
	checkNull( ( pChunk = new cDtsChunkInfo( tokens[j] )), 
		status, VOID ); 
	cdlChunkList[std::string(pChunk->name())] = pChunk;
	cdlNumChunks = 0;
    }
    free( tmp );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::updateObject
//
// INVOCATION:
// updateObject( status, datasetName, dsPtr );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// (>) pathName (char *) Path to dataset.
// (>) format (int) Data type.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To update the currently pointed to object
//
// DESCRIPTION:
// Update the object with the path, format and compression type.
// This is only valid for standalone mode.
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

void		cDtsDatasetList::updateObject
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*pathName,		// (in)  Path to dataset.
    int		format			// (in)  Data format sent.

)
{
    //
    //  Check the status.
    //

    checkStat( status, return );

    cdlPath = strsav( pathName );
    cdlFormat = format;
    cdlComp = DTS_FM_UNKNOWN;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsDatasetList::updateStatus
//
// INVOCATION:
// updateStatus( status, datasetName, dsPtr );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// (>) datasetName (char *) Dataset name.
// (>) dsPtr (cDtsDatasetList *) Pointer to the dataset table information.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To update the process table in the database
//
// DESCRIPTION:
// If the status was not ok, then the received state is put back to not
// received.  Then the database is updated.  Do not do database stuff
// if in standalone mode.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// This will fail if the dataset was never put in the database.
// Also will not work entirely correctly for datatypes of SDS (see
// comment below).
//-
//***********************************************************************
//

void		cDtsDatasetList::updateStatus
(
    cDtsStatus	&status,		// (mod) Returned function status.
    char	*datasetName,		// (in)  Dataset name.
    cDtsDatasetTable
		*dsPtr			// (in)  Ptr to the dataset table.
)
{
    //
    // WARNING, this will fail if the dataset was never put in.
    //

    if ( !status.ok() )
    {
	//
	//  If the status is not ok, then the data was not saved.
	// WARNING, this will not work for SDS structures, as part of the data
	// may have been saved, so should revert to the initial received state.
	// This has been put in code wise, but not tested.
	//

	dsPtr->dstReceived = DTS_DS_NOT_RECEIVED;
    }
    if ( !status.standAlone() )
    {
	cDtsDatasetTable::fileUpdate( status, datasetName, dsPtr );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::cDtsUniqueList
//
// INVOCATION:
// n.a. Constructor.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) uniqueName (char *) unique name.
// (>) dataNotified (long ) Data notified about this unique name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor
//
// DESCRIPTION:
// Save the passed in variables to the object created.  Then add the object
// to the unique name list.
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

		cDtsUniqueList::cDtsUniqueList
(
    char	*uniqueName,		// (in)  Unique name.
    long	dateNotified		// (in)  Date notified.
)
{
    culUniqueName = strsav( uniqueName );
    culDateNotified = (long) dateNotified;
    culUq = new cDtsUniqueTable( uniqueName, dateNotified );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::~cDtsUniqueList
//
// INVOCATION:
// n.a. Deconstructor.
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
// Deconstructor.  Free variables.
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
		cDtsUniqueList::~cDtsUniqueList
(
    void
)
{
    if ( culUniqueName != NULL )
    {
	free( culUniqueName );
    }

    if ( culUq != NULL )
    {
	delete culUq;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::add
//
// INVOCATION:
// cDtsUniqueList::add()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To add to the unique name list
//
// DESCRIPTION:
// Add this unique name object to the unique name list.
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

void	 cDtsUniqueList::add
(
    cDtsStatus	&status			// (mod) Status passed in.
)
{
    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Now add this to the unique list.
    //

     culUniqueList[ std::string( culUniqueName ) ] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::composeUName
//
// INVOCATION:
// composeUName( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Determine what the unique name counter and prefix should be
//
// DESCRIPTION:
// Compose the unique name prefix and get the counter from the database.
// This is called at startup and reset/init.
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

void		cDtsUniqueList::composeUName
(
    cDtsStatus	&status		// (mod) Function return status.
)
{
    long	t;		// Time.
    struct tm	*tm;		// Time structure.
    struct tm	tm1;		// Time structure.
    char	s[20];		// String.
    char	tmp[11];	// Another tmp string.
    int		num;		// Counter.
    char	*tmp2;		// Another tmp string.


    //
    //  Check the status.
    //

    checkStat( status, return );


    //
    //  Compose the prefix of the unique name.  Get the time, convert to
    //  local time.
    //

    t = time(NULL);
    tm = localtime_r( &t, &tm1 );
    if ( tm != NULL && 
	    ( tm->tm_hour >= 14 ) )
    {
	//
	//  It is after 14:00, so the day in the prefix is tomorrow's date.
	// JLU: Added less than 24 hours to avoid problems with DST

	t = t + ( 64800 ) ;
	tm = localtime_r(&t, &tm1);
    }


    //
    //  Compose the time part of the prefix into YYYYddMM.
    //

    strftime( s, sizeof(s), "%Y%m%d", tm );



    //
    //  If working standalone, then just search the list, otherwise
    //  search the database.
    //

    if ( status.standAlone() )
    {
	//
	//  Compose search string.
	//  Need to find the max number.  This returns max. num plus
	//  1, and we don't need that, we just need max num.
	//

	sprintf(tmp , "%c%s%c", cDtsLists::defaultLocation(), s, 
		cDtsLists::defaultSource() );

	findNameMax( status, tmp, &num );
	num++;
    }
    else
    {
	//
	//  Compose search string for search in the uniqueName table for 
	//  this string.  Then find it in the db.
	//

	sprintf(tmp , "%c%s%c%s", cDtsLists::defaultLocation(), s, 
		cDtsLists::defaultSource(), "%" );
	cDtsUniqueTable::uniqueNameMax( status, tmp, &num );
    }

    if ( status.ok() && num >= 0 )
    {
	//
	//  Worked ok, so have the number and unique prefix.
	//

	tmp2 = strsav( tmp );
	if ( !status.standAlone() ) tmp2[strlen(tmp2)-1] = '\0';
	cDtsLists::setUniquePrefix( tmp2, num );
	free( tmp2 );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::createUniqueName
//
// INVOCATION:
// cmdDhsObj.createUniqueName( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (<) newName (char **) New unique name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a new unique name and return
//
// DESCRIPTION:
// Determine if the unique name prefix has to be reset.  Then 
// generate the unique name as LYYYYmmddS###, where
// L = location from config file, eg. N or S
// YYYYmmdd = Is year, month, day, eg. 19970917
// S = source from config file, eg. O or S
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

void		cDtsUniqueList::createUniqueName
(
    cDtsStatus	&status,	// (mod) Function return status.
    char	**newName	// (out) New name.
)
{
    char	tmp[DTS_DATASET_NAME_LEN];
    long	t;
    struct tm	*tm;
    struct tm	tm1;		// Time structure.
    char	s[20];
    char	tmp2[20];
    int		num;


    //
    //  Check the status.
    //
    checkStat( status, return );

    cDtsUniqueList::ukNameLock();

    //
    //  Get the current date, subtract a day if after midnight and 
    //  before noon, see if its the same as the stored date.
    //

    t = time(NULL);
    tm = localtime_r( &t, &tm1 );
    if ( tm != NULL && 
	    ( tm->tm_hour >= 14  ) )
    {
	//
	//  It is after 14:00, so the day in the prefix is tomorrow's date.
	//

	t = t + ( 64800 ) ;
	tm = localtime_r(&t, &tm1 );
    }
    strftime( s, sizeof(s), "%Y%m%d", tm );
    (void) sprintf( tmp2, "%c%s%c", cDtsLists::defaultLocation(), s,
	    cDtsLists::defaultSource() );

    //
    //  Check against stored date, if not different, then increment the num.
    //  otherwise set the num back to 0
    //

    if (uniquePrefix() != NULL &&  strcmp( tmp2, uniquePrefix() ) == 0 )
    {
	num = nextPrefixNum();
    }
    else
    {
	setUniquePrefix( tmp2, 0 );
	num = nextPrefixNum();
    }
    sprintf( tmp, "%s%.4d", uniquePrefix(), num );
    *newName = strsav ( tmp );

    cDtsUniqueList::ukNameUnlock();

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::exit()
//
// INVOCATION:
// exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus & ) Function status.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all unique names in the list
//
// DESCRIPTION:
// Stops the label purse event loop and deletes and erase all 
// unique names in the list.
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

void	cDtsUniqueList::exit
(
    cDtsStatus	&status		// (mod) Function return status.
)
{


    checkStat( status, return );
 

    //
    //  Stop the label purge event loop.
    //

    culElExit = true;
    culElCond.broadcast(); 
    if (culElThread > 0) {pthread_join( culElThread, NULL );} // XXX allan: added check


    //
    //  Remove all labels in the list.
    //
	     
    for( iterator i = culUniqueList.begin(); i != culUniqueList.end();
		i = culUniqueList.begin() )
    {
	delete (*i).second;
	culUniqueList.erase( i );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::findNameMax
//
// INVOCATION:
// cDtsUniqueList::findNameMax
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
// To get the maximum number, given the name
//
// DESCRIPTION:
// To get the maximum number from the list, given the name.
// It searches for the internal list for name + - + #.  
// It looks for the largest number.
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

void	cDtsUniqueList::findNameMax
(
    cDtsStatus	&status,		// (mod) Function return status.
    char	*uniquePart,		// (in)  Name.
    int		*num			// (out) Maximum number.
)
{
    char	*tmp;			// Temporary string.
    int		itmp;			// Temporary number.
    int		largestNum;		// Largest number found.
    cDtsUniqueList	*pUqList;	// Ptr to unique list.


    //
    //  Check status, set variables.
    //

    checkStat( status, return );
    pUqList = NULL;
    itmp = 0;
    largestNum = 0;
  
    std::string strPrefix;	
    int numPrefix;

    if (fileReadLabelPrefix(strPrefix, numPrefix)) {                    // got the previous prefix 
	if (strstr(strPrefix.c_str(), uniquePart)) {                    // if same day continue the count where we left off
            *num = numPrefix;                                               
        }
        else {
            *num = 0;                                                   // no match (aka new day) - start from 0
        }
    }
    else {                                                              // no file - start from 0
        *num = 0;                                                       // count will be incremented 
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::init
//
// INVOCATION:
// init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To start the event loop to purge the unique list
//
// DESCRIPTION:
// To start the event loop to purge the unique list.
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

void cDtsUniqueList::init
(
    cDtsStatus	&status 		// (mod) Status passed in.
)
{

    //
    //  Start the labelList parse event loop.
    //

    culElExit = false;
    checkSystem( pthread_create( &culElThread, NULL, eventLoop, NULL ),
		    status, return );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDtsUniqueList::uniqueNameVerify
//
// INVOCATION:
// uniqueNameVerify( status, datasetName, uniqueName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cDtsStatus &) Function return status.
// (>) datasetName (char *) Name of the dataset.
// (<) uniqueName (char *) Name of the uniqueName.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To parse out the uniqueName porition of the datasetName and verify that
// there is prior knowledge of the uniqueName
//
// DESCRIPTION:
// To parse out the uniqueName porition of the datasetName and verify that
// there is prior knowledge of the uniqueName, otherwise add it to db.
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

void cDtsUniqueList::uniqueNameVerify
(
    cDtsStatus	&status,		// (mod) Status passed in.
    char	*datasetName,		// (in)  Dataset name.
    char	**uniqueName		// (in)  Unique name.
)
{
    iterator		i;		// Ptr to object in list.
    cDtsUniqueList	*pTemp;		// Ptr to new object.
    char		temp[DTS_DATASET_NAME_LEN];
    					// Temporary ptr.
    timespec		accessTime;	// Time for the next condition variable 
    					// timeout.



    //
    //  Check the status.
    //

    checkStat( status, return );
    pTemp = NULL;


    //
    //  Strip off the label - everything up to the first period.
    //

    int j = 0;
    temp[0]='\0';
    while ( 1 )
    {
	temp[j] = datasetName[j];
	if ( datasetName[j++] == '\0' || datasetName[j] == '.' )
	{
	    temp[j] = '\0';
	    break;
	}
    }
    if ( temp[0] == '\0' )
    {
	//
	//  Error, Datasetname name is null.
	//

	checkNull( *uniqueName = strsav( datasetName ), status, return );
    }
    else
    {
	checkNull( *uniqueName = strsav( temp ), status, return );
    }
	

    //
    //  First find out if its already in the list, and add it if not there.
    //

    i = culUniqueList.find( std::string( *uniqueName ) );

    if ( i == culUniqueList.end() )
    {
	long		dateNotified;


	//
	//  Do nothing if working standalone.
	//

	if ( !status.standAlone() )
	{
	    //
	    //  Not found in list, first see if its in the database.
	    //

	    cDtsUniqueTable::uniqueNameGet( status, *uniqueName, 
		    & dateNotified );

	    if ( status == status.S_NO_ROWS )
	    {
		//
		//  If its not in the database, then add it to the database.
		//

		dateNotified = time(NULL);
		cDtsUniqueTable::uniqueNamePut( status, *uniqueName, 
		    dateNotified );
	    }
	}
	checkStat( status, return );
	pTemp = new cDtsUniqueList( *uniqueName, dateNotified );


	//
	//  Add the one from the database or the new one to the list.
	//

	pTemp->add( status );
	status.S_UQ_LIST_ADD( status, *uniqueName );
    }
    else
    {
	//
	//  Already in list, 
	//

        pTemp = (*i).second;
    }


    //
    //  Update the last time accessed in the unique label.
    //

    clock_gettime( CLOCK_REALTIME, &accessTime );
    pTemp->culLastAccess = accessTime.tv_sec;
    return;
}
