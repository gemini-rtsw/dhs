static char rcsid[] = "$Id: utils.c,v 1.1.1.1 2002-11-24 20:35:53 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997			    	(c) 1997
 * National Research Council	    	Conseil national de recherches
 * Ottawa, Canada, K1A 0R6	    	Ottawa, Canada, K1A 0R6
 * All rights reserved		    	Tous droits reserves
 *					
 * NRC disclaims any warranties,    	Le CNRC denie toute garantie
 * expressed, implied, or statu- 	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-    	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,    	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.    	fortuit, resultant de l'utili-
 *					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * sfLibrary/src/utils.c
 *
 * PURPOSE:
 * Some utility fucntions for sf
 *
 * FUNCTION NAME(S)
 * sfCheckWithDd	:	Find the keyword from a given sds attribute,
 *			  	check it's type and return it's comment and
 *			  	array values.
 * sfCloseFitsFile	:	Closes a FITS file.
 * sfExtractKeyword	: 	Extract the keyword from a FITS cfitsio card.
 * sfFitsFileSize	:	Returns the size of a FITS file.
 * sfFitsFlush		:	Flush the FITS file. 
 * sfGetFilename	: 	Constructs a filename based on the path of the
 *			  	original FITS file, frame identifier and file
 *			  	extension.
 * sfGetValue		: 	Find the value of an attibute in a dhs object.
 * sfHasData		: 	Determine if data follows a FITS header.
 * sfObjectFree		: 	Free an SF object from memory.
 * sfOpenFitsFile  	: 	Open a fits file and return its pointer.
 * sfSetFitsMem		: 	Prepares cfitsio for writing a FITS file to
 *			  	a memory buffer.
 * sfWriteDhsObject	: 	Writes a dhsObject either to a FITS file or
 *			  	a SDS file.
 * sfWriteMemToFile	:	Write a FITS memory buffer to a disk file.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.13  1999/02/09 20:23:39  jaeger
 * Changed the logic an error message reported by sfCheckWithDd.  Error
 * message is not SF_S_NOT_FOUND, and this is reported if the attribute
 * is not found in the data dictionary at all. (used to complain if it
 * didn't find it for the given instrument.)
 *
 * Revision 1.12  1999/01/26 06:47:54  jaeger
 * Added SF_E_DATA_TYPE to checkWithDd
 *
 * Revision 1.11  1998/05/25 21:43:43  jaeger
 * Added more informative message for dhsBdAttribInfo errors.
 *
 * Revision 1.10  1998/05/14 18:47:39  jaeger
 * Made a more informative error message when an attirbute is not found
 * on a SDS structure.
 *
 * Revision 1.9  1998/04/30 06:10:20  jaeger
 * Removed all occurence of FITS longname function and usage of FITS
 * file pointer structure values.  sfGetValue now optionally reports
 * an error if the attribute is not found.
 *
 * Revision 1.8  1997/12/02 21:02:56  jaeger
 * Changed sfCheckWithDd so that all "out" variables have a value even
 * if the attribute was not found in the data dictionary.
 *
 * Revision 1.7  1997/09/05 23:04:53  jaeger
 * fixed remaining memory leaks.
 *
 * Revision 1.6  1997/09/05 18:58:07  jaeger
 * Fixed some memory leaks
 *
 * Revision 1.5  1997/08/22 22:10:35  jaeger
 * Added support for multi-dimensional arrays.
 *
 * Revision 1.4  1997/08/19 19:03:45  jaeger
 * Checked in for install
 *
 * Revision 1.3  1997/08/15 20:43:37  jaeger
 * Checked in for install
 *
 * Revision 1.2  1997/08/12 21:07:38  jaeger
 * Checked in for install
 *
 * Revision 1.1  1997/08/06 18:30:48  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


/*
 ************************************************************************
 *  General unix include files.
 ************************************************************************
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/errno.h>


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
#include "localSf.h"

void	sfFormatMessage();


/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfCheckWithDd
 *
 * INVOCATION:
 * sfCheckWithDd( instrument, type, ndims, attribute, arrayStatus, comment,
 *			keyword, writeToFileFlag, status )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) instrument   (char[FLEN_KEYWORD])    Instrument to search with.
 * (>) type	    (DHS_DATA_TYPE)         Data type of the attribute.
 * (>) ndims	    (int)		    Number of dimensions.
 * (>) attribute    (char*)		    Name of the attribute.
 * (<) array	    (DD_ARRAY_TYPE)         Attribute array indicator.
 * (<) comm	    (char[FLEN_COMMENT])    Comment for the FITS file.
 * (<) keyword	    (char[FLEN_KEYWORD])    Keyword for the FITS file.
 * (<) writeToFile  (boolean)		    Write to FITS file indicator.
 * (!) sfStatus	    (SF_STATUS*)	    Sf library status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Check the validity of an attribute with the data dictionary and
 * find the keyword and comments to be written to the FITS file.
 *
 * DESCRIPTION:
 * 1. If the attribute is required by FITS ignore it.
 * 2. If the attribute is required by DHS ignore it.
 * 3. Find the row of information in the data dictionary.
 * 4. Check the attributes type with the expected type given in the
 *    data dictionary.  If they don't match change "sfStatus" to
 *    SF_E_TYPE.
 * 5. Based on the data dictionary values set the "keyword", "comm"
 *    and "array" values.
 * 
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * sfInit has been called prior to the invocation of sfCheckWithDD.
 *
 * DEFICIENCIES:
 * Ignores all keywords beginning with the character '_'.
 *-									
 ************************************************************************
 */

void sfCheckWithDd
(   		
    char	    instrument[FLEN_KEYWORD],
				/* (in)  Instrument to search with.	*/
    DHS_DATA_TYPE   type,	/* (in)  Data type of the attribute.  	*/
    int		    ndims,	/* (in)  Number of dimensions.		*/
    char	    *attribute,	/* (in)  Name of the attribute.		*/
    DD_ARRAY_TYPE   *array,	/* (out) Attribute array indicator.	*/
    char	    comm[FLEN_COMMENT],
				/* (out) Comment for FITS file. 	*/
    char	    keyword[FLEN_KEYWORD],
    boolean	    *writeToFile,/* (out) Write to FITS file indicator. */ 
    SF_STATUS	    *sfStatus	 /* (mod) Sf library status.	        */
)									
{
    DD_STATUS	ddStatus;	/* Status from dd routines.		*/
    DD_ROW	*row;		/* Keyword/attribute dd object.		*/

    
    fnEntry( "sfCheckWithDd", *sfStatus );
    statCheck( *sfStatus, VOID );
    isInit( *sfStatus, VOID );
    
    ddStatus = DD_S_SUCCESS;

	
    /*
     * If the attribute starts with "_" then this is not something that
     * goes in the FITS header.  Also ignore "origin" and "axisSize"
     * since these are temporary - only appear in FITS files sent to
     * the quick look server.
     */

    if ( strncmp( attribute, "_", 1 ) == 0 ||
            streq( attribute, "axisSize" ) ||
           streq( attribute, "origin" ) )
    {	    
	*array = DD_NO_ARRAY;
	comm[0] = '\0';
	*writeToFile = FALSE;
	fnReturn( VOID );
    }

   
    /*
     * Extract the information for the keyword from the data dictionary
     */

    ddKeywordGet( instrument, attribute, &row, &ddStatus );

    if ( ddStatus != DD_S_SUCCESS && strne( instrument, "*" ) )
    {
	ddStatus = DD_S_SUCCESS;
	ddKeywordGet( "*", attribute, &row, &ddStatus );
	if ( ddStatus != DD_S_SUCCESS )
	{	
	    *sfStatus = SF_S_NOT_FOUND;
	    sfFormatMessage( SF_S_NOT_FOUND, attribute, "*" );
	}
	else
	{
	    sfFormatMessage( SF_S_NOT_FOUND, attribute, instrument );
	}
    }
    else if ( ddStatus != DD_S_SUCCESS )
    {
	*sfStatus = SF_S_NOT_FOUND;
	sfFormatMessage( SF_S_NOT_FOUND, attribute, instrument );
    }
        

    if ( ddStatus != DD_S_SUCCESS )
    {
	/*
	 * The attribute was not found so make sure the return values
	 * are initialized to something.
	 */

	*writeToFile = TRUE;
	strnCpy ( keyword, attribute, FLEN_KEYWORD, *sfStatus, VOID );
	strnCpy ( comm, "", FLEN_COMMENT, *sfStatus, VOID );
	switch ( ndims )
	{
	    case 0 : *array = DD_NO_ARRAY; break;
	    case 1 : *array = DD_INDEX; break;
	    default: *array = DD_EXTENSION; break;
	}
	msg_append( sfMsg, ddMsg );
	msg_clear( ddMsg );
    }
    else
    {
	/*
	 * Get the keyword, array and comment values.
	 */
	
	strnCpy( keyword, row->ddrKeyword, FLEN_KEYWORD, *sfStatus, VOID );
	strnCpy( comm, row->ddrCmts, FLEN_COMMENT, *sfStatus, VOID );
	*array = row->ddrArray;
	*writeToFile = TRUE;


	/*
	 * Verify the data type.
	 */
        
	if ( row->ddrType != DD_ANY_TYPE && type != row->ddrType )
	{
	    *sfStatus = SF_S_TYPE;
	    sfFormatMessage( *sfStatus, attribute );
	}
    }
    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfCloseFitsFile
 *
 * INVOCATION:
 * sfCloseFitsFile( fitsFilePointer, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)  fptr	    (fitsfile*)		The FITS file to close.
 * (!) 	sfStatus    (SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Close a FITS file.
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void 	sfCloseFitsFile
(
    fitsfile	*fptr,		/* (in)  The FITS file to close.	*/
    SF_STATUS	*sfStatus	/* (mod) SF library status.		*/
)
{
    FITS_STATUS	fitsStatus;	/* Cfitsio status indicator.		*/
    
    fnEntry( "sfCloseFitsFile", *sfStatus );
    statCheck( *sfStatus, VOID );

    fitsStatus = FITS_S_SUCCESS;
    
    fitsCheck(  ffclos( fptr, &fitsStatus ), fitsStatus, *sfStatus, VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfExtractKeyword
 *
 * INVOCATION:
 * keyword = sfExtractKeyword( card, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) card	(char)		A card as defined in cfitsio library.
 * (!) sfStatus (SF_STATUS*)  	Return status for sf library.
 *
 * FUNCTION VALUE:
 * (char*)  : NULL if an error occurred
 * (char*)  : The keyword in the card, if no errors occured.
 *
 * PURPOSE:
 * To extract out the keyword from a card read from a FITS file.
 *
 * DESCRIPTION:
 * Copy the contents of the card starting at the left until the
 * first space or "=" is encountered into the keyword.
 *
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

char *sfExtractKeyword
(
    char	card[FLEN_CARD], /* (in) standard string lengths 	*/
				 /*	 defined in the fitsio.     	*/
    SF_STATUS	*sfStatus	 /* (mod) Function return status.	*/

)
{
    int	       i;   
    char       *keyword;	/* Keyword extracted from the card.	*/
    
    fnEntry( "sfExtractKeyword", *sfStatus );
    statCheck( *sfStatus, NULL );

    nullCheck( keyword = char_alloc ( FLEN_KEYWORD ), *sfStatus, NULL );
    
    i = 0;
    while ( ( ( card[i] != ' ' ) && 
			( card[i] != '=' ) ) && 
			( i < FLEN_KEYWORD ) )
    {
	keyword[i] = card[i];
	i++;
    }

	
    /*
     * Null terminate the string.
     */
	
    if ( i < FLEN_KEYWORD )
    {
	keyword[i] = '\0';
    }
    else
    {
	keyword[FLEN_KEYWORD-1] = '\0';
    }

    fnReturn( keyword );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFitsFileSize
 *
 * INVOCATION:
 * size = sfFitsFileSize( fitsFilePointer, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fptr	(fitsfile*)		The FITS file.
 * (!) sfStatus	(SF_STATUS*)		SF library status.
 *
 * FUNCTION VALUE:
 * (int) : The size of the FITS file if no errors have occured, if
 *	   an error occurs then SF_NULL_SIZE is return.
 *
 * PURPOSE:
 * To find the size, in bytes, of a FITS file.
 *
 * DESCRIPTION:
 * Add the sizes of all the HDU's to find the SIZE of the entire file.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

long sfFitsFileSize
(
    fitsfile	*fptr,		/* (in)  The FITS file.			*/
    SF_STATUS	*sfStatus	/* (mod) SF library status.		*/
)
{
    long	curHdu;		/* Address of the current header unit.	*/
    int		curPos;		/* Current position of the FITS file.	*/
    FITS_STATUS	fitsStatus;	/* Cfitsio library status.		*/
    int		hdutype;	/* Type of the current header unit.	*/
    int		i;
    long	nextHdu;	/* Address of the next header unit.	*/
    long	size;		/* Size of the FITS file.		*/

    fnEntry( "sfFitsFileSize", *sfStatus );
    statCheck( *sfStatus, SF_NULL_SIZE );

    /*
     * Initialize variables.  The curhdu values start at 0 in the
     * file pointers structure but the cfitsio function consider
     * the first header unit to be 1.
     */

    fitsStatus = FITS_S_SUCCESS;
    size = 0;
    ffghdn( fptr, &curPos );

    
    /*
     * Cycle through all of the header units and find the size of
     * each.
     */

    ffmahd( fptr, 1, &hdutype, &fitsStatus );
    for( i = 1; fitsStatus == FITS_S_SUCCESS; i++ )
    {
	long endHdu; // XXX allan: added
	if ( ffghad( fptr, &curHdu, &nextHdu, &endHdu, &fitsStatus ) != FITS_S_SUCCESS )
	{
	    *sfStatus = SF_E_FITS;
	    sfFormatMessage( *sfStatus, "ffghad( fptr, &curHdu, &nextHdu )",
		    fitsStatus );
	    return SF_NULL_SIZE;
	}
	
	size += nextHdu - curHdu;
	ffmahd( fptr, i+1, &hdutype, &fitsStatus );
    }

    /*
     * Move back to the original position in the file.
     */

    fitsStatus = FITS_S_SUCCESS;
    fitsCheck( ffmahd( fptr, curPos, &hdutype, &fitsStatus ), fitsStatus,
	    *sfStatus, SF_NULL_SIZE );

    fnReturn( size );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFitsFlush
 *
 * INVOCATION:
 * sfFitsFlush( fitsFilePointer, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fptr	(fitsfile*)	FITS file pointer.
 * (!) sfStatus	(SF_STATUS*)	SF library status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Flush a FITS file.
 *
 * This is partiuclarily useful with FITS memory buffer.  Calling the
 * flush routing ensures that the contents of the FITS file is in the
 * memory buffer.
 *
 * DESCRIPTION:
 * Call "ffflus", cfitsio routine, to flush the buffer.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */


void	sfFitsFlush
(
    fitsfile	*fptr,		/* (in)  FITS file pointer.		*/
    SF_STATUS	*sfStatus	/* (mod) SF library status.		*/
)
{
    FITS_STATUS	fitsStatus;

    
    /*
     * Flush the FITS buffer so the FITS file gets updated.
     */

    fitsStatus = FITS_S_SUCCESS;
    fitsCheck( ffflus( fptr, &fitsStatus ), fitsStatus, *sfStatus, VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfGetValue
 *
 * INVOCATION:
 * sfGetValue( dhsObject, attributeName, type, numberOfDimensions,
 *		dimensionArray, attributeValue, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) dhsObject    (DHS_BD_OBJECT)	The dataset or frame to query.
 * (>) name	    (char*)		Attribute whose value is wanted.
 * (>) type	    (DHS_DATA_TYPE*)	Type of the attribute.	  	   
 * (>) ndims	    (int*)		Number of dimensions.		  
 * (>) dims	    (unsigned long*)	Size of each dimension.	   
 * (>) value	    (void**)		Pointer to its value.
 * (!) sfStatus	    (SF_STATUS*)	Return status for SF library.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To obtain the value of a specified keyword from a dhsObject
 * (frame or dataset).
 *
 * DESCRIPTION:
 * 1. Find the attribute identifier of the given attribute.
 * 2. Get the attributes bits of information.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The dataset or frame was created with dhsBdDsNew or dhsBdFrameNew.
 *
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfGetValue
(
    DHS_BD_OBJECT   dhsObject,	/* (in)  Dataset or frame to query.	*/
    boolean	    existErr,	/* (in)  TRUE if name not found is error.*/
    char	    *name, 	/* (in)  Attribute to search for.	*/
    DHS_DATA_TYPE   *type,	/* (out) Type of the attribute.		*/
    int		    *ndims,	/* (out) Number of dimensions.		*/
    unsigned long   *dims,	/* (out) Size of each dimension.  	*/
    void	    **value,	/* (out) Pointer to its value.		*/
    SF_STATUS 	    *sfStatus  	/* (mod) SF function status.  		*/
)
{
    DHS_BD_ATTRIB_ID	attribId;    	/* Attribute id.		*/
    char	      	*attribName;	/* Name of the attribufe.     	*/
    DHS_STATUS	      	dhsStatus;      /* From dhs routines.		*/

    fnEntry( "sfGetValue", *sfStatus );
    statCheck( *sfStatus, VOID );

    dhsStatus = DHS_S_SUCCESS;

	
    /*
     * Search for the given attribute.
     */

    if ( existErr )
    {
	attribId = dhsBdAttribFind ( dhsObject, name, &dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    *sfStatus = SF_E_ATTRIB_NOT_FOUND;
	    sfFormatMessage( *sfStatus, name, dhsStatus );
	    fnReturn( VOID );
	}
    }
    else
    {
	attribId = dhsBdAttribFind ( dhsObject, name, &dhsStatus );
	if ( dhsStatus != DHS_S_SUCCESS )
	{
	    *sfStatus = SF_E_DHS;
	    fnReturn( VOID );
	}
    }
    

	
    /*
     * Find the detailed information about the attribute.
     */

    dhsBdAttribInfo ( attribId, &attribName, type, ndims, dims,
	    value, &dhsStatus );
    if ( dhsStatus != DHS_S_SUCCESS )
    {
	*sfStatus = SF_E_ATTRIB_INFO;
	sfFormatMessage( *sfStatus, name, dhsStatus );
	fnReturn( VOID );
    }
    

    fnReturn( VOID );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfGetFilename
 *
 * INVOCATION:
 * filename = sfGetFilename( pathname, frameid, ext, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pathname	(const char*) 	Pathname of the original FITS file.
 * (>) frameId	(const char*)	Unique string that identifies a frame.
 * (>) ext	(const char*)	The files extension.
 * (!) sfStatus (SF_STATUS*)	Function status for SF library.
 *
 * FUNCTION VALUE:
 * (char*) : "filename" if no errors occured.
 * (char*) : NULL if errors occured.
 *
 * PURPOSE:
 * To create a new file name based on a pathname (path + filename) and
 * a frameId.  The new pathname will be of the form:
 *    basename.frameId.extension.
 *
 * Note: that the new filename does not contain the path from the pathname
 *	 that was passed into the function.
 *
 * DESCRIPTION:
 * 1. extract out the basename ( path and extsension are removed from
 *    the pathname ).
 * 2. Add .framId.extension to the basename.
 * 3. return the filename if no errors occured, if there were errors
 *    then return NULL.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

char  *sfGetFilename
( 
    const char	*pathname,  /* (in)  Pathname of the original FITS file.*/
    const char	*frameId,   /* (in)  Unique string identifing a frame. 	*/
    const char	*ext,	    /* (in)  The files extension.		*/
    SF_STATUS   *sfStatus   /* (mod) Function status for SF library.	*/
)
{
    char	*filename;    /* The name of the file.			*/
    int		size;	      /* the expected size of the string.	*/

    fnEntry ( "sfGetFilename", *sfStatus );
    statCheck( *sfStatus,  NULL );

    
    /*
     * Find the basename.
     */
	
    nullCheck( filename = strsav( strtail( (char*)pathname ) ), *sfStatus,
			NULL );
    strroot ( filename );

	
    /*
     * Add the frame identifier.
     */
	
    size = strlen( filename ) + 2;
    nullCheck( filename = strnapp( filename, ".", size ), *sfStatus, NULL );

    size += strlen( frameId );
    nullCheck( filename = strnapp( filename, (char*)frameId, size ), *sfStatus,
			NULL );
	 
    size += 1;
    nullCheck( filename = strnapp( filename, ".", size ), *sfStatus, NULL );


    /*
     * Add the file extension.
     */
	
    size += strlen( ext );
    nullCheck( filename = strnapp( filename, (char*)ext, size ), *sfStatus,
			NULL );

    fnReturn ( filename );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfHasData
 *
 * INVOCATION:
 * sfHasData = sfHasData( naxis, naxes );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) naxis	(long) 			Number of axies the data has in a
 *					FITS file.
 * (>) naxes	(long[SF_MAXDIM]) 	Array giving size of each axis.
 *
 * FUNCTION VALUE:
 * (boolean) : TRUE if there is data
 * (boolean) : FALSE if there is no data
 *
 * PURPOSE:
 * To determine if there is data associated with a particular header
 * unit.
 *
 * DESCRIPTION:
 * 1. If naxis = 0 then return FALSE
 * 2. If naxes[0] = 0 then return FALSE
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

boolean sfHasData
(
    int     naxis,	    	/* (in) Number of axes in the FITS data.*/
    long    naxes[SF_MAXDIM]	/* (in) Array giving size of each axis.	*/
)
{
    SF_STATUS	sfStatus = SF_S_SUCCESS;
    
    fnEntry( "sfHasData", sfStatus );


    /*
     * Check the "NAXIS" value.
     */
	
    if ( naxis == 0 )
    {
	fnReturn( FALSE );
    }


    /*
     * Check naxes[0] value.
     */

    if ( naxes[0] == 0 )
    {
	fnReturn( FALSE );
    }
    
    fnReturn( TRUE );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfObjectFree
 *
 * INVOCATION:
 * sfObjectFree( sfObjectPoinster );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) sfOjbect	(SF_DS_INFO*)	 SF object to be freed from memory.	
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Free a SF object from memory.  Intended to be used with "eptr_free"
 *
 * DESCRIPTION:
 * 1. Free the DHS object.
 * 2. Free the object's id.
 * 3. Free the object.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfObject" is not Null
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfObjectFree
(
    SF_OBJECT*	sfObject	/* SF object to be freed from memory.	*/
)
{
    DHS_STATUS	dhsStatus;	/* DHS library status.			*/

    /*
     * Initialize variables.
     */
    
    dhsStatus =	DHS_S_SUCCESS;

    dhsBdDsFree( sfObject->dhsObject, &dhsStatus );
    gen_free( sfObject->id );
    gen_free( sfObject);
    
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfOpenFitsFile
 *
 * INVOCATION:
 * filePtr = sfOpenFitsFile( fitsFilename, ReadWriteIndicator, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	fitsFilename	(char*)		The pathname of the fits file.
 * (>)	RW		(char*)		Open with Read and/or write. 
 * (!)  sfStatus	(SF_STATUS*)	Function status for SF library.
 *
 * FUNCTION VALUE:
 * (fitsfile*)  : A fits file pointer, if no errors occured
 * (fitsfile*)  : NULL, if errors occured
 *
 * PURPOSE:
 * To open a fits file for reading and/or writing.
 *
 * DESCRIPTION:
 * 1. Initialize fits status
 * 2. Based on the read/write indicator open the fits file accordingly.
 * 3. If an error has occured return NULL, otherwise return the pointer.
 * 
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

fitsfile *sfOpenFitsFile
( 
    char       *fitsFilename,	/* (in)  The pathname of the fits file.	*/
    char       *RW,		/* (in)  Read/write indicator.		*/
    SF_STATUS  *sfStatus	/* (mod) Function status for SF library.*/
)

{
    FITS_STATUS fitsStatus;	/* From cfitsio routines.		*/
    fitsfile   *fptr;		/* Pointer to fitsfile object.		*/
    int	       unlinkStatus;	/* Return value from "unlink".		*/


    fnEntry ( "sfOpenFitsFile", *sfStatus );
    statCheck( *sfStatus,  NULL );

    fitsStatus = FITS_S_SUCCESS;
    
    if ( streq( RW, "READONLY" ) )
    {
	/*
	 * Open the FITS file for reading only.
	 */
	
	fitsCheck( ffopen( &fptr, fitsFilename, READONLY, &fitsStatus ),
		fitsStatus, *sfStatus, NULL );
    }
    else if ( streq( RW, "READWRITE" ) )
    {
	/*
	 * Open the FITS file for reading and writing.
	 */
	
	fitsCheck( ffopen( &fptr, fitsFilename, READWRITE, &fitsStatus ),
		fitsStatus, *sfStatus, NULL );
    }
    else if ( streq( RW, "NEW" ) )
    {
	/*
	 * Create a new file and open it for writing.
	 * An error occurs if unlink return a value other than
	 * 0 or ENOENT ( no such file or directory ).
	 */
	
	unlinkStatus = unlink( fitsFilename );
	if ( unlinkStatus != 0 && errno != ENOENT ) 
	{
	    *sfStatus = SF_E_FITS_FILE ;
	    sfFormatMessage( *sfStatus, fitsFilename );
	}
	
	fitsCheck( fits_create_file( &fptr, fitsFilename, &fitsStatus ),
		fitsStatus, *sfStatus, NULL );
    } 
    else
    {
	*sfStatus = SF_E_FILE_TYPE;
	sfFormatMessage( *sfStatus, RW );
    }


    /*
     * Return the appropriate value, based on the function status.
     */

    if ( *sfStatus >= SF_S_SUCCESS )
    {
	fnReturn( fptr );
    } 
    else 
    {
	fnReturn ( NULL );
    }
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfSetFitsMem
 *
 * INVOCATION:
 * fitsfilePtr = sfSetFitsMem( buffer, size, datasetInformation, sfStatus)
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	buffer	    (void*)	    Block of memory for the FITS file.	
 * (>)	size	    (long)	    Size of the memory buffer.		
 * (>)	dsInfo	    (SF_DS_INFO*)   Dataset information.	
 * (!)  sfStatus    (SF_STATUS*)    Status messsage for SF library. 
 *
 * FUNCTION VALUE:
 * (fitsfile*) : If no errors occured a pointer to a FITS file in memory.
 * (fitsfile*) : NULL if an error has occured.
 *
 * PURPOSE:
 * Intialize a FITS file in the given memory block.
 *
 * DESCRIPTION:
 * 1. Set the memory buffer to a FITS file.
 * 2. Create/open the FITS file for writing.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" has a value greater than or equal to  SF_S_SUCCESS.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

fitsfile    *sfSetFitsMem
(
    void	**buffer,   /* (in)  Block of memory for the FITS file.	*/
    long	*size,	    /* (in)  Size of the buffer.		*/
    SF_DS_INFO	*dsInfo,    /* (in)  Dataset information.		*/
    SF_STATUS	*sfStatus   /* (mod) Status messsage for FITS routines. */
)
{
    fitsfile	*fptr;
    FITS_STATUS	fitsStatus;
    
    fnEntry( "sfSetFitsMem", *sfStatus );
    statCheck( *sfStatus, NULL );

    fitsStatus = FITS_S_SUCCESS;

    /* XXX allan: 11/24/02: outdated code for cfitsio2430
    (void) ffsbuf( &fptr, buffer, (unsigned int*)size, 2880, realloc,
	    &fitsStatus );
    fitsCheck( ffinit( &fptr, dsInfo->name, &fitsStatus ), fitsStatus,
	    *sfStatus, NULL );
    */
    /* XXX allan: here is the replacement code for the above */
    fitsCheck( ffimem( &fptr, buffer, (unsigned int*)size, 2880, realloc, &fitsStatus ), 
	       fitsStatus, *sfStatus, NULL );
    

    fnReturn( fptr );
}

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteDhsObject
 *
 * INVOCATION:
 * sfWriteDhsObject( dhsObject, pathname, Id, fileExtension, oneChunk,
 *			dsInfo, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	dhsObject   (DHS_BD_OBJECT)	The dhs object to be written.
 * (>)	pathname    (const char*)	Pathname for the file.
 * (>)	id	    (const char*)	The object id.
 * (>)	ext	    (const char*)	File extension.
 * (>)	oneChunk    (boolean)		TRUE if chunk is written to a
 *					file by itself, FALSE otherwise.
 * (!)	dsInfo	    (SF_DS_INFO*)	Dataset information.
 * (!)	sfStatus    (SF_STATUS*)	Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Statement of purpose	 
 * To write a SDS structure to disk.
 *
 * How the file gets written depends on the value of "ext".  If "ext"
 * begins with "fit" then it will be written as a FITS file.  if "ext"
 * has value "sds" then an SDS buffer is written to file.
 *
 * DESCRIPTION:
 * If "ext" begins with "fit" then
 *	open a new FITS file
 *	convert the SDS structure to a FITS file on disk.
 * else if "ext" is "sds" then
 *	write the SDS buffer.
 * else
 *	unable to write file.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatu" has value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void sfWriteDhsObject
(
    DHS_BD_OBJECT dhsObject,	/* (in)	 The dhs object to be written.	*/
    const char	  *pathname,	/* (in)	 Pathname for the file.		*/
    const char	  *id,		/* (in)	 The object id.			*/
    const char	  *ext,		/* (in)	 File extension.		*/
    boolean	  oneChunk,	/* (in)  TRUE if chunk is written to a	*/
				/*       single file, FALSE otherwise.  */
    SF_DS_INFO	  *dsInfo,	/* (mod) Dataset information.		*/
    SF_STATUS	  *sfStatus	/* (mod) Function return status.       	*/
)
{
    void	    *buffer;	/* Memory buffer for SDS structure.	*/
    DHS_STATUS	    dhsStatus;	/* Status from DHS library.		*/
    char       	    *filename;	/* Filname for a fits file.		*/
    fitsfile   	    *fptr;	/* Pointer to fitsfile object.		*/
    int	       	    fitsStatus; /* Status from cfitsio library.		*/
    int		    sdsFile;	/* Sds disk file.			*/
    unsigned long   size;	/* Size of the SDS memory buffer.	*/
    
    fnEntry( "sfWriteDhsObject", *sfStatus );
    statCheck( *sfStatus,  VOID );

    
    /*
     * Initialize variables.
     */
    
    fitsStatus = 0;
    dhsStatus = DHS_S_SUCCESS;
    check( filename = sfGetFilename( pathname, id, ext, sfStatus ), *sfStatus,
	    VOID );

    if ( strncmp( ext, "fit", 3 ) == 0 )
    {
	/*
	 * If the extension starts with "fit" then writing a FITS file.
	 */
	
	check( fptr = sfOpenFitsFile( filename, "NEW", sfStatus ), *sfStatus,
		VOID );

	sfSds2Fits( dhsObject, fptr, dsInfo, oneChunk, sfStatus );
	fitsCheck( ffclos( fptr, &fitsStatus ), fitsStatus, *sfStatus, VOID);
    }
    else if (streq( ext, "sds" ) )
    {
	/*
	 * If the extension is "sds" then writing an SDS file.
	 */

	size = dhsBdDsSize( dhsObject, &dhsStatus );
	buffer = (void*)malloc( size );
	dhsBdDsExport( dhsObject, buffer, size, &dhsStatus );
	
	if ( ( sdsFile = open( filename, O_WRONLY | O_CREAT, 0664 ) ) > 2 )
	{
	    if ( write( sdsFile, buffer, size ) < 0 )
	    {
		*sfStatus = SF_E_SDS_FILE;
		sfFormatMessage( *sfStatus, "creating", filename );
	    }
	    close( sdsFile );
	}
	else
	{
	    *sfStatus = SF_E_SDS_FILE;
	    sfFormatMessage( *sfStatus, "opening", filename );
	}
	gen_free( buffer );
    }
    else
    {
	*sfStatus = SF_E_FILE_EXT;
	sfFormatMessage( *sfStatus, ext );
    }

    if( filename != NULL )
    {
	gen_free( filename );
	filename = NULL;
    }
    
    
    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfWriteMemToFile
 *
 * INVOCATION:
 * sfWriteMemToFile( buffer, filename, &size, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	buffer		(void**)	FITS memory buffer.
 * (>)	filename	(char*)		Name of the disk file.
 * (!)	size		(long*)		Size of the buffer.
 * (!)	sfStatus	(SF_STATUS*)	SF library staus
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To Write a FITS memory buffer to a disk file.
 *
 * The actual size of the file written is returned in the "size" parameter.
 *
 * DESCRIPTION:
 *
 * 1. Check the size given (make sure it is greater than 1.)
 * 2. Open the file and write the contents.
 * 
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "sfStatus" have a value greater than or equal to SF_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfWriteMemToFile
(
    void	**buffer,		/* (in)  FITS memory buffer.	*/
    char	*filename,		/* (in)  Name of the disk file.	*/
    long	*size,			/* (mod) Size of the buffer.	*/
    SF_STATUS	*sfStatus		/* (mod) SF library staus.	*/
)
{
    int		fptr;			/* The file pointer.		*/
    
    fnEntry( "sfWriteMemToFile", *sfStatus );
    statCheck( *sfStatus, VOID );


    /*
     * Make sure the size is a positive value.
     */
    
    if ( *size < 0  )
    {
	*sfStatus = SF_E_FILE_SIZE;
	sfFormatMessage( SF_E_FILE_SIZE, size );
	fnReturn( VOID );
    }

    if ( ( fptr = open( filename, O_WRONLY | O_CREAT, 0664 ) ) > 2 )
    {
	if ( ( *size = write( fptr, *buffer, *size ) ) < 0 )
	{
	    *sfStatus = SF_E_FITS_FILE;
	    sfFormatMessage( *sfStatus, filename );
	}
	close( fptr );
    }
    else
    {
	*sfStatus = SF_E_FITS_FILE;
	sfFormatMessage( *sfStatus, filename );
    }
    
    fnReturn( VOID );
}
