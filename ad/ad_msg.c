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
*   Module Name:	ad/src/ad_msg.c
*
*   Purpose:
*	message formatting routines.
*
*   Routines:
*	void	ad_format_message	: Formats the message.
*
*   Date		: Nov 22, 1996.
*
*   Programmer		: Marc LeBlanc
*
*   Field SCCS data	: @(#)
*	Module Name	: ad_msg.c
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 05/10/00
*
*   Modification History:
*	97/06/20 SEC  :	Added AD_E_COMP and AD_E_FMT messages. 
*	97/08/25 SEC  :	Added AD_E_NO_REMOTE ande AD_E_SITE_ID messages.
*     2000/03/24 SDJ  : Added messages AD_E_MEDIUM_NAME, AD_E_MEDIUM_TYPE,
*			  AD_E_MFS_VOL_INFO.
*     2000/05/10 NRH  : Added AD_E_FM_TYPE_ENUM message.
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

#include "ad.h"
#include "local_ad.h"

#define	AD_PREFIX	"ad"

char	ad_msg[MSG_MAX_LEN];

MSG	ad_msgs[] = {
    { AD_CFILE_OPEN,
	"Configuration file `%s' has been opened." },
    { AD_NO_FILE,
       "There are no files available." },
    { AD_NO_VOLUME,
       "There are no volumes available." },
    
    { AD_SUCCESS,
       "AD operation completed successfully." },

    { AD_E_CANNOT_DEL,
	"Error: Cannot delete file from medium type %d." },
    { AD_E_COMP,
    	"Error: Invalid compression type in config file. Valid types listed in ad.h file." },
    { AD_E_CONFIG,
	"Error: parsing configuration file." },
    { AD_E_DB,
	"Error: SYBASE db library reports an error." },
    { AD_E_DB_LIB,
	"Error: CADC db library reports an error." },
    { AD_E_FM_TYPE_ENUM,
	"Error: Invalid value for an AD_FILEMEDIA variable: %d." },
    { AD_E_FMT,
    	"Error: Invalid format type in config file. Valid types listed in ad.h file." },
    { AD_E_INIT,
	"Error: Ad library is not initialized." },
    { AD_E_MEDIUM_ENUM,
	"Error: Invalid medium enumerated value: %d." },
    { AD_E_MEDIUM_TYPE,
	"Error: Unknown medium type: `%s'." },
    { AD_E_MEMORY,
       "Error: Memory allociation failure." },
    { AD_E_MFS_VOL_INFO,
      "Error: Invalid MFS volume type/format pair: %s/%s." },
    { AD_E_NO_REMOTE,
	"Error: No `remote' tables in use. Check configuration." },
    { AD_E_SITE_ID,
	"Error: Configured Site ID incorrect : `%s' ." },
    { AD_E_SELECT_TYPE,
	"Error: Invalid selection type." },
};

int	ad_num_msgs = sizeof( ad_msgs ) / sizeof( MSG );


/*+
************************************************************************
*
*   Synopsis:
*	void	ad_format_message( status, ... )
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	ad_format_message
( 
    int 	status, 	/* (in)  The message number to print.	*/
    ...				/* (in)  the message arguments.		*/
)
{
    va_list	args;


    va_start( args, status );
    msg_format( ad_msg, AD_PREFIX, ad_num_msgs, ad_msgs, status, args );

    va_end( args );
}
