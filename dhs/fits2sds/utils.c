static char rcsid[] = "$Id: utils.c,v 1.1.1.1 2002-11-24 20:32:59 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) <year>				(c) <year>
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
 * All rights reserved			Tous droits reserves
 * 					
 * NRC disclaims any warranties,	Le CNRC denie toute garantie
 * expressed, implied, or statu-	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.	fortuit, resultant de l'utili-
 * 					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * <filename>
 *
 * PURPOSE:
 * Whatever
 *
 * FUNCTION NAME(S)
 * function-1 - description of the function
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:38  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.7  1998/05/14 18:45:15  jaeger
 * exit
 *
 * Revision 1.6  1998/04/30 06:05:41  jaeger
 * Changed byte_allocs to mallocs
 *
 * Revision 1.5  1998/01/14 17:57:16  jaeger
 * Fixed the resetting of the dataset info structure.
 *
 * Revision 1.4  1997/12/02 21:25:29  jaeger
 * Changed message printing so the sfMsg and f2sMsg get written and
 * cleared after each call to sf.
 *
 * Revision 1.3  1997/11/19 00:13:59  jaeger
 * Fixed messages so they are always printed and use the prefix "f2s"
 *
 * Revision 1.2  1997/09/05 23:56:18  jaeger
 * Fixed memory leaks.
 *
 * Revision 1.1  1997/09/05 19:25:37  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "gen_msg.h"


/*
 ************************************************************************
 *  sf library include files.
 ************************************************************************
 */

#include "sf.h"


/*
 ************************************************************************
 *  fits2Sds application include files.
 ************************************************************************
 */

#include "fits2sds.h"
#include "f2sLocal.h"
#include "f2sGlobals.h"

/*
 ************************************************************************
 * Internal Function prototypes.
 ************************************************************************
 */

void f2sResetDsInfo( SF_DS_INFO* );


/*
 ************************************************************************
 * External Function prototypes.
 ************************************************************************
 */

void f2sFormatMessage();



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sComposeChunks
 *
 * INVOCATION:
 * f2sComposeChunks( inputFileName, objectList, datasetInformation,
 *      	status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	inFile	    (char*)	    Input file name.
 * (>)	objectList  (EPTR*)	    List of SDS objects.
 * (!)	dsInfo	    (SF_DS_INFO*)   Dataset info. on the objects.
 * (!)	status	    (F2S_STATUS*)   fits2sds application status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To compose one or more SDS chunks into one FITS file, both in
 * memory and on disk.
 *

 * DESCRIPTION:
 * 1. Initialize variables
 * 2. Write all SDS objects to a single FITS disk file.
 * 3. Reset the dataset information.
 * 4. Write all SDS objects to a single FITS memory file.
 * 5. Reset the dataset information.
 *
 * EXTERNAL VARIABLES:
 * globals
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void f2sComposeChunks
(
    char	*inFile,	/* (in)  Input file name.		*/	
    EPTR	*objectList,	/* (in)  List of SDS objects.		*/
    SF_DS_INFO	*dsInfo,	/* (mod) Dataset info. on the objects.	*/
    F2S_STATUS  *status	   	/* (mod) fits2sds application status.	*/
)
{
    void	**buffer;	/* FITS memory buffer.			*/	
    int		bufFile;	/* File descriptor of the buffer's file.*/
    char        *filename;	/* Out put file name.			*/	
    long	fitsSize;	/* Size of the FITS memory buffer.	*/	
    fitsfile	*fptr;		/* FITS file pointer.			*/
    int		i;
    int		last;		/* Last SDS object.			*/
    SF_OBJECT	*sfObject;	/* Single SDS object.			*/
    SF_STATUS   sfStatus;	/* SF library status.			*/
    

    /*
     * Initialize variables.
     */
    
    sfStatus = SF_S_SUCCESS;

    
    /*
     * Construct the FITS disk file.
     */

    if ( globals.f2sDisk )
    {
	sfCheck( filename = sfGetFilename( inFile, COMPOSE, FITS_DISK,
		&sfStatus ), sfStatus, *status, VOID );
	sfCheck( fptr = sfOpenFitsFile( filename, "NEW", &sfStatus ),
		sfStatus, *status, VOID );

	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	    f2sFormatMessage( F2S_S_COMPOSE_FILE, "FITS disk file", filename );
	    f2sPrintMessage();
	}
	else
	{
	    msg_clear( sfMsg );
	}
	
    
	last = eptrlen( objectList );
	for( i = 0; i < last - 1; i++ )
	{
	    /*
	     * Extract the sf object from the EPTR list.
	     */
	
	    sfObject = (SF_OBJECT*)eptritem( objectList, i );

	
	    /*
	     * Use sfSds2Fits to write the dhsObject.
	     */

	    sfCheck( sfSds2Fits( sfObject->dhsObject, fptr, dsInfo, FALSE,
		    &sfStatus ), sfStatus, *status, VOID );
	}
	
	sfCheck( sfCloseFitsFile( fptr, &sfStatus ), sfStatus, *status, VOID );
	if( filename != NULL )
	{
	    gen_free( filename );
	    filename = NULL;
	}
    
	    
	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	}
	else
	{
	    msg_clear( sfMsg );
	}
	
	    
	/*
	 * Reset the dataset information.
	 */
    
	f2sResetDsInfo( dsInfo );
    }
    

    /*
     * Write the FITS memory buffer.
     */
    
    if ( globals.f2sMem )
    {
	
	/*
	 * write the small SDS structure into one FITS file in
	 * memory.
	 */

	fitsSize = 10;
	nullCheck( buffer = malloc( sizeof( buffer ) ), *status, VOID );
	nullCheck( buffer[0] = byte_alloc( fitsSize ), *status, VOID );
	sfCheck( fptr = sfSetFitsMem( buffer, &fitsSize, dsInfo, &sfStatus ),
		sfStatus, *status, VOID );

	sfCheck( filename = sfGetFilename( inFile, COMPOSE, FITS_MEMORY,
	    &sfStatus ), sfStatus, *status, VOID );
	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	    f2sFormatMessage( F2S_S_COMPOSE_FILE, "FITS memory file",
		    filename );
	    f2sPrintMessage();	
	}
	else
	{
	    msg_clear ( sfMsg );
	}
	
    
	last = eptrlen( objectList );
	for( i = 0; i < last - 1; i++ )
	{
	    /*
	     * Extract the sf object from the EPTR list.
	     */
	
	    sfObject = (SF_OBJECT*)eptritem( objectList, i );

	
	    /*
	     * Use sfSds2Fits to write the dhsObject.
	     */

	    sfCheck( sfSds2Fits( sfObject->dhsObject, fptr, dsInfo, FALSE,
		    &sfStatus ), sfStatus, *status, VOID );
	    
	}
	
	if( ( bufFile = open( filename, O_WRONLY | O_CREAT, 0664 ) ) > 2 )
	{
	    sfCheck( fitsSize = sfFitsFileSize( fptr, &sfStatus ), sfStatus,
		    *status, VOID );
	    sfCheck( sfCloseFitsFile( fptr, &sfStatus ), sfStatus, *status,
		    VOID );
	    write( bufFile, buffer[0], fitsSize );
	    close( bufFile );
	}
	gen_free( buffer[0] );
	gen_free( buffer );
	buffer = NULL;
	if( filename != NULL )
	{
	    gen_free( filename );
	    filename = NULL;
	}


	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	}
	else
	{
	    msg_clear( sfMsg );
	}

    
	/*
	 * Reset the dataset information.
	 */
    
	f2sResetDsInfo( dsInfo );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sResetDsInfo
 *
 * INVOCATION:
 * f2sResetDsInfo( datasetInformation )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!)	dsInfo	(SF_DS_INFO*)	 Dataset information structure.	
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To reset the dataset information so that it can be re-used with
 * the SF library.
 *
 *
 * DESCRIPTION:
 * Initialize all the indicies into the FITS file to NULL_INDEX,
 * reset the number of indicies to "0", and reset the instrument
 * value to the empty string.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * This relies on how the dataset information is used in the SF library.
 * This really should be something the SF library provides.
 *-
 ************************************************************************
 */

void f2sResetDsInfo
(
    SF_DS_INFO	*dsInfo		/* (mod) Dataset information structure.	*/
)
{
    SF_EXT_INFO	*ext;		/* Extension list for a dataset.	*/
    int 	i;		
    int		last;		/* Last item in the extension list.	*/
    
    /*
     * Initialize all indicies back to NULL, set the instrument to the
     * empty string and last to 0.
     */

    strcpy( dsInfo->instrument, "" );
    dsInfo->last = 0;
    if ( dsInfo->exts != NULL )
    {
	last = eptrlen( dsInfo->exts );
	for( i = 0; i < last; i++ )
	{
	    ext = (SF_EXT_INFO*)eptritem( dsInfo->exts, i );
	    ext->fitsIndex = NULL_INDEX;
	    ext->extWritten = 0;
	    strcpy( ext->frmName, "" );
	}
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sWriteChunks
 *
 * INVOCATION:
 * f2sWriteChunks( inputFileName, objectList, datasetInformation, status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	inFile	    (char*)	    Input file name.
 * (>)	objectList  (EPTR*)	    List of SDS objects.
 * (>)	dsInfo	    (SF_DS_INFO*)   Dataset info. on the objects.
 * (!)	status	    (F2S_STATUS*)   fits2sds application status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To write an SDS structure, expected to be a single chunk, as a
 * FITS file on disk, a FITS file in memory (which gets written
 * to disk), and an SDS buffer as a disk file.
 *
 * DESCRIPTION:
 * 1. Initialize variables.
 * 2. For each object in the object List
 *	a) Write the object as a FITS disk file.
 *	b) Write the object as a FITS memory buffer.
 * 	c) Write the object as a SDS memory buffer.
 *
 * EXTERNAL VARIABLES:
 * "globals".
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	f2sWriteChunks
(
    char	*inFile,	/* (in) Input file name.		*/
    EPTR	*objectList,	/* (in) List of SDS objects.		*/
    SF_DS_INFO	*dsInfo,	/* (in) Dataset info. on the objects.	*/
    F2S_STATUS  *status	   	/* (mod) fits2sds application status.	*/
)
{
    void	**buffer;	/* FITS memory buffer.			*/
    char        *filename;	/* Out put file name.			*/
    long	fitsSize;	/* Size of the FITS memory buffer.	*/
    fitsfile	*fptr;		/* FITS file pointer.			*/
    int		i;	
    char	id[30];		/* SDS identifier.			*/
    int		last;		/* Last SDS object.			*/
    SF_OBJECT	*sfObject;	/* Single SDS object.			*/
    SF_STATUS   sfStatus;	/* SF library status.			*/


    /*
     * Initizlize the variables.
     */
    
    sfStatus = SF_S_SUCCESS;
    last = eptrlen( objectList );
    
    for( i = 0; i < last - 1; i++ )
    {
	/*
	 * Extract the sf object from the EPTR list.
	 */
	
	sfObject = (SF_OBJECT*)eptritem( objectList, i );
	sprintf( id, "%d", i );

	
	/*
	 * Write the object to a FITS file on disk.
	 */
	
	if ( globals.f2sDisk )
	{
	    
	    if ( globals.f2sVerbose )
	    {
		printSfMsg();
		f2sFormatMessage( F2S_S_CHUNK_FILE, i, "FITS disk file" );
		f2sPrintMessage();
	    }
	    else
	    {	
		msg_clear( sfMsg );
	    }
	    
	    sfCheck( sfWriteDhsObject( sfObject->dhsObject, inFile, id,
		    FITS_DISK, TRUE, dsInfo, &sfStatus ), sfStatus, *status,
		    VOID );
	}
	

	/*
	 * Write the object to a FITS file in memory.
	 */

	if ( globals.f2sMem )
	{
	    
	    if ( globals.f2sVerbose )
	    {
		printSfMsg();
		f2sFormatMessage( F2S_S_CHUNK_FILE, i, "FITS memory file" );
		f2sPrintMessage();
	    }
	    else
	    {
		msg_clear( sfMsg );
	    }

	    fitsSize = 10;
	    nullCheck( buffer = (void**)malloc( 4 ), *status, VOID );
	    nullCheck( *buffer = malloc( fitsSize ), *status, VOID );
	    sfCheck( fptr = sfSetFitsMem( buffer, &fitsSize, dsInfo,
		    &sfStatus ), sfStatus, *status, VOID );
	    sfCheck( sfSds2Fits( sfObject->dhsObject, fptr, dsInfo, TRUE,
		    &sfStatus ), sfStatus, *status, VOID );
	
	    sfCheck( filename = sfGetFilename( inFile, id, FITS_MEMORY,
		    &sfStatus ), sfStatus, *status, VOID );
	    sfCheck( sfFitsFlush( fptr, &sfStatus ), sfStatus, *status, VOID );
	    sfCheck( fitsSize = sfFitsFileSize( fptr, &sfStatus ), sfStatus,
		    *status, VOID );
	    sfCheck( sfWriteMemToFile( buffer, filename, &fitsSize,
		    &sfStatus ), sfStatus, *status, VOID );
	    gen_free( buffer[0] );
	    gen_free( buffer );
	    buffer = NULL;
	    sfCheck( sfCloseFitsFile( fptr, &sfStatus ), sfStatus, *status,
		    VOID );
	    if( filename != NULL )
	    {
		gen_free( filename );
		filename = NULL;
	    }		
	}
	

	/*
	 * Write the object to a SDS file on disk.
	 */

	if ( globals.f2sSds )
	{
	
	    if ( globals.f2sVerbose )
	    {
		printSfMsg();
		f2sFormatMessage( F2S_S_CHUNK_FILE, i, "SDS file" );
		f2sPrintMessage();
	    }
	    else
	    {
		msg_clear( sfMsg );
	    }

	    sfCheck( sfWriteDhsObject( sfObject->dhsObject, inFile, id,
		    SDS_MEMORY, 	TRUE, dsInfo, &sfStatus ), sfStatus,
		    *status, VOID );
	}

	    if ( globals.f2sVerbose )
	    {
		printSfMsg();
	    }
	    else
	    {
		msg_clear( sfMsg );
	    }
    }
}



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sWriteFull
 *
 * INVOCATION:
 * f2sWriteFull( inputFileName, objectList, datasetInformation, status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	inFile	    (char*)	    Input file name.
 * (>)	objectList  (EPTR*)	    List of SDS objects.
 * (!)	dsInfo	    (SF_DS_INFO*)   Dataset info. on the objects.
 * (!)	status	    (F2S_STATUS*)   fits2sds application status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To write an SDS structure containing the a complete FITS file
 * as an SDS memory file, FITS disk file and FITS memory file.
 *
 * DESCRIPTION:
 * Description of the algorithm.
 *
 * EXTERNAL VARIABLES:
 * "globals".
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - other function name.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void f2sWriteFull
(
    char	*inFile,	/* (in) Input file name.		*/	
    SF_OBJECT	*sfObject,	/* (in) List of SDS objects.		*/	
    SF_DS_INFO	*dsInfo,	/* (mod) Dataset info. on the objects.	*/
    F2S_STATUS  *status	   	/* (mod) fits2sds application status.	*/
)
{
    void	**buffer;	/* FITS memory buffer.			*/	
    int		bufFile;	/* File descriptor of the buffer's file.*/
    char        *filename;	/* Out put file name.			*/	
    long	fitsSize;	/* Size of the FITS memory buffer.	*/	
    fitsfile	*fptr;		/* FITS file pointer.			*/
    SF_STATUS   sfStatus;	/* SF library status.			*/
    

    /*
     * Initialize variables.
     */
    
    sfStatus = SF_S_SUCCESS;

    
    /*
     * Write the object to a FITS file in memory.
     */

    if ( globals.f2sMem )
    {
	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	    f2sFormatMessage( F2S_S_FULL_FILE, "FITS memory file" );
	    f2sPrintMessage();
	}
	else
	{
	    msg_clear( sfMsg );
	}

	fitsSize = 10;
	buffer = malloc( sizeof( buffer ) );
	buffer[0] = byte_alloc( fitsSize );
	sfCheck( fptr = sfSetFitsMem( buffer, &fitsSize, dsInfo, &sfStatus ),
		sfStatus, *status, VOID );
	sfCheck( sfSds2Fits( sfObject->dhsObject, fptr, dsInfo, FALSE,
		&sfStatus ), sfStatus, *status, VOID );
	
	sfCheck( filename = sfGetFilename( inFile, FULL, FITS_MEMORY,
	    &sfStatus ), sfStatus, *status, VOID );
	if( ( bufFile = open( filename, O_WRONLY | O_CREAT, 0664 ) ) > 2 )
	{
	    sfCheck( fitsSize = sfFitsFileSize( fptr, &sfStatus ), sfStatus,
		    *status, VOID );
	    sfCheck( sfCloseFitsFile( fptr, &sfStatus ), sfStatus, *status,
		    VOID );
	    write( bufFile, buffer[0], fitsSize );
	    close( bufFile );
	}
	gen_free( buffer[0] );
	gen_free( buffer );
	buffer = NULL;
	if( filename != NULL )
	{
	    gen_free( filename );
	    filename = NULL;
	}


	/*
	 * Reset the dataset information.
	 */
    
	f2sResetDsInfo( dsInfo );
    }
    

    /*
     * Write the object as a FITS disk file.
     */
    if ( globals.f2sDisk )
    {
	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	    f2sFormatMessage( F2S_S_FULL_FILE, "FITS disk file" );
	    f2sPrintMessage();
	}
	else
	{
	    msg_clear( sfMsg );
	}

	sfCheck(  sfWriteDhsObject( sfObject->dhsObject, inFile, FULL,
		FITS_DISK, FALSE, dsInfo, &sfStatus ), sfStatus, *status,
		VOID );
	
	/*
	 * Reset the dataset information.
	 */
    
	f2sResetDsInfo( dsInfo );
    }
    

    /*
     * Write the SDS object.
     */

    if ( globals.f2sSds )
    {
	
	if ( globals.f2sVerbose )
	{
	    printSfMsg();
	    f2sFormatMessage( F2S_S_FULL_FILE, "SDS file" );
	    f2sPrintMessage();
	}
	else
	{
	    msg_clear( sfMsg );
	}

	sfCheck( sfWriteDhsObject( sfObject->dhsObject, inFile, FULL,
		SDS_MEMORY, FALSE, dsInfo, &sfStatus ), sfStatus, *status,
		VOID );
    }
    
    if ( globals.f2sVerbose )
    {
	printSfMsg();
    }
    else
    {
	msg_clear( sfMsg );
    }
}
