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
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream.h>
#include <sys/types.h>
#include <unistd.h>

extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#define	DHS_NO_CLIENT
#define	DHS_NO_GET
#include "dhs++.H"

#include "cadef.h"


#include "globals.H"
#include "hisDbm.H"
#include "hisDhs.H"
#include "hisPutFile.H"
#include "hisLogWrite.H"
#include "hisEvent.H"
#include "hisFits.H"
#include "hisCa.H"


//
// Define and initialize the static data in the cHisLogWrite class.
//

bool	  	cHisLogWrite::hlwRunning = true; 
bool	  	cHisLogWrite::hlwStopping = false; 
cCond		cHisLogWrite::hlwRunCond; 
cDhsConnection	cHisLogWrite::hlwDataServer;
char     	*cHisLogWrite::hlwDSAddress = NULL;
char     	*cHisLogWrite::hlwDSIdentity = NULL;



//
//***********************************************************************
//+
// FUNCTION NAME:
// composePaths
//
// INVOCATION:
// composePaths( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Compose paths to the raw and fits files
//
// DESCRIPTION:
// Using the class data member hlwLabel, composes the paths to the raw
// and fits files. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisLogWrite::composePaths
( 
    cHisStatus	&status		// (mod) Program status. 
)
{
    char	tempPath[PATH_MAX];	
				// Buffer into which to write path. 
    

    checkStat( status, return );


    //
    // Compose path of raw file. 
    //
    
    sprintf( tempPath, "%s/%s.%s", cHisCa::tempDir(), hlwLabel, 
	    FNAME_SUFFIX_RAW );
    checkNull( hlwRawPath = strsav( tempPath ), status, return );


    //
    // Compose path of fits file. 
    //
    
    sprintf( tempPath, "%s/%s.%s", cHisCa::tempDir(), hlwLabel, 
	    FNAME_SUFFIX_FITS );
    checkNull( hlwFitsPath = strsav( tempPath ), status, return );


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// putFile
//
// INVOCATION:
// putFile( filePath, label, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) filePath	(char *) 	Path to file. 
// (>) label	(char *) 	DHS label. 
// (!) status	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Puts the file to the DHS dataServer
//
// DESCRIPTION:
// Creates a put-able object from the file, puts it to the dataServer, 
// wait for the put to complete, save new label if it has one, 
// remove the file. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisLogWrite::putFile
( 
    char 	*filePath, 
    char	*label,
    cHisStatus 	&status 
)
{
    void	*buffer;	// Buffer into which to read file. 
    int		bufSize;	// Size of buffer. 
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );	
    				// DHS library function return status.
    cHisPutFile *pPutFile;	// Put-able object.
    int		remove_status;	// Return status from remove().
    

    checkStat( status, return );
    dhsStatus = DHS_S_SUCCESS;


    //
    // Create put-able object, Read file into its buffer. 
    //
    
    pPutFile = new cHisPutFile( status, filePath, &buffer, bufSize );
    checkStat( status, return );
    pPutFile->buffer( buffer, bufSize );


    //
    // Put object. Use default life of perm and contrib list of just 'me'.
    //
    
    checkDhs( pPutFile->put( hlwDataServer, label, true, dhsStatus ),
	    dhsStatus, status, return );


    //
    // Wait for put to complete, delete object.
    //

    checkDhs( pPutFile->wait( dhsStatus ), dhsStatus, status, return );
    checkDhs( pPutFile->status( dhsStatus ), dhsStatus, status, return );
    if ( pPutFile->newLabel() != NULL && strne( pPutFile->newLabel(), label ) )
    {
	status.E_NEWLABEL( status, label, pPutFile->newLabel() );
    }
    delete pPutFile;


    //
    // Remove the fits file.
    //
    
    remove_status = remove( filePath );
    if ( remove_status == -1 )
    {
	status.E_FREMOVE( status, filePath );
	status.sysErrno();
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// exec
//
// INVOCATION:
// n.a. Called automatically by the command handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Processes a hisLogWrite command
//
// DESCRIPTION:
// Respond 'BUSY' to the client, log command reception, check for 
// an early 'simulated' command completion, process the command, 
// respond to client when completed. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisLogWrite::exec
(
    void
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
    char        *id;                    // Identity of client.
    char        *ip;                    // Ip address of client.
    cHisStatus	status;			// Status. 


    //
    // Only one HLW command can be processed at once. 
    //

    HLWlock();
    if ( HLWRunning() )
    {
	response( DHS_CS_ERROR, HIS_RSP_ALREADY_HLW_MSG, dhsStatus );
	status.E_HISLOGWRITE( status ); 
	HLWunlock();
	return;
    }
    else
    {
	setHLWRunning( true );
	status.S_HIS_LOG_WRITE_BEGIN( status );
	HLWunlock();
    }


    //
    // If cmd not being stopped by init or exec, continue. 
    //

    if ( !stoppingHLW() )
    {
	//
	// Respond to let client continue. Log receipt of command.
	//

	response( DHS_CS_BUSY, HIS_RSP_WORKING_MSG, dhsStatus );
	dhsConnectInfo( connect(), &ip, &id, &dhsStatus );
	status.S_COMMAND_RECEIVED( status, name(), id, ip );


	//
	// If in simulation mode, just respond DONE. 
	//

	if ( cHisDhs::simulate() )
	{
	    HLWlock();
	    setHLWRunning( false );
	    HLWunlock();
	    response( DHS_CS_DONE, dhsStatus );
	    status.S_HIS_LOG_WRITE_COMPLETE( status );
	    status.resetHealth( status );
	    return;
	}

	
	//
	// Process all steps in the hisLogWrite command. 
	//
	
	processCmd( HLW_ALL_STEPS, status );
    }


    //
    // Respond. 
    //

    if ( !status.ok() )
    {
	response( DHS_CS_ERROR, HIS_RSP_ERROR_HLW_MSG, dhsStatus );
    }
    else if ( stoppingHLW() )
    {
	response( DHS_CS_ERROR, HIS_RSP_HLW_CANCEL_MSG, dhsStatus );
    }
    else
    {
	response( DHS_CS_DONE, dhsStatus );
    }


    //
    // Unlock so commands may be processed. 
    //
    
    HLWlock();
    setHLWRunning( false );
    HLWunlock();
    HLWbroadcast();


    if ( ( !stoppingHLW() ) && status.ok() )
    {
	//
	// Print & log message. Reset health from WARNING to GOOD. 
	//

	status.S_HIS_LOG_WRITE_COMPLETE( status );
	status.resetHealth( status );
    }

    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// config
//
// INVOCATION:
// config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status 	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the configuration parameters
//
// DESCRIPTION:
// Read the runtime 'dataserver' configuration parameters. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisLogWrite::config
(
    cHisStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    checkConfig( config_get( HIS_CFG_DATASERVER, dataServer ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// dataServer
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) 		Number of tokens on the configuration line.
// (>) tokens 	 (char **) 	List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (boolean) TRUE	- Tokens were successfully parsed.
// (boolean) FALSE	- Tokens were not successfully parsed.
//
// PURPOSE:
// Read the dataServer line from a configuration file and store it.
//
// DESCRIPTION:
// Save the dataServer identity and host address. 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cHisLogWrite::dataServer
(
    int		numTokens,	// (in)  Number of tokens.
    char	**tokens	// (in)  list of tokens.
)
{
    boolean	ret;		// Function return value.
    cHisStatus	status;		// Status code.


    //
    // Check that the number of tokens on the line is correct.
    //

    if ( numTokens == 3 )
    {
	//
	// Free any existing dataServer strings.
	//

	if ( hlwDSIdentity != NULL )
	{
	    free( hlwDSIdentity );
	}

	if ( hlwDSAddress != NULL )
	{
	    free( hlwDSAddress );
	}


	//
	// Save the new dataServer strings.
	//

	checkNull( hlwDSIdentity = (char *) strsav( tokens[1] ), status,
		return( FALSE ) );
	checkNull( hlwDSAddress = (char *) strsav( tokens[2] ), status,
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
// processCmd
//
// INVOCATION:
// processCmd( startStep, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) startStep	(int) 		Command 'step' to start up. 
// (!) status		(cHisStatus &)	Program status. 
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Take the steps necessary to complete a hisLogWrite command. 
//
// DESCRIPTION:
// Startstep indicates where in the process to start - this was necessary 
// for recovery purposes. Check if a hisLogWrite command is already being
// processed, open a connection to the dataServer, Get a label for the
// file, 
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisLogWrite::processCmd
(
    int		startStep,	// (in)  Start step number
    cHisStatus	&status		// (mod) status. 
)
{
    char	*tempLabel;	// Temporary char pointer to label.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );


    //
    // Open connection to dataserver. 
    //
    
    checkDhs( hlwDataServer.open( hlwDSAddress, hlwDSIdentity,
	    dhsStatus ), dhsStatus, status, VOID );
    if ( status.ok() )
    {
	if ( startStep <= HLW_STEP_GETLABEL && !stoppingHLW() )
	{
	    //
	    // Get a label. Save it to the class.
	    //
	    
	    checkDhs( dhsBdCtl( hlwDataServer.dhsConnect(), DHS_BD_CTL_GETNAME, 
		    &tempLabel, &dhsStatus ), dhsStatus, status, VOID );
		    hlwLabel = strsav( tempLabel );
	}

	composePaths( status );
	
	if ( startStep <= HLW_STEP_RENAME && !stoppingHLW() )
	{
	    //
	    // Rename event file, open new event file.
	    //

	    cHisEvent::fileRename( hlwRawPath, status );
	}

	if ( startStep <= HLW_STEP_INGEST && !stoppingHLW() )
	{
	    //
	    // Ingest the file into the database. 
	    //

	    cHisDbManager::ingestFile( hlwLabel, hlwRawPath, status );
	}

	if ( startStep <= HLW_STEP_FITS && !stoppingHLW() )
	{
	    //
	    // Create the fits file. 
	    //

	    cHisFits::createFits( hlwRawPath, hlwFitsPath, hlwLabel, status );
	}

	if ( startStep <= HLW_STEP_PUT && !stoppingHLW() )
	{
	    //
	    // Put the fits file to the dataServer. 
	    //

	    putFile( hlwFitsPath, hlwLabel, status );
	}


	//
	// Close connection to dataServer. 
	//

	checkDhs( hlwDataServer.close( dhsStatus ), 
		dhsStatus, status, VOID );
    }

    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// stopHLW
//
// INVOCATION:
// stopHLW
//
// PARAMETERS: (">" input, "!" modified, "<" output)
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
//
// DESCRIPTION:
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cHisLogWrite::stopHLW
(
    void
)
{
    HLWlock();
    if ( HLWRunning() )
    {
	hlwStopping = true;
	while( HLWRunning() )
	{
	    HLWwait();
	}
    }

    //
    // Delete command handler so no more get executed ... ?
    //

    HLWunlock();


    return;
}
