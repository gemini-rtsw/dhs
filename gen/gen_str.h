/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	gen/h/gen_str.h
*
*   Purpose:
*	Contains defines and declarations for string manipulation.
*
*   Date		: Dec 10, 1990
*
*   SCCS data		: @(#)
*	Module Name	: gen_str.h
*	Version Number	: 1.17
*	Release Number	: 1
*	Last Updated	: 12/14/99
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	92/03/02 Norman Hill - Added strnsav.
*	97/10/20 Norman Hill - Added functions ssprintf and svsprintf.
*	98/10/26 SDJ	     - Added str2double, str2short, str2uint, 
*		               str2ushort
*	99/12/13 NRH	     - Removed ssprintf and svsprintf except for
*			       the hpux and vxWorks platforms.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifndef _gen_str_h
#define _gen_str_h

#if defined( hpux ) || defined ( vxWorks )
#define	NO_SNPRINTF
#endif

/*
 *  Standard include file.
 */

#include <stdarg.h>
#include <string.h>

/*
 *  Definitions.
 */

#ifndef STRSIZ
#define STRSIZ			256
#endif

#define streq(s1,s2)		(strcmp(s1,s2) == 0)
#define strne(s1,s2)		(strcmp(s1,s2) != 0)

/*
 *  External function declarations.
 */

extern  boolean str2bool( const char *s, boolean *b );
extern	boolean str2double( const char *s,double *d );
extern	boolean	str2float(const char *s, float *f);
extern	boolean	str2int(const char *s, int *i);
extern	void	str2lower(char *lower, const char *s);
extern	boolean	str2short(const char *s, short *i);
extern	boolean	str2uint(const char *s, unsigned int *u);
extern	void	str2upper(char *upper, const char *s);
extern	boolean	str2ushort(const char *s, unsigned short *u);
extern	char	*strapp(char *string1, const char *string2);
extern	char	*strext(const char *pathname);
extern	char	*strfit(char *string);
extern	boolean	strfloat(const char *s);
extern	void	strhead(char *pathname);
extern	boolean	strint(const char *s);
extern	char	*strnapp(char *s1, const char *s2, int l);
extern	char	*strnsav(const char *s, int l);
extern	boolean	strpattern(const char *source, const char *pattern);
extern	void	strroot(char *pathname);
extern	char	*strsav(const char *s);
extern	char	*strtail(const char *pathname);
extern	void	strtokens(char *buffer, int max_tokens, char **tokens, int *num_tokens);
#ifdef NO_SNPRINTF
extern	int	ssprintf( const int, char *, const char * const, ... );
extern	int	svsprintf( const int, char *, const char * const, va_list );
#endif /* NO_SNPRINTF */

#endif /* _gen_str_h */
