/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	gen/h/gen_msg.h
*
*   Purpose:
*	Defines the message structure.
*
*   Date		: Aug 23, 1991
*
*   SCCS data		: @(#)
*	Module Name	: gen_msg.h
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 02/28/97
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifndef _gen_msg_h
#define _gen_msg_h

/*
 *  A huge negative number is required for the MSG_ERRNO value.
 */

#define	MSG_ERRNO	(-9999)


/*
 *  Define the maximum message size.
 */

#define	MSG_MAX_LEN	2048


typedef struct	msg
{
    int		m_status;
    char	*m_format;
} MSG;

extern	void	msg_append(char *msg1, char *msg2);
extern	void	msg_clear(char *msg);
#ifdef __cplusplus
extern	void	msg_format(char *buffer, char *prefix, int num_msgs, 
			MSG *msgs, int status, void * args);
#else /* __cplusplus */
extern	void	msg_format();
#endif /* __cplusplus */

#endif /* _gen_msg_h */
