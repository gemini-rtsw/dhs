static char rcsid[] = "$Id: dd.c,v 1.1.1.1 2002-11-24 20:24:12 brighton Exp $";
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
 * dd/src/dd.c
 *
 * PURPOSE:
 *	The dd package instantiates the keyword/attribute dictionary 
 *	( a.k.a. data dictionary, dd ) and provides access function to 
 *	get out keyword and attribute information.
 *
 * FUNCTION NAME(S)
 *      int 	ddAttribGet	: Function to get attribute out of the dd.
 *	int	ddEptrFree	: Function to free the eptr list.
 *	int	ddExit		: Function to free the dd.
 *	int	ddInit		: Function to initialize the dd.
 *      int 	ddGetNext	: 
 *      int 	ddKeywordGet	: Function to get keyword out of dd.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.15  1997/12/02 20:00:57  jaeger
 * Fixed string overruns with strcpy.
 *
 * Revision 1.14  1997/09/05 23:54:02  jaeger
 * removed remaining memory leaks.
 *
 * Revision 1.13  1997/09/05 18:45:43  jaeger
 * Fixed some memory leaks
 *
 * Revision 1.12  1997/08/29 21:59:45  dunn
 * Freed rowKey leak.
 *
 * Revision 1.11  1997/08/15 20:39:51  jaeger
 * Changed searching method to ddCompAttrib in ddKeywordGet
 *
 * Revision 1.10  1997/08/12 19:17:06  jaeger
 * Fixed bug - Used strsav instead of strcpy with ddrAttrib
 *
 * Revision 1.9  1997/07/29 20:04:07  dunn
 * Lots of changes.
 *
 * Revision 1.8  1997/07/17 23:02:37  dunn
 * Fixed format msg for get attributes.
 *
 * Revision 1.7  1997/07/17 21:51:20  dunn
 * Fixed messages and added ddGetNext
 *
 * Revision 1.6  1997/07/15 18:28:47  jaeger
 * checked in for compile with dhs
 *
 * Revision 1.5  1997/07/11 22:38:09  jaeger
 * Added "ANY" to datatype
 *
 * Revision 1.4  1997/07/09 16:35:34  jaeger
 *  Converted library to use dhs instead of bdt
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

#include <string.h>


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
 *  dd library include files.
 ************************************************************************
 */

#include "dd.h"
#include "local_dd.h"

void	        ddFormatMessage();
void            configInit();


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddAttribGet
 *
 * INVOCATION:
 * ddAttribGet( instrument, keyword, row, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) instr (char *) Instrument name.
 * (>) kw (char *) Keyword name to look in eptr list with.
 * (<) row (DD_ROW **) Data dict. entry returned.
 * (!) status (DD_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Function to get attribute out of dd.
 *
 * DESCRIPTION:
 * Function to get attribute out of dd given the keyword name and instrument.
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

 
void             ddAttribGet
(
    char       *instr,	/* (in)  Instrument 				*/
    char       *kw,	/* (in)  Keyword 				*/
    DD_ROW     **row, 	/* (out) dd entry corresponding to instr, kw 	*/
    DD_STATUS  *status  /* (mod) Function return status.                */
)
{
    int		index;
    boolean	rc;
    DD_ROW	rowKey;
 

    statCheck( *status, VOID );
 

    /* 
     *  Fill in the row_key keyword and instrument values.
     */

    strnCpy( rowKey.ddrKeyword, kw, DD_KEYWORD_LEN, *status, VOID );
    strnCpy( rowKey.ddrInstr, instr, DD_INSTR_LEN, *status, VOID );


    /*
     *  Call eptr_search with the ddCompKeyword function as an argument.
     */

    rc = eptr_search( ddKeyword, &rowKey, &index, ddCompKeyword );
    if ( rc == FALSE )
    {
	*status = DD_S_NOT_FOUND;
	ddFormatMessage( *status, "keyword", "instr/keyword",
		rowKey.ddrInstr, rowKey.ddrKeyword );
        strnCpy( rowKey.ddrInstr, "*", DD_INSTR_LEN, *status, VOID );
	rc = eptr_search( ddKeyword, &rowKey, &index, ddCompKeyword );
	if ( rc == FALSE )
	{
	    *status = DD_E_NOT_FOUND;
	    ddFormatMessage( *status, "keyword", "instr/keyword",
		    rowKey.ddrInstr, rowKey.ddrKeyword );
	    return;
	}
    }
    

    /* 
     *  Get the row out at position of index.
     */

    *row = (DD_ROW *) eptritem( ddKeyword, index );
 

    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddEptrFree
 *
 * INVOCATION:
 * Passed as parameter to eptr_free( kkKeyword, ddEptrFree );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) row (DD_ROW *) Row to free.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * To free malloc'ed strings.
 *
 * DESCRIPTION:
 * To free malloc'ed strings.
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

void 	ddEptrFree
(
    DD_ROW	*row		/* (in)  The row to free.		*/
)
{
    if ( row->ddrAttrib != NULL )
    {
	gen_free( row->ddrAttrib );
    }

    if ( row->ddrColName != NULL )
    {
	gen_free( row->ddrColName );
    }

    gen_free( row );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddExit
 *
 * INVOCATION:
 * ddExt( status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DD_STATUS *) Return function status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Function to cleanup the data dictionary.
 *
 * DESCRIPTION:
 * Function to cleanup the data dictionary.
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

void 		ddExit
(
    DD_STATUS  *status  /* (mod) function return status.                */
)
{
    statCheck( *status, VOID );
    
    /*
     * Free the eptr lists.
     * Note: ddKeyword, ddAttrib and ddColumn point to the same
     * memory space.
     */

    eptr_free( ddAttrib, ddEptrFree );
    ddAttrib = NULL;
    eptr_free( ddKeyword, NULL );
    ddKeyword = NULL;
    eptr_free( ddColumn, NULL );
    ddColumn = NULL;


    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddInit
 *
 * INVOCATION:
 * ddInit( status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DD_STATUS *) Return function status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Function to initialize the data dictionary.
 *
 * DESCRIPTION:
 * Description of the algorithm.
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

void 		ddInit
(
    DD_STATUS  *status  /* (mod) function return status.                */
)
{

    statCheck( *status, VOID );

    /*
     * Create the eptr lists.
     */

    ddAttrib = NULL;
    ddKeyword = NULL;
    ddColumn = NULL;

    nullCheck( ddKeyword = eptr_alloc( DD_LIST_CHUNK ), *status, VOID );
    nullCheck( ddAttrib = eptr_alloc( DD_LIST_CHUNK ), *status, VOID );
    nullCheck( ddColumn = eptr_alloc( DD_LIST_CHUNK ), *status, VOID );


    /*
     * Open and read the configuration file.
     */

    configInit( status );
    if ( *status != DD_S_SUCCESS ) 
    {
	return;
    }


    /* Sort the eptr lists */

    nullCheck( ddKeyword = eptr_sort( ddKeyword, ddCompKeyword ), *status,
	       VOID );
    nullCheck( ddAttrib = eptr_sort( ddAttrib, ddCompAttrib ), *status, VOID );


    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddGetNext
 *
 * INVOCATION:
 * ddGetNext ( i )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) i (int) Counter for eptr list.
 * (!) status (DD_STATUS) Modified return function status.
 *
 * FUNCTION VALUE:
 * Returns DD_ROW in data dictionary or NULL if you reach the end.
 *
 * PURPOSE:
 * Get data dictionary items from the eptr list.
 *
 * DESCRIPTION:
 * Get data dictionary items from the eptr list.
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
 
DD_ROW		*ddGetNext
(       
    int	 	i,	 	/* (in)  Index into eptr list.		*/
    DD_STATUS   *status  	/* (mod) function return status.	*/
)
{
 
    statCheck( *status, NULL );
   
    if ( i >=  eptrlen( ddKeyword ) )
    {
	/*
	 *  At the end of the list.
	 */

	return( NULL );
    }

    return( eptritem( ddKeyword, i ) );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddKeywordGet
 *
 * INVOCATION:
 * ddKeywordGet( instrument, attribute, row );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) instr (char *) Instrument name.
 * (>) attrib (char *) Attribute name to look in eptr list with.
 * (<) row (DD_ROW **) Data dict. entry returned.
 * (!) status (DD_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * none
 *
 * PURPOSE:
 * Function to get keyword out of dd.
 *
 * DESCRIPTION:
 * Function to get keyword out of dd given the attribute name.
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
 
void             ddKeywordGet
(       
    char 	*instr, 	/* (in)  Instrument.			*/
    char 	*attrib, 	/* (in)  Attribute to look up.		*/
    DD_ROW 	**row,		/* (out) Row returned.			*/
    DD_STATUS   *status  	/* (mod) function return status.	*/
)
{
    int		index;
    boolean	rc;
    DD_ROW	rowKey;
 
    statCheck( *status, VOID );
    
    /* 
     * Fill in the row_key values.
     */

    nullCheck( rowKey.ddrAttrib = strsav( attrib ), *status, VOID );
    strnCpy( rowKey.ddrInstr, instr, DD_INSTR_LEN, *status, VOID );


    /*
     * Call eptr_search with the ddCompAttrib function as an argument.
     */

    rc = eptr_search( ddAttrib, &rowKey, &index, ddCompAttrib );
    if ( rc == FALSE )
    {
	*status = DD_S_NOT_FOUND;
	ddFormatMessage( *status, "keyword", "instr/attribute",
		rowKey.ddrInstr, rowKey.ddrAttrib );
    	strnCpy( rowKey.ddrInstr, "*", DD_INSTR_LEN, *status, VOID );
	rc = eptr_search( ddAttrib, &rowKey, &index, ddCompAttrib );
	if ( rc == FALSE )
	{
	    ddFormatMessage( DD_E_NOT_FOUND, "attribute", "instr/attribute",
		    rowKey.ddrInstr, rowKey.ddrAttrib );
	    *status = DD_E_NOT_FOUND;
	    return;
	}
    }
    

    /* 
     * Get the row out at position of index.
     */

    *row = (DD_ROW *) eptritem( ddAttrib, index );
    free( rowKey.ddrAttrib );

    return;
}
