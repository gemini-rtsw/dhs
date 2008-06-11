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
 * dd/h/dd.h
 *
 * PURPOSE:
 *	The dd package instantiates the keyword/attribute dictionary and
 * 	provides access function to get out keyword and attribute infor-
 *	mation.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.12  1997/12/02 20:06:20  jaeger
 * Added new warning to indicate when a string is truncated.
 *
 * Revision 1.11  1997/10/29 22:12:13  jaeger
 * removed ddCleanup - it was obsolete
 *
 * Revision 1.10  1997/08/22 17:51:21  jaeger
 * Allowable array types changed to "EXTEN", "INDEX" and "NONE"
 * Keywords are converted to uppercase, warning if they are not
 *
 * Revision 1.9  1997/07/29 20:04:39  dunn
 * Lots of changes.
 *
 * Revision 1.8  1997/07/17 21:52:50  dunn
 * Fixed order of dd_status's and added .
 *
 * Revision 1.7  1997/07/15 18:29:29  jaeger
 * checked in for compile with dhs
 *
 * Revision 1.6  1997/07/11 22:38:43  jaeger
 * Added "ANY" to datatype
 *
 * Revision 1.5  1997/07/09 16:36:19  jaeger
 * Converted library to use dhs instead of bdt
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef _dd_h
#define _dd_h

#include "dhs.h"

#ifdef  __cplusplus
extern "C" {
#endif

#include "fitsio.h"


/*
 ************************************************************************
 *  Constant definitions.
 ************************************************************************
 */

/* lengths for charatcter strings in row data type, from fitsio.h */
    
#define DD_CMTS_LEN		FLEN_COMMENT
#define DD_INSTR_LEN		FLEN_VALUE
#define DD_KEYWORD_LEN		FLEN_KEYWORD
#define DD_VALUE_LEN            FLEN_VALUE

    
#define DD_MAX_TOKENS           (10)

#define DD_ANY_TYPE             (-1)


/*
 ************************************************************************
 *  enumerated types
 ************************************************************************
 */


/*
 * return codes
 */

typedef enum
{
    DD_S_LOWER			= 11, 	/* Keyword has lower case value.*/
    DD_S_NOT_FOUND	 	= 10,	/* Item not found in data dict.	*/
    DD_S_STRING_SIZE		= 9,	/* String trunctated.		*/

    DD_S_SUCCESS		= 0,	/* Normal Completion.		*/

    DD_E_ARRAY   		= -1,	/* Bad array value given.	*/
    DD_E_CFILE_OPEN 	 	= -5,	
    DD_E_CONFIG 		= -10,
    DD_E_FORMAT 		= -12,
    DD_E_FORMAT_NUM 		= -13,
    DD_E_MEMORY 		= -14,	/* Ran out of memory.		*/
    DD_E_NOT_FOUND 		= -15,	/* Item not found in data dict.	*/
    DD_E_TYPE   		= -16,	
    DD_E_UK_KEY   		= -20	/* Data dict. has two rows similar.*/
} DD_STATUS;    


/*
 * Possible values for "Array", "Fits Mandatory", and "Type" fields
 */


typedef enum
{
    DD_EXTENSION  		= 'E',
    DD_INDEX         		= 'I',
    DD_NO_ARRAY     		= 'N'
} DD_ARRAY_TYPE;



 /*
 ************************************************************************
 *  Structure types
 ************************************************************************
 */

  
typedef struct dd_row
{
    char        	ddrInstr[DD_INSTR_LEN];
    char         	*ddrAttrib;
    char           	ddrKeyword[DD_KEYWORD_LEN];
    DHS_DATA_TYPE       ddrType;
    int    	    	ddrGemMand;
    char                ddrNullVal[DD_VALUE_LEN];
    char                ddrArray;
    char                *ddrColName;
    char		ddrCmts[DD_CMTS_LEN];
} DD_ROW;


/*
 ************************************************************************
 *  External functions
 ************************************************************************
 */

extern  void    ddExit( DD_STATUS* );
extern	void 	ddInit( DD_STATUS* );
extern 	void	ddKeywordGet( char*, char*, DD_ROW**, DD_STATUS* );
extern 	void	ddAttribGet( char*, char*, DD_ROW**, DD_STATUS* );
extern 	DD_ROW	*ddGetNext( int, DD_STATUS * ); 
extern char 	ddMsg[];

  
#ifdef    __cplusplus
}
#endif

#endif /* _dd_h */
