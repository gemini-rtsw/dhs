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
// dhsHistory/src/hisFits.C
//
// PURPOSE:
// Class methods to write the fits file. 
//
// METHOD NAME(S)
// cHisFits::createFits - creates the fits file. 
// 
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
}

#include "fitsio.h"
#include "cadef.h"

#include "globals.H"
#include "hisFits.H"
#include "hisCa.H"
#include "hisEvent.H"
#include "hisLogWrite.H"


//
// Define and initialize static data. 
//

int	cHisFits::hfTFields = 4;
char	*cHisFits::hfTType[] = { "DATE", "NSEC", "LABEL", "VALUE" };
char	*cHisFits::hfTForm[] = { "J1", "J1", "40A", "40A" };
char	*cHisFits::hfTUnit[] = { "Unix Time", "Nanoseconds", 
					"ASCII", "ASCII" };


//
//***********************************************************************
//+
// FUNCTION NAME:
// createFits
//
// INVOCATION:
// createFits( inFilePath, fitsFilePath, extName, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) inFilePath	(char *) 	Path to the input file. 
// (>) fitsFilePath	(char *) 	Path to the fits file to write. 
// (>) extName		(char *) 	Extension name (DHS Label).
// (!) status 		(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Creates the fits file. 
//
// DESCRIPTION:
// Create a new fits file, add some keywords, add a table extension,
// Open the input file, add the file to the table extention, close
// the file, close the fits file, remove the input file. 
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

void		cHisFits::createFits
(
    char 	*inFilePath,	// (in)  Input file path. 
    char	*fitsFilePath,	// (in)  Output file path.
    char	*extName,	// (in)  ASCII table extension name. 
    cHisStatus	&status		// (mod) Status. 
)
{
    fitsfile	*cffp;		// Cfitsio file pointer. 
    int		cfStatus = 0;	// Cfitsio library status. 
    int		close_status;	// Return status from fclose().
    int		fileDesc;	// File descriptor. 
    int		remove_status;	// Return status from remove().


    checkStat( status, return );
    
    
    //
    // Open and initialize new empty FITS file.
    //

    checkFits( ffinit( &cffp, fitsFilePath, &cfStatus ), 
	    status, return );


    //
    // Add a primary header. Add extra keywords.
    //

    checkFits( ffphps( cffp, HCF_BITPIX, HCF_NAXIS, NULL, &cfStatus ), 
	    status, VOID );
    checkFits( ffpkys( cffp, HCF_INSTR_KWD, HCF_INSTR_VAL, HCF_INSTR_COM,
	    &cfStatus ), status, VOID );
    checkFits( ffpkys( cffp, HCF_TSCOP_KWD, HCF_TSCOP_VAL, HCF_TSCOP_COM,
	    &cfStatus ), status, VOID );


    //
    // Add table extension header and table. 
    //

    checkFits( ffcrtb( cffp, BINARY_TBL, 999, hfTFields, 
	    hfTType, hfTForm, hfTUnit, extName, &cfStatus ), 
	    status, VOID );


    //
    // Open input file. 
    //

    fileDesc = open( inFilePath, O_RDONLY );
    if ( fileDesc == -1 )
    {
	status.E_FOPEN( status, inFilePath );
	status.sysErrno();
    }


    //
    // Add data to fits table.
    //

    fitsAddEvents( fileDesc, cffp, status );


    //
    // Close input file. 
    //

    close_status = close( fileDesc );
    if ( close_status == -1 )
    {
	status.E_FCLOSE( status, inFilePath );
	status.sysErrno();
    }


    //
    // Close FITS file. 
    //

    checkFits( ffclos( cffp, &cfStatus ), status, return );


    //
    // If hisLogWrite cmd was stopped, remove FITS file. Else input file. 
    //
    
    if ( cHisLogWrite::stoppingHLW() )
    {
	//
	// Remove FITS file. 
	//

	remove_status = remove( fitsFilePath );
	if ( remove_status != 0 )
	{
	    status.E_FREMOVE( status, fitsFilePath );
	    status.sysErrno();
	}
    }
    else
    {
	//
	// Remove input file. 
	//

	remove_status = remove( inFilePath );
	if ( remove_status != 0 )
	{
	    status.E_FREMOVE( status, inFilePath );
	    status.sysErrno();
	}
    }

    
    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// fitsAddEvents
//
// INVOCATION:
// fitsAddEvents( fileDescriptor, cffp, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fileDescriptor	(int)		File descriptor to read events from.
// (!) cffp		(fitsfile *)	Fits File Pointer.
// (!) status 		(cPutStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Loop through input file, add to fits file. 
//
// DESCRIPTION:
// Loop through the fits file adding the events to the table extension. 
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

void		cHisFits::fitsAddEvents
(
    int		fileDescriptor,	// (in)  File descriptor to read events from.
    fitsfile	*cffp,		// (mod) Fits File Pointer.
    cHisStatus 	&status		// (mod) Program status.
)
{
    int		cfStatus = 0;	// Cfitsio library status.
    cHisEvent	event;		// Event object. 
    int		eventDate;	// Event date.
    int		eventNsec;	// Event nanoseconds. 
    char	*eventLabel;	// Event label.
    char	*eventValue;	// Event value.
    int		lineNumber;	// Line number in file. 
    bool	loopDone;	// Are we done looping? 


    checkStat( status, return );


    loopDone = false;
    for( lineNumber = 1; !loopDone; lineNumber++ )
    {
	//
	// If hisLogWrite cmd has been stooped by init or exit cmd, break.
	//

	if ( cHisLogWrite::stoppingHLW() )
	{
	    break;
	}


	//
	// Read line from input file. 
	//

	event.readRecord( fileDescriptor, loopDone );
	if ( loopDone )
	{
	    break;
	}
	eventDate = event.date();
	eventNsec = event.nsec();
	eventLabel = event.SIR();
	eventValue = event.value();


	//
	// Add data to FITS ASCII table cells. 
	//
	
	checkFits( ffpcl( cffp, TLONG, HCF_DATE_COLNUM, lineNumber, 1, 1, 
		&eventDate, &cfStatus ), 
		status, break );

	checkFits( ffpcl( cffp, TLONG, HCF_NSEC_COLNUM, lineNumber, 1, 1, 
		&eventNsec, &cfStatus ), 
		status, break );

	checkFits( ffpcls( cffp, HCF_LABEL_COLNUM, lineNumber, 1, 1, 
		&eventLabel, &cfStatus ), 
		status, break );

	checkFits( ffpcls( cffp, HCF_VALUE_COLNUM, lineNumber, 1, 1, 
		&eventValue, &cfStatus ), 
		status, break );
    }


    return;
}
