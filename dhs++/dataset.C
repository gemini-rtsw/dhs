static char rcsid[] = "$Id: dataset.C,v 1.1.1.1 2002-11-24 20:25:00 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998				(c) 1998
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
// dhs++/src/dataset.C
//
// PURPOSE:
// Contains the implementation of the dataset class.
//
// METHOD NAME(S)
// cDhsDataset::cDhsDataset - Constructor for the class.
// cDhsDataset::exportDataset - Export this dataset.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:01  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.1  1998/04/02 19:39:38  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define	DHS_NO_CLIENT
#define DHS_NO_SERVER
#define	DHS_NO_COMMAND
#include "dhs++.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::cDhsDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataset (cDhsDataset &) The original dataset.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy constructor for a dataset
//
// DESCRIPTION:
// Copy the underlying dataset.  None that is is expensive, and should
// be avoided.
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

		cDhsDataset::cDhsDataset
(
    cDhsDataset	&dataset	// (in)  The original dataset.
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    ddsLength = dataset.ddsLength;
    ddsBufferFree = dataset.ddsBufferFree;
    if ( ddsBufferFree )
    {
	ddsPBuffer = malloc( ddsLength );
	memcpy( ddsPBuffer, dataset.ddsPBuffer, ddsLength );
    }
    else
    {
	ddsPBuffer = dataset.ddsPBuffer;
    }

    if ( dataset.ddsObject != DHS_BD_DATASET_NULL )
    {
	ddsObject = dhsBdDsCopy( dataset.ddsObject, &status );
	ddsObjectFree = true;
	if ( status != DHS_S_SUCCESS )
	{
	    throw status;
	}
    }
    else
    {
	ddsObjectFree = false;
	ddsObject = DHS_BD_DATASET_NULL;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::exportDataset
//
// INVOCATION:
// dataset.exportDataset( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Export a dataset into the data buffer
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

void		cDhsDataset::exportDataset
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( status != DHS_S_SUCCESS )
    {
	return;
    }

    if ( ddsPBuffer != NULL && ddsBufferFree )
    {
	free( ddsPBuffer );
    }

    ddsLength = dhsBdDsSize( ddsObject, &status );
    if ( status == DHS_S_SUCCESS )
    {
	ddsPBuffer = malloc( ddsLength );
	ddsBufferFree = true;
	dhsBdDsExport( ddsObject, ddsPBuffer, ddsLength, &status );
    }
}
