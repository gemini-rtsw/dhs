static char rcsid[] = "$Id: qlsFitsKeyword.C,v 1.1.1.1 2002-11-24 20:29:15 brighton Exp $";
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
// dhsQlServer/src/qlsFitsKeyword.C
//
// PURPOSE:
// Contains the implementation of the cQlsFitsKeyword class.
//
// METHOD NAME(S)
// cQlsFitsKeyword::cQlsFitsKeyword - Constructor for the cQlsFitsKeyword class.
// cQlsFitsKeyword::~cQlsFitsKeyword - Destructor for the cQlsFitsKeyword class.
// cQlsFitsKeyword::operator< - Comparison operator for lists of 
//		cQlsFitsKeyword objects.
// cQlsFitsKeyword::config - Read FITS keyword information from the 
//		configuration file.
// cQlsFitsKeyword::exit - Delete all existing cQlsFitsKeyword objects.
// cQlsFitsKeyword::find - Find a cQlsFitsKeyword object by keyword name.
// cQlsFitsKeyword::fitsKeyword - Process a fitsKeyword line from the 
//		configuration file.
// cQlsFitsKeyword::strtobool - convert a character string to a boolean value.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/15 22:51:52  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.2  1997/08/20 21:24:04  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#include "globals.H"
#include "qlsFitsKeyword.H"


//
// Define the static data members of the cQlsFitsKeyword class.
//

cQlsFitsKeyword::tKeywordList	
		cQlsFitsKeyword::qfkKeywordList;
cQlsFitsKeyword	*cQlsFitsKeyword::qfkPDefaultKwd = NULL;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::cQlsFitsKeyword
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the FITS keyword.
// (>) mainHeader (bool) Is the keyword from the main header.
// (>) firstChunk (bool) Copy the keyword from the first chunk.
// (>) nextChunk (bool) Copy the keyword from any data chunk.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a cQlsFitsKeyword object
//
// The only valid way to create objects of this class is via the 
// cQlsFitsKeyword::config function.
//
// DESCRIPTION:
// - Initialize the data members.
// - Add the keyword to the static list of all keywords.
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

		cQlsFitsKeyword::cQlsFitsKeyword
(
    char	*name,		// (in)  Name of the FITS keyword.
    bool	mainHeader,	// (in)  Is the keyword from the main header.
    bool	firstChunk,	// (in)  Copy the keyword from the first chunk.
    bool	nextChunk	// (in)  Copy the keyword from any data chunk.
)
{
    //
    // Initialize the data members.
    //

    qfkFirstChunk = firstChunk;
    qfkMainHeader = mainHeader;
    qfkNextChunk = nextChunk;
    qfkName = strsav( name );


    //
    // Add the keyword to the list of keywords.
    //

    qfkKeywordList[ cFKWKey( qfkName, qfkMainHeader ) ] = this;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::~cQlsFitsKeyword
//
// INVOCATION:
// n.a. Called when an object is destroyed.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for cQlsFitsKeyword objects
//
// DESCRIPTION:
// - Remove the object from the list of all cQlsFitsKeyword objects.
// - Free memory allocated for the object.
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

		cQlsFitsKeyword::~cQlsFitsKeyword
(
    void
)
{
    iKeywordList	i;


    //
    // Find the object in the list of cQlsFitsKeyword objects.
    //

    i = qfkKeywordList.find( cFKWKey( qfkName, qfkMainHeader ) );


    //
    // Remove the object from the list of objects.
    //

    if ( i != qfkKeywordList.end() )
    {
	qfkKeywordList.erase( i );
    }


    //
    // Free the memory allocated to the object.
    //

    gen_free( qfkName );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::cFKWKey::operator<
//
// INVOCATION:
// truth = kwk1 < kwk2;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fkwd (const cQlsFitsKeyword::cFKWKey &) The keyword key to compare
//
// FUNCTION VALUE:
// (bool) True if *this < fkwd
//
// PURPOSE:
// Less than operator for the cQlsFitsKeyword::cFKWKey class
//
// DESCRIPTION:
// Compare the keyword names first, and then the fkwkMainHeader member.
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

bool		cQlsFitsKeyword::cFKWKey::operator<
(
    const cQlsFitsKeyword::cFKWKey &
		fkwd		// (in)  The keyword to compare.
) const
{
    int		ret;		// The value returned by strcmp.


    //
    // Compare the keyword names.
    //

    if ( ( ret = strcmp( fkwkName, fkwd.fkwkName ) ) == 0 )
    {
	//
	// If the keyword names are the same, compare the fkwkMainHeader
	// values.
	//

	if ( fkwkMainHeader && ! fkwd.fkwkMainHeader )
	{
	    return( true );
	}
	else
	{
	    return( false );
	}
    }
    else
    {
	return( ret < 0 );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::config
//
// INVOCATION:
// cQlsFitsKeyword::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the fits keyword configuration information from the configuration
// file
//
// DESCRIPTION:
// - Call config_get to process all "fitsKeyword" lines in the configuration 
//   file.
// - Create a default cQlsFitsKeyword object to handle objects not in the
//   configuration file.
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

void		cQlsFitsKeyword::config
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    //
    // Read the "fitsKeyword" lines from the configuration file.
    //

    checkConfig( config_get( "fitsKeyword", fitsKeyword ), status, return );


    //
    // Create a default object to handle keywords not in the config file.
    //

    qfkPDefaultKwd = new cQlsFitsKeyword( "*", true, true, true );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::exit
//
// INVOCATION:
// cQlsFitsKeyword::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all cQlsFitsKeyword objects
//
// DESCRIPTION:
// Loop through all cQlsFitsKeyword objects in the qfkKeywordList data member,
// deleting all of them.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// This isn't done with any mutexes locked, but that shouldn't be a problem
// since this should only be executed when the system is reseting or shutting
// down.
//-
//***********************************************************************
//

void		cQlsFitsKeyword::exit
(
    cQlsStatus	&		// (mod) Function return status.
)
{
    iKeywordList	i;


    //
    // Loop through every object in the qfkKeywordList data member, deleting 
    // them.
    //

    for ( i = qfkKeywordList.begin(); i != qfkKeywordList.end(); 
	    i = qfkKeywordList.begin() )
    {
	delete (*i).second;
    }


    qfkPDefaultKwd = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::find
//
// INVOCATION:
// pKwd = cQlsFitsKeyword::find( "KEYWORD", true, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) keywordName (const char *) Name of the keyword.
// (>) inMain (bool) Keyword is in the main header.
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// (cQlsFitsKeyword *) A pointer to the object found.
//
// PURPOSE:
// Locate a cQlsFitsKeyword object based on is name and whether it
// is in a main header or extension header
//
// If an object matching the specific keyword cannot be found, the default
// cQlsFitsKeyword object is returned.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The fits keywords must have been read from the configuration file with
// the cQlsFitsKeyword::config function.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

cQlsFitsKeyword	*cQlsFitsKeyword::find
(
    const char	*keywordName,	// (in)  Name of the keyword to find.
    bool	inMain,		// (in)  Is the keyword in the main header?
    cQlsStatus	&status		// (mod) Function return status.
)
{
    iKeywordList	i;


    checkStat( status, return( NULL ) );


    //
    // Try to find the matching cQlsFitsKeyword object in the STL map.
    //

    if ( ( i = qfkKeywordList.find( cFKWKey( keywordName, inMain ) ) ) == 
	    qfkKeywordList.end() )
    {
	//
	// No match found, return the default object.
	//

	return( qfkPDefaultKwd );
    }
    else
    {
	//
	// Return the pointer to the object.
	//

	return( (*i).second );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFitsKeyword::fitsKeyword
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens from the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (boolean) True if the function succeeded, false otherwise.
//
// PURPOSE:
// Parse a fitsKeyword line from the configuration file
//
// This function is called once for each fitsKeyword line in the file.
//
// DESCRIPTION:
// Parse the configuration line.
//
// The configuration lines should look like:
// fitsKeyword	<name> <is in main HDU> <copy from first> <copy from non first>
// where:
//	<name>			This is the FITS keyword name.
//	<is in main HDU>	True if the data refers to a keyword in the
//				main HDU. This allows keywords in the main 
//				HDU to be treated differently from keywords 
//				in extensions.
//	<copy from first>	Is true if the keyword should be copied from
//				the first chunk of a dataset.
//	<copy from non first>	Is true if the keyword should be copied when
//				it occurs in a chunk other than the first.
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

boolean		cQlsFitsKeyword::fitsKeyword
(
    int		numTokens,	// (in)  Number of tokens on the configuration 
				//	 line.
    char	**tokens	// (in)  Tokens on the configuration line.
)
{
    boolean	ret;		// The return value.
    cQlsStatus	status;		// Status value.


    ret = TRUE;


    //
    // Check that the line contains the correct number of tokens.
    //

    if ( numTokens == 5 )
    {
	//
	// Check to make sure the keyword isn't already in the list.
	//

	if ( qfkKeywordList.find( cFKWKey( tokens[1], 
		strtobool( tokens[2] ) ) ) == qfkKeywordList.end() )
	{
	    //
	    // Create a new cQlsFitsKeyword object for the keyword.
	    //

	    checkNull( new cQlsFitsKeyword( tokens[1], strtobool( tokens[2] ), 
		    strtobool( tokens[3] ), strtobool( tokens[4] ) ), 
		    status, ret = FALSE );
	}
	else
	{
	    //
	    // The keyword is already in the list, which is an error.
	    //

	    status.E_DUP_FITS_KW( status, tokens[1], tokens[2] );
	}
    }
    else
    {
	//
	// Wrong number of tokens. The format of the file is incorrect.
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
// cQlsFitsKeyword::strtobool
//
// INVOCATION:
// val = cQlsFitsKeyword::strtobool( str );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) str (char *) String to be converted to boolean.
//
// FUNCTION VALUE:
// (bool) The boolean value of the string.
//
// PURPOSE:
// Return true if the string contains a positive word, false otherwise
//
// Positive words are "yes" or "true", with the case ignored.
//
// DESCRIPTION:
// Trivial.
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

bool		cQlsFitsKeyword::strtobool
(
    char	*str		// (in)  String to be converted to boolean.
)
{
    if ( strcasecmp( str, "yes" ) == 0 || strcasecmp( str, "true" ) == 0 )
    {
	return( true );
    }
    else
    {
	return( false );
    }
}
