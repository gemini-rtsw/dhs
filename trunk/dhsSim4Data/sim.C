static char rcsid[] = "$Id: sim.C,v 1.1.1.1 2002-11-24 20:30:08 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) <year>				(c) <year>
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsSim4Data/src/sim.C
//
// PURPOSE:
// Contains functions of the cSim class. 
//
// METHOD NAME(S)
// cSim::config
// cSim::dataServer
// cSim::identity
// cSim::imp
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:36  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:51:23  nhill
// Updated to use the latest version of the cDhsStatus class.
// Removed the cSim::imp and cSim::identity methods.
//
// Revision 1.1  1997/10/03 23:07:19  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "globals.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_config.h"
}

#include "sim.H"
#include "simError.H"
#include "simConnect.H"
#include "simCmdDefault.H"
#include "simCmdOldps.H"
#include "simCmdQls.H"
#include "simCmdSs.H"
#include "simPutDefault.H"
#include "simPutFits.H"


//
//  Initializations. 
//

char		*cSim::sdDataServer 	= NULL;
char		*cSim::sdDataServIP 	= NULL;

bool		cSim::sdNotify = FALSE;


//
//  Config keyword definitions. 
//

#define STV_KEYWORD_IDENTITY	"identity"
#define STV_KEYWORD_DATASERVER	"dataServer"
#define STV_KEYWORD_IMP		"imp"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cSim::config
//
// INVOCATION:
// cSim::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cSimStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the dhs library configuration parameters
//
// DESCRIPTION:
// Read in the identity keyword info and then the imp keyword info.
// And finally the server identity keyword information.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
 
void            cSim::config
(
    cSimStatus  &status         // (mod) Function return status.
)
{
    checkStat( status, return );


    check( cDhs::config( status ), status, return );
    checkConfig( config_get( STV_KEYWORD_DATASERVER, dataServer ), 
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cSim::dataServer
//
// INVOCATION:
// n.a. Called by the config_get function.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) Number of tokens on the configuration line.
// (>) tokens (char **) List of tokens from the configuration line.
//
// FUNCTION VALUE:
// (bool) False if an error occurs, true otherwise.
//
// PURPOSE:
// Read the identity line from a configuration file and store it.
//
// DESCRIPTION:
// Save the identity string.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
 
boolean         cSim::dataServer
(
    int         numTokens,      // (in)  Number of tokens.
    char        **tokens        // (in)  list of tokens.
)
{
    boolean     ret;            // Function return value.
    cSimStatus  status;         // Status code.
 
    if ( numTokens == 3 )
    {
        if ( sdDataServer != NULL )
        {
            free( sdDataServer );
	    free( sdDataServIP );
        }
        checkNull( sdDataServer = (char *) strsav( tokens[1] ), status, 
                return( FALSE ) );
        checkNull( sdDataServIP = (char *) strsav( tokens[2] ), status, 
                return( FALSE ) );
        ret = TRUE;
    }
    else
    {
        status.E_CONFIG_KWD( status, tokens[0] );
        ret = FALSE;
    }
 
    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cSim::exit
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) paramName (int) Descripton of the parameter
// (!) paramName (float) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
void		cSim::exit
(
    cSimStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );

//    cDhsHandler::forEach( delHandler );

    delete sdPDhs;
    sdPDhs = NULL;
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// cSim::init
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) paramName (int) Descripton of the parameter
// (!) paramName (float) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cSim::init
(
    cSimStatus	&status		// (mod) Function return status.
)
{
    DHS_STATUS	s( DHS_S_SUCCESS );
    DHS_DEBUG_LEVEL
		level( DHS_DEBUG_OFF );


//    sdRetry = false;
    checkStat( status, return );


    //
    //  Set the debug level
    //

    switch ( cSimStatus::debugLevel() )
    {
	case    cSimStatus::DEBUG_NONE:
	    level = DHS_DEBUG_OFF;
	    break;
	case    cSimStatus::DEBUG_MIN:
	    level = DHS_DEBUG_ON;
	    break;
	case    cSimStatus::DEBUG_FULL:
	    level = DHS_DEBUG_FULL;
	    break;
    }


    //
    //  Set up the dhs object.
    //

    checkNull( sdPDhs = new cDhs( level ), status, return );


    //
    //  Set up the error handlers.
    //

    checkNull( new cDhsErrorHandler<cSimError>, 
	    status, return );
    checkNull( new cDhsErrorHandler<cSimConnect>( DHS_S_CONNECT ),
	    status, return );
    checkNull( new cDhsErrorHandler<cSimConnect>( DHS_S_DISCONNECT ),
	    status, return );


    //
    // Set up the command handler. Default first, then others.
    //

    checkNull( new cDhsCmdHandler<cSimCmdDefault>( true ),
	    status, return );
    checkNull( new cDhsCmdHandler<cSimCmdOldps>( true, STV_CMD_OLDPS ),
	    status, return );
    checkNull( new cDhsCmdHandler<cSimCmdSs>( true, STV_CMD_SS ),
	    status, return );
    checkNull( new cDhsCmdHandler<cSimCmdQls>( true, STV_CMD_QLS ),
	    status, return );


    //
    // Set up the put handlers. Default first, then others.
    //

    checkNull( new cDhsPutHandler<cSimPutDefault>( true ),
	    status, return );
    checkNull( new cDhsPutHandler<cSimPutFits>( true, DHS_BD_PT_FITS ),
	    status, return );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cSim::start
//
// INVOCATION:
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) paramName (int) Descripton of the parameter
// (!) paramName (float) Descripton of the parameter
// (<) paramName (long) Descripton of the parameter
//
// FUNCTION VALUE:
// ([C type]) description of the return value.
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - other function name.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cSim::start
(
    cSimStatus  &status		// (mod) Function return status.
)
{
    DHS_STATUS	dhsStatus( DHS_S_SUCCESS );
				// DHS status, initialized to success. 


    checkStat( status, return );


    //
    //  Start the event loop.
    //

    checkDhs( sdPDhs->start( dhsStatus ), dhsStatus, status, return );
}
