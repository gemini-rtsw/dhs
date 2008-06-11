static char rcsid[] = "$Id: sf_msg.c,v 1.1.1.1 2002-11-24 20:35:48 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997				(c) 1997
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6		Ottawa, Canada, K1A 0R6
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
 *					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * sfLibrary/src/msg.c
 *
 * PURPOSE:
 *	Message handling routines for the sf program.
 *
 * FUNCTION NAME(S)
 *	void	sfFormatMessage	: Formats a message.
 *	void	sfPrintMessage	: Prints the message.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.19  1999/02/12 18:10:29  jaeger
 * Added messages for handling data sizes which are larger than the
 * "axisSz" values given.
 *
 * Revision 1.18  1999/02/09 20:25:17  jaeger
 * Changed SF_S_NOT_FOUND message, no longer claims is is "searching".
 *
 * Revision 1.17  1999/02/08 22:09:49  jaeger
 * Added keyword name and instrument name to SF_S_NOT_FOUND.  Changed
 * SF_E_TYPE to SF_S_TYPE.
 *
 * Revision 1.16  1999/01/26 06:48:22  jaeger
 * Added messages for handling invalid origin values and axis sizes.
 *
 * Revision 1.15  1998/05/25 21:43:43  jaeger
 * Added more informative message for dhsBdAttribInfo errors.
 *
 * Revision 1.14  1998/05/14 18:48:38  jaeger
 * Added FRAME_NOT_FOUND and ATTRIB_NOT_FOUND error messages.
 *
 * Revision 1.13  1998/04/30 06:08:29  jaeger
 * Added a number of messages for EXTNAME, EXTVER, FRMID, and FRMNAME
 * evaluation.
 *
 * Revision 1.12  1997/12/02 21:03:49  jaeger
 * Add some new errors/error messages.
 *
 * Revision 1.11  1997/11/19 00:12:18  jaeger
 * Added sorting of "extension" list in the dataset information, removed
 * sfPrintMessage, and fixed bug in fits2sds (big dataset did not have
 * the proper frame structure).
 *
 * Revision 1.10  1997/09/05 18:58:07  jaeger
 * Fixed some memory leaks
 *
 * Revision 1.9  1997/08/22 22:10:35  jaeger
 * Added support for multi-dimensional arrays.
 *
 * Revision 1.8  1997/08/19 19:03:45  jaeger
 * Checked in for install
 *
 * Revision 1.7  1997/08/15 20:43:37  jaeger
 * Checked in for install
 *
 * Revision 1.6  1997/08/12 21:39:00  jaeger
 * Fixed up error messaging
 *
 * Revision 1.5  1997/08/12 21:07:38  jaeger
 * Checked in for install
 *
 * Revision 1.4  1997/08/06 18:30:48  jaeger
 *  Checked in for install and testing with dhs
 *
 * Revision 1.3	 1997/07/09 16:31:20  jaeger
 * Converted library from bdt to dhs
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

#include <stdarg.h>


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_eptr.h"


/*
 ************************************************************************
 *  sf library specific include files.
 ************************************************************************
 */

#include "sf.h"
#include "localSf.h"


/*
 ************************************************************************
 *  Constant definitions.
 ************************************************************************
 */

#define	SF_PREFIX	"sf"


/*
 ************************************************************************
 *  data types 
 ************************************************************************
 */

/*
 * Create a place to store the sf and fits messages.
 */

char sfMsg[MSG_MAX_LEN];
char fitsMsg[MSG_MAX_LEN];

/*
 * Set the error messages.
 */

static MSG	sfMsgs[] = {
    { SF_S_EXTNAME,
	"Warning: Frame has two different EXTNAME identifiers." },
    { SF_S_EXTVER,
	"Warning: Two different EXTVER valuess given for a frame." },
    { SF_S_FN_EXIT,
	"Warning: SF status when exiting %s was: %d" },
    { SF_S_FRMNAME,
	"Warning: `%s' and `%s' frame name's were both given for a frame %s." },
    { SF_S_INST,
	"Warning: Two different instruments given for a dataset." },
    { SF_S_INST_ARRAY,
	"Warning: Instrument should have array type, 'N', not %c." },
    { SF_S_INST_NOT_FOUND,
	"Warning: Instrument not found." },
    { SF_S_NOT_FOUND,
	"Warning: Keyword `%s' NOT found in DD with instrument %s" },
    { SF_S_TYPE,
	"Warning: Data type given for `%s' doesn't match with the data"
        "dictionary entry." },

    { SF_S_SUCCESS,
	"Successful completion of SF library." },

    { SF_E_ARRAY_TYPE,
	"Erros: Bad array designation, %c, from data dictionary." },
    { SF_E_ATTRIB_DIMS,
	"Error: Wrong number of dimensions, %d, for attribute `%s'." },
    { SF_E_ATTRIB_INFO,
	"Error: Failure in DHS routine:\n\t"
	"dhsBdAttribInfo( dhsObject, `%s', &dhsStatus)\n\t"
	"DHS error number is: %d" },
    { SF_E_ATTRIB_NOT_FOUND,
	"Error: Failure in DHS routine:\n\t"
	"dhsBdAttribFind( dhsObject, `%s', &dhsStatus)\n\t"
        "DHS error number is: %d" },
    { SF_E_AXISSZ_2D,
        "Error: Length of data axis %d is %d which is greated than the"
        "expected length %d" },    
    { SF_E_AXISSZ_2D,
        "Error: Size of data (%d, %d) is greater than the expectedn\n"
        "size (%d,%d)" },
    { SF_E_AXISSZ_3D,
        "Error: Size of data (%d, %d, %d) is greater than the expected\n"
        "size (%d,%d, %d)" },
    { SF_E_AXISSZ_DIMS,
	"Error: Attribute `axisSize' should be an array of `%d' value but `%d'"
	" values were found." },
    { SF_E_BITPIX,
	"Error: Bad 'bitpix' value: %d" },
    { SF_E_BITPIX_VAL,
	"Error: Two different bitpix values, %d and %d, given for %s." },
    { SF_E_CHUNK,
	"Error: Number of data chunks must be greater than 0.\n"
	"The number of data chunks given was: %d" },
    { SF_E_CORNER,
	"Error: No blc or trc value." },
    { SF_E_CONV_TYPE,
	"Error: An error has occured converting between FITS and DHS data"
        "types.\nThe error type is: %d" },
    { SF_E_DD,
        "Error: Failure in DD routine: \n  %s\n"
	"Error number is: %d" },
    { SF_E_DD_FILE,
	"Error %s file: '%s'." },
    { SF_E_DHS,
	"Error: Failure in DHS routine:\n\t%s\n\tError number is: %d" },
    { SF_E_DHS_KEYS,
	"Error: Bad DHS keywords found for frame `%s'." },
    { SF_E_DHS_TYPE,
	"Error: %d is not a valud DHS type." },
    { SF_E_DIMS,
	"Error: Bad dimension given for an indexed array or data array.\n"
	"The number of dimension given was: %d" },
    { SF_E_DS_INFO,
	"Erro: Bad dataset information." },	
    { SF_E_EXT_NOT_FOUND,
	"Error: Unable to compose observation, no information for frame %s." },
    { SF_E_FILE_EXT,
	"Error: Invalid file extension: '%s'. \n"
	"Must be one of 'sds', 'fits' or 'fit'." },
    { SF_E_FILE_SIZE,
	"Error: File size must be greater than 0.\n"
	"The value given was: %d" },
    { SF_E_FILE_TYPE,
	"Error: FITS files can be opened with 'NEW', 'READWRITE' or"
        "'READONLY'\nThe value given was: %s" },
    { SF_E_FITS,
	"Error: cfitsio reports error %d occured when executing\n\t%s" },
    { SF_E_FITS_FILE,
	"Error: Unable to create FITS file '%s'." },
    { SF_E_FITS_TYPE,
	"Error: Bad FITS type encountered.\n"
	"The value of the FITS type is: %d" },
    { SF_E_FPTR,
	"Error: FITS file pointer is NULL." },
    { SF_E_FRAMEID,
	"Error: Bad frame identifier '%s'." },
    {  SF_E_FRAME_NOT_FOUND,
         "Unable to locate frame description for frame %s." },
    { SF_E_INIT,
	"Error: Unable to initialize SF library." },
    { SF_E_MEMORY,
	"Error: Ran out of memory." },
    { SF_E_NAXIS,
	"Error: Two different naxis values, %d and %d, given for frame %s." },
    { SF_E_NAXES,
	"Error: Two different axis sizes, %d and %d, given for axis %d\n"
        "for frame %s." },
    { SF_E_NOT_DATASET,
	"Error: Sds2Fits has received something other than a dataset." },
    { SF_E_NULL_EXT,
	"Error: Extension pointer is NULL." },
    { SF_E_ORIGIN,
	"Error: Bad origin value for axis %d.  A value of %d was found and\n"
        "it should be between 1 and %d" },
    { SF_E_ORIGIN_2D,
	"Error: Origin must be between 1,1 and %d, %d the values given\n"
         "were %d, %d." },
    { SF_E_ORIGIN_3D,
	"Error: Origin must be between 1,1,1 and %d, %d, %d the values\n"
        "given were %d, %d, %d." },
    { SF_E_ORIGIN_DIMS,
	"Error: Attribute `origin' should be an array of `%d' value but `%d'"
	" values were found." },
    { SF_E_PRIME_DATA,
	"Error: Data exists for the prime header unit." },
    { SF_E_SDS_FILE,
	"Error: %s file: '%s'." },
    { SF_E_STR_CONV,
        "Error: Unable to convert string %s data type." }
};


/*
 * Determine the number of messages that can be stored.
 */

static int	sfNumMsgs = sizeof( sfMsgs ) / sizeof( MSG );



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFormatMessage
 *
 * INVOCATION:
 * sfFormatMessage( va_alist);
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	status	(int)		The message being formatted.	
 * (>)	 ...	(unknown)	The message arguments.		
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Formats a message from the contents of the va_alist.
 *
 * DESCRIPTION:
 * For each argument ( message ) format it and add it to the buffer.
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
 * None.
 *-
 ************************************************************************
 */

void	sfFormatMessage
(
    int		*status,/* (in)	 The message being formatted.	*/
    ...			/* (in)	 The message arguments.		*/
) 

{
    va_list	args;  

    va_start( args, status );
    msg_format( sfMsg, SF_PREFIX, sfNumMsgs, sfMsgs, status, args );
    va_end( args );
}


