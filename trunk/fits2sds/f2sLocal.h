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
 * fits2sdsApp/h/localF2s.h
 *
 * PURPOSE:
 * 
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:39  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.4  1997/12/02 21:27:48  jaeger
 * Added a macro for printing sfMsg.
 *
 * Revision 1.3  1997/11/19 00:15:06  jaeger
 * Fixed messages so they are always printed and use the prefix "f2s"
 *
 * Revision 1.2  1997/09/05 23:56:49  jaeger
 * fixed memory leaks.
 *
 * Revision 1.1  1997/09/05 19:25:37  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef LOCAL_F2S_H
#define	LOCAL_F2S_H


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
 
#define COMPOSE		"comp"	/* Part of file name that indicates the	*/
				/* file is composed of several chunks.	*/
#define FULL		"full"	/* Part of file name that indicates the	*/
				/* file was constructed from a SDS	*/
				/* structure containing a complete FITS	*/
				/* file.				*/
    
#define FITS_DISK	"fits"	/* File extension for a FITS disk file.	*/
#define FITS_MEMORY	"mem"	/* File extension for a FITS memory file*/
#define SDS_MEMORY	"sds"	/* File extension for a SDS memory file.*/

#define VOID                    /* Return value for void functions.     */

    
/*
 ************************************************************************
 *   Macro definitions.
 ************************************************************************
 */    



/************************************************************************
 *+
 * FUNCTION NAME:
 * check
 *
 * INVOCATION:
 * check( function( args, &status ), status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (function invocation) A function to invoke.
 * (>) status (F2S_STATUS) The return status of the fn parameter.
 * (>) val (unknown) The value to return should the function fail.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute the specified function and verify the return status is success
 * or a warning.
 *
 * If the return status is an error, has a negative value, then an error
 * message is formatted, and the current function is returned with the
 * value specified in the "val" parameter. A special return value of "VOID"
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
 
#define check( fn, status, val )					\
    fn;									\
    if ( status < F2S_S_SUCCESS )					\
    { 									\
        return val;							\
    }



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * exitF2s
 *
 * INVOCATION:
 * exitF2s( status, sfStatus, dhsStatus, returnValue, messageToAppend )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) 	status	    (F2S_STATUS)	Fits2sds application status.
 * (>) 	sfStatus    (SF_STATUS)	Fits2sds application status.
 * (>) 	dhsStatus   (DHS_STATUS)	Fits2sds application status.
 * (>) 	val	    (int)		Return value.
 * (>)	msg	    (char*)		MessaageToAppend
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * To exit the fits2sds application. 
 *
 * DESCRIPTION:
 * 1. Format a message, ane display it.
 * 2. Exit both the SF library and DHS library.
 * 3. Free used memory.
 * 4. Return the given value.
 *
 * EXTERNAL VARIABLES:
 * f2sMsg	- 	Contains the status messages from fits2sds.
 *
 * PRIOR REQUIREMENTS:
 * "dsInfo", "inFile", and "objectList" are all defined.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
    
#define exitApp( status, sfStatus, dhsStatus, val, msg )		\
    {									\
        status = val;							\
        f2sFormatMessage( status );					\
        if( msg != NULL )						\
        {								\
 	    msg_append( f2sMsg, msg );					\
	    msg_clear( msg );						\
        }								\
        f2sPrintMessage();						\
	if ( inFile != NULL )						\
	{								\
	    gen_free( inFile );						\
	}								\
	if ( dsInfo != NULL )						\
	{								\
	    sfDsInfoFree( dsInfo );					\
	}								\
	if ( objectList != NULL )					\
	{								\
	    eptr_free( objectList, sfObjectFree );			\
	}								\
	if ( sfStatus != NULL )						\
	{								\
	    sfExit( sfStatus );						\
	}				       				\
	if ( dhsStatus != NULL )					\
	{								\
	    dhsExit( dhsStatus );					\
	}								\
	return val;							\
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
 * (>) status (F2S_STATUS) Function return status value.
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
 * F2S_E_MEMORY, and causes the function to return the value specified in
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
 * "sfStatus" needs to be defined.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
 
#define nullCheck( ptr, status, val )					\
    if ( ( ptr ) == NULL )						\
    {									\
	status = F2S_E_MEMORY;						\
        f2sFormatMessage( status );					\
        return val;							\
    }


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * printSfMsg
 *
 * INVOCATION:
 * printSfMsg( )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * None.
 * 
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * To print the current  sf messages and clear sf messages stack.
 *
 * DESCRIPTION:
 * Trivial
 *
 * EXTERNAL VARIABLES:
 * sfMsg	- 	Contains the status messages from sf.
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
    
#define	printSfMsg( )							\
    (void) printf( "%s\n", sfMsg );					\
    (void) fflush( stdout );						\
    msg_clear( sfMsg);

    


/************************************************************************
 *+
 * FUNCTION NAME:
 * sfCheck
 *
 * INVOCATION:
 * sfCheck( function( args, &sfStatus ), sfStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (function invocation) A function to invoke.
 * (>) sfStatus (SF_STATUS) The return status of the fn parameter.
 * (>) status (F2S_STATUS) The fits2sds application status.
 * (>) val (unknown) The value to return should the function fail.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute the specified function and verify the return status is success
 * or a warning.
 *
 * If the return status is an error, has a negative value, then an error
 * message is formatted, and the current function is returned with the
 * value specified in the "val" parameter. A special return value of "VOID"
 * has been defined as the return value of void functions.  "status" is
 * set to F2S_E_SF.
 *
 * DESCRIPTION:
 * Execute the function and check the return status.
 *
 * EXTERNAL VARIABLES:
 * sfMsg	- 	Contains the status messages from sf.
 * f2sMsg	- 	Contains the status messages from fits2sds.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
 
#define sfCheck( fn, sfStatus, status, val )				\
    fn;									\
    if ( sfStatus < SF_S_SUCCESS )					\
    { 									\
	status = F2S_E_SF;						\
        f2sFormatMessage( status, #fn, sfStatus );			\
        msg_append( f2sMsg, sfMsg );					\
        msg_clear( sfMsg);						\
        return val;							\
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
 * (>) status (F2S_STATUS) Function return status value.
 * (>) val (unknown) Value to return if the status is not SF_S_SUCCESS.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Check the status value and return immediately if it is less than
 * F2S_S_SUCCESS
 *
 * The function return value is specified in the "val" parameter. A special
 * return value of "VOID" has been defined for void functions.
 *
 * DESCRIPTION:
 * Check the status and if it is not F2S_SUCCESS, return the specified value.
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
    if ( ( status ) < F2S_S_SUCCESS )                                   \
    {                                                                   \
        return val ;                                                    \
    }

 
/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern 	void	f2sComposeChunks( char*, EPTR*, SF_DS_INFO*, F2S_STATUS* );
extern 	void 	f2sWriteChunks( char*, EPTR*, SF_DS_INFO*, F2S_STATUS* );
extern	void	f2sWriteFull( char*, SF_OBJECT*, SF_DS_INFO*, F2S_STATUS* );
extern 	void	f2sPrintMessage( void );
    
    
/*    
 ************************************************************************
 *  For C++
 ************************************************************************
 */
 
#ifdef    __cplusplus
}
#endif

#endif /* LOCAL_F2S_H */
