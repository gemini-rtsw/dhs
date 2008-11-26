static char rcsid[] = "$Id: arrayMath.C,v 1.1.1.1 2002-11-24 20:23:35 brighton Exp $";
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
// arrayMath/src/arrayMath.C
//
// PURPOSE:
// This contains the implementation of the array math library.
//
// METHOD NAME(S)
// cArrayBase::cArrayBase
//		- Constructor for the class which creates a sub-array based
//		  on an existing array.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:06  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1999/01/20 18:31:43  nhill
// Changed to use the cDhsStatus class for error reporting.
//
// Revision 1.1  1998/11/13 18:19:27  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include "arrayMath.H"

msgText( cAmStatus, E_INCOMPAT, P_NULL, 
	"Attempt to perform operation %s on incompatable arrays\n"
	"( %d x %d ) vs ( %d x %d )." );
msgText( cAmStatus, E_BOUNDS, P_NULL, 
	"Attempt to create a sub-array ( %d-%d x %d-%d ) outside the bounds\n"
	"of the parent array ( %d-%d x %d-%d )." );

//
//***********************************************************************
//+
// FUNCTION NAME:
// cArrayBase::cArrayBase
//
// INVOCATION:
// N.A. Called only when a new cArray class is created.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) array (cArrayBase &) The existing array.
// (>) startX (int) Start of the X axis.
// (>) spanX (int) Length of the X axis.
// (>) startY (int) Start of the Y axis.
// (>) spanY (int) Length of the Y axis.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This constructor creates a sub-array based on an existing array
//
// DESCRIPTION:
// - set the data member values.
// - Verify the bounds of the sub array are correct.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// This doesn't work if the original array is also a sub-array. This isn't
// required for the current application.
//-
//***********************************************************************
//

		cArrayBase::cArrayBase
(
    cArrayBase	&array,		// (in)  Pointer to the data array.
    int		startX,		// (in)  Start of the X axis.
    int		spanX,		// (in)  End of the X axis.
    int		startY,		// (in)  Start of the Y axis.
    int		spanY		// (in)  End of the Y axis.
): abPData( array.abPData ), abSizeX( array.abSizeX ), 
	abSizeY( array.abSizeY ), abStartX( startX ), abStartY( startY ), 
	abSpanX( spanX ), abSpanY( spanY ), abSize( array.abSize )
{
    cAmStatus	status;


    status.displayStop();


    if ( abStartX < 0 || abStartY < 0 || ( abStartX + abSpanX ) > abSizeX || 
	    (abStartY + abSpanY ) > abSizeY || spanX < 0 || spanY < 0 )
    {
	status.E_BOUNDS( status, startX, startX + spanX, startY, 
		startY + spanY, abStartX, abStartX + abSpanX, abStartY, 
		abStartY + abSpanY );
	throw status;
    }
}
