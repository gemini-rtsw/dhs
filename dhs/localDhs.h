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
 * dhsLibrary/h/localDhs.h
 *
 * PURPOSE:
 * This header file contains the definitions only required internally by
 * the DHS library.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:50  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.34  2001/03/27 20:09:46  nhill
 * Changed the name of the thread.h file to localThread.h.
 *
 * Revision 1.33  1998/08/04 16:16:36  nhill
 * Rearranged the code to be more object-oriented.
 *
 * Revision 1.32  1998/06/02 18:45:15  nhill
 * Removed function dhsConnectVerify.
 *
 * Revision 1.31  1998/05/25 19:54:38  nhill
 * Added dcGetType to the tDhsCmd structure.
 *
 * Revision 1.30  1998/05/13 22:31:01  nhill
 * Added a flag to indicate when processing of a command is completely done.
 *
 * Revision 1.29  1998/05/11 16:31:29  nhill
 * Added an automatic broadcast to the connection callback macro.
 *
 * Revision 1.28  1998/03/05 23:37:58  nhill
 * Changed function dhsConnectDelete to dhsConnectDeref.
 * Added a separate counter of opens and references.
 *
 * Revision 1.27  1998/02/24 01:09:27  nhill
 * Added dlExitCount to the dhsLocal structure.
 * Added functions dhsConnectCloseAll and dhsElNoExit.
 *
 * Revision 1.26  1998/02/16 23:46:35  nhill
 * Added a void message.
 * Added function dhsConnectUnlock
 *
 * Revision 1.25  1998/01/20 18:26:17  nhill
 * Moved the tDhsConState enumerated type to file dhs.h.
 * Added macro connectCallback to execute the connection callback function.
 *
 * Revision 1.24  1997/12/24 19:31:38  nhill
 * Added the dlLockout data member to the dhsLocal structure.
 *
 * Revision 1.23  1997/12/18 22:17:13  nhill
 * Made the changes required for automatic reconnection after server
 * crashes.
 *
 * Revision 1.22  1997/11/30 21:10:22  nhill
 * Added the DHS_CON_LOST connection status.
 *
 * Revision 1.21  1997/09/26 18:03:59  nhill
 * Added the dhsDisconnected function.
 *
 * Revision 1.20  1997/09/17 20:07:56  nhill
 * Added function dhsCmdRefCount.
 * Added the imp name to the dhsLocal structure.
 *
 * Revision 1.19  1997/08/27 22:04:13  nhill
 * Added the dhsLocalIsInit flag.
 *
 * Revision 1.18  1997/08/19 19:48:52  nhill
 * Changed the impCheckNoRet macro to take a mutex to be unlock when an error
 * occurs.
 *
 * Revision 1.17  1997/08/06 16:15:56  nhill
 * Added an ers error handler.
 *
 * Revision 1.16  1997/06/09 21:21:15  nhill
 * Changed the hp unix symbol to hpux from HPUX.
 *
 * Revision 1.15  1997/04/15 19:19:53  nhill
 * Added macro impCheckRet.
 * Added reference count to connection list and event loop.
 *
 * Revision 1.14  1997/03/26 16:59:52  nhill
 * Modified to support HP and VxWorks.
 *
 * Revision 1.13  1997/02/17 20:16:27  nhill
 * checkin for install.
 *
 * Revision 1.12  1997/02/13 21:58:46  nhill
 * Checkin for install.
 *
 * Revision 1.11  1997/02/12 22:05:47  nhill
 * Checkin for install.
 *
 * Revision 1.10  1997/02/04 19:42:19  nhill
 * Checkin for install.
 *
 * Revision 1.9  1997/01/29 18:18:59  nhill
 * checkin for install.
 *
 * Revision 1.8  1997/01/28 19:00:25  nhill
 * checkin before install.
 *
 * Revision 1.7  1997/01/27 22:35:29  nhill
 * Checkin before install.
 *
 * Revision 1.6  1997/01/22 21:36:10  nhill
 * Stable Checkin.
 *
 * Revision 1.5  1997/01/20 17:15:30  nhill
 * checkpoint.
 *
 * Revision 1.3  1997/01/14 20:25:39  nhill
 * Stable check in.
 *
 * Revision 1.2  1997/01/13 17:59:20  nhill
 * Reasonably stable checkpoint.
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef LOCALDHS_H
#define	LOCALDHS_H


/*
 ************************************************************************
 *  Include files.
 ************************************************************************
 */

/*
 *  Standard include files.
 *
#ifndef ulong
typedef unsigned long ulong;
#endif
*/

#include <stdarg.h>
#include <sys/types.h>


/*
 *  Drama include files.
 */

#include "drama.h"
#include "imp.h"
#include "sds.h"
#include "Ers.h"


/*
 *  Standard CADC include files.
 */

#include "gen_types.h"
#include "gen_eptr.h"


/*
 *  DHS library specific include files.
 */


/*
 ************************************************************************
 *  Simple data types types
 ************************************************************************
 */

#if	defined( VxWorks ) || defined( hpux )
typedef unsigned long	ulong;	/* Unsigned long for VxWorks.		*/
#endif	/* VxWorks || hpux */
#if	defined( VxWorks )
typedef unsigned int	uint;	/* Unsigned long for VxWorks.		*/
#endif	/* VxWorks */
typedef struct dhsCmd tDhsCmd;
typedef struct dhsConnect tDhsConnect;


#include "dhs.h"
#include "cadcMsg.h"
#include "localThread.h"
#include "mutex.h"
#include "cond.h"
#include "msg.h"
#include "tsd.h"
#include "eventLoop.h"
#include "connect.h"
#include "cmd.h"
#include "bulkTransfer.h"


/*
 ************************************************************************
 *  Constant definitions.
 ************************************************************************
 */

#define	VOID			/* Return value for void functions.	*/
#define	HEART_RATE	60	/* Rate of heartbeat messages.		*/
#define	SDS_NAME_SIZE	15	/* Maximum allowed size of sds names.	*/


/*
 ************************************************************************
 *  Macro definitions.
 ************************************************************************
 */

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
 * (>) status (DHS_STATUS) Current status value.
 * (>) val (unkown) Value to return if the DHS library is not initialized.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Verify that the DHS library has been initialized with the dhsInit function.
 *
 * If the library has not been initialized an error status is set and the
 * function will return with the specified return value. A special return
 * value of VOID has been defined as the return value of void functions.
 *
 * DESCRIPTION:
 * Check that the DHS library has been initialized, and if it hasn't, 
 * set the status value to DHS_E_INIT and cause the function to return
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

#define	isInit( status, val )						\
	if ( ! dhsInitTest() )						\
	{								\
	    status = DHS_E_INIT;					\
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
 * (>) status (DHS_STATUS) The return status of the fn parameter.
 * (>) val (unknown) The value to return should the function fail.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute the specified function and verify the return status is success
 *
 * If the return status is not DHS_S_SUCCESS, and error message is 
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

#define	check( fn, status, val )					\
	fn;								\
	if ( status != DHS_S_SUCCESS )					\
	{								\
	    dhsMsgFmt( DHS_S_FN_EXIT, DHS_DEBUG_FULL, dhsFnName );	\
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

#define	nullCheck( ptr, status, val )					\
	if ( ( ptr ) == NULL )						\
	{								\
	    dhsMsgFmt( DHS_E_MEMORY, DHS_DEBUG_OFF );			\
	    status = DHS_E_MEMORY;					\
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

#define	statCheck( status, val )					\
	if ( ( status ) != DHS_S_SUCCESS )				\
	{								\
	    return val ;						\
	}


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * impCheckRet
 *
 * INVOCATION:
 * impCheckRet( impFunction( args, &impStatus ), impStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (function) IMP function.
 * (>) impStatus (IMP_Status) IMP function return status.
 * (>) status (DHS_STATUS) DHS library function return status.
 * (>) val (unknown) Value to return if the IMP function fails.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Check that an IMP function succeeded
 *
 * This assumes the IMP function has already been executed. The function
 * is in the parameter list only so that it can be used for docuementing
 * the error.
 *
 * If the imp status value is not IMP_STATUS_OK, the DHS library status value
 * will be set to DHS_E_IMP and the function will return the value 
 * specified in the "val" parameter. A special value of "VOID" can be
 * specified as the return value of void functions.
 *
 * DESCRIPTION:
 * Check the imp status value, and if it isn't IMP_STATUS_OK, format a 
 * message, set the return status to DHS_E_IMP, and return the specified 
 * value.
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

#define	impCheckRet( fn, impStatus, status, val )			\
	if ( impStatus != IMP_STATUS_OK )				\
	{								\
	    StatusType	s;						\
	    dhsMsgFmt( DHS_E_IMP, DHS_DEBUG_OFF, #fn, impStatus, 	\
		    ImpErrorText( impStatus ) );			\
	    (status) = DHS_E_IMP;					\
	    s = 0;							\
	    ErsFlush( &s );						\
	}								\
	if ( (status) != DHS_S_SUCCESS )				\
	{								\
	    return val;							\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * impCheck
 *
 * INVOCATION:
 * impCheck( impFunction( args, &impStatus ), impStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (function) IMP function to execute.
 * (>) impStatus (IMP_Status) IMP function return status.
 * (>) status (DHS_STATUS) DHS library function return status.
 * (>) val (unknown) Value to return if the IMP function fails.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute an IMP function and then check that it succeeded
 *
 * DESCRIPTION:
 * Execute the function and then use the impCheckRet macro to check the
 * return stuats.
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

#define	impCheck( fn, impStatus, status, val )				\
	dhsMutexLock( &(dhsLocal.dlImpMutex), &(status) )		\
	fn;								\
	dhsMutexUnlock( &(dhsLocal.dlImpMutex), &(status) )		\
	impCheckRet( fn, impStatus, status, val );


/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * impCheckNoRet
 *
 * INVOCATION:
 * impCheckNoRet( impFunction( args, &impStatus ), impStatus, status );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (function) IMP function to execute.
 * (>) pMutex (tDhsMutex *) Mutex locked to protect the function.
 * (>) impStatus (IMP_Status) IMP function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Check that an IMP function succeeded
 *
 * Execute an imp function and format an error message if it doesn't
 * succeed.
 *
 * DESCRIPTION:
 * Check the imp status value, and if it isn't IMP_STATUS_OK, format a 
 * message.
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

#define	impCheckNoRet( fn, pMutex, impStatus )				\
	fn;								\
	if ( impStatus != IMP_STATUS_OK )				\
	{								\
	    StatusType	s;						\
	    dhsMsgFmt( DHS_E_IMP, DHS_DEBUG_OFF, #fn, impStatus, 	\
		    ImpErrorText( impStatus ) );			\
	    s = 0;							\
	    if ( pMutex != NULL )					\
	    {								\
		dhsMutexUnlock( pMutex, pStatus );			\
		ErsFlush( &s );						\
		dhsMutexLock( pMutex, pStatus );			\
	    }								\
	    else							\
	    {								\
		ErsFlush( &s );						\
	    }								\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sdsCheck
 *
 * INVOCATION:
 * sdsCheck( sdsFunction( args, &sdsStatus ), sdsStatus, status, val );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) fn (sds function) Sds function to execute.
 * (>) sdsStatus (StatusType) SDS function return status.
 * (>) status (DHS_STATUS) DHS library function return status.
 * (>) val (unknown) Value to return if the SDS function fails.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Execute an SDS function and check for errors
 *
 * If the SDS function fails, the DHS library function return status is 
 * set to DHS_E_SDS, and message is formatted, and the function returns
 * the value specified in the "val" parameter. A special return value
 * of "VOID" is provided for void functions.
 *
 * DESCRIPTION:
 * Execute the SDS function, check the return status and if it isn't 
 * SDS__OK, format an error message, set the DHS library function return
 * status and return the specified value.
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

#define	sdsCheck( fn, sdsStatus, status, val )				\
	fn;								\
	if ( sdsStatus != SDS__OK )					\
	{								\
	    dhsMsgFmt( DHS_E_SDS, DHS_DEBUG_OFF, #fn, sdsStatus );	\
	    status = DHS_E_SDS;						\
	    return val;							\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsSdsId
 *
 * INVOCATION:
 * sdsId = dhsSdsId( avList );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) avList (DHS_AV_LIST) An attribute value list identifier.
 *
 * MACRO VALUE:
 * (SdsIdType) SDS id coresponding to the attribute value list.
 *
 * PURPOSE:
 * Given an attribute value list id, return the SDS object id
 *
 * This is made to look like a function just in case the mapping
 * becomes more complicated in the future.
 *
 * DESCRIPTION:
 * Cast the attribute value list id into an SDS id.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsSdsAvList
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsSdsId( avList ) ( (SdsIdType) avList )

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsSdsAvList
 *
 * INVOCATION:
 * avList = dhsSdsAvList( sdsId );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) sdsId (SdsIdType) SDS object id to map.
 *
 * MACRO VALUE:
 * (DHS_AV_LIST) Attribute value list coresponding to the SDS id.
 *
 * PURPOSE:
 * Given an SDS object id, return a DHS_AV_LIST object for the same object
 *
 * This is made to look like a function just in case the mapping becomes
 * more complicated in the future.
 *
 * DESCRIPTION:
 * Cast the SDS id into a DHS_AV_LIST.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsSdsId
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

#define	dhsSdsAvList( sdsId ) ( (DHS_AV_LIST) sdsId )


/*
 ************************************************************************
 *  Structure types
 ************************************************************************
 */


/*
 * Create a type to contain data global to the DHS library, but not public
 * outside the DHS library.
 */

typedef struct dhsLocal
{
    unsigned long	dlBuffSize;	/* Imp buffer size.		*/
    DHS_CB_FN_PTR	dlCBFCommand;	/* Command callback function.	*/
    DHS_CB_FN_PTR	dlCBFConnect;	/* Connection callback function.*/
    DHS_CB_FN_PTR	dlCBFError;	/* Error callback function.	*/
    DHS_CB_FN_PTR	dlCBFGet;	/* Client get callback function.*/
    DHS_CB_FN_PTR	dlCBFPut;	/* Client put callback function.*/
    DHS_CB_FN_PTR	dlCBFResponse;	/* Response callback function.	*/
    DHS_CB_FN_PTR	dlCBFServerGet;	/* Server get callback function.*/
    DHS_CB_FN_PTR	dlCBFServerPut;	/* Server put callback function.*/
    int			dlExitCount;	/* El exit message counter.	*/
    IMP_ID		dlImpId;	/* The IMP process id.		*/
    IMP_ID		dlImpIdIn;	/* The IMP process id.		*/
    tDhsMutex		dlImpMutex;	/* Make sure IMP isn't used by	*/
    					/* mutiple threads.		*/
    char		dlImpName[IMP_MAX_NAME_LEN+1];
    boolean		dlLockout;	/* Lock out new connections?	*/
    tDhsMsg		dlMsg;		/* First message structure.	*/
    char		dlMsgString[MSG_MAX_LEN];
    int			dlTimeout;	/* Number of seconds to wait for*/
    					/* a reconnection.		*/
} tDhsLocal;


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern void		dhsInitSet( boolean );
extern boolean		dhsInitTest( void );
extern void		dhsLocalExit( DHS_STATUS * );
extern void		dhsLocalInit( DHS_STATUS * );
extern unsigned long	crc_buffer( int, char * );


/*
 ************************************************************************
 *  Global data declarations.
 ************************************************************************
 */

extern	tDhsLocal	dhsLocal;
extern	boolean		dhsLocalIsInit;

#endif /* LOCALDHS_H */
