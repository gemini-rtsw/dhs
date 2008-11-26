//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998.
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
// dhsStorage/h/stoEpicsRecord.H
//
// PURPOSE:
// Contains class definitions useful for maintaining information in EPICS. 
//
// CLASSES:
// cStoEpicsRecord - Template class which allows creation of a data member
// 		     which will flush itself to EPICS. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/27 17:15:09  brighton
// ported to gcc-3.2.1/linux
//
// Revision 1.1.1.1  2002/11/24 20:31:26  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  2000/12/19 19:37:18  nhill
// Updated for workshop 5.
//
// Revision 1.7  1999/06/16 23:48:50  nhill
// Major re-organization.
//
// Revision 1.6  1998/04/23 16:15:50  cockayne
// Modified to use new status and health.
//
// Revision 1.5  1998/04/20 20:37:15  cockayne
// Modified the unknown epics value string from "UNKNOWN" to
// the more eye-pleasing "-", which looks better on the console.
//
// Revision 1.4  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOEPICSRECORD_H
#define STOEPICSRECORD_H

#include <iostream>
#include <string>
#include <sstream>
#include "globals.H"


#define STO_ERV_UNKNOWN		"-"
#define STO_ERV_BLANK		""

static	char	trueString[] = "True";
static	char	falseString[] = "False";


//
//***********************************************************************
//+
// CLASS NAME:
// cStoEpicsRecord
//
// PUBLIC METHODS:
// (cStoEpicsRecord)	cStoEpicsRecord	- Constructor (5 flavours).
// ()			~cStoEpicsRecord - Deconstructor.
// ()			operator const cUserClass &
//					- Cast operator to produce the value
//					  of the record.
// (cStoEpicsRecord &)	operator=	- Assigns value of object.
// (void)		blank		- Blank the value of the record.
// (void)		flush		- Send the value to the epics database.
// (void)		setIndex	- Sets object's index (if indexed).
// (cUserClass &)	value		- Returns object's value.
// (bool)		valueKnown	- Returns true if the value is known.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
//
// PURPOSE:
// Facilitates updating SAD records in Epics through the Status Server.
//
// This template class handily stores any type of value and flushes it 
// to the DHS Status Server in order for Epics to be kept current. At time
// of creation, the Epics record name is set. It allows a 'transformation' 
// function to be registered at object creation time, to allow a value 
// to have an operation performed on it before it ends up in Epics. 
//-
//***********************************************************************
//
 
template< class cUserClass, class cParentClass, class cStatusClass >
class   cStoEpicsRecord
{
    public:
	typedef char		*(cParentClass::*pfcs)( cUserClass );
	typedef char		*(*pfcs2)( cUserClass );

	//
	// Constructors (5) and Destructor.
	//
	
	inline		cStoEpicsRecord( void )
			{
			    erFormat = NULL;
			    erPParentObject = NULL;
			    erTransFunction = NULL;
			};
			cStoEpicsRecord( const char *, cParentClass *, pfcs );
						// Non-indexed. 
			cStoEpicsRecord( const char *, const cUserClass &, 
				cParentClass *, pfcs );
						// Non-indexed plus init-
						// ializing the value.
			cStoEpicsRecord( int, const char *, cParentClass *, pfcs );
						// Indexed.
			cStoEpicsRecord( int, const char *, 
				const cUserClass &, cParentClass *, pfcs );
						// Indexed plus initialization
						// of the value.
			~cStoEpicsRecord( void );


	//
	// Set value, Get value, and Set index methods. 
	//
	
			operator const cUserClass & ()
			{
			    return( erValue );
			}
	cStoEpicsRecord	&operator=( const cUserClass & );
	void		blank( void )
			{
			    cStatusClass
					status;		// Program status. 
			    cStaStatus::update( erName, "", status );
			    cStaStatus::flush( status );
			    if ( !status.ok() )
			    {
				status.E_STA( status, erName );
			    }
			}
	void		flush( void );
	void		setIndex( const int );
	inline cUserClass	
			&value( void )
			{
			    return( erValue );
			};
	inline bool	valueKnown( void )
			{
			    return( erValueKnown );
			}

    protected:

    private:
	bool		erIndexed;	// Is this record indexed?
	int		erIndex;	// Index in array (if indexed).
	char		*erFormat;	// Format string for record name.
	char		*erName;	// Name of EPICS record. 
	cUserClass	erValue;	// Value to store in EPICS.
	bool		erValueKnown;	// Is value known or unknown?
	pfcs		erTransFunction;// Transformation function. 
	cParentClass	*erPParentObject;// 
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass >
//              cStoEpicsRecord< cUserClass >::cStoEpicsRecord
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name  (char *)  Name of the EPICS Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the class.
//
// This version of the constructor is for un-initialized, un-indexed 
// EPICS records. 
//
// DESCRIPTION:
// - Save the EPICS record name.
// - As value is not yet known, update the record with UNKNOWN.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::
		cStoEpicsRecord
(
    const char	*name,		// Name of the EPICS record. 
    cParentClass *pParentObject, 
    pfcs	fnPtr		// Transformation function pointer.
)
{
    cStatusClass
		status;		// Program status. 


    //
    // Initialize class data.
    //
    
    erIndexed = false;
    erFormat = strsav( name );
    erName = erFormat;
    erValueKnown = false;

    erPParentObject = pParentObject;

    erTransFunction = ( ( fnPtr == 0 ) ? 0 : fnPtr );


    flush();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cStatusClass >
//              cStoEpicsRecord< cUserClass, cStatusClass >::cStoEpicsRecord
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name  (char *)       Name of the EPICS Record.
// (>) value (cUserClass &) Value with which to Initialize the Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the class
//
// This version of the constructor is for un-indexed but initialized
// EPICS records. 
//
// DESCRIPTION:
// - Save the EPICS record name.
// - Update the record with the string representation of the value.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::cStoEpicsRecord
(
    const char	*name,		// Name of the EPICS record. 
    const cUserClass
    		&value,		// Value to initialize the EPICS record. 
    cParentClass	*pParentObject,
    pfcs	fnPtr		// Transformation function pointer.
)
{
    //
    // Initialize class data.
    //
    
    erIndexed = false;
    erFormat = strsav( name );
    erName = erFormat;
    erValue = value;
    erValueKnown = true;

    erPParentObject = pParentObject; 

    erTransFunction = ( ( fnPtr == 0 ) ? 0 : fnPtr );


    flush();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass >
//              cStoEpicsRecord< cUserClass >::cStoEpicsRecord
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name  (char *)  Name of the EPICS Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the class
//
// This version of the constructor is for un-initialized, but indexed 
// EPICS records.
//
// DESCRIPTION:
// - Save the EPICS record name.
// - As value is not yet known, update the record with UNKNOWN.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::cStoEpicsRecord
(
    int		index,		// Index of the array row. 
    const char	*format,	// Format string for the EPICS record. 
    cParentClass	*pParentObject,
    pfcs	fnPtr		// Transformation function pointer.
)
{
    cStatusClass
		status;		// Program status. 


    //
    // Save the record name and index for the object. 
    //
    
    erIndexed = true;
    erFormat = strsav( format );
    erPParentObject = pParentObject;
    erTransFunction = ( ( fnPtr == 0 ) ? 0 : fnPtr );
    erValueKnown = false;
    setIndex( index );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// template< class cUserClass, class cStatusClass >
//              cStoEpicsRecord< cUserClass, cStatusClass >::cStoEpicsRecord
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name  (char *)       Name of the EPICS Record.
// (>) value (cUserClass &) Value with which to Initialize the Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the class
//
// DESCRIPTION:
// - Save the EPICS record name.
// - Update the record with the string representation of the value.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::cStoEpicsRecord
(
    int		index,		// Index of the array row. 
    const char	*format,	// Format string for the EPICS record. 
    const cUserClass
    		&value,		// Value to initialize the EPICS record. 
    cParentClass	*pParentObject,
    pfcs	fnPtr		// Transformation function pointer.
)
{
    char	tempName[40];	// Temporary string to format record name.


    erIndexed = true;
    erIndex = index;
    erFormat = strsav( format );
    (void) sprintf( tempName, format, erIndex );
    erName = strsav( tempName );
    erValue = value;
    erValueKnown = true;

    erPParentObject = pParentObject;

    erTransFunction = ( ( fnPtr == 0 ) ? 0 : fnPtr );


    flush();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoEpicsRecord::&operator=
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) value  (cUserClass)  New Value for the EPICS Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Assignment operator for the class. 
//
// DESCRIPTION:
// - Update the value stored in the class. 
// - Update the record with the string representation of the value.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
cStoEpicsRecord<cUserClass, cParentClass, cStatusClass>	
		&cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::operator=
(
    const cUserClass
    		&value		// New value of the EPICS record. 
)
{
    //
    // Set value.
    //
    
    erValue = value;
    erValueKnown = true;


    flush();


    return( *this );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoEpicsRecord::flush
//
// INVOCATION:
// record.flush()
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Flush the value of an epics record to the epics database
//
// DESCRIPTION:
// Description of the algorithm.
//
// EXTERNAL VARIABLES:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

template< class cUserClass, class cParentClass, class cStatusClass >
void		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::flush
(
    void
)
{
    cStatusClass status;		// Program status. 

    // XXX allan: replaced char buffer and ostrstream (deprecated)
    //const int	bufLen = 128;	// Character buffer length.
    //char	buffer[bufLen];	// Character buffer.
    //ostrstream	oss( buffer, bufLen, ios::out );
    std::ostringstream	oss;    // Output string stream object.


    //
    // If possible, output value or UNKNOWN. 
    //

    if ( ( ( erIndexed && erIndex != -1 ) || ( !erIndexed ) ) &&
	    &(cStaStatus::connection()) != NULL && 
	    cStaStatus::connection().isOpen() )
    {
	if ( erValueKnown )
	{
	    if ( erTransFunction == 0 ) 
	    {
		oss << erValue;
	    }
	    else
	    {
		oss << (erPParentObject->*erTransFunction)( erValue );
	    }
	}
	else
	{
	    oss << STO_ERV_UNKNOWN;
	}
	cStaStatus::update( erName, oss.str().c_str(), status );
	cStaStatus::flush( status );
	if ( !status.ok() )
	{
	    status.E_STA( status, erName );
	}
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoEpicsRecord::setIndex
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) value  (cUserClass)  New Value for the EPICS Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Assignment operator for the class. 
//
// DESCRIPTION:
// - Update the value stored in the class. 
// - Update the record with the string representation of the value.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
void		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::setIndex
(
    const int	index		// New index of EPICS record. 
)
{
    char        tempName[40];   // Temporary string to format record name.


    //
    // If this record is not indexed, this doesn't make much sense.
    //

    if ( !erIndexed )
    {
	//
	// Print some error message about trying to index non-indexed bla bla. 
	//
	return;
    }


    //
    // Set index, create new name from format and index.. 
    //

    erIndex = index;
    (void) sprintf( tempName, erFormat, erIndex );
    erName = strsav( tempName );


    flush();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStoEpicsRecord::~cStoEpicsRecord
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) value  (cUserClass)  New Value for the EPICS Record. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Assignment operator for the class. 
//
// DESCRIPTION:
// - Update the value stored in the class. 
// - Update the record with the string representation of the value.
// - Flush the status Server. 
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

template< class cUserClass, class cParentClass, class cStatusClass >
		cStoEpicsRecord< cUserClass, cParentClass, cStatusClass >::~cStoEpicsRecord
(
    void
)
{
    //
    // Free memory which was allocated for class data. 
    //

    if ( erFormat != NULL )
    {
	if ( erFormat != erName )
	{
	    gen_free( erName );
	    erName = NULL;
	}
	gen_free( erFormat );
	erFormat = NULL;
    }
}
#endif // STOEPICSRECORD_H
