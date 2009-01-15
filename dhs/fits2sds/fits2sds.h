/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997.				(c) 1997
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
 * fits2sdsApp/h/fits2sds.h
 *
 * PURPOSE:
 * Main header file for the fits2sds application.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:39  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1997/09/05 19:25:37  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


#ifndef FITS_2_SDS
#define	FITS_2_SDS

/*
 ************************************************************************
 *  For C++
 ************************************************************************
 */
 
 
#ifdef  __cplusplus
extern "C" {
#endif

#define F2S_VERSION 1.0    

    
/*
 ************************************************************************
 *  Enumerated types
 ************************************************************************
 */

    
/*
 *  Function return/status codes.
 */

typedef enum
{
    F2S_S_CHUNK_FILE	= 8,	/* Writing a chunk to a file.		*/
    F2S_S_COMPOSE_FILE	= 7,	/* Composing chunks into a file.	*/
    F2S_S_FULL_FILE	= 6,	/* Writing complete SDS structre to file*/
    F2S_S_HELP		= 5,	/* Display help message.		*/
    F2S_S_READ_FILE	= 4,	/* Reading a FITS file.			*/
    F2S_S_QUIT       	= 3,
    F2S_S_USAGE		= 2,
    F2S_S_VERSION    	= 1,	

    F2S_S_SUCCESS	= 0,

    F2S_E_BAD_CMD_ARGS	= -1,	/* Bad command argument(s) given.	*/
    F2S_E_DHS		= -2, 	/* Error in the DHS library.		*/
    F2S_E_MEMORY 	= -3,
    F2S_E_NON_INT	= -4,	/* Expected an integer value but didn't
				   get one. 				*/
    F2S_E_SF		= -5,	/* Error in the SF library.		*/
    F2S_E_USAGE		= -6
} F2S_STATUS;

    
/*
 ************************************************************************
 *  Global variables.
 ************************************************************************
 */
 
extern  char    f2sMsg[];

/************************************************************************
 * For C++
 ************************************************************************
 */
 
#ifdef    __cplusplus
}
#endif

#endif  /* FITS_2_SDS */
