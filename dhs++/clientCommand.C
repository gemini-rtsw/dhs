static char rcsid[] = "$Id: clientCommand.C,v 1.1.1.1 2002-11-24 20:24:58 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
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
// dhs++/src/clientCommand.C
//
// PURPOSE:
// Contains implementation code used by client commands.
//
// MEHTOD NAME(S)
// cDhsClientCommand::cDhsClientCommand - Constructor for a client command.
// cDhsClientCommand::~cDhsClientCommand - Destructor for a client command.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:01  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.8  1998/02/16 23:49:43  nhill
// Added setting the user data pointers to NULL to avoid segmentation faults
// when an object is deleted.
//
// Revision 1.7  1997/08/26 21:14:58  nhill
// Changed references to dccTag to dtTag.
//
// Revision 1.6  1997/07/10 19:04:57  nhill
// Removed the locks to ensure the command exists when the tag is assigned
// and replaced it with a deleted flag. The locks could cause a deadlock in
// some cases.
//
// Revision 1.5  1997/06/12 16:51:33  nhill
// Fixed running the library without the event loop.
//
// Revision 1.4  1997/05/13 17:23:38  nhill
// Added a broadcast to the apply method so the response callback function
// can wait until the dccTag data member has been set before proceeding.
//
// Revision 1.3  1997/05/01 23:24:11  nhill
// Move the apply method into here from the header file.
// Added a mutex to ensure the command can't be deleted before the apply
// method assigns the command data attributes.
//
// Revision 1.2  1997/04/15 20:04:41  nhill
// Added a constructor to allow a client command to be created from a server
// command.
// Added a function to abort a client command.
//
// Revision 1.1  1997/03/06 19:20:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <stdlib.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
}

#define	DHS_NO_BULK
#include "dhs++.H"
#include "genMutex.H"
static	cMutex	commandMutex;

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientCommand::cDhsClientCommand
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) command (char *) The command name.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a dhsClient command
//
// DESCRIPTION:
// Intialize the data members.
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

		cDhsClientCommand::cDhsClientCommand
(
    char	*command	// (in)  The command name.
) : cDhsAvList()
{
    dtTag = DHS_TAG_NULL;
    dccPExists = NULL;
    dccPConnect = NULL;
    dccCommand = strsav( command );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientCommand::cDhsClientCommand
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) serverCommand (cDhsServercommand &) The source server command.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a client command given a server command as the starting point
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

		cDhsClientCommand::cDhsClientCommand
(
    const cDhsServerCommand	// (in)  The source server command.
		&serverCommand
): cDhsAvList( serverCommand )
{
    dtTag = DHS_TAG_NULL;
    dccPExists = NULL;
    dccPConnect = NULL;
    dccCommand = strsav( serverCommand.name() );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientCommand::~cDhsClientCommand
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for a dhsClient command
//
// DESCRIPTION:
// Free the command tag if necessary.
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

		cDhsClientCommand::~cDhsClientCommand
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    commandMutex.lock();
    if ( dccPExists != NULL )
    {
	*dccPExists = false;
    }
    commandMutex.unlock();

    free( dccCommand );

    if ( dtTag != DHS_TAG_NULL )
    {
	dhsUserDataSet( dtTag, NULL, &status );
	dhsTagFree( dtTag, &status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientCommand::apply
//
// INVOCATION:
// Send a command to the server.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (cDhsConnection &) Connection to receive the command.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a command to a server
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
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cDhsClientCommand::apply
(
    const cDhsConnection	// (in)  Connection to send the command on.
    		&connect,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    bool	existsFlag( true );
    DHS_TAG	tag;


    if ( dtTag != DHS_TAG_NULL )
    {
	dhsUserDataSet( dtTag, NULL, &status );
	dhsTagFree( dtTag, &status );
	dtTag = DHS_TAG_NULL;
    }

    dccPExists = &existsFlag;

    dccPConnect = &connect;
    tag = dhsApply( connect.dcConnection, dccCommand, 
	    dalAvList, this, &status );

    commandMutex.lock();
    if ( existsFlag )
    {
	dtTag = tag;
	dccPExists = NULL;
    }
    commandMutex.unlock();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientCommand::cmdAbort
//
// INVOCATION:
// cmd.cmdAbort( dhsStatus );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) dhsStatus (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Abort a command
//
// DESCRIPTION:
// Create an abort command and send it to the server.
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

void		cDhsClientCommand::cmdAbort
(
    DHS_STATUS	&dhsStatus	// (mod) Function return status.
)
{
    cDhsClientCommand		// The abort command.
		abortCmd( "cmdAbort" );	


    abortCmd.add( "tag", DHS_DT_TAG, dtTag, dhsStatus );
    abortCmd.apply( *dccPConnect, dhsStatus );
    abortCmd.wait( dhsStatus );
}
