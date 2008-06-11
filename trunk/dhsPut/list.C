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
// dhsPut/src/list.C
//
// PURPOSE:
// Whatever
//
// MEHTOD NAME(S)
// cPutList::readIn() 	- Reads all file names from command line.
// cPutList::cPutList()	- Constructor.
// cPutList::~cPutList()- Deconstructor.  Deletes from list.
// cPutList::getNext() 	- Gets the next file name off of the file list.
// cPutList::setLast() 	- Set the last flag to true on the last filename.
// cPutList::setAllDSNames()- Set the datasetname for the files.
// cPutList::exit() 	- Exits and erases items from the list.
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_config.h"
}

#include "globals.H"
#include "putDhs.H"
#include "list.H"


//
//  Define locals
//

cPutList::tPutList	cPutList::cptPutList;	// List of files.


//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutList::cPutList
//
// INVOCATION:
// cPutList::cPutList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) filename (char *) filename.
// (>) fileId (char *) fileid.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor.  Add to the file list.
//
// DESCRIPTION:
// Save the filename to the 
// object created.  Then add the object to the file  list.
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

		cPutList::cPutList
(
    const char  *fileName	// (in) File name.
)
{
    cPutStatus		status;	// Status code.



    //
    //  Save the filename.
    //

    checkNull( cptFilename = strsav( fileName ), status, return );
    cptLastFlag = FALSE;

    status.S_PUT_ADD( status, cptFilename );


    //
    //  Now add this filename to the put list.
    //

     cptPutList[ std::string( cptFilename ) ] = this;
    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutList::getNext
//
// INVOCATION:
// cPutList::getNext()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) i (cPutList::iterator) Iterator into filename put list.
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

cPutList	*cPutList::getNext
(
    cPutList::iterator	&i,		//(mod)  Iterator.
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
	i =   cptPutList.begin();
    }
    else
    {
	    i++;
    }


    //
    //  If its not the end of the list, then return a pointer to the 
    //  object.
    //

    if ( i != cptPutList.end() )
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
// cPutList::setAllDSNames
//
// INVOCATION:
// cPutList::setAllDSNames()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set all dataset names to where ever the -d option was, if given.
//
// DESCRIPTION:
// Set all dataset names to where ever the -d option was, if given.
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

void	cPutList::setAllDSNames
(
    cPutStatus	&status		// (mod) Function return status.
)
{
    cPutList::iterator	i;	// Ptr to object in list.
    cPutList		*pTemp;	// Ptr to new object.
    char		*tmp;	// Temporary ptr to ds name.


    checkStat( status, return );

    for ( i = cptPutList.begin(); i != cptPutList.end(); i++ )
    {
	pTemp = (*i).second;
	if ( (tmp = putDhs::getDSName())  == NULL)
	{
	    if ( ( tmp = strrchr( pTemp->getName(), '/' ) ) == NULL )
	    {
		tmp = pTemp->getName();
	    }
	    else
	    {
		++tmp;
	    }
	}
	pTemp->cptFileId = tmp;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutList::setLast
//
// INVOCATION:
// cPutList::setLast()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the last flag on the last item in the put list.
//
// DESCRIPTION:
// Set the last flag on the last item in the put list.
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

void	cPutList::setLast
(
    cPutStatus	&status		// (mod) Function return status.
)
{
    cPutList::iterator	i;	// Iterator.
    cPutList	*tempPtr;

    checkStat( status, return );

    if ( putDhs::getDSName() == NULL )
    {
	//
	// The dataset name has not been set, so every file will be sent as
	// a separate datsaet.
	//

	for ( i = cptPutList.begin(); i != cptPutList.end(); i++ )
	{ 
	    tempPtr = (*i).second;
	    tempPtr->cptLastFlag = TRUE;
	}
    }
    else
    {
	//
	//  The dataset name was set, so the files are chunks of one datset.
	//  If the last thing to be sent is NOT a zero buffer, then
	//  Point the iterator at the end of the list then decrement to
	//  go to the last element.  Set last flag to true.
	//
	
	i =   cptPutList.end();
	if ( i != cptPutList.begin() && !putDhs::buffZero()  )
	{
	    //
	    //  Only do this if there is something in the list.
	    //

	    i--;
	    tempPtr = (*i).second;
	    tempPtr->cptLastFlag = TRUE;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutList::~cPutList
//
// INVOCATION:
// cPutList::cPutList()
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

		cPutList::~cPutList
(
    void 
)
{
    cPutStatus		status;	// Status code.


    //
    //  Erase all objects in the list.
    //
    
    status.S_PUT_DEL( status, cptFilename );

    cptPutList.erase( cptFilename );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutList::readIn
//
// INVOCATION:
// cPutList::readIn( filename, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) filename (char *) Name of the file.
// (!) status ( cPutStatus & ) Return method status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the filename from the command line
//
// DESCRIPTION:
// Read the filename from the command line.
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

boolean	cPutList::readIn
(
    char	*filename,	// (in)  Filename name in command line.
    cPutStatus	&status		// (mod) Function return status.
)
{
    cPutList	*pPut;		// Pointer to the newly created object.
    bool	ret;		// Return value.


    checkStat( status, return( FALSE ) );


    if ( strlen( filename ) > 0 )
    {
	//
	//  Then call the constructor, which adds the filename to the list.
	//

	checkNull( pPut = new cPutList( filename ), status, 
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_ARGUMENT_LINE( status, filename );
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutList::exit
//
// INVOCATION:
// cPutList::exit()
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

void		cPutList::exit
(
    cPutStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );
	
    while( cptPutList.size() != 0 )
    {
	delete( (*cptPutList.begin()).second );
    }

}
