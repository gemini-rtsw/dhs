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


#include <iostream.h>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
}

#include "globals.H"
#include "hisPutFile.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisPutFile::cHisPutFile
//
// INVOCATION:
// putDataset = new pPutDs( filename );
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

		cHisPutFile::cHisPutFile
(
    cHisStatus		&status,	// (mod) Status to check and return.
    char		*filePath,	// (in)  File path.
    void		**buffer,	// (out) Buffer to hold file.
    int			&bufSize	// (out) Size of buffer.
)
:cDhsPutDataset( NULL, 0, DHS_BD_PT_FITS )
{
    struct stat statStruct;		// Output of stat() call.
    FILE	*fp;			// File pointer.
    int		sizeRead;		// Actual size read from fread().


    //
    // Check status. Initialize buffer size.
    //

    checkStat( status, return );
    bufSize = 0;


    //
    // Ensure the file exists with stat(). Get the size.
    //

    if ( stat( filePath, &statStruct ) == -1 )
    {
	status.E_FSTAT( status, filePath );
	status.sysErrno();
	return;
    }
    bufSize = statStruct.st_size;


    //
    // Open file for reading.
    //

    if ( ( fp = fopen( filePath,  "r" ) ) == NULL )
    {
	status.E_FOPEN( status, filePath );
	status.sysErrno();
        return;
    }


    //
    //  Allocate room. Read file.
    //

    *buffer = gen_alloc( ( bufSize + 1 ) );
    sizeRead = fread( *buffer, bufSize, 1, fp );

    if ( sizeRead < 1 )
    {
	status.E_FREAD( status, filePath );
	status.sysErrno();
    }

    (void) fclose( fp );

    //
    // Initialize data member. 
    //

    hpfNewLabel = NULL;


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cHisPutFile::done
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

void		cHisPutFile::done
(
    DHS_CMD_STATUS	cmdStatus,	// (in)  Command Status
    char		*string,	// (in)  Returned String. 
    cDhsAvList		&avList		// (in)  Returned AvList.
)
{
    cHisStatus		status;
    cDhsAttrib		attribute;
    DHS_STATUS		dhsStatus( DHS_S_SUCCESS );
    void		*pData;


    if ( cmdStatus == DHS_CS_BUSY || cmdStatus == DHS_CS_DONE )
    {
	return;
    }
    else if ( cmdStatus == DHS_CS_ERROR  )
    {
	attribute = avList.find( HIS_ATTR_NEWLABEL, dhsStatus );
	if ( dhsStatus == DHS_S_NO_ATTRIB )
	{
	    status.E_PUT( status, label() );
	    status.E_DATASERVER_MSG( status, string );
	}
	else
	{
	    checkDhs( VOID, dhsStatus, status, return );
	}
	attribute.info( (void **) &pData, dhsStatus );
	hpfNewLabel = strsav( (char *) pData );
    }


    return;
}
