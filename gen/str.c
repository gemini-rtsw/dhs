/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	gen/src/str.c
*
*   Purpose:
*	This file contains miscellaneous string manipulation routines.
*
*   Routines:
*	int	    genstrcasecmp: Compare two string ignoring case.
*	static int  pattinc	: Return a increment value to pattmatch.
*	static char pattmatch	: Try to match a string and a pattern.
*	boolean	    str2bool	: Convert a string to a boolean.
* 	boolean     str2double  : Convert a string to a double value.
*	boolean	    str2float	: Convert a string to a float.
*	boolean	    str2int	: Convert a string to an integer.
* 	boolean     str2long    : Convert a string to a long value.
*	void	    str2lower	: Make all chars lower case.
* 	boolean	    str2short   : Convert a string to a short value.
* 	boolean     str2uint    : Convert a string to a unsigned int value.
* 	boolean	    str2ulong   : Convert a string to a unsigned long value.
*	void	    str2upper	: Make all chars upper case.
* 	boolean	    str2ushort  : Convert a string to a unsigned short value.
*	char	    *strapp	: Append second string to first, adjusting
*				  memory allocation of the first string.
*	char	    *strext	: Removes prefixes ending with a dot (:e).
*	char	    *strfit	: Removes leading and trailing spaces.
*	boolean	    strdouble   : Is the string a valid double?
*	boolean	    strfloat	: Is the string to a valid float?
*	void	    strhead	: Remove last component of pathname (:h).
*	int	    strchar	: Pointer to a non-escaped char in a string.
*	boolean	    strint	: Is the string to a valid integer?
*	char	    *strnapp	: Appends second string to first.
*	char	    *strnsav	: Make a copy of a string with a max length..
*	boolean	    strpattern	: Check if a string matches a pattern.
*	void	    strroot	: Removes suffixes ending with a dot (:r).
*	char	    *strsav	: Make a copy of a string.
*	char	    *strtail	: Returns basename of the pathname (:t).
*	void	    strtokens	: Tokenize a string.
*
*   Date		: Dec 12, 1990
*
*   SCCS data		: @(#)
*	Module Name	: str.c
*	Version Number	: 1.27
*	Release Number	: 1
*	Last Updated	: 02/08/99
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	91/03/05 gjf -	Added strext, strhead, strroot, strtail.
*	92/03/02 Norman Hill - Added strnsav.
*	92/03/03 Bruce Enns - Added strpattern and strindex.
*	92/09/16 Norman Hill - Added strapp.
*	92/11/25 Norman Hill - Added strnapp.
*	93/02/16 Norman Hill - Modified strtokens to recognize quotes 
*			enclosing tokens.
*	98/10/26 SDJ	: Added str2double, str2short, str2uint, str2ushort
*	99/01/25 SDJ	: Changed logic of str2double to match the logic
*			  used in str2float.
*	99/02/06 SDJ	: Fixed but in str2double (== changed to !=).
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/
#include <errno.h>
#ifndef vxWorks
#include <stdarg.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include "gen_types.h"
#include "gen_str.h"
static int	genstrcasecmp( const char *, const char * );


/*+
************************************************************************
*
*   Synopsis:
*	static int	pattinc( str, c )
*
*   Purpose:
*	Returns the number of characters to add to str
*	to get to the character specified. 
*
*   Parameters:
*	char	*str		: (in)  The string.
*	char	c		: (in)  THe character to find.
*
*   Values Returned:
*	int			: The index of the character in the string.
*
*   References:
*	From the od_strloc1 function in llevel.c module of pODlib from ECF.
*
************************************************************************
-*/

static int	pattinc(const char *str, char c)
{
    const char	*p;

    for ( p = str; *p; p++ )
    {
	if ( *p == '\\' )
	{
	    p++; continue;
	}
	if ( *p == c )
	{
	    break;
	}
    }

    return( p - str );
}

/*+
************************************************************************
*
*   Synopsis:
*	static char	pattmatch( source, pattern )
*
*   Purpose:
*	Check if a source string matches the pattern given.
*
*   Parameters:
*	char	*source		: (in)  The string to look for.
*	char	*pattern	: (in)  The pattern to use.
*
*   Values Returned:
*	char			: A non-zero character indicates that
*				  the string did not match the pattern.
*				  Zero indicates a match.
*
*   References:
*	From the strpdiff function in pODlib from ECF.
*	Enhanced with checking for ranges of characters in lists by BE.
*
************************************************************************
-*/

static char	pattmatch(const char *source, const char *pattern)
{
    int	pattinc(const char *str, char c);

    char	cc;
    int		d;
    int		found;
    int		range;
    const char	*os;
    const char	*p;
    const char	*s;

    s = source;
    p = pattern;


    /*
     *  Fixed pattern.
     */

    while ( *p != '*' )
    {
	if ( *p == '\\' )
	{
	    p++;
	}


	/*
	 *  Match any single character.
	 */

	else if ( *p == '?' )
	{
	    if ( *s )
	    {
		s++;
		p++;
		continue;
	    }


	    /*
	     *  Doesn't match...
	     */

	    return( *p );
	}


	/*
	 *  Check if in list or range.
	 */

	else if ( *p == '[' )
	{
	    range = 0;
	    for ( os = ++p; *p; p++ )
	    {
		if ( *p == ']' )
		{
		    /*
		     *  Mismatch...
		     */

		    return( *p );
		}

		os = p;
		if ( *p == '\\' )
		{
		    p++;
		}

                if ( *s == *p )
                {
                    break;
                }
		else if ( *p == '-' )
		{
		    /*
		     *  Check all characters within the range
		     *  except the first, which has already been checked.
		     */

		    found = 0;
		    for ( cc = *(p - 1) + 1; cc <= *(p + 1); cc++ )
		    {
			if ( *s == cc )
			{
			    found = 1;
			    break;
			}
		    }


		    /*
		     *  If the character was found, increment s
		     *  and increment p to the first character 
		     *  following this list.
		     */

		    if ( found )
		    {
			s++;
			while ( *p != ']' )
			{
			    p++;
			}
			p++;


			/* 
			 *  If another list follows, process it as well.
			 *  Otherwise, break.
			 */

                        if ( *p != '[' )
                        {
			    range = 1;
			    if ( *p == '\\' )
			    {
				p++;	/* Esc.	*/
			    }
			    break;
                        }
		    }
		}
            }

    	    if ( *s != *p )
	    {
	        return( *s );
	    }
	    if ( streq( p, "" ) )
	    {
		return( 0 );
	    }
	    if ( range == 0 )
	    {
		p = os + pattinc( os, ']' );
	    }
	    p++;
	    s++;
	    continue;
	}

	d = *p - *s;
	if ( d != 0 )
	{
	    return( d );
	}

	if ( *p == '\0' )
	{
	    return( 0 );
	}

	p++;
	s++;
    }


    /*
     *  Variable-length pattern.  Match as many as possible.
     */

    os = s;
    p += 1;			/* Old source position	*/
    s += strlen( s );		/* Match as many as possible */

    while ( s >= os )
    {
	if ( pattmatch( s, p ) == 0 )
	{
	    /*
	     *  Match...
	     */

	    return( 0 );
	}

	s--;
    }


    /*
     *  Mismatch...
     */

    return( *os );
}

/*+
************************************************************************
*
*   Function:	str2bool
*
*   Purpose:
* 	Convert a string to a boolean value
*
*	Allowed strings for true are: yes, true, y, t, or 1 (case insensitive).
*	Allowed strings for false are: no, false, n, f, or 0 (case insensitive).
*
*   Values Returned:
*	boolean	TRUE		: The value was successfully coverted
*	boolean	FALSE		: The value could not be converted.
*
************************************************************************
-*/

boolean		str2bool
(
    const char	*str,		/* (in)  The string to convert.		*/
    boolean	*value		/* (out) The return boolean value.	*/
)
{
    if ( genstrcasecmp( str, "yes" ) == 0 || 
	    genstrcasecmp( str, "true" ) == 0 ||
	    genstrcasecmp( str, "y" ) == 0 || 
	    genstrcasecmp( str, "t" ) == 0 ||
	    strcmp( str, "1" ) == 0 )
    {
	*value = TRUE;
	return( TRUE );
    }
    else if ( genstrcasecmp( str, "no" ) == 0 || 
	    genstrcasecmp( str, "false" ) == 0 ||
	    genstrcasecmp( str, "n" ) == 0 || 
	    genstrcasecmp( str, "f" ) == 0 ||
	    strcmp( str, "0" ) == 0 )
    {
	*value = FALSE;
	return( TRUE );
    }
    else
    {
	return( FALSE );
    }
}

/*+
************************************************************************
*
*   Synopsis:
*       boolean str2double( s, d )
*
*   Purpose:
*       This routine converts the string 's' into a double value.
*       If the string does not contain a valid number, FALSE is returned.
*
*   Parameters:
*       char    *s              : (in)  String to verify.
*       double  *d              : (out) Floating point result.
*
*   Values Returned:
*       boolean TRUE            : Successful conversion.
*       boolean FALSE           : 's' does not represent a valid number.
*
************************************************************************
-*/

boolean	str2double
(
    const char	*s,		/* (in)  String to convert.		*/
    double	*d		/* (out) Double return value.		*/
)	
{
    char	*endptr;	/* Where the conversion stopped.	*/
    int		len;		/* Length of the string.		*/
     boolean	ret;		/* Status of conversion.		*/
    
    *d = 0.0;
    len = strlen( s );
    errno = 0;
    

    /*
     * Convert the string to a double value.
     */
    
    *d = strtod( s, &endptr );
    

    /*
     * Check for errors.
     */

    if ( endptr - s == len && len > 0 && errno != ERANGE && errno != EINVAL &&
	    ( *s == '-' || *s == '+' || *s == '.' || isdigit( *s ) ) )
    {
	ret = TRUE;
    }
    else
    {
	ret = FALSE;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Synopsis:
*       boolean	str2float( s, f )
*
*   Purpose:
*	This routine converts the string 's' into a floating point number.
*	If the string does not contain a valid number, FALSE is returned.
*
*   Parameters:
*	char	*s		: (in)  String to verify.
*	float	*f		: (out) Floating point result.
*
*   Values Returned:
*	boolean	TRUE		: Successful conversion.
*	boolean FALSE		: 's' does not represent a valid number.
*
************************************************************************
-*/

boolean	str2float(const char *s, float *f)
{
    if ( !strfloat( s ) )
    {
	return( FALSE );
    }
    
    *f = (float) atof( s );
    return( TRUE );
}

/*+
************************************************************************
*
*   Synopsis:
*       boolean	str2int( s, i )
*
*   Purpose:
*	This routine converts the string 's' into an integer number.  If
*	the string does not contain a valid number, FALSE is returned.
*
*   Parameters:
*	char	*s		: (in)  String to verify.
*	int	*i		: (out) Integer result.
*
*   Values Returned:
*	boolean	TRUE		: Successful conversion.
*	boolean FALSE		: 's' does not represent a valid number.
*
************************************************************************
-*/

boolean	str2int(const char *s, int *i)
{
    if ( !strint( s ) )
    {
	return( FALSE );
    }
    
    *i = atoi( s );
    return( TRUE );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	str2lower( lower, s )
*
*   Purpose:
*	This routines copies string 's' into 'lower', changing all
*	uppercase characters to lower case.
*
*   Parameters:
*	char	*lower		: (out) Will contain lower case version of s.
*	char	*s		: (in)  String of characters to be "lowered".
*
************************************************************************
-*/

void	str2lower(char *lower, const char *s)
{
    char	*dst;
    const char	*src;
    
    for ( dst = lower, src = s; *src != '\0'; ++src, ++dst )
    {
	if ( isalpha( *src ) && isupper( *src ) )
	{
	    *dst = tolower( *src );
	}
	else
	{
	    *dst = *src;
	}
    }
    
    *dst = '\0';
}

/*+
************************************************************************
*
*   Synopsis:
*       boolean str2short( s, si )
*
*   Purpose:
*       This routine converts the string 's' into a short integer.  If
*       the string does not contain a valid number, FALSE is returned.
*
*   Parameters:
*       char    *s              : (in)  String to verify.
*       int     *si        	: (out) Short integer result.
*
*   Values Returned:
*       boolean TRUE            : Successful conversion.
*       boolean FALSE           : 's' does not represent a valid number.
*
************************************************************************
-*/

boolean	str2short
(
    const char	*s,		/* (in)  String to convert.		*/
    short	*si		/* (out) short result.			*/
)
{
    int	i;		/* Temporary integer value.		*/
    boolean	ret;		/* status of the conversion.		*/

    /*
     * Do the conversion.
     */

    
    ret = TRUE;
    if( !str2int( s, &i ) )
    {
	ret = FALSE;
    }
    else if ( i > SHRT_MAX )
    {
	ret = FALSE;
    }
    else if ( i < SHRT_MIN )
    {
	ret = FALSE;
    }
    else
    {
	*si = (short)i;
    }
    
    return( ret );
}

/*+
************************************************************************
*
*   Function:	str2uint
*
*   Purpose:
*       This routine converts the string 's' into an unsigned integer.  If
*       the string does not contain a valid number, FALSE is returned.
*
*   Parameters:
*       char    *s              : (in)  String to verify.
*       int     *u        	: (out) Short integer result.
*
*   Values Returned:
*       boolean TRUE            : Successful conversion.
*       boolean FALSE           : 's' does not represent a valid number.
*
************************************************************************
-*/

boolean	str2uint
(
    const char	    *s,		/* (in)  String to convet.		*/
    unsigned int    *u		/* (out) Unsigned int result.		*/
)
{
    double	d;		/* Temporary double result.		*/
    boolean	ret;		/* Status of conversion.		*/

    /*
     * Do the conversion.
     */
    
    ret = TRUE;
    d = 0;
    if ( ! str2double( s, &d ) )
    {
	ret = FALSE;
    }
    else if ( d > UINT_MAX )
    {
	ret = FALSE;
    }
    else
    {
	*u = (unsigned int)d;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	str2upper( upper, s )
*
*   Purpose:
*	This routines copies string 's' into 'upper', changing all
*	uppercase characters to lower case.
*
*   Parameters:
*	char	*upper		: (out) Will contain upper case version of s.
*	char	*s		: (in)  String of characters to be "uppered".
*
************************************************************************
-*/

void	str2upper(char *upper, const char *s)
{
    char	*dst;
    const char	*src;
    
    for ( dst = upper, src = s; *src != '\0'; ++src, ++dst )
    {
	if ( isalpha( *src ) && islower( *src ) )
	{
	    *dst = toupper( *src );
	}
	else
	{
	    *dst = *src;
	}
    }
    
    *dst = '\0';
}

/*+
************************************************************************
*
*   Function:	str2ushort
*
*   Purpose:
*       This routine converts the string 's' into an unsigned short.  If
*       the string does not contain a valid number, FALSE is returned.
*
*   Parameters:
*       char    *s              : (in)  String to verify.
*       int     *u        	: (out) Short integer result.
*
*   Values Returned:
*       boolean TRUE            : Successful conversion.
*       boolean FALSE           : 's' does not represent a valid number.
*
************************************************************************
-*/

boolean	str2ushort
(
    const char	    *s,		/* (in)  String to convert.		*/
    unsigned short  *u		/* (out) Unsigned short resulte.	*/
)
{
    unsigned int    ui;		/* Temporary unsigned int value.	*/
    boolean	    ret;	/* Status of conversion.		*/


    ret = TRUE;
    *u = 0;

    
    /*
     * Do the conversion.
     */
    if ( ! str2uint( s, &ui ) )
    {
	ret = FALSE;
    }
    else if ( ui > USHRT_MAX )
    {
	ret = FALSE;
    }
    else
    {
	*u = (unsigned short)ui;
    }

    return( ret );
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strapp( string1, string2 )
*
*   Purpose:
*	Appends string2 to string1, adjusting the memory allocation if
*	necessary.  A pointer to the new string is returned.  Note that
*	the routine may move string1, so it it neccessary to use the
*	output value.
*
*   Parameters:
*	char	*string1	: (in)	First string.
*	char	*string2	: (in)	String to append.
*
*   Values Returned:
*	char	*string1	: A pointer to the new string, or null 
*				  if for memory allocation failure.
*
************************************************************************
-*/
char	*strapp(char *string1, const char *string2)
{
    string1 = gen_realloc( string1, strlen( string1 ) + 
	    strlen( string2 ) + 1 );

    if ( string1 != NULL )
    {
	(void) strcat( string1, string2 );
    }

    return( string1 );
}

/*+
************************************************************************
*
*   Function:	genstrcasecmp
*
*   Purpose:
*	Compare two strings ignoring the case.
*
*   Values Returned:
*	int	0		: The strings are the same.
*	int	> 0		: str1 > str2.
*	int	< 0		: str1 < str2.
*
************************************************************************
-*/

int		genstrcasecmp
(
    const char	*pString1,	/* (in)  The first string to compare.	*/
    const char	*pString2	/* (in)  The second string to compare.	*/
)
{
        int             i;
        char    pCopy1 [256];
        char    pCopy2 [256];

        strncpy (pCopy1, pString1, 256);
        strncpy (pCopy2, pString2, 256);

        for (i = 0; i < strlen (pCopy1); i++) pCopy1 [i] &= ~0x20;
        for (i = 0; i < strlen (pCopy2); i++) pCopy2 [i] &= ~0x20;

        return (strcmp (pCopy1, pCopy2));
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strext( pathname )
*
*   Purpose:
*	Takes in the pathname and returns a pointer to the suffix or 
*	extension of the filename.  The suffix is preceded by the last
*	dot in the pathname.  If there is no suffix, a NULL pointer is
*	returned.
*
*   Parameters:
*	char	*pathname	: (in) The pathname.
*
*   Values Returned:
*	char	*end		: The suffix of the path.
*	char	NULL		: No suffix was found.
*
************************************************************************
-*/

char	*strext(const char *pathname)
{
    char	*ext;		/* the suffix of the pathname.		*/


    /*
     *  Find the last dot in the pathname and return a pointer to its
     *  position (strrchr returns NULL if not found).
     */

    ext = strrchr( pathname, '.' );
    if ( ext != NULL )
    {
	++ext;
    }

    return( ext );
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strfit( string )
*
*   Purpose:
*	Removes the leading and trailing space characters from the
*	given string.
*
*   Parameters:
*	char	*string		: (mod) The string to fit.
*
*   Values Returned:
*	char	*string		: Pointer to the modified string.
*
************************************************************************
-*/

char	*strfit(char *string)
{
    char	*s;		/* temporary pointer.			*/
    char	*t;		/* temporary pointer.			*/


    /*
     *  Remove trailing spaces.
     */

    s = &(string[strlen(string)-1]);
    if ( isspace( *s ) )
    {
	while ( --s >= string && isspace( *s ) )
	{
	    /* empty */;
	}
	*++s = '\0';
    }


    /*
     *  Remove leading spaces.
     */

    s = string;
    if ( isspace( *s ) )
    {
	while ( isspace( *++s ) )
	{
	    /* empty */;
	}

	t = string;
	while ( *s != '\0' )
	{
	    *t++ = *s++;
	}
	*t = '\0';
    }

    return( string );
}

/*+
************************************************************************
*
*   Synopsis:
*       boolean	strfloat( s )
*
*   Purpose:
*	This routine verifies that string 's' is an ASCII representation
*	of an a floating point number.  For now, any string consisting of
*	digits, with at most 1 decimal point, is considered a float number.
*	Note that this will not detect scientific notation floats!
*
*   Parameters:
*	char	*s		: (in)  String to verify.
*
*   Values Returned:
*	boolean	TRUE		: 's' is an ASCII digit string.
*	boolean FALSE		: 's' does not represent a digit
*
************************************************************************
-*/

boolean strfloat(const char *s)
{
    char	*endptr;
    int		len;
    

    len = strlen( s );

    (void) strtod( s, &endptr );

    if ( endptr - s == len && len > 0 && 
	    ( *s == '-' || *s == '+' || *s == '.' || isdigit( *s ) ) )
    {
	return( TRUE );
    }
    else
    {
	return( FALSE );
    }
}    

/*+
************************************************************************
*
*   Synopsis:
*	void	strhead( pathname )
*
*   Purpose:
*	Takes in the pathname and returns the path of the directory in
*	which the file resides.  If no directory is specified in the
*	pathname, leave pathname unchanged.
*
*   Parameters:
*	char	*pathname	: (mod) The pathname of the file.
*
************************************************************************
-*/

void	strhead(char *pathname)
{
    char	*temp;		/* temporary pointer.			*/


    /*
     *  Find the last `/' in the pathname.  If no `/' is found, leave
     *  pathname unchanged.  Otherwise, move the end-of-string character
     *  to the position of the `/'.
     */

    temp = strrchr( pathname, '/' );
    if ( temp != NULL )
    {
	*temp = '\0';
    }
}

/*+
************************************************************************
*
*   Synopsis:
*       boolean	strint( s )
*
*   Purpose:
*	This routine verifies that string 's' is an ASCII representation
*	of an integer digit.
*
*   Parameters:
*	char	*s		: (in)  String to verify.
*
*   Values Returned:
*	boolean	TRUE		: 's' is an ASCII digit string.
*	boolean FALSE		: 's' does not represent a digit
*
************************************************************************
-*/

boolean	strint(const char *s)
{
    boolean	digit_found;

    if ( *s == '-' || *s == '+' )
    {
        ++s;
    }
    
    digit_found = FALSE;
    for( ; *s; ++s )
    {
        if ( !isdigit( *s ) )
	{
	    return( FALSE );
	}
	else
	{
	    digit_found = TRUE;
	}
    }
    
    return( digit_found );
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strnapp( s1, s2, l )
*
*   Purpose:
*	Appends s2 to s1, for at most l characters.  Both s1 and s2
*	must be null terminated strings.  The returned string is also
*	always null terminated.
*
*   Parameters:
*	char	*s1		: (in)	String to append to.
*	char	*s2		: (in)  String to append.
*	int	*l		: (in)  Maximum length to copy.
*
*   Values Returned:
*	char	NULL		: Memory allocation error.
*	char	*copy		: Pointer to copied string.
*
************************************************************************
-*/

char	*strnapp(char *s1, const char *s2, int l)
{
    char	*copy;
    int		length;
    
    length = strlen( s1 ) + l;
    if ( ( copy = gen_realloc( s1, length + 1 ) ) != NULL )
    {
        (void) strncat( copy, s2, l );
	*(copy + length) = '\0';
    }
    
    return( copy );
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strnsav( s, l )
*
*   Purpose:
*	Makes a copy of string 's' by allocating new memory for it then
*	doing a strncpy.  s should be a null terminated string, and 
*	a null terminated string is alwayrs returned.
*
*   Parameters:
*	char	*s		: (in)  String to copy.
*	int	*l		: (in)  Maximum length to copy.
*
*   Values Returned:
*	char	NULL		: Memory allocation error.
*	char	*copy		: Pointer to copied string.
*
************************************************************************
-*/

char	*strnsav(const char *s, int l)
{
    char	*copy;
    
    if ( ( copy = gen_alloc( l + 1 ) ) != NULL )
    {
        (void) strncpy( copy, s, l );
	*( copy + l ) = '\0';
    }
    
    return( copy );
}

/*+
************************************************************************
*
*   Synopsis:
*	boolean	strpattern( source, pattern )
*
*   Purpose:
*	Check if a string matches a pattern.
*
*   Parameters:
*	char	*source		: (in)  The string.
*	char	*pattern	: (in)  The pattern to match.
*
*   Values Returned:
*	boolean	TRUE		: The source string matches the pattern.
*	boolean	FALSE		: The source string does not match.
*
************************************************************************
-*/

boolean	strpattern(const char *source, const char *pattern)
{
    char	pattmatch(const char *source, const char *pattern);

    if ( pattmatch( source, pattern ) == 0 )
    {
	return( TRUE );
    }

    return( FALSE );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	strroot( pathname )
*
*   Purpose:
*	Takes in the pathname and removes the suffix from the filename
*	(the suffix is the extension).  The suffix begins with the last
*	dot in the pathname.  If there is no suffix in the path, strroot
*	has no effect.
*
*   Parameters:
*	char	*pathname	: (mod) The pathname of the file.
*
************************************************************************
-*/

void	strroot(char *pathname)
{
    char	*temp;


    /*
     *  Find the last `.' in the pathname.  If no `.' is found, leave
     *  pathname unchanged.  Otherwise, move the end-of-string character
     *  to the position of the `.'.
     */

    temp = strrchr( pathname, '.' );
    if ( temp != NULL )
    {
	*temp = '\0';
    }
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strsav( s )
*
*   Purpose:
*	Makes a copy of string 's' by allocating new memory for it then
*	doing a strcpy.
*
*   Parameters:
*	char	*s		: (in)  String to copy.
*
*   Values Returned:
*	char	NULL		: Memory allocation error.
*	char	*copy		: Pointer to copied string.
*
************************************************************************
-*/

char	*strsav(const char *s)
{
    char	*copy;
    int		length;
    
    length = strlen( s );
    if ( ( copy = gen_alloc( (length+1) * sizeof( char ) ) ) != NULL )
    {
        (void) strcpy( copy, s );
    }
    
    return( copy );
}

/*+
************************************************************************
*
*   Synopsis:
*	char	*strtail( pathname )
*
*   Purpose:
*	Takes in the pathname and returns the basename of the path.  The
*	basename begins after the last `/' in the pathname.  If the
*	pathname has `/', the whole string is returned.
*
*   Parameters:
*	char	*pathname	: (in) The pathname of the file.
*
*   Values Returned:
*	char	*tail		: The basename of the pathname.
*
************************************************************************
-*/

char	*strtail(const char *pathname)
{
    const char	*tail;		/* the basename of the pathname.	*/


    /*
     *  Find the last `/' in the pathname and return a pointer to its 
     *  position (strrchr returns a NULL pointer if no `/' is found).
     */

    tail = strrchr( pathname, '/' );
    if ( tail == NULL )
    {
	tail = pathname;
    }
    else
    {
	++tail;
    }

    return( (char *) tail );
}

/*+
************************************************************************
*
*   Synopsis:
*	void	strtokens( buffer, max_tokens, tokens, num_tokens )
*
*   Purpose:
*	Takes an input string and breaks it up into tokens.  Tokens are
*	identified by being separated by white space.
*
*	N.B.  This function modifies the input string!
*
*   Parameters:
*	char	*buffer		: (mod)	Input string.
*	int	max_tokens	: (in)	The maximum number of tokens.
*	char	**tokens	: (out) Array of pointers to tokens.
*	int	*num_tokens	: (out) Number of tokens found.
*
*   Values Returned:
*	None.
*
************************************************************************
-*/

void	strtokens(char *buffer, int max_tokens, char **tokens, int *num_tokens)
{
    char	*c;
    int		count;
    boolean	in_string;
    boolean	in_quotes;


    in_string = FALSE;
    in_quotes = FALSE;
    count = 0;

    for ( c = buffer; *c; c++ )
    {
	if ( in_string )
	{
	    if ( ( ! in_quotes && isspace( *c ) ) ||
		 ( in_quotes && *c == '"' ) )
	    {
		*c = '\0';
		in_string = FALSE;
		in_quotes = FALSE;
		if ( count == max_tokens )
		{
		    break;
		}
	    }
	}
	else
	{
	    if ( *c == '"' )
	    {
		in_quotes = TRUE;
		in_string = TRUE;
		tokens[count] = c + 1;
		count++;
	    }
	    else if ( ! isspace( *c ) )
	    {
		in_string = TRUE;
		tokens[count] = c;
		count++;
	    } 
	}
    }

    *num_tokens = count;
}
