/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) 1998.				(c) 1998
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
 * dhs/h/connect.h
 *
 * PURPOSE:
 * Whatever
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:43  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.3  2001/03/27 20:03:40  nhill
 * Added value DHS_CF_CONNECTING to the tDhsConFind enumerated type.
 *
 * Revision 1.2  1999/02/12 18:13:43  nhill
 * Added a new search type for dhsConnectFind
 *
 * Revision 1.1  1998/08/04 16:17:35  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef CONNECT_H
#define	CONNECT_H

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * connectCallback
 *
 * INVOCATION:
 * connectCallback( pConnect );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) pConnect (tDhsConnect *) The connection to send to the callback.
 * (!) pStatus (DHS_STATUS *) The function return status.
 *
 * MACRO VALUE:
 * None.
 *
 * PURPOSE:
 * Try to call the connection callback function.
 *
 * DESCRIPTION:
 * If the connection callback function has been set, call it.
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

#define	connectCallback( pConnect, pStatus )				\
	if ( dhsLocal.dlCBFConnect != NULL )				\
	{								\
	    dhsConnectRef( pConnect, pStatus );				\
	    dhsElBroadcast( pStatus );					\
	    dhsLocal.dlCBFConnect( dhsConnectId( pConnect ), 		\
		    pConnect->dcState );				\
	    dhsConnectDeref( pConnect, pStatus );			\
	}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectId
 *
 * INVOCATION:
 * connect = dhsConnectId( connection );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connection (tDhsConnect *) Pointer to a connection object.
 *
 * MACRO VALUE:
 * (DHS_CONNECT) The connection id coresponding to the connection structure.
 *
 * PURPOSE:
 * Given a connection structure, return a connection id
 *
 * This is made to look like a function call in case the mapping from 
 * connection structure to connection id gets more complicated in the
 * future.
 *
 * DESCRIPTION:
 * Cast the pointer to the connection structure into a DHS_CONNECT.
 *
 * Note that whatever transformation is used, it must not be possible
 * for a connection id to have the same value as a command tag.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsConnectPtr
 *
 * DEFICIENCIES:
 * Won't work if a pointer can't be cast into a DHS_CONNECT and back without
 * loss.
 *-
 ************************************************************************
 */

#define	dhsConnectId( connection )					\
		    ( (DHS_CONNECT) connection )

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsConnectPtr
 *
 * INVOCATION:
 * pConnection = dhsConnectPtr( connect );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) connection (DHS_CONNECT) Connection id.
 *
 * MACRO VALUE:
 * (tDhsConnect *) Pointer to the connect strucure coresponding to the 
 *			connection id.
 *
 * PURPOSE:
 * Given a connection id, return a pointer to the connection structure
 *
 * This is made to look like a function in case the mapping gets more
 * complicated in the future.
 *
 * DESCRIPTION:
 * Cast the connection id into a pointer to a connection structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsConnectId
 *
 * DEFICIENCIES:
 * Won't work if a pointer can't be cast into a DHS_CONNECT and back without
 * loss.
 *-
 ************************************************************************
 */

#define	dhsConnectPtr( connection )					\
		    ( (tDhsConnect *) connection )


/*
 * Create an enumerated type to be used as the type parameter to the 
 * dhsConnectFind function.
 */

typedef enum
{
    DHS_CF_CON_ID,		/* Find by DHS id.			*/
    DHS_CF_CON_ID2,		/* Find by DHS id.			*/
    DHS_CF_CONNECTING,		/* Find first that is in the process of	*/
    				/* connecting.				*/
    DHS_CF_DISCONNECTING,	/* Find by task id a disconnecting 	*/
				/* connection.				*/
    DHS_CF_IMP_ID,		/* Find by IMP connection id.		*/
    DHS_CF_TASK_ID		/* Find by task id.			*/
} tDhsConFind;


/*
 * Create a type to contain information about a connection.
 */

struct dhsConnect
{
    boolean		dcClientConnect;/* Connection was initiated by	*/
    					/* an outside client.		*/
    IMP_ConnectInfo	dcConnectInfo;	/* IMP connectinfo structure.	*/
    tDhsConnect		*dcDupConnect;	/* Pointer to a duplicate 	*/
    					/*  connection.			*/
    int			dcImpConnect;	/* IMP connection id.		*/
    char		*dcIpAddress;	/* Address of the client system.*/
    IMP_AbsTime		dcLastUsed;	/* Time the connection was last	*/
    					/* used.			*/
    tDhsMutex		dcMutex;	/* Connection mutex.		*/
    char		*dcName;	/* Name of the client system.	*/
    boolean		dcNotified;	/* Has a notify been received.	*/
    int			dcOpenCount;	/* Open counter.		*/
    int			dcRefCount;	/* Reference counter.		*/
    tDhsConState 	dcState;	/* State of the connection.	*/
    int			dcTimeout;	/* Reconnect timeout for this	*/
    					/* connection.			*/
    void		*dcUserData;	/* Connection user data pointer.*/
};


/*
 ************************************************************************
 *  External function declarations.
 ************************************************************************
 */

extern boolean		dhsConnectAllClosed( boolean * );
extern void		dhsConnectBreak( tDhsConnect *, DHS_STATUS * );
extern void		dhsConnectCloseAll( DHS_STATUS * );
extern void		dhsConnectDeref( tDhsConnect *, DHS_STATUS * );
extern boolean		dhsConnected( tDhsConnect * );
extern tDhsConnect	*dhsConnectFind( tDhsConFind, DHS_STATUS *, ... );
extern void		dhsConnectLock( tDhsConnect *, DHS_STATUS * );
extern void		dhsConnectLost( IMP_TaskID *, DHS_STATUS * );
extern void		dhsConnectMake( tDhsConnect *, DHS_STATUS * );
extern tDhsConnect	*dhsConnectNew( const char *, const char *, void *, 
				boolean, DHS_STATUS * );
extern void		dhsConnectRef( tDhsConnect *, DHS_STATUS * );
extern void		dhsConnectReqClose( DHS_STATUS * );
extern void		dhsConnectSend( tDhsConnect *, tDhsCmd *, 
				DHS_STATUS * );
extern void		dhsConnectUnlock( tDhsConnect *, DHS_STATUS * );
extern boolean		dhsDisconnected( tDhsConnect * );

#endif /* CONNECT_H */
