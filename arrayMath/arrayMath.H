//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998
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
// array/h/arrayMath.H
//
// PURPOSE:
// This file contains the interface to the array math classes.
//
// CLASSES:
// cArrayBase	- This is a base class for the cArray class. This class 
//		  contains the non-template code for the cArray class.
// cArray	- This is a class for doing arrithmatic on arrays.
//
// METHODS:
// cArrayBase::cArrayBase( char *, int, int, int )
//		- Constructor to initialize a new object based on an
//		  existing array in memory.
// cArray<T>::cArray( T *pData, T blank, int xSize, int ySize )
//		- Constructor for the cArray class which creates an array
//		  pointing to an existing memory buffer.
// cArray<T>::cArray( void )
//		- Constructor for the cArray class which creates an empty
//		  array.
// cArray<T>::cArray( cArray<T>, int, int, int, int )
//		- Constructor for the cArray class which creats a sub-array
//		  of an existing array.
// operator<<	- Function to print an array.
//
// MACROS:
// builtInOperator
//		- Defines an operator for the cArray class, which takes
//		  a built in type as an operand.
// arrayInOperator
//		- Defines an operator for the cArray class, which takes
//		  an array as an operand.
// assignArrayOperator
//		- Defines an assignment operator for the cArray class, 
//		  which takes an array as the operand.
// assignConstOperator
//		- Defines an assignment operator for the cArray class, 
//		  which takes a constant as the operand.
// maskOperator	- Defines an mask operator for the cArray class.
// makeOperators- Creates all of the operators for all of the supported
//		  data types.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:06  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1999/01/20 18:34:30  nhill
// Removed debug statement.
//
// Revision 1.3  1999/01/20 18:30:29  nhill
// Changed to use the cDhsStatus class for error reporting.
//
// Revision 1.2  1998/11/13 18:23:51  nhill
// Added some comments.
//
// Revision 1.1  1998/11/13 18:18:56  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef ARRAY_H
#define	ARRAY_H

#include <stdio.h>

#ifdef HAVE_BOOL_H
#include <bool.h>
#endif

#include <iostream.h>

#include "dhsGenStatus.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cAmStatus
//
// PUBLIC METHODS: 
// None.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This is the status class for the array math routines.
//
//-
//***********************************************************************
//

class cAmStatus: public cDhsStatus
{
    msgPrefix( "am" );

    public:
	    msgId( E_INCOMPAT );
	    msgId( E_BOUNDS );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cArrayBase
//
// PUBLIC METHODS: 
//	  cArrayBase - constructor for the class.
// (int) sizeX - Return the X size of the array.
// (int) sizeY - Return the Y size of the array.
// (int) startX - Return the start of the sub-array in the X dimension.
// (int) startY - Return the start of the sub-array in the Y dimension.
// (int) spanX - Return the length of the sub-array in the X dimension.
// (int) spanY - Return the length of the sub-array in the Y dimension.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This is the base class for the cArray class
//
// Its purpose is to allow code that isn't array type dependant to reside
// in a non-template class, and therefore reduce code bloat.
//
//-
//***********************************************************************
//

class cArrayBase
{
    public:
	inline		cArrayBase( char *, int, int, int );
			cArrayBase( cArrayBase &, int, int, int, int );
	int		sizeX( void ) const { return( abSizeX ); };
	int		sizeY( void ) const { return( abSizeY ); };
	int		startX( void ) const { return( abStartX ); };
	int		startY( void ) const { return( abStartY ); };
	int		spanX( void ) const { return( abSpanX ); };
	int		spanY( void ) const { return( abSpanY ); };
    protected:
	char		*abPData;	// Pointer to the array's data.
	int		abSizeX;	// Size of the array in the X dimension.
	int		abSizeY;	// Size of the array in the Y dimension.
	int		abStartX;	// Start of the sub-array in X.
	int		abStartY;	// Start of the sub-array in Y.
	int		abSpanX;	// Length of the sub-array in X.
	int		abSpanY;	// Length of the sub-array in Y.
	const int	abSize;		// Size of elements in the array.
    private:
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cArrayBase::cArrayBase
//
// INVOCATION:
// N.A. Only called when an object of type cArray is created.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pData (char *) This is a pointer to the data for the array.
// (>) xSize (int) This is the X size of the array.
// (>) ySize (int) This is the Y size of the array.
// (>) size (int) This is the size in bytes of the elements of the array.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor to create an array given a data buffer already in memory
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

		cArrayBase::cArrayBase
(
    char	*pData,		// (in)  Pointer to the data array.
    int		xSize,		// (in)  X dimension of the array.
    int		ySize,		// (in)  Y dimension of the array.
    int		size		// (in)  Size of each data element.
): abPData( pData ), abSizeX( xSize ), abSizeY( ySize ), abStartX( 0 ),
	abStartY( 0 ), abSpanX( xSize ), abSpanY( ySize ), abSize( size )
{
}

//
//***********************************************************************
//+
// MACRO NAME:
// builtInOperator
//
// INVOCATION:
// builtInOperator( +=, int );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) thisOperator : This must be one of the C/C++ assignment operators.
// (>) thisType : This must be a C/C++ built in type.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This macro defines an operator method to allow the specified operation
// (thisOperator) to be performed on an array, using a single value of the 
// specified type (thisType) as the operand
//
// This macro is only useful for defining operator functions in the cArray
// class.
//
// DESCRIPTION:
// Loop over each element in the array, performing the required operation.
//
// PRIOR REQUIREMENTS:
// This macro should only be used in the cArray class definition, and in
// fact is only used in the makeOperators macro.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

#define	builtInOperator( thisOperator, thisType )			\
    cArray<T>	&operator thisOperator				\
    (									\
	thisType		value					\
    )									\
    {									\
	int			ix;					\
	int			iy;					\
									\
	for ( iy = 0; iy < abSpanY; iy++ )				\
	{								\
	    for ( ix = 0; ix < abSpanY; ix++ )				\
	    {								\
		if ( (*this)( ix, iy ) == blank() ) 			\
		{							\
		    (*this)( ix, iy ) = aBlank;				\
		}							\
		else							\
		{							\
		    (*this)( ix, iy ) thisOperator value;		\
		}							\
	    }								\
	}								\
									\
	return( *this );						\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// arrayOperator
//
// INVOCATION:
// arrayOperator( +=, int );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) thisOperator : This must be one of the C/C++ assignment operators.
// (>) thisType : This must be a C/C++ built in type.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This macro defines an operator method to allow the specified operation
// (thisOperator) to be performed on an array, using an array of type 
// thisType as the operand
//
// This macro is only useful for defining operator functions in the cArray
// class.
//
// DESCRIPTION:
// Loop over each element in the array, performing the required function.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This macro should only be used in the cArray class definition.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

#define	arrayOperator( thisOperator, thisType )				\
    cArray<T>	    &operator thisOperator				\
    (									\
	cArray<thisType>	&op2					\
    )									\
    {									\
	int		ix;						\
	int		iy;						\
	cAmStatus 	status;						\
	status.displayStop();						\
									\
									\
	if ( abSpanY == op2.spanY() && abSpanX == op2.spanX() ) 	\
	{								\
	    for ( iy = 0; iy < abSpanY; iy++ )				\
	    {								\
		for ( ix = 0; ix < abSpanX; ix++ )			\
		{							\
		    if ( (*this)( ix, iy ) == blank() || 		\
			    op2( ix, iy ) == op2.blank() )		\
		    {							\
			(*this)( ix, iy ) = aBlank;			\
		    }							\
		    else						\
		    {							\
			(*this)( ix, iy ) thisOperator op2( ix, iy );	\
		    }							\
		}							\
	    }								\
	}								\
	else								\
	{								\
	    status.E_INCOMPAT( status, # thisOperator, 			\
		    abSpanX, abSpanY, op2.spanX(), op2.spanY() );	\
	    throw status;						\
	}								\
									\
    return( *this );							\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// assignArrayOperator
//
// INVOCATION:
// assignArrayOperator( dataType )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataType : The data type to create the operator for.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create an assignment operator for the specified data type (dataType)
//
// This macro should only be used to define an assignment operator in the 
// cArray class.
//
// DESCRIPTION:
// - verify the arrays are compatable.
// - copy the values from the source array to the destination. Any blank
//   pixels in the source array are replaced with the destination array's
//   blank value.
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

#define	assignArrayOperator( dataType )					\
    cArray<T>	&operator=						\
    ( 									\
	cArray<dataType>						\
			&op2						\
    )									\
    {									\
	int		ix;						\
	int		iy;						\
	cAmStatus 	status;						\
	status.displayStop();						\
									\
	if ( spanY() == op2.spanY() && spanX() == op2.spanX() )		\
	{								\
	    for ( iy = 0; iy < spanY(); iy++ )				\
	    {								\
		for ( ix = 0; ix < spanX(); ix++ )			\
		{							\
		    if ( op2( ix, iy ) == op2.blank() )			\
		    {							\
			(*this)( ix, iy ) = (*this).blank();		\
		    }							\
		    else						\
		    {							\
			(*this)( ix, iy ) = op2( ix, iy );		\
		    }							\
		}							\
	    }								\
	}								\
	else								\
	{								\
	    status.E_INCOMPAT( status, "=",				\
		    abSpanX, abSpanY, op2.spanX(), op2.spanY() );	\
	    throw status;						\
	}								\
									\
	return( *this );						\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// assignConstOperator
//
// INVOCATION:
// assignConstOperator( int )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) dataType : The data type to create the operator for.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create an assignment operator to assign a constant to an array
//
// This macro should only be used to define an operator in the cArray
// class.
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

#define		assignConstOperator( dataType )				\
    cArray<T>	&operator=						\
    ( 									\
	dataType	constant					\
    )									\
    {									\
	int		ix;						\
	int		iy;						\
									\
	for ( iy = 0; iy < spanY(); iy++ )				\
	{								\
	    for ( ix = 0; ix < spanX(); ix++ )				\
	    {								\
		(*this)( ix, iy ) = constant;				\
	    }								\
	}								\
									\
	return( *this );						\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// maskOperator
//
// INVOCATION:
// maskOperator( int )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// dataType : The data type of the mask array.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create a bad pixel mask function where the mask array has the specified 
// data type
//
// This macro should only be used to define an operator in the cArray class.
//
// DESCRIPTION:
// - Loop over each of the elements in the data array.
// - If the corresponding element in the mask array has the mask array's
//   "blank" value, set the target array to the target array's blank value.
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

#define		maskOperator( dataType )				\
    cArray<T>	&operator^=						\
    ( 									\
	cArray<dataType>						\
			&mask						\
    )									\
    {									\
	int		ix;						\
	int		iy;						\
	cAmStatus 	status;						\
	status.displayStop();						\
									\
	if ( spanY() == mask.spanY() && spanX() == mask.spanX() )	\
	{								\
	    for ( iy = 0; iy < spanY(); iy++ )				\
	    {								\
		for ( ix = 0; ix < spanX(); ix++ )			\
		{							\
		    if ( mask( ix, iy ) == mask.blank() )		\
		    {							\
			(*this)( ix, iy ) = blank();			\
		    }							\
		}							\
	    }								\
	}								\
	else								\
	{								\
	    status.E_INCOMPAT( status, "^-",				\
		    spanX(), spanY(), mask.spanX(), mask.spanY() );	\
	    throw status;						\
	}								\
									\
	return( *this );						\
    }

//
//***********************************************************************
//+
// MACRO NAME:
// makeOperators
//
// INVOCATION:
// makeOperators( += )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) thisOperator : This must be one of the C/C++ assignment operators.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Create an operator function for arrays and constants of the specified
// type (thisType) for each of the operators supported
//
// DESCRIPTION:
// - Use the builtInOperator and arrayOperator macros to create an operator
//   for each of the supported built in and array types.
// - Use the assignConstOperator and assignArrayOperator macros to create
//   assignment operators for arrays and constants
// - Use the maskOperator macro to create mask operator for a mask array
//   of the specified type.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// This macro is only useful when used in the cArray class definition.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

#define	makeOperators( thisType )					\
	builtInOperator( +=, thisType );				\
	builtInOperator( -=, thisType );				\
	builtInOperator( *=, thisType );				\
	builtInOperator( /=, thisType );				\
	assignConstOperator( thisType );				\
	arrayOperator( +=, thisType );					\
	arrayOperator( -=, thisType );					\
	arrayOperator( *=, thisType );					\
	arrayOperator( /=, thisType );					\
	assignArrayOperator( thisType );				\
	maskOperator( thisType )


//
//***********************************************************************
//+
// CLASS NAME:
// cArray
//
// PUBLIC METHODS: 
// cArray - Constructor for the class.
// (T) operator()( int, int ) - return a reference to a specific element
//				in the array.
// (cArray<T>) operator()( int, int, int, int )
//			      - return a new array which is a sub-array 
//				of this array.
// (cArray<T>) operator()( cArray<T>, int, int, int, int ) 
//			      - return a new array which is a sub-array of 
//				this array.
// (cArray<T>) operator<op>( cArray<T2> )
//			      - Apply the specified operation to the array,
//				using an array of type T2 as the operand.
// (carray<T>) operator<op>( T2 )
//			      - Apply the specified operation ot the array,
//				using a value of type T2 as the operand.
// (<T>) blank( void )	      - Return the blank value for this array.
// (<T>) blank( <T> )	      - Set the blank value for this array.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// This class is used to store an array of values
//
// The class allows arithmatic to be performed on the array.
// The class allows a sub-array to be specified, as part of an existing 
// array, allowing arithmatic on sub-arrays.
//
// The makeOperators macro is used to define all of the operator methods
// supporting the permutations of operator and operand type. The macros
// could be simplified or eliminated if the compiler supported member 
// template functions.
//
//-
//***********************************************************************
//

template <class T>
class cArray : public cArrayBase
{
    public:
	inline		cArray( void );
	inline		cArray( T *, T, int, int );
	inline		cArray( cArray<T> &, int, int, int, int );
	T		&operator()( int ix, int iy )
			{
			    return( aPData[ ( ( iy + abStartY ) * abSizeX) + 
				    ix  + abStartX] );
			};
	cArray<T>	operator()( int startX, int spanX, int startY, 
				int spanY )
			{
			    cArray<T>	tmp( *this, startX, spanX, 
				    startY, spanY );
			    return( tmp );
			};
	inline cArray<T>
			&operator()( cArray<T>, int, int, int, int );
			makeOperators( unsigned char );
			makeOperators( signed char );
			makeOperators( short );
			makeOperators( unsigned short );
			makeOperators( int );
			makeOperators( long );
			makeOperators( float );
			makeOperators( double );
	T		blank( void ) { return( aBlank ); };
	T		blank( T blank ) { return( aBlank = blank ); };
    protected:
    private:
	T		*aPData;	// Pointer to the data for the array.
	T		aBlank;		// The value for blank pixels.
};
#undef builtInOperator
#undef arrayOperator
#undef assignArrayOperator
#undef assignConstOperator
#undef maskOperator
#undef makeOperators

//
//***********************************************************************
//+
// FUNCTION NAME:
// cArray
//
// INVOCATION:
// cArray<type>	array( data, blank, xSize, ySize );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) pData (<T> *) The pointer to the data array.
// (>) blank (<T>) The blank value for this array.
// (>) xSize (int) The size of the X axis.
// (>) ySize (int) The size of the Y axis.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the cArray class
//
// This constructor takes a pointer to a memory area containing the array
// data, the blank value for the array, and the size of the array.
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

template <class T>
		cArray<T>::cArray
(
    T		*pData,		// (in)  Pointer to the data array.
    T		blank,		// (in)  Blank value.
    int		xSize,		// (in)  X size of the data array.
    int		ySize		// (in)  Y size of the data array.
) : cArrayBase( (char *) pData, xSize, ySize, sizeof( T ) ) 
{
    aPData = pData;
    aBlank = blank;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cArray<T>::cArray
//
// INVOCATION:
// cArray<type>	array();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Default constructor for an array type
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

template <class T>
		cArray<T>::cArray
(
    void
) : cArrayBase( (char *) NULL, 0, 0, sizeof( T ) )
{
    aPData = NULL;
    aBlank = 0;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cArray<T>::cArray
//
// INVOCATION:
// N.A.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) array (cArray<T>) The array for which the sub-array is being created.
// (>) startX (int) Start of the X dimension.
// (>) spanX (int) Length of the X dimension.
// (>) startY (int) Start of the Y dimension.
// (>) spanY (int) Length of the Y dimension.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// This constructor creates a sub-array of an existing array
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

template <class T>
		cArray<T>::cArray
(
    cArray<T>	&array,		// (in)  Comment
    int		startX,		// (in)  Start of the X dimension.
    int		spanX,		// (in)  Length of the X dimension.
    int		startY,		// (in)  Start of the Y dimension.
    int		spanY		// (in)  Length of the Y dimension.
) : cArrayBase( array, startX, spanX, startY, spanY )
{
    aPData = array.aPData;
    aBlank = array.aBlank;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cArray<T>::operator()
//
// INVOCATION:
// cArray<int>	tempArray
// tempArray( sourceArray, 10, 100, 20, 200 );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) array (cArray<T>) The array this array will be a sub-array of.
// (>) startX (int) The X start position of the array in the sub-array.
// (>) spanX (int) The X length position of the array in the sub-array.
// (>) startY (int) The Y start position of the array in the sub-array.
// (>) spanY (int) The Y length position of the array in the sub-array.
//
// FUNCTION VALUE:
// (cArray<T> &) Return "this" array.
//
// PURPOSE:
// This function sets "this" array to a sub-region of the specified array
//
// DESCRIPTION:
// Check that the specified sub-region is valid.
// Change this array to point to the sub-region of the specified array.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// - This wouldn't work for finding a sub-region of a sub-region, but that
//   isn't required for the current application.
//
//-
//***********************************************************************
//

template <class T>
cArray<T>	&cArray<T>::operator()
(
    cArray<T>	array,		// (in)  The source of data for this array.
    int		startX,		// (in)  The start of the X dimension.
    int		spanX,		// (in)  The end of the X dimension.
    int		startY,		// (in)  The start of the Y dimension.
    int		spanY		// (in)  The end of the Y dimension.
)
{
    cAmStatus 	status;	
    status.displayStop();


    if ( startX < 0 || startY < 0 || spanX < 0 || spanY < 0 || 
	    startX + spanX > array.abSizeX || startY + spanY > array.abSizeY )
    {
	status.E_BOUNDS( status, startX, startX + spanX, startY, 
		startY + spanY, 0, array.abSizeX, 0, array.abSizeY );
	throw status;						\
    }
    abPData = array.abPData;
    abSizeX = array.abSizeX;
    abSizeY = array.abSizeY;
    abStartX = startX;
    abSpanX = spanX;
    abStartY = startY;
    abSpanY = spanY;
    aBlank = array.aBlank;
    aPData = array.aPData;

    return( *this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// operator<<
//
// INVOCATION:
// cArray<int>	array;
// ostream << array;
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) s (ostream) The stream to which the array will be written.
// (>) array (cArray<T>) The array to write.
//
// FUNCTION VALUE:
// (ostream) The stream to which the array was written.
//
// PURPOSE:
// Write the data for an array to the specified stream
//
// DESCRIPTION:
// Loop through the array, writting the data to the stream.
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

template< class T >
ostream		&operator<<
(
    ostream	&s,		// (in)  The stream to send to.
    cArray<T>	&array		// (in)  The array to display.
)
{
    int		ix;
    int		iy;

    for ( iy = 0; iy < array.spanY(); iy++ )
    {
	for ( ix = 0; ix < array.spanX(); ix++ )
	{
	    if ( array( ix, iy ) == array.blank() )
	    {
		s << "blank";
	    }
	    else
	    {
		s << array( ix, iy );
	    }
	    s << " ";
	}
	s << endl;
    }

    return( s );
}
#endif // ARRAY_H
