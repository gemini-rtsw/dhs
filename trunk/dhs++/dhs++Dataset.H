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
// dhs++/h/dhs++Dataset.H
//
// PURPOSE:
// Contains the class heirarchy to define datasets.
//
// CLASSES:
// cDhsDataStruct - Base class for frames and datasets.
// cDhsFrame - Class to represent frames within a dataset.
// cDhsDataset - Class to represent instrument datasets.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:02  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.14  1998/05/12 21:12:09  nhill
// Updated the documentation.
//
// Revision 1.13  1998/04/02 19:40:40  nhill
// Moved the constructor to file dataset.C.
// Moved cDhsDataset::export to file dataset.C.
//
// Revision 1.12  1997/12/01 21:39:46  cockayne
// Modified to suit Rational Rose analyzer.
//
// Revision 1.11  1997/08/19 20:01:31  nhill
// Added function cDhsDataStruct::objectId.
// Added function cDhsDataset::~cDhsDataset.
// Added function cDhsDataset::exportDataset.
//
// Revision 1.10  1997/07/11 21:41:30  nhill
// Changed to make sure accessed datasets are freed.
//
// Revision 1.9  1997/07/10 19:03:45  nhill
// Added the cDhsDataset::buffer function.
//
// Revision 1.8  1997/05/13 17:17:58  nhill
// Added a destructor to free a dataset if necessary.
// Added code to set a flag indicating if the dataset needs to be freed.
//
// Revision 1.7  1997/05/01 23:11:46  nhill
// Added initialization of the dataset data members.
//
// Revision 1.6  1997/04/24 20:26:05  nhill
// Removed some the the const keywords.
//
// Revision 1.5  1997/04/24 16:56:39  nhill
// Added "const" keywords to function prototypes.
//
// Revision 1.4  1997/03/06 19:16:30  nhill
// Cleaned up the header file dependancies.
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

#ifndef DHSPPDATASET_H
#define	DHSPPDATASET_H

class	cDhsFrame;

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsDataStruct
//
// PUBLIC METHODS: 
// (void) attribAdd - add an attribute to the data structure.
// (cDhsAttrib) attribfind - Find an attribute in the data structure.
// (cDhsFrame) frameAdd - Add a frame to the data structure.
// (cDhsFrame) frameFind - Find a frame in the data structure.
// (DHS_BD_OBJECT) objectId - Accessor function to return the object id
//		associated with this object.
//
// PURPOSE:
// Contains the methods and data common to both frames and datasets
//
//-
//***********************************************************************
//

class cDhsDataStruct
{
    public:
	inline void		attribAdd( const char *, DHS_DATA_TYPE, int, 
					const unsigned long *, const void *, 
					DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, char, 
					DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, 
					unsigned char, DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, short, 
					DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, 
					unsigned short, DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, int, 
					DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, 
					unsigned int, DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, long, 
					DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, 
					unsigned long, DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, double, 
					DHS_STATUS & );
	inline void		attribAdd( const char *, DHS_DATA_TYPE, 
					const char *, DHS_STATUS & );
	inline cDhsAttrib	attribFind( const char *, DHS_STATUS & );
	inline cDhsAttrib	attribFind( int, DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, char *&, 
					DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, 
					unsigned char *&, DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, short *&, 
					DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, 
					unsigned short *&, DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, int *&, 
					DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, 
					unsigned int *&, DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, long *&, 
					DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, 
					unsigned long *&, DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, double *&, 
					DHS_STATUS & );
	inline cDhsFrame	frameAdd( const char *, int, DHS_DATA_TYPE, int,
					const unsigned long *, float *&, 
					DHS_STATUS & );
	inline cDhsFrame	frameFind( const char *, DHS_STATUS & );
	inline cDhsFrame	frameFind( int, DHS_STATUS & );
	inline DHS_BD_OBJECT	objectId( void );
    protected:
	inline			cDhsDataStruct( DHS_BD_FRAME );
	inline			cDhsDataStruct( void );
	inline			~cDhsDataStruct( void );

	DHS_BD_OBJECT		ddsObject;	// Frame or dataset id.
	bool			ddsObjectFree;	// True if the object needs
						// to be freed.
    private:
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::cDhsDataStruct
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
// constructor for a cDhsDataStruct object
//
// DESCRIPTION:
// Assign the object id.
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

		cDhsDataStruct::cDhsDataStruct
(
    void
)
{
    ddsObject = DHS_BD_DATASET_NULL;
    ddsObjectFree = false;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::cDhsDataStruct
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) object (DHS_BD_FRAME) The id of the new object.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// constructor for a cDhsDataStruct object
//
// DESCRIPTION:
// Assign the object id.
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

		cDhsDataStruct::cDhsDataStruct
(
    DHS_BD_FRAME		// (in)  The initial object id.
		object
)
{
    ddsObject = object;
    ddsObjectFree = false;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::~cDhsDataStruct
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
// Destructor for a data object
//
// DESCRIPTION:
// If necessary, delete the bulk data object.
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

		cDhsDataStruct::~cDhsDataStruct
(
    void
)
{
    DHS_STATUS	status( DHS_S_SUCCESS );


    if ( ddsObjectFree )
    {
	dhsBdDsFree( ddsObject, &status );
	if ( status != DHS_S_SUCCESS )
	{
	    throw status;
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::objectId
//
// INVOCATION:
// id = object.objectId();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (DHS_BD_OBJECT) The DHS id of the object.
//
// PURPOSE:
// Return the object id of the object
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

DHS_BD_OBJECT	cDhsDataStruct::objectId
(
    void
)
{
    return ddsObject;
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsFrame
//
// PUBLIC METHODS: 
// (None) cDhsFrame - Constructor.
// (None) ~cDhsFrame - Destructor.
// (void) info - Return various information about the frame.
//
// PURPOSE:
// Contains methods only used by frames
//
//-
//***********************************************************************
//

class 	cDhsFrame: public cDhsDataStruct
{
    public:
	inline		cDhsFrame( DHS_BD_FRAME );
	inline		cDhsFrame( void );
	inline void	info( char *&, DHS_DATA_TYPE &, DHS_STATUS & ) const;
	inline void	info( int &, unsigned long *, void **, 
				DHS_STATUS & ) const;
	inline void	info( char *&, DHS_DATA_TYPE &, int &, unsigned long *,
				void **, DHS_STATUS & ) const;
    protected:
    private:
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsFrame::cDhsFrame
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
// Constructor for a frame
//
// DESCRIPTION:
// Only executes the cDhsDataStruct constructor.
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

		cDhsFrame::cDhsFrame
(
    void
): cDhsDataStruct( DHS_BD_FRAME_NULL )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsFrame::cDhsFrame
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) frameId (DHS_BD_FRAME) Id of the new frame
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for a frame
//
// DESCRIPTION:
// Only executes the cDhsDataStruct constructor.
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

		cDhsFrame::cDhsFrame
(
    DHS_BD_FRAME frameId	// (in)  Frame id of the new frame.
): cDhsDataStruct( frameId )
{
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsFrame::info
//
// INVOCATION:
// char			*name;
// DHS_DATA_TYPE	type;
// DHS_STATUS		status;
// How to call the function or proceedure
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) name (char *&) Name of the frame.
// (<) type (DHS_DATA_TYPE &) Data type in the data array.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get information about a frame
//
// DESCRIPTION:
// Execute the DHS library dhsBdFrameInfo function.
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

void		cDhsFrame::info
(
    char	*&name,		// (out) The name of the frame.
    DHS_DATA_TYPE		// (out) The type of data in the frame.
    		&type,
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsBdFrameInfo( ddsObject, &name, &type, NULL, NULL, NULL, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsFrame::info
//
// INVOCATION:
// int			nDims;
// unsigned long	dims[7];
// void			*pData;
// DHS_STATUS		status;
// frame.info( nDims, dims, &pData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) nDims (int &) Number of dimensions in the array.
// (<) dims (unsigned long *) Dimensions of the data array.
// (<) pData (void **) Pointer to the data array.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get information about a frame
//
// DESCRIPTION:
// Execute the DHS library dhsBdFrameInfo function.
//
// This function should be a template member function with the pData
// parameter replaced by a reference to a pointer to the template type.
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

void		cDhsFrame::info
(
    int		&nDims,		// (out) Number of dimensions in the array.
    unsigned long		// (out) Dimensions of the data array.
		*dims,
    void	**pData,	// (out) Pointer to the data array.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsBdFrameInfo( ddsObject, NULL, NULL, &nDims, dims, pData, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsFrame::info
//
// INVOCATION:
// char			*name;
// DHS_DATA_TYPE	type;
// int			nDims;
// unsigned long	dims[7];
// void			*pData;
// DHS_STATUS		status;
// frame.info( name, type, nDims, dims, &pData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (<) name (char *&) Name of the frame.
// (<) type (DHS_DATA_TYPE &) Data type in the data array.
// (<) nDims (int &) Number of dimensions in the array.
// (<) dims (unsigned long *) Dimensions of the data array.
// (<) pData (void **) Pointer to the data array.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Get information about a frame
//
// DESCRIPTION:
// Execute the DHS library dhsBdFrameInfo function.
//
// This function should be a template member function with the pData
// parameter replaced by a reference to a pointer to the template type.
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

void		cDhsFrame::info
(
    char	*&name,		// (out) The name of the frame.
    DHS_DATA_TYPE		// (out) The type of data in the frame.
		&type,
    int		&nDims,		// (out) Number of dimensions in the array.
    unsigned long		// (out) Dimensions of the data array.
		*dims,
    void	**pData,	// (out) Pointer to the data array.
    DHS_STATUS	&status		// (mod) Function return status.
) const
{
    dhsBdFrameInfo( ddsObject, &name, &type, &nDims, dims, pData, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::attribAdd
//
// INVOCATION:
// unsigned long	dims[7];
// int			dataArray[128][256];
// dims[0] = 128;
// dims[1] = 256;
// ds.attribAdd( "attrib name", DHS_DT_INT, 2, dims, dataArray, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (const char *) Name for the attribute.
// (>) type (DHS_DATA_TYPE) Data type of the data.
// (>) nDims (int) Number of dimensions for the data.
// (>) dims (const unsigned long []) Dimensions of the data array.
// (>) pData (const void *) Pointer to the data array.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a new attribute in a dataset or frame containing an array of data
//
// DESCRIPTION:
// Add the attribute with the dhsBdAttribAdd function.
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

void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    int		nDims,		// (in)  Number of dimensions for the data.
    const unsigned long		// (in)  Dimensions of the data array.
    		*dims,
    const void	*pData,		// (in)  Pointer to the data array.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, nDims, dims, pData, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::attribAdd
//
// INVOCATION:
// ds.attribAdd( "attrib name", type, value, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name for the attribute.
// (>) type (DHS_DATA_TYPE) Data type of the data.
// (>) value (type) The data value.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a new attribute in a dataset or frame containing a scalar value
//
// DESCRIPTION:
// Add the attribute with the dhsBdAttribAdd function.
//
// This function should be implemented as a template member function, but
// since our compiler doesn't support this, each possible incarnation of
// the function has been implemented.
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

void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    char	value,		// (in)  The data value.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    unsigned char		// (in)  The data value.
		value,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    short	value,		// (in)  The data value.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    unsigned short		// (in)  The data value.
		value,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    int 	value,		// (in)  The data value.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    unsigned int 		// (in)  The data value.
		value,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    long 	value,		// (in)  The data value.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    unsigned long		// (in)  The data value.
		value,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    double	value,		// (in)  The data value.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}
void		cDhsDataStruct::attribAdd
(
    const char	*name,		// (in)  Name for the attribute.
    DHS_DATA_TYPE		// (in)  Type of the data.
    		type,
    const char	*value,		// (in)  The data value.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    dhsBdAttribAdd( ddsObject, name, type, 0, NULL, value, &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::attribFind
//
// INVOCATION:
// attrib = ds.attribFind( "attribute name", status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the attribute to find.
// (!) status (DHS_STATUS &) function return status.
//
// FUNCTION VALUE:
// (cDhsAttrib) The attribute located.
//
// PURPOSE:
// Locate an attribute in a frame or dataset given its name
//
// DESCRIPTION:
// Execute the dhsBdAttribFind function.
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

cDhsAttrib	cDhsDataStruct::attribFind
(
    const char	*name,		// (in)  Name of the attribute to find.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_AV_ID	attribId;	// The frame located.


    attribId = dhsBdAttribFind( ddsObject, name, &status );

    return( cDhsAttrib( attribId ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::frameAdd
//
// INVOCATION:
// int			index;
// DHS_DATA_TYPE	dataType;
// int			nDims;
// unsigned long	dims[7];
// <type>		*pData;
// object.frameAdd( "frame name", index, dataType, nDims, dims, pData, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) The name for the new frame.
// (>) index (int) The index for the new frame.
// (>) dataType (DHS_DATA_TYPE) The data type for the data array.
// (>) nDims (int) The number of dimensions of the data array.
// (>) dims (unsigned long []) The dimensions of the data array.
// (<) pData (type *&) The pointer to the allocated data array.
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// (cDhsFrame) The frame object created.
//
// PURPOSE:
// Add a new frame to a frame or dataset
//
// DESCRIPTION:
// Use the dhsBdFrameNew function to create a new frame.
// 
// This function should be implemented as a template member function,
// but since this is not available on our compilers, each of the possible
// incarnations of the function have been creaated manually.
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

cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    char	*&pData,	// (out) Pointer to the allocated data array.
    DHS_STATUS	&status		// (mod) Function return status.
) 
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    unsigned char		// (out) Pointer to the allocated data array.
		*&pData,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    short	*&pData,	// (out) Pointer to the allocated data array.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    unsigned short		// (out) Pointer to the allocated data array.
		*&pData,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    int		*&pData,	// (out) Pointer to the allocated data array.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    unsigned int		// (out) Pointer to the allocated data array.
		*&pData,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    long	*&pData,	// (out) Pointer to the allocated data array.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    unsigned long		// (out) Pointer to the allocated data array.
		*&pData,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    double			// (out) Pointer to the allocated data array.
		*&pData,
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}
cDhsFrame	cDhsDataStruct::frameAdd
(
    const char	*name,		// (in)  The name for the new frame.
    int		index,		// (in)  The index for the new frame.
    DHS_DATA_TYPE		// (in)  The data type for the data array.
		dataType,
    int		nDims,		// (in)  Number of dimensions of the data array.
    const unsigned long		// (in)  The dimensions of the data array.
		*dims,
    float	*&pData,	// (out) Pointer to the allocated data array.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    return( cDhsFrame( dhsBdFrameNew( ddsObject, name, index, dataType, 
	    nDims, dims, (const void **) &pData, &status ) ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::attribFind
//
// INVOCATION:
// attrib = ds.attribFind( i, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) index (int) Index of the attribute to find.
// (!) status (DHS_STATUS &) function return status.
//
// FUNCTION VALUE:
// (cDhsAttrib) The attribute located.
//
// PURPOSE:
// Locate an attribute in a frame or dataset given its index
//
// DESCRIPTION:
// Execute the dhsBdattribFind function.
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

cDhsAttrib	cDhsDataStruct::attribFind
(
    int		index,		// (in)  Name of the attribute to find.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_AV_ID	attribId;	// The frame located.


    attribId = dhsBdAttribIndex( ddsObject, index, &status );

    return( cDhsAttrib( attribId ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::frameFind
//
// INVOCATION:
// frame = ds.frameFind( "frame name", status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (const char *) Name of the frame to find.
// (!) status (DHS_STATUS &) function return status.
//
// FUNCTION VALUE:
// (cDhsFrame) The frame located.
//
// PURPOSE:
// Locate a frame in a frame or dataset given its name
//
// DESCRIPTION:
// Execute the dhsBdFrameFind function.
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

cDhsFrame	cDhsDataStruct::frameFind
(
    const char	*name,		// (in)  Name of the frame to find.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_BD_FRAME		// The frame located.
		frameId;


    frameId = dhsBdFrameFind( ddsObject, name, &status );

    return( cDhsFrame( frameId ) );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataStruct::frameFind
//
// INVOCATION:
// frame = ds.frameFind( i, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) index (int) Index of the frame to find.
// (!) status (DHS_STATUS &) function return status.
//
// FUNCTION VALUE:
// (cDhsFrame) The frame located.
//
// PURPOSE:
// Locate an frame in a frame or dataset given its index
//
// DESCRIPTION:
// Execute the dhsBdFrameIndex function.
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

cDhsFrame	cDhsDataStruct::frameFind
(
    int		index,		// (in)  Name of the frame to find.
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    DHS_BD_FRAME		// The frame located.
		frameId;


    frameId = dhsBdFrameIndex( ddsObject, index, &status );

    return( cDhsFrame( frameId ) );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cDhsDataset
//
// PUBLIC METHODS: 
// (void) access - Make a data buffer accessable as a dataset.
// (void *) buffer - Return a pointer to the data buffer for the dataset.
// (void) create - Reset the dataset to an empty state.
// (void) exportDataset - Export a database to the memory buffer.
// (ulong) length - Return the length of the buffer.
// (void) print - Print the dataset to standard out.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// (type) data - description
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
//-
// XXX allan: 2002/11/18: export is a reserved word: renamed to exportDataset
//***********************************************************************
//

class 	cDhsDataset: 
	    public cDhsDataStruct,
	    public cDhsAvList
{
    friend class cDhs;

    public:
	inline void	access( const void *, DHS_STATUS & );
	inline void	access( DHS_STATUS & );
	inline void	*buffer( void );
	inline void	buffer( void *, unsigned long );
	inline void	create( DHS_STATUS & );
	void		exportDataset( DHS_STATUS & );
	inline unsigned long	
			length( void );
	inline void	print( DHS_STATUS & );
	
    protected:
	inline		cDhsDataset( void );
	inline		cDhsDataset( DHS_BD_DATASET );
			cDhsDataset( cDhsDataset & );
	inline		~cDhsDataset( void );

	unsigned long	ddsLength;	// Length of the data buffer.
	void		*ddsPBuffer;	// Data buffer pointer.
	bool		ddsBufferFree;	// Free the buffer on exit?
    private:
};

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
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// constructor for the cDhsDataset class
//
// DESCRIPTION:
// Call the cDhsDataStruct constructor.
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
    void
)
{
    ddsPBuffer = NULL;
    ddsBufferFree = false;
    ddsLength = 0;
}

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
// (>) dataset (DHS_BD_DATASET) Initial dataset id.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// constructor for the cDhsDataset class
//
// DESCRIPTION:
// Call the cDhsDataStruct constructor.
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
    DHS_BD_DATASET		// (in)  Initial dataset id.
	    dataset
) : cDhsDataStruct( dataset )
{
    ddsPBuffer = NULL;
    ddsBufferFree = false;
    ddsLength = 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::~cDhsDataset
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
// Destructor for the cDhsDataset class
//
// Frees the memory allocated to the data buffer if necessary.
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

		cDhsDataset::~cDhsDataset
(
    void
)
{
    if ( ddsBufferFree && ddsPBuffer != NULL )
    {
	free( ddsPBuffer );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::access
//
// INVOCATION:
// dataset.access( pDataBuffer, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pDataBuffer (const void *) Pointer to the data buffer.
// (!) status (DHS_STATUS &) Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Makes an exported buffer available for reading
//
// DESCRIPTION:
// Execute the dhsBdDsAccess function.
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

void		cDhsDataset::access
(
    const void	*pDataBuffer,	// (in)  Pointer to the data buffer.
    DHS_STATUS	&status		// (mod) Function return pointer.
)
{
    if ( ddsObjectFree )
    {
	dhsBdDsFree( ddsObject, &status );
    }

    ddsObject = dhsBdDsAccess( pDataBuffer, &status );
    ddsObjectFree = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::access
//
// INVOCATION:
// dataset.access( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Makes the default buffer available for use as a dataset 
//
// DESCRIPTION:
// Execute the dhsBdDsAccess function.
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

void		cDhsDataset::access
(
    DHS_STATUS	&status		// (mod) Function return pointer.
)
{
    if ( ddsObjectFree )
    {
	dhsBdDsFree( ddsObject, &status );
    }
    ddsObject = dhsBdDsAccess( ddsPBuffer, &status );
    ddsObjectFree = true;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::buffer
//
// INVOCATION:
// buff = ds.buffer();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (void *) Pointer to the data buffer.
//
// PURPOSE:
// Accessor function to return the pointer to the data buffer.
//
// DESCRIPTION:
// n.a.
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

void		*cDhsDataset::buffer
(
    void
)
{
    return( ddsPBuffer );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::buffer
//
// INVOCATION:
// ds.buffer( pData, length );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pData (void * ) Pointer to the data buffer.
// (>) length (unsigned long) Length of the data buffer.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Function to set the data buffer pointers.
//
// DESCRIPTION:
// n.a.
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

void		cDhsDataset::buffer
(
    void	*pData,		// (in)  Pointer to the data buffer.
    unsigned long
    		length		// (in)  Length of the data buffer.
)
{
    ddsPBuffer = pData;
    ddsLength = length;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::create
//
// INVOCATION:
// ds.create( status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Reset a dataset to an empty state
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

void		cDhsDataset::create
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( ddsObject != DHS_BD_DATASET_NULL && ddsObjectFree )
    {
	dhsBdDsFree( ddsObject, &status );
    }
    ddsObject = dhsBdDsNew( &status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::length
//
// INVOCATION:
// len = ds.length();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (unsigned long) Length of the data buffer.
//
// PURPOSE:
// Accessor function to return the length of the data buffer pointed to
// by the ddsPBuffer member.
//
// DESCRIPTION:
// n.a.
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

unsigned long	cDhsDataset::length
(
    void
)
{
	return ddsLength;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cDhsDataset::print
//
// INVOCATION:
// ds.print( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (DHS_STATUS &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Print a dataset to standard out
//
// DESCRIPTION:
// Use the dhsBdDsPrint function to pritn the dataset.
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

void		cDhsDataset::print
(
    DHS_STATUS	&status		// (mod) Function return status.
)
{
    if ( ddsObject != DHS_BD_FRAME_NULL )
    {
	dhsBdDsPrint( ddsObject, &status );
    }
}
#endif // DHSPPDATASET_H
