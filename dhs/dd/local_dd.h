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
 * ddLibrary/local_dd.h
 *
 * PURPOSE:
 *	Defines the local functions for the dd routines.	
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:03  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.11  1998/01/05 19:19:10  jaeger
 * fixed strnCpy so no error message is produced when no characters are lost.
 *
 * Revision 1.10  1997/12/02 20:03:52  jaeger
 * added macro for checking string lengths with strncpy.
 *
 * Revision 1.9  1997/09/05 23:54:33  jaeger
 * statCheck now return if status is less than DD_S_SUCCESS.
 *
 * Revision 1.8  1997/08/22 17:51:21  jaeger
 * Allowable array types changed to "EXTEN", "INDEX" and "NONE"
 * Keywords are converted to uppercase, warning if they are not
 *
 * Revision 1.7  1997/07/29 20:04:44  dunn
 * Lots of changes.
 *
 * Revision 1.6  1997/07/17 21:52:22  dunn
 * Changed fnReturn to not format a message.
 *
 * Revision 1.5  1997/07/11 22:38:43  jaeger
 * Added "ANY" to datatype
 *
 * Revision 1.4  1997/07/09 16:36:19  jaeger
 * Converted library to use dhs instead of bdt
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/


#ifndef _local_dd_h
#define _local_dd_h


/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_eptr.h"

/*
 ************************************************************************
 *  dhs library include files.
 ************************************************************************
 */

#include "dd.h"

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
 ************************************************************************
 */

#define DD_LIBRARY     "libdd"
#define DD_LIST_CHUNK   100

#define VOID                    /* Return value for void functions.     */

/* 
 ************************************************************************
 *  Message types.
 ************************************************************************
 */

#define	MESS_ERROR	0
#define	MESS_INFO	1


/*
 ************************************************************************
 *  Macro definitions.
 ************************************************************************
 */
    

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * check
 *
 * INVOCATION:
 * check( function( args, &status ), status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (function invocation) A function to invoke.
 * (>) status (DD_STATUS) The return status of the fn parameter.
 * (>) val (unknown) The value to return should the function fail.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute the specified function and verify the return status is success
 *
 * If the return status is less than DD_S_SUCCESS, an error message is 
 * formatted, and the current function is returned with the value
 * specified in the "val" parameter. A special return value of "VOID"
 * has been defined as the return value of void functions.
 *
 * DESCRIPTION:
 * Execute the function and check the return status.
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
 
#define check( fn, status, val )                                        \
        fn;                                                             \
        if ( status < DD_S_SUCCESS )                                    \
        {                                                               \
            return val;                                                 \
        }

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * checkConfig
 *
 * INVOCATION:
 * checkConfig( function( args, &status ), status, val ); 
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) paramName (int) Descripton of the parameter
 * (!) paramName (float) Descripton of the parameter
 * (<) paramName (long) Descripton of the parameter
 *
 * MACRO VALUE:
 * None
 *
 * PURPOSE:
 * Execute the configureation function and check the return status.
 *
 * If the return status is not DD_S_SUCCESS, an error message is
 * formatted, and the current function is returned with the value
 * specifiend in the "val" parameter.  A special return vaoue of
 * "VOID" has been deefined a sthe return value of void functions.
 *
 * DESCRIPTION:
 * Execute the function and check the return status.
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



#define checkConfig( fn, status, val )                                  \
    if ( ( fn ) != GC_SUCCESS )                                         \
    {                                                                   \
	 status = DD_E_CONFIG;                                          \
         ddFormatMessage( DD_E_CONFIG );                                \
	 msg_append( ddMsg, gc_msg );					\
	 msg_clear( gc_msg );						\
         return val;                                                    \
     }


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * nullCheck
 *
 * INVOCATION:
 * nullcheck( ptr = malloc( size ), status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) ptr (void *) Pointer to allocated memory.
 * (>) status (DHS_STATUS) Function return status value.
 * (>) value (unknown) Return value for the function.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Check that the pointer returned by a memory allocation routine is not
 * NULL
 *
 * If the pointer is null, this macro sets the status parameter to 
 * DHS_E_MEMORY, and causes the function to return the value specified in
 * the "val" parameter. A special value of "VOID" is defined as the
 * return value of void functions.
 *
 * DESCRIPTION:
 * Check the pointer value, and if it is null, format a message, set the
 * status value and return.
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
 
#define nullCheck( ptr, status, val )                                   \
        if ( ( ptr ) == 0 )                                          \
        {                                                               \
            ddFormatMessage( DD_E_MEMORY );                             \
            status = DD_E_MEMORY;                                       \
            return val;                                                 \
        }
    

    
/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * statCheck
 *
 * INVOCATION:
 * statCheck( status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) status (DHS_STATUS) Function return status value.
 * (>) val (unknown) Value to return if the status is not DHS_S_SUCCESS.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Check the status value and return immediately if it is not DHS_S_SUCCESS
 *
 * The function return value is specified in the "val" parameter. A special
 * return value of "VOID" has been defined for void functions.
 *
 * DESCRIPTION:
 * Check the status and if it is not DHS_SUCCESS, return the specified value.
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
 
#define statCheck( status, val )                                        \
        if ( ( status ) < DD_S_SUCCESS )                 		\
        {                                                               \
            return val ;                                                \
        }



  /*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * strnCpy
 *
 * INVOCATION:
 * strnCpy( toString, fromString, size, status, val )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) toString         (char*)         Copy of of the given string.
 * (<) fromString       (char*)         String that will be copied.
 * (>) size             (int)           Maximum lenght of toString.
 * (>) status           (DD_STATUS)     Function return status value.
 * (>) value            (unknown)       Return value for the function.
 *
 * MACRO VALUE
 * NONE
 *
 * PURPOSE:
 * To ensure the toString is null terminated, since the C library
 * function "strncpy" does not do this.
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
 
#define strnCpy( toString, fromString, size, status, val )              \
    nullCheck( strncpy( toString, fromString, size - 1 ), status, val );\
    if ( strlen( fromString ) <= size - 1 )                              \
    {                                                                   \
             toString[ strlen( fromString ) ] = '\0';                   \
    }                                                                   \
    else                                                                \
    {                                                                   \
            toString[ size - 1 ] = '\0';    				\
	    status = DD_S_STRING_SIZE;					\
	    ddFormatMessage( status, size - 1, fromString );		\
    }
 
    
/*
 ************************************************************************
 *  Internal function declarations.
 ************************************************************************
 */
	

extern	int	config_init();
extern	int	ddCompAttrib( DD_ROW*, DD_ROW* );
extern	int	ddCompColumn( DD_ROW*, DD_ROW* );
extern 	int	ddCompKeyword( DD_ROW*, DD_ROW* );
extern	void 	ddEptrFree( DD_ROW * );
extern	void	uniqueCheck( DD_ROW*, DD_STATUS* );


#ifdef    __cplusplus
}
#endif

  
  
#endif /* _local_dd_h */
#ifdef  __EDD
￼
EPTR   *ddAttrib;
EPTR     *ddColumn;
EPTR     *ddKeyword;
#endif
