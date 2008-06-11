static char rcsid[] = "$Id: stoPut.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1999				(c) 1999
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
// dhsStorage/src/stoPut.C
//
// PURPOSE:
// Contains the implementation of the cStoPut class.
//
// METHOD NAME(S)
// cStoPut::exec - Executed to begin the processing of a put request.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:32:11  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.2  2000/08/22 19:30:11  nhill
// Fixed a bug which caused file names to be wrong when data is retrieved
// through a get.
//
// Revision 1.1  1999/10/12 23:24:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <list>
extern "C"
{
#include "gen_types.h"
#include "ad.h"
}

#include "globals.H"
#include "stoPut.H"
#include "stoFile.H"
#include "stoConnection.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoPut::exec
//
// INVOCATION:
// N.A. Called automatically by the dhs put handler.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This function is executed when a put request is received.
//
// DESCRIPTION:
// - Get a list of files from the media_files table that match the received
//   file id, and which haven't been received yet.
// - If there are no files in the list, report an error.
// - Save the file to the location specified in the media_files table.
// - Send a response to indicate the put has been handled.
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

void		cStoPut::exec
(
    void
)
{
    AM_FILE	amFile;		// A single am file.
    std::list<AM_FILE>		// A list of files matching the file id.
		amFileList;
    std::list<AM_FILE>::iterator	// Iterator in the list of files.
		iAmFileList;
    int		amStatus;	// AM library status value.
    DBPROCESS	*dbProcess;	// The database dbprocess.
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );	
				// Dhs library status.
    char	*ext;		// The file extension.
    int		numFiles( 0 );	// Number of files in media_files that used
    				// this file.
    cStoStatus	status;		// Error status.


    //
    // Stop the automatic display of messages.
    //

    status.displayStop();

    if ( type() != DHS_BD_PT_FITS && type() != DHS_BD_PT_RAW )
    {
	status.E_PUT_TYPE( status, label(), type() );
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ), 
		dhsStatus, status, status.display(); return );
	return;
    }


    //
    // Get a list of files from the am library that match the file id, and
    // which haven't been received yet. (There could be more than one if
    // the file is being put onto to media requests.)
    //

    dbProcess = cStoDbManager::acquire();
    for( amStatus = amFileGet( AM_FG_FILE_ID, &amFile, dbProcess, label() );
	    amStatus == AM_SUCCESS; 
	    amStatus = amFileGet( AM_FG_NEXT, &amFile, dbProcess ) )
    {
	if ( amFile.amfStatus == AMF_ERROR || 
		amFile.amfStatus == AMF_UNAVAILABLE || 
		amFile.amfStatus == AMF_UNPROCESSED )
	{
	    amFileList.push_back( amFile );
	    numFiles++;
	}
    }
    cStoDbManager::release();


    //
    //  Ensure that there is at least one file in media_files matching this
    //  file id.
    //

    if ( amStatus == AM_NO_FILE )
    {
	if ( numFiles == 0 )
	{
	    status.E_UNWANTED_PUT( status, label() );
	}
    }
    else
    {
	checkAm( amStatus, status, return );
    }


    //
    // Save the file to the location required for each of the files matching
    // this file id.
    //

    for ( iAmFileList = amFileList.begin(); iAmFileList != amFileList.end(); 
	    iAmFileList++ )
    {
	cStoFile	stoFile( &(*iAmFileList) );

	memcpy( &amFile, &(*iAmFileList), sizeof( AM_FILE ) );
	if ( streq( amFile.amfFilename, "" ) )
	{
	    strcpy( amFile.amfFilename, ( *iAmFileList).amfFileId );
	}

	ext = strext( (*iAmFileList).amfFilename );
	if ( ext != NULL && streq( ext, "gz" ) )
	{
	    stoFile.save( *this, this->label(), STO_CAV_COMP_GZIP, amFile, 
		    status );
	}
	else
	{
	    stoFile.save( *this, this->label(), AD_COMP_UNKNOWN, amFile,
		    status );
	}
    }


    //
    // Send a response to indicate the put is complete.
    //

    if ( status.ok() )
    {
	checkDhs( response( DHS_CS_DONE, "done", dhsStatus ), 
		dhsStatus, status, status.display(); return );
    }
    else
    {
	checkDhs( response( DHS_CS_ERROR, status.message(), dhsStatus ), 
		dhsStatus, status, status.display(); return );
    }
}
