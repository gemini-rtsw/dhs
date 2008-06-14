/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	gen/h/gen_types.h
*
*   Purpose:
*	General constants and macros.
*
*   Date		: Dec 10, 1990
*
*   SCCS data		: @(#)
*	Module Name	: gen_types.h
*	Version Number	: 1.16
*	Release Number	: 1
*	Last Updated	: 12/14/99
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	Dec 02, 1997 pdd	: added double precision macros
*	Oct 27, 1999 nrh	: Fixed the usage of EPSILON to scale to
*				  the value magnitude.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifndef _gen_types_h
#define _gen_types_h


/*
 *  Standard include files.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <math.h>


/*
 *  Data structures
 */

typedef	char		boolean;
typedef	unsigned char	byte;
typedef boolean		(*pfb)();
typedef int		(*pfi)();
typedef float		(*pff)();
typedef void		(*pfv)();
typedef void		*pointer;


/*
 *  Constant definitions
 */

#ifndef	FALSE
#define	FALSE		( (boolean) 0 )
#define TRUE		( (boolean) 1 )
#endif


/*
 *  To be POSIX compliant on SUNOS, one should use PATH_MAX rather
 *  than MAXPATHLEN found in sys/param.h.  This can be removed once
 *  SUNOS is no longer supported.
 */

#ifndef  PATH_MAX
#define	PATH_MAX	1024
#endif


/*
 *  Memory allocation macro definitions.
 */

#define gen_alloc(n)            ((pointer) malloc((unsigned)(n)))
#define gen_calloc(n,s)		((pointer) calloc((unsigned)(n),	\
					(unsigned)(s)))
#define gen_cmp(p,q,n)		(memcmp((pointer)(p),(pointer)(q),	\
					(int)(n)))
#define gen_cpy(p,q,n)		((pointer) memcpy((pointer) (p),	\
					(pointer) (q),(int) (n)))
#define	gen_free(p)		((void) free((pointer) (p)))
#define gen_realloc(p,n)        ((pointer) realloc((pointer)(p),	\
					(unsigned)(n)))
#define gen_zero(p,n)		((void) memset((pointer)(p),0,(int)(n)))

#define boolean_alloc(n)        ((boolean *) gen_alloc(((n)*sizeof(boolean))))
#define byte_alloc(n)		((byte *) gen_alloc(((n) * sizeof(byte))))
#define char_alloc(n)		((char *) (gen_alloc((n) * sizeof(char))))
#define double_alloc(n)		((double *) gen_alloc((n)*sizeof(double)))
#define float_alloc(n)		((float *) gen_alloc((n)*sizeof(float)))
#define int_alloc(n)		((int *) gen_alloc((n)*sizeof(int)))
#define ptr_alloc(n)		((pointer *) gen_alloc((n)*sizeof(pointer)))
#define short_alloc(n)		((short *) gen_alloc((n)*sizeof(short)))
#define long_alloc(n)		((long *) gen_alloc((n)*sizeof(long)))


/*
 *  General macro definitions.
 */

#ifdef	ABS
#undef	ABS
#endif
#define ABS(a)		((a) < 0 ? -(a) : (a))
#define	EVEN(x)		((0x01 & x) ? FALSE : TRUE)

#define INRANGE(x,lo,hi)	((x)>=(lo) && (x)<=(hi))
#define INBOX(x,y,l,r,b,t)	(INRANGE(x,l,r) && INRANGE(y,b,t))

#define	ROUND(x)	((x) + 0.5)
#define XOR(a,b)	(((a) || (b)) && !((a) && (b)))

#ifdef  MAX 
#undef  MAX
#endif
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))

#ifdef  MIN 
#undef  MIN
#endif
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))

#ifndef NULL
#define NULL 0
#endif



/*
 *  Double-specific macros
 */

#define DEPSILON(a)	0.000000000000001*fabs(a)/* num digits = DBL_DIG */
#define DEQ(a,b)	(ABS((a)-(b)) <= DEPSILON(a))/* Equal	*/
#define DLE(a,b)        (DLT(a,b) || DEQ(a,b))	/* Less than or equal	*/
#define DLT(a,b)        ((b)-(a) > DEPSILON(a))/* Less than		*/
#define DGE(a,b)        (DGT(a,b) || DEQ(a,b))	/* Greater than or equal*/
#define DGT(a,b)        ((a)-(b) > DEPSILON(a))/* Greater than		*/
#define DNE(a,b)        (!(DEQ(a,b)))		/* Not equal		*/


/*
 *  Float specific macros.
 */

#define EPSILON		0.000001*fabs(a)	/* num digits = FLT_DIG */
#define FEQ(a,b)        (ABS((a)-(b)) <= EPSILON(a))	/* Equal	*/
#define FLE(a,b)        (FLT(a,b) || FEQ(a,b))	/* Less than or equal	*/
#define FLT(a,b)        ((b)-(a) > EPSILON(a))	/* Less than		*/
#define FGE(a,b)        (FGT(a,b) || FEQ(a,b))	/* Greater than or equal*/
#define FGT(a,b)        ((a)-(b) > EPSILON(a))	/* Greater than		*/
#define FNE(a,b)        (!(FEQ(a,b)))		/* Not equal		*/

#define SIGN(x)		(((x) < (0)) ? (FEQ((x),0.0) ? 1 : (-1)) : 1)
#define SWAP(a,b)	{float swpv; swpv=(a);(a)=(b);(b)=swpv;}

#endif /* _gen_types_h */
