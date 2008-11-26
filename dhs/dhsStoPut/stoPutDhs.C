static char rcsid[] = "$Id: stoPutDhs.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// dhsStoPut/src/stoPutDhs.C
//
// PURPOSE:
// Class methods to handle the dhs library startup, init, and errors.
//
// METHOD NAME(S)
// stoPutDhsError::error	- Error handler.
//    
// cStoPutDhsCmd::done		- Command completion handler.
//
// stoPutDhs::~stoPutDhs	- Deconstructor.
// stoPutDhs::exit		- Get rid of dhs handlers and objects.
// stoPutDhs::init		- Setup handlers and init dhs library.
// stoPutDhs::kill		- Force an exit.
// stoPutDhs::start		- Start the event loop.
// stoPutDhs::config		- Read configuration data.
// stoPutDhs::defaultConfig	- Retrieve the default values for the
//				  command-line options from the config
//				  file.
// stoPutDhs::identityConfig	- Retrieves the dhsStoClient IMP identity
//				  string from dhsStoPut.config
// stoPutDhs::organizationConfig- Retrieves the organization name from
//				  dhsStoPut.config.
// stoPutDhs::storageConfig	- Retrieves the Storage server identity
//				  and IP address from dhsStoPut.config
// stoPutDhs::setArchiveStream	- Set the archive stream to use.
// stoPutDhs::setCompression	- Set the type of file compression to use.
// stoPutDhs::setMediaType	- Set the type of media to be written.
// stoPutDhs::setIdentity	- Set the identity of the dhsStoPut client.
// stoPutDhs::setStoIdentity	- Set the identity of the Storage Server.
// stoPutDhs::setStoIpAddr	- Set the machine (IP Address) where
//				- the Storage Server is running.
// stoPutDhs::setUnitPrefix	- Set the unit name prefix string.
// stoPutDhs::setUserStream	- Set the user data stream to use.
// stoPutDhs::archiveFileAvail	- Informs the Storage Server that a
//				  file is available for an archive stream.
// stoPutDhs::userMediaRequest	- Initiates a user media request with
//				  Storage Server.
// putFile::putFile		- Constructor for the cPutFile class.
// putFile::done		- Put completion handler.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:15  brighton
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

#include <iostream>


#include "globals.H"
#include "stoPutDhs.H"
#include "lists.H"
extern "C"
{
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <netdb.h>
#include "dhs.h"
#include "gen_types.h"
#include "gen_config.h"
}

    
//***********************************************************************
//    			Local defines.
//***********************************************************************
    
#define	STOPUT_CFG_IDENTITY	"identity"
#define	STOPUT_CFG_STORAGE	"storageServer"
#define	STOPUT_CFG_ORG		"organization"
#define	STOPUT_CFG_DEFAULTS	"defaults"

#define STO_CMD_ARCHIVE_FILE_AVAIL  "stoArchiveFileAvailable"
#define STO_CMD_USER_MEDIA_REQUEST  "stoUserMediaRequest"

#define STOPUT_NOTIFY_CLIENT		""
#define STOPUT_REUSE			"1"


//***********************************************************************
//    		    Variable initializatoin.
//***********************************************************************

char	*cStoPutDhs::sdArchiveStream 	= NULL; // Archive data stream.
char	*cStoPutDhs::sdCompression 	= NULL;	// File compression to use.
bool	cStoPutDhs::sdFileNotifyOk 	= false; // File notification ok?
char	*cStoPutDhs::sdIdentity 	= NULL;	// dhsStoPut ID.
char	*cStoPutDhs::sdMediaType 	= NULL;	// Type of media to write
int	cStoPutDhs::sdNumCopies 	= -1;   // Number of copies to write.
char	*cStoPutDhs::sdOrganization 	= NULL; // Who's writting the media.
char	*cStoPutDhs::sdUnitPrefix 	= NULL; // Media unit name prefix.
char	*cStoPutDhs::sdStoServer 	= NULL;	// Storage server ID.
char	*cStoPutDhs::sdStoIpAddr 	= NULL;	// Storage server IP Addr.
cDhsConnection
	cStoPutDhs::sdStoConnect;		// Storage Server connection.
bool	cStoPutDhs::sdSimulate 		= false; // Simulate mode?
char	*cStoPutDhs::sdUserStream 	= NULL; // User data stream.

    
//***********************************************************************
//    			Local function definitions.
//***********************************************************************

static void	delHandler( cDhsHandler *handler )
{
    delete handler;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhsError::error
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

void		cStoPutDhsError::error
(
    void
)
{
    cStoPutStatus	status;
    

    //
    //  If its a debug message, print to stdout.
    //

    if ( level() == DHS_EL_DEBUG )
    {
	status.S_DEBUG_MSG( status, message() );
    }
    else
    {

	//
	//  Log the error, then print the message.
	//

	std::cout << "Error, file not put successfully : " << message() << std::endl;
    }
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhsCmd::done
//
// INVOCATION:
// n.a.  This function is called by the DHS++ library when a command is
// complete.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cs (DHS_CMD_STATUS) The status the command finished with.
// (>) msg (char *) The message associated with the status.
// (>) avList (cDhsAvList) The response attribute value list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Handle the completion of a command.  
//
// DESCRIPTION:
// Print information about the command.  Also interpret any error
// messges.  If the error was "already archived" then we will consider
// the Storage Server to be aware of the file it is about to recieve
// and we will send the file, later, despite the error.    
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

void            cStoPutDhsCmd::done 
(
    DHS_CMD_STATUS	cs,		// DHS return status.
    char		*msg,		// Message from storage server.
    const cDhsAvList	&avList		// Information list.
)
{
    DHS_STATUS          status( DHS_S_SUCCESS );    // DHS Return status.
    cStoPutStatus	stoPutStat;		    // dhsStoPut return status.
    char*		end;			    // End of file ID.
    char*		start;			    // Start of the file ID.
    

    if ( cs == DHS_CS_ERROR )
    {
	//
	// Parse message string to see if the
	// file was already archived!
	//
	
	checkNull ( ( start = strsav( msg ) ), stoPutStat, return );
	if ( msg != NULL && 
		( end = strstr( start, "already archived" ) ) != NULL )
	{	
	    end -= 3;
	    if ( ( start = strstr( start, "ID:" ) ) != NULL )
	    {
		start += 5;
		end[0] = '0';
		stoPutStat.S_ARCHIVE_WARN( stoPutStat, start );
		end[0] = '\'';
		cStoPutDhs::setFileNotify( true );
	    }
	}
	else
	{
	    cStoPutDhs::setFileNotify( false );
	}
    }
    else if ( cs == DHS_CS_DONE )
    {
	cStoPutDhs::setFileNotify( true );
    }
    else
    {
	cStoPutDhs::setFileNotify( false );
    }


    //
    // Display the message from the Storage Server.
    //

    if ( msg != NULL )
    {
	std::cout <<  endl << "message: " <<  msg;
    }
    std::cout << std::endl;
    
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::~stoPutDhs
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
// Destructor for the stoPutDhs class.
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

		cStoPutDhs::~cStoPutDhs
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
// cStoPutDhs::exit
//
// INVOCATION:
// cStoPutDhs::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of all DHS handlers and the cDhs object.
//
// DESCRIPTION:
// For each handler attached, delete it.  Then delete the dhs library
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
void		cStoPutDhs::exit
(
    cStoPutStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Close connection to the Storage Server.
    //

    checkDhs( sdStoConnect.close( dhsStatus), dhsStatus, status, return );


    //
    //  Remove each handler.
    //

    cDhsHandler::forEach( delHandler );

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::init
//
// INVOCATION:
// cStoPutDhs::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoPutStatus &) Function return status.
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

void		cStoPutDhs::init
(
    cStoPutStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
    		level( DHS_DEBUG_OFF );


    checkStat( status, return );


    //
    //  Set the debug level
    //

    switch ( cStoPutStatus::debugLevel() )
    {
	case	cStoPutStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case	cStoPutStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case	cStoPutStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }


    //
    //  Set up the dhs object.
    //

    checkNull( sdPDhs = new cDhs( sdIdentity, 1 * 51 * 1024, 
	    level ), status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDhsErrorHandler<cStoPutDhsError>, status, return );


}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::kill
//
// INVOCATION:
// cStoPutDhs::kill( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get rid of the cDhs object.
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
void		cStoPutDhs::kill
(
    VOID
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    //
    //  Close connection to the Storage Server.
    //

    sdStoConnect.close( dhsStatus );

    delete sdPDhs;
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::start
//
// INVOCATION:
// cmdDhsObj.start( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Start the DHS library event loop
//
// DESCRIPTION:
// Check the status, then start the event loop.
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

void		cStoPutDhs::start
(
    cStoPutStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    checkStat( status, return );


    //
    //  Start the event loop.
    //

    checkDhs( sdPDhs->startThread( dhsStatus ), dhsStatus, status, return );



    //
    // Connect to the Storage Server only if not in simulation mode.
    //

    if ( ! simulate() )
    {
	checkDhs( sdStoConnect.open( sdStoIpAddr, sdStoServer, dhsStatus), 
		dhsStatus, status, return );
    }
    

    //
    // Set up the error handler.
    //	

    checkNull( new cDhsErrorHandler<cStoPutDhsError>, status, 
	return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::config
//
// INVOCATION:
// cStoPutDhs::config( status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoPutStatus & )	- Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To readconfiguration information for the dhsStoPut paramenters.
//
// DESCRIPTION:
// Get the list of default values for some of the command-line
// options from the configuration file.  Get the storage server
// and dhsStoPut identity information, making sure we have one.
// Also determine the organization who will be writing the media.
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

void		cStoPutDhs::config
(
    cStoPutStatus	&status
)
{
    checkStat( status, return );

    checkConfig( config_get( STOPUT_CFG_IDENTITY, identityConfig ), status,
	    return );
    if ( sdIdentity == NULL )
    {
	status.E_NO_IDENTITY( status );
    }

    checkConfig( config_get( STOPUT_CFG_STORAGE, storageConfig ), status,
	    return );
    checkConfig( config_get( STOPUT_CFG_ORG, organizationConfig ), status,
	    return );
    checkConfig( config_get( STOPUT_CFG_DEFAULTS, defaultConfig ), status,
	    return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::defaultConfig
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse configuration file lines with keyword "defaults" from the
// configuration file.
//
// DESCRIPTION:
// Set the command-line default values to be used.  In particular
// the default compression,  media type and number of copies to
// create.  The compression value is used for both archive and user
// data streams where as media type and number of copies is used
// for user data streams only.  The values read from the configuration
// file can be over-ridden by specifying the -comp, -media, and -copies
// command-line options, respectively. Note that the values on the last 
// line in the configuration file with the keyword "defaults" will be used.
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

boolean		cStoPutDhs::defaultConfig
(
    int         numTokens,      // (in)  Number of tokens.
    char        **tokens        // (in)  list of tokens.
)
{
    boolean         ret;            // Function return value.
    cStoPutStatus   status;         // Status code.

    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 4 )
    {
	if ( sdCompression == NULL )
	{
	    checkNull( ( sdCompression = strsav( tokens[1] ) ), status,
    		return( FALSE ) );
	}

	if ( sdMediaType == NULL )
	{
	    checkNull( ( sdMediaType = strsav( tokens[2] ) ), status,
    		return( FALSE ) );
	}
	if ( sdNumCopies < 1 )
	{
	    if( !str2int( tokens[3], &sdNumCopies ) || sdNumCopies < 1 )
	    {
		status.E_NUM_COPIES( status, tokens[3] );
	    }
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
// cStoPutDhs::identityConfig
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse configuration file lines with keyword "identity" from the
// configuration file.
//
// DESCRIPTION:
// Set the default dhsStoPut client identity for IMP.  This value
// can be over-ridden by specifying the -id command-line option.
// Note that the values on the last line with the keyword "identity"
// and whose hostname is the same as the machine where dhsStoPut was
// executed from.
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

boolean		cStoPutDhs::identityConfig
(
    int         numTokens,      // (in)  Number of tokens.
    char        **tokens        // (in)  list of tokens.
)
{
    char	hostname[256];
    				// Host dhsStoPut is runnint on.
    boolean     ret;            // Function return value.
    cStoPutStatus  status;      // Status code.


    if ( gethostname( hostname,  255 ) != 0 )
    {
	status.E_HOSTNAME( status );
	status.sysErrno();
    }
        

    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 3 )
    {	
	if ( streq( hostname, tokens[1] ) && sdIdentity == NULL )
	{
	    checkNull( ( sdIdentity = strsav( tokens[2] ) ), status,
    		return( FALSE ) );
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
// cStoPutDhs::organizationConfig
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse configuration file lines with keyword "organization" from the
// configuration file.
//
// DESCRIPTION:
// Set the name of the organization that will be creating the media.
// Note that the values on the last line with the keyword "organization"
// will be used.  The organization is used when creating the default
// unit name prefix for user data streams.    
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

boolean		cStoPutDhs::organizationConfig
(
    int         numTokens,      // (in)  Number of tokens.
    char        **tokens        // (in)  list of tokens.
)
{
    boolean     ret;            // Function return value.
    cStoPutStatus  status;         // Status code.


    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 2 )
    {
	checkNull( ( sdOrganization = strsav( tokens[1] ) ), status, 
		return( FALSE) );
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
// cStoPutDhs::storageConfig
//
// INVOCATION:
// Only to be invoked from the config_get() gen library routine.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int)    - Number of tokens in the tokens array.
// (>) tokens   (char **) - The array of tokens.
//
// FUNCTION VALUE:
// (boolean) TRUE if the configuration line represented by the array of
//           tokens is correctly parseable. FALSE otherwise.
//
// PURPOSE:
// To parse configuration file lines with keyword "storageServer" from the
// configuration file.
//
// DESCRIPTION:
// Set the IMP name and IP address of the Storage Server that dhsStoPut
// will connect to.  The values in the configuration file can be 
// over-ridden with the -s and -ip command-line options.  Not that
// the values on the last line with the keyword "sotrageServer" will be used.
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

boolean		cStoPutDhs::storageConfig
(
    int         numTokens,      // (in)  Number of tokens.
    char        **tokens        // (in)  list of tokens.
)
{
    boolean     ret;            // Function return value.
    cStoPutStatus  status;         // Status code.

    //
    // Ensure the correct number of tokens.
    //

    ret = TRUE;
    if ( numTokens == 3 )
    {
	if ( sdStoServer == NULL )
	{
	    checkNull( ( sdStoServer = strsav( tokens[1] ) ), status,
    		return(FALSE) );
	}
	if ( sdStoIpAddr == NULL )
	{
	    checkNull( ( sdStoIpAddr = strsav( tokens[2] ) ), status,
    		return(FALSE) );
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
// cStoPutDhs::setArchiveStream
//
// INVOCATION:
// cStoPutDhs::setArchiveStream( archiveStream, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) archiveStream	(char *)	    archive data stream to be used.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the archive stream the data is to be sent too.
//
// DESCRIPTION:
// To set the archive stream the data is to be sent too.
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

void	cStoPutDhs::setArchiveStream
(
    char 	    *archiveStream, 	// (in)  archive Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    checkNull( archiveStream, status, return );		
    sdArchiveStream = archiveStream;
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setCompression
//
// INVOCATION:
// cStoPutDhs::setArchiveStream( compression, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) compression	(char *)	    compression value.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the type of file compression to be used on the files.
//
// DESCRIPTION:
// To set the type of file compression to be used on the files.
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

void	cStoPutDhs::setCompression
(
    char 	    *compression, 	// (in)  archive Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    
    checkNull( compression, status, return );		

    if ( strne( compression, "ASIS" )  		
	    && strne( compression, "NONE" )  		
	    && strne( compression, "GZIP" ) ) {
	status.E_BAD_COMP( status, compression );
	sdCompression = NULL;
    }
    else
    {				
	sdCompression = compression;
    }
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setMediaType
//
// INVOCATION:
// cStoPutDhs::setMediaType( mediaType, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) mediaType (char *)     	    A media type.
// (!) status	 (cStoPutStatus)    Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To set the media type for the class.
//
// DESCRIPTION:
// Set type of media to be written.  Note that the media type is
// needed for user data streams only.
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

void 	cStoPutDhs::setMediaType
(
    char	    *mediaType,	// (in)  New media type.
    cStoPutStatus   &status	// (mod) Function return status.
)
{
    checkStat( status, return );

    checkNull( mediaType, status, return );

    sdMediaType = mediaType;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setIdentity
//
// INVOCATION:
// cStoPutDhs::setIdentity( identity, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) identity	(char *)	    Identity of dhsStoPut client
// (!) status	(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the identity of the dhsStoPut client.
//
// DESCRIPTION:
// To set the identity of the dhsStoPut client.  Note that the client
// name will differ from machine to machine.  Also note that the identity
// should match one of the authorized clients listed in the DHS Storage
// servers configuration file.    
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

void	cStoPutDhs::setIdentity
(
    char 	    *identity, 	// (in)  archive Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    checkNull( identity, status, return );		
    sdIdentity = identity;
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setStoIdentity
//
// INVOCATION:
// cStoPutDhs::setStoIdentity( stoIdentity, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) stoIdentity	(char *)	    Identity of the Storage Server.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the IMP identity for the Storage Server.
//
// DESCRIPTION:
// To set the IMP identity for the Storage Server.
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

void	cStoPutDhs::setStoIdentity
(
    char 	    *stoIdentity, 	// (in)  archive Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    checkNull( stoIdentity, status, return );		
    sdStoServer = stoIdentity;
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setStoIpAddr
//
// INVOCATION:
// cStoPutDhs::setStoIpAddr( stoIpAddr, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) stoIpAddr	(char *)	    Storage Server IP Address.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the IP address of the Storage Server.
//
// DESCRIPTION:
// Set the IP address of the Storage Server.
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

void	cStoPutDhs::setStoIpAddr
(
    char 	    *stoIpAddr, 	// (in)  archive Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    checkNull( stoIpAddr, status, return );		
    sdStoIpAddr = stoIpAddr;
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setUnitPrefix
//
// INVOCATION:
// cStoPutDhs::setUnitPrefix( unitPrefix, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) unitPrefix	(char *)	    The unit name prefix.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the unit name prefix for user streamed data.
//
// DESCRIPTION:
// Set the unit name prefix for user streamed data.  Note that unit
// names are always in uppercase characters.
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

void	cStoPutDhs::setUnitPrefix
(
    char 	    *unitPrefix, 	// (in)  archive Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    checkNull( unitPrefix, status, return );		
    sdUnitPrefix = unitPrefix;
    str2upper( sdUnitPrefix, sdUnitPrefix );
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::setUserStream
//
// INVOCATION:
// cStoPutDhs::setUserStream( userStream, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) userStream	(char *)	    user data stream to be used.
// (!) status		(cStoPutStatus&)    Function status.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// To set the user stream the data is to be sent too.
//
// DESCRIPTION:
// To set the user stream the data is to be sent too.
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

void	cStoPutDhs::setUserStream
(
    char 	    *userStream, 	// (in)  user Stream.
    cStoPutStatus   &status	    	// (mod) function status.
)
{ 
    checkStat( status, return );
    checkNull( userStream, status, return );		
    sdUserStream = userStream;
}		

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::archiveFileAvail
//
// INVOCATION:
// cStoPutDhs.archiveFileAvail()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To inform the Storage Server that all of the files listed on
// the command-line will be sent soon (are available).
//
// DESCRIPTION:
// For each file listed on the command-line send the
// stoArchiveFileAvailable command to the storage server.  If
// we are in simulate mode the command is not sent, but pretends
// to send it successfully.
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

void		cStoPutDhs::archiveFileAvail
(
    cStoPutStatus	&status		// (mod) Function return status.
)
{
    cStoPutDhsCmd	*pCmd;
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    DHS_CMD_STATUS	cmdStat = DHS_CS_DONE;
    char		fileSizeStr[10];    
    
    //
    // Check the status.
    //
    
    checkStat( status, return );


    //
    //  Cycle through the put list, and for each file listed notify
    //  the Storage Server of the file.
    //
    
    cStoPutList			*pStoPutList;
    cStoPutList::iterator	i;
    bool			firstTime = TRUE;


    while( status.ok() && 
	    ( pStoPutList = cStoPutList::getNext( i, firstTime ) ) != NULL )
    {
	firstTime = FALSE;
	pCmd = new cStoPutDhsCmd( STO_CMD_ARCHIVE_FILE_AVAIL );

	checkDhs( pCmd->add( "fileId", DHS_DT_STRING, 
	         pStoPutList->getFileId(), dhsStatus ), dhsStatus, status, 
		VOID );
	sprintf( fileSizeStr, "%d", pStoPutList->getFileSize() );
	checkDhs( pCmd->add( "fileSize", DHS_DT_INT32, 
		pStoPutList->getFileSize(), dhsStatus ), 
		dhsStatus, status, VOID );
	checkDhs( pCmd->add( "segregationDir", DHS_DT_STRING, sdArchiveStream,
		dhsStatus ), dhsStatus, status, VOID );
	checkDhs( pCmd->add( "filePath", DHS_DT_STRING,
		pStoPutList->getDestName(), dhsStatus ), dhsStatus, status, 
		VOID );
	checkDhs( pCmd->add( "compFlag", DHS_DT_STRING, sdCompression,
		dhsStatus ), dhsStatus, status, VOID );

	
	//
	// Display the command that we are sending if debug level is
	// set to "MIN" or "FULL".
	//
	
	if ( cStoPutStatus::debugLevel() != cStoPutStatus::DEBUG_NONE )
	{
	    pCmd->print( dhsStatus );
	}
        

	//
	// If in simulation mode then pretend to send the command
	// successfully, otherwised send the command.
	//
    
	if ( sdSimulate )
	{
	    status.S_FILE_AVAIL( status, pStoPutList->getSrcName() );
	    std::cout << "Sto Request SUCCESSFUL for File : "<< pStoPutList->getFileId();
	}
	else
	{
	    checkDhs( pCmd->apply( getConnect(), dhsStatus ), dhsStatus, status,
    		VOID );
	    checkDhs( pCmd->wait( dhsStatus ), dhsStatus, status, VOID );
	    checkDhs( ( cmdStat = pCmd->status( dhsStatus ) ), dhsStatus,
		status, VOID );
	
	    if ( ( cmdStat != DHS_CS_DONE &&  ! sdFileNotifyOk ) ||
		! status.ok() || dhsStatus != DHS_S_SUCCESS )
	    {
		status.E_FILE_AVAIL( status, pStoPutList->getSrcName() );
	    }
	    else if ( pCmd->status( dhsStatus ) == DHS_CS_DONE 
    		|| sdFileNotifyOk )
	    {
		status.S_FILE_AVAIL( status, pStoPutList->getSrcName() );
	    }
	}
	
	delete pCmd;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPutDhs::userMediaRequest
//
// INVOCATION:
// cStoPutDhs::userMediaRequest( status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStoPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To inform the Storage Server that there are a number of files
// to be added to a user media request.  Note that this may cause a
// new user media request to be opened, an old user media request to
// be reopened, or files to be appended to an "open" user media request.
//
// DESCRIPTION:
// First create the list of file Identifiers, files, file sizes, and
// compression values.  Create the command and send it.  If we are
// in simulate mode then the command is not sent to the storage server
// but the method pretends to send it successfully.
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

void		cStoPutDhs::userMediaRequest
(
     cStoPutStatus	&status		// (mod) Function return status.
)
{
    ulong               dims[7];	// Dimension of the arrays.
    cDhsClientCommand	*pCmd;		// The command to send.
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    					// dhs function return status.
    char		**fileIds;	// List of file IDs
    char		**filePaths;	// List of file paths.
    char		**compressions;	// file compression list.
    char		**fileSizes;	// file size list.
    char		numCopiesStr[10];
    char		fileSizeStr[10];
    
    
    //
    // Check the status.
    //
    
    checkStat( status, return );


    //
    // Cycle through the put list and construct the file arrays
    // need to be sent with the command.
    //

    cStoPutList			*pStoPutList;
    cStoPutList::iterator	i;
    bool			firstTime = TRUE;
    int				index = 0;
    

    checkNull( ( fileIds = (char **) gen_alloc( pStoPutList->size()
	    * sizeof( char *) ) ), status, return );
    checkNull( ( filePaths = (char **) gen_alloc( pStoPutList->size()
	    * sizeof( char *) ) ), status, return );
    checkNull( ( fileSizes = (char **) gen_alloc( pStoPutList->size()
	    * sizeof( char *) ) ), status, return );
    checkNull( ( compressions = (char **)gen_alloc( pStoPutList->size()
	    * sizeof( char *) ) ), status, return );

    while( status.ok() && ( pStoPutList = cStoPutList::getNext( i, firstTime ) )
	    != NULL )
    {
	if ( firstTime )
	{
	    firstTime = FALSE;
	}

        checkNull( ( fileIds[ index ] = strsav( pStoPutList->getFileId() ) ),
	        status, return );
    	checkNull( ( filePaths[ index ] = strsav( pStoPutList->getDestName() ) ),
	        status, return );
	sprintf( fileSizeStr, "%d", pStoPutList->getFileSize() );
	checkNull( ( fileSizes[ index ] = strsav( fileSizeStr ) ), status,
		return );
    	checkNull( ( compressions[ index ] = strsav( sdCompression) ),
		status, return );
	index++;
    }


    //
    // Find out how many files there are.
    //

    dims[0] = pStoPutList->size();
    dims[1] = dims[2] = dims[3] = dims[4] = dims[5] = dims[6] = 0;


    //
    // Create the new command to be sent to the Storage Server.
    //

    pCmd = new cStoPutDhsCmd( STO_CMD_USER_MEDIA_REQUEST );
    checkDhs( pCmd->add( "requestName", DHS_DT_STRING, sdUserStream,
	dhsStatus ), dhsStatus, status, VOID );
    checkDhs( pCmd->add( "notifyClient", DHS_DT_STRING, STOPUT_NOTIFY_CLIENT, 
	dhsStatus ), dhsStatus, status, VOID );
    checkDhs( pCmd->add( "unitNameRoot", DHS_DT_STRING, sdUnitPrefix,
	dhsStatus ), dhsStatus, status, VOID );
    sprintf( numCopiesStr, "%d", sdNumCopies );
    checkDhs( pCmd->add( "numCopies", DHS_DT_STRING, numCopiesStr,
	dhsStatus ), dhsStatus, status, VOID );
    checkDhs( pCmd->add( "mediaType", DHS_DT_STRING, sdMediaType, dhsStatus ),
	dhsStatus, status, VOID );
    checkDhs( pCmd->add( "reuse", DHS_DT_STRING, STOPUT_REUSE, dhsStatus ),
	dhsStatus, status, VOID );
    checkDhs( pCmd->add( "fileIdArray", DHS_DT_STRING, 1, dims, fileIds,
	dhsStatus ), dhsStatus, status, VOID );
    checkDhs( pCmd->add( "filePathArray", DHS_DT_STRING, 1, dims, filePaths,
	dhsStatus ), dhsStatus, status, VOID );
    checkDhs( pCmd->add( "compFlagArray", DHS_DT_STRING, 1, dims, compressions,
	dhsStatus ), dhsStatus, status, VOID );
    checkDhs( pCmd->add( "fileSizeArray", DHS_DT_STRING, 1, dims, fileSizes,
	dhsStatus ), dhsStatus, status, VOID );

    //
    // Display the command that we are sending if debug level is
    // set to "MIN" or "FULL".
    //
	
    if ( cStoPutStatus::debugLevel() != cStoPutStatus::DEBUG_NONE )
    {
	pCmd->print( dhsStatus );
    }
        

    //
    // If in simulation mode then pretend to send the command
    // successfully, otherwised send the command.
    //
    
    if ( sdSimulate )
    {
	status.S_USR_MEDIA_REQ( status );
	for( int j = 0; j < pStoPutList->size(); j++ )
	{
	    std::cout << "Sto Request SUCCESSFUL for File : "<< fileIds[j];
	}
    }
    else
    {
	checkDhs( pCmd->apply( getConnect(), dhsStatus ), dhsStatus, status,
		VOID );
	checkDhs( pCmd->wait( dhsStatus ), dhsStatus, status, VOID );
	
	if ( pCmd->status( dhsStatus ) == DHS_CS_ERROR )
	{
	    status.E_USR_MEDIA_REQ( status );
	}
	else if ( pCmd->status( dhsStatus ) == DHS_CS_DONE )
	{
	    status.S_USR_MEDIA_REQ( status );
	}
    }
    
    delete pCmd;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutFile::cPutFile
//
// INVOCATION:
// cPutFile = new cPutFile( status, filePath, fileSize );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStoPutStatus &) 	Function return status.
// (>) filePath ( char *)		Complete filename and path.
// (>) size	( int &)		Size of the file.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To Open the file and read it into a buffer in memory.
//
// DESCRIPTION:
// Open the file and read it into a buffer in memory.
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

	cPutFile::cPutFile
(
    cStoPutStatus	&status,	// (mod) Status passed in.
    char		*filePath,	// (in)  Complete filename and path.
    int			fileSize	// (in)  Size of the file.
)
:cDhsPutDataset( NULL, 0, DHS_BD_PT_RAW )
{
    FILE	*fp;			// fp of the file.
    void	*dataBuffer;		// File contents.
    int		sizeRead;		// Size read.


    //
    //  Check the status.
    //

    checkStat( status, return );
    

    //
    //  Open the file for reading.
    //
    
    if ( ( fp = fopen( filePath,  "r" ) ) == NULL )
    {
	status.E_SYSTEM( status, filePath );
	status.E_NO_READ( status, filePath );
	status.sysErrno();
	return;
    }
    status.S_OPEN_FILE( status, filePath );


    //
    //  Alloc room and read in the file.
    //

    dataBuffer = gen_alloc( (fileSize + 1) );
    sizeRead = fread( dataBuffer, 1, fileSize, fp );
    if ( sizeRead != fileSize )
    {
	status.E_NO_READ( status, filePath );
    }
    (void) fclose( fp );

    buffer( dataBuffer, fileSize );
    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cPutFile::done
//
// INVOCATION:
// n.a. This function is called by the DHS++ library when a file
// "put" is complete.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) cs (DHS_CMD_STATUS) The status the command finished with.
// (>) msg (char *) The message associated with the status.
// (>) avList (cDhsAvList) The response attribute value list.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To check on the completion status of the file put and to display
// any messages from the Storage Server.
//
// DESCRIPTION:
// If the put has completed then display the appropriage message:
// successful or error.  If the put hasn't completed then display
// the message that was sent back.
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

void		cPutFile::done
(
    DHS_CMD_STATUS	cs,		// DHS return status.
    char		*msg,		// Message from storage server.
    cDhsAvList 		&avList		// Information list.
)
{
    cStoPutStatus	status;
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    if ( cs == DHS_CS_ERROR ||  !status.ok() )
    {
	std::cout << "StoPut Request FAILED for File     : "<< label();
    }
    else if ( cs == DHS_CS_DONE )
    {
	std::cout << "Sto Request SUCCESSFUL for File : "<< label();
    }

    if ( cs != DHS_CS_BUSY )
    {
	cDhsAttrib  attrib;		// An attribute in the list.
	void	    *fileId;		// The files ID.


	//
	//  Find the file Id and print it.
	//

	attrib = avList.find( "fileId", dhsStatus );
	attrib.info( &fileId, dhsStatus);
	if ( dhsStatus == DHS_S_SUCCESS)
	{
	    std::cout << std::endl << "              File name returned: "<< (char *) fileId;
	}


	//
	// Display the message from the Storage Server.
	//
	
	if ( msg != NULL )
	{
	    std::cout << std::endl << "message: " <<  msg;
	}
	std::cout << std::endl;
    }
}


