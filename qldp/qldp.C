static char rcsid[] = "$Id: qldp.C,v 1.2 2002-11-27 17:15:09 brighton Exp $";
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
// qldp/src/qldp.C
//
// PURPOSE:
// This file contains the interface to the Quick Look data processing
// tcl commands.
//
// METHOD NAME(S)
// dpApply - Implements the dpApply TCL command.
// dpClear - Clean up any data processing that was set.
// dpSet - Set up the data processing.
// Qldp_Init - Initialize the data processing commands.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:35:07  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:35  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:06  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.2  1999/01/20 19:04:40  nhill
// Added option to apply math to the whole image.
// Changed to always create a temporary target array, even if the original
// was a floating point image.
// Changed dpSet to return the temporary file names.
// Improved the error checking.
//
// Revision 1.1  1998/11/13 23:37:07  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <tcl.h>

//
// The VOID symbol is defined in tcl.h. It isn't an global part of the
// interface, so undefine it to avoid conflics with the DHS error handling
// header.
//

#undef	VOID


#include <iostream>
#include <sys/mman.h>
#include <limits>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "localQldp.H"

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
}
#include "arrayMath.H"
#include "image.H"


//
// The interface to the global functions must be C in order for them to 
// be used by tcl.
//

extern	"C"
int	Qldp_Init( Tcl_Interp * );
extern	"C"
int	dpSet( ClientData, Tcl_Interp *, int, char *[] );
extern	"C"
int	dpApply( ClientData, Tcl_Interp *, int, char *[] );
extern	"C"
int	dpClear( ClientData, Tcl_Interp *, int, char *[] );


//
// Define types for an array operator function pointer where the operation is
// between two floating point arrays, and an array operator function pointer
// where the operation is between a floating point array and a floating point
// constant.
//

typedef	cArray<float> &(cArray<float>::*arrayOpType)( cArray<float> & );
typedef	cArray<float> &(cArray<float>::*constOpType)( float );


//
// Create some global variables which will define the data processing.
// These variables are set by the dpSet command.
//

static char	*operation = NULL;	// The operator string.
static float	operand = 0;		// A constant operand value.
static cImage	*targetImage = NULL;	// The image being modified.
static cImage	*operandImage = NULL;	// An image operand array.
static cImage	*originalImage = NULL;	// The original image.
static char 	*workPath = NULL;	// The path to use for temporary files.
static arrayOpType			// The function to use to perform
		arrayOpFn = NULL;	// the specified array operation.
static constOpType			// The function to use to perform
		constOpFn = NULL;	// the specified constant operation.


//
// Define the text for the error and status messages.
//

msgText( cQldpStatus, E_ARRAY_OP, P_NULL,
	"Invalid array operation `%s'.\n"
	"Valid operators are `+=', `-=', `*=', `/=', `^='." );
msgText( cQldpStatus, E_CONST_OP, P_NULL,
	"Invalid constant operation `%s'.\n"
	"Valid operators are `+=', `-=', `*=', `/='" );
msgText( cQldpStatus, E_DPAPPLY_USAGE, P_NULL,
	"usage: dpApply\n"
	"       dpApply xstart xspan ystart yspan" );
msgText( cQldpStatus, E_DPCLEAR_USAGE, P_NULL,
	"usage: dpClear" );
msgText( cQldpStatus, E_DPSET_USAGE, P_NULL,
	"usage: dpSet workPath operator {headerFname dataFname} \n"
	"                      {headerFname dataFname}\n"
	"       dpSet workPath operator {headerFname dataFname} constant\n"
	"%s" );
msgText( cQldpStatus, E_FOPEN, P_NULL,
	"Error opening file `%s'." );
msgText( cQldpStatus, E_MMAP, P_NULL,
	"Error mapping file `%s'." );
msgText( cQldpStatus, E_OPERAND_SIZE, P_NULL,
	"Target size (%dx%d) does not match operand size (%dx%d)" );

//
//***********************************************************************
//+
// FUNCTION NAME:
// dpApply
//
// INVOCATION:
// Called by the TCL interpreter. The tcl command is invoked as follows:
// dpApply xStart, xSpan, yStart, ySpan
// or
// dpApply
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) clientData (ClientData) The client data for the command.
// (>) Tcl_Interp (interp) The interpreter which invoked the command.
// (>) argc (int) The number of command arguments.
// (>) argv (char *[]) This list of command arguments.
//
// FUNCTION VALUE:
// (int) TCL_OK, or TCL_ERROR.
//
// PURPOSE:
// This function implements the dpApply TCL command
//
// With arguments, the arithmetic is applied to the specified region.
// Without arguments, arithmetic is applied to the entire image.
//
// DESCRIPTION:
// - Check that the operands are correct.
// - Extract the values from the operands.
// - Set up cArray objects for the target and operand images.
// - Apply the operation.
//
// EXTERNAL VARIABLES:
// targetImage, operandImage, originalImage, arrayOpFn, constOpFn.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

extern	"C"
int		dpApply
(
    ClientData	,		// (in)  The client data pointer
    Tcl_Interp	*interp,	// (in)  The TCL interpreter id.
    int		argc,		// (in)  The argument count
    char	*argv[]		// (in)  The list of arguments.
)
{
    cQldpStatus	status;		// DHS status variable.
    int		xStart;		// X start of the region to be modified.
    int		xSpan;		// Size of the region to be modified.
    int		yStart;		// Y start of the region to be modified.
    int		ySpan;		// Y size of the region to be modified.
    float	fBlank;		// Blank value for the images.
    cArray<float> targetArray( (float *) targetImage->dataPtr(), 
	    0., targetImage->xSize(), targetImage->ySize() );
				// This is the array containing the data
				// for the target image. The correct blank
				// value will be filled in later.


    //
    // Disable message display.
    //

    status.displayStop();


    //
    // The arrayMath library throws exceptions with a cAmStatus value when
    // it detects an error, so these have to be caught.
    //

    try
    {
	//
	// Ensure the correct number of arguments are present.
	//

	if ( argc == 5 )
	{
	    //
	    // A region was specified, extract the region arguments.
	    //

	    xStart = atoi( argv[1] );
	    xSpan = atoi( argv[2] );
	    yStart = atoi( argv[3] );
	    ySpan = atoi( argv[4] );
	}
	else if ( argc == 1 )
	{
	    //
	    // No arguments, set the region to the entire image.
	    //

	    xStart = 1;
	    xSpan = targetImage->xSize();
	    yStart = 1;
	    ySpan = targetImage->ySize();
	}
	else
	{
	    //
	    // Wrong number of arguments specified.
	    //

	    status.E_DPAPPLY_USAGE( status );
	    Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	    return( TCL_ERROR );
	}


	//
	// Get the blank value for the target image, and set it in the target
	// array.
	//

	if ( targetImage->keyword( "BLANK", fBlank ) == 0 )
	{
	    targetArray.blank( fBlank );
	}


	//
	// Create a sub-array coresponding to the region to be affected.
	//

	cArray<float> targetSubArray( targetArray, xStart - 1, xSpan, 
		yStart - 1, ySpan );
	targetImage->regionCopy( *originalImage, xStart, xSpan, yStart, ySpan );


	if ( operandImage == NULL )
	{
	    //
	    // Apply the constant operation function.
	    //

	    (targetArray.*constOpFn)( operand );
	}
	else
	{
	    //
	    // If the operand is an array, create an array for the operand.
	    //

	    cArray<float> operandArray( (float *) operandImage->dataPtr(),
		    0., operandImage->xSize(), operandImage->ySize() );


	    //
	    // set the correct blank value for the operand array.
	    //

	    if ( operandImage->keyword( "BLANK", fBlank ) == 0 )
	    {
		operandArray.blank( fBlank );
	    }


	    //
	    // Create a sub-array for the part of the operand to be involved
	    // with the operation.
	    //

	    cArray<float> operandSubArray( operandArray, xStart - 1, xSpan, 
		    yStart - 1, ySpan );


	    //
	    // Apply the operation.
	    //

	    (targetSubArray.*arrayOpFn)( operandSubArray );
	}
    }
    catch ( cAmStatus &amStatus )
    {
	status.E_DPAPPLY_USAGE( status );
	status += amStatus;
	Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	return( TCL_ERROR );
    }


    return( TCL_OK );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// dpClear
//
// INVOCATION:
// Called by the TCL interpreter. The tcl command is invoked as follows:
// dpClear
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) clientData (ClientData) The client data for the command.
// (>) Tcl_Interp (interp) The interpreter which invoked the command.
// (>) argc (int) The number of command arguments.
// (>) argv (char *[]) This list of command arguments.
//
// FUNCTION VALUE:
// (int) TCL_OK, or TCL_ERROR.
//
// PURPOSE:
// This function implements the dpClear TCL command
//
// DESCRIPTION:
// Clear all of the global variables which describe data processing.
//
// EXTERNAL VARIABLES:
// operation, targetImage, operandImage, originalImage, workPath.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

extern	"C"
int		dpClear
(
    ClientData	,		// (in)  The client data for the command.
    Tcl_Interp	*interp,	// (in)  The interpreter for the command.
    int		argc,		// (in)  The argument count.
    char	*argv[]		// (in)  The list of arguments.
)
{
    cQldpStatus	status;		// DHS status variable.


    //
    // Stop the automatic display of messages.
    //

    status.displayStop();


    //
    // Verify that there are no arguments.
    //

    if ( argc != 1 )
    {
	status.E_DPCLEAR_USAGE( status );
	Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	return( TCL_ERROR );
    }


    //
    // Clean up the global variables.
    //

    if ( operation != NULL )
    {
	gen_free( operation );
	operation = NULL;
    }

    if ( targetImage != NULL )
    {
	delete targetImage;
	targetImage = NULL;
    }

    if ( operandImage != NULL )
    {
	delete operandImage;
	operandImage = NULL;
    }

    if ( originalImage != NULL )
    {
	delete originalImage;
	originalImage = NULL;
    }

    if ( workPath != NULL )
    {
	gen_free( workPath );
	workPath = NULL;
    }

    return( TCL_OK );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// dpSet
//
// INVOCATION:
// Called by the TCL interpreter. The tcl command is invoked as follows:
// set files [ dcSet /tmp/path += {targetHeaderFile targetDataFile} 	\
//		{operandHeaderFile operandDataFile} ]
// or
// set files [ dcSet /tmp/path += {targetHeaderFile targetDataFile} 1.4 ]
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) clientData (ClientData) The client data for the command.
// (>) Tcl_Interp (interp) The interpreter which invoked the command.
// (>) argc (int) The number of command arguments.
// (>) argv (char *[]) This list of command arguments.
//
// FUNCTION VALUE:
// (int) TCL_OK, or TCL_ERROR.
//
// The TCL command returns the header and data file names coresponding to
// the files created to contain the result image.
//
// PURPOSE:
// This function implements the dpSet TCL command
//
// The data processing is an operation applied to a target array and a
// constant, or an operation applied to a target array and another array
// of the same size.
//
// DESCRIPTION:
// - Check the correct number of command parameters are present.
// - Save the command parameters describing the data processing.
// - Create cImage objects representing the target, and if necessary
//   the operand image.
// - If necessary, convert the target and operand images to floating point
//   images.
//
// EXTERNAL VARIABLES:
// operation, targetImage, operandImage, originalImage, workPath, 
// arrayOpFn, constOpFn.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

extern	"C"
int		dpSet
(
    ClientData	,		// (in)  The Client data for the command.
    Tcl_Interp	*interp,	// (in)  The interpreter for the command.
    int		argc,		// (in)  The argument count.
    char	*argv[]		// (in)  The list of arguments.
)
{
    //
    // Enclude the whole function in a try block to allow errors from the
    // arrayMath library to be caught.
    //

    try
    {
	char		**list;		// This is used when extracting sub-
					// lists from the arguments.
	int		llength;	// Length of sub-lists.
	cQldpStatus	status;		// DHS status variable.
	cImage		*oldImage;	// Temporary image object storage.


	//
	// Disable message display.
	//

	status.displayStop();


	//
	// Check that the correct number of arguments are present.
	//

	if ( argc != 5 )
	{
	    status.E_DPSET_USAGE( status, "Wrong number of arguments" );
	    Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	    return( TCL_ERROR );
	}


	//
	// Save the work path.
	//

	if ( workPath != NULL )
	{
	    gen_free( workPath );
	}
	workPath = strsav( argv[1] );


	//
	// Save the operator.
	//

	if ( operation != NULL )
	{
	    gen_free( operation );
	}
	operation = strsav( argv[2] );


	//
	// Get the target file names from the sublist in the third argument.
	//

	if ( Tcl_SplitList( interp, argv[3], &llength, &list ) != TCL_OK )
	{
	    status.E_DPSET_USAGE( status, 
		    "Couldn't split target file name list." );
	    Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	    return( TCL_ERROR );
	}
	if ( llength != 2 )
	{
	    status.E_DPSET_USAGE( status, 
		    "Wrong number of arguments in target file name list." );
	    Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	    return( TCL_ERROR );
	}


	//
	// Create a new cImage object for the original target image.
	//

	if ( originalImage != NULL )
	{
	    delete originalImage;
	}
	originalImage = new cImage( list[0], list[1] );
	free( list );


	//
	// Convert the target image to a floating point image.
	//

	targetImage = originalImage->toFloat( workPath, status );


	//
	// Figure out the type of the operand. If the fourth argument is
	// a list of two items, then the operand should be two file names
	// containing a FITS file.
	//

	if ( Tcl_SplitList( interp, argv[4], &llength, &list ) != TCL_OK )
	{
	    status.E_DPSET_USAGE( status, "Couldn't split operand list." );
	    Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	    return( TCL_ERROR );
	}


	//
	// Delete any existing operand image.
	//

	if ( operandImage != NULL )
	{
	    delete operandImage;
	    operandImage = NULL;
	}


	if ( llength == 2 )
	{
	    //
	    // The operand is an image, create a new cImage for the operand.
	    //

	    operandImage = new cImage( list[0], list[1] );


	    //
	    // Make sure the two images have the same size.
	    //

	    if ( operandImage->xSize() != targetImage->xSize() || 
		    operandImage->ySize() != targetImage->ySize() )
	    {
		status.E_OPERAND_SIZE( status, 
			targetImage->xSize(), targetImage->ySize(),
			operandImage->xSize(), operandImage->ySize() );
		Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
		free( list );
		return( TCL_ERROR );
	    }


	    //
	    // If necessary, convert the operand to a floating point array.
	    //

	    if ( (*operandImage) != FLOAT_IMAGE )
	    {
		oldImage = operandImage;
		operandImage = operandImage->toFloat( workPath, status );
		operandImage->regionCopy( *oldImage );
		delete oldImage;
	    }


	    //
	    // Save a pointer to the appropriate operator function.
	    //

	    if ( strcmp( operation, "+=" ) == 0 )
	    {
		arrayOpFn = &cArray<float>::operator+=;
	    }
	    else if ( strcmp( operation, "-=" ) == 0 )
	    {
		arrayOpFn = &cArray<float>::operator-=;
	    }
	    else if ( strcmp( operation, "*=" ) == 0 )
	    {
		arrayOpFn = &cArray<float>::operator*=;
	    }
	    else if ( strcmp( operation, "/=" ) == 0 )
	    {
		arrayOpFn = &cArray<float>::operator/=;
	    }
	    else if ( strcmp( operation, "^=" ) == 0 )
	    {
		arrayOpFn = &cArray<float>::operator^=;
	    }
	    else
	    {
		status.E_ARRAY_OP( status, operation );
		Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
		free( list );
		return( TCL_ERROR );
	    }
	}
	else if ( llength == 1 )
	{
	    //
	    // The operand is a constant, save its value.
	    //

	    operand = atof( strsav( list[0] ) );


	    //
	    // Save a pointer to the appropriate operator function.
	    //

	    if ( strcmp( operation, "+=" ) == 0 )
	    {
		constOpFn = &cArray<float>::operator+=;
	    }
	    else if ( strcmp( operation, "-=" ) == 0 )
	    {
		constOpFn = &cArray<float>::operator-=;
	    }
	    else if ( strcmp( operation, "*=" ) == 0 )
	    {
		constOpFn = &cArray<float>::operator*=;
	    }
	    else if ( strcmp( operation, "/=" ) == 0 )
	    {
		constOpFn = &cArray<float>::operator/=;
	    }
	    else
	    {
		status.E_CONST_OP( status, operation );
		Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
		free( list );
		return( TCL_ERROR );
	    }
	}
	else
	{
	    status.E_DPSET_USAGE( status, 
		    "Operand list length was not 1 or 2." );
	    Tcl_SetResult( interp, (char*)status.message(), TCL_VOLATILE );
	    free( list );
	    return( TCL_ERROR );
	}

	free( list );


	//
	// Put the temporary target image file names into the return value.
	//

	Tcl_AppendElement( interp, targetImage->headerFName() );
	Tcl_AppendElement( interp, targetImage->dataFName() );

	return( TCL_OK );
    }
    catch( cQldpStatus status )
    {
	status.display();
	status.clear();
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// Qldp_Init
//
// INVOCATION:
// N.A. Called automatically by TCL.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) interp (Tcl_Interp *) The tcl interpreter.
//
// FUNCTION VALUE:
// (int) Return status value.
//
// PURPOSE:
// This function is called by the TCL interpreter to set up the commands
// defined in this file
//
// DESCRIPTION:
// Use Tcl_CreateCommand to set up the commands defined in this file.
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

extern "C" 
int	Qldp_Init
(
    Tcl_Interp	*interp		// (in)  The tcl interpreter.
)
{

    Tcl_CreateCommand( interp, "dpSet", dpSet, (ClientData) NULL, 
	    (Tcl_CmdDeleteProc *) NULL );
    Tcl_CreateCommand( interp, "dpApply", dpApply, (ClientData) NULL, 
	    (Tcl_CmdDeleteProc *) NULL );
    Tcl_CreateCommand( interp, "dpClear", dpClear, (ClientData) NULL, 
	    (Tcl_CmdDeleteProc *) NULL );

    Tcl_PkgProvide( interp, "qldp", "1.0" );

    return( TCL_OK );
}
