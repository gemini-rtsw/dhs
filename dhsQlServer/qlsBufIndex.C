static char rcsid[] = "$Id: qlsBufIndex.C,v 1.1.1.1 2002-11-24 20:29:02 brighton Exp $";
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
// dhsQlServer/src/qlsBufIndex.C
//
// PURPOSE:
// Contains the implementation of the cQlsBufIndex class.
//
// METHOD NAME(S)
// cQlsBufIndex::operator++ - Increment a buffer index.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:24  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.3  1998/04/15 22:37:50  nhill
// Updated the documentation.
//
// Revision 1.2  1997/08/20 21:23:02  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:12:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//
#include "globals.H"
#include "qlsDataset.H"
#include "qlsBufIndex.H"

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsBufIndex::operator++
//
// INVOCATION:
// bufIndex++;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) (int) Flag to indicate prefix or postfix.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Increment the index in a cQlsBufindex object to the next row in the image
//
// DESCRIPTION:
// - Increment qbiIndex[1].
// - If the index overflows, set it to zero and increment the next index.
// - Repeat the above until no overflow occurs, or there are no more indexes
//   to increment.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// We don't have images with more than 2 dimensions, or a way to test such
// images, so this code has only be tested with 2 dimensional images.
// 
//-
//***********************************************************************
//

void		cQlsBufIndex::operator++
(
    int
)
{
    int		i;


    //
    // Increment the first index. Index [0] is not incremented, since an
    // increment by row is required, not an increment by pixel.
    //

    qbiIndex[1] ++;


    //
    // Propegate any overflow down the indexes of a multi dimensional image.
    //

    for ( i = 1; i < qbiFitsFrame->qfdNaxes &&
	    qbiIndex[i] >= qbiChunkNaxis[i]; i++ )
    {
	if ( i == qbiFitsFrame->qfdNaxes - 1 )
	{
	    //
	    // If the last index overflows, the end of the image has been 
	    // reached.
	    //

	    qbiEnd = TRUE;
	    break;
	}
	else
	{
	    //
	    // The index has overflowed, so zero it and increment the next
	    // index.
	    //

	    qbiIndex[i] = 0;
	    qbiIndex[i+1]++;
	}
    }
}
