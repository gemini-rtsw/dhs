/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1997.				(c) 1997.
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
*   Module Name:	gen/ersSprintf.c
*
*   Purpose:
*	Contains the safe sprintf routines borrowed from the AAO
*	DRAMA ERS library.
*
*   Routines:
*	int	ssprintf	: Safe version of sprintf.
*	int	svsprintf	: Safe version of vsprintf.
*
*   Date		: Oct 20, 1997
*
*   SCCS data		: @(#)
*	Module Name	: ersSprintf.c
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 12/14/99
*
*   Programmer		: Norm Hill
*
*   Modification History:
*	Dec 4, 1997	pdd : #define'd DFLOAT_OK and included values for
*			      ERS_MAXEXP and ERS_MAXFRACT (taken from Ers.h)
*	Dec 13, 1999	nrh : Changed to only compile under hpux. Should be
*			      removed altogether when hpux supports snprintf
*			      and vsnprintf.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <gen_types.h>
#include <gen_str.h>
#ifdef NO_SNPRINTF


/*
 *  These defines rename the functions, and set up the environment
 *  normally set in the ers.h file.
 */

#define	ErsSPrintf	ssprintf
#define	ErsVSPrintf	svsprintf
#define	DPUBLIC		extern
#define	DCONSTV		const
#define	DCONSTR		const
#define	DPRIVATE	static
#define	DSTDARG_OK
#define DFLOAT_OK


/*
 *  Floating point stuff.  Only used in ErsVSPrintf.
 */
#ifdef DFLOAT_OK
/*
 *  These values taken from bsd floatio.h
 */
#    define ERS_MAXEXP 308
#    define ERS_MAXFRACT 39
#endif


/*
 *+			E r s S P r i n t f

 *  Function name:
      ErsSPrintf

 *  Function:
	A safe version of the C RTL sprintf function.


 *  Description:
	The standard C RTL version of sprintf is unsafe as nothing
	limits the length of the output string.  It is easy to overwrite
	the stack.  By providing a length argument string argument, this 
	routine implements a safe version of sprintf.

	See ErsVSPrintf() for more details.	

 *  Language:
      C

 *  Call:
	(int) = ErsSPrintf(length, string, format, args...)

 *  Parameters:   (">" input, "!" modified, "W" workspace, "<" output)
	(>) length  (int) The length of string.
	(<) string  (char *) The pointer to the output string	
	(>) format  (char *) A format specification
	(>) arg...  (anything) argument list 


 *  Function value:
	EOF indicates the format string exceeds 
	the length available, otherwise, the number of characters output.

 *  Include files: Ers.h, stdio.h


 *  External functions used:
	    ErsVSPrintf  (Ers) A save version of vsprintf.

 *  External values used: 
	    none

 *  Prior requirements:
	    none 

 *  Support: Tony Farrell, AAO

 *-

 *  Copyright (c) Anglo-Australian Telescope Board, 1995.
    Not to be used for commercial purposes without AATB permission.
  
 *  History:
      23-Nov-1992 - TJF - Original version,
      29-Sep-1993 - TJF - Add Sccs id
      {@change entry@}
 
 
 *  Sccs Id:  erssprintf.c, Release 1.7, 6/30/95
 
 */
#define ERS_MAIN			/* Indication to Ers.h		*/
#ifndef DNOCONST
#    define DNOCONST 1
#endif 
/* #include "Ers.h" */
#ifdef DSTDARG_OK	    
    DPUBLIC int ErsSPrintf (
			DCONSTV int maxLength, 
			char *string, 
			DCONSTV char * DCONSTR fmt,...)
#else
    DPUBLIC int ErsSPrintf (va_alist)
    va_dcl
#endif
{
    va_list args;

#   ifdef DSTDARG_OK
	va_start(args,fmt);
#   else
	int maxLength;
	char *string;
	char *fmt;
        va_start(args);
        maxLength = va_arg(args, int);
        string = va_arg(args, char *);
	fmt = va_arg(args, char *);
#   endif
    
    return(ErsVSPrintf(maxLength,string,fmt,args));

}

/*
 *  The following function was copied verbatim from the AAO DRAMA ERS 
 *  library.  A macro is used to change the name from ErsVSPrintf
 *  to svsprintf.
 */

/*
 *+			E r s V S P r i n t f

 *  Function name:
      ErsVSPrintf

 *  Function:
	A safe version of the C RTL vsprintf function.


 *  Description:
	The standard C RTL version of vsprintf is unsafe as nothing
	limits the length of the output string.  It is easy to overwrite
	the stack.  By providing a length argument string argument, this 
	routine implements a safe version of vsprintf.
	
	When not under VxWorks, this module is based upon the Berkeley Unix 
	vprintf.c module.  (based on version 5.47, 22-Mar-1991).  The header 
	for this file and the appropaite copyright appears below. 

	Under VxWorks, the fioFormatV routine is used.

 *  Language:
      C

 *  Call:
	(int) = ErsVSPrintf(length, string, format, arg)

 *  Parameters:   (">" input, "!" modified, "W" workspace, "<" output)
	(>) length  (int) The length of string.
	(<) string  (char *) The pointer to the output string	
	(>) format  (char *) A format specification
	(>) arg	    (va_list) Variable argument list 


 *  Function value:
	EOF indicates the format string exceeds the length available,
	otherwise, the number of characters output.

 *  Include files: Ers.h, stdio.h


 *  External functions used:
	    fioFormatV (VxWorks) Do a C style format.

 *  External values used: 
	    none

 *  Prior requirements:
	    none 

 *  Support: Tony Farrell, AAO

 *-

 *  Copyright (c) Anglo-Australian Telescope Board, 1995.
    Not to be used for commercial purposes without AATB permission.
  
 *  History:
      23-Nov-1992 - TJF - Original version, based on Berkeley Unix
		    vprintf.c, version 5.47, 22-Mar-1991.
      27-Nov-1992 - TJF - Under VxWorks, use fioFormatV.
      29-Sep-1993 - TJF - Add Sccs id
      29-Apr-1994 - TJF - Solaris 2 does not have isinf, so use !finite
      21-Oct-1994 - TJF - Osf does not have isinf either - it appears to have
				been droped from ieee.  We use finite instead.
      08-Dec-1995 - TJF - Under solaris 2 we need to include ieeefp.h to
			get the definition of finite.  In addition, it appears
			isnan has been replaced by isnand, so we define a maro,
      18-Oct-1996 - TJF - VxWorks version was not correctly returning
			the result of the format.
      {@change entry@}
 
 
 *  Sccs Id:  ersvsprintf.c, Release 1.14, 2/17/97
 
 */


/*
 *  Original Berkeley copyright notice.
 *
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#define ERS_MAIN			/* Indication to Ers.h		*/
#ifndef DNOCONST
#   define DNOCONST 1
#endif

/* 
#include "Ers.h" 
#ifdef DSTDARG_OK
#include <stdarg.h>
#else
#include <varargs.h>
#endif
*/

#ifndef VxWorks
/*
 *
 * Berkley unix based version, for all systems except VxWorks.
 *
 * This code is large and complicated...
 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#ifdef VAX
#   include <strmac.h>
#elif defined(VxWorks)
#   include <vxWorks.h>
#elif defined(solaris2)
/* Under solaris2, we need ieeefp.h and must define isnan to be isnand*/
#   include <ieeefp.h>
#   define isnan isnand
#endif
#ifdef DFUDGE_PROTOTYPES_C
   DPUBLIC char *memchr(DCONSTV char * DCONSTR s1, 
		        DCONSTV int c,
		        DCONSTV size_t size);
#endif

/* end of configuration stuff */

/*
 *  Copy a string to the output string. If the output string exceeds the
 *  length required, return false, otherwise true.
 */

#ifdef DPROTOTYPES_OK
    DPRIVATE int Ers___SPrintf (
		DCONSTV int outLength,/* Max number allowed in string */
		DCONSTV char * DCONSTR ptr, /* Point to string to print	    */
		DCONSTV int inLength,/* Length of string to print    */
		char *string,		    /* Output string		    */
		int *outNext)		    /* Current length of string	    */
#else
    DPRIVATE int Ers___SPrintf (outLength, ptr, inLength, string, outNext)
		       DCONSTV int outLength;
		       DCONSTV char * DCONSTR ptr;
		       DCONSTV int inLength;
		       char *string;
		       int *outNext;
#endif
{
    register int i=0;
/*
 *  We use outLength-1 to ensure we leave space for the null terminator.
 */
    while ((*outNext < outLength-1)&&(i < inLength))
	string[(*outNext)++] = ptr[i++];

    string[*outNext] = '\0';	/* Null terminate	*/
/*
 *  If we managed to output everything, return 1, else 0.
 */
    if (i == inLength)
	return(1);
    else
	return(0);
}



#ifdef DFLOAT_OK

#define	BUF		(ERS_MAXEXP+ERS_MAXFRACT+1)	/* + decimal point */
#define	DEFPREC		6

#ifdef DPROTOTYPES_OK
    DPRIVATE int Ers___Cvt(
	double number,
	register int prec,
	int flags,
	char *signp,
	int fmtch,
	char *startp,
	char *endp);
#else
    DPRIVATE int Ers___Cvt();
#endif

#else

#define	BUF		40

#endif


/*
 * Macros for converting digits to letters and vice versa
 */
#define	to_digit(c)	((c) - '0')
#define is_digit(c)	((unsigned)to_digit(c) <= 9)
#define	to_char(n)	((n) + '0')

/*
 * Flags used during conversion.
 */
#define	LONGINT		0x01		/* long integer */
#define	LONGDBL		0x02		/* long double; unimplemented */
#define	SHORTINT	0x04		/* short integer */
#define	ALT		0x08		/* alternate form */
#define	LADJUST		0x10		/* left adjustment */
#define	ZEROPAD		0x20		/* zero (as opposed to blank) pad */
#define	HEXPREFIX	0x40		/* add 0x or 0X prefix */

#ifdef DPROTOTYPES_OK
    extern int ErsVSPrintf(
		 DCONSTV int maxLength, 
		 char *string , 
		 DCONSTV char * DCONSTR fmt0, 
		 va_list ap) 
#else
    extern int ErsVSPrintf(maxLength, string , fmt0, ap) 
		 DCONSTV int maxLength;
		 char *string;
		 DCONSTV char * DCONSTR fmt0;
		 va_list ap;
#endif
{
	register char *fmt;	/* format string */
	register int ch;	/* character from fmt */
	register int n;		/* handy integer (short term usage) */
	register char *cp;/* handy char pointer (short term usage) */
	register int flags;	/* flags as above */
	int ret;		/* return value accumulator */
	int width;		/* width from format (%8d), or 0 */
	int prec;		/* precision from format (%.3d), or -1 */
	char sign;		/* sign prefix (' ', '+', '-', or \0) */
#ifdef DFLOAT_OK
	char softsign;		/* temporary negative sign for floats */
	double _double;		/* double precision arguments %[eEfgG] */
	int fpprec;		/* `extra' floating precision in [eEfgG] */
#endif
	unsigned long _ulong ;	/* integer arguments %[diouxX] */
	enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
	int dprec;		/* a copy of prec if [diouxX], 0 otherwise */
	int fieldsz;		/* field size expanded by sign, etc */
	int realsz;		/* field size expanded by dprec */
	int size;		/* size of converted field or string */
	char *xdigs="";		/* digits for [xX] conversion */
	char buf[BUF];		/* space for %c, %[diouxX], %[eEfgG] */
	char ox[2];		/* space for 0x hex-prefix */

	int current;		/* Pointer to current length */

	/*
	 * Choose PADSIZE to trade efficiency vs size.  If larger
	 * printf fields occur frequently, increase PADSIZE (and make
	 * the initialisers below longer).
	 */
#define	PADSIZE	16		/* pad chunk size */
	static char blanks[PADSIZE] =
	 {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
	static char zeroes[PADSIZE] =
	 {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};

	/*
	 * BEWARE, these `goto error' on error, and PAD uses `n'.
	 */
#define	PRINT(ptr, len) { \
	if (!Ers___SPrintf(maxLength,(ptr),(len),(string),&current)) \
	    return(EOF);\
}
#define	PAD(howmany, with) { \
	if ((n = (howmany)) > 0) { \
		while (n > PADSIZE) { \
			PRINT(with, PADSIZE); \
			n -= PADSIZE; \
		} \
		PRINT(with, n); \
	} \
}
#define	FLUSH()

	/*
	 * To extend shorts properly, we need both signed and unsigned
	 * argument extraction methods.
	 */
#define	SARG() \
	(flags&LONGINT ? va_arg(ap, long) : \
	    flags&SHORTINT ? (long)(short)va_arg(ap, int) : \
	    (long)va_arg(ap, int))
#define	UARG() \
	(flags&LONGINT ? va_arg(ap, unsigned long) : \
	    flags&SHORTINT ? (unsigned long)(unsigned short)va_arg(ap, int) : \
	    (unsigned long)va_arg(ap, unsigned int))

	if ((maxLength < 0)||(string == 0)) 
	    return(EOF);

	fmt = fmt0;
	ret = 0;
	current = 0;

	/*
	 * Scan the format for conversions (`%' character).
	 */
	for (;;) {
		for (cp = fmt; (ch = *fmt) != '\0' && ch != '%'; fmt++)
			/* void */;
		if ((n = fmt - cp) != 0) {
			PRINT(cp, n);
			ret += n;
		}
		if (ch == '\0')
			goto done;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
#ifdef DFLOAT_OK
		fpprec = 0;
#endif
		width = 0;
		prec = -1;
		sign = '\0';

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
		case ' ':
			/*
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign)
				sign = ' ';
			goto rflag;
		case '#':
			flags |= ALT;
			goto rflag;
		case '*':
			/*
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			if ((width = va_arg(ap, int)) >= 0)
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if ((ch = *fmt++) == '*') {
				n = va_arg(ap, int);
				prec = n < 0 ? -1 : n;
				goto rflag;
			}
			n = 0;
			while (is_digit(ch)) {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			}
			prec = n < 0 ? -1 : n;
			goto reswitch;
		case '0':
			/*
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= ZEROPAD;
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit(ch);
				ch = *fmt++;
			} while (is_digit(ch));
			width = n;
			goto reswitch;
#ifdef DFLOAT_OK
		case 'L':
			flags |= LONGDBL;
			goto rflag;
#endif
		case 'h':
			flags |= SHORTINT;
			goto rflag;
		case 'l':
			flags |= LONGINT;
			goto rflag;
		case 'c':
			*(cp = buf) = va_arg(ap, int);
			size = 1;
			sign = '\0';
			break;
		case 'D':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_ulong = SARG();
			if ((long)_ulong < 0) {
				_ulong = -_ulong;
				sign = '-';
			}
			base = DEC;
			goto number;
#ifdef DFLOAT_OK
		case 'e':
		case 'E':
		case 'f':
		case 'g':
		case 'G':
			_double = va_arg(ap, double);

#ifndef VMS
			/* do this before tricky precision changes */
			/* (this checks for infinity and not a number) */
			/* (not necessary on VAX/VMS)		*/
			if (!finite(_double)) {
				if (_double < 0)
					sign = '-';
				cp = "Inf";
				size = 3;
				break;
			}
			if (isnan(_double)) {
				cp = "NaN";
				size = 3;
				break;
			}
#endif
			/*
			 * don't do unrealistic precision; just pad it with
			 * zeroes later, so buffer size stays rational.
			 */
			if (prec > ERS_MAXFRACT) {
				if ((ch != 'g' && ch != 'G') || (flags&ALT))
					fpprec = prec - ERS_MAXFRACT;
				prec = ERS_MAXFRACT;
			} else if (prec == -1)
				prec = DEFPREC;
			/*
			 * cvt may have to round up before the "start" of
			 * its buffer, i.e. ``intf("%.2f", (double)9.999);'';
			 * if the first character is still NUL, it did.
			 * softsign avoids negative 0 if _double < 0 but
			 * no significant digits will be shown.
			 */
			cp = buf;
			*cp = '\0';
			size = Ers___Cvt(_double, prec, flags, &softsign, ch,
			    cp, buf + sizeof(buf));
			if (softsign)
				sign = '-';
			if (*cp == '\0')
				cp++;
			break;
#endif
		case 'n':
			if (flags & LONGINT)
				*va_arg(ap, long *) = ret;
			else if (flags & SHORTINT)
				*va_arg(ap, short *) = ret;
			else
				*va_arg(ap, int *) = ret;
			continue;	/* no output */
		case 'O':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_ulong = UARG();
			base = OCT;
			goto nosign;
		case 'p':
			/*
			 * ``The argument shall be a pointer to void.  The
			 * value of the pointer is converted to a sequence
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_ulong = (unsigned long)va_arg(ap, void *);
			base = HEX;
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
			ch = 'x';
			goto nosign;
		case 's':
			if ((cp = va_arg(ap, char *)) == NULL)
				cp = "(null)";
			if (prec >= 0) {
				/*
				 * can't use strlen; can only look for the
				 * NUL in the first `prec' characters, and
				 * strlen() will go further.
				 */
				char *p = memchr(cp, 0, prec);

				if (p != NULL) {
					size = p - cp;
					if (size > prec)
						size = prec;
				} else
					size = prec;
			} else
				size = strlen(cp);
			sign = '\0';
			break;
		case 'U':
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_ulong = UARG();
			base = DEC;
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_ulong = UARG();
			base = HEX;
			/* leading 0x/X only if non-zero */
			if (flags & ALT && _ulong != 0)
				flags |= HEXPREFIX;

			/* unsigned conversions */
nosign:			sign = '\0';
			/*
			 * ``... diouXx conversions ... if a precision is
			 * specified, the 0 flag will be ignored.''
			 *	-- ANSI X3J11
			 */
number:			if ((dprec = prec) >= 0)
				flags &= ~ZEROPAD;

			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
			if (_ulong != 0 || prec != 0) {
				/*
				 * unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
				case OCT:
					do {
						*--cp = to_char(_ulong & 7);
						_ulong >>= 3;
					} while (_ulong);
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
						*--cp = '0';
					break;

				case DEC:
					/* many numbers are 1 digit */
					while (_ulong >= 10) {
						*--cp = to_char(_ulong % 10);
						_ulong /= 10;
					}
					*--cp = to_char(_ulong);
					break;

				case HEX:
					do {
						*--cp = xdigs[_ulong & 15];
						_ulong >>= 4;
					} while (_ulong);
					break;

				default:
					cp = "bug in vfprintf: bad base";
					size = strlen(cp);
					goto skipsize;
				}
			}
			size = buf + BUF - cp;
		skipsize:
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
			size = 1;
			sign = '\0';
			break;
		}

		/*
		 * All reasonable formats wind up here.  At this point,
		 * `cp' points to a string which (if not flags&LADJUST)
		 * should be padded out to `width' places.  If
		 * flags&ZEROPAD, it should first be prefixed by any
		 * sign or other prefix; otherwise, it should be blank
		 * padded before the prefix is emitted.  After any
		 * left-hand padding and prefixing, emit zeroes
		 * required by a decimal [diouxX] precision, then print
		 * the string proper, then emit zeroes required by any
		 * leftover floating precision; finally, if LADJUST,
		 * pad with blanks.
		 */

		/*
		 * compute actual size, so we know how much to pad.
		 * fieldsz excludes decimal prec; realsz includes it
		 */
#ifdef DFLOAT_OK
		fieldsz = size + fpprec;
#else
		fieldsz = size;
#endif
		if (sign)
			fieldsz++;
		else if (flags & HEXPREFIX)
			fieldsz += 2;
		realsz = dprec > fieldsz ? dprec : fieldsz;

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD(width - realsz, blanks);

		/* prefix */
		if (sign) {
			PRINT(&sign, 1);
		} else if (flags & HEXPREFIX) {
			ox[0] = '0';
			ox[1] = ch;
			PRINT(ox, 2);
		}

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD(width - realsz, zeroes);

		/* leading zeroes from decimal precision */
		PAD(dprec - fieldsz, zeroes);

		/* the string or number proper */
		PRINT(cp, size);

#ifdef DFLOAT_OK
		/* trailing f.p. zeroes */
		PAD(fpprec, zeroes);
#endif

		/* left-adjusting padding (always blank) */
		if (flags & LADJUST)
			PAD(width - realsz, blanks);

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;

		FLUSH();	/* copy out the I/O vectors */
	}
done:
	FLUSH();
	return (ret);
	/* NOTREACHED */
}

#ifdef DFLOAT_OK
#include <math.h>

#ifdef DPROTOTYPES_OK
    DPRIVATE char * Ers___Exponent(register char *p,register int exp,int fmtch);
    DPRIVATE char * Ers___Round(
	double fract,
	int *exp,
	register char *start, 
	register char *end,
	char ch, 
	char *signp);
#else
    DPRIVATE char * Ers___Exponent();
    DPRIVATE char * Ers___Round();
#endif

#ifdef DPROTOTYPES_OK
    DPRIVATE int Ers___Cvt(
	double number,
	register int prec,
	int flags,
	char *signp,
	int fmtch,
	char *startp,
	char *endp)
#else
    DPRIVATE int Ers___Cvt(number, prec, flags, signp, fmtch, startp, endp)
	double number;
	register int prec;
	int flags;
	char *signp;
	int fmtch;
	char *startp, *endp;
#endif
{
	register char *p, *t;
	register double fract;
	int dotrim, expcnt, gformat;
	double integer, tmp;

	dotrim = expcnt = gformat = 0;
	if (number < 0) {
		number = -number;
		*signp = '-';
	} else
		*signp = 0;

	fract = modf(number, &integer);

	/* get an extra slot for rounding. */
	t = ++startp;

	/*
	 * get integer portion of number; put into the end of the buffer; the
	 * .01 is added for modf(356.0 / 10, &integer) returning .59999999...
	 */
	for (p = endp - 1; integer; ++expcnt) {
		tmp = modf(integer / 10, &integer);
		*p-- = to_char((int)((tmp + .01) * 10));
	}
	switch (fmtch) {
	case 'f':
		/* reverse integer into beginning of buffer */
		if (expcnt)
			for (; ++p < endp; *t++ = *p);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.
		 */
		if (prec || flags&ALT)
			*t++ = '.';
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec)
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while (--prec && fract);
			if (fract)
				startp = Ers___Round(fract, (int *)NULL, startp,
				    t - 1, (char)0, signp);
		}
		for (; prec--; *t++ = '0');
		break;
	case 'e':
	case 'E':
eformat:	if (expcnt) {
			*t++ = *++p;
			if (prec || flags&ALT)
				*t++ = '.';
			/* if requires more precision and some integer left */
			for (; prec && ++p < endp; --prec)
				*t++ = *p;
			/*
			 * if done precision and more of the integer component,
			 * round using it; adjust fract so we don't re-round
			 * later.
			 */
			if (!prec && ++p < endp) {
				fract = 0;
				startp = Ers___Round((double)0, &expcnt, startp,
				    t - 1, *p, signp);
			}
			/* adjust expcnt for digit in front of decimal */
			--expcnt;
		}
		/* until first fractional digit, decrement exponent */
		else if (fract) {
			/* adjust expcnt for digit in front of decimal */
			for (expcnt = -1;; --expcnt) {
				fract = modf(fract * 10, &tmp);
				if (tmp)
					break;
			}
			*t++ = to_char((int)tmp);
			if (prec || flags&ALT)
				*t++ = '.';
		}
		else {
			*t++ = '0';
			if (prec || flags&ALT)
				*t++ = '.';
		}
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec)
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while (--prec && fract);
			if (fract)
				startp = Ers___Round(fract, &expcnt, startp,
				    t - 1, (char)0, signp);
		}
		/* if requires more precision */
		for (; prec--; *t++ = '0');

		/* unless alternate flag, trim any g/G format trailing 0's */
		if (gformat && !(flags&ALT)) {
			while (t > startp && *--t == '0');
			if (*t == '.')
				--t;
			++t;
		}
		t = Ers___Exponent(t, expcnt, fmtch);
		break;
	case 'g':
	case 'G':
		/* a precision of 0 is treated as a precision of 1. */
		if (!prec)
			++prec;
		/*
		 * ``The style used depends on the value converted; style e
		 * will be used only if the exponent resulting from the
		 * conversion is less than -4 or greater than the precision.''
		 *	-- ANSI X3J11
		 */
		if (expcnt > prec || (!expcnt && fract && fract < .0001)) {
			/*
			 * g/G format counts "significant digits, not digits of
			 * precision; for the e/E format, this just causes an
			 * off-by-one problem, i.e. g/G considers the digit
			 * before the decimal point significant and e/E doesn't
			 * count it as precision.
			 */
			--prec;
			fmtch -= 2;		/* G->E, g->e */
			gformat = 1;
			goto eformat;
		}
		/*
		 * reverse integer into beginning of buffer,
		 * note, decrement precision
		 */
		if (expcnt)
			for (; ++p < endp; *t++ = *p, --prec);
		else
			*t++ = '0';
		/*
		 * if precision required or alternate flag set, add in a
		 * decimal point.  If no digits yet, add in leading 0.
		 */
		if (prec || flags&ALT) {
			dotrim = 1;
			*t++ = '.';
		}
		else
			dotrim = 0;
		/* if requires more precision and some fraction left */
		if (fract) {
			if (prec) {
				do {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				} while(!tmp);
				while (--prec && fract) {
					fract = modf(fract * 10, &tmp);
					*t++ = to_char((int)tmp);
				}
			}
			if (fract)
				startp = Ers___Round(fract, (int *)NULL, startp,
				    t - 1, (char)0, signp);
		}
		/* alternate format, adds 0's for precision, else trim 0's */
		if (flags&ALT)
			for (; prec--; *t++ = '0');
		else if (dotrim) {
			while (t > startp && *--t == '0');
			if (*t != '.')
				++t;
		}
	}
	return (t - startp);
}

#ifdef DPROTOTYPES_OK
    DPRIVATE char * Ers___Round(
	double fract,
	int *exp,
	register char *start, 
	register char *end,
	char ch, 
	char *signp)
#else
    DPRIVATE char * Ers___Round(fract, exp, start, end, ch, signp)
	double fract;
	int *exp;
	register char *start, *end;
	char ch, *signp;
#endif
{
	double tmp;

	if (fract)
		(void)modf(fract * 10, &tmp);
	else
		tmp = to_digit(ch);
	if (tmp > 4)
		for (;; --end) {
			if (*end == '.')
				--end;
			if (++*end <= '9')
				break;
			*end = '0';
			if (end == start) {
				if (exp) {	/* e/E; increment exponent */
					*end = '1';
					++*exp;
				}
				else {		/* f; add extra digit */
				*--end = '1';
				--start;
				}
				break;
			}
		}
	/* ``"%.3f", (double)-0.0004'' gives you a negative 0. */
	else if (*signp == '-')
		for (;; --end) {
			if (*end == '.')
				--end;
			if (*end != '0')
				break;
			if (end == start)
				*signp = 0;
		}
	return (start);
}

#ifdef DPROTOTYPES_OK
    DPRIVATE char * Ers___Exponent(register char *p,register int exp,int fmtch)
#else
    DPRIVATE char * Ers___Exponent(p, exp, fmtch)
	register char *p;
	register int exp;
	int fmtch;
#endif
{
	register char *t;
	char expbuf[ERS_MAXEXP];

	*p++ = fmtch;
	if (exp < 0) {
		exp = -exp;
		*p++ = '-';
	}
	else
		*p++ = '+';
	t = expbuf + ERS_MAXEXP;
	if (exp > 9) {
		do {
			*--t = to_char(exp % 10);
		} while ((exp /= 10) > 9);
		*--t = to_char(exp);
		for (; t < expbuf + ERS_MAXEXP; *p++ = *t++);
	}
	else {
		*p++ = '0';
		*p++ = to_char(exp);
	}
	return (p);
}
#endif
/*
 *	End of Non-VxWorks format
 */
#else
/*
 *	Under VxWorks, we call the FioFormatV routine
 */
#include <vxWorks.h>
#include <fioLib.h>

typedef struct {
		char * string;
		int    Length;
		int    Next;
			} OutBufType;
/*
 *	Ers___Sprintf is called by fioFormatV to output the character.  
 */
#ifdef DPROTOTYPES_OK
    DPRIVATE int Ers___SPrintf (
		char *ptr,
		int  inLength,
		OutBufType *out)
#else
    DPRIVATE int Ers___SPrintf (ptr, inLength, out) 
		char *ptr;
		int  inLength;
		OutBufType *out;
#endif
{
    register int i=0;
/*
 *  We use outLength-1 to ensure we leave space for the null terminator.
 */
    while ((out->Next < out->Length-1)&&(i < inLength))
	out->string[(out->Next)++] = ptr[i++];

    out->string[out->Next] = '\0';	/* Null terminate	*/
/*
 *  If we managed to output everything, return 1, else 0.
 */
    if (i == inLength)
	return(OK);
    else
	return(ERROR);
	
}
#ifdef DPROTOTYPES_OK
    DPUBLIC int ErsVSPrintf(
		 DCONSTV int maxLength, 
		 char *string , 
		 DCONSTV char * DCONSTR fmt0, 
		 va_list ap) 
#else
    DPUBLIC int ErsVSPrintf(maxLength, string , fmt0, ap) 
		 DCONSTV int maxLength;
		 char *string;
		 DCONSTV char * DCONSTR fmt0;
		 va_list ap;
#endif

{
     register i;
     OutBufType out;
     out.string = string;
     out.Length = maxLength;
     out.Next   = 0;
     if ((i = fioFormatV(fmt0, ap, Ers___SPrintf, (int)&out)) == ERROR)
	return(EOF);
     else
	return(i);
    
}
#endif
#endif /* NO_SNPRINTF */
