/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	gen/h/gen_eptr.h
*
*   Purpose:
*	Defines the EPTR structure and operations on EPTRs.
*
*   Date		: Dec 10, 1990
*
*   SCCS data		: @(#)
*	Module Name	: gen_eptr.h
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 04/22/98
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*	92/05/22 SM	: Change to reflect changes in eptr.c.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifndef _gen_eptr_h
#define _gen_eptr_h

#ifdef	__cplusplus
extern "C" {
#endif


/*
 *  EPTRs provide means of managing arrays of general purpose
 *  pointers.  Thus lists of pointers to any kind of data
 *  can be created and manipulated with a single set of 
 *  operations ( eptr macros and functions ).
 *
 *  Each EPTR maintains a dynamic array of pointers, the
 *  number of elements used in the array, and the amount
 *  of memory allocated for the array.
 *
 *  The pointer array is always terminated with a NULL entry.
 */

typedef struct	eptr {
    pointer	*e_pointer;		/* pointer array		*/
    int		e_used;			/* elements used		*/
    int		e_allocd;		/* elements allocated		*/
    int		e_increment;		/* elements added by realloc	*/
} EPTR, *EPTR_P;


/*
 *  Macros
 */

#define eptritem(e, i)	((e)->e_pointer[i])
#define eptrlen(e)	((e)->e_used)


/*
 *  External function declarations.
 */
 
extern	EPTR	*eptr_alloc(int n);
extern	void	eptr_clear(EPTR *eptr, pfv e_free);
extern	void	eptr_del(EPTR *eptr, int start, int n, pfv e_free);
extern	EPTR	*eptr_fit(EPTR *eptr);
extern	void	eptr_free(EPTR *eptr, pfv e_free);
extern	EPTR	*eptr_insert(EPTR *eptr, int index, pointer ptr);
extern	EPTR	*eptr_init(EPTR *eptr, int n);
extern	void	eptr_replace(EPTR *eptr, pointer p, int index, pfv e_free);
extern	boolean	eptr_search(EPTR_P eptr, pointer key, int *index, pfi e_compare);
extern	EPTR	*eptr_sort(EPTR *eptr, pfi e_compare);

#ifdef	__cplusplus
}
#endif

#endif /* _gen_eptr_h */
