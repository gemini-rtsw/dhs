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
*   Module Name:	tp/src/config.c
*
*   Purpose:
*	Initailizes the configuration for the tp library.
*
*   Routines:
*	static boolean	add_type	: Get the types from the config file.
*	int		tp_config_init	: Initialize the tp configuration.
*
*   Date		: Feb 10, 1998
*
*   Field SCCS data	: @(#)
*	Module Name	: config.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 03/23/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*     99/03/23 SDJ    : Changed the TP_E_CONFIG msg to print out the
*                       name of the configuration file.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
#include "gen_eptr.h"

#include "tp.h"
#include "local_tp.h"

#define KEY_TYPE	"type"

static boolean	add_type( int, char ** );


/*+
************************************************************************
*
*   Function:  	add_type 
*
*   Purpose:
*	Add types to the tp_scripts list.
*
*   Values Returned:
*       boolean TRUE            : Data parsed & inserted correctly.
*       boolean FALSE           : Data failed to parse.
*
************************************************************************
-*/
 
static boolean	add_type 
(
    int         num_tokens,     /* (in)  Number of tokens on input line.*/
    char        **tokens        /* (in)  The list of tokens.            */
)
{
    TP_SCRIPTS	*tp_script;

    if ( num_tokens < 4 || num_tokens > 5 )
    {
	return( FALSE );
    }

    TP_CHECK_NULL_CONFIG( tp_script = gen_alloc( sizeof( TP_SCRIPTS ) ) );

    TP_CHECK_NULL_CONFIG( tp_script->tp_type = strsav( tokens[1] ) );
    TP_CHECK_NULL_CONFIG( tp_script->tp_ingest = strsav( tokens[2] ) );
    TP_CHECK_NULL_CONFIG( tp_script->tp_retrieve = strsav( tokens[3] ) );

    if ( num_tokens == 5 )
    {
	TP_CHECK_NULL( tp_script->tp_name = strsav( tokens[4] ) );
    }
    else
    {
	tp_script->tp_name = NULL;
    }

    if ( tp_scripts_list == NULL )
    {
        TP_CHECK_NULL_CONFIG( tp_scripts_list = eptr_alloc( 4 ) );
    }
    TP_CHECK_NULL_CONFIG( eptr_insert( tp_scripts_list,
    	    eptrlen( tp_scripts_list ), tp_script ) );

    return( TRUE );
}

/*+
************************************************************************
*
*   Function:	tp_config_init
*
*   Purpose:
*	Initialize the configuration.
*
*   Values Returned:
*	int	TP_SUCCESS	    : Normal completion.
*	int	TP_E_CONFIG	    : Error reading config file.
*
************************************************************************
-*/

int		tp_config_init
(
    void
)
{
    char	*config_file;


    TP_CHECK_CONFIG( config_open( TP_LIBRARY, &config_file ), "" );
    tp_format_message( TP_CFILE_OPEN, config_file );

    tp_scripts_list = NULL;

    /*
     *  Parse the file for required keywords.
     */

    TP_CHECK_CONFIG( config_get( KEY_TYPE, add_type ), config_file );

    /*
     *  Close config file, sort lists. 
     */

    config_close();

    (void) eptr_sort( tp_scripts_list, tp_type_compare_sort );

    return( TP_SUCCESS );
}
