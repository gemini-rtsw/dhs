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
// dhs++/h/dhs++ClientDataset.H
//
// PURPOSE:
// Contains the class heirarchy to define client datasets.
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
// Revision 1.9  1998/04/02 19:38:29  nhill
// Added an avlist to the done method.
//
// Revision 1.8  1997/08/26 21:13:48  nhill
// Move the cDhsClientDataset::isDone and cDhsClientDataset::wait
// functions to the cDhsTag base class.
//
// Revision 1.7  1997/08/19 20:00:10  nhill
// Added a new constructor to the cDhsClientDataset class.
//
// Revision 1.6  1997/06/12 16:55:33  nhill
// Added a function to return the label.
//
// Revision 1.5  1997/05/13 17:16:20  nhill
// Added the isDone method to the cDhsClientDataset class.
//
// Revision 1.4  1997/05/01 23:11:05  nhill
// Added a mutex to the cDhsClientDataset class.
// Move the cDhsClientDataset constructors and destructor to the C file.
//
// Revision 1.3  1997/03/06 19:16:30  nhill
// Cleaned up the header file dependancies.
//
// Revision 1.2  1997/02/28 00:54:14  nhill
// Checkin for install.
//
// Revision 1.1  1997/02/21 21:46:47  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DHSPPCLIENTDATASET_H
#define	DHSPPCLIENTDATASET_H

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsClientDataset
//
// PUBLIC METHODS: 
// (char *) label - Return the label associated with a dataset.
// (void) label - Set the lavel associated with a dataset.
//
// PURPOSE:
// Contains methods use by both put and get datasets on a client.
//
//-
//***********************************************************************
//

class 	cDhsClientDataset: public cDhsDataset, public cDhsTag
{
    friend class cDhs;

    public:
	inline char	*label( void ) const;
	void		label( const char * );
    protected:
			cDhsClientDataset( void );
			cDhsClientDataset( DHS_BD_DATASET );
			cDhsClientDataset( cDhsClientDataset & );
			cDhsClientDataset( cDhsDataset & );
	virtual		~cDhsClientDataset( void );

	void		*dcdPMutex;	// Pointer to a mutex for the object.
    private:
	virtual void	done( DHS_CMD_STATUS, char *, cDhsAvList & ){};

	char		*dcdLabel;	// Pointer to the datalabel
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsClientDataset::label
//
// INVOCATION:
// label = dataset.label();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (char *) The label name.
//
// PURPOSE:
// Return the data label associated with a dataset
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

char		*cDhsClientDataset::label
(
    void
) const
{
    return( dcdLabel );
}
#endif // DHSPPCLIENTDATASET_H
