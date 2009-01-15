/*
 ************************************************************************
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1997.				(c) 1997
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
 * sf/h/localSf.h
 *
 * PURPOSE:
 * Contains all of the definitions that are needed internally to
 * the sf library.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.13  1999/02/09 20:40:52  jaeger
 * Now supports converstoin from DHS_DT_INT8 to some FITS type.
 *
 * Revision 1.12  1999/01/26 18:47:18  jaeger
 * Removed string2 function and placed them in the gen library.
 *
 * Revision 1.11  1998/05/12 23:10:56  nhill
 * Changed str2bool to match the one in gen.
 *
 * Revision 1.10  1998/04/30 06:14:22  jaeger
 * Added appending of FITS error messages to sfMsg.  Moved extension
 * function definitions to extension.h
 *
 * Revision 1.9  1997/12/02 21:09:35  jaeger
 * Added a strnCat macro that makes sure the length of the string being
 * concatenated is not surpassed.
 *
 * Revision 1.8  1997/11/21 00:22:28  jaeger
 * Fixed problem with data being separated from headers during composition.
 *
 * Revision 1.7  1997/11/19 00:10:15  jaeger
 * Added sorting of "extension" list in the dataset information, removed
 * sfPrintMessage, and fixed bug in fits2sds (big dataset did not have
 * the proper frame structure).
 *
 * Revision 1.6  1997/08/22 22:10:35  jaeger
 * Added support for multi-dimensional arrays.
 *
 * Revision 1.5  1997/08/19 19:03:45  jaeger
 * Checked in for install
 *
 * Revision 1.4  1997/08/15 20:43:37  jaeger
 * Checked in for install
 *
 * Revision 1.3  1997/08/12 21:08:03  jaeger
 * Checked in for install
 *
 * Revision 1.2  1997/08/06 18:30:48  jaeger
 * Checked in for install and testing with dhs
 *
 * Revision 1.1	 1997/07/09 16:34:00  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N	 A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef LOCAL_SF_H
#define	LOCAL_SF_H

/*
 ************************************************************************
 *  For C++
 ************************************************************************
 */


#ifdef	__cplusplus
extern "C" {
#endif
  


/************************************************************************
 *  Constant definitions.
 ************************************************************************
 */

#define VOID			/* Return value for void functions.	*/


/*
 * various constants needed for the eptr list.
 */
    
#define NULL_KEYWORDS	0		/* Null number of keywords	   			*/
#define NULL_EXT_TYPE	'X'  	/* Null extension type		   			*/

#define FITS_S_SUCCESS	0		/* Success flag for FITS library.		*/

/*
 ************************************************************************
 *  Simple data types types
 ************************************************************************
 */

#define FITS_STATUS		    int



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * fnEntry
 *
 * INVOCATION:
 * fnEntry( "functionName " );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) paramName (int) Descripton of the parameter
 * (!) paramName (float) Descripton of the parameter
 * (<) paramName (long) Descripton of the parameter
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Display a message when entring a function.
 *
 * This macro also saves the name of the function so that it can be
 * displayed when the function exits.
 *
 * DESCRIPTION:
 * Description of the algorithm.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * Must be used after the last data declaration and before the first
 * executable statement of a function.
 *
 * SEE ALSO:
 * - fnReturn.
 *
 * DEFICIENCIES:
 * Contains both data definition statements and executable statements,
 * excluding use of another macro that does the same thing.
 *-
 ************************************************************************
 */

#define fnEntry( fn, status )						\
    const char	*sfFnName = fn;						\
    SF_STATUS	sfFnStatus = status;					


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * fnReturn
 *
 * INVOCATION:
 * fnReturn( val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) val (unkown) return value for the function.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Return from a function, printing a message idicating the function is
 * exiting
 *
 * The argument to this macro is the to be returned by the function. A 
 * special macro "VOID" is declared to be used as the return value of
 * void functions.
 *
 * DESCRIPTION:
 * Format a message indicating a function has been entered.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The fnEntry macro must have been used at the top of the function to
 * indicate entry to the function.
 *
 * SEE ALSO:
 * - fnEntry
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define fnReturn( val )							\
	return val


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * isInit
 *
 * INVOCATION:
 * isInit( status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) status (SF_STATUS) Current status value.
 * (>) val (unkown) Value to return if the SF library is not initialized.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Verify that the sf library has been initialized with the sfInit function.
 *
 * If the library has not been initialized an error status is set and the
 * function will return with the specified return value. A special return
 * value of VOID has been defined as the return value of void functions.
 *
 * DESCRIPTION:
 * Check that the SF library has been initialized, and if it hasn't, 
 * set the status value to SF_E_INIT and cause the function to return
 * with the value specified in the "val" parameter.
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
#define isInit( status, val )						\
    if ( ! sfInitTest() )						\
    {									\
	status = SF_E_INIT;						\
	return val;							\
    }



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
 * (>) status (SF_STATUS) The return status of the fn parameter.
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
        fn;								\
	if ( status < SF_S_SUCCESS )					\
	{								\
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
 * (>) status (SF_STATUS) Function return status value.
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
 * SF_E_MEMORY, and causes the function to return the value specified in
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

#define nullCheck( ptr, status, val )					\
	if ( ( ptr ) == NULL )						\
	{								\
	    sfFormatMessage( SF_E_MEMORY );				\
	    status = SF_E_MEMORY;					\
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
 * (>) status (SF_STATUS) Function return status value.
 * (>) val (unknown) Value to return if the status is not SF_S_SUCCESS.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Check the status value and return immediately if it is not SF_S_SUCCESS
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

#define statCheck( status, val )					\
    if ( ( status ) < SF_S_SUCCESS )					\
    {									\
        return val ;							\
    }


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * ddCheck
 *
 * INVOCATION:
 * ddCheck( dhsFunction( args, &dhsStatus ), dhsStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (dhs function) Dhs function to execute.
 * (>) ddStatus (DD_STATUS) DHS function return status.
 * (>) status (SF_STATUS) SF library function return status.
 * (>) val (unknown) Value to return if the DHS function fails.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute a dhs function and check for errors
 *
 * If the DD function fails, the SF library function return status is 
 * set to SF_E_DD, and message is formatted, and the function returns
 * the value specified in the "val" parameter. A special return value
 * of "VOID" is provided for void functions.
 *
 * DESCRIPTION:
 * Execute the DD function, check the return status and if it is an
 * error, has a value less than zero, format an error message,
 * set the SF library function return status and return the specified value.
 *
 * If the return status is a warning, has a value greater then zero,
 * format an error message.
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

#define ddCheck( fn, ddStatus, status, val )				\
	fn;								\
	if ( ddStatus < DD_S_SUCCESS )					\
	{								\
	    sfFormatMessage( SF_E_DD, #fn, ddStatus );			\
	    msg_append( sfMsg, ddMsg );					\
	    msg_clear( ddMsg );						\
	    status = SF_E_DD;						\
	    return val;							\
	}								\
        else if ( ddStatus > DD_S_SUCCESS )				\
        {								\
	    msg_append( sfMsg, ddMsg );					\
	    msg_clear( ddMsg );						\
	}
	    
	 

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCheck
 *
 * INVOCATION:
 * dhsCheck( dhsFunction( args, &dhsStatus ), dhsStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (dhs function) Dhs function to execute.
 * (>) dhsStatus (DHS_STATUS) DHS function return status.
 * (>) status (SF_STATUS) SF library function return status.
 * (>) val (unknown) Value to return if the DHS function fails.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute a dhs function and check for errors
 *
 * If the DHS function fails, the SF library function return status is 
 * set to DHS_E_DHS, and message is formatted, and the function returns
 * the value specified in the "val" parameter. A special return value
 * of "VOID" is provided for void functions.
 *
 * DESCRIPTION:
 * Execute the DHS function, check the return status and if it isn't 
 * DHS_S_SUCCESS, format an error message, set the SF library function
 * return status and return the specified value.
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

#define dhsCheck( fn, dhsStatus, status, val )				\
	fn;								\
	if ( dhsStatus != DHS_S_SUCCESS )				\
	{								\
	    sfFormatMessage( SF_E_DHS, #fn, dhsStatus );		\
	    status = SF_E_DHS;						\
	    return val;							\
	}


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * fitsCheck
 *
 * INVOCATION:
 * fitsCheck( fitsFunction( args, &fitsStatus ), fitsStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (cfitsio function) cfitsio function to execute.
 * (>) fitsStatus (int) cfitsio function return status.
 * (>) status (SF_STATUS) SF library function return status.
 * (>) val (unknown) Value to return if the cfitsio function fails.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute a cfitsio function and check for errors
 *
 * If the cfitsio function fails, the SF library function return status is 
 * set to DHS_E_FITS, and message is formatted, and the function returns
 * the value specified in the "val" parameter. A special return value
 * of "VOID" is provided for void functions.
 *
 * DESCRIPTION:
 * Execute the DHS function, check the return status and if it isn't 
 * 0, format an error message, set the SF library function
 * return status and return the specified value.
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

#define fitsCheck( fn, fitsStatus, status, val )			\
	fn;								\
	if ( fitsStatus !=  FITS_S_SUCCESS )				\
	{								\
	    status = SF_E_FITS;						\
	    sfFormatMessage( status, fitsStatus, #fn );			\
 	    while( ffgmsg( fitsMsg ) )					\
	    {								\
	    	msg_append( sfMsg, fitsMsg );				\
	    }								\
	    ffcmsg( );							\
	    return val;							\
	}
 
/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhs2fits
 *
 * INVOCATION:
 * dhs2fits( dhsType, fitsType, status, val)
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
 * To convert from a given DHS data type, defined in the dhs
 * libaray, to a FITS data type, defined in cfitsio.
 *
 *
 * DESCRIPTION:
 * Based on the DHS type given assign a value to the FITS Type.  If
 * an undefined DHS type was given then set the status to SF_E_DHS_TYPE,
 * format a status message and return from the function.  If the
 * DHS type is DHS_DT_TAG or DHS_DT_AVLIST set sf status to
 * SF_E_CONV_TYPE and  format a status message and return from the
 * function.  
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - fits2dhs
 *
 * DEFICIENCIES:
 * Unable to assign a valud to FITS Type if the DHS type has the
 * value DHS_DT_TAG or DHS_DT_AVLIST since there is no similar 
 * types in cfitsio.
 *
 * cfitsio has no signed 8 bit datatype so the unsigned 8 bit data
 * type is used instead.
 *-
 ************************************************************************
 */

#define	 dhs2fits( dhsType, fitsType, status, val )			\
    switch( dhsType )							\
    {									\
	    case DHS_DT_BOOLEAN:					\
	        fitsType = TLOGICAL;					\
	        break;							\
	    case DHS_DT_CHAR:						\
	        fitsType = TBYTE;					\
	        break;							\
	    case DHS_DT_STRING:						\
	        fitsType = TSTRING;				  	\
	        break;							\
	    case DHS_DT_UINT8:						\
	        fitsType = TBYTE;			   		\
	        break;							\
	    case DHS_DT_INT8:					   	\
	        fitsType = TBYTE;			  		\
	        break;							\
	    case DHS_DT_INT16:						\
	        fitsType = TSHORT;					\
	        break;							\
	    case DHS_DT_UINT16:						\
	        fitsType = TUSHORT;					\
	        break;							\
	    case DHS_DT_INT32:						\
	        fitsType = TLONG;					\
	        break;							\
	    case DHS_DT_UINT32:						\
	        fitsType = TULONG;					\
	        break;							\
	    case DHS_DT_FLOAT:						\
	        fitsType = TFLOAT;					\
	        break;							\
	    case DHS_DT_DOUBLE:						\
	        fitsType = TDOUBLE;					\
	        break;							\
	    case DHS_DT_TAG:						\
	    case DHS_DT_AVLIST:						\
	        *sfStatus = SF_E_CONV_TYPE;	 			\
	        sfFormatMessage( *sfStatus, fitsType );			\
	        fnReturn( val );					\
	    default:							\
	        *sfStatus = SF_E_DHS_TYPE;				\
	        sfFormatMessage( *sfStatus, type );			\
	        fnReturn( val );					\
    }									


 
/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhs2bitpix
 *
 * INVOCATION:
 * dhs2fits( dhsType, fitsType, status, val )
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
 * To convert from a given DHS data type, defined in the dhs
 * libaray, to a valid fits type, defined in the cfitsio library.
 *
 *
 * DESCRIPTION:
 * Based on the DHS type given assign a value to bitpix Type.  If
 * an undefined DHS type was given then set the status to SF_E_DHS_TYPE,
 * format a status message and return from the function.  If the
 * DHS type is DHS_DT_TAG or DHS_DT_AVLIST set sf status to
 * SF_E_CONV_TYPE.
 *
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - fits2dhs
 *
 * DEFICIENCIES:
 * Unable to assign a valud to FITS Type if the DHS type has the
 * value DHS_DT_TAG or DHS_DT_AVLIST since there is no similar 
 * types in cfitsio.
 *-
 ************************************************************************
 */

#define	 dhs2bitpix( dhsType, bitpix, status, val )			\
    switch( dhsType )							\
    {									\
	    case DHS_DT_BOOLEAN:					\
	    case DHS_DT_CHAR:						\
	    case DHS_DT_UINT8:						\
	    case DHS_DT_INT8:						\
	        bitpix = BYTE_IMG;					\
		dhsType = DHS_DT_UINT8;					\
	        break;							\
	    case DHS_DT_INT16:						\
	        bitpix = SHORT_IMG;					\
	        break;							\
            case DHS_DT_UINT16:						\
	        bitpix = USHORT_IMG;					\
	        break;							\
	    case DHS_DT_INT32:						\
	        bitpix = LONG_IMG;					\
	        break;							\
	    case DHS_DT_UINT32:						\
	        bitpix = ULONG_IMG;					\
	        break;							\
	    case DHS_DT_FLOAT:						\
	        bitpix = FLOAT_IMG;					\
	        break;							\
	    case DHS_DT_DOUBLE:						\
	        bitpix = DOUBLE_IMG;					\
	        break;							\
	    case DHS_DT_STRING:						\
		*sfStatus = SF_E_CONV_TYPE;		   		\
                break;							\
	    case DHS_DT_TAG:						\
	    case DHS_DT_AVLIST:						\
	    default:							\
	        *sfStatus = SF_E_DHS_TYPE;			  	\
	        sfFormatMessage( *sfStatus, type );			\
	        fnReturn( val );					\
    }									



/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * fits2dhs
 *
 * INVOCATION:
 * fits2dhs( fitsType, dhsType, status, val)
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
 * To convert from a given FITS type, defined in cfitsio, to a
 * DHS data type.
 *
 *
 * DESCRIPTION:
 * Based on the FITS type given assign a value to the DHS Type.  If
 * an undefined FITS type was given then set the status to SF_E_TYPE,
 * format a status message and return from the function.  If the
 * FITS type is TCOMPLEX or TDBLCOMPLEX set sf status to SF_E_CONF_TYPE
 * format a status message and return from the function.  
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhs2fits
 *
 * DEFICIENCIES:
 * Unable to assign a valud to DHS Type if the FITS type has the
 * value TCOMPLEX or TDBLCOMPLEX since there are no similarily defined
 * types in DHS.
 *-
 ************************************************************************
 */

    
#define fits2dhs( fitsType, dhsType, status, val )			\
    switch( fitsType )							\
    {									\
        case TLOGICAL:							\
	        dhsType = DHS_DT_BOOLEAN;				\
	        break;							\
	    case TBIT:							\
	        dhsType = DHS_DT_CHAR;					\
	        break;							\
	    case TSTRING:						\
	        dhsType = DHS_DT_STRING;				\
	        break;							\
	    case TBYTE:							\
	        dhsType = DHS_DT_UINT8;					\
	        break;							\
	    case TINT:							\
	        dhssType = DHS_DT_INT8;					\
		break;							\
	    case TSHORT:						\
	        dhsType = DHS_DT_INT16;					\
	        break;							\
	    case TUSHORT:						\
	        dhsType = DHS_DT_UINT16;				\
	        break;							\
	    case TLONG:							\
	        dhsType = DHS_DT_INT32;					\
	        break;							\
	    case TULONG:						\
	        dhsType = DHS_DT_UINT32;				\
	        break;							\
	    case TFLOAT:						\
	        dhsType = DHS_DT_FLOAT;					\
	        break;							\
	    case TDOUBLE:						\
	        dhsType = DHS_DT_DOUBLE;				\
	        break;							\
	    case TCOMPLEX:						\
	    case TDBLCOMPLEX:						\
	        *sfStatus = SF_E_CONV_TYPE;				\
	        sfFormatMessage( *sfStatus, fitsType );			\
	        fnReturn( val );					\
	    default:							\
		*sfStatus = SF_E_DHS_TYPE;				\
		sfFormatMessage( *sfStatus, dhsType );			\
	        fnReturn( val );					\
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
 * (>) toString		(char*)		Copy of of the given string.
 * (<) fromString	(char*) 	String that will be copied.
 * (>) size		(int) 		Maximum lenght of toString.
 * (>) status 		(SF_STATUS)     Function return status value.
 * (>) value 		(unknown) 	Return value for the function.
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

#define strnCpy( toString, fromString, size, status, val )		\
    nullCheck( strncpy( toString, fromString, size - 1 ), status, val );\
    if ( strlen( fromString ) < size - 1 )                              \
    {                                                                   \
            ( toString )[ strlen( fromString ) ] = '\0';                \
    }                                                                   \
    else                                                                \
    {                                                                   \
            ( toString )[ size - 1 ] = '\0';                            \
    }

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * strnCat
 *
 * INVOCATION:
 * strnCat( toString, fromString, size, status, val )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) toString		(char*)		Copy of of the given string.
 * (<) fromString	(char*) 	String that will be copied.
 * (>) size		(int) 		Maximum lenght of toString.
 * (>) status 		(SF_STATUS) 	Function return status value.
 * (>) value 		(unknown) 	Return value for the function.
 *
 * MACRO VALUE
 * NONE
 *
 * PURPOSE:
 * To ensure the toString is null terminated, and that the desired
 * number of character are concatenated in order to maintain the
 * maximum string length for the "toString".
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

#define strnCat( toString, fromString, size, status, val )		\
    if ( strlen( fromString ) < size - 1 - strlen( toString ) )		\
    {		      						\
        nullCheck( strncat( toString, fromString, 			\
		strlen( fromString ) ), status, val );			\
    }									\
    else								\
    {									\
        nullCheck( strncat( toString, fromString, size - 1 - 		\
		strlen( toString ) ), status, val );			\
    }									
    
    

/*
 ************************************************************************
 *  External data defitions.
 ************************************************************************
 */

extern char	fitsMsg[];
    

/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */


extern	boolean		sfInitTest( void );
extern 	boolean		sfHasData( int, long[] );
extern	boolean		str2byte( char*, char );
extern	boolean		str2ubyte( char*, unsigned char );    
    
/*
 ************************************************************************
 *  Needed for C++ applications
 ************************************************************************
 */

  
#ifdef	  __cplusplus
}
#endif
  
#endif /* LOCAL_SF_H */


