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
 * dhs/h/cmd.h
 *
 * PURPOSE:
 * Contains the interface to the command functions and data.
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/09/30 20:31:40  brighton
 * Imported sources
 *
 * Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:57:59  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.1  1998/08/04 16:17:15  nhill
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/

#ifndef H_CMD_H
#define	H_CMD_H

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdTag
 *
 * INVOCATION:
 * tag = dhsCmdTag( cmd );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) cmd (tDhsCmd *) Pointer to the command to convert.
 *
 * MACRO VALUE:
 * (DHS_TAG) DHS_TAG for the command object.
 *
 * PURPOSE:
 * Convert a pointer to a tDhsCmd command object into a command tag
 *
 * This is made look like a function just in case the mapping from
 * command object to command tag becomes more complicated in the future.
 *
 * DESCRIPTION:
 * Cast the pointer to the command object into an unsigned long.
 *
 * Note that whatever transformation is used, it must not be possible
 * for a tag to have the same value as a connection id.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsCmdPtr
 *
 * DEFICIENCIES:
 * Won't work if a pointer can't be cast into a DHS_TAG and back without
 * loss.
 *-
 ************************************************************************
 */

#define	dhsCmdTag( cmd )						\
			( (DHS_TAG) cmd )

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * dhsCmdPtr
 *
 * INVOCATION:
 * pCmd = dhsCmdPtr( tag );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) tag (DHS_TAG) Tag to convert to a command object pointer.
 *
 * MACRO VALUE:
 * (tDhsCmd *) Pointer to the command structure coresponding to the tag.
 *
 * PURPOSE:
 * Given a DHS_TAG, return a pointer to the command structure
 *
 * This is made to look like a function in case the mapping gets more
 * complicated in the future.
 *
 * DESCRIPTION:
 * Cast the DHS_TAG into a pointer to a tDhsCmd structure.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * - dhsCmdTag
 *
 * DEFICIENCIES:
 * Won't work if a pointer can't be cast into a DHS_TAG and back without
 * loss.
 *-
 ************************************************************************
 */

#define	dhsCmdPtr( tag )						\
	( (tDhsCmd * ) tag )


/*
 * Create an enumerated type to be used as the type parameter to the 
 * dhsCmdFind function.
 */

typedef enum
{
    DHS_CF_PTR			/* Find by pointer.			*/
} tDhsCmdFind;


/*
 * Create an enumerated type to indicate what kind of bulk data is associated
 * with a tDhsCmd structure.
 */

typedef enum
{
    DHS_BT_NONE,		/* No bulk data present.		*/
    DHS_BT_SDS,			/* Bulk data is in SDS structure.	*/
    DHS_BT_BUFFER		/* Bulk data is in a memory buffer.	*/
} tDhsBulk;


/*
 *  Create an enumerated type for the command flags.
 */

typedef enum
{
    DHS_CF_NONE = 0,		/* No command flags.			*/
    DHS_CF_CALLBACK = 1,	/* Execute the callback for repsonses?	*/
    DHS_CF_PRIORITY = 2		/* Is the mesasge 			*/
} tCmdFlag;


/*
 * Create a type to contain information about a command.
 */

struct dhsCmd
{
    DHS_AV_LIST		dcAvList;	/* The attribute value list.	*/
    unsigned long	dcBulkLen;	/* Bulk data buffer length.	*/
    void		*dcBulkPtr;	/* Pointer to a bulk data buffer.*/
    tDhsBulk		dcBulkType;	/* Type of bulk data.		*/
    boolean		dcCmdDone;	/* Is the command compelty done?*/
    DHS_BD_DATASET	dcDataset;	/* Bulk data dataset.		*/
    long		dcFlags;	/* Flags for the command.	*/
    DHS_BD_GET_TYPE	dcGetType;	/* The type of data being	*/
    					/* retrieved with dhsBdGet.	*/
    char		*dcName;	/* Command name.		*/
    IMP_SharedMemInfo	dcSharedMem;	/* Imp shared memory structure.	*/
    boolean		dcSharedMemUsed;/* is the shared memory in use?	*/
    IMP_MsgInfo		dcMsgInfo;	/* Message information structure*/
    unsigned long	dcMsgSize;	/* Size of the message.		*/
    tDhsMutex		dcMutex;	/* Mutex for the command.	*/
    int			dcNumResp;	/* Number of responses in the 	*/
    					/* dcResponse array.		*/
    int			dcRefCount;	/* Number of references to this.*/
    DHS_AV_LIST		*dcResponse;	/* The command responses.	*/
    int			dcRespSize;	/* Size of response array.	*/
    int			dcRetries;	/* Times messages was sent.	*/
    tDhsConnect		*dcSentTo;	/* Connection received the msg.	*/
    DHS_CMD_STATUS	dcState;	/* Current state of the cmd.	*/
    char		*dcString;	/* Command response string.	*/
    DHS_TAG		dcTag;		/* The command tag.		*/
    tDhsUsrMsg		dcType;		/* Command type.		*/
    void		*dcUserData;	/* The user data pointer.	*/
};


extern void		dhsBdResp( tDhsCmd *, DHS_CMD_STATUS, DHS_AV_LIST,
				char *, ulong, void *, DHS_STATUS * );
extern void		dhsCmdConnectLost( tDhsConnect *, DHS_STATUS * );
extern void		dhsCmdDecode( byte *, int, char **, DHS_AV_LIST *,
				DHS_CMD_STATUS *, byte **, unsigned long *, 
				DHS_STATUS * );
extern void		dhsCmdDelete( tDhsCmd *, DHS_STATUS * );
extern void		dhsCmdDeref( tDhsCmd *, DHS_STATUS * );
extern void		dhsCmdDisconnect( tDhsConnect *, DHS_STATUS * );
extern boolean		dhsCmdDone( tDhsCmd *, DHS_STATUS * );
extern void		dhsCmdExit( DHS_STATUS * );
extern tDhsCmd		*dhsCmdFind( tDhsCmdFind, DHS_STATUS *, ... );
extern void		dhsCmdInit( DHS_STATUS * );
void			dhsCmdMsgFmt( tDhsCmd *, void *, ulong, ulong, 
				ulong, DHS_STATUS * );
extern tDhsCmd		*dhsCmdNew( const char *, DHS_AV_LIST, void *, 
				tDhsUsrMsg, tCmdFlag, DHS_TAG, 
				DHS_CMD_STATUS, DHS_STATUS * );
extern void		dhsCmdRef( tDhsCmd *, DHS_STATUS * );
extern int		dhsCmdRefCount( tDhsCmd *, DHS_STATUS * );
extern void		dhsCmdResp( tDhsCmd *, DHS_CMD_STATUS, DHS_AV_LIST, 
				char *, DHS_STATUS * );
extern boolean		dhsCmdStart( tDhsCmd * );
#endif /* H_CMD_H */
