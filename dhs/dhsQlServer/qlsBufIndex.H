//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997.				(c) 1997
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
// dhsQlServer/h/qlsBufIndex.H
//
// PURPOSE:
// Contains the interface to the cQlsBufIndex class.
//
// CLASSES:
// <class> - Description
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1997/08/20 21:19:02  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSBUFINDEX_H
#define	QLSBUFINDEX_H

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsBufIndex
//
// PUBLIC METHODS: 
// 		cQlsBufIndex	- Constructor for the class.
// (void)	operator++	- Increment the index to the next row.
// (int)	bufSize		- Return the number of bytes in each row.
// (bool)	end		- Return true if at the end of the buffer.
// (char *)	outAddr		- Return the address where the row should
//				  be inserted in the output buffer.
// (void)	start		- Set the object to point back at the start
//				  of the buffer.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Used as in index into the image data of a chunk being merged
//
// This class calculates the output address of the "current" row in a chunk.
// The row can then be copied into the merged image.
//
//-
//***********************************************************************
//

class		cQlsBufIndex
{
    public:
    	inline		cQlsBufIndex( long[], long[], cQlsFrame * );
	void		operator++( int );
	inline int	bufSize( void )
			{
			    return( qbiChunkNaxis[0] * qbiMultiplier[0] );
			}
	inline bool	end( void )
			{
			    return( qbiEnd );
			}
	inline char	*outAddr( void );
	inline void	start( void );
    protected:
    private:
	long		*qbiChunkNaxis;	// Size of the data chunk array.
	long		*qbiChunkOrigin;// Origin of the data chunk.
	bool		qbiEnd;		// True if the index has incremented 
					// off the end.
	cQlsFrame	*qbiFitsFrame;	// Fits object to receive the data.
	long		qbiIndex[7];	// Current indices into the chunk.
	long		qbiMultiplier[7];// Multiplier to get offsets.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsBufIndex::cQlsBufIndex
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) naxis (long []) The size of the chunk to be merged.
// (>) origin (long []) The origin of a chunk in the merged chunk.
// (>) frame (cQlsFrame *) A pointer to the frame into which the chunk will
//			be merged.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cQlsBufIndex class
//
// This function sets up the data members describing the chunk and the
// merged image.
//
// DESCRIPTION:
// Initialize the data members.
// A multiplier is set up to be used when calculating the output address
// to simplify the calculation. The multiplier for the first dimension is
// the number of bytes per pixel. The multiplier for the second dimension
// is the bytes per pixel * the number of pixels in the first dimension of
// the merged image, etc.
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

		cQlsBufIndex::cQlsBufIndex
(
    long	naxis[],	// (in)  The size of the chunk to be merged.
    long	origin[],	// (in)  The origin of the chunk.
    cQlsFrame	*frame		// (in)  The frame into which the chunk will
    				//	 be merged.
)
{
    int		i;

    qbiChunkNaxis = naxis;
    qbiChunkOrigin = origin;
    qbiFitsFrame = frame;


    //
    // The multiplier is the number of bytes the output changes for each
    // dimension of the image.
    //

    qbiMultiplier[0] = qbiFitsFrame->qfdBytePix;
    for ( i = 1; i < qbiFitsFrame->qfdNaxes; i++ )
    {
	qbiMultiplier[i] = qbiMultiplier[i-1] * qbiFitsFrame->qfdNaxis[i-1];
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsBufIndex::outAddr
//
// INVOCATION:
// addr = bufIndex.outAddr();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// (char *) The address where the current row should be put in the output
//		data image.
//
// PURPOSE:
// Calcuate the address where the current row should be put
//
// The row data in the chunk should be copied to the address returned by
// this function.
//
// DESCRIPTION:
// The offset for the row is calculated from the sum of the origin for 
// each axis plus the current index of each axis, multiplied by the multiplier
// factor for the row.
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

char		*cQlsBufIndex::outAddr
(
    void
)
{
    int		i;
    char	*addr;


    addr = qbiFitsFrame->qfdData;
    for ( i = 0; i < qbiFitsFrame->qfdNaxes; i++ )
    {
	addr += ( qbiIndex[i] + qbiChunkOrigin[i] - 1 ) * qbiMultiplier[i];
    }

    return( addr );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsBufIndex::start
//
// INVOCATION:
// bufIndex.start();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Reset the index to the begining of the chunk
//
// DESCRIPTION:
// Reset all of the qbiIndex values to zero.
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

void		cQlsBufIndex::start
(
    void
)
{
    int		i;

    qbiEnd = false;
    for ( i = 0; i < qbiFitsFrame->qfdNaxes; i++ )
    {
	qbiIndex[i] = 0;
    }
}
#endif // QLSBUFINDEX_H
