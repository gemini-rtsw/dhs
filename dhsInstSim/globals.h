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
*   Include File Name:	sim/src/globals.h
*
*   Purpose:
*	Contains global definitions for sim .
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
*	98/05/05 SEC  :	Added a char * for the data label to the event 
*			struct. 
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Define the return code symbols.
 */

#define S_QUIT		5	/* Quit at user's request.		*/
#define S_RESP_CALLBACK 4
#define	S_SENDING	3
#define S_USAGE		2	/* Command line usage message.		*/
#define	S_VERSION	1	/* Version message.			*/

#define	S_SUCCESS	0	/* Normal completion.			*/

#define E_DHS		(-1)
#define E_DHS_MESG	(-2)
#define E_ERR_CALLBACK	(-3)
#define	E_INPUT		(-4)
#define	E_MEMORY	(-9)	/* Memory allocation failure.		*/
#define	E_START_TIME	(-10)	/* Err parsing start time or is in past.*/
#define	E_SYSTEM	(-12)	/* Error in a system call.		*/
#define	E_USAGE		(-14)	/* Command line usage error.		*/


/*
 *  This is the subject used for general messages to the operator.
 */

#define	APPLICATION		"Simulator"
#define	APP_SHORT		"sim"

/*
 *  Define stuff for av lists.
 */

#define	SIM_LIFETIME	"lifetime"
#define	SIM_CONTRIBS	"contribs"
#define	SIM_STREAM	"stream"
#define	SIM_BDCTL	"bdCtl"
#define	SIM_CTL_CMD	"ctl"


/*
 *  Define put type characters from input file.
 */

#define SIM_BD_PT_DS	'D'
#define SIM_BD_PT_DS_QL	'Q'
#define SIM_BD_PT_FITS	'F'
#define SIM_BD_PT_RAW	'R'
#define SIM_BD_PT_SDS	'S'


/*
 *  Define lifetime characters from input file. 
 */

#define SIM_BD_LT_PERMANENT	'P'
#define SIM_BD_LT_TEMPORARY	'T'
#define SIM_BD_LT_TRANSIENT	'Q'


/*
 *  Message symbols.
 */

#define	MESS_LOG	0x01	/* To logfile.				*/
#define	MESS_CON	0x02	/* Send to console.			*/
#define	MESS_CON_VERB	0x04	/* Send to console if in verbose mode.	*/
#define	MESS_NOCR	0x08	/* No carriage return on end of line.	*/

#define MESS_ERROR      MESS_LOG | MESS_CON
				/* To console and logfile.		*/
#define MESS_INFO       MESS_LOG | MESS_CON_VERB
				/* Only to logfile unless verbose mode.	*/
#define MESS_OPER       MESS_LOG | MESS_CON
                                /* Non-error message that the operator 
				   should see - to console and logfile. */


/*
 *  Return status handling macros.
 */


#define	CHECK(s)							\
	{   int		estatus;					\
	    if ( ( estatus = (s) ) < 0 || estatus == S_QUIT )		\
	 	return( estatus ); }

#define	CHECK_MEMORY(s)							\
	{   if ( (s) == NULL )						\
	    {	formatMessage( E_MEMORY );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MEMORY ); } }

#define	CHECK_NULL(s)							\
	{   if ( (s) == NULL )						\
	    {	formatMessage( E_MEMORY );				\
		printMessage( msg, MESS_ERROR );			\
		return( E_MEMORY ); } }

#define	CHECK_SYS( s, file )						\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, file );			\
		printMessage( msg, MESS_ERROR );			\
		return( E_SYSTEM ); } }

#define CHECK_DHS( s, e )                                               \
	{   DHS_ERR_LEVEL       elevel;                                 \
	    DHS_STATUS          enumber;                                \
	    DHS_STATUS          estatus;                                \
	    char                *mesg;                                  \
	    (s);                                                        \
	    if ( e != DHS_S_SUCCESS )                                   \
	    {                                                           \
		estatus = DHS_S_SUCCESS;                                \
		formatMessage( E_DHS );                                 \
		printMessage( msg, MESS_ERROR );                        \
		if ( ( mesg = dhsMessage(&enumber,&elevel,&estatus) )   \
			!= NULL )                                       \
		{                                                       \
		    formatMessage( E_DHS_MESG, mesg );                  \
		    printMessage( msg, MESS_ERROR );                    \
		    dhsMessageClear(&estatus);                          \
		}                                                       \
		if ( e )                                                \
		{                                                       \
		    return( E_DHS );                                    \
		}                                                       \
	    }                                                           \
	}


/* 
 *  Define the event structure.
 */

#define	DIS_MAX_CONTRIBS	256
#define	DIS_MAX_STREAMS		256

typedef	struct	event_struct
{
    char	*fname;
    char	*dsname;
    char	*dsdatalabel;
    char	*identity;
    int		time;
    DHS_BD_PUT_TYPE
    		type;
    DHS_BOOLEAN	last;
    boolean	master;
    DHS_BD_LIFETIME 
    		lifetime;
    char	*contribs[DIS_MAX_CONTRIBS];
    int		num_contribs;
    char	*streams[DIS_MAX_STREAMS];
    int		num_streams;
    byte	*buffer;
    int		length;
    DHS_BD_DATASET
    		dataset;
} EVENT;

 
/* 
 *  Define the globals structure.
 */

typedef	struct	globals
{
    char	*g_log_file;		/* Log file name.		*/
    FILE	*g_log_fp;		/* File pointer for log file.	*/
    char	*g_inputfile;		/* Input file name.		*/
    char	*g_identity;		/* IMP Identity.		*/
    char	*g_dataServer;		/* Dataserver Identity.		*/
    char	*g_dataServIP;		/* Dataserver IP address.	*/
    int		gFilesSent;		/* Counter - neeeds mutex lox.	*/
    int		g_debug_mode;		/* DHS debug mode.		*/
    DHS_CONNECT	g_connect;		/* DHS Connection to dataServer.*/
    pthread_mutex_t
		gMutex;
    pthread_cond_t
		gCond;
    EPTR	*gSimList;		/* Event list.			*/
    char	*gSynchTimeStr;		/* Start time string.		*/
    int		gSynchTime;		/* Synchronized start time.	*/
    boolean	gSynchNow;		/* Forget synch-ing, start now.	*/
    boolean	gQlData;		/* Data is for quick look server.*/
    int		gNumBuffers;		/* Number of available buffers.	*/
} GLOBALS;

 

/*
 *  Declare the global symbols.
 */

extern  int     errno;
extern	GLOBALS	globals;
extern	char	msg[];


/*
 *  Declare the global functions.
 */

int	globalsInit( void );
int	simInit( void );
int	process( void );
int	cleanup( void );
void	formatMessage( int, ... );
void	printMessage( char *, int );
void errCB( DHS_CONNECT, DHS_STATUS, DHS_ERR_LEVEL, char *, DHS_TAG, void * );
void respCB(DHS_CONNECT,DHS_TAG,char *,DHS_CMD_STATUS,char *,DHS_AV_LIST,void *);
void putCB(DHS_CONNECT,DHS_TAG,DHS_CMD_STATUS,char *,char *,void *);


