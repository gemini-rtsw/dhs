static char rcsid[] = "$Id: putDataset.C,v 1.1.1.1 2002-11-24 20:25:01 brighton Exp $";
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
// dhs++/src/putDataset.C
//
// PURPOSE:
// Contains the routines for the cDhsPutDataset class.
//
// MEHTOD NAME(S)
// function-1 - description of the function
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:01  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.6  1998/02/16 23:50:59  nhill
// Added setting the user data pointers to NULL to avoid segmentation faults
// when an object is deleted.
//
// Revision 1.5  1997/08/26 21:18:40  nhill
// Changed references to dcdTag to dtTag.
//
// Revision 1.4  1997/08/19 19:53:40  nhill
// Added function cDhsPutDataset to create a put clients put dataset based
// on a servers put request.
//
// Revision 1.3  1997/06/12 16:54:02  nhill
// Fixed running the library without an event loop.
//
// Revision 1.2  1997/05/13 17:32:07  nhill
// Added a call to the cDhs::broadcast method to allow the callback function
// to resume if it is waiting for the tag to be set.
//
// Revision 1.1  1997/05/01 23:26:22  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_COMMAND
#define	DHS_NO_GET

#include "dhs++.H"
#include "genMutex.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutDataset::cDhsPutDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) putRequest (cDhsPutRequest) The put request to copy.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor to create a client's put dataset based on a servers put request
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

		cDhsPutDataset::cDhsPutDataset
(
    cDhsPutRequest		// (in)  The put request to copy.
		&putRequest
): cDhsClientDataset( putRequest )
{
    dpdType = putRequest.type();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsPutDataset::put
//
// INVOCATION:
// ds.put( connection, "data label", true, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) connect (cDhsConnection &) Connection to send data on.
// (>) dataLabel (char *) Data label for the data.
// (>) last (bool) Is this the last data for the data label.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Send a dataset to a server
//
// DESCRIPTION:
// Use the dhsBdPut function to send the dataset to the server.
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

void		cDhsPutDataset::put
(
    const cDhsConnection	// (in)  Connection to send data on.
		&connect,
    const char	*dataLabel, 	// (in)  Data label for the data.
    bool	last,		// (in)  Is this the last data for the label.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( dtTag != DHS_TAG_NULL )
    {
	dhsUserDataSet( dtTag, NULL, &status );
	dhsTagFree( dtTag, &status );
	dtTag = DHS_TAG_NULL;
    }

    label( dataLabel );
    ((cMutex *) dcdPMutex)->lock();

    if ( dpdType == DHS_BD_PT_DS || dpdType == DHS_BD_PT_DS_QL )
    {
	dtTag = dhsBdPut( connect.dcConnection, dataLabel, dpdType, 
		(DHS_BOOLEAN) last, ddsObject, this, &status );
    }
    else
    {
	dtTag = dhsBdPut( connect.dcConnection, dataLabel, dpdType, 
		(DHS_BOOLEAN) last, ddsPBuffer, ddsLength, this, &status );
    }

    ((cMutex *) dcdPMutex)->unlock();
}
