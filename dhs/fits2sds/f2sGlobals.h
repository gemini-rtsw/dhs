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
 * fits2sdsApp/h/globals.h
 *
 * PURPOSE:
 * Contains the definitions of all the global definitions.
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

#ifndef F2S_GLOBALS_H
#define	F2S_GLOBALS_H

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
 *  Structure definitions.
 ************************************************************************
 */


/*
 * Values of the command line arguments.
 */
    
typedef struct globals
{
    boolean 	f2sArray;
    boolean 	f2sDd;
    boolean	f2sDdMake;
    boolean	f2sDisk;
    boolean	f2sMem;
    boolean	f2sSds;
    boolean	f2sVerbose;
    int  	f2sXC;
    int  	f2sYC;
} GLOBALS;


/*
 ************************************************************************
 *  External/global variables
 ************************************************************************
 */
    
GLOBALS globals;

    
    
/*
 ************************************************************************
 *  Needed for C++ applications
 ************************************************************************
 */
  
#ifdef    __cplusplus
}
#endif
   

#endif /* F2S_GLOBALS_H */
