static char rcsid[] = "$Id: lists.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 2000				(c) 2000
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
// dhsStoPut/src/lists.C
//
// PURPOSE:
// Defines the methods for the two lists used by dhsStoPut.  The lists
// are the list of filenames and the list of media types.
//
// MEHTOD NAME(S)
// cStoPutList::cStoPutList()	- Constructor.
// cStoPutList::~cStoPutList()	- Deconstructor.  Deletes from list.
// cStoPutList::readIn() 	- Reads file names from command line.
// cStoPutList::exit() 		- Exits and erases items from the list.
// cStoPutList::findFile	- Find a given file in the list.   
// cStoPutList::getNext() 	- Gets the next file off of the file list.
// cMediaTypeList::cMediaTypeList()	
//				- Constructor.
// cMediaTypeList::~cMediaTypeList()	
//    				- Deconstructor.  Deletes from list.
// cMediaTypeList::config() 	- Reads media type information from
//				  the dhsMediaType configuration file.	
// cMediaTypeList::findDhsMediaType() 	
//				- Finds the corresponding DHS media
//				  type given a retrieval media type.
// cMediaTypeList::exit()	- Exits and erases items from the list.//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:05  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
}

#include "globals.H"
#include "stoPutDhs.H"
#include "lists.H"


//
//  Define locals
//

#define	STO_CFG_MEDIATYPE	"mediaType"

cStoPutList::tStoPutList	cStoPutList::cptStoPutList;		// List of files.
cMediaTypeList::tMediaTypeList	cMediaTypeList::cptMediaTypeList;	// List of media types.

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutList::cStoPutList
//
// INVOCATION:
// lStoPutList = new cStoPutList( srcFilename, destFilename, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) srcFilename 	(char *) 		location of the file.
// (>) destFilename 	(char *) 		filename on media.
// (!) status		(cStoPutStatus &)	Function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor.  Add to the file list.
//
// DESCRIPTION:
// Save the source and destination filenames to the 
// object created.  Construct the file ID for this file
// Then add the object to the file  list.
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

	cStoPutList::cStoPutList
(
    const char      *srcFileName,	// (in)  Source file name.
    const char      *destFileName,	// (in)  Destination file name.
    cStoPutStatus   &status		// (mod) Status code.
)
{
    char 	*destFile;	// Local destination filename.
    char	*extension;	// File extension.
    char 	*fileID;	// File ID for this file.
    struct stat	statBuffer;	// Output from stat().


    checkStat( status, return );
    

    //
    // Stat the file making sure it really exists and to extract
    // the filesize.
    //
    
    if ( stat( srcFileName, &statBuffer ) == -1 )
    {
	//
	//  File doesn't exit. Error out.
	//

		status.E_NO_FILE( status, srcFileName );
		return;
    }
    cptFileSize = statBuffer.st_size;
        

    //
    //  Save the source filename.
    //

    checkNull( cptSrcFilename = strsav( srcFileName ), status, return );


    //
    // Save the destination filename.  If no destination file name
    // was given (destFileName == NULL) then the destination file name
    // is the root name of the srcFileName.
    //

    if ( destFileName == NULL )
    {
	checkNull( destFile = strtail( srcFileName ), status, return );
    }
    else
    {
	checkNull( destFile = strsav( destFileName ), status, return );
    }
    checkNull( cptDestFilename = strsav( destFile ), status, return );


    //
    // Construct the File ID from the source file name.  The file ID
    // is the root name with compression extensions stripped of.
    //

    checkNull( fileID = strtail( destFile ), status, return );
    if ( ( extension = strext( fileID ) ) != NULL 
	&& streq( extension, "gz" ) )
    {
	//
	// Strip the extension off of the file ID.
	//

	strroot( fileID );

    }
    checkNull( cptFileId = strsav( fileID ), status, return );


    //
    // Add this filename to the list.
    //

    status.S_PUT_ADD( status, cptSrcFilename );
    cptStoPutList[ std::string( cptSrcFilename ) ] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutList::~cStoPutList
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor. Erase all objects in the list. 
//
// DESCRIPTION:
// Erase all objects in the list and free both chars.
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

		cStoPutList::~cStoPutList
(
    void 
)
{
    cStoPutStatus		status;	// Status code.


    //
    //  Erase all objects in the list.
    //
    
    status.S_PUT_DEL( status, cptFileId );
    cptStoPutList.erase( cptFileId );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutList::readIn
//
// INVOCATION:
// cStoPutList::readIn( srcFilename, destFilename, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) srcFile	(char *) 	    File to be sent.
// (>) destFile	(char *) 	    Location of the file on media.
// (!) status 	(cStoPutStatus &)   Function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the filename from the command line
//
// DESCRIPTION:
// Add the new file to the list of files.
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

void	cStoPutList::readIn
(
    char	*srcFileName,	// (in)  Source file name in cmd. line.
    char	*destFileName,	// (in)  Destination file name in cmd. line.
    cStoPutStatus
    		&status		// (mod) Function return status.
)
{
    cStoPutList	*pStoPut;	// Pointer to the newly created object.
    bool	ret;		// Return value.


    checkStat( status, return );


    if ( strlen( srcFileName ) > 0 )
    {
	//
	//  Then call the constructor, which adds the file to the list.
	//

	checkNull( pStoPut = new cStoPutList( srcFileName, destFileName,
		status ), status, return );
	ret = TRUE;
    }
    else
    {
	if ( destFileName != NULL )
	{
	    status.E_CMD_LINE_FILES( status, srcFileName, destFileName );
	}
	else
	{
	    status.E_CMD_SRC_FILE( status, srcFileName );
	}
    }

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutList::exit
//
// INVOCATION:
// cStoPutList::exit()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (clStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all connection objects.
//
// DESCRIPTION:
// While the size of the list is not 0, delete from the list.
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

void		cStoPutList::exit
(
    cStoPutStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );
	
    while( cptStoPutList.size() != 0 )
    {
	delete( (*cptStoPutList.begin()).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutList::findFile
//
// INVOCATION:
// cStoPutList::findFile( srcFileName )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) srcFilename (char *) filename.
//
// FUNCTION VALUE:
//    cStoPutList	(cStoPutList*)	// The instance found in the list.
//    cStoPutList	NULL		// If not found.
//
// PURPOSE:
// Return a pointer to the object with the matching srcFileName.
//
// DESCRIPTION:
// Return a pointer to the object with the matching srcFileName.
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
cStoPutList	*cStoPutList::findFile
(
    const char      *srcName	// (in)  Source file name.
)
{
    iterator	i;
    
    if (  cptStoPutList.empty()  )
    {
	//
	// There is nothing in the list so return a NULL pointer.
	//
	
	return( NULL );
    }


    //
    // Try and find the given file.
    //
    
    i = cptStoPutList.find( std::string( srcName ) );
    if ( i == cptStoPutList.end() )
    {
	return( NULL );
    }
    else
    {
	return( (*i).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutList::getNext
//
// INVOCATION:
// cStoPutList::getNext( i, firstTime )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) i (cStoPutList::iterator) Iterator into filename put list.
// (>) firstTime (bool) Indicates first time into this.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Return a pointer to the next  object in the list
//
// DESCRIPTION:
// Return a pointer to the next object in the list.
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

cStoPutList	*cStoPutList::getNext
(
    cStoPutList::iterator
			&i,		//(mod)  Iterator.
    bool		firstTime	// (in)  True for first time in.
)
{
    if ( firstTime )
    {
	//
	//  This is the first time, set the iterator to the beginning 
	//  of the list.
	//

	firstTime = FALSE;
	i =   cptStoPutList.begin();
    }
    else
    {
	    i++;
    }


    //
    //  If its not the end of the list, then return a pointer to the 
    //  object.
    //

    if ( i != cptStoPutList.end() )
    {
	return( (*i).second );
    }
    else
    {
	return ( NULL );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMediatypeList::cMediatypeList
//
// INVOCATION:
// pMediatypeList = new cMediatypeList( retMediaType, dhsMediaType, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) retMediaType	(char *)	    Retrieval media type.
// (>) dhsMediaType	(char *)	    DHS media type.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor.  Add to the media type list.
//
// DESCRIPTION:
// Save the retrieval and DHS media types to the newly
// created object.  The add the object to the media list.
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

	cMediaTypeList::cMediaTypeList
(
    const char      *retMediaType,	// (in)  Source file name.
    const char      *dhsMediaType,	// (in)  Destination file name.
    cStoPutStatus   &status		// (mod) Status code.
)
{
    checkStat( status, return );

    
    //
    // Verify that we don't already have the given media type.
    //

    if ( findDhsMediaType( retMediaType ) != NULL )
    {
	status.E_DUP_TYPE( status, retMediaType );
	return;
    }
    
    
    //
    //  Save the retrieval and DHS media type
    //

    checkNull( cptRetMediaType = strsav( retMediaType ), status, return );
    checkNull( cptDhsMediaType = strsav( dhsMediaType ), status, return );
    

    //
    //  Now add to the media type list.
    //

    status.S_MEDIATYPE_ADD( status, retMediaType, dhsMediaType );
    cptMediaTypeList[ std::string( cptRetMediaType ) ] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMediatypeList::~cMediatypeList
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor. Erase all objects in the list. 
//
// DESCRIPTION:
// Erase all objects in the list and free both chars.
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

		cMediaTypeList::~cMediaTypeList
(
    void 
)
{
    cStoPutStatus		status;	// Status code.


    //
    //  Erase all objects in the list.
    //
    
    status.S_MEDIATYPE_DEL( status, cptRetMediaType, cptDhsMediaType );
    cptMediaTypeList.erase( cptRetMediaType );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMediatypeList::exit
//
// INVOCATION:
// cMediatypeList::exit()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (clStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all medit type objects.
//
// DESCRIPTION:
// While the size of the list is not 0, delete from the list.
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

void		cMediaTypeList::exit
(
    cStoPutStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );
	
    while( cptMediaTypeList.size() != 0 )
    {
	delete( ( *cptMediaTypeList.begin() ).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMediatypeList::config
//
// INVOCATION:
// cMediatypeList::config()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoPutStatus & )	- Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Perform run-time configuration of some of the media type.
// parameters.
//
// DESCRIPTION:
// Get the list of media types from the configuration file.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Configuration file needs to be open, i.e. config_open has been called.
//
// SEE ALSO:
// configInit()
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cMediaTypeList::config
(
    cStoPutStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

    checkConfig( config_get( STO_CFG_MEDIATYPE, mediaTypeConfig ),
	status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cMediaTypeList::config
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens    (int)    	Number of tokens in the tokens array.
// (>) tokens	    (char **) 	The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse the configuration file lines with the keyword "mediaType"
// from the configuration file, storing the information found.
//
// DESCRIPTION:
// Verify that the number of tokens given is correct.  Create a
// new mediaTypeList object with the tokens given and put it on the
// list.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// cDhsStoPut::config
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cMediaTypeList::mediaTypeConfig
(
    int         numTokens,      // (in)  Number of tokens.
    char        **tokens        // (in)  list of tokens.
)
{
    boolean     	ret;            // Function return value.
    cMediaTypeList	*pMediaTypeList;	// Pointer to the new object.
    cStoPutStatus  	status;         // Status code.


    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 3 )
    {
	checkNull( pMediaTypeList = new cMediaTypeList( tokens[1], 
		tokens[2], status ), status, return( FALSE ) );
	if ( status.ok() )
	{
	    ret = TRUE;
	}
	else
	{
	    ret = FALSE;
	}
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
// cMediaTypeList::findDhsMedType
//
// INVOCATION:
// cStoPutList::findDhsMediaType( mediaType );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaType    (const char *)	stoPut media type to find.
//
// FUNCTION VALUE:
// (char *)	dhsMediaType	The DHS mediaType paired with the
//				given retrieval mediaType.
// (char *)	NULL		If no match is found.
//
// PURPOSE:
// To find the dhsMediaType that is associated with the given
// retrieval media type.
//
// DESCRIPTION:
// Search the list of media types looking for the given retrieval media
// type.  If a match is found return it, if not return NULL.
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

const char *		cMediaTypeList::findDhsMediaType
(
    const char 	    *mediaType	// (in)  dhsStoPut media type to find.
)
{
    iterator	i;
    cMediaTypeList
		*pMediaTypeList; // The mediaType found.

    if ( cptMediaTypeList.empty() )
    {
	//
	// There is nothing in the list so return a NULL pointer.
	//

	return( NULL );
    }


    //
    // Try and find the given media type.
    //
    
    i = cptMediaTypeList.find( std::string( mediaType ) );
    if ( i == cptMediaTypeList.end() )
    {
	return( NULL );
    }
    else
    {
	pMediaTypeList = (*i).second;
	return( pMediaTypeList->getDhsMediaType() );
    }
}

