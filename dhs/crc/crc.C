//+
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998.				(c) 1998.
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties		Le CNRC denie toute garantie
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
//   Module Name:	crc/src/crc.C
//
//   Purpose:
//	Contains the definition of crc_buffer, the C wrapper to the C++
//	32 bit crc class.
//
//   Routines:
//	char		*crc322str	: Convert a 32 bit crc to a string.
//	char		*crc642str	: Convert a 64 bit crc to a string.
//	crc32_t 	crc_buffer	: The C wrapper function.
//	crc64_t		crc_buffer64	: The 64 bit C wrapper function.
//
//   Date		: Nov 1, 1999
//
//   SCCS data		: %Z%
//	Module Name	: %M%
//	Version Number	: %I%
//	Release Number	: %R%
//	Last Updated	: %G%
//
//   Programmer		: Norm Hill
//
//   Modification History:
//	99/12/02 nrh	: Added function crc_buffer64.
//	00/04/06 nrh	: Added functions crc322str and crc642str.
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//-

#include "gen_types.h"
#include "crc.H"
#include "crc.h"

//+
//***********************************************************************
//
//   Function:	crc322str
//
//   Purpose:
//	Convert a 32 bit crc into a hex string. The string passed in must
//	be at least 11 bytes long.
//
//   Values Returned:
//	char *			: The string passed in.
//
//***********************************************************************
//-

extern "C" 
char		*crc322str
(
    crc32_t 	crc,		// (in)  The crc value to format.
    char	*string		// (out) The string to format.
)
{
    sprintf( string, "0x%08x", crc );
    return( string );
}

//+
//***********************************************************************
//
//   Function:	crc642str
//
//   Purpose:
//	Convert a 64 bit crc into a hex string. The string passed in must
//	be at least 19 bytes long.
//
//   Values Returned:
//	char *			: The string passed in.
//
//***********************************************************************
//-

extern "C" 
char		*crc642str
(
    crc64_t	crc,		// (in)  The crc value to format.
    char	*string		// (out) The string to format.
)
{
    sprintf( string, "0x%016llx", crc );
    return( string );
}

//+
//***********************************************************************
//
//   Function:	crc_buffer.
//
//   Purpose:
//	The crc wrapper function.
//
//   Values Returned:
//	unsigned long		: The crc value.
//
//***********************************************************************
//-
/* XXX allan: multiply defined ...
crc32_t		crc_buffer
(
    unsigned int		// (in)  The buffer length.
    		length,
    const char	*buffer		// (in)  The data buffer.
)
{
    return( crc32( buffer, length ) );
}
XXX */

//+
//***********************************************************************
//
//   Function:	crc_buffer64
//
//   Purpose:
//	The 64 bit crc wrapper function.
//
//   Values Returned:
//	unsigned long long	: The crc value.
//
//***********************************************************************
//-

extern "C" 
crc64_t		 crc_buffer64
(
    unsigned int		// (in)  The buffer length.
    		length,
    const char	*buffer		// (in)  The data buffer.
)
{
    return( crc64( buffer, length ) );
}
