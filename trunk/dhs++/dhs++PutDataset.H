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
// dhs++/h/dhs++PutDataset.H
//
// PURPOSE:
// Contains the the cDhsPutDataset class.
//
// CLASSES:
// <class> - Description
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.6  1997/08/19 20:04:57  nhill
// Added a new constructor.
//
// Revision 1.5  1997/05/01 23:16:36  nhill
// Move the put method to a C file.
//
// Revision 1.4  1997/04/24 16:56:39  nhill
// Added "const" keywords to function prototypes.
//
// Revision 1.3  1997/02/28 00:54:14  nhill
// Checkin for install.
//
// Revision 1.2  1997/02/24 19:23:10  nhill
// checkin for install.
//
// Revision 1.1  1997/02/21 21:46:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSPPPUTDATASET_H
#define	DHSPPPUTDATASET_H

class	cDhsPutRequest;

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsPutDataset
//
// PUBLIC METHODS: 
// (void) put - Send the dataset to the server.
//
// PURPOSE:
// An appliation would create a class derived from this class to represent
// data to be sent to a server.
//
//-
//***********************************************************************
//

class	cDhsPutDataset: public cDhsClientDataset
{
    public:
	inline		cDhsPutDataset( void );
	inline		cDhsPutDataset( void *, unsigned long, 
				DHS_BD_PUT_TYPE );
			cDhsPutDataset( cDhsPutRequest & );

	void		put( const cDhsConnection &, const char *, bool, 
				DHS_STATUS & );
    protected:
    private:
	DHS_BD_PUT_TYPE	dpdType;	// Type of data being sent.
};

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
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a put dataset
//
// DESCRIPTION:
// Create a new dataset and assign it to the object member.
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
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    ddsPBuffer = NULL;
    ddsLength = 0;
    dpdType = DHS_BD_PT_DS;
    ddsObject = dhsBdDsNew( &status );

    if ( status != DHS_S_SUCCESS )
    {
	throw status;
    }
}

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
// (>) pBuffer (void *) Pointer to the data buffer.
// (>) length (unsigned long) Length of the data buffer.
// (>) type (DHS_BD_PUT_TYPE) Type of data being sent.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a put dataset
//
// DESCRIPTION:
// Initialize the member data.
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
    void	*pBuffer,	// (in)  Pointer to the data buffer.
    unsigned long		// (in)  Length of the data buffer.
		length,
    DHS_BD_PUT_TYPE		// (in)  Type of data in the buffer.
		type
)
{
    ddsObject = DHS_BD_DATASET_NULL;
    ddsPBuffer = pBuffer;
    ddsLength = length;
    dpdType = type;
}
#endif // DHSPPPUTDATASET_H
