#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "sds.h"

#include "gen_types.h"
#include "gen_str.h"

#include "dhs.h"

typedef struct cmdStruct
{
    DHS_CONNECT		connection;
    DHS_TAG		tag;
    char		*command;
    DHS_AV_LIST		avList;
} tCmdStruct;

void	*handleCmd( void * );
boolean	strict = TRUE;

#define	VOID
#define	check( fn, status, val )					\
	fn;								\
	if ( status != DHS_S_SUCCESS )					\
	{								\
	    DHS_STATUS		stat2, errorNum;			\
	    DHS_ERR_LEVEL	errorLev;				\
	    char		*string;				\
	    (void) printf( "function: %s\n", #fn );			\
	    stat2 = DHS_S_SUCCESS;					\
	    for ( ; stat2 == DHS_S_SUCCESS; )				\
	    {								\
		string = dhsMessage( &errorNum, &errorLev, &stat2 );	\
		if ( stat2 == DHS_S_SUCCESS )				\
		{							\
		    (void) printf( "status: %d message: %s\n", 		\
			    status, string );				\
		    dhsMessageClear( &stat2 );				\
		}							\
		else if ( stat2 != DHS_S_NO_MESSAGE )			\
		{							\
		    (void) printf( 					\
			    "status: %d dhsMessage failed with status: %d\n",\
			    status, stat2 );				\
		}							\
	    }								\
	    return val;							\
	}


/*+
************************************************************************
*
*   Function:	name
*
*   Purpose:
*	Statement of purpose.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

static void	on_signal
(
    int		sig		/* (in)  The signal sent.		*/
)
{
    DHS_STATUS	status;
    DHS_STATUS	stat2;
    DHS_STATUS		errorNum;
    DHS_ERR_LEVEL	errorLev;
    char		*string;


    fprintf( stderr, "handling signal\n" );
    status = DHS_S_SUCCESS;
    (void) dhsEventLoopEnd( &status );
    if ( status != DHS_S_SUCCESS )
    {
	for ( stat2 = DHS_S_SUCCESS; stat2 == DHS_S_SUCCESS; )
	{
	    string = dhsMessage( &errorNum, &errorLev, &stat2 );
	    if ( stat2 == DHS_S_SUCCESS )
	    {
		(void) printf( "status: %d message: %s\n", status, string );
		dhsMessageClear( &stat2 );
	    }
	    else if ( stat2 != DHS_S_NO_MESSAGE )
	    {
		(void) printf( "status: %d dhsMessage failed with status: %d\n",
			status, stat2 );
	    }
	}
    }
    (void) signal( sig, on_signal );
}
void		ctlCommand
( 
    DHS_CONNECT		connection,
    DHS_TAG		tag,
    DHS_AV_LIST		avList
)
{
    DHS_AV_ID		av;
    DHS_STATUS		status;
    DHS_BD_CTL		*ctl;
    DHS_AV_LIST		responseAvList;

    status = DHS_S_SUCCESS;
    av = dhsAvFind( avList, "ctl", &status );
    dhsAvInfo( av, NULL, NULL, NULL, NULL, &ctl, &status );

    if ( *ctl == DHS_BD_CTL_GETNAME )
    {
	responseAvList = dhsAvListNew( &status );
	dhsAvAdd( responseAvList, "dataLabel", DHS_DT_STRING, 0, NULL, 
		"new Label",  &status );
    }
    else
    {
	responseAvList = DHS_AV_LIST_NULL;
    }
    dhsCmdResponse( connection, tag, DHS_CS_DONE, "done", responseAvList, 
	    &status );
    dhsAvListFree( responseAvList, &status );
}

/*+
************************************************************************
*
*   Function:	name
*
*   Purpose:
*	Statement of purpose.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

void		commandCallback
( 
    DHS_CONNECT		connection,
    DHS_TAG		tag,
    char		*command,
    DHS_AV_LIST		avList
)
{
    DHS_STATUS		status;
    tCmdStruct		*pCmdStruct;
    char		*s1;
    char		*s2;
    StatusType		sdsStatus;
    pthread_t		thread;
    

    status = DHS_S_SUCCESS;
    sdsStatus = SDS__OK;
    pCmdStruct = malloc( sizeof( tCmdStruct ) );
    pCmdStruct->connection = connection;
    pCmdStruct->tag = tag;
    printf( "Received `%s' command.\n", command );

    if ( command == NULL )
    {
	pCmdStruct->command = NULL;
    }
    else
    {
	pCmdStruct->command = strsav( command );
    }


    status = DHS_S_SUCCESS;
    pCmdStruct->avList = dhsAvListCopy( avList, &status );


    dhsConnectInfo( connection, &s1, &s2, &status );
    if ( status == DHS_S_SUCCESS )
    {
	printf( "command: '%s'\nconnection info: %s %s\n", command, s1, s2 );
    }
    /* (void) dhsEventLoopEnd( &status ); */


#ifdef	HPUX
    pthread_create( &thread, pthread_attr_default, handleCmd, pCmdStruct );
    pthread_detach( &thread );
#else	/* HPUX */
    pthread_create( &thread, NULL, handleCmd, pCmdStruct );
    pthread_detach( thread );
#endif /* HPUX */
}
void 	*handleCmd
(
    void	*arg
)
{
    StatusType		sdsStatus;
    DHS_STATUS		status;
    DHS_AV_LIST		responseAvList;
    DHS_AV_ID		av;
    int			i;
    int			j;
    int			k;
    int			ndims;
    unsigned long	dims[7];
    void		*value;
    char		**charValue;
    char		*name;
    DHS_DATA_TYPE	type;
    static int		delay = 0;
    int			*pDelay;
    tCmdStruct		*pCmdStruct;
    static 		count = 0;
    
    sdsStatus = SDS__OK;
    status = DHS_S_SUCCESS;
    pCmdStruct = (tCmdStruct *) arg;

    count ++;
    printf( "handling %d\n", count );
    SdsList( pCmdStruct->avList, &sdsStatus );

    if ( strcmp( pCmdStruct->command, "bdCtl" ) == 0 )
    { 
	ctlCommand( pCmdStruct->connection, pCmdStruct->tag, 
		pCmdStruct->avList );
    }
    else if( strcmp( pCmdStruct->command, "set delay" ) == 0 )
    {
	av = dhsAvFind( pCmdStruct->avList, "delay", &status );
	dhsAvInfo( av, NULL, NULL, NULL, NULL, (void **) &pDelay, &status );
	delay = *pDelay;
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, 
		DHS_CS_DONE, pCmdStruct->command, DHS_AV_LIST_NULL, &status );
    }
    else if ( strcmp( pCmdStruct->command, "flush" ) == 0 )
    {
	fflush( stdout );
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, 
		DHS_CS_DONE, "done", DHS_AV_LIST_NULL, &status );
    }
    else if ( strcmp( pCmdStruct->command, "datasetInfo" ) == 0 )
    {
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, 
		DHS_CS_BUSY, pCmdStruct->command, DHS_AV_LIST_NULL, &status );

	responseAvList = dhsAvListNew( &status );
	dhsAvAdd( responseAvList, "database", DHS_DT_STRING, 0, NULL, "",
		&status );
	dhsAvAdd( responseAvList, "password", DHS_DT_STRING, 0, NULL,
		"", &status );
	dhsAvAdd( responseAvList, "server", DHS_DT_STRING, 0, NULL,
		"", &status );
	dhsAvAdd( responseAvList, "userName", DHS_DT_STRING, 0, NULL,
		"", &status );
	dhsAvAdd( responseAvList, "isqlString", DHS_DT_STRING, 0, NULL,
		"", &status );
	dhsAvListPrint( responseAvList, &status );
	
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_DONE, 
		"dtsDatasetInfo command done", responseAvList, &status );
    }
    else if ( strcmp( pCmdStruct->command, "stoInfo" ) == 0 )
    {
	responseAvList = dhsAvListNew( &status );
	dhsAvAdd( responseAvList, "stoDataHeader", DHS_DT_STRING, 0, NULL,
"Test Data File   Request Id    Unit Name",	&status );
	dhsAvAdd( responseAvList, "stoData", DHS_DT_STRING, 0, NULL,
"test1                3         USER3_32\n\
test002              3         USER3_32\n\
fullTest.fits        7         USER6_01\n\
image2.fits          7         USER6_01\n\
ng4321.fits          7         USER6_01\n\
image23.fits         7         USER6_01\n\
m51.fits             7         USER6_01\n\
test23              10         GEMINI_0023\n\
test89              10         GEMINI_0023\n\
test984             10         GEMINI_0023\n\
test239             10         GEMINI_0023\n\
wvs293              10         GEMINI_0023\n\
test03w             10         GEMINI_0023\n\
focus1              10         GEMINI_0023\n\
focus2              10         GEMINI_0023\n\
focus3              10         GEMINI_0023\n", &status );
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_DONE, 
		"stoInfo command done", responseAvList, &status );
    }
    else if ( strcmp( pCmdStruct->command, "test Command1" ) == 0 )
    {
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_BUSY, 
		"1 response", DHS_AV_LIST_NULL, &status );
	av = dhsAvFind( pCmdStruct->avList, "StringArray", &status );
	dhsAvInfo( av, &name, &type, &ndims, dims, (void **) &charValue, &status );
	printf( "name: %s ndims: %d, dims: %d\n", 
		name, ndims, dims[0] );
	for ( i = 0, j = 1; i < ndims; i++ )
	{
	    j *= dims[i];
	}
	for ( i = 0; i < j; i++ )
	{
	    printf( "%s\n", *charValue );
	    charValue ++;
	}
	SdsList( av, &sdsStatus );
	for ( k = 0; k < 25; k++ )
	{
	    av = dhsAvIndex( pCmdStruct->avList, k, &status );
	    if ( status == DHS_S_NO_ATTRIB )
	    {
		status = DHS_S_SUCCESS;
		break;
	    }
	    dhsAvInfo( av, &name, &type, &ndims, dims, (void **) 
		    &value, &status );
	    printf( "name: %s ndims: %d, dims: %d\n", 
		    name, ndims, dims[0] );
	    SdsList( av, &sdsStatus );


	    /*
	     * Display the unsigned integer scalar data.
	     */

	    if ( strcmp( name, "uint32Scalar" ) == 0 )
	    {
		unsigned int	*uintdata;
		uintdata = value;

		printf( "uint scalar should be 999 with dhsClient: %u\n", *uintdata );
	    }


	    /*
	     * Display the unsigned integer array data.
	     */

	    if ( strcmp( name, "uint32Array" ) == 0 )
	    {
		unsigned int	*uintdata;
		uintdata = value;

		for ( i = 0; i < dims[0]; i++ )
		{
		    for ( j = 0; j < dims[1]; j++ )
		    {
			printf( "%d %d = %u\n", i, j, uintdata[j + i * dims[0]] );
		    }
		}
	    }
	}

	/*
	 *  Create a new attribute value list.
	 */

	responseAvList = dhsAvListNew( &status );


	/*
	 *  Add a character to the attribute value list.
	 */

	dhsAvAdd( responseAvList, "charScalar", DHS_DT_CHAR, 0, NULL, 'C',
		&status );

	
	printf( "set command state to busy\n" );
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_BUSY, 
		"1 response", responseAvList, &status );
	dhsAvListFree( responseAvList, &status );

	printf( "set command state to done\n" );


	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_DONE, 
		"command done", DHS_AV_LIST_NULL, &status );
    }
    else if ( strncmp( pCmdStruct->command, "test command2", 
	    strlen( "test command2" ) ) == 0 )
    {
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_BUSY, 
		pCmdStruct->command, DHS_AV_LIST_NULL, &status );
	if ( delay != 0 )
	{
	    printf( "sleeping\n" );
	    sleep( delay );
	}
	dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, DHS_CS_DONE, 
		pCmdStruct->command, DHS_AV_LIST_NULL, &status );
    }
    else
    {
	if ( strict )
	{
	    dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, 
		    DHS_CS_ERROR, "1 unknown command?", DHS_AV_LIST_NULL, &status );
	}
	else
	{
	    dhsCmdResponse( pCmdStruct->connection, pCmdStruct->tag, 
		    DHS_CS_DONE, "ok.", DHS_AV_LIST_NULL, &status );
	}
    }

    if ( pCmdStruct->avList != DHS_AV_LIST_NULL )
    {
	dhsAvListFree( pCmdStruct->avList, &status );
    }
    if ( pCmdStruct->command != NULL )
    {
	free( pCmdStruct->command );
    }
    free( pCmdStruct );

    count --;
    return( NULL );
}

/*+
************************************************************************
*
*   Function:	name
*
*   Purpose:
*	Statement of purpose.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

void		putCallback
(
    DHS_CONNECT	connect,	/* (in)  The client connection.		*/
    DHS_TAG	tag,		/* (in)  The client request tag.	*/
    char	*label,		/* (in)  The data label to get.		*/
    DHS_BD_PUT_TYPE
    		type,		/* (in)  Type of data to return.	*/
    boolean	last,		/* (in)  Is this the last of the data?	*/
    void	*data,		/* (in)  Pointer to the data buffer.	*/
    unsigned long	
		length		/* (ion) Length of the data buffer.	*/
)
{
    StatusType	sdsStatus;
    DHS_STATUS	status;
    SdsIdType	sdsId;
    DHS_BD_DATASET dataset;

    
    status = DHS_S_SUCCESS;

    sdsStatus = SDS__OK;

    if ( type == DHS_BD_PT_DS || type == DHS_BD_PT_DS_QL || 
	    type == DHS_BD_PT_SDS )
    {
	dataset = dhsBdDsAccess( data, &status );
	dhsBdDsPrint( dataset, &status );

	SdsWrite( dataset, "server.out", &sdsStatus );
	dhsBdDsFree( dataset, &status );
    }

    dhsCmdResponse( connect, tag, DHS_CS_DONE, "put done", 
	    DHS_AV_LIST_NULL, &status );
}

/*+
************************************************************************
*
*   Function:	name
*
*   Purpose:
*	Statement of purpose.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

void		getCallback
(
    DHS_CONNECT	connect,	/* (in)  The client connection.		*/
    DHS_TAG	tag,		/* (in)  The client request tag.	*/
    char	*label,		/* (in)  The data label to get.		*/
    DHS_BD_GET_TYPE
    		type		/* (in)  Type of data to return.	*/
)
{
    DHS_STATUS	status;
    char	response[] = "this is the response.";
    StatusType	sdsStatus;
    SdsIdType	sdsId;
    DHS_AV_LIST	responseAvList;

    
    status = DHS_S_SUCCESS;
    sdsStatus = SDS__OK;

    printf( "in response, type = %d\n", type );
    responseAvList = dhsAvListNew( &status );
    dhsBdResponse( connect, tag, DHS_CS_BUSY, "get starting", 
		responseAvList, DHS_BD_GT_RAW, NULL, 0, &status );
    dhsAvListFree( responseAvList, &status );
    switch( type )
    {
	case	DHS_BD_GT_FITS:
	case	DHS_BD_GT_FITS_ASIS:
	case	DHS_BD_GT_FITS_HEADER:
	case	DHS_BD_GT_RAW:
	case	DHS_BD_GT_RAW_ASIS:
	    dhsBdResponse( connect, tag, DHS_CS_DONE, "put done", 
		    DHS_AV_LIST_NULL, type, response, strlen( response ), 
		    &status );
    }
    if ( status != DHS_S_SUCCESS )
    {
	status = DHS_S_SUCCESS;
	dhsBdResponse( connect, tag, DHS_CS_ERROR, "Error sending data", 
		DHS_AV_LIST_NULL, DHS_BD_GT_RAW, DHS_BD_DATASET_NULL, &status );
    }
}

/*+
************************************************************************
*
*   Function:	name
*
*   Purpose:
*	Statement of purpose.
*
*   Values Returned:
*	type	name		: Meaning	*** delete if void function ***
*	type	name		: Meaning	*** delete if void function ***
*
*   References:
*	*** delete if not applicable ***
*
************************************************************************
-*/

void		errorCallback
( 
    DHS_CONNECT		connection,
    DHS_STATUS		errorNum,
    DHS_ERR_LEVEL	errorLev,
    char		*msg,
    DHS_TAG		tag,
    void		*userData
)
{
    char	*s1, *s2;
    DHS_STATUS	status;


    status = DHS_S_SUCCESS;

    if ( errorNum == DHS_S_CONNECT )
    {
	check( dhsConnectInfo( connection, &s1, &s2, &status ), status, VOID );
	fprintf( stderr, "connection info: %s %s\n", s1, s2 );
	fflush( stderr );
#ifdef omit
	if ( strcmp( s1, "204.174.103.38" ) == 0 )
	{
	    fprintf( stderr, "Connection refused\n" );
	    fflush( stderr );
	    check( dhsDisconnect( connection, &status ), status, VOID );
	}
#endif
    }
    else if ( errorNum == DHS_E_CON_LOST )
    {
	(void) fprintf( stderr, "connection lost %s\n", msg );
	fflush( stderr );

	dhsDisconnect( connection, &status );
    }
    else
    {
	(void) fprintf( stderr, "%s\n", msg );
	fflush( stderr );
    }
}
int	main
(
    int		argc,
    char	*argv[]
)
{
    DHS_STATUS		status;
    char		*name;


    if ( argc < 2 || argc > 3 )
    {
	(void) fprintf( stderr, "usage: dhsServer [ -noStrict ] <name>\n" );
	exit( 0 );
    }

    strict = TRUE;
    if ( argc == 3 )
    {
	if ( strcmp( argv[1], "-noStrict" ) == 0 )
	{
	    strict = FALSE;
	    name = argv[2];
	}
	else if ( strcmp( argv[2], "-noStrict" ) == 0 )
	{
	    strict = FALSE;
	    name = argv[1];
	}
	else
	{
	    (void) fprintf( stderr, "usage: dhsServer [ -noStrict ] <name>\n" );
	    exit( 0 );
	}
    }
    else if ( argc == 2 )
    {
	name = argv[1];
    }


    (void) signal( SIGINT, on_signal );
    (void) signal( SIGTERM, on_signal );
    
    status = DHS_S_SUCCESS;
    check( dhsInit( name, 5, &status ), status, -1 );
    dhsSetTimeout( 5 );
    check( dhsDebugLevel( DHS_DEBUG_ON, &status ), status, -2 ); 
    check( dhsCallbackSet( DHS_CBT_ERROR, (DHS_CB_FN_PTR) errorCallback, 
	    &status ), status, -2 );
    check( dhsCallbackSet( DHS_CBT_COMMAND, (DHS_CB_FN_PTR) commandCallback, 
	    &status ), status, -2 );
    check( dhsCallbackSet( DHS_CBT_SERVER_PUT, (DHS_CB_FN_PTR) putCallback, 
	    &status ), status, -2 );
    check( dhsCallbackSet( DHS_CBT_SERVER_GET, (DHS_CB_FN_PTR) getCallback, 
	    &status ), status, -2 );

    check( dhsEventLoop( DHS_ELT_WAIT, &status ), status, -4 );
    
    check( dhsExit( &status ), status, -3  );



    return( status );
}
