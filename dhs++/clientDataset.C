static char rcsid[] = "$Id: clientDataset.C,v 1.1.1.1 2002-11-24 20:24:59 brighton Exp $";
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
// dhs++/src/clientDataset.C
//
// PURPOSE:
// Contains client dataset routines for the DHS++ library.
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
// Revision 1.5  1998/02/16 23:50:18  nhill
// Added setting the user data pointers to NULL to avoid segmentation faults
// when an object is deleted.
//
// Revision 1.4  1997/08/26 21:16:15  nhill
// Changed references to dcdTag to dtTag.
//
// Revision 1.3  1997/08/19 19:52:47  nhill
// Added function cDhsClientDataset to create a dataset based on an existing
// cDhsDataset structure.
//
// Revision 1.2  1997/06/12 16:52:35  nhill
// Added a function to return a datasets label.
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
#define	DHS_NO_SERVER
#define	DHS_NO_GET
#define	DHS_NO_PUT

#include "dhs++.H"
#include "genMutex.H"
extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::cDhsClientDataset
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
// constructor for the cDhsClientDataset class
//
// DESCRIPTION:
// Call the cDhsClientDataset constructor.
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

		cDhsClientDataset::cDhsClientDataset
(
    void
)
{
    dtTag = DHS_TAG_NULL;
    dcdPMutex = new cMutex;
    dcdLabel = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::cDhsClientDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataset (DHS_BD_DATASET) Initial dataset id.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// constructor for the cDhsClientDataset class
//
// DESCRIPTION:
// Call the cDhsDataset constructor.
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

		cDhsClientDataset::cDhsClientDataset
(
    DHS_BD_DATASET		// (in)  Initial dataset id.
	    dataset
) : cDhsDataset( dataset )
{
    dtTag = DHS_TAG_NULL;
    dcdPMutex = new cMutex;
    dcdLabel = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::cDhsClientDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) origDataset (cDhsClientDataset &) The original dataset.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Copy constructor for the cDhsClientDataset class.
//
// DESCRIPTION:
// Copy the tag a create a new mutex.
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

		cDhsClientDataset::cDhsClientDataset
(
    cDhsClientDataset		// (in)  The original dataset.
		&origDataset
) : cDhsDataset( origDataset )
{
    dtTag = origDataset.dtTag;
    dcdPMutex = new cMutex;
    if ( origDataset.dcdLabel == NULL )
    {
	dcdLabel = NULL;
    }
    else
    {
	dcdLabel = strsav( origDataset.dcdLabel );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::cDhsClientDataset
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) origDataset (cDhsDataset &) The original dataset.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Initialize a client dataset based on a dataset structure
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

		cDhsClientDataset::cDhsClientDataset
(
    cDhsDataset	&origDataset	// (in)  The original dataset.
) : cDhsDataset( origDataset )
{
    dtTag = DHS_TAG_NULL;
    dcdPMutex = new cMutex;
    dcdLabel = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::~cDhsClientDataset
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
// Destructor for the cDhsClientDataset class
//
// DESCRIPTION:
// If a tag has been created for this dataset, free the tag.
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

		cDhsClientDataset::~cDhsClientDataset
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    ((cMutex *) dcdPMutex)->lock();

    if ( dtTag != DHS_TAG_NULL )
    {
	dhsUserDataSet( dtTag, NULL, &status );
	dhsTagFree( dtTag, &status );
    }

    if ( dcdLabel != NULL )
    {
	gen_free( dcdLabel );
    }

    delete (cMutex *) dcdPMutex;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::label
//
// INVOCATION:
// dataset.label( labelName );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) labelName (const char *) Name of the label for the datset.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Set the label for a dataset
//
// DESCRIPTION:
// Trivial.
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

void		cDhsClientDataset::label
(
    const char	*labelName	// (in)  Name of the label for the datset.
)
{
    if ( dcdLabel != NULL )
    {
	gen_free( dcdLabel );
    }
    
    dcdLabel = strsav( labelName );
}
