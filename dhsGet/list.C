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
// dhsGet/src/list.C
//
// PURPOSE:
// Maintains a list of datasets to get.
//
// MEHTOD NAME(S)
// cGetList::readIn() 	- Reads all datasetnames from command line.
// cGetList::cGetList()	- Constructor.
// cGetList::~cGetList()- Deconstructor.  Deletes from list.
// cGetList::getNext() 	- Gets the next dataset off of the dataset list.
// cGetList::exit() 	- Exits and erases items from the list.
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
#include "getDhs.H"
#include "list.H"


//
//  Define locals
//

cGetList::tGetList	cGetList::cgtGetList;	// List of dataset names to get.



//
//***********************************************************************
//+
// FUNCTION NAME:
// cGetList::cGetList
//
// INVOCATION:
// cGetList::cGetList()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) tokens (char **) Lost of tokens in config file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor.  Add to the dataset list.
//
// DESCRIPTION:
// Save the datasetname to the 
// object created.  Then add the object to the dataset list.
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

		cGetList::cGetList
(
    const char  *datasetName		// (in) datasetname.
)
{
    cGetStatus		status;	// Status code.



    //
    //  Save the datasetname.
    //

    checkNull( cgtDatasetName = strsav( datasetName ), status, return );


    status.S_GET_ADD( status, cgtDatasetName );


    //
    //  Now add this datasetname to the dataset list.
    //

     cgtGetList[ string( cgtDatasetName ) ] = this;
    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cGetList::getNext
//
// INVOCATION:
// cGetList::getNext()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Return a pointer to the next dataset object in the list
//
// DESCRIPTION:
// Return a pointer to the next dataset object in the list.
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

cGetList	*cGetList::getNext
(
    cGetList::iterator	&i	//(mod)  Iterator.
)
{
    static bool		firstTime = TRUE;


    if ( firstTime )
    {
	//
	//  This is the first time, set the iterator to the beginning 
	//  of the list.
	//

	firstTime = FALSE;
	i =   cgtGetList.begin();
    }
    else
    {
	    i++;
    }


    //
    //  If its not the end of the list, then return a pointer to the 
    //  dataset object.
    //

    if ( i != cgtGetList.end() )
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
// cGetList::~cGetList
//
// INVOCATION:
// cGetList::cGetList()
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

		cGetList::~cGetList
(
    void 
)
{
    cGetStatus		status;	// Status code.


    //
    //  Erase all objects in the list.
    //
    
    status.S_GET_DEL( status, cgtDatasetName );

    cgtGetList.erase( cgtDatasetName );

}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cGetList::readIn
//
// INVOCATION:
// cGetList::readIn(  i &, argc, argv, status & )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) i ( int & ) Which argument number we are on.
// (>) argc (int) Number of argument.
// (>) argv (char*[]) Arguments.
// (!) status (cGetStatus &) Returned function status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the datasetnames from the command line
//
// DESCRIPTION:
// Read the datasetnames from the command line.
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

boolean	cGetList::readIn
(
    char	*datasetName,	// (in)  Dataset name in command line.
    cGetStatus	&status		// (mod) Function return status.
)
{
    cGetList	*pGet;		// Pointer to the newly created dataset object.
    bool	ret;		// Return value.


    //
    //  
    //

    if ( strlen( datasetName ) > 0 )
    {
	//
	//  This calls the constructor, which adds the datasetName to the list.
	//

        checkNull( pGet = new cGetList( datasetName ), status, 
		return( FALSE ) );
	ret = TRUE;
    }
    else
    {
	status.E_ARGUMENT_LINE( status, datasetName );
	status.display();
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cGetList::exit
//
// INVOCATION:
// cGetList::exit()
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

void		cGetList::exit
(
    cGetStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );
	
    while( cgtGetList.size() != 0 )
    {
	delete( (*cgtGetList.begin()).second );
    }

}
