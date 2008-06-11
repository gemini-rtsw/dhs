static char rcsid[] = "$Id: merger.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998				(c) 1998
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
// dhsStatus/src/merger.C
//
// PURPOSE:
// Contains the implementation of the cStaMerger class.
//
// METHOD NAME(S)
// cStaMerger::cStaMerger - Constructor for the class.
// cStaMerger::~cStaMerger - Destructor for the class.
// cStaMerger::exit - Clean up all objects of this class before exit.
// cStaMerger::put - Update the value of a merged channel.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:30:28  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/06/19 18:03:28  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.1  1998/05/12 20:26:46  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include "globals.H"
#include "staChannel.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMerger::cStaMerger
//
// INVOCATION:
// N.A. 
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the merged record.
// (>) numValues (int) Number of values in the values list.
// (>) values (char *[]) List of possible values for the record.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cStaMerger class
//
// This method initializes al; data members, and creates a cStaChannel
// object for the merged channels.
//
// DESCRIPTION:
// - Create a name for the merged channel.
// - Create a cStaChannel object for the merged channel.
// - Set up the array of known values the channel could have.
// - Create a list of cStaChannel objects being merged into this channel.
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

		cStaMerger::cStaMerger
(
    char	*name,		// (in)  Name of the merged record.
    int		numValues,	// (in)  Number of values in the values list.
    char	**values	// (in)  List of possible values for the record.
) : cListOf< cStaMerger, std::string, cStaStat >( name, true )
{
    int		i;
    cDhsSubsystem::iterator
    		j;
    cStaChannel	*pChannel;
    cStaStat	status;
    char	channelAlias[128];


    //
    // Create a name for the merged channel.
    //

    sprintf( channelAlias, "%s.merged", name );


    //
    // Create a new cStaChannel object for the merged channel.
    //

    cStaChannel::channelAdd( name, channelAlias, false, status );
    smPChannel = cStaChannel::find( "*", channelAlias, status );


    //
    // Set up the array of known values the merged channel can have.
    //

    smNumValues = numValues;
    smValues = new char *[ numValues ];
    for ( i = 0; i < numValues; i++ )
    {
	smValues[i] = strsav( values[i] );
    }


    //
    // Create the list of channels being merged into this channel.
    // This is all channels with the specified name, for all of the DHS
    // subsystems.
    //

    for ( j = cDhsSubsystem::begin(); j != cDhsSubsystem::end(); j++ )
    {
	check( pChannel = cStaChannel::find( (*j)->name(), name, status ),
	    status, continue );
	pChannel->scMerger = this;
	smChannelList.insert( cStaChannel::tChMap::value_type( 
		cStaChannel::cKey( name, (*j)->name() ), pChannel ) );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMerger::~cStaMerger
//
// INVOCATION:
// N.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for the cStaMerger class
//
// DESCRIPTION:
// Free the resources allocated to this object.
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

		cStaMerger::~cStaMerger
(
    void
)
{
    int		i;


    for ( i = 0; i < smNumValues; i++ )
    {
	free( smValues[i] );
    }
    delete [] smValues;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMerger::exit
//
// INVOCATION:
// cStaMerger::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Clean up the cStaMerger class before exit
//
// DESCRIPTION:
// Delete every object of class cStaMerger.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// All objects of the class are assumed to be allocated on the heap.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cStaMerger::exit
(
    cStaStat	&status	// (mod) Function return status.
)
{
    iObjectList	i;


    checkStat( status, return );


    for ( i = begin(); i != end(); i = begin() )
    {
	delete (*i).second;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMerger::priority
//
// INVOCATION:
// merger.priority( string );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string (char *) The string to prioritize.
//
// FUNCTION VALUE:
// (int) The priority of the string.
//
// PURPOSE:
// Given a string, determine where it falls in a cStaMerger's priority list
//
// The method assumes that higher priorities will occur first in the list.
// The method assumes that strings not in the list of strings have lowest
// priority.
//
// DESCRIPTION:
// Check each string in the list of know strings agains the given string.
// The priority of the string is the index of the first matching string.
// If no known string matches the given string, return the lowest priority
// + 1.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// The priority number are backwards.
//-
//***********************************************************************
//

int		cStaMerger::priority
(
    const char	*string		// (in)  The string to prioritize.
)
{
    int		i;

    for ( i = 0; i < smNumValues; i++ )
    {
	if ( strcmp( string, smValues[i] ) == 0 )
	{
	    return( i );
	} 
    }

    return( smNumValues );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMerger::put
//
// INVOCATION:
// merger.put( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) newChannel (cStaChannel *) The channel modified.
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// If appropriate, update the value of a merged channel
//
// DESCRIPTION:
// - Set the default merged channel value to the new channel value.
// - Look for a merged channel with a higher priority value, and if
//   found, assign that value to the merged channel.
// - Update the merged channel value.
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

void		cStaMerger::put
(
    cStaChannel	*newChannel,	// (mod) The new channel data.
    cStaStat	&status		// (mod) Function return status.
)
{
    int		j;
    cStaChannel::iChMap
    		i;
    bool	update = false;
    cStaChannel	*pChannel;
    int		iNewValue;
    char	*newValue;
    char	*newMessage;


    checkStat( status, return );


    cStaChannel::scMutex.lock();


    //
    // The default new values for the merged channel is the values of the
    // channel that was just changed. This should only come into play when
    // the value is not one of the known values, or if the value is the
    // same as the previous value.
    //

    newValue = newChannel->scData;
    newMessage = newChannel->scMessage;
    iNewValue = priority( newValue );


    //
    // Check to see if the value of any other of the merged channels has
    // a higher priority than the new value.
    //

    for ( i = smChannelList.begin(); i != smChannelList.end(); i++ )
    {
	pChannel = (*i).second;
	j = priority( pChannel->scData );
	if ( j < iNewValue )
	{
	    //
	    // A higher priority value was found, put this into the merged
	    // channel's value.
	    //

	    iNewValue = j;
	    newValue = pChannel->scData;
	    newMessage = pChannel->scMessage;
	} 
    }

    cStaChannel::scMutex.unlock();
    

    //
    // Update the merged channel value.
    //

    smPChannel->put( newValue, newMessage, status );
}
