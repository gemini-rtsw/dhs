static char rcsid[] = "$Id: main.c,v 1.1.1.1 2002-11-24 20:21:49 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1996				(c) 1996.
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
 * dhsClient/src/main.c
 *
 * PURPOSE:
 * Program to test the DHS library from a client point of view.
 *
 * FUNCTION NAME(S)
 * main - Main function.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:38  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:33  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.11  1998/09/15 19:20:17  dunn
 * Fixed checkRet marco, Cleared the message using dhsMessageClear after printing it.
 *
 * Revision 1.10  1998/08/05 17:30:59  nhill
 * Added some mutexes to protect the command counter.
 *
 * Revision 1.9  1998/08/04 22:58:30  nhill
 * Added a loop to send a lot of bulk data.
 *
 * Revision 1.8  1998/06/16 23:26:41  nhill
 * Fixed ulong for vxworks.
 *
 * Revision 1.7  1998/06/16 22:26:27  nhill
 * Added an unknown command to the commands sent.
 *
 * Revision 1.6  1998/02/05 19:11:28  nhill
 * *** empty log message ***
 *
 * Revision 1.5  1997/06/10 23:10:12  nhill
 * Added attributes with long names.
 *
 * Revision 1.4  1997/04/16 21:13:48  nhill
 * *** empty log message ***
 *
 * Revision 1.3  1997/03/27 23:08:43  nhill
 * Changed to support hpux.
 *
 * Revision 1.2  1997/01/22 21:54:31  nhill
 * *** empty log message ***
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifndef	VxWorks
#define	logMsg	printf
#endif	/* !VxWorks */
/*
 *  sds.h is only required so some SDS functions can be used directly.
 */

#include "sds.h"

#include "gen_types.h"

#include "dhs.h"

#define	VOID

/*
 *  Macro to check the return status of a DHS library function and exit
 *  if it failed.
 */

#define	check( fn, st, val )						\
	fn;								\
	if ( st != DHS_S_SUCCESS )					\
	{								\
	    DHS_STATUS		stat2, errorNum;			\
	    DHS_ERR_LEVEL	errorLev;				\
	    char		*string;				\
	    stat2 = DHS_S_SUCCESS;					\
	    string = dhsMessage( &errorNum, &errorLev, &stat2 );	\
	    logMsg( "error checking function:\n%s\n", #fn );		\
	    if ( stat2 == DHS_S_SUCCESS )				\
	    {								\
		(void) logMsg( "status: %d message: %s\n", st, string );\
	    }								\
	    else							\
	    {								\
		(void) logMsg( "status: %d dhsMessage failed with status: %d",\
			st, stat2 );					\
	    }								\
	    return val;							\
	}
#define	checkRet( fn, st, val )						\
	fn;								\
	if ( st != DHS_S_SUCCESS )					\
	{								\
	    DHS_STATUS		stat2, errorNum;			\
	    DHS_ERR_LEVEL	errorLev;				\
	    char		*string;				\
	    stat2 = DHS_S_SUCCESS;					\
	    while ( stat2 == DHS_S_SUCCESS )				\
	    {								\
		string = dhsMessage( &errorNum, &errorLev, &stat2 );	\
		logMsg( "error checking function return:\n%s\n", #fn );	\
		if ( stat2 == DHS_S_SUCCESS )				\
		{							\
		    (void) logMsg( "status: %d message: %s\n", st, string );\
		}							\
		else if ( stat2 == DHS_S_NO_MESSAGE )			\
		{							\
		    stat2 = DHS_S_SUCCESS;				\
		    break;						\
		}							\
		else							\
		{							\
		    (void) logMsg( 					\
			    "status: %d dhsMessage failed with status: %d",\
			    st, stat2 );				\
		}							\
		dhsMessageClear( &stat2 );				\
	    }								\
	    if ( connect1 != DHS_CONNECT_NULL )				\
	    {								\
		dhsDisconnect( connect1, &stat2 );			\
		connect1 = DHS_CONNECT_NULL;				\
	    }								\
	    if ( connect2 != DHS_CONNECT_NULL )				\
	    {								\
		dhsDisconnect( connect2, &stat2 );			\
	    	connect2 = DHS_CONNECT_NULL;				\
	    }								\
	    dhsExit( &stat2 );						\
	    return val;							\
	}
static int	cc = 0;
static pthread_mutex_t	ccMutex;
DHS_BOOLEAN	isZero
(
    void	*arg
)
{
    boolean	resp;

    pthread_mutex_lock( &ccMutex );
    resp = ( *(int *) arg == 0 );
    pthread_mutex_unlock( &ccMutex );
    return( resp );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * on_signal
 *
 * INVOCATION:
 * (void) signal( sig, on_signal );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) sig (int) The signal number that caused the function to be executed.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Signal handler to shut down the DHS library event loop.
 *
 * DESCRIPTION:
 * Use the dhsEventLoopEnd function to shut down the DHS library event loop.
 * This function should only be called in response to a signal.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library event loop should be running. The signal handler must
 * be set up with the signal function.
 *
 * DEFICIENCIES:
 * No error checking is done.
 *-
 ************************************************************************
 */

static void	on_signal
(
    int		sig		/* (in)  The signal sent.		*/
)
{
    DHS_STATUS	status;


    fprintf( stderr, "handling signal\n" );
    status = DHS_S_SUCCESS;
    (void) dhsEventLoopEnd( &status );
    (void) signal( sig, on_signal );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * errorCallback
 *
 * INVOCATION:
 * dhsCallbackSet( DHS_CBT_ERROR, errorCallback, &status ); 
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connection (DHS_CONNECT) DHS connection associated with the error.
 * (>) errorNum (DHS_STATUS) The error status value.
 * (>) errorLev (DHS_ERR_LEVEL) The error level of the error.
 * (>) msg (char *) The message string associated with the error.
 * (>) tag (DHS_TAG) The command tag associated with the error.
 * (>) userData (void *) The user data pointer of the command associated
 *			with the error.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Report an error from the DHS library
 *
 * This function is executed when an error occurs in the DHS library which
 * cannot be reported back to a calling function.
 *
 * DESCRIPTION:
 * The error is evaluated and appropriate action is taken.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The callback must be set up with the dhsCallbackSet function.
 *
 * DEFICIENCIES:
 * More evaluation of the status is required.
 *-
 ************************************************************************
 */

static void	errorCallback
( 
    DHS_CONNECT	connection,	/* Connection associated with the error.*/
    DHS_STATUS	errorNum,	/* Error number.			*/
    DHS_ERR_LEVEL		/* Error level.				*/
		errorLev,
    char	*msg,		/* The error message string.		*/
    DHS_TAG	tag,		/* Command tag for the error.		*/
    void	*userData	/* User data pointer for the command.	*/
)
{
    DHS_STATUS	status;

    status = DHS_S_SUCCESS;
    if ( errorNum == DHS_E_CON_LOST )
    {
	(void) logMsg( "%s\n", msg );

	dhsDisconnect( connection, &status );
    }
    else
    {
	(void) logMsg( "%s\n", msg );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * responseCallback
 *
 * INVOCATION:
 * dhsCallbackSet( DHS_CBT_RESPONSE, responseCallback, &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connection (DHS_CONNECT) Connection command was sent to.
 * (>) tag (DHS_TAG) The command tag.
 * (>) command (char *) The name of the command.
 * (>) cmdStatus (DHS_CMD_STATUS) The current command status.
 * (>) string (char *) The message string sent with the response.
 * (>) avList (DHS_AV_LIST) The response attribute value list.
 * (>) userData (void *) The user data pointer associated with the command.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Handle the response to a command
 *
 * This function is called to notify a client that a response to a command
 * has been received. This version of the function simply displays various
 * information about the response.
 *
 * DESCRIPTION:
 * Display various information.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * This callback function must be set up with a call to the dhsCallbackSet
 * function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	responseCallback
(
    DHS_CONNECT	connection,	/* Connection to the server.		*/
    DHS_TAG	tag,		/* The command tag.			*/
    char	*command,	/* The command name.			*/
    DHS_CMD_STATUS		/* The status sent with the response.	*/
		cmdStatus,
    char	*string,	/* Status string sent with the response.*/
    DHS_AV_LIST	avList,		/* Response av list.			*/
    void	*userData	/* Command user data pointer.		*/
)
{
    StatusType	sdsStatus;	/* Sds function return status.		*/
    DHS_STATUS	status;		/* DHS function return status.		*/


    logMsg( "in response callback\n" );


    /* 
     *  Initialize the return status variables.
     */

    sdsStatus = SDS__OK;
    status = DHS_S_SUCCESS;


    /*
     *  Display the attribute value list.
     */

    SdsList( avList, &sdsStatus );


    /*
     *  Print the user data pointer value.
     */

    logMsg( "User data pointer: %0x\n", 
	    dhsConUserDataGet( connection, &status ) );

    if ( dhsTagDone( tag, &status ) && strncmp( command, "test command2", 
	    strlen( "test command2" ) ) == 0 )
    {
	pthread_mutex_lock( &ccMutex );
	cc --;
	pthread_mutex_unlock( &ccMutex );
    }


    if ( dhsTagDone( tag, &status ) && userData != NULL && 
	    strcmp( (char *) userData, "freeMe" ) == 0 )
    {
	dhsTagFree( tag, &status );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * getCallback
 *
 * INVOCATION:
 * dhsCallbackSet( DHS_CBT_GET, getCallback, &status ); 
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connection (DHS_CONNECT) Connection to the server.
 * (>) tag (DHS_TAG) Request tag.
 * (>) label (char *) Data label requested.
 * (>) type (DHS_BD_GET_TYPE) The form requested for the data.
 * (>) cmdStatus (DHS_CMD_STATUS) The status sent with the response.
 * (>) string (char *) The status string sent with the response.
 * (>) avList (DHS_AV_LIST) Attributes sent with the response.
 * (>) userData (void *) User data pointer associated with the request.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Notify client that a response to a dhsBdGet request is received
 *
 * This function is executed whenever a response to a dhsBdGet function
 * is received.
 *
 * DESCRIPTION:
 * Display the data.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The callback must be setup with the dhsCallbackSet function.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

static void	getCallback
(
    DHS_CONNECT	connection,	/* Connection to the server.		*/
    DHS_TAG	tag,		/* Request tag.				*/
    char	*label,		/* Data label requested.		*/
    DHS_BD_GET_TYPE		/* The form requested for the data.	*/
		type,
    DHS_CMD_STATUS		/* The status sent with the response.	*/
		cmdStatus,
    char	*string,	/* Status string sent with the response.*/
    DHS_AV_LIST	avList,		/* Attributes sent with the response.	*/
    void	*data,		/* Pointer to the data buffer.		*/
    unsigned long	
		length,		/* length of the data buffer.		*/
    void	*userData	/* User data pointer associated with the*/
				/* request.				*/
)
{
    StatusType	sdsStatus;	/* SDS function return status.		*/
    DHS_STATUS	status;		/* DHS function return status.		*/
    FILE	*fp;		/* File pointer to write output.	*/

    logMsg( "in get callback\n" );


    /* 
     *  Initialize the function return status variables.
     */

    sdsStatus = SDS__OK;
    status = DHS_S_SUCCESS;


    /* 
     *  Display the response attribute value list.
     */

    if ( avList != DHS_AV_LIST_NULL )
    {
	SdsList( avList, &sdsStatus );
    }


    /*
     *  Print the user data pointer.
     */

    logMsg( "User data pointer: %0x\n", userData );

    if ( data != NULL && length != 0 )
    {
	fp = fopen( label, "wb" );
	fwrite( data, length, 1, fp );
	fclose( fp );
    }
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * createAvList
 *
 * INVOCATION:
 * createAvList( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (!) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_AV_LIST) The attribute value list created.
 *
 * PURPOSE:
 * Create a new attribute value list
 *
 * DESCRIPTION:
 * Use the DHS library functions to create a new attribute value list.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library must have been initialized.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_AV_LIST	createAvList
(
    DHS_STATUS	*status		/* The function return status.		*/
)
{
    DHS_AV_LIST	avList;		/* The new attribute value list.	*/
    DHS_AV_LIST	avList2;	/* Second av list.			*/
    static boolean
		boolArray[2][3] = {
			{ TRUE, FALSE, TRUE },
			{ FALSE, TRUE, FALSE } };
    static char	charArray[5][6] = { 
			{ 'A', 'B', 'C', 'D', 'E', 'F' },
			{ 'G', 'H', 'I', 'J', 'K', 'L' },
			{ 'M', 'N', 'O', 'P', 'Q', 'R' },
			{ 'S', 'T', 'U', 'V', 'W', 'X' },
			{ 'Y', 'Z', 'a', 'b', 'c', 'd' } };
    unsigned long 	
		dims[2];	/* Array dimensions.			*/
    static double
		doubleArray[5][6] = {
			{ 0., -1., 2., -3., 4., -5. },
			{ 21.0, -21.1, 21.2, -21.3, 21.4, -21.5 },
			{ 20.20, -20.21, 20.22, -20.23, 20.24, -20.25 },
			{ 20.030, -20.031, 20.032, -20.033, 20.034, -20.035 },
			{ 200.040, -200.041, 200.042, -200.043, 200.044, 
				-200.045 } };
    static float
		floatArray[5][6] = {
			{ 0., -1., 2., -3., 4., -5. },
			{ 21.0, -21.1, 21.2, -21.3, 21.4, -21.5 },
			{ 20.20, -20.21, 20.22, -20.23, 20.24, -20.25 },
			{ 20.030, -20.031, 20.032, -20.033, 20.034, -20.035 },
			{ 200.040, -200.041, 200.042, -200.043, 200.044, 
				-200.045 } };
    static char	int8Array[5][6] = {
			{ 0, -1, 2, -3, 4, -5 },
			{ 10, -11, 12, -13, 14, -15 },
			{ 20, -21, 22, -23, 24, -25 },
			{ 30, -31, 32, -33, 34, -35 },
			{ 40, -41, 42, -43, 44, -45 } };
    static int	intArray[5][6] = {
			{ 0, -1, 2, -3, 4, -5 },
			{ 210, -211, 212, -213, 214, -215 },
			{ 2020, -2021, 2022, -2023, 2024, -2025 },
			{ 20030, -20031, 20032, -20033, 20034, -20035 },
			{ 200040, -200041, 200042, -200043, 200044, -200045 } };
    StatusType	sdsStatus;	/* Sds function return status.		*/
    static short
		shortArray[5][6] = {
			{ 0, -1, 2, -3, 4, -5 },
			{ 210, -211, 212, -213, 214, -215 },
			{ 2020, -2021, 2022, -2023, 2024, -2025 },
			{ 20030, -20031, 20032, -20033, 20034, -20035 },
			{ 40, -41, 42, -43, 44, -45 } };
    static char	*stringArray[2][3] = { 
			{ "string zero, zero", "string zero, one", 
				"string zero, two" },
			{ "string one, zero", "string one, one", 
				"string one, two" } };
    static unsigned char
		uint8Array[5][6] = {
			{ 0, 1, 2, 3, 4, 5 },
			{ 10, 11, 12, 13, 14, 15 },
			{ 20, 21, 22, 23, 24, 25 },
			{ 30, 31, 32, 33, 34, 35 },
			{ 240, 241, 242, 243, 244, 245 } };
    static unsigned int	
		uintArray[5][6] = {
			{ 0, 1, 2, 3, 4, 5 },
			{ 210, 211, 212, 213, 214, 215 },
			{ 2020, 2021, 2022, 2023, 2024, 2025 },
			{ 20030, 20031, 20032, 20033, 20034, 20035 },
			{ 3000000040ul, 3000000041ul, 3000000042ul, 
				3000000043ul, 3000000044ul, 3000000045ul } };
    static unsigned short
		ushortArray[5][6] = {
			{ 0, 1, 2, 3, 4, 5 },
			{ 210, 211, 212, 213, 214, 215 },
			{ 2020, 2021, 2022, 2023, 2024, 2025 },
			{ 20030, 20031, 20032, 20033, 20034, 20035 },
			{ 40240, 40241, 40242, 40243, 40244, 40245 } };
    DHS_AV_ID		id;


    sdsStatus = SDS__OK;


    /*
     *  Create a new attribute value list.
     */

    check( avList = dhsAvListNew( status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add a character to the attribute value list.
     */

    check( dhsAvAdd( avList, 
	    "charScalar1234561111111111111111111111111111111111111121", 
	    DHS_DT_CHAR, 0, NULL, 'C', status ), *status, DHS_AV_LIST_NULL );
    check( dhsAvAdd( avList, 
	    "charScalar1234561111111111111111111111111111111111111111", 
	    DHS_DT_CHAR, 0, NULL, 'C', status ), *status, DHS_AV_LIST_NULL );
    id = dhsAvFind( avList, 
	    "charScalar1234561111111111111111111111111111111111111111", 
	    status );


    /*
     *  Add a character array to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "charArray", DHS_DT_CHAR, 2, dims, charArray, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an INT8 value to the attribute value list.
     */

    check( dhsAvAdd( avList, "int8Scalar", DHS_DT_INT8, 0, NULL, 99,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add a INT8 array to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "int8Array", DHS_DT_INT8, 2, dims, int8Array, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an unsigned INT8 value to the attribute value list.
     */

    check( dhsAvAdd( avList, "uint8Scalar", DHS_DT_UINT8, 0, NULL, 99,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add a unsigned INT8 array to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "uint8Array", DHS_DT_UINT8, 2, dims, uint8Array, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an short scalar value to the attribute value list.
     */

    check( dhsAvAdd( avList, "int16Scalar", DHS_DT_INT16, 0, NULL, (short) 999, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an array of shorts to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "int16Array", DHS_DT_INT16, 2, dims, shortArray,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an unsigned short scalar value to the attribute value list.
     */

    check( dhsAvAdd( avList, "uint16Scalar", DHS_DT_UINT16, 0, NULL, 
	    (short) 999, status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an array of unsigned shorts to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "uint16Array", DHS_DT_UINT16, 2, dims, ushortArray,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an int scalar value to the attribute value list.
     */

    check( dhsAvAdd( avList, "int32Scalar", DHS_DT_INT32, 0, NULL, 999, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an array of ints to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "int32Array", DHS_DT_INT32, 2, dims, intArray,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an unsigned int scalar value to the attribute value list.
     */

    check( dhsAvAdd( avList, "uint32Scalar", DHS_DT_UINT32, 0, NULL, 999, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an array of ints to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "uint32Array", DHS_DT_UINT32, 2, dims, uintArray,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add float scalar value to the attribute value list.
     */

    check( dhsAvAdd( avList, "floatScalar", DHS_DT_FLOAT, 0, NULL, 
	    (float) 999.99, status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an array of floats to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "floatArray", DHS_DT_FLOAT, 2, dims, floatArray,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add double scalar value to the attribute value list.
     */

    check( dhsAvAdd( avList, "floatScalar", DHS_DT_FLOAT, 0, NULL, 999.99, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an array of doubles to the attribute value list.
     */

    dims[0] = 5;
    dims[1] = 6;
    check( dhsAvAdd( avList, "doubleArray", DHS_DT_DOUBLE, 2, dims, doubleArray,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *   Add a string to the av list.
     */

    check( dhsAvAdd( avList, "StringScalar", DHS_DT_STRING, 0, NULL, 
	    "TestString", status ), *status, DHS_AV_LIST_NULL );


    /*
     *   Add an array of strings to the av list.
     */

    dims[0] = 2;
    dims[1] = 3;
    check( dhsAvAdd( avList, "StringArray", DHS_DT_STRING, 2, dims, 
	    stringArray, status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add a boolean value to the av list.
     */

    check( dhsAvAdd( avList, "boolScalar", DHS_DT_BOOLEAN, 0, NULL, FALSE,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add a boolean array to the attribute value list.
     */

    dims[0] = 2;
    dims[1] = 3;
    check( dhsAvAdd( avList, "boolArray", DHS_DT_BOOLEAN, 2, dims, boolArray, 
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Add an avlist to an avlist.
     */

    check( avList2 = dhsAvListNew( status ), *status, DHS_AV_LIST_NULL );
    check( dhsAvAdd( avList2, "StringScalar", DHS_DT_STRING, 0, NULL, 
	    "nested avlist", status ), *status, DHS_AV_LIST_NULL );
    check( dhsAvAdd( avList, "avListScalar", DHS_DT_AVLIST, 0, NULL, avList2,
	    status ), *status, DHS_AV_LIST_NULL );


    /*
     *  Display the attribute value list.
     */

    SdsList( (SdsIdType) avList, &sdsStatus );

    return( avList );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * createDataset
 *
 * INVOCATION:
 * dataset = createDataset( &status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) status (DHS_STATUS *) Function return status.
 *
 * FUNCTION VALUE:
 * (DHS_BD_DATASET) The dataset created.
 *
 * PURPOSE:
 * Create a new dataset structure.
 *
 * DESCRIPTION:
 * Use the DHS library function to create a new dataset.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * The DHS library should be initialized.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

DHS_BD_DATASET	createDataset
(
    DHS_STATUS	*status		/* Function return status.		*/
)
{
    DHS_AV_LIST	av;
    DHS_BD_DATASET
		dataset;	/* The new dataset.			*/
    unsigned long	
		dims[2];	/* The array dimensions.		*/
    static float
		floatArray[5][6] = {
			{ 0., -1., 2., -3., 4., -5. },
			{ 21.0, -21.1, 21.2, -21.3, 21.4, -21.5 },
			{ 20.20, -20.21, 20.22, -20.23, 20.24, -20.25 },
			{ 20.030, -20.031, 20.032, -20.033, 20.034, -20.035 },
			{ 200.040, -200.041, 200.042, -200.043, 200.044, 
				-200.045 } };
    DHS_BD_FRAME
		frame;		/* Frame to add to the dataset.		*/
    DHS_BD_FRAME
		frame2;		/* Frame to add to the dataset.		*/
    int		i;
    int		j;
    int		*pData1;	/* Pointer to the integer data array.	*/
    int		*pData2;	/* Pointer to the integer data array.	*/
    StatusType	sdsStatus;	/* Sds function return status.		*/


    sdsStatus = SDS__OK;


    /* 
     *  Create the dataset.
     */

    check( dataset = dhsBdDsNew( status ), *status, DHS_BD_DATASET_NULL );

    dims[0] = 500;
    dims[1] = 500;
    check( frame = dhsBdFrameNew( dataset, "frame 1", 1, DHS_DT_INT32, 2, dims, 
	    (const void **) &pData1, status ), *status, DHS_BD_DATASET_NULL );

    
    /*
     *  Add a second frame to the dataset.
     */

    logMsg( "new frame\n" );
    check( frame2 = dhsBdFrameNew( frame, "frame 2", 2, DHS_DT_INT32, 2, dims, 
	    (const void **) &pData2, status ), *status, DHS_BD_DATASET_NULL );
    av = dhsAvIndex( frame2, 0, status );
    av = dhsAvIndex( frame, 0, status );

    /* 
     *  Add a new frame to the dataset which will contain a 100x200 
     *  int 32 data array.
     */

    logMsg( "new frame\n" );
    check( frame = dhsBdFrameNew( dataset, "frame 1", 1, DHS_DT_INT32, 2, dims, 
	    (const void **) &pData1, status ), *status, DHS_BD_DATASET_NULL );

    
    /*
     *  Add a second frame to the dataset.
     */

    logMsg( "new frame\n" );
    check( frame = dhsBdFrameNew( dataset, "frame 2", 2, DHS_DT_INT32, 2, dims, 
	    (const void **) &pData2, status ), *status, DHS_BD_DATASET_NULL );
    av = dhsAvIndex( frame, 0, status );


    /*
     *  Add an array of floats to the frame as an attribute.
     */

    dims[0] = 5;
    dims[1] = 6;

    check( dhsBdAttribAdd( frame, "floatArray", DHS_DT_FLOAT, 2, dims, 
	    floatArray, status ), *status, DHS_BD_DATASET_NULL );


    /* 
     *  fill in the data array.
     */

    logMsg( "fill frame\n" );
    for ( i = 0; i < dims[0]; i++ )
    {
	for ( j = 0; j < dims[1]; j++ )
	{
	    pData1[ i * 200 + j ] = i * 200 + j;
	    pData2[ i * 200 + j ] = i * 200 + j;
	}
    }


    /* 
     * Display the dataset structure.
     */

    SdsList( (SdsIdType) dataset, &sdsStatus );

    return( dataset );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * main
 *
 * INVOCATION:
 * From command line.
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) argc (int) Number of command line args.
 * (>) argv (char *[]) List of command line args.
 *
 * FUNCTION VALUE:
 * (int) Return status.
 *
 * PURPOSE:
 * Main routine to exercise the DHS library from a client point of view
 *
 * DESCRIPTION:
 * Use all client DHS library functions.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * DEFICIENCIES:
 * Error checking could be better.
 *-
 ************************************************************************
 */

#ifdef VxWorks
int		dhsClient
(
    char *argClient,
    char *argHost,
    char *argName
)
{
#else	/* !VxWorks */
int		main
(
    int		argc,		/* Number of command line args.		*/
    char	*argv[]		/* List of command line args.		*/
)
{
    char *argClient;
    char *argHost;
    char *argName;
#endif	/* VxWorks */

    DHS_AV_ID av;
    DHS_AV_LIST	avList;		/* Attribute value list for commands.	*/
    DHS_CONNECT	connect1;	/* Connection to a server.		*/
    DHS_CONNECT	connect2;	/* Connection to a server.		*/
    char	*contrib[] = 	/* List of contributors to the dataset.	*/
			{ "first contrib", "second contrib" };
    char	*dataLabel;	/* Name of a data label retrieved.	*/
    DHS_BD_FRAME
		frame;		/* Frame from the dataset.		*/
    int		i;
    int		*pData;		/* Pointer to the integer data array.	*/
    char	*qlStreams[] = 	/* Quick look streams to get the data.	*/
			{ "first stream", "second stream" };
    DHS_AV_LIST	response;	/* The response to a command.		*/
    char	*serverAddress;	/* Address of the server.		*/
    char	*serverName;	/* Name of the server.			*/
    DHS_STATUS	status;		/* DHS library function return status.	*/
    char	*statusMsg;	/* Status message with a response.	*/
    DHS_TAG	tags[3];	/* List of command tags.		*/
    DHS_THREAD	thread;		/* Id of event loops thread.		*/
    char	command[80];
    unsigned long	length;
    void	*getData;
    DHS_BD_GET_TYPE
    		type;


    StatusType		sdsStatus;
    DHS_BD_DATASET	dataset;
    int			j;
    DHS_CMD_STATUS	cmdStatus;
    void		*userData;

    connect1 = DHS_CONNECT_NULL;
    connect2 = DHS_CONNECT_NULL;


    sdsStatus = SDS__OK;
    pthread_mutex_init( &ccMutex, NULL );


#ifndef VxWorks
    if ( argc != 4 && argc != 6 )
    {
	logMsg( "usage: dhsClient <name> <serverHost> <serverName> "
		"[<server2> <serverName2>]\n" );
	exit( -1 );
    }
    else
    {
	argClient = argv[1];
	argHost = argv[2];
	argName = argv[3];
    }
#endif


    (void) signal( SIGINT, on_signal );
    (void) signal( SIGTERM, on_signal );
    
    status = DHS_S_SUCCESS;
    checkRet( dhsInit( argClient, 102*1024, &status ), status, -1 );
    /* checkRet( dhsDebugLevel( DHS_DEBUG_FULL, &status ), status, -23 ); */
    checkRet( dhsCallbackSet( DHS_CBT_ERROR, errorCallback, &status ), 
	    status, -2 );
    checkRet( dhsCallbackSet( DHS_CBT_RESPONSE, responseCallback, &status ), 
	    status, -2 );
    checkRet( dhsCallbackSet( DHS_CBT_GET, getCallback, &status ), 
	    status, -2 );

    checkRet( dhsEventLoop( DHS_ELT_THREADED, &thread, &status ), status, -3 );

    
    /*
     *  connect to a server.
     */

    checkRet( connect1 = dhsConnect( argHost, argName, &i, &status ), 
	    status, -4 );
    checkRet( connect2 = dhsConnect( argHost, argName, &i, &status ), 
	    status, -4 );
    

    /*
     *  Get and display information about the connection.
     */

    checkRet( dhsConnectInfo( connect1, &serverAddress, &serverName, &status ), 
	    status, -5 );
    logMsg( "connection info: %s %s\n", serverAddress, serverName );


    /* 
     *  Create an attribute value list containing all possible options.
     */

    checkRet( avList = createAvList( &status ), status, -6 );


    /*
     *  Send the command to the server.
     */

    checkRet( tags[0] = dhsApply( connect1, "test Command1", avList, NULL, 
	    &status ), status, -9 );


    /*
     *  Send another command to the server, reusing the avlist.
     */

    checkRet( tags[1] = dhsApply( connect2, "test Command1", avList, NULL, 
	    &status ), status, -9 );


    /*
     *  Send an unknown command to the server, reusing the avlist.
     */

    checkRet( tags[2] = dhsApply( connect2, "unknown command", avList, NULL, 
	    &status ), status, -9 );


    /*
     *  Request and display the current status of the first command.
     */

    checkRet( cmdStatus = dhsStatus( tags[0], &statusMsg, &status ), status, -12 );
    if ( statusMsg != NULL )
    {
	(void) logMsg( "command status: %d, %s\n", cmdStatus, statusMsg );
    }


    /*
     *  Wait for all commands to complete.
     */

    checkRet( dhsWait( 3, tags, &status ), status, -13 );


    /*
     *  Request and display the current status of the first command.
     */

    checkRet( cmdStatus = dhsStatus( tags[0], &statusMsg, &status ), status, -12 );
    if ( statusMsg != NULL )
    {
	(void) logMsg( "command status: %d, %s\n", cmdStatus, statusMsg );
    }


    /* 
     * checkRet all responses sent for the first command.
     */

    for ( i = 0; status == DHS_S_SUCCESS; i++ )
    {
	/*
	 *  Get a response.
	 */

	response = dhsResponseGet( tags[0], i, &status );

	if ( status == DHS_S_NO_RESP )
	{
	    /*
	     *  No more responses to look at, so exit the loop.
	     */

	    status = DHS_S_SUCCESS;
	    break;
	}
	else if ( status == DHS_S_SUCCESS )
	{
	    /*
	     * If the attribute value list isn't null, display it.
	     */

	    if ( response != DHS_AV_LIST_NULL )
	    {
		SdsList( (SdsIdType) response, &sdsStatus );
	    }
	}
	else
	{
	    /*
	     *  An error must have occured, so exit the loop.
	     */

	    break;
	}
    }


    /*
     *  Get the user data pointer for the first command.
     */

    checkRet( userData = dhsUserDataGet( tags[0], &status ), status, -14 );


    /*
     *  Free the command tags.
     */

    checkRet( dhsTagFree( tags[0], &status ), status, -13 );
    checkRet( dhsTagFree( tags[1], &status ), status, -13 );
    checkRet( dhsTagFree( tags[2], &status ), status, -13 );


    /* 
     *  Free the attribute value list.
     */

    checkRet( dhsAvListFree( avList, &status), status, -6 );


    /*
     *  Create a new dataset.
     */

    checkRet( dataset = createDataset( &status ), status, -15 );

    checkRet( frame = dhsBdFrameFind( dataset, "frame 2", &status ), status, -17 );

    av = dhsAvIndex( frame, 0, &status );
    av = dhsAvIndex( dataset, 0, &status );
    for ( i = 1;; i++ )
    {
	frame = dhsBdFrameIndex( dataset, i, &status );
	if ( status == DHS_S_SUCCESS )
	{
	    char	*name;
	    DHS_DATA_TYPE type;
	    int		ndims;
	    unsigned long dims[7];

	    SdsList( (SdsIdType) frame, &sdsStatus );
	    checkRet( dhsBdFrameInfo( frame, &name, &type, &ndims, dims, 
		    (void *) &pData, &status ), status, -18 )
	    (void) logMsg( "name: %s, type: %d, ndims: %d, dims: %d\n",
		    name, type, ndims, dims[0] );
	}
	else if ( status == DHS_S_NO_FRAME )
	{
	    status = DHS_S_SUCCESS;
	    break;
	}
	else
	{
	    DHS_STATUS		stat2, errorNum;
	    DHS_ERR_LEVEL	errorLev;
	    char		*string;
	    stat2 = DHS_S_SUCCESS;
	    string = dhsMessage( &errorNum, &errorLev, &stat2 );
	    if ( stat2 == DHS_S_SUCCESS )
	    {
		(void) logMsg( "status: %d message: %s\n", status, string );
	    }
	    else
	    {
		(void) logMsg( "status: %d dhsMessage failed with status: %d",
			status, stat2 );
	    }
	}
    }

    checkRet( dhsBdCtl( connect1, DHS_BD_CTL_ABORT, "label 1", &status ), 
	    status, -19 );
    checkRet( dhsBdCtl( connect2, DHS_BD_CTL_RESET, "label 2", &status ), 
	    status, -19 );

    checkRet( dataLabel = dhsBdName( connect1, &status ), status, -20 );

    for ( i = 0; i < 50; i++ )
    {
	checkRet( tags[0] = dhsBdPut( connect1, "data label", DHS_BD_PT_DS, 
		FALSE, dataset, NULL, &status ), status, -21 );

	checkRet( tags[1] = dhsBdPut( connect2, "data label 2", 
		DHS_BD_PT_RAW_UNIQUE, FALSE, "12345678901234567890", 21, 
		NULL, &status ), status, -21 );

	checkRet( dhsWait( 2, tags, &status ), status, -13 );

	checkRet( dhsTagFree( tags[0], &status ), status, -13 );
	checkRet( dhsTagFree( tags[1], &status ), status, -13 );
    }

    checkRet( dhsBdDsFree( dataset, &status ), status, -16 );


    checkRet( tags[0] = dhsBdGet( connect1, "dataLabel1", DHS_BD_GT_FITS, 
	    NULL, &status ), status, -22 );


    checkRet( tags[1] = dhsBdGet( connect2, "dataLabel2", DHS_BD_GT_RAW, 
	    NULL, &status ), status, -22 );

    checkRet( dhsWait( 2, tags, &status ), status, -13 );

    checkRet( dhsTagFree( tags[0], &status ), status, -13 );
    checkRet( dhsTagFree( tags[1], &status ), status, -13 );


    /*
     *  Try some high speed commands.
     */

    checkRet( avList = dhsAvListNew( &status ), status, DHS_AV_LIST_NULL );
    checkRet( dhsAvAdd( avList, "delay", DHS_DT_INT32, 0, NULL, 0,
	    &status ), status, -13 );
    checkRet( tags[0] = dhsApply( connect1, "set delay", avList, NULL, 
	    &status ), status, -9 );
    checkRet( dhsWait( 1, tags, &status ), status, -13 );
    checkRet( dhsTagFree( tags[0], &status ), status, -13 );

    for ( i = 0; i < 100; i++ )
    {
	pthread_mutex_lock( &ccMutex );
	cc ++;
	pthread_mutex_unlock( &ccMutex );
	sprintf( command, "test command2 %d", i );
	checkRet( tags[0] = dhsApply( connect1, command, avList, "freeMe", 
		&status ), status, -9 );
    }
    checkRet( dhsAvListFree( avList, &status), status, -6 );
    checkRet( avList = dhsAvListNew( &status ), status, DHS_AV_LIST_NULL );
    checkRet( dhsAvAdd( avList, "delay", DHS_DT_INT32, 0, NULL, 1,
	    &status ), status, -13 );
    checkRet( tags[0] = dhsApply( connect1, "set delay", avList, NULL, 
	    &status ), status, -9 );
    checkRet( dhsWait( 1, tags, &status ), status, -13 );
    checkRet( dhsTagFree( tags[0], &status ), status, -13 );

    for ( i = 100; i < 200; i++ )
    {
	sprintf( command, "test command2 %d", i );
	pthread_mutex_lock( &ccMutex );
	cc ++;
	pthread_mutex_unlock( &ccMutex );
	checkRet( tags[0] = dhsApply( connect1, command, avList, "freeMe", 
		&status ), status, -9 );
    }
    checkRet( dhsAvListFree( avList, &status), status, -6 );

    checkRet( dhsEventLoop( DHS_ELT_COND, isZero, &cc, &status ), status, -3 );

    /*
    sleep( 30 );
    */
    
    checkRet( dhsDisconnect( connect1, &status ), status, -8 );
    connect1 = DHS_CONNECT_NULL;
    if ( connect2 != DHS_CONNECT_NULL )
    {
	checkRet( dhsDisconnect( connect2, &status ), status, -8 );
	connect2 = DHS_CONNECT_NULL;
    }

    checkRet( dhsExit( &status ), status, -3  );


    return( status );
}
