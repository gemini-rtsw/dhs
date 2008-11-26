static char rcsid[] = "$Id: simPutFits.C,v 1.1.1.1 2002-11-24 20:30:13 brighton Exp $";
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
// simPutFits.C
//
// PURPOSE:
// Handle data put commands of type FITS.
//
// MEHTOD NAME(S)
// cSimPutFits::exec()
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:37  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1998/04/20 20:53:55  nhill
// Updated to use the latest version of the cDhsStatus class.
//
// Revision 1.1  1997/10/03 23:08:15  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//


//
// Include Files. 
//

#include "globals.H"
#include "simPutFits.H"



//
//***********************************************************************
//+
// FUNCTION NAME:
// cSimPutFits::exec
//
// INVOCATION:
// n.a. called by put handler
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Handles the FITS case of the dhs put command.
//
// DESCRIPTION:
// Not much different from purpose.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		cSimPutFits::exec
(
    void
)
{
    int		actWritten;	// Number of bytes actually written.
    char	*ip;		// Ip address of the connector.
    char	*datasetName;	// Dataset name. 
    DHS_STATUS  dhsStatus( DHS_S_SUCCESS );
                                // DHS library function return status.
    char	*id;		// Name of the connector.
    FILE	*fp;		// File Pointer. 
    char	*path; 		// Path to dataset. 
    cSimStatus
		status;		// Status.
    char	lastFlag;	// 'T' or 'F' - value of last flag.
 

    //
    //  Send a response to the client so it can get on with its life.
    //
 
    response( DHS_CS_BUSY, "working", dhsStatus );
 
 
    //
    //  Get information about who sent the data. 
    //
 
    checkDhs( dhsConnectInfo( connect(), &ip, &id, &dhsStatus ),
            dhsStatus, status, VOID );
 

    //
    //  Check that the label is not empty. Get last flag value.
    //

    if ( strlen( label() ) == 0 )
    {
	status.E_INVALID_PUT_LABEL( status, id, ip );
	response( DHS_CS_ERROR, "Invalid put received - QLS Simulator", 
		dhsStatus );
	return;
    }
    else
    {
	datasetName = strsav( label() );
    }
    lastFlag = ( ( last() == TRUE ) ? 'T' : 'F' );


/*******************
    //
    //  Write the file to /tmp
    //

    path = NULL;
    path = tmpnam( path );
    if ( ( fp = fopen( path,  "w" ) ) == NULL )
    {
	status.E_FILE_OPEN( status, path );
	status.sysErrno();
	response( DHS_CS_ERROR, "QLS Simulator could not write file", 
		dhsStatus );
	return;
    }
    if ( ( actWritten = fwrite( buffer(), 1, length(), fp ) ) != length() )
    {
	status.E_FILE_WRITE( status, path );
	status.sysErrno();
	response( DHS_CS_ERROR, "QLS Simulator could not write file", 
		dhsStatus );
	(void) fclose ( fp );
	return;
    }
    (void) fclose ( fp );
*******************/


    //
    //  Display a message.
    //
 
    status.S_PUT( status, datasetName, lastFlag, id, ip );
 
 
    //
    //  Respond that put is complete. 
    //
 
    response( DHS_CS_DONE, "Success - QLS Simulator", dhsStatus );
}
