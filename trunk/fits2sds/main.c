static char rcsid[] = "$Id: main.c,v 1.1.1.1 2002-11-24 20:32:55 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * ( c ) <year>				( c ) <year>
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
 * fits2sdsApp/src/main.c
 *
 * PURPOSE:
 * To create sds structures from FITS files.
 *
 * FUNCTION NAME( S )
 * 
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:38  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.7  1998/07/03 17:37:37  jaeger
 * Add conditional in main so fits2sds exits after the data dictionary
 * is created.
 *
 * Revision 1.6  1998/04/30 06:04:21  jaeger
 * Added proper version information/display.
 *
 * Revision 1.5  1998/01/29 21:25:46  jaeger
 * Added proper printing of version information.
 *
 * Revision 1.4  1998/01/13 19:19:39  jaeger
 * made sure "inputFile" was initialized to NULL and added sf error messages
 * that occur when calling sfFits2Sds.
 *
 * Revision 1.3  1997/12/02 21:26:48  jaeger
 * Removed unnecessary variables and added printing of sfMsg and
 * f2sMsg after the input FITS file has been read.
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


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_eptr.h"
#include "gen_str.h"
#include "gen_msg.h"


/*
 ************************************************************************
 *  sf library include files.
 ************************************************************************
 */

#include "sf.h"


/*
 ************************************************************************
 *  fits2sds application include files.
 ************************************************************************
 */

#include "fits2sds.h"
#include "f2sGlobals.h"
#include "f2sLocal.h"


/*
 ************************************************************************
 * Internal function prototypese
 ************************************************************************
 */

static 	void	f2sParsCmdLine( int, char*[], char**, F2S_STATUS );
static	void	f2sGlobalsInit( void );


/*
 ************************************************************************
 * External function prototypese
 ************************************************************************
 */

void	f2sFormatMessage();
void	f2sPrintMessage();


/*
 ************************************************************************
 * External variables.
 ************************************************************************
 */

extern	char 	*version_long;
extern	char	*version_short;


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sParseCmdLine
 *
 * INVOCATION:
 * f2sPaseCmdLine( argc, argv, filename, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)	argc	    (int)		Number of command-line args.
 * (>)	argv	    (char*[])	  	Comand-line arguments.
 * (<)	filename    (char**)	  	Input filename.
 * (!)	status	    (F2S_STATUS*)	Fits2sds application status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Parse the command line arguments
 *
 * DESCRIPTION:
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

void	f2sParseCmdLine
( 
    int         argc, 		/* (in)  Number of command line args.	*/
    char        *argv[],        /* (in)  The command line args.         */
    char        **inFile,       /* (out) The input file name.		*/
    F2S_STATUS	*status		/* (mod) Fits2sds application status.	*/
)
{
    int         i; 
    boolean     minusArray;    	/* Was the -array option used.		*/
    boolean     minusDd;       	/* Was the -dd option used.		*/
    boolean     minusDdMake;	/* Was the -ddMake option used.		*/
    boolean	minusDisk;	/* Write FITS disk files.		*/
    boolean     minusHelp;     	/* Was the -help option used.		*/
    boolean     minusI;        	/* Was the -i option used.		*/
    boolean	minusMem;	/* Write FITS memory buffers.		*/
    boolean	minusSds;	/* Write SDS memory buffers.		*/    
    boolean	minusv;	       	/* Was the verbose mode option used.    */
    boolean     minusV;        	/* Was the Version option used.		*/
    boolean     minusXc;       	/* Was the -xc option used.		*/
    boolean     minusYc;       	/* Was the -yc option used. 		*/

    statCheck( *status, VOID );
    

    /*
     * Initialize the variables.
     */
    
    minusArray = FALSE;
    minusDd 	= FALSE;
    minusDdMake = FALSE;
    minusDisk 	= FALSE;
    minusHelp 	= FALSE;
    minusI 	= FALSE;
    minusMem	= FALSE;
    minusSds	= FALSE;
    minusv 	= FALSE;
    minusV 	= FALSE;        
    minusXc 	= FALSE;        
    minusYc 	= FALSE;


    /*
     * evaluate each of the command line arguments.
     */
    
    for( i = 0; i < argc && *status == F2S_S_SUCCESS;  i++ )
    {
	if ( streq( argv[i], "-array" ) && !minusArray )
	{
	    minusArray = TRUE;
	    globals.f2sArray = TRUE;
	}
	else if ( streq( argv[i], "-dd" ) && !minusDd )
	{
	    minusDd = TRUE;
	    globals.f2sDd = TRUE;	    
	}
	else if ( streq( argv[i], "-ddMake" ) && !minusDdMake )
	{
	    minusDdMake = TRUE;
	    globals.f2sDdMake = TRUE;
	}
	else if ( streq( argv[i], "-disk" ) && !minusDisk )
	{
	    minusDisk = TRUE;
	    globals.f2sDisk = TRUE;
	}
	else if ( streq( argv[i], "-help" ) && !minusHelp )
	{
	    minusHelp= TRUE;
	}
	else if ( streq( argv[i], "-i" ) && !minusI )
	{
	    nullCheck( *inFile = strsav( argv[++i] ), *status, VOID );
	    minusI = TRUE;
	}
	else if ( streq( argv[i], "-mem" ) && !minusMem )
	{
	    minusMem = TRUE;
	    globals.f2sMem = TRUE;
	}
	else if ( streq( argv[i], "-sds" ) && !minusSds )
	{
	    minusSds = TRUE;
	    globals.f2sSds = TRUE;
	}
	else if	( streq( argv[i], "-v" ) && !minusv )
	{
	    minusv = TRUE;
	    globals.f2sVerbose = TRUE;
	}
	else if ( streq( argv[i], "-V" ) && !minusV )
	{
	    minusV = TRUE;
	}
	else if ( streq( argv[i], "-xc" ) && !minusXc )
	{
	    if ( str2int( argv[++i], &( globals.f2sXC ) ) )
	    {
		minusXc = TRUE;
	    }
	    else
	    {
		*status = F2S_E_NON_INT;
	    }
	}
	else if ( streq( argv[i], "-yc" ) && !minusYc )
	{
	    if ( str2int( argv[++i], &( globals.f2sYC ) ) )
	    {
		minusYc = TRUE;
	    }
	    else
	    {
		*status = F2S_E_NON_INT;
	    }
	}
	else
	{
	    *status = F2S_E_BAD_CMD_ARGS;
	}
    }

    
	
    /*
     * If the -help option was used, print the usage message
     * and exit.
     */
    
    if ( minusHelp )
    {
	*status = F2S_S_HELP;
    }
    
    
    /*
     * If the -V option was used, print the long version
     * message.
     */
    
    if ( minusV )
    {
	*status = F2S_S_VERSION;
    }
    
    
    /*
     * Check for the required set of options and make sure that
     * only of on -dd and -ddMake have been used.
     */
    
    if ( ( !minusI && !minusHelp && !minusV ) ||
	    ( minusDd && minusDdMake ) )
    {
	*status = F2S_E_USAGE;
    }
}


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * f2sGlobalsInit
 *
 * INVOCATION:
 * f2sGlobalsInit()
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * None.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To initialize the global variables.
 *
 * DESCRIPTION:
 * Trivial.
 * 
 * EXTERNAL VARIABLES:
 * None.
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

void	f2sGlobalsInit
( 
    void
)
{
    globals.f2sArray = FALSE;
    globals.f2sDd = FALSE;
    globals.f2sDdMake = FALSE;
    globals.f2sDisk = FALSE;
    globals.f2sMem = FALSE;
    globals.f2sSds = FALSE;
    globals.f2sVerbose = FALSE;
    globals.f2sYC = 1;
    globals.f2sXC = 1;
}



/*
************************************************************************
 *+
 * FUNCTION NAME:
 * main
 *
 * INVOCATION:
 * called automatically when sftest is exectued.
 *
 * FUNCTION 
 * None.
 *
 * PURPOSE:
 * To test the sf library.
 *
 * DESCRIPTION:
 * 1. Parse the command-line arguments.
 * 2. Determine what fits file is to be read-in, where the output
 *    should be saved and what options to execute.
 * 3. use sfFits2Sds to create and SDS file
 * 4. use the SDS file in 3 with sfSds2Fits to create a fits file.
 * 5. savve the fits file created in 4.
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

int	main
( 
    int         argc,        /* ( in ) The number of arguments.        */
    char        *argv[]      /* ( in ) The list of arguments.          */
)
{

    /*
     * Local variables
     */

    DHS_STATUS      dhsStatus;
    SF_DS_INFO	    *dsInfo;
    float	    fitsVersion;
    char	    fitsStrVer[10];
    char	    *inFile;
    int		    last;
    EPTR	    *objectList;
    SF_OBJECT	    *sfObject;
    SF_STATUS       sfStatus;
    F2S_STATUS	    status;
        

    /*
     * Initialize variables.
     */
    
    sfStatus = SF_S_SUCCESS;
    dhsStatus = DHS_S_SUCCESS;
    status = F2S_S_SUCCESS;
    objectList = NULL;
    dsInfo = NULL;
    inFile = NULL;		
    

    objectList = eptr_alloc( 10 );
    if ( objectList == NULL )
    {
	exitApp( status, NULL, NULL, F2S_E_MEMORY, NULL );
    }


    /*
     * Initialize globals, and the various libraries used.
     */
    
    f2sGlobalsInit();
    
    f2sParseCmdLine( --argc, ++argv, &inFile, &status );
    if ( status != F2S_S_SUCCESS )
    {
	if ( status == F2S_VERSION )
	{
	    ffvers( &fitsVersion );
	    sprintf( fitsStrVer, "%.2f", fitsVersion );
	    f2sFormatMessage( status, version_short, version_long,
		    fitsStrVer );
	    f2sPrintMessage();
	    return status;
	}
	else
	{
	    exitApp( status, NULL, NULL, status, NULL );
	}
    }

    dhsInit( "sf", 0, &dhsStatus );
    if ( dhsStatus != DHS_S_SUCCESS )
    {
	exitApp( status, NULL, NULL, F2S_E_DHS, NULL );
    }

    sfInit( &sfStatus );
    if( sfStatus != SF_S_SUCCESS )
    {
	exitApp( status, NULL, &dhsStatus, F2S_E_SF, sfMsg );
    }
    
    if ( globals.f2sVerbose )
    {
	f2sFormatMessage( F2S_S_READ_FILE, inFile );
	f2sPrintMessage();
    }
    

    dsInfo = sfDsInfoInit( strtail( inFile ), &sfStatus );
    if ( sfStatus != SF_S_SUCCESS )
    {
	exitApp( status, &sfStatus, &dhsStatus, F2S_S_SUCCESS, NULL );
    }
   
    
    /*
     * Read the file into 1 or more SDS structures.
     */
    
    sfFits2Sds( inFile, globals.f2sXC, globals.f2sYC, globals.f2sDd,
	    globals.f2sDdMake, globals.f2sArray, objectList, &sfStatus );
    if ( sfStatus != SF_S_SUCCESS )
    {
	msg_append( f2sMsg, sfMsg );
	msg_clear( sfMsg );
	exitApp( status, &sfStatus, &dhsStatus, F2S_E_SF, NULL );
    }

    if ( globals.f2sVerbose )
    {
	printSfMsg();
    }
    else
    {
	msg_clear( sfMsg );
    }


    if ( globals.f2sDdMake )
    {
	/*
	 * Creating Data Dictionary so don't do any data processing.
	 */

	exitApp( status, &sfStatus, &dhsStatus, F2S_S_SUCCESS, NULL );
    }
    
	 
    
    /*
     * write all of the SDS chunks. The last chunk in the list is
     * the whole FITS file.  So this is not to be treated as a
     * chunk.
     */
     
    f2sWriteChunks( inFile, objectList, dsInfo, &status );
    if ( status != F2S_S_SUCCESS )
    {
	exitApp( status, &sfStatus, &dhsStatus, status, NULL );
    }	

    /*
     * write the small SDS structure into one FITS file.
     * First get the filename and open it for writing.
     */

    f2sComposeChunks( inFile, objectList, dsInfo, &status );
    if ( status != F2S_S_SUCCESS )
    {
	exitApp( status, &sfStatus, &dhsStatus, status, NULL );
    }	


    /*
     * Write the SDS structure containing all of the original FITS file
     * information.
     */

    last = eptrlen( objectList );
    if ( last - 1 > 0 )
    {
	sfObject = (SF_OBJECT*)eptritem( objectList, last - 1 );    
	f2sWriteFull( inFile, sfObject, dsInfo, &status );
	if ( status != F2S_S_SUCCESS )
	{	
	    exitApp( status, &sfStatus, &dhsStatus, status, NULL );
	}	
    }

    if ( globals.f2sVerbose )
    {
	(void) printf ( "Done \n" );
    }

    if( inFile != NULL )
    {
	gen_free( inFile );
	inFile = NULL;
    }
    
    exitApp( status, &sfStatus, &dhsStatus, F2S_S_SUCCESS, NULL );
}
