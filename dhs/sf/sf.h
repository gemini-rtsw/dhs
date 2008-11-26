/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997.			        (c) 1997
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 	    	Ottawa, Canada, K1A 0R6
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
 * 					                sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * sfLibrary/h/sf.h
 *
 * PURPOSE:
 * This is the main header file for the SF library.  
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.21  1999/02/12 18:11:20  jaeger
 * Added messages for handling data sizes which are larger than the
 * "axisSz" values given.
 *
 * Revision 1.20  1999/02/09 20:40:02  jaeger
 * Changed S_E_NOT_FOUND to SF_S_NOT_FOUND and SF_E_TYPE to SF_S_TYPE.
 *
 * Revision 1.19  1999/01/26 18:47:45  jaeger
 * Added some new messages for better error reporting.
 *
 * Revision 1.18  1998/05/25 21:43:14  jaeger
 * Added more informative message for dhsBdAttribInfo errors.
 *
 * Revision 1.17  1998/05/14 18:53:19  jaeger
 * Added two error messages SF_E_ATTRIB_NAME and SF_E_FRAME_NOT_FOUND.
 *
 * Revision 1.16  1998/04/30 06:13:05  jaeger
 * Added some more error messages.  Added extension version and name
 * to the extension and dataset information structures.
 *
 * Revision 1.15  1997/12/02 21:10:40  jaeger
 * Added some new errors.
 *
 * Revision 1.14  1997/11/21 00:22:28  jaeger
 * Fixed problem with data being separated from headers during composition.
 *
 * Revision 1.13  1997/11/19 00:10:15  jaeger
 * Added sorting of "extension" list in the dataset information, removed
 * sfPrintMessage, and fixed bug in fits2sds (big dataset did not have
 * the proper frame structure).
 *
 * Revision 1.12  1997/09/05 23:05:12  jaeger
 * fixed remaining memory leaks.
 *
 * Revision 1.11  1997/09/05 18:59:07  jaeger
 * Add some new error codes.
 *
 * Revision 1.10  1997/08/22 22:10:35  jaeger
 * Added support for multi-dimensional arrays.
 *
 * Revision 1.9  1997/08/19 19:03:45  jaeger
 * Checked in for install
 *
 * Revision 1.8  1997/08/15 20:43:37  jaeger
 * Checked in for install
 *
 * Revision 1.7  1997/08/12 21:39:00  jaeger
 * Fixed up error messaging
 *
 * Revision 1.6  1997/08/12 21:08:03  jaeger
 * Checked in for install
 *
 * Revision 1.5  1997/08/06 18:30:48  jaeger
 *  Checked in for install and testing with dhs
 *
 * Revision 1.4  1997/07/09 16:34:00  jaeger
 *  Converted library from bdt to dhs
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef _sf_h
#define _sf_h


/*
 ************************************************************************
 *  cfitsio library specific include files.
 ************************************************************************
 */

#include "fitsio.h"

/*
 ************************************************************************
 *  dhs library specific include files.
 ************************************************************************
 */
    #include "dd.h"
    #include "dhs.h"

/*
 ************************************************************************
 *  For C++
 ************************************************************************
 */


#ifdef  __cplusplus
extern "C" {
#endif


/*
 ************************************************************************
 *  Constant definitions.
 *  ??? Make sure these are all needed at a global level!!!
 ************************************************************************
 */

#define SF_MAXDIM 		7   /* Maximum number of dimensions.	*/

#define SF_NULL_SIZE		(-1)/* Null FITS file size.		*/
    
#define NULL_INDEX		0   /* Null index into a FITS file.	*/

#define NAME_SIZE		30  /* Maximum allowed size of dataset
				       name and extension identifiers,
				       should be the same as
				       SDS_NAME_SIZE in dhs library.	*/

/*
 ************************************************************************
 *  Enumerated types
 ************************************************************************
 */

    
/*
 * Various types of FITS extension handled in the SF library.
 */
    
typedef enum
{
    SF_BINARY		= 'B',
    SF_IMAGE		= 'I'
} SF_XTENSION;
    

    
 /*
  *  Function return/status codes.
  */

typedef enum
{
    SF_S_EXTNAME	=  10,	/* Frame has two different id's.	*/
    SF_S_EXTVER		=  9,	/* Frame has two different id's.	*/
    SF_S_FN_EXIT	=  8, 	/* Exiting function.                    */
    SF_S_FRMNAME	=  7,	/* Two different frame names.		*/
    SF_S_INST		=  5,	/* Two different instruments.		*/
    SF_S_INST_ARRAY	=  4,	/* Bad array type for inst. attribute.	*/
    SF_S_INST_NOT_FOUND	=  3,	/* Instrument not found.		*/
    SF_S_NOT_FOUND  	=  2,  	/* Keywords not found in DD		*/
				/* searching with instrument "*"	*/
    SF_S_TYPE		=  1,	/* Bad type encountered in SF.		*/

    SF_S_SUCCESS    	=  0,

    SF_E_ARRAY_TYPE	= -1,  	/* Bad array type from DD.		*/
    SF_E_ATTRIB_DIMS	= -2,	/* Bad dimensions found for an attribute*/
    SF_E_ATTRIB_INFO	= -3,	/* Bad dimensions found for an attribute*/
    SF_E_ATTRIB_NOT_FOUND = -4,	/* Bad dimensions found for an attribute*/
    SF_E_AXISSZ		= -5,	/* Data size bigger than axissz.	*/
    SF_E_AXISSZ_2D	= -6,	/* 2D Data size bigger than axissz.	*/
    SF_E_AXISSZ_3D	= -7,	/* 3D Data size bigger than axissz.	*/
    SF_E_AXISSZ_DIMS	= -8,	/* Bad dimensions found for axisSize	*/
    SF_E_BITPIX		= -9, 	/* Bad Image type from FITS.		*/
    SF_E_BITPIX_VAL	= -10,  /* Two bitpix values given for a frame. */
    SF_E_CHUNK		= -11,	/* Error in chunking data.		*/
    SF_E_CORNER		= -12, 	/* No blc or trc value.			*/
    SF_E_CONV_TYPE	= -13, 	/* Error converting between FITS and	*/
				/* DHS data types.			*/
    SF_E_DD		= -14,	/* Error in the dd library.		*/
    SF_E_DD_FILE	= -15,	/* Error creating a data dictionary.	*/
    SF_E_DHS		= -16,	/* Error in the dsh library.		*/
    SF_E_DHS_KEYS	= -17,	/* Bad DHS keyword(s) found.		*/
    SF_E_DHS_TYPE	= -18,	/* Bad dhs type encountered.		*/
    SF_E_DIMS		= -19,	/* Bad data dimensions given.		*/
    SF_E_DS_INFO	= -20,	/* Bad dataset information.		*/
    SF_E_EXT_NOT_FOUND	= -21,	/* Error finding item eptr list.	*/
    SF_E_FILE_EXT	= -22,	/* Bad file extenstion given.		*/
    SF_E_FILE_SIZE	= -23,  /* Bad FITS file size given.		*/
    SF_E_FILE_TYPE	= -24,	/* Bad file open status given.		*/
    SF_E_FITS		= -25,	/* Error in the fits library.		*/
    SF_E_FITS_FILE	= -26,	/* Bad FITS type encountered.		*/
    SF_E_FITS_TYPE	= -27,	/* Bad FITS type encountered.		*/
    SF_E_FPTR		= -28,	/* FITS file pointer is NULL.		*/
    SF_E_FRAMEID	= -29,  /* Bad or missing frame identifier.	*/
    SF_E_FRAME_NOT_FOUND= -30,  /* Unable to locate frame in frameList. */
    SF_E_INIT		= -31,	/* SF library not intialized.		*/
    SF_E_MEMORY		= -32,	/* Error allocating memory.		*/
    SF_E_NAXIS		= -33,	/* Two different naxis values given.	*/
    SF_E_NAXES		= -34,  /* Two different naxes values given.	*/
    SF_E_NOT_DATASET	= -35,	/* Not a DHS dataset object.		*/
    SF_E_NULL_EXT	= -36,	/* Extension pointer is NULL.		*/
    SF_E_ORIGIN		= -37,	/* Bad origin value given.		*/
    SF_E_ORIGIN_2D	= -38,	/* Bad origin value given on a 2D image.*/
    SF_E_ORIGIN_3D	= -39,	/* Bad origin value given on a 3D image.*/
    SF_E_ORIGIN_DIMS	= -40,	/* Bad dimensions found for origin.	*/
    SF_E_PRIME_DATA	= -41,	/* Data following prime HDU.		*/
    SF_E_SDS_FILE	= -42,	/* Error with SDS file.			*/
    SF_E_STR_CONV	= -43 	/* Converting from a string to another	*/
			        /*  data type. 				*/
} SF_STATUS;



/*
 ************************************************************************
 *  Structure types
 ************************************************************************
 */

	
/*
 * Structure to hold information about a particular FITS extension.
 * A FITS extension is created for each unique frame. 
 */ 

typedef struct sf_ext_info
{
    char    	    extName[FLEN_VALUE];/* Value of "EXTNAME" keyword.	*/
    long	    extVer;
    boolean	    extWritten;
    char	    frmName[FLEN_VALUE];/* Value of "FRMNAME" keyword.	*/
    char	    frmId[FLEN_VALUE];
    int     	    numKeywords;    /* Number of keywords in ext. header*/
    int     	    fitsIndex; 	    /* Index into the FITS file.	*/
    SF_XTENSION     type;	    /* Extensions type, IMAGE, BINARY,  */
				    /* or ASCII.			*/
    int		    bitpix;         /* Number of bits per pixel.      	*/
    int		    naxis;	    /* Number of axies.			*/
    long	    naxes[SF_MAXDIM];/* Size of each axis.		*/
} SF_EXT_INFO;


/*
 * Structure to hold information for a given dataset.  Note that the
 * two eptr lists (ext and extVers) point to the same items in memory
 * but have a different ordering.
 */
    
typedef struct sf_ds_info
{
    char    name[NAME_SIZE];	/* Name of the dataset.			*/
    int     numKeywords;	/* Number of keywords in Primary 	*/
				/* FITS Header unit.			*/
    char    instrument[FLEN_VALUE];
    				/* The Instrument for the dataset. 	*/
    int     last;		/* Index to last HDU in a FITS file.	*/
    EPTR    *exts;		/* List of extension information.	*/
    EPTR    *extVers;		/* List of extension versions. 		*/
} SF_DS_INFO;


/*
 * Useful struture for test data.  Contains a DHS dataset id and a
 * unique string that identifies it.  The unique string could be a
 * filename.
 */

typedef struct
{
    DHS_BD_OBJECT   dhsObject;
    char	    *id;
} SF_OBJECT;


/*
************************************************************************
*  Global function declarations
************************************************************************
*/


extern	void	    sfCheckWithDd( char[FLEN_KEYWORD], DHS_DATA_TYPE, int,
			char*, DD_ARRAY_TYPE*, char[], char[], boolean*,
			SF_STATUS* );
extern	void	    sfCloseFitsFile( fitsfile*, SF_STATUS* );
extern  void        sfDsInfoFree( SF_DS_INFO* );
extern  SF_DS_INFO  *sfDsInfoInit( const char*, SF_STATUS* );
extern  void 	    sfExit( SF_STATUS* );
extern	char	    *sfExtractKeyword( char[FLEN_CARD], SF_STATUS* );
extern	long	    sfFitsFileSize( fitsfile*, SF_STATUS* );
extern	void	    sfFitsFlush( fitsfile*, SF_STATUS* );
extern  void	    sfFits2Sds( char*, int , int, boolean, boolean, boolean,
			EPTR*, SF_STATUS* );
extern	void	    sfGetValue( DHS_BD_OBJECT, boolean, char*, DHS_DATA_TYPE*,
			int*, unsigned long*, void**, SF_STATUS* );
extern 	char	    *sfGetFilename( const char*, const char*, const char*,
			SF_STATUS* );
extern  void        sfInit( SF_STATUS* );
extern	void	    sfObjectFree( SF_OBJECT* );
extern	fitsfile    *sfOpenFitsFile( char*, char*, SF_STATUS* );
extern  void        sfSds2Fits( DHS_BD_OBJECT, fitsfile*, SF_DS_INFO*, boolean,
			SF_STATUS* );
extern	fitsfile    *sfSetFitsMem( void**, long*, SF_DS_INFO*,	SF_STATUS* );
extern	void	    sfWriteDhsObject( DHS_BD_OBJECT, const char*, const char*,
			const char*, boolean, SF_DS_INFO*, SF_STATUS* );
extern	void	    sfWriteMemToFile( void**, char*, long*, SF_STATUS* );


/*
 ************************************************************************
 *  Global variables.
 ************************************************************************
 */

extern  char    sfMsg[];


/*
 ************************************************************************
 *  For C++
 ************************************************************************
 */

#ifdef    __cplusplus
}
#endif


#endif /* _sf_h */



