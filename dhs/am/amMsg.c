/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Module Name:	am/src/amMsg.c
*
*   Purpose:
*       Message routines.
*
*   Routines:
*       void    amFormatMessage	: Formats the message.
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: amMsg.c
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 02/03/00
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/10/27 SDJ  : Added a message for no devices.
*	98/11/23 JSD  : Added a message AM_E_TYPEGET_ENUM, bad media type.
*	98/12/06 SDJ  : Added messagse for booking devices.
*	99/02/23 SDJ  : Added AM_E_UNITNAME message.
*	99/03/05 NRH  : Added AM_E_MSTAGE_DIR message.
*	99/06/16 NRH  : Added AM_E_LOG_DEVGET_ENUM message.
*     2000/02/03 SDJ  : Added AM_E_MEDIA_OPTION_GET message.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
 
#include <sybfront.h>
#include <sybdb.h>
 
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"
#include "db.h"

#include "am.h"
#include "localAm.h"

#define	AM_PREFIX	"am"

char	amMsg[MSG_MAX_LEN];

static MSG	amMsgs[] = {
    { AM_CANT_MSBOOK,
           "Unable to book media stage." },
    { AM_CFILE_OPEN,
           "Configuration file '%s' has been opened." },
    { AM_NO_DEVICE,
           "No devices found." },
    { AM_NO_LOGICAL_DEV,
           "Unable to find a logcial device for `%s'." },
    { AM_NO_TYPE,
           "No media type match found." },

    { AM_E_BOOKCLEAR_ENUM,
           "Error: Book device table selection type %d is not valid." },

    { AM_E_CONFIG,
            "Error: parsing configuration file." },
    { AM_E_DB,
            "Error: SYBASE db library reports an error." },
    { AM_E_DB_LIB,
            "Error: CADC db library reports an error." },
    { AM_E_DEVICEGET_ENUM,
            "Error: device get type %d is not valid." },
    { AM_E_FILEGET_ENUM,
            "Error: File get type %d is not valid." },
    { AM_E_FILEUPDATE_ENUM,
            "Error: File update type %d is not valid." },
    { AM_E_INIT,
      	    "Error: am library has not been initialized." },
    { AM_E_LOG_DEVGET_ENUM,
            "Error: logical device get type %d is not valid." },
    { AM_E_MEDIA_OPTION_GET,
            "Error: Unable to get options for request %d, action %s.\n" },
    { AM_E_MEDIAGET_ENUM,
	    "Error: Media get type %d is not valid." },
    { AM_E_MEMORY,
            "Error: Memory allocation failure." },
    { AM_E_MSTAGE_DIR,
	    "Error: Could not find staging directory %s\n"
	    "in table `%s..%s'." },
    { AM_E_NOT_BOOKED,
           "Error: Unable to unbook device.`%s' Expect to book one device not %d." },
    { AM_E_STATUS,
            "Error: status string for table '%s', status '%c' \n"
	    "not available from table : %s..%s." },
    { AM_E_TYPEGET_ENUM,
	    "Error: Media Type get type %d is not valid." },
    { AM_E_UNITNAME,
      	    "Error: Unable to convert number appended to `%s' to an integer.\n"
      "       Unit root name used in conversion was `%s.'" },
    { AM_E_UNITSGET_ENUM,
	    "Error: Get media_units select type %d is not valid." },
    { AM_E_UNITUPDATE_ENUM,
            "Error: Unit update type %d is not valid." },
};

static int	amNumMsgs = sizeof( amMsgs ) / sizeof( MSG );

/*+
************************************************************************
*
*   Function:	amFormatMessage
*
*   Purpose:
*	Formats a message from the contents of the va_list.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	amFormatMessage
(
    int		status,		/* (in)  The message to print.		*/
    ...				/* (in)  The message arguments.		*/
)
{
    va_list	args;

    va_start( args, status );
    msg_format( amMsg, AM_PREFIX, amNumMsgs, amMsgs, status, args );

    va_end( args );
}
