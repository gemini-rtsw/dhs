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
 * dhsLibrary/h/dhs.h
 *
 * PURPOSE:
 * This is the main header file for the DHS library. Any module wanting
 * to use the functions documented in the ICD 1c or ICD 3 documents must
 * include this header file first.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:46  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.36  2001/03/27 20:07:30  nhill
 * Added several new error codes.
 * Added the dhsBdAttribEdit function.
 *
 * Revision 1.35  2000/03/04 00:01:23  nhill
 * Added function dhsAvDelete, and macro dhsBdAttribDelete.
 *
 * Revision 1.34  1998/06/02 18:44:59  nhill
 * Added function dhsConnectCount.
 *
 * Revision 1.33  1998/05/25 20:35:06  nhill
 * Added the DHS_S_NULL message.
 *
 * Revision 1.32  1998/05/25 19:53:59  nhill
 * Updated the message numbers.
 * Fixed some of the function prototypes.
 *
 * Revision 1.31  1998/05/11 16:30:49  nhill
 * Added a reconnect message and a reconnect status to the connection
 * states.
 *
 * Revision 1.30  1998/02/16 23:46:19  nhill
 * Added functions dhsConUserDataSet and dhsUserDataSet.
 *
 * Revision 1.29  1998/02/05 19:25:01  nhill
 * Changed the name of the vxpthread.h header file.
 *
 * Revision 1.28  1998/01/20 18:25:23  nhill
 * Moved the tDhsConState enumerated type here from the localDhs.h file.
 * Added the connection callback function.
 * Added function dhsIsConnected.
 *
 * Revision 1.27  1997/12/18 22:16:51  nhill
 * Added status DHS_S_COND_TIMEOUT and DHS_E_CON_LOCKOUT.
 *
 * Revision 1.26  1997/09/18 20:56:04  nhill
 * Added some new status values.
 *
 * Revision 1.25  1997/08/19 19:48:07  nhill
 * Change the prototype of dhsAvAdd to match the documentation in ICD 1c.
 *
 * Revision 1.24  1997/08/06 16:13:33  nhill
 * Added the dhsAvListExport and dhsAvListSize functions.
 *
 * Revision 1.23  1997/07/29 22:17:35  nhill
 * Added DHS_E_DISCONNECT and DHS_E_NOT_AVLIST to the status list.
 *
 * Revision 1.22  1997/07/24 21:29:05  nhill
 * Added data type DHS_DT_DONE.
 *
 * Revision 1.21  1997/07/02 20:22:22  greimel
 * added include file for draft 4 pthreads
 *
 * Revision 1.20  1997/06/04 18:39:59  nhill
 * Dropped the DHS_BD_GT?? types.
 * Added the DHS_BD_GT_??_ASIS types.
 * Added the DHS_BD_PT_??_UNIQUE types.
 *
 * Revision 1.19  1997/05/13 17:09:13  nhill
 * Added the dhsElBroadcast function.
 *
 * Revision 1.18  1997/04/24 16:47:12  nhill
 * Added "const" keywords in serveral function prototypes.
 *
 * Revision 1.17  1997/04/15 19:17:47  nhill
 * Added some new status values.
 * Added a "THREAD_NULL" value.
 *
 * Revision 1.16  1997/03/26 16:59:52  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.15  1997/02/24 19:17:49  nhill
 * Checkin for install.
 *
 * Revision 1.14  1997/02/17 20:16:27  nhill
 * Added new messages.
 *
 * Revision 1.13  1997/02/12 22:05:47  nhill
 * Checkin for install.
 *
 * Revision 1.12  1997/02/04 19:42:19  nhill
 * Checkin for install.
 *
 * Revision 1.11  1997/01/30 17:19:13  nhill
 * Checkin for install.
 *
 * Revision 1.10  1997/01/29 18:18:59  nhill
 * checkin for install.
 *
 * Revision 1.9  1997/01/28 19:00:25  nhill
 * checkin before install.
 *
 * Revision 1.8  1997/01/27 22:35:29  nhill
 * Checkin before install.
 *
 * Revision 1.7  1997/01/22 21:36:10  nhill
 * Stable Checkin.
 *
 * Revision 1.6  1997/01/20 19:01:57  nhill
 * Checking for install.
 *
 * Revision 1.4  1997/01/15 20:41:17  nhill
 * *** empty log message ***
 *
 * Revision 1.3  1997/01/15 17:17:24  nhill
 * Fixed omitions and typos.
 *
 * Revision 1.2  1997/01/13 17:59:20  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef DHS_H
#define	DHS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef	vxWorks
#include <dhsVxPthread.h>
#else	/* vxWorks */
#include <pthread.h>
#endif	/* vxWorks */


/*
 ************************************************************************
 *  Constant definitions.
 ************************************************************************
 */

#define	DHS_TAG_NULL		( (DHS_TAG) NULL )	
					/* An unused tag value.		*/
#define	DHS_AV_LIST_NULL 	-1	/* An unused avlist value.	*/
#define	DHS_AV_ID_NULL		-1	/* An unused attribute id.	*/
#define	DHS_BD_FRAME_NULL 	-1	/* An unused frame id.		*/
#define	DHS_BD_DATASET_NULL	-1	/* An unused dataset id.	*/
#define	DHS_CONNECT_NULL	( (DHS_CONNECT) NULL )
					/* An unused connection id.	*/
#define	DHS_THREAD_NULL		0	/* A null thread value.		*/

#define	DHS_DATA_LABEL_LEN	255	/* Maximum length of data label.*/

#define	DHS_BUFF_PER_CONNECT	52224	/* Bytes required per connection.*/



/*
 ************************************************************************
 *  Simple data types types
 ************************************************************************
 */

typedef long		DHS_AV_ID;	/* Id of an attribute/value pair*/
typedef	long		DHS_AV_LIST;	/* Attribute value list id.	*/
typedef	DHS_AV_ID	DHS_BD_ATTRIB_ID;
					/* Id of a bulk data attribure.	*/
typedef	DHS_AV_LIST	DHS_BD_DATASET;	/* Id of a dataset.		*/
typedef	DHS_AV_LIST	DHS_BD_FRAME;	/* Id of a dataset.		*/
typedef DHS_AV_LIST	DHS_BD_OBJECT;	/* A dataset or frame.		*/
typedef	unsigned long	DHS_CONNECT;	/* DHS conneltion id.		*/
typedef unsigned long	DHS_TAG;	/* DHS tag.			*/


/*
 ************************************************************************
 *  Enumerated types
 ************************************************************************
 */

/*
 *  Function return status codes.
 */

typedef enum
{
    DHS_S_CMD_BULK_SENT	= 23,	/* Message sent as bulk data.		*/
    DHS_S_CMD_DELETE	= 22,	/* Deleting command structure.		*/
    DHS_S_CMD_MSG_SENT	= 21,	/* Message sent as regular message.	*/
    DHS_S_CMD_NEW	= 20,	/* Creating command structure.		*/
    DHS_S_COND_TIMEOUT	= 19,	/* Condition variable timed out.	*/
    DHS_S_CONNECT	= 18,	/* Client is requesting connection.	*/
    DHS_S_DISCONNECT	= 17,	/* Client has desconnected.		*/
    DHS_S_EL_EXIT	= 16,	/* Exit the event loop.			*/
    DHS_S_FN_ENTRY	= 15,	/* Entering function.			*/
    DHS_S_FN_EXIT	= 14,	/* Exiting function.			*/
    DHS_S_IMP_EVENT	= 13,	/* Imp event received.			*/
    DHS_S_MUTEX_LOCK	= 12,	/* Locking a mutex.			*/
    DHS_S_MUTEX_UNLOCK	= 11,	/* Unlocking a mutex.			*/
    DHS_S_NO_ATTRIB	= 10,	/* No matching attribute found.		*/
    DHS_S_NO_DATA	= 9,	/* Requested data not available.	*/
    DHS_S_NO_FRAME	= 8,	/* No matching frame found.		*/
    DHS_S_NO_MESSAGE	= 7,	/* No message found in message queue.	*/
    DHS_S_NO_RESP	= 6,	/* No response found matching index.	*/
    DHS_S_NULL		= 5,	/* NULL error message.			*/
    DHS_S_RECONNECT	= 4,	/* A connection was automatically 	*/
    				/* re-connected.			*/
    DHS_S_SHUTDOWN	= 3,	/* Server requests client disconnect.	*/
    DHS_S_SYS_EVENT	= 2,	/* System event arrives.		*/
    DHS_S_USER_EVENT	= 1,	/* User event arrives.			*/

    DHS_S_SUCCESS 	= 0,	/* Normal completion.			*/

    DHS_E_AVEDIT_DIMS	= -1,	/* Attempt to edit attribute with 	*/
    				/* non-zero dimensions.			*/
    DHS_E_AVEDIT_INFO	= -2,	/* Edit information structure has wrong	*/
    				/* size.				*/
    DHS_E_AVEDIT_TYPE	= -3,	/* Attempt to change data type of 	*/
    				/* editted data.			*/
    DHS_E_AVLIST_ARRAY	= -4,	/* Attempt to add array of avlists to	*/
    				/* an avlist.				*/
    DHS_E_BT_FIND	= -5,	/* Invalid btFind type.			*/
    DHS_E_BT_NOT_FOUND	= -6,	/* Bulk transfer structure not found.	*/
    DHS_E_CB_NULL	= -7,	/* A required callback function is NULL.*/
    DHS_E_CB_TYPE	= -8,	/* Invalid callback type.		*/
    DHS_E_CMD_DELETED	= -9,  /* Attemp to reference a command	*/
    				/* scheduled for deletion.		*/
    DHS_E_CMD_LOST	= -10,	/* A command was lost.			*/
    DHS_E_CMD_FIND	= -11,	/* Invalid cmdFind type.		*/
    DHS_E_CMD_NOT_FOUND	= -12,	/* Command not found.			*/
    DHS_E_CON_FIND	= -13,	/* Could not find connection.		*/
    DHS_E_CON_INVALID	= -14,	/* Attempt to send to invalid connection*/
    DHS_E_CON_LOCKOUT	= -15,	/* Attempt to open a connection after	*/
    				/* connections have been locked out.	*/
    DHS_E_CON_LOST	= -16,	/* Connection was lost.			*/
    DHS_E_CON_NOT_FOUND	= -17,	/* Connection could not be located.	*/
    DHS_E_COND		= -18,	/* Error accessing condition variable.	*/
    DHS_E_CONNECT	= -19,	/* Error connecting to server.		*/
    DHS_E_CONNECT_REPLY	= -20,	/* Unexpected connect reply message.	*/
    DHS_E_CTL_CMD	= -21,	/* CTL command to a server failed.	*/
    DHS_E_DISCONNECT	= -22, /* Invalid disconnect message received.	*/
    DHS_E_EL_RUNNING	= -23,	/* Event loop is already running.	*/
    DHS_E_ERS		= -24,	/* Error in the ERS library.		*/
    DHS_E_ERS_MSG	= -25,	/* Message displayed by the ERS system.	*/
    DHS_E_IMP		= -26,	/* Error in an IMP routine.		*/
    DHS_E_IMP_CONNECT	= -27,	/* Error making IMP connection.		*/
    DHS_E_IMP_MSG	= -28,	/* Invalid IMP message format.		*/
    DHS_E_IMP_REGISTER	= -29,	/* Error registering with IMP.		*/
    DHS_E_IMP_SYS_MSG	= -30,	/* Unknown imp system message.		*/
    DHS_E_IMP_USER_MSG	= -31,	/* Unkown imp user message.		*/
    DHS_E_INIT		= -32,	/* DHS library is not initialized.	*/
    DHS_E_LOCATE	= -33,	/* Imp locate failed.			*/
    DHS_E_LOCATE_MSG	= -34,	/* Unexpected imp locate message.	*/
    DHS_E_LOOP_TYPE	= -35,	/* Invalid event loop type.		*/
    DHS_E_MEMORY	= -36,	/* Memory allocation failure.		*/
    DHS_E_MSG_LENGTH	= -37,	/* Message length is invalid.		*/
    DHS_E_MSG_SENDER	= -38,	/* User message without sender info.	*/
    DHS_E_MUTEX		= -39,	/* Error accessing a mutex.		*/
    DHS_E_NO_ATTRIB	= -40,	/* Attribute not found in av list.	*/
    DHS_E_NOT_AVLIST	= -41,	/* Structure is not an avList.		*/
    DHS_E_NULLVALUE	= -42,	/* Data value cannot be null.		*/
    DHS_E_PTR_SIZE	= -43,	/* Invalid pointer size.		*/
    DHS_E_SDS		= -44,	/* Error in an sds routine.		*/
    DHS_E_TASK_UNKNOWN	= -45,	/* Unknown task.			*/
    DHS_E_THREAD_CREATE	= -46,	/* Error creating thread.		*/
    DHS_E_TSD		= -47,	/* Error attempting to access thread	*/
    				/* specific data.			*/
    DHS_E_TYPE		= -48	/* Unknown data type.			*/
} DHS_STATUS;


/*
 *  Error levels of the status codes.
 */

typedef enum
{
    DHS_EL_NULL,		/* Null error level.			*/
    DHS_EL_SEVERE,		/* Severe error.			*/
    DHS_EL_ERROR,		/* Operation has failed.		*/
    DHS_EL_WARNING,		/* Warning.				*/
    DHS_EL_INFO,		/* Information message only.		*/
    DHS_EL_DEBUG		/* Debugging message only.		*/
} DHS_ERR_LEVEL;


/*
 *  Create a type for a boolean value.
 */

typedef enum
{
    DHS_TRUE		= 1,
    DHS_FALSE		= 0
} DHS_BOOLEAN;


/*
 *  Create an enumerated type to identify callback function type for 
 *  functon dhsCallbackSet.
 */

typedef enum
{
    DHS_CBT_COMMAND,		/* Command callback function.		*/
    DHS_CBT_CONNECT,		/* Connection callback function.	*/
    DHS_CBT_ERROR,		/* Error callback function.		*/
    DHS_CBT_GET,		/* Client get bulk data callback.	*/
    DHS_CBT_PUT,		/* Client put bulk data callback.	*/
    DHS_CBT_RESPONSE,		/* Command response callback function.	*/
    DHS_CBT_SERVER_GET,		/* Server get bulk data callback.	*/
    DHS_CBT_SERVER_PUT		/* Server put bulk data callback.	*/
} DHS_CB_TYPE;


/*
 * Create an enumerated type specifying a connections state.
 */

typedef enum
{
    DHS_CON_CONNECTED,		/* Connection is made.			*/
    DHS_CON_DISCONNECTING,	/* In the process of disconnecting.	*/
    DHS_CON_DUPLICATE,		/* Duplicate connection.		*/
    DHS_CON_ERROR,		/* Error trying to connect.		*/
    DHS_CON_LOCATED,		/* Connection was located.		*/
    DHS_CON_LOST,		/* Connection was lost.			*/
    DHS_CON_NOT_ACCEPTED,	/* Connection not yet accepted.		*/
    DHS_CON_NOT_CONNECTED,	/* Not connected.			*/
    DHS_CON_RECONNECT,		/* Connection has reconnected.		*/
    DHS_CON_REFUSED,		/* Server refused connection.		*/
    DHS_CON_REQUESTED,		/* Requested by client.			*/
    DHS_CON_TIMEOUT		/* Timeout trying to connect.		*/
} tDhsConState;
#define	DHS_CON_STATE	tDhsConState


/*
 *  Create a type to indicate the status of a command.
 */

typedef enum
{
    DHS_CS_ABORTED,		/* Intentionally aborted.		*/
    DHS_CS_BUSY,		/* Activly processing.			*/
    DHS_CS_DONE,		/* Completed successfully.		*/
    DHS_CS_ERROR,		/* Terminated with an error.		*/
    DHS_CS_IDLE,		/* Waiting for something.		*/
    DHS_CS_LOST,		/* Connection to server was lost.	*/
    DHS_CS_PENDING		/* Command is being evaluated.		*/
} DHS_CMD_STATUS;


/*
 *  Create a type to identify a data type.
 */

typedef enum
{
    DHS_DT_CHAR,		/* A single character.			*/
    DHS_DT_INT8,		/* An 8 bit integer.			*/
    DHS_DT_UINT8,		/* An unsigned 8 bit integer.		*/
    DHS_DT_INT16,		/* A 16 bit integer.			*/
    DHS_DT_UINT16,		/* An unsigned 16 bit integer.		*/
    DHS_DT_INT32,		/* A 32 bit integer.			*/
    DHS_DT_UINT32,		/* An unsigned 32 bit integer.		*/
    DHS_DT_FLOAT,		/* A 32 bit floating point value.	*/
    DHS_DT_DOUBLE,		/* A 64 bit floating point value.	*/
    DHS_DT_STRING,		/* A null terminated string.		*/
    DHS_DT_TAG,			/* A DHS tag.				*/
    DHS_DT_AVLIST,		/* An attribute value list.		*/
    DHS_DT_BOOLEAN,		/* A boolean value.			*/
    DHS_DT_NONE			/* No data type.			*/
} DHS_DATA_TYPE;


/*
 *  Create an enumerated type to indicate debugging level passed to 
 *  function dhsDebugLevel.
 */

typedef enum
{
    DHS_DEBUG_OFF,		/* Debugging is off.			*/
    DHS_DEBUG_ON,		/* A moderate amount of debugging.	*/
    DHS_DEBUG_FULL		/* All possible debugging info.		*/
} DHS_DEBUG_LEVEL;


/* 
 *  Create an enumerated type to indicate how the event loop can be run
 *  by function dhsEventLoop.
 */

typedef enum
{
    DHS_ELT_COND,		/* Run the event loop in this thread	*/
    				/* until the condition function returns	*/
				/* true.				*/
    DHS_ELT_NO_WAIT,		/* Process all pending events and then	*/
    				/* return.				*/
    DHS_ELT_THREADED,		/* Run the event loop in a thread.	*/
    DHS_ELT_WAIT		/* Run the event loop in this thread.	*/
} DHS_EL_TYPE;


/*
 *  Create an enumerated type to indicate which function should be executed
 *  by function dhsBdCtl
 */

typedef enum
{
    DHS_BD_CTL_ABORT,		/* Abort a data transfer.		*/
    DHS_BD_CTL_RESET,		/* Reset all or part of a dataset.	*/
    DHS_BD_CTL_LIFETIME,	/* Set lifetime of a dataset.		*/
    DHS_BD_CTL_CONTRIB,		/* Set contributors to a dataset.	*/
    DHS_BD_CTL_QLSTREAM,	/* Set quick look streams for a dataset.*/
    DHS_BD_CTL_GETNAME		/* Get a new data label.		*/
} DHS_BD_CTL;


/*
 *  Create an enumerated type to indicate the format for the file returned
 *  in response to a call to function dhsBdGet.
 */

typedef enum
{
    DHS_BD_GT_FITS,		/* Get a fits file.			*/
    DHS_BD_GT_FITS_ASIS,	/* Get a fits file.			*/
    DHS_BD_GT_FITS_HEADER,	/* Get a fits header.			*/
    DHS_BD_GT_RAW,		/* Get a raw data buffer.		*/
    DHS_BD_GT_RAW_ASIS,		/* Get a raw data buffer.		*/
    DHS_BD_GT_UNKNOWN		/* Unknown get type.			*/
} DHS_BD_GET_TYPE;


/*
 *  Create an enumerated type to specify a lifetime for a dataset.
 */

typedef enum
{
    DHS_BD_LT_PERMANENT,	/* The dataset should be permanent.	*/
    DHS_BD_LT_TEMPORARY,	/* The dataset should be temporary.	*/
    DHS_BD_LT_TRANSIENT		/* The dataset is for quick look only.	*/
} DHS_BD_LIFETIME;


/*
 *  Create an enumerated type to specify the format of a file to be sent
 *  with function dhsBdPut.
 */

typedef enum
{
    DHS_BD_PT_DS,		/* Data is a dataset structure.		*/
    DHS_BD_PT_DS_QL,		/* Send data to quick look, but don't	*/
    				/* make it part of the dataset.		*/
    DHS_BD_PT_FITS,		/* Data is a fits buffer.		*/
    DHS_BD_PT_FITS_UNIQUE,	/* Data is a fits buffer, add counter to*/
    				/* dsname.				*/
    DHS_BD_PT_RAW,		/* Data is a BLOB from the DHS point of	*/
    				/*  view.				*/
    DHS_BD_PT_RAW_UNIQUE,	/* Data is a BLOB from the DHS point of	*/
    				/*  view, add a counter to dsname.	*/
    DHS_BD_PT_SDS,		/* Data is an sds structure.		*/
    DHS_BD_PT_UNKNOWN		/* Unknown put type.			*/
} DHS_BD_PUT_TYPE;


/*
 ************************************************************************
 *  Function pointer types
 ************************************************************************
 */

/*
 *  This is the type for a generic callback function passed to 
 *  function dhsCallbackSet.
 */

typedef void		(*DHS_CB_FN_PTR)();


/*
 *  This is the type for a condition function passed to function 
 *  dhsEventLoop.
 */

typedef DHS_BOOLEAN	(*DHS_COND_FN)( void * );


/*
 *  This is the type of a thread id. This will have to be replaced with
 *  another type for VxWorks.
 */

typedef pthread_t	DHS_THREAD;


/*
 ************************************************************************
 *  Global function declarations
 ************************************************************************
 */

/*
 * The following map bulk data functions into their identical attribute
 * value list counterparts.
 */

#define dhsBdDsAccess			dhsAvListAccess
#define	dhsBdDsCopy			dhsAvListCopy
#define	dhsBdDsExport			dhsAvListExport
#define	dhsBdDsFree			dhsAvListFree
#define	dhsBdDsSize			dhsAvListSize
#define	dhsBdDsNew			dhsAvListNew
#define	dhsBdDsPrint			dhsAvListPrint
#define	dhsBdAttribAdd			dhsAvAdd
#define	dhsBdAttribDelete		dhsAvDelete
#define	dhsBdAttribEdit			dhsAvEdit
#define	dhsBdAttribInfo			dhsAvInfo
#define	dhsBdAttribFind			dhsAvFind
#define	dhsBdAttribIndex		dhsAvIndex


/*
 *  Normal function declarations.
 */

extern DHS_TAG		dhsApply( DHS_CONNECT, const char *, DHS_AV_LIST, 
				void *, DHS_STATUS *);
extern void		dhsAvAdd( DHS_AV_LIST, const char *, DHS_DATA_TYPE, 
				int, const unsigned long *, ... );
extern void		dhsAvDelete( DHS_AV_LIST, const char *, DHS_STATUS * );
extern void		dhsAvEdit( DHS_AV_ID, DHS_DATA_TYPE, ... );
extern DHS_AV_ID	dhsAvFind( DHS_AV_LIST, const char *, DHS_STATUS * );
extern DHS_AV_ID	dhsAvIndex( DHS_AV_LIST, int, DHS_STATUS * );
extern void		dhsAvInfo( DHS_AV_ID, char **, DHS_DATA_TYPE *, 
				int *, unsigned long [], void *, DHS_STATUS *);
extern DHS_AV_LIST	dhsAvListAccess( const void *, DHS_STATUS * );
extern DHS_AV_LIST	dhsAvListCopy( DHS_AV_LIST, DHS_STATUS * );
extern void		dhsAvListExport( DHS_AV_LIST, void *, unsigned long,
				DHS_STATUS * );
extern void		dhsAvListFree( DHS_AV_LIST, DHS_STATUS * );
extern DHS_AV_LIST	dhsAvListNew( DHS_STATUS * );
extern void		dhsAvListPrint( DHS_AV_LIST, DHS_STATUS * );
extern unsigned long	dhsAvListSize( DHS_AV_LIST, DHS_STATUS * );
extern DHS_BD_ATTRIB_ID	dhsBdAttribFind( DHS_BD_OBJECT, const char *, 
				DHS_STATUS * );
extern void		dhsBdCtl( DHS_CONNECT, DHS_BD_CTL, ... );
extern void		dhsBdDelete( DHS_CONNECT, char *, DHS_STATUS * );
extern void		dhsBdDsFree( DHS_BD_DATASET, DHS_STATUS * );
extern void		dhsElBroadcast( DHS_STATUS * );
extern DHS_BD_FRAME	dhsBdFrameFind( DHS_BD_OBJECT, const char *, 
				DHS_STATUS * );
extern DHS_BD_FRAME	dhsBdFrameIndex( DHS_BD_OBJECT, int, DHS_STATUS * );
extern void		dhsBdFrameInfo( DHS_BD_FRAME, char **, DHS_DATA_TYPE *, 
				int *, unsigned long *, void **, DHS_STATUS * );
extern DHS_BD_FRAME	dhsBdFrameNew( DHS_BD_OBJECT, const char *, int, 
				DHS_DATA_TYPE, int, const unsigned long *, 
				const void **, DHS_STATUS * );
extern DHS_TAG		dhsBdGet( DHS_CONNECT, const char *, DHS_BD_GET_TYPE, 
				void *, DHS_STATUS * );
extern char		*dhsBdName( DHS_CONNECT, DHS_STATUS * );
extern DHS_TAG		dhsBdPut( DHS_CONNECT, const char *, DHS_BD_PUT_TYPE, 
				DHS_BOOLEAN, ... );
extern void		dhsBdResponse( DHS_CONNECT, DHS_TAG, DHS_CMD_STATUS, 
				const char *, DHS_AV_LIST, 
				DHS_BD_GET_TYPE, ... );
extern DHS_CB_FN_PTR	dhsCallbackSet( DHS_CB_TYPE, DHS_CB_FN_PTR,
				DHS_STATUS * );
extern void		dhsCmdResponse( DHS_CONNECT, DHS_TAG, DHS_CMD_STATUS, 
				const char *, DHS_AV_LIST, DHS_STATUS * );
extern DHS_CONNECT	dhsConnect( const char *, const char *, void *, 
				DHS_STATUS * );
extern void		dhsConnectCount( int *, int *, DHS_STATUS * );
extern void		dhsConnectInfo( DHS_CONNECT, char **, char **, 
				DHS_STATUS * );
extern void		*dhsConUserDataGet( DHS_CONNECT, DHS_STATUS * );
extern void		dhsConUserDataSet( DHS_CONNECT, void *, DHS_STATUS * );
extern DHS_DEBUG_LEVEL	dhsDebugLevel( DHS_DEBUG_LEVEL, DHS_STATUS * );
extern void		dhsDisconnect( DHS_CONNECT, DHS_STATUS * );
extern void		dhsEventLoop( DHS_EL_TYPE , ... );
extern void		dhsEventLoopEnd( DHS_STATUS * );
extern void		dhsExit( DHS_STATUS * );
extern void		dhsInit( const char *, const int, DHS_STATUS * );
extern DHS_BOOLEAN	dhsIsConnected( DHS_CONNECT, DHS_STATUS * );
extern char		*dhsMessage( DHS_STATUS *, DHS_ERR_LEVEL *, 
				DHS_STATUS * );
extern void		dhsMessageClear( DHS_STATUS * );
extern void		dhsMsgCallback( DHS_CONNECT, DHS_TAG );
extern DHS_AV_LIST	dhsResponseGet( DHS_TAG, int, DHS_STATUS * );
extern void		dhsServerExit( DHS_STATUS * );
extern void		dhsSetTimeout( int );
extern DHS_CMD_STATUS	dhsStatus( DHS_TAG, char **, DHS_STATUS * );
extern DHS_BOOLEAN	dhsTagDone( DHS_TAG, DHS_STATUS * );
extern void		dhsTagFree( DHS_TAG, DHS_STATUS * );
extern void		*dhsUserDataGet( DHS_TAG, DHS_STATUS * );
extern void		dhsUserDataSet( DHS_TAG, void *, DHS_STATUS * );
extern void		dhsWait( int, const DHS_TAG *, DHS_STATUS * );

#ifdef __cplusplus
}
#endif
#endif /* DHS_H */
