/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	gen/h/gen_stack.h
*
*   Purpose:
*	Defines the STACK structure and operations on STACKs.  Stacks are
*	really just a functional layer built on top of EPTRs.
*
*   Date		: Nov 02, 1993
*
*   SCCS data		: @(#)
*	Module Name	: gen_stack.h
*	Version Number	: 1.5
*	Release Number	: 1
*	Last Updated	: 02/27/97
*
*   Programmer		: Severin Gaudet
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifndef _gen_stack_h
#define _gen_stack_h

typedef struct	stack {
    int		s_current;		/* Current item position.	*/
    pointer	s_eptr;			/* ETPR pointer array		*/
    pfv		s_free;			/* Function to free stack item.	*/
} STACK, *STACK_P;


/*
 *  Constants returned by stack_position().
 */

#define	STACK_POS_BOTTOM	0
#define	STACK_POS_EMPTY		1
#define	STACK_POS_MIDDLE	2
#define	STACK_POS_SINGLE	3
#define	STACK_POS_TOP		4


/*
 *  External function declarations.
 */

extern	pointer	stack_bottom(STACK *stack);
extern	pointer	stack_down(STACK *stack);
extern	void	stack_free(STACK *stack);
extern	STACK	*stack_init(int num_entries, pfv free_function);
extern	pointer	stack_item(STACK *stack);
extern	pointer	stack_pop(STACK *stack);
extern	int	stack_position(STACK *stack);
extern	pointer	stack_push(STACK *stack, pointer item);
extern	pointer	stack_top(STACK *stack);
extern	pointer	stack_up(STACK *stack);

#endif /* _gen_stack_h */
