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
// dhsPut/src/putDhs.C
//
// PURPOSE:
// Class methods to handle the dhs library startup, init, and errors.
//
// METHOD NAME(S)
// putDataset::putDataset	- Constructor.
// putDataset::done
// 
//
//INDENT-OFF*
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


#include <iostream>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <dirent.h>
}

#include "cadef.h"

#include "globals.H"
#include "hisRecovery.H"
#include "hisLogWrite.H"
#include "hisCa.H"
#include "hisDbm.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisRecovery::checkRecovery
//
// INVOCATION:
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Open the file and read it into the buffer.
//
// DESCRIPTION:
// Open the file and read it into the buffer.
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

void		cHisRecovery::checkRecovery
(
    cHisStatus	&status		// (mod) Status to check and return.
)
{
    bool	raw;
    bool	fits;
    bool	db;
    bool	recoveryNecessary;
    char	*label;
    char	*fitsPath;
    int		startStep;
    int		remove_status;
    cHisLogWrite
    		*hisLogWrite;


    checkStat( status, return );


    //
    // Initialize. 
    //

    label = NULL;
    fitsPath = NULL;
    raw = false;
    fits = false;
    db = false;


    //
    // Check for label.raw and/or label.fits in working directory.
    //

    checkWorkingDir( &label, &fitsPath, raw, fits, status );


    if ( label == NULL )
    {
	db = false;
    }
    else
    {
	//
	// Check the database for the label. 
	//

	cHisDbManager::checkLabelTbl( label, db, status );
    }


    //
    // Parse the flags. 
    //

    recoveryNecessary = true;
    if ( ( !raw && !fits && !db ) || ( !raw && !fits && db ) )
    {
	//
	// No recoverable state is detectable. 
	//
	
	recoveryNecessary = false;
    }
    else if ( !raw && fits && !db )
    {
	//
	// SHOULDN'T HAPPEN. Only thing we can do is to put fits file.
	//

	startStep = HLW_STEP_PUT;
    }
    else if ( !raw && fits && db )
    {
	//
	// FITS FILE HAS NOT YET BEEN PUT.
	//

	startStep = HLW_STEP_PUT;
    }
    else if ( raw && !fits && !db )
    {
	//
	// RAW FILE HAS NOT YET BEEN INGESTED TO DB.
	//

	startStep = HLW_STEP_INGEST;
    }
    else if ( raw && !fits && db )
    {
	//
	// RAW FILE HAS NOT YET BEEN CONVERTED TO FITS.
	//

	startStep = HLW_STEP_FITS;
    }
    else if ( raw && fits && !db )
    {
	//
	// SHOULDN'T HAPPEN. Remove fits file, start by ingesting raw file.
	//

	remove_status = remove( fitsPath );
	if ( remove_status !=0 )
	{
	    status.E_FREMOVE( status, fitsPath );
	    status.sysErrno();
	    return;
	}
	startStep = HLW_STEP_INGEST;
    }
    else if ( raw && fits && db )
    {
	//
	// CAN'T TRUST FITS FILE. Remove fits file, re-create. 
	//

	remove_status = remove( fitsPath );
	if ( remove_status !=0 )
	{
	    status.E_FREMOVE( status, fitsPath );
	    status.sysErrno();
	    return;
	}
	startStep = HLW_STEP_FITS;
    }


    //
    // If recovery neccesary, process remaining steps of hisLogWrite command. 
    //

    if ( recoveryNecessary )
    {
	status.S_RECOVERING( status );
	status.status( status.S_SUCCESS );
	hisLogWrite = new cHisLogWrite( label );
	hisLogWrite->processCmd( startStep, status );
	status.S_HIS_LOG_WRITE_COMPLETE( status );
	delete hisLogWrite;
    }


    //
    // Up to this point, hisLogWrite commands have been locked out so
    // recovery tests could be properly performed. Unlock. 
    //

    cHisLogWrite::HLWlock();
    cHisLogWrite::setHLWRunning( false );
    cHisLogWrite::HLWunlock();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisRecovery::checkWorkingDir
//
// INVOCATION:
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cPutStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Open the file and read it into the buffer.
//
// DESCRIPTION:
// Open the file and read it into the buffer.
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

void		cHisRecovery::checkWorkingDir
(
    char	**label,	// (out)
    char	**fitsPath,	// (out)
    bool	&rawPresent,	// (out)
    bool	&fitsPresent,	// (out)
    cHisStatus	&status		// (mod) Status to check and return.
)
{
    char	tempName[PATH_MAX];
    char	*tempExt;
    char	savedLabel[PATH_MAX];
    struct dirent 
		*direntPtr;     
    DIR         *dirPtr;        


    checkStat( status, return );
    savedLabel[0] = '\0';


    //
    // Open working directory.
    //

    if ( ( dirPtr = opendir( cHisCa::tempDir() ) ) == NULL )
    { 
	status.E_WORKING_DIR( status, cHisCa::tempDir() );
	status.sysErrno();
	return;
    }


    //
    // Loop thru working directory. 
    //

    while ( ( direntPtr = readdir( dirPtr ) ) != NULL )
    {
	//
	// Ignore '.' and '..' and 'historyLog.raw'.
	//

	if ( streq( direntPtr->d_name, "." ) || 
		streq( direntPtr->d_name, ".." ) ||
		streq( direntPtr->d_name, FNAME_HISTORYLOG ) )
	{
	    continue;
	}


	//
	// If file has extension raw or fits, take label from front. 
	// If label is the most recent, save it until second pass. 
	//

	tempExt = strext( direntPtr->d_name );
	if ( streq( tempExt, FNAME_SUFFIX_RAW ) || 
		streq( tempExt, FNAME_SUFFIX_FITS ) )
	{
	    (void) strroot( direntPtr->d_name );
	    if ( strcmp( direntPtr->d_name, savedLabel ) > 0 );
	    {
		(void) sprintf( savedLabel, "%s", direntPtr->d_name );
	    }
	}
	else
	{
	    //
	    // file is not .raw or .fits. Prompt user to get rid of it. 
	    //

	    status.S_FREMOVE( status, direntPtr->d_name, cHisCa::tempDir() );
	}
    }

    //
    // If savedLabel is still NULL, didn't find any .raw  or .fits files. 
    //

    if ( savedLabel[0] == '\0' )
    {
	*label = NULL;
	rawPresent = false;
	fitsPresent = false;
    }
    else
    {
	*label = strsav( savedLabel );


	//
	// Second pass of working dir. Only look at .raw or .fits files. 
	//
	
	rewinddir( dirPtr );
	while ( ( direntPtr = readdir( dirPtr ) ) != NULL )
	{
	    if ( streq( direntPtr->d_name, "." ) || 
		    streq( direntPtr->d_name, ".." ) ||
		    streq( direntPtr->d_name, FNAME_HISTORYLOG ) )
	    {
		continue;
	    }


	    //
	    // If not *.raw and not *.fits, skip it.
	    //

	    tempExt = strext( direntPtr->d_name );
	    if ( strne( tempExt, FNAME_SUFFIX_RAW ) && 
		    strne( tempExt, FNAME_SUFFIX_FITS ) )
	
	    {
		continue;
	    }

	    
	    //
	    // If file is savedLabel.*, set flag according to extension.
	    // If not, report about its presence and continue. 
	    //

	    (void) sprintf( tempName, "%s.%s", savedLabel, FNAME_SUFFIX_RAW ); 
	    if ( streq( direntPtr->d_name, tempName ) )
	    {
		rawPresent = true;
		continue;
	    }

	    (void) sprintf( tempName, "%s.%s", savedLabel, FNAME_SUFFIX_FITS ); 
	    if ( streq( direntPtr->d_name, tempName ) )
	    {
		fitsPresent = true;
		(void) sprintf( tempName, "%s/%s.%s", cHisCa::tempDir(),
			savedLabel, FNAME_SUFFIX_FITS );
		*fitsPath = strsav( tempName );
		continue;
	    }

	    //
	    // If get to this point, report the file's presence.
	    //

	    status.S_FREMOVE( status, direntPtr->d_name, cHisCa::tempDir() );
	}
    }

    //
    // Close working directory. 
    //

    (void) closedir( dirPtr );


    return;
}
