/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	sim/src/process.c
*
*   Purpose:
*	Contains routines for processing the simulation.
*
*   Routines:
*
*   Date		: 
*
*   SCCS data		: 
*	Module Name	: 
*	Version Number	: 
*	Release Number	: 
*	Last Updated	: 
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/05/05 SEC  :	Mod'ed to request a daat label from the data server.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/uio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "dhs.h"

#include "globals.h"


/*
 *  Static function prototypes. 
 */

static	int	prepBdCtl( EVENT *, DHS_AV_LIST *, DHS_AV_LIST *, DHS_AV_LIST * );
static	int	sendFile( EVENT * );
static	int	file2buf( char	*, byte	**, int * );
static	int	sendBdCtl( EVENT * );
static	int	determine_start_time( void );
static	int	readFile( EVENT * );


/*+
************************************************************************
*
*   Function:	buildList
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		buildList
(
    void
)
{
    EVENT	*event;
    char	line[256];	/* Line from the file.			*/
    char	*tokens[10];
    char	*masterdatalabel;
    int		numtokens;
    FILE	*fp;
    int		index;
    int		tmpNum;
    char	*tmpArray[DIS_MAX_CONTRIBS];
    int		i;
    char	*tmpStr;


    CHECK_NULL( globals.gSimList = eptr_alloc( 1000 ) );


    /*
     *  Open the file for reading. Loop until end of file.
     */
	
    fp = fopen( globals.g_inputfile, "r" );
    if ( fp == NULL )
    {
	formatMessage( E_SYSTEM, globals.g_inputfile );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }

    index = 0;
    while ( !feof(fp) )
    {
	/*
	 *  Fetch line from file. '#' in 1st column means comment.
	 *  Blank lines ignored.
	 */

	if ( ( fgets( line, sizeof(line), fp ) ) == NULL )
	{
	    break;
	}
	if ( line[0] == '#' )
	{
	    continue;
	}
	

	/*
	 *  Separate into tokens. Make sure numtokens is 7 or 10.
	 *  Ignore blank lines. If identity doesn't match, ignore line. 
	 */
	
	strtokens( line, 10, tokens, &numtokens );
	if ( numtokens == 0 )
	{
	    continue;
	}
	if ( ( numtokens != 7 ) && ( numtokens != 10 ) )
	{
	    return( E_INPUT );
	}
	if ( strne( globals.g_identity, tokens[2] ) )
	{
	    continue;
	}
	

	/*
	 *  Create new struct. Save mandatory tokens to struct. 
	 */

	event = (EVENT *) gen_alloc( sizeof( EVENT ) );
	CHECK_NULL( event->fname = strsav( tokens[0] ) );
	CHECK_NULL( event->dsname = strsav( tokens[1] ) );
	CHECK_NULL( event->identity = strsav( tokens[2]  ));

	if ( !str2int( tokens[3], &( event->time ) ) )
	{
	    return( E_INPUT );
	}

	switch ( tokens[4][0] )
	{
	    case SIM_BD_PT_DS_QL:
		event->type = DHS_BD_PT_DS_QL;
		break;

	    case SIM_BD_PT_DS:
		event->type = DHS_BD_PT_DS;
		break;

	    case SIM_BD_PT_FITS:
		event->type = DHS_BD_PT_FITS;
		break;

	    case SIM_BD_PT_RAW:
		event->type = DHS_BD_PT_RAW;
		break;

	    default:
		return( E_INPUT );
	}
	event->last = ( ( tokens[5][0] == 'T' ) ? DHS_TRUE : DHS_FALSE );
	event->master = ( ( tokens[6][0] == 'T' ) ? DHS_TRUE : DHS_FALSE );


	/*
	 *  If num is 10, save optional 'master' tokens. 
	 */
	
	if ( numtokens == 10 )
	{
	    switch ( tokens[7][0] )
	    {

		case SIM_BD_LT_TEMPORARY:
		    event->lifetime = DHS_BD_LT_TEMPORARY;
		    break;

		case SIM_BD_LT_TRANSIENT:
		    event->lifetime = DHS_BD_LT_TRANSIENT;
		    break;

		case SIM_BD_LT_PERMANENT:
		default:
		    event->lifetime = DHS_BD_LT_PERMANENT;
		    break;
	    }
	    strtokens( tokens[8], DIS_MAX_CONTRIBS, tmpArray,
		    &event->num_contribs );
	    for ( i = 0; i < event->num_contribs; i++ )
	    {
		tmpStr = strsav( tmpArray[i] );
		event->contribs[i] = tmpStr;
	    }
	    strtokens( tokens[9], DIS_MAX_STREAMS, tmpArray,
		    &event->num_streams );
	    for ( i = 0; i < event->num_streams; i++ )
	    {
		tmpStr = strsav( tmpArray[i] );
		event->streams[i] = tmpStr;
	    }

	    /*
	     * Allocate room for the dsdatalabel, which will be 
	     * requested from the dataserver. If event type is quicklook
	     * only, use label from input file. 
	     */
	    
	    if ( event->type == DHS_BD_PT_DS_QL )
	    {
		event->dsdatalabel = strsav( event->dsname );
	    }
	    else
	    {
		event->dsdatalabel = (char *) gen_alloc( 40 * sizeof(char) );
		masterdatalabel = event->dsdatalabel;
	    }
	}
	else
	{
	    if ( event->type == DHS_BD_PT_DS_QL )
	    {
		event->dsdatalabel = strsav( event->dsname );
	    }
	    else
	    {
		event->dsdatalabel = masterdatalabel;
	    }
	}


	/*
	 *  Save struct into eptr list. 
	 */

	CHECK_NULL( eptr_insert( globals.gSimList, index, event ) );
	index++;
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	process
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

int		process
(
    void
)
{
    EVENT	*event;
    int		i;
    struct timespec 	ts_struct;
    int		msecs_to_sleep;


    /*
     *  Build list of puts to send. 
     */

    CHECK( buildList() );


    /*
     *  Determine synch start time from passed-in string.
     */

    CHECK( determine_start_time() );
    
    for ( i = 0; i < eptrlen( globals.gSimList ); i++ )
    {
	event = (EVENT *) eptritem( globals.gSimList, i );
	
	
	/*
	 *  Read in the file. 
	 */
	
	CHECK( readFile( event ) );

	
	/*
	 *  Wait until the time to send.
	 */

        CHECK_SYS( clock_gettime( CLOCK_REALTIME, &ts_struct ),
		"clock_gettime" );
	msecs_to_sleep = ( ( globals.gSynchTime - ts_struct.tv_sec ) * 1000 
		+ event->time - (int) ts_struct.tv_nsec/1000000 );
	if ( msecs_to_sleep >= 0 )
	{
	    (void) usleep( msecs_to_sleep*1000 );
	}

	
	/*
	 *  If necessary, send bdCtls.
	 */

	if ( event->master && ! globals.gQlData )
	{
	    CHECK( sendBdCtl( event ) );
	}


	/*
	 *  Send file, do not wait until returned. 
	 */

	CHECK( sendFile( event ) );
    }
    

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	sendBdCtl
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

static	int	sendBdCtl
(
    EVENT	*event
)
{
    DHS_STATUS	dhsStatus;
    char	*datalabel;


    dhsStatus = DHS_S_SUCCESS;


    /*
     *  If data type is ql only, only bdctl to send is streams.
     */

    if ( event->type != DHS_BD_PT_DS_QL )
    {
	/*
	 * Request a data label from the dataserver. 
	 */

	CHECK_DHS( dhsBdCtl( globals.g_connect, DHS_BD_CTL_GETNAME,
		&datalabel, &dhsStatus ), dhsStatus );
	sprintf( event->dsdatalabel, "%s", datalabel );

	/*
	 *  Send bdCtls for the event. 
	 */

	CHECK_DHS( dhsBdCtl( globals.g_connect, DHS_BD_CTL_LIFETIME,
		event->dsdatalabel, event->lifetime, 
		&dhsStatus ), dhsStatus );

	if ( event->contribs != NULL )
	{
	    CHECK_DHS( dhsBdCtl( globals.g_connect, DHS_BD_CTL_CONTRIB,
		    event->dsdatalabel, event->num_contribs, event->contribs, 
		    &dhsStatus ), dhsStatus );
	}
    }

    if ( event->streams != NULL )
    {
	CHECK_DHS( dhsBdCtl( globals.g_connect, DHS_BD_CTL_QLSTREAM,
		event->dsdatalabel, event->num_streams, event->streams, 
		&dhsStatus ), dhsStatus );
    }

    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	readFile
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

static	int	readFile
(
    EVENT	*event
)
{
    DHS_STATUS	dhsStatus;


    dhsStatus = DHS_S_SUCCESS;


    /*
     *  Read file into buffer. 
     */

    CHECK( file2buf( event->fname, &event->buffer, &event->length ) );


    /*
     *  If file is DS type, import into internal structure.
     */
	
    if ( event->type == DHS_BD_PT_DS || event->type == DHS_BD_PT_DS_QL )
    {
	CHECK_DHS( ( event->dataset = dhsBdDsAccess( event->buffer, 
		&dhsStatus ) ), dhsStatus );
    }


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	file2buf
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

static	int	file2buf
(
    char	*filename,
    byte	**buffer,
    int		*length
)
{
    byte	tempbuf[4096];
    byte	*bufptr;
    int		len;
    int		pos;
    int		status;
    int		fd;
    struct stat	stat_struct;


    /*
     *  Open the file for reading. Stat it to find out how much buffer to 
     *  allocate.
     */


    fd = open( filename, O_RDONLY );
    if ( fd == -1 )
    {
	formatMessage( MSG_ERRNO, filename );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }

    status = fstat( fd, &stat_struct );
    if ( status == -1 )
    {
	formatMessage( MSG_ERRNO, filename );
	printMessage( msg, MESS_ERROR );
	return( E_SYSTEM );
    }

    CHECK_NULL( *buffer = byte_alloc( stat_struct.st_size ) );


    /*
     *  Slurp the file into the buffer. 
     */

    pos = 0;
    bufptr = &tempbuf[0];

    for ( ; ; )
    {
	len = read( fd, bufptr, 4096 );
	if ( len <= 0 )
	{
	    break;
	}

	memcpy( *buffer + pos, bufptr, len );
	pos += len;
    }
    *length = pos;


    /*
     *  Close file.
     */
     
    status = close( fd );
    if ( status == -1 )
    {
	formatMessage( MSG_ERRNO, filename );
	printMessage( msg, MESS_ERROR );
    }


    return(S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	sendFile
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*
************************************************************************
-*/

static	int	sendFile
(
    EVENT	*event
)
{
    DHS_STATUS	dhsStatus;
    DHS_TAG	tag;


    dhsStatus = DHS_S_SUCCESS;

    formatMessage( S_SENDING, event->time, event->dsname, 
	    event->dsdatalabel );
    printMessage( msg, MESS_OPER );

    pthread_mutex_lock( &globals.gMutex );
    printf( "numBuffers: %d\n", globals.gFilesSent );
    while ( globals.gFilesSent >= globals.gNumBuffers )
    {
	pthread_cond_wait( &globals.gCond, &globals.gMutex );
    }
    globals.gFilesSent++;
    pthread_mutex_unlock( &globals.gMutex );
    if ( event->type == DHS_BD_PT_DS || event->type == DHS_BD_PT_DS_QL )
    {
	CHECK_DHS( ( tag = dhsBdPut( globals.g_connect, event->dsdatalabel, 
		event->type, event->last, event->dataset, event,
		&dhsStatus ) ), dhsStatus );
	CHECK_DHS( dhsBdDsFree( event->dataset, &dhsStatus ), dhsStatus );
    }
    else
    {
	CHECK_DHS( ( tag = dhsBdPut( globals.g_connect, event->dsdatalabel, 
		event->type, event->last, (void *) event->buffer, 
		event->length, event, &dhsStatus ) ), dhsStatus );
    }
    gen_free( event->buffer );


    return( S_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	determine_start_time
*
*   Purpose:
*
*   Values Returned:
*	int	S_SUCCESS	: Normal completion.
*	int	E_START_TIME	: Start time unparseable. 
*
************************************************************************
-*/

static	int	determine_start_time
(
    void
)
{
    int		hour;
    int		minute;
    time_t	curTime;
    time_t	startTime;
    struct tm	*tm_struct;


    tm_struct = NULL;


    if ( globals.gSynchNow )
    {
	/*
	 *  Forget about synchronizing, start ASAP.
	 */
	
	startTime = time( NULL );

    }
    else
    {
	/*
	 *  Must synchronize start. Scan hour and minute from time string.
	 */

	if ( sscanf( globals.gSynchTimeStr, "%d:%d", &hour, &minute ) != 2 )
	{
	    return( E_START_TIME );
	}


	/*
	 *  Get local day and add hour and minute, then make sure it isn't
	 *  already past. 
	 */

	curTime = time( NULL );
	tm_struct = localtime( &curTime );
	tm_struct->tm_hour = hour;
	tm_struct->tm_min = minute;
	tm_struct->tm_sec = 0;
	startTime = mktime( tm_struct );
	gen_free( tm_struct );

	if ( startTime < curTime )
	{
	    formatMessage( E_START_TIME );
	    printMessage( msg, MESS_ERROR );
	    return( E_START_TIME );
	}
    }

    globals.gSynchTime = (int) startTime;


    return( S_SUCCESS );
}
