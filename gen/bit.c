/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1995				(c) 1995.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	gen/src/bit.c
*
*   Purpose:
*	Contains founctions to mainipulate bit strings.  All bit strings
*	are left justified, i.e. if a bit string contains uses only part
*	of a byte, the high order bits of the byte are the ones used.
*
*   Routines:
*	byte	*bitand		: And two bit strings.
*	int	bitcmp		: Compare two bit strings.
*	int	bitcomp		: Complement a bit string.
*	byte	*bitor		: Or two bit strings.
*	int	bitprint	: Print a bit string to a character string.
*	byte	*bitscan	: Read a string into a bit string.
*	byte	*bitshift	: Shift a bit string.
*	byte	*bitxor		: Exclusive or two bit strings.
*
*   Date		: Sep 27, 1995
*
*   SCCS data		: @(#)
*	Module Name	: bit.c
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 02/27/97
*
*   Programmer		: Norm Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include "gen_types.h"
#include "gen_bit.h"

static byte	mask_bits[] = 
			{ 0x00, 0x80, 0xc0, 0xe0, 0xf0, 
			        0xf8, 0xfc, 0xfe, 0xff };

/*+
************************************************************************
*
*   Function:	bitand
*
*   Purpose:
*	And two bit strings.  Either of the input bit strings may
*	be the same as the output bit string.  The result string is
*	always returned.
*
*   Values Returned:
*	byte	*result		: Pointer to the output bit string.
*
************************************************************************
-*/

byte		*bitand
(
    byte	*bitstr1,	/* (in)  The first bit string.		*/
    byte	*bitstr2,	/* (in)  The second bit string.		*/
    byte	*result,	/* (out) The result bit string.		*/
    int		bitstr_len	/* (in)  Number of bits in the strings.	*/
)
{
    int		bits_left;	/* Number of bit a partially used byte.	*/
    int		i;
    int		last_byte;	/* The byte number of the last byte.	*/


    for ( i = 0; i < bitstr_len / 8 ; i++ )
    {
	result[i] = bitstr1[i] & bitstr2[i];
    }

    bits_left = bitstr_len % 8;
    last_byte = ( bitstr_len - 1 ) / 8;
    if ( bits_left != 0 )
    {
	result[last_byte] = ( result[last_byte] & ~ mask_bits[ bits_left ] ) | 
		( ( bitstr1[ last_byte ] & bitstr2[ last_byte ] ) & 
		mask_bits[ bits_left ] );
    }

    return( result );
}

/*+
************************************************************************
*
*   Function:	bitcmp
*
*   Purpose:
*	Compare two bit strings.  0 is returned if the bit strings are
*	the same, otherwise the bit number of the first different bit is
*	returned.
*
*   Values Returned:
*	int	0		: The strings are the same.
*	int	diff_bit	: the bit of the first different bit.
*
************************************************************************
-*/

int		bitcmp
(
    byte	*bitstr1,	/* (in)  The first bit string.		*/
    byte	*bitstr2,	/* (in)  The second bit string.		*/
    int		bitstr_len	/* (in)  Number of bits in the strings.	*/
)
{
    int		bits_left;	/* Number of bit a partially used byte.	*/
    int		i;
    int		last_byte;	/* The byte number of the last byte.	*/


    for ( i = 0; i < bitstr_len / 8 ; i++ )
    {
	if ( ( bitstr1[i] - bitstr2[i] ) != 0 )
	{
	    return( bitstr1[i] - bitstr2[i] );
	}
    }

    bits_left = bitstr_len % 8;
    last_byte = ( bitstr_len - 1 ) / 8;
    if ( bits_left != 0 )
    {
	return( ( bitstr1[ last_byte ] & mask_bits[ bits_left ] ) -
		( bitstr2[ last_byte ] & mask_bits[ bits_left ] ) );
    }

    return( 0 );
}

/*+
************************************************************************
*
*   Function:	bitcomp
*
*   Purpose:
*	Complement a bit string.  The input string may be the same as
*	the result string.  The result string is always returned.
*
*   Values Returned:
*	byte	*result		: Pointer to the output bit string.
*
************************************************************************
-*/

byte		*bitcomp
(
    byte	*bitstr,	/* (in)  The first bit string.		*/
    byte	*result,	/* (out) The result bit string.		*/
    int		bitstr_len	/* (in)  Number of bits in the strings.	*/
)
{
    int		bits_left;	/* Number of bit a partially used byte.	*/
    int		i;
    int		last_byte;	/* The byte number of the last byte.	*/


    for ( i = 0; i < bitstr_len / 8 ; i++ )
    {
	result[i] = ~ bitstr[i];
    }

    bits_left = bitstr_len % 8;
    last_byte = ( bitstr_len - 1 ) / 8;
    if ( bits_left != 0 )
    {
	result[last_byte] = ( result[last_byte] & ~ mask_bits[ bits_left ] ) | 
		( ~ bitstr[ last_byte ] & mask_bits[ bits_left ] );
    }

    return( result );
}

/*+
************************************************************************
*
*   Function:	bitor
*
*   Purpose:
*	Or two bit strings.  Either of the input bit strings may
*	be the same as the output bit string.  The result string is
*	always returned.
*
*   Values Returned:
*	byte	*result		: Pointer to the output bit string.
*
************************************************************************
-*/

byte		*bitor
(
    byte	*bitstr1,	/* (in)  The first bit string.		*/
    byte	*bitstr2,	/* (in)  The second bit string.		*/
    byte	*result,	/* (out) The result bit string.		*/
    int		bitstr_len	/* (in)  Number of bits in the strings.	*/
)
{
    int		bits_left;	/* Number of bit a partially used byte.	*/
    int		i;
    int		last_byte;	/* The byte number of the last byte.	*/


    for ( i = 0; i < bitstr_len / 8 ; i++ )
    {
	result[i] = bitstr1[i] | bitstr2[i];
    }

    bits_left = bitstr_len % 8;
    last_byte = ( bitstr_len - 1 ) / 8;
    if ( bits_left != 0 )
    {
	result[last_byte] = ( result[last_byte] & ~ mask_bits[ bits_left ] ) | 
		( ( bitstr1[ last_byte ] | bitstr2[ last_byte ] ) & 
		mask_bits[ bits_left ] );
    }

    return( result );
}

/*+
************************************************************************
*
*   Function:	bitprint
*
*   Purpose:
*	Write a bit string to a character array.  The output string
*	length should be at least input string length * 2 + 3.
*
*   Values Returned:
*	int	0		: Error occured.
*	int	nbytes		: length of output string.
*
************************************************************************
-*/

int		bitprint
(
    char	*string,	/* (out) String to print to.		*/
    byte	*bitstr,	/* (in)  The input bit string.		*/
    int		bitstr_len	/* (in)  Length of the bit string (bits)*/
)
{
    char	buffer[3];
    int		i;


    (void) strcpy( string, "0x" );
    for ( i = 0; i < ( bitstr_len + 1 ) / 8; i++ )
    {
	(void) sprintf( buffer, "%02x", bitstr[i] );
	(void) strcat( string, buffer );
    }

    return( strlen( string ) );
}

/*+
************************************************************************
*
*   Function:	bitscan
*
*   Purpose:
*	Turn a character string in the form "0xfff..." into a bit array.
*
*   Values Returned:
*	byte	*result		: Pointer to the output bit string.
*
************************************************************************
-*/

byte		*bitscan
(
    char	*string,	/* (out) String to print to.		*/
    byte	*result,	/* (in)  The input bit string.		*/
    int		bitstr_len	/* (in)  Length of the bit string (bits)*/
)
{
    int		bit_pos;
    char	*char_start;
    int		i;
    int		value;


    /*
     *  Look for the leading "0x"
     */

    if ( ( char_start = strstr( string, "0x" ) ) == NULL )
    {
	return( NULL );
    }

    for ( i = 2, bit_pos = 0; 
	    i < strlen( char_start ) && bit_pos < ( bitstr_len + 1 ) / 8 ; 
	    i+=2, bit_pos++ )
    {
	(void) sscanf( char_start + i, "%02x", &value );
	result[ bit_pos ] = value;
    }

    for ( i = bit_pos; i < ( bitstr_len + 1 ) / 8; i++ )
    {
	result[ i ] = 0;
    }

    return( result );
}

/*+
************************************************************************
*
*   Function:	bitshift
*
*   Purpose:
*	Shift a bit string.  Not yet implemented.  The shift should be
*	positive for shift right, negative for shift left.
*
*   Values Returned:
*	byte	*result		: Pointer to the output bit string.
*
************************************************************************
-*/

byte		*bitshift
(
    byte	*bitstr,	/* (in)  The bit string.		*/
    byte	*result,	/* (out) The result bit string.		*/
    int		bitstr_len,	/* (in)  Number of bits in the strings.	*/
    int		shift		/* (in)  Number of bits to shift.	*/
)
{
    (void) fprintf( stderr, "bitshift not implemented\n" );

    return( NULL );
}

/*+
************************************************************************
*
*   Function:	bitxor
*
*   Purpose:
*	Exculsive or two bit strings.  Either of the input bit strings may
*	be the same as the output bit string.  The result string is
*	always returned.
*
*   Values Returned:
*	byte	*result		: Pointer to the output bit string.
*
************************************************************************
-*/

byte		*bitxor
(
    byte	*bitstr1,	/* (in)  The first bit string.		*/
    byte	*bitstr2,	/* (in)  The second bit string.		*/
    byte	*result,	/* (out) The result bit string.		*/
    int		bitstr_len	/* (in)  Number of bits in the strings.	*/
)
{
    int		bits_left;	/* Number of bit a partially used byte.	*/
    int		i;
    int		last_byte;	/* The byte number of the last byte.	*/


    for ( i = 0; i < bitstr_len / 8 ; i++ )
    {
	result[i] = bitstr1[i] ^ bitstr2[i];
    }

    bits_left = bitstr_len % 8;
    last_byte = ( bitstr_len - 1 ) / 8;
    if ( bits_left != 0 )
    {
	result[last_byte] = ( result[last_byte] & ~ mask_bits[ bits_left ] ) | 
		( ( bitstr1[ last_byte ] ^ bitstr2[ last_byte ] ) & 
		mask_bits[ bits_left ] );
    }

    return( result );
}
