static char rcsid[] = "$Id: misc.c,v 1.1.1.1 2002-11-24 20:24:15 brighton Exp $";
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
* dd/src/misc.c
*
* PURPOSE:
*	Contains the routines to parse the configuration file and 
*	compare the keywords.
*
* FUNCTION NAME(S)
*	static	void 	addArray	: Reads Array from config & checks.
*	static boolean	addKeyword	: Function to read the config 
*					  file.
*	static	void 	addType		: Reads & checks data type field.
*	int		configInit	: Read the configuration file.
*					  complete.
*	static int	ddCompAttrib	: Eptr search func. for ddAttrib.
*	static int	ddCompColumn	: Eptr search func. for ddColumn.
*	static int	ddCompKeyword	: Eptr search func. for ddKeyword.
*	static int	uniqueCheck	: Check eptr uniqueness & insert.
*
*INDENT-OFF*
* $Log: not supported by cvs2svn $
* Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
*
*
* Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
* Initial install into CVS of dhs-0.16
*
* Revision 1.9  1997/12/02 20:00:57  jaeger
* Fixed string overruns with strcpy.
*
* Revision 1.8  1997/09/05 23:54:02  jaeger
* removed remaining memory leaks.
*
* Revision 1.7  1997/08/22 17:51:21  jaeger
* Allowable array types changed to "EXTEN", "INDEX" and "NONE"
* Keywords are converted to uppercase, warning if they are not
*
* Revision 1.6  1997/08/15 20:41:07  jaeger
* Converted all "keywords" to upper case
*
* Revision 1.5  1997/07/29 20:04:27  dunn
* Lots of changes.
*
* Revision 1.4  1997/07/15 18:28:47  jaeger
* checked in for compile with dhs
*
* Revision 1.3  1997/07/11 22:38:09  jaeger
* Added "ANY" to datatype
*
* Revision 1.2  1997/07/09 16:35:34  jaeger
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

#include <ctype.h>
#include <string.h>

/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */


#include "gen_types.h"
#include "gen_config.h"
#include "gen_str.h"
#include "gen_msg.h"



/*
 ************************************************************************
 *  dd library include files.
 ************************************************************************
 */

#include "local_dd.h"
#include "dd.h"

#define	KEY_KEYWORD		"OK"


static void     addArray( char*, DD_ROW*, DD_STATUS* );
static boolean	addKeyword( int, char ** );
static void     addOneKeyword( char*, DD_ROW*, DD_STATUS* );
static void     addType( char*, DD_ROW*, DD_STATUS* );

void     ddFormatMessage();



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * addArray
 *
 * INVOCATION:
 * addArray( token, row, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) token (char *) Passed in array string token from config file.
 * (>) row (DD_ROW *) Row in eptr list.
 * (!) status (DD_STATUS *) Function status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Add the array value to a row.  In the process it is
 * converted from a string to a character value.
 *
 * DESCRIPTION:
 * Add the array value to a row.  In the process it is
 * converted from a string to a character value.
 * Error out if the column name is set and the array type is
 * not NO_ARRAY.
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

void	addArray
(
    char	*token,		/* (in)  The token with the value	*/
    DD_ROW	*row,		/* (mod) A row from the config file.	*/
    DD_STATUS	*status		/* (mod) DD library status.		*/
)
{

    statCheck( *status, VOID );

    if ( streq( token, "EXTEN" ) )
    {
	row->ddrArray = DD_EXTENSION;
    }
    else if ( streq( token, "INDEX" ) )
    {
	row->ddrArray = DD_INDEX;
    }
    else if ( streq( token, "NONE" ) )
    {
	row->ddrArray = DD_NO_ARRAY;
    }
    else 
    {
	*status = DD_E_ARRAY;
	ddFormatMessage( *status, token );
	return;
    }

   
    /*
     *  Check that the column name is set to NULL if
     *  the array type is NOT DD_NO_ARRAY.
     */

    if ( row->ddrArray != DD_NO_ARRAY &&
	    strne( row->ddrColName, "NULL" ) )
    {
	*status = DD_E_ARRAY;
	ddFormatMessage( *status, token );
    }
    
    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * addKeyword
 *
 * INVOCATION:
 * addKeyword( numTokens, tokens, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) numTokens (int) Number of tokens in 1 line in config file.
 * (>) tokens (char **) A line in the config file.
 *
 * FUNCTION VALUE:
 *     boolean TRUE            : Normal completion
 *     boolean FALSE           : Something went wrong.
 *
 * PURPOSE:
 * Decodes the information in the configuration file.
 *
 * DESCRIPTION:
 * Decodes the information in the configuration file.
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

static boolean addKeyword
(
    int		numTokens,	/* (in)  The number of arguments.	*/
    char	**tokens        /* (in)  The list of tokens.		*/
)
{
    DD_ROW 	*row;		/* Temporary row.		*/
    char	typeString[DD_VALUE_LEN];
    				/* Temporary type string.	*/
    char	arrayString[DD_VALUE_LEN];
    				/* Temporary array value.	*/
    DD_STATUS   status;         /* Function return status.	*/


    status = DD_S_SUCCESS;
   

    if ( numTokens != DD_MAX_TOKENS )
    {
	ddFormatMessage( DD_E_FORMAT_NUM, numTokens, DD_MAX_TOKENS );
	return( FALSE );
    }


    /* 
     *  Allocate memory for the row.
     */

    nullCheck( row = malloc( sizeof (*row) ), status, FALSE );


    /*
     *  Fill in the values of the eptr item
     */

    strnCpy( row->ddrInstr, tokens[1], DD_INSTR_LEN, status, FALSE );
    nullCheck( row->ddrAttrib = strsav( tokens[2] ), status, FALSE );
    strnCpy( typeString, tokens[3], DD_VALUE_LEN, status, FALSE );
    check( addOneKeyword( tokens[4], row, &status ), status, FALSE );

    strnCpy( row->ddrKeyword, tokens[4], DD_KEYWORD_LEN, status, FALSE );
    
    if ( streq( tokens[5], "T" ) )
    {
	row->ddrGemMand = TRUE;
    }
    else if ( streq( tokens[5], "F" ) )
    {
	row->ddrGemMand = FALSE;
    }
    else
    {
	ddFormatMessage( DD_E_FORMAT, tokens[5], "boolean" );
	return( FALSE );
    }
    strnCpy( row->ddrNullVal, tokens[6], DD_VALUE_LEN, status, FALSE );
    strnCpy( arrayString, tokens[7], DD_VALUE_LEN, status, FALSE );
    nullCheck( row->ddrColName = strsav( tokens[8] ), status, FALSE );
    strnCpy( row->ddrCmts, tokens[9], DD_CMTS_LEN, status, FALSE );

    
    /*
     *  Delay adding data type and array, since it requires the 
     *  null value, as well as column name for error checking.
     */
    
    check( addType( tokens[3], row, &status ), status, FALSE );
    check( addArray( arrayString, row, &status ), status, FALSE );

    /*
     *  Check uniqueness of the instr/keyword and instr/attrib 
     *  combinations and insert into eptr list.
     */

    check( uniqueCheck( row, &status ), status, FALSE );

    return( TRUE );
}



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * addOneKeyword
 *
 * INVOCATION:
 * addType( token, row, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) token (char *) Passed in type string token from config file.
 * (>) row (DD_ROW *) Row in eptr list.
 * (!) status (DD_STATUS *) Function status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Add a keyword to a partiuclar row in the data dictionary.
 *
 * DESCRIPTION:
 * Check the token to make sure it is an uppercase value.  If it
 * is not then give a warning.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * "status" has the value DD_S_SUCCESS.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	addOneKeyword
(
    char	*token,		/* (in)  The token with the value	*/
    DD_ROW	*row,		/* (mod) A row from the config file.	*/
    DD_STATUS	*status		/* (mod) DD library status.		*/
)
{
    int i;
    
    statCheck( *status, VOID );

    for( i = 0; token[i] != '\0'; i++ )
    {
	if( islower( token[i] ) )
	{
	    *status = DD_S_LOWER;
	    ddFormatMessage( DD_S_LOWER, token );
	    break;
	}
	
    }
    
    str2upper( row->ddrKeyword, token );
}


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * addType
 *
 * INVOCATION:
 * addType( token, row, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) token (char *) Passed in type string token from config file.
 * (>) row (DD_ROW *) Row in eptr list.
 * (!) status (DD_STATUS *) Function status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Store the data type in the row.
 *
 * DESCRIPTION:
 * Store the data type in the row.
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

void	addType
(
    char	*token,		/* (in)  The token with the value	*/
    DD_ROW	*row,		/* (mod) A row from the config file.	*/
    DD_STATUS	*status		/* (mod) DD library status.		*/
)
{

    statCheck( *status, VOID );


    if ( streq( token, "CHAR" ) )
    {
	row->ddrType = DHS_DT_CHAR;
    }
    else if ( streq( token, "STRING" ) )
    {
	row->ddrType = DHS_DT_STRING;
    }
    else if ( streq( token, "BOOLEAN" ) )
    {
	row->ddrType = DHS_DT_BOOLEAN;;
    }
    else if ( streq( token, "BYTE" ) )
    {
	row->ddrType = DHS_DT_INT8;;
    }
    else if ( streq( token, "UBYTE" ) )
    {
	row->ddrType = DHS_DT_UINT8;;
    }
    else if ( streq( token, "SHORT" ) )
    {
	row->ddrType = DHS_DT_INT16;
    }
    else if ( streq( token, "USHORT" ) )
    {
	row->ddrType = DHS_DT_UINT16;
    }
    else if ( streq( token, "INT" ) )
    {
	row->ddrType = DHS_DT_INT32;
    }
    else if ( streq( token, "UINT" ) )
    {
	row->ddrType = DHS_DT_UINT32;
    }
    else if ( streq( token, "FLOAT" ) )
    {
	row->ddrType = DHS_DT_FLOAT;
    }
    else if ( streq( token, "DOUBLE" ) )
    {
	row->ddrType = DHS_DT_DOUBLE;
    }
    else if ( streq( token, "ANY" ) )
    {
	if ( strne( row->ddrColName, "NULL" ) )
	{
	    *status = DD_E_TYPE;
	    ddFormatMessage( *status, token );
	}
	else
	{
	    row->ddrType = DD_ANY_TYPE;
	}
    }
    else 
    {
	*status = DD_E_TYPE;
	ddFormatMessage( *status, token );
    }
    
    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * configInit
 *
 * INVOCATION:
 * configInit( status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DD_STATUS *) Function status.
 *
 * FUNCTION VALUE:
 * None
 *
 * PURPOSE:
 * Reads the configuration file.
 *
 * DESCRIPTION:
 * Reads the configuration file.
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

void	configInit
(
    DD_STATUS   *status         /* (mod) function return status.        */
)
{
    char	*configFile;


    statCheck( *status, VOID );
   

    /* 
     *  Open the config file.
     */

    checkConfig( config_open( DD_LIBRARY, &configFile ), *status, VOID ); 


    /*
     *  Read the configuration information.
     */

    checkConfig(  config_get( KEY_KEYWORD, addKeyword ), *status, VOID ); 

    config_close( );

    return;
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddCompAttrib
 *
 * INVOCATION:
 * ddCompAttrib( ddRow1, ddRow2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) ddRow1 (DD_ROW *) First row to compare against.
 * (>) ddRow2 (DD_ROW *) Row to compare against.
 *
 * FUNCTION VALUE:
 * int     1               : ddRow1 > ddRow2
 * int     0               : ddRow1 = ddRow2
 * int     -1              : ddRow1 < ddRow2
 *
 * PURPOSE:
 * Sorting routine for the eptr_sort and eptr_search calls.
 * This only compares the attr and instr fields.
 *
 * DESCRIPTION:
 * Sorting routine for the eptr_sort and eptr_search calls.
 * This only compares the attr and instr fields.
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
 
int      ddCompAttrib
(
    DD_ROW	*ddRow1,	/* (in)  Item to match row.	*/
    DD_ROW	*ddRow2 	/* (in)  Eptr list row.		*/
)
{
    int		compare;

    
    if ( ( compare = strcmp( ddRow1->ddrAttrib, ddRow2->ddrAttrib ) ) != 0 )
    {
	return( compare );
    }

    compare = strcmp( ddRow1->ddrInstr, ddRow2->ddrInstr );

    return( compare );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddCompColumn
 *
 * INVOCATION:
 * ddCompColumn( ddRow1, ddRow2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) ddRow1 (DD_ROW *) First row to compare against.
 * (>) ddRow2 (DD_ROW *) Row to compare against.
 *
 * FUNCTION VALUE:
 * int     1               : ddRow1 > ddRow2
 * int     0               : ddRow1 = ddRow2
 * int     -1              : ddRow1 < ddRow2
 *
 * PURPOSE:
 * Sorting routine for the eptr_sort and eptr_search calls.
 * This only compares the column and instr fields.
 *
 * DESCRIPTION:
 * Sorting routine for the eptr_sort and eptr_search calls.
 * This only compares the column and instr fields.
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
 
int      ddCompColumn
(
    DD_ROW	*ddRow1,	/* (in)  Item to match row.	*/
    DD_ROW	*ddRow2 	/* (in)  Eptr list row.		*/
)
{
    int		compare;

    
    if ( ( compare = strcmp( ddRow1->ddrColName, ddRow2->ddrColName ) ) != 0 )
    {
	return( compare );
    }

    compare = strcmp( ddRow1->ddrInstr, ddRow2->ddrInstr );

    return( compare );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddCompKeyword
 *
 * INVOCATION:
 * ddCompKeyword( ddRow1, ddRow2 );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) ddRow1 (DD_ROW *) First row to compare against.
 * (>) ddRow2 (DD_ROW *) Row to compare against.
 *
 * FUNCTION VALUE:
 * int	1		: ddRow1 > ddRow2
 * int	0		: ddRow1 = ddRow2
 * int	-1		: ddRow1 < ddRow2
 *
 * PURPOSE:
 * Sorting routine for the eptr_sort and eptr_search calls.
 * This only compares the kw and instr fields.
 *
 * DESCRIPTION:
 * Sorting routine for the eptr_sort and eptr_search calls.
 * This only compares the kw and instr fields.
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

int	ddCompKeyword	
(
    DD_ROW	*ddRow1,	/* (in)  Item to match row.	*/
    DD_ROW	*ddRow2 	/* (in)  Eptr list row.		*/
)
{
    int		compare;


    if ( ( compare = strcmp( ddRow1->ddrKeyword, ddRow2->ddrKeyword ) ) != 0 )
    {
	return( compare );
    }

    compare = strcmp( ddRow1->ddrInstr, ddRow2->ddrInstr );

    return( compare );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * uniqueCheck
 *
 * INVOCATION:
 * uniqueCheck( row, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) row (DD_ROW *) Row in eptr list.
 * (!) status (DD_STATUS *) Function status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To verify that the instrument & keyword combination is unique and that
 * that instrument attribute combination is unique.
 *
 * DESCRIPTION:
 * To verify that the instrument & keyword combination is unique and that
 * that instrument attribute combination is unique.  And then insert them
 * into the eptr lists in sorted order.
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

void	uniqueCheck
(
    DD_ROW	*row,		/* (mod) A row from the config file.	*/
    DD_STATUS	*status		/* (mod) DD library status.		*/
)
{
    int		index;		/* Index into the eptr list.		*/
    EPTR   	*ddAttrib;
    EPTR     	*ddColumn;
    EPTR     	*ddKeyword;
    /*
     *  Check the status passed in.
     */

    statCheck( *status, VOID );


    /*
     *  Check that the instrument and attrib don't already exist in the
     *  ddAttrib list.  Error if they are there.
     */
     
    if ( eptr_search( ddAttrib, row, &index, ddCompAttrib ) )
    {
	*status = DD_E_UK_KEY;
	ddFormatMessage( *status, "instru/attrib", row->ddrInstr, 
		row->ddrAttrib);
	ddEptrFree( row );
	return;
    }

    
    /*
     *  Check that the instrument and keyword don't already exist in the
     *  ddKeyword list.  Then insert into the lists if not there.
     */
     
    if ( eptr_search( ddKeyword, row, &index, ddCompKeyword ) )
    {
	*status = DD_E_UK_KEY;
	ddFormatMessage( *status, "instru/keyword", row->ddrInstr, 
		row->ddrKeyword);
	ddEptrFree( row );
	return;
    }

    nullCheck( ddAttrib = eptr_insert( ddAttrib, index,
	       (pointer) (row) ), *status, VOID );

    nullCheck( ddKeyword = eptr_insert( ddKeyword, index,
	       (pointer) (row) ), *status, VOID );


    /*
     *  If the column is not null, then check that the instrument and 
     *  column don't already exist in the ddColumn list.  Then insert
     *  into list if not there.
     */
     
    if ( !streq( row->ddrColName, "NULL" ) )
    {
	if ( eptr_search( ddColumn, row, &index, ddCompColumn ) )
	{
	    *status = DD_E_UK_KEY;
	    ddFormatMessage( *status, "instru/column", row->ddrInstr, 
		    row->ddrColName );
	    return;
	}
	nullCheck( ddColumn = eptr_insert( ddColumn, index,
		(pointer) (row) ), *status, VOID );
    }
}

